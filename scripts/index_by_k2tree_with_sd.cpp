//
// Created by Cristobal Miranda, 2020
//

#include <fstream>
#include <iostream>
#include <string>

#include <PredicatesCacheManager.hpp>
#include <SDEntitiesMapping.hpp>
#include <getopt.h>

#include <StringDictionaryHTFC.h>
#include <StringDictionaryPFC.h>

#include <raptor2.h>
#include <raptor_util.hpp>

struct parsed_options {
  std::string subjects_sd_file;
  std::string predicates_sd_file;
  std::string objects_sd_file;

  std::string nt_file;

  std::string output_k2tree;
};

unsigned long bytes_processed = 0;
unsigned long strings_processed = 0;
unsigned long strings_processed_reset_th = 0;
const unsigned long RESET_TH = 1000000;

parsed_options parse_cmline(int argc, char **argv);
void print_help();
void process_nt_file(PredicatesCacheManager &pcm, std::ifstream &nt_ifs);
void statement_handler(void *pcm_ptr, const raptor_statement *statement);
void print_stats();

int main(int argc, char **argv) {
  parsed_options parsed = parse_cmline(argc, argv);

  std::unique_ptr<ISDManager> isd_manager;
  {
    std::ifstream ifs_s(parsed.subjects_sd_file, std::ios::binary);
    std::ifstream ifs_p(parsed.predicates_sd_file, std::ios::binary);
    std::ifstream ifs_o(parsed.objects_sd_file, std::ios::binary);
    isd_manager = std::make_unique<SDEntitiesMapping<
        StringDictionaryPFC, StringDictionaryPFC, StringDictionaryHTFC>>(
        ifs_s, ifs_p, ifs_o);
  }

  PredicatesCacheManager cache_manager(std::move(isd_manager));

  std::ifstream ifs_nt(parsed.nt_file);

  process_nt_file(cache_manager, ifs_nt);

  cache_manager.get_predicates_cache().dump_to_file(parsed.output_k2tree);
}

void statement_handler(void *pcm_ptr, const raptor_statement *statement) {
  auto &pcm = *reinterpret_cast<PredicatesCacheManager *>(pcm_ptr);

  raptor_term *predicate = statement->predicate;
  raptor_term *subject = statement->subject;
  raptor_term *object = statement->object;

  auto predicate_value = get_term_value(predicate);
  auto subject_value = get_term_value(subject);
  auto object_value = get_term_value(object);

  pcm.add_triple(subject_value, predicate_value, object_value);

  strings_processed += 3;
  strings_processed_reset_th += 3;
  bytes_processed +=
      predicate_value.size() + subject_value.size() + object_value.size();

  if (strings_processed_reset_th >= RESET_TH) {
    strings_processed_reset_th %= RESET_TH;
    print_stats();
  }
}

void process_nt_file(PredicatesCacheManager &pcm, std::ifstream &nt_ifs) {
  raptor_world *world = raptor_new_world();
  raptor_parser *parser = raptor_new_parser(world, "ntriples");

  raptor_parser_set_statement_handler(
      parser, (void *)&pcm, (raptor_statement_handler)statement_handler);

  raptor_parser_parse_start(parser, nullptr);

  std::vector<char> buffer(4096, 0);
  while (nt_ifs.read(buffer.data(), buffer.size())) {
    raptor_parser_parse_chunk(parser,
                              reinterpret_cast<unsigned char *>(buffer.data()),
                              (size_t)nt_ifs.gcount(), 0);
  }
  raptor_parser_parse_chunk(parser, NULL, 0, 1);

  raptor_free_parser(parser);
  raptor_free_world(world);
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "s:p:o:n:k:";
  struct option long_options[] = {
      {"subjects-set-file", required_argument, nullptr, 's'},
      {"predicates-set-file", required_argument, nullptr, 'p'},
      {"objects-set-file", required_argument, nullptr, 'o'},
      {"nt-file", required_argument, nullptr, 'n'},
      {"output-k2tree", required_argument, nullptr, 'k'},
  };

  int opt, opt_index;

  bool has_subjects = false;
  bool has_predicates = false;
  bool has_objects = false;
  bool has_nt = false;
  bool has_output = false;

  parsed_options out{};
  while ((
      opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }

    switch (opt) {
    case 's':
      out.subjects_sd_file = optarg;
      has_subjects = true;
      break;
    case 'p':
      out.predicates_sd_file = optarg;
      has_predicates = true;
      break;
    case 'o':
      out.objects_sd_file = optarg;
      has_objects = true;
      break;
    case 'n':
      out.nt_file = optarg;
      has_nt = true;
      break;
    case 'k':
      out.output_k2tree = optarg;
      has_output = true;
      break;
    case 'h': // to implement
    case '?':
    default:
      print_help();
      break;
    }
  }

  if (!has_subjects) {
    std::cerr << "Missing option --subjects-sd-file\n" << std::endl;
    print_help();
    exit(1);
  }

  if (!has_predicates) {
    std::cerr << "Missing option --predicates-sd-file\n" << std::endl;
    print_help();
    exit(1);
  }

  if (!has_objects) {
    std::cerr << "Missing option --objects-sd-file\n" << std::endl;
    print_help();
    exit(1);
  }

  if (!has_nt) {
    std::cerr << "Missing option --nt-file\n" << std::endl;
    print_help();
    exit(1);
  }

  if (!has_output) {
    std::cerr << "Missing option --output-k2tree\n" << std::endl;
    print_help();
    exit(1);
  }

  return out;
}

void print_help() {
  std::cout << "--subjects-sd-file\t(-s)\t\t(string-required)\n"
            << "--predicates-sd-file\t(-p)\t\t(string-required)\n"
            << "--objects-sd-file\t(-o)\t\t(string-required)\n"
            << "--nt-file\t(-n)\t\t(string-required)\n"
            << "--output-k2tree\t(-k)\t\t(string-required)\n"
            << std::endl;
}

void print_stats() {
  std::cout << "Strings processed: " << strings_processed << "\t"
            << "MBytes processed: " << bytes_processed / 1'000'000 << "\t"
            << std::endl;
}