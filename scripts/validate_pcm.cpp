//
// Created by Cristobal Miranda, 2020
//
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <memory>
#include <string>

#include <PredicatesCacheManager.hpp>
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
  std::string k2trees_file;
};

parsed_options parse_cmline(int argc, char **argv);

void print_help();

int main(int argc, char **argv) {
  parsed_options parsed = parse_cmline(argc, argv);

  std::ifstream ifs_iris(parsed.iris_file, std::ios::in | std::ios::binary);
  std::ifstream ifs_blanks(parsed.blanks_file, std::ios::in | std::ios::binary);
  std::ifstream ifs_literals(parsed.literals_file,
                             std::ios::in | std::ios::binary);

  auto sds_tmp = std::make_unique<
      SDEntitiesMapping<StringDictionaryPFC, StringDictionaryPFC,
                        StringDictionaryHASHRPDACBlocks>>(ifs_iris, ifs_blanks,
                                                          ifs_literals);

  PredicatesCacheManager pcm(std::move(sds_tmp), parsed.k2trees_file);

  auto *sds = pcm.get_isd_manager();

  for (unsigned long i = 1; i < sds->last_id(); i++) {
    auto res = sds->get_resource(i);
    auto index = pcm.get_resource_index(res);
    if (i != index) {
      std::cerr << "Error at index i = " << i << " with value " << res.value
                << ", type = " << res.resource_type
                << ", is giving index = " << index;
      exit(1);
    }
  }

  std::cout << "Valid PCM" << std::endl;

  return 0;
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "i:b:l:k:";
  struct option long_options[] = {
      {"iris-file", required_argument, nullptr, 'i'},
      {"blanks-file", required_argument, nullptr, 'b'},
      {"literals-file", required_argument, nullptr, 'l'},
      {"k2trees-file", required_argument, nullptr, 'l'},
  };

  int opt, opt_index;

  bool has_iris = false;
  bool has_blanks = false;
  bool has_literals = false;
  bool has_k2trees = false;

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
    case 'k':
      out.k2trees_file = optarg;
      has_k2trees = true;
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
  if (!has_k2trees) {
    std::cerr << "Missing option --k2trees-file\n" << std::endl;
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
            << std::endl;
}
