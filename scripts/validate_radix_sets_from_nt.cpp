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
  std::string subjects_set_file;
  std::string predicates_set_file;
  std::string objects_set_file;

  std::string input_nt_file;
};

struct RadixTreesHolder {
  const RadixTree<> &subjects_set;
  const RadixTree<> &predicates_set;
  const RadixTree<> &objects_set;

  RadixTreesHolder(RadixTree<> &subjects_set, RadixTree<> &predicates_set,
                   RadixTree<> &objects_set)
          : subjects_set(subjects_set), predicates_set(predicates_set),
            objects_set(objects_set) {}
};

parsed_options parse_cmline(int argc, char **argv);
void print_help();
void process_nt_file(const RadixTreesHolder &r_holder, std::ifstream &nt_ifs);
void statement_handler(void *radix_trees_holder_ptr,
                       const raptor_statement *statement);
std::string get_term_value(raptor_term *term);
void print_stats();

unsigned long bytes_processed = 0;
unsigned long strings_processed = 0;
unsigned long strings_processed_reset_th = 0;

const unsigned long RESET_TH = 1000000;


int main(int argc, char **argv) {
  parsed_options p_options = parse_cmline(argc, argv);

  std::ifstream sub_ifs(p_options.subjects_set_file);
  std::ifstream pred_ifs(p_options.predicates_set_file);
  std::ifstream obj_ifs(p_options.objects_set_file);
  std::ifstream nt_ifs(p_options.input_nt_file);

  if(sub_ifs.fail()){
    std::cerr << "Error opening subjects file " << p_options.subjects_set_file << std::endl;
    exit(1);
  }

  if(pred_ifs.fail()){
    std::cerr << "Error opening predicates file " << p_options.predicates_set_file << std::endl;
    exit(1);
  }

  if(obj_ifs.fail()){
    std::cerr << "Error opening objects file " << p_options.objects_set_file << std::endl;
    exit(1);
  }

  if(nt_ifs.fail()){
    std::cerr << "Error opening NT file " << p_options.input_nt_file << std::endl;
    exit(1);
  }

  RadixTree<> subj_set, pred_set, obj_set;

  subj_set.deserialize(sub_ifs);
  sub_ifs.close();

  pred_set.deserialize(pred_ifs);
  pred_ifs.close();

  obj_set.deserialize(obj_ifs);
  obj_ifs.close();

  RadixTreesHolder r_holder(subj_set, pred_set, obj_set);


  process_nt_file(r_holder, nt_ifs);

  return 0;
}

void process_nt_file(const RadixTreesHolder &r_holder, std::ifstream &nt_ifs){
  raptor_world *world = raptor_new_world();
  raptor_parser *parser = raptor_new_parser(world, "ntriples");

  raptor_parser_set_statement_handler(
          parser, (void *)&r_holder,
          (raptor_statement_handler)statement_handler);

  raptor_parser_parse_start(parser, nullptr);

  std::vector<char> buffer(4096, 0);
  while (nt_ifs.read(buffer.data(), buffer.size())) {
    raptor_parser_parse_chunk(parser,
                              reinterpret_cast<unsigned char *>(buffer.data()),
                              (size_t)nt_ifs.gcount(), 0);
  }

  std::cout << "Done. Valid File!\t";
  print_stats();

  raptor_free_parser(parser);
  raptor_free_world(world);
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
    std::cerr << "Predicate '" << predicate_value << "' Not found on predicates set" << std::endl;
    exit(1);
  }

  auto subject_lookup = subjects_set.lookup(subject_value);
  if (!subject_lookup.was_found()) {
    std::cerr << "Subject '" << subject_value << "' Not found on subjects set" << std::endl;
    exit(1);
  }

  auto object_lookup = objects_set.lookup(object_value);
  if (!object_lookup.was_found()) {
    std::cerr << "Object '" << object_value << "' Not found on objects set" << std::endl;
    exit(1);
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
            << "MBytes processed: " << bytes_processed / 1'000'000 << "\t"
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
  const char short_options[] = "s:p:o";
  struct option long_options[] = {
          {"subjects-set-file", required_argument, nullptr, 's'},
          {"predicates-set-file", required_argument, nullptr, 'p'},
          {"objects-set-file", required_argument, nullptr, 'o'},
          {"nt-file", required_argument, nullptr, 'n'},
  };


  int opt, opt_index;

  bool has_subjects = false;
  bool has_predicates = false;
  bool has_objects = false;
  bool has_nt = false;


  parsed_options out{};
  while ((
          opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }

    switch (opt) {
      case 's':
        out.subjects_set_file = optarg;
        has_subjects = true;
        break;
      case 'p':
        out.predicates_set_file = optarg;
        has_predicates = true;
        break;
      case 'o':
        out.objects_set_file = optarg;
        has_objects = true;
        break;
      case 'n':
        out.input_nt_file = optarg;
        has_nt = true;
        break;
      case 'h': // to implement
      case '?':
      default:
        print_help();
        break;
    }
  }

  if (!has_subjects) {
    std::cerr << "Missing option --subjects-set-file\n" << std::endl;
    print_help();
    exit(1);
  }

  if (!has_predicates) {
    std::cerr << "Missing option --predicates-set-file\n" << std::endl;
    print_help();
    exit(1);
  }

  if (!has_objects) {
    std::cerr << "Missing option --objects-set-file\n" << std::endl;
    print_help();
    exit(1);
  }

  if (!has_nt) {
    std::cerr << "Missing option --nt-file\n" << std::endl;
    print_help();
    exit(1);
  }

  return out;
}

void print_help() {
  std::cout << "--subjects-set-file\t(-s)\t\t(string-required)\n"
            << "--predicates-set-file\t(-p)\t\t(string-required)\n"
            << "--objects-set-file\t(-p)\t\t(string-required)\n"
            << "--nt-file\t(-p)\t\t(string-required)\n"
            << std::endl;
}
