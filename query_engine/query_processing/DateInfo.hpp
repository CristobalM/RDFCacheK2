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
  bool operator==(const DateInfo &rhs) const;

  static int year_days(int year);

  static bool leap_year(int year);
  static int month_days(int month, int year);
  static DateInfo zero_offset_date_info(DateInfo date_info);

  static int normalize_diff_cmp(int diff);
  int cmp_to(const DateInfo &rhs) const;
  int rev_cmp_to(const DateInfo &rhs) const;
};

#endif // RDFCACHEK2_DATEINFO_HPP
