//
// Created by Cristobal Miranda, 2020
//

#include <EntitiesMapping.h>
#include <gtest/gtest.h>

TEST(index_file_tests_ones, test_name_one_file_test) {
  EntitiesMapping entities_mapping;

  entities_mapping.add_object("Hola", Entity::EntityType::LITERAL_ENTITY);

  entities_mapping.add_object("Holanda", Entity::EntityType::LITERAL_ENTITY);
  entities_mapping.add_object("Chao", Entity::EntityType::LITERAL_ENTITY);

  entities_mapping.add_subject("Hola", Entity::EntityType::LITERAL_ENTITY);

  auto serialized = entities_mapping.serialize();

  auto string_serialized = serialized->SerializeAsString();


  proto_msg::EntitiesMapping to_deserialize;
  to_deserialize.ParseFromString(string_serialized);

  EntitiesMapping deserialized_entities_mapping(to_deserialize);

  ASSERT_TRUE(deserialized_entities_mapping.has_object("Hola"))
      << "Doesn't have Hola (object)";
  ASSERT_TRUE(deserialized_entities_mapping.has_object("Holanda"))
      << "Doesn't have Holanda (object)";
  ASSERT_TRUE(deserialized_entities_mapping.has_object("Chao"))
      << "Doesn't have Chao (object)";
  ASSERT_TRUE(deserialized_entities_mapping.has_subject("Hola"))
      << "Doesn't have Holanda (subject)";

  std::cout << "Entities Mapping serialized: '" << string_serialized << "'"
            << std::endl;

  entities_mapping._debug_print_radix_tree();

}