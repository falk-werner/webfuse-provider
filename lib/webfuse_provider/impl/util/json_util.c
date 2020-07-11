#include "webfuse_provider/impl/util/json_util.h"

int wfp_impl_json_get_integer(json_t const * object, char const * key, int default_value)
{
	int result = default_value;

	json_t * holder = json_object_get(object, key);
	if (json_is_integer(holder))
	{
		result = json_integer_value(holder);
	}

	return result;
}

wfp_status 
wfp_impl_jsonrpc_get_status(
	json_t const * error)
{
	wfp_status status = WFP_GOOD;
	if (NULL != error)
	{
		status = wfp_impl_json_get_integer(error, "code", WFP_BAD_FORMAT);
	}

	return status;
}
