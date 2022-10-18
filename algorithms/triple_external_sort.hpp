#ifndef _TRIPLE_EXTERNAL_SORT_HPP_
#define _TRIPLE_EXTERNAL_SORT_HPP_

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <istream>
#include <list>
#include <memory>
#include <ostream>
#include <queue>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#include "ParallelWorker.hpp"
#include "serialization_util.hpp"

namespace fs = std::filesystem;
namespace k2cache {
// TODO: refactor to use external-sort library and delete this file
struct TripleValue {
  uint64_t first{};
  uint64_t second{};
  uint64_t third{};

  TripleValue() = default;

  TripleValue(uint64_t first, uint64_t second, uint64_t third)
      : first(first), second(second), third(third) {}

  explicit TripleValue(std::istream &file) { read_from_file(file); }

  void read_from_file(std::istream &file) {
    first = read_u64(file);
    second = read_u64(file);
    third = read_u64(file);
  }

  void write_to_file(std::ostream &file) const {
    write_u64(file, first);
    write_u64(file, second);
    write_u64(file, third);
  }

};

struct FileData {
  uint64_t size;
  uint64_t current_triple;

  TripleValue read_triple(std::istream &file) {
    auto triple = TripleValue(file);
    current_triple++;
    return triple;
  }

  bool finished() const { return current_triple >= size; }
};

using pair_tvalue_ul = std::pair<TripleValue, unsigned long>;

template <typename Comparator> struct PairComp {
  bool operator()(const pair_tvalue_ul &lhs, const pair_tvalue_ul &rhs) {
    return !Comparator()(lhs.first, rhs.first);
  }
};

template <typename Comparator>
static void parallel_sort(std::vector<TripleValue> &data, int max_workers,
                          unsigned long segment_size, Comparator &comparator) {

  std::vector<unsigned long> offsets = {0};
  std::set<unsigned long> offsets_set;
  unsigned long acc_size = sizeof(TripleValue);
  offsets.reserve(data.size() / (segment_size / sizeof(TripleValue)) + 10);
  for (unsigned long i = 1; i < data.size(); i++) {
    acc_size += sizeof(TripleValue);
    if (acc_size >= segment_size) {
      offsets.push_back(i);
      offsets_set.insert(i);
      acc_size = 0;
    }
  }
  offsets.push_back(data.size());
  offsets_set.insert(data.size());

  unsigned long parts = offsets.size() - 1;
  unsigned long workers = std::min((unsigned long)max_workers, parts);
  if (workers == 1) {
    std::sort(data.begin(), data.end(), comparator);
    return;
  }

  ParallelWorkerPool pool(workers);

  for (unsigned long i = 0; i < parts; i++) {
    pool.add_task([i, &offsets, &data, &comparator]() {
      std::sort(data.begin() + offsets[i], data.begin() + offsets[i + 1],
                comparator);
    });
  }

  pool.stop_all_workers();
  pool.wait_workers();

  std::vector<TripleValue> result;

  std::priority_queue<pair_tvalue_ul, std::vector<pair_tvalue_ul>,
                      PairComp<Comparator>>
      pqueue;

  for (unsigned long i = 0; i < parts; i++) {
    pqueue.push({data[offsets[i]], offsets[i]});
  }

  while (!pqueue.empty()) {
    auto &current = pqueue.top();
    result.push_back(current.first);
    auto next = current.second + 1;
    pqueue.pop();
    if (offsets_set.find(next) != offsets_set.end()) {
      continue;
    }
    pqueue.push({data[next], next});
  }

  data = std::move(result);
}

template <typename Comparator>
static void create_file_part(
    const std::string &input_filename_base, const std::string &tmp_dir,
    int workers, // workers,
    std::vector<char> &buffer_out, Comparator &comparator,
    unsigned long &accumulated_size, std::vector<TripleValue> &data,
    int &current_file_index, std::vector<std::string> &filenames,
    unsigned long segment_size) {
  accumulated_size = 0;
  auto filename = (std::filesystem::path(tmp_dir) /
                   std::filesystem::path(input_filename_base + "-p" +
                                         std::to_string(current_file_index++)))
                      .string();
  std::ofstream ofs(filename, std::ios::out | std::ios::binary);
  ofs.rdbuf()->pubsetbuf(buffer_out.data(), buffer_out.size());
  filenames.push_back(filename);
  // std::sort(data.begin(), data.end(), comparator);
  parallel_sort(data, workers, segment_size, comparator);
  uint64_t size = data.size();
  write_u64(ofs, size);
  for (auto &triple : data) {
    write_u64(ofs, triple.first);
    write_u64(ofs, triple.second);
    write_u64(ofs, triple.third);
  }
  ofs.flush();
  data.clear();
}

template <typename Comparator>
static std::vector<std::string>
split_file(const std::string &input_filename, const std::string &tmp_dir,
           unsigned long memory_budget, int workers,
           std::vector<char> &buffer_in, std::vector<char> &buffer_out,
           unsigned long segment_size, Comparator &comparator) {

  std::vector<std::string> filenames;
  std::ifstream input_file(input_filename, std::ios::in | std::ios::binary);

  input_file.rdbuf()->pubsetbuf(buffer_in.data(), buffer_in.size());

  int current_file_index = 0;
  unsigned long accumulated_size = 0;

  std::vector<TripleValue> data;

  FileData filedata{};
  filedata.size = read_u64(input_file);
  filedata.current_triple = 0;

  auto max_triples_to_hold =
      std::min(memory_budget / (sizeof(TripleValue)), filedata.size);
  data.reserve(max_triples_to_hold + 10);

  // auto upper_bound_offsets = segment_size / sizeof(TripleValue) + 1;
  auto offsets_upper_bound_qty = memory_budget / segment_size + 1;
  auto offsets_size_ub = offsets_upper_bound_qty * sizeof(unsigned long);
  auto offsets_set_size_ub =
      offsets_upper_bound_qty * (sizeof(unsigned long) + sizeof(void *));

  auto extra_structs_budget = offsets_size_ub + offsets_set_size_ub;

  auto mem_budget_bound = memory_budget > extra_structs_budget
                              ? memory_budget - extra_structs_budget
                              : extra_structs_budget;

  while (!filedata.finished()) {
    auto triple = filedata.read_triple(input_file);
    if (accumulated_size >= mem_budget_bound) {
      create_file_part(input_filename, tmp_dir, workers, buffer_out, comparator,
                       accumulated_size, data, current_file_index, filenames,
                       segment_size);
    }
    data.push_back(triple);
    accumulated_size += sizeof(TripleValue);
  }
  if (accumulated_size > 0)
    create_file_part(input_filename, tmp_dir, workers, buffer_out, comparator,
                     accumulated_size, data, current_file_index, filenames,
                     segment_size);
  return filenames;
}

static inline bool fill_with_file(std::list<TripleValue> &data_block,
                                  std::unique_ptr<std::ifstream> &input_file,
                                  FileData &filedata,
                                  unsigned long block_size) {
  std::string line;
  unsigned long accumulated_size = 0;
  while (accumulated_size < block_size) {
    if (filedata.finished()) {
      input_file = nullptr;
      break;
    }
    accumulated_size += sizeof(TripleValue);
    data_block.push_back(filedata.read_triple(*input_file));
  }
  return !data_block.empty();
}

template <typename Comparator>
static void
block_update(unsigned long index, std::vector<std::list<TripleValue>> &data,
             std::vector<std::unique_ptr<std::ifstream>> &opened_files,
             std::vector<FileData> &files_data,
             std::priority_queue<pair_tvalue_ul, std::vector<pair_tvalue_ul>,
                                 PairComp<Comparator>> &pqueue,
             unsigned long block_size) {
  if (data[index].empty() && opened_files[index]) {
    if (!fill_with_file(data[index], opened_files[index], files_data[index],
                        block_size))
      return;
  } else if (data[index].empty()) {
    return;
  }
  auto current_str = data[index].front();
  pqueue.push({std::move(current_str), index});
  data[index].pop_front();
}

template <typename Comparator>
static std::string
merge_pass(const std::vector<std::string> &filenames, int start, int end,
           const std::string &tmp_dir, unsigned long block_size,
           std::vector<std::vector<char>> &buffers, Comparator &) {

  std::vector<std::unique_ptr<std::ifstream>> opened_files;

  auto result_template = (std::filesystem::path(tmp_dir) / "m_XXXXXX").string();
  auto mut_fname_template = std::vector<char>(result_template.size() + 1);
  std::copy(result_template.begin(), result_template.end(),
            mut_fname_template.data());
  mut_fname_template[result_template.size()] = '\0';
  int created = mkstemp(mut_fname_template.data());
  auto result_filename =
      std::string(mut_fname_template.begin(), mut_fname_template.end());
  if (!created)
    throw std::runtime_error("couldn't generate tmp file with name " +
                             result_filename);
  std::ofstream ofs(result_filename, std::ios::out | std::ios::binary);

  ofs.rdbuf()->pubsetbuf(buffers[buffers.size() - 1].data(),
                         buffers[buffers.size() - 1].size());

  std::priority_queue<pair_tvalue_ul, std::vector<pair_tvalue_ul>,
                      PairComp<Comparator>>
      pqueue;

  for (int i = start; i < end; i++) {
    opened_files.push_back(std::make_unique<std::ifstream>(
        filenames[i], std::ios::in | std::ios::binary));

    opened_files.back()->rdbuf()->pubsetbuf(buffers[i - start].data(),
                                            buffers[i - start].size());
  }

  std::vector<std::list<TripleValue>> data;

  std::vector<FileData> files_data;

  std::string line;
  for (auto &file : opened_files) {
    unsigned long accumulated_size = 0;

    FileData current_file_data;
    current_file_data.size = read_u64(*file);
    current_file_data.current_triple = 0;

    std::list<TripleValue> current_block;
    while (accumulated_size < block_size) {
      if (current_file_data.finished()) {
        file = nullptr;
        break;
      }
      auto triple = current_file_data.read_triple(*file);

      accumulated_size += sizeof(TripleValue);

      current_block.push_back(triple);
    }
    data.push_back(std::move(current_block));
    files_data.push_back(current_file_data);
  }

  uint64_t total_size = 0;
  for (auto &fdata : files_data) {
    total_size += fdata.size;
  }

  for (unsigned long i = 0; i < data.size(); i++) {
    block_update(i, data, opened_files, files_data, pqueue, block_size);
  }

  write_u64(ofs, total_size);

  while (!pqueue.empty()) {
    auto &current = pqueue.top();
    auto index = current.second;
    current.first.write_to_file(ofs);
    pqueue.pop();
    block_update(index, data, opened_files, files_data, pqueue, block_size);
  }

  ofs.flush();
  ofs.close();

  for (int i = start; i < end; i++) {
    remove(filenames.at(i).c_str());
  }

  return result_filename;
}

template <typename Comparator>
static std::vector<std::string> merge_bottom_up(
    const std::vector<std::string> &filenames, const std::string &tmp_dir,
    int max_files, unsigned long block_size,
    std::vector<std::vector<char>> &buffers, Comparator &comparator) {
  std::vector<std::string> result_filenames;

  int level_passes = (filenames.size() / max_files) +
                     (filenames.size() % max_files != 0 ? 1 : 0);
  for (int current_pass = 0; current_pass < level_passes; current_pass++) {
    auto pass_file = merge_pass(
        filenames, current_pass * max_files,
        std::min<int>((current_pass + 1) * max_files, filenames.size()),
        tmp_dir, block_size, buffers, comparator);
    result_filenames.push_back(pass_file);
  }

  return result_filenames;
}

static inline std::vector<std::vector<char>>
init_buffers(int max_files, unsigned long block_size) {
  std::vector<std::vector<char>> buffers;
  for (int i = 0; i < max_files + 1; i++) {
    buffers.push_back(std::vector<char>(block_size));
  }
  return buffers;
}

template <typename Comparator>
void external_sort_triples(const std::string &input_filename,
                           const std::string &output_filename,
                           const std::string &tmp_dir, int workers,
                           int max_files, unsigned long memory_budget,
                           unsigned long block_size, unsigned long segment_size,
                           Comparator comparator) {

  auto buffers = init_buffers(max_files, block_size);

  auto current_filenames =
      split_file(input_filename, tmp_dir, memory_budget, workers, buffers[0],
                 buffers[max_files], segment_size, comparator);

  while (current_filenames.size() > 1) {
    current_filenames = merge_bottom_up(current_filenames, tmp_dir, max_files,
                                        block_size, buffers, comparator);
  }
  fs::rename(current_filenames[0], output_filename);
}
} // namespace k2cache
#endif /* _TRIPLE_EXTERNAL_SORT_HPP_ */