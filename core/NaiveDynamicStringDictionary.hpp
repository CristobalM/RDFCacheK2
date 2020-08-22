
#ifndef NAIVE_DYNAMIC_STRING_DICTIONARY_HPP
#define NAIVE_DYNAMIC_STRING_DICTIONARY_HPP
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class NaiveDynamicStringDictionary {

  std::vector<std::string> subjects_extra;
  std::vector<std::string> predicates_extra;
  std::vector<std::string> objects_extra;

  std::unordered_map<std::string, unsigned long> reverse_subjects_extra;
  std::unordered_map<std::string, unsigned long> reverse_predicates_extra;
  std::unordered_map<std::string, unsigned long> reverse_objects_extra;

public:
  NaiveDynamicStringDictionary() = default;
  NaiveDynamicStringDictionary(
      std::vector<std::string> &&subjects_extra,
      std::vector<std::string> &&predicates_extra,
      std::vector<std::string> &&objects_extra,
      std::unordered_map<std::string, unsigned long> &&reverse_subjects_extra,
      std::unordered_map<std::string, unsigned long> &&reverse_predicates_extra,
      std::unordered_map<std::string, unsigned long> &&reverse_objects_extra);

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
  void add_subject(std::string subject);
  void add_predicate(std::string predicate);
  void add_object(std::string object);
  unsigned long locate_subject(const std::string &subject) const;
  unsigned long locate_predicate(const std::string &predicate) const;
  unsigned long locate_object(const std::string &object) const;
  std::string extract_subject(unsigned long id) const;
  std::string extract_predicate(unsigned long id) const;
  std::string extract_object(unsigned long id) const;
};

#endif /* NAIVE_DYNAMIC_STRING_DICTIONARY_HPP */
