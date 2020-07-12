#ifndef WFP_IMPL_JSON_READER_H
#define WFP_IMPL_JSON_READER_H

#ifndef __cplusplus
#include <stddef.h>
#include <stdbool.h>
#else
#include <cstddef>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_json_reader
{
    char * contents;
    size_t length;
    size_t pos;
};

extern void
wfp_impl_json_reader_init(
    struct wfp_json_reader * reader,
    char * contents,
    size_t length);

extern void
wfp_impl_json_reader_skip_whitespace(
    struct wfp_json_reader * reader);

extern char
wfp_impl_json_reader_peek(
    struct wfp_json_reader * reader);

extern char
wfp_impl_json_reader_read_char(
    struct wfp_json_reader * reader);

extern void
wfp_impl_json_reader_unget_char(
    struct wfp_json_reader * reader);

extern bool
wfp_impl_json_reader_read_const(
    struct wfp_json_reader * reader,
    char const * value,
    size_t length);

extern int
wfp_impl_json_reader_read_int(
    struct wfp_json_reader * reader,
    char first);

extern bool
wfp_impl_json_reader_read_string(
    struct wfp_json_reader * reader,
    char * * value);

#ifdef __cplusplus
}
#endif

#endif
