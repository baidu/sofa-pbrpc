// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: zhangdi05@baidu.com (Zhangdi Di)

#include "common.h"
#include "ext_rpc_service_stub_impl.h"

namespace sofa_php_ext
{

zval* PhpRpcServiceStubImpl::GetFieldDescriptors(zval* obj)
{
    zval method;
    zval* descriptors
    TSRMLS_FETCH();
    INIT_ZVAL(method);
    ZVAL_STRINGL(&method, PB_FIELDS_METHOD.c_str(), PB_FIELDS_METHOD.length(), 0);
    if (call_user_function_ex(NULL, &obj, &method, &descriptors, 0, NULL, 0, NULL TSRMLS_CC) != SUCCESS)
    {
        return NULL;
    }
    return descriptors;
}

zval** PhpRpcServiceStubImpl::GetFieldType(zval** field_descriptor, uint32_t field_number)
{
    zval** field_type;
    TSRMLS_FETCH();
    if (zend_hash_find(Z_ARRVAL_PP((field_descriptor)), 
                PB_FIELD_TYPE.c_str(), PB_FIELD_TYPE.length() + 1, 
                (void**) &(field_type)) == FAILURE)
    {
        return NULL;
    }
    return field_type;
}

zval** PhpRpcServiceStubImpl::GetRequiredLabel(zval** field_descriptor, uint32_t field_number)
{
    zval** field_required;
    TSRMLS_FETCH();
    if (zend_hash_find(Z_ARRVAL_PP((field_descriptor)), 
                PB_FIELD_REQUIRED.c_str(), 
                PB_FIELD_REQUIRED.length() + 1, 
                (void**) &(field_required)) == FAILURE)
    {
        return NULL;
    }
    return field_required;
}

zval** PhpRpcServiceStubImpl::GetRepeatedLabel(zval** field_descriptor, uint32_t field_number)
{
    zval** field_repeated;
    TSRMLS_FETCH();
    if (zend_hash_find(Z_ARRVAL_PP((field_descriptor)), 
                PB_FIELD_REPEATED.c_str(), 
                PB_FIELD_REPEATED.length() + 1, 
                (void**) &(field_repeated)) == FAILURE)
    {
        return NULL;
    }
    return field_repeated;
}

const char* PhpRpcServiceStubImpl::GetFieldName(zval** field_descriptor)
{
    zval** field_name;
    TSRMLS_FETCH();
    if (zend_hash_find(Z_ARRVAL_PP((field_descriptor)), 
                PB_FIELD_NAME.c_str(), 
                PB_FIELD_NAME.length() + 1, 
                (void**) &(field_name)))
    {
        return NULL;
    }
    return (const char*) Z_STRVAL_PP(field_name);
}

zval** PhpRpcServiceStubImpl::GetUserValues(zval* obj)
{
    zval** values;
    TSRMLS_FETCH();
    zend_hash_find((Z_OBJPROP_P(obj)), 
            PB_VALUES_PROPERTY.c_str(), 
            PB_VALUES_PROPERTY.length() + 1, 
            (void**) &(values));
    return values;
}

void PhpRpcServiceStubImpl::GetMessageType(zval* obj, 
                                           std::string& message_type) 
{
    zval method;
    zval* msg_type;
    TSRMLS_FETCH();
    INIT_ZVAL(method);
    ZVAL_STRINGL(&method, CLASS_TYPE.c_str(), CLASS_TYPE.length(), 0);
    call_user_function_ex(NULL, &obj, &method, &msg_type, 0, NULL, 0, NULL TSRMLS_CC);
    convert_to_string(msg_type);
    message_type = Z_STRVAL_P(msg_type);
}

zval* PhpRpcServiceStubImpl::GetSubMessage(zval* obj,
                                        ulong field_number)
{
    zval** tmp_val;
    zval** sub_msg;
    zval** values;
    if ((values = GetUserValues(obj)) == NULL)
    {
        return NULL;
    }
    if (zend_hash_index_find((Z_ARRVAL_PP(values)), field_number, (void**) &(sub_msg)) == FAILURE) 
    {
        return NULL;
    }
    if (Z_TYPE_PP(sub_msg) == IS_ARRAY) 
    {
        if (zend_hash_index_find((Z_ARRVAL_PP(sub_msg)), 0, (void**) &(tmp_val)) == FAILURE)
        {
            return NULL;
        }
        sub_msg = tmp_val;
    }
    return *sub_msg;
}

int PhpRpcServiceStubImpl::GetSofaFieldType(zval** type, 
                                            google::protobuf::FieldDescriptorProto::Type& pb_type) 
{
    if (Z_TYPE_PP(type) == IS_STRING)
    {
        pb_type = google::protobuf::FieldDescriptorProto::TYPE_MESSAGE;
    }
    else if (Z_TYPE_PP(type) == IS_LONG)
    {
        switch (Z_LVAL_PP(type))
        {   
        case PB_TYPE_DOUBLE:
        {
            pb_type = google::protobuf::FieldDescriptorProto::TYPE_DOUBLE;
            break;
        }
        case PB_TYPE_FIXED32:
        {
            pb_type = google::protobuf::FieldDescriptorProto::TYPE_FIXED32;
            break;
        }
        case PB_TYPE_FIXED64:
        {
            pb_type = google::protobuf::FieldDescriptorProto::TYPE_FIXED64;
            break;
        }
        case PB_TYPE_FLOAT:
        {
            pb_type = google::protobuf::FieldDescriptorProto::TYPE_FLOAT;
            break;
        }
        case PB_TYPE_INT:
        {
            pb_type = google::protobuf::FieldDescriptorProto::TYPE_INT32;
            break;
        }
        case PB_TYPE_SIGNED_INT:
        {
            pb_type = google::protobuf::FieldDescriptorProto::TYPE_SINT32;
            break;
        }
        case PB_TYPE_STRING:
        {
            pb_type = google::protobuf::FieldDescriptorProto::TYPE_STRING;
            break;
        }
        case PB_TYPE_BOOL:
        {
            pb_type = google::protobuf::FieldDescriptorProto::TYPE_BOOL;
            break;
        }
        case PB_TYPE_UINT64:
        {
            pb_type = google::protobuf::FieldDescriptorProto::TYPE_UINT64;
            break;
        }
        default:
            SLOG(ERROR, "transform pb type to sofa failed for bad type");
            return -1;
        }
    }
    else
    {
        SLOG(ERROR, "transform pb type to sofa failed for bad type");
        return -1;
    }
    return 0;
}

int PhpRpcServiceStubImpl::ToSofaField(zval** type, 
                                     const bool repeated,
                                     zval** value, 
                                     google::protobuf::Message* sofa_msg, 
                                     const google::protobuf::FieldDescriptor* field, 
                                     const google::protobuf::Reflection* reflection) 
{
    if (!reflection || !field) 
    {
        SLOG(ERROR, "transform to sofa field failed for sofa message not initialized");
        return -1;
    }
    int array_size;
    if (repeated)
    {
        array_size = reflection->FieldSize(*sofa_msg, field);
    }
    switch (Z_LVAL_PP(type))
    {
    case PB_TYPE_DOUBLE:
    {
        if (repeated) 
        {
            zval** array = value;
            zval** item;
            HashPosition i;
            EXT_ZEND_FOREACH(&i, Z_ARRVAL_PP(value))
            {
                zend_hash_get_current_data_ex(Z_ARRVAL_PP(array), (void **) &item, &i);
                reflection->AddDouble(sofa_msg, field, Z_DVAL_PP(item));
            }
        }
        else 
        {
            reflection->SetDouble(sofa_msg, field, Z_DVAL_PP(value));
        }
        break;
    }
    case PB_TYPE_FIXED32:
    {
        if (repeated) 
        {
            zval** array = value;
            zval** item;
            HashPosition i;
            EXT_ZEND_FOREACH(&i, Z_ARRVAL_PP(value))
            {
                zend_hash_get_current_data_ex(Z_ARRVAL_PP(array), (void **)&item, &i);
                reflection->AddUInt32(sofa_msg, field, Z_LVAL_PP(item));
            }
        } 
        else 
        {
            reflection->SetUInt32(sofa_msg, field, Z_LVAL_PP(value));
        }
        break;
    }
    case PB_TYPE_INT:
    {
        if (repeated) 
        {
            zval** array = value;
            zval** item;
            HashPosition i;
            EXT_ZEND_FOREACH(&i, Z_ARRVAL_PP(value))
            {
                zend_hash_get_current_data_ex(Z_ARRVAL_PP(array), (void **)&item, &i);
                reflection->AddInt32(sofa_msg, field, Z_LVAL_PP(item));
            }
        } 
        else 
        {
            reflection->SetInt32(sofa_msg, field, Z_LVAL_PP(value));
        }
        break;
    }
    case PB_TYPE_BOOL:
    {
        if (repeated) 
        {
            zval** array = value;
            zval** item;
            HashPosition i;
            EXT_ZEND_FOREACH(&i, Z_ARRVAL_PP(value))
            {
                zend_hash_get_current_data_ex(Z_ARRVAL_PP(array), (void **)&item, &i);
                reflection->AddBool(sofa_msg, field, Z_LVAL_PP(item));
            }
        } 
        else 
        {
            reflection->SetBool(sofa_msg, field, Z_LVAL_PP(value));
        }
        break;
    }
    case PB_TYPE_FIXED64:
    {
        if (repeated) 
        {
            zval** array = value;
            zval** item;
            HashPosition i;
            EXT_ZEND_FOREACH(&i, Z_ARRVAL_PP(value))
            {
                zend_hash_get_current_data_ex(Z_ARRVAL_PP(array), (void **)&item, &i);
                reflection->AddUInt64(sofa_msg, field, Z_LVAL_PP(item));
            }
        } 
        else 
        {
            reflection->SetUInt64(sofa_msg, field, Z_LVAL_PP(value));
        }
        break;
    }
    case PB_TYPE_FLOAT:
    {
        if (repeated) 
        {
            zval** array = value;
            zval** item;
            HashPosition i;
            EXT_ZEND_FOREACH(&i, Z_ARRVAL_PP(value))
            {
                zend_hash_get_current_data_ex(Z_ARRVAL_PP(array), (void **)&item, &i);
                reflection->AddFloat(sofa_msg, field, Z_DVAL_PP(item));
            }
        } 
        else 
        {
            reflection->SetFloat(sofa_msg, field, Z_DVAL_PP(value));
        }
        break;
    }
    case PB_TYPE_SIGNED_INT:
    {
        if (repeated) 
        {
            zval** array = value;
            zval** item;
            HashPosition i;
            EXT_ZEND_FOREACH(&i, Z_ARRVAL_PP(value))
            {
                zend_hash_get_current_data_ex(Z_ARRVAL_PP(array), (void **)&item, &i);
                reflection->AddInt32(sofa_msg, field, Z_LVAL_PP(item));
            }
        } 
        else 
        {
            reflection->SetInt32(sofa_msg, field, Z_LVAL_PP(value));
        }
        break;
    }
    case PB_TYPE_STRING:
    {
        if (repeated) 
        {
            zval** array = value;
            zval** item;
            HashPosition i;
            EXT_ZEND_FOREACH(&i, Z_ARRVAL_PP(value))
            {
                zend_hash_get_current_data_ex(Z_ARRVAL_PP(array), (void **)&item, &i);
                reflection->AddString(sofa_msg, field, Z_STRVAL_PP(item));
            }
        } 
        else 
        {
            reflection->SetString(sofa_msg, field, Z_STRVAL_PP(value));
        }
        break;
    }
    case PB_TYPE_UINT64:
    {
        if (repeated) 
        {
            zval** array = value;
            zval** item;
            HashPosition i;
            EXT_ZEND_FOREACH(&i, Z_ARRVAL_PP(value))
            {
                zend_hash_get_current_data_ex(Z_ARRVAL_PP(array), (void **)&item, &i);
                reflection->AddUInt64(sofa_msg, field, Z_LVAL_PP(item));
            }
        } 
        else 
        {
            reflection->SetUInt64(sofa_msg, field, Z_LVAL_PP(value));
        }
        break;
    }
    default:
        SLOG(ERROR, "transform to sofa field failed for bad field");
        return -1;
    }
    return 0;
}

int PhpRpcServiceStubImpl::PhpTransformToSofa(zval* user_msg, 
                                             google::protobuf::Message* sofa_msg) 
{
    HashPosition i;
    const google::protobuf::Reflection* reflection = sofa_msg->GetReflection();
    const google::protobuf::Descriptor* message_descriptor = sofa_msg->GetDescriptor();

    if (!message_descriptor || !reflection) 
    {
        SLOG(ERROR, "transform to sofa message failed for sofa message is not initialized");
        return -1;
    }
    zval* field_descriptors = GetFieldDescriptors(user_msg);
    if (!field_descriptors)
    {
        SLOG(ERROR, "transform to sofa message failed for field_descriptor is null");
        return -1;
    }
    zval** values = GetUserValues(user_msg);
    EXT_ZEND_FOREACH(&i, Z_ARRVAL_P(field_descriptors)) 
    {
        ulong field_number;
        zval** value;
        zval** type;
        zval** required;
        bool repeated = false;
        zval** field_descriptor;
        zend_hash_get_current_key_ex(Z_ARRVAL_P(field_descriptors), NULL, NULL, &field_number, 0, &i);
        zend_hash_get_current_data_ex(Z_ARRVAL_P(field_descriptors), (void **) &field_descriptor, &i);
        if (zend_hash_index_find(Z_ARRVAL_PP(values), field_number, (void **) &value) == FAILURE) 
        {
            return -1;
        }
        if (Z_TYPE_PP(value) == IS_NULL)
        {
            continue;
        }
        if (Z_TYPE_PP(value) == IS_ARRAY) 
        {
            repeated = true;
        }
        if ((type = GetFieldType(field_descriptor, field_number)) == NULL)
        {
            return -1;
        }
        const char* field_name = GetFieldName(field_descriptor);
        if (!field_name)
        {
            SLOG(ERROR, "get field_name failed in field %d", field_number);
            return -1;
        }
        const google::protobuf::FieldDescriptor* field 
            = message_descriptor->FindFieldByName(field_name);
        if (!field)
        {
            SLOG(ERROR, "field %s not found", field_name);
            return -1;
        }
        if (Z_TYPE_PP(type) == IS_LONG)
        {
            if (Z_TYPE_PP(value) == IS_NULL) 
            {
                if (zend_hash_find(Z_ARRVAL_PP(field_descriptor), 
                            PB_FIELD_REQUIRED.c_str(), 
                            PB_FIELD_REQUIRED.length(), 
                            (void **) &required) == FAILURE) 
                {
                    return -1;
                }

                if (Z_BVAL_PP(required)) 
                {
                    zend_throw_exception_ex(NULL, 
                            0 TSRMLS_CC, 
                            "'%s' field is required and must be set", 
                            field_name);
                    return -1;
                }
                continue;
            }
            if(ToSofaField(type, repeated, value, sofa_msg, field, reflection) != 0)
            {
                SLOG(ERROR, "transform to sofa field failed");
                return -1;
            }	
        }
        else if(Z_TYPE_PP(type) == IS_STRING)
        {
            if (repeated)
            {
                zval** array = value;
                HashPosition i;
                EXT_ZEND_FOREACH (&i, Z_ARRVAL_PP(value))
                {
                    zval** item;
                    google::protobuf::Message* sub_message_item = reflection->AddMessage(sofa_msg, field);
                    zend_hash_get_current_data_ex(Z_ARRVAL_PP(array), (void **) &item, &i);
                    PhpTransformToSofa(*item, sub_message_item); 
                }
            }
            else
            {
                google::protobuf::Message* sub_message = reflection->MutableMessage(sofa_msg, field);
                PhpTransformToSofa(*value, sub_message);
            }
        }
    }
    return 0;
}

int PhpRpcServiceStubImpl::ToUserField(zval* value, 
                                     bool repeated,
                                     int array_size,
                                     google::protobuf::FieldDescriptor::Type sofa_type, 
                                     const google::protobuf::Message* sofa_msg, 
                                     const google::protobuf::FieldDescriptor* field, 
                                     const google::protobuf::Reflection* reflection) 
{
    if (!field || ! reflection) 
    {
        SLOG(ERROR, "transform to user field failed for sofa message not initialized");
        return -1;
    }
    switch (sofa_type)
    {
    case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
    {
        if (repeated) 
        {
            array_init(value);
            for (int i = 0; i < array_size; ++i) 
            {
                zval* user_item;
                ALLOC_INIT_ZVAL(user_item);
                double sofa_item = reflection->GetRepeatedDouble(*sofa_msg, field, i);  
                ZVAL_DOUBLE(user_item, sofa_item);
                add_next_index_zval(value, user_item);
                zval_ptr_dtor(&user_item);
            }
        }
        else 
        {
            double item = reflection->GetDouble(*sofa_msg, field);
            ZVAL_DOUBLE(value, item);
        }
        break;
    }
    case google::protobuf::FieldDescriptor::TYPE_FLOAT:
    {
        if (repeated) 
        {
            array_init(value);
            for (int i = 0; i < array_size; ++i) 
            {   
                zval* user_item;
                ALLOC_INIT_ZVAL(user_item);
                float item = reflection->GetRepeatedFloat(*sofa_msg, field, i); 
                ZVAL_DOUBLE(user_item, item);
                add_next_index_zval(value, user_item);
                zval_ptr_dtor(&user_item);
            }
        }
        else 
        {
            float item = reflection->GetFloat(*sofa_msg, field);
            ZVAL_DOUBLE(value, item);
        }
        break;
    }
    case google::protobuf::FieldDescriptor::TYPE_INT32:
    {
        if (repeated) 
        {
            array_init(value);
            for (int i = 0; i < array_size; ++i) 
            {   
                zval* user_item;
                ALLOC_INIT_ZVAL(user_item);
                int32_t item = reflection->GetRepeatedInt32(*sofa_msg, field, i); 
                ZVAL_LONG(user_item, item);
                add_next_index_zval(value, user_item);
                zval_ptr_dtor(&user_item);
            }
        }
        else 
        {
            int32_t item = reflection->GetInt32(*sofa_msg, field);
            ZVAL_LONG(value, item);
        }
        break;
    }
    case google::protobuf::FieldDescriptor::TYPE_INT64:
    {
        if (repeated) 
        {
            array_init(value);
            for (int i = 0; i < array_size; ++i) 
            {
                zval* user_item;
                ALLOC_INIT_ZVAL(user_item);
                int64_t item = reflection->GetRepeatedInt64(*sofa_msg, field, i); 
                ZVAL_LONG(user_item, item);
                add_next_index_zval(value, user_item);
                zval_ptr_dtor(&user_item);
            }
        }
        else 
        {
            int64_t item = reflection->GetInt64(*sofa_msg, field);
            ZVAL_LONG(value, item);
        }
        break;
    }
    case google::protobuf::FieldDescriptor::TYPE_UINT64:
    {
        if (repeated)
        {
            array_init(value);
            for (int i = 0; i < array_size; ++i) 
            {
                zval* user_item;
                ALLOC_INIT_ZVAL(user_item);
                uint64_t item = reflection->GetRepeatedUInt64(*sofa_msg, field, i); 
                ZVAL_LONG(user_item, item);
                add_next_index_zval(value, user_item);
                zval_ptr_dtor(&user_item);
            }
        }
        else 
        {
            uint64_t item = reflection->GetUInt64(*sofa_msg, field);
            ZVAL_LONG(value, item);
        }
        break;
    }
    case google::protobuf::FieldDescriptor::TYPE_BOOL:
    {
        if (repeated) 
        {
            array_init(value);
            for (int i = 0; i < array_size; ++i) 
            {
                zval* user_item;
                ALLOC_INIT_ZVAL(user_item);
                bool item = reflection->GetRepeatedBool(*sofa_msg, field, i); 
                ZVAL_BOOL(user_item, item);
                add_next_index_zval(value, user_item);
                zval_ptr_dtor(&user_item);
            }
        }
        else 
        {
            bool item = reflection->GetBool(*sofa_msg, field);
            ZVAL_BOOL(value, item);
        }
        break;
    }
    case google::protobuf::FieldDescriptor::TYPE_STRING:
    {
        if (repeated) 
        {
            array_init(value);
            for (int i = 0; i < array_size; ++i) 
            {
                zval* user_item;
                ALLOC_INIT_ZVAL(user_item);
                std::string item = reflection->GetRepeatedString(*sofa_msg, field, i); 
                ZVAL_STRINGL(user_item, item.c_str(), item.length(), 0);
                add_next_index_zval(value, user_item);
                zval_ptr_dtor(&user_item);
            }
        }
        else 
        {
            std::string item = reflection->GetString(*sofa_msg, field);
            ZVAL_STRINGL(value, item.c_str(), item.length(), 0);
        }
        break;
    }
    default:
        SLOG(ERROR, "transform sofa to user field failed for bad type");
        return -1;
    }
    return 0;
}

int PhpRpcServiceStubImpl::CreateMessageDescriptor(zval* message,
                                                 zval* message_descriptor,
                                                 google::protobuf::DescriptorProto* message_proto,
                                                 google::protobuf::FileDescriptorProto* file_proto) 
{
    int index = 1;
    HashPosition i;
    EXT_ZEND_FOREACH(&i, Z_ARRVAL_P(message_descriptor)) 
    {
        ulong field_number;
        zval** field_descriptor;
        zval** value;
        zval** type;

        zend_hash_get_current_key_ex(Z_ARRVAL_P(message_descriptor), NULL, NULL, &field_number, 0, &i);
        zend_hash_get_current_data_ex(Z_ARRVAL_P(message_descriptor), (void **) &field_descriptor, &i);
        std::string field_name = GetFieldName(field_descriptor);
        if ((type = GetFieldType(field_descriptor, field_number)) == NULL)
        {
            return -1;
        }
        zval** repeated_zval = GetRepeatedLabel(field_descriptor, field_number);
        bool repeated = repeated_zval? Z_BVAL_PP(repeated_zval):false;
        zval** required_zval = GetRequiredLabel(field_descriptor, field_number);
        bool required = required_zval? Z_BVAL_PP(required_zval):false;
        if (Z_TYPE_PP(type) == IS_STRING)
        {
            std::string type_str = (const char*)Z_STRVAL_PP(type);
            zval* sub_user_message = GetSubMessage(message, field_number);
            CreateMessage(sub_user_message, type_str, file_proto);
        }
        google::protobuf::FieldDescriptorProto* field_proto = message_proto->add_field();
        field_proto->set_name(field_name);
        google::protobuf::FieldDescriptorProto::Type sofa_type;
        int ret = GetSofaFieldType(type, sofa_type); 
        if (-1 == ret) 
        {
            SLOG(ERROR, "create message descriptor failed in field %s \
                for bad pb field type", field_name.c_str());
            return -1;
        }
        if (sofa_type == google::protobuf::FieldDescriptorProto::TYPE_MESSAGE)
        {
            field_proto->set_type_name((const char*)Z_STRVAL_PP(type));
        }
        field_proto->set_type(sofa_type);
        field_proto->set_number(index);
        
        if (required_zval) 
        {
            if (Z_BVAL_PP(required_zval))
            {
                field_proto->set_label(google::protobuf::FieldDescriptorProto::LABEL_REQUIRED);
            }
            else
            {
                field_proto->set_label(google::protobuf::FieldDescriptorProto::LABEL_OPTIONAL);
            }
        }
        else if (repeated_zval != NULL && Z_BVAL_PP(repeated_zval)) 
        {
            field_proto->set_label(google::protobuf::FieldDescriptorProto::LABEL_REPEATED);
        }
        else
        {
            SLOG(ERROR, "create message descriptor failed in field %s \
                for label not found", field_name.c_str());
            return -1;
        }
        ++index;
    }
    return 0;
}

int PhpRpcServiceStubImpl::SofaTransformToPhp(zval* user_msg,
                                            const google::protobuf::Message* sofa_msg) 
{
    std::string response_str;
    sofa_msg->SerializeToString(&response_str);
    zval method;
    zval arg;
    zval ret;
    zval* args;
    INIT_ZVAL(method);
    ZVAL_STRINGL(&method, PARSE_FUNCTION.c_str(), PARSE_FUNCTION.length(), 0);
    INIT_ZVAL(arg);
    ZVAL_STRINGL(&arg, response_str.c_str(), response_str.length(), 0);
    args = &arg;
    if (call_user_function(NULL, &user_msg, &method, &ret, 1, &args TSRMLS_CC) == FAILURE)
    {
        return -1;
    }
    return 0;
}

int PhpRpcServiceStubImpl::CreateMessage(zval* message,
                                       const std::string& message_type, 
                                       google::protobuf::FileDescriptorProto* file_proto)
{
    if (_message_set->find(message_type) != _message_set->end())
    {
        return 0;
    }
    google::protobuf::DescriptorProto* message_proto = file_proto->add_message_type();
    if (!message_proto) 
    {
        SLOG(FATAL, "create message %s proto failed for add message type error", message_type.c_str());
        return -1;
    }
    message_proto->set_name(message_type);
    zval* message_descriptor = GetFieldDescriptors(message);
    if (!message_descriptor)
    {
        SLOG(ERROR, "create message %s proto failed for message_descriptor is null", message_type.c_str());
        return -1;
    }
    int ret = CreateMessageDescriptor(message, message_descriptor, message_proto, file_proto);
    if (ret)
    {
        SLOG(FATAL, "create message %s failed for create message descriptor error", message_type.c_str());
        return -1;
    }
    _message_set->insert(message_type);
    return 0;
}

void PhpRpcServiceStubImpl::SetTimeout(long timeout)
{
    _timeout = timeout;
}

bool PhpRpcServiceStubImpl::Failed()
{
    return _cntl->Failed();
}

std::string PhpRpcServiceStubImpl::ErrorText()
{
    return _cntl->ErrorText();
}

void PhpRpcServiceStubImpl::RegisterMethod(const std::string& method_name,
                                          zval* request,
                                          zval* response)
{
    if (!_service_proto)
    {
        SLOG(FATAL, "init method failed for service not initialized");
        return;
    }
    MethodBoard::iterator it = _method_board->find(method_name.data());
    if (_method_board->end() != it) 
    {
        SLOG(NOTICE, "method %s already registered", method_name.data());
        return;
    }
    google::protobuf::MethodDescriptorProto* method_proto = _service_proto->add_method();
    if (!method_proto)
    {
        SLOG(FATAL, "register method %s to service %s failed", \
            method_name.data(),
            _service_name.c_str()); 
        return;
    }
    method_proto->set_name(method_name.data());
    std::string request_type, response_type;
    GetMessageType(request, request_type);
    GetMessageType(response, response_type);
    method_proto->set_input_type(request_type);
    method_proto->set_output_type(response_type);
    int ret = CreateMessage(request, request_type, _file_proto);
    if (ret != 0) 
    {
        SLOG(FATAL, "init method %s failed for create message %s failed", \
            method_name.data(), request_type.c_str());
        return;
    }

    ret = CreateMessage(response, response_type, _file_proto);
    if (ret != 0) 
    {
        SLOG(FATAL, "init method %s failed for create message %s failed", \
            method_name.data(), response_type.c_str());
        return;
    }
    MethodWrapper* mwrapper = new MethodWrapper();
    _method_board->insert(std::make_pair(method_name.data(), mwrapper));
}

void PhpRpcServiceStubImpl::InitMethods()
{
    _file_descriptor = _pool->BuildFile(*_file_proto);
    if (!_file_descriptor) {
        SLOG(FATAL, "init methods failed for build rpc descriptor failed");
        return;
    }
    const google::protobuf::ServiceDescriptor* service_descriptor 
        = _file_descriptor->FindServiceByName(_service_name);
    if (!service_descriptor) 
    {
        SLOG(FATAL, "init methods failed for service %s not found in file proto", \
            _service_name.c_str());
        return;
    }
    MethodBoard::iterator it = _method_board->begin();
    MethodBoard::iterator end = _method_board->end();
    for (; it != end; ++it)
    {
        std::string method_name = it->first;
        MethodWrapper* mwrapper = it->second;
        const google::protobuf::MethodDescriptor* method_descriptor 
            = service_descriptor->FindMethodByName(method_name);
        if (!method_descriptor) 
        {
            SLOG(ERROR, "init methods failed for method %s not found", method_name.c_str());
            return;
        }
        const google::protobuf::Descriptor* request = method_descriptor->input_type();
        const google::protobuf::Descriptor* response = method_descriptor->output_type();
        mwrapper->_method = method_descriptor;
        mwrapper->_request = request;
        mwrapper->_response = response;
    }
}

void PhpRpcServiceStubImpl::CallMethod(const std::string& method_name,
                                     zval* request, 
                                     zval* response, 
                                     zval* closure)
{
    MethodBoard::iterator it = _method_board->find(method_name.data());
    if (it == _method_board->end()) 
    {
        SLOG(FATAL, "method %s not found", method_name.data());
        return;
    }
    MethodWrapper* mwrapper = it->second; 
    if (!mwrapper || !mwrapper->_method \
                  || !mwrapper->_response) 
    {
        SLOG(FATAL, "PhpRpcServiceStub call method failed for method %s not \
            initialized", method_name.data());
        return;
    }
    google::protobuf::DynamicMessageFactory factory(_pool);
    const google::protobuf::Message* request_message = factory.GetPrototype(mwrapper->_request);
    const google::protobuf::Message* response_message = factory.GetPrototype(mwrapper->_response);
    google::protobuf::Message* request_instance = request_message->New();
    google::protobuf::Message* response_instance = response_message->New();

    if (0 != (PhpTransformToSofa(request, request_instance)))
    {
        delete request_instance;
        delete response_instance;
        return;
    }
    _cntl->Reset();
    _cntl->SetTimeout(_timeout);
    _channel->CallMethod(mwrapper->_method, 
                         _cntl, 
                         request_instance, 
                         response_instance, 
                         NULL);
    
    SofaTransformToPhp(response, response_instance);
    delete request_instance;
    delete response_instance;
}
    
}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
