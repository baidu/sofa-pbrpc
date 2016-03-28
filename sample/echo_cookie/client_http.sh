#!/bin/sh

# POST
curl -d '{"message":"Hello, world!"}' http://localhost:12321/sofa.pbrpc.test.EchoServer.Echo
echo

# GET
curl http://localhost:12321/sofa.pbrpc.test.EchoServer.Echo?request=%7B%22message%22%3A%22Hello%2C%20world%21%22%7D
echo

