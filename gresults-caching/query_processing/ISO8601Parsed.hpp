//
// Created by cristobal on 23-04-21.
//

#ifndef RDFCACHEK2_ISO8601PARSED_HPP
#define RDFCACHEK2_ISO8601PARSED_HPP

struct ISO8601Parsed {
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

#endif // RDFCACHEK2_ISO8601PARSED_HPP
