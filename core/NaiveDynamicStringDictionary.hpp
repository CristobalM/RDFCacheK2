
#ifndef NAIVE_DYNAMIC_STRING_DICTIONARY_HPP
#define NAIVE_DYNAMIC_STRING_DICTIONARY_HPP
#include "RDFTriple.hpp"
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct NaiveHash {
  std::size_t operator()(const RDFResource &key) const {
    return std::hash<std::string>()(key.value) ^ key.resource_type;
  }
};

class NaiveDynamicStringDictionary {

  std::vector<RDFResource> resources_extra;
  std::unordered_map<RDFResource, unsigned long, NaiveHash> reverse_resources;

public:
  NaiveDynamicStringDictionary() = default;
  NaiveDynamicStringDictionary(
      std::vector<RDFResource> &&resources_extra,
      std::unordered_map<RDFResource, unsigned long, NaiveHash>
          &&reverse_resources_extra);

  void serialize(std::ofstream &ofs);
  static NaiveDynamicStringDictionary deserialize(std::ifstream &ifs);

  static void serialize_dict(std::vector<RDFResource> &resources,
                             const std::string &fname);
  static std::vector<RDFResource> deserialize_dict(const std::string &fname);
  static std::unordered_map<RDFResource, unsigned long, NaiveHash>
  create_reverse(std::vector<RDFResource> &input);
  void save(const std::string &res_fname);
  static NaiveDynamicStringDictionary load(const std::string &res_fname);

  void add_resource(RDFResource resource);
  unsigned long locate_resource(const RDFResource &resource) const;
  RDFResource extract_resource(unsigned long id) const;

  size_t size() const;
  void merge_with_extra_dict(NaiveDynamicStringDictionary &other_dict);
};

#endif /* NAIVE_DYNAMIC_STRING_DICTIONARY_HPP */
