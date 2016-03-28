/***************************************************************************
 * 
 * Copyright (c) 2016 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file rpc_attachment.h
 * @author zhangdi05(zhangdi05@baidu.com)
 * @date 2016/03/16 11:14:32
 * @version $Revision$ 
 * @brief 
 *  
 **/
#ifndef RPC_ATTACHMENT_H
#define RPC_ATTACHMENT_H

#include <sofa/pbrpc/smart_ptr/shared_ptr.hpp>
#include <sofa/pbrpc/buffer.h>

namespace sofa {
namespace pbrpc {

class RpcAttachment;
typedef sofa::pbrpc::shared_ptr<RpcAttachment> RpcAttachmentPtr;

class RpcAttachment
{
public:
    virtual ~RpcAttachment() {}

    virtual bool Serialize(ReadBufferPtr& attachment_buffer) = 0;

    virtual bool Deserialize(ReadBufferPtr& attachment_buffer) = 0;
};

} // namespace pbrpc
} // namespace sofa

#endif  // RPC_ATTACHMENT_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
