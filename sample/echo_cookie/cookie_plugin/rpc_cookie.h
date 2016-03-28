// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: zhangdi05@baidu.com (Zhang Di)

#ifndef _RPC_COOKIE_H_
#define _RPC_COOKIE_H_

#include <map>
#include <string>
#include <sofa/pbrpc/common.h>
#include <sofa/pbrpc/buffer.h>
#include <sofa/pbrpc/locks.h>
#include <sofa/pbrpc/rpc_attachment.h>

namespace sofa {
namespace pbrpc {

enum KeepType 
{
    ALWAYS_NEW = 0,
    USER_FIRST = 1,
    MANAGER_FIRST = 2
};

typedef std::map<std::string, std::string> MapType;

//class RpcCookieManager
class RpcCookieManager
{
public:
    RpcCookieManager()
    { }

    ~RpcCookieManager()
    { }

    bool Has(const std::string& key);

    void Load(MapType& kv_map);

    void Store(const MapType& kv_map);

private:
    MapType _kv_map;

    MutexLock _lock;
};

//class Cookie
class Cookie : public sofa::pbrpc::RpcAttachment
{
public:
    Cookie();

    Cookie(RpcCookieManager* manager, KeepType keep_type = USER_FIRST);

    virtual ~Cookie() { }

    bool Get(const std::string& key, std::string& value);

    void Set(const std::string& key, const std::string& value);

    bool Has(const std::string& key);

    bool Erase(const std::string& key);

    void Clear();
    
    void Store();

    virtual bool Serialize(sofa::pbrpc::ReadBufferPtr& attach_buffer); 

    virtual bool Deserialize(sofa::pbrpc::ReadBufferPtr& attach_buffer); 

    int SerializeLen() const;

private:
    MapType _kv_map;

    RpcCookieManager* _manager;

    KeepType _keep_type;

    SOFA_PBRPC_DISALLOW_EVIL_CONSTRUCTORS(Cookie);
}; //class RpcCookie

} // namespace pbrpc
} // namespace sofa

#endif  // _SOFA_PBRPC_RPC_COOKIE_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
