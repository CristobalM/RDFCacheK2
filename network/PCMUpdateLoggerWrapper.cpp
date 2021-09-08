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
