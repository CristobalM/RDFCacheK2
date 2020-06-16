//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_SDENTITIESMAPPING_HPP
#define RDFCACHEK2_SDENTITIESMAPPING_HPP

#include <StringDictionary.h>
#include <fstream>
#include <memory>

template <class SD> class SDEntitiesMapping {
  std::unique_ptr<StringDictionary> subjects_dictionary;
  std::unique_ptr<StringDictionary> predicates_dictionary;
  std::unique_ptr<StringDictionary> objects_dictionary;

public:
  SDEntitiesMapping(std::ifstream &subjects_dict_ifs,
                    std::ifstream &predicates_dict_ifs,
                    std::ifstream &objects_dict_ifs)
      : subjects_dictionary(SD::load(subjects_dict_ifs)),
        predicates_dictionary(SD::load(predicates_dict_ifs)),
        objects_dictionary(SD::load(objects_dict_ifs)) {}

  void save(const std::string &sub_fname, const std::string &pred_fname,
            const std::string &obj_fname) {
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
};

#endif // RDFCACHEK2_SDENTITIESMAPPING_HPP
