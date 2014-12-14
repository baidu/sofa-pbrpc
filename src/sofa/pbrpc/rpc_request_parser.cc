#include <sofa/pbrpc/rpc_request.h>
#include <sofa/pbrpc/binary_rpc_request_parser.h>
//#include <sofa/pbrpc/http_rpc_request_parser.h>

namespace sofa {
namespace pbrpc {

bool RpcRequestParser::ParseMethodFullName(const std::string& method_full_name,
        std::string* service_name, std::string* method_name)
{
    std::string::size_type pos = method_full_name.rfind('.');
    if (pos == std::string::npos)
    {
        return false;
    }
    *service_name = method_full_name.substr(0, pos);
    *method_name = method_full_name.substr(pos + 1);
    return true;
}

void RpcRequestParser::RegisteredParsers(std::vector<RpcRequestParserPtr>* parsers)
{
    parsers->push_back(RpcRequestParserPtr(new BinaryRpcRequestParser()));
    //parsers->push_back(RpcRequestParserPtr(new HTTPRpcRequestParser()));
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
