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
#include <StringDictionaryHASHRPDACBlocks.h>
#include <StringDictionaryHTFC.h>
#include <StringDictionaryPFC.h>
#include <StringDictionaryRPDAC.h>

#include <iterators/IteratorDictString.h>
#include <iterators/IteratorDictStringPlain.h>

#include <MemoryPool.hpp>

#include <base64.h>

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
  HRPDACBlocks = 4,
};
}

struct parsed_options {
  std::string input_file;
  std::string output_file;
  SDType sd_type;
  bool base64;
  int thread_count;
  unsigned long cut_size;
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
    std::string decoded;
    if (parsed.base64)
      decoded = base64_decode(line, true);
    else
      decoded = line;
    if (bytes_used + decoded.size() + 1 > one_MB) {
      current_buffer = pool.request_memory();
      bytes_used_total += bytes_used;
      bytes_used = 0;
    }
    bd.data = reinterpret_cast<unsigned char *>(*current_buffer) + bytes_used;
    memcpy(bd.data, decoded.data(), decoded.size() + 1);
    bd.size = decoded.size();
    bytes_used += decoded.size() + 1;
    data_holder.push_back(bd);
  }

  std::sort(data_holder.begin(), data_holder.end(),
            [](const BufferedData &lhs, const BufferedData &rhs) {
              return strcmp(reinterpret_cast<char *>(lhs.data),
                            reinterpret_cast<char *>(rhs.data)) < 0;
            });

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
        std::make_unique<StringDictionaryHASHRPDAC>(it, buffered_data.size, 25);
    break;
  case SDType::HRPDACBlocks:
    std::cout << "Creating HASHRPDACBlocks" << std::endl;
    sd = std::make_unique<StringDictionaryHASHRPDACBlocks>(
        dynamic_cast<IteratorDictStringPlain *>(it), buffered_data.size, 25,
        parsed.cut_size, parsed.thread_count);
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
  const char short_options[] = "f:o:t::b::p::c::";
  struct option long_options[] = {
      {"input-file", required_argument, nullptr, 'f'},
      {"output-file", required_argument, nullptr, 'o'},
      {"sd-type", optional_argument, nullptr, 't'},
      {"base64", optional_argument, nullptr, 'b'},
      {"thread-count", optional_argument, nullptr, 'p'},
      {"cut-size", optional_argument, nullptr, 'c'}};

  int opt, opt_index;

  bool has_input = false;
  bool has_output = false;
  bool has_sd_type = false;
  bool has_thread_count = false;
  bool has_cut_size = false;

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
        } else if (given_type == "HRPDACBlocks") {
          out.sd_type = SDType::HRPDACBlocks;
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
    out.cut_size = 1UL << 27UL;
  }

  return out;
}

void print_help() {
  std::cout
      << "--input-file\t(-f)\t\t(string-required)\n"
      << "--output-file\t(-o)\t\t(string-required)\n"
      << "--sd-type\t(-t)\t\t([PFC,HTFC,HRPDAC,RPDAC,HRPDACBlocks]-required)\n"
      << "--thread-count\t(-p)\t\t(integer >=1 -optional, default=1)\n"
      << "--cut-size\t(-c)\t\t(integer-optional, default=134217728 bytes)\n"
      << std::endl;
}
