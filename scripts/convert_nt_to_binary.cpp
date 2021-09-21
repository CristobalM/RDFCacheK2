
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include <getopt.h>
#include <nt_parser.hpp>
#include <serialization_util.hpp>
#include <triple_external_sort.hpp>

#include <ISDManager.hpp>
#include <SDWrapper.hpp>
#include <StringDictionaryHASHRPDACBlocks.h>
#include <StringDictionaryPFC.h>

struct parsed_options {
  std::string input_file;
  std::string output_file;
  std::string iris_sd_file;
  std::string blanks_sd_file;
  std::string literals_sd_file;
};

struct FSHolder {
  std::ofstream &ofs;
  std::unique_ptr<ISDManager> sd_manager;
  uint64_t &triples_count;
  FSHolder(std::ofstream &ofs, std::unique_ptr<ISDManager> &&sd_manager,
           uint64_t &triples_count)
      : ofs(ofs), sd_manager(std::move(sd_manager)),
        triples_count(triples_count) {}
};

void print_help();
void processor(NTTriple *ntriple, void *fs_holder_ptr);
std::unique_ptr<ISDManager> read_sds(parsed_options &options);
parsed_options parse_cmline(int argc, char **argv);

int main(int argc, char **argv) {
  uint64_t triples_count = 0;

  auto parsed = parse_cmline(argc, argv);

  auto sd_manager = read_sds(parsed);

  std::ofstream ofs(parsed.output_file,
                    std::ios::out | std::ios::binary | std::ios::trunc);
  FSHolder fs_holder(ofs, std::move(sd_manager), triples_count);

  auto start = ofs.tellp();
  write_u64(ofs, triples_count);

  std::ifstream ifs(parsed.input_file, std::ios::in);
  NTParser ntparser(&ifs, processor, &fs_holder);
  ntparser.parse();

  ofs.seekp(start);
  write_u64(ofs, triples_count);
}

uint64_t get_resource_index(NTRes res, ISDManager &isd_manager) {
  switch (res.type) {
  case RDFType::IRI:
    return isd_manager.iris_index(std::string(res.data));
  case RDFType::LITERAL:
    return isd_manager.literals_index(std::string(res.data));
  case RDFType::BLANK_NODE:
    return isd_manager.blanks_index(std::string(res.data));
  default:
    return NORESULT;
  }
}

void processor(NTTriple *ntriple, void *fs_holder_ptr) {
  auto &h = *reinterpret_cast<FSHolder *>(fs_holder_ptr);

  uint64_t subject_index = get_resource_index(ntriple->subject, *h.sd_manager);
  uint64_t predicate_index =
      get_resource_index(ntriple->predicate, *h.sd_manager);
  uint64_t object_index = get_resource_index(ntriple->object, *h.sd_manager);

  TripleValue triple(subject_index, predicate_index, object_index);
  triple.write_to_file(h.ofs);
  h.triples_count++;
}

std::unique_ptr<ISDManager> read_sds(parsed_options &parsed) {

  std::unique_ptr<ISDManager> isd_manager;

  std::ifstream ifs_iris(parsed.iris_sd_file, std::ios::in | std::ios::binary);
  std::ifstream ifs_blanks(parsed.blanks_sd_file,
                           std::ios::in | std::ios::binary);
  std::ifstream ifs_literals(parsed.literals_sd_file,
                             std::ios::in | std::ios::binary);
  if (ifs_iris.fail()) {
    std::stringstream ss;
    ss << "Failed to open iris file '" << parsed.iris_sd_file << "'";
    throw std::runtime_error(ss.str());
  }

  if (ifs_blanks.fail()) {
    std::stringstream ss;
    ss << "Failed to open blanks file '" << parsed.blanks_sd_file << "'";

    throw std::runtime_error(ss.str());
  }
  if (ifs_literals.fail()) {
    std::stringstream ss;
    ss << "Failed to open literals file '" << parsed.literals_sd_file << "'";
    throw std::runtime_error(ss.str());
  }

  isd_manager =
      std::make_unique<SDWrapper<StringDictionaryPFC, StringDictionaryPFC,
                                 StringDictionaryHASHRPDACBlocks>>(
          ifs_iris, ifs_blanks, ifs_literals);

  return isd_manager;
}

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "i:b:l:I:O:";
  struct option long_options[] = {
      {"iris-sd-file", required_argument, nullptr, 'i'},
      {"blanks-sd-file", required_argument, nullptr, 'b'},
      {"literals-sd-file", required_argument, nullptr, 'l'},
      {"input-file", required_argument, nullptr, 'I'},
      {"output-file", required_argument, nullptr, 'O'},
  };

  int opt, opt_index;

  bool has_iris = false;
  bool has_blanks = false;
  bool has_literals = false;
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
    case 'I':
      out.input_file = optarg;
      has_input = true;
      break;
    case 'O':
      out.output_file = optarg;
      has_output = optarg;
      break;
    default:
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

  if (!has_input)
    throw std::runtime_error("input-file (i) argument is required");
  if (!has_output)
    throw std::runtime_error("output-file (o) argument is required");

  return out;
}

void print_help() {
  std::cout << "--iris-sd-file\t(-i)\t\t(string-required)\n"
            << "--blanks-sd-file\t(-b)\t\t(string-required)\n"
            << "--literals-sd-file\t(-l)\t\t(string-required)\n"
            << "--input-file\t(-I)\t\t(string-required)\n"
            << "--output-file\t(-O)\t\t(string-required)\n"
            << std::endl;
}
