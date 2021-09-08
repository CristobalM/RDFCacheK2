//
// Created by cristobal on 9/7/21.
//

#include "mock_structures/FHMock.hpp"
#include <NaiveDynamicStringDictionary.hpp>
#include <gtest/gtest.h>

static bool matching_naive_sds(NaiveDynamicStringDictionary &lhs,
                               NaiveDynamicStringDictionary &rhs) {
  if (lhs.size() != rhs.size())
    return false;
  for (size_t i = 0; i < lhs.size(); i++) {
    auto resource_left = lhs.extract_resource(i + 1);
    auto resource_right = rhs.extract_resource(i + 1);
    if (resource_left.value != resource_right.value ||
        resource_left.resource_type != resource_right.resource_type)
      return false;
  }
  return true;
}

TEST(naive_string_dictionary_test, can_serialize_and_deserialize_test_1) {

  NaiveDynamicStringDictionary sd;
  NaiveDynamicStringDictionary sd2;

  std::string data;
  auto frw_handler = FHMock(data);

  const int resources_amount = 10000;

  auto resource_name_gen = [](int i) {
    return "Resource #" + std::to_string(i);
  };
  auto resource_name_gen2 = [](int i) {
    return "Resource (SD2) #" + std::to_string(i);
  };

  for (int i = 0; i < resources_amount; i++) {
    sd.add_resource(
        RDFResource(resource_name_gen(i), RDFResourceType::RDF_TYPE_LITERAL));
    sd2.add_resource(
        RDFResource(resource_name_gen2(i), RDFResourceType::RDF_TYPE_LITERAL));
  }

  {
    auto writer = frw_handler.get_writer(std::ios::binary);
    sd.serialize(writer->get_stream());
    sd2.serialize(writer->get_stream());
  }

  std::unique_ptr<NaiveDynamicStringDictionary> sd_des;
  std::unique_ptr<NaiveDynamicStringDictionary> sd2_des;
  {
    auto reader = frw_handler.get_reader(std::ios::binary);
    sd_des = std::make_unique<NaiveDynamicStringDictionary>(
        NaiveDynamicStringDictionary::deserialize(reader->get_stream()));
    sd2_des = std::make_unique<NaiveDynamicStringDictionary>(
        NaiveDynamicStringDictionary::deserialize(reader->get_stream()));
  }

  for (int i = 0; i < resources_amount; i++) {
    auto resource = RDFResource(resource_name_gen(i), RDF_TYPE_LITERAL);
    auto id = sd.locate_resource(resource);
    auto id_des = sd_des->locate_resource(resource);
    ASSERT_EQ(id, id_des);
  }

  for (int i = 0; i < resources_amount; i++) {
    auto resource = RDFResource(resource_name_gen2(i), RDF_TYPE_LITERAL);
    auto id = sd2.locate_resource(resource);
    auto id_des = sd2_des->locate_resource(resource);
    ASSERT_EQ(id, id_des);
  }

  for (int i = 0; i < resources_amount; i++) {
    int id = i + 1;
    auto resource = sd.extract_resource(id);
    auto resource_des = sd_des->extract_resource(id);
    auto gen_value = resource_name_gen(i);
    ASSERT_EQ(resource.value, resource_des.value);
    ASSERT_EQ(resource.value, gen_value);
    ASSERT_EQ(resource.resource_type, RDF_TYPE_LITERAL);
    ASSERT_EQ(resource_des.resource_type, RDF_TYPE_LITERAL);
  }

  for (int i = 0; i < resources_amount; i++) {
    int id = i + 1;
    auto resource = sd2.extract_resource(id);
    auto resource_des = sd2_des->extract_resource(id);
    auto gen_value = resource_name_gen2(i);
    ASSERT_EQ(resource.value, resource_des.value);
    ASSERT_EQ(resource.value, gen_value);
    ASSERT_EQ(resource.resource_type, RDF_TYPE_LITERAL);
    ASSERT_EQ(resource_des.resource_type, RDF_TYPE_LITERAL);
  }

  ASSERT_EQ(sd.size(), resources_amount);
  ASSERT_EQ(sd_des->size(), resources_amount);
  ASSERT_EQ(sd2.size(), resources_amount);
  ASSERT_EQ(sd2_des->size(), resources_amount);
  ASSERT_TRUE(matching_naive_sds(sd, *sd_des));
  ASSERT_TRUE(matching_naive_sds(sd2, *sd2_des));
}
