//
// Created by Cristobal Miranda, 2020
//

#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include <PredicatesCacheManager.hpp>
#include <RDFTriple.hpp>
#include <SDEntitiesMapping.hpp>
#include <getopt.h>

#include <StringDictionaryHASHRPDACBlocks.h>
#include <StringDictionaryPFC.h>

#include <nt_parser.hpp>

#include <PredicatesIndexCacheBuilder.hpp>

struct parsed_options {
  std::string iris_sd_file;
  std::string blanks_sd_file;
  std::string literals_sd_file;

  std::string nt_file;

  std::string output_k2tree;

  int workers_pool_size;
  unsigned long max_insertion_queue_size;
};

struct PCMBuilderHolder {
  PredicatesCacheManager &pcm;
  PredicatesIndexCacheBuilder &pcb;
  PCMBuilderHolder(PredicatesCacheManager &pcm,
                   PredicatesIndexCacheBuilder &pcb)
      : pcm(pcm), pcb(pcb) {}
};

unsigned long bytes_processed = 0;
unsigned long strings_processed = 0;
unsigned long strings_processed_reset_th = 0;
const unsigned long RESET_TH = 1000000;

parsed_options parse_cmline(int argc, char **argv);
void print_help();
void process_nt_file(PCMBuilderHolder &pcm, std::ifstream &nt_ifs);
void print_stats();
void processor(NTTriple *ntriple, void *pcmb_holder);

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
      std::cerr << "Failed to open iris file '" << parsed.iris_sd_file << "'"
                << std::endl;
      return 1;
    }

    if (ifs_blanks.fail()) {
      std::cerr << "Failed to open blanks file '" << parsed.blanks_sd_file
                << "'" << std::endl;
      return 1;
    }
    if (ifs_literals.fail()) {
      std::cerr << "Failed to open literals file '" << parsed.literals_sd_file
                << "'" << std::endl;
      return 1;
    }

    isd_manager = std::make_unique<
        SDEntitiesMapping<StringDictionaryPFC, StringDictionaryPFC,
                          StringDictionaryHASHRPDACBlocks>>(
        ifs_iris, ifs_blanks, ifs_literals);
  }

  PredicatesCacheManager cache_manager(std::move(isd_manager));
  PredicatesIndexCacheBuilder pcbuilder(parsed.workers_pool_size,
                                        parsed.max_insertion_queue_size);

  PCMBuilderHolder pcmb_holder(cache_manager, pcbuilder);
  std::ifstream ifs_nt(parsed.nt_file);

  std::cout << "Processing nt file..." << std::endl;
  process_nt_file(pcmb_holder, ifs_nt);

  std::cout << "Saving to disk..." << std::endl;

  cache_manager.get_predicates_cache().dump_to_file(parsed.output_k2tree);
  cache_manager.get_dyn_dicts().save(parsed.output_k2tree +
                                     "_extra_resources.bin");
}

RDFResource resource_from_nres(NTRes *nres) {
  RDFResourceType res_type;
  switch (nres->type) {
  case IRI:
    res_type = RDF_TYPE_IRI;
    break;
  case BLANK_NODE:
    res_type = RDF_TYPE_BLANK;
    break;
  case LITERAL:
    res_type = RDF_TYPE_LITERAL;
    break;
  default:
    throw std::runtime_error("Invalid resource type " +
                             std::to_string(nres->type));
  }
  return RDFResource(std::string(nres->data), res_type);
}

void processor(NTTriple *ntriple, void *pcmb_holder) {
  auto &pcmb = *reinterpret_cast<PCMBuilderHolder *>(pcmb_holder);
  RDFResource object = resource_from_nres(&ntriple->object);
  RDFTripleResource rdf_triple(resource_from_nres(&ntriple->subject),
                               resource_from_nres(&ntriple->predicate),
                               resource_from_nres(&ntriple->object));

  auto &pcm = pcmb.pcm;
  auto &pcb = pcmb.pcb;
  pcm.add_triple(rdf_triple, pcb);

  strings_processed += 3;
  strings_processed_reset_th += 3;
  bytes_processed += rdf_triple.subject.value.size() +
                     rdf_triple.predicate.value.size() +
                     rdf_triple.object.value.size();

  if (strings_processed_reset_th >= RESET_TH) {
    strings_processed_reset_th %= RESET_TH;
    print_stats();
  }
}

void process_nt_file(PCMBuilderHolder &pcmb_holder, std::ifstream &nt_ifs) {
  NTParser ntparser(&nt_ifs, processor, &pcmb_holder);
  ntparser.parse();
  pcmb_holder.pcb.finish();
  pcmb_holder.pcm.replace_index_cache(pcmb_holder.pcb.get());
  std::cout << "Total time inserting into k2tree: "
            << pcmb_holder.pcb.get_measured_insertion_time() << " ns \n"
            << "Total time on string dictionary lookup: "
            << pcmb_holder.pcm.measured_time_sd_lookup << " ns" << std::endl;
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "i:b:l:n:k:w::q::";
  struct option long_options[] = {
      {"iris-sd-file", required_argument, nullptr, 'i'},
      {"blanks-sd-file", required_argument, nullptr, 'b'},
      {"literals-sd-file", required_argument, nullptr, 'l'},
      {"nt-file", required_argument, nullptr, 'n'},
      {"output-k2tree", required_argument, nullptr, 'k'},
      {"worker-pool-size", optional_argument, nullptr, 'w'},
      {"max-insertion-queue-size", optional_argument, nullptr, 'q'},
  };

  int opt, opt_index;

  bool has_iris = false;
  bool has_blanks = false;
  bool has_literals = false;
  bool has_nt = false;
  bool has_output = false;
  bool has_worker_pool_size = false;
  bool has_max_insertion_queue_size = false;

  parsed_options out{};
  while ((
      opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }

    switch (opt) {
    case 'i':
      out.iris_sd_file = optarg;
      has_iris = true;
      break;
    case 'b':
      out.blanks_sd_file = optarg;
      has_blanks = true;
      break;
    case 'l':
      out.literals_sd_file = optarg;
      has_literals = true;
      break;
    case 'n':
      out.nt_file = optarg;
      has_nt = true;
      break;
    case 'k':
      out.output_k2tree = optarg;
      has_output = true;
      break;
    case 'w':
      if (optarg) {
        out.workers_pool_size = std::stoi(std::string(optarg));
        has_worker_pool_size = true;
      }
      break;
    case 'q':
      if (optarg) {
        out.max_insertion_queue_size = std::stoul(std::string(optarg));
        has_max_insertion_queue_size = true;
      }
      break;
    case 'h': // to implement
    case '?':
    default:
      print_help();
      break;
    }
  }

  if (!has_iris) {
    std::cerr << "Missing option --iris-sd-file\n" << std::endl;
    print_help();
    exit(1);
  }

  if (!has_blanks) {
    std::cerr << "Missing option --blanks-sd-file\n" << std::endl;
    print_help();
    exit(1);
  }

  if (!has_literals) {
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

  if (!has_worker_pool_size) {
    out.workers_pool_size = PredicatesCacheManager::DEFAULT_WORKER_POOL_SZ;
  }

  if (!has_max_insertion_queue_size) {
    out.max_insertion_queue_size =
        PredicatesCacheManager::DEFAULT_MAX_QUEUE_SIZE;
  }

  return out;
}

void print_help() {
  std::cout << "--iris-sd-file\t(-i)\t\t(string-required)\n"
            << "--blanks-sd-file\t(-b)\t\t(string-required)\n"
            << "--literals-sd-file\t(-l)\t\t(string-required)\n"
            << "--nt-file\t(-n)\t\t(string-required)\n"
            << "--output-k2tree\t(-k)\t\t(string-required)\n"
            << std::endl;
}

void print_stats() {
  std::cout << "Strings processed: " << strings_processed << "\t"
            << "MBytes processed: " << bytes_processed / 1'000'000 << "\t"
            << std::endl;
}