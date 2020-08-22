//
// Created by Cristobal Miranda, 2020
//

#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <string>

#include <RadixTree.hpp>

#include <StringDictionaryHASHRPDACBlocks.h>
#include <StringDictionaryPFC.h>
#include <raptor2.h>
#include <raptor_util.hpp>

#include <base64.h>

namespace fs = std::filesystem;

enum FailMode { STOP = 0, LOG = 1 };

struct parsed_options {
  std::string subjects_sd_file;
  std::string predicates_sd_file;
  std::string objects_sd_file;

  std::string input_nt_file;

  std::string log_location;

  bool base64;
  bool encode_b64output;

  FailMode fail_mode;
};

struct SDHolder {
  StringDictionary *subjects_sd;
  StringDictionary *predicates_sd;
  StringDictionary *objects_sd;

  bool base64;
  bool encode_b64output;
  FailMode fail_mode;

  std::fstream *fail_subs;
  std::fstream *fail_preds;
  std::fstream *fail_objs;

  SDHolder(StringDictionary *subjects_sd, StringDictionary *predicates_sd,
           StringDictionary *objects_sd, bool base64, bool encode_b64output,
           FailMode fail_mode)
      : subjects_sd(subjects_sd), predicates_sd(predicates_sd),
        objects_sd(objects_sd), base64(base64),
        encode_b64output(encode_b64output), fail_mode(fail_mode) {}
};

parsed_options parse_cmline(int argc, char **argv);

void print_help();

void process_nt_file(SDHolder &sd_holder, std::ifstream &nt_ifs);

void statement_handler(void *radix_trees_holder_ptr,
                       const raptor_statement *statement);

void print_stats();

unsigned long bytes_processed = 0;
unsigned long strings_processed = 0;
unsigned long strings_processed_reset_th = 0;

const unsigned long RESET_TH = 1000000;

unsigned long failed_subjs = 0;
unsigned long failed_preds = 0;
unsigned long failed_objs = 0;

int main(int argc, char **argv) {
  parsed_options p_options = parse_cmline(argc, argv);

  std::cout << "Subjects file: " << p_options.subjects_sd_file << "\n"
            << "Predicates file: " << p_options.predicates_sd_file << "\n"
            << "Objects file: " << p_options.objects_sd_file << std::endl;

  std::ifstream sub_ifs(p_options.subjects_sd_file, std::ios::binary);
  std::ifstream pred_ifs(p_options.predicates_sd_file, std::ios::binary);
  std::ifstream obj_ifs(p_options.objects_sd_file, std::ios::binary);
  std::ifstream nt_ifs(p_options.input_nt_file, std::ios::binary);

  if (sub_ifs.fail()) {
    std::cerr << "Error opening subjects file " << p_options.subjects_sd_file
              << std::endl;
    exit(1);
  }

  if (pred_ifs.fail()) {
    std::cerr << "Error opening predicates file "
              << p_options.predicates_sd_file << std::endl;
    exit(1);
  }

  if (obj_ifs.fail()) {
    std::cerr << "Error opening objects file " << p_options.objects_sd_file
              << std::endl;
    exit(1);
  }

  if (nt_ifs.fail()) {
    std::cerr << "Error opening NT file " << p_options.input_nt_file
              << std::endl;
    exit(1);
  }

  auto subj_sd =
      std::unique_ptr<StringDictionary>(StringDictionaryPFC::load(sub_ifs));
  auto pred_sd =
      std::unique_ptr<StringDictionary>(StringDictionaryPFC::load(pred_ifs));
  auto obj_sd = std::unique_ptr<StringDictionary>(
      StringDictionaryHASHRPDACBlocks::load(obj_ifs));

  SDHolder sd_holder(subj_sd.get(), pred_sd.get(), obj_sd.get(),
                     p_options.base64, p_options.encode_b64output,
                     p_options.fail_mode);

  std::unique_ptr<std::fstream> fail_subj, fail_pred, fail_obj;

  if (p_options.fail_mode == LOG) {

    fs::path basepath(p_options.log_location);
    fs::path subj_file_rel("_validate_sd_from_nt.subj.not-found.log");
    fs::path pred_file_rel("_validate_sd_from_nt.pred.not-found.log");
    fs::path obj_file_rel("_validate_sd_from_nt.obj.not-found.log");
    auto subj_path = basepath / subj_file_rel;
    auto pred_path = basepath / pred_file_rel;
    auto obj_path = basepath / obj_file_rel;

    fail_subj = std::make_unique<std::fstream>(subj_path.string(),
                                               std::ios::out | std::ios::trunc);
    fail_pred = std::make_unique<std::fstream>(pred_path.string(),
                                               std::ios::out | std::ios::trunc);
    fail_obj = std::make_unique<std::fstream>(obj_path.string(),
                                              std::ios::out | std::ios::trunc);
    sd_holder.fail_subs = fail_subj.get();
    sd_holder.fail_preds = fail_pred.get();
    sd_holder.fail_objs = fail_obj.get();
  }

  process_nt_file(sd_holder, nt_ifs);

  return 0;
}

void process_nt_file(SDHolder &sd_holder, std::ifstream &nt_ifs) {
  raptor_world *world = raptor_new_world();
  raptor_parser *parser = raptor_new_parser(world, "ntriples");

  raptor_parser_set_statement_handler(
      parser, (void *)&sd_holder, (raptor_statement_handler)statement_handler);

  raptor_parser_parse_start(parser, nullptr);

  std::vector<char> buffer(4096, 0);
  while (nt_ifs.read(buffer.data(), buffer.size())) {
    raptor_parser_parse_chunk(parser,
                              reinterpret_cast<unsigned char *>(buffer.data()),
                              (size_t)nt_ifs.gcount(), 0);
  }
  raptor_parser_parse_chunk(parser, NULL, 0, 1);

  if (failed_preds == 0 && failed_subjs == 0 && failed_objs == 0) {
    std::cout << "Done. Valid File!\n";
  } else {
    std::cout << "Values missing, subjects " << failed_subjs << ", predicates "
              << failed_preds << ", objects " << failed_objs << "\n";
  }

  print_stats();

  raptor_free_parser(parser);
  raptor_free_world(world);
}

static inline std::string get_term_b64_cond(raptor_term *t, bool base64) {
  if (base64) {
    return base64_encode(get_term_value(t));
  }

  return get_term_value(t);
}

void statement_handler(void *sd_holder_ptr, const raptor_statement *statement) {
  auto &sd_holder = *reinterpret_cast<SDHolder *>(sd_holder_ptr);

  FailMode fail_mode = sd_holder.fail_mode;

  auto *subjects_sd = sd_holder.subjects_sd;
  auto *predicates_sd = sd_holder.predicates_sd;
  auto *objects_sd = sd_holder.objects_sd;

  raptor_term *subject = statement->subject;
  raptor_term *predicate = statement->predicate;
  raptor_term *object = statement->object;

  auto subject_value = get_term_b64_cond(subject, sd_holder.base64);
  auto predicate_value = get_term_b64_cond(predicate, sd_holder.base64);
  auto object_value = get_term_b64_cond(object, sd_holder.base64);

  auto subj_id = subjects_sd->locate(
      reinterpret_cast<unsigned char *>(subject_value.data()),
      subject_value.size());
  if (subj_id == NORESULT) {
    failed_subjs++;
    switch (fail_mode) {
    case STOP:
      std::cerr << "Subject '" << subject_value
                << "' Not found on subjects sd (" << subj_id << ")"
                << std::endl;
      exit(1);
    case LOG:
      if (sd_holder.encode_b64output) {
        *sd_holder.fail_subs << base64_encode(subject_value) << "\n";
      } else {
        *sd_holder.fail_subs << subject_value << "\n";
      }

      break;
    }
  }

  auto pred_id = predicates_sd->locate(
      reinterpret_cast<unsigned char *>(predicate_value.data()),
      predicate_value.size());
  if (pred_id == NORESULT) {
    failed_preds++;
    switch (fail_mode) {
    case STOP:
      std::cerr << "Predicate '" << predicate_value
                << "' Not found on predicates sd" << std::endl;
      exit(1);
    case LOG:
      if (sd_holder.encode_b64output) {
        *sd_holder.fail_preds << base64_encode(predicate_value) << "\n";
      } else {
        *sd_holder.fail_preds << predicate_value << "\n";
      }

      break;
    }
  }

  auto obj_id =
      objects_sd->locate(reinterpret_cast<unsigned char *>(object_value.data()),
                         object_value.size());
  if (obj_id == NORESULT) {
    failed_objs++;
    switch (fail_mode) {
    case STOP:
      std::cerr << "Object '" << object_value << "' Not found on objects sd"
                << std::endl;
      exit(1);
    case LOG:
      if (sd_holder.encode_b64output) {
        *sd_holder.fail_objs << base64_encode(object_value) << "\n";
      } else {
        *sd_holder.fail_objs << object_value << "\n";
      }

      break;
    }
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

parsed_options parse_cmline(int argc, char **argv) {
  const char short_options[] = "s:p:o:n:bem::l::";
  struct option long_options[] = {
      {"subjects-set-file", required_argument, nullptr, 's'},
      {"predicates-set-file", required_argument, nullptr, 'p'},
      {"objects-set-file", required_argument, nullptr, 'o'},
      {"nt-file", required_argument, nullptr, 'n'},
      {"base64", optional_argument, nullptr, 'b'},
      {"encode-base64-output", optional_argument, nullptr, 'e'},
      {"fail-mode", optional_argument, nullptr, 'm'},
      {"log-location", optional_argument, nullptr, 'l'},
  };

  int opt, opt_index;

  bool has_subjects = false;
  bool has_predicates = false;
  bool has_objects = false;
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

  return out;
}

void print_help() {
  std::cout
      << "--subjects-sd-file\t(-s)\t\t(string-required)\n"
      << "--predicates-sd-file\t(-p)\t\t(string-required)\n"
      << "--objects-sd-file\t(-o)\t\t(string-required)\n"
      << "--nt-file\t(-n)\t\t(string-required)\n"
      << "--base64\t(-b)\t\t(bool-optional, default=false)\n"
      << "--fail-mode\t(-m)\t\t(string-optional(STOP, LOG), default=STOP)\n"
      << std::endl;
}
