//
// Created by Cristobal Miranda on 14-01-23.
//

#include "random_dataset_generation.hpp"
#include "builder/PredicatesIndexFileBuilder.hpp"
#include "k2tree/K2TreeBulkOp.hpp"
#include "util_algorithms/fisher_yates.hpp"
#include "nodeids/NodesSequence.hpp"
#include <random>
namespace k2cache {

static uint64_t random_generate(uint64_t start, uint64_t end) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<uint64_t> distr(start, end);
  return distr(gen);
}

struct K2TreeFeedRandom : public K2TreesFeed {
  uint64_t triples_per_predicate;
  uint64_t remaining;
  uint64_t node_ids_sz;
  const std::vector<uint64_t> &predicates;
  K2TreeConfig config;

  uint64_t pred_cnt;
  K2TreeFeedRandom(uint64_t triples_per_predicate, uint64_t remaining,
                   uint64_t node_ids_sz,
                   const std::vector<uint64_t> &predicates, K2TreeConfig config)
      : triples_per_predicate(triples_per_predicate), remaining(remaining),
        node_ids_sz(node_ids_sz), predicates(predicates), config(config),
        pred_cnt(0) {}

  bool has_next() override { return pred_cnt < predicates.size(); }
  K2TreeContainer get_next() override {
    K2TreeContainer container;
    container.tree = std::make_unique<K2TreeMixed>(config);
    K2TreeBulkOp op(*container.tree);
    random_insert_n_points(triples_per_predicate, node_ids_sz, op);

    if (pred_cnt + 1 == predicates.size()) {
      random_insert_n_points(remaining, node_ids_sz, op);
    }

    container.predicate = predicates[pred_cnt];
    pred_cnt++;
    return container;
  }

  static void random_insert_n_points(uint64_t points_num, uint64_t node_ids_sz,
                                     K2TreeBulkOp &op) {
    for (uint64_t i = 0; i < points_num; i++) {
      bool was_inserted;
      do {
        const auto col = random_generate(0, node_ids_sz-1);
        const auto row = random_generate(0, node_ids_sz-1);
        was_inserted = op.insert_was_inserted(col, row);
      } while (!was_inserted);
    }
  };
};

void generate_random_dataset(k2cache::K2TreeConfig config, uint64_t triples_num,
                             uint64_t resources_num, I_OStream &trees_ostream,
                             I_IOStream &trees_tmp_ostream,
                             I_OStream &nodeids_ostream) {

  auto node_ids = fisher_yates(resources_num, 1ULL << 63ULL);
  std::sort(node_ids.begin(), node_ids.end());

  auto predicates_count = static_cast<uint64_t>(
      node_ids.size() > 10 ? node_ids.size() / 10ULL : 1ULL);
  auto predicate_ids_selection =
      fisher_yates(predicates_count, node_ids.size());
  std::sort(predicate_ids_selection.begin(), predicate_ids_selection.end());

  const auto triples_per_predicate =
      triples_num / predicate_ids_selection.size();
  const auto remaining = triples_num % predicate_ids_selection.size();

  {
    auto feed =
        K2TreeFeedRandom(triples_per_predicate, remaining, node_ids.size(),
                         predicate_ids_selection, config);

    PredicatesIndexFileBuilder::build_with_k2tree_feed(feed, trees_ostream.get_ostream(),
                                                       trees_tmp_ostream.get_stream(), config);
  }

  NodesSequence nodes_sequence(std::move(node_ids));
  nodes_sequence.serialize(nodeids_ostream);
}

} // namespace k2cache