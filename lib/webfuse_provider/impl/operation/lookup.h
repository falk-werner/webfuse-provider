#ifndef WFP_IMPL_OPERATION_LOOKUP_H
#define WFP_IMPL_OPERATION_LOOKUP_H

#include "webfuse_provider/impl/provider.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wfp_impl_respond_lookup(
    struct wfp_request * request,
    struct stat const * stat);

extern void wfp_impl_lookup(
    struct wfp_impl_invokation_context * context,
    struct wfp_json const * params,
    int id);

extern void wfp_impl_lookup_default(
    struct wfp_request * request,
    ino_t parent,
    char const * name,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif