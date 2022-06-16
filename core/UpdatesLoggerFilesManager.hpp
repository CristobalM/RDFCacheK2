//
// Created by cristobal on 11-06-22.
//

#ifndef RDFCACHEK2_UPDATESLOGGERFILESMANAGER_HPP
#define RDFCACHEK2_UPDATESLOGGERFILESMANAGER_HPP

#include <memory>

#include "CacheArgs.hpp"
#include "I_FileRWHandler.hpp"

class UpdatesLoggerFilesManager {
  std::unique_ptr<I_FileRWHandler> index_logs_fh;
  std::unique_ptr<I_FileRWHandler> offsets_fh;
  std::unique_ptr<I_FileRWHandler> metadata_fh;

public:
  UpdatesLoggerFilesManager(std::unique_ptr<I_FileRWHandler> &&index_logs_fh,
                            std::unique_ptr<I_FileRWHandler> &&offsets_fh,
                            std::unique_ptr<I_FileRWHandler> &&metadata_fh);

  explicit UpdatesLoggerFilesManager(const CacheArgs &args);

  I_FileRWHandler &get_index_logs_fh();
  I_FileRWHandler &get_offsets_fh();
  I_FileRWHandler &get_metadata_fh();
};

#endif // RDFCACHEK2_UPDATESLOGGERFILESMANAGER_HPP
