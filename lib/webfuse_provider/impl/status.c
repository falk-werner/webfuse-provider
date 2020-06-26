#include "webfuse_provider/impl/status_intern.h"

#include <errno.h>

 int wfp_status_to_rc(wfp_status status)
{
	switch(status)
	{
		case WFP_GOOD: return 0;
		case WFP_BAD_NOTIMPLEMENTED: return -ENOSYS;
		case WFP_BAD_TIMEOUT: return -ETIMEDOUT;
		case WFP_BAD_BUSY: return -ENOENT;
		case WFP_BAD_FORMAT: return -ENOENT;
		case WFP_BAD_NOENTRY: return -ENOENT;
		case WFP_BAD_ACCESS_DENIED: return -EACCES;
		default: return -ENOENT;
	}
}

char const * wfp_status_tostring(wfp_status status)
{
	switch(status)
	{
		case WFP_GOOD: return "Good";
		case WFP_BAD: return "Bad";
		case WFP_BAD_NOTIMPLEMENTED: return "Bad (not implemented)";
		case WFP_BAD_TIMEOUT: return "Bad (timeout)";
		case WFP_BAD_BUSY: return "Bad (busy)";
		case WFP_BAD_FORMAT: return "Bad (format)";
		case WFP_BAD_NOENTRY: return "Bad (no entry)";
		case WFP_BAD_ACCESS_DENIED: return "Bad (access denied)";
		default: return "Bad (unknown)";
	}
}
