#!/bin/bash

function run()
{
    echo "===========" $1 "==========="
    ./$1
    if [ $? -ne 0 ]; then
        echo "TEST FAILED!!!"
        exit 1
    fi
}

num=0
for test_case in `find . -name 'test_*' -a -perm -100`
do
    run $test_case
    num=$((num+1))
done

echo
echo "CASE NUM: $num"
echo "ALL CASE PASSED!!!"

