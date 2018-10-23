// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: zhangdi05@baidu.com (Zhang Di)

#include <set>
#include <sofa/pbrpc/serialize.h>
#include <sofa/pbrpc/plugin/cookie/rpc_cookie.h>

namespace sofa {
namespace pbrpc {

bool RpcCookieManager::Has(const std::string& key)
{
    ScopedLocker<MutexLock> _(_lock);
    return _kv_map.find(key) != _kv_map.end();
}

void RpcCookieManager::Load(std::map<std::string, std::string>& kv_map)
{
    ScopedLocker<MutexLock> _(_lock);
    kv_map = _kv_map;
}

void RpcCookieManager::Store(const std::map<std::string, std::string>& kv_map)
{
    ScopedLocker<MutexLock> _(_lock);
    std::set<std::string> remove_keys;
    std::map<std::string, std::string> add_or_modify_pairs;
    std::map<std::string, std::string>::const_iterator old_it = _kv_map.begin();
    std::map<std::string, std::string>::const_iterator new_it = kv_map.begin();
    while (old_it != _kv_map.end() && new_it != kv_map.end())
    {
        if(old_it->first < new_it->first)
        {
            remove_keys.insert(old_it->first);
            ++old_it;
        }
        else if (old_it->first > new_it->first)
        {
            add_or_modify_pairs.insert(*new_it);
            ++new_it;
        }
        else
        {
            if (old_it->second != new_it->second)
            {
                add_or_modify_pairs.insert(*new_it);
            }
            ++old_it;
            ++new_it;
        }
    }
    while (old_it != _kv_map.end())
    {
        remove_keys.insert(old_it->first);
        ++old_it;
    }
    if (new_it != kv_map.end())
    {
        add_or_modify_pairs.insert(new_it, kv_map.end());
    }
    for (std::set<std::string>::const_iterator it = remove_keys.begin();
            it != remove_keys.end(); ++it)
    {
        _kv_map.erase(*it); 
    }
    for (std::map<std::string, std::string>::const_iterator it = add_or_modify_pairs.begin();
            it != add_or_modify_pairs.end(); ++it)
    {
        _kv_map[it->first] = it->second;
    }
}

RpcCookie::RpcCookie() : _manager(NULL)
{ }

RpcCookie::RpcCookie(RpcCookieManager* manager) : _manager(manager)
{ }

RpcCookie::~RpcCookie()
{ }

bool RpcCookie::Get(const std::string& key, std::string& value)
{
    std::map<std::string, std::string>::const_iterator it = _kv_map.find(key);   
    if (it == _kv_map.end())
    {
        return false;
    }
    value = it->second;
    return true;
}

void RpcCookie::Set(const std::string& key, const std::string& value)
{
    _kv_map[key] = value;
}

void RpcCookie::Load()
{
    if (_manager != NULL)
    {
        _manager->Load(_kv_map);
    }
}

void RpcCookie::Store()
{
    if (_manager != NULL)
    {
        _manager->Store(_kv_map);
    }
}

bool RpcCookie::Has(const std::string& key)
{
    return _kv_map.find(key) != _kv_map.end();
}

bool RpcCookie::Empty()
{
    return _kv_map.empty();
}

bool RpcCookie::Erase(const std::string& key)
{
    if (_kv_map.find(key) == _kv_map.end())
    {
        return false;
    }
    _kv_map.erase(key);
    return true;
}

void RpcCookie::Clear()
{
    _kv_map.clear();
}

bool RpcCookie::Serialize(sofa::pbrpc::ReadBufferPtr& attach_buffer)
{
    int size = _kv_map.size();
    SCHECK_LE(static_cast<size_t>(size), SOFA_CONTAINER_MAX_SERIALIZE_SIZE);
    WriteBufferPtr write_buffer(new WriteBuffer());
    Serializer serializer(write_buffer);
    if (!serializer.serialize_varint(size))
    {
        return false;
    }
    std::map<std::string, std::string>::const_iterator it = _kv_map.begin();
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

int RpcCookie::SerializeLen() const
{
    int len = Serializer::varint_len(_kv_map.size());
    std::map<std::string, std::string>::const_iterator it = _kv_map.begin();
    for (; it != _kv_map.end(); ++it)
    {
        len += Serializer::string_len(it->first);
        len += Serializer::string_len(it->second);
    }
    return len;
}

bool RpcCookie::Deserialize(sofa::pbrpc::ReadBufferPtr& attach_buffer)
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
