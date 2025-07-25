//
// Created by cristobal on 06-09-21.
//

#ifndef RDFCACHEK2_UPDATESLOGGERIMPL_HPP
#define RDFCACHEK2_UPDATESLOGGERIMPL_HPP

#include <cstdint>
#include <map>
#include <memory>
#include <ostream>
#include <vector>

#include "DataMerger.hpp"
#include "I_IOStream.hpp"
#include "I_IStream.hpp"
#include "I_OStream.hpp"
#include "K2TreeUpdates.hpp"
#include "UpdatesLogger.hpp"
#include "UpdatesLoggerFilesManager.hpp"
#include "k2tree/K2TreeMixed.hpp"

namespace k2cache {
class UpdatesLoggerImpl : public UpdatesLogger {
  std::unique_ptr<DataMerger> data_merger;
  UpdatesLoggerFilesManager fm;

  std::unique_ptr<I_OStream> current_file_writer;
  std::unique_ptr<I_IStream> current_file_reader;
  std::unique_ptr<I_IOStream> metadata_file_rw;

  using offsets_map_t =
      std::map<uint64_t, std::unique_ptr<std::vector<long>>>;

  offsets_map_t offsets_map;

  int total_updates;

public:
  UpdatesLoggerImpl(std::unique_ptr<DataMerger> &&data_merger,
                    UpdatesLoggerFilesManager &&files_manager);
  void recover_all() override;
  void log(std::vector<K2TreeUpdates> &k2tree_updates) override;

  void recover_predicate(uint64_t predicate_id) override;

  bool has_predicate_stored(uint64_t predicate_id) override;

  void clean_append_log() override;

  std::vector<uint64_t> get_predicates() override;

  void compact_logs() override;

  int logs_number() override;

  UpdatesLoggerFilesManager &get_fh_manager() override;

  void recover(const std::vector<uint64_t> &predicates);

private:
  static void log(std::vector<K2TreeUpdates> &k2tree_updates,
                  offsets_map_t &offsets, I_OStream &trees_writer);
  static void register_update_offset(offsets_map_t &offsets,
                                     uint64_t predicate_id,
                                     std::ostream &ofs);
  static void dump_offsets_map(UpdatesLoggerImpl::offsets_map_t &_offsets_map,
                               I_OStream &offsets_file);

  static void commit_total_updates(I_OStream &_metadata_file_rw,
                                   int _total_updates);

  struct PredicateUpdate {
    uint64_t predicate_id;
    std::unique_ptr<K2TreeMixed> add_update;
    std::unique_ptr<K2TreeMixed> del_update;
    void merge_with(PredicateUpdate &update);
  };

  void recover_data(const std::vector<uint64_t> &predicates);
  void recover_single_update(I_IStream &ifs);
  void retrieve_offsets_map();
  void dump_offsets_map();
  PredicateUpdate recover_single_predicate_update(I_IStream &ifs);
  void merge_update(PredicateUpdate &predicate_update);
  void register_update_offset(uint64_t predicate_id, std::ostream &ofs);
  void recover_all_data();
  int read_total_updates();
  void commit_total_updates();

  std::unique_ptr<UpdatesLoggerImpl::PredicateUpdate>
  compact_predicate(uint64_t predicate_id);
};
} // namespace k2cache

#endif // RDFCACHEK2_UPDATESLOGGERIMPL_HPP
