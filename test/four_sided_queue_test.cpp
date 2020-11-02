#include <gtest/gtest.h>

#include <FourSidedQueue.hpp>

struct QItem {
  int number;
};

TEST(FOUR_SIDED_QUEUE_TCASE, can_push_back_pop_back_simple) {
  FourSidedQueue<QItem> fqueue;
  QItem q1 = {1};
  QItem q2 = {2};
  QItem q3 = {3};
  QItem q4 = {4};
  fqueue.push_back(q1, 1);
  fqueue.push_back(q2, 2);
  fqueue.push_back(q3, 3);
  fqueue.push_back(q4, 4);

  for (int i = 4; !fqueue.empty() && i > 0; i--) {
    QItem item = fqueue.pop_back();
    ASSERT_EQ(item.number, i);
  }
}

TEST(FOUR_SIDED_QUEUE_TCASE, can_push_front_pop_front_simple) {
  FourSidedQueue<QItem> fqueue;
  QItem q1 = {1};
  QItem q2 = {2};
  QItem q3 = {3};
  QItem q4 = {4};
  fqueue.push_front(q1, 1);
  fqueue.push_front(q2, 2);
  fqueue.push_front(q3, 3);
  fqueue.push_front(q4, 4);

  for (int i = 4; !fqueue.empty() && i > 0; i--) {
    QItem item = fqueue.pop_front();
    ASSERT_EQ(item.number, i);
  }
}

TEST(FOUR_SIDED_QUEUE_TCASE, can_push_back_pop_front_simple) {
  FourSidedQueue<QItem> fqueue;
  QItem q1 = {1};
  QItem q2 = {2};
  QItem q3 = {3};
  QItem q4 = {4};
  fqueue.push_back(q1, 1);
  fqueue.push_back(q2, 2);
  fqueue.push_back(q3, 3);
  fqueue.push_back(q4, 4);

  for (int i = 1; !fqueue.empty() && i <= 4; i++) {
    QItem item = fqueue.pop_front();
    ASSERT_EQ(item.number, i);
  }
}

TEST(FOUR_SIDED_QUEUE_TCASE, can_push_front_pop_back_simple) {
  FourSidedQueue<QItem> fqueue;
  QItem q1 = {1};
  QItem q2 = {2};
  QItem q3 = {3};
  QItem q4 = {4};
  fqueue.push_front(q1, 1);
  fqueue.push_front(q2, 2);
  fqueue.push_front(q3, 3);
  fqueue.push_front(q4, 4);

  for (int i = 1; !fqueue.empty() && i <= 4; i++) {
    QItem item = fqueue.pop_back();
    ASSERT_EQ(item.number, i);
  }
}

TEST(FOUR_SIDED_QUEUE_TCASE, can_pop_back_vertically_after_push_front) {
  FourSidedQueue<QItem> fqueue;
  QItem q1 = {1};
  QItem q2 = {2};
  QItem q3 = {3};
  QItem q4 = {4};
  fqueue.push_front(q1, 1);
  fqueue.push_front(q2, 2);
  fqueue.push_front(q3, 3);
  fqueue.push_front(q4, 4);

  for (int i = 1; i <= 4; i++) {
    QItem item = fqueue.pop_back_vertical(i);
    ASSERT_EQ(item.number, i) << "Different item stored than expected ("
                              << item.number << ", " << i << ")";
  }
  ASSERT_TRUE(fqueue.empty())
      << "Queue was not empty after clearing vertical queues";
}

TEST(FOUR_SIDED_QUEUE_TCASE, can_pop_front_vertically_after_push_back) {
  FourSidedQueue<QItem> fqueue;
  QItem q1 = {1};
  QItem q2 = {2};
  QItem q3 = {3};
  QItem q4 = {4};
  fqueue.push_back(q1, 1);
  fqueue.push_back(q2, 2);
  fqueue.push_back(q3, 3);
  fqueue.push_back(q4, 4);

  for (int i = 1; i <= 4; i++) {
    ASSERT_FALSE(fqueue.is_vertical_empty(i))
        << "Vertical queue " << i << " appears to be empty";
    QItem item = fqueue.pop_front_vertical(i);
    ASSERT_EQ(item.number, i) << "Different item stored than expected ("
                              << item.number << ", " << i << ")";
    ASSERT_TRUE(fqueue.is_vertical_empty(i))
        << "Vertical queue " << i << " appears to be  non-empty";
  }
  ASSERT_TRUE(fqueue.empty())
      << "Queue was not empty after clearing vertical queues";
}

TEST(FOUR_SIDED_QUEUE_TCASE, can_mix_vertical_and_horizontal_pop_1) {
  FourSidedQueue<QItem> fqueue;
  QItem q1 = {1};
  QItem q2 = {2};
  QItem q3 = {3};
  QItem q4 = {4};
  fqueue.push_back(q1, 1);
  ASSERT_EQ(fqueue.vertical_size(1), 1) << "Vertical size unexpected value";
  fqueue.push_back(q2, 2);
  ASSERT_EQ(fqueue.vertical_size(2), 1) << "Vertical size unexpected value";
  fqueue.push_back(q3, 3);
  ASSERT_EQ(fqueue.vertical_size(3), 1) << "Vertical size unexpected value";
  fqueue.push_back(q4, 4);
  ASSERT_EQ(fqueue.vertical_size(4), 1) << "Vertical size unexpected value";

  auto item = fqueue.pop_back_vertical(1);
  ASSERT_EQ(item.number, 1) << "Different value than expected";
  ASSERT_EQ(fqueue.size(), 3) << "4sided queue size different than expected";
  ASSERT_EQ(fqueue.vertical_size(1), 0) << "Vertical size unexpected value";

  item = fqueue.pop_back();
  ASSERT_EQ(item.number, 4) << "Different value than expected";
  ASSERT_EQ(fqueue.size(), 2) << "4sided queue size different than expected";
  ASSERT_EQ(fqueue.vertical_size(4), 0) << "Vertical size unexpected value";

  item = fqueue.pop_front_vertical(3);
  ASSERT_EQ(item.number, 3) << "Different value than expected";
  ASSERT_EQ(fqueue.size(), 1) << "4sided queue size different than expected";
  ASSERT_EQ(fqueue.vertical_size(3), 0) << "Vertical size unexpected value";

  item = fqueue.pop_front();
  ASSERT_EQ(item.number, 2) << "Different value than expected";
  ASSERT_EQ(fqueue.vertical_size(2), 0) << "Vertical size unexpected value";

  ASSERT_TRUE(fqueue.empty())
      << "Queue was not empty after clearing vertical queues";
}

TEST(FOUR_SIDED_QUEUE_TCASE, can_pop_next) {
  FourSidedQueue<QItem> fqueue;
  QItem q1 = {1};
  QItem q2 = {2};
  QItem q3 = {3};
  QItem q4 = {4};
  fqueue.push_back(q1, 1);
  fqueue.push_back(q2, 2);
  fqueue.push_back(q3, 3);
  fqueue.push_back(q4, 4);

  ASSERT_EQ(fqueue.pop_next(1).number, 1);
  ASSERT_EQ(fqueue.pop_next(2).number, 2);
  ASSERT_EQ(fqueue.pop_next(3).number, 3);
  ASSERT_EQ(fqueue.pop_next(4).number, 4);
}

TEST(FOUR_SIDED_QUEUE_TCASE, can_pop_next_2) {
  FourSidedQueue<QItem> fqueue;
  QItem q1 = {1};
  QItem q2 = {2};
  QItem q3 = {3};
  QItem q4 = {4};
  fqueue.push_back(q1, 1);
  fqueue.push_back(q2, 2);
  fqueue.push_back(q3, 3);
  fqueue.push_back(q4, 4);

  ASSERT_EQ(fqueue.pop_next(4).number, 4);
  ASSERT_EQ(fqueue.pop_next(4).number, 1);
  ASSERT_EQ(fqueue.pop_next(4).number, 2);
  ASSERT_EQ(fqueue.pop_next(4).number, 3);
}
