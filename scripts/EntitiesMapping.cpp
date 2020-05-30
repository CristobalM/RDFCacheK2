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

  for (int i = 0; i < input_proto.subjects_mapping_size(); i++) {
    auto &current = input_proto.subjects_mapping(i);
    auto proto_type = current.entity().entity_type();
    auto entity_type = get_entity_type_from_proto(proto_type);
    subjects_mapping[current.entity().value()] = {entity_type,
                                                  current.mapped_value()};
  }

  for (int i = 0; i < input_proto.predicates_mapping_size(); i++) {
    auto &current = input_proto.predicates_mapping(i);
    auto proto_type = current.entity().entity_type();
    auto entity_type = get_entity_type_from_proto(proto_type);
    predicates_mapping[current.entity().value()] = {entity_type,
                                                    current.mapped_value()};
  }

  for (int i = 0; i < input_proto.objects_mapping_size(); i++) {
    auto &current = input_proto.objects_mapping(i);
    auto proto_type = current.entity().entity_type();
    auto entity_type = get_entity_type_from_proto(proto_type);
    objects_mapping[current.entity().value()] = {entity_type,
                                                 current.mapped_value()};
  }

  subjects_count = input_proto.subjects_mapping_size();
  predicates_count = input_proto.predicates_mapping_size();
  objects_count = input_proto.objects_mapping_size();
}

std::unique_ptr<proto_msg::EntitiesMapping> EntitiesMapping::serialize() {
  auto out = std::make_unique<proto_msg::EntitiesMapping>();
  for (auto &sitem : subjects_mapping) {
    auto *current = out->add_subjects_mapping();
    current->mutable_entity()->set_value(sitem.first);
    current->mutable_entity()->set_entity_type(
        get_proto_type_from_etype(sitem.second.entity_type));
    current->set_mapped_value(sitem.second.value);
  }

  for (auto &sitem : predicates_mapping) {
    auto *current = out->add_predicates_mapping();
    current->mutable_entity()->set_value(sitem.first);
    current->mutable_entity()->set_entity_type(
        get_proto_type_from_etype(sitem.second.entity_type));
    current->set_mapped_value(sitem.second.value);
  }

  for (auto &sitem : objects_mapping) {
    auto *current = out->add_objects_mapping();
    current->mutable_entity()->set_value(sitem.first);
    current->mutable_entity()->set_entity_type(
        get_proto_type_from_etype(sitem.second.entity_type));
    current->set_mapped_value(sitem.second.value);
  }

  return out;
}

unsigned long EntitiesMapping::add_subject(const std::string &value,
                                           Entity::EntityType entity_type) {
  if (has_subject(value)) {
    return subjects_mapping[value].value;
  }
  auto mapped_value = subjects_count++;
  subjects_mapping[value] = {entity_type, mapped_value};
  return mapped_value;
}
unsigned long EntitiesMapping::add_predicate(const std::string &value,
                                             Entity::EntityType entity_type) {
  if (has_predicate(value)) {
    return predicates_mapping[value].value;
  }
  auto mapped_value = predicates_count++;
  predicates_mapping[value] = {entity_type, mapped_value};
  return mapped_value;
}

unsigned long EntitiesMapping::add_object(const std::string &value,
                                          Entity::EntityType entity_type) {
  if (has_object(value)) {
    return objects_mapping[value].value;
  }
  auto mapped_value = objects_count++;
  objects_mapping[value] = {entity_type, mapped_value};
  return mapped_value;
}
bool EntitiesMapping::has_subject(const std::string &value) {
  return subjects_mapping.find(value) != subjects_mapping.end();
}
bool EntitiesMapping::has_predicate(const std::string &value) {
  return predicates_mapping.find(value) != predicates_mapping.end();
}
bool EntitiesMapping::has_object(const std::string &value) {
  return objects_mapping.find(value) != objects_mapping.end();
}
EntitiesMapping::EntitiesMapping()
    : subjects_count(0), predicates_count(0), objects_count(0) {}
