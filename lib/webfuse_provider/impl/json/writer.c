#include "webfuse_provider/impl/json/writer.h"
#include "webfuse_provider/impl/util/base64.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define WFP_JSON_WRITER_MAX_LEVEL 7
#define WFP_INT_BUFFER_SIZE 64

enum wfp_json_writer_state
{
    WFP_JSON_WRITER_STATE_INIT,
    WFP_JSON_WRITER_STATE_ARRAY_FIRST,
    WFP_JSON_WRITER_STATE_ARRAY_NEXT,
    WFP_JSON_WRITER_STATE_OBJECT_FIRST,
    WFP_JSON_WRITER_STATE_OBJECT_NEXT
};

struct wfp_json_writer
{
    enum wfp_json_writer_state state[WFP_JSON_WRITER_MAX_LEVEL + 1];
    size_t level;
    size_t capacity;
    size_t pre;
    size_t offset;
    char * data;
    char * raw_data;
};

static char
wfp_impl_json_writer_get_esc(
    char c)
{
    switch (c)
    {
        case '\\': return '\\';
        case '\"': return '\"';
        case '/' : return '/';
        case '\b': return 'b';
        case '\f': return 'f';
        case '\n': return 'n';
        case '\r': return 'r';
        case '\t': return 't';
        default:
            // error
            return '/';
    }
}

static void
wfp_impl_json_writer_write_raw_char(
    struct wfp_json_writer * writer,
    char value)
{
    writer->data[writer->offset++] = value;
}

static void
wfp_impl_json_writer_reserve(
    struct wfp_json_writer * writer,
    size_t needed)
{
    if ((writer->capacity - writer->offset) < needed)
    {
        size_t new_capacity = 2 * writer->capacity;

        while ((new_capacity - writer->offset) < needed)
        {
            new_capacity *= 2;
        }

        writer->raw_data = realloc(writer->raw_data, writer->pre + new_capacity);
        writer->data = &(writer->raw_data[writer->pre]);
        writer->capacity = new_capacity;
    }
}

static void
wfp_impl_json_writer_begin_value(
    struct wfp_json_writer * writer)
{
    wfp_impl_json_writer_reserve(writer, 1);
    if (writer->state[writer->level] == WFP_JSON_WRITER_STATE_ARRAY_NEXT)
    {
        wfp_impl_json_writer_write_raw_char(writer, ',');
    }    
}

static void
wfp_impl_json_writer_end_value(
    struct wfp_json_writer * writer)
{
    if (WFP_JSON_WRITER_STATE_ARRAY_FIRST == writer->state[writer->level])
    {
        writer->state[writer->level] = WFP_JSON_WRITER_STATE_ARRAY_NEXT;
    }
}

static void
wfp_impl_json_writer_push(
    struct wfp_json_writer * writer,
    enum wfp_json_writer_state state)
{
    if (writer->level < WFP_JSON_WRITER_MAX_LEVEL)
    {
        writer->level++;
        writer->state[writer->level] = state;
    }
}

static void
wfp_impl_json_writer_pop(
    struct wfp_json_writer * writer)
{
    if (writer->level > 0)
    {
        writer->level--;
    }
}

void
wfp_impl_json_writer_write_raw(
    struct wfp_json_writer * writer,
    char const * value,
    size_t length)
{
    memcpy(&(writer->data[writer->offset]), value, length);
    writer->offset += length;
}

struct wfp_json_writer *
wfp_impl_json_writer_create(
    size_t initial_capacity,
    size_t pre)
{
    struct wfp_json_writer * writer = malloc(sizeof(struct wfp_json_writer));
    writer->level = 0;
    writer->state[writer->level] = WFP_JSON_WRITER_STATE_INIT;
    writer->pre = pre;
    writer->capacity = initial_capacity;
    writer->offset = 0;
    writer->raw_data = malloc(writer->pre + writer->capacity);
    writer->data = &(writer->raw_data[pre]);

    return writer; 
}

void
wfp_impl_json_writer_dispose(
    struct wfp_json_writer * writer)
{
    free(writer->raw_data);
    free(writer);
}

void
wfp_impl_json_writer_reset(
    struct wfp_json_writer * writer)
{
    writer->level = 0;
    writer->state[writer->level] = WFP_JSON_WRITER_STATE_INIT;
    writer->offset = 0;
}


char *
wfp_impl_json_writer_take_data(
    struct wfp_json_writer * writer,
    size_t * size)
{
    wfp_impl_json_writer_reserve(writer, 1);    
    writer->data[writer->offset] = '\0';

    char * result = writer->raw_data;
    writer->raw_data = NULL;

    if (NULL != size)
    {
        *size = writer->offset;
    }

    return result;
}

void
wfp_impl_json_writer_write_int(
    struct wfp_json_writer * writer,
    int value)
{
    wfp_impl_json_writer_begin_value(writer);
    wfp_impl_json_writer_reserve(writer, WFP_INT_BUFFER_SIZE);

    bool const is_signed = (0 > value);
    char buffer[WFP_INT_BUFFER_SIZE];
    size_t offset = WFP_INT_BUFFER_SIZE;
    buffer[--offset] = '\0';
    if (is_signed) 
    {
        if (INT_MIN == value)
        {
            char const actual = (char) abs(value % 10);
            buffer[--offset] = (char) ('0' + actual);
            value /= 10;
        }
        value = -value;
    }

    do
    {
        char const actual = (char) (value % 10);
        buffer[--offset] = ('0' + actual);
        value /= 10;
    }
    while (0 != value);

    if (is_signed)
    {
        buffer[--offset] = '-';
    }

    size_t const length = (WFP_INT_BUFFER_SIZE - offset - 1);
    wfp_impl_json_writer_write_raw(writer, &(buffer[offset]), length);

    wfp_impl_json_writer_end_value(writer);
}

void
wfp_impl_json_writer_write_string(
    struct wfp_json_writer * writer,
    char const * value)
{
    wfp_impl_json_writer_begin_value(writer);

    size_t length = strlen(value);
    wfp_impl_json_writer_reserve(writer, length + 2);

    wfp_impl_json_writer_write_raw_char(writer, '\"');
    for(size_t i = 0; i < length; i++)
    {
        char c = value[i];
        if ((' ' <= c) && (c != '\\') && (c != '\"'))
        {
            wfp_impl_json_writer_write_raw_char(writer, c);
        }
        else
        {
            char esc = wfp_impl_json_writer_get_esc(c);

            wfp_impl_json_writer_reserve(writer, (length - i) + 2);
            wfp_impl_json_writer_write_raw_char(writer, '\\');
            wfp_impl_json_writer_write_raw_char(writer, esc);
        }
        
    }
    wfp_impl_json_writer_write_raw_char(writer, '\"');

    wfp_impl_json_writer_end_value(writer);
}

void
wfp_impl_json_writer_write_string_nocheck(
    struct wfp_json_writer * writer,
    char const * value)
{
    wfp_impl_json_writer_begin_value(writer);

    size_t length = strlen(value);
    wfp_impl_json_writer_reserve(writer, length + 2);

    wfp_impl_json_writer_write_raw_char(writer, '\"');
    wfp_impl_json_writer_write_raw(writer, value, length);
    wfp_impl_json_writer_write_raw_char(writer, '\"');

    wfp_impl_json_writer_end_value(writer);
}

void
wfp_impl_json_writer_write_bytes(
    struct wfp_json_writer * writer,
    char const * data,
    size_t length)
{
    wfp_impl_json_writer_begin_value(writer);

    size_t encoded_length = wfp_impl_base64_encoded_size(length);
    wfp_impl_json_writer_reserve(writer, encoded_length + 2);

    wfp_impl_json_writer_write_raw_char(writer, '\"');
    wfp_impl_base64_encode((uint8_t const*) data, length, &(writer->data[writer->offset]), encoded_length);
    writer->offset += encoded_length;
    wfp_impl_json_writer_write_raw_char(writer, '\"');

    wfp_impl_json_writer_end_value(writer);
}

void
wfp_impl_json_writer_object_begin(
    struct wfp_json_writer * writer)
{
    wfp_impl_json_writer_begin_value(writer);
    wfp_impl_json_writer_reserve(writer, 1);

    wfp_impl_json_writer_push(writer, WFP_JSON_WRITER_STATE_OBJECT_FIRST);
    wfp_impl_json_writer_write_raw_char(writer, '{');
}

void
wfp_impl_json_writer_object_end(
    struct wfp_json_writer * writer)
{
    wfp_impl_json_writer_reserve(writer, 1);
    wfp_impl_json_writer_write_raw_char(writer, '}');

    wfp_impl_json_writer_pop(writer);
    wfp_impl_json_writer_end_value(writer);
}

void
wfp_impl_json_writer_object_key(
    struct wfp_json_writer * writer,
    char const * key)
{
    wfp_impl_json_writer_reserve(writer, 1);

    size_t length = strlen(key);
    wfp_impl_json_writer_reserve(writer, length + 4);

    if (WFP_JSON_WRITER_STATE_OBJECT_NEXT == writer->state[writer->level])
    {
        wfp_impl_json_writer_write_raw_char(writer, ',');
    }
    else
    {
        writer->state[writer->level] = WFP_JSON_WRITER_STATE_OBJECT_NEXT;
    }
    
    wfp_impl_json_writer_write_raw_char(writer, '\"');
    wfp_impl_json_writer_write_raw(writer, key, length);
    wfp_impl_json_writer_write_raw_char(writer, '\"');
    wfp_impl_json_writer_write_raw_char(writer, ':');
}

void
wfp_impl_json_writer_array_begin(
    struct wfp_json_writer * writer)
{
    wfp_impl_json_writer_begin_value(writer);
    wfp_impl_json_writer_push(writer, WFP_JSON_WRITER_STATE_ARRAY_FIRST);

    wfp_impl_json_writer_reserve(writer, 1);
    wfp_impl_json_writer_write_raw_char(writer, '[');
}

void
wfp_impl_json_writer_array_end(
    struct wfp_json_writer * writer)
{
    wfp_impl_json_writer_reserve(writer, 1);
    wfp_impl_json_writer_write_raw_char(writer, ']');

    wfp_impl_json_writer_pop(writer);
    wfp_impl_json_writer_end_value(writer);
}