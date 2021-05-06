//
// Created by cristobal on 4/23/21.
//

#include "DayTimeDurationResource.hpp"
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
