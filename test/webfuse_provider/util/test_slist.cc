#include <gtest/gtest.h>
#include "webfuse_provider/impl/util/slist.h"

TEST(wfp_slist, init)
{
    struct wfp_slist list;
    wfp_slist_init(&list);

    ASSERT_EQ(nullptr, list.head.next);
    ASSERT_EQ(nullptr, list.last->next);
    ASSERT_EQ(&list.head, list.last);
    ASSERT_TRUE(wfp_slist_empty(&list));
    ASSERT_EQ(nullptr, wfp_slist_first(&list));    
}

TEST(wfp_slist, append)
{
    struct wfp_slist list;
    struct wfp_slist_item item[3];

    wfp_slist_init(&list);
    ASSERT_TRUE(wfp_slist_empty(&list));

    wfp_slist_append(&list, &item[0]);
    ASSERT_NE(&list.head, list.last);
    ASSERT_FALSE(wfp_slist_empty(&list));
    ASSERT_EQ(&item[0], wfp_slist_first(&list));    
    ASSERT_EQ(&item[0], list.head.next);
    ASSERT_EQ(&item[0], list.last);
    ASSERT_EQ(nullptr, list.last->next);
    ASSERT_EQ(nullptr, item[0].next);

    wfp_slist_append(&list, &item[1]);
    ASSERT_NE(&list.head, list.last);
    ASSERT_FALSE(wfp_slist_empty(&list));
    ASSERT_EQ(&item[0], wfp_slist_first(&list));    
    ASSERT_EQ(&item[0], list.head.next);
    ASSERT_EQ(&item[1], list.last);
    ASSERT_EQ(nullptr, list.last->next);
    ASSERT_EQ(&item[1], item[0].next);
    ASSERT_EQ(nullptr, item[1].next);

    wfp_slist_append(&list, &item[2]);
    ASSERT_NE(&list.head, list.last);
    ASSERT_FALSE(wfp_slist_empty(&list));
    ASSERT_EQ(&item[0], wfp_slist_first(&list));    
    ASSERT_EQ(&item[0], list.head.next);
    ASSERT_EQ(&item[2], list.last);
    ASSERT_EQ(nullptr, list.last->next);
    ASSERT_EQ(&item[1], item[0].next);
    ASSERT_EQ(&item[2], item[1].next);
    ASSERT_EQ(nullptr, item[2].next);
}

TEST(wfp_slist_remove_after, remove_first)
{
    struct wfp_slist list;
    struct wfp_slist_item item[3];

    wfp_slist_init(&list);
    wfp_slist_append(&list, &item[0]);
    wfp_slist_append(&list, &item[1]);
    wfp_slist_append(&list, &item[2]);

    wfp_slist_item * removed;
    
    removed = wfp_slist_remove_first(&list);
    ASSERT_FALSE(wfp_slist_empty(&list));
    ASSERT_EQ(&item[0], removed);

    removed = wfp_slist_remove_first(&list);
    ASSERT_FALSE(wfp_slist_empty(&list));
    ASSERT_EQ(&item[1], removed);

    removed = wfp_slist_remove_first(&list);
    ASSERT_TRUE(wfp_slist_empty(&list));
    ASSERT_EQ(&item[2], removed);

    ASSERT_EQ(nullptr, list.head.next);
    ASSERT_EQ(nullptr, list.last->next);
    ASSERT_EQ(&list.head, list.last);
    ASSERT_EQ(nullptr, wfp_slist_first(&list));    
}

TEST(wfp_slist_remove_after, remove_last)
{
    struct wfp_slist list;
    struct wfp_slist_item item[3];

    wfp_slist_init(&list);
    wfp_slist_append(&list, &item[0]);
    wfp_slist_append(&list, &item[1]);
    wfp_slist_append(&list, &item[2]);

    wfp_slist_item * removed;
    
    removed = wfp_slist_remove_after(&list, &item[1]);
    ASSERT_FALSE(wfp_slist_empty(&list));
    ASSERT_EQ(&item[2], removed);

    removed = wfp_slist_remove_after(&list, &item[0]);
    ASSERT_FALSE(wfp_slist_empty(&list));
    ASSERT_EQ(&item[1], removed);

    removed = wfp_slist_remove_after(&list, &list.head);
    ASSERT_TRUE(wfp_slist_empty(&list));
    ASSERT_EQ(&item[0], removed);

    ASSERT_EQ(nullptr, list.head.next);
    ASSERT_EQ(nullptr, list.last->next);
    ASSERT_EQ(&list.head, list.last);
    ASSERT_EQ(nullptr, wfp_slist_first(&list));    
}

TEST(wfp_slist_remove_after, remove_after)
{
    struct wfp_slist list;
    struct wfp_slist_item item[3];

    wfp_slist_init(&list);
    wfp_slist_append(&list, &item[0]);
    wfp_slist_append(&list, &item[1]);
    wfp_slist_append(&list, &item[2]);

    wfp_slist_item * removed;
    
    removed = wfp_slist_remove_after(&list, &item[0]);
    ASSERT_FALSE(wfp_slist_empty(&list));
    ASSERT_EQ(&item[1], removed);

    ASSERT_NE(&list.head, list.last);
    ASSERT_FALSE(wfp_slist_empty(&list));
    ASSERT_EQ(&item[0], wfp_slist_first(&list));    
    ASSERT_EQ(&item[0], list.head.next);
    ASSERT_EQ(&item[2], list.last);
    ASSERT_EQ(nullptr, list.last->next);
    ASSERT_EQ(&item[2], item[0].next);
    ASSERT_EQ(nullptr, item[2].next);
}
