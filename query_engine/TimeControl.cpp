//
// Created by cristobal on 7/16/21.
//

#include "TimeControl.hpp"
namespace k2cache {

bool TimeControl::tick() {
  current_ticks++;
  if (time_has_passed)
    return false;
  if (current_ticks < ticks_until_check)
    return true;
  current_ticks = 0;
  auto current_time = std::chrono::system_clock::now();
  std::lock_guard lg(m);
  if (current_time - starting_time > time_duration) {
    time_has_passed = true;
    return false;
  }
  return true;
}
TimeControl::TimeControl(long ticks_until_check,
                         std::chrono::milliseconds time_duration)
    : ticks_until_check(ticks_until_check), time_duration(time_duration),
      current_ticks(0), time_has_passed(false), with_error(false),
      q_error(nullptr) {
  // can be called again for reset
  start_timer();
}
void TimeControl::start_timer() {
  starting_time = std::chrono::system_clock::now();
}
bool TimeControl::finished() const { return !time_has_passed; }
bool TimeControl::has_error() { return with_error; }

void TimeControl::report_error(std::unique_ptr<QueryError> &&query_error) {
  with_error = true;
  time_has_passed = true;
  this->q_error = std::move(query_error);
}
QueryError &TimeControl::get_query_error() { return *q_error; }
void TimeControl::report_error(std::string error_msg) {
  report_error(std::make_unique<QueryError>(std::move(error_msg)));
}
TimeControl::TimeControl(const TimeControl &other)
    : ticks_until_check(other.ticks_until_check),
      time_duration(other.time_duration), starting_time(other.starting_time),
      current_ticks(other.current_ticks),
      time_has_passed(other.time_has_passed), with_error(other.with_error),
      q_error(std::make_unique<QueryError>(*other.q_error)) {}

TimeControl::TimeControl(TimeControl &&other) noexcept
    : ticks_until_check(other.ticks_until_check),
      time_duration(other.time_duration), starting_time(other.starting_time),
      current_ticks(other.current_ticks),
      time_has_passed(other.time_has_passed), with_error(other.with_error),
      q_error(std::move(other.q_error)) {}

TimeControl &TimeControl::operator=(const TimeControl &other) {
  ticks_until_check = other.ticks_until_check;
  time_duration = other.time_duration;
  starting_time = other.starting_time;
  current_ticks = other.current_ticks;
  time_has_passed = other.time_has_passed;
  with_error = other.with_error;
  q_error = std::make_unique<QueryError>(*other.q_error);

  return *this;
}
TimeControl &TimeControl::operator=(TimeControl &&other) noexcept {
  ticks_until_check = other.ticks_until_check;
  time_duration = other.time_duration;
  starting_time = other.starting_time;
  current_ticks = other.current_ticks;
  time_has_passed = other.time_has_passed;
  with_error = other.with_error;
  q_error = std::move(other.q_error);
  return *this;
}
void TimeControl::force_cancel() {
  std::lock_guard lg(m);
  time_duration = std::chrono::milliseconds (0);
}
}
