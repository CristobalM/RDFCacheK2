#include <gtest/gtest.h>

#include <unordered_set>

#include <LRU/LRUController.hpp>
#include <LRU/LRUQueue.hpp>

class DummyLRUController : public LRUController {
  unsigned long max_size_bytes;
  std::unordered_set<unsigned long> data;
  public:

  explicit DummyLRUController(unsigned long max_size_bytes) 
  : max_size_bytes(max_size_bytes){};
  void retrieve_element(unsigned long element_id) override {
    data.insert(element_id);
  };
  void discard_element(unsigned long element_id) override {
    data.erase(element_id);
  };
  unsigned long get_max_size_bytes() override {
    return max_size_bytes;
  };

  unsigned long size() {
    return data.size();
  }
};

TEST(CacheReplacementSuite, LRUReplacementTest_1){
  const unsigned long max_elements = 10;
  const unsigned long max_size_bytes = sizeof(unsigned long) * max_elements;
  DummyLRUController dummy_lru_controller(max_size_bytes);
  LRUQueue queue(dummy_lru_controller);

  for(unsigned long i = 0; i < 11; i++){
    queue.hit_element(i, sizeof(unsigned long));
  }

  ASSERT_EQ(queue.get_stats_erase_count(), 1);
  ASSERT_EQ(queue.get_stats_retrieval_count(), 11);
  for(unsigned long i = 1; i < 11; i++){
    queue.hit_element(i, sizeof(unsigned long));

    ASSERT_EQ(queue.get_stats_erase_count(), 1);
    ASSERT_EQ(queue.get_stats_retrieval_count(), 11);
  }
}
