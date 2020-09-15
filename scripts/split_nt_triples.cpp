//
// Created by Cristobal Miranda, 2020
//

#include <algorithm>
#include <base64.h>
#include <cstring>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <nt_parser.hpp>
#include <string>
#include <vector>

struct parsed_options {
  std::string input_file;
  std::string output_file;
  unsigned long buffer_sz;
  bool base64;
};

struct FsHolder {
  std::ofstream &iris_ofs;
  std::ofstream &blanks_ofs;
  std::ofstream &literals_ofs;

  unsigned long buf_size;

  bool base64;

  FsHolder(std::ofstream &iris_ofs, std::ofstream &blanks_ofs,
           std::ofstream &literals_ofs, unsigned long buf_size, bool base64)
      : iris_ofs(iris_ofs), blanks_ofs(blanks_ofs), literals_ofs(literals_ofs),
        buf_size(buf_size), base64(base64) {}
};

parsed_options parse_cmline(int argc, char **argv);
void print_help();
void process_nt_file(FsHolder &fs_holder, std::ifstream &nt_ifs);
void print_stats();

void cond_write_data(char *data, RDFType type, FsHolder &o);
int cond_write(NTRes &res, FsHolder &o);
void processor(NTTriple *ntriple, void *fs_holder_ptr);

unsigned long bytes_processed = 0;
unsigned long strings_processed = 0;
unsigned long strings_processed_reset_th = 0;

const unsigned long RESET_TH = 1000000;

const unsigned long BUFFER_SZ = 1024;

int main(int argc, char **argv) {
  parsed_options p_options = parse_cmline(argc, argv);

  // std::cout << "Buffer Size: " << p_options.buffer_sz << std::endl;

  /*
  std::vector<char> sub_buf(p_options.buffer_sz, 0);
  std::vector<char> pred_buf(p_options.buffer_sz, 0);
  std::vector<char> obj_buf(p_options.buffer_sz, 0);

  std::vector<char> nt_buf(p_options.buffer_sz, 0);
  */

  std::ofstream iris_ofs(p_options.output_file + ".iris.txt", std::ios::out);
  std::ofstream blanks_ofs(p_options.output_file + ".blanks.txt",
                           std::ios::out);
  std::ofstream literals_ofs(p_options.output_file + ".literals.txt",
                             std::ios::out);

  // iris_ofs.rdbuf()->pubsetbuf(sub_buf.data(), sub_buf.size());
  // blanks_ofs.rdbuf()->pubsetbuf(pred_buf.data(), pred_buf.size());
  // literals_ofs.rdbuf()->pubsetbuf(obj_buf.data(), obj_buf.size());

  FsHolder fs_holder(iris_ofs, blanks_ofs, literals_ofs, p_options.buffer_sz,
                     p_options.base64);

  std::ifstream ifs(p_options.input_file, std::ios::in);
  // ifs.rdbuf()->pubsetbuf(nt_buf.data(), nt_buf.size());

  process_nt_file(fs_holder, ifs);

  return 0;
}

void process_nt_file(FsHolder &fs_holder, std::ifstream &nt_ifs) {
  NTParser ntparser(&nt_ifs, processor, &fs_holder);
  ntparser.parse();
  std::cout << "Done!";
  print_stats();
}

void cond_write_data(char *data, RDFType type, FsHolder &o) {
  switch (type) {
  case IRI:
    o.iris_ofs << data << "\n";
    break;
  case LITERAL:
    o.literals_ofs << data << "\n";
    break;
  case BLANK_NODE:
    o.blanks_ofs << data << "\n";
    break;
  }
}

int cond_write(NTRes *res, FsHolder &o) {
  char *data = res->data;

  if (!o.base64) {
    cond_write_data(data, res->type, o);
    return strlen(data);
  } else {
    auto encoded = base64_encode(std::string(data));
    cond_write_data(encoded.data(), res->type, o);
    return encoded.size();
  }
}

void processor(NTTriple *ntriple, void *fs_holder_ptr) {
  auto &h = *reinterpret_cast<FsHolder *>(fs_holder_ptr);
  bytes_processed += cond_write(&ntriple->subject, h);
  bytes_processed += cond_write(&ntriple->predicate, h);
  bytes_processed += cond_write(&ntriple->object, h);

  strings_processed += 3;
  strings_processed_reset_th += 3;

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
  const char short_options[] = "f:o:B::b";
  struct option long_options[] = {
      {"input-file", required_argument, nullptr, 'f'},
      {"output-file", required_argument, nullptr, 'o'},
      {"buffer-size", optional_argument, nullptr, 'B'},
      {"base64", optional_argument, nullptr, 'b'}};

  int opt, opt_index;

  bool has_input = false;
  bool has_output = false;
  bool has_bsz = false;

  parsed_options out{};

  out.base64 = false;

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
    case 'b':
      out.base64 = true;
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
      << "--buffer-size\t(-B)\t\t(integer-optional (bytes), DEFAULT=1KB)\n"
      << "--base64\t(-b)\t\t(bool-optional, DEFAULT=false)\n"
      << std::endl;
}
