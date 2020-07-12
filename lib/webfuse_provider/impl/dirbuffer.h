#ifndef WFP_IMPL_DIRBUFFER_H
#define WFP_IMPL_DIRBUFFER_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_dirbuffer_entry
{
    char * name;
    ino_t inode;
};

struct wfp_dirbuffer
{
    struct wfp_dirbuffer_entry * entries;
    size_t size;
    size_t capacity;
};

extern struct wfp_dirbuffer * wfp_impl_dirbuffer_create(void);

extern void wfp_impl_dirbuffer_dispose(
    struct wfp_dirbuffer * buffer);

extern void wfp_impl_dirbuffer_add(
    struct wfp_dirbuffer * buffer,
    char const * name,
    ino_t inode);

extern size_t
wfp_impl_dirbuffer_size(
    struct wfp_dirbuffer * buffer);

extern struct wfp_dirbuffer_entry const *
wfp_impl_dirbuffer_entry_at(
    struct wfp_dirbuffer * buffer,
    size_t pos);

#ifdef __cplusplus
}
#endif

#endif
