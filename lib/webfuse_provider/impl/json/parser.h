#ifndef WFP_IMPL_JSON_PARSER_H
#define WFP_IMPL_JSON_PARSER_H

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_json;
struct wfp_json_doc;

extern struct wfp_json_doc *
wfp_impl_json_parse(
    char * data);

extern struct wfp_json_doc *
wfp_impl_json_parse_buffer(
    char * data,
    size_t length);

extern void
wfp_impl_json_dispose(
    struct wfp_json_doc * doc);

extern struct wfp_json const *
wfp_impl_json_root(
    struct wfp_json_doc * doc);

#ifdef __cplusplus
}
#endif

#endif
