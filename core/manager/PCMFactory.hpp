//
// Created by cristobal on 26-06-22.
//

#ifndef RDFCACHEK2_PCMFACTORY_HPP
#define RDFCACHEK2_PCMFACTORY_HPP

#include "CacheArgs.hpp"
#include "I_FileRWHandler.hpp"
#include "PredicatesCacheManager.hpp"
#include "updating/UpdatesLoggerFilesManager.hpp"
#include <memory>
#include <string>

namespace k2cache {
struct PCMFactory {
  //  static std::unique_ptr<PredicatesCacheManager>
  //  create(std::unique_ptr<I_FileRWHandler> &&index_file_handler,
  //         UpdatesLoggerFilesManager &&updates_logger_fm,
  //         std::unique_ptr<FullyIndexedCache> &&fully_indexed_cache);
  //
  //  static std::unique_ptr<PredicatesCacheManager>
  //  create(std::unique_ptr<I_FileRWHandler> &&index_file_handler,
  //         std::unique_ptr<FullyIndexedCache> &&fully_indexed_cache);

  static std::unique_ptr<PredicatesCacheManager>
  create(const CacheArgs &cache_args);

  static std::unique_ptr<PredicatesCacheManager>
  create(const std::string &location);

  static std::unique_ptr<PredicatesCacheManager>
  create(std::unique_ptr<I_FileRWHandler> &&index_file_handler);
  static std::unique_ptr<PredicatesCacheManager>
  create(std::unique_ptr<I_FileRWHandler> &&index_file_handler,
         UpdatesLoggerFilesManager &&updates_logger_files_manager,
         bool has_fic);
};
} // namespace k2cache
#endif // RDFCACHEK2_PCMFACTORY_HPP
