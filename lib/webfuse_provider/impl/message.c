#include "webfuse_provider/impl/message.h"

#include <stdlib.h>
#include <libwebsockets.h>

extern struct wfp_message * wfp_message_create(json_t const * value)
{
    size_t const length = json_dumpb(value, NULL, 0, JSON_COMPACT);

    char * data = malloc(sizeof(struct wfp_message) + LWS_PRE + length);
    struct wfp_message * message = (struct wfp_message *) data;
    message->data = &data[sizeof(struct wfp_message) + LWS_PRE];
    message->length = length;

    json_dumpb(value, message->data, length, JSON_COMPACT);

    return message;
}

void wfp_message_dispose(
    struct wfp_message * message)
{
    free(message);    
}
