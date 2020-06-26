#ifndef PATH_H
#define PATH_H

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
using ::std::size_t;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct path;

extern struct path *
path_create(
    char const * value);

extern void
path_dispose(
    struct path * path);

extern size_t
path_element_count(
    struct path * path);

extern char const *
path_get_element(
    struct path * path,
    size_t i);

extern char const *
path_get_filename(
    struct path * path);

#ifdef __cplusplus
}
#endif

#endif
