#ifndef WFP_IMPL_CREDENTIALS_H
#define WFP_IMPL_CREDENTIALS_H

#include "webfuse_provider/credentials.h"
#include "webfuse_provider/impl/jsonrpc/proxy_intern.h"

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_json_writer;

struct wfp_credentials_entry
{
    char * key;
    char * value;
};

struct wfp_credentials
{
    char * type;
    struct wfp_credentials_entry * entries;
    size_t size;
    size_t capacity;
};

extern void  wfp_impl_credentials_init(
    struct wfp_credentials * credentials);

extern void wfp_impl_credentials_cleanup(
    struct wfp_credentials * credentials);

extern void wfp_impl_credentials_set_type(
    struct wfp_credentials * credentials,
    char const * type);

extern void wfp_impl_credentials_add(
    struct wfp_credentials * credentials,
    char const * key,
    char const * value);

extern void
wfp_impl_credentials_write(
    struct wfp_json_writer * writer,
    void * data);

extern char const *
wfp_impl_credentials_get_type(
    struct wfp_credentials * credentials);


#ifdef __cplusplus
}
#endif

#endif
