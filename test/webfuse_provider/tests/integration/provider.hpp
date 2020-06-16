#ifndef WFP_TEST_INTEGRATION_PROVIDER
#define WFP_TEST_INTEGRATION_PROVIDER

namespace webfuse_test
{

class Provider
{
public:
    explicit Provider(char const * url);
    ~Provider();
private:
    class Private;
    Private * d;
};

}

#endif
