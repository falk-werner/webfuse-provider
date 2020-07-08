#include "webfuse_provider/impl/operation/lookup.h"

#include <stdbool.h>

#include "webfuse_provider/impl/operation/error.h"
#include "webfuse_provider/impl/request.h"
#include "webfuse_provider/impl/message_writer.h"
#include "webfuse_provider/impl/util/util.h"

void wfp_impl_lookup(
    struct wfp_impl_invokation_context * context,
    json_t * params,
    int id)
{
    size_t const count = json_array_size(params);
    if (3 == count)
    {
        json_t * inode_holder = json_array_get(params, 1);
        json_t * name_holder = json_array_get(params, 2);

        if (json_is_integer(inode_holder) &&
            json_is_string(name_holder))
        {
            ino_t inode = json_integer_value(inode_holder);
            char const * name = json_string_value(name_holder);

            struct wfp_request * request = wfp_impl_request_create(context->request, id);
            context->provider->lookup(request, inode, name, context->user_data);
        }
    }
}

void wfp_impl_respond_lookup(
    struct wfp_request * request,
    struct stat const * stat)
{
    bool const is_file = (0 != (stat->st_mode & S_IFREG));
    bool const is_dir = (0 != (stat->st_mode & S_IFDIR));

    struct wfp_message_writer * writer = wfp_impl_request_get_writer(request);
    wfp_impl_message_writer_add_int(writer, "inode", stat->st_ino);
    wfp_impl_message_writer_add_int(writer, "mode", stat->st_mode & 0777);    
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

void wfp_impl_lookup_default(
    struct wfp_request * request,
    ino_t WFP_UNUSED_PARAM(parent),
    char const * WFP_UNUSED_PARAM(name),
    void * WFP_UNUSED_PARAM(user_data))
{
    wfp_impl_respond_error(request, WFP_BAD_NOENTRY);
}

