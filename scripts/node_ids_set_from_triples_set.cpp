#include <stdexcept>
#include <string>

#include <getopt.h>
#include <triple_external_sort.hpp>

#include "sort/triples_to_node_ids_file.hpp"

using namespace k2cache;

TriplesToNodeIdsSortParams parse_cmd_line(int argc, char **argv);

namespace fs = std::filesystem;

int main(int argc, char **argv) {
  auto parsed = parse_cmd_line(argc, argv);
  return triples_to_sorted_node_ids(parsed);
}

TriplesToNodeIdsSortParams parse_cmd_line(int argc, char **argv) {
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
  TriplesToNodeIdsSortParams out{};

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