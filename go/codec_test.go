package pbrpc

import (
	"bytes"
	"io"
	"os"
	"path/filepath"
	"reflect"
	"testing"

	meta "github.com/baidu/sofa-pbrpc/go/rpc_meta"
	"github.com/baidu/sofa-pbrpc/go/testdata/echo"
	"github.com/golang/protobuf/proto"
)

const (
	methodName  = "sofa.pbrpc.test.EchoServer.Echo"
	serviceName = "sofa.pbrpc.test.EchoServer"
	message     = "Hello from qinzuoyan01"
)

type nopCloser struct {
	io.ReadWriter
}

func (nopCloser) Close() error { return nil }

func diffStream(t *testing.T, b io.ReadWriteCloser, dump string, bodyType proto.Message, ctype meta.CompressType) {
	t.Logf("test for %s", dump)
	f, err := os.Open(filepath.Join("testdata", dump))
	if err != nil {
		t.Fatal(err)
	}
	defer f.Close()
	c1 := newCodec(f)
	c2 := newCodec(b)

	var m1, m2 meta.RpcMeta
	if err := c1.ReadHeader(&m1); err != nil {
		t.Fatal(err)
	}
	if err := c2.ReadHeader(&m2); err != nil {
		t.Fatal(err)
	}

	if m1.GetType() != m2.GetType() ||
		m1.GetMethod() != m2.GetMethod() ||
		m1.GetServerTimeout() != m2.GetServerTimeout() ||
		m1.GetFailed() != m2.GetFailed() ||
		m1.GetErrorCode() != m2.GetErrorCode() ||
		m1.GetReason() != m2.GetReason() ||
		m1.GetCompressType() != m2.GetCompressType() ||
		m1.GetExpectedResponseCompressType() != m2.GetExpectedResponseCompressType() {
		t.Errorf("rpc meta not equal.\nfile:\n%s \nbuffer:\n%s",
			proto.MarshalTextString(&m1), proto.MarshalTextString(&m2))
	}

	b1 := proto.Clone(bodyType)
	b2 := proto.Clone(bodyType)
	if err := c1.ReadBody(b1, newCompressReader(ctype)); err != nil {
		t.Fatal(err)
	}

	if err := c2.ReadBody(b2, newCompressReader(ctype)); err != nil {
		t.Fatal(err)
	}
	if !reflect.DeepEqual(b1, b2) {
		t.Errorf("body not equal.\nfile:\n%s\nbuffer:\n%s",
			proto.MarshalTextString(b1), proto.MarshalTextString(b2))
	}
}

func TestCodecCompress(t *testing.T) {
	type testcase struct {
		ctype meta.CompressType
		dump  string
	}
	var cases = []testcase{
		{meta.CompressType_CompressTypeNone, "client-none-none.dump"},
		{meta.CompressType_CompressTypeGzip, "client-gzip-gzip.dump"},
		{meta.CompressType_CompressTypeZlib, "client-zlib-zlib.dump"},
		//	{meta.CompressType_CompressTypeSnappy, "client-snappy-snappy.dump"},
		//	{meta.CompressType_CompressTypeLZ4, "client-lz4-lz4.dump"},
	}
	for _, cs := range cases {
		b := nopCloser{new(bytes.Buffer)}
		codec := newCodec(b)
		m := &meta.RpcMeta{
			Type:                         meta.RpcMeta_REQUEST.Enum(),
			SequenceId:                   proto.Uint64(1),
			Method:                       proto.String(methodName),
			CompressType:                 cs.ctype.Enum(),
			ExpectedResponseCompressType: cs.ctype.Enum(),
		}
		msg := &echo.EchoRequest{Message: proto.String(message)}
		err := codec.Write(m, msg, newCompressWriter(cs.ctype))
		if err != nil {
			t.Error(err)
		}
		diffStream(t, b, cs.dump, new(echo.EchoRequest), cs.ctype)
	}
}
