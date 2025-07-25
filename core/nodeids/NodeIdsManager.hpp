//
// Created by cristobal on 26-06-22.
//

#ifndef RDFCACHEK2_NODEIDSMANAGER_HPP
#define RDFCACHEK2_NODEIDSMANAGER_HPP
#include <cstdint>
namespace k2cache {
struct NodeIdsManager {
  virtual ~NodeIdsManager() = default;
  [[nodiscard]] virtual uint64_t get_id(uint64_t real_id) const = 0;
  virtual uint64_t get_id_or_create(uint64_t real_id) = 0;

  virtual uint64_t get_real_id(uint64_t mapped_id, int *err_code) const = 0;
};
} // namespace k2cache

#endif // RDFCACHEK2_NODEIDSMANAGER_HPP
