//
// Created by cristobal on 30-05-20.
//

#include <exception>
#include <functional>

#include "EntitiesMapping.h"

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

EntitiesMapping::EntitiesMapping(proto_msg::EntitiesMapping &input_proto) {


}

std::unique_ptr<proto_msg::EntitiesMapping> EntitiesMapping::serialize() {
  auto out = std::make_unique<proto_msg::EntitiesMapping>();



  return out;
}

unsigned long EntitiesMapping::add_subject(const std::string &value,
                                           Entity::EntityType entity_type) {

  auto lookup_result = entities_mapping.lookup(value);

  if (lookup_result.was_found()) {
    if(!lookup_result.result().is_subject()){
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
    if(!lookup_result.result().is_predicate()){
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
    if(!lookup_result.result().is_object()){
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
