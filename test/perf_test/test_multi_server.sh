#!/bin/bash

#############################################
HOST=$HOSTNAME
PORT_LIST="12345 12346 12347"
THREAD_NUM=8
SERVER=echo_server
CLIENT=client_multi_server
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
    rm __test_multi_server.*.log
    exit 1
}

trap 'at_exit' INT QUIT
killall $SERVER &>/dev/null
killall $CLIENT &>/dev/null

i=0
for PORT in $PORT_LIST
do
    SERVER_LOG=__test_multi_server.server.$i.log
    echo "./$SERVER $HOST $PORT $THREAD_NUM &>$SERVER_LOG &"
    ./$SERVER $HOST $PORT $THREAD_NUM &>$SERVER_LOG &
    sleep 1
    if
        grep "ERROR" $SERVER_LOG
    then
        at_exit
    fi
done

CLIENT_PARAM=""
for PORT in $PORT_LIST
do
    CLIENT_PARAM="$CLIENT_PARAM $HOST:$PORT"
done

echo "./$CLIENT $CLIENT_PARAM"
./$CLIENT $CLIENT_PARAM

at_exit

