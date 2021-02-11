
#include "cache_test_util.hpp"

#include <filesystem>
#include <fstream>

#include <K2TreeMixed.hpp>
#include <PredicatesIndexFileBuilder.hpp>
#include <serialization_util.hpp>

void build_cache_test_file(const std::string &fname,
                           std::vector<TripleValue> &data) {
  std::string plain_predicates_fname = fname + "plain_predicates_file.bin";
  {
    std::ofstream ofs_plain(plain_predicates_fname,
                            std::ios::out | std::ios::binary | std::ios::trunc);

    write_u64(ofs_plain, data.size());
    for (auto &tvalue : data) {
      tvalue.write_to_file(ofs_plain);
    }
  }

  K2TreeConfig config;
  config.cut_depth = 10;
  config.max_node_count = 256;
  config.treedepth = 32;

  std::ifstream ifs_plain(plain_predicates_fname,
                          std::ios::in | std::ios::binary);

  std::string index_fname_tmp = fname + ".tmp";

  {
    std::ofstream ofs_index(fname,
                            std::ios::out | std::ios::binary | std::ios::trunc);
    std::fstream ofs_index_tmp(index_fname_tmp, std::ios::in | std::ios::out |
                                                    std::ios::binary |
                                                    std::ios::trunc);

    PredicatesIndexFileBuilder::build(ifs_plain, ofs_index, ofs_index_tmp,
                                      config);
  }

  std::filesystem::remove(plain_predicates_fname);
  std::filesystem::remove(index_fname_tmp);
}

void build_cache_test_file(const std::string &fname,
                           std::vector<TripleValue> &&data) {
  build_cache_test_file(fname, data);
}

std::vector<TripleValue> build_initial_values_triples_vector(uint64_t size) {
  std::vector<TripleValue> result;

  for (uint64_t i = 1; i <= size; i++) {
    result.emplace_back(i, i, i);
  }

  return result;
}
