//
// Created by cristobal on 30-05-20.
//

#ifndef RDFCACHEK2_ENTITIESMAPPING_H
#define RDFCACHEK2_ENTITIESMAPPING_H

#include <entities_mapping.pb.h>
#include <string>

#include <radix_tree.pb.h>
#include <rax.h>

#include "Entity.h"
#include "RadixTree.hpp"

class EntitiesMapping {
  RadixTree<Entity> entities_mapping;

  unsigned long subjects_count;
  unsigned long predicates_count;
  unsigned long objects_count;

public:
  EntitiesMapping();

  EntitiesMapping(proto_msg::EntitiesMapping &input_proto);

  std::unique_ptr<proto_msg::EntitiesMapping> serialize();

  unsigned long add_subject(const std::string &value,
                            Entity::EntityType entity_type);
  unsigned long add_predicate(const std::string &value,
                              Entity::EntityType entity_type);
  unsigned long add_object(const std::string &value,
                           Entity::EntityType entity_type);

  bool has_subject(const std::string &value);
  bool has_predicate(const std::string &value);
  bool has_object(const std::string &value);

  void deserialize_tree(proto_msg::RadixTree &proto_radix_tree);
  proto_msg::RadixTree serialize_tree();

private:
  void deserialize_node(rax *rax_tree, raxNode *parent_node,
                        const proto_msg::RadixNode &proto_node);
  void serialize_node(proto_msg::RadixNode *proto_node, raxNode *rax_node);
};

#endif // RDFCACHEK2_ENTITIESMAPPING_H
