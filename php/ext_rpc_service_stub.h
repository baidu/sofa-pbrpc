// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: zhangdi05@baidu.com (Zhangdi Di)

#ifndef EXT_RPC_SERVICE_STUB_H
#define EXT_RPC_SERVICE_STUB_H

extern "C" {
#include <php.h>
#include <ext/standard/php_string.h>
#include <Zend/zend_exceptions.h>
}

namespace sofa_php_ext
{

zend_class_entry* rpc_service_stub_ce;

PHP_METHOD(PhpRpcServiceStub, InitService);

PHP_METHOD(PhpRpcServiceStub, SetTimeout);

PHP_METHOD(PhpRpcServiceStub, GetFailed);

PHP_METHOD(PhpRpcServiceStub, GetErrorText);

PHP_METHOD(PhpRpcServiceStub, RegisterMethod);

PHP_METHOD(PhpRpcServiceStub, InitMethods);

PHP_METHOD(PhpRpcServiceStub, CallMethod);

}
#endif  // EXT_RPC_SERVICE_STUB_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
