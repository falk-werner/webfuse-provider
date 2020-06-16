#include "webfuse_provider/impl/jsonrpc/proxy_intern.h"
#include "webfuse_provider/impl/jsonrpc/response_intern.h"
#include "webfuse_provider/impl/jsonrpc/error.h"
#include "webfuse_provider/status.h"

#include "webfuse_provider/impl/timer/timer.h"

#include <stdlib.h>
#include <string.h>

struct wfp_jsonrpc_proxy *
wfp_jsonrpc_proxy_create(
    struct wfp_timer_manager * manager,
    int timeout,
    wfp_jsonrpc_send_fn * send,
    void * user_data)
{
    struct wfp_jsonrpc_proxy * proxy = malloc(sizeof(struct wfp_jsonrpc_proxy));
    wfp_jsonrpc_proxy_init(proxy, manager, timeout, send, user_data);

    return proxy;
}

void wfp_jsonrpc_proxy_dispose(
    struct wfp_jsonrpc_proxy * proxy)
{
    wfp_jsonrpc_proxy_cleanup(proxy);
    free(proxy);
}

static void wfp_jsonrpc_proxy_on_timeout(
    struct wfp_timer * timer, void * proxy_ptr)
{
    struct wfp_jsonrpc_proxy * proxy = proxy_ptr;

    if (proxy->request.is_pending)
    {
        wfp_jsonrpc_proxy_finished_fn * finished = proxy->request.finished;
        void * user_data = proxy->request.user_data;

        proxy->request.is_pending = false;
        proxy->request.id = 0;
        proxy->request.user_data = NULL;
        proxy->request.finished = NULL;
        wfp_timer_cancel(timer);

        wfp_jsonrpc_propate_error(finished, user_data, WFP_BAD_TIMEOUT, "Timeout");
    }
}

static json_t * wfp_jsonrpc_request_create(
	char const * method,
	int id,
	char const * param_info,
	va_list args)
{
	json_t * request = json_object();
	json_object_set_new(request, "method", json_string(method));
	json_t * params = json_array();
	
	for (char const * param_type = param_info; '\0' != *param_type; param_type++)
	{
		switch(*param_type)
		{
			case 's':
			{
				char const * const value = va_arg(args, char const *);
				json_array_append_new(params, json_string(value));
			}
			break;
			case 'i':
			{
				int const value = va_arg(args, int);
				json_array_append_new(params, json_integer(value));
			}
			break;
            case 'j':
            {
                json_t * const value = va_arg(args, json_t *);
                json_array_append_new(params, value);
            }
            break;
			default:
			fprintf(stderr, "fatal: unknown param_type '%c'\n", *param_type);
            json_decref(params);
            json_decref(request);
            return NULL;
		}
	}
	

	json_object_set_new(request, "params", params);
	if (0 != id)
	{
		json_object_set_new(request, "id", json_integer(id));
	}
	
	return request;
}

void wfp_jsonrpc_proxy_init(
    struct wfp_jsonrpc_proxy * proxy,
    struct wfp_timer_manager * timeout_manager,
    int timeout,
    wfp_jsonrpc_send_fn * send,
    void * user_data)
{
    proxy->send = send;
    proxy->timeout = timeout;
    proxy->user_data = user_data;
    proxy->request.is_pending = false;
    proxy->request.timer = wfp_timer_create(timeout_manager, 
        &wfp_jsonrpc_proxy_on_timeout, proxy);    
}

void wfp_jsonrpc_proxy_cleanup(
    struct wfp_jsonrpc_proxy * proxy)
{
    if (proxy->request.is_pending)
    {
        void * user_data = proxy->request.user_data;
        wfp_jsonrpc_proxy_finished_fn * finished = proxy->request.finished;

        proxy->request.is_pending = false;
        proxy->request.finished = NULL;
        proxy->request.user_data = NULL;
        proxy->request.id = 0;
        wfp_timer_cancel(proxy->request.timer);

        wfp_jsonrpc_propate_error(finished, user_data, WFP_BAD, "Bad: cancelled pending request during shutdown");
    }

    wfp_timer_dispose(proxy->request.timer);
}

void wfp_jsonrpc_proxy_vinvoke(
	struct wfp_jsonrpc_proxy * proxy,
	wfp_jsonrpc_proxy_finished_fn * finished,
	void * user_data,
	char const * method_name,
	char const * param_info,
	va_list args)
{
    if (!proxy->request.is_pending)
    {
        proxy->request.is_pending = true;
        proxy->request.finished = finished;
        proxy->request.user_data = user_data;
        proxy->request.id = 42;
        wfp_timer_start(proxy->request.timer, proxy->timeout);
        
        json_t * request = wfp_jsonrpc_request_create(method_name, proxy->request.id, param_info, args);

        bool const is_send = ((NULL != request) && (proxy->send(request, proxy->user_data)));
        if (!is_send)
        {
            proxy->request.is_pending = false;
            proxy->request.finished = NULL;
            proxy->request.user_data = NULL;
            proxy->request.id = 0;
            wfp_timer_cancel(proxy->request.timer);

            wfp_jsonrpc_propate_error(finished, user_data, WFP_BAD, "Bad: requenst is not sent");
        }

        if (NULL != request)
        {
            json_decref(request);
        }
    }
    else
    {
        wfp_jsonrpc_propate_error(finished, user_data, WFP_BAD_BUSY, "Busy");
    }
}

extern void wfp_jsonrpc_proxy_vnotify(
	struct wfp_jsonrpc_proxy * proxy,
	char const * method_name,
	char const * param_info,
	va_list args)
{
    json_t * request = wfp_jsonrpc_request_create(method_name, 0, param_info, args);

    if (NULL != request)
    {
        proxy->send(request, proxy->user_data);
        json_decref(request);
    }
}


void wfp_jsonrpc_proxy_onresult(
    struct wfp_jsonrpc_proxy * proxy,
    json_t * message)
{
	struct wfp_jsonrpc_response response;
	wfp_jsonrpc_response_init(&response, message);

    if ((proxy->request.is_pending) && (response.id == proxy->request.id))
    {
        wfp_jsonrpc_proxy_finished_fn * finished = proxy->request.finished;
        void * user_data = proxy->request.user_data;

        proxy->request.is_pending = false;
        proxy->request.id = 0;
        proxy->request.user_data = NULL;
        proxy->request.finished = NULL;
        wfp_timer_cancel(proxy->request.timer);

        finished(user_data, response.result, response.error);
    }

    wfp_jsonrpc_response_cleanup(&response);
}

