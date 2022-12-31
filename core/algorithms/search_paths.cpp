//
// Created by cristobal on 11-12-22.
//

#include <algorithm>
#include <cassert>
#include <queue>
#include <random>
#include <set>
#include <stack>
#include <unordered_set>
#include <vector>

#include "DirectedPath.hpp"
#include "search_paths.hpp"

namespace k2cache {

namespace {
using TripleHSet = std::unordered_set<RDFTriple, RDFTriple::hash>;
}

uint64_t  choose_predicate_index(uint64_t  size);

struct SPPair {
  uint64_t  subject;
  uint64_t  predicate;
};

struct POPair {
  uint64_t  predicate;
  uint64_t  object;
};

void find_paths_subj_origin(
        std::stack<RDFTriple> current_path,
        uint64_t first_subject, SPPair sp_pair,
        std::vector<std::pair<uint64_t, uint64_t>> &paths,
        PredicatesCacheManager &pcm, TripleHSet &visited_edges,
        K2TreeMixed &current_k2tree, uint64_t current_predicate,
        uint64_t current_pred_idx, int curr_path_size, int max_path_size,
        int max_paths, const std::vector<uint64_t> &sorted_predicates);

std::vector<std::pair<uint64_t , uint64_t >>
find_n_paths(PredicatesCacheManager &pcm, int n, int max_number) {

  auto sorted_predicate_ids =
      pcm.get_predicates_index_cache().get_predicates_ids();
  std::sort(sorted_predicate_ids.begin(), sorted_predicate_ids.end(),
            [&](auto l_predicate, auto r_predicate) {
              auto lfetched =
                  pcm.get_predicates_index_cache().fetch_k2tree(l_predicate);

              if (!lfetched.exists()) {
                throw std::runtime_error(
                    "left tree not found on predicate sorting: " +
                    std::to_string(l_predicate));
              }
              if (!lfetched.loaded()) {
                throw std::runtime_error(
                    "left tree not loaded on predicate sorting: " +
                    std::to_string(l_predicate));
              }

              auto rfetched =
                  pcm.get_predicates_index_cache().fetch_k2tree(r_predicate);

              if (!rfetched.exists()) {
                throw std::runtime_error(
                    "right tree not found on predicate sorting: " +
                    std::to_string(r_predicate));
              }
              if (!rfetched.loaded()) {
                throw std::runtime_error(
                    "right tree not loaded on predicate sorting: " +
                    std::to_string(r_predicate));
              }

              // decreasing order by number of points
              return lfetched.get().size() > rfetched.get().size();
            });

  std::vector<std::pair<uint64_t , uint64_t >> paths;

  for (auto pidx = 0UL; pidx < sorted_predicate_ids.size(); pidx++) {
    auto p = sorted_predicate_ids[pidx];
    auto fetched = pcm.get_predicates_index_cache().fetch_k2tree(p);
    assert(fetched.exists() && fetched.loaded());
    auto &k2tree = fetched.get_mutable();
    auto scanner = k2tree.create_full_scanner();
    std::unordered_set<uint64_t > subjects;
    TripleHSet visited;
    while (scanner->has_next()) {
      auto [subj, obj] = scanner->next();

      if (subjects.find(subj) != subjects.end()) {
        continue;
      }

      subjects.insert(subj);
      SPPair sp_pair = {subj, p};
      std::stack<RDFTriple> current_path;

      find_paths_subj_origin(current_path, subj, sp_pair, paths, pcm, visited, k2tree, p,
                             pidx, 1, n, max_number, sorted_predicate_ids);
      if((int)paths.size() >= max_number) break;
    }
  }
  return paths;
}
void debug_reverse_print_stack(const std::stack<RDFTriple> &s);
void find_paths_subj_origin(
        std::stack<RDFTriple> current_path,
        uint64_t first_subject, SPPair sp_pair,
        std::vector<std::pair<uint64_t, uint64_t>> &paths,
        PredicatesCacheManager &pcm, TripleHSet &visited_edges,
        K2TreeMixed &current_k2tree, uint64_t current_predicate,
        uint64_t current_pred_idx, int curr_path_size, int max_path_size,
        int max_paths, const std::vector<uint64_t> &sorted_predicates) {
  if((int)paths.size() >= max_paths){
    return;
  }
  auto bscanner = current_k2tree.create_band_scanner(
      sp_pair.subject, K2TreeScanner::BandType::COLUMN_BAND_TYPE);
  while (bscanner->has_next()) {
    auto [subj2, obj2] = bscanner->next();
    assert(sp_pair.subject == subj2);
    RDFTriple triple = {sp_pair.subject, current_predicate, obj2};

    if (visited_edges.find(triple) != visited_edges.end()) {
      continue;
    }

    if (curr_path_size + 1 > max_path_size) {
      paths.emplace_back(first_subject, obj2);
      current_path.push(triple);
      debug_reverse_print_stack(current_path);
      current_path.pop();
      if((int)paths.size() >= max_paths){
        return;
      }
    } else {
      visited_edges.insert(triple);
      current_path.push(triple);
      for (auto cpidx = 0UL; cpidx < sorted_predicates.size(); cpidx++) {
        auto cp = sorted_predicates[cpidx];
        auto ck2fetch = pcm.get_predicates_index_cache().fetch_k2tree(cp);
        auto &ck2 = ck2fetch.get_mutable();

        // obj2 is next subject
        SPPair next_sp_pair = {obj2, cp};

        find_paths_subj_origin(current_path, first_subject, next_sp_pair, paths, pcm,
                               visited_edges, ck2, cp, cpidx,
                               curr_path_size + 1, max_path_size, max_paths,
                               sorted_predicates);
      }
      current_path.pop();
      visited_edges.erase(triple);
    }
  }
}

static void debug_reverse_print_stack_rec(std::stack<RDFTriple> &s){
  if(s.empty()) return;
  auto t = s.top();
  s.pop();
  debug_reverse_print_stack_rec(s);
  std::cout << "("
            << t.subject << ", "
            << t.predicate << ", "
            << t.object << "), ";
}
void debug_reverse_print_stack(const std::stack<RDFTriple> &s) {
  auto copied = s;
  std::cout << "printing path of size " << s.size() << ": " << std::endl;
  debug_reverse_print_stack_rec(copied);
  std::cout << std::endl;
}



uint64_t  choose_predicate_index(uint64_t  size) {
  if (size == 0) {
    return 0;
  }
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<uint64_t > dis(0, size - 1UL);
  return dis(gen);
}

} // namespace k2cache