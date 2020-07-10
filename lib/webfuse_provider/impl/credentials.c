#include "webfuse_provider/impl/credentials.h"
#include "webfuse_provider/impl/json/writer.h"

#include <stdlib.h>
#include <string.h>

void  wfp_impl_credentials_init(
    struct wfp_credentials * credentials)
{
    credentials->type = NULL;
    credentials->contents = json_object();
}

void wfp_impl_credentials_cleanup(
    struct wfp_credentials * credentials)
{
    free(credentials->type);
    json_decref(credentials->contents);
}

void wfp_impl_credentials_set_type(
    struct wfp_credentials * credentials,
    char const * type)
{
    free(credentials->type);
    credentials->type = strdup(type);
}

void wfp_impl_credentials_add(
    struct wfp_credentials * credentials,
    char const * key,
    char const * value)
{
    json_object_set_new(credentials->contents, key, json_string(value));
}

void
wfp_impl_credentials_write(
    struct wfp_json_writer * writer,
    void * data)
{
    struct wfp_credentials * credentials = (struct wfp_credentials *) data;

    wfp_impl_json_writer_object_begin(writer);
    char const * key;
    json_t * value;
    json_t * contents = credentials->contents;
    json_object_foreach(contents, key, value)
    {
        wfp_impl_json_writer_object_key(writer, key);
        wfp_impl_json_writer_write_string(writer, json_string_value(value));
    }

    wfp_impl_json_writer_object_end(writer);
}

char const * wfp_impl_credentials_get_type(
    struct wfp_credentials * credentials)
{
    return credentials->type;
}

