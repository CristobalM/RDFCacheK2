//
// Created by cristobal on 22-09-22.
//

#ifndef RDFCACHEK2_NOUPDATE_HPP
#define RDFCACHEK2_NOUPDATE_HPP
#include "UpdatesLogger.hpp"
namespace k2cache {
class NoUpdate : public UpdatesLogger {
public:
  void recover_predicate(uint64_t predicate_id) override;
  bool has_predicate_stored(uint64_t predicate_id) override;
  void compact_logs() override;
  std::vector<uint64_t> get_predicates() override;
  void clean_append_log() override;
  void recover_all() override;
  void log(std::vector<K2TreeUpdates> &updates) override;
  UpdatesLoggerFilesManager &get_fh_manager() override;
  int logs_number() override;

public:
};
} // namespace k2cache

#endif // RDFCACHEK2_NOUPDATE_HPP
