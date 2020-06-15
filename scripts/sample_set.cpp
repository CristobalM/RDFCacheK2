//
// Created by Cristobal Miranda, 2020
//
#include <fstream>
#include <getopt.h>
#include <iostream>

#include <RadixTree.hpp>

#include <rax.h>

struct parsed_options {
  std::string input_file;
  std::string output_file;
  int samples;
};

parsed_options parse_cmline(int argc, char **argv);
void print_help();

int main(int argc, char **argv) {
  parsed_options parsed = parse_cmline(argc, argv);
  RadixTree<> radix_tree;
  std::ifstream ifs(parsed.input_file, std::ios::binary);
  radix_tree.deserialize(ifs);

  int count = 0;
  for(const auto &value: radix_tree){
    std::cout << value << "\n";
    count++;
    if(count >= parsed.samples){
      break;
    }
  }

  std::flush(std::cout);
  return 0;
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "f:s:";
  struct option long_options[] = {
      {"input-file", required_argument, nullptr, 'f'},
      {"samples", required_argument, nullptr, 's'},
  };

  int opt, opt_index;

  bool has_input = false;
  bool has_samples = false;

  parsed_options out{};
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
    case 's':
      out.samples = std::stoi(optarg);
      has_samples = true;
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

  if (!has_samples) {
    std::cerr << "Missing option --samples\n" << std::endl;
    print_help();
    exit(1);
  }

  return out;
}

void print_help() {
  std::cout << "--input-file\t(-f)\t\t(string-required)\n"
            << "--samples\t(-s)\t\t(integer-required)\n"
            << std::endl;
}