#include "webfuse_provider/impl/credentials.h"
#include "webfuse_provider/impl/json/writer.h"

#include <stdlib.h>
#include <string.h>

#define WFP_IMPL_CREDENTIALS_DEFAULT_SIZE 8

void  wfp_impl_credentials_init(
    struct wfp_credentials * credentials)
{
    credentials->type = NULL;
    credentials->size = 0;
    credentials->capacity = WFP_IMPL_CREDENTIALS_DEFAULT_SIZE;
    credentials->entries = malloc(sizeof(struct wfp_credentials_entry) * credentials->capacity);
}

void wfp_impl_credentials_cleanup(
    struct wfp_credentials * credentials)
{
    for(size_t i = 0; i < credentials->size; i++)
    {
        free(credentials->entries[i].key);
        free(credentials->entries[i].value);
    }
    free(credentials->entries);
    free(credentials->type);
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
    if (credentials->size >= credentials->capacity)
    {
        credentials->capacity *= 2;
        credentials->entries = realloc(credentials->entries, sizeof(struct wfp_credentials_entry) * credentials->capacity);
    }

    credentials->entries[credentials->size].key = strdup(key);
    credentials->entries[credentials->size].value = strdup(value);
    credentials->size++;
}

void
wfp_impl_credentials_write(
    struct wfp_json_writer * writer,
    void * data)
{
    struct wfp_credentials * credentials = (struct wfp_credentials *) data;

    wfp_impl_json_writer_object_begin(writer);
    for(size_t i = 0; i < credentials->size; i++)
    {
        wfp_impl_json_writer_object_write_string(writer, 
            credentials->entries[i].key,
            credentials->entries[i].value);
    }
    wfp_impl_json_writer_object_end(writer);
}

char const * wfp_impl_credentials_get_type(
    struct wfp_credentials * credentials)
{
    return credentials->type;
}

