//
// Created by cristobal on 9/6/21.
//

#ifndef RDFCACHEK2_I_UPDATELOGGERPCM_HPP
#define RDFCACHEK2_I_UPDATELOGGERPCM_HPP

#include <vector>

class I_UpdateLoggerPCM {
public:
  virtual ~I_UpdateLoggerPCM() = default;
  virtual void recover_predicate(unsigned long predicate_id) = 0;
  virtual bool has_predicate_stored(unsigned long predicate_id) = 0;
  virtual void compact_logs() = 0;
  virtual std::vector<unsigned long> get_predicates() = 0;
  virtual void clean_append_log() = 0;
  virtual void recover_all() = 0;
};

#endif // RDFCACHEK2_I_UPDATELOGGERPCM_HPP
