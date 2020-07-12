#ifndef WFP_MOCK_REQUEST_HPP
#define WFP_MOCK_REQUEST_HPP

#include "webfuse_provider/impl/request.h"
#include "webfuse_provider/impl/json/node.h"
#include <gmock/gmock.h>
#include <cstring>


namespace webfuse_test
{

class MockRequest
{
public:
    struct wfp_request * create_request(int id);
    MOCK_METHOD2(respond, void(wfp_json const * result, int id));
    MOCK_METHOD2(respond_error, void(int error_code, int id));
};

MATCHER_P3(StatMatcher, inode, mode, file_type, "") 
{
    wfp_json const * inode_holder = wfp_impl_json_object_get(arg, "inode");
    if ((!wfp_impl_json_is_int(inode_holder)) || (inode != wfp_impl_json_int_get(inode_holder)))
    {
        *result_listener << "missing inode";
        return false;
    }

    wfp_json const * mode_holder = wfp_impl_json_object_get(arg, "mode");
    if ((!wfp_impl_json_is_int(mode_holder)) || (mode != wfp_impl_json_int_get(mode_holder)))
    {
        *result_listener << "missing mode";
        return false;
    }

    wfp_json const * type_holder = wfp_impl_json_object_get(arg, "type");
    if ((!wfp_impl_json_is_string(type_holder)) || (0 != strcmp(file_type, wfp_impl_json_string_get(type_holder))))
    {
        *result_listener << "missing type";
        return false;
    }

    return true;
}

MATCHER_P(OpenMatcher, handle, "") 
{
    wfp_json const * handle_holder = wfp_impl_json_object_get(arg, "handle");
    if ((!wfp_impl_json_is_int(handle_holder)) || (handle != wfp_impl_json_int_get(handle_holder)))
    {
        *result_listener << "missing handle";
        return false;
    }

    return true;
}

MATCHER_P3(ReadResultMatcher, data, format, count, "") 
{
    wfp_json const * format_holder = wfp_impl_json_object_get(arg, "format");
    if ((!wfp_impl_json_is_string(format_holder)) || (0 != strcmp(format, wfp_impl_json_string_get(format_holder))))
    {
        *result_listener << "invalid or missing format: " << wfp_impl_json_string_get(format_holder);
        return false;
    }

    wfp_json const * count_holder = wfp_impl_json_object_get(arg, "count");
    if ((!wfp_impl_json_is_int(count_holder)) || (count != wfp_impl_json_int_get(count_holder)))
    {
        *result_listener << "invalid or missing count: " << wfp_impl_json_int_get(count_holder);
        return false;
    }

    wfp_json const * data_holder = wfp_impl_json_object_get(arg, "data");
    if ((!wfp_impl_json_is_string(data_holder)) || (0 != strcmp(data, wfp_impl_json_string_get(data_holder))))
    {
        *result_listener << "invalid or missing data: " << wfp_impl_json_string_get(data_holder);
        return false;
    }

    return true;
}

MATCHER_P(ReaddirMatcher, contained_elements , "") 
{
    if (!wfp_impl_json_is_array(arg))
    {
        *result_listener << "result is not array";
        return false;
    }

    {
        for(size_t i = 0; i < wfp_impl_json_array_size(arg); i++)
        {
            wfp_json const * value = wfp_impl_json_array_get(arg, i);
            wfp_json const * inode = wfp_impl_json_object_get(value, "inode");
            wfp_json const * name = wfp_impl_json_object_get(value, "name");

            if(!wfp_impl_json_is_int(inode))
            {
                *result_listener << "invalid result: missing inode";
                return false;
            }

            if (!wfp_impl_json_is_string(name))
            {
                *result_listener << "invalid result: missing name";
                return false;
            }
        }
    }

    for(size_t i = 0; NULL != contained_elements[i]; i++)
    {
        char const * element = contained_elements[i];
        bool found = false;

        for(size_t j = 0; j < wfp_impl_json_array_size(arg); j++)
        {
            wfp_json const * value = wfp_impl_json_array_get(arg, j);
            wfp_json const * name = wfp_impl_json_object_get(value, "name");

            found  = (0 == strcmp(element, wfp_impl_json_string_get(name)));
            if (found)
            {
                break;
            }
        }

        if (!found)
        {
            *result_listener << "missing required directory element: " << element;
            return false;
        }
    }

    return true;
}

}


#endif
