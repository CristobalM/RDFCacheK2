//
// Created by cristobal on 30-06-22.
//

#include "NodeIdsManagerIdentity.hpp"

long k2cache::NodeIdsManagerIdentity::get_id(long real_id) { return real_id; }
long k2cache::NodeIdsManagerIdentity::get_real_id(long mapped_id) {
  return mapped_id;
}
long k2cache::NodeIdsManagerIdentity::get_id_or_create(long real_id) {
  return real_id;
}
