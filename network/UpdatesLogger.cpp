//
// Created by cristobal on 06-09-21.
//
#include <filesystem>

#include "UpdatesLogger.hpp"

namespace fs = std::filesystem;

UpdatesLogger::UpdatesLogger(I_DataMerger &data_merger,
                             I_FileRWHandler &logs_file_handler,
                             I_FileRWHandler &predicates_offsets_file_handler,
                             I_FileRWHandler &metadata_rw_handler)
    : data_merger(data_merger), logs_file_handler(logs_file_handler),
      predicates_offsets_file_handler(predicates_offsets_file_handler),
      metadata_rw_handler(metadata_rw_handler),
      metadata_file_rw(
          metadata_rw_handler.get_reader_writer(std::ios::binary)) {
  retrieve_offsets_map();

  if (metadata_rw_handler.exists()) {
    // auto reader = logs_file_handler.get_reader(std::ios::binary);
    // total_updates = (int)read_u32(rw_metadata->get_stream());
    total_updates = read_total_updates();
  }
  // TODO: unused for now, might want to remove it
  (void)(this->metadata_rw_handler);
}

void UpdatesLogger::log(std::vector<K2TreeUpdates> &k2tree_updates) {
  current_file_reader = nullptr;
  if (!current_file_writer) {
    current_file_writer =
        logs_file_handler.get_writer(std::ios::binary | std::ios::app);
  }

  auto &writer_real = current_file_writer->get_stream();

  write_u32(writer_real, k2tree_updates.size());
  for (auto &update : k2tree_updates) {
    register_update_offset(update.predicate_id, writer_real);
    UPDATE_KIND update_kind;
    write_u64(writer_real, update.predicate_id);
    if (update.k2tree_del && update.k2tree_add) {
      update_kind = BOTH_UPDATE;
    } else if (update.k2tree_add) {
      update_kind = INSERT_UPDATE;
    } else {
      update_kind = DELETE_UPDATE;
    }
    write_u32(writer_real, update_kind);
    switch (update_kind) {
    case INSERT_UPDATE:
      update.k2tree_add->write_to_ostream(writer_real);
      break;
    case DELETE_UPDATE:
      update.k2tree_del->write_to_ostream(writer_real);
      break;
    case BOTH_UPDATE:
      update.k2tree_add->write_to_ostream(writer_real);
      update.k2tree_del->write_to_ostream(writer_real);
      break;
    }
  }
  current_file_writer->flush();
  dump_offsets_map();
  data_merger.merge_update(k2tree_updates);
  total_updates++;
  commit_total_updates();
}

void UpdatesLogger::recover(const std::vector<unsigned long> &predicates) {
  if (current_file_writer) {
    current_file_writer->flush();
    current_file_writer = nullptr; // close file if it's open
  }
  recover_data(predicates);
}

void UpdatesLogger::recover_all() {
  if (current_file_writer) {
    current_file_writer->flush();
    current_file_writer = nullptr; // close file if it's open
  }
  recover_all_data();
}

void UpdatesLogger::recover_all_data() {
  if (!logs_file_handler.exists())
    return;
  // auto ifs_logs = logs_file_handler.get_reader(std::ios::binary);
  if (!current_file_reader) {
    current_file_reader = logs_file_handler.get_reader(std::ios::binary);
  }
  for (int i = 0; i < total_updates; i++) {
    recover_single_update(*current_file_reader);
  }
}

void UpdatesLogger::recover_data(const std::vector<unsigned long> &predicates) {
  if (!logs_file_handler.exists())
    return;
  // auto ifs_logs = logs_file_handler.get_reader(std::ios::binary);
  if (!current_file_reader) {
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
void UpdatesLogger::recover_single_update(I_IStream &ifs) {
  auto updates_amount = static_cast<size_t>(read_u32(ifs.get_stream()));
  for (size_t i = 0; i < updates_amount; i++) {
    recover_single_predicate_update(ifs);
  }
}
void UpdatesLogger::retrieve_offsets_map() {
  if (!predicates_offsets_file_handler.exists())
    return;
  auto ifs = predicates_offsets_file_handler.get_reader(std::ios::binary);
  auto &ifs_real = ifs->get_stream();
  auto offsets_map_size = (int)read_u32(ifs_real);
  for (int i = 0; i < offsets_map_size; i++) {
    auto predicate_id = read_u64(ifs_real);
    auto offsets_size = (int)read_u32(ifs_real);
    std::vector<long> offsets_retrieved;
    offsets_retrieved.reserve(offsets_size);
    for (int offset_i = 0; offset_i < offsets_size; offset_i++) {
      offsets_retrieved.push_back((long)read_u64(ifs_real));
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
    auto &ofs_temp_real = ofs_temp->get_stream();
    write_u32(ofs_temp_real, offsets_map.size());
    for (const auto &kv : offsets_map) {
      auto predicate_id = kv.first;
      const auto &offsets = kv.second;
      write_u64(ofs_temp_real, predicate_id);
      write_u32(ofs_temp_real, offsets.size());
      for (auto offset : offsets) {
        write_u64(ofs_temp_real, (unsigned long)offset);
      }
    }
  }

  predicates_offsets_file_handler.commit_temp_writer();
}
void UpdatesLogger::recover_single_predicate_update(I_IStream &ifs) {
  auto &ifs_real = ifs.get_stream();
  auto predicate_id = static_cast<unsigned long>(read_u64(ifs_real));
  auto update_kind = static_cast<UPDATE_KIND>(read_u32(ifs_real));
  std::unique_ptr<K2TreeMixed> added_triples{};
  std::unique_ptr<K2TreeMixed> removed_triples{};
  switch (update_kind) {
  case INSERT_UPDATE:
    added_triples =
        std::make_unique<K2TreeMixed>(K2TreeMixed::read_from_istream(ifs_real));
    break;
  case DELETE_UPDATE:
    removed_triples =
        std::make_unique<K2TreeMixed>(K2TreeMixed::read_from_istream(ifs_real));
    break;
  case BOTH_UPDATE:
    added_triples =
        std::make_unique<K2TreeMixed>(K2TreeMixed::read_from_istream(ifs_real));
    removed_triples =
        std::make_unique<K2TreeMixed>(K2TreeMixed::read_from_istream(ifs_real));
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
  if (current_file_writer) {
    current_file_writer->flush();
    current_file_writer = nullptr;
  }
  if (!current_file_reader) {
    current_file_reader = logs_file_handler.get_reader(std::ios::binary);
  }
  const auto &offsets = offsets_map[predicate_id];
  for (auto offset : offsets) {
    current_file_reader->seekg(offset, std::ios::beg);
    recover_single_predicate_update(*current_file_reader);
  }
}
bool UpdatesLogger::has_predicate_stored(uint64_t predicate_id) {
  return offsets_map.find(predicate_id) != offsets_map.end();
}
int UpdatesLogger::read_total_updates() {
  auto last_offset = metadata_file_rw->tellg();
  metadata_file_rw->seekg(0, std::ios::beg);
  auto result = (int)read_u32(metadata_file_rw->get_stream());
  if (last_offset != 0) {
    metadata_file_rw->seekg(last_offset, std::ios::beg);
  }
  return result;
}
void UpdatesLogger::commit_total_updates() {
  auto last_offset = metadata_file_rw->tellp();
  metadata_file_rw->seekp(0, std::ios::beg);
  write_u32(metadata_file_rw->get_stream(), total_updates);
  if (last_offset != 0) {
    metadata_file_rw->seekp(last_offset, std::ios::beg);
  }
  metadata_file_rw->flush();
}
