//
// Created by Cristobal Miranda, 2020
//

#include <fstream>
#include <getopt.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <exception>

#include <StringDictionaryHTFC.h>
#include <StringDictionaryPFC.h>

#include <iterators/IteratorDictString.h>

class VecIteratorDictString : public IteratorDictString {
  std::vector<std::string> &v;
  size_t current;

public:
  explicit VecIteratorDictString(std::vector<std::string> &v) : v(v), current(0) {}

  bool hasNext() override { return current < v.size(); }

  unsigned char *next(uint *str_length) override {
    auto &result = v[current++];
    *str_length = result.size();
    return reinterpret_cast<unsigned  char*>(result.data());
  }
};

namespace {
enum SDType { PFC = 0, HTFC = 1 };
}

struct parsed_options {
  std::string input_file;
  std::string output_file;
  SDType sd_type;
};

parsed_options parse_cmline(int argc, char **argv);

void print_help();

int main(int argc, char **argv) {
  parsed_options parsed = parse_cmline(argc, argv);

  std::ifstream ifs(parsed.input_file);

  std::string line;

  std::vector<std::string> data_holder;
  while(std::getline(ifs, line)){
    data_holder.push_back(line);
  }

  std::sort(data_holder.begin(), data_holder.end());

  auto *it =
      dynamic_cast<IteratorDictString *>(new VecIteratorDictString(data_holder));
  std::unique_ptr<StringDictionary> sd;

  switch (parsed.sd_type) {
  case SDType ::PFC:
    sd = std::make_unique<StringDictionaryPFC>(it, 3);
    break;
  case SDType ::HTFC:
    sd = std::make_unique<StringDictionaryHTFC>(it, 3);
    break;
  }

  ifs.close();

  std::ofstream ofs(parsed.output_file, std::ofstream::binary);

  sd->save(ofs);

  return 0;
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "f:o:t::";
  struct option long_options[] = {
      {"input-file", required_argument, nullptr, 'f'},
      {"output-file", required_argument, nullptr, 'o'},
      {"sd-type", optional_argument, nullptr, 't'}};

  int opt, opt_index;

  bool has_input = false;
  bool has_output = false;
  bool has_sd_type = false;

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
          out.sd_type = SDType::PFC;
        } else if (given_type == "HTFC") {
          out.sd_type = SDType::HTFC;
        } else {
          throw std::runtime_error("Unknown String Dictionary type: " +
                                   given_type);
        }
        has_sd_type = true;
      }
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
    out.sd_type = SDType::PFC;
  }

  return out;
}

void print_help() {
  std::cout << "--input-file\t(-f)\t\t(string-required)\n"
            << "--output-file\t(-o)\t\t(string-required)\n"
            << "--sd-type\t(-t)\t\t([PFC,HTFC]-optional, default=PFC)\n"
            << std::endl;
}
