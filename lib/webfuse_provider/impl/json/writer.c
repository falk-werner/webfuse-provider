#include "webfuse_provider/impl/json/writer.h"
#include <stdlib.h>
#include <string.h>

#define WFP_JSON_WRITER_MAX_LEVEL 8

enum wfp_json_writer_state
{
    WFP_JSON_WRITER_STATE_INIT
};

struct wfp_json_writer
{
    enum wfp_json_writer_state state[WFP_JSON_WRITER_MAX_LEVEL];
    size_t level;
    size_t capacity;
    size_t pre;
    size_t offset;
    char * data;
    char * raw_data;
};

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
        writer->data = &(writer->data[writer->pre]);
        writer->capacity = new_capacity;
    }
}

void
wfp_impl_json_writer_write_raw_char(
    struct wfp_json_writer * writer,
    char value)
{
    writer->data[writer->offset] = value;
    writer->offset++;
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

char *
wfp_impl_json_writer_take_data(
    struct wfp_json_writer * writer)
{
    wfp_impl_json_writer_reserve(writer, 1);    
    writer->data[writer->offset] = '\0';

    char * result = writer->raw_data;
    writer->raw_data = NULL;

    return result;
}

void
wfp_impl_json_writer_write_int(
    struct wfp_json_writer * writer,
    int value)
{

}

void
wfp_impl_json_writer_write_string(
    struct wfp_json_writer * writer,
    char const * value)
{
    size_t length = strlen(value);
    wfp_impl_json_writer_reserve(writer, length + 2);

    wfp_impl_json_writer_write_raw_char(writer, '\"');
    wfp_impl_json_writer_write_raw(writer, value, length);
    wfp_impl_json_writer_write_raw_char(writer, '\"');

}

void
wfp_impl_json_writer_write_bytes(
    struct wfp_json_writer * writer,
    char const * data,
    size_t length)
{

}

void
wfp_impl_json_writer_object_begin(
    struct wfp_json_writer * writer)
{
    wfp_impl_json_writer_reserve(writer, 1);
    wfp_impl_json_writer_write_raw_char(writer, '{');
}

void
wfp_impl_json_writer_object_end(
    struct wfp_json_writer * writer)
{
    wfp_impl_json_writer_reserve(writer, 1);
    wfp_impl_json_writer_write_raw_char(writer, '}');
}

void
wfp_impl_json_writer_object_key(
    struct wfp_json_writer * writer,
    char const * key)
{
    wfp_impl_json_writer_reserve(writer, 1);
    wfp_impl_json_writer_write_raw_char(writer, '[');
}

void
wfp_impl_json_writer_array_begin(
    struct wfp_json_writer * writer)
{
    wfp_impl_json_writer_reserve(writer, 1);
    wfp_impl_json_writer_write_raw_char(writer, '[');
}

void
wfp_impl_json_writer_array_end(
    struct wfp_json_writer * writer)
{
    wfp_impl_json_writer_reserve(writer, 1);
    wfp_impl_json_writer_write_raw_char(writer, ']');
}