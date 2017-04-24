package pbrpc

import (
	"bufio"
	"bytes"
	"encoding/binary"
	"errors"
	"fmt"
	"io"
	"io/ioutil"
	"net/rpc"
	"sync"

	meta "github.com/baidu/sofa-pbrpc/go/rpc_meta"
	"github.com/golang/protobuf/proto"
)

var (
	MagicStr          = [4]byte{'S', 'O', 'F', 'A'}
	ErrBadMagic       = errors.New("bad magic number")
	ErrBadMessageSize = errors.New("message size exceed")
)

const (
	maxMessageSize = 64<<20 + 24 // 64M+sizeof(rpcHeader)
)

type rpcHeader struct {
	Magic [4]byte
	X     struct {
		MetaSize  int32
		DataSize  int64
		TotalSize int64
	}
}

// codec is a common rpc codec for implementing rpc.ClientCodec and rpc.ServerCodec
type codec struct {
	conn io.ReadWriteCloser
	w    *bufio.Writer
	r    *bufio.Reader

	// temporary work space
	h rpcHeader
	m *meta.RpcMeta
}

func newCodec(conn io.ReadWriteCloser) *codec {
	return &codec{
		conn: conn,
		w:    bufio.NewWriter(conn),
		r:    bufio.NewReader(conn),
	}
}

// Write send rpc header and body to peer
func (c *codec) Write(meta *meta.RpcMeta, x interface{}, cw compressWriter) error {
	buffer := new(bytes.Buffer)
	metasize := proto.Size(meta)
	h := rpcHeader{
		Magic: MagicStr,
	}
	h.X.MetaSize = int32(metasize)
	h.X.DataSize = 0
	h.X.TotalSize = int64(metasize)

	// write header, for placeholder, we will change DataSize and TotalSize later if no error in meta
	binary.Write(buffer, binary.LittleEndian, &h)

	// write meta
	buf, err := proto.Marshal(meta)
	if err != nil {
		return err
	}
	buffer.Write(buf)

	// skip write body if error
	if meta.GetFailed() {
		buffer.WriteTo(c.w)
		return c.w.Flush()
	}

	// write body
	msg := x.(proto.Message)
	buf, err = proto.Marshal(msg)
	if err != nil {
		return err
	}
	// record the offset before we write data
	len1 := buffer.Len()
	if cw != nil {
		wc, err := cw(buffer)
		if err != nil {
			return err
		}
		wc.Write(buf)
		wc.Close()
	} else {
		buffer.Write(buf)
	}

	h.X.DataSize = int64(buffer.Len() - len1)
	h.X.TotalSize = int64(metasize) + h.X.DataSize
	// write new header
	w := bytes.NewBuffer(buffer.Bytes()[:0])
	binary.Write(w, binary.LittleEndian, &h)

	buffer.WriteTo(c.w)
	return c.w.Flush()
}

func mustDecode(r io.Reader, x interface{}) {
	err := binary.Read(r, binary.LittleEndian, x)
	if err != nil {
		panic(err)
	}
}

func mustReadFull(r io.Reader, buf []byte) {
	_, err := io.ReadFull(r, buf)
	if err != nil {
		panic(err)
	}
}

// ReadHeader read rpc header from peer
func (c *codec) ReadHeader(meta *meta.RpcMeta) (err error) {
	defer func() {
		catch := recover()
		if catch != nil {
			if e, ok := catch.(error); ok {
				err = e
			} else {
				err = fmt.Errorf("%v", catch)
			}
		}
	}()

	// decode rpc header
	mustDecode(c.r, &c.h.Magic)
	if c.h.Magic != MagicStr {
		return ErrBadMagic
	}
	mustDecode(c.r, &c.h.X)

	if c.h.X.TotalSize > maxMessageSize {
		return ErrBadMessageSize
	}

	// decode rpc meta
	buf := make([]byte, c.h.X.MetaSize)
	mustReadFull(c.r, buf)
	err = proto.Unmarshal(buf, meta)
	return

}

// ReadBody read rpc body from peer which corresponding to last rpc header
func (c *codec) ReadBody(x interface{}, cr compressReader) error {
	if x == nil {
		_, err := c.r.Discard(int(c.h.X.DataSize))
		return err
	}

	msg := x.(proto.Message)

	var buf []byte
	if cr != nil {
		// construct a compress reader
		rc, err := cr(io.LimitReader(c.r, c.h.X.DataSize))
		if err != nil {
			return err
		}
		// read all uncompressed data to buf
		buf, err = ioutil.ReadAll(rc)
		if err != nil {
			rc.Close()
			return err
		}
		rc.Close()
	} else {
		buf = make([]byte, c.h.X.DataSize)
		_, err := io.ReadFull(c.r, buf)
		if err != nil {
			return err
		}
	}

	return proto.Unmarshal(buf, msg)
}

func (c *codec) Close() error {
	return c.conn.Close()
}

type clientCodec struct {
	c *codec

	// temporary work space
	m meta.RpcMeta
}

// newClientCodec returns a new rpc.ClientCodec using sofa-pbrpc on conn.
func newClientCodec(conn io.ReadWriteCloser) rpc.ClientCodec {
	return &clientCodec{c: newCodec(conn)}
}

func (c *clientCodec) WriteRequest(req *rpc.Request, x interface{}) error {
	m := &meta.RpcMeta{
		Type:       meta.RpcMeta_REQUEST.Enum(),
		SequenceId: proto.Uint64(req.Seq),
		Method:     proto.String(req.ServiceMethod),
	}
	// TODO support request compress type
	return c.c.Write(m, x, nil)
}

func (c *clientCodec) ReadResponseHeader(resp *rpc.Response) error {
	err := c.c.ReadHeader(&c.m)
	if err != nil {
		return err
	}
	resp.Seq = c.m.GetSequenceId()
	if c.m.Failed != nil && c.m.GetFailed() {
		resp.Error = fmt.Sprintf("code:%d, reason:%s", c.m.GetErrorCode(), c.m.GetReason())
	}

	return nil
}

func (c *clientCodec) ReadResponseBody(x interface{}) error {
	return c.c.ReadBody(x, newCompressReader(c.m.GetCompressType()))
}

func (c *clientCodec) Close() error {
	return c.c.Close()
}

type serverCodec struct {
	c *codec

	// temporary work space
	m meta.RpcMeta

	mutex   sync.Mutex
	pending map[uint64]meta.CompressType
}

// newServerCodec returns a new rpc.ServerCodec using sofa-pbrpc on conn.
func newServerCodec(conn io.ReadWriteCloser) rpc.ServerCodec {
	return &serverCodec{
		c:       newCodec(conn),
		pending: make(map[uint64]meta.CompressType),
	}
}

func (s *serverCodec) ReadRequestHeader(req *rpc.Request) error {
	err := s.c.ReadHeader(&s.m)
	if err != nil {
		return err
	}
	req.Seq = s.m.GetSequenceId()
	req.ServiceMethod = s.m.GetMethod()

	s.mutex.Lock()
	s.pending[req.Seq] = s.m.GetCompressType()
	s.mutex.Unlock()
	return nil
}

func (s *serverCodec) ReadRequestBody(x interface{}) error {
	cr := newCompressReader(s.m.GetCompressType())
	return s.c.ReadBody(x, cr)
}

func (s *serverCodec) WriteResponse(resp *rpc.Response, x interface{}) error {
	s.mutex.Lock()
	compressType := s.pending[resp.Seq]
	delete(s.pending, resp.Seq)
	s.mutex.Unlock()

	meta := &meta.RpcMeta{
		Type:         meta.RpcMeta_RESPONSE.Enum(),
		SequenceId:   proto.Uint64(resp.Seq),
		Failed:       proto.Bool(false),
		CompressType: compressType.Enum(),
	}

	if resp.Error != "" {
		meta.Failed = proto.Bool(true)
		meta.ErrorCode = proto.Int32(400)
		meta.Reason = proto.String(resp.Error)
	}

	return s.c.Write(meta, x, newCompressWriter(compressType))
}

func (s *serverCodec) Close() error {
	return s.c.Close()
}
