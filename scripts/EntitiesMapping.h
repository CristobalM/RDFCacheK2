//
// Created by cristobal on 30-05-20.
//

#ifndef RDFCACHEK2_ENTITIESMAPPING_H
#define RDFCACHEK2_ENTITIESMAPPING_H

#include <entities_mapping.pb.h>
#include <string>
#include <map>

struct Entity{

  enum EntityType {
    UNKNOWN_ENTITY = 0,
    URI_ENTITY = 1,
    LITERAL_ENTITY = 2,
    BLANK_ENTITY = 3
  };

  EntityType entity_type;
  unsigned long value;
};

class EntitiesMapping {
  std::map<std::string, Entity> subjects_mapping;
  std::map<std::string, Entity> predicates_mapping;
  std::map<std::string, Entity> objects_mapping;

  unsigned long subjects_count;
  unsigned long predicates_count;
  unsigned long objects_count;

public:

  EntitiesMapping();

  EntitiesMapping(proto_msg::EntitiesMapping &input_proto);

  enum Kind{
    SUBJECT = 0,
    PREDICATE = 1,
    OBJECT = 2
  };

  std::unique_ptr<proto_msg::EntitiesMapping> serialize();

  unsigned long add_subject(const std::string &value, Entity::EntityType entity_type);
  unsigned long add_predicate(const std::string &value, Entity::EntityType entity_type);
  unsigned long add_object(const std::string &value, Entity::EntityType entity_type);

  bool has_subject(const std::string &value);
  bool has_predicate(const std::string &value);
  bool has_object(const std::string &value);
};

#endif // RDFCACHEK2_ENTITIESMAPPING_H
