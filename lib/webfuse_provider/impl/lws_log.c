#include "webfuse_provider/impl/lws_log.h"
#include <stdbool.h>
#include <libwebsockets.h>

#define WFP_LWSLOG_DISABLE 0

static bool wfp_impl_lwslog_is_diabled = false;

void wfp_impl_lwslog_disable(void)
{
    if (!wfp_impl_lwslog_is_diabled)
    {
	    lws_set_log_level(WFP_LWSLOG_DISABLE, NULL);
        wfp_impl_lwslog_is_diabled = true;
    }
}


