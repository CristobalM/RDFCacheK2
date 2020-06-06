//
// Created by cristobal on 30-05-20.
//

#ifndef RDFCACHEK2_ENTITIESMAPPING_H
#define RDFCACHEK2_ENTITIESMAPPING_H

#include <entities_mapping.pb.h>
#include <string>

#include <radix_tree.pb.h>

extern "C" {
#include <rax.h>
}

#include "Entity.h"
#include "RadixTree.hpp"

class EntitiesMapping {
  RadixTree<Entity> entities_mapping;

  unsigned long subjects_count;
  unsigned long predicates_count;
  unsigned long objects_count;

public:

  struct DeserializedResult {
    raxNode * deserialized_node;
    uint32_t child_id;
  };

  EntitiesMapping();

  EntitiesMapping(const EntitiesMapping &) = delete;

  explicit EntitiesMapping(EntitiesMapping &&);

  explicit EntitiesMapping(std::istream &input_stream);

  void serialize(std::ostream &output_stream);

  unsigned long add_subject(const std::string &value,
                            Entity::EntityType entity_type);
  unsigned long add_predicate(const std::string &value,
                              Entity::EntityType entity_type);
  unsigned long add_object(const std::string &value,
                           Entity::EntityType entity_type);

  bool has_subject(const std::string &value);
  bool has_predicate(const std::string &value);
  bool has_object(const std::string &value);

  void _debug_print_radix_tree();

  static std::shared_ptr<EntitiesMapping>
  load_from_file(const std::string &previous_mapping_fpath);

private:


  void deserialize(std::istream &input_stream);

  DeserializedResult deserialize_node(const proto_msg::RadixNode &proto_node,
      std::map<uint32_t, raxNode * > &deserialized);

  uint32_t serialize_node(
      proto_msg::RadixNode *proto_node, raxNode *rax_node,
      std::map<uint32_t, std::unique_ptr<proto_msg::RadixNode>> &to_serialize,
      uint32_t &node_counter, std::ostream &output_stream, uint32_t &max_size);

  
};

#endif // RDFCACHEK2_ENTITIESMAPPING_H
