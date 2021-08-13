//
// Created by cristobal on 25-07-21.
//

#ifndef RDFCACHEK2_EXTERNALSORTEDFILEHANDLER_HPP
#define RDFCACHEK2_EXTERNALSORTEDFILEHANDLER_HPP

#include "RowSortConnector.hpp"
#include <filesystem>
#include <list>
#include <memory>
#include <query_processing/iterators/QueryIterator.hpp>
#include <vector>

namespace fs = std::filesystem;

class ExternalSortedFileHandler {
  std::shared_ptr<QueryIterator> input_it;
  const proto_msg::OrderNode order_node;
  size_t max_in_memory_size;
  TimeControl &time_control;
  EvalData eval_data;
  bool remove_duplicates;

  int current_pos_initial_data;
  bool disk_operations;
  std::vector<RowSortConnector> initial_data;
  RowSortConnector::Comparator comparator;

  std::set<std::string> active_files;
  std::unique_ptr<std::ifstream> sorted_file_is;
  std::vector<char> buffer;

  std::vector<unsigned long> next_value;
  bool next_available;

public:
  ExternalSortedFileHandler(std::shared_ptr<QueryIterator> input_it,
                            proto_msg::OrderNode order_node,
                            size_t max_in_memory_size,
                            TimeControl &time_control, EvalData eval_data,
                            bool remove_duplicates);

  ~ExternalSortedFileHandler();
  bool has_next();
  std::vector<unsigned long> next();
  void perform_disk_operations();
  void fall_back_to_memory_operations_only();
  bool has_next_wdisk();
  bool has_next_mem();
  std::vector<unsigned long> next_wdisk();
  std::vector<unsigned long> next_wmem();
  void dump_data_to_file(fs::path &path);
  void reset_iterator();
};

#endif // RDFCACHEK2_EXTERNALSORTEDFILEHANDLER_HPP
