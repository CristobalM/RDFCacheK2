//
// Created by cristobal on 06-09-21.
//

#ifndef RDFCACHEK2_UPDATESLOGGER_HPP
#define RDFCACHEK2_UPDATESLOGGER_HPP

#include <map>
#include <ostream>

#include "I_DataMerger.hpp"
#include "I_FileRWHandler.hpp"
#include "I_IOStream.hpp"
#include "K2TreeUpdates.hpp"
#include <I_IStream.hpp>
#include <I_OStream.hpp>

class UpdatesLogger {

  static constexpr auto *LOGS_FILENAME = ".updates-logs.bin";
  std::unique_ptr<I_OStream> current_file_writer;
  std::unique_ptr<I_IStream> current_file_reader;
  I_DataMerger &data_merger;
  I_FileRWHandler &logs_file_handler;
  I_FileRWHandler &predicates_offsets_file_handler;
  I_FileRWHandler &metadata_rw_handler;
  std::unique_ptr<I_IOStream> metadata_file_rw;

  std::map<unsigned long, std::vector<long>> offsets_map;

  int total_updates;

public:
  UpdatesLogger(I_DataMerger &data_merger, I_FileRWHandler &logs_file_handler,
                I_FileRWHandler &predicates_offsets_file_handler,
                I_FileRWHandler &metadata_rw_handler);
  void recover(const std::vector<unsigned long> &predicates);
  void recover_all();
  void log(std::vector<K2TreeUpdates> &k2tree_updates);

  void recover_predicate(unsigned long predicate_id);

  bool has_predicate_stored(uint64_t predicate_id);

  void clean_append_log();

private:
  void recover_data(const std::vector<unsigned long> &predicates);
  void recover_single_update(I_IStream &ifs);
  void retrieve_offsets_map();
  void dump_offsets_map();
  void recover_single_predicate_update(I_IStream &ifs);
  void register_update_offset(unsigned long predicate_id, std::ostream &ofs);
  void recover_all_data();
  int read_total_updates();
  void commit_total_updates();
};

#endif // RDFCACHEK2_UPDATESLOGGER_HPP
