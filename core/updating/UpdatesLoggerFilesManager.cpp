//
// Created by cristobal on 11-06-22.
//

#include "UpdatesLoggerFilesManager.hpp"
#include "FileRWHandler.hpp"
namespace k2cache {

UpdatesLoggerFilesManager::UpdatesLoggerFilesManager(
    std::unique_ptr<I_FileRWHandler> &&index_logs_fh,
    std::unique_ptr<I_FileRWHandler> &&offsets_fh,
    std::unique_ptr<I_FileRWHandler> &&metadata_fh)
    : index_logs_fh(std::move(index_logs_fh)),
      offsets_fh(std::move(offsets_fh)), metadata_fh(std::move(metadata_fh)) {}

UpdatesLoggerFilesManager::UpdatesLoggerFilesManager(const CacheArgs &args)
    : index_logs_fh(std::make_unique<FileRWHandler>(args.update_log_filename)),
      offsets_fh(std::make_unique<FileRWHandler>(args.update_log_filename +
                                                 ".offsets")),
      metadata_fh(std::make_unique<FileRWHandler>(args.update_log_filename +
                                                  ".meta")) {}
I_FileRWHandler &UpdatesLoggerFilesManager::get_index_logs_fh() {
  return *index_logs_fh;
}
I_FileRWHandler &UpdatesLoggerFilesManager::get_offsets_fh() {
  return *offsets_fh;
}
I_FileRWHandler &UpdatesLoggerFilesManager::get_metadata_fh() {
  return *metadata_fh;
}
} // namespace k2cache
