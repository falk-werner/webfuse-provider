#include "webfuse_provider/impl/dirbuffer.h"
#include <stdlib.h>
#include <string.h>

#define WFP_IMPL_DIRBUFFER_DEFAULT_CAPACITY 8

struct wfp_dirbuffer * wfp_impl_dirbuffer_create(void)
{
    struct wfp_dirbuffer * buffer = malloc(sizeof(struct wfp_dirbuffer));
    buffer->size = 0;
    buffer->capacity = WFP_IMPL_DIRBUFFER_DEFAULT_CAPACITY;
    buffer->entries = malloc(sizeof(struct wfp_dirbuffer_entry) * buffer->capacity);

    return buffer;
}

void wfp_impl_dirbuffer_dispose(
    struct wfp_dirbuffer * buffer)
{
    for (size_t i = 0; i < buffer->size; i++)
    {
        free(buffer->entries[i].name);
    }

    free(buffer->entries);
    free(buffer);
}

void wfp_impl_dirbuffer_add(
    struct wfp_dirbuffer * buffer,
    char const * name,
    ino_t inode)
{
    if (buffer->size >= buffer->capacity)
    {
        buffer->capacity *= 2;
        buffer->entries = realloc(buffer->entries, (sizeof(struct wfp_dirbuffer_entry) * buffer->capacity));
    }

    buffer->entries[buffer->size].name = strdup(name);
    buffer->entries[buffer->size].inode = inode;
    buffer->size++;
}

size_t
wfp_impl_dirbuffer_size(
    struct wfp_dirbuffer * buffer)
{
    return buffer->size;
}

struct wfp_dirbuffer_entry const *
wfp_impl_dirbuffer_entry_at(
    struct wfp_dirbuffer * buffer,
    size_t pos)
{
    struct wfp_dirbuffer_entry const * entry = NULL;

    if (pos < buffer->size)
    {
        entry = &(buffer->entries[pos]);
    }

    return entry;
}
