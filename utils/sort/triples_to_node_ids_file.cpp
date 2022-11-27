//
// Created by cristobal on 25-11-22.
//

#include "triples_to_node_ids_file.hpp"
#include "ULConnectorWHeaderCustomSerialization.hpp"
#include "ULHeaderIOHandlerCustomSerialization.hpp"
#include "external_sort.hpp"
#include "triple_external_sort.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace k2cache{

int triples_to_sorted_node_ids(const TriplesToNodeIdsSortParams &params){
  std::cout << "processing " << params.input_file << " ..." << std::endl;
  if (!fs::exists(params.input_file)) {
    std::cerr << "file " << params.input_file << " doesn't exist" << std::endl;
    return 1;
  }

  std::cout << "output file: " << params.output_file << " ..." << std::endl;
  std::cout << "workers: " << params.workers << std::endl;
  std::cout << "memory-budget: " << params.memory_budget << std::endl;

  std::ifstream ifs(params.input_file, std::ios::in | std::ios::binary);
  std::ofstream ofs(params.output_file,
                    std::ios::out | std::ios::binary | std::ios::trunc);
  FileData filedata{};
  filedata.size = read_u64(ifs);
  filedata.current_triple = 0;
  uint64_t total_nodes = filedata.size * 3;
  write_u64(ofs, total_nodes);
  while (!filedata.finished()) {
    auto triple = filedata.read_triple(ifs);
    // ofs << triple.first << "," << triple.second << "," << triple.third <<
    // "\n";
    write_u64(ofs, triple.first);
    write_u64(ofs, triple.second);
    write_u64(ofs, triple.third);
  }
  ifs.close();
  ofs.close();

  auto base_path = fs::path(params.output_file).parent_path();
  auto dir_path = base_path / fs::path("tmpdir");

  bool existed = true;
  if (!fs::exists(dir_path)) {
    std::cout << "creating directory " << dir_path << std::endl;
    fs::create_directory(dir_path);
    existed = false;
  } else {
    std::cout << "using existing directory " << dir_path << std::endl;
  }

  auto tmp_sorted = params.output_file + "-sorted";

  std::cout << "starting external sort..." << std::endl;

  ExternalSort::ExternalSort<
      ULConnectorWHeaderCustomSerialization, ExternalSort::DATA_MODE::BINARY,
      ExternalSort::NoTimeControl,
      ULHeaderIOHandlerCustomSerialization>::sort(params.output_file,
                                                  tmp_sorted, dir_path.string(),
                                                  params.workers, 10,
                                                  params.memory_budget, 4096,
                                                  true);
  std::cout << "external sort done" << std::endl;
  fs::remove(params.output_file);
  fs::rename(tmp_sorted, params.output_file);

  if (!existed) {
    fs::remove_all(dir_path);
  }
  return 0;
}
}
