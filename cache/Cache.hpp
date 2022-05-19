//
// Created by Cristobal Miranda, 2020
//

#ifndef RDFCACHEK2_CACHE_HPP
#define RDFCACHEK2_CACHE_HPP

#include <memory>
#include <mutex>
#include <string>

#include <I_FileRWHandler.hpp>
#include <NodesSequence.hpp>
#include <PredicatesCacheManager.hpp>
#include <replacement/I_CacheReplacement.hpp>
#include <request_msg.pb.h>

#include "CacheArgs.hpp"

class Cache {
  std::shared_ptr<PredicatesCacheManager> cache_manager;
  std::unique_ptr<NodesSequence> nodes_sequence;

  std::unique_ptr<I_CacheReplacement> cache_replacement;

  I_CacheReplacement::REPLACEMENT_STRATEGY strategy_id;

  std::string update_log_filename;

  std::unique_ptr<I_FileRWHandler> file_rw_handler;
  std::unique_ptr<I_FileRWHandler> file_offsets_rw_handler;
  std::unique_ptr<I_FileRWHandler> file_metadata_rw_handler;

public:
  Cache(std::shared_ptr<PredicatesCacheManager> predicates_cache_manager,
        const CacheArgs &args);

  PredicatesCacheManager &get_pcm();
  NodesSequence &get_nodes_sequence();
  I_CacheReplacement &get_replacement();
  I_CacheReplacement::REPLACEMENT_STRATEGY get_strategy_id();
  std::vector<unsigned long> extract_loaded_predicates_from_sequence(
      const std::vector<unsigned long> &input_predicates_ids);

  I_FileRWHandler &get_log_file_handler();
  I_FileRWHandler &get_log_offsets_file_handler();
  I_FileRWHandler &get_log_metadata_file_handler();
  void sync_logs_with_indexes();
};

#endif // RDFCACHEK2_CACHE_HPP