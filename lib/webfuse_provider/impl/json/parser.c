#include "webfuse_provider/impl/json/parser.h"
#include "webfuse_provider/impl/json/node_intern.h"
#include "webfuse_provider/impl/json/reader.h"

#include <stdlib.h>

#define WFP_IMPL_JSON_DEFAULT_CAPACITY 4

struct wfp_json_doc
{
    struct wfp_json root;
};

static bool
wfp_impl_json_parse_value(
    struct wfp_json_reader * reader,
    struct wfp_json * json);

static bool
wfp_impl_json_parse_true(
    struct wfp_json_reader * reader,
    struct wfp_json * json);

static bool
wfp_impl_json_parse_false(
    struct wfp_json_reader * reader,
    struct wfp_json * json);

static void
wfp_impl_json_parse_int(
    struct wfp_json_reader * reader,
    char first,
    struct wfp_json * json);

static bool
wfp_impl_json_parse_string(
    struct wfp_json_reader * reader,
    struct wfp_json * json);

static bool
wfp_impl_json_parse_array(
    struct wfp_json_reader * reader,
    struct wfp_json * json);

static bool
wfp_impl_json_parse_object(
    struct wfp_json_reader * reader,
    struct wfp_json * json);

struct wfp_json_doc *
wfp_impl_json_parse_buffer(
    char * data,
    size_t length)
{
    struct wfp_json_reader reader;
    wfp_impl_json_reader_init(&reader, data, length);

    struct wfp_json_doc * doc = malloc(sizeof(struct wfp_json_doc));
    if (!wfp_impl_json_parse_value(&reader, &doc->root))
    {
        free(doc);
        doc = NULL;
    }

    return doc;
}

void
wfp_impl_json_dispose(
    struct wfp_json_doc * doc)
{
    wfp_impl_json_cleanup(&doc->root);
    free(doc);
}

struct wfp_json const *
wfp_impl_json_root(
    struct wfp_json_doc * doc)
{
    return &(doc->root);
}

static bool
wfp_impl_json_parse_value(
    struct wfp_json_reader * reader,
    struct wfp_json * json)
{
    wfp_impl_json_reader_skip_whitespace(reader);
    char c = wfp_impl_json_reader_read_char(reader);

    switch (c)
    {
        case 't':
            return wfp_impl_json_parse_true(reader, json);
        case 'f':
            return wfp_impl_json_parse_false(reader, json);
        case '\"':
            wfp_impl_json_reader_unget_char(reader);
            return wfp_impl_json_parse_string(reader, json);
        case '{':
            return wfp_impl_json_parse_object(reader, json);
        case '[':
            return wfp_impl_json_parse_array(reader, json);
        default:
            if ((('0' <= c) && (c <= '9')) || ('-' == c))
            {
                wfp_impl_json_parse_int(reader, c, json);
                return true;
            }
            else
            {
                return false;
            }            
    }
}

static bool
wfp_impl_json_parse_true(
    struct wfp_json_reader * reader,
    struct wfp_json * json)
{
    bool const result = wfp_impl_json_reader_read_const(reader, "rue", 3);
    if (result)
    {
        json->type = WFP_JSON_BOOL;
        json->value.b = true;
    }

    return result;
}

static bool
wfp_impl_json_parse_false(
    struct wfp_json_reader * reader,
    struct wfp_json * json)
{
    bool const result = wfp_impl_json_reader_read_const(reader, "alse", 4);
    if (result)
    {
        json->type = WFP_JSON_BOOL;
        json->value.b = false;
    }

    return result;
}

static void
wfp_impl_json_parse_int(
    struct wfp_json_reader * reader,
    char first,
    struct wfp_json * json)
{
    json->type = WFP_JSON_INT;
    json->value.i = wfp_impl_json_reader_read_int(reader, first);
}

static bool
wfp_impl_json_parse_string(
    struct wfp_json_reader * reader,
    struct wfp_json * json)
{
    json->type = WFP_JSON_STRING;
    return wfp_impl_json_reader_read_string(reader, &json->value.s);
}

static bool
wfp_impl_json_parse_array(
    struct wfp_json_reader * reader,
    struct wfp_json * json)
{
    size_t capacity = WFP_IMPL_JSON_DEFAULT_CAPACITY;
    json->type = WFP_JSON_ARRAY;
    json->value.a.size = 0;
    json->value.a.items = malloc(sizeof(struct wfp_json) * capacity);

    wfp_impl_json_reader_skip_whitespace(reader);
    char c = wfp_impl_json_reader_peek(reader);
    if (']' == c)
    {
        wfp_impl_json_reader_read_char(reader);
        return true;
    }

    bool result;
    do
    {
        if (json->value.a.size >= capacity)
        {
            capacity *= 2;
            json->value.a.items = realloc(json->value.a.items, sizeof(struct wfp_json) * capacity);
        }

        result = wfp_impl_json_parse_value(reader, &(json->value.a.items[json->value.a.size]));
        if (result)
        {
            json->value.a.size++;
            wfp_impl_json_reader_skip_whitespace(reader);
            c = wfp_impl_json_reader_read_char(reader);
        }

    } while ((result) && (',' == c));

    if ((result) && (']' != c))
    {
        result = false;
    }

    if (!result)
    {
        wfp_impl_json_cleanup(json);
    }

    return result;
}

static bool
wfp_impl_json_parse_object(
    struct wfp_json_reader * reader,
    struct wfp_json * json)
{
    size_t capacity = WFP_IMPL_JSON_DEFAULT_CAPACITY;
    json->type = WFP_JSON_OBJECT;
    json->value.o.size = 0;
    json->value.o.items = malloc(sizeof(struct wfp_json_object_item) * capacity);

    wfp_impl_json_reader_skip_whitespace(reader);
    char c = wfp_impl_json_reader_peek(reader);
    if ('}' == c)
    {
        wfp_impl_json_reader_read_char(reader);
        return true;
    }

    bool result;
    do
    {
        if (json->value.a.size >= capacity)
        {
            capacity *= 2;
            json->value.a.items = realloc(json->value.a.items, sizeof(struct wfp_json_object_item) * capacity);
        }

        struct wfp_json_object_item * item = &(json->value.o.items[json->value.o.size]);
        result = wfp_impl_json_reader_read_string(reader, &(item->key));
        if (result)
        {
            wfp_impl_json_reader_skip_whitespace(reader);
            result = (':' == wfp_impl_json_reader_read_char(reader));
        }

        if (result)
        {
            result = wfp_impl_json_parse_value(reader, &(item->json));
        }

        if (result)
        {
            json->value.o.size++;
            wfp_impl_json_reader_skip_whitespace(reader);
            c = wfp_impl_json_reader_read_char(reader);
        }
    } while ((result) && (',' == c));

    if ((result) && ('}' != c))
    {
        result = false;
    }

    if (!result)
    {
        wfp_impl_json_cleanup(json);
    }

    return result;
}
