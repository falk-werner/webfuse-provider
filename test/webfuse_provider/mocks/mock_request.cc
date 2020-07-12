#include "webfuse_provider/mocks/mock_request.hpp"
#include "webfuse_provider/impl/json/doc.h"
#include "webfuse_provider/impl/message.h"
#include "webfuse_provider/impl/message_writer.h"
#include "webfuse_provider/status.h"
#include <cstdlib>

extern "C" 
{

static void webfuse_test_MockRequest_respond(
    struct wfp_message * response,
    void * user_data)
{
    auto * request = reinterpret_cast<webfuse_test::MockRequest*>(user_data);
    int error_code = WFP_BAD;
    int id = -1;

    wfp_json_doc * doc = wfp_impl_json_doc_loadb(response->data, response->length);
    if (NULL != doc)
    {
        wfp_json const * response = wfp_impl_json_doc_root(doc);
        wfp_json const * result = wfp_impl_json_object_get(response, "result");
        wfp_json const * id_holder = wfp_impl_json_object_get(response, "id");
        if (wfp_impl_json_is_int(id_holder)) 
        {
            id = wfp_impl_json_int_get(id_holder);
        }

        if (nullptr != result)
        {
            error_code = WFP_GOOD;
            request->respond(result, id);
        }
        else
        {
            wfp_json const * error = wfp_impl_json_object_get(response, "error");
            if (wfp_impl_json_is_object(error))
            {
                wfp_json const * error_code_holder = wfp_impl_json_object_get(response, "error");
                if (wfp_impl_json_is_int(error_code_holder))
                {
                    error_code = wfp_impl_json_int_get(error_code_holder);
                }
            }

        }

        wfp_impl_json_doc_dispose(doc);
    }


    if (WFP_GOOD != error_code)
    {
        request->respond_error(error_code, id);
    }

    wfp_message_dispose(response);
}

}

namespace webfuse_test
{

struct wfp_request * MockRequest::create_request(int id)
{
    auto * request = reinterpret_cast<wfp_request*>(malloc(sizeof(wfp_request)));
    request->respond   = &webfuse_test_MockRequest_respond;
    request->user_data = reinterpret_cast<void*>(this);
    request->id = id;
    request->writer = wfp_impl_message_writer_create(id);

    return request;
}



}