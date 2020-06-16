#ifndef WFP_URL_H
#define WFP_URL_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif
struct wfp_url
{
    char * host;
    int port;
    char * path;
    bool use_tls;
};

extern bool wfp_url_init(
    struct wfp_url * url,
    char const * value);

extern void wfp_url_cleanup(
    struct wfp_url * url);


#ifdef __cplusplus
}
#endif

#endif
