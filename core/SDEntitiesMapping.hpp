//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_SDENTITIESMAPPING_HPP
#define RDFCACHEK2_SDENTITIESMAPPING_HPP

#include "ISDManager.hpp"
#include <StringDictionary.h>
#include <fstream>
#include <memory>

template <class SDIRIS, class SDBlanks, class SDLiterals>
class SDEntitiesMapping : public ISDManager {
  std::unique_ptr<StringDictionary> iris_dictionary;
  std::unique_ptr<StringDictionary> blanks_dictionary;
  std::unique_ptr<StringDictionary> literals_dictionary;

public:
  SDEntitiesMapping(std::istream &iris_dict_ifs, std::istream &blanks_dict_ifs,
                    std::istream &literals_dict_ifs)
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

  uint64_t blanks_index(const std::string &blank) override {
    return _offset_index_cond(
        blanks_dictionary->locate(
            reinterpret_cast<unsigned char *>(const_cast<char *>(blank.data())),
            static_cast<uint>(blank.size())),
        iris_dictionary->numElements());
  }

  uint64_t literals_index(const std::string &literal) override {
    return _offset_index_cond(
        literals_dictionary->locate(reinterpret_cast<unsigned char *>(
                                        const_cast<char *>(literal.data())),
                                    static_cast<uint>(literal.size())),
        iris_dictionary->numElements() + blanks_dictionary->numElements());
  }

  uint64_t iris_index(const std::string &iri) override {
    return iris_dictionary->locate(
        reinterpret_cast<unsigned char *>(const_cast<char *>(iri.data())),
        static_cast<uint>(iri.size()));
  }

  bool has_blanks_index(const std::string &blank) override {
    auto result = blanks_index(blank);
    return result != NORESULT;
  }

  bool has_literals_index(const std::string &literal) override {
    auto result = literals_index(literal);
    return result != NORESULT;
  }

  bool has_iris_index(const std::string &iri) override {
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
    unsigned char *result = iris_dictionary->extract(iris_index, &result_sz);
    std::string s(reinterpret_cast<char *>(result), result_sz);
    delete[] result;
    return s;
  }

  RDFResource get_resource(uint64_t index) override {
    RDFResourceType res_type;
    std::string data;
    if (index <= iris_dictionary->numElements()) {

      res_type = RDF_TYPE_IRI;
      data = _extract_from_sd(iris_dictionary.get(), index);
    } else if (index <= iris_dictionary->numElements() +
                            blanks_dictionary->numElements()) {
      res_type = RDF_TYPE_BLANK;
      data = _extract_from_sd(blanks_dictionary.get(),
                              index - iris_dictionary->numElements());
    } else {
      res_type = RDF_TYPE_LITERAL;
      data = _extract_from_sd(literals_dictionary.get(),
                              index - iris_dictionary->numElements() -
                                  blanks_dictionary->numElements());
    }
    return RDFResource(std::move(data), res_type);
  }

  unsigned long last_iri_id() override {
    return iris_dictionary->numElements();
  }

  unsigned long last_blank_id() override {
    return last_iri_id() + blanks_dictionary->numElements();
  }

  unsigned long last_literal_id() override {
    return last_blank_id() + literals_dictionary->numElements();
  }

  unsigned long last_id() override { return last_literal_id(); }

private:
  uint64_t _offset_index_cond(uint64_t index, uint64_t offset) {
    if (index > 0)
      return index + offset;
    return 0;
  }

  std::string _pop_string_from_ucarr(unsigned char *data, size_t size) {
    std::string result(reinterpret_cast<char *>(data), size);
    delete[] data;
    return result;
  }

  std::string _extract_from_sd(StringDictionary *sd, uint64_t index) {
    unsigned int string_size;
    unsigned char *data = sd->extract(index, &string_size);
    return _pop_string_from_ucarr(data, string_size);
  }
};

#endif // RDFCACHEK2_SDENTITIESMAPPING_HPP
