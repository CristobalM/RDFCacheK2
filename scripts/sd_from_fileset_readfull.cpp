//
// Created by Cristobal Miranda, 2020
//

#include <exception>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <StringDictionaryHASHRPDAC.h>
#include <StringDictionaryHTFC.h>
#include <StringDictionaryPFC.h>
#include <StringDictionaryRPDAC.h>

#include <iterators/IteratorDictString.h>
#include <iterators/IteratorDictStringPlain.h>

#include <MemoryPool.hpp>

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

namespace {
enum SDType {
  PFC = 0,
  HTFC = 1,
  HRPDAC = 2,
  RPDAC = 3,
};
}

struct parsed_options {
  std::string input_file;
  std::string output_file;
  SDType sd_type;
};

struct BufferedData {
  unsigned char *data;
  size_t size;
};

parsed_options parse_cmline(int argc, char **argv);

void print_help();

BufferedData put_in_buffer(std::vector<BufferedData> &input_vec);

int main(int argc, char **argv) {
  parsed_options parsed = parse_cmline(argc, argv);

  std::ifstream ifs(parsed.input_file);

  std::string line;

  std::vector<BufferedData> data_holder;

  // These memory pool operations are done to clear the read data faster
  static constexpr size_t one_MB = 1'000'000'000;
  using buffer_t = unsigned char[one_MB]; // 1MB
  MemoryPool<buffer_t> pool(1);
  buffer_t *current_buffer = pool.request_memory();
  size_t bytes_used = 0;
  size_t bytes_used_total = 0;
  while (std::getline(ifs, line)) {
    BufferedData bd{};
    // bsd.data = std::make_unique<unsigned char[]>(line.size()+1);
    if (bytes_used + line.size() + 1 > one_MB) {
      current_buffer = pool.request_memory();
      bytes_used_total += bytes_used;
      bytes_used = 0;
    }
    bd.data = reinterpret_cast<unsigned char *>(*current_buffer) + bytes_used;
    memcpy(bd.data, line.data(), line.size() + 1);
    bd.size = line.size();
    bytes_used += line.size() + 1;
    data_holder.push_back(bd);
  }
  /*
  std::sort(data_holder.begin(), data_holder.end(), [](const BufferedData &lhs,
  const BufferedData &rhs){ return strcmp(reinterpret_cast<char *>(lhs.data),
  reinterpret_cast<char *>(rhs.data)) < 0;
  });
   */

  auto buffered_data = put_in_buffer(data_holder);
  pool.free_all_memory();
  data_holder.clear();

  auto *it = dynamic_cast<IteratorDictString *>(
      new IteratorDictStringPlain(buffered_data.data, buffered_data.size));
  std::unique_ptr<StringDictionary> sd;

  switch (parsed.sd_type) {
  case SDType::PFC:
    sd = std::make_unique<StringDictionaryPFC>(it, 3);
    break;
  case SDType::HTFC:
    sd = std::make_unique<StringDictionaryHTFC>(it, 3);
    break;
  case SDType::HRPDAC:
    std::cout << "Creating HASHRPDAC" << std::endl;
    sd =
        std::make_unique<StringDictionaryHASHRPDAC>(it, buffered_data.size, 32);
    break;
  case SDType::RPDAC:
    std::cout << "Creating RPDAC" << std::endl;
    sd = std::make_unique<StringDictionaryRPDAC>(it);
    break;
  }

  ifs.close();

  std::ofstream ofs(parsed.output_file, std::ofstream::binary);

  sd->save(ofs);

  return 0;
}

BufferedData put_in_buffer(std::vector<BufferedData> &input_vec) {
  unsigned long buffer_sz = 0;
  for (auto bd : input_vec) {
    buffer_sz += bd.size + 1;
  }
  auto *data = new unsigned char[buffer_sz];

  unsigned long pos = 0;
  for (auto bd : input_vec) {
    memcpy(data + pos, bd.data, bd.size + 1);
    pos += bd.size + 1;
  }

  BufferedData out{
      .data = data,
      .size = buffer_sz,
  };

  return out;
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
        } else if (given_type == "HRPDAC") {
          out.sd_type = SDType::HRPDAC;
        } else if (given_type == "RPDAC") {
          out.sd_type = SDType ::RPDAC;
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
  std::cout
      << "--input-file\t(-f)\t\t(string-required)\n"
      << "--output-file\t(-o)\t\t(string-required)\n"
      << "--sd-type\t(-t)\t\t([PFC,HTFC,HRPDAC,RPDAC]-optional, default=PFC)\n"
      << std::endl;
}
