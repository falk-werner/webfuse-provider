#include "webfuse_provider/impl/operation/readdir.h"
#include "webfuse_provider/impl/operation/error.h"
#include "webfuse_provider/impl/dirbuffer.h"
#include "webfuse_provider/impl/request.h"
#include "webfuse_provider/impl/util.h"

void wfp_impl_readdir(
    struct wfp_impl_invokation_context * context,
    json_t * params,
    int id)
{
    size_t const count = json_array_size(params);
    if (2 == count)
    {
        json_t * inode_holder = json_array_get(params, 1);

        if (json_is_integer(inode_holder))
        {
            ino_t inode = (ino_t) json_integer_value(inode_holder);
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
    json_t * result = wfp_impl_dirbuffer_take(dirbuffer);
    wfp_impl_respond(request, result);
}

