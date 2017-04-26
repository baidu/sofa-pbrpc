package pbrpc

import (
	"compress/gzip"
	"compress/zlib"
	"io"
	"io/ioutil"

	meta "github.com/baidu/sofa-pbrpc/go/rpc_meta"
	"github.com/golang/snappy"
	"github.com/pierrec/lz4"
)

type (
	compressReader func(r io.Reader) (io.ReadCloser, error)
	compressWriter func(w io.Writer) (io.WriteCloser, error)
)

// gzip compress
func gzipCompressReader(r io.Reader) (io.ReadCloser, error) {
	return gzip.NewReader(r)
}

func gzipCompressWriter(w io.Writer) (io.WriteCloser, error) {
	return gzip.NewWriter(w), nil
}

// zlib compress
func zlibCompressReader(r io.Reader) (io.ReadCloser, error) {
	return zlib.NewReader(r)
}

func zlibCompressWriter(w io.Writer) (io.WriteCloser, error) {
	return zlib.NewWriter(w), nil
}

// snappy compress
func snappyCompressReader(r io.Reader) (io.ReadCloser, error) {
	return ioutil.NopCloser(snappy.NewReader(r)), nil
}

func snappyCompressWriter(w io.Writer) (io.WriteCloser, error) {
	return snappy.NewWriter(w), nil
}

// lz4 compress
func lz4CompressReader(r io.Reader) (io.ReadCloser, error) {
	return ioutil.NopCloser(lz4.NewReader(r)), nil
}

func lz4CompressWriter(w io.Writer) (io.WriteCloser, error) {
	return lz4.NewWriter(w), nil
}

func newCompressReader(tp meta.CompressType) compressReader {
	switch tp {
	case meta.CompressType_CompressTypeGzip:
		return compressReader(gzipCompressReader)
	case meta.CompressType_CompressTypeZlib:
		return compressReader(zlibCompressReader)
	case meta.CompressType_CompressTypeSnappy:
		return compressReader(snappyCompressReader)
	case meta.CompressType_CompressTypeLZ4:
		return compressReader(lz4CompressReader)
	default:
		return nil

	}
}

func newCompressWriter(tp meta.CompressType) compressWriter {
	switch tp {
	case meta.CompressType_CompressTypeGzip:
		return compressWriter(gzipCompressWriter)
	case meta.CompressType_CompressTypeZlib:
		return compressWriter(zlibCompressWriter)
	case meta.CompressType_CompressTypeSnappy:
		return compressWriter(snappyCompressWriter)
	case meta.CompressType_CompressTypeLZ4:
		return compressWriter(lz4CompressWriter)
	default:
		return nil

	}
}
