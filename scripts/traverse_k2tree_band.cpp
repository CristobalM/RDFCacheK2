#include <filesystem>
#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <string>

#include "k2tree/K2TreeBulkOp.hpp"
#include "k2tree/K2TreeMixed.hpp"
#include "manager/PredicatesIndexCacheMD.hpp"
#include <FileRWHandler.hpp>
#include <set>
#include <triple_external_sort.hpp>
using namespace k2cache;
namespace fs = std::filesystem;

enum BandType { COL = 0, ROW };

struct parsed_options {
  std::string input_file;
  std::string output_file;
  BandType band;
  uint64_t position;
  uint64_t predicate;
};

parsed_options parse_cmd_line(int argc, char **argv);

int main(int argc, char **argv) {
  auto parsed = parse_cmd_line(argc, argv);

  if (!fs::exists(parsed.input_file)) {
    throw std::runtime_error("Not found file " + parsed.input_file);
  }

  auto frw_handler = std::make_unique<FileRWHandler>(parsed.input_file);

  PredicatesIndexCacheMD pc(std::move(frw_handler));

  auto fetch_result = pc.fetch_k2tree(parsed.predicate);
  auto &k2tree = fetch_result.get_mutable();

  auto bulk_op = K2TreeBulkOp(k2tree);

  bool debug_validate = k2tree.has_valid_structure(bulk_op.get_stw());

  std::ofstream ofs(parsed.output_file, std::ios::out | std::ios::trunc);

  std::cout << "position: " << parsed.position << "\n"
            << "predicate: " << parsed.predicate << "\n"
            << "points: " << k2tree.size() << "\n"
            << "debug validate " << debug_validate << "\n"
            << std::endl;

  std::set<uint64_t> band_tr;

  if (parsed.band == BandType::COL) {
    k2tree.traverse_column(
        parsed.position,
        [](uint64_t, uint64_t row, void *report_state) {
          reinterpret_cast<std::set<uint64_t> *>(report_state)
              ->insert(row);
        },
        &band_tr, bulk_op.get_stw());
  } else {
    k2tree.traverse_row(
        parsed.position,
        [](uint64_t col, uint64_t, void *report_state) {
          *reinterpret_cast<std::ofstream *>(report_state) << col << "\n";
          reinterpret_cast<std::set<uint64_t> *>(report_state)
              ->insert(col);
        },
        &band_tr, bulk_op.get_stw());
  }

  std::vector<const K2TreeMixed *> trees = {&k2tree};

  int real_points_count = 0;
  trees[0]->scan_points(
      [](uint64_t, uint64_t, void *report_state) {
        (*reinterpret_cast<int *>(report_state))++;
      },
      &real_points_count, bulk_op.get_stw());

  auto config = pc.get_config();

  K2TreeMixed copy_k2tree(config);
  K2TreeBulkOp copy_bulk_op(copy_k2tree);

  trees[0]->scan_points(
      [](uint64_t col, uint64_t row, void *report_state) {
        (*reinterpret_cast<K2TreeBulkOp *>(report_state)).insert(col, row);
      },
      &copy_bulk_op, copy_bulk_op.get_stw());

  std::vector<const K2TreeMixed *> other_trees = {&copy_k2tree};

  uint64_t sz_serialized_original;
  uint64_t sz_serialized_copy;
  {
    std::stringstream ss_original;
    k2tree.write_to_ostream(ss_original);
    sz_serialized_original = ss_original.str().size();
  }
  {
    std::stringstream ss_copy;
    copy_k2tree.write_to_ostream(ss_copy);
    sz_serialized_copy = ss_copy.str().size();
  }

  std::cout

      << "real_points_count: " << real_points_count << std::endl
      << "traverse size: " << band_tr.size() << std::endl
      << std::endl
      << "sz serialized original: " << sz_serialized_original << std::endl
      << "sz serialized copy: " << sz_serialized_copy << std::endl
      << "sz original: " << k2tree.size() << std::endl
      << "sz copy: " << copy_k2tree.size() << std::endl
      << std::endl;

  return 0;
}

parsed_options parse_cmd_line(int argc, char **argv) {
  const char short_options[] = "i:o:b:p:P:";
  struct option long_options[] = {
      {"input-file", required_argument, nullptr, 'i'},
      {"output-file", required_argument, nullptr, 'o'},
      {"band", required_argument, nullptr, 'b'},
      {"position", required_argument, nullptr, 'p'},
      {"predicate", required_argument, nullptr, 'P'},

  };

  int opt, opt_index;

  bool has_input = false;
  bool has_output = false;
  bool has_band = false;
  bool has_position = false;
  bool has_predicate = false;
  parsed_options out{};

  std::string band_string;

  while ((
      opt = getopt_long(argc, argv, short_options, long_options, &opt_index))) {
    if (opt == -1) {
      break;
    }
    switch (opt) {
    case 'i':
      out.input_file = optarg;
      has_input = true;
      break;
    case 'o':
      out.output_file = optarg;
      has_output = true;
      break;

    case 'b':
      band_string = optarg;
      if (band_string == "COL") {
        out.band = BandType::COL;
      } else if (band_string == "ROW") {
        out.band = BandType::ROW;
      } else {
        throw std::runtime_error("Unknown band type '" + band_string + "'");
      }
      has_band = true;
      break;
    case 'p':
      out.position = std::stoul(optarg);
      has_position = true;
      break;
    case 'P':
      out.predicate = std::stoul(optarg);
      has_predicate = true;
      break;
    default:
      break;
    }
  }

  if (!has_input)
    throw std::runtime_error("input-file (i) argument is required");
  if (!has_output)
    throw std::runtime_error("output-file (o) argument is required");
  if (!has_band)
    throw std::runtime_error("band (b)-COL,ROW argument is required");
  if (!has_position)
    throw std::runtime_error("position (p) argument is required");
  if (!has_predicate)
    throw std::runtime_error("predicate (P) argument is required");

  return out;
}