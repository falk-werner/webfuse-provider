#include "webfuse_provider/impl/message_writer.h"
#include "webfuse_provider/impl/message.h"
#include "webfuse_provider/impl/util/base64.h"
#include "webfuse_provider/impl/dirbuffer.h"

#include <jansson.h>
#include <libwebsockets.h>
#include <stdlib.h>

struct wfp_message_writer
{
    json_t * result;
    json_t * error;
    int id;
};

struct wfp_message_writer *
wfp_impl_message_writer_create(int id)
{
    struct wfp_message_writer * writer = malloc(sizeof(writer));
    writer->result = json_object();
    writer->error = NULL;
    writer->id = id;

    return writer;
}

void
wfp_impl_message_writer_dispose(
    struct wfp_message_writer * writer)
{
    json_decref(writer->result);
    json_decref(writer->error);
    free(writer);
}

struct wfp_message *
wfp_impl_message_writer_take_message(
    struct wfp_message_writer * writer)
{
    json_t * response = json_object();

    if (writer->result)
    {
        json_object_set_new(response, "result", writer->result);
    }
    else
    {
        json_object_set_new(response, "error", writer->error);
    }
    json_object_set_new(response, "id", json_integer(writer->id));

    size_t const length = json_dumpb(response, NULL, 0, JSON_COMPACT);

    char * data = malloc(sizeof(struct wfp_message) + LWS_PRE + length);
    struct wfp_message * message = (struct wfp_message *) data;
    message->data = &data[sizeof(struct wfp_message) + LWS_PRE];
    message->length = length;

    json_dumpb(response, message->data, length, JSON_COMPACT);


    json_decref(response);
    return message;
}

void
wfp_impl_message_writer_add_int(
    struct wfp_message_writer * writer,
    char const * key,
    int value)
{
    json_object_set_new(writer->result, key, json_integer(value));
}

void
wfp_impl_message_writer_add_string(
    struct wfp_message_writer * writer,
    char const * key,
    char const * value)
{
    json_object_set_new(writer->result, key, json_string(value));
}

void
wfp_impl_message_writer_add_bytes(
    struct wfp_message_writer * writer,
    char const * key,
    char const * data,
    size_t length)
{
    size_t const size = wfp_impl_base64_encoded_size(length) + 1;
    char * buffer = malloc(size);
    wfp_impl_base64_encode((uint8_t const *) data, length, buffer, size);

    json_object_set_new(writer->result, "data", json_string(buffer));

    free(buffer);
}

void
wfp_impl_message_writer_add_dirbuffer(
    struct wfp_message_writer * writer,
    struct wfp_dirbuffer * dirbuffer)
{
    json_decref(writer->result);
    writer->result = wfp_impl_dirbuffer_take(dirbuffer);
}

void
wfp_impl_message_writer_set_error(
    struct wfp_message_writer * writer,
    int error_code)
{
    json_decref(writer->result);
    writer->result = NULL;

    json_decref(writer->error);
    writer->error = json_object();
    json_object_set_new(writer->error, "code", json_integer(error_code));
}
