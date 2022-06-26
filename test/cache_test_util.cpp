
#include "cache_test_util.hpp"
#include "mock_structures/FHMock.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "builder/PredicatesIndexFileBuilder.hpp"
#include "k2tree/K2TreeMixed.hpp"
#include <serialization_util.hpp>

namespace k2cache {

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

void build_cache_test_file(const std::string &fname) {
  build_cache_test_file(fname, {});
}

UpdatesLoggerFilesManager mock_fh_manager() {
  std::string data;
  std::string data_offsets;
  std::string metadata;
  auto fh = std::make_unique<FHMock>(data);
  auto fh_offsets = std::make_unique<FHMock>(data_offsets);
  auto fh_metadata = std::make_unique<FHMock>(metadata);
  return {std::move(fh), std::move(fh_offsets), std::move(fh_metadata)};
}

PredicatesCacheManager basic_pcm() {
  K2TreeConfig config;
  config.treedepth = 32;
  config.cut_depth = 10;
  config.max_node_count = 256;

  std::unique_ptr<I_FileRWHandler> fh_pcm{};
  {
    std::string cache_data;
    fh_pcm = std::make_unique<FHMock>(cache_data);
    auto fh_writer = fh_pcm->get_writer(std::ios::out | std::ios::binary);
    PredicatesCacheMetadata metadata_pcm(config);
    metadata_pcm.write_to_ostream(fh_writer->get_ostream());
    fh_writer->flush();
  }
  return {std::move(fh_pcm), mock_fh_manager()};
}
} // namespace k2cache
