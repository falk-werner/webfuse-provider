#include "webfuse_provider/impl/operation/read.h"

#include <stdlib.h>

#include "webfuse_provider/impl/operation/error.h"
#include "webfuse_provider/impl/request.h"
#include "webfuse_provider/impl/message_writer.h"
#include "webfuse_provider/impl/util/util.h"

void wfp_impl_read(
    struct wfp_impl_invokation_context * context,
    json_t * params,
    int id)
{
    size_t const count = json_array_size(params);
    if (5 == count)
    {
        json_t * inode_holder = json_array_get(params, 1);
        json_t * handle_holder = json_array_get(params, 2);
        json_t * offset_holder = json_array_get(params, 3);
        json_t * length_holder = json_array_get(params, 4);

        if (json_is_integer(inode_holder) &&
            json_is_integer(handle_holder) &&
            json_is_integer(offset_holder) &&
            json_is_integer(length_holder))
        {
            ino_t inode = (ino_t) json_integer_value(inode_holder);
            int handle = json_integer_value(handle_holder);
            size_t offset = json_integer_value(offset_holder);
            size_t length = json_integer_value(length_holder);
            struct wfp_request * request = wfp_impl_request_create(context->request, id);

            context->provider->read(request, inode, handle, offset, length, context->user_data);
        }
    } 
}

void wfp_impl_read_default(
    struct wfp_request * request,
    ino_t WFP_UNUSED_PARAM(inode),
    uint32_t WFP_UNUSED_PARAM(handle),
    size_t WFP_UNUSED_PARAM(offset),
    size_t WFP_UNUSED_PARAM(length),
    void * WFP_UNUSED_PARAM(user_data))
{
    wfp_impl_respond_error(request, WFP_BAD_NOENTRY);
}

void wfp_impl_respond_read(
    struct wfp_request * request,
    char const * data,
    size_t length)
{
    struct wfp_message_writer * writer = wfp_impl_request_get_writer(request);

    if (0 < length)
    {
        wfp_impl_message_writer_add_bytes(writer, "data", data, length);
        wfp_impl_message_writer_add_string(writer, "format", "base64");
    }
    else
    {
        wfp_impl_message_writer_add_string(writer, "data", "");
        wfp_impl_message_writer_add_string(writer, "format", "identity");
    }

    wfp_impl_message_writer_add_int(writer, "count", ((int) length));
    wfp_impl_respond(request);        
}