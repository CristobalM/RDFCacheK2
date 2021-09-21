
#include <cassert>
#include <sstream>

#include "NaiveDynamicStringDictionary.hpp"
#include "serialization_util.hpp"

NaiveDynamicStringDictionary::NaiveDynamicStringDictionary(
    std::vector<NodeId> &&node_ids_extra,
    std::unordered_map<NodeId, unsigned long, NaiveHash> &&reverse_node_ids)
    : node_ids(std::move(node_ids_extra)),
      reverse_node_ids(std::move(reverse_node_ids)) {}

void NaiveDynamicStringDictionary::serialize_dict(std::vector<NodeId> &node_ids,
                                                  const std::string &fname) {
  std::ofstream ofs(fname, std::ios::out | std::ofstream::binary);
  uint64_t size = 0;
  size += sizeof(unsigned long) * node_ids.size();

  write_u64(ofs, size);

  for (auto &node_id : node_ids) {
    ofs << node_id;
  }
}

std::vector<NodeId>
NaiveDynamicStringDictionary::deserialize_dict(const std::string &fname) {
  std::ifstream ifs(fname, std::ios::in | std::ifstream::binary);
  uint64_t size = read_u64(ifs);
  std::vector<NodeId> out;
  for (uint64_t i = 0; i < size; i++) {
    NodeId node_id{};
    ifs >> node_id;
  }
  return out;
}

std::unordered_map<NodeId, unsigned long, NaiveHash>
NaiveDynamicStringDictionary::create_reverse(
    std::vector<NodeId> &input_nodes_ids) {
  std::unordered_map<NodeId, unsigned long, NaiveHash> out;
  unsigned long counter = 0;
  for (auto &node_id : input_nodes_ids) {
    out[node_id] = counter++;
  }
  return out;
}

void NaiveDynamicStringDictionary::save(const std::string &res_fname) {
  serialize_dict(node_ids, res_fname);
}

NaiveDynamicStringDictionary
NaiveDynamicStringDictionary::load(const std::string &res_fname) {
  auto res = deserialize_dict(res_fname);
  auto reverse_res = create_reverse(res);
  return {std::move(res), std::move(reverse_res)};
}

void NaiveDynamicStringDictionary::add_node_id(NodeId node_id) {
  reverse_node_ids[node_id] = node_ids.size();
  node_ids.push_back(node_id);
}

unsigned long
NaiveDynamicStringDictionary::locate_node_id(const NodeId &node_id) const {
  if (reverse_node_ids.find(node_id) == reverse_node_ids.end())
    return 0;
  return reverse_node_ids.at(node_id) + 1;
}

NodeId NaiveDynamicStringDictionary::extract_node_id(unsigned long id) const {
  long next_id = static_cast<long>(id) - 1;
  assert(id > 0 && next_id >= 0 && next_id < (long)node_ids.size());
  return node_ids[next_id];
}

size_t NaiveDynamicStringDictionary::size() const { return node_ids.size(); }
void NaiveDynamicStringDictionary::merge_with_extra_dict(
    NaiveDynamicStringDictionary &other_dict) {
  if (node_ids.empty()) {
    *this = other_dict;
    return;
  }
  node_ids.reserve(node_ids.size() + other_dict.node_ids.size());
  for (auto &node_id : other_dict.node_ids) {
    add_node_id(node_id);
  }
}
void NaiveDynamicStringDictionary::serialize(std::ostream &ofs) {
  uint64_t size = sizeof(unsigned long) * node_ids.size();
  write_u64(ofs, size);

  for (auto &node_id : node_ids) {
    ofs << node_id;
  }
}

NaiveDynamicStringDictionary
NaiveDynamicStringDictionary::deserialize(std::istream &ifs) {
  uint64_t size = read_u64(ifs);
  std::vector<NodeId> out;
  for (uint64_t i = 0; i < size; i++) {
    NodeId node_id{};
    ifs >> node_id;
  }
  auto reverse = create_reverse(out);
  return {std::move(out), std::move(reverse)};
}
