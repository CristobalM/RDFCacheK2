#include <string>
#include <stdexcept>
#include <getopt.h>
#include <filesystem>
#include <fstream>

#include <PredicatesIndexFileBuilder.hpp>
#include <K2TreeMixed.hpp>

namespace fs = std::filesystem;

struct parsed_options {
  std::string input_file;
  std::string output_file;
};

parsed_options parse_cmline(int argc, char **argv);

int main(int argc, char **argv){
  auto parsed = parse_cmline(argc, argv);

  if(!fs::exists(parsed.input_file)){
    throw std::runtime_error("Not found file " + parsed.input_file);
  }  

  K2TreeConfig config;
  config.max_node_count = 256;
  config.cut_depth = 10;
  config.treedepth = 32;

  std::ifstream ifs(parsed.input_file, std::ios::in | std::ios::binary);
  std::ofstream ofs(parsed.output_file, std::ios::out | std::ios::binary | std::ios::trunc);
  std::fstream tmp_fs(parsed.output_file + ".tmp", std::ios::in | std::ios::out | std::ios::binary );
  PredicatesIndexFileBuilder::build(ifs, ofs, tmp_fs, config);


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