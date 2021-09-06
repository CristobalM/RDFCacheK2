//
// Created by cristobal on 06-09-21.
//

#ifndef RDFCACHEK2_UPDATESLOGGER_HPP
#define RDFCACHEK2_UPDATESLOGGER_HPP

#include "I_DataMerger.hpp"
#include "K2TreeUpdates.hpp"
#include <Cache.hpp>
#include <NaiveDynamicStringDictionary.hpp>
#include <fstream>

class UpdatesLogger {

  static constexpr auto *LOGS_FILENAME = ".updates-logs.bin";
  std::unique_ptr<std::ofstream> current_file;
  I_DataMerger &data_merger;
  I_FileRWHandler &file_handler;

  enum UPDATE_KIND{
    INSERT_UPDATE=0,
    DELETE_UPDATE,
    BOTH_UPDATE
  };

public:
  UpdatesLogger(I_DataMerger &cache, I_FileRWHandler &file_handler);
  void recover();
  void log(NaiveDynamicStringDictionary *added_resources,
           std::vector<K2TreeUpdates> &k2tree_updates);
  void recover_data();
  void recover_single_update(std::ifstream &ifs);
};

#endif // RDFCACHEK2_UPDATESLOGGER_HPP
