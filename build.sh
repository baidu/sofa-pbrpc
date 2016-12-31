#!/usr/bin/env bash
set -x -e

########################################
# download & build depend software
########################################

DEPS_PREFIX=`pwd`/thirdparty
DEPS_CONFIG="--prefix=${DEPS_PREFIX} --disable-shared --with-pic"
FLAG_DIR=`pwd`/.build

export PATH=${DEPS_PREFIX}/bin:$PATH
mkdir -p ${DEPS_PREFIX} ${FLAG_DIR}

# boost
if [ ! -f "${FLAG_DIR}/boost_1_57_0" ] \
    || [ ! -d "${DEPS_PREFIX}/boost_1_57_0/boost" ]; then
    wget -O ${DEPS_PREFIX}/boost_1_57_0.tar.gz http://sourceforge.net/projects/boost/files/boost/1.57.0/boost_1_57_0.tar.gz
    tar zxf ${DEPS_PREFIX}/boost_1_57_0.tar.gz -C ${DEPS_PREFIX}
    touch "${FLAG_DIR}/boost_1_57_0"
fi

# protobuf
if [ ! -f "${FLAG_DIR}/protobuf_2_6_1" ] \
    || [ ! -f "${DEPS_PREFIX}/lib/libprotobuf.a" ] \
    || [ ! -d "${DEPS_PREFIX}/include/google/protobuf" ]; then
    wget -O ${DEPS_PREFIX}/protobuf-2.6.1.tar.gz https://github.com/google/protobuf/releases/download/v2.6.1/protobuf-2.6.1.tar.gz
    tar zxf ${DEPS_PREFIX}/protobuf-2.6.1.tar.gz -C ${DEPS_PREFIX}
    cd ${DEPS_PREFIX}/protobuf-2.6.1
    autoreconf -ivf
    ./configure ${DEPS_CONFIG}
    make -j4
    make install
    cd -
    touch "${FLAG_DIR}/protobuf_2_6_1"
fi

# snappy
if [ ! -f "${FLAG_DIR}/snappy_1_1_1" ] \
    || [ ! -f "${DEPS_PREFIX}/lib/libsnappy.a" ] \
    || [ ! -f "${DEPS_PREFIX}/include/snappy.h" ]; then
    wget -O ${DEPS_PREFIX}/snappy-1.1.3.tar.gz https://github.com/google/snappy/releases/download/1.1.3/snappy-1.1.3.tar.gz
    tar zxf ${DEPS_PREFIX}/snappy-1.1.3.tar.gz -C ${DEPS_PREFIX}
    cd ${DEPS_PREFIX}/snappy-1.1.3
    ./configure ${DEPS_CONFIG}
    make -j4
    make install
    cd -
    touch "${FLAG_DIR}/snappy_1_1_1"
fi

########################################
# config depengs.mk
########################################

echo "BOOST_HEADER_DIR="${DEPS_PREFIX}"/boost_1_57_0" > depends.mk
echo "PROTOBUF_DIR="${DEPS_PREFIX} >> depends.mk
echo "SNAPPY_DIR="${DEPS_PREFIX} >> depends.mk

########################################
# install sofa-pbrpc
########################################

make clean
make -j4 && make install
