#include "webfuse_provider/impl/dirbuffer.h"
#include <gtest/gtest.h>

TEST(DirBuffer, CreateDispose)
{
    wfp_dirbuffer * buffer = wfp_impl_dirbuffer_create();
    wfp_impl_dirbuffer_dispose(buffer);
}

TEST(DirBuffer, Add)
{
    wfp_dirbuffer * buffer = wfp_impl_dirbuffer_create();
    wfp_impl_dirbuffer_add(buffer, "answer", 42);

    ASSERT_EQ(1, wfp_impl_dirbuffer_size(buffer));

    auto * entry = wfp_impl_dirbuffer_entry_at(buffer, 0);
    ASSERT_STREQ("answer", entry->name);
    ASSERT_EQ(42, entry->inode);

    wfp_impl_dirbuffer_dispose(buffer);
}

TEST(DirBuffer, EntryAt)
{
    wfp_dirbuffer * buffer = wfp_impl_dirbuffer_create();
    auto * entry = wfp_impl_dirbuffer_entry_at(buffer, 0);
    ASSERT_EQ(nullptr, entry);
    wfp_impl_dirbuffer_dispose(buffer);
}
