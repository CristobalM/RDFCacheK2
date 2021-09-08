//
// Created by cristobal on 9/6/21.
//

#ifndef RDFCACHEK2_I_UPDATELOGGERPCM_HPP
#define RDFCACHEK2_I_UPDATELOGGERPCM_HPP

class I_UpdateLoggerPCM {
public:
  virtual ~I_UpdateLoggerPCM() = default;
  virtual void recover_predicate(unsigned long predicate_id) = 0;
  virtual bool has_predicate_stored(uint64_t predicate_id) = 0;
};

#endif // RDFCACHEK2_I_UPDATELOGGERPCM_HPP
