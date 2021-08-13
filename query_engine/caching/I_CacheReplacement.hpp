//
// Created by cristobal on 04-08-21.
//

#ifndef RDFCACHEK2_I_CACHEREPLACEMENT_HPP
#define RDFCACHEK2_I_CACHEREPLACEMENT_HPP

#include <cstddef>
#include <mutex>
class I_CacheReplacement {
public:
  enum REPLACEMENT_STRATEGY { NO_CACHING = 0, LRU, FREQUENCY };

  virtual ~I_CacheReplacement() = default;
  virtual bool hit_key(unsigned long key, size_t space_required) = 0;
  virtual void mark_using(unsigned long key) = 0;
  virtual void mark_ready(unsigned long key) = 0;
  virtual std::mutex &get_replacement_mutex() = 0;
};

#endif // RDFCACHEK2_I_CACHEREPLACEMENT_HPP
