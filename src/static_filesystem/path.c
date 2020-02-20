#include "path.h"
#include <stdlib.h>
#include <string.h>

#define PATH_DEFAULT_CAPACITY (8)

struct path
{
    char * * elements;
    size_t count;
    size_t capacity;
};

static void 
path_add(
    struct path * path,
    char const * element,
    size_t element_size)
{
    if (0 < element_size)
    {
        if (path->count >= path->capacity)
        {
            size_t new_capacity = 2 * path->capacity;
            size_t new_size = sizeof(char*) * new_capacity;

            char * * elements = realloc(path->elements, new_size);
            if (NULL != elements)
            {
                path->elements = elements;
                path->capacity = new_capacity;
            }
        }

        if (path->count < path->capacity)
        {
            path->elements[path->count] = strndup(element, element_size);
            path->count++;
        }
    }
}

struct path *
path_create(
    char const * value)
{
    struct path * path = malloc(sizeof(struct path));
    if (NULL != path)
    {
        path->elements = malloc(sizeof(char*) * PATH_DEFAULT_CAPACITY);
        path->capacity = PATH_DEFAULT_CAPACITY;
        path->count = 0;

        char const * remainder = value;
        char const * pos = strchr(remainder, '/');
        while (NULL != pos)
        {
            path_add(path, remainder, (pos - remainder));
            remainder = pos + 1;
            pos = strchr(remainder, '/');
        }

        path_add(path, remainder, strlen(remainder));
    }

    return path;
}

void
path_dispose(
    struct path * path)
{
    for(size_t i = 0; i < path->count; i++)
    {
        free(path->elements[i]);
    }

    free(path->elements);
    free(path);
    (void) path;
}

size_t
path_element_count(
    struct path * path)
{
    return path->count;
}

char const *
path_get_element(
    struct path * path,
    size_t i)
{  
    char const * result = NULL;
    if (i < path->count)
    {
        result = path->elements[i];
    }

    return result;
}

char const *
path_get_filename(
    struct path * path)
{
    char const * result = NULL;

    if (0 < path->count)
    {
        result = path->elements[path->count - 1];
    }

    return result;
}
