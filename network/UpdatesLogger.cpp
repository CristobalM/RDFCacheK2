//
// Created by cristobal on 06-09-21.
//
#include <filesystem>

#include "UpdatesLogger.hpp"

namespace fs = std::filesystem;

void UpdatesLogger::log(NaiveDynamicStringDictionary *added_resources,
                        std::vector<K2TreeUpdates> &k2tree_updates) {
  // to indicate there is a resources dict
  write_u32(*current_file, added_resources != nullptr);

  if (added_resources) {
    added_resources->serialize(*current_file);
  }

  write_u32(*current_file, k2tree_updates.size());
  for (auto &update : k2tree_updates) {
    UPDATE_KIND update_kind;
    write_u64(*current_file, update.predicate_id);
    if (update.k2tree_del && update.k2tree_add) {
      update_kind = BOTH_UPDATE;
    } else if (update.k2tree_add) {
      update_kind = INSERT_UPDATE;
    } else {
      update_kind = DELETE_UPDATE;
    }
    write_u32(*current_file, update_kind);
    switch (update_kind) {
    case INSERT_UPDATE:
      update.k2tree_add->write_to_ostream(*current_file);
      break;
    case DELETE_UPDATE:
      update.k2tree_del->write_to_ostream(*current_file);
      break;
    case BOTH_UPDATE:
      update.k2tree_add->write_to_ostream(*current_file);
      update.k2tree_del->write_to_ostream(*current_file);
      break;
    }
  }
  current_file->flush();
}
UpdatesLogger::UpdatesLogger(I_DataMerger &data_merger,
                             I_FileRWHandler &file_handler)
    : data_merger(data_merger), file_handler(file_handler) {}
void UpdatesLogger::recover() {
  recover_data();
  current_file = std::make_unique<std::ofstream>(
      LOGS_FILENAME, std::ios::out | std::ios::binary | std::ios::app);
}
void UpdatesLogger::recover_data() {
  if (!fs::exists(LOGS_FILENAME))
    return;
  std::ifstream ifs(LOGS_FILENAME, std::ios::in | std::ios::binary);
  while (ifs) {
    recover_single_update(ifs);
  }
}
void UpdatesLogger::recover_single_update(std::ifstream &ifs) {
  bool has_dict = read_u32(ifs);
  if (has_dict) {
    auto recovered_dict = NaiveDynamicStringDictionary::deserialize(ifs);
    data_merger.merge_with_extra_dict(recovered_dict);
  }
  auto updates_amount = static_cast<size_t>(read_u32(ifs));
  for (size_t i = 0; i < updates_amount; i++) {
    auto predicate_id = static_cast<unsigned long>(read_u64(ifs));
    auto update_kind = static_cast<UPDATE_KIND>(read_u32(ifs));
    std::unique_ptr<K2TreeMixed> added_triples{};
    std::unique_ptr<K2TreeMixed> removed_triples{};
    switch (update_kind) {
    case INSERT_UPDATE:
      added_triples =
          std::make_unique<K2TreeMixed>(K2TreeMixed::read_from_istream(ifs));
      break;
    case DELETE_UPDATE:
      removed_triples =
          std::make_unique<K2TreeMixed>(K2TreeMixed::read_from_istream(ifs));
      break;
    case BOTH_UPDATE:
      added_triples =
          std::make_unique<K2TreeMixed>(K2TreeMixed::read_from_istream(ifs));
      removed_triples =
          std::make_unique<K2TreeMixed>(K2TreeMixed::read_from_istream(ifs));
      break;
    }
    if (added_triples)
      data_merger.merge_add_tree(predicate_id, *added_triples);
    if (removed_triples)
      data_merger.merge_delete_tree(predicate_id, *removed_triples);
  }
}
