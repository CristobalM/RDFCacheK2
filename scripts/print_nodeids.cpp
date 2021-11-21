#include <filesystem>
#include <stdexcept>
#include <string>

#include <UnsignedLongSortConnector.hpp>
#include <external_sort.hpp>
#include <getopt.h>
#include <serialization_util.hpp>
#include <triple_external_sort.hpp>

struct parsed_options {
  std::string input_file;
  std::string output_file;
};

parsed_options parse_cmline(int argc, char **argv);

namespace fs = std::filesystem;

int main(int argc, char **argv) {
  auto parsed = parse_cmline(argc, argv);

  std::ifstream ifs(parsed.input_file, std::ios::in | std::ios::binary);
  std::ofstream ofs(parsed.output_file, std::ios::out | std::ios::trunc);
  auto total_nodes = read_u64(ifs);
  for (size_t i = 0; i < total_nodes; i++) {
    auto nodeid = read_u64(ifs);
    ofs << nodeid << "\n";
  }

  ifs.close();
  ofs.close();

  return 0;
}

parsed_options parse_cmline(int argc, char **argv) {
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