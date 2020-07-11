#ifndef WFP_IMPL_JSON_NODE_H
#define WFP_IMPL_JSON_NODE_H

#ifndef __cplusplus
#include <stddef.h>
#include <stdbool.h>
#else
#include <cstddef>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_json;

extern bool
wfp_impl_json_is_bool(
    struct wfp_json const * json);

extern bool
wfp_impl_json_is_int(
    struct wfp_json const * json);

extern bool
wfp_impl_json_is_string(
    struct wfp_json const * json);

extern bool
wfp_impl_json_is_array(
    struct wfp_json const * json);

extern bool
wfp_impl_json_is_object(
    struct wfp_json const * json);

extern bool
wfp_impl_json_get_bool(
    struct wfp_json const * json);

extern int
wfp_impl_json_get_int(
    struct wfp_json const * json);

extern const char *
wfp_impl_json_get_string(
    struct wfp_json const * json);

extern struct wfp_json const *
wfp_impl_json_array_get(
    struct wfp_json const * json,
    size_t pos);

extern size_t
wfp_impl_json_array_size(
    struct wfp_json const * json);

extern size_t
wfp_impl_json_object_size(
    struct wfp_json const * json);

extern struct wfp_json const *
wfp_impl_json_object_get(
    struct wfp_json const * json,
    char const * key);

extern char const *
wfp_impl_json_object_key(
    struct wfp_json const * json,
    size_t pos);

extern struct wfp_json const *
wfp_impl_json_object_value(
    struct wfp_json const * json,
    size_t pos);

#ifdef __cplusplus
}
#endif


#endif
