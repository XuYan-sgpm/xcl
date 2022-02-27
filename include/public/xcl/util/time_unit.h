//
// Created by 徐琰 on 2021/11/28.
//

#ifndef SCL_TIMEUNIT_H
#define SCL_TIMEUNIT_H

#include <cstdint>

namespace xcl {
class TimeUnit {
 protected:
  enum TimeType : char {
	TYPE_DAYS,
	TYPE_HOURS,
	TYPE_MINUTES,
	TYPE_SECONDS,
	TYPE_MILLISECONDS,
	TYPE_MICROS,
	TYPE_NANOS
  };

 protected:
  TimeUnit() = default;

 protected:
  virtual TimeType
  Type() const = 0;

 public:
  virtual ~TimeUnit() = default;

 private:
  static TimeUnit &days_;
  static TimeUnit &hours_;
  static TimeUnit &minutes_;
  static TimeUnit &seconds_;
  static TimeUnit &millis_;
  static TimeUnit &micros_;
  static TimeUnit &nanos_;

 public:
  virtual uint64_t
  ToDays(uint64_t d) const = 0;
  virtual uint64_t
  ToHours(uint64_t d) const = 0;
  virtual uint64_t
  ToMicros(uint64_t d) const = 0;
  virtual uint64_t
  ToMillis(uint64_t d) const = 0;
  virtual uint64_t
  ToNanos(uint64_t d) const = 0;
  virtual uint64_t
  ToSeconds(uint64_t d) const = 0;
  virtual uint64_t
  ToMinutes(uint64_t d) const = 0;
  virtual uint64_t
  Convert(uint64_t d, const TimeUnit &timeUnit) const final;

 public:
  static TimeUnit &
  days();
  static TimeUnit &
  hours();
  static TimeUnit &
  micros();
  static TimeUnit &
  millis();
  static TimeUnit &
  nanos();
  static TimeUnit &
  seconds();
  static TimeUnit &
  minutes();
};
}// namespace xcl

#endif// SCL_TIMEUNIT_H
