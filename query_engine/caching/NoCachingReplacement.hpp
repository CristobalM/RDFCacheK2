//
// Created by cristobal on 05-08-21.
//

#ifndef RDFCACHEK2_NOCACHINGREPLACEMENT_HPP
#define RDFCACHEK2_NOCACHINGREPLACEMENT_HPP

#include "I_CacheReplacement.hpp"
class NoCachingReplacement : public I_CacheReplacement {
  std::mutex m;

public:
  bool hit_key(unsigned long key, size_t space_required) override;
  void mark_using(unsigned long key) override;
  void mark_ready(unsigned long key) override;
  std::mutex &get_replacement_mutex() override;
};

#endif // RDFCACHEK2_NOCACHINGREPLACEMENT_HPP
