#include <gtest/gtest.h>

#include <PredicatesIndexCacheMD.hpp>
#include <PredicatesIndexFileBuilder.hpp>
#include <external_sort.hpp>
#include <serialization_util.hpp>
#include <sstream>
#include <string>
#include <utility>

static std::pair<PredicatesIndexCacheMD, unsigned long> build_picmd() {

  K2TreeConfig config;
  config.cut_depth = 10;
  config.max_node_count = 256;
  config.treedepth = 32;

  std::stringstream ss;
  auto out = std::make_unique<std::stringstream>();
  std::stringstream tmp;
  unsigned long sz = 10000;
  write_u64(ss, sz);
  for (unsigned long i = sz; i > 0; i--) {
    TripleValue triple(i, i, i);
    triple.write_to_file(ss);
  }
  ss.seekg(0);
  out->seekp(0);
  PredicatesIndexFileBuilder::build(ss, *out, tmp, config);

  return {PredicatesIndexCacheMD(std::move(out)), sz};
}

TEST(predicates_metadata_serialization, can_create_save_and_retrieve) {

  auto [pc, sz] = build_picmd();

  for (unsigned long i = sz; i > 0; i--) {
    auto &k2tree = pc.fetch_k2tree(i);
    ASSERT_TRUE(k2tree.has(i, i));
    ASSERT_FALSE(k2tree.has(i, i + 1));
  }
}

TEST(predicates_metadata_serialization, can_sync_with_external) {
  auto [pc, sz] = build_picmd();

  for(unsigned long i = 20'000; i < 30'000; i++){
    pc.insert_point(i, i, i);
  }

  auto new_data = std::make_unique<std::stringstream>();
  pc.sync_to_stream(*new_data);
  pc.replace_istream(std::move(new_data));

  for (unsigned long i = sz; i > 0; i--) {
    auto &k2tree = pc.fetch_k2tree(i);
    ASSERT_TRUE(k2tree.has(i, i));
    ASSERT_FALSE(k2tree.has(i, i + 1));
  }

  for (unsigned long i = 20'000; i < 30'000; i++) {
    auto &k2tree = pc.fetch_k2tree(i);
    ASSERT_TRUE(k2tree.has(i, i));
    ASSERT_FALSE(k2tree.has(i, i + 1));
  }

}
