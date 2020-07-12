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
    struct wfp_message_writer * writer = malloc(sizeof(struct wfp_message_writer));
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

    size_t length;
    char * data = wfp_impl_json_writer_take_data(writer->json_writer, &length);

    return wfp_message_create(data, length);
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

    size_t const count = wfp_impl_dirbuffer_size(dirbuffer);
    for (size_t i = 0; i < count; i++)
    {
        struct wfp_dirbuffer_entry const * entry = wfp_impl_dirbuffer_entry_at(dirbuffer, i);

        wfp_impl_json_writer_object_begin(writer->json_writer);
        wfp_impl_json_writer_object_key(writer->json_writer, "name");
        wfp_impl_json_writer_write_string(writer->json_writer, entry->name);
        wfp_impl_json_writer_object_key(writer->json_writer, "inode");
        wfp_impl_json_writer_write_int(writer->json_writer, entry->inode);
        wfp_impl_json_writer_object_end(writer->json_writer);

    }

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
