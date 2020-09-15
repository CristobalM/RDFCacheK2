
#ifndef NAIVE_DYNAMIC_STRING_DICTIONARY_HPP
#define NAIVE_DYNAMIC_STRING_DICTIONARY_HPP
#include "RDFTriple.hpp"
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class NaiveDynamicStringDictionary {

  std::vector<std::string> iris_extra;
  std::vector<std::string> blanks_extra;
  std::vector<std::string> literals_extra;

  std::unordered_map<std::string, unsigned long> reverse_iris_extra;
  std::unordered_map<std::string, unsigned long> reverse_blanks_extra;
  std::unordered_map<std::string, unsigned long> reverse_literals_extra;

public:
  NaiveDynamicStringDictionary() = default;
  NaiveDynamicStringDictionary(
      std::vector<std::string> &&iris_extra,
      std::vector<std::string> &&blanks_extra,
      std::vector<std::string> &&literals_extra,
      std::unordered_map<std::string, unsigned long> &&reverse_iris_extra,
      std::unordered_map<std::string, unsigned long> &&reverse_blanks_extra,
      std::unordered_map<std::string, unsigned long> &&reverse_literals_extra);

  static void serialize_dict(std::vector<std::string> &strings,
                             const std::string &fname);
  static std::vector<std::string> deserialize_dict(const std::string &fname);
  static std::unordered_map<std::string, unsigned long>
  create_reverse(std::vector<std::string> &input);
  void save(const std::string &sub_fname, const std::string &pred_fname,
            const std::string &obj_fname);
  static NaiveDynamicStringDictionary load(const std::string &sub_fname,
                                           const std::string &pred_fname,
                                           const std::string &obj_fname);
  void add_iri(std::string iri);
  void add_blank(std::string blank);
  void add_literal(std::string literal);
  void add_resource(RDFResource &resource);
  unsigned long locate_iri(const std::string &iri) const;
  unsigned long locate_blank(const std::string &blank) const;
  unsigned long locate_literal(const std::string &literal) const;
  unsigned long locate_resource(const RDFResource &resource) const;
  std::string extract_iri(unsigned long id) const;
  std::string extract_blank(unsigned long id) const;
  std::string extract_literal(unsigned long id) const;
  RDFResource extract_resource(unsigned long id) const;
};

#endif /* NAIVE_DYNAMIC_STRING_DICTIONARY_HPP */
