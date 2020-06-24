//
// Created by Cristobal Miranda, 2020
//

#include <algorithm>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include <raptor2.h>
#include <raptor_util.hpp>

#define strtk_no_tr1_or_boost
#include <base64.h>

struct parsed_options {
  std::string input_file;
  std::string output_file;
  unsigned long buffer_sz;
};

struct FsHolder {
  std::ofstream &sub_ofs;
  std::ofstream &pred_ofs;
  std::ofstream &obj_ofs;

  unsigned long buf_size;

  FsHolder(std::ofstream &sub_ofs, std::ofstream &pred_ofs,
           std::ofstream &obj_ofs, unsigned long buf_size)
      : sub_ofs(sub_ofs), pred_ofs(pred_ofs), obj_ofs(obj_ofs),
        buf_size(buf_size) {}
};

parsed_options parse_cmline(int argc, char **argv);
void print_help();
void process_nt_file(FsHolder &fs_holder, std::ifstream &nt_ifs);
void statement_handler(void *fs_holder_ptr, const raptor_statement *statement);
void print_stats();

unsigned long bytes_processed = 0;
unsigned long strings_processed = 0;
unsigned long strings_processed_reset_th = 0;

const unsigned long RESET_TH = 1000000;

const unsigned long BUFFER_SZ = 100'000'000;

int main(int argc, char **argv) {
  parsed_options p_options = parse_cmline(argc, argv);

  std::cout << "Buffer Size: " << p_options.buffer_sz << std::endl;

  std::vector<char> sub_buf(p_options.buffer_sz, 0);
  std::vector<char> pred_buf(p_options.buffer_sz, 0);
  std::vector<char> obj_buf(p_options.buffer_sz, 0);

  std::vector<char> nt_buf(p_options.buffer_sz, 0);

  std::ofstream sub_ofs(p_options.output_file + ".subjects.txt");
  std::ofstream pred_ofs(p_options.output_file + ".predicates.txt");
  std::ofstream obj_ofs(p_options.output_file + ".objects.txt");

  sub_ofs.rdbuf()->pubsetbuf(sub_buf.data(), sub_buf.size());
  pred_ofs.rdbuf()->pubsetbuf(pred_buf.data(), pred_buf.size());
  obj_ofs.rdbuf()->pubsetbuf(obj_buf.data(), obj_buf.size());

  FsHolder fs_holder(sub_ofs, pred_ofs, obj_ofs, p_options.buffer_sz);

  std::ifstream ifs(p_options.input_file);
  ifs.rdbuf()->pubsetbuf(nt_buf.data(), nt_buf.size());

  process_nt_file(fs_holder, ifs);

  return 0;
}

void process_nt_file(FsHolder &fs_holder, std::ifstream &nt_ifs) {
  raptor_world *world = raptor_new_world();
  raptor_parser *parser = raptor_new_parser(world, "ntriples");

  raptor_parser_set_statement_handler(
      parser, (void *)&fs_holder, (raptor_statement_handler)statement_handler);

  raptor_parser_parse_start(parser, nullptr);

  std::vector<char> buffer(fs_holder.buf_size, 0);
  while (nt_ifs.read(buffer.data(), buffer.size())) {
    raptor_parser_parse_chunk(parser,
                              reinterpret_cast<unsigned char *>(buffer.data()),
                              (size_t)nt_ifs.gcount(), 0);
  }

  std::cout << "Done!";
  print_stats();

  raptor_free_parser(parser);
  raptor_free_world(world);
}

std::regex nl_re("\n");

void statement_handler(void *fs_holder_ptr, const raptor_statement *statement) {
  auto &fs_holder = *reinterpret_cast<FsHolder *>(fs_holder_ptr);

  auto &sub_ofs = fs_holder.sub_ofs;
  auto &pred_ofs = fs_holder.pred_ofs;
  auto &obj_ofs = fs_holder.obj_ofs;

  raptor_term *predicate = statement->predicate;
  raptor_term *subject = statement->subject;
  raptor_term *object = statement->object;

  auto subject_value = get_term_value(subject);
  auto predicate_value = get_term_value(predicate);
  auto object_value = get_term_value(object);

  /*
  std::string
    subject_value_proc,
    predicate_value_proc,
    object_value_proc;

  strtk::replace_pattern(subject_value, "\n", "\\n", subject_value_proc);
  strtk::replace_pattern(predicate_value, "\n", "\\n", predicate_value_proc);
  strtk::replace_pattern(object_value, "\n", "\\n", object_value_proc);
*/

  // auto subject_value_proc = std::regex_replace(subject_value, nl_re, "\\n");
  // auto predicate_value_proc = std::regex_replace(predicate_value, nl_re,
  // "\\n"); auto object_value_proc = std::regex_replace(object_value, nl_re,
  // "\\n");
  auto subject_value_proc = base64_encode(subject_value);
  auto predicate_value_proc = base64_encode(predicate_value);
  auto object_value_proc = base64_encode(object_value);

  sub_ofs << subject_value_proc << "\n";
  pred_ofs << predicate_value_proc << "\n";
  obj_ofs << object_value_proc << "\n";

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
  const char short_options[] = "f:o:B::";
  struct option long_options[] = {
      {"input-file", required_argument, nullptr, 'f'},
      {"output-file", required_argument, nullptr, 'o'},
      {"buffer-size", optional_argument, nullptr, 'B'}};

  int opt, opt_index;

  bool has_input = false;
  bool has_output = false;
  bool has_bsz = false;

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
    case 'B':
      if (optarg) {
        std::string s(optarg);
        out.buffer_sz = std::stoul(s);
        has_bsz = true;
      }
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

  if (!has_bsz) {
    out.buffer_sz = BUFFER_SZ;
  }

  return out;
}

void print_help() {
  std::cout
      << "--input-file\t(-f)\t\t(string-required)\n"
      << "--output-file\t(-o)\t\t(string-required)\n"
      << "--buffer-size\t(-B)\t\t(integer-optional (bytes), DEFAULT=100MB)\n"
      << std::endl;
}
