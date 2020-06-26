#ifndef STATIC_FILESYSTEM_H
#define STATIC_FILESYSTEM_H

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

struct wfp_client_config;
struct wfp_static_filesystem;

typedef size_t
static_filesystem_read_fn(
    size_t offset,
    char * buffer,
    size_t buffer_size,
    void * user_data);

typedef void
static_filesystem_get_info_fn(
    void * user_data,
    int * result_mode,
    size_t * result_size);

extern struct static_filesystem *
static_filesystem_create(
    struct wfp_client_config * config);

extern void
static_filesystem_dispose(
    struct static_filesystem * filesystem);

extern void
static_filesystem_add(
    struct static_filesystem * filesystem,
    char const * path,
    int mode,
    char const * content,
    size_t length);

extern void
static_filesystem_add_text(
    struct static_filesystem * filesystem,
    char const * path,
    int mode,
    char const * content);

extern void
static_filesystem_add_file(
    struct static_filesystem * filesystem,
    char const * path,
    char const * filename);

extern void
static_filesystem_add_generic(
    struct static_filesystem * filesystem,
    char const * path,
    static_filesystem_read_fn * read,
    static_filesystem_get_info_fn * get_info,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
