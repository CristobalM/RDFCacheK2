//
// Created by cristobal on 25-11-22.
//
#include "FileIStream.hpp"
#include "UuidGenerator.hpp"
#include "external_sort.hpp"
#include "fisher_yates.hpp"
#include "nodeids/NodesSequence.hpp"
#include "sort/triples_to_node_ids_file.hpp"
#include <filesystem>
#include <gtest/gtest.h>
#include <serialization_util.hpp>

namespace fs = std::filesystem;

static void
write_triples_dataset_to_file(const std::string &filename,
                              const std::vector<unsigned long> &dataset) {
  std::ofstream ofs(filename, std::ios::binary | std::ios::out);
  k2cache::write_u64(ofs, dataset.size());
  for (auto v : dataset) {
    k2cache::write_u64(ofs, v);
    k2cache::write_u64(ofs, v);
    k2cache::write_u64(ofs, v);
  }
}

static void do_checks(const std::vector<unsigned long> &dataset,
                      k2cache::NodesSequence &nseq) {

  long last_read = std::numeric_limits<long>::lowest();
  for (auto i = 0UL; i < dataset.size(); i++) {
    auto nodeid = nseq.get_real_id(i);
    ASSERT_LT(last_read, nodeid)
        << "the sequence failed the increasing check at i = " << i
        << " -- with last_read = " << last_read << " and nodeid = " << nodeid;
    auto pos_mapped = nseq.get_id(nodeid);
    ASSERT_EQ(i, pos_mapped)
        << "i doesn't coincide with pos_mapped, at i = " << i
        << ", pos_mapped = " << pos_mapped << ", nodeid = " << nodeid;
  }
}

// Warning: this does disk I/O
TEST(nodeids_build_flow_test, can_sort_correctly_on_int32_range) {

  auto input_id = ExternalSort::generate_uuid_v4();
  auto output_id = ExternalSort::generate_uuid_v4();

  auto dataset = k2cache::fisher_yates(100000, 1UL << 31UL);
  std::cout << "fisher yates set created" << std::endl;
  write_triples_dataset_to_file(input_id, dataset);

  std::cout << "done writing triples dataset to file" << std::endl;

  constexpr auto workers = 1;
  constexpr auto mem_budget = 10'000'000UL; // 10 MB

  k2cache::TriplesToNodeIdsSortParams params{};
  params.input_file = input_id;
  params.output_file = output_id;
  params.memory_budget = mem_budget;
  params.workers = workers;

  int err_code = triples_to_sorted_node_ids(params);
  ASSERT_EQ(0, err_code) << "err code was not 0, it was: " << err_code;

  k2cache::FileIStream fis(output_id, std::ios::binary | std::ios::in);
  auto nseq = k2cache::NodesSequence::from_input_stream(fis);

  fs::remove(input_id);
  fs::remove(output_id);

  do_checks(dataset, nseq);
}

// Warning: this does disk I/O
TEST(nodeids_build_flow_test, can_sort_correctly_out_of_int32_range) {

  auto input_id = ExternalSort::generate_uuid_v4();
  auto output_id = ExternalSort::generate_uuid_v4();

  auto dataset = k2cache::fisher_yates(100000, 1UL << 31UL);
  // get out of i32 range
  for (auto i = 0UL; i < dataset.size() / 2; i++) {
    dataset[i] += 1UL << 32UL;
  }

  std::cout << "fisher yates set created" << std::endl;
  write_triples_dataset_to_file(input_id, dataset);

  std::cout << "done writing triples dataset to file" << std::endl;

  constexpr auto workers = 1;
  constexpr auto mem_budget = 10'000'000UL; // 10 MB

  k2cache::TriplesToNodeIdsSortParams params{};
  params.input_file = input_id;
  params.output_file = output_id;
  params.memory_budget = mem_budget;
  params.workers = workers;

  int err_code = triples_to_sorted_node_ids(params);
  ASSERT_EQ(0, err_code) << "err code was not 0, it was: " << err_code;

  k2cache::FileIStream fis(output_id, std::ios::binary | std::ios::in);
  auto nseq = k2cache::NodesSequence::from_input_stream(fis);

  fs::remove(input_id);
  fs::remove(output_id);

  do_checks(dataset, nseq);
}

// Warning: this does disk I/O
TEST(nodeids_build_flow_test, can_sort_correctly_out_of_int63_range) {

  auto input_id = ExternalSort::generate_uuid_v4();
  auto output_id = ExternalSort::generate_uuid_v4();

  auto dataset = k2cache::fisher_yates(100000, 1UL << 31UL);
  // get out of i32 range
  for (auto i = 0UL; i < dataset.size() / 2; i++) {
    dataset[i] += 1UL << 63UL;
  }

  std::cout << "fisher yates set created" << std::endl;
  write_triples_dataset_to_file(input_id, dataset);

  std::cout << "done writing triples dataset to file" << std::endl;

  constexpr auto workers = 1;
  constexpr auto mem_budget = 10'000'000UL; // 10 MB

  k2cache::TriplesToNodeIdsSortParams params{};
  params.input_file = input_id;
  params.output_file = output_id;
  params.memory_budget = mem_budget;
  params.workers = workers;

  int err_code = triples_to_sorted_node_ids(params);
  ASSERT_EQ(0, err_code) << "err code was not 0, it was: " << err_code;

  k2cache::FileIStream fis(output_id, std::ios::binary | std::ios::in);
  auto nseq = k2cache::NodesSequence::from_input_stream(fis);

  fs::remove(input_id);
  fs::remove(output_id);

  std::cout << "dataset size: " << dataset.size() << std::endl;
  std::cout << "last assigned: " << nseq.get_last_assigned() << std::endl;
  //  std::cout << "dataset is\n";
  //  for(auto v : dataset){
  //    std::cout << v << ", ";
  //  }
  //  std::cout << std::endl;
  //  std::cout << "nodes sequence is;\n";
  //  for(auto i = 0L; i <= nseq.get_last_assigned(); i++){
  //    auto real_id = nseq.get_real_id(i);
  //    std::cout << (unsigned long)real_id << ", ";
  //  }
  //  std::cout << std::endl;

  do_checks(dataset, nseq);
}
