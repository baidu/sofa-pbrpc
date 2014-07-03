#!/bin/sh

#############################################
HOST=$HOSTNAME
PORT=12345
SERVER=echo_server
CLIENT=client_parallel
SERVER_LOG=__kill_server.server.tmp.log
CLIENT_LOG=__kill_server.client.tmp.log
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
    exit 1
}

trap 'at_exit' INT QUIT
killall $SERVER &>/dev/null
killall $CLIENT &>/dev/null

i=0
while true
do
    echo "------------ [$i] --------------"
    date

    echo "./$SERVER $HOST $PORT &>$SERVER_LOG &"
    ./$SERVER $HOST $PORT &>$SERVER_LOG &
    SERVER_PID=$!
    sleep 1

    SIZE=$(((RANDOM % 3) * (RANDOM % 10) * 1024 + RANDOM % 1024))
    echo "./$CLIENT $HOST $PORT $SIZE false &>$CLIENT_LOG &"
    ./$CLIENT $HOST $PORT $SIZE false &>$CLIENT_LOG &
    CLIENT_PID=$!

    SLEEP_TIME=$((RANDOM % 3 + 2))
    echo "Sleep $SLEEP_TIME"
    sleep $SLEEP_TIME

    kill $SERVER_PID &>/dev/null
    wait $SERVER_PID
    wait $CLIENT_PID

    if ls core* 2>/dev/null; then
        at_exit
    fi

    grep 'Succeed' $CLIENT_LOG
    rm $CLIENT_LOG $SERVER_LOG &>/dev/null
    echo "Done"
    echo

    i=$((i+1))
    sleep 1
done

at_exit

