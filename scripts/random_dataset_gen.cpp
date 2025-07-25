//
// Created by Cristobal Miranda on 07-01-23.
//

#include <getopt.h>

#include <filesystem>
#include <iostream>
#include <random>
#include <string>

#include "builder/K2TreesFeed.hpp"
#include "builder/PredicatesIndexFileBuilder.hpp"
#include "builder/TriplesFeedSortedByPredicate.hpp"
#include "k2tree/K2TreeBulkOp.hpp"
#include "k2tree/K2TreeMixed.hpp"
#include "util_algorithms/fisher_yates.hpp"
#include "nodeids/NodesSequence.hpp"
#include "FileOStream.hpp"
#include "FileIOStream.hpp"
#include "util_algorithms/random_dataset_generation.hpp"

struct parsed_options {
  uint64_t triples_num;
  uint64_t resources_num;
  std::string dataset_name;
};

parsed_options parse_cmd_line(int argc, char **argv);

using namespace k2cache;

namespace fs = std::filesystem;

int main(int argc, char **argv) {
  auto opts = parse_cmd_line(argc, argv);

  if (opts.resources_num <= 0 || opts.resources_num >= (1ULL << 32ULL)) {
    std::cerr << "Invalid number of resources num: " << opts.resources_num;
    return 1;
  }

  if (opts.triples_num <= 0) {
    std::cerr << "Invalid number of triples num: " << opts.triples_num;
    return 1;
  }

  K2TreeConfig config{};
  config.max_node_count = 128;
  config.cut_depth = 10;
  config.treedepth = 32;

  const auto tmp_filename = opts.dataset_name + ".tmp";
  const auto node_ids_file = opts.dataset_name + ".nodeids.bin";


  FileOStream ofs(opts.dataset_name, std::ios::binary);
  FileIOStream fs_tmp(tmp_filename, std::ios::binary | std::ios::trunc);
  FileOStream nodeids_ofs(node_ids_file, std::ios::binary);

  generate_random_dataset(config, opts.triples_num, opts.resources_num, ofs, fs_tmp, nodeids_ofs);

  fs::remove(tmp_filename);

  return 0;
}

parsed_options parse_cmd_line(int argc, char **argv) {
  const char short_options[] = "t:r:d:";
  struct option long_options[] = {
      {"triples-num", required_argument, nullptr, 't'},
      {"resources-num", required_argument, nullptr, 'r'},
      {"dataset-name", required_argument, nullptr, 'd'},
  };

  int opt, opt_index;

  bool has_triples_num = false;
  bool has_resources_num = false;
  bool has_dataset_name = false;

  parsed_options out{};

  while ((
      opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }
    switch (opt) {
    case 't':
      out.triples_num = std::stoull(optarg);
      has_triples_num = true;
      break;
    case 'r':
      out.resources_num = std::stoull(optarg);
      has_resources_num = true;
      break;
    case 'd':
      out.dataset_name = optarg;
      has_dataset_name = true;
      break;
    default:
      break;
    }
  }

  if (!has_triples_num)
    throw std::runtime_error("triples-num (t) argument is required");
  if (!has_resources_num)
    throw std::runtime_error("resources-num (r) argument is required");
  if (!has_dataset_name)
    throw std::runtime_error("dataset-name (d) argument is required");

  return out;
}