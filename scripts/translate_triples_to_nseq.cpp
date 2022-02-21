#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

#include <FileIStream.hpp>
#include <NodesSequence.hpp>
#include <getopt.h>
#include <serialization_util.hpp>
#include <triple_external_sort.hpp>

struct parsed_options {
  std::string input_file;
  std::string output_file;
  std::string nodes_sequence_file;
};

parsed_options parse_cmline(int argc, char **argv);

namespace fs = std::filesystem;

int main(int argc, char **argv) {
  auto parsed = parse_cmline(argc, argv);

  if (!fs::exists(parsed.input_file)) {
    std::cerr << "file does not exist (input_file): " << parsed.input_file
              << std::endl;
    exit(1);
  }
  if (!fs::exists(parsed.nodes_sequence_file)) {
    std::cerr << "file does not exist (nodes_sequence_file): "
              << parsed.nodes_sequence_file << std::endl;
    exit(1);
  }

  std::ifstream ifs(parsed.input_file, std::ios::in | std::ios::binary);
  std::ofstream ofs(parsed.output_file,
                    std::ios::out | std::ios::binary | std::ios::trunc);

  std::unique_ptr<NodesSequence> nodes_sequence = nullptr;
  {
    FileIStream nodes_fis(parsed.nodes_sequence_file,
                          std::ios::in | std::ios::binary);
    nodes_sequence = std::make_unique<NodesSequence>(
        NodesSequence::from_input_stream(nodes_fis));
  }
  FileData filedata{};
  filedata.size = read_u64(ifs);
  filedata.current_triple = 0;
  write_u64(ofs, filedata.size);
  while (!filedata.finished()) {
    auto triple = filedata.read_triple(ifs);
    // ofs << triple.first << "," << triple.second << "," << triple.third <<
    // "\n";
    auto first = (unsigned long)nodes_sequence->get_id((long)triple.first);
    auto second = (unsigned long)nodes_sequence->get_id((long)triple.second);
    auto third = (unsigned long)nodes_sequence->get_id((long)triple.third);
    TripleValue triple_value(first, second, third);
    triple_value.write_to_file(ofs);
  }
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "i:o:n:";
  struct option long_options[] = {
      {"input-file", required_argument, nullptr, 'i'},
      {"output-file", required_argument, nullptr, 'o'},
      {"nodes-sequence-file", required_argument, nullptr, 'n'},
  };

  int opt, opt_index;

  bool has_input = false;
  bool has_output = false;
  bool has_nodes = false;
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
    case 'n':
      out.nodes_sequence_file = optarg;
      has_nodes = optarg;
      break;
    default:
      break;
    }
  }

  if (!has_input)
    throw std::runtime_error("input-file (i) argument is required");
  if (!has_output)
    throw std::runtime_error("output-file (o) argument is required");
  if (!has_nodes)
    throw std::runtime_error("nodes-sequence-file (o) argument is required");

  return out;
}