//
// Created by cristobal on 04-08-21.
//

#ifndef RDFCACHEK2_I_CACHEREPLACEMENT_HPP
#define RDFCACHEK2_I_CACHEREPLACEMENT_HPP

#include <cstddef>
#include <mutex>
namespace k2cache {
class I_CacheReplacement {
public:
  enum REPLACEMENT_STRATEGY { NO_CACHING = 0, LRU, FREQUENCY };

  virtual ~I_CacheReplacement() = default;
  virtual bool hit_key(uint64_t key, size_t space_required) = 0;
  virtual void mark_using(uint64_t key) = 0;
  virtual void mark_ready(uint64_t key) = 0;
  virtual std::mutex &get_replacement_mutex() = 0;
};
} // namespace k2cache

#endif // RDFCACHEK2_I_CACHEREPLACEMENT_HPP
