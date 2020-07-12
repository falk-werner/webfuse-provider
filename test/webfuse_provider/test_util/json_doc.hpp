#ifndef WFP_TEST_UTIL_JSON_DOC_HPP
#define WFP_TEST_UTIL_JSON_DOC_HPP

#include "webfuse_provider/impl/json/parser.h"
#include "webfuse_provider/impl/json/node.h"

#include <string>

namespace webfuse_test
{

class JsonDoc
{
public:
    JsonDoc(std::string const & json);
    ~JsonDoc();
    wfp_json const * root();
private:
    std::string contents;
    wfp_json_doc * doc;
};

}

#endif
