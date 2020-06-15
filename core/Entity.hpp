//
// Created by cristobal on 01-06-20.
//

#ifndef RDFCACHEK2_ENTITY_HPP
#define RDFCACHEK2_ENTITY_HPP

constexpr uint8_t _SUBJECT_MASK = 1u;
constexpr uint8_t _PREDICATE_MASK = 1u << 1u;
constexpr uint8_t _OBJECT_MASK = 1u << 2u;

struct Entity {

  enum EntityType {
    UNKNOWN_ENTITY = 0,
    URI_ENTITY = 1,
    LITERAL_ENTITY = 2,
    BLANK_ENTITY = 3
  };

  static std::unique_ptr<Entity>
  create_from_bytes_string(const std::string &bytes_string) {
    auto result = std::make_unique<Entity>();
    auto both_type_kind = static_cast<uint8_t>(bytes_string[0]);

    result->entity_kinds = both_type_kind & 7u;
    result->entity_type = static_cast<Entity::EntityType>(both_type_kind >> 3u);
    result->subject_value = *reinterpret_cast<const uint64_t *>(
        bytes_string.data() + sizeof(uint8_t));
    result->predicate_value = *reinterpret_cast<const uint64_t *>(
        bytes_string.data() + sizeof(uint8_t) + sizeof(uint64_t));
    result->object_value = *reinterpret_cast<const uint64_t *>(
        bytes_string.data() + sizeof(uint8_t) + 2 * sizeof(uint64_t));
    return result;
  }

  std::string to_bytes_string() {
    std::string result(25, 0);
    uint8_t both_type_kind = 0;
    both_type_kind = static_cast<uint8_t>(entity_type) << 3u;
    both_type_kind |= entity_kinds;
    result[0] = both_type_kind;
    memcpy(result.data() + sizeof(uint8_t),
           reinterpret_cast<char *>(&subject_value), sizeof(uint64_t));
    memcpy(result.data() + sizeof(uint8_t) + sizeof(uint64_t),
           reinterpret_cast<char *>(&predicate_value), sizeof(uint64_t));
    memcpy(result.data() + sizeof(uint8_t) + 2 * sizeof(uint64_t),
           reinterpret_cast<char *>(&object_value), sizeof(uint64_t));
    return result;
  }

  bool is_subject() { return entity_kinds & _SUBJECT_MASK; }

  bool is_predicate() { return entity_kinds & _PREDICATE_MASK; }

  bool is_object() { return entity_kinds & _OBJECT_MASK; }

  void mark_as_subject() { entity_kinds |= _SUBJECT_MASK; }

  void mark_as_predicate() { entity_kinds |= _PREDICATE_MASK; }

  void mark_as_object() { entity_kinds |= _OBJECT_MASK; }

  EntityType entity_type : 2;
  uint8_t entity_kinds : 3;

  unsigned long subject_value;
  unsigned long predicate_value;
  unsigned long object_value;
};
#endif // RDFCACHEK2_ENTITY_HPP
