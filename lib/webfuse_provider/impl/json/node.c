#include "webfuse_provider/impl/json/node.h"
#include "webfuse_provider/impl/json/node_intern.h"

#include <stdlib.h>
#include <string.h>

static struct wfp_json const wfp_json_null = 
{
    .type = WFP_JSON_NULL,
    .value = { .b = false }
};

bool
wfp_impl_json_is_null(
    struct wfp_json const * json)
{
    return (WFP_JSON_NULL == json->type);
}

bool
wfp_impl_json_is_bool(
    struct wfp_json const * json)
{
    return (WFP_JSON_BOOL == json->type);
}

bool
wfp_impl_json_is_int(
    struct wfp_json const * json)
{
    return (WFP_JSON_INT == json->type);
}

bool
wfp_impl_json_is_string(
    struct wfp_json const * json)
{
    return (WFP_JSON_STRING == json->type);
}

bool
wfp_impl_json_is_array(
    struct wfp_json const * json)
{
    return (WFP_JSON_ARRAY == json->type);
}

bool
wfp_impl_json_is_object(
    struct wfp_json const * json)
{
    return (WFP_JSON_OBJECT == json->type);
}

bool
wfp_impl_json_bool_get(
    struct wfp_json const * json)
{
    return (WFP_JSON_BOOL == json->type) ? json->value.b : false;
}

int
wfp_impl_json_int_get(
    struct wfp_json const * json)
{
    return (WFP_JSON_INT == json->type) ? json->value.i : 0;
}

const char *
wfp_impl_json_string_get(
    struct wfp_json const * json)
{
    return (WFP_JSON_STRING == json->type) ? json->value.s : "";
}

struct wfp_json const *
wfp_impl_json_array_get(
    struct wfp_json const * json,
    size_t pos)
{
    if ((WFP_JSON_ARRAY == json->type) && (pos < json->value.a.size))
    {
        return &(json->value.a.items[pos]);
    }
    
    return &wfp_json_null;
}

size_t
wfp_impl_json_array_size(
    struct wfp_json const * json)
{
    return (WFP_JSON_ARRAY == json->type) ? json->value.a.size : 0;
}

size_t
wfp_impl_json_object_size(
    struct wfp_json const * json)
{
    return (WFP_JSON_OBJECT == json->type) ? json->value.o.size : 0;
}

struct wfp_json const *
wfp_impl_json_object_get(
    struct wfp_json const * json,
    char const * key)
{
    size_t const count = wfp_impl_json_object_size(json);
    for (size_t i = 0; i < count; i++)
    {
        if (0 == strcmp(key, json->value.o.items[i].key))
        {
            return &(json->value.o.items[i].json);
        }
    }

    return &wfp_json_null;
}

char const *
wfp_impl_json_object_key(
    struct wfp_json const * json,
    size_t pos)
{
    size_t const count = wfp_impl_json_object_size(json);
    if (pos < count) 
    {
        return json->value.o.items[pos].key;
    }

    return "";
}

struct wfp_json const *
wfp_impl_json_object_value(
    struct wfp_json const * json,
    size_t pos)
{
    size_t const count = wfp_impl_json_object_size(json);
    if (pos < count) 
    {
        return &(json->value.o.items[pos].json);
    }

    return &wfp_json_null;
}

void
wfp_impl_json_cleanup(
    struct wfp_json * json)
{
    switch (json->type)
    {
        case WFP_JSON_ARRAY:
            {
                size_t const count = json->value.a.size;
                for (size_t i = 0; i < count; i++)
                {
                    wfp_impl_json_cleanup(&(json->value.a.items[i]));
                }

                free(json->value.a.items);
            }
            break;
        case WFP_JSON_OBJECT:
            {
                size_t const count = json->value.o.size;
                for (size_t i = 0; i < count; i++)
                {
                    wfp_impl_json_cleanup(&(json->value.o.items[i].json));
                }

                free(json->value.o.items);
            }
            break;
        default:
            break;
    }
}
