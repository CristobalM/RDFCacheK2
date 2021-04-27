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
};

#endif // RDFCACHEK2_DATEINFO_HPP
