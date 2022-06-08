//
// Created by cristobal on 9/6/21.
//

#include "PCMUpdateLoggerWrapper.hpp"
PCMUpdateLoggerWrapper::PCMUpdateLoggerWrapper(UpdatesLogger &logger)
    : logger(logger) {}
void PCMUpdateLoggerWrapper::recover_predicate(unsigned long predicate_id) {
  logger.recover_predicate(predicate_id);
}
bool PCMUpdateLoggerWrapper::has_predicate_stored(uint64_t predicate_id) {
  return logger.has_predicate_stored(predicate_id);
}
void PCMUpdateLoggerWrapper::compact_logs() {
  logger.compact_logs();
}
std::vector<unsigned long> PCMUpdateLoggerWrapper::get_predicates() {
  return logger.get_predicates();
}
void PCMUpdateLoggerWrapper::clean_append_log() {
  logger.clean_append_log();
}
