//
// Created by cristobal on 26-06-22.
//

#include "PCMFactory.hpp"
#include "FileRWHandler.hpp"
#include "PredicatesCacheManagerImpl.hpp"
#include "nodeids/NodeIdsManagerFactory.hpp"
#include "nodeids/NodeIdsManagerIdentity.hpp"

namespace k2cache {
std::unique_ptr<PredicatesCacheManager>
PCMFactory::create(std::unique_ptr<I_FileRWHandler> &&index_file_handler,
                   UpdatesLoggerFilesManager &&updates_logger_fm,
                   std::unique_ptr<NodeIdsManager> &&nis) {
  return std::make_unique<PredicatesCacheManagerImpl>(
      std::move(index_file_handler), std::move(updates_logger_fm),
      std::move(nis));
}
std::unique_ptr<PredicatesCacheManager>
PCMFactory::create(std::unique_ptr<I_FileRWHandler> &&index_file_handler,
                   UpdatesLoggerFilesManager &&updates_logger_fm) {
  return create(std::move(index_file_handler), std::move(updates_logger_fm),
                std::make_unique<NodeIdsManagerIdentity>());
}

std::unique_ptr<PredicatesCacheManager>
PCMFactory::create(std::unique_ptr<I_FileRWHandler> &&index_file_handler,
                   std::unique_ptr<NodeIdsManager> &&nis) {
  return std::make_unique<PredicatesCacheManagerImpl>(
      std::make_unique<PredicatesIndexCacheMD>(std::move(index_file_handler)),
      std::move(nis));
}
std::unique_ptr<PredicatesCacheManager>
PCMFactory::create(std::unique_ptr<I_FileRWHandler> &&index_file_handler) {
  return create(std::move(index_file_handler),
                std::make_unique<NodeIdsManagerIdentity>());
}

std::unique_ptr<PredicatesCacheManager>
PCMFactory::create(const CacheArgs &cache_args) {
  return create(std::make_unique<FileRWHandler>(cache_args.index_filename),
                UpdatesLoggerFilesManager(cache_args),
                NodeIdsManagerFactory::create(cache_args));
}

std::unique_ptr<PredicatesCacheManager>
PCMFactory::create(const std::string &location,
                   std::unique_ptr<NodeIdsManager> &&nis) {
  return create(std::make_unique<FileRWHandler>(location), std::move(nis));
}

std::unique_ptr<PredicatesCacheManager>
PCMFactory::create(const std::string &location) {
  return create(location, std::make_unique<NodeIdsManagerIdentity>());
}

} // namespace k2cache
