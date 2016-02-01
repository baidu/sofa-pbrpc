/***************************************************************************
 * 
 * Copyright (c) 2016 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file inexus_address_provider.h
 * @author zhangdi05(zhangdi05@baidu.com)
 * @date 2016/01/29 13:31:48
 * @version $Revision$ 
 * @brief 
 *  
 **/
#ifndef INEXUS_ADDRESS_PROVIDER_H
#define INEXUS_ADDRESS_PROVIDER_H

#include <vector>
#include <string>
#include <sofa/pbrpc/pbrpc.h>
#include "ins_sdk.h"

namespace sofa {
namespace pbrpc {

class InsAddressProvider : public sofa::pbrpc::RpcChannel::AddressProvider 
{
public:
    typedef std::vector<sofa::pbrpc::RpcChannel::EventHandler*> EventHandlerVector;
    typedef sofa::pbrpc::RpcChannel::EventHandler EventHandler;

    InsAddressProvider(std::string ins_address_list,
                       std::string server_path);

    ~InsAddressProvider();

    void Init();

    virtual void GetInitAddress(std::vector<std::string>* address_list);

    virtual bool RegisterEventHandler(EventHandler* event_handler);

    bool RegisterNode(std::string server_addr);

    void UpdateServer();
private:
    std::string _ins_address_list;
    std::string _server_path;

    sofa::pbrpc::MutexLock _lock;
    std::set<std::string> _server_set;
    EventHandlerVector _handler_list;
    galaxy::ins::sdk::InsSDK* _ins_sdk;
};

}
}
#endif  // INEXUS_ADDRESS_PROVIDER_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
