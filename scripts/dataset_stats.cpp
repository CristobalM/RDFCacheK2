//
// Created by Cristobal Miranda, 2020
//
#include <chrono>
#include <getopt.h>
#include <iostream>
#include <manager/PCMFactory.hpp>
#include <memory>
#include <string>

#include "nodeids/NodeIdsManagerFactory.hpp"
#include <FileRWHandler.hpp>
#include <exception>

using namespace k2cache;

struct parsed_options {
  std::string k2trees_file;
  std::string nodeids_file;
  std::string mapped_nodeids_file;
  std::string mapped_nodeids_log_file;
  std::string output_file;
};

parsed_options parse_cmd_line(int argc, char **argv);

void print_help();

int main(int argc, char **argv) {
  parsed_options parsed = parse_cmd_line(argc, argv);

  auto frw_handler = std::make_unique<FileRWHandler>(parsed.k2trees_file);
  auto pcm = PCMFactory::create(std::move(frw_handler));
  auto &predicates_index = pcm->get_predicates_index_cache();
  const auto &predicates_ids = predicates_index.get_predicates_ids();

  std::ofstream ofs(parsed.output_file, std::ios::out | std::ios::trunc);
  using namespace k2cache;

  CacheArgs args;
  args.node_ids_filename = parsed.nodeids_file;
  args.mapped_node_ids_filename = parsed.mapped_nodeids_file;
  args.node_ids_logs_filename = parsed.mapped_nodeids_log_file;
  auto nim = NodeIdsManagerFactory::create(args);

  uint64_t total_points = 0;

  ofs << "#,"
      << "nodeid,"
      << "points,"
      << "total_block_nodes,"
      << "bytes_topology,"
      << "total_bytes,"
      << "serialized_bytes,"
      << "size_in_memory_stored,"
      << "retrieval_time_microsecs"
      << "\n";

  const auto &metadata = predicates_index.get_metadata();
  const auto &metadata_map = metadata.get_map();

  for (auto predicate_id : predicates_ids) {
    auto start = std::chrono::high_resolution_clock::now();
    auto fetch_result = predicates_index.fetch_k2tree(predicate_id);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ms =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    const auto &predicate_metadata = metadata_map.at(predicate_id);
    auto serialized_bytes = predicate_metadata.tree_size;
    auto size_in_memory = predicate_metadata.tree_size_in_memory;

    const auto &k2tree = fetch_result.get();
    auto stats = k2tree.k2tree_stats();
    size_t tree_number_of_points = k2tree.size();
    total_points += tree_number_of_points;
    auto nodeid = nim->get_real_id(predicate_id, nullptr);
    ofs << predicate_id << ","
        << nodeid << ","
        << tree_number_of_points << ","
        << stats.total_blocks << "," << stats.bytes_topology << ","
        << stats.total_bytes << "," << serialized_bytes << "," << size_in_memory
        << "," << duration_ms.count() << "\n";

    predicates_index.discard_in_memory_predicate(predicate_id);
  }

  std::cout << "Total triples: " << total_points << "\n" << std::endl;

  return 0;
}

parsed_options parse_cmd_line(int argc, char **argv) {
  const char short_options[] = "k:n:m:l:o:";
  struct option long_options[] = {
      {"k2trees-file", required_argument, nullptr, 'k'},
      {"nodeids-file", required_argument, nullptr, 'n'},
      {"mapped-nodeids-file", required_argument, nullptr, 'm'},
      {"mapped-nodeids-log-file", required_argument, nullptr, 'l'},
      {"output-file", required_argument, nullptr, 'o'},
  };

  int opt, opt_index;

  bool has_k2trees = false;
  bool has_nodeids = false;
  bool has_mapped_nodeids = false;
  bool has_mapped_nodeids_log = false;
  bool has_output_file = false;

  parsed_options out{};

  std::string given_type;
  while ((
      opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }

    switch (opt) {
    case 'k':
      out.k2trees_file = optarg;
      has_k2trees = true;
      break;
    case 'n':
      out.nodeids_file = optarg;
      has_nodeids = true;
      break;
    case 'm':
      out.mapped_nodeids_file = optarg;
      has_mapped_nodeids = true;
      break;
    case 'l':
      out.mapped_nodeids_log_file = optarg;
      has_mapped_nodeids_log = true;
      break;
    case 'o':
      out.output_file = optarg;
      has_output_file = true;
      break;
    case 'h': // to implement
    case '?':
    default:
      print_help();
      break;
    }
  }

  if (!has_k2trees) {
    std::cerr << "Missing option --k2trees-file\n" << std::endl;
    print_help();
    exit(1);
  }
  if (!has_nodeids) {
    std::cerr << "Missing option --nodeids-file\n" << std::endl;
    print_help();
    exit(1);
  }
  if (!has_mapped_nodeids) {
    std::cerr << "Missing option --mapped-nodeids-file\n" << std::endl;
    print_help();
    exit(1);
  }

  if (!has_mapped_nodeids_log) {
    std::cerr << "Missing option --mapped-nodeids-log-file\n" << std::endl;
    print_help();
    exit(1);
  }

  if (!has_output_file) {
    std::cerr << "Missing option --output-file\n" << std::endl;
    print_help();
    exit(1);
  }

  return out;
}

void print_help() {
  std::cout
            << "--k2trees-file\t(-k)\t\t(string-required)\n"
            << "--nodeids-file\t(-n)\t\t(string-required)\n"
            << "--mapped-nodeids-file\t(-m)\t\t(string-required)\n"
            << "--mapped-nodeids-log-file\t(-l)\t\t(string-required)\n"
            << "--output-file\t(-o)\t\t(string-required)\n"
            << std::endl;
}
