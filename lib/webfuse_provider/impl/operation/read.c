#include "webfuse_provider/impl/operation/read.h"

#include <stdlib.h>

#include "webfuse_provider/impl/operation/error.h"
#include "webfuse_provider/impl/request.h"
#include "webfuse_provider/impl/message_writer.h"
#include "webfuse_provider/impl/util/util.h"
#include "webfuse_provider/impl/json/node.h"

void wfp_impl_read(
    struct wfp_impl_invokation_context * context,
    struct wfp_json const * params,
    int id)
{
    size_t const count = wfp_impl_json_array_size(params);
    if (5 == count)
    {
        struct wfp_json const * inode_holder = wfp_impl_json_array_get(params, 1);
        struct wfp_json const * handle_holder = wfp_impl_json_array_get(params, 2);
        struct wfp_json const * offset_holder = wfp_impl_json_array_get(params, 3);
        struct wfp_json const * length_holder = wfp_impl_json_array_get(params, 4);

        if (wfp_impl_json_is_int(inode_holder) &&
            wfp_impl_json_is_int(handle_holder) &&
            wfp_impl_json_is_int(offset_holder) &&
            wfp_impl_json_is_int(length_holder))
        {
            ino_t inode = (ino_t) wfp_impl_json_int_get(inode_holder);
            int handle = wfp_impl_json_int_get(handle_holder);
            size_t offset = wfp_impl_json_int_get(offset_holder);
            size_t length = wfp_impl_json_int_get(length_holder);
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