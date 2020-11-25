//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_ISDMANAGER_HPP
#define RDFCACHEK2_ISDMANAGER_HPP

#include "RDFTriple.hpp"
#include <string>

class ISDManager {
public:
  virtual void save(const std::string &sub_fname, const std::string &pred_fname,
                    const std::string &obj_fname) = 0;

  virtual uint64_t blanks_index(const std::string &blank) = 0;
  virtual uint64_t literals_index(const std::string &literal) = 0;
  virtual uint64_t iris_index(const std::string &iri) = 0;

  virtual bool has_blanks_index(const std::string &blank) = 0;
  virtual bool has_literals_index(const std::string &literal) = 0;
  virtual bool has_iris_index(const std::string &iri) = 0;

  virtual std::string get_blank(uint64_t _blanks_index) = 0;
  virtual std::string get_literal(uint64_t _literals_index) = 0;
  virtual std::string get_iri(uint64_t _iris_index) = 0;
  virtual RDFResource get_resource(uint64_t index) = 0;

  virtual unsigned long last_iri_id() = 0;
  virtual unsigned long last_blank_id() = 0;
  virtual unsigned long last_literal_id() = 0;
  virtual unsigned long last_id() = 0;

  virtual ~ISDManager() = default;
};
#endif // RDFCACHEK2_ISDMANAGER_HPP
