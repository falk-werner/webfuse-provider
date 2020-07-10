#include "webfuse_provider/impl/jsonrpc/request.h"
#include <stdlib.h>

bool
wfp_jsonrpc_is_request(
    json_t * message)
{
    json_t * id = json_object_get(message, "id");
    json_t * method = json_object_get(message, "method");
    json_t * params = json_object_get(message, "params");

    return (json_is_integer(id) && json_is_string(method) &&
            (json_is_array(params) || json_is_object(params)));
}
