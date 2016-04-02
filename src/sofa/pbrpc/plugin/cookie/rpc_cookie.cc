// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: zhangdi05@baidu.com (Zhang Di)

#include <vector>
#include <sofa/pbrpc/serialize.h>
#include <sofa/pbrpc/plugin/cookie/rpc_cookie.h>

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

    std::vector<std::string> remove_list;
    MapType::const_iterator old_it = _kv_map.begin();
    MapType::const_iterator old_end = _kv_map.end();
    MapType::const_iterator new_it = kv_map.begin();
    MapType::const_iterator new_end = kv_map.end();
    while (old_it != old_end && new_it != new_end)
    {
        if(old_it->first < new_it->first)
        {
            remove_list.push_back(old_it->first);
            ++old_it;
        }
        else
        {
            ++new_it;
            ++old_it;
        }
    }
    while (old_it != old_end)
    {
        remove_list.push_back(old_it->first);
        ++old_it;
    }
    if (!remove_list.empty())
    {
        for (std::vector<std::string>::const_iterator it = remove_list.begin();
                it != remove_list.end(); ++it)
        {
            _kv_map.erase(*it);
        }
    }
}

Cookie::Cookie() : _manager(NULL)
{ }

Cookie::Cookie(RpcCookieManager* manager) : _manager(manager)
{ }

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
    _kv_map[key] = value;
}

void Cookie::Load()
{
    if (_manager != NULL)
    {
        _manager->Load(_kv_map);
    }
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

bool Cookie::Empty()
{
    return _kv_map.empty();
}

bool Cookie::Erase(const std::string& key)
{
    if (_kv_map.find(key) == _kv_map.end())
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
