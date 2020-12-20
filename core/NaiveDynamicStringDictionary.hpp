
#ifndef NAIVE_DYNAMIC_STRING_DICTIONARY_HPP
#define NAIVE_DYNAMIC_STRING_DICTIONARY_HPP
#include "RDFTriple.hpp"
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class NaiveDynamicStringDictionary {

  std::vector<std::string> resources_extra;
  std::unordered_map<std::string, unsigned long> reverse_resources;

public:
  NaiveDynamicStringDictionary() = default;
  NaiveDynamicStringDictionary(
      std::vector<std::string> &&resources_extra,
      std::unordered_map<std::string, unsigned long> &&reverse_resources_extra);

  static void serialize_dict(std::vector<std::string> &strings,
                             const std::string &fname);
  static std::vector<std::string> deserialize_dict(const std::string &fname);
  static std::unordered_map<std::string, unsigned long>
  create_reverse(std::vector<std::string> &input);
  void save(const std::string &res_fname);
  static NaiveDynamicStringDictionary load(const std::string &res_fname);

  void add_resource(std::string resource);
  unsigned long locate_resource(const std::string &resource) const;
  std::string extract_resource(unsigned long id) const;
};

#endif /* NAIVE_DYNAMIC_STRING_DICTIONARY_HPP */
