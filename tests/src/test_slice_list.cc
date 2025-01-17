#include <gtest/gtest.h>
#include <xcl/util/slice_list.h>

class SliceListTest : public testing::Test {
 protected:
  SliceList* sl;
  int values[1024];

  void SetUp() {
    ASSERT_NE((sl = SliceList_new(4)), nullptr);
    for (int i = 0; i < 1024; i++)
      values[i] = i;
  }
  void TearDown() { SliceList_free(sl); }
};

TEST_F(SliceListTest, test1) {
  ASSERT_EQ(SliceList_cap(sl), 0);
  ASSERT_TRUE(SliceList_insert(sl, SliceList_size(sl), values, 1024));
  ASSERT_EQ(SliceList_size(sl), 1024);
  for (int i = 0; i < 1024; i++)
    ASSERT_EQ(values[i], *(int*)SliceList_at(sl, i));
  ASSERT_TRUE(SliceList_removen(sl, 0, 512, nullptr));
  ASSERT_EQ(SliceList_size(sl), 512);
  for (int i = 0; i < 512; i++)
    ASSERT_EQ(values[i + 512], *(int*)SliceList_at(sl, i));
  SliceList_clear(sl);
  ASSERT_TRUE(SliceList_filln(sl, 1024, &values[3]));
  for (int i = 0; i < 1024; i++)
    ASSERT_EQ(values[3], *(int*)SliceList_at(sl, i));
  ASSERT_EQ(SliceList_size(sl), 1024);
  SliceList_clear(sl);
  ASSERT_TRUE(SliceList_fill(sl, values, 1024));
  ASSERT_EQ(SliceList_size(sl), 1024);
  for (int i = 0; i < 1024; i++)
    ASSERT_EQ(values[i], *(int*)SliceList_at(sl, i));
  ASSERT_TRUE(SliceList_insertn(sl, 19, 10, &values[98]));
  ASSERT_EQ(SliceList_size(sl), 1034);
  for (int i = 0; i < 19; i++)
    ASSERT_EQ(values[i], *(int*)SliceList_at(sl, i));
  for (int i = 29; i < 1034; i++)
    ASSERT_EQ(values[i - 10], *(int*)SliceList_at(sl, i));
  for (int i = 19; i < 29; i++)
    ASSERT_EQ(*(int*)SliceList_at(sl, i), values[98]);
}

TEST_F(SliceListTest, test2) {
  for (int i = 0; i < 1024; i++)
    ASSERT_TRUE(SliceList_push(sl, &values[i]));
  ASSERT_EQ(SliceList_size(sl), 1024);
  int count = 0;
  while (!SliceList_empty(sl)) {
    ++count;
    int val;
    ASSERT_TRUE(SliceList_pop(sl, &val));
    ASSERT_EQ(val, values[1024 - count]);
    ASSERT_EQ(SliceList_size(sl), 1024 - count);
  }
  ASSERT_TRUE(SliceList_empty(sl));
}
