//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_ISDMANAGER_HPP
#define RDFCACHEK2_ISDMANAGER_HPP

#include <string>

class ISDManager {
public:
  virtual void save(const std::string &sub_fname, const std::string &pred_fname,
                    const std::string &obj_fname) = 0;

  virtual uint64_t predicate_index(std::string &predicate) = 0;
  virtual uint64_t object_index(std::string &object) = 0;
  virtual uint64_t subject_index(std::string &subject) = 0;

  virtual bool has_predicate_index(std::string &predicate) = 0;
  virtual bool has_object_index(std::string &object) = 0;
  virtual bool has_subject_index(std::string &subject) = 0;

  virtual std::string get_predicate(uint64_t predicate_index) = 0;
  virtual std::string get_object(uint64_t object_index) = 0;
  virtual std::string get_subject(uint64_t subject_index) = 0;

  virtual unsigned long last_subject_id() = 0;
  virtual unsigned long last_predicate_id() = 0;
  virtual unsigned long last_object_id() = 0;

  virtual ~ISDManager() = default;
};
#endif // RDFCACHEK2_ISDMANAGER_HPP
