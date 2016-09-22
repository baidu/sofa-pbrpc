sofa-pbrpc
==========
[![Build Status](https://travis-ci.org/baidu/sofa-pbrpc.svg)](https://travis-ci.org/baidu/sofa-pbrpc)

A light-weight RPC implementation of Google's protobuf RPC framework.

Wiki: https://github.com/baidu/sofa-pbrpc/wiki

### Features
* High performace.
* Easy to use. Refer to sample code in './sample'.
* Supports sync call and async call. Refer to './sample/echo'.
* Supports three level (service/method/request) timeout. Refer to './sample/timeout_sample'.
* Supports transparent compression. Refer to './sample/compress_sample'.
* Supports mock test. Refer to './sample/mock_sample'.
* Supports network flow control.
* Supports auto connecting and reconnecting.
* Supports keep alive time of idle connections.
* Supports statistics for profiling.
* Supports multi-server load balance and fault tolerance.
* Supports http protocol.
* Provides web monitor.
* Provides python client library.

### Dependencies
This lib depends on boost-1.53.0 (only need header), protobuf-2.4.1, snappy and zlib:
* boost - http://www.boost.org/
* protobuf - http://code.google.com/p/protobuf/
* snappy - http://code.google.com/p/snappy/
* zlib - http://zlib.net/

ATTENTION: boost header is only needed when compiling the lib, but is not needed for user code.

Extrally, './unit-test' and './sample/mock_sample' also depends on gtest:
* gtest - http://code.google.com/p/googletest/

### Build
1. Modify the file './depends.mk' to specify depending libs.<br>
  The necessary libs are boost, protobuf, snappy, and zlib.
2. Run 'make' to build sofa-pbrpc.<br>
  The default optimization level is 'O2'.<br>
  To change it, modify the 'OPT' variable in file './Makefile'.
3. Run 'make install' to install sofa-pbrpc.<br>
  The default install directory is './output'.<br>
  To change it, modify the 'PREFIX' variable in file './Makefile'.

For more details, please refer to the wiki.

### Sample
For sample code, please refer to './sample' and the wiki.

### Performance
For performace details, please refer to the wiki.

### Support
opensearch@baidu.com
