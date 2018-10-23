// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: zhangdi05@baidu.com (Zhang Di)

#ifndef _SOFA_PBRPC_PLUGIN_COOKIE_RPC_COOKIE_H_
#define _SOFA_PBRPC_PLUGIN_COOKIE_RPC_COOKIE_H_

#include <map>
#include <string>
#include <sofa/pbrpc/buffer.h>
#include <sofa/pbrpc/locks.h>
#include <sofa/pbrpc/rpc_attachment.h>

namespace sofa {
namespace pbrpc {

class RpcCookieManager
{
public:
    RpcCookieManager()
    { }

    ~RpcCookieManager()
    { }

    bool Has(const std::string& key);

    void Load(std::map<std::string, std::string>& kv_map);

    void Store(const std::map<std::string, std::string>& kv_map);

private:
    std::map<std::string, std::string> _kv_map;

    MutexLock _lock;
}; // class RpcCookieManager

class RpcCookie : public sofa::pbrpc::RpcAttachment
{
public:
    RpcCookie();

    explicit RpcCookie(RpcCookieManager* manager);

    virtual ~RpcCookie();

    bool Get(const std::string& key, std::string& value);

    void Set(const std::string& key, const std::string& value);

    bool Has(const std::string& key);
    
    bool Empty();

    bool Erase(const std::string& key);

    void Clear();

    void Load();
    
    void Store();

    virtual bool Serialize(sofa::pbrpc::ReadBufferPtr& attach_buffer); 

    virtual bool Deserialize(sofa::pbrpc::ReadBufferPtr& attach_buffer); 

    int SerializeLen() const;

private:
    std::map<std::string, std::string> _kv_map;

    RpcCookieManager* _manager;

    SOFA_PBRPC_DISALLOW_EVIL_CONSTRUCTORS(RpcCookie);
}; // class RpcCookie

} // namespace pbrpc
} // namespace sofa

#endif  // _SOFA_PBRPC_PLUGIN_COOKIE_RPC_COOKIE_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
