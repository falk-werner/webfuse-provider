#include "webfuse_provider/impl/operation/readdir.h"
#include "webfuse_provider/impl/operation/error.h"
#include "webfuse_provider/impl/dirbuffer.h"
#include "webfuse_provider/impl/request.h"
#include "webfuse_provider/impl/message_writer.h"
#include "webfuse_provider/impl/util/util.h"
#include "webfuse_provider/impl/json/node.h"

void wfp_impl_readdir(
    struct wfp_impl_invokation_context * context,
    struct wfp_json const * params,
    int id)
{
    size_t const count = wfp_impl_json_array_size(params);
    if (2 == count)
    {
        struct wfp_json const * inode_holder = wfp_impl_json_array_get(params, 1);

        if (wfp_impl_json_is_int(inode_holder))
        {
            ino_t inode = (ino_t) wfp_impl_json_int_get(inode_holder);
            struct wfp_request * request = wfp_impl_request_create(context->request, id);

            context->provider->readdir(request, inode, context->user_data);
        }
    }
}

void wfp_impl_readdir_default(
    struct wfp_request * request,
    ino_t WFP_UNUSED_PARAM(directory),
    void * WFP_UNUSED_PARAM(user_data))
{
    wfp_impl_respond_error(request, WFP_BAD_NOENTRY);
}

void wfp_impl_respond_readdir(
    struct wfp_request * request,
    struct wfp_dirbuffer * dirbuffer)
{
    struct wfp_message_writer * writer = wfp_impl_request_get_writer(request);
    wfp_impl_message_writer_add_dirbuffer(writer, dirbuffer);
    wfp_impl_respond(request);
}

