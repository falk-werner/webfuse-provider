#include "webfuse_provider/impl/operation/close.h"
#include <limits.h>
#include "webfuse_provider/impl/util/util.h"
#include "webfuse_provider/impl/json/node.h"

void wfp_impl_close(
    struct wfp_impl_invokation_context * context,
    struct wfp_json const * params,
    int WFP_UNUSED_PARAM(id))
{
    size_t const param_count = wfp_impl_json_array_size(params);
    if (4 == param_count)
    {
        struct wfp_json const  * inode_holder = wfp_impl_json_array_get(params, 1);
        struct wfp_json const  * handle_holder = wfp_impl_json_array_get(params, 2);
        struct wfp_json const  * flags_holder = wfp_impl_json_array_get(params, 3);

        if (wfp_impl_json_is_int(inode_holder) && 
            wfp_impl_json_is_int(handle_holder) &&
            wfp_impl_json_is_int(flags_holder))
        {
            ino_t inode = (ino_t) wfp_impl_json_int_get(inode_holder);
            uint32_t handle = (uint32_t) (wfp_impl_json_int_get(handle_holder) & UINT32_MAX);
            int flags = wfp_impl_json_int_get(flags_holder);

            context->provider->close(inode, handle, flags, context->user_data);
        }
    }

}

void wfp_impl_close_default(
    ino_t WFP_UNUSED_PARAM(inode),
    uint32_t WFP_UNUSED_PARAM(handle),
    int WFP_UNUSED_PARAM(flags),
    void * WFP_UNUSED_PARAM(user_data))
{
    // empty
}