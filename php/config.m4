dnl $Id$
dnl config.m4 for extension sofa-pbrpc
PHP_ARG_ENABLE(sofa_pbrpc, whether to enable sofa_pbrpc support,
[  --enable-sofa_pbprc       enable sofa_pbrpc support])

if test "$PHP_SOFAPBRPC" != "no"; then
    RPC_LIBNAME=sofa-pbrpc
    RPCLIB_DIR=/home/zhangdi05/svn_space/ps/opensource/sofa-pbrpc
    PROTOBUF_LIBNAME=protobuf
    PROTOBUFLIB_DIR=/home/zhangdi05/svn_space/third-64/protobuf
    PHP_REQUIRE_CXX()
    PHP_SUBST(SOFA_PBRPC_SHARED_LIBADD)
    PHP_ADD_LIBRARY(stdc++, 1, SOFA_PBRPC_SHARED_LIBADD)

    PHP_ADD_LIBRARY(sofa-pbrpc, 1, SOFA_PBRPC_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH($RPC_LIBNAME, $RPCLIB_DIR/output/lib)
    PHP_ADD_INCLUDE($RPCLIB_DIR/output/include)
    PHP_ADD_LIBRARY(protobuf, 1, SOFA_PBRPC_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH($PROTOBUF_LIBNAME, $PROTOBUFLIB_DIR/lib)
    PHP_ADD_INCLUDE($PROTOBUFLIB_DIR/include)
    PHP_NEW_EXTENSION(sofa_pbrpc, ext_rpc_service_stub.cc ext_rpc_service_stub_impl.cc, $ext_shared)
fi

PHP_C_BIGENDIAN()
