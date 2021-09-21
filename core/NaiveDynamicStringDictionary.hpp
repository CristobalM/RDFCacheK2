
#ifndef NAIVE_DYNAMIC_STRING_DICTIONARY_HPP
#define NAIVE_DYNAMIC_STRING_DICTIONARY_HPP
#include "RDFTriple.hpp"
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "NodeId.hpp"

struct NaiveHash {
  std::size_t operator()(const NodeId &key) const {
    return std::hash<unsigned long>()(key.get_raw());
  }
};

class NaiveDynamicStringDictionary {

  std::vector<NodeId> node_ids;
  std::unordered_map<NodeId, unsigned long, NaiveHash> reverse_node_ids;

public:
  NaiveDynamicStringDictionary() = default;
  NaiveDynamicStringDictionary(
      std::vector<NodeId> &&node_ids_extra,
      std::unordered_map<NodeId, unsigned long, NaiveHash> &&reverse_node_ids);

  void serialize(std::ostream &ofs);
  static NaiveDynamicStringDictionary deserialize(std::istream &ifs);

  static void serialize_dict(std::vector<NodeId> &node_ids,
                             const std::string &fname);
  static std::vector<NodeId> deserialize_dict(const std::string &fname);
  static std::unordered_map<NodeId, unsigned long, NaiveHash>
  create_reverse(std::vector<NodeId> &input_nodes_ids);
  void save(const std::string &res_fname);
  static NaiveDynamicStringDictionary load(const std::string &res_fname);

  void add_node_id(NodeId node_id);
  unsigned long locate_node_id(const NodeId &node_id) const;
  NodeId extract_node_id(unsigned long id) const;

  size_t size() const;
  void merge_with_extra_dict(NaiveDynamicStringDictionary &other_dict);
};

#endif /* NAIVE_DYNAMIC_STRING_DICTIONARY_HPP */
