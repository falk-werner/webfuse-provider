#include "webfuse_provider/impl/message.h"

#include <stdlib.h>
#include <libwebsockets.h>

extern struct wfp_message * wfp_message_create(
    char * data,
    size_t length)
{
    struct wfp_message * message = malloc(sizeof(struct wfp_message));
    message->data = data;
    message->length = length;
    message->raw_data = data - LWS_PRE;

    return message;
}

void wfp_message_dispose(
    struct wfp_message * message)
{
    free(message->raw_data);
    free(message);    
}
