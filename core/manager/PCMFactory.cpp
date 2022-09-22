//
// Created by cristobal on 26-06-22.
//

#include "PCMFactory.hpp"
#include "FetcherWrapperLazyInit.hpp"
#include "FileRWHandler.hpp"
#include "PredicatesCacheManagerImpl.hpp"
#include "fic/NoFIC.hpp"
#include "updating/NoUpdate.hpp"
#include "updating/PCMMergerWrapper.hpp"
#include "updating/UpdatesLoggerImpl.hpp"

namespace k2cache {
// std::unique_ptr<PredicatesCacheManager>
// PCMFactory::create(std::unique_ptr<I_FileRWHandler> &&index_file_handler,
//                    UpdatesLoggerFilesManager &&updates_logger_fm,
//                    std::unique_ptr<FullyIndexedCache> &&fully_indexed_cache)
//                    {
//   return std::make_unique<PredicatesCacheManagerImpl>(
//       std::move(index_file_handler), std::move(updates_logger_fm));
// }
//
// std::unique_ptr<PredicatesCacheManager>
// PCMFactory::create(std::unique_ptr<I_FileRWHandler> &&index_file_handler,
//                    std::unique_ptr<FullyIndexedCache> &&fully_indexed_cache)
//                    {
//   return std::make_unique<PredicatesCacheManagerImpl>(
//       std::make_unique<PredicatesIndexCacheMD>(std::move(index_file_handler)),
//       std::move(fully_indexed_cache)
//       );
// }

std::unique_ptr<PredicatesCacheManager>
PCMFactory::create(const CacheArgs &cache_args) {
  auto frw_handler = std::make_unique<FileRWHandler>(cache_args.index_filename);
  UpdatesLoggerFilesManager update_logger_fm(cache_args);
  auto pcm_merger_wrapper = std::make_unique<PCMMergerWrapper>();
  auto &pcm_merger_wrapper_ref = *pcm_merger_wrapper;
  auto update_logger = std::make_unique<UpdatesLoggerImpl>(
      std::move(pcm_merger_wrapper), std::move(update_logger_fm));
  std::unique_ptr<FullyIndexedCache> fic{};
  auto fetcher = std::make_unique<FetcherWrapperLazyInit>();
  auto &fetcher_ref = *fetcher;
  if (cache_args.has_fic) {
    fic = std::make_unique<FullyIndexedCacheImpl>(std::move(fetcher));
  } else {
    fic = std::make_unique<NoFIC>();
  }
  auto picmd = std::make_unique<PredicatesIndexCacheMD>(std::move(frw_handler));
  auto pcm = std::make_unique<PredicatesCacheManagerImpl>(
      std::move(picmd), std::move(update_logger), std::move(fic));
  fetcher_ref.set_ref(&pcm->get_predicates_index_cache());
  pcm_merger_wrapper_ref.set_ref(pcm.get());
  return pcm;
}

// read only simpler version
std::unique_ptr<PredicatesCacheManager>
PCMFactory::create(const std::string &location) {
  auto frw_handler = std::make_unique<FileRWHandler>(location);
  auto picmd = std::make_unique<PredicatesIndexCacheMD>(std::move(frw_handler));
  auto pcm = std::make_unique<PredicatesCacheManagerImpl>(
      std::move(picmd), std::make_unique<NoUpdate>(),
      std::make_unique<NoFIC>());
  return pcm;
}
std::unique_ptr<PredicatesCacheManager>
PCMFactory::create(std::unique_ptr<I_FileRWHandler> &&index_file_handler) {
  auto picmd =
      std::make_unique<PredicatesIndexCacheMD>(std::move(index_file_handler));
  auto pcm = std::make_unique<PredicatesCacheManagerImpl>(
      std::move(picmd), std::make_unique<NoUpdate>(),
      std::make_unique<NoFIC>());
  return pcm;
}
std::unique_ptr<PredicatesCacheManager>
PCMFactory::create(std::unique_ptr<I_FileRWHandler> &&index_file_handler,
                   UpdatesLoggerFilesManager &&updates_logger_files_manager,
                   bool has_fic = false) {
  auto picmd =
      std::make_unique<PredicatesIndexCacheMD>(std::move(index_file_handler));

  auto pcm_merger_wrapper = std::make_unique<PCMMergerWrapper>();
  auto &pcm_merger_wrapper_ref = *pcm_merger_wrapper;
  auto update_logger = std::make_unique<UpdatesLoggerImpl>(
      std::move(pcm_merger_wrapper), std::move(updates_logger_files_manager));

  std::unique_ptr<FullyIndexedCache> fic{};
  auto fetcher = std::make_unique<FetcherWrapperLazyInit>();
  auto &fetcher_ref = *fetcher;
  if (has_fic) {
    fic = std::make_unique<FullyIndexedCacheImpl>(std::move(fetcher));
  } else {
    fic = std::make_unique<NoFIC>();
  }
  auto pcm = std::make_unique<PredicatesCacheManagerImpl>(
      std::move(picmd), std::move(update_logger), std::move(fic));

  pcm_merger_wrapper_ref.set_ref(pcm.get());
  fetcher_ref.set_ref(&pcm->get_predicates_index_cache());
  return pcm;
}
} // namespace k2cache
