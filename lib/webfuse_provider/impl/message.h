#ifndef WFP_MESSAGE_H
#define WFP_MESSAGE_H

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
using std::size_t;
#endif

#include "webfuse_provider/impl/util/slist.h"

struct wfp_message
{
    struct wfp_slist_item item;
    char * data;
    char * raw_data;
    size_t length;
};

#ifdef __cplusplus
extern "C"
{
#endif

extern struct wfp_message * 
wfp_message_create(
    char * data,
    size_t length);

extern void wfp_message_dispose(
    struct wfp_message * message);

#ifdef __cplusplus
}
#endif

#endif
