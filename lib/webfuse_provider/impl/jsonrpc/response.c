#include "webfuse_provider/impl/jsonrpc/response_intern.h"
#include "webfuse_provider/impl/jsonrpc/error.h"
#include "webfuse_provider/status.h"

bool
wfp_jsonrpc_is_response(
    json_t * message)
{
	json_t * id = json_object_get(message, "id");
	json_t * err = json_object_get(message, "error");
	json_t * result = json_object_get(message, "result");

	return (json_is_integer(id) && 
		(json_is_object(err) ||  (NULL != result)));
}


void
wfp_jsonrpc_response_init(
	struct wfp_jsonrpc_response * result,
	json_t * response)
{
	result->id = -1;
	result->result = NULL;
	result->error = NULL;

	json_t * id_holder = json_object_get(response, "id");
	if (!json_is_integer(id_holder))
	{
		result->error = wfp_jsonrpc_error(WFP_BAD_FORMAT, "invalid format: missing id");
		return;
	}
	
	result->id = json_integer_value(id_holder);
	result->result = json_object_get(response, "result");
	if (NULL != result->result)
	{
		json_incref(result->result);
	}
	else
	{
		json_t * error = json_object_get(response, "error");
		if ((json_is_object(error)) && (json_is_integer(json_object_get(error, "code"))))
		{
			result->error = error;
			json_incref(result->error);
		}
		else
		{
			result->error = wfp_jsonrpc_error(WFP_BAD_FORMAT, "invalid format: invalid error object");
		}
	}
}

void
wfp_jsonrpc_response_cleanup(
	struct wfp_jsonrpc_response * response)
{
    if (NULL != response->result)
    {
        json_decref(response->result);
    }

    if (NULL != response->error)
    {
        json_decref(response->error);
    }
}
