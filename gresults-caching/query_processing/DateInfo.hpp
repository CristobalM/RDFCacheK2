//
// Created by cristobal on 23-04-21.
//

#ifndef RDFCACHEK2_DATEINFO_HPP
#define RDFCACHEK2_DATEINFO_HPP

struct DateInfo {
  bool matched;
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
  int second_fraction;
  int offset_sign;
  int offset_hour;
  int offset_minute;
  bool operator==(const DateInfo &rhs) const {
    return year == rhs.year && month == rhs.month && day == rhs.day &&
           hour == rhs.hour && minute == rhs.minute &&
           second_fraction == rhs.second_fraction &&
           offset_sign * rhs.offset_sign > 0 &&
           offset_hour == rhs.offset_hour && offset_minute == rhs.offset_minute;
  }
};

#endif // RDFCACHEK2_DATEINFO_HPP
