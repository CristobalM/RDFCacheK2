//
// Created by Cristobal Miranda, 2020
//

#include <fstream>
#include <getopt.h>
#include <iostream>

#include <RadixTree.hpp>

#include <raptor2.h>

struct parsed_options {
  std::string input_file;
  std::string output_file;
};

parsed_options parse_cmline(int argc, char **argv);

void print_help();

void process_nt_file(const std::string &input_file_path,
                     const std::string &output_file_path);

void statement_handler(void *radix_tree_ptr, const raptor_statement *statement);

std::string get_term_value(raptor_term *term);

void print_stats();

unsigned long bytes_processed = 0;
unsigned long strings_processed = 0;
unsigned long unique_strings_processed = 0;

unsigned long bytes_stored = 0;

unsigned long strings_processed_reset_th = 0;

const unsigned long RESET_TH = 1000000;

int main(int argc, char **argv) {
  parsed_options p_options = parse_cmline(argc, argv);

  process_nt_file(p_options.input_file, p_options.output_file);

  return 0;
}

void process_nt_file(const std::string &input_file_path,
                     const std::string &output_file_path) {
  RadixTree<> radix_tree;
  raptor_world *world = raptor_new_world();
  raptor_parser *parser = raptor_new_parser(world, "ntriples");

  raptor_parser_set_statement_handler(
      parser, (void *)&radix_tree, (raptor_statement_handler)statement_handler);

  raptor_parser_parse_start(parser, nullptr);

  std::ifstream ifstream(input_file_path, std::ifstream::binary);
  std::vector<char> buffer(4096, 0);
  while (ifstream.read(buffer.data(), buffer.size())) {
    raptor_parser_parse_chunk(parser,
                              reinterpret_cast<unsigned char *>(buffer.data()),
                              (size_t)ifstream.gcount(), 0);
  }

  std::cout << "Done. \t";
  print_stats();

  raptor_free_parser(parser);
  raptor_free_world(world);

  std::ofstream ofstream(output_file_path, std::ofstream::binary);
  radix_tree.serialize(ofstream);
}

void statement_handler(void *radix_tree_ptr,
                       const raptor_statement *statement) {
  auto &radix_tree = *reinterpret_cast<RadixTree<> *>(radix_tree_ptr);
  raptor_term *subject = statement->subject;
  raptor_term *predicate = statement->predicate;
  raptor_term *object = statement->object;

  auto predicate_value = get_term_value(predicate);
  auto subject_value = get_term_value(subject);
  auto object_value = get_term_value(object);

  auto predicate_lookup = radix_tree.lookup(predicate_value);
  if (!predicate_lookup.was_found()) {
    radix_tree.insert(predicate_value);
    bytes_stored += predicate_value.size();
    unique_strings_processed++;
  }

  auto subject_lookup = radix_tree.lookup(subject_value);
  if (!subject_lookup.was_found()) {
    radix_tree.insert(subject_value);
    bytes_stored += subject_value.size();
    unique_strings_processed++;
  }

  auto object_lookup = radix_tree.lookup(object_value);
  if (!object_lookup.was_found()) {
    radix_tree.insert(object_value);
    bytes_stored += object_value.size();
    unique_strings_processed++;
  }

  strings_processed += 3;
  strings_processed_reset_th += 3;
  bytes_processed +=
      predicate_value.size() + subject_value.size() + object_value.size();

  if (strings_processed_reset_th >= RESET_TH) {
    strings_processed_reset_th %= RESET_TH;
    print_stats();
  }
}

void print_stats() {
  std::cout << "Strings processed: " << strings_processed << "\t"
            << "Unique strings processed: " << unique_strings_processed << "\t"
            << "MBytes processed: " << bytes_processed / 1'000'000 << "\t"
            << "MBytes stored (virtually): " << bytes_stored / 1'000'000
            << std::endl;
}

std::string get_term_value(raptor_term *term) {
  char *value;
  size_t value_sz;
  value =
      reinterpret_cast<char *>(raptor_term_to_counted_string(term, &value_sz));
  return std::string(value, value_sz);
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "f:o:";
  struct option long_options[] = {
      {"input-file", required_argument, nullptr, 'f'},
      {"output-file", required_argument, nullptr, 'o'}};

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
    case 'f':
      out.input_file = optarg;
      has_input = true;
      break;
    case 'o':
      out.output_file = optarg;
      has_output = true;
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

  return out;
}

void print_help() {
  std::cout << "--input-file\t(-f)\t\t(string-required)\n"
            << "--output-file\t(-o)\t\t(string-required)\n"
            << std::endl;
}
