/***************************************************************************
 * 
 * Copyright (c) 2016 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file inexus_address_provider.cc
 * @author zhangdi05(zhangdi05@baidu.com)
 * @date 2016/01/29 13:33:22
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include <iostream>
#include <algorithm>
#include "ins_address_provider.h"

namespace sofa {
namespace pbrpc {

InsAddressProvider::InsAddressProvider(std::string ins_address_list, 
                                    std::string server_path) 
                                : _ins_address_list(ins_address_list), 
                                  _server_path(server_path)
{
    _ins_sdk = new galaxy::ins::sdk::InsSDK(_ins_address_list); 
}

InsAddressProvider::~InsAddressProvider()
{
    delete _ins_sdk;
    _ins_sdk = NULL;
}

static void OnServerChanged(const galaxy::ins::sdk::WatchParam& param,
                            galaxy::ins::sdk::SDKError error);

void InsAddressProvider::UpdateServer()
{
    SLOG(NOTICE, "on server node changed");
    galaxy::ins::sdk::ScanResult* result = _ins_sdk->Scan(_server_path + "/!",
                                                          _server_path + "/~");
    std::set<std::string> new_server_set;
    while (!result->Done())
    {
        std::string key = result->Key();
        size_t prefix_len = (_server_path + "/").size();
        std::string server_addr = key.substr(prefix_len);
        new_server_set.insert(server_addr);
        result->Next();
    }
    galaxy::ins::sdk::SDKError err;
    int watch_ret = _ins_sdk->Watch(_server_path, &OnServerChanged, this, &err);
    if (!watch_ret)
    {
        SLOG(ERROR, "watch server_path %s failed", _server_path.c_str());
        return;
    }

    std::vector<std::string> add_server_list;
    std::vector<std::string> remove_server_list;
    std::set<std::string>::iterator old_it = _server_set.begin();
    std::set<std::string>::iterator old_end = _server_set.end();
    std::set<std::string>::iterator new_it = new_server_set.begin();
    std::set<std::string>::iterator new_end = new_server_set.end();
    while (old_it != old_end && new_it != new_end) 
    {
        if (*old_it == *new_it) 
        {
            ++old_it;
            ++new_it;
        }
        else if (*old_it < *new_it) 
        {
            remove_server_list.push_back(*old_it);
            ++old_it;
        }
        else 
        {
            add_server_list.push_back(*new_it);
            ++new_it;
        }
    }
    if (old_it != old_end) 
    {
        remove_server_list.insert(remove_server_list.end(), old_it, old_end);
    }
    if (new_it != new_end) 
    {
        add_server_list.insert(add_server_list.end(), new_it, new_end);
    }
    if (!add_server_list.empty() || !remove_server_list.empty()) 
    {
        for (EventHandlerVector::iterator it = _handler_list.begin();
                it != _handler_list.end(); ++it) 
        {
            if (!add_server_list.empty()) 
            {
                (*it)->OnAddressAdded(add_server_list);
            }
            if (!remove_server_list.empty()) 
            {
                (*it)->OnAddressRemoved(remove_server_list);
            }
        }
    }
    _server_set = new_server_set;
}

bool InsAddressProvider::RegisterNode(std::string server_addr)
{
    galaxy::ins::sdk::SDKError err;
    std::string lock_key = _server_path + "/" + server_addr;
    return _ins_sdk->Lock(lock_key, &err);
}

static void OnServerChanged(const galaxy::ins::sdk::WatchParam& param,
                            galaxy::ins::sdk::SDKError error)
{
    InsAddressProvider* ins_provider = static_cast<InsAddressProvider*>(param.context);
    ins_provider->UpdateServer();
}

void InsAddressProvider::Init()
{
    galaxy::ins::sdk::SDKError err;
    galaxy::ins::sdk::ScanResult* result = _ins_sdk->Scan(_server_path + "/!",
                                                          _server_path + "/~"); 
    while (!result->Done())
    {
        std::string key = result->Key();
        std::cout << "server_path : " << _server_path << std::endl;
        std::cout << "ins result : " << key << std::endl;
        size_t prefix_len = (_server_path + "/").size();
        std::string server_addr = key.substr(prefix_len);
        _server_set.insert(server_addr);
       result->Next();
    }
    int watch_ret = _ins_sdk->Watch(_server_path, &OnServerChanged, this, &err);
    if (!watch_ret)
    {
        SLOG(ERROR, "watch server_path %s failed", _server_path.c_str());
        return;
    }
}

void InsAddressProvider::GetInitAddress(std::vector<std::string>* address_list)
{
    if (address_list == NULL)
    {
        return;
    }
    sofa::pbrpc::ScopedLocker<sofa::pbrpc::MutexLock> _(_lock);
    address_list->assign(_server_set.begin(), _server_set.end());
}

bool InsAddressProvider::RegisterEventHandler(EventHandler* event_handler)
{
    sofa::pbrpc::ScopedLocker<sofa::pbrpc::MutexLock> _(_lock);
    _handler_list.push_back(event_handler);
    return true;
}

}
}
 
/* vim: set ts=4 sw=4 sts=4 tw=100 */
