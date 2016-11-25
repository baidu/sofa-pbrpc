// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <set>

#include <sofa/pbrpc/pbrpc.h>
#include "echo_service.pb.h"

class ReloadableAddressProvider : public sofa::pbrpc::RpcChannel::AddressProvider
{
public:
    typedef std::vector<sofa::pbrpc::RpcChannel::EventHandler*> EventHandlerVector;
public:
    ReloadableAddressProvider(const std::string& addr_file) : _addr_file(addr_file) {
        Reload();
    }

    virtual ~ReloadableAddressProvider() {
        sofa::pbrpc::ScopedLocker<sofa::pbrpc::MutexLock> _(_lock);
        for (EventHandlerVector::iterator it = _handler_list.begin();
                it != _handler_list.end(); ++it) {
            delete *it;
        }
    }

    virtual void GetInitAddress(std::vector<std::string>* address_list) {
        sofa::pbrpc::ScopedLocker<sofa::pbrpc::MutexLock> _(_lock);
        address_list->assign(_addr_set.begin(), _addr_set.end());
    }

    virtual bool RegisterEventHandler(sofa::pbrpc::RpcChannel::EventHandler* event_handler) {
        sofa::pbrpc::ScopedLocker<sofa::pbrpc::MutexLock> _(_lock);
        _handler_list.push_back(event_handler);
        return true;
    }

    void Reload() {
        sofa::pbrpc::ScopedLocker<sofa::pbrpc::MutexLock> _(_lock);
        SLOG(NOTICE, "start reloading address list from file \"%s\"", _addr_file.c_str());
        // open file
        std::ifstream ifs(_addr_file.c_str(), std::ifstream::in);
        if (!ifs.good()) {
            SLOG(ERROR, "open address list file \"%s\" fail", _addr_file.c_str());
            return;
        }
        // read new addresses
        std::set<std::string> new_addr_set;
        std::string addr;
        while (std::getline(ifs, addr)) {
            if (!addr.empty()) {
                new_addr_set.insert(addr);
            }
        }
        // make diff
        std::vector<std::string> add_list;
        std::vector<std::string> remove_list;
        std::set<std::string>::iterator old_it = _addr_set.begin();
        std::set<std::string>::iterator old_end = _addr_set.end();
        std::set<std::string>::iterator new_it = new_addr_set.begin();
        std::set<std::string>::iterator new_end = new_addr_set.end();
        while (old_it != old_end && new_it != new_end) {
            if (*old_it == *new_it) {
                // keep
                ++old_it;
                ++new_it;
            }
            else if (*old_it < *new_it) {
                // remove
                remove_list.push_back(*old_it);
                ++old_it;
            }
            else {
                // add
                add_list.push_back(*new_it);
                ++new_it;
            }
        }
        if (old_it != old_end) {
            remove_list.insert(remove_list.end(), old_it, old_end);
        }
        if (new_it != new_end) {
            add_list.insert(add_list.end(), new_it, new_end);
        }
        // notice handler
        if (!add_list.empty() || !remove_list.empty()) {
            for (EventHandlerVector::iterator it = _handler_list.begin();
                    it != _handler_list.end(); ++it) {
                if (!add_list.empty()) {
                    (*it)->OnAddressAdded(add_list);
                }
                if (!remove_list.empty()) {
                    (*it)->OnAddressRemoved(remove_list);
                }
            }
        }
        // update _addr_set
        _addr_set = new_addr_set;
    }

private:
    std::string _addr_file;
    sofa::pbrpc::MutexLock _lock;
    std::set<std::string> _addr_set;
    EventHandlerVector _handler_list;
};

static ReloadableAddressProvider* g_address_provider; 

static void sigcatcher(int sig)
{
    SLOG(NOTICE, "signal catched: %d", sig);
    if (g_address_provider) {
        g_address_provider->Reload();
    }
}

int main(int argc, char** argv)
{
    SOFA_PBRPC_SET_LOG_LEVEL(NOTICE);

    if (argc < 2) {
        fprintf(stderr, "USAGE: %s <address-list-file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    std::string addr_file = argv[1];
    g_address_provider = new ReloadableAddressProvider(addr_file);

    signal(SIGTERM,  &sigcatcher);

    // Define an rpc client.
    sofa::pbrpc::RpcClientOptions client_options;
    sofa::pbrpc::RpcClient* rpc_client = new sofa::pbrpc::RpcClient(client_options);

    // Define an rpc channel.
    sofa::pbrpc::RpcChannelOptions channel_options;
    sofa::pbrpc::RpcChannel* rpc_channel = 
        new sofa::pbrpc::RpcChannel(rpc_client, g_address_provider, channel_options);

    // Define an rpc stub.
    sofa::pbrpc::test::EchoServer_Stub* stub =
        new sofa::pbrpc::test::EchoServer_Stub(rpc_channel);

    while (true) {
        // Prepare parameters.
        sofa::pbrpc::RpcController* cntl = new sofa::pbrpc::RpcController();
        cntl->SetTimeout(3000);
        sofa::pbrpc::test::EchoRequest* request = new sofa::pbrpc::test::EchoRequest();
        request->set_message("Hello from qinzuoyan01");
        sofa::pbrpc::test::EchoResponse* response = new sofa::pbrpc::test::EchoResponse();

        // Sync call.
        stub->Echo(cntl, request, response, NULL);

        // Check if the request has been sent.
        // If has been sent, then can get the sent bytes.
        SLOG(NOTICE, "RemoteAddress=%s", cntl->RemoteAddress().c_str());
        SLOG(NOTICE, "IsRequestSent=%s", cntl->IsRequestSent() ? "true" : "false");
        if (cntl->IsRequestSent()) {
            SLOG(NOTICE, "LocalAddress=%s", cntl->LocalAddress().c_str());
            SLOG(NOTICE, "SentBytes=%ld", cntl->SentBytes());
        }

        // Check if failed.
        if (cntl->Failed()) {
            SLOG(ERROR, "request failed: %s", cntl->ErrorText().c_str());
        }
        else {
            SLOG(NOTICE, "request succeed: %s", response->message().c_str());
        }

        // Destroy objects.
        delete cntl;
        delete request;
        delete response;

        sleep(1);
    }

    delete stub;
    delete rpc_channel;
    delete rpc_client;
    delete g_address_provider;
    g_address_provider = NULL;

    return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
