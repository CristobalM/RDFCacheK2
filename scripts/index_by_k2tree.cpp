//
// Created by Cristobal Miranda, 2020
//

#include <cstring>
#include <getopt.h>

#include "NTParser.hpp"
#include <entities_mapping.pb.h>
#include <fstream>
#include <iostream>

#include "EntitiesMapping.h"

enum InputFormat { NT = 0 };

struct parsed_options {
  InputFormat input_format;
  bool generate_mapping;
  std::string input_data;
  std::string output_file;
  std::string predicates_to_find_file;
  bool has_predicates;
};

void print_help();
parsed_options parse_cmline(int argc, char **argv);

void write_mapping(const std::string &fname,
                   proto_msg::EntitiesMapping &mapping);

std::vector<std::string> read_predicates(const std::string &predicates_file);

int main(int argc, char **argv) {
  auto parsed_cmline = parse_cmline(argc, argv);
  switch (parsed_cmline.input_format) {
  case InputFormat::NT:
  default:
    NTParser nt_parser(parsed_cmline.input_data);
    if (parsed_cmline.has_predicates) {
      auto predicates = read_predicates(parsed_cmline.predicates_to_find_file);
      nt_parser.set_predicates(predicates);
      std::cout << "setting predicates = True" << std::endl;
    }
    auto parsed_result = nt_parser.parse();
    if (parsed_cmline.generate_mapping) {
      std::cout << "serializing mapping" << std::endl;
      auto serialized_entities_mapping =
          parsed_result->entities_mapping.serialize();
      write_mapping(parsed_cmline.output_file + ".map",
                    *serialized_entities_mapping);
    }

    std::cout << "serializing k2trees" << std::endl;
    parsed_result->predicates_index_cache.dump_to_file(
        parsed_cmline.output_file);
  }
}
std::vector<std::string> read_predicates(const std::string &predicates_file) {
  std::vector<std::string> out;
  std::ifstream ifs(predicates_file);
  std::string line;
  while (std::getline(ifs, line)) {
    out.push_back(line);
  }
  ifs.close();
  return out;
}

void print_help() {
  std::cout
      << "--input-format\t(-f)\t\t[NT]\n"
      << "--generate-mapping\t(-g)\t\t(boolean)\n"
      << "--input-data\t(-d)\t\t(string-required)\n"
      << "--output-data\t(-o)\t\t(string-required)\n"
      << "--predicates-input\t(-p)\t\t(string-optional) (if none match all)\n"
      << std::endl;
}

void write_mapping(const std::string &fname,
                   proto_msg::EntitiesMapping &mapping) {
  std::ofstream ofs(fname, std::ios::binary);

  mapping.SerializeToOstream(&ofs);

  ofs.close();
}

parsed_options parse_cmline(int argc, char **argv) {
  const char const short_options[] = "f::gp::d:o:";
  struct option long_options[] = {
      {"input-format", optional_argument, nullptr, 'f'},
      {"generate-mapping", optional_argument, nullptr, 'g'},
      {"predicates-input", optional_argument, nullptr, 'p'},
      {"input-data", required_argument, nullptr, 'd'},
      {"output-data", required_argument, nullptr, 'o'},
      {nullptr, 0, nullptr, 0}};

  int opt;
  int opt_index;

  parsed_options out;
  out.input_format = InputFormat::NT;
  out.generate_mapping = false;
  out.has_predicates = false;

  bool has_input = false;
  bool has_output = false;
  while ((
      opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }

    switch (opt) {
    case 'f':
      std::cout << "f: " << optarg << std::endl;
      if (std::strncmp(optarg, "NT", 2 * sizeof(char)) == 0) {
        out.input_format = InputFormat::NT;
      }
      break;
    case 'g':
      out.generate_mapping = true;
      break;

    case 'p':
      std::cout << "p: " << optarg << std::endl;
      out.predicates_to_find_file = optarg;
      out.has_predicates = true;
      break;
    case 'd':
      std::cout << "d: " << optarg << std::endl;
      out.input_data = optarg;
      has_input = true;
      break;

    case 'o':
      std::cout << "o: " << optarg << std::endl;
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