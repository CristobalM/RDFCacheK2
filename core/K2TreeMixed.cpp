#include "K2TreeMixed.hpp"

#include "block_serialization.hpp"
#include "serialization_util.hpp"
#include <algorithm>
#include <stdexcept>
#include <vector>

#include "BandScanner.hpp"
#include "EmptyScanner.hpp"
#include "FullScanner.hpp"
#include "K2TreeScanner.hpp"
#include "block_stats.hpp"

K2TreeMixed::K2TreeMixed(uint32_t treedepth)
    : K2TreeMixed(treedepth, MAX_NODES_IN_BLOCK) {}

K2TreeMixed::K2TreeMixed(uint32_t treedepth, uint32_t max_node_count)
    : K2TreeMixed(treedepth, max_node_count, std::max(treedepth / 4, 1U)) {}
K2TreeMixed::K2TreeMixed(uint32_t treedepth, uint32_t max_node_count,
                         uint32_t cut_depth)
    : K2TreeMixed(create_k2node(), treedepth, max_node_count, cut_depth, 0) {}

K2TreeMixed::K2TreeMixed(struct k2node *root, uint32_t treedepth,
                         uint32_t max_node_count, uint32_t cut_depth,
                         uint64_t points_count)
    : root(root), points_count(points_count), tree_depth(treedepth),
      cut_depth(cut_depth), max_nodes_count(max_node_count) {}

K2TreeMixed::K2TreeMixed(K2TreeConfig config)
    : K2TreeMixed(config.treedepth, config.max_node_count, config.cut_depth) {}

void K2TreeMixed::clean_up() {
  if (root) {
    free_rec_k2node(root, 0, cut_depth);
  }
  root = nullptr;
}

K2TreeMixed::K2TreeMixed(K2TreeMixed &&other) noexcept {
  root = nullptr;
  std::swap(root, other.root);
  points_count = other.points_count;
  tree_depth = other.tree_depth;
  cut_depth = other.cut_depth;
  max_nodes_count = other.max_nodes_count;
}

K2TreeMixed &K2TreeMixed::operator=(K2TreeMixed &&other) noexcept {
  clean_up();
  std::swap(root, other.root);
  points_count = other.points_count;
  tree_depth = other.tree_depth;
  cut_depth = other.cut_depth;
  max_nodes_count = other.max_nodes_count;
  return *this;
}

K2TreeMixed::~K2TreeMixed() { clean_up(); }

void K2TreeMixed::insert(unsigned long col, unsigned long row,
                         K2QStateWrapper &stw) {
  int already_exists;
  k2node_insert_point(root, col, row, stw.get_ptr(), &already_exists);
  if (!already_exists)
    points_count++;
}

void K2TreeMixed::remove(unsigned long col, unsigned long row,
                         K2QStateWrapper &stw) {
  int already_not_exists;
  k2node_delete_point(root, col, row, stw.get_ptr(), &already_not_exists);
  if (!already_not_exists)
    points_count--;
}
void K2TreeMixed::remove(unsigned long col, unsigned long row) {
  auto stw = create_k2qw();
  remove(col, row, stw);
}

bool K2TreeMixed::has(unsigned long col, unsigned long row,
                      K2QStateWrapper &stw) const {
  int result;
  k2node_has_point(root, col, row, stw.get_ptr(), &result);
  return (bool)result;
}

unsigned long K2TreeMixed::get_tree_depth() const { return tree_depth; }

std::vector<std::pair<unsigned long, unsigned long>>
K2TreeMixed::get_all_points(K2QStateWrapper &stw) {
  struct vector_pair2dl_t result;
  vector_pair2dl_t__init_vector(&result);
  k2node_naive_scan_points(root, stw.get_ptr(), &result);
  std::vector<std::pair<unsigned long, unsigned long>> out;

  for (long i = 0; i < result.nof_items; i++) {
    struct pair2dl current = result.data[i];
    out.emplace_back(current.col, current.row);
  }
  vector_pair2dl_t__free_vector(&result);
  return out;
}

void K2TreeMixed::scan_points(point_reporter_fun_t fun_reporter,
                              void *report_state, K2QStateWrapper &stw) const {
  int err = k2node_scan_points_interactively(root, stw.get_ptr(), fun_reporter,
                                             report_state);
  if (err)
    throw std::runtime_error("k2node_scan_points_interactively threw error " +
                             std::to_string(err));
}
void K2TreeMixed::traverse_row(unsigned long row,
                               point_reporter_fun_t fun_reporter,
                               void *report_state, K2QStateWrapper &stw) const {
  int err = k2node_report_row_interactively(root, row, stw.get_ptr(),
                                            fun_reporter, report_state);
  if (err)
    throw std::runtime_error("k2node_report_row_interactively threw error " +
                             std::to_string(err));
}
void K2TreeMixed::traverse_column(unsigned long column,
                                  point_reporter_fun_t fun_reporter,
                                  void *report_state,
                                  K2QStateWrapper &stw) const {
  int err = k2node_report_column_interactively(root, column, stw.get_ptr(),
                                               fun_reporter, report_state);
  if (err)
    throw std::runtime_error("k2node_report_column_interactively threw error " +
                             std::to_string(err));
}

struct k2tree_measurement K2TreeMixed::measure_in_memory_size() const {
  return k2node_measure_tree_size(root, cut_depth);
}

k2tree_measurement K2TreeMixed::k2tree_stats() const {
  return k2node_measure_tree_size(root, cut_depth);
}

bool same_k2node(struct k2node *lhs, struct k2node *rhs, uint32_t current_depth,
                 uint32_t cut_depth) {
  if (current_depth < cut_depth) {
    for (int i = 0; i < 4; i++) {
      if ((bool)lhs->k2subtree.children[i] !=
          (bool)rhs->k2subtree.children[i]) {
        return false;
      }
    }
    for (int i = 0; i < 4; i++) {
      auto *lhs_child = lhs->k2subtree.children[i];
      auto *rhs_child = rhs->k2subtree.children[i];

      if ((lhs_child == nullptr && rhs_child != nullptr) ||
          (lhs_child != nullptr && rhs_child == nullptr)) {
        return false;
      }

      if (lhs_child != nullptr && rhs_child != nullptr &&
          !same_k2node(lhs_child, rhs_child, current_depth + 1, cut_depth)) {
        return false;
      }
    }
    return true;
  }

  return same_blocks(lhs->k2subtree.block_child, rhs->k2subtree.block_child);
}

bool K2TreeMixed::identical_structure_as(const K2TreeMixed &other) const {
  return same_k2node(root, other.root, 0, cut_depth);
}

int count_k2nodes_wchildren(struct k2node *node, uint32_t current_depth,
                            uint32_t cut_depth) {
  if (current_depth < cut_depth) {
    int sum = 0;
    for (int i = 0; i < 4; i++) {
      if (node->k2subtree.children[i]) {
        sum += count_k2nodes_wchildren(node->k2subtree.children[i],
                                       current_depth + 1, cut_depth);
      }
    }
    return sum + 1;
  }
  return 0;
}

void serialize_to_vec_with_k2node_ptrs(struct k2node *node,
                                       uint32_t current_depth,
                                       uint32_t cut_depth,
                                       std::vector<uint32_t> &data,
                                       uint32_t &current_node_location) {
  if (current_depth < cut_depth) {
    int container_pos =
        4 * current_node_location / 32; // 4*current_node_location/32;
    int subpos_bits_start = (4 * current_node_location) % 32;
    uint32_t &container = data[container_pos];
    current_node_location++;
    for (int i = 0; i < 4; i++) {
      if (node->k2subtree.children[i]) {
        auto bit_location = static_cast<uint32_t>(31 - (subpos_bits_start + i));
        container |= (1U << bit_location);
      }
    }
    for (int i = 0; i < 4; i++) {
      if (node->k2subtree.children[i]) {
        serialize_to_vec_with_k2node_ptrs(node->k2subtree.children[i],
                                          current_depth + 1, cut_depth, data,
                                          current_node_location);
      }
    }
  }
}

unsigned long write_blocks_from_k2nodes(struct k2node *node,
                                        uint32_t current_depth,
                                        uint32_t cut_depth,
                                        uint32_t max_nodes_count,
                                        uint32_t tree_depth, std::ostream &os) {
  unsigned long bytes_written = 0;
  if (current_depth < cut_depth) {
    for (int i = 0; i < 4; i++) {
      if (node->k2subtree.children[i]) {
        bytes_written += write_blocks_from_k2nodes(
            node->k2subtree.children[i], current_depth + 1, cut_depth,
            max_nodes_count, tree_depth, os);
      }
    }
  } else {
    k2tree_data serialization_data;
    serialization_data.root = *node->k2subtree.block_child;
    serialization_data.max_node_count = max_nodes_count;
    serialization_data.treedepth = tree_depth;
    bytes_written += write_tree_to_ostream(serialization_data, os);
  }

  return bytes_written;
}

unsigned long K2TreeMixed::write_to_ostream(std::ostream &os) const {
  int k2nodes_count_wchildren = count_k2nodes_wchildren(root, 0, cut_depth);
  int bits_count = k2nodes_count_wchildren * 4;
  const int bits_per_container = (sizeof(uint32_t) * 8);
  int containers_count = (bits_count / bits_per_container) +
                         ((bits_count % bits_per_container == 0) ? 0 : 1);
  std::vector<uint32_t> containers(containers_count, 0);
  uint32_t current_node_location = 0;

  serialize_to_vec_with_k2node_ptrs(root, 0, cut_depth, containers,
                                    current_node_location);

  write_u64(os, points_count);
  write_u32(os, tree_depth);
  write_u32(os, cut_depth);
  write_u32(os, max_nodes_count);
  write_u32(os, static_cast<uint32_t>(containers_count));
  unsigned long bytes_written = sizeof(uint32_t) * 4 + sizeof(uint64_t);
  for (int i = 0; i < containers_count; i++) {
    write_u32(os, containers[i]);
    bytes_written += sizeof(uint32_t);
  }
  bytes_written += write_blocks_from_k2nodes(root, 0, cut_depth,
                                             max_nodes_count, tree_depth, os);

  return bytes_written;
}

struct k2node *deserialize_k2node_tree(std::istream &is,
                                       std::vector<uint32_t> &containers,
                                       uint32_t current_depth,
                                       uint32_t cut_depth,
                                       uint32_t &current_node_location) {
  struct k2node *node = create_k2node();
  if (current_depth < cut_depth) {
    int container_pos = 4 * static_cast<int>(current_node_location) / 32;
    int subpos_bits_start = (4 * static_cast<int>(current_node_location)) % 32;
    uint32_t container = containers[container_pos];
    current_node_location++;
    for (int i = 0; i < 4; i++) {
      if (container & (1U << (31 - (subpos_bits_start + i)))) {
        node->k2subtree.children[i] =
            deserialize_k2node_tree(is, containers, current_depth + 1,
                                    cut_depth, current_node_location);
      }
    }
  } else {
    k2tree_data subtree_deserialized = read_tree_from_istream(is);
    node->k2subtree.block_child = create_block();
    *node->k2subtree.block_child = subtree_deserialized.root;
  }

  return node;
}

K2TreeMixed K2TreeMixed::read_from_istream(std::istream &is) {
  uint64_t points_count = read_u64(is);
  uint32_t k2tree_depth = read_u32(is);
  uint32_t cut_depth = read_u32(is);
  uint32_t max_nodes_count = read_u32(is);
  uint32_t containers_count = read_u32(is);
  std::vector<uint32_t> containers(containers_count, 0);
  for (uint32_t i = 0; i < containers_count; i++) {
    containers[i] = read_u32(is);
  }
  uint32_t current_node_location = 0;
  struct k2node *root = deserialize_k2node_tree(is, containers, 0, cut_depth,
                                                current_node_location);
  return K2TreeMixed(root, k2tree_depth, max_nodes_count, cut_depth,
                     points_count);
}

size_t K2TreeMixed::size() const { return points_count; }

struct k2node *K2TreeMixed::get_root_k2node() {
  return root;
}

k2node *deserialize_k2node_tree(std::istream &is,
                                std::vector<uint32_t> &containers,
                                uint32_t current_depth, uint32_t cut_depth,
                                uint32_t &current_node_location,
                                MemorySegment *memory_segment);
bool K2TreeMixed::has_valid_structure(K2QStateWrapper &stw) const {

  int result = debug_validate_k2node_rec(root, stw.get_ptr(), 0);

  return result == 0;
}

std::unique_ptr<K2TreeScanner> K2TreeMixed::create_full_scanner() {
  return std::make_unique<FullScanner>(*this);
}
std::unique_ptr<K2TreeScanner>
K2TreeMixed::create_band_scanner(unsigned long band,
                                 K2TreeScanner::BandType band_type) {
  return std::make_unique<BandScanner>(*this, band, band_type);
}
std::unique_ptr<K2TreeScanner> K2TreeMixed::create_empty_scanner() {
  return std::make_unique<EmptyScanner>(*this);
}
K2QStateWrapper K2TreeMixed::create_k2qw() const {
  return {tree_depth, cut_depth, max_nodes_count};
}
std::vector<std::pair<unsigned long, unsigned long>>
K2TreeMixed::get_all_points() {
  auto stw = create_k2qw();
  return get_all_points(stw);
}
bool K2TreeMixed::has_valid_structure() const {
  auto stw = create_k2qw();
  return has_valid_structure(stw);
}
void K2TreeMixed::insert(unsigned long col, unsigned long row) {
  auto stw = create_k2qw();
  return insert(col, row, stw);
}
bool K2TreeMixed::has(unsigned long col, unsigned long row) const {
  auto stw = create_k2qw();
  return has(col, row, stw);
}
void K2TreeMixed::scan_points(point_reporter_fun_t fun_reporter,
                              void *report_state) const {
  auto stw = create_k2qw();
  return scan_points(fun_reporter, report_state, stw);
}
void K2TreeMixed::traverse_row(unsigned long row,
                               point_reporter_fun_t fun_reporter,
                               void *report_state) const {
  auto stw = create_k2qw();
  return traverse_row(row, fun_reporter, report_state, stw);
}
void K2TreeMixed::traverse_column(unsigned long column,
                                  point_reporter_fun_t fun_reporter,
                                  void *report_state) const {
  auto stw = create_k2qw();
  return traverse_column(column, fun_reporter, report_state, stw);
}
K2TreeMixed K2TreeMixed::read_from_istream(std::istream &is,
                                           MemorySegment *memory_segment) {
  uint64_t points_count = read_u64(is);
  uint32_t k2tree_depth = read_u32(is);
  uint32_t cut_depth = read_u32(is);
  uint32_t max_nodes_count = read_u32(is);
  uint32_t containers_count = read_u32(is);
  std::vector<uint32_t> containers(containers_count, 0);
  for (uint32_t i = 0; i < containers_count; i++) {
    containers[i] = read_u32(is);
  }
  uint32_t current_node_location = 0;
  struct k2node *root = deserialize_k2node_tree(
      is, containers, 0, cut_depth, current_node_location, memory_segment);
  return K2TreeMixed(root, k2tree_depth, max_nodes_count, cut_depth,
                     points_count);
}

static NODES_BV_T *copy_preorders(NODES_BV_T *original, int size) {
  auto *preorders = (NODES_BV_T *)k2tree_alloc_preorders(size);
  std::copy(original, original + size, preorders);
  return preorders;
}

static BVCTYPE *copy_block_container(BVCTYPE *original, int size) {
  auto *container = k2tree_alloc_u32array(size);
  std::copy(original, original + size, container);
  return container;
}

static void copy_block_tree(block *original, block *copy) {
  copy->children_blocks = k2tree_alloc_blocks_array(original->children);
  copy->children = original->children;
  copy->container_size = original->container_size;
  copy->nodes_count = original->nodes_count;
  copy->preorders = copy_preorders(original->preorders, original->children);
  copy->container =
      copy_block_container(original->container, original->container_size);
  for (int i = 0; i < original->children; i++) {
    auto *child = &copy->children_blocks[i];
    copy_block_tree(&original->children_blocks[i], child);
  }
}

static k2node *copy_tree(k2node *root, int cut_depth, int tree_depth,
                         int current_depth = 0) {
  auto *node_copy = create_k2node();

  if (current_depth < cut_depth) {
    for (int i = 0; i < 4; i++) {
      auto *child = root->k2subtree.children[i];
      k2node *copied_child{};
      if (child) {
        copied_child =
            copy_tree(child, cut_depth, tree_depth, current_depth + 1);
      }
      node_copy->k2subtree.children[i] = copied_child;
    }
  } else {
    node_copy->k2subtree.block_child = create_block();
    copy_block_tree(root->k2subtree.block_child,
                    node_copy->k2subtree.block_child);
  }
  return node_copy;
}

K2TreeMixed::K2TreeMixed(const K2TreeMixed &other) {
  root = copy_tree(other.root, other.cut_depth, other.tree_depth);
  points_count = other.points_count;
  tree_depth = other.tree_depth;
  cut_depth = other.cut_depth;
  max_nodes_count = other.max_nodes_count;
}

K2TreeMixed &K2TreeMixed::operator=(const K2TreeMixed &other) {
  root = copy_tree(other.root, other.cut_depth, other.tree_depth);
  points_count = other.points_count;
  tree_depth = other.tree_depth;
  cut_depth = other.cut_depth;
  max_nodes_count = other.max_nodes_count;
  return *this;
}

static bool blocks_any_same_ref(block *lhs, block *rhs) {
  if (!lhs || !rhs)
    return false; // if any of the two is nullptr we can't continue comparing
  if (lhs == rhs)
    return true;
  if (lhs->preorders == rhs->preorders)
    return true;
  if (lhs->children_blocks == rhs->children_blocks)
    return true;
  if (lhs->container == rhs->container)
    return true;
  if (lhs->children != rhs->children)
    return false; // can't continue comparing as the same structure is broken
  for (int i = 0; i < lhs->children; i++) {
    if (blocks_any_same_ref(&lhs->children_blocks[i], &rhs->children_blocks[i]))
      return true;
  }
  return false;
}

static bool k2node_any_same_ref(k2node *lhs, k2node *rhs, int tree_depth,
                                int cut_depth, int current_depth) {
  if (!lhs || !rhs)
    return false; // if any of the two is nullptr we can't continue comparing
  if (lhs == rhs)
    return true;
  if (current_depth < cut_depth) {
    for (int i = 0; i < 4; i++) {
      if (lhs->k2subtree.children[i] != nullptr &&
          rhs->k2subtree.children[i] != nullptr &&
          lhs->k2subtree.children[i] == rhs->k2subtree.children[i])
        return true;
    }
    for (int i = 0; i < 4; i++) {
      if (k2node_any_same_ref(lhs->k2subtree.children[i],
                              rhs->k2subtree.children[i], tree_depth, cut_depth,
                              current_depth + 1))
        return true;
    }
    return false;
  }

  return blocks_any_same_ref(lhs->k2subtree.block_child,
                             rhs->k2subtree.block_child);
}

bool K2TreeMixed::shares_any_reference_to(K2TreeMixed &other) {
  // for this check to make sense, we assume that they share the same structure,
  // that is, identical_structure_as is true
  if (cut_depth != other.cut_depth || tree_depth != other.tree_depth ||
      max_nodes_count != other.max_nodes_count ||
      points_count != other.points_count)
    return false;
  return k2node_any_same_ref(root, other.root, tree_depth, cut_depth, 0);
}

k2node *deserialize_k2node_tree(std::istream &is,
                                std::vector<uint32_t> &containers,
                                uint32_t current_depth, uint32_t cut_depth,
                                uint32_t &current_node_location,
                                MemorySegment *memory_segment) {
  auto *node_mem_space = memory_segment->require_bytes(sizeof(struct k2node));
  struct k2node *node = new (node_mem_space) struct k2node();
  if (current_depth < cut_depth) {
    int container_pos = 4 * static_cast<int>(current_node_location) / 32;
    int subpos_bits_start = (4 * static_cast<int>(current_node_location)) % 32;
    uint32_t container = containers[container_pos];
    current_node_location++;
    for (int i = 0; i < 4; i++) {
      if (container & (1U << (31 - (subpos_bits_start + i)))) {
        node->k2subtree.children[i] = deserialize_k2node_tree(
            is, containers, current_depth + 1, cut_depth, current_node_location,
            memory_segment);
      }
    }
  } else {
    k2tree_data subtree_deserialized =
        read_tree_from_istream(is, memory_segment);
    auto *block_mem_space = memory_segment->require_bytes(sizeof(struct block));
    node->k2subtree.block_child = new (block_mem_space) struct block();
    *node->k2subtree.block_child = subtree_deserialized.root;
  }

  return node;
}