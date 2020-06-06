//
// Created by Cristobal Miranda, 2020
//

#include <EntitiesMapping.h>
#include <gtest/gtest.h>

TEST(oss, t1) {
  std::ostringstream oss;
  std::string hola = "hola hola AEKJEOEIO";
  oss.write(hola.data(), hola.size());
  auto r = oss.str();
  std::istringstream iss(r);
  std::vector<char> r_data(hola.size(), 0);
  iss.read(r_data.data(), hola.size());
  std::string wr(r_data.begin(), r_data.end());
  std::cout << wr << std::endl;
}

TEST(index_file_tests_ones, test_name_one_file_test) {
  EntitiesMapping entities_mapping;

  entities_mapping.add_object("Hola", Entity::EntityType::LITERAL_ENTITY);

  entities_mapping.add_object("Holanda", Entity::EntityType::LITERAL_ENTITY);
  entities_mapping.add_object("Chao", Entity::EntityType::LITERAL_ENTITY);

  entities_mapping.add_subject("Hola", Entity::EntityType::LITERAL_ENTITY);

  std::cout << "Added stuff" << std::endl;

  std::ostringstream output_stream;
  entities_mapping.serialize(output_stream);
  std::cout << "Serialized" << std::endl;

  entities_mapping._debug_print_radix_tree();

  std::cout << "going to deserialize" << std::endl;

  std::istringstream iss(output_stream.str());

  EntitiesMapping deserialized_entities_mapping(iss);
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

  std::cout << "Entities Mapping serialized: '" << output_stream.str() << "'"
            << std::endl;

  entities_mapping._debug_print_radix_tree();
}

TEST(index_file_tests_ones, deserialize_twice) {
  EntitiesMapping entities_mapping;

  entities_mapping.add_object("Hola", Entity::EntityType::LITERAL_ENTITY);
  entities_mapping.add_object("Chao", Entity::EntityType::LITERAL_ENTITY);

  std::ostringstream oss;

  entities_mapping.serialize(oss);

  std::istringstream iss(oss.str());

  EntitiesMapping deserialized(iss);

  std::ostringstream oss2;

  deserialized.serialize(oss2);

  std::istringstream iss2(oss2.str());

  EntitiesMapping deserialized_2(iss2);

  deserialized_2._debug_print_radix_tree();

  std::ostringstream oss3;

  deserialized_2.serialize(oss3);

  ASSERT_TRUE(deserialized_2.has_object("Hola"))
      << "Doesn't have Hola (object)";
  ASSERT_TRUE(deserialized_2.has_object("Chao"))
      << "Doesn't have Chao (object)";
}
