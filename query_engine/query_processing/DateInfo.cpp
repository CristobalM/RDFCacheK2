//
// Created by cristobal on 7/6/21.
//

#include "DateInfo.hpp"
#include <array>

static constexpr std::array<int, 12> days_months = {
    31, // jan / 1
    28, // feb / 2 non leap
    31, // mar /  3
    30, // apr / 4
    31, // may / 5
    30, // jun / 6
    31, // jul / 7
    31, // ago / 8
    30, // sep / 9
    31, // oct / 10
    30, // nov / 11
    31, // dec / 12
};
bool DateInfo::operator==(const DateInfo &rhs) const {
  return year == rhs.year && month == rhs.month && day == rhs.day &&
         hour == rhs.hour && minute == rhs.minute &&
         second_fraction == rhs.second_fraction &&
         offset_sign * rhs.offset_sign > 0 && offset_hour == rhs.offset_hour &&
         offset_minute == rhs.offset_minute;
}

int DateInfo::year_days(int year) {
  if (leap_year(year))
    return 366;
  return 365;
}

bool DateInfo::leap_year(int year) {
  return (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0));
}

int DateInfo::month_days(int month, int year) {
  if (month == 2 && leap_year(year)) {
    // february in leap year
    return 29;
  }
  return days_months[month - 1];
}

DateInfo DateInfo::zero_offset_date_info(DateInfo date_info) {
  int sign = date_info.offset_sign >= 0 ? 1 : -1;

  int minute_sum = date_info.offset_minute * sign + date_info.minute;
  int carry_hour = minute_sum / 60;
  date_info.minute = minute_sum % 60;

  int hour_sum = date_info.offset_hour * sign + date_info.hour + carry_hour;
  int carry_day = hour_sum / 24;
  date_info.hour = hour_sum % 24;

  int day_sum = date_info.day + carry_day;
  int days_in_month = month_days(date_info.month, date_info.year);
  int carry_month = day_sum / days_in_month;
  date_info.day = day_sum % days_in_month;

  int month_sum = date_info.month + carry_month;
  int carry_year = month_sum / 12;
  date_info.month = month_sum % 12;

  date_info.year += carry_year;

  date_info.offset_hour = 0;
  date_info.offset_minute = 0;
  date_info.offset_sign = 1;
  return date_info;
}

int DateInfo::normalize_diff_cmp(int diff) {
  if (diff > 0)
    return 1;
  else if (diff < 0)
    return -1;
  return 0;
}
int DateInfo::cmp_to(const DateInfo &rhs) const {
  return rhs.rev_cmp_to(*this);
}

int DateInfo::rev_cmp_to(const DateInfo &rhs) const {
  auto other_date_info = zero_offset_date_info(rhs);
  auto this_date_info = zero_offset_date_info(*this);
  auto year_diff = other_date_info.year - this_date_info.year;
  if (year_diff != 0)
    return normalize_diff_cmp(year_diff);
  auto month_diff = other_date_info.month - this_date_info.month;
  if (month_diff != 0)
    return normalize_diff_cmp(month_diff);
  auto day_diff = other_date_info.day - this_date_info.day;
  if (day_diff != 0)
    return normalize_diff_cmp(day_diff);
  auto hour_diff = other_date_info.hour - this_date_info.hour;
  if (hour_diff != 0)
    return normalize_diff_cmp(hour_diff);
  auto minute_diff = other_date_info.minute - this_date_info.minute;
  if (minute_diff != 0)
    return normalize_diff_cmp(minute_diff);
  auto second_diff = other_date_info.second - this_date_info.second;
  if (second_diff != 0)
    return normalize_diff_cmp(second_diff);
  auto seconds_frac_diff =
      other_date_info.second_fraction - this_date_info.second_fraction;
  if (seconds_frac_diff != 0)
    return normalize_diff_cmp(seconds_frac_diff);
  return 0;
}
