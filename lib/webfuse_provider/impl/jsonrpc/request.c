#include "webfuse_provider/impl/jsonrpc/request.h"
#include "webfuse_provider/impl/json/node.h"
#include <stdlib.h>

bool
wfp_jsonrpc_is_request(
   struct wfp_json const * message)
{
    if (NULL == message) { return false; }

    struct wfp_json const * id = wfp_impl_json_object_get(message, "id");
    struct wfp_json const * method = wfp_impl_json_object_get(message, "method");
    struct wfp_json const * params = wfp_impl_json_object_get(message, "params");

    return (wfp_impl_json_is_int(id) && wfp_impl_json_is_string(method) &&
            (wfp_impl_json_is_array(params) || wfp_impl_json_is_object(params)));
}
