// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: zhangdi05@baidu.com (Zhang Di)

#ifndef _SOFA_PBRPC_RPC_ATTACHMENT_H_
#define _SOFA_PBRPC_RPC_ATTACHMENT_H_

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
}; // class RpcAttachment

} // namespace pbrpc
} // namespace sofa

#endif  // _SOFA_PBRPC_RPC_ATTACHMENT_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
