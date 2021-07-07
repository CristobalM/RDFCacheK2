//
// Created by cristobal on 4/23/21.
//

#include "DayTimeDurationResource.hpp"
#include <query_processing/ExprProcessorPersistentData.hpp>
#include <sstream>
bool DayTimeDurationResource::operator==(const TermResource &rhs) const {
  if (!rhs.is_day_time_duration())
    return false;
  const auto &rhs_c = dynamic_cast<const DayTimeDurationResource &>(rhs);
  return sign == rhs_c.sign && days == rhs_c.days && hours == rhs_c.hours &&
         minutes == rhs_c.minutes && seconds == rhs_c.seconds;
}

DayTimeDurationResource::DayTimeDurationResource(int sign, int days, int hours,
                                                 int minutes, int seconds)
    : sign(sign >= 0 ? 1 : -1), days(days), hours(hours), minutes(minutes),
      seconds(seconds) {}
DayTimeDurationResource::DayTimeDurationResource()
    : DayTimeDurationResource(1, 0, 0, 0, 0) {}
void DayTimeDurationResource::set_sign(int new_sign) { sign = new_sign; }
void DayTimeDurationResource::set_days(int new_days) { days = new_days; }
void DayTimeDurationResource::set_hours(int new_hours) { hours = new_hours; }
void DayTimeDurationResource::set_minutes(int new_minutes) {
  minutes = new_minutes;
}
void DayTimeDurationResource::set_seconds(int new_seconds) {
  seconds = new_seconds;
}
bool DayTimeDurationResource::is_literal() const { return true; }
int DayTimeDurationResource::reverse_diff_compare(
    const DayTimeDurationResource &day_time_duration_resource) const {
  if (day_time_duration_resource.sign > sign) {
    return 1;
  } else if (day_time_duration_resource.sign < sign) {
    return -1;
  }

  int days_diff = day_time_duration_resource.days - days;
  if (days_diff != 0)
    ExprProcessorPersistentData::get().normalize_diff_cmp(days_diff);
  int hours_diff = day_time_duration_resource.hours - hours;
  if (hours_diff != 0)
    ExprProcessorPersistentData::get().normalize_diff_cmp(hours_diff);
  int minutes_diff = day_time_duration_resource.minutes - minutes;
  if (minutes_diff != 0)
    ExprProcessorPersistentData::get().normalize_diff_cmp(minutes_diff);
  int seconds_diff = day_time_duration_resource.seconds - seconds;
  if (seconds_diff != 0)
    ExprProcessorPersistentData::get().normalize_diff_cmp(seconds_diff);
  return 0;
}
int DayTimeDurationResource::diff_compare(const TermResource &rhs) const {
  return rhs.reverse_diff_compare(*this);
}
bool DayTimeDurationResource::can_cast_to_literal_string() const {
  return true;
}
std::string DayTimeDurationResource::get_content_string_copy() const {
  std::stringstream ss;
  if (sign < 0) {
    ss << "-";
  }
  if (days > 0) {
    ss << days << "D";
  }
  if (hours > 0 || minutes > 0 || seconds > 0) {
    ss << "T";
  }
  if (hours > 0) {
    ss << hours << "H";
  }
  if (minutes > 0) {
    ss << minutes << "M";
  }
  if (seconds > 0) {
    ss << seconds << "S";
  }
  return ss.str();
}
