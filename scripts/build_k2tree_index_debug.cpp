#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <stdexcept>
#include <string>

#include "builder/PredicatesIndexFileBuilder.hpp"
#include "k2tree/K2TreeMixed.hpp"
#include <iostream>

namespace fs = std::filesystem;
using namespace k2cache;

struct parsed_options {
  std::string input_file;
  std::string output_file;
  uint32_t max_node_count;
  uint32_t cut_depth;
  uint32_t tree_depth;
  bool exhaustive_check;
};

parsed_options parse_cmline(int argc, char **argv);

int main(int argc, char **argv) {
  auto parsed = parse_cmline(argc, argv);

  if (!fs::exists(parsed.input_file)) {
    throw std::runtime_error("Not found file " + parsed.input_file);
  }

  K2TreeConfig config{};
  config.max_node_count = parsed.max_node_count;
  config.cut_depth = parsed.cut_depth;
  config.treedepth = parsed.tree_depth;

  std::cout << "max_node_count: " << config.max_node_count << "\n"
            << "cut_depth: " << config.cut_depth << "\n"
            << "treedepth: " << config.treedepth << "\n"
            << "exhaustive check: " << (parsed.exhaustive_check ? "YES" : "NO")
            << std::endl;

  auto tmp_fname = parsed.output_file + ".tmp";
  std::ifstream ifs(parsed.input_file, std::ios::in | std::ios::binary);
  std::ofstream ofs(parsed.output_file,
                    std::ios::out | std::ios::binary | std::ios::trunc);
  std::fstream tmp_fs(tmp_fname, std::ios::in | std::ios::out |
                                     std::ios::binary | std::ios::trunc);

  PredicatesIndexFileBuilder::build_debug(ifs, ofs, tmp_fs, config,
                                          parsed.exhaustive_check);

  fs::remove(tmp_fname);

  return 0;
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "i:o:N:C:T:E";
  struct option long_options[] = {
      {"input-file", required_argument, nullptr, 'i'},
      {"output-file", required_argument, nullptr, 'o'},
      {"max-node-count", required_argument, nullptr, 'N'},
      {"cut-depth", required_argument, nullptr, 'C'},
      {"tree-depth", required_argument, nullptr, 'T'},
      {"exhaustive-check", optional_argument, nullptr, 'E'},
  };

  int opt, opt_index;

  bool has_input = false;
  bool has_output = false;
  bool has_max_node_count = false;
  bool has_cut_depth = false;
  bool has_tree_depth = false;
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
    case 'N':
      out.max_node_count = std::stoi(std::string(optarg));
      has_max_node_count = true;
      break;
    case 'C':
      out.cut_depth = std::stoi(std::string(optarg));
      has_cut_depth = true;
      break;
    case 'T':
      out.tree_depth = std::stoi(std::string(optarg));
      has_tree_depth = true;
      break;
    case 'E':
      out.exhaustive_check = true;
      break;
    default:
      break;
    }
  }

  if (!has_input)
    throw std::runtime_error("input-file (i) argument is required");
  if (!has_output)
    throw std::runtime_error("output-file (o) argument is required");
  if (!has_max_node_count)
    throw std::runtime_error("max-node-count (N) argument is required");
  if (!has_cut_depth)
    throw std::runtime_error("cut-depth (C) argument is required");
  if (!has_tree_depth)
    throw std::runtime_error("tree-depth (T) argument is required");

  return out;
}