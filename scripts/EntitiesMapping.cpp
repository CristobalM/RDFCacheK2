//
// Created by cristobal on 30-05-20.
//

#include <exception>

#include <rax.h>

#include "EntitiesMapping.h"

/* Exposes functions in rax.c */
raxNode *raxNewNode(size_t children, int datafield);
raxNode *raxCompressNode(raxNode *n, unsigned char *s, size_t len,
                         raxNode **child);
class InvalidEntityType : public std::runtime_error {
public:
  InvalidEntityType(int given_entity)
      : std::runtime_error("Invalid Entity Type " +
                           std::to_string(given_entity)){};
};

Entity::EntityType
get_entity_type_from_proto(proto_msg::EntityType proto_entity_type) {
  Entity::EntityType entity_type;
  switch (proto_entity_type) {
  case proto_msg::EntityType::UNKNOWN_ENTITY:
    entity_type = Entity::EntityType::UNKNOWN_ENTITY;
    break;
  case proto_msg::EntityType::BLANK_ENTITY:
    entity_type = Entity::EntityType::BLANK_ENTITY;
    break;
  case proto_msg::EntityType ::LITERAL_ENTITY:
    entity_type = Entity::EntityType ::LITERAL_ENTITY;
    break;
  case proto_msg::EntityType ::URI_ENTITY:
    entity_type = Entity::EntityType ::URI_ENTITY;
    break;
  default:
    throw InvalidEntityType(proto_entity_type);
  }

  return entity_type;
}

proto_msg::EntityType
get_proto_type_from_etype(Entity::EntityType entity_type) {
  proto_msg::EntityType proto_entity_type;
  switch (entity_type) {
  case Entity::EntityType::UNKNOWN_ENTITY:
    proto_entity_type = proto_msg::EntityType::UNKNOWN_ENTITY;
    break;
  case Entity::EntityType::BLANK_ENTITY:
    proto_entity_type = proto_msg::EntityType::BLANK_ENTITY;
    break;
  case Entity::EntityType ::LITERAL_ENTITY:
    proto_entity_type = proto_msg::EntityType ::LITERAL_ENTITY;
    break;
  case Entity::EntityType ::URI_ENTITY:
    proto_entity_type = proto_msg::EntityType ::URI_ENTITY;
    break;
  default:
    throw InvalidEntityType(entity_type);
  }

  return proto_entity_type;
}

EntitiesMapping::EntitiesMapping(proto_msg::EntitiesMapping &input_proto) {}

std::unique_ptr<proto_msg::EntitiesMapping> EntitiesMapping::serialize() {
  auto out = std::make_unique<proto_msg::EntitiesMapping>();

  return out;
}

unsigned long EntitiesMapping::add_subject(const std::string &value,
                                           Entity::EntityType entity_type) {

  auto lookup_result = entities_mapping.lookup(value);

  if (lookup_result.was_found()) {
    if (!lookup_result.result().is_subject()) {
      lookup_result.result().mark_as_subject();
      lookup_result.result().subject_value = subjects_count++;
    }
    return lookup_result.result().subject_value;
  }
  auto mapped_value = subjects_count++;
  Entity saving_entity{};
  saving_entity.subject_value = mapped_value;
  saving_entity.entity_type = entity_type;
  saving_entity.mark_as_subject();
  entities_mapping.insert(value, saving_entity);
  return mapped_value;
}

unsigned long EntitiesMapping::add_predicate(const std::string &value,
                                             Entity::EntityType entity_type) {
  auto lookup_result = entities_mapping.lookup(value);

  if (lookup_result.was_found()) {
    if (!lookup_result.result().is_predicate()) {
      lookup_result.result().mark_as_predicate();
      lookup_result.result().predicate_value = predicates_count++;
    }
    return lookup_result.result().predicate_value;
  }
  auto mapped_value = predicates_count++;
  Entity saving_entity{};
  saving_entity.predicate_value = mapped_value;
  saving_entity.entity_type = entity_type;
  saving_entity.mark_as_predicate();
  entities_mapping.insert(value, saving_entity);
  return mapped_value;
}

unsigned long EntitiesMapping::add_object(const std::string &value,
                                          Entity::EntityType entity_type) {
  auto lookup_result = entities_mapping.lookup(value);

  if (lookup_result.was_found()) {
    if (!lookup_result.result().is_object()) {
      lookup_result.result().mark_as_object();
      lookup_result.result().object_value = objects_count++;
    }
    return lookup_result.result().object_value;
  }
  auto mapped_value = objects_count++;
  Entity saving_entity{};
  saving_entity.object_value = mapped_value;
  saving_entity.entity_type = entity_type;
  saving_entity.mark_as_object();
  entities_mapping.insert(value, saving_entity);
  return mapped_value;
}

bool EntitiesMapping::has_subject(const std::string &value) {
  auto lookup_result = entities_mapping.lookup(value);
  return lookup_result.was_found() && lookup_result.result().is_subject();
}

bool EntitiesMapping::has_predicate(const std::string &value) {
  auto lookup_result = entities_mapping.lookup(value);
  return lookup_result.was_found() && lookup_result.result().is_predicate();
}

bool EntitiesMapping::has_object(const std::string &value) {
  auto lookup_result = entities_mapping.lookup(value);
  return lookup_result.was_found() && lookup_result.result().is_object();
}

EntitiesMapping::EntitiesMapping()
    : subjects_count(0), predicates_count(0), objects_count(0) {}
proto_msg::RadixTree EntitiesMapping::serialize_tree() {
  proto_msg::RadixTree out;

  rax *inner_rt = entities_mapping.get_inner_rt();

  out.set_numele(inner_rt->numele);
  out.set_num_nodes(inner_rt->numnodes);
  serialize_node(out.mutable_root(), inner_rt->head);

  return out;
}

void EntitiesMapping::serialize_node(proto_msg::RadixNode *proto_node,
                                     raxNode *rax_node) {

  proto_node->set_is_key(rax_node->iskey);
  proto_node->set_is_null(rax_node->isnull);
  proto_node->set_is_compr(rax_node->iscompr);
  proto_node->set_size(rax_node->size);

  if (rax_node->iskey && !rax_node->isnull) {
    size_t skipping_ptrs =
        sizeof(void *) * (rax_node->iscompr ? 1 : rax_node->size);
    auto *entity = reinterpret_cast<Entity *>(rax_node->data + rax_node->size +
                                              skipping_ptrs);
    uint8_t both_type_kind = 0;
    both_type_kind = static_cast<uint8_t>(entity->entity_type) << 3u;
    both_type_kind |= entity->entity_kinds;
    proto_node->mutable_entity()->set_entity_type_kind(&both_type_kind,
                                                       sizeof(both_type_kind));
  }

  if (rax_node->iscompr) {
    proto_node->mutable_compr_node()->set_compressed_data(rax_node->data,
                                                          rax_node->size);
    serialize_node(
        proto_node->mutable_compr_node()->mutable_child(),
        reinterpret_cast<raxNode *>(rax_node->data + rax_node->size));

  } else {
    proto_node->mutable_normal_node()->set_children_chars(rax_node->data,
                                                          rax_node->size);
    for (int i = 0; i < rax_node->size; i++) {
      serialize_node(proto_node->mutable_normal_node()->mutable_children(i),
                     reinterpret_cast<raxNode *>(
                         rax_node->data + rax_node->size + sizeof(void *) * i));
    }
  }
}
void EntitiesMapping::deserialize_tree(proto_msg::RadixTree &proto_radix_tree) {
  rax *inner_rt = entities_mapping.get_inner_rt();


  deserialize_node(inner_rt, nullptr, proto_radix_tree.root());
}
void EntitiesMapping::deserialize_node(rax *rax_tree, raxNode *parent_node,
                                       const proto_msg::RadixNode &proto_node) {
  
}
