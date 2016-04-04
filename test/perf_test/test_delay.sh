#!/bin/bash

#############################################
HOST=$HOSTNAME
PORT=12345
THREAD_NUM=8
SERVER=echo_server
CLIENT=client_serial
SERVER_LOG=__test_delay.server.log
CLIENT_LOG=__test_delay.client.log
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
    rm __test_delay.*.log
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

for i in "100 0.1K" "900 1K-" "1100 1K+" "10000 10K" "100000 100K" "1000000 1M" "10000000 10M"; do
    arr=($i)
    num=${arr[0]}
    str=${arr[1]}
    echo
    echo "============= test $str data =============="
    echo "./$CLIENT $HOST $PORT $num &>$CLIENT_LOG &"
    ./$CLIENT $HOST $PORT $num &>$CLIENT_LOG &
    PID=$!
    SLEEP=5
    echo "sleep $SLEEP"
    sleep $SLEEP
    grep -o -a 'elapsed time in us: [0-9]*$' $CLIENT_LOG \
        | awk 'BEGIN{sum=0;num=0}{if(NR > 10){sum+=$5;++num;}}END{ \
        print "Succeed count: " num; \
        print "Average elapsed time per request: " (sum/num) "us";}'
    kill $PID
    sleep 1
done

echo
at_exit

