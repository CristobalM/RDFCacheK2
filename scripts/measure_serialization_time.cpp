//
// Created by cristobal on 29-08-21.
//

#include "manager/PCMFactory.hpp"
#include <chrono>
#include <filesystem>
#include <getopt.h>
#include <iostream>
#include <string>

using namespace k2cache;

struct parsed_options {
  std::string k2trees_file;
  std::string output_file;
  std::string tmp_serialization_file;
};

parsed_options parse_cmd_line(int argc, char **argv);

void print_help();

int main(int argc, char **argv) {
  parsed_options parsed = parse_cmd_line(argc, argv);

  auto pcm = PCMFactory::create(parsed.k2trees_file);

  auto &predicates_index = pcm->get_predicates_index_cache();
  const auto &predicates_ids = predicates_index.get_predicates_ids();

  std::ofstream ofs(parsed.output_file, std::ios::out | std::ios::trunc);
  std::ofstream ofs_tmp(parsed.tmp_serialization_file,
                        std::ios::out | std::ios::binary | std::ios::trunc);

  ofs << "#,"
      << "serialization_time_microsecs"
      << "\n";

  for (auto predicate_id : predicates_ids) {
    auto fetch_result = predicates_index.fetch_k2tree(predicate_id);
    const auto &k2tree = fetch_result.get();

    auto start = std::chrono::high_resolution_clock::now();
    k2tree.write_to_ostream(ofs_tmp);
    auto end = std::chrono::high_resolution_clock::now();
    ofs_tmp.seekp(0);
    predicates_index.discard_in_memory_predicate(predicate_id);
    auto duration_ms =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    ofs << predicate_id << "," << duration_ms.count() << "\n";
  }
  ofs_tmp.close();
  std::filesystem::remove(parsed.tmp_serialization_file);

  return 0;
}

parsed_options parse_cmd_line(int argc, char **argv) {
  const char short_options[] = "k:o:T:";
  struct option long_options[] = {
      {"k2trees-file", required_argument, nullptr, 'k'},
      {"output-file", required_argument, nullptr, 'o'},
      {"tmp-serialization-file", required_argument, nullptr, 'T'},
  };

  int opt, opt_index;

  bool has_k2trees = false;
  bool has_output_file = false;
  bool has_tmp_file = false;

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
    case 'o':
      out.output_file = optarg;
      has_output_file = true;
      break;
    case 'T':
      out.tmp_serialization_file = optarg;
      has_tmp_file = true;
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

  if (!has_output_file) {
    std::cerr << "Missing option --output-file\n" << std::endl;
    print_help();
    exit(1);
  }
  if (!has_tmp_file) {
    std::cerr << "Missing option --tmp-serialization-file\n" << std::endl;
    print_help();
    exit(1);
  }

  return out;
}

void print_help() {
  std::cout << "--iris-file\t(-i)\t\t(string-required)\n"
            << "--blanks-file\t(-b)\t\t(string-required)\n"
            << "--literals-file\t(-l)\t\t(string-required)\n"
            << "--k2trees-file\t(-k)\t\t(string-required)\n"
            << "--output-file\t(-o)\t\t(string-required)\n"
            << "--tmp-serialization-file\t(-T)\t\t(string-required)\n"
            << std::endl;
}
