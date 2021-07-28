//
// Created by cristobal on 7/28/21.
//

#include <fstream>
#include <getopt.h>
#include <iostream>
#include <nt_parser.hpp>
#include <stdexcept>
#include <string>
#include <unordered_set>

struct parsed_options {
  std::string input_file;
};

parsed_options parse_cmline(int argc, char **argv);

struct FSHolder {
  std::unordered_set<std::string> predicates;
};

void processor(NTTriple *ntriple, void *fs_holder_ptr);

int main(int argc, char **argv) {
  auto parsed = parse_cmline(argc, argv);

  std::ifstream ifs(parsed.input_file, std::ios::in);
  FSHolder fs_holder{};
  NTParser ntparser(&ifs, processor, &fs_holder);
  ntparser.parse();

  std::cout << "Total predicates: " << fs_holder.predicates.size() << std::endl;

  return 0;
}

void processor(NTTriple *ntriple, void *fs_holder_ptr) {
  auto &h = *reinterpret_cast<FSHolder *>(fs_holder_ptr);
  auto predicate = std::string(ntriple->predicate.data);
  h.predicates.insert(predicate);
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "i:";
  struct option long_options[] = {
      {"input-file", required_argument, nullptr, 'i'},
  };

  int opt, opt_index;

  bool has_input = false;

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
    default:
      break;
    }
  }

  if (!has_input)
    throw std::runtime_error("Argument input-file (i) required");

  return out;
}