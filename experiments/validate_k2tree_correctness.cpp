//
// Created by cristobal on 05-10-22.
//

#include "fisher_yates.hpp"
#include "k2tree/K2TreeMixed.hpp"
#include <algorithm>
#include <iostream>
#include <random>
#include <set>
#include <sstream>

namespace uuid {
static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_int_distribution<> dis(0, 15);
static std::uniform_int_distribution<> dis2(8, 11);

std::string generate_uuid_v4() {
  std::stringstream ss;
  int i;
  ss << std::hex;
  for (i = 0; i < 8; i++) {
    ss << dis(gen);
  }
  ss << "-";
  for (i = 0; i < 4; i++) {
    ss << dis(gen);
  }
  ss << "-4";
  for (i = 0; i < 3; i++) {
    ss << dis(gen);
  }
  ss << "-";
  ss << dis2(gen);
  for (i = 0; i < 3; i++) {
    ss << dis(gen);
  }
  ss << "-";
  for (i = 0; i < 12; i++) {
    ss << dis(gen);
  };
  return ss.str();
}
} // namespace uuid

using namespace k2cache;

using vpu_t = std::vector<std::pair<unsigned long, unsigned long>>;
using spu_t = std::set<std::pair<unsigned long, unsigned long>>;

struct tree_data_t {
  K2TreeMixed tree;
  vpu_t data;
};

void draw_square(K2TreeMixed &tree, K2QStateWrapper &st,
                 unsigned long start_pos, unsigned long side, vpu_t &data,
                 unsigned long max_side);
void draw_square_randomly(K2TreeMixed &tree, K2QStateWrapper &st,
                 unsigned long start_pos, unsigned long side, vpu_t &data,
                 unsigned long max_side);
vpu_t compact_data(vpu_t &data);

void validate(K2TreeMixed &tree, const vpu_t &data, K2TreeConfig config,
              unsigned long attempt, const std::string &identifier);
void report_fail_missing_from_input(
    const std::pair<unsigned long, unsigned long> &pair, const vpu_t &data,
    unsigned long counter, K2TreeConfig config, unsigned long attempt,
    const std::string &identifier);

void report_fail_missing_from_tree(
    const std::pair<unsigned long, unsigned long> &pair, const vpu_t &data,
    unsigned long counter, K2TreeConfig config, unsigned long attempt,
    const std::string &identifier);
void report_common(const std::pair<unsigned long, unsigned long> &pair,
                   const vpu_t &vector, unsigned long counter,
                   K2TreeConfig config, const std::string &fname,
                   unsigned long attempt);

tree_data_t build_rand_tree(K2TreeConfig config);
void wanted_config_validations_squares() {
  auto time_ui = (unsigned int)(time(nullptr));
  std::srand(time_ui);
  auto uuid_value = uuid::generate_uuid_v4();

  for (auto d = 32; d <= 32; d++) {
    K2TreeConfig config{};
    config.treedepth = d;
    config.max_node_count = 128;
    config.cut_depth = 10;
    const auto attempts = 1024UL;

    auto u = uuid_value + "---#" + std::to_string(d);


    for (auto i = 0UL; i < attempts; i++) {
      std::cout << "at attempt " << i << " with depth " << d << " id: " << u
                << std::endl;
      auto [t, data] = build_rand_tree(config);
      validate(t, data, config, i, u);
    }
  }
}

static unsigned long random_int(unsigned long max_val){
  static std::random_device rd;
  static std::mt19937 mt(rd());
  std::uniform_real_distribution<double> dist(1.0, (double)max_val);
  return (unsigned long)dist(mt);
}

tree_data_t build_rand_tree(K2TreeConfig config) {
  K2TreeMixed t(config);
  auto s = t.create_k2qw();
//  auto fy_set = fisher_yates(128UL, 1UL << 32UL);
  auto fy_set = fisher_yates(128UL, 1UL << 32UL);
  vpu_t data;
  auto max_side = 1UL << config.treedepth;
  for (auto v : fy_set) {
    auto r = random_int(1024UL);
    r = r < 20 ? r + 32 : r;
//    draw_square(t, s, v, r, data, max_side);
    draw_square_randomly(t, s, v, r, data, max_side);
  }
  return {t, data};
}

void validate(K2TreeMixed &tree, const vpu_t &data, K2TreeConfig config,
              unsigned long attempt, const std::string &identifier) {
  auto scan = tree.create_full_scanner();
  spu_t set(data.begin(), data.end());
  std::cout << "points: " << set.size() << std::endl;

  auto counter = 0UL;
  spu_t scanned;

  std::set<unsigned long> columns;
  std::set<unsigned long> rows;

  while (scan->has_next()) {
    auto p = scan->next();
    columns.insert(p.first);
    rows.insert(p.second);
    auto it = set.find(p);
    if (it == set.end()) {
      report_fail_missing_from_input(p, data, counter, config, attempt,
                                     identifier);
      return;
    }
    counter++;
    scanned.insert(p);
  }

  counter = 0;
  for (auto p : set) {
    auto it = scanned.find(p);
    if (it == scanned.end()) {
      report_fail_missing_from_tree(p, data, counter, config, attempt,
                                    identifier);
      return;
    }
    counter++;
  }

  for(auto c : columns){
    scan  = tree.create_band_scanner(c, k2cache::K2TreeScanner::COLUMN_BAND_TYPE);
    counter = 0;
    while(scan->has_next()){
      auto p = scan->next();
      auto it = set.find(p);
      if(it == set.end()){
        auto fname = identifier + "_attempt-" + std::to_string(attempt) +
                     "_missing_from_col_scan.txt";
        report_common(p, data, counter, config, fname, attempt);
      }
      counter++;
    }
  }
  for(auto row : rows){
    scan  = tree.create_band_scanner(row, k2cache::K2TreeScanner::ROW_BAND_TYPE);
    counter = 0;
    while(scan->has_next()){
      auto p = scan->next();
      auto it = set.find(p);
      if(it == set.end()){
        auto fname = identifier + "_attempt-" + std::to_string(attempt) +
                     "_missing_from_row_scan.txt";
        report_common(p, data, counter, config, fname, attempt);
      }
      counter++;
    }
  }
  std::stringstream ss;
  tree.write_to_ostream(ss);
  auto des_tree = K2TreeMixed::read_from_istream(ss);
  auto des_scan = des_tree.create_full_scanner();
  counter = 0;
  spu_t scanned_des;
  while(des_scan->has_next()){
    auto p = des_scan->next();
    auto it = set.find(p);
    if(it == set.end()){
      auto fname = identifier + "_attempt-" + std::to_string(attempt) +
                   "_missing_from_des_full_scan.txt";
      report_common(p, data, counter, config, fname, attempt);
    }
    scanned_des.insert(p);
    counter++;
  }
  auto des_st = des_tree.create_k2qw();
  counter = 0;
  for(auto p: set){
    auto it = scanned_des.find(p);
    if(it == scanned_des.end()){
      auto fname = identifier + "_attempt-" + std::to_string(attempt) +
                   "_missing_from_des_output_fscan.txt";
      report_common(p, data, counter, config, fname, attempt);
    }
    counter++;
  }
}

void report_fail_missing_from_tree(
    const std::pair<unsigned long, unsigned long> &pair, const vpu_t &data,
    unsigned long counter, K2TreeConfig config, unsigned long attempt,
    const std::string &identifier) {
  auto fname = identifier + "_attempt-" + std::to_string(attempt) +
               "_missing_from_tree.txt";
  report_common(pair, data, counter, config, fname, attempt);
}
void report_common(const std::pair<unsigned long, unsigned long> &pair,
                   const vpu_t &vector, unsigned long counter,
                   K2TreeConfig config, const std::string &fname,
                   unsigned long attempt) {
  std::cout << "Missing data found"
            << "with pair (" << pair.first << ", " << pair.second << ")"
            << ", count = " << counter << ", config = ("
            << config.max_node_count << ", " << config.cut_depth << ", "
            << config.treedepth << ")"
            << ", attempt = " << attempt << std::endl;
  std::cout << "Writing to a file " << fname << std::endl;
  std::ofstream ofs(fname, std::ios::out);
  for (auto &p : vector) {
    ofs << p.first << "," << p.second << "\n";
  }
}

void report_fail_missing_from_input(
    const std::pair<unsigned long, unsigned long> &pair, const vpu_t &data,
    unsigned long counter, K2TreeConfig config, unsigned long attempt,
    const std::string &identifier) {
  auto fname = identifier + "_attempt-" + std::to_string(attempt) +
               "missing_from_input.txt";
  report_common(pair, data, counter, config, fname, attempt);
}

void draw_square(K2TreeMixed &tree, K2QStateWrapper &st,
                 unsigned long start_pos, unsigned long side, vpu_t &data,
                 unsigned long max_side) {
  for (auto i = 0UL; i < side; i++) {
    auto col = start_pos + i;
    if (col >= max_side)
      break;
    for (auto j = 0UL; j < side; j++) {
      auto row = start_pos + j;
      if (row >= max_side)
        break;
      tree.insert(col, row, st);
      data.push_back({col, row});
    }
  }
}

void draw_square_randomly(K2TreeMixed &tree, K2QStateWrapper &st,
                 unsigned long start_pos, unsigned long side, vpu_t &data,
                 unsigned long max_side) {
  for (auto i = 0UL; i < side; i++) {
    auto col = start_pos + i;
    if (col >= max_side)
      break;
    for (auto j = 0UL; j < side; j++) {
      auto row = start_pos + j;
      if (row >= max_side)
        break;

      auto r = random_int(10);
      if(r < 7) continue;
      tree.insert(col, row, st);
      data.push_back({col, row});
    }
  }
}

vpu_t compact_data(vpu_t &data) {
  std::sort(data.begin(), data.end());
  vpu_t out;
  out.reserve(data.size());
  for (auto i = 1UL; i < data.size(); i++) {
    auto &curr = data[i];
    auto &prev = data[i - 1];
    if (curr != prev) {
      out.push_back(curr);
    }
  }
  return out;
}

int main() { wanted_config_validations_squares(); }