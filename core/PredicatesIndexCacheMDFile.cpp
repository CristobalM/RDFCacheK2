#include <filesystem>
#include <memory>
#include <fstream>

#include "PredicatesIndexCacheMDFile.hpp"

namespace fs = std::filesystem;


static std::unique_ptr<std::istream> load_file(const std::string &fname){
  if(!fs::exists(fs::path(fname))){
    throw std::runtime_error("File " + fname + " doesn't exist");
  }
  return std::make_unique<std::ifstream>(fname, std::ios::in | std::ios::binary);
}

PredicatesIndexCacheMDFile::PredicatesIndexCacheMDFile(
    const std::string &fname, K2TreeConfig k2tree_config)
    : PredicatesIndexCacheMD(load_file(fname),
                             k2tree_config),
      fname(fname) {}

PredicatesIndexCacheMDFile::PredicatesIndexCacheMDFile(
    PredicatesIndexCacheMDFile &&other) noexcept
    : PredicatesIndexCacheMD(std::move(other)) {}

bool PredicatesIndexCacheMDFile::load_single_predicate(
    uint64_t predicate_index) {
  return PredicatesIndexCacheMD::load_single_predicate(predicate_index);
}

K2TreeMixed &
PredicatesIndexCacheMDFile::fetch_k2tree(uint64_t predicate_index) {
  return PredicatesIndexCacheMD::fetch_k2tree(predicate_index);
}

bool PredicatesIndexCacheMDFile::has_predicate(uint64_t predicate_index) {
  return PredicatesIndexCacheMD::has_predicate(predicate_index);
}

bool PredicatesIndexCacheMDFile::has_predicate_active(
    uint64_t predicate_index) {
  return PredicatesIndexCacheMD::has_predicate_active(predicate_index);
}

bool PredicatesIndexCacheMDFile::has_predicate_stored(
    uint64_t predicate_index) {
  return PredicatesIndexCacheMD::has_predicate_stored(predicate_index);
}
void PredicatesIndexCacheMDFile::add_predicate(uint64_t predicate_index) {
  return PredicatesIndexCacheMD::add_predicate(predicate_index);
}

void PredicatesIndexCacheMDFile::insert_point(uint64_t subject_index,
                                              uint64_t predicate_index,
                                              uint64_t object_index) {
  return PredicatesIndexCacheMD::insert_point(subject_index, predicate_index,
                                              object_index);
}

void PredicatesIndexCacheMDFile::sync_file() {
  auto temp_fname = fname + "_tmp";
  {
    std::ofstream ofs(temp_fname,
                      std::ios::binary | std::ios::out | std::ios::trunc);
    PredicatesIndexCacheMD::sync_to_stream(ofs);
  }

  this->is = nullptr;

  std::filesystem::remove(fname);
  std::filesystem::rename(temp_fname, fname);
  this->is =
      std::make_unique<std::ifstream>(fname, std::ios::binary | std::ios::in);
}

void PredicatesIndexCacheMDFile::discard_in_memory_predicate(uint64_t predicate_index) {
  PredicatesIndexCacheMD::discard_in_memory_predicate(predicate_index);
}
