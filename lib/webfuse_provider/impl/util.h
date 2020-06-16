#ifndef WFP_UTIL_H
#define WFP_UTIL_H

#ifdef __GNUC__
#define WFP_UNUSED_PARAM(param) param __attribute__((unused))
#else
#define WFP_UNUSED_PARAM(param)
#endif

#endif
