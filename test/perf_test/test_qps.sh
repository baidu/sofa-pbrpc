#!/bin/bash

#############################################
HOST=$HOSTNAME
PORT=12345
THREAD_NUM=8
CLIENT_COUNT=5
SERVER=echo_server
CLIENT=client_parallel
SERVER_LOG=__test_qps.server.log
#############################################

if ! [ -f "$SERVER" ] || ! [ -f "$CLIENT" ]
then
    echo "binary not generated, to make!"
    exit 1
fi

function at_exit()
{
    killall $SERVER &>/dev/null
    killall $CLIENT &>/dev/null
    rm __test_qps.*.log
    exit 1
}

trap 'at_exit' INT QUIT
killall $SERVER &>/dev/null
killall $CLIENT &>/dev/null

echo "./$SERVER $HOST $PORT $THREAD_NUM &>$SERVER_LOG &"
./$SERVER $HOST $PORT $THREAD_NUM &>$SERVER_LOG &
sleep 1
if
    grep "ERROR" $SERVER_LOG
then
    at_exit
fi

i=0
while [ $i -lt $CLIENT_COUNT ]
do
    CLIENT_LOG=__test_qps.client.$i.log
    echo "./$CLIENT $HOST $PORT 1 &>$CLIENT_LOG &"
    ./$CLIENT $HOST $PORT 1 &>$CLIENT_LOG &
    sleep 1
    i=$((i+1))
done

echo "tail -f $SERVER_LOG"
tail -f $SERVER_LOG

at_exit

