//
// Created by Cristobal Miranda, 2020
//

#include <fstream>
#include <getopt.h>
#include <iostream>
#include <string>

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
void statement_handler(void *radix_trees_holder_ptr,
                       const raptor_statement *statement);
std::string get_term_value(raptor_term *term);
void print_stats();

unsigned long bytes_processed = 0;
unsigned long strings_processed = 0;
unsigned long unique_strings_processed = 0;

unsigned long bytes_stored = 0;

unsigned long strings_processed_reset_th = 0;

const unsigned long RESET_TH = 1000000;

struct RadixTreesHolder {
  RadixTree<> &subjects_set;
  RadixTree<> &predicates_set;
  RadixTree<> &objects_set;

  RadixTreesHolder(RadixTree<> &subjects_set, RadixTree<> &predicates_set,
                   RadixTree<> &objects_set)
      : subjects_set(subjects_set), predicates_set(predicates_set),
        objects_set(objects_set) {}
};

int main(int argc, char **argv) {
  parsed_options p_options = parse_cmline(argc, argv);

  process_nt_file(p_options.input_file, p_options.output_file);

  return 0;
}

void process_nt_file(const std::string &input_file_path,
                     const std::string &output_file_path) {
  RadixTree<> subjects_set;
  RadixTree<> predicates_set;
  RadixTree<> objects_set;

  RadixTreesHolder trees_holder(subjects_set, predicates_set, objects_set);
  raptor_world *world = raptor_new_world();
  raptor_parser *parser = raptor_new_parser(world, "ntriples");

  raptor_parser_set_statement_handler(
      parser, (void *)&trees_holder,
      (raptor_statement_handler)statement_handler);

  raptor_parser_parse_start(parser, nullptr);

  std::ifstream ifstream(input_file_path, std::ifstream::binary);

  if (ifstream.fail()) {
    std::cerr << "Error opening NT file " << input_file_path << std::endl;
    exit(1);
  }

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

  {
    std::cout << "Serializing subjects" << std::endl;
    std::ofstream ofstream_subjects(output_file_path + ".subjects.bin",
                                    std::ofstream::binary);
    subjects_set.serialize(ofstream_subjects);
  }

  {
    std::cout << "Serializing predicates" << std::endl;
    std::ofstream ofstream_predicates(output_file_path + ".predicates.bin",
                                      std::ofstream::binary);
    predicates_set.serialize(ofstream_predicates);
  }

  {
    std::cout << "Serializing objects" << std::endl;
    std::ofstream ofstream_objects(output_file_path + ".objects.bin",
                                   std::ofstream::binary);
    objects_set.serialize(ofstream_objects);
  }
}

void statement_handler(void *radix_trees_holder_ptr,
                       const raptor_statement *statement) {
  auto &radix_trees_holder =
      *reinterpret_cast<RadixTreesHolder *>(radix_trees_holder_ptr);

  auto &predicates_set = radix_trees_holder.predicates_set;
  auto &subjects_set = radix_trees_holder.subjects_set;
  auto &objects_set = radix_trees_holder.objects_set;

  raptor_term *predicate = statement->predicate;
  raptor_term *subject = statement->subject;
  raptor_term *object = statement->object;

  auto predicate_value = get_term_value(predicate);
  auto subject_value = get_term_value(subject);
  auto object_value = get_term_value(object);

  auto predicate_lookup = predicates_set.lookup(predicate_value);
  if (!predicate_lookup.was_found()) {
    predicates_set.insert(predicate_value);
    bytes_stored += predicate_value.size();
    unique_strings_processed++;
  }

  auto subject_lookup = subjects_set.lookup(subject_value);
  if (!subject_lookup.was_found()) {
    subjects_set.insert(subject_value);
    bytes_stored += subject_value.size();
    unique_strings_processed++;
  }

  auto object_lookup = objects_set.lookup(object_value);
  if (!object_lookup.was_found()) {
    objects_set.insert(object_value);
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
  auto result = std::string(value, value_sz);
  free(value);

  return result;
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
