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

  std::cout << "Added stuff" << std::endl;

  auto serialized = entities_mapping.serialize();
  std::cout << "Serialized" << std::endl;

  std::cout << serialized->DebugString() << std::endl;

  auto string_serialized = serialized->SerializeAsString();
  std::cout << "Serialized as str" << std::endl;

  entities_mapping._debug_print_radix_tree();

  proto_msg::EntitiesMapping to_deserialize;

  to_deserialize.ParseFromString(string_serialized);
  std::cout << "going to deserialize" << std::endl;

  std::cout << to_deserialize.DebugString() << std::endl;
  EntitiesMapping deserialized_entities_mapping(to_deserialize);
  std::cout << "deserialized" << std::endl;

  deserialized_entities_mapping._debug_print_radix_tree();

  std::cout << "DEBUG" << std::endl;

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

TEST(index_file_tests_ones, deserialize_twice) {
  EntitiesMapping entities_mapping;

  entities_mapping.add_object("Hola", Entity::EntityType::LITERAL_ENTITY);
  entities_mapping.add_object("Chao", Entity::EntityType::LITERAL_ENTITY);

  auto serialized = entities_mapping.serialize();

  auto string_serialized = serialized->SerializeAsString();

  proto_msg::EntitiesMapping to_deserialize;
  to_deserialize.ParseFromString(string_serialized);

  EntitiesMapping deserialized(to_deserialize);

  auto serialized_2 = deserialized.serialize();
  auto string_serialized_2 = serialized_2->SerializeAsString();

  proto_msg::EntitiesMapping to_deserialize_2;
  to_deserialize_2.ParseFromString(string_serialized_2);

  EntitiesMapping deserialized_2(to_deserialize_2);

  deserialized_2._debug_print_radix_tree();

  auto serialized_3 = deserialized_2.serialize();

  serialized->PrintDebugString();
  std::cout << "--\n\n" << std::endl;
  serialized_2->PrintDebugString();

  ASSERT_TRUE(deserialized_2.has_object("Hola"))
      << "Doesn't have Hola (object)";
  ASSERT_TRUE(deserialized_2.has_object("Chao"))
      << "Doesn't have Chao (object)";
}
