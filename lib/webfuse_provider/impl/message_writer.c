#include "webfuse_provider/impl/message_writer.h"
#include "webfuse_provider/impl/message.h"
#include "webfuse_provider/impl/util/base64.h"
#include "webfuse_provider/impl/dirbuffer.h"
#include "webfuse_provider/impl/json/writer.h"

#include <libwebsockets.h>
#include <stdlib.h>
#include <stdbool.h>

struct wfp_message_writer
{
    struct wfp_json_writer * json_writer;
    int id;
    bool is_finished;
};

struct wfp_message_writer *
wfp_impl_message_writer_create(int id)
{
    struct wfp_message_writer * writer = malloc(sizeof(writer));
    writer->json_writer = wfp_impl_json_writer_create(1024, LWS_PRE);
    writer->id = id;
    writer->is_finished = false;

    wfp_impl_json_writer_object_begin(writer->json_writer);
    wfp_impl_json_writer_object_key(writer->json_writer, "result");
    wfp_impl_json_writer_object_begin(writer->json_writer);

    return writer;
}

void
wfp_impl_message_writer_dispose(
    struct wfp_message_writer * writer)
{
    wfp_impl_json_writer_dispose(writer->json_writer);
    free(writer);
}

struct wfp_message *
wfp_impl_message_writer_take_message(
    struct wfp_message_writer * writer)
{
    if (!writer->is_finished)
    {
        wfp_impl_json_writer_object_end(writer->json_writer);
        wfp_impl_json_writer_object_key(writer->json_writer, "id");
        wfp_impl_json_writer_write_int(writer->json_writer, writer->id);
        wfp_impl_json_writer_object_end(writer->json_writer);
        writer->is_finished = true;
    }

    struct wfp_message * message = malloc(sizeof(struct wfp_message));
    message->raw_data = wfp_impl_json_writer_take_data(writer->json_writer, &message->length);
    message->data = &(message->raw_data[LWS_PRE]);

    return message;
}

void
wfp_impl_message_writer_add_int(
    struct wfp_message_writer * writer,
    char const * key,
    int value)
{
    wfp_impl_json_writer_object_key(writer->json_writer, key);
    wfp_impl_json_writer_write_int(writer->json_writer, value);
}

void
wfp_impl_message_writer_add_string(
    struct wfp_message_writer * writer,
    char const * key,
    char const * value)
{
    wfp_impl_json_writer_object_key(writer->json_writer, key);
    wfp_impl_json_writer_write_string(writer->json_writer, value);
}

void
wfp_impl_message_writer_add_bytes(
    struct wfp_message_writer * writer,
    char const * key,
    char const * data,
    size_t length)
{
    wfp_impl_json_writer_object_key(writer->json_writer, key);
    wfp_impl_json_writer_write_bytes(writer->json_writer, data, length);
}

void
wfp_impl_message_writer_add_dirbuffer(
    struct wfp_message_writer * writer,
    struct wfp_dirbuffer * dirbuffer)
{
    wfp_impl_json_writer_reset(writer->json_writer);

    wfp_impl_json_writer_object_begin(writer->json_writer);
    wfp_impl_json_writer_object_key(writer->json_writer, "result");
    wfp_impl_json_writer_array_begin(writer->json_writer);

    json_t * entries = wfp_impl_dirbuffer_take(dirbuffer);
    for (size_t i = 0; i < json_array_size(entries); i++)
    {
        json_t * entry = json_array_get(entries, i);
        char const * name = json_string_value(json_object_get(entry, "name"));
        int inode = json_integer_value(json_object_get(entry, "inode"));

        wfp_impl_json_writer_object_begin(writer->json_writer);
        wfp_impl_json_writer_object_key(writer->json_writer, "name");
        wfp_impl_json_writer_write_string(writer->json_writer, name);
        wfp_impl_json_writer_object_key(writer->json_writer, "inode");
        wfp_impl_json_writer_write_int(writer->json_writer, inode);
        wfp_impl_json_writer_object_end(writer->json_writer);

    }
    json_decref(entries);

    wfp_impl_json_writer_array_end(writer->json_writer);

    wfp_impl_json_writer_object_key(writer->json_writer, "id");
    wfp_impl_json_writer_write_int(writer->json_writer, writer->id);
    wfp_impl_json_writer_object_end(writer->json_writer);

    writer->is_finished = true;
}

void
wfp_impl_message_writer_set_error(
    struct wfp_message_writer * writer,
    int error_code)
{
    wfp_impl_json_writer_reset(writer->json_writer);

    wfp_impl_json_writer_object_begin(writer->json_writer);
    wfp_impl_json_writer_object_key(writer->json_writer, "error");
    wfp_impl_json_writer_object_begin(writer->json_writer);
    wfp_impl_json_writer_object_key(writer->json_writer, "code");
    wfp_impl_json_writer_write_int(writer->json_writer, error_code);
    wfp_impl_json_writer_object_end(writer->json_writer);
    wfp_impl_json_writer_object_key(writer->json_writer, "id");
    wfp_impl_json_writer_write_int(writer->json_writer, writer->id);
    wfp_impl_json_writer_object_end(writer->json_writer);

    writer->is_finished = true;
}
