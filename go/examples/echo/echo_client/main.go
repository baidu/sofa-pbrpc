package main

import (
	"context"
	"flag"
	"log"
	"time"

	pbrpc "github.com/baidu/sofa-pbrpc/go"
	"github.com/baidu/sofa-pbrpc/go/examples/echo/echo"
	"github.com/golang/protobuf/proto"
)

var (
	addr = flag.String("addr", "127.0.0.1:12321", "server address")
)

func main() {
	flag.Parse()

	ctx, cancel := context.WithTimeout(context.Background(), time.Second)
	defer cancel()

	conn, err := pbrpc.Dial(ctx, *addr)
	if err != nil {
		log.Fatal(err)
	}
	defer conn.Close()

	c := echo.NewEchoServerClient(conn)
	reply, err := c.Echo(ctx, &echo.EchoRequest{Message: proto.String("hello")})
	if err != nil {
		log.Fatal(err)
	}
	log.Print(reply.GetMessage())
}
