//
// Created by cristobal on 7/16/21.
//

#ifndef RDFCACHEK2_TIMECONTROL_HPP
#define RDFCACHEK2_TIMECONTROL_HPP

#include "QueryError.hpp"
#include <chrono>
#include <memory>
#include <string>
#include <vector>

class TimeControl {
  long ticks_until_check;
  std::chrono::milliseconds time_duration;

  std::chrono::time_point<std::chrono::system_clock> starting_time;
  long current_ticks;
  bool time_has_passed;
  bool with_error;
  std::unique_ptr<QueryError> q_error;

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
  void tick_only_count();
  bool has_error();
  QueryError &get_query_error();

  void report_error(std::unique_ptr<QueryError> &&query_error);
  void report_error(std::string error_msg);
};

#endif // RDFCACHEK2_TIMECONTROL_HPP
