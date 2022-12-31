//
// Created by cristobal on 05-08-21.
//

#ifndef RDFCACHEK2_NOCACHINGREPLACEMENT_HPP
#define RDFCACHEK2_NOCACHINGREPLACEMENT_HPP

#include "I_CacheReplacement.hpp"
namespace k2cache {
class NoCachingReplacement : public I_CacheReplacement {
  std::mutex m;

public:
  bool hit_key(uint64_t key, size_t space_required) override;
  void mark_using(uint64_t key) override;
  void mark_ready(uint64_t key) override;
  std::mutex &get_replacement_mutex() override;
};
} // namespace k2cache

#endif // RDFCACHEK2_NOCACHINGREPLACEMENT_HPP
