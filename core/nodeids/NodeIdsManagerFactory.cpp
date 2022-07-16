//
// Created by cristobal on 26-06-22.
//

#include "NodeIdsManagerFactory.hpp"
#include "FileRWHandler.hpp"
#include "NodeIdsManagerImpl.hpp"

namespace k2cache {
std::unique_ptr<NodeIdsManager>
NodeIdsManagerFactory::create(const k2cache::CacheArgs &args) {
  return create(std::make_unique<FileRWHandler>(args.node_ids_filename),
                std::make_unique<FileRWHandler>(args.mapped_node_ids_filename),
                std::make_unique<FileRWHandler>(args.node_ids_logs_filename),
                std::make_unique<FileRWHandler>(args.node_ids_logs_filename +
                                                ".counter"));
}

std::unique_ptr<NodeIdsManager> NodeIdsManagerFactory::create(
    std::unique_ptr<I_FileRWHandler> &&plain_ni_fh,
    std::unique_ptr<I_FileRWHandler> &&mapped_ni_fh,
    std::unique_ptr<I_FileRWHandler> &&logs_fh,
    std::unique_ptr<I_FileRWHandler> &&logs_counter_fh) {
  return std::make_unique<NodeIdsManagerImpl>(
      std::move(plain_ni_fh), std::move(mapped_ni_fh), std::move(logs_fh),
      std::move(logs_counter_fh));
}

} // namespace k2cache
