#include "EmptyISDManager.hpp"
#include <stdexcept>
void EmptyISDManager::save(const std::string &, const std::string &,
                           const std::string &) {}

uint64_t EmptyISDManager::blanks_index(std::string &) { return 0; }
uint64_t EmptyISDManager::literals_index(std::string &) { return 0; }
uint64_t EmptyISDManager::iris_index(std::string &) { return 0; }

bool EmptyISDManager::has_blanks_index(std::string &) { return false; }
bool EmptyISDManager::has_literals_index(std::string &) { return false; }
bool EmptyISDManager::has_iris_index(std::string &) { return false; }

std::string EmptyISDManager::get_blank(uint64_t) {
  throw std::runtime_error("Not implemented");
}
std::string EmptyISDManager::get_literal(uint64_t) {
  throw std::runtime_error("Not implemented");
}
std::string EmptyISDManager::get_iri(uint64_t) {
  throw std::runtime_error("Not implemented");
}
RDFResource EmptyISDManager::get_resource(uint64_t) {
  throw std::runtime_error("Not implemented");
}

unsigned long EmptyISDManager::last_iri_id() { return 0; }
unsigned long EmptyISDManager::last_blank_id() { return 0; }
unsigned long EmptyISDManager::last_literal_id() { return 0; }
unsigned long EmptyISDManager::last_id() { return 0; }
