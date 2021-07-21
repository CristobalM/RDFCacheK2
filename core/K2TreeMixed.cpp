#include "K2TreeMixed.hpp"

#include "block_serialization.hpp"
#include "serialization_util.hpp"
#include <algorithm>
#include <stdexcept>
#include <vector>

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
    : root(root), points_count(points_count) {
  st = std::make_unique<struct k2qstate>();
  int err = init_k2qstate(st.get(), treedepth, max_node_count, cut_depth);
  if (err)
    throw std::runtime_error("Cant init k2qstate: " + std::to_string(err));
}

K2TreeMixed::K2TreeMixed(K2TreeConfig config)
    : K2TreeMixed(config.treedepth, config.max_node_count, config.cut_depth) {}

void K2TreeMixed::clean_up() {
  if (root && st) {
    free_rec_k2node(root, 0, st->cut_depth);
  }
  root = nullptr;
  if (st) {
    clean_k2qstate(st.get());
  }
  st = nullptr;
}

K2TreeMixed::K2TreeMixed(K2TreeMixed &&other) {
  clean_up();
  std::swap(root, other.root);
  std::swap(st, other.st);
  points_count = other.points_count;
}

K2TreeMixed &K2TreeMixed::operator=(K2TreeMixed &&other) {
  clean_up();
  std::swap(root, other.root);
  std::swap(st, other.st);
  points_count = other.points_count;
  return *this;
}

K2TreeMixed::~K2TreeMixed() { clean_up(); }

void K2TreeMixed::insert(unsigned long col, unsigned long row) {
  int already_exists;
  k2node_insert_point(root, col, row, st.get(), &already_exists);
  if (!already_exists)
    points_count++;
}

bool K2TreeMixed::has(unsigned long col, unsigned long row) const {
  int result;
  k2node_has_point(root, col, row, st.get(), &result);
  return (bool)result;
}

unsigned long K2TreeMixed::get_tree_depth() const { return st->k2tree_depth; }

std::vector<std::pair<unsigned long, unsigned long>>
K2TreeMixed::get_all_points() {
  struct vector_pair2dl_t result;
  vector_pair2dl_t__init_vector(&result);
  k2node_naive_scan_points(root, st.get(), &result);
  std::vector<std::pair<unsigned long, unsigned long>> out;

  for (long i = 0; i < result.nof_items; i++) {
    struct pair2dl current = result.data[i];
    out.emplace_back(current.col, current.row);
  }
  vector_pair2dl_t__free_vector(&result);
  return out;
}

std::vector<unsigned long> K2TreeMixed::get_row(unsigned long row) const {
  struct vector_pair2dl_t result;
  vector_pair2dl_t__init_vector(&result);
  k2node_report_row(root, row, st.get(), &result);
  std::vector<unsigned long> out;
  for (long i = 0; i < result.nof_items; i++) {
    unsigned long col = result.data[i].col;
    out.push_back(col);
  }
  vector_pair2dl_t__free_vector(&result);
  return out;
}

std::vector<unsigned long> K2TreeMixed::get_column(unsigned long col) const {
  struct vector_pair2dl_t result;
  vector_pair2dl_t__init_vector(&result);
  k2node_report_column(root, col, st.get(), &result);
  std::vector<unsigned long> out;
  for (long i = 0; i < result.nof_items; i++) {
    unsigned long row = result.data[i].row;
    out.push_back(row);
  }
  vector_pair2dl_t__free_vector(&result);
  return out;
}

void K2TreeMixed::scan_points(point_reporter_fun_t fun_reporter,
                              void *report_state) const {
  int err = k2node_scan_points_interactively(root, st.get(), fun_reporter,
                                             report_state);
  if (err)
    throw std::runtime_error("k2node_scan_points_interactively threw error " +
                             std::to_string(err));
}
void K2TreeMixed::traverse_row(unsigned long row,
                               point_reporter_fun_t fun_reporter,
                               void *report_state) const {
  int err = k2node_report_row_interactively(root, row, st.get(), fun_reporter,
                                            report_state);
  if (err)
    throw std::runtime_error("k2node_report_row_interactively threw error " +
                             std::to_string(err));
}
void K2TreeMixed::traverse_column(unsigned long column,
                                  point_reporter_fun_t fun_reporter,
                                  void *report_state) const {
  int err = k2node_report_column_interactively(root, column, st.get(),
                                               fun_reporter, report_state);
  if (err)
    throw std::runtime_error("k2node_report_column_interactively threw error " +
                             std::to_string(err));
}

struct k2tree_measurement K2TreeMixed::measure_in_memory_size() const {
  return k2node_measure_tree_size(root, st->cut_depth);
}

ResultTable K2TreeMixed::column_as_table(unsigned long) const {
  throw "column_as_table not implemented";
}

ResultTable K2TreeMixed::row_as_table(unsigned long) const {
  throw "row_as_table not implemented";
}

K2TreeStats K2TreeMixed::k2tree_stats() const {
  throw "k2tree_stats not implemented";
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

bool K2TreeMixed::same_as(const K2TreeMixed &other) const {
  return same_k2node(root, other.root, 0, st->cut_depth);
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
        uint32_t bit_location = (31 - (subpos_bits_start + i));
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
                                        uint32_t cut_depth, std::ostream &os,
                                        struct k2qstate *st) {
  unsigned long bytes_written = 0;
  if (current_depth < cut_depth) {
    for (int i = 0; i < 4; i++) {
      if (node->k2subtree.children[i]) {
        bytes_written += write_blocks_from_k2nodes(
            node->k2subtree.children[i], current_depth + 1, cut_depth, os, st);
      }
    }
  } else {
    k2tree_data serialization_data;
    serialization_data.root = node->k2subtree.block_child;
    serialization_data.max_node_count = st->qs.max_nodes_count;
    serialization_data.treedepth = st->qs.treedepth;
    bytes_written += write_tree_to_ostream(serialization_data, os);
  }

  return bytes_written;
}

unsigned long K2TreeMixed::write_to_ostream(std::ostream &os) const {
  int k2nodes_count_wchildren = count_k2nodes_wchildren(root, 0, st->cut_depth);
  int bits_count = k2nodes_count_wchildren * 4;
  const int bits_per_container = (sizeof(uint32_t) * 8);
  int containers_count = (bits_count / bits_per_container) +
                         ((bits_count % bits_per_container == 0) ? 0 : 1);
  std::vector<uint32_t> containers(containers_count, 0);
  uint32_t current_node_location = 0;

  serialize_to_vec_with_k2node_ptrs(root, 0, st->cut_depth, containers,
                                    current_node_location);

  write_u64(os, points_count);
  write_u32(os, st->k2tree_depth);
  write_u32(os, st->cut_depth);
  write_u32(os, st->qs.max_nodes_count);
  write_u32(os, containers_count);
  unsigned long bytes_written = sizeof(uint32_t) * 4 + sizeof(uint64_t);
  for (int i = 0; i < containers_count; i++) {
    write_u32(os, containers[i]);
    bytes_written += sizeof(uint32_t);
  }
  bytes_written +=
      write_blocks_from_k2nodes(root, 0, st->cut_depth, os, st.get());

  return bytes_written;
}

struct k2node *deserialize_k2node_tree(std::istream &is,
                                       std::vector<uint32_t> &containers,
                                       uint32_t current_depth,
                                       uint32_t cut_depth,
                                       uint32_t &current_node_location) {
  struct k2node *node = create_k2node();
  if (current_depth < cut_depth) {
    int container_pos = 4 * current_node_location / 32;
    int subpos_bits_start = (4 * current_node_location) % 32;
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
    node->k2subtree.block_child = subtree_deserialized.root;
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

std::vector<unsigned long> K2TreeMixed::sip_join_k2trees(
    const std::vector<const K2TreeMixed *> &trees,
    std::vector<struct sip_ipoint> &join_coordinates) {
  if (trees.size() != join_coordinates.size()) {
    throw std::runtime_error(
        "Trees amount differ from join coordinates amount " +
        std::to_string(trees.size()) +
        " != " + std::to_string(join_coordinates.size()));
  }

  std::vector<struct k2node *> root_nodes(trees.size());
  std::vector<struct k2qstate *> states(trees.size());

  for (size_t i = 0; i < trees.size(); i++) {
    root_nodes[i] = trees[i]->root;
    states[i] = trees[i]->st.get();
  }

  struct k2node_sip_input sip_input;
  sip_input.nodes = root_nodes.data();
  sip_input.sts = states.data();
  sip_input.join_coords = join_coordinates.data();
  sip_input.join_size = join_coordinates.size();

  std::vector<unsigned long> result;

  int err = k2node_sip_join(
      sip_input,
      [](unsigned long coord, void *report_state) {
        reinterpret_cast<std::vector<unsigned long> *>(report_state)
            ->push_back(coord);
      },
      &result);

  if (err) {
    throw std::runtime_error(
        "K2TreeMixed::sip_join_k2trees:: error in k2node_sip_join: " +
        std::to_string(err));
  }

  return result;
}

size_t K2TreeMixed::size() const { return points_count; }

struct k2node *K2TreeMixed::get_root_k2node() {
  return root;
}

struct k2qstate *K2TreeMixed::get_k2qstate() {
  return st.get();
}

bool K2TreeMixed::has_valid_structure() const {
  int result = debug_validate_k2node_rec(root, st.get(), 0);

  return result == 0;
}

std::vector<std::pair<unsigned long, unsigned long>>
K2TreeMixed::scan_points_into_vector() const {
  std::vector<std::pair<unsigned long, unsigned long>> result;
  scan_points(
      [](unsigned long col, unsigned long row, void *container_ptr) {
        reinterpret_cast<
            std::vector<std::pair<unsigned long, unsigned long>> *>(
            container_ptr)
            ->push_back({col, row});
      },
      &result);
  return result;
}

K2TreeMixed::FullScanIterator::FullScanIterator(K2TreeMixed &k2tree) {
  k2node_naive_scan_points_lazy_init(k2tree.get_root_k2node(),
                                     k2tree.get_k2qstate(), &lazy_handler);

  init_called = true;
  finished = !lazy_handler.has_next;
  if (!finished) {
    pair2dl_t result;
    k2node_naive_scan_points_lazy_next(&lazy_handler, &result);
    current = {static_cast<unsigned long>(result.col),
               static_cast<unsigned long>(result.row)};
  }
}

K2TreeMixed::FullScanIterator::ul_pair_t &
K2TreeMixed::FullScanIterator::operator*() {
  return current;
}
K2TreeMixed::FullScanIterator::pointer
K2TreeMixed::FullScanIterator::operator->() {
  return &current;
}
K2TreeMixed::FullScanIterator &K2TreeMixed::FullScanIterator::operator++() {
  if (!lazy_handler.has_next) {
    finished = true;
    return *this;
  }
  pair2dl_t result;
  k2node_naive_scan_points_lazy_next(&lazy_handler, &result);
  current = {static_cast<unsigned long>(result.col),
             static_cast<unsigned long>(result.row)};

  return *this;
}
K2TreeMixed::FullScanIterator K2TreeMixed::FullScanIterator::operator++(int) {
  K2TreeMixed::FullScanIterator tmp = *this;
  ++(*this);
  return tmp;
}
K2TreeMixed::FullScanIterator::FullScanIterator(
    const FullScanIterator &original) {
  lazy_handler = deep_copy_handler(original.lazy_handler);
  current = original.current;
  finished = original.finished;
  init_called = original.init_called;
}
k2node_lazy_handler_naive_scan_t
K2TreeMixed::FullScanIterator::deep_copy_handler(
    const k2node_lazy_handler_naive_scan_t &handler) {
  k2node_lazy_handler_naive_scan_t new_handler{};
  new_handler.next_result = handler.next_result;
  new_handler.st = handler.st;
  new_handler.has_next = handler.has_next;
  new_handler.sub_handler = deep_copy_sub_handler(handler.sub_handler);
  new_handler.base_col = handler.base_col;
  new_handler.base_row = handler.base_row;
  new_handler.at_leaf = handler.at_leaf;
  new_handler.states_stack = deep_copy_states_stack(handler.states_stack);
  return new_handler;
}
lazy_handler_naive_scan_t K2TreeMixed::FullScanIterator::deep_copy_sub_handler(
    const lazy_handler_naive_scan_t &handler) {
  lazy_handler_naive_scan_t new_handler{};
  new_handler.has_next = handler.has_next;
  new_handler.next_result = handler.next_result;
  new_handler.qs = handler.qs;
  new_handler.states_stack = deep_copy_sub_states_stack(handler.states_stack);
  return new_handler;
}
k2node_lazy_naive_state_stack
K2TreeMixed::FullScanIterator::deep_copy_states_stack(
    const k2node_lazy_naive_state_stack &original_stack) {
  k2node_lazy_naive_state_stack result{};
  init_k2node_lazy_naive_state_stack(&result,
                                     static_cast<int>(original_stack.capacity));
  for (size_t i = 0; i < static_cast<size_t>(original_stack.capacity); i++) {
    result.data[i] = original_stack.data[i];
  }
  return result;
}
lazy_naive_state_stack
K2TreeMixed::FullScanIterator::deep_copy_sub_states_stack(
    const lazy_naive_state_stack &original_stack) {
  lazy_naive_state_stack result{};
  init_lazy_naive_state_stack(&result,
                              static_cast<int>(original_stack.capacity));

  for (size_t i = 0; i < static_cast<size_t>(original_stack.capacity); i++) {
    result.data[i] = original_stack.data[i];
  }
  return result;
}
K2TreeMixed::FullScanIterator::FullScanIterator(
    K2TreeMixed::FullScanIterator::pointer ptr)
    : finished(!ptr), init_called(false) {
  if (ptr)
    throw std::runtime_error("Non null ptr not allowed");
}
bool operator==(const K2TreeMixed::FullScanIterator &lhs,
                const K2TreeMixed::FullScanIterator &rhs) {
  if (lhs.finished == rhs.finished)
    return true;
  return false;
}
bool operator!=(const K2TreeMixed::FullScanIterator &lhs,
                const K2TreeMixed::FullScanIterator &rhs) {
  return !(lhs == rhs);
}
K2TreeMixed::FullScanIterator::~FullScanIterator() {
  if (init_called)
    k2node_naive_scan_points_lazy_clean(&lazy_handler);
}

K2TreeMixed::FullScanIterator K2TreeMixed::begin_full_scan() {
  return K2TreeMixed::FullScanIterator(*this);
}
K2TreeMixed::FullScanIterator K2TreeMixed::end_full_scan() {
  return K2TreeMixed::FullScanIterator(nullptr);
}

K2TreeMixed::BandScanIterator::pointer
K2TreeMixed::BandScanIterator::operator->() {
  return &current;
}

K2TreeMixed::BandScanIterator &K2TreeMixed::BandScanIterator::operator++() {
  if (!lazy_handler.has_next) {
    finished = true;
    return *this;
  }
  k2node_report_band_next(&lazy_handler, &current);
  return *this;
}

K2TreeMixed::BandScanIterator K2TreeMixed::BandScanIterator::operator++(int) {
  K2TreeMixed::BandScanIterator tmp = *this;
  ++(*this);
  return tmp;
}

K2TreeMixed::BandScanIterator::BandScanIterator(
    K2TreeMixed &k2tree, unsigned long band, K2TreeMixed::BandType band_type) {
  if (band_type == COLUMN_BAND_TYPE) {
    k2node_report_column_lazy_init(&lazy_handler, k2tree.get_root_k2node(),
                                   k2tree.get_k2qstate(), band);
  } else {
    k2node_report_row_lazy_init(&lazy_handler, k2tree.get_root_k2node(),
                                k2tree.get_k2qstate(), band);
  }
  init_called = true;
  finished = !lazy_handler.has_next;
  if (!finished)
    k2node_report_band_next(&lazy_handler, &current);
}

K2TreeMixed::BandScanIterator::BandScanIterator(
    K2TreeMixed::BandScanIterator::pointer ptr)
    : finished(!ptr), init_called(false) {
  if (ptr)
    throw std::runtime_error("Non null ptr not allowed");
}
K2TreeMixed::BandScanIterator::BandScanIterator(
    const K2TreeMixed::BandScanIterator &original) {
  lazy_handler = deep_copy_band_lazy_handler(original.lazy_handler);
}
K2TreeMixed::BandScanIterator::~BandScanIterator() {
  k2node_report_band_lazy_clean(&lazy_handler);
}
unsigned long &K2TreeMixed::BandScanIterator::operator*() { return current; }
k2node_lazy_handler_report_band_t
K2TreeMixed::BandScanIterator::deep_copy_band_lazy_handler(
    const k2node_lazy_handler_report_band_t &original_handler) {
  k2node_lazy_handler_report_band_t result{};
  result.has_next = original_handler.has_next;
  result.next_result = original_handler.next_result;
  result.at_leaf = original_handler.at_leaf;
  result.base_col = original_handler.base_col;
  result.base_row = original_handler.base_row;
  result.st = original_handler.st;
  result.which_report = original_handler.which_report;
  result.sub_handler =
      deep_copy_sub_band_lazy_handler(original_handler.sub_handler);
  result.stack = deep_copy_band_lazy_stack(original_handler.stack);
  return result;
}
lazy_handler_report_band_t
K2TreeMixed::BandScanIterator::deep_copy_sub_band_lazy_handler(
    const lazy_handler_report_band_t &sub_handler) {
  lazy_handler_report_band_t result{};
  result.stack = deep_copy_band_sub_stack(sub_handler.stack);
  result.which_report = sub_handler.which_report;
  result.next_result = sub_handler.next_result;
  result.has_next = sub_handler.has_next;
  result.qs = sub_handler.qs;
  return result;
}
k2node_lazy_report_band_state_t_stack
K2TreeMixed::BandScanIterator::deep_copy_band_lazy_stack(
    const k2node_lazy_report_band_state_t_stack &original_stack) {
  k2node_lazy_report_band_state_t_stack result{};
  init_k2node_lazy_report_band_state_t_stack(
      &result, static_cast<int>(original_stack.capacity));
  for (size_t i = 0; i < static_cast<size_t>(original_stack.capacity); i++) {
    result.data[i] = original_stack.data[i];
  }
  return result;
}
lazy_report_band_state_t_stack
K2TreeMixed::BandScanIterator::deep_copy_band_sub_stack(
    const lazy_report_band_state_t_stack &original_stack) {
  lazy_report_band_state_t_stack result{};
  init_lazy_report_band_state_t_stack(
      &result, static_cast<int>(original_stack.capacity));
  for (size_t i = 0; i < static_cast<size_t>(original_stack.capacity); i++) {
    result.data[i] = original_stack.data[i];
  }
  return result;
}
bool operator==(const K2TreeMixed::BandScanIterator &lhs,
                const K2TreeMixed::BandScanIterator &rhs) {
  if (lhs.finished && rhs.finished)
    return true;
  return false;
}
bool operator!=(const K2TreeMixed::BandScanIterator &lhs,
                const K2TreeMixed::BandScanIterator &rhs) {
  return !(lhs == rhs);
}

K2TreeMixed::BandScanIterator
K2TreeMixed::begin_band_scan(unsigned long band_value, BandType band_type) {
  return K2TreeMixed::BandScanIterator(*this, band_value, band_type);
}
K2TreeMixed::BandScanIterator K2TreeMixed::end_band_scan() {
  return K2TreeMixed::BandScanIterator(nullptr);
}
std::unique_ptr<K2TreeMixed::K2TreeScanner>
K2TreeMixed::create_full_scanner(TimeControl &time_control) {
  return std::unique_ptr<K2TreeScanner>(
      std::make_unique<FullScanner>(*this, time_control));
}
std::unique_ptr<K2TreeMixed::K2TreeScanner>
K2TreeMixed::create_band_scanner(unsigned long band,
                                 K2TreeMixed::BandType band_type,
                                 TimeControl &time_control) {
  return std::unique_ptr<K2TreeScanner>(
      std::make_unique<BandScanner>(*this, band, band_type, time_control));
}
std::unique_ptr<K2TreeMixed::K2TreeScanner>
K2TreeMixed::create_empty_scanner() {
  return std::make_unique<EmptyScanner>(*this);
}

bool K2TreeMixed::FullScanner::has_next() { return lazy_handler.has_next; }
std::pair<unsigned long, unsigned long> K2TreeMixed::FullScanner::next() {
  pair2dl_t result;
  k2node_naive_scan_points_lazy_next(&lazy_handler, &result);
  time_control.tick_only_count();
  return {result.col, result.row};
}

K2TreeMixed::FullScanner::FullScanner(K2TreeMixed &k2tree,
                                      TimeControl &time_control)
    : k2tree(k2tree), time_control(time_control) {
  k2node_naive_scan_points_lazy_init(k2tree.get_root_k2node(),
                                     k2tree.get_k2qstate(), &lazy_handler);
}
K2TreeMixed::FullScanner::~FullScanner() {
  k2node_naive_scan_points_lazy_clean(&lazy_handler);
}
void K2TreeMixed::FullScanner::reset_scan() {
  k2node_naive_scan_points_lazy_reset(&lazy_handler);
}
bool K2TreeMixed::FullScanner::is_band() { return false; }
K2TreeMixed::BandType K2TreeMixed::FullScanner::get_band_type() {
  throw std::runtime_error("Not band");
}
K2TreeMixed &K2TreeMixed::FullScanner::get_tree() { return k2tree; }
unsigned long K2TreeMixed::FullScanner::get_band_value() {
  throw std::runtime_error("Not band");
}

bool K2TreeMixed::BandScanner::has_next() { return lazy_handler.has_next; }
std::pair<unsigned long, unsigned long> K2TreeMixed::BandScanner::next() {
  uint64_t result;
  k2node_report_band_next(&lazy_handler, &result);
  if (band_type == COLUMN_BAND_TYPE) {
    return {band, result};
  }
  time_control.tick_only_count();
  return {result, band};
}
K2TreeMixed::BandScanner::~BandScanner() {
  k2node_report_band_lazy_clean(&lazy_handler);
}

K2TreeMixed::BandScanner::BandScanner(K2TreeMixed &k2tree, unsigned long band,
                                      K2TreeMixed::BandType band_type,
                                      TimeControl &time_control)
    : band(band), band_type(band_type), k2tree(k2tree),
      time_control(time_control) {
  if (band_type == COLUMN_BAND_TYPE) {
    k2node_report_column_lazy_init(&lazy_handler, k2tree.get_root_k2node(),
                                   k2tree.get_k2qstate(), band);
  } else {
    k2node_report_row_lazy_init(&lazy_handler, k2tree.get_root_k2node(),
                                k2tree.get_k2qstate(), band);
  }
}
void K2TreeMixed::BandScanner::reset_scan() {
  k2node_report_band_reset(&lazy_handler);
}
bool K2TreeMixed::BandScanner::is_band() { return true; }
K2TreeMixed::BandType K2TreeMixed::BandScanner::get_band_type() {
  return band_type;
}
K2TreeMixed &K2TreeMixed::BandScanner::get_tree() { return k2tree; }
unsigned long K2TreeMixed::BandScanner::get_band_value() { return band; }

bool K2TreeMixed::EmptyScanner::has_next() { return false; }
std::pair<unsigned long, unsigned long> K2TreeMixed::EmptyScanner::next() {
  throw std::runtime_error("empty scanner");
}
void K2TreeMixed::EmptyScanner::reset_scan() {}
bool K2TreeMixed::EmptyScanner::is_band() { return false; }
K2TreeMixed::BandType K2TreeMixed::EmptyScanner::get_band_type() {
  throw std::runtime_error("empty scanner");
}
unsigned long K2TreeMixed::EmptyScanner::get_band_value() {
  throw std::runtime_error("empty scanner");
}
K2TreeMixed &K2TreeMixed::EmptyScanner::get_tree() { return k2tree; }
K2TreeMixed::EmptyScanner::EmptyScanner(K2TreeMixed &k2tree) : k2tree(k2tree) {}
