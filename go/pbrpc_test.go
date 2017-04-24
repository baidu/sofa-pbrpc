package pbrpc

import (
	"bytes"
	"context"
	"log"
	"net"
	"testing"

	meta "github.com/baidu/sofa-pbrpc/go/rpc_meta"
	"github.com/baidu/sofa-pbrpc/go/testdata/echo"
	"github.com/golang/protobuf/proto"
)

// bufferConn extends net.Conn by recording read stream and write stream
type bufferConn struct {
	net.Conn
	wbuf bytes.Buffer
	rbuf bytes.Buffer
}

func newBufferConn(conn net.Conn) *bufferConn {
	return &bufferConn{
		Conn: conn,
	}
}

func (b *bufferConn) Write(buf []byte) (int, error) {
	n, err := b.Conn.Write(buf)
	if err != nil {
		return 0, err
	}
	b.wbuf.Write(buf)
	return n, nil
}

func (b *bufferConn) Read(buf []byte) (int, error) {
	n, err := b.Conn.Read(buf)
	if err != nil {
		return 0, err
	}
	b.rbuf.Write(buf[:n])
	return n, nil
}

func (b *bufferConn) RBuffer() *bytes.Buffer {
	return &b.rbuf
}

func (b *bufferConn) WBuffer() *bytes.Buffer {
	return &b.wbuf
}

// pipe behaves like net.Pipe but returns bufferConn
func pipe() (*bufferConn, *bufferConn) {
	conn1, conn2 := net.Pipe()
	return newBufferConn(conn1), newBufferConn(conn2)
}

// echoService is an EchoServer implementation
type echoService struct{}

func (s *echoService) Echo(req *echo.EchoRequest, rep *echo.EchoResponse) error {
	rep.Message = proto.String("echo message: " + req.GetMessage())
	return nil
}

func TestRoundTrip(t *testing.T) {
	conn1, conn2 := pipe()
	defer conn1.Close()
	defer conn2.Close()

	server := NewServer()
	server.RegisterService(serviceName, new(echoService))
	go server.ServeConn(conn2)

	client := NewClientConn(conn1)
	request := &echo.EchoRequest{Message: proto.String(message)}
	reply := new(echo.EchoResponse)
	err := Invoke(context.Background(), methodName, request, reply, client)
	if err != nil {
		log.Fatal(err)
	}

	diffStream(t, nopCloser{conn1.WBuffer()}, "client-none-none.dump",
		new(echo.EchoRequest), meta.CompressType_CompressTypeNone)

	diffStream(t, nopCloser{conn1.RBuffer()}, "server-none.dump",
		new(echo.EchoResponse), meta.CompressType_CompressTypeNone)
}
