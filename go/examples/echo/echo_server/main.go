package main

import (
	"flag"
	"log"
	"net"

	pbrpc "github.com/baidu/sofa-pbrpc/go"
	"github.com/baidu/sofa-pbrpc/go/examples/echo/echo"
	"github.com/golang/protobuf/proto"
)

var (
	addr = flag.String("addr", ":12321", "listen address")
)

type echoService struct {
}

func (s *echoService) Echo(req *echo.EchoRequest, reply *echo.EchoResponse) error {
	reply.Message = proto.String(req.GetMessage())
	return nil
}

func main() {
	flag.Parse()

	server := pbrpc.NewServer()
	echo.RegisterEchoServer(server, new(echoService))

	l, err := net.Listen("tcp", *addr)
	if err != nil {
		log.Fatal(err)
	}
	log.Fatal(server.Serve(l))

}
