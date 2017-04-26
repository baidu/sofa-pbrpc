package pbrpc

import (
	"context"
	"net"
	"net/rpc"
)

// ClientConn represents a client connection to an RPC server.
type ClientConn struct {
	c *rpc.Client
}

// Close tears down the ClientConn and all underlying connections.
func (c *ClientConn) Close() error {
	return c.c.Close()
}

// Invoke sends the RPC request on the wire and returns after response is received. Invoke is called by generated code. Also users can call Invoke directly when it is really needed in their use cases.
func Invoke(ctx context.Context, method string, args, reply interface{}, cc *ClientConn) error {
	call := cc.c.Go(method, args, reply, make(chan *rpc.Call, 1))
	select {
	case <-call.Done:
		return call.Error
	case <-ctx.Done():
		return ctx.Err()
	}
}

// Dial creates a client connection to the given target.
// The provided Context must be non-nil. If the context expires before the connection is complete, an error is returned. Once successfully connected, any expiration of the context will not affect the connection.
func Dial(ctx context.Context, addr string) (*ClientConn, error) {
	dialer := new(net.Dialer)
	conn, err := dialer.DialContext(ctx, "tcp", addr)
	if err != nil {
		return nil, err
	}

	return NewClientConn(conn), nil
}

// NewClientConn creates a ClientConn on a given connection
func NewClientConn(conn net.Conn) *ClientConn {
	c := rpc.NewClientWithCodec(newClientCodec(conn))
	return &ClientConn{c: c}
}
