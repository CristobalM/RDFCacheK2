//
// Created by cristobal on 26-06-22.
//

#include "PCMFactory.hpp"
#include "PredicatesCacheManagerImpl.hpp"

namespace k2cache {
std::unique_ptr<PredicatesCacheManager>
PCMFactory::create(std::unique_ptr<I_FileRWHandler> &&index_file_handler,
                   k2cache::UpdatesLoggerFilesManager &&updates_logger_fm) {
  return std::make_unique<PredicatesCacheManagerImpl>(
      std::move(index_file_handler), std::move(updates_logger_fm));
}
std::unique_ptr<PredicatesCacheManager>
PCMFactory::create(std::unique_ptr<I_FileRWHandler> &&index_file_handler) {
  return std::make_unique<PredicatesCacheManagerImpl>(
      std::move(index_file_handler));
}

std::unique_ptr<PredicatesCacheManager>
PCMFactory::create(const CacheArgs &cache_args) {
  return std::make_unique<PredicatesCacheManagerImpl>(cache_args);
}
std::unique_ptr<PredicatesCacheManager>
PCMFactory::create(const std::string &location) {
  return std::make_unique<PredicatesCacheManagerImpl>(location);
}

} // namespace k2cache
