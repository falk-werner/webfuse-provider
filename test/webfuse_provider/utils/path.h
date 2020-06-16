#ifndef WFP_PATH_H
#define WFP_PATH_H

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

struct wfp_path;

extern struct wfp_path *
wfp_path_create(
    char const * value);

extern void
wfp_path_dispose(
    struct wfp_path * path);

extern size_t
wfp_path_element_count(
    struct wfp_path * path);

extern char const *
wfp_path_get_element(
    struct wfp_path * path,
    size_t i);

extern char const *
wfp_path_get_filename(
    struct wfp_path * path);

#ifdef __cplusplus
}
#endif

#endif
