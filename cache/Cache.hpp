//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHE_HPP
#define RDFCACHEK2_CACHE_HPP

#include <memory>
#include <mutex>
#include <string>

#include <I_FileRWHandler.hpp>
#include <PredicatesCacheManager.hpp>
#include <replacement/I_CacheReplacement.hpp>
#include <request_msg.pb.h>

#include "CacheArgs.hpp"

struct CacheStats {
  int allocated_u32s;
  int nodes_count;
  int containers_sz_sum;
  int frontier_data;
  int blocks_data;

  int max_points_k2;
  int number_of_points_avg;
  int blocks_counted;
};

class Cache {
  std::shared_ptr<PredicatesCacheManager> cache_manager;

  std::string temp_files_dir;
  unsigned long timeout_ms;

  std::unique_ptr<I_CacheReplacement> cache_replacement;

  I_CacheReplacement::REPLACEMENT_STRATEGY strategy_id;

  std::string update_log_filename;

  std::unique_ptr<I_FileRWHandler> file_rw_handler;
  std::unique_ptr<I_FileRWHandler> file_offsets_rw_handler;
  std::unique_ptr<I_FileRWHandler> file_metadata_rw_handler;

public:
  Cache(std::shared_ptr<PredicatesCacheManager> predicates_cache_manager,
        CacheArgs args);

  RDFResource extract_resource(unsigned long index) const;

  PredicatesCacheManager &get_pcm();
  I_CacheReplacement &get_replacement();
  I_CacheReplacement::REPLACEMENT_STRATEGY get_strategy_id();
  std::vector<unsigned long> extract_loaded_predicates_from_sequence(
      const std::vector<unsigned long> &input_predicates_ids);

  I_FileRWHandler &get_log_file_handler();
  I_FileRWHandler &get_log_offsets_file_handler();
  I_FileRWHandler &get_log_metadata_file_handler();
};

#endif // RDFCACHEK2_CACHE_HPP