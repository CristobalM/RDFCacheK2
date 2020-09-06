//
// Created by Cristobal Miranda, 2020
//

#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <string>

#include <StringDictionaryHASHRPDACBlocks.h>
#include <StringDictionaryPFC.h>

#include <base64.h>

#include <nt_parser.hpp>

namespace fs = std::filesystem;

enum FailMode { STOP = 0, LOG = 1 };

struct parsed_options {
  std::string iris_sd_file;
  std::string blanks_sd_file;
  std::string literals_sd_file;

  std::string input_nt_file;

  std::string log_location;

  bool base64;
  bool encode_b64output;

  FailMode fail_mode;
};

struct SDHolder {
  StringDictionary *iris_sd;
  StringDictionary *blanks_sd;
  StringDictionary *literals_sd;

  bool base64;
  bool encode_b64output;
  FailMode fail_mode;

  std::fstream *fail_iris;
  std::fstream *fail_blanks;
  std::fstream *fail_literals;

  SDHolder(StringDictionary *iris_sd, StringDictionary *blanks_sd,
           StringDictionary *literals_sd, bool base64, bool encode_b64output,
           FailMode fail_mode)
      : iris_sd(iris_sd), blanks_sd(blanks_sd), literals_sd(literals_sd),
        base64(base64), encode_b64output(encode_b64output),
        fail_mode(fail_mode) {}
};

parsed_options parse_cmline(int argc, char **argv);

void print_help();

void process_nt_file(SDHolder &sd_holder, std::ifstream &nt_ifs);
void print_stats();
void processor(NTTriple *ntriple, void *sd_holder_ptr);

unsigned long bytes_processed = 0;
unsigned long strings_processed = 0;
unsigned long strings_processed_reset_th = 0;

const unsigned long RESET_TH = 1000000;

unsigned long failed_iris = 0;
unsigned long failed_blanks = 0;
unsigned long failed_literals = 0;

int main(int argc, char **argv) {
  parsed_options p_options = parse_cmline(argc, argv);

  std::cout << "IRIS file: " << p_options.iris_sd_file << "\n"
            << "Blank nodes file: " << p_options.blanks_sd_file << "\n"
            << "Literals file: " << p_options.literals_sd_file << std::endl;

  std::ifstream iris_ifs(p_options.iris_sd_file, std::ios::binary);
  std::ifstream blanks_ifs(p_options.blanks_sd_file, std::ios::binary);
  std::ifstream literals_ifs(p_options.literals_sd_file, std::ios::binary);
  std::ifstream nt_ifs(p_options.input_nt_file, std::ios::binary);

  if (iris_ifs.fail()) {
    std::cerr << "Error opening iris file " << p_options.iris_sd_file
              << std::endl;
    exit(1);
  }

  if (blanks_ifs.fail()) {
    std::cerr << "Error opening blanks file " << p_options.blanks_sd_file
              << std::endl;
    exit(1);
  }

  if (literals_ifs.fail()) {
    std::cerr << "Error opening literals file " << p_options.literals_sd_file
              << std::endl;
    exit(1);
  }

  if (nt_ifs.fail()) {
    std::cerr << "Error opening NT file " << p_options.input_nt_file
              << std::endl;
    exit(1);
  }

  auto iris_sd =
      std::unique_ptr<StringDictionary>(StringDictionaryPFC::load(iris_ifs));
  auto blanks_sd =
      std::unique_ptr<StringDictionary>(StringDictionaryPFC::load(blanks_ifs));
  auto literals_sd = std::unique_ptr<StringDictionary>(
      StringDictionaryHASHRPDACBlocks::load(literals_ifs));

  SDHolder sd_holder(iris_sd.get(), blanks_sd.get(), literals_sd.get(),
                     p_options.base64, p_options.encode_b64output,
                     p_options.fail_mode);

  std::unique_ptr<std::fstream> fail_iris, fail_blanks, fail_literals;

  if (p_options.fail_mode == LOG) {

    fs::path basepath(p_options.log_location);
    fs::path iris_file_rel("_validate_sd_from_nt.iris.not-found.log");
    fs::path blanks_file_rel("_validate_sd_from_nt.blanks.not-found.log");
    fs::path literals_file_rel("_validate_sd_from_nt.objects.not-found.log");
    auto iris_path = basepath / iris_file_rel;
    auto blanks_path = basepath / blanks_file_rel;
    auto literals_path = basepath / literals_file_rel;

    fail_iris = std::make_unique<std::fstream>(iris_path.string(),
                                               std::ios::out | std::ios::trunc);
    fail_blanks = std::make_unique<std::fstream>(blanks_path.string(),
                                               std::ios::out | std::ios::trunc);
    fail_literals = std::make_unique<std::fstream>(literals_path.string(),
                                              std::ios::out | std::ios::trunc);
    sd_holder.fail_iris = fail_iris.get();
    sd_holder.fail_blanks = fail_blanks.get();
    sd_holder.fail_literals = fail_literals.get();
  }

  process_nt_file(sd_holder, nt_ifs);

  return 0;
}

void process_nt_file(SDHolder &sd_holder, std::ifstream &nt_ifs) {
  NTParser ntparser(&nt_ifs, processor, &sd_holder);
  ntparser.parse();

  if (failed_blanks == 0 && failed_iris == 0 && failed_literals == 0) {
    std::cout << "Done. Valid File!\n";
  } else {
    std::cout << "Values missing, IRIS " << failed_iris << ", Blanks "
              << failed_blanks << ", Literals " << failed_literals << "\n";
  }

  print_stats();

}

static inline std::string get_term_b64_cond(NTRes *res, bool base64) {
  if (base64) {
    return base64_encode(std::string(res->data));
  }

  return std::string(res->data);
}

void not_found_res_handler(unsigned long result, unsigned long &fail_counter,
                           RDFType rdf_type, const std::string &target_string,
                           FailMode fail_mode, SDHolder &sd_holder) {
  if (result == NORESULT) {
    fail_counter++;
    switch (fail_mode) {
    case STOP:
      switch (rdf_type) {
      case IRI:
        std::cerr << "IRI";
        break;
      case BLANK_NODE:
        std::cerr << "BLANK NODE";
        break;
      case LITERAL:
        std::cerr << "LITERAL";
        break;
      }
      std::cerr << " '" << target_string
                << "' Not found on string dictionary" <<
          std::endl;
      exit(1);
    case LOG:
      if (sd_holder.encode_b64output) {
        *sd_holder.fail_iris << base64_encode(target_string) << "\n";
      } else {
        *sd_holder.fail_iris << target_string << "\n";
      }

      break;
    }
  }
}

unsigned long cond_locate(SDHolder &sd_holder, std::string &data, RDFType type ) {
  auto *udata = reinterpret_cast<unsigned char *>(data.data());
  unsigned long result;
  switch (type) {
  case IRI:
    result = sd_holder.iris_sd->locate(udata, data.size());
    not_found_res_handler(result, failed_iris, IRI, data, sd_holder.fail_mode, sd_holder);
    return result;
  case BLANK_NODE:
    result = sd_holder.blanks_sd->locate(udata, data.size());
    not_found_res_handler(result, failed_blanks, BLANK_NODE, data, sd_holder.fail_mode, sd_holder);
    return result;
  case LITERAL:
    result = sd_holder.literals_sd->locate(udata, data.size());
    not_found_res_handler(result, failed_literals, LITERAL, data, sd_holder.fail_mode, sd_holder);
    return result;
    break;
  }
  return 0;
}

void processor(NTTriple *ntriple, void *sd_holder_ptr) {
  auto &sd_holder = *reinterpret_cast<SDHolder *>(sd_holder_ptr);
  auto subject = get_term_b64_cond(&ntriple->subject, sd_holder.base64);
  auto predicate = get_term_b64_cond(&ntriple->predicate, sd_holder.base64);
  auto object = get_term_b64_cond(&ntriple->object, sd_holder.base64);
  cond_locate(sd_holder, subject, ntriple->subject.type);
  cond_locate(sd_holder, predicate, ntriple->predicate.type);
  cond_locate(sd_holder, object, ntriple->object.type);


  strings_processed += 3;
  strings_processed_reset_th += 3;
  bytes_processed +=
      subject.size() + predicate.size() + object.size();

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

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "i:l:b:n:6em::g::";
  struct option long_options[] = {
      {"iris-sd-file", required_argument, nullptr, 'i'},
      {"literals-sd-file", required_argument, nullptr, 'l'},
      {"blanks-sd-file", required_argument, nullptr, 'b'},
      {"nt-file", required_argument, nullptr, 'n'},
      {"base64", optional_argument, nullptr, '6'},
      {"encode-base64-output", optional_argument, nullptr, 'e'},
      {"fail-mode", optional_argument, nullptr, 'm'},
      {"log-location", optional_argument, nullptr, 'g'},
  };

  int opt, opt_index;

  bool has_iris = false;
  bool has_blanks = false;
  bool has_literals = false;
  bool has_nt = false;

  parsed_options out{};

  out.base64 = false;
  out.fail_mode = STOP;
  out.log_location = "";
  out.encode_b64output = false;
  while ((
      opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }

    switch (opt) {
    case 's':
      out.iris_sd_file = optarg;
      has_iris = true;
      break;
    case 'p':
      out.blanks_sd_file = optarg;
      has_blanks = true;
      break;
    case 'o':
      out.literals_sd_file = optarg;
      has_literals = true;
      break;
    case 'n':
      out.input_nt_file = optarg;
      has_nt = true;
      break;
    case 'b':
      out.base64 = true;
      break;
    case 'e':
      out.encode_b64output = true;
      break;
    case 'm':
      if (optarg) {
        std::string fmodes(optarg);
        if (fmodes == "LOG") {
          out.fail_mode = LOG;
        } else if (fmodes == "STOP") {
          out.fail_mode = STOP;
        } else {
          throw std::runtime_error("Invalid fail mode: '" + fmodes + "'");
        }
      }
      break;
    case 'l':
      if (optarg) {
        out.log_location = optarg;
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

  return out;
}

void print_help() {
  std::cout
      << "--iris-sd-file\t\t(-i)\t\t(string-required)\n"
      << "--literals-sd-file\t(-l)\t\t(string-required)\n"
      << "--blanks-sd-file\t(-b)\t\t(string-required)\n"
      << "--nt-file\t\t(-n)\t\t(string-required)\n"
      << "--base64\t\t(-6)\t\t(bool-optional, default=false)\n"
      << "--encode-base64-output\t(-e)\t\t(bool-optional, default=false)\n"
      << "--fail-mode\t\t(-m)\t\t(string-optional(STOP, LOG), default=STOP)\n"
      << "--log-location\t\t(-g)\t\t(string-optional, default=./)\n"
      << std::endl;
}

/*
 {"encode-base64-output", optional_argument, nullptr, 'e'},
      {"fail-mode", optional_argument, nullptr, 'm'},
      {"log-location", optional_argument, nullptr, 'g'},
*/
