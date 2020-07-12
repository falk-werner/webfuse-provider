#include "webfuse_provider/impl/request.h"
#include "webfuse_provider/impl/operation/error.h"
#include "webfuse_provider/impl/message_writer.h"

#include <stdlib.h>

struct wfp_request * wfp_impl_request_create(
    struct wfp_request * prototype,
    int id)
{
    struct wfp_request * request = malloc(sizeof(struct wfp_request));
    request->respond = prototype->respond;
    request->user_data = prototype->user_data;
    request->writer = wfp_impl_message_writer_create(id);

    return request;
}

void wfp_impl_request_dispose(
    struct wfp_request * request)
{
    wfp_impl_message_writer_dispose(request->writer);
    free(request);
}

struct wfp_message_writer *
wfp_impl_request_get_writer(
    struct wfp_request * request)
{
    return request->writer;
}


extern void wfp_impl_respond(
    struct wfp_request * request)
{
    struct wfp_message * response = wfp_impl_message_writer_take_message(request->writer);
    request->respond(response, request->user_data);

    wfp_impl_request_dispose(request);
}

void wfp_impl_respond_error(
    struct wfp_request * request,
    wfp_status status)
{
    wfp_impl_message_writer_set_error(request->writer, status);
    wfp_impl_respond(request);
}