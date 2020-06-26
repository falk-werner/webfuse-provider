#include <gtest/gtest.h>
#include "webfuse_provider/impl/status_intern.h"

TEST(wfp_status, tostring)
{
    ASSERT_STREQ("Good", wfp_status_tostring(WFP_GOOD));
    ASSERT_STREQ("Bad", wfp_status_tostring(WFP_BAD));
    ASSERT_STREQ("Bad (not implemented)", wfp_status_tostring(WFP_BAD_NOTIMPLEMENTED));
    ASSERT_STREQ("Bad (busy)", wfp_status_tostring(WFP_BAD_BUSY));
    ASSERT_STREQ("Bad (timeout)", wfp_status_tostring(WFP_BAD_TIMEOUT));
    ASSERT_STREQ("Bad (format)", wfp_status_tostring(WFP_BAD_FORMAT));
    ASSERT_STREQ("Bad (no entry)", wfp_status_tostring(WFP_BAD_NOENTRY));
    ASSERT_STREQ("Bad (access denied)", wfp_status_tostring(WFP_BAD_ACCESS_DENIED));

    ASSERT_STREQ("Bad (unknown)", wfp_status_tostring(-1));
}

TEST(wfp_status, to_rc)
{
    ASSERT_EQ(0, wfp_status_to_rc(WFP_GOOD));
    ASSERT_EQ(-ENOENT, wfp_status_to_rc(WFP_BAD));
    ASSERT_EQ(-ENOSYS, wfp_status_to_rc(WFP_BAD_NOTIMPLEMENTED));
    ASSERT_EQ(-ENOENT, wfp_status_to_rc(WFP_BAD_BUSY));
    ASSERT_EQ(-ETIMEDOUT, wfp_status_to_rc(WFP_BAD_TIMEOUT));
    ASSERT_EQ(-ENOENT, wfp_status_to_rc(WFP_BAD_FORMAT));
    ASSERT_EQ(-ENOENT, wfp_status_to_rc(WFP_BAD_NOENTRY));
    ASSERT_EQ(-EACCES, wfp_status_to_rc(WFP_BAD_ACCESS_DENIED));

    ASSERT_EQ(-ENOENT, wfp_status_to_rc(-1));
}