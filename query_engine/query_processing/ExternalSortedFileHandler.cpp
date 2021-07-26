//
// Created by cristobal on 25-07-21.
//

#include "ExternalSortedFileHandler.hpp"

#include <external_sort.hpp>
#include <filesystem>
#include <introsort.hpp>
#include <query_processing/utility/UuidGenerator.hpp>

namespace fs = std::filesystem;

bool ExternalSortedFileHandler::has_next() {
  if (disk_operations)
    return has_next_wdisk();
  return has_next_mem();
}
ExternalSortedFileHandler::ExternalSortedFileHandler(
    std::shared_ptr<ResultTableIterator> input_it,
    proto_msg::OrderNode order_node, size_t max_in_memory_size,
    TimeControl &time_control, EvalData eval_data, bool remove_duplicates)
    : input_it(std::move(input_it)), order_node(std::move(order_node)),
      max_in_memory_size(max_in_memory_size), time_control(time_control),
      eval_data(std::move(eval_data)), remove_duplicates(remove_duplicates),
      current_pos_initial_data(0), disk_operations(false),
      comparator(this->eval_data, this->order_node), sorted_file_is(nullptr),
      next_available(false) {
  size_t accumulated_size = 0;
  while (this->input_it->has_next()) {
    auto curr = this->input_it->next();
    if (!time_control.tick())
      return;
    initial_data.emplace_back(std::move(curr));
    accumulated_size += initial_data[initial_data.size() - 1].size();
    if (accumulated_size > this->max_in_memory_size)
      break;
  }
  if (this->input_it->has_next())
    perform_disk_operations();
  else
    fall_back_to_memory_operations_only();
}
std::vector<unsigned long> ExternalSortedFileHandler::next() {
  if (disk_operations)
    return next_wdisk();
  return next_wmem();
}

void ExternalSortedFileHandler::fall_back_to_memory_operations_only() {
  disk_operations = false;
  std::cout << "external sort falled back to memory sort" << std::endl;
  ExternalSort::IntroSort<RowSortConnector, TimeControl>::sort(
      initial_data, comparator, time_control);
}

void ExternalSortedFileHandler::perform_disk_operations() {
  disk_operations = true;
  std::cout << "starting external sort..." << std::endl;

  auto base_path = fs::path(eval_data.temp_files_dir);
  auto file_name = UuidGenerator::generate_uuid_v4();
  auto file_name_sorted = file_name + "_sorted";

  auto file_path_source = base_path / fs::path(file_name);
  auto file_path_destination = base_path / fs::path(file_name_sorted);

  dump_data_to_file(file_path_source);
  if (!time_control.tick()) {
    if (fs::exists(file_path_source))
      fs::remove(file_path_source);
    return;
  }

  ExternalSort::ExternalSort<RowSortConnector, ExternalSort::BINARY,
                             TimeControl>::sort(file_path_source.string(),
                                                file_path_destination.string(),
                                                base_path.string(), 1, 10,
                                                1'000'000'000, 4096,
                                                remove_duplicates, comparator,
                                                time_control);

  std::cout << "external sort done" << std::endl;
  fs::remove(file_path_source);

  if (!time_control.tick()) {
    std::cout << "external sort timedout" << std::endl;
    if (fs::exists(file_path_destination))
      fs::remove(file_path_destination);
    next_available = false;
    return;
  }
  active_files.insert(file_path_destination.string());
  buffer = std::vector<char>(max_in_memory_size);

  sorted_file_is = std::make_unique<std::ifstream>(
      file_path_destination, std::ios::in | std::ios::binary);
  sorted_file_is->rdbuf()->pubsetbuf(
      buffer.data(), static_cast<std::streamsize>(buffer.size()));

  next_wdisk();
}
bool ExternalSortedFileHandler::has_next_wdisk() { return next_available; }
bool ExternalSortedFileHandler::has_next_mem() {
  return (size_t)current_pos_initial_data < initial_data.size();
}
std::vector<unsigned long> ExternalSortedFileHandler::next_wdisk() {
  auto result = next_value;
  next_available = false;
  RowSortConnector value;
  if (RowSortConnector::read_value(*sorted_file_is, value)) {
    next_available = true;
    next_value = std::move(value.get_row());
  }
  return result;
}
std::vector<unsigned long> ExternalSortedFileHandler::next_wmem() {
  return initial_data[current_pos_initial_data++].get_row();
}
ExternalSortedFileHandler::~ExternalSortedFileHandler() {
  for (auto &active_file : active_files) {
    if (fs::exists(active_file))
      fs::remove(active_file);
  }
}
void ExternalSortedFileHandler::dump_data_to_file(fs::path &path) {
  std::ofstream ofs(path, std::ios::out | std::ios::trunc | std::ios::binary);
  std::cout << "dumping data to file: " << path.string() << std::endl;
  for (auto &row_value : initial_data) {
    if (!time_control.tick())
      return;
    ofs << row_value;
  }
  initial_data.clear();

  while (input_it->has_next()) {
    auto value = input_it->next();
    if (!time_control.tick())
      return;
    ofs << RowSortConnector(std::move(value));
  }
}
void ExternalSortedFileHandler::reset_iterator() {
  if (disk_operations) {
    sorted_file_is->seekg(0);
    next_wdisk();
  } else {
    current_pos_initial_data = 0;
  }
}
