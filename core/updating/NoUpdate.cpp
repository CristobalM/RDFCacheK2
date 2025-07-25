//
// Created by cristobal on 22-09-22.
//

#include "NoUpdate.hpp"

namespace k2cache {

void NoUpdate::recover_predicate(uint64_t) {
}
bool NoUpdate::has_predicate_stored(uint64_t) {
  return false;
}
void NoUpdate::compact_logs() {
}
std::vector<uint64_t> NoUpdate::get_predicates() {
  throw std::runtime_error("get_predicates not implemented on NoUpdate");
}
void NoUpdate::clean_append_log() {
  throw std::runtime_error("clean_append_log not implemented on NoUpdate");
}
void NoUpdate::recover_all() {
//  throw std::runtime_error("recover_all not implemented on NoUpdate");
}
void NoUpdate::log(std::vector<K2TreeUpdates> &) {
  throw std::runtime_error("log not implemented on NoUpdate");
}
UpdatesLoggerFilesManager &NoUpdate::get_fh_manager() {
  throw std::runtime_error("get_fh_manager not implemented on NoUpdate");
}
int NoUpdate::logs_number() {
  throw std::runtime_error("logs_number not implemented on NoUpdate");
}
} // namespace k2cache