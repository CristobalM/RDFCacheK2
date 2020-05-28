//
// Created by Cristobal Miranda, 2020
//

#include <cstring>
#include <getopt.h>

#include <iostream>

enum InputFormat { NT = 0 };

struct parsed_options {
  InputFormat input_format;
  bool generate_mapping;
  std::string input_data;
};

void print_help() {
  std::cout << "--input-format\t(-f)\t\t[NT]\n"
            << "--generate-mapping\t(-g)\t\t(boolean)\n"
            << "--input-data\t(-d)\t\t(string-required)" << std::endl;
}

parsed_options parse_cmline(int argc, char **argv) {
  const char *const short_options = "f::gd:";
  struct option long_options[] = {
      {"input-format", optional_argument, nullptr, 'f'},
      {"generate-mapping", optional_argument, nullptr, 'g'},
      {"input-data", required_argument, nullptr, 'd'},
      {nullptr, 0, nullptr, 0}};

  int opt;
  int opt_index;

  parsed_options out;
  out.input_format = InputFormat::NT;
  out.generate_mapping = false;

  bool has_input = false;

  while ((
      opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }

    switch (opt) {
    case 'f':
      if (std::strncmp(optarg, "NT", 2 * sizeof(char)) == 0) {
        out.input_format = InputFormat::NT;
      }
      break;
    case 'g':
      out.generate_mapping = true;
      break;
    case 'd':
      out.input_data = optarg;
      has_input = true;
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

  return out;
}

int main(int argc, char **argv) {
  auto parsed_cmline = parse_cmline(argc, argv);
}