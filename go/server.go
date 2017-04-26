package pbrpc

import (
	"net"
	"net/rpc"
)

// Server is a pbrpc server to serve RPC requests.
type Server struct {
	s *rpc.Server
}

// NewServer creates a pbrpc server which has no service registered and has not started to accept requests yet.
func NewServer() *Server {
	return &Server{rpc.NewServer()}
}

// RegisterService register a service and its implementation to the pbrpc server. Called from the IDL generated code. This must be called before invoking Serve.
func (s *Server) RegisterService(name string, srv interface{}) {
	s.s.RegisterName(name, srv)
}

// ServeConn runs the server on a single connection. ServeConn blocks, serving the connection until the client hangs up. The caller typically invokes ServeConn in a go statement.
func (s *Server) ServeConn(conn net.Conn) {
	codec := newServerCodec(conn)
	s.s.ServeCodec(codec)
}

// Serve accepts incoming connections on the listener l, creating a new ServerConn and service goroutine for each. The service goroutines read pbrpc requests and then call the registered handlers to reply to them. Serve returns when l.Accept fails with errors.
// TODO Handle non fatal errors
func (s *Server) Serve(l net.Listener) error {
	for {
		conn, err := l.Accept()
		if err != nil {
			return err
		}
		go s.ServeConn(conn)
	}
}
