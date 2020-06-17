//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_SDENTITIESMAPPING_HPP
#define RDFCACHEK2_SDENTITIESMAPPING_HPP

#include "ISDManager.hpp"
#include <StringDictionary.h>
#include <fstream>
#include <memory>

template <class SD> class SDEntitiesMapping : public ISDManager {
  std::unique_ptr<StringDictionary> subjects_dictionary;
  std::unique_ptr<StringDictionary> predicates_dictionary;
  std::unique_ptr<StringDictionary> objects_dictionary;

  std::map<std::string, uint64_t> extra_subjects_mapping;
  std::map<std::string, uint64_t> extra_predicates_mapping;
  std::map<std::string, uint64_t> extra_objects_mapping;

  std::map<uint64_t, std::string> extra_subjects_rev_mapping;
  std::map<uint64_t, std::string> extra_predicates_rev_mapping;
  std::map<uint64_t, std::string> extra_objects_rev_mapping;

public:
  SDEntitiesMapping(std::ifstream &subjects_dict_ifs,
                    std::ifstream &predicates_dict_ifs,
                    std::ifstream &objects_dict_ifs)
      : subjects_dictionary(SD::load(subjects_dict_ifs)),
        predicates_dictionary(SD::load(predicates_dict_ifs)),
        objects_dictionary(SD::load(objects_dict_ifs)) {}

  void save(const std::string &sub_fname, const std::string &pred_fname,
            const std::string &obj_fname) override {
    {
      std::ofstream ofs(sub_fname, std::ofstream::binary);
      subjects_dictionary->save(ofs);
    }

    {
      std::ofstream ofs(pred_fname, std::ofstream::binary);
      predicates_dictionary->save(ofs);
    }

    {
      std::ofstream ofs(obj_fname, std::ofstream::binary);
      objects_dictionary->save(ofs);
    }
  }

  uint64_t predicate_index(std::string &predicate) override {
    return predicates_dictionary->locate(
        reinterpret_cast<unsigned char *>(predicate.data()), predicate.size());
  }

  uint64_t object_index(std::string &object) override {
    return objects_dictionary->locate(
        reinterpret_cast<unsigned char *>(object.data()), object.size());
  }

  uint64_t subject_index(std::string &subject) override {
    return subjects_dictionary->locate(
        reinterpret_cast<unsigned char *>(subject.data()), subject.size());
  }

  bool has_predicate_index(std::string &predicate) override {
    auto result = predicate_index(predicate);
    return result != NORESULT;
  }

  bool has_object_index(std::string &object) override {
    auto result = object_index(object);
    return result != NORESULT;
  }

  bool has_subject_index(std::string &subject) override {
    auto result = subject_index(subject);
    return result != NORESULT;
  }

  std::string get_predicate(uint64_t predicate_index) override {
    unsigned int result_sz;
    unsigned char *result =
        predicates_dictionary->extract(predicate_index, &result_sz);
    std::string s(reinterpret_cast<char *>(result), result_sz);
    delete[] result;
    return s;
  }

  std::string get_object(uint64_t object_index) override {
    unsigned int result_sz;
    unsigned char *result =
        objects_dictionary->extract(object_index, &result_sz);
    std::string s(reinterpret_cast<char *>(result), result_sz);
    delete[] result;
    return s;
  }

  std::string get_subject(uint64_t subject_index) override {
    unsigned int result_sz;
    unsigned char *result =
        subjects_dictionary->extract(subject_index, &result_sz);
    std::string s(reinterpret_cast<char *>(result), result_sz);
    delete[] result;
    return s;
  }
};

#endif // RDFCACHEK2_SDENTITIESMAPPING_HPP
