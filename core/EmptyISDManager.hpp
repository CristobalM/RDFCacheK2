#ifndef EMPTY_ISD_MANAGER_HPP
#define EMPTY_ISD_MANAGER_HPP

#include "ISDManager.hpp"

class EmptyISDManager : public ISDManager {
public:
  void save(const std::string &sub_fname, const std::string &pred_fname,
            const std::string &obj_fname);

  uint64_t blanks_index(std::string &blank);
  uint64_t literals_index(std::string &literal);
  uint64_t iris_index(std::string &iri);

  bool has_blanks_index(std::string &blank);
  bool has_literals_index(std::string &literal);
  bool has_iris_index(std::string &iri);

  std::string get_blank(uint64_t _blanks_index);
  std::string get_literal(uint64_t _literals_index);
  std::string get_iri(uint64_t _iris_index);
  RDFResource get_resource(uint64_t index);

  unsigned long last_iri_id();
  unsigned long last_blank_id();
  unsigned long last_literal_id();
  unsigned long last_id();
};

#endif