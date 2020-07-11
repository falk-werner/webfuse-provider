#include "webfuse_provider/impl/jsonrpc/response_intern.h"
#include "webfuse_provider/impl/jsonrpc/error.h"
#include "webfuse_provider/status.h"
#include "webfuse_provider/impl/json/node.h"

bool
wfp_jsonrpc_is_response(
    struct wfp_json const * message)
{
	if (NULL == message) { return false; }

	struct wfp_json const * id = wfp_impl_json_object_get(message, "id");
	struct wfp_json const * err = wfp_impl_json_object_get(message, "error");
	struct wfp_json const * result = wfp_impl_json_object_get(message, "result");

	return (wfp_impl_json_is_int(id) && 
		(wfp_impl_json_is_object(err) ||  (NULL != result)));
}


void
wfp_jsonrpc_response_init(
	struct wfp_jsonrpc_response * result,
	struct wfp_json const * response)
{
	result->id = -1;
	result->result = NULL;
	result->error = NULL;

	struct wfp_json const * id_holder = wfp_impl_json_object_get(response, "id");
	if (!wfp_impl_json_is_int(id_holder))
	{
		result->error = wfp_jsonrpc_error_create(WFP_BAD_FORMAT, "invalid format: missing id");
		return;
	}
	
	result->id = wfp_impl_json_get_int(id_holder);
	result->result = wfp_impl_json_object_get(response, "result");
	if (NULL == result->result)
	{
		struct wfp_json const * error = wfp_impl_json_object_get(response, "error");
		if ((wfp_impl_json_is_object(error)) && (wfp_impl_json_is_int(wfp_impl_json_object_get(error, "code"))))
		{
			int code = wfp_impl_json_get_int(wfp_impl_json_object_get(error, "code"));
			char const * message = "";
			if (wfp_impl_json_is_string(wfp_impl_json_object_get(error, "message")))
			{
				message = wfp_impl_json_get_string(wfp_impl_json_object_get(error, "message"));
			}
			result->error = wfp_jsonrpc_error_create(code, message);
		}
		else
		{
			result->error = wfp_jsonrpc_error_create(WFP_BAD_FORMAT, "invalid format: invalid error object");
		}
	}
}

void
wfp_jsonrpc_response_cleanup(
	struct wfp_jsonrpc_response * response)
{
	if (NULL != response->error)
	{
		wfp_jsonrpc_error_dispose(response->error);
	}
}
