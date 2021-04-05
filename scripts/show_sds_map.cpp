//
// Created by Cristobal Miranda, 2020
//
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <memory>
#include <string>

#include <SDEntitiesMapping.hpp>
#include <StringDictionaryHASHRPDAC.h>
#include <StringDictionaryHASHRPDACBlocks.h>
#include <StringDictionaryHTFC.h>
#include <StringDictionaryPFC.h>
#include <StringDictionaryRPDAC.h>

#include <exception>

struct parsed_options {
  std::string iris_file;
  std::string blanks_file;
  std::string literals_file;
  std::string output_file;
};

parsed_options parse_cmline(int argc, char **argv);

void print_help();

int main(int argc, char **argv) {
  parsed_options parsed = parse_cmline(argc, argv);

  std::ifstream ifs_iris(parsed.iris_file, std::ios::in | std::ios::binary);
  std::ifstream ifs_blanks(parsed.blanks_file, std::ios::in | std::ios::binary);
  std::ifstream ifs_literals(parsed.literals_file,
                             std::ios::in | std::ios::binary);

  SDEntitiesMapping<StringDictionaryPFC, StringDictionaryPFC,
                    StringDictionaryHASHRPDACBlocks>
      sds(ifs_iris, ifs_blanks, ifs_literals);

  std::ofstream ofs(parsed.output_file, std::ios::out | std::ios::trunc);

  for (unsigned long i = 1; i <= sds.last_id(); i++) {
    ofs << i << "," << sds.get_resource(i).value << "\n";
  }

  return 0;
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "i:b:l:o:";
  struct option long_options[] = {
      {"iris-file", required_argument, nullptr, 'i'},
      {"blanks-file", required_argument, nullptr, 'b'},
      {"literals-file", required_argument, nullptr, 'l'},
      {"output-file", required_argument, nullptr, 'o'},
  };

  int opt, opt_index;

  bool has_iris = false;
  bool has_blanks = false;
  bool has_literals = false;
  bool has_output = false;

  parsed_options out{};

  std::string given_type;
  while ((
      opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }

    switch (opt) {
    case 'i':
      out.iris_file = optarg;
      has_iris = true;
      break;
    case 'b':
      out.blanks_file = optarg;
      has_blanks = true;
      break;
    case 'l':
      out.literals_file = optarg;
      has_literals = true;
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

  if (!has_iris) {
    std::cerr << "Missing option --iris-file\n" << std::endl;
    print_help();
    exit(1);
  }
  if (!has_blanks) {
    std::cerr << "Missing option --blanks-file\n" << std::endl;
    print_help();
    exit(1);
  }
  if (!has_literals) {
    std::cerr << "Missing option --literals-file\n" << std::endl;
    print_help();
    exit(1);
  }

  if (!has_output) {
    std::cerr << "Missing option --output-file\n" << std::endl;
    print_help();
    exit(1);
  }

  return out;
}

void print_help() {
  std::cout << "--iris-file\t(-i)\t\t(string-required)\n"
            << "--blanks-file\t(-b)\t\t(string-required)\n"
            << "--literals-file\t(-l)\t\t(string-required)\n"
            << "--output-file\t(-o)\t\t(string-required)\n"
            << std::endl;
}
