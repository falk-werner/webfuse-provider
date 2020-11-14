#ifndef WFP_IMPL_CLIENT_PROTOCOL_H
#define WFP_IMPL_CLIENT_PROTOCOL_H

#include "webfuse_provider/impl/provider.h"
#include "webfuse_provider/impl/request.h"

#include "webfuse_provider/impl/util/slist.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_client_config;
struct lws_protocols;
struct lws_context;
struct wfp_jsonrpc_proxy;
struct wfp_timer_manager;

struct wfp_client_protocol
{
    bool is_connected;
    bool is_shutdown_requested;
    struct wfp_request request;
    struct wfp_provider provider;
    char * fs_name;
    void * user_data;
    struct lws * wsi;
    struct wfp_timer_manager * timer_manager;
    struct wfp_jsonrpc_proxy * proxy;
    struct wfp_slist messages;
};

extern void wfp_impl_client_protocol_init(
    struct wfp_client_protocol * protocol,
    struct wfp_provider const * provider,
    char const * fs_name,
    void * user_data);

extern void wfp_impl_client_protocol_cleanup(
    struct wfp_client_protocol * protocol);

extern struct wfp_client_protocol * wfp_impl_client_protocol_create(
    struct wfp_client_config const * config);

extern void wfp_impl_client_protocol_dispose(
    struct wfp_client_protocol * protocol);

extern void wfp_impl_client_protocol_init_lws(
    struct wfp_client_protocol * protocol,
    struct lws_protocols * lws_protocol);

extern void wfp_impl_client_protocol_connect(
    struct wfp_client_protocol * protocol,
    struct lws_context * context,
    char const * url);

extern void wfp_impl_client_protocol_disconnect(
    struct wfp_client_protocol * protocol);

#ifdef __cplusplus
}
#endif

#endif
