//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_SDENTITIESMAPPING_HPP
#define RDFCACHEK2_SDENTITIESMAPPING_HPP

#include "ISDManager.hpp"
#include <StringDictionary.h>
#include <fstream>
#include <memory>

template <class SDIRIS, class SDBlanks,
          class SDLiterals>
class SDEntitiesMapping : public ISDManager {
  std::unique_ptr<StringDictionary> iris_dictionary;
  std::unique_ptr<StringDictionary> blanks_dictionary;
  std::unique_ptr<StringDictionary> literals_dictionary;

public:
  SDEntitiesMapping(std::ifstream &iris_dict_ifs,
                    std::ifstream &blanks_dict_ifs,
                    std::ifstream &literals_dict_ifs)
      : iris_dictionary(SDIRIS::load(iris_dict_ifs)),
        blanks_dictionary(SDBlanks::load(blanks_dict_ifs)),
        literals_dictionary(SDLiterals::load(literals_dict_ifs)) {}

  void save(const std::string &iris_fname, const std::string &blanks_fname,
            const std::string &literals_fname) override {
    {
      std::ofstream ofs(iris_fname, std::ofstream::binary);
      iris_dictionary->save(ofs);
    }

    {
      std::ofstream ofs(blanks_fname, std::ofstream::binary);
      blanks_dictionary->save(ofs);
    }

    {
      std::ofstream ofs(literals_fname, std::ofstream::binary);
      literals_dictionary->save(ofs);
    }
  }

  uint64_t blanks_index(std::string &blank) override {
    return blanks_dictionary->locate(
        reinterpret_cast<unsigned char *>(blank.data()), blank.size());
  }

  uint64_t literals_index(std::string &literal) override {
    return literals_dictionary->locate(
        reinterpret_cast<unsigned char *>(literal.data()), literal.size());
  }

  uint64_t iris_index(std::string &iri) override {
    return iris_dictionary->locate(
        reinterpret_cast<unsigned char *>(iri.data()), iri.size());
  }

  bool has_blanks_index(std::string &blank) override {
    auto result = blanks_index(blank);
    return result != NORESULT;
  }

  bool has_literals_index(std::string &literal) override {
    auto result = literals_index(literal);
    return result != NORESULT;
  }

  bool iris_index(std::string &iri) override {
    auto result = iris_index(iri);
    return result != NORESULT;
  }

  std::string get_blank(uint64_t blanks_index) override {
    unsigned int result_sz;
    unsigned char *result =
        blanks_dictionary->extract(blanks_index, &result_sz);
    std::string s(reinterpret_cast<char *>(result), result_sz);
    delete[] result;
    return s;
  }

  std::string get_literal(uint64_t literals_index) override {
    unsigned int result_sz;
    unsigned char *result =
        literals_dictionary->extract(literals_index, &result_sz);
    std::string s(reinterpret_cast<char *>(result), result_sz);
    delete[] result;
    return s;
  }

  std::string get_iri(uint64_t iris_index) override {
    unsigned int result_sz;
    unsigned char *result =
        iris_dictionary->extract(iris_index, &result_sz);
    std::string s(reinterpret_cast<char *>(result), result_sz);
    delete[] result;
    return s;
  }

  unsigned long last_iri_id() override {
    return iris_dictionary->numElements();
  }

  unsigned long last_blank_id() override {
    return blanks_dictionary->numElements();
  }

  unsigned long last_literal_id() override {
    return literals_dictionary->numElements();
  }
};

#endif // RDFCACHEK2_SDENTITIESMAPPING_HPP
