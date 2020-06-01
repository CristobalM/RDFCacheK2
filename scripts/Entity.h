//
// Created by cristobal on 01-06-20.
//

#ifndef RDFCACHEK2_ENTITY_H
#define RDFCACHEK2_ENTITY_H

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
#endif // RDFCACHEK2_ENTITY_H
