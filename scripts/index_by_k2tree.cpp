//
// Created by Cristobal Miranda, 2020
//

#include <cstring>
#include <getopt.h>

#include <NTParser.hpp>
#include <entities_mapping.pb.h>
#include <fstream>
#include <iostream>

enum InputFormat { NT = 0 };

struct parsed_options {
  InputFormat input_format;
  bool generate_mapping;
  std::string input_data;
  std::string output_file;
  std::string predicates_to_find_file;
  std::string previous_mapping_fpath;
  bool has_predicates;
  bool has_previous_mapping;
};

void print_help();

parsed_options parse_cmline(int argc, char **argv);

void write_mapping(const std::string &fname, EntitiesMapping &mapping);

std::vector<std::string> read_predicates(const std::string &predicates_file);

int main(int argc, char **argv) {
  auto parsed_cmline = parse_cmline(argc, argv);

  std::cout << "parsed " << std::endl;

  std::shared_ptr<EntitiesMapping> entities_mapping = nullptr;

  if (parsed_cmline.has_previous_mapping) {
    std::cout << "Previous mapping available... Importing" << std::endl;
    entities_mapping =
        EntitiesMapping::load_from_file(parsed_cmline.previous_mapping_fpath);
    std::cout << "Previous mapping imported" << std::endl;
  }

  switch (parsed_cmline.input_format) {
  case InputFormat::NT:
  default:
    std::unique_ptr<NTParser> nt_parser;
    if (entities_mapping == nullptr) {
      nt_parser = std::make_unique<NTParser>(parsed_cmline.input_data);
    } else {
      nt_parser = std::make_unique<NTParser>(parsed_cmline.input_data,
                                             entities_mapping);
    }
    if (parsed_cmline.has_predicates) {
      auto predicates = read_predicates(parsed_cmline.predicates_to_find_file);
      nt_parser->set_predicates(predicates);
      std::cout << "setting predicates = True" << std::endl;
    }
    auto parsed_result = nt_parser->parse();
    if (parsed_cmline.generate_mapping) {
      std::cout << "serializing mapping" << std::endl;
      write_mapping(parsed_cmline.output_file + ".map",
                    *parsed_result->entities_mapping);
    }

    std::cout << "serializing k2trees" << std::endl;
    parsed_result->predicates_index_cache->dump_to_file(
        parsed_cmline.output_file);
  }
}

std::vector<std::string> read_predicates(const std::string &predicates_file) {
  std::vector<std::string> out;
  std::ifstream ifs(predicates_file);
  if (ifs.fail()) {
    throw std::runtime_error("File '" + predicates_file + "' not found");
  }
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
      << "--previous-mapping\t(-m)\t\t(string-optional)\n"
      << std::endl;
}

void write_mapping(const std::string &fname, EntitiesMapping &mapping) {
  std::fstream outfs(fname, std::ios::out | std::ios::trunc | std::ios::binary);
  mapping.serialize(outfs);
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "f::p::m::gd:o:";
  struct option long_options[] = {
      {"input-format", optional_argument, nullptr, 'f'},
      {"predicates-input", optional_argument, nullptr, 'p'},
      {"previous-mapping", optional_argument, nullptr, 'm'},
      {"generate-mapping", optional_argument, nullptr, 'g'},
      {"input-data", required_argument, nullptr, 'd'},
      {"output-data", required_argument, nullptr, 'o'},

      {nullptr, 0, nullptr, 0}};

  int opt;
  int opt_index;

  parsed_options out;
  out.input_format = InputFormat::NT;
  out.generate_mapping = false;
  out.has_predicates = false;
  out.has_previous_mapping = false;

  bool has_input = false;
  bool has_output = false;

  while ((
      opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }

    switch (opt) {
    case 'f':
      if (optarg != nullptr) {
        std::string format_str(optarg);
        std::cout << "f: " << format_str << std::endl;
        if (format_str == "NT") {
          out.input_format = InputFormat::NT;
        }
      } else {
        std::cout << "No f" << std::endl;
      }
      break;
    case 'g':
      out.generate_mapping = true;
      std::cout << "g is on!" << std::endl;
      break;

    case 'p':
      if (optarg != nullptr) {
        out.predicates_to_find_file = optarg;
        std::cout << "p: " << out.predicates_to_find_file << std::endl;
        out.has_predicates = true;
      } else {
        std::cout << "No p" << std::endl;
      }
      break;
    case 'm':
      if (optarg != nullptr) {
        out.previous_mapping_fpath = optarg;
        std::cout << "m: " << out.previous_mapping_fpath << std::endl;
        out.has_previous_mapping = true;
      } else {
        std::cout << "No m" << std::endl;
      }
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