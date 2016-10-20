// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cstdlib>
#include <cstdio>

#include <sofa/pbrpc/pbrpc.h>
#include <sofa/pbrpc/builtin_service.pb.h>
#include <sofa/pbrpc/http-agent/http_agent.h>

static std::string g_server_address;

void usage();
void error(const std::string& message, bool print_usage);
int run_health(int argc, const char** argv);
int run_status(int argc, const char** argv);
int run_option(int argc, const char** argv);
int run_stat(int argc, const char** argv);
int run_list(int argc, const char** argv);
int run_desc(int argc, const char** argv);
int run_call(int argc, const char** argv);
int call_method(std::string method, std::string request, int timeout);

void usage() {
    fprintf(stderr,
            "Usage: sofa-pbrpc-client <server-address> <sub-command> [args]\n"
            "\n"
            "Available subcommands:\n"
            "   help\n"
            "           Print this usage help.\n"
            "\n"
            "   health  \n"
            "           Check if the server is healthy.\n"
            "\n"
            "   status  \n"
            "           Get status of the server.\n"
            "\n"
            "   option  \n"
            "           Get RpcServerOptions of the server.\n"
            "\n"
            "   list    \n"
            "           List all services provided by the server.\n"
            "\n"
            "   desc    <protobuf-type-name>\n"
            "           Get descriptor of a protobuf type (service/message/enum).\n"
            "\n"
            "   call    <method-full-name> <request-text|request-json> [timeout-in-ms]\n"
            "           Call a method using the text or json format of request message.\n"
            "           The \"timeout-in-ms\" is optional, default is 3000 milli-seconds.\n"
            "\n"
            "   stat    [service-full-name] [period-in-seconds]\n"
            "           Get the service statistics in the latest period of seconds.\n"
            "           The \"service-full-name\" is optional, default is \"all\".\n"
            "           The \"period-in-seconds\" is optional, default is 60 seconds.\n"
            "\n"
            "For more usage help, please see:\n"
            "<URL:https://github.com/BaiduPS/sofa-pbrpc/wiki>.\n"
            );
}

void error(const std::string& message, bool print_usage)
{
    fprintf(stderr, "*****  ERROR: %s *****\n", message.c_str());
    if (print_usage) {
        fprintf(stderr, "\n");
        usage();
    }
}

int run_health(int /*argc*/, const char** /*argv*/)
{
    sofa::pbrpc::RpcClient rpc_client;
    sofa::pbrpc::RpcChannel rpc_channel(&rpc_client, g_server_address);
    sofa::pbrpc::builtin::BuiltinService_Stub stub(&rpc_channel);

    sofa::pbrpc::RpcController cntl;
    sofa::pbrpc::builtin::HealthRequest request;
    sofa::pbrpc::builtin::HealthResponse response;
    stub.Health(&cntl, &request, &response, NULL);
    if (cntl.Failed()) {
        error("call method failed: " + cntl.ErrorText(), false);
        return EXIT_FAILURE;
    }
    else {
        fprintf(stdout,
                "Response:\n"
                "------------------------------\n"
                "%s"
                "------------------------------\n",
                response.DebugString().c_str());
        return EXIT_SUCCESS;
    }
}

int run_status(int /*argc*/, const char** /*argv*/)
{
    sofa::pbrpc::RpcClient rpc_client;
    sofa::pbrpc::RpcChannel rpc_channel(&rpc_client, g_server_address);
    sofa::pbrpc::builtin::BuiltinService_Stub stub(&rpc_channel);

    sofa::pbrpc::RpcController cntl;
    sofa::pbrpc::builtin::ServerStatusRequest request;
    sofa::pbrpc::builtin::ServerStatusResponse response;
    stub.ServerStatus(&cntl, &request, &response, NULL);
    if (cntl.Failed()) {
        error("call method failed: " + cntl.ErrorText(), false);
        return EXIT_FAILURE;
    }
    else {
        fprintf(stdout,
                "Response:\n"
                "------------------------------\n"
                "%s"
                "------------------------------\n",
                response.DebugString().c_str());
        return EXIT_SUCCESS;
    }
}

int run_option(int /*argc*/, const char** /*argv*/)
{
    sofa::pbrpc::RpcClient rpc_client;
    sofa::pbrpc::RpcChannel rpc_channel(&rpc_client, g_server_address);
    sofa::pbrpc::builtin::BuiltinService_Stub stub(&rpc_channel);

    sofa::pbrpc::RpcController cntl;
    sofa::pbrpc::builtin::ServerOptionsRequest request;
    sofa::pbrpc::builtin::ServerOptionsResponse response;
    stub.ServerOptions(&cntl, &request, &response, NULL);
    if (cntl.Failed()) {
        error("call method failed: " + cntl.ErrorText(), false);
        return EXIT_FAILURE;
    }
    else {
        fprintf(stdout,
                "Response:\n"
                "------------------------------\n"
                "%s"
                "------------------------------\n",
                response.DebugString().c_str());
        return EXIT_SUCCESS;
    }
}

int run_stat(int argc, const char** argv)
{
    sofa::pbrpc::RpcClient rpc_client;
    sofa::pbrpc::RpcChannel rpc_channel(&rpc_client, g_server_address);
    sofa::pbrpc::builtin::BuiltinService_Stub stub(&rpc_channel);

    sofa::pbrpc::RpcController cntl;
    sofa::pbrpc::builtin::StatRequest request;
    if (argc >= 1) {
        request.set_service_name(argv[0]);
    }
    else {
        request.set_service_name("all");
    }
    if (argc >= 2) {
        request.set_period_seconds(atoi(argv[1]));
    }
    else {
        request.set_period_seconds(60);
    }
    sofa::pbrpc::builtin::StatResponse response;
    stub.Stat(&cntl, &request, &response, NULL);
    if (cntl.Failed()) {
        error("call method failed: " + cntl.ErrorText(), false);
        return EXIT_FAILURE;
    }
    else {
        fprintf(stdout,
                "Response:\n"
                "------------------------------\n"
                "%s"
                "------------------------------\n",
                response.DebugString().c_str());
        return EXIT_SUCCESS;
    }
}


int run_list(int /*argc*/, const char** /*argv*/)
{
    sofa::pbrpc::RpcClient rpc_client;
    sofa::pbrpc::http_agent::HttpAgent agent(&rpc_client);
    if (!agent.Init(g_server_address)) {
        error("init failed", false);
        return EXIT_FAILURE;
    }
    std::map<std::string, std::string> desc_map;
    if (!agent.ListService(&desc_map)) {
        error("list failed", false);
        return EXIT_FAILURE;
    }
    for (std::map<std::string, std::string>::iterator it = desc_map.begin();
            it != desc_map.end(); ++it) {
        if (it != desc_map.begin()) fprintf(stderr, "\n");
        fprintf(stderr,
                "Service [%s]:\n"
                "------------------------------\n"
                "%s"
                "------------------------------\n",
                it->first.c_str(), it->second.c_str());
    }
    return EXIT_SUCCESS;
}

int run_desc(int argc, const char** argv)
{
    if (argc < 1) {
        error("no enough arguments", true);
        return EXIT_FAILURE;
    }
    std::string name = argv[0];
    if (name.find('.') == 0) {
        // remove leading '.'
        name = name.substr(1);
    }
    sofa::pbrpc::RpcClient rpc_client;
    sofa::pbrpc::http_agent::HttpAgent agent(&rpc_client);
    if (!agent.Init(g_server_address)) {
        error("init failed", false);
        return EXIT_FAILURE;
    }
    sofa::pbrpc::http_agent::HttpAgent::ProtobufType type;
    std::string desc;
    if (!agent.GetDescriptor(name, &type, &desc)) {
        error("type not found", false);
        return EXIT_FAILURE;
    }
    std::string type_str;
    if (type == sofa::pbrpc::http_agent::HttpAgent::PT_SERVICE) {
        type_str = "Service";
    }
    else if (type == sofa::pbrpc::http_agent::HttpAgent::PT_MESSAGE) {
        type_str = "Message";
    }
    else if (type == sofa::pbrpc::http_agent::HttpAgent::PT_ENUM) {
        type_str = "Enum";
    }
    else {
        type_str = "Unknown";
    }
    fprintf(stdout,
            "Descriptor of [%s]:\n"
            "------------------------------\n"
            "%s"
            "------------------------------\n",
            name.c_str(), desc.c_str()); 
    return EXIT_SUCCESS;
}

int run_call(int argc, const char** argv)
{
    if (argc < 2) {
        error("no enough arguments", true);
        return EXIT_FAILURE;
    }
    int timeout = 3000;
    if (argc >= 3) {
        timeout = atoi(argv[2]);
    }
    return call_method(argv[0], argv[1], timeout);
}

int call_method(std::string method, std::string request, int timeout)
{
    sofa::pbrpc::RpcClient rpc_client;
    sofa::pbrpc::http_agent::HttpAgent agent(&rpc_client);
    if (!agent.Init(g_server_address)) {
        error("init failed", false);
        return EXIT_FAILURE;
    }

    sofa::pbrpc::RpcController cntl;
    cntl.SetTimeout(timeout);
    std::string response;
    agent.CallMethod(method, &cntl, &request, &response, NULL);
    if (cntl.Failed()) {
        error("call method failed: " + cntl.ErrorText(), false);
        return EXIT_FAILURE;
    }
    else {
        fprintf(stdout,
                "Response:\n"
                "------------------------------\n"
                "%s"
                "------------------------------\n",
                response.c_str());
        return EXIT_SUCCESS;
    }
}

int main(int argc, const char** argv)
{
    if (argc < 3) {
        error("no enough arguments", true);
        return EXIT_SUCCESS;
    }

    g_server_address = argv[1];
    std::string cmd = argv[2];
    argc = argc - 3;
    argv = argv + 3;
    if (g_server_address == "help" || cmd == "help") {
        usage();
        return EXIT_SUCCESS;
    }
    else if (cmd == "health") {
        return run_health(argc, argv);
    }
    else if (cmd == "status") {
        return run_status(argc, argv);
    }
    else if (cmd == "option") {
        return run_option(argc, argv);
    }
    else if (cmd == "stat") {
        return run_stat(argc, argv);
    }
    else if (cmd == "list") {
        return run_list(argc, argv);
    }
    else if (cmd == "desc") {
        return run_desc(argc, argv);
    }
    else if (cmd == "call") {
        return run_call(argc, argv);
    }
    else {
        error("invalid command", true);
        return EXIT_FAILURE;
    }
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
