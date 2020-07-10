#ifndef WFP_IMPL_JSON_WRITER_H
#define WFP_IMPL_JSON_WRTIER_H

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_json_writer;

extern struct wfp_json_writer *
wfp_impl_json_writer_create(
    size_t initial_capacity,
    size_t pre);

extern void
wfp_impl_json_writer_dispose(
    struct wfp_json_writer * writer);

extern void
wfp_impl_json_writer_reset(
    struct wfp_json_writer * writer);

extern char *
wfp_impl_json_writer_take_data(
    struct wfp_json_writer * writer,
    size_t * size);

extern void
wfp_impl_json_writer_write_int(
    struct wfp_json_writer * writer,
    int value);

extern void
wfp_impl_json_writer_write_string(
    struct wfp_json_writer * writer,
    char const * value);

extern void
wfp_impl_json_writer_write_string_nocheck(
    struct wfp_json_writer * writer,
    char const * value);

extern void
wfp_impl_json_writer_write_bytes(
    struct wfp_json_writer * writer,
    char const * data,
    size_t length);

extern void
wfp_impl_json_writer_object_begin(
    struct wfp_json_writer * writer);

extern void
wfp_impl_json_writer_object_end(
    struct wfp_json_writer * writer);

extern void
wfp_impl_json_writer_object_key(
    struct wfp_json_writer * writer,
    char const * key);

extern void
wfp_impl_json_writer_object_write_int(
    struct wfp_json_writer * writer,
    char const * key,
    int value);

extern void
wfp_impl_json_writer_object_write_string(
    struct wfp_json_writer * writer,
    char const * key,
    char const * value);

extern void
wfp_impl_json_writer_object_begin_object(
    struct wfp_json_writer * writer,
    char const * key);

extern void
wfp_impl_json_writer_object_begin_array(
    struct wfp_json_writer * writer,
    char const * key);

extern void
wfp_impl_json_writer_array_begin(
    struct wfp_json_writer * writer);

extern void
wfp_impl_json_writer_array_end(
    struct wfp_json_writer * writer);


#ifdef __cplusplus
}
#endif

#endif
