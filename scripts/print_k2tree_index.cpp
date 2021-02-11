#include <string>
#include <stdexcept>
#include <getopt.h>
#include <filesystem>
#include <fstream>

#include <K2TreeMixed.hpp>
#include <external_sort.hpp>
#include <PredicatesIndexCacheMDFile.hpp>

namespace fs = std::filesystem;

struct parsed_options {
  std::string input_file;
  std::string output_file;
};

parsed_options parse_cmline(int argc, char **argv);

struct DataHolder{
  std::ofstream &ofs;
  uint64_t predicate_id;
  uint64_t &total_triples;
  DataHolder(std::ofstream &ofs, uint64_t predicate_id, uint64_t &total_triples) :
  ofs(ofs),
  predicate_id(predicate_id),
  total_triples(total_triples)
  {}
};
int main(int argc, char **argv){
  auto parsed = parse_cmline(argc, argv);

  if(!fs::exists(parsed.input_file)){
    throw std::runtime_error("Not found file " + parsed.input_file);
  }  

  PredicatesIndexCacheMDFile pc(parsed.input_file);

  auto predicates_ids = pc.get_predicates_ids();


  std::ofstream ofs(parsed.output_file, std::ios::out | std::ios::binary | std::ios::trunc);
  uint64_t total_triples = 0;
  auto start = ofs.tellp();
  write_u64(ofs, total_triples);
  for(auto predicate_id : predicates_ids){
    auto &k2tree = pc.fetch_k2tree(predicate_id);
    DataHolder data_holder(ofs, predicate_id, total_triples);
    k2tree.scan_points([](unsigned long col, unsigned long row, void *st){
      auto &dh = *reinterpret_cast<DataHolder*>(st);
      TripleValue tvalue(col, dh.predicate_id, row);
      tvalue.write_to_file(dh.ofs);
      dh.total_triples++;

    }, &data_holder);
    pc.discard_in_memory_predicate(predicate_id);
  }
  ofs.seekp(start);
  write_u64(ofs, total_triples);
  return 0;
}


parsed_options parse_cmline(int argc, char **argv){
  const char short_options[] = "i:o:";
  struct option long_options[] = {
      {"input-file", required_argument, nullptr, 'i'},
      {"output-file", required_argument, nullptr, 'o'},
  };

  int opt, opt_index;

  bool has_input = false;
  bool has_output = false;
  parsed_options out{};

  while ((
      opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }
    switch(opt){
      case 'i':
      out.input_file = optarg;
      has_input = true;
      break;
      case 'o':
      out.output_file = optarg;
      has_output = optarg;
      break;
      default:
      break;
    }
  }

  if(!has_input) throw std::runtime_error("input-file (i) argument is required");
  if(!has_output) throw std::runtime_error("output-file (o) argument is required");
 
  return out;
}