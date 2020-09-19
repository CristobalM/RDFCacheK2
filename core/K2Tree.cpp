//
// Created by Cristobal Miranda, 2020
//

extern "C" {
#include <definitions.h>
#include <memalloc.h>
#include <vector.h>
}

#include <cmath>
#include <memory>
#include <unordered_map>

#include "K2Tree.hpp"
#include "exceptions.hpp"

bool same_blocks(const struct block *lhs, const struct block *rhs);

bool same_block_frontiers(const block_frontier *lhs, const block_frontier *rhs);

bool same_block_topologies(const block_topology *lhs,
                           const block_topology *rhs);

bool same_bvs(const bitvector *lhs, const bitvector *rhs);

bool same_vectors(const struct vector &lhs, const struct vector &rhs);

K2Tree::K2Tree(uint32_t tree_depth) : root(create_block(tree_depth)) {
  qs = std::make_unique<struct queries_state>();
  init_queries_state(qs.get(), tree_depth);
}

K2Tree::K2Tree(uint32_t tree_depth, uint32_t max_node_count)
    : K2Tree(tree_depth) {
  root->max_node_count = max_node_count;
}

K2Tree::K2Tree(struct block *root) : root(root) {
  qs = std::make_unique<struct queries_state>();
  init_queries_state(qs.get(), root->tree_depth);
}

K2Tree::K2Tree(K2Tree &&other) {
  root = nullptr;
  qs = nullptr;
  std::swap(root, other.root);
  std::swap(qs, other.qs);
}

K2Tree &K2Tree::operator=(K2Tree &&rhs) {
  root = nullptr;
  qs = nullptr;
  std::swap(root, rhs.root);
  std::swap(qs, rhs.qs);
  return *this;
}

K2Tree::~K2Tree() noexcept(false) {
  int err_check = 0;
  if (root) {
    err_check = free_rec_block(root);
    if (err_check)
      throw std::runtime_error(
          "free_rec_block: ERROR WHILE FREEING MEMORY, CODE = " +
          std::to_string(err_check));
  }
  if (qs) {
    err_check = finish_queries_state(qs.get());
    if (err_check)
      throw std::runtime_error(
          "finish_queries_state: ERROR WHILE FREEING MEMORY, CODE = " +
          std::to_string(err_check));
  }
}

void K2Tree::insert(unsigned long col, unsigned long row) {
  int err_check = insert_point(root, col, row, qs.get());
  if (err_check)
    throw std::runtime_error("insert: CANT INSERT POINT " +
                             std::to_string(col) + ", " + std::to_string(row) +
                             ", ERROR CODE= " + std::to_string(err_check));
}

bool K2Tree::has(unsigned long col, unsigned long row) {
  int result;
  int err_check = has_point(root, col, row, qs.get(), &result);
  if (err_check)
    throw std::runtime_error("has: ERROR WHILE SEARCHING POINT " +
                             std::to_string(col) + ", " + std::to_string(row) +
                             ", ERROR CODE= " + std::to_string(err_check));
  return result == 1;
}

std::vector<std::pair<unsigned long, unsigned long>> K2Tree::scan_points() {
  struct vector result {};
  int err_check;
  err_check = init_vector_with_capacity(&result, sizeof(unsigned long), 1024);
  if (err_check)
    throw std::runtime_error("scan_points: CAN'T INITIALIZE VECTOR, CODE: " +
                             std::to_string(err_check));

  err_check = naive_scan_points(root, qs.get(), &result);
  if (err_check) {
    free_vector(&result);
    throw std::runtime_error("scan_points: CODE: " + std::to_string(err_check));
  }

  std::vector<std::pair<unsigned long, unsigned long>> out;

  for (int i = 0; i < result.nof_items; i++) {
    struct pair2dl *current;
    get_element_at(&result, i, (char **)&current);
    out.emplace_back(current->col, current->row);
  }

  free_vector(&result);

  return out;
}

void serialize_block(struct block *b, std::vector<uint64_t> &children_ids,
                     proto_msg::K2Tree *to_feed) {
  proto_msg::Block *block = to_feed->add_blocks();

  uint32_t block_index = b->block_index;
  uint32_t block_depth = b->block_depth;
  uint32_t topology_sz = b->bt->bv->size_in_bits;
  uint32_t frontier_sz = b->bf->frontier.nof_items;
  uint32_t container_sz = b->bt->bv->container_size;
  uint32_t max_nodes_count = b->max_node_count;

  block->set_block_index(block_index);
  block->set_block_depth(block_depth);
  block->set_topology_sz(topology_sz);
  block->set_frontier_sz(frontier_sz);
  block->set_container_sz(container_sz);
  block->set_max_node_count(max_nodes_count);

  for (auto children_id : children_ids) {
    block->add_children_ids(children_id);
  }

  struct vector *frontier_preorders = &b->bf->frontier;
  for (int i = 0; i < frontier_preorders->nof_items; i++) {
    uint32_t current_preorder =
        reinterpret_cast<uint32_t *>(frontier_preorders->data)[i];
    block->add_frontier(current_preorder);
  }

  uint32_t *data = b->bt->bv->container;

  for (int i = 0; i < (int)container_sz; i++) {
    uint32_t current_sub_block = data[i];
    block->add_topology(current_sub_block);
  }
}

uint32_t traverse_tree_proto(struct block *b, proto_msg::K2Tree *to_feed) {
  struct vector &child_blocks = b->bf->blocks;

  std::vector<uint64_t> children_ids;

  uint32_t blocks_counted = 1;

  for (int i = 0; i < child_blocks.nof_items; i++) {
    struct block **current_child_block;
    get_element_at(&child_blocks, i, (char **)&current_child_block);
    children_ids.push_back((uint64_t)(*current_child_block)->block_index);
    blocks_counted += traverse_tree_proto(*current_child_block, to_feed);
  }

  serialize_block(b, children_ids, to_feed);
  return blocks_counted;
}

void K2Tree::produce_proto(proto_msg::K2Tree *to_feed) {
  to_feed->set_tree_depth(root->tree_depth);
  auto blocks_counted = traverse_tree_proto(root, to_feed);
  to_feed->set_blocks_qty(blocks_counted);
}

K2Tree::K2Tree(const proto_msg::K2Tree &k2tree_proto) {
  uint32_t tree_depth = k2tree_proto.tree_depth();

  std::unordered_map<uint64_t, struct block *> blocks_map;

  for (uint32_t i = 0; i < k2tree_proto.blocks_qty(); i++) {
    auto &block = k2tree_proto.blocks(i);

    struct block *new_block = k2tree_alloc_block();
    struct block_topology *new_block_topology = k2tree_alloc_block_topology();
    struct bitvector *bv = k2tree_alloc_bitvector();
    struct block_frontier *new_block_frontier = k2tree_alloc_block_frontier();

    bv->container_size = block.container_sz();

    struct u32array_alloc alloc_container =
        k2tree_alloc_u32array((int)bv->container_size);
    bv->container = reinterpret_cast<BVCTYPE *>(alloc_container.data);
    for (int sub_block_i = 0; sub_block_i < block.topology_size();
         sub_block_i++) {
      bv->container[sub_block_i] = block.topology(sub_block_i);
    }
    bv->size_in_bits = block.topology_sz();
    bv->alloc_tag = alloc_container.size;

    init_block_topology(new_block_topology, bv, block.topology_sz() / 4);

    new_block->block_index = block.block_index();
    new_block->tree_depth = tree_depth;
    new_block->max_node_count = block.max_node_count();
    new_block->block_depth = block.block_depth();
    new_block->bt = new_block_topology;
    if (block.block_depth() == 0) {
      root = new_block;
    }

    init_block_frontier_with_capacity(new_block_frontier, block.frontier_sz());

    new_block_frontier->frontier.nof_items = block.frontier_sz();
    new_block_frontier->blocks.nof_items = block.frontier_sz();

    for (int j = 0; j < block.frontier_size(); j++) {
      uint32_t frontier_element = block.frontier(j);
      set_element_at(&new_block_frontier->frontier,
                     reinterpret_cast<char *>(&frontier_element), j);
    }

    new_block->bf = new_block_frontier;

    blocks_map[block.block_index()] = new_block;
  }

  for (uint32_t i = 0; i < k2tree_proto.blocks_qty(); i++) {
    auto &block = k2tree_proto.blocks(i);
    auto *b = blocks_map[block.block_index()];

    for (int j = 0; j < block.children_ids_size(); j++) {
      auto child_index = block.children_ids(j);
      struct block *child_block = blocks_map[child_index];
      if (child_block == nullptr) {
        throw std::runtime_error("GOT NULL PTR");
      }
      set_element_at(&b->bf->blocks, reinterpret_cast<char *>(&child_block), j);
    }

    b->root = root;
  }

  qs = std::make_unique<struct queries_state>();
  init_queries_state(qs.get(), tree_depth);
}

void rec_occup_ratio_count(struct block *b, K2TreeStats &k2tree_stats) {
  k2tree_stats.allocated_u32s += b->bt->bv->container_size;
  k2tree_stats.nodes_count += b->bt->nodes_count;
  k2tree_stats.containers_sz_sum += sizeof(struct block) +
                                    sizeof(struct block_topology) +
                                    sizeof(struct block_frontier);
  k2tree_stats.frontier_data +=
      b->bf->frontier.nof_items * b->bf->frontier.element_size;
  k2tree_stats.blocks_data +=
      b->bf->blocks.nof_items * b->bf->blocks.element_size;
  k2tree_stats.blocks_counted += 1;

  struct vector children = b->bf->blocks;
  for (int i = 0; i < children.nof_items; i++) {
    struct block *child_block;
    get_element_at(&children, i, (char **)&child_block);
    rec_occup_ratio_count(child_block, k2tree_stats);
  }
}

K2TreeStats K2Tree::k2tree_stats() {
  K2TreeStats result{};
  result.allocated_u32s = 0;
  result.nodes_count = 0;
  result.containers_sz_sum = 0;
  result.frontier_data = 0;
  result.blocks_data = 0;
  result.number_of_points = 0;
  result.blocks_counted = 0;

  rec_occup_ratio_count(root, result);

  auto scanned_points = scan_points();
  result.number_of_points = scanned_points.size();

  return result;
}

K2Tree::K2Tree(
    proto_msg::CacheFeedFullK2TreeRequest &cache_feed_full_k2tree_request)
    : K2Tree(cache_feed_full_k2tree_request.tree_depth()) {
  for (int i = 0; i < cache_feed_full_k2tree_request.subject_object_pair_size();
       i++) {
    auto subject =
        cache_feed_full_k2tree_request.subject_object_pair(i).subject();
    auto predicate =
        cache_feed_full_k2tree_request.subject_object_pair(i).object();
    insert(subject, predicate);
  }
}

unsigned long K2Tree::get_tree_depth() { return root->tree_depth; }

bool K2Tree::same_as(const K2Tree &other) {
  return same_blocks(root, other.root);
}

bool same_blocks(const struct block *lhs, const struct block *rhs) {
  auto fields_result = lhs->block_depth == rhs->block_depth &&
                       lhs->tree_depth == rhs->tree_depth &&
                       lhs->max_node_count == rhs->max_node_count;
  auto bf_result = same_block_frontiers(lhs->bf, rhs->bf);
  auto bt_result = same_block_topologies(lhs->bt, rhs->bt);
  auto result = fields_result && bf_result && bt_result;
  return result;
}

bool same_block_topologies(const block_topology *lhs,
                           const block_topology *rhs) {
  return lhs->nodes_count == rhs->nodes_count && same_bvs(lhs->bv, rhs->bv);
}

bool same_bvs(const bitvector *lhs, const bitvector *rhs) {
  if (!(lhs->container_size == rhs->container_size &&
        lhs->alloc_tag == rhs->alloc_tag &&
        lhs->size_in_bits == rhs->size_in_bits)) {
    return false;
  }

  for (uint32_t i = 0; i < lhs->container_size; i++) {
    if (lhs->container[i] != rhs->container[i]) {
      return false;
    }
  }
  return true;
}

bool same_block_frontiers(const block_frontier *lhs,
                          const block_frontier *rhs) {
  if (!same_vectors(lhs->frontier, rhs->frontier)) {
    return false;
  }
  if (lhs->blocks.element_size != rhs->blocks.element_size ||
      lhs->blocks.nof_items != rhs->blocks.nof_items) {
    return false;
  }

  for (int i = 0; i < lhs->blocks.nof_items; i++) {
    struct block **lhs_ptr, **rhs_ptr;
    get_element_at(const_cast<struct vector *>(&lhs->blocks), i,
                   reinterpret_cast<char **>(&lhs_ptr));
    get_element_at(const_cast<struct vector *>(&rhs->blocks), i,
                   reinterpret_cast<char **>(&rhs_ptr));

    if (!same_blocks(*lhs_ptr, *rhs_ptr)) {
      return false;
    }
  }

  return true;
}

bool same_vectors(const struct vector &lhs, const struct vector &rhs) {
  if (lhs.element_size != rhs.element_size || lhs.nof_items != rhs.nof_items) {
    return false;
  }

  for (int i = 0; i < lhs.nof_items; i++) {
    uint32_t *lhs_element, *rhs_element;
    get_element_at(const_cast<struct vector *>(&lhs), i,
                   reinterpret_cast<char **>(&lhs_element));
    get_element_at(const_cast<struct vector *>(&rhs), i,
                   reinterpret_cast<char **>(&rhs_element));
    if (*lhs_element != *rhs_element) {
      return false;
    }
  }
  return true;
}
