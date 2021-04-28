//
// Created by cristobal on 4/23/21.
//

#ifndef RDFCACHEK2_DAYTIMEDURATIONRESOURCE_HPP
#define RDFCACHEK2_DAYTIMEDURATIONRESOURCE_HPP

#include "TermResource.hpp"
class DayTimeDurationResource : public TermResource {
  int sign;
  int days;
  int hours;
  int minutes;
  int seconds;

public:
  DayTimeDurationResource(int sign, int days, int hours, int minutes,
                          int seconds);
  DayTimeDurationResource();

  void set_sign(int sign);
  void set_days(int days);
  void set_hours(int hours);
  void set_minutes(int minutes);
  void set_seconds(int seconds);
  bool operator==(const TermResource &rhs) const override;
};

#endif // RDFCACHEK2_DAYTIMEDURATIONRESOURCE_HPP
