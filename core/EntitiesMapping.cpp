//
// Created by cristobal on 30-05-20.
//

#include <exception>
#include <fstream>
#include <netinet/in.h>
#include <string>

#include <stdlib.h>

#include "EntitiesMapping.hpp"
#include "serialization_util.hpp"

EntitiesMapping::EntitiesMapping(std::istream &input_stream) {
  deserialize(input_stream);
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

struct HeaderHolderEM {
  uint32_t subjects_count;
  uint32_t predicates_count;
  uint32_t objects_count;
};

void write_header(HeaderHolderEM header_holder, std::ostream &output_stream) {
  write_u32(output_stream, header_holder.subjects_count);
  write_u32(output_stream, header_holder.predicates_count);
  write_u32(output_stream, header_holder.objects_count);
}

HeaderHolderEM read_header(std::istream &input_stream) {
  HeaderHolderEM output{};
  output.subjects_count = read_u32(input_stream);
  output.predicates_count = read_u32(input_stream);
  output.objects_count = read_u32(input_stream);
  return output;
}

void EntitiesMapping::serialize(std::ostream &output_stream) {
  HeaderHolderEM header{};
  header.subjects_count = subjects_count;
  header.predicates_count = predicates_count;
  header.objects_count = objects_count;

  write_header(header, output_stream);
  entities_mapping.serialize(output_stream);
}

void EntitiesMapping::deserialize(std::istream &input_stream) {
  auto header = read_header(input_stream);
  subjects_count = header.subjects_count;
  predicates_count = header.predicates_count;
  objects_count = header.predicates_count;
  entities_mapping.deserialize(input_stream);
}

EntitiesMapping::EntitiesMapping(EntitiesMapping &&rhs)
    : entities_mapping(std::move(rhs.entities_mapping)),
      subjects_count(rhs.subjects_count),
      predicates_count(rhs.predicates_count), objects_count(rhs.objects_count) {
}

void EntitiesMapping::_debug_print_radix_tree() {
  entities_mapping._debug_print_radix_tree();
}

std::shared_ptr<EntitiesMapping>
EntitiesMapping::load_from_file(const std::string &previous_mapping_fpath) {
  std::ifstream ifs(previous_mapping_fpath, std::ios::binary);
  return std::make_shared<EntitiesMapping>(ifs);
}
