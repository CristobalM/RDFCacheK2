//
// Created by cristobal on 30-06-22.
//

#include "NodeIdsManagerIdentity.hpp"

uint64_t k2cache::NodeIdsManagerIdentity::get_id(uint64_t real_id) const { return real_id; }
uint64_t k2cache::NodeIdsManagerIdentity::get_real_id(uint64_t mapped_id, int *) const {
  return mapped_id;
}
uint64_t k2cache::NodeIdsManagerIdentity::get_id_or_create(uint64_t real_id) {
  return real_id;
}
