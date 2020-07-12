#ifndef WFP_IMPL_MESSAGE_WRITER_H
#define WFP_IMPL_MESSAGE_WRITER_H

#ifndef _cplusplus
#include <stddef.h>
#else
#include <cstddef>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_message_writer;
struct wfp_message;
struct wfp_dirbuffer;

extern struct wfp_message_writer *
wfp_impl_message_writer_create(int id);

extern void
wfp_impl_message_writer_dispose(
    struct wfp_message_writer * writer);

extern struct wfp_message *
wfp_impl_message_writer_take_message(
    struct wfp_message_writer * writer);

extern void
wfp_impl_message_writer_add_int(
    struct wfp_message_writer * writer,
    char const * key,
    int value);

extern void
wfp_impl_message_writer_add_string(
    struct wfp_message_writer * writer,
    char const * key,
    char const * value);

extern void
wfp_impl_message_writer_add_bytes(
    struct wfp_message_writer * writer,
    char const * key,
    char const * data,
    size_t length);

extern void
wfp_impl_message_writer_add_dirbuffer(
    struct wfp_message_writer * writer,
    struct wfp_dirbuffer * dirbuffer);

extern void
wfp_impl_message_writer_set_error(
    struct wfp_message_writer * writer,
    int code);

#ifdef __cplusplus
}
#endif

#endif
