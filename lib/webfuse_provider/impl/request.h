#ifndef WFP_IMPL_REQUEST_H
#define WFP_IMPL_REQUEST_H

#include <jansson.h>
#include "webfuse_provider/impl/provider.h"
#include "webfuse_provider/status.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_message;
struct wfp_message_writer;

typedef void wfp_impl_request_respond_fn(
    struct wfp_message * response,
    void * user_data);

struct wfp_request
{
    wfp_impl_request_respond_fn * respond;
    void * user_data;
    int id;
    struct wfp_message_writer * writer;
};

extern struct wfp_message_writer *
wfp_impl_request_get_writer(
    struct wfp_request * request);

extern void wfp_impl_respond_error(
    struct wfp_request * request,
    wfp_status status);

extern struct wfp_request * wfp_impl_request_create(
    struct wfp_request * prototype,
    int id);
            
extern void wfp_impl_request_dispose(
    struct wfp_request * request);

extern void wfp_impl_respond(
    struct wfp_request * request);

#ifdef __cplusplus    
}
#endif

#endif
