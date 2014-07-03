#!/bin/sh

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

echo
echo "============= test 1K data =============="
echo "./$CLIENT $HOST $PORT 900 &>$CLIENT_LOG &"
./$CLIENT $HOST $PORT 900 &>$CLIENT_LOG &
echo "sleep 3"
sleep 3
grep -o -a 'elapsed time in us: [0-9]*$' $CLIENT_LOG \
| awk 'BEGIN{sum=0;num=0}{if(NR > 10){sum+=$5;++num;}}END{ \
print "Succeed count: " num; \
print "Average elapsed time per request: " (sum/num) "us";}'

echo
echo "============= test 1M data =============="
echo "./$CLIENT $HOST $PORT 1000000 &>$CLIENT_LOG &"
./$CLIENT $HOST $PORT 1000000 &>$CLIENT_LOG &
echo "sleep 10"
sleep 10
grep -o -a 'elapsed time in us: [0-9]*$' $CLIENT_LOG \
| awk 'BEGIN{sum=0;num=0}{sum+=$5;++num;}END{ \
print "Succeed count: " num; \
print "Average elapsed time per request: " (sum/num) "us";}'

echo
echo "============= test 10M data =============="
echo "./$CLIENT $HOST $PORT 10000000 &>$CLIENT_LOG &"
./$CLIENT $HOST $PORT 10000000 &>$CLIENT_LOG &
echo "sleep 10"
sleep 10
grep -o -a 'elapsed time in us: [0-9]*$' $CLIENT_LOG \
| awk 'BEGIN{sum=0;num=0}{sum+=$5;++num;}END{ \
print "Succeed count: " num; \
print "Average elapsed time per request: " (sum/num) "us";}'

echo
at_exit

