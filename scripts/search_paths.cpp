//
// Created by cristobal on 11-12-22.
//

#include "algorithms/search_paths.hpp"
#include "manager/PCMFactory.hpp"
#include "nodeids/NodeIdsManagerFactory.hpp"
#include "nodeids/node_ids_constants.hpp"
#include <getopt.h>
#include <stdexcept>
#include <string>

using namespace k2cache;

struct parsed_options {
  std::string k2tree_file;
  std::string node_ids_file;
  std::string mapped_node_ids_file;
  std::string output_file;
  int path_size;
  int max_number_paths;
};

parsed_options parse_cmd_line(int argc, char **argv);

int main(int argc, char **argv) { 
  auto parsed = parse_cmd_line(argc, argv);
  auto pcm = PCMFactory::create(parsed.k2tree_file);
  CacheArgs args;
  args.node_ids_filename = parsed.node_ids_file;
  args.mapped_node_ids_filename = parsed.mapped_node_ids_file;
  args.node_ids_logs_filename = args.node_ids_filename + ".logs_search_path.log";
  auto nim = NodeIdsManagerFactory::create(args);
  auto paths = find_n_paths(*pcm, parsed.path_size, parsed.max_number_paths);
  std::ofstream ofs(parsed.output_file, std::ios::out);
  int err_code = 0;
  auto validate = [](int err_code, uint64_t original_value) -> bool {
    if(err_code != (int)NidsErrCode::SUCCESS_ERR_CODE){
      std::cerr << "NodeId not found for value: " << original_value << std::endl;
      return false;
    }
    return true;
  };
  for(auto p: paths){
    for(const auto &t: p.get_vec()){
      auto subject = nim->get_real_id(t.subject, &err_code);
      if(!validate(err_code, subject)) continue;
      auto predicate = nim->get_real_id(t.predicate, &err_code);
      if(!validate(err_code, predicate)) continue;
      auto object = nim->get_real_id(t.object, &err_code);
      if(!validate(err_code, object)) continue;
      ofs << "(" << subject << "," << predicate << "," << object << ");";
    }
    ofs << "\n";
  }
}

parsed_options parse_cmd_line(int argc, char **argv) {
  const char short_options[] = "k:N:M:o:s:n:";
  struct option long_options[] = {
      {"k2tree-file", required_argument, nullptr, 'k'},
      {"node-ids-file", required_argument, nullptr, 'N'},
      {"mapped-node-ids-file", required_argument, nullptr, 'M'},
      {"output-file", required_argument, nullptr, 'o'},
      {"path-size", required_argument, nullptr, 's'},
      {"max-number-paths", required_argument, nullptr, 'n'},
  };

  int opt, opt_index;

  bool has_input = false;
  bool has_node_ids = false;
  bool has_mapped_node_ids = false;
  bool has_output = false;
  bool has_path_size = false;
  bool has_max_number = false;
  parsed_options out{};

  while ((
      opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }
    switch (opt) {
    case 'k':
      out.k2tree_file = optarg;
      has_input = true;
      break;
    case 'N':
      out.node_ids_file = optarg;
      has_node_ids = true;
      break;
    case 'M':
      out.mapped_node_ids_file = optarg;
      has_mapped_node_ids = true;
      break;
    case 'o':
      out.output_file = optarg;
      has_output = optarg;
      break;
    case 's':
      out.path_size = std::stoi(optarg);
      has_path_size = optarg;
      break;
    case 'n':
      out.max_number_paths = std::stoi(optarg);
      has_max_number = optarg;
      break;
    default:
      break;
    }
  }

  if (!has_input)
    throw std::runtime_error("k2tree-file (k) argument is required");
  if (!has_node_ids)
    throw std::runtime_error("node-ids-file (N) argument is required");
  if (!has_mapped_node_ids)
    throw std::runtime_error("mapped-node-ids-file (M) argument is required");
  if (!has_output)
    throw std::runtime_error("output-file (o) argument is required");
  if (!has_path_size)
    throw std::runtime_error("path-size (s) argument is required");
  if (!has_max_number)
    throw std::runtime_error("max-number-paths (n) argument is required");

  return out;
}