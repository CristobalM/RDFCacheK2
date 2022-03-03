#include <portable_fs.h>
#include <stdexcept>
#include <string>

#include <external_sort.hpp>
#include <getopt.h>
#include <iostream>
#include <serialization_util.hpp>
#include <triple_external_sort.hpp>

#include "ULConnectorWHeaderCustomSerialization.hpp"
#include "ULHeaderIOHandlerCustomSerialization.hpp"

struct parsed_options {
  std::string input_file;
  std::string output_file;
  int workers;
  unsigned long memory_budget;
};

parsed_options parse_cmline(int argc, char **argv);

namespace fs = std::filesystem;

int main(int argc, char **argv) {
  auto parsed = parse_cmline(argc, argv);

  std::cout << "processing " << parsed.input_file << " ..." << std::endl;
  if (!fs::exists(parsed.input_file)) {
    std::cerr << "file " << parsed.input_file << " doesn't exist" << std::endl;
    return 1;
  }

  std::cout << "output file: " << parsed.output_file << " ..." << std::endl;
  std::cout << "workers: " << parsed.workers << std::endl;
  std::cout << "memory-budget: " << parsed.memory_budget << std::endl;

  std::ifstream ifs(parsed.input_file, std::ios::in | std::ios::binary);
  std::ofstream ofs(parsed.output_file,
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

  auto base_path = fs::path(parsed.output_file).parent_path();
  auto dir_path = base_path / fs::path("tmpdir");

  bool existed = true;
  if (!fs::exists(dir_path)) {
    std::cout << "creating directory " << dir_path << std::endl;
    fs::create_directory(dir_path);
    existed = false;
  } else {
    std::cout << "using existing directory " << dir_path << std::endl;
  }

  auto tmp_sorted = parsed.output_file + "-sorted";

  std::cout << "starting external sort..." << std::endl;

  ExternalSort::ExternalSort<
      ULConnectorWHeaderCustomSerialization, ExternalSort::DATA_MODE::BINARY,
      ExternalSort::NoTimeControl,
      ULHeaderIOHandlerCustomSerialization>::sort(parsed.output_file,
                                                  tmp_sorted, dir_path.string(),
                                                  parsed.workers, 10,
                                                  parsed.memory_budget, 4096,
                                                  true);
  std::cout << "external sort done" << std::endl;
  fs::remove(parsed.output_file);
  fs::rename(tmp_sorted, parsed.output_file);

  if (!existed) {
    fs::remove_all(dir_path);
  }

  return 0;
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "i:o:w:m:";
  struct option long_options[] = {
      {"input-file", required_argument, nullptr, 'i'},
      {"output-file", required_argument, nullptr, 'o'},
      {"workers", required_argument, nullptr, 'w'},
      {"memory-budget", required_argument, nullptr, 'm'},
  };

  int opt, opt_index;

  bool has_input = false;
  bool has_output = false;
  bool has_workers = false;
  bool has_memory_budget = false;
  parsed_options out{};

  while ((
      opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }
    switch (opt) {
    case 'i':
      out.input_file = optarg;
      has_input = true;
      break;
    case 'o':
      out.output_file = optarg;
      has_output = true;
      break;
    case 'w':
      out.workers = std::stoi(optarg);
      has_workers = true;
      break;
    case 'm':
      out.memory_budget = std::stoul(optarg);
      has_memory_budget = true;
      break;
    default:
      break;
    }
  }

  if (!has_input)
    throw std::runtime_error("input-file (i) argument is required");
  if (!has_output)
    throw std::runtime_error("output-file (o) argument is required");

  if (!has_workers)
    throw std::runtime_error("workers (w) argument is required");
  if (!has_memory_budget)
    throw std::runtime_error("memory-budget (m) argument is required");

  return out;
}