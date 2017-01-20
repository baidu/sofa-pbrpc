// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_RPC_CHANNEL_H_
#define _SOFA_PBRPC_RPC_CHANNEL_H_

#include <vector>

#include <google/protobuf/service.h>

#include <sofa/pbrpc/common.h>

namespace sofa {
namespace pbrpc {

// Defined in other files.
class RpcClient;
class RpcChannelImpl;

struct RpcChannelOptions {
    // Connect timeout (in seconds).
    // If a request can't get an healthy connection after a connect_timeout
    // milliseconds, it will fail and return to the caller.
    //
    // Not supported now.
    int64 connect_timeout;

    //////////// The following options are only useful for multiple servers. //////////
    // Server load capacity, which limits the max request count pending on one channel.
    // If all servers exceed the limit, calling will returns RPC_ERROR_SERVER_BUSY.
    // Value 0 means no limit, default value is 0.
    uint32 server_load_capacity;

    // If initialize the RpcChannel in construct function, default is true.
    // If create_with_init is false, RpcChannel should be initialized by calling Init().
    bool create_with_init;

    RpcChannelOptions()
        : connect_timeout(10)
        , server_load_capacity(0)
        , create_with_init(true)
    {}
};

class RpcChannel : public google::protobuf::RpcChannel
{
public:
    class EventHandler {
    public:
        virtual ~EventHandler() {}
        
        // This method is called when some addresses added.
        virtual void OnAddressAdded(const std::vector<std::string>& address_list) = 0;

        // This method is called when some addresses removed.
        virtual void OnAddressRemoved(const std::vector<std::string>& address_list) = 0;
    };

    class AddressProvider {
    public:
        virtual ~AddressProvider() {}

        // Get the init address list.
        // This will be called for once when create the channel.
        virtual void GetInitAddress(std::vector<std::string>* address_list) = 0;

        // Register an event handler.
        // The "event_handler" is take ownership by the address provider.
        virtual bool RegisterEventHandler(EventHandler* event_handler) = 0;
    };

public:
    // Create single server point by server address.
    // The "rpc_client" is owned by the caller.
    // The "server_address" should be in format of "ip:port".
    RpcChannel(RpcClient* rpc_client,
               const std::string& server_address,
               const RpcChannelOptions& options = RpcChannelOptions());

    // Create single server point by server ip and port.
    // The "rpc_client" is owned by the caller.
    RpcChannel(RpcClient* rpc_client,
               const std::string& server_ip,
               uint32 server_port,
               const RpcChannelOptions& options = RpcChannelOptions());

    // Create multiple server points by server address list.
    // The "rpc_client" is owned by the caller.
    RpcChannel(RpcClient* rpc_client,
               const std::vector<std::string>& address_list,
               const RpcChannelOptions& options = RpcChannelOptions());

    // Create multiple server points by address provider.
    // The "rpc_client" is owned by the caller.
    // The "address_provider" is owned by the caller.
    RpcChannel(RpcClient* rpc_client,
               AddressProvider* address_provider,
               const RpcChannelOptions& options = RpcChannelOptions());

    // Initialize RpcChannel.
    // For single server point, it will resolve server address in this function,
    // and if resolve server address succeed return true, otherwise return false.
    // For multiple server points, it will update internal server list and
    // register detect task. After all of these are completed return true, and
    // never return false.
    bool Init();

    // Destructor.
    virtual ~RpcChannel();

    // Implements the google::protobuf::RpcChannel interface.  If the
    // "done" is NULL, it's a synchronous call, or it's asynchronous and
    // uses the callback to inform the completion (or failure). 
    virtual void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                            ::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Message* request,
                            ::google::protobuf::Message* response,
                            ::google::protobuf::Closure* done);

public:
    const sofa::pbrpc::shared_ptr<RpcChannelImpl>& impl() const
    {
        return _impl;
    }

private:
    sofa::pbrpc::shared_ptr<RpcChannelImpl> _impl;

    SOFA_PBRPC_DISALLOW_EVIL_CONSTRUCTORS(RpcChannel);
}; // class RpcChannel

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_RPC_CHANNEL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
