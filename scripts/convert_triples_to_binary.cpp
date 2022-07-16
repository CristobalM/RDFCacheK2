#include <iostream>
#include <stdexcept>
#include <string>

#include <getopt.h>
#include <serialization_util.hpp>
#include <triple_external_sort.hpp>

using namespace k2cache;

struct parsed_options {
  std::string input_file;
  std::string output_file;
};

parsed_options parse_cmd_line(int argc, char **argv);

int main(int argc, char **argv) {
  auto parsed = parse_cmd_line(argc, argv);

  std::ifstream ifs(parsed.input_file, std::ios::in);
  std::ofstream ofs(parsed.output_file,
                    std::ios::out | std::ios::binary | std::ios::trunc);

  std::string line;
  std::string number_string;

  unsigned long total_lines = 0;
  while (std::getline(ifs, line)) {
    total_lines++;
  }
  ifs.clear();
  ifs.seekg(0, std::ios::beg);
  std::cout << "total lines " << total_lines << std::endl;
  write_u64(ofs, total_lines);
  while (std::getline(ifs, line)) {
    std::stringstream ss(line);
    int i = 0;
    unsigned long numbers[3];
    while (std::getline(ss, number_string, ',')) {
      numbers[i++] = std::stoul(number_string);
    }
    TripleValue(numbers[0], numbers[1], numbers[2]).write_to_file(ofs);
  }
}

parsed_options parse_cmd_line(int argc, char **argv) {
  const char short_options[] = "i:o:";
  struct option long_options[] = {
      {"input-file", required_argument, nullptr, 'i'},
      {"output-file", required_argument, nullptr, 'o'},
  };

  int opt, opt_index;

  bool has_input = false;
  bool has_output = false;
  parsed_options out{};

  while ((
      opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }
    switch (opt) {
    case 'i':
      out.input_file = optarg;
      has_input = true;
      break;
    case 'o':
      out.output_file = optarg;
      has_output = optarg;
      break;
    default:
      break;
    }
  }

  if (!has_input)
    throw std::runtime_error("input-file (i) argument is required");
  if (!has_output)
    throw std::runtime_error("output-file (o) argument is required");

  return out;
}