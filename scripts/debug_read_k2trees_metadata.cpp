#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <string>

#include "manager/PredicatesIndexCacheMD.hpp"
#include <FileRWHandler.hpp>

namespace fs = std::filesystem;
using namespace k2cache;

struct parsed_options {
  std::string input_file;
};

parsed_options parse_cmline(int argc, char **argv);

int main(int argc, char **argv) {
  auto parsed = parse_cmline(argc, argv);

  if (!fs::exists(parsed.input_file)) {
    throw std::runtime_error("Not found file " + parsed.input_file);
  }

  auto frw_handler = std::make_unique<FileRWHandler>(parsed.input_file);
  PredicatesIndexCacheMD predicates_manager(std::move(frw_handler));

  // auto &metadata = predicates_manager.get_metadata();

  unsigned long total_points = 0;
  for (auto predicate_id : predicates_manager.get_predicates_ids()) {
    auto fetch_result = predicates_manager.fetch_k2tree(predicate_id);
    total_points += fetch_result.get().size();
  }

  std::cout << "total points: " << total_points << std::endl;

  return 0;
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "i:";
  struct option long_options[] = {
      {"input-file", required_argument, nullptr, 'i'},
  };

  int opt, opt_index;

  bool has_input = false;
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
    default:
      break;
    }
  }

  if (!has_input)
    throw std::runtime_error("input-file (i) argument is required");

  return out;
}