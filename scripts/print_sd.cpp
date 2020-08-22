//
// Created by Cristobal Miranda, 2020
//
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <memory>
#include <string>

#include <StringDictionaryHASHRPDAC.h>
#include <StringDictionaryHASHRPDACBlocks.h>
#include <StringDictionaryHTFC.h>
#include <StringDictionaryPFC.h>
#include <StringDictionaryRPDAC.h>
#include <iterators/IteratorDictString.h>

#include <exception>

#include <base64.h>

namespace {
enum SDType {
  PFC = 0,
  HTFC = 1,
  HRPDAC = 2,
  RPDAC = 3,
  HRPDACBlocks = 4,
};
}

struct parsed_options {
  std::string input_file;
  std::string output_file;
  SDType sd_type;
  bool base64;
};

parsed_options parse_cmline(int argc, char **argv);

void print_help();

int main(int argc, char **argv) {
  parsed_options parsed = parse_cmline(argc, argv);

  std::ifstream ifs(parsed.input_file, std::ios::in | std::ios::binary);

  std::unique_ptr<StringDictionary> sd;

  switch (parsed.sd_type) {
  case SDType::PFC:
    sd = std::unique_ptr<StringDictionary>(StringDictionaryPFC::load(ifs));
    break;
  case SDType::HTFC:
    sd = std::unique_ptr<StringDictionary>(StringDictionaryHTFC::load(ifs));
    break;
  case SDType::HRPDAC:
    sd =
        std::unique_ptr<StringDictionary>(StringDictionaryHASHRPDAC::load(ifs));
    break;
  case SDType::RPDAC:
    sd = std::unique_ptr<StringDictionary>(StringDictionaryRPDAC::load(ifs));
    break;
  case SDType::HRPDACBlocks:
    sd = std::unique_ptr<StringDictionary>(
        StringDictionaryHASHRPDACBlocks::load(ifs));
    break;
  }

  ifs.close();

  std::ofstream ofs(parsed.output_file);
  size_t bytes_now = 0;
  static constexpr size_t bytes_th = (1L << 20L);
  auto *it = sd->extractTable();
  while (it->hasNext()) {
    unsigned int strlen;
    auto *str = it->next(&strlen);
    std::string_view s(reinterpret_cast<char *>(str), strlen);
    if (parsed.base64) {
      auto b64_str = base64_encode(s);
      ofs << b64_str << "\n";
      bytes_now += b64_str.size();
    } else {
      ofs << s << "\n";
      bytes_now += s.size();
    }

    if (bytes_now >= bytes_th) {
      ofs.flush();
    }

    delete[] str;
  }
  delete it;
  return 0;
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "f:o:t:b";
  struct option long_options[] = {
      {"input-file", required_argument, nullptr, 'f'},
      {"output-file", required_argument, nullptr, 'o'},
      {"sd-type", required_argument, nullptr, 't'},
      {"base64", optional_argument, nullptr, 'b'}};

  int opt, opt_index;

  bool has_input = false;
  bool has_output = false;
  bool has_sd_type = false;

  parsed_options out{};
  out.base64 = false;

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
    case 't':
      given_type = optarg;
      if (given_type == "PFC") {
        out.sd_type = SDType::PFC;
      } else if (given_type == "HTFC") {
        out.sd_type = SDType::HTFC;
      } else if (given_type == "HRPDAC") {
        out.sd_type = SDType::HRPDAC;
      } else if (given_type == "RPDAC") {
        out.sd_type = SDType ::RPDAC;
      } else if (given_type == "HRPDACBlocks") {
        out.sd_type = SDType ::HRPDACBlocks;
      } else {
        throw std::runtime_error("Unknown String Dictionary type: " +
                                 given_type);
      }
      has_sd_type = true;

      break;
    case 'b':
      out.base64 = true;
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

  if (!has_sd_type) {
    std::cerr << "Missing option --sd-type\n" << std::endl;
    print_help();
    exit(1);
  }

  return out;
}

void print_help() {
  std::cout
      << "--input-file\t(-f)\t\t(string-required)\n"
      << "--output-file\t(-o)\t\t(string-required)\n"
      << "--sd-type\t(-t)\t\t([PFC,HTFC,HRPDAC,RPDAC, HRPDACBlocks]-required)\n"
      << "--base64\t(-b)\t\t(bool-optional, default=false)\n"
      << std::endl;
}
