#ifndef WFP_STATUS_INTERN_H
#define WFP_STATUS_INTERN_H

#include "webfuse_provider/status.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int wfp_status_to_rc(wfp_status status);

extern char const * wfp_status_tostring(wfp_status status);

#ifdef __cplusplus
}
#endif

#endif

