// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: zhangdi05@baidu.com (Zhangdi Di)

extern "C" {
#include <php.h>
#include <php_ini.h>
#include <ext/standard/info.h>
#include <ext/standard/php_string.h>
#include <Zend/zend_exceptions.h>
}

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef add_method
#undef add_method
#include <sofa/pbrpc/pbrpc.h>
#endif

#include "ext_rpc_service_stub.h"
#include "ext_rpc_service_stub_impl.h"

namespace sofa_php_ext
{

#define PB_CONSTANT(name) \
	zend_declare_class_constant_long(rpc_service_stub_ce, #name, sizeof(#name) - 1, name TSRMLS_CC)

zend_object_handlers rpc_service_stub_object_handlers;
struct rpc_service_stub_object 
{
    zend_object std;
    PhpRpcServiceStubImpl* stub_impl;
};

ZEND_BEGIN_ARG_INFO_EX(rpc_service_stub_arginfo_initservice, 0, 0, 3)
    ZEND_ARG_INFO(0, address_str)
    ZEND_ARG_INFO(0, package_name)
    ZEND_ARG_INFO(0, service_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(rpc_service_stub_arginfo_settimeout, 0, 0, 1)
    ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(rpc_service_stub_arginfo_getfailed, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(rpc_service_stub_arginfo_geterrortext, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(rpc_service_stub_arginfo_registermethod, 0, 0, 3)
    ZEND_ARG_INFO(0, method_name)
    ZEND_ARG_INFO(0, request)
    ZEND_ARG_INFO(0, response)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(rpc_service_stub_arginfo_initmethod, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(rpc_service_stub_arginfo_callmethod, 0, 0, 4)
    ZEND_ARG_INFO(0, method_name)
    ZEND_ARG_INFO(0, request)
    ZEND_ARG_INFO(0, response)
    ZEND_ARG_INFO(0, closure)
ZEND_END_ARG_INFO()

void rpc_service_stub_free_storage(void *object TSRMLS_DC) 
{
    rpc_service_stub_object *obj = (rpc_service_stub_object*)object;
    delete obj->stub_impl;
    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

zend_object_value rpc_service_stub_create_handler(zend_class_entry* type TSRMLS_DC) 
{
    zval* tmp;
    zend_object_value retval; 
    rpc_service_stub_object* obj 
        = (rpc_service_stub_object*)emalloc(sizeof(rpc_service_stub_object));
    memset(obj, 0, sizeof(rpc_service_stub_object));
    zend_object_std_init(&obj->std, type TSRMLS_CC);
#if PHP_VERSION_ID < 50399
    zend_hash_copy(obj->std.properties, 
            &(type->default_properties_table), 
            (copy_ctor_func_t)zval_add_ref, 
            (void*)&tmp, 
            sizeof(zval*));
#else
    object_properties_init(&obj->std, type);
#endif 
    retval.handle = zend_objects_store_put(obj, 
                                        (zend_objects_store_dtor_t) zend_objects_destroy_object,
                                        (zend_objects_free_object_storage_t) rpc_service_stub_free_storage, 
                                        NULL TSRMLS_CC);
    retval.handlers = &rpc_service_stub_object_handlers; 
    return retval;
}

PHP_METHOD(PhpRpcServiceStub, InitService)
{
    srand((unsigned)time(NULL));
    zval* stub_zval = getThis();
    char* address_str;
    int address_str_len;
    char* package_name;
    int package_len;
    char* service_name;
    int service_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, 
                              "sss", 
                              &address_str, 
                              &address_str_len,
                              &package_name,
                              &package_len,
                              &service_name,
                              &service_len) == FAILURE)
    {
        SLOG(ERROR, "create stub failed for bad parameters");
        return;
    }
    std::string server_addr_str(address_str, address_str_len);
    std::string package_name_str(package_name, package_len);
    std::string service_name_str(service_name, service_len);

    PhpRpcServiceStubImpl* _impl 
        = new PhpRpcServiceStubImpl(server_addr_str, package_name_str, service_name_str);
    rpc_service_stub_object* stub_obj 
        = (rpc_service_stub_object*)zend_object_store_get_object(stub_zval TSRMLS_CC);
    if (!stub_obj)
    {
        RETURN_FALSE;
    }
    stub_obj->stub_impl = _impl;
    RETURN_TRUE;
}

PHP_METHOD(PhpRpcServiceStub, SetTimeout)
{
    long timeout;
    zval* stub_zval = getThis();
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &timeout) == FAILURE)
    {
        SLOG(ERROR, "set timeout failed for bad parameters");
        return;
    }
    rpc_service_stub_object* stub_obj 
        = (rpc_service_stub_object*)zend_object_store_get_object(stub_zval TSRMLS_CC);
    if (stub_obj && stub_obj->stub_impl)
    {
        PhpRpcServiceStubImpl* _impl = stub_obj->stub_impl; 
        _impl->SetTimeout(timeout);
        RETURN_TRUE;
    }
    RETURN_FALSE;
}

PHP_METHOD(PhpRpcServiceStub, GetFailed)
{
    zval* stub_zval = getThis();
    rpc_service_stub_object* stub_obj 
        = (rpc_service_stub_object*)zend_object_store_get_object(stub_zval TSRMLS_CC);
    if (stub_obj && stub_obj->stub_impl)
    {
        PhpRpcServiceStubImpl* _impl = stub_obj->stub_impl; 
        RETURN_BOOL(_impl->Failed());
    }
    RETURN_FALSE;
}

PHP_METHOD(PhpRpcServiceStub, GetErrorText)
{
    zval* stub_zval = getThis();
    rpc_service_stub_object* stub_obj 
        = (rpc_service_stub_object*)zend_object_store_get_object(stub_zval TSRMLS_CC);
    if (stub_obj && stub_obj->stub_impl)
    {
        PhpRpcServiceStubImpl* _impl = stub_obj->stub_impl; 
        std::string err_text = _impl->ErrorText();
        RETURN_STRINGL(err_text.c_str(), err_text.length(), 1);
    }
    RETURN_EMPTY_STRING();
}

PHP_METHOD(PhpRpcServiceStub, RegisterMethod)
{
    char* method_name;
    int method_length;
    zval* request;
    zval* response;
    zval* stub_zval = getThis();
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, 
                              "szz", 
                              &method_name,
                              &method_length, 
                              &request,
                              &response) == FAILURE)
    {
        SLOG(ERROR, "register method failed for bad parameters");
        return;
    }
    std::string method_name_str(method_name, method_length);
    rpc_service_stub_object* stub_obj 
        = (rpc_service_stub_object*)zend_object_store_get_object(stub_zval TSRMLS_CC);
    if (stub_obj && stub_obj->stub_impl)
    {
        PhpRpcServiceStubImpl* _impl = stub_obj->stub_impl; 
        RETURN_BOOL(_impl->RegisterMethod(method_name_str, request, response));
    }
    RETURN_FALSE;
}

PHP_METHOD(PhpRpcServiceStub, InitMethods)
{
    zval* stub_zval = getThis();
    rpc_service_stub_object* stub_obj 
        = (rpc_service_stub_object*)zend_object_store_get_object(stub_zval TSRMLS_CC);
    if (stub_obj && stub_obj->stub_impl)
    {
        PhpRpcServiceStubImpl* _impl = stub_obj->stub_impl; 
        RETURN_BOOL(_impl->InitMethods());
    }
    RETURN_FALSE; 
}

PHP_METHOD(PhpRpcServiceStub, CallMethod)
{
    char* method_name;
    int method_length;
    zval* request_zval;
    zval* response_zval;
    zval* controller_zval;
    zval* closure_zval;
    zval* stub_zval = getThis();
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, 
                              "szzz", 
                              &method_name,
                              &method_length, 
                              &request_zval,
                              &response_zval,
                              &closure_zval) == FAILURE)
    {
        SLOG(ERROR, "call method  failed for bad parameters");
        return;
    }

    rpc_service_stub_object* stub_obj 
        = (rpc_service_stub_object*)zend_object_store_get_object(stub_zval TSRMLS_CC);
    if (stub_obj && stub_obj->stub_impl)
    {
        PhpRpcServiceStubImpl* _impl = stub_obj->stub_impl; 
        std::string method_name_str(method_name, method_length);
        _impl->CallMethod(method_name_str, request_zval, response_zval, closure_zval);
    }
}

const zend_function_entry sofa_pbrpc_functions[] = {

    PHP_ME(PhpRpcServiceStub, InitService, rpc_service_stub_arginfo_initservice, ZEND_ACC_PUBLIC)
    PHP_ME(PhpRpcServiceStub, SetTimeout, rpc_service_stub_arginfo_settimeout, ZEND_ACC_PUBLIC)
    PHP_ME(PhpRpcServiceStub, GetFailed, rpc_service_stub_arginfo_getfailed, ZEND_ACC_PUBLIC)
    PHP_ME(PhpRpcServiceStub, GetErrorText, rpc_service_stub_arginfo_geterrortext, ZEND_ACC_PUBLIC)
    PHP_ME(PhpRpcServiceStub, RegisterMethod, rpc_service_stub_arginfo_registermethod, ZEND_ACC_PUBLIC)
    PHP_ME(PhpRpcServiceStub, InitMethods, rpc_service_stub_arginfo_initmethod, ZEND_ACC_PUBLIC)
    PHP_ME(PhpRpcServiceStub, CallMethod, rpc_service_stub_arginfo_callmethod, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL, 0, 0}
};

PHP_MINIT_FUNCTION(sofa_pbrpc)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "PhpRpcServiceStub", sofa_pbrpc_functions);
    rpc_service_stub_ce = zend_register_internal_class(&ce TSRMLS_CC);
    rpc_service_stub_ce->create_object = rpc_service_stub_create_handler;
    memcpy(&rpc_service_stub_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    rpc_service_stub_object_handlers.clone_obj = NULL;

    PB_CONSTANT(PB_TYPE_DOUBLE);
    PB_CONSTANT(PB_TYPE_FIXED32);
    PB_CONSTANT(PB_TYPE_FIXED64);
    PB_CONSTANT(PB_TYPE_FLOAT);
    PB_CONSTANT(PB_TYPE_INT);
    PB_CONSTANT(PB_TYPE_SIGNED_INT);
    PB_CONSTANT(PB_TYPE_STRING);
    PB_CONSTANT(PB_TYPE_BOOL);

    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(sofa_pbrpc)
{
    return SUCCESS;
}

PHP_RINIT_FUNCTION(sofa_pbrpc)
{
    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(sofa_pbrpc)
{
    return SUCCESS;
}

PHP_MINFO_FUNCTION(sofa_pbrpc)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "sofa_pbrpc support", "enabled");
    php_info_print_table_end();
}

zend_module_entry sofa_pbrpc_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "sofa_pbrpc",
    sofa_pbrpc_functions,
    PHP_MINIT(sofa_pbrpc),
    PHP_MSHUTDOWN(sofa_pbrpc),
    PHP_RINIT(sofa_pbrpc),
    PHP_RSHUTDOWN(sofa_pbrpc),
    PHP_MINFO(sofa_pbrpc),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1",
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_SOFA_PBRPC
ZEND_GET_MODULE(sofa_pbrpc)
#endif

}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
