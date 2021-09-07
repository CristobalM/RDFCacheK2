//
// Created by cristobal on 06-09-21.
//
#include <filesystem>

#include "UpdatesLogger.hpp"

namespace fs = std::filesystem;

UpdatesLogger::UpdatesLogger(I_DataMerger &data_merger,
                             I_FileRWHandler &logs_file_handler,
                             I_FileRWHandler &predicates_offsets_file_handler)
    : data_merger(data_merger), logs_file_handler(logs_file_handler),
      predicates_offsets_file_handler(predicates_offsets_file_handler) {
  retrieve_offsets_map();
}

void UpdatesLogger::log(NaiveDynamicStringDictionary *added_resources,
                        std::vector<K2TreeUpdates> &k2tree_updates) {
  current_file_reader = nullptr;
  if (!current_file_writer) {
    current_file_writer =
        logs_file_handler.get_writer(std::ios::binary | std::ios::app);
  }
  // to indicate there is a resources dict
  write_u32(*current_file_writer, added_resources != nullptr);

  if (added_resources) {
    added_resources->serialize(*current_file_writer);
  }

  write_u32(*current_file_writer, k2tree_updates.size());
  for (auto &update : k2tree_updates) {
    register_update_offset(update.predicate_id, *current_file_writer);
    UPDATE_KIND update_kind;
    write_u64(*current_file_writer, update.predicate_id);
    if (update.k2tree_del && update.k2tree_add) {
      update_kind = BOTH_UPDATE;
    } else if (update.k2tree_add) {
      update_kind = INSERT_UPDATE;
    } else {
      update_kind = DELETE_UPDATE;
    }
    write_u32(*current_file_writer, update_kind);
    switch (update_kind) {
    case INSERT_UPDATE:
      update.k2tree_add->write_to_ostream(*current_file_writer);
      break;
    case DELETE_UPDATE:
      update.k2tree_del->write_to_ostream(*current_file_writer);
      break;
    case BOTH_UPDATE:
      update.k2tree_add->write_to_ostream(*current_file_writer);
      update.k2tree_del->write_to_ostream(*current_file_writer);
      break;
    }
  }
  current_file_writer->flush();
  dump_offsets_map();
}

void UpdatesLogger::recover(const std::vector<unsigned long> &predicates) {
  current_file_writer = nullptr; // close file if it's open
  recover_data(predicates);
}

void UpdatesLogger::recover_all() {
  current_file_writer = nullptr; // close file if it's open
  recover_all_data();
}

void UpdatesLogger::recover_all_data() {
  if (!logs_file_handler.exists())
    return;
  // auto ifs_logs = logs_file_handler.get_reader(std::ios::binary);
  if(!current_file_reader){
    current_file_reader = logs_file_handler.get_reader(std::ios::binary);
  }
  current_file_reader->seekg(0, std::ios::beg);
  while (*current_file_reader) {
    recover_single_update(*current_file_reader);
  }
}

void UpdatesLogger::recover_data(const std::vector<unsigned long> &predicates) {
  if (!logs_file_handler.exists())
    return;
  // auto ifs_logs = logs_file_handler.get_reader(std::ios::binary);
  if(!current_file_reader){
    current_file_reader = logs_file_handler.get_reader(std::ios::binary);
  }
  for (auto predicate_id : predicates) {
    const auto &offsets = offsets_map[predicate_id];
    for (auto offset : offsets) {
      current_file_reader->seekg(offset, std::ios::beg);
      recover_single_predicate_update(*current_file_reader);
    }
  }
}
void UpdatesLogger::recover_single_update(std::istream &ifs) {
  bool has_dict = read_u32(ifs);
  if (has_dict) {
    auto recovered_dict = NaiveDynamicStringDictionary::deserialize(ifs);
    data_merger.merge_with_extra_dict(recovered_dict);
  }
  auto updates_amount = static_cast<size_t>(read_u32(ifs));
  for (size_t i = 0; i < updates_amount; i++) {
    recover_single_predicate_update(ifs);
  }
}
void UpdatesLogger::retrieve_offsets_map() {
  if (!predicates_offsets_file_handler.exists())
    return;
  auto ifs = predicates_offsets_file_handler.get_reader(std::ios::binary);
  auto offsets_map_size = (int)read_u32(*ifs);
  for (int i = 0; i < offsets_map_size; i++) {
    auto predicate_id = read_u64(*ifs);
    auto offsets_size = (int)read_u32(*ifs);
    std::vector<long> offsets_retrieved;
    offsets_retrieved.reserve(offsets_size);
    for (int offset_i = 0; offset_i < offsets_size; offset_i++) {
      offsets_retrieved.push_back((long)read_u64(*ifs));
    }
    offsets_map[predicate_id] = std::move(offsets_retrieved);
  }
}
void UpdatesLogger::dump_offsets_map() {
  if (offsets_map.empty())
    return;
  {
    auto ofs_temp = predicates_offsets_file_handler.get_writer_temp(
        std::ios::binary | std::ios::trunc);
    write_u32(*ofs_temp, offsets_map.size());
    for (const auto &kv : offsets_map) {
      auto predicate_id = kv.first;
      const auto &offsets = kv.second;
      write_u64(*ofs_temp, predicate_id);
      write_u32(*ofs_temp, offsets.size());
      for (auto offset : offsets) {
        write_u64(*ofs_temp, (unsigned long)offset);
      }
    }
  }

  predicates_offsets_file_handler.commit_temp_writer();
}
void UpdatesLogger::recover_single_predicate_update(std::istream &ifs) {
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
void UpdatesLogger::register_update_offset(unsigned long predicate_id,
                                           std::ostream &ofs) {
  auto offset = ofs.tellp();
  auto it = offsets_map.find(predicate_id);
  if (it == offsets_map.end()) {
    offsets_map[predicate_id] = {offset};
    return;
  }
  it->second.push_back(offset);
}

void UpdatesLogger::recover_predicate(unsigned long predicate_id) {
  if (!logs_file_handler.exists())
    return;
  // auto ifs_logs = logs_file_handler.get_reader(std::ios::binary);
  current_file_writer = nullptr;
  if(!current_file_reader){
    current_file_reader = logs_file_handler.get_reader(std::ios::binary);
  }
  const auto &offsets = offsets_map[predicate_id];
  for (auto offset : offsets) {
    current_file_reader->seekg(offset, std::ios::beg);
    recover_single_predicate_update(*current_file_reader);
  }
}
