//
// Created by Cristobal Miranda, 2020
//

#include <fstream>
#include <iostream>
#include <string>

#include <PredicatesCacheManager.hpp>
#include <SDEntitiesMapping.hpp>
#include <getopt.h>

#include <StringDictionaryHASHRPDACBlocks.h>
#include <StringDictionaryPFC.h>

#include <nt_parser.hpp>

struct parsed_options {
  std::string iris_sd_file;
  std::string blanks_sd_file;
  std::string literals_sd_file;

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
void print_stats();
void processor(NTTriple *ntriple, void *pcm_ptr);

int main(int argc, char **argv) {
  parsed_options parsed = parse_cmline(argc, argv);

  std::unique_ptr<ISDManager> isd_manager;
  {
    std::ifstream ifs_iris(parsed.iris_sd_file,
                        std::ios::in | std::ios::binary);
    std::ifstream ifs_blanks(parsed.blanks_sd_file,
                        std::ios::in | std::ios::binary);
    std::ifstream ifs_literals(parsed.literals_sd_file,
                        std::ios::in | std::ios::binary);
    if (ifs_iris.fail()) {
      std::cerr << "Failed to open iris file '" << parsed.iris_sd_file
                << "'" << std::endl;
      return 1;
    }

    if (ifs_blanks.fail()) {
      std::cerr << "Failed to open blanks file '"
                << parsed.blanks_sd_file << "'" << std::endl;
      return 1;
    }
    if (ifs_literals.fail()) {
      std::cerr << "Failed to open literals file '" << parsed.literals_sd_file
                << "'" << std::endl;
      return 1;
    }

    isd_manager = std::make_unique<
        SDEntitiesMapping<StringDictionaryPFC, StringDictionaryPFC,
                          StringDictionaryHASHRPDACBlocks>>(ifs_iris, ifs_blanks,
                                                            ifs_literals);
  }

  PredicatesCacheManager cache_manager(std::move(isd_manager));

  std::ifstream ifs_nt(parsed.nt_file);

  std::cout << "Processing nt file..." << std::endl;
  process_nt_file(cache_manager, ifs_nt);

  std::cout << "Saving to disk..." << std::endl;

  cache_manager.get_predicates_cache().dump_to_file(parsed.output_k2tree);
  cache_manager.get_dyn_dicts().save("_extra_iris.bin", "_extra_blanks.bin",
                                     "_extra_literals.bin");
}

void processor(NTTriple *ntriple, void *pcm_ptr) {
  auto &pcm = *reinterpret_cast<PredicatesCacheManager *>(pcm_ptr);
  std::string subject(ntriple->subject.data);
  std::string predicate(ntriple->predicate.data);
  std::string object(ntriple->object.data);
  pcm.add_triple(subject, predicate, object);

  strings_processed += 3;
  strings_processed_reset_th += 3;
  bytes_processed += predicate.size() + subject.size() + object.size();

  if (strings_processed_reset_th >= RESET_TH) {
    strings_processed_reset_th %= RESET_TH;
    print_stats();
  }
}

void process_nt_file(PredicatesCacheManager &pcm, std::ifstream &nt_ifs) {
  NTParser ntparser(&nt_ifs, processor, &pcm);
  ntparser.parse();
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "i:l:b:n:k:";
  struct option long_options[] = {
      {"iris-sd-file", required_argument, nullptr, 'i'},
      {"blanks-sd-file", required_argument, nullptr, 'l'},
      {"literals-sd-file", required_argument, nullptr, 'b'},
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
    case 'i':
      out.iris_sd_file = optarg;
      has_subjects = true;
      break;
    case 'l':
      out.blanks_sd_file = optarg;
      has_predicates = true;
      break;
    case 'b':
      out.literals_sd_file = optarg;
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
    std::cerr << "Missing option --iris-sd-file\n" << std::endl;
    print_help();
    exit(1);
  }

  if (!has_predicates) {
    std::cerr << "Missing option --blanks-sd-file\n" << std::endl;
    print_help();
    exit(1);
  }

  if (!has_objects) {
    std::cerr << "Missing option --literals-sd-file\n" << std::endl;
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
  std::cout << "--iris-sd-file\t(-i)\t\t(string-required)\n"
            << "--blanks-sd-file\t(-l)\t\t(string-required)\n"
            << "--literals-sd-file\t(-b)\t\t(string-required)\n"
            << "--nt-file\t(-n)\t\t(string-required)\n"
            << "--output-k2tree\t(-k)\t\t(string-required)\n"
            << std::endl;
}

void print_stats() {
  std::cout << "Strings processed: " << strings_processed << "\t"
            << "MBytes processed: " << bytes_processed / 1'000'000 << "\t"
            << std::endl;
}