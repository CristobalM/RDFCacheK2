//
// Created by cristobal on 11-12-22.
//

#include <algorithm>
#include <cassert>
#include <queue>
#include <random>
#include <set>
#include <list>
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
        std::vector<RDFTriple> current_path,
        uint64_t first_subject, SPPair sp_pair,
        std::vector<DirectedPath> &paths,
        PredicatesCacheManager &pcm, TripleHSet &visited_edges,
        K2TreeMixed &current_k2tree, uint64_t current_predicate,
        int curr_path_size, int max_path_size,
        int max_paths, const std::vector<uint64_t> &sorted_predicates);

std::vector<DirectedPath>
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

  std::vector<DirectedPath> paths;

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
      std::vector<RDFTriple> current_path(n);


      find_paths_subj_origin(current_path, subj, sp_pair, paths, pcm, visited, k2tree, p,
                             0, n, max_number, sorted_predicate_ids);
      if((int)paths.size() >= max_number) break;
    }
  }
  return paths;
}
void debug_print_path(const std::vector<RDFTriple> &path);
void find_paths_subj_origin(
    std::vector<RDFTriple> current_path,
    uint64_t first_subject, SPPair sp_pair,
    std::vector<DirectedPath> &paths,
    PredicatesCacheManager &pcm, TripleHSet &visited_edges,
    K2TreeMixed &current_k2tree, uint64_t current_predicate,
    int curr_path_size, int max_path_size,
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

    if (curr_path_size ==  max_path_size - 1) {
      current_path[curr_path_size] = triple;
      // report path
      paths.emplace_back(current_path);
      debug_print_path(current_path);
      curr_path_size--;
      if((int)paths.size() >= max_paths){
        return;
      }
    } else {
      visited_edges.insert(triple);
      current_path[curr_path_size] = triple;
      curr_path_size++;
      for (auto pred_idx = 0UL; pred_idx < sorted_predicates.size();
           pred_idx++) {
        auto pred_value = sorted_predicates[pred_idx];
        auto pred_fetch_result = pcm.get_predicates_index_cache().fetch_k2tree(pred_value);
        auto &tree_selected = pred_fetch_result.get_mutable();

        // obj2 is next subject
        SPPair next_sp_pair = {obj2, pred_value};

        find_paths_subj_origin(current_path, first_subject, next_sp_pair, paths, pcm,
                               visited_edges, tree_selected, pred_value,
                               curr_path_size + 1, max_path_size, max_paths,
                               sorted_predicates);
      }
      curr_path_size--;
      visited_edges.erase(triple);
    }
  }
}

void debug_print_path(const std::vector<RDFTriple> &path) {
  std::cout << "printing path of size " << path.size() << ": " << std::endl;
  for(const auto & triple: path){
    std::cout << "(" << triple.subject << ", " << triple.predicate << ", " << triple.object << "); ";
  }
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