#ifndef WFP_IMPL_JSON_NODE_INTERN_H
#define WFP_IMPL_JSON_NODE_INTERN_H

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

#ifdef __cplusplus
extern "C"
{
#endif

enum wfp_json_type
{
    WFP_JSON_BOOL,
    WFP_JSON_INT,
    WFP_JSON_STRING,
    WFP_JSON_ARRAY,
    WFP_JSON_OBJECT
};

struct wfp_json_object_item;

struct wfp_json_array
{
    size_t size;
    struct wfp_json * items;
};

struct wfp_json_object
{
    size_t size;
    struct wfp_json_object_item * items;
};

union wfp_json_value
{
    bool b;
    int i;
    char * s;
    struct wfp_json_array a;
    struct wfp_json_object o;
};

struct wfp_json
{
    enum wfp_json_type type;
    union wfp_json_value value;
};


struct wfp_json_object_item
{
    struct wfp_json json;
    char * key;
};

extern void
wfp_impl_json_cleanup(
    struct wfp_json * json);

#ifdef __cplusplus
}
#endif


#endif
