//
// Created by Cristobal Miranda, 2020
//

#include <exception>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>

#include <SDBuilder.hpp>

class VecIteratorDictString : public IteratorDictString {
  std::vector<std::string> &v;
  size_t current;

public:
  explicit VecIteratorDictString(std::vector<std::string> &v)
      : v(v), current(0) {}

  bool hasNext() override { return current < v.size(); }

  unsigned char *next(uint *str_length) override {
    auto &result = v[current++];
    *str_length = result.size();
    return reinterpret_cast<unsigned char *>(result.data());
  }
};

struct parsed_options {
  std::string input_file;
  std::string output_file;
  SDBuilder::SDType sd_type;
  bool base64;
  int thread_count;
  unsigned long cut_size;
  int bucket_size;
};


parsed_options parse_cmline(int argc, char **argv);

void print_help();

int main(int argc, char **argv) {
  parsed_options parsed = parse_cmline(argc, argv);

  std::ifstream ifs(parsed.input_file);

  SDInput input;
  input.thread_count = parsed.thread_count;
  input.cut_size = parsed.cut_size;
  input.bucket_size = parsed.bucket_size;
  auto sd = SDBuilder(parsed.sd_type, parsed.base64, input).build(ifs);

  ifs.close();

  std::ofstream ofs(parsed.output_file, std::ofstream::binary);

  sd->save(ofs);

  return 0;
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "f:o:t::bp::c::B::";
  struct option long_options[] = {
      {"input-file", required_argument, nullptr, 'f'},
      {"output-file", required_argument, nullptr, 'o'},
      {"sd-type", optional_argument, nullptr, 't'},
      {"base64", optional_argument, nullptr, 'b'},
      {"thread-count", optional_argument, nullptr, 'p'},
      {"cut-size", optional_argument, nullptr, 'c'},
      {"bucket-size", optional_argument, nullptr, 'B'}};

  int opt, opt_index;

  bool has_input = false;
  bool has_output = false;
  bool has_sd_type = false;
  bool has_thread_count = false;
  bool has_cut_size = false;
  bool has_bucket_size = false;

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
    case 'o':
      out.output_file = optarg;
      has_output = true;
      break;
    case 't':
      if (optarg) {
        std::string given_type(optarg);
        if (given_type == "PFC") {
          out.sd_type = SDBuilder::SDType::PFC;
        } else if (given_type == "HTFC") {
          out.sd_type = SDBuilder::SDType::HTFC;
        } else if (given_type == "HRPDAC") {
          out.sd_type = SDBuilder::SDType::HRPDAC;
        } else if (given_type == "RPDAC") {
          out.sd_type = SDBuilder::SDType ::RPDAC;
        } else if (given_type == "HRPDACBlocks") {
          out.sd_type = SDBuilder::SDType::HRPDACBlocks;
        } else {
          throw std::runtime_error("Unknown String Dictionary type: " +
                                   given_type);
        }
        has_sd_type = true;
      }
      break;
    case 'b':
      out.base64 = true;
      break;
    case 'p':
      if (optarg) {
        out.thread_count = std::stoi(std::string(optarg));
        has_thread_count = true;
      }
      break;
    case 'c':
      if (optarg) {
        out.cut_size = std::stoul(std::string(optarg));
        has_cut_size = true;
      }
      break;
    case 'B':
      if (optarg) {
        out.bucket_size = std::stoi(std::string(optarg));
        has_bucket_size = true;
      }
      break;
    case 'h': // to implement
    case '?':
    default:
      print_help();
      exit(1);
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

  if (!has_thread_count) {
    out.thread_count = 1;
  } else {
    if (out.thread_count < 1) {
      std::cerr << "Invalid thread count " << out.thread_count << std::endl;
      print_help();
      exit(1);
    }
  }

  if (!has_cut_size) {
    out.cut_size = 10'000'000;
  }

  if (!has_bucket_size) {
    out.bucket_size = 32;
  }

  return out;
}

void print_help() {
  std::cout
      << "--input-file\t(-f)\t\t(string-required)\n"
      << "--output-file\t(-o)\t\t(string-required)\n"
      << "--sd-type\t(-t)\t\t([PFC,HTFC,HRPDAC,RPDAC,HRPDACBlocks]-required)\n"
      << "--thread-count\t(-p)\t\t(integer >=1 -optional, default=1)\n"
      << "--cut-size\t(-c)\t\t(integer-optional, default=10MB)\n"
      << "--bucket-size\t(-B)\t\t(integer-optional, default=32)\n"
      << "--base64\t(-b)\t\t(bool-optional, default=(false))\n"

      << std::endl;
}
