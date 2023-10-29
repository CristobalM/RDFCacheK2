//
// Created by cristobal on 7/16/21.
//

#ifndef RDFCACHEK2_TIMECONTROL_HPP
#define RDFCACHEK2_TIMECONTROL_HPP

#include "QueryError.hpp"
#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
namespace k2cache {

class TimeControl {
  long ticks_until_check;
  std::chrono::milliseconds time_duration;

  std::chrono::time_point<std::chrono::system_clock> starting_time;
  long current_ticks;
  bool time_has_passed;
  bool with_error;
  std::unique_ptr<QueryError> q_error;

  std::mutex m;

public:
  static constexpr bool with_time_control = true;

  TimeControl(long ticks_until_check, std::chrono::milliseconds time_duration);

  TimeControl(const TimeControl &other);
  TimeControl(TimeControl &&other) noexcept;
  TimeControl &operator=(const TimeControl &other);
  TimeControl &operator=(TimeControl &&other) noexcept;

  bool tick();
  void start_timer();
  bool finished() const;
  bool has_error();
  QueryError &get_query_error();

  void report_error(std::unique_ptr<QueryError> &&query_error);
  void report_error(std::string error_msg);

  void force_cancel();
};
}

#endif // RDFCACHEK2_TIMECONTROL_HPP
