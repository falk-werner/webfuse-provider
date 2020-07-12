#include "webfuse_provider/test_util/json_doc.hpp"

namespace webfuse_test
{

JsonDoc::JsonDoc(std::string const & json)
: contents(json)
{
    doc = wfp_impl_json_parse(const_cast<char*>(contents.data()));
}

JsonDoc::~JsonDoc()
{
    wfp_impl_json_dispose(doc);
}

wfp_json const * JsonDoc::root()
{
    return wfp_impl_json_root(doc);
}

}