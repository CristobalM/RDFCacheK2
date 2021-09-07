//
// Created by cristobal on 06-09-21.
//

#ifndef RDFCACHEK2_UPDATESLOGGER_HPP
#define RDFCACHEK2_UPDATESLOGGER_HPP

#include <ostream>

#include <Cache.hpp>
#include <NaiveDynamicStringDictionary.hpp>
#include "I_DataMerger.hpp"
#include "I_FileRWHandler.hpp"
#include "K2TreeUpdates.hpp"


class UpdatesLogger {

  static constexpr auto *LOGS_FILENAME = ".updates-logs.bin";
  std::unique_ptr<std::ostream> current_file_writer;
  std::unique_ptr<std::istream> current_file_reader;
  I_DataMerger &data_merger;
  I_FileRWHandler &logs_file_handler;
  I_FileRWHandler &predicates_offsets_file_handler;
  enum UPDATE_KIND{
    INSERT_UPDATE=0,
    DELETE_UPDATE,
    BOTH_UPDATE
  };

  std::map<unsigned long, std::vector<long>> offsets_map;

public:
  UpdatesLogger(I_DataMerger &data_merger, I_FileRWHandler &logs_file_handler,
                I_FileRWHandler &predicates_offsets_file_handler);
  void recover(const std::vector<unsigned long> &predicates);
  void recover_all();
  void log(NaiveDynamicStringDictionary *added_resources,
           std::vector<K2TreeUpdates> &k2tree_updates);

  void recover_predicate(unsigned long predicate_id);

private:
  void recover_data(const std::vector<unsigned long> &predicates);
  void recover_single_update(std::istream &ifs);
  void retrieve_offsets_map();
  void dump_offsets_map();
  void recover_single_predicate_update(std::istream &ifs);
  void register_update_offset(unsigned long predicate_id, std::ostream &ofs);
  void recover_all_data();
};

#endif // RDFCACHEK2_UPDATESLOGGER_HPP
