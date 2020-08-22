//
// Created by Cristobal Miranda, 2020
//
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <memory>
#include <string>

#include <exception>

#include <base64.h>

enum ACTION { ENCODE, DECODE };

struct parsed_options {
  std::string input_file;
  std::string output_file;
  ACTION action;
};

parsed_options parse_cmline(int argc, char **argv);

void print_help();

int main(int argc, char **argv) {
  parsed_options parsed = parse_cmline(argc, argv);

  std::ifstream ifs(parsed.input_file, std::ios::in);
  std::ofstream ofs(parsed.output_file, std::ios::out);

  std::string line;
  std::string transformed;
  while (std::getline(ifs, line)) {

    if (parsed.action == ENCODE) {
      transformed = base64_encode(line);
    } else {
      transformed = base64_decode(line, true);
    }

    ofs << transformed << "\n";
  }
  return 0;
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "f:o:a:";
  struct option long_options[] = {
      {"input-file", required_argument, nullptr, 'f'},
      {"output-file", required_argument, nullptr, 'o'},
      {"action", required_argument, nullptr, 'a'},
  };

  int opt, opt_index;

  bool has_input = false;
  bool has_output = false;
  bool has_action = false;

  std::string invalid_argument;
  bool has_invalid_argument = false;

  parsed_options out{};

  std::string given_type;
  std::string action;
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
    case 'a':
      action = optarg;
      if (action == "ENCODE") {
        out.action = ENCODE;
      } else if (action == "DECODE") {
        out.action = DECODE;
      } else {
        invalid_argument = action;
        has_invalid_argument = true;
      }
      has_action = true;
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

  if (!has_action) {
    std::cerr << "Missing option --action\n" << std::endl;
    print_help();
    exit(1);
  }

  if (has_invalid_argument) {
    std::cerr << "Invalid argument " << invalid_argument << "in --action\n"
              << std::endl;
    print_help();
    exit(1);
  }

  return out;
}

void print_help() {
  std::cout << "--input-file\t(-f)\t\t(string-required)\n"
            << "--output-file\t(-o)\t\t(string-required)\n"
            << "--action\t(-a)\t\t([ENCODE, DECODE]-required)\n"
            << std::endl;
}
