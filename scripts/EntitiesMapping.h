//
// Created by cristobal on 30-05-20.
//

#ifndef RDFCACHEK2_ENTITIESMAPPING_H
#define RDFCACHEK2_ENTITIESMAPPING_H

#include <entities_mapping.pb.h>
#include <string>

#include "RadixTree.hpp"


constexpr uint8_t _SUBJECT_MASK = 1u;
constexpr uint8_t _PREDICATE_MASK = 1u << 1u;
constexpr uint8_t _OBJECT_MASK = 1u << 2u;


struct Entity{

  enum EntityType {
    UNKNOWN_ENTITY = 0,
    URI_ENTITY = 1,
    LITERAL_ENTITY = 2,
    BLANK_ENTITY = 3
  };


  bool is_subject(){
    return entity_kinds & _SUBJECT_MASK;
  }

  bool is_predicate(){
    return entity_kinds & _PREDICATE_MASK;
  }

  bool is_object(){
    return entity_kinds & _OBJECT_MASK;
  }

  void mark_as_subject(){
    entity_kinds |= _SUBJECT_MASK;
  }

  void mark_as_predicate(){
    entity_kinds |= _PREDICATE_MASK;
  }

  void mark_as_object(){
    entity_kinds |= _OBJECT_MASK;
  }


  EntityType entity_type:2;
  uint8_t entity_kinds:3;

  unsigned long subject_value;
  unsigned long predicate_value;
  unsigned long object_value;
};

class EntitiesMapping {
  //std::map<std::string, Entity> subjects_mapping;
  //std::map<std::string, Entity> predicates_mapping;
  //std::map<std::string, Entity> objects_mapping;
  RadixTree<Entity> entities_mapping;



  unsigned long subjects_count;
  unsigned long predicates_count;
  unsigned long objects_count;

public:

  EntitiesMapping();

  EntitiesMapping(proto_msg::EntitiesMapping &input_proto);



  std::unique_ptr<proto_msg::EntitiesMapping> serialize();

  unsigned long add_subject(const std::string &value, Entity::EntityType entity_type);
  unsigned long add_predicate(const std::string &value, Entity::EntityType entity_type);
  unsigned long add_object(const std::string &value, Entity::EntityType entity_type);

  bool has_subject(const std::string &value);
  bool has_predicate(const std::string &value);
  bool has_object(const std::string &value);
};

#endif // RDFCACHEK2_ENTITIESMAPPING_H
