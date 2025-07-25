//
// Created by cristobal on 9/6/21.
//

#ifndef RDFCACHEK2_UPDATESLOGGER_HPP
#define RDFCACHEK2_UPDATESLOGGER_HPP

#include "K2TreeUpdates.hpp"
#include "UpdatesLoggerFilesManager.hpp"
#include <vector>

namespace k2cache {
struct UpdatesLogger {
  virtual ~UpdatesLogger() = default;
  virtual void recover_predicate(uint64_t predicate_id) = 0;
  virtual bool has_predicate_stored(uint64_t predicate_id) = 0;
  virtual void compact_logs() = 0;
  virtual std::vector<uint64_t> get_predicates() = 0;
  virtual void clean_append_log() = 0;
  virtual void recover_all() = 0;
  virtual void log(std::vector<K2TreeUpdates> &updates) = 0;
  virtual UpdatesLoggerFilesManager &get_fh_manager() = 0;
  virtual int logs_number() = 0;
};
} // namespace k2cache

#endif // RDFCACHEK2_UPDATESLOGGER_HPP
