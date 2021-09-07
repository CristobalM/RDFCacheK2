//
// Created by cristobal on 9/6/21.
//

#ifndef RDFCACHEK2_PCMUPDATELOGGERWRAPPER_HPP
#define RDFCACHEK2_PCMUPDATELOGGERWRAPPER_HPP

#include "UpdatesLogger.hpp"
#include <I_UpdateLoggerPCM.hpp>
class PCMUpdateLoggerWrapper : public I_UpdateLoggerPCM {
  UpdatesLogger &logger;
public:
  explicit PCMUpdateLoggerWrapper(UpdatesLogger &logger);
  void recover_predicate(unsigned long predicate_id) override;
  bool has_predicate_stored(uint64_t predicate_id) override;
};

#endif // RDFCACHEK2_PCMUPDATELOGGERWRAPPER_HPP
