//
// Created by Cristobal Miranda, 2020
//
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <memory>
#include <string>

#include <exception>

#include <PredicatesIndexCache.hpp>

struct parsed_options {
  std::string input_file;
  std::string output_file;
};

parsed_options parse_cmline(int argc, char **argv);

void print_help();

int main(int argc, char **argv) {
  parsed_options parsed = parse_cmline(argc, argv);

  PredicatesIndexCache pic;
  pic.load_dump_file(parsed.input_file);
  std::cout << "loaded dump file" << std::endl;
  std::ofstream ofs(parsed.output_file);

  auto predicates_indexes = pic.get_predicates_ids();
  for (auto id : predicates_indexes) {
    auto &k2tree = pic.get_k2tree(id);
    auto scanned_points = k2tree.scan_points();
    for (auto &scanned_point : scanned_points) {
      ofs << scanned_point.first << "," << id << "," << scanned_point.second
          << "\n";
    }
  }
  ofs.close();

  return 0;
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "f:o:";
  struct option long_options[] = {
      {"input-file", required_argument, nullptr, 'f'},
      {"output-file", required_argument, nullptr, 'o'}};

  int opt, opt_index;

  bool has_input = false;
  bool has_output = false;

  parsed_options out{};

  std::string given_type;
  while ((
      opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }

    switch (opt) {
    case 'f':
      out.input_file = optarg;
      has_input = true;
      break;
    case 'o':
      out.output_file = optarg;
      has_output = true;
      break;
    case 'h': // to implement
    case '?':
    default:
      print_help();
      break;
    }
  }

  if (!has_input) {
    std::cerr << "Missing option --input-data\n" << std::endl;
    print_help();
    exit(1);
  }

  if (!has_output) {
    std::cerr << "Missing option --output-data\n" << std::endl;
    print_help();
    exit(1);
  }

  return out;
}

void print_help() {
  std::cout << "--input-file\t(-f)\t\t(string-required)\n"
            << "--output-file\t(-o)\t\t(string-required)\n"
            << std::endl;
}
