//
// Created by cristobal on 06-09-21.
//
#include <filesystem>

#include "UpdatesLogger.hpp"

namespace fs = std::filesystem;

UpdatesLogger::UpdatesLogger(I_DataMerger &data_merger,
                             UpdatesLoggerFilesManager &&files_manager)
    : data_merger(data_merger), fm(std::move(files_manager)),
      metadata_file_rw(
          fm.get_metadata_fh().get_reader_writer(std::ios::binary)) {
  retrieve_offsets_map();
  if (fm.get_metadata_fh().exists())
    total_updates = read_total_updates();
}

void UpdatesLogger::log(std::vector<K2TreeUpdates> &k2tree_updates,
                        offsets_map_t &offsets, I_OStream &trees_writer) {
  auto &writer_real = trees_writer.get_ostream();

  write_u32(writer_real, k2tree_updates.size());
  for (auto &update : k2tree_updates) {
    auto update_kind = update.get_kind();
    if (update_kind == K2TreeUpdates::NO_UPDATE)
      continue;

    register_update_offset(offsets, update.predicate_id, writer_real);
    write_u64(writer_real, update.predicate_id);
    write_u32(writer_real, update_kind);

    switch (update_kind) {
    case K2TreeUpdates::INSERT_UPDATE:
      update.k2tree_add->write_to_ostream(writer_real);
      break;
    case K2TreeUpdates::DELETE_UPDATE:
      update.k2tree_del->write_to_ostream(writer_real);
      break;
    case K2TreeUpdates::BOTH_UPDATE:
      update.k2tree_add->write_to_ostream(writer_real);
      update.k2tree_del->write_to_ostream(writer_real);
      break;
    case K2TreeUpdates::NO_UPDATE:
      break;
    }
  }
}

void UpdatesLogger::log(std::vector<K2TreeUpdates> &k2tree_updates) {
  current_file_reader = nullptr;
  if (!current_file_writer) {
    current_file_writer = fm.get_index_logs_fh().get_writer(std::ios::binary);
  }

  int starting_point = current_file_writer->get_ostream().tellp();
  (void)starting_point;

  log(k2tree_updates, offsets_map, *current_file_writer);
  current_file_writer->flush();

  {
    // offsets sync
    auto ofs_temp = fm.get_offsets_fh().get_writer_temp(
        std::ios::binary | std::ios::trunc);
    dump_offsets_map(offsets_map, *ofs_temp);
    ofs_temp->flush();
    fm.get_offsets_fh().commit_temp_writer();
  }

  {
    // updates sync
    data_merger.merge_update(k2tree_updates);
    total_updates++;
    commit_total_updates();
  }
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
  if (!fm.get_index_logs_fh().exists())
    return;
  // auto ifs_logs = logs_file_handler.get_reader(std::ios::binary);
  if (!current_file_reader) {
    current_file_reader = fm.get_index_logs_fh().get_reader(std::ios::binary);
  }
  for (int i = 0; i < total_updates; i++) {
    recover_single_update(*current_file_reader);
  }
}

void UpdatesLogger::recover_data(const std::vector<unsigned long> &predicates) {
  if (!fm.get_index_logs_fh().exists())
    return;
  // auto ifs_logs = logs_file_handler.get_reader(std::ios::binary);
  if (!current_file_reader) {
    current_file_reader = fm.get_index_logs_fh().get_reader(std::ios::binary);
  }
  for (auto predicate_id : predicates) {
    const auto &offsets = offsets_map[predicate_id];
    for (auto offset : *offsets) {
      current_file_reader->seekg(offset, std::ios::beg);
      auto updates = recover_single_predicate_update(*current_file_reader);
      merge_update(updates);
    }
  }
}

void UpdatesLogger::recover_single_update(I_IStream &ifs) {
  auto updates_amount = static_cast<size_t>(read_u32(ifs.get_istream()));
  for (size_t i = 0; i < updates_amount; i++) {
    auto updates = recover_single_predicate_update(ifs);
    merge_update(updates);
  }
}

void UpdatesLogger::retrieve_offsets_map() {
  if (!fm.get_offsets_fh().exists())
    return;
  auto ifs = fm.get_offsets_fh().get_reader(std::ios::binary);
  auto &ifs_real = ifs->get_istream();
  auto offsets_map_size = (int)read_u32(ifs_real);
  for (int i = 0; i < offsets_map_size; i++) {
    auto predicate_id = read_u64(ifs_real);
    auto offsets_size = (int)read_u32(ifs_real);
    auto offsets_retrieved = std::make_unique<std::vector<long>>();
    offsets_retrieved->reserve(offsets_size);
    for (int offset_i = 0; offset_i < offsets_size; offset_i++) {
      offsets_retrieved->push_back((long)read_u64(ifs_real));
    }
    offsets_map[predicate_id] = std::move(offsets_retrieved);
  }
}

void UpdatesLogger::dump_offsets_map(UpdatesLogger::offsets_map_t &_offsets_map,
                                     I_OStream &offsets_file) {
  if (_offsets_map.empty())
    return;

  auto &ofs_temp_real = offsets_file.get_ostream();
  write_u32(ofs_temp_real, _offsets_map.size());
  for (const auto &kv : _offsets_map) {
    auto predicate_id = kv.first;
    const auto &offsets = kv.second;
    write_u64(ofs_temp_real, predicate_id);
    write_u32(ofs_temp_real, offsets->size());
    for (auto offset : *offsets) {
      write_u64(ofs_temp_real, (unsigned long)offset);
    }
  }
}

void UpdatesLogger::dump_offsets_map() {
  if (offsets_map.empty())
    return;
  {
    auto ofs_temp = fm.get_offsets_fh().get_writer_temp(
        std::ios::binary | std::ios::trunc);
    dump_offsets_map(offsets_map, *ofs_temp);
    ofs_temp->flush();
    fm.get_offsets_fh().commit_temp_writer();
  }
}

UpdatesLogger::PredicateUpdate
UpdatesLogger::recover_single_predicate_update(I_IStream &ifs) {
  auto &ifs_real = ifs.get_istream();
  auto predicate_id = static_cast<unsigned long>(read_u64(ifs_real));
  auto update_kind =
      static_cast<K2TreeUpdates::UPDATE_KIND>(read_u32(ifs_real));
  std::unique_ptr<K2TreeMixed> added_triples{};
  std::unique_ptr<K2TreeMixed> removed_triples{};
  switch (update_kind) {
  case K2TreeUpdates::INSERT_UPDATE:
    added_triples =
        std::make_unique<K2TreeMixed>(K2TreeMixed::read_from_istream(ifs_real));
    break;
  case K2TreeUpdates::DELETE_UPDATE:
    removed_triples =
        std::make_unique<K2TreeMixed>(K2TreeMixed::read_from_istream(ifs_real));
    break;
  case K2TreeUpdates::BOTH_UPDATE:
    added_triples =
        std::make_unique<K2TreeMixed>(K2TreeMixed::read_from_istream(ifs_real));
    removed_triples =
        std::make_unique<K2TreeMixed>(K2TreeMixed::read_from_istream(ifs_real));
    break;
  case K2TreeUpdates::NO_UPDATE:
    break;
  }
  PredicateUpdate out;
  out.predicate_id = predicate_id;
  out.add_update = std::move(added_triples);
  out.del_update = std::move(removed_triples);
  return out;
}

/**
 * Register the current offset of ofs output stream to an offsets_map
 * which is keyed by predicate_id, the internal map ultimately contains
 * mappings from predicate_ids to offsets to where the data for that predicate
 * is stored inside a file
 *
 * @param predicate_id Will act as key for the mapping
 * @param ofs The output stream from which to get the current offset
 */
void UpdatesLogger::register_update_offset(unsigned long predicate_id,
                                           std::ostream &ofs) {
  register_update_offset(offsets_map, predicate_id, ofs);
}

void UpdatesLogger::register_update_offset(
    UpdatesLogger::offsets_map_t &offsets, unsigned long predicate_id,
    std::ostream &ofs) {
  auto offset = ofs.tellp();
  auto it = offsets.find(predicate_id);
  if (it == offsets.end()) {
    auto vec = std::make_unique<std::vector<long>>();
    vec->push_back(offset);
    offsets[predicate_id] = std::move(vec);
    return;
  }
  it->second->push_back(offset);
}

/**
 * Recovers a predicate's data from file log into memory
 * @param predicate_id
 */
void UpdatesLogger::recover_predicate(unsigned long predicate_id) {
  if (!fm.get_index_logs_fh().exists())
    return;
  if (current_file_writer) {
    current_file_writer->flush();
    current_file_writer = nullptr;
  }
  if (!current_file_reader) {
    current_file_reader = fm.get_index_logs_fh().get_reader(std::ios::binary);
  }
  const auto &offsets = offsets_map[predicate_id];
  for (auto offset : *offsets) {
    current_file_reader->seekg(offset, std::ios::beg);
    auto updates = recover_single_predicate_update(*current_file_reader);
    merge_update(updates);
  }
}

bool UpdatesLogger::has_predicate_stored(uint64_t predicate_id) {
  return offsets_map.find(predicate_id) != offsets_map.end();
}

int UpdatesLogger::read_total_updates() {
  auto last_offset = metadata_file_rw->tellg();
  metadata_file_rw->seekg(0, std::ios::beg);
  auto result = (int)read_u32(metadata_file_rw->get_istream());
  if (last_offset != 0) {
    metadata_file_rw->seekg(last_offset, std::ios::beg);
  }
  return result;
}

void UpdatesLogger::commit_total_updates() {
  commit_total_updates(*metadata_file_rw, total_updates);
  //  auto last_offset = metadata_file_rw->tellp();
  //  metadata_file_rw->seekp(0, std::ios::beg);
  //  write_u32(metadata_file_rw->get_ostream(), total_updates);
  //  if (last_offset != 0) {
  //    metadata_file_rw->seekp(last_offset, std::ios::beg);
  //  }
  //  metadata_file_rw->flush();
}

void UpdatesLogger::commit_total_updates(I_OStream &_metadata_file_rw,
                                         int _total_updates) {
  auto last_offset = _metadata_file_rw.tellp();
  _metadata_file_rw.seekp(0, std::ios::beg);
  write_u32(_metadata_file_rw.get_ostream(), _total_updates);
  if (last_offset != 0) {
    _metadata_file_rw.seekp(last_offset, std::ios::beg);
  }
  _metadata_file_rw.flush();
}

void UpdatesLogger::clean_append_log() {
  if (current_file_writer)
    current_file_writer = nullptr;
  if (current_file_reader)
    current_file_reader = nullptr;
  if (metadata_file_rw)
    metadata_file_rw = nullptr;
  offsets_map.clear();
  fm.get_index_logs_fh().clean();
  fm.get_metadata_fh().clean();
  fm.get_offsets_fh().clean();
  metadata_file_rw = fm.get_metadata_fh().get_reader_writer(std::ios::binary);
  total_updates = 0;
}

std::unique_ptr<UpdatesLogger::PredicateUpdate>
UpdatesLogger::compact_predicate(unsigned long predicate_id) {
  auto it = offsets_map.find(predicate_id);
  if (it == offsets_map.end())
    return nullptr;
  if (!current_file_reader) {
    current_file_reader = fm.get_index_logs_fh().get_reader(std::ios::binary);
  }

  PredicateUpdate merged_updates;
  bool first = true;
  for (auto offset : *it->second) {
    current_file_reader->seekg(offset);
    auto updates = recover_single_predicate_update(*current_file_reader);
    if (updates.predicate_id != predicate_id) {
      throw std::runtime_error(
          ""
          "predicate id doesn't match the expected: actual = " +
          std::to_string(updates.predicate_id) +
          ", expected = " + std::to_string(predicate_id));
    }

    if (first) {
      merged_updates = std::move(updates);
      first = false;
    } else {
      merged_updates.merge_with(updates);
    }
  }
  return std::make_unique<PredicateUpdate>(std::move(merged_updates));
}

void UpdatesLogger::compact_logs() {
  current_file_reader = nullptr;
  current_file_writer = nullptr;

  auto tmp_writer = fm.get_index_logs_fh().get_writer_temp(std::ios::binary);
  offsets_map_t new_offsets;
  for (const auto &p : offsets_map) {
    auto compacted = compact_predicate(p.first);
    std::vector<K2TreeUpdates> updates = {
        K2TreeUpdates(compacted->predicate_id, compacted->add_update.get(),
                      compacted->del_update.get())};
    log(updates, new_offsets, *tmp_writer);
  }

  current_file_reader = nullptr;
  current_file_writer = nullptr;

  {
    // offsets sync
    auto offsets_writer =
        fm.get_offsets_fh().get_writer_temp(std::ios::binary);
    dump_offsets_map(new_offsets, *offsets_writer);
    offsets_writer->flush();
    fm.get_offsets_fh().commit_temp_writer();
    offsets_map = std::move(new_offsets);
  }

  {
    // metadata sync
    auto metadata_writer =
        fm.get_metadata_fh().get_writer_temp(std::ios::binary);
    commit_total_updates(*metadata_writer, (int)offsets_map.size());
    metadata_writer->flush();
    fm.get_metadata_fh().commit_temp_writer();
    total_updates = (int)offsets_map.size();
  }

  {
    // trees sync
    tmp_writer->flush();
    fm.get_index_logs_fh().commit_temp_writer();
  }
}

void UpdatesLogger::merge_update(
    UpdatesLogger::PredicateUpdate &predicate_update) {
  if (predicate_update.add_update)
    data_merger.merge_add_tree(predicate_update.predicate_id,
                               *predicate_update.add_update);
  if (predicate_update.del_update)
    data_merger.merge_delete_tree(predicate_update.predicate_id,
                                  *predicate_update.del_update);
}

int UpdatesLogger::logs_number() { return total_updates; }

std::vector<unsigned long> UpdatesLogger::get_predicates() {
  std::vector<unsigned long> out;
  out.reserve(offsets_map.size());
  for (auto &it : offsets_map) {
    out.push_back(it.first);
  }
  return out;
}
UpdatesLoggerFilesManager &UpdatesLogger::get_fh_manager() {
  return fm;
}

void UpdatesLogger::PredicateUpdate::merge_with(
    UpdatesLogger::PredicateUpdate &update) {
  if (!update.add_update && !update.del_update)
    return;

  std::unique_ptr<K2QStateWrapper> add_stw{};
  std::unique_ptr<K2QStateWrapper> del_stw{};

  if (add_update)
    add_stw = std::make_unique<K2QStateWrapper>(add_update->create_k2qw());
  if (del_update)
    del_stw = std::make_unique<K2QStateWrapper>(del_update->create_k2qw());

  if (update.add_update) {
    auto add_scanner = update.add_update->create_full_scanner();
    while (add_scanner->has_next()) {
      auto p = add_scanner->next();
      if (del_update)
        del_update->remove(p.first, p.second, *del_stw);
      if (add_update)
        add_update->insert(p.first, p.second, *add_stw);
    }
  }
  if (update.del_update) {
    auto del_scanner = update.del_update->create_full_scanner();
    while (del_scanner->has_next()) {
      auto p = del_scanner->next();
      if (add_update)
        add_update->remove(p.first, p.second, *add_stw);
      if (del_update)
        del_update->insert(p.first, p.second, *del_stw);
    }
  }
}
