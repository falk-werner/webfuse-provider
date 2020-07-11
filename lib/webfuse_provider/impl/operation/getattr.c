#include "webfuse_provider/impl/operation/getattr.h"

#include <stdbool.h>

#include "webfuse_provider/impl/operation/error.h"
#include "webfuse_provider/impl/request.h"
#include "webfuse_provider/impl/message_writer.h"
#include "webfuse_provider/impl/util/util.h"
#include "webfuse_provider/impl/json/node.h"


void wfp_impl_getattr(
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
            ino_t inode = (ino_t) wfp_impl_json_get_int(inode_holder);
            struct wfp_request * request = wfp_impl_request_create(context->request, id);

            context->provider->getattr(request, inode, context->user_data);
        }
    }
}

void wfp_impl_getattr_default(
    struct wfp_request * request,
    ino_t WFP_UNUSED_PARAM(inode),
    void * WFP_UNUSED_PARAM(user_data))
{
    wfp_impl_respond_error(request, WFP_BAD_NOENTRY);
}

void wfp_impl_respond_getattr(
    struct wfp_request * request,
    struct stat const * stat)
{
    bool const is_file = (0 != (stat->st_mode & S_IFREG));
    bool const is_dir = (0 != (stat->st_mode & S_IFDIR));

    struct wfp_message_writer * writer = wfp_impl_request_get_writer(request);
    wfp_impl_message_writer_add_int(writer, "inode", stat->st_ino);
    wfp_impl_message_writer_add_int(writer, "mode" , stat->st_mode & 0777);    
    wfp_impl_message_writer_add_int(writer, "atime", stat->st_atime);
    wfp_impl_message_writer_add_int(writer, "mtime", stat->st_mtime);
    wfp_impl_message_writer_add_int(writer, "ctime", stat->st_ctime);

    if (is_file)
    {
        wfp_impl_message_writer_add_string(writer, "type", "file");
        wfp_impl_message_writer_add_int(writer, "size", stat->st_size);
    }

    if (is_dir)
    {
        wfp_impl_message_writer_add_string(writer, "type", "dir");
    }

    wfp_impl_respond(request);
}
