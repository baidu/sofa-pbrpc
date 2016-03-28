// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: zhangdi05@baidu.com (Zhang Di)

#include <sofa/pbrpc/serialize.h>
#include "rpc_cookie.h"

namespace sofa {
namespace pbrpc {

bool RpcCookieManager::Has(const std::string& key)
{
    ScopedLocker<MutexLock> _(_lock);
    return _kv_map.find(key) != _kv_map.end();
}

void RpcCookieManager::Load(MapType& kv_map)
{
    ScopedLocker<MutexLock> _(_lock);
    kv_map = _kv_map;
}

void RpcCookieManager::Store(const MapType& kv_map)
{
    ScopedLocker<MutexLock> _(_lock);
    _kv_map.insert(kv_map.begin(), kv_map.end());
}

Cookie::Cookie() : _keep_type(ALWAYS_NEW)
{ }

Cookie::Cookie(RpcCookieManager* manager, KeepType keep_type) : 
                _manager(manager), _keep_type(keep_type)
{
    if (_keep_type == ALWAYS_NEW)
    {
        return;
    }
    else if (_manager != NULL)
    {
        _manager->Load(_kv_map);
    }
}

bool Cookie::Get(const std::string& key, std::string& value)
{
    MapType::const_iterator it = _kv_map.find(key);   
    if (it == _kv_map.end())
    {
        return false;
    }
    value = it->second;
    return true;
}

void Cookie::Set(const std::string& key, const std::string& value)
{
    if (_keep_type == MANAGER_FIRST)
    {
        if (!_manager->Has(key))
        {
            _kv_map[key] = value;
        }
        return;
    }
    _kv_map[key] = value;
}

void Cookie::Store()
{
    if (_manager != NULL)
    {
        _manager->Store(_kv_map);
    }
}

bool Cookie::Has(const std::string& key)
{
    return _kv_map.find(key) != _kv_map.end();
}

bool Cookie::Erase(const std::string& key)
{
    MapType::iterator it = _kv_map.find(key);
    if (it == _kv_map.end())
    {
        return false;
    }
    _kv_map.erase(key);
    return true;
}

void Cookie::Clear()
{
    _kv_map.clear();
}

bool Cookie::Serialize(sofa::pbrpc::ReadBufferPtr& attach_buffer)
{
    int size = _kv_map.size();
    SCHECK_LE(static_cast<size_t>(size), SOFA_CONTAINER_MAX_SERIALIZE_SIZE);
    WriteBufferPtr write_buffer(new WriteBuffer());
    Serializer serializer(write_buffer);
    if (!serializer.serialize_varint(size))
    {
        return false;
    }
    MapType::const_iterator it = _kv_map.begin();
    for (; it != _kv_map.end(); ++it)
    {
        if (!serializer.serialize_string(it->first))
        {
            return false;
        }
        if (!serializer.serialize_string(it->second))
        {
            return false;
        }
    }
    write_buffer->SwapOut(attach_buffer.get());
    return true;
}

int Cookie::SerializeLen() const
{
    int len = Serializer::varint_len(_kv_map.size());
    MapType::const_iterator it = _kv_map.begin();
    for (; it != _kv_map.end(); ++it)
    {
        len += Serializer::string_len(it->first);
        len += Serializer::string_len(it->second);
    }
    return len;
}

bool Cookie::Deserialize(sofa::pbrpc::ReadBufferPtr& attach_buffer)
{
    int size = 0;
    Deserializer deserializer(attach_buffer);
    if (!deserializer.deserialize_varint(size))
    {
        return false;
    }
    if (static_cast<size_t>(size) > SOFA_CONTAINER_MAX_SERIALIZE_SIZE)
    {
        return false;
    }
    _kv_map.clear();
    for (int i = 0; i < size; ++i)
    {
        std::string key;
        if (!deserializer.deserialize_string(key))
        {
            return false;
        }
        std::string value;
        if (!deserializer.deserialize_string(value))
        {
            return false;
        }
        _kv_map[key] = value;
    }
    return true;
}
} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
