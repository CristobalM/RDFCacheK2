#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <string>

#include <K2TreeMixed.hpp>
#include <PredicatesIndexCacheMDFile.hpp>
#include <triple_external_sort.hpp>

namespace fs = std::filesystem;

enum BandType { COL = 0, ROW };

struct parsed_options {
  std::string input_file;
  std::string output_file;
  BandType band;
  unsigned long position;
  unsigned long predicate;
};

parsed_options parse_cmline(int argc, char **argv);

int main(int argc, char **argv) {
  auto parsed = parse_cmline(argc, argv);

  if (!fs::exists(parsed.input_file)) {
    throw std::runtime_error("Not found file " + parsed.input_file);
  }

  PredicatesIndexCacheMDFile pc(parsed.input_file);

  auto fetch_result = pc.fetch_k2tree(parsed.predicate);
  auto &k2tree = fetch_result.get_mutable();

  int debug_validate = debug_validate_k2node_rec(k2tree.get_root_k2node(),
                                                 k2tree.get_k2qstate(), 0);

  std::ofstream ofs(parsed.output_file, std::ios::out | std::ios::trunc);

  std::cout << "position: " << parsed.position << "\n"
            << "predicate: " << parsed.predicate << "\n"
            << "points: " << k2tree.size() << "\n"
            << "debug validate " << debug_validate << "\n"
            << std::endl;

  std::set<unsigned long> band_tr;

  if (parsed.band == BandType::COL) {
    k2tree.traverse_column(
        parsed.position,
        [](unsigned long, unsigned long row, void *report_state) {
          reinterpret_cast<std::set<unsigned long> *>(report_state)
              ->insert(row);
        },
        &band_tr);
  } else {
    k2tree.traverse_row(
        parsed.position,
        [](unsigned long col, unsigned long, void *report_state) {
          *reinterpret_cast<std::ofstream *>(report_state) << col << "\n";
          reinterpret_cast<std::set<unsigned long> *>(report_state)
              ->insert(col);
        },
        &band_tr);
  }

  std::vector<const K2TreeMixed *> trees = {&k2tree};
  struct sip_ipoint join_coordinate;
  join_coordinate.coord = parsed.position;
  join_coordinate.coord_type =
      (parsed.band == BandType::COL ? COLUMN_COORD : ROW_COORD);

  std::vector<sip_ipoint> join_coordinates = {join_coordinate};
  auto join_result = K2TreeMixed::sip_join_k2trees(trees, join_coordinates);

  std::set<unsigned long> sip_set(join_result.begin(), join_result.end());

  int real_points_count = 0;
  trees[0]->scan_points(
      [](unsigned long, unsigned long, void *report_state) {
        (*reinterpret_cast<int *>(report_state))++;
      },
      &real_points_count);

  auto config = pc.get_config();

  K2TreeMixed copy_k2tree(config);

  trees[0]->scan_points(
      [](unsigned long col, unsigned long row, void *report_state) {
        (*reinterpret_cast<K2TreeMixed *>(report_state)).insert(col, row);
      },
      &copy_k2tree);

  std::vector<const K2TreeMixed *> other_trees = {&copy_k2tree};

  auto join_result_copy =
      K2TreeMixed::sip_join_k2trees(other_trees, join_coordinates);

  unsigned long sz_serialized_original;
  unsigned long sz_serialized_copy;
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

  std::cout << "join_result sz: " << join_result.size() << std::endl
            << "join_coordinate.coord: " << join_coordinate.coord << std::endl
            << "join_coordinate.coord_type: " << join_coordinate.coord_type
            << std::endl
            << "join_coordinates.size: " << join_coordinates.size() << std::endl
            << "join_coordinates[0].coord: " << join_coordinates[0].coord
            << std::endl
            << "join_coordinates[0].coord_type: "
            << join_coordinates[0].coord_type << std::endl
            << "real_points_count: " << real_points_count << std::endl
            << "traverse size: " << band_tr.size() << std::endl
            << "same as traverse: " << (sip_set == band_tr ? "TRUE" : "FALSE")
            << std::endl
            << "other_join_result sz: " << join_result_copy.size() << std::endl
            << "sz serialized original: " << sz_serialized_original << std::endl
            << "sz serialized copy: " << sz_serialized_copy << std::endl
            << "sz original: " << k2tree.size() << std::endl
            << "sz copy: " << copy_k2tree.size() << std::endl
            << std::endl;

  for (auto value : join_result) {
    ofs << value << "\n";
  }

  return 0;
}

parsed_options parse_cmline(int argc, char **argv) {
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