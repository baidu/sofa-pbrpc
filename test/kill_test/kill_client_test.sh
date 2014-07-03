#!/bin/sh

#############################################
HOST=$HOSTNAME
PORT=12346
SERVER=echo_server
CLIENT=client_parallel
CLIENT_LOG=__kill_client.client.tmp.log
SERVER_LOG=__kill_client.server.log
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

echo "./$SERVER $HOST $PORT &>$SERVER_LOG &"
./$SERVER $HOST $PORT &>$SERVER_LOG &
sleep 1

i=0
while true
do
    echo "------------ [$i] --------------"
    date

    SIZE=$(((RANDOM % 3) * (RANDOM % 10) * 1024 + RANDOM % 1024))
    GRACE_FLAG=$((RANDOM % 2))
    if [ $GRACE_FLAG == 1 ]; then
        IS_GRACE="true"
    else
        IS_GRACE="false"
    fi
    echo "./$CLIENT $HOST $PORT $SIZE $IS_GRACE &>$CLIENT_LOG &"
    ./$CLIENT $HOST $PORT $SIZE $IS_GRACE &>$CLIENT_LOG &
    PID=$!

    SLEEP_TIME=$((RANDOM % 3 + 1))
    echo "Sleep $SLEEP_TIME"
    sleep $SLEEP_TIME

    kill $PID &>/dev/null
    wait $PID

    if ls core* 2>/dev/null; then
        at_exit
    fi
    if [ $GRACE_FLAG == 1 ]; then
        if grep ERROR $CLIENT_LOG; then
            at_exit
        fi
    fi

    grep Succeed $CLIENT_LOG
    rm $CLIENT_LOG &>/dev/null
    echo "Done"
    echo

    i=$((i+1))
    sleep 1
done

at_exit

