#ifndef EMPTY_ISD_MANAGER_HPP
#define EMPTY_ISD_MANAGER_HPP

#include "ISDManager.hpp"

class EmptyISDManager : public ISDManager {
public:
  void save(const std::string &sub_fname, const std::string &pred_fname,
            const std::string &obj_fname) override;

  uint64_t blanks_index(const std::string &blank) override;
  uint64_t literals_index(const std::string &literal) override;
  uint64_t iris_index(const std::string &iri) override;

  bool has_blanks_index(const std::string &blank) override;
  bool has_literals_index(const std::string &literal) override;
  bool has_iris_index(const std::string &iri) override;

  std::string get_blank(uint64_t _blanks_index) override;
  std::string get_literal(uint64_t _literals_index) override;
  std::string get_iri(uint64_t _iris_index) override;
  RDFResource get_resource(uint64_t index) override;

  unsigned long last_iri_id() override;
  unsigned long last_blank_id() override;
  unsigned long last_literal_id() override;
  unsigned long last_id() override;
};

#endif