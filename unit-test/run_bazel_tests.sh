#!/bin/bash

function build()
{
  name=$(echo $1 | awk -F '.' '{print $2}' | awk -F '/' '{print $2}')
  bazel build :$name
  if [ $? -ne 0 ]; then
      echo "BUILD FAILED!!! for $name"
      exit 1
  fi
}

function run()
{
    name=$(echo $1 | awk -F '.' '{print $2}' | awk -F '/' '{print $2}')
    echo "===========" $name "==========="
    bazel run :$name
    ../bazel-bin/unit-test/$name
    if [ $? -ne 0 ]; then
        echo "TEST FAILED!!!"
        exit 1
    fi
}

num=0
for test_case in `find . -name 'test_*.cc' -a` # -perm -100`
do
    build $test_case
    num=$((num+1))
done

num=0
for test_case in `find . -name 'test_*.cc' -a` # -perm -100`
do
    run $test_case
    num=$((num+1))
done

echo
echo "CASE NUM: $num"
echo "ALL CASE PASSED!!!"
