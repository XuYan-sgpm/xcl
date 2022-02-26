//
// Created by 徐琰 on 2021/11/28.
//

#include <xcl/util/time_unit.h>

namespace xcl {
class Days : public TimeUnit {
public:
  uint64_t ToDays(uint64_t d) const override { return d; }

  uint64_t ToHours(uint64_t d) const override { return d * 24; }

  uint64_t ToMicros(uint64_t d) const override { return ToMillis(d) * 1000; }

  uint64_t ToMillis(uint64_t d) const override { return ToSeconds(d) * 1000; }

  uint64_t ToNanos(uint64_t d) const override { return ToMicros(d) * 1000; }

  uint64_t ToSeconds(uint64_t d) const override { return ToMinutes(d) * 60; }

  uint64_t ToMinutes(uint64_t d) const override { return ToHours(d) * 60; }

protected:
  TYPE Type() const override { return TYPE_DAYS; }
};

class Hours : public TimeUnit {
public:
  uint64_t ToDays(uint64_t d) const override { return d / 24; }

  uint64_t ToHours(uint64_t d) const override { return d; }

  uint64_t ToMicros(uint64_t d) const override { return ToMillis(d) * 1000; }

  uint64_t ToMillis(uint64_t d) const override { return ToSeconds(d) * 1000; }

  uint64_t ToNanos(uint64_t d) const override { return ToMicros(d) * 1000; }

  uint64_t ToSeconds(uint64_t d) const override { return ToMinutes(d) * 60; }

  uint64_t ToMinutes(uint64_t d) const override { return d * 60; }

protected:
  TYPE Type() const override { return TYPE_HOURS; }
};

class Minutes : public TimeUnit {
public:
  uint64_t ToDays(uint64_t d) const override { return ToHours(d) / 24; }

  uint64_t ToHours(uint64_t d) const override { return d / 60; }

  uint64_t ToMicros(uint64_t d) const override { return ToMillis(d) * 1000; }

  uint64_t ToMillis(uint64_t d) const override { return ToSeconds(d) * 1000; }

  uint64_t ToNanos(uint64_t d) const override { return ToMicros(d) * 1000; }

  uint64_t ToSeconds(uint64_t d) const override { return d * 60; }

  uint64_t ToMinutes(uint64_t d) const override { return d; }

protected:
  TYPE Type() const override { return TYPE_MINUTES; }
};

class Seconds : public TimeUnit {
public:
  uint64_t ToDays(uint64_t d) const override { return ToHours(d) / 24; }

  uint64_t ToHours(uint64_t d) const override { return ToMinutes(d) / 60; }

  uint64_t ToMicros(uint64_t d) const override { return ToMillis(d) * 1000; }

  uint64_t ToMillis(uint64_t d) const override { return d * 1000; }

  uint64_t ToNanos(uint64_t d) const override { return ToMicros(d) * 1000; }

  uint64_t ToSeconds(uint64_t d) const override { return d; }

  uint64_t ToMinutes(uint64_t d) const override { return d / 60; }

protected:
  TYPE Type() const override { return TYPE_SECONDS; }
};

class Millis : public TimeUnit {
public:
  uint64_t ToDays(uint64_t d) const override { return ToHours(d) / 24; }

  uint64_t ToHours(uint64_t d) const override { return ToMinutes(d) / 60; }

  uint64_t ToMicros(uint64_t d) const override { return d * 1000; }

  uint64_t ToMillis(uint64_t d) const override { return d; }

  uint64_t ToNanos(uint64_t d) const override { return ToMicros(d) * 1000; }

  uint64_t ToSeconds(uint64_t d) const override { return d / 1000; }

  uint64_t ToMinutes(uint64_t d) const override { return ToSeconds(d) / 60; }

protected:
  TYPE Type() const override { return TYPE_MILLISECONDS; }
};

class Micros : public TimeUnit {
public:
  uint64_t ToDays(uint64_t d) const override { return ToHours(d) / 24; }

  uint64_t ToHours(uint64_t d) const override { return ToMinutes(d) / 60; }

  uint64_t ToMicros(uint64_t d) const override { return d; }

  uint64_t ToMillis(uint64_t d) const override { return d / 1000; }

  uint64_t ToNanos(uint64_t d) const override { return d * 1000; }

  uint64_t ToSeconds(uint64_t d) const override { return ToMillis(d) / 1000; }

  uint64_t ToMinutes(uint64_t d) const override { return ToSeconds(d) / 60; }

protected:
  TYPE Type() const override { return TYPE_MICROS; }
};

class Nanos : public TimeUnit {
public:
  uint64_t ToDays(uint64_t d) const override { return ToHours(d) / 24; }

  uint64_t ToHours(uint64_t d) const override { return ToMinutes(d) / 60; }

  uint64_t ToMicros(uint64_t d) const override { return d / 1000; }

  uint64_t ToMillis(uint64_t d) const override { return ToMicros(d) / 1000; }

  uint64_t ToNanos(uint64_t d) const override { return d; }

  uint64_t ToSeconds(uint64_t d) const override { return ToMillis(d) / 1000; }

  uint64_t ToMinutes(uint64_t d) const override { return ToSeconds(d) / 60; }

protected:
  TYPE Type() const override { return TYPE_NANOS; }
};

namespace time_unit {
static Days days;
static Hours hours;
static Minutes minutes;
static Seconds seconds;
static Millis milliseconds;
static Micros microseconds;
static Nanos nanoseconds;
} // namespace time_unit

uint64_t TimeUnit::Convert(uint64_t d, const TimeUnit &timeUnit) const {
  switch (timeUnit.Type()) {
  case TYPE_DAYS:
    return ToDays(d);
  case TYPE_HOURS:
    return ToHours(d);
  case TYPE_MINUTES:
    return ToMinutes(d);
  case TYPE_SECONDS:
    return ToSeconds(d);
  case TYPE_MILLISECONDS:
    return ToMillis(d);
  case TYPE_MICROS:
    return ToMicros(d);
  case TYPE_NANOS:
    return ToNanos(d);
  }
}

TimeUnit &TimeUnit::days_ = xcl::time_unit::days;
TimeUnit &TimeUnit::hours_ = xcl::time_unit::hours;
TimeUnit &TimeUnit::minutes_ = xcl::time_unit::minutes;
TimeUnit &TimeUnit::seconds_ = xcl::time_unit::seconds;
TimeUnit &TimeUnit::millis_ = xcl::time_unit::milliseconds;
TimeUnit &TimeUnit::micros_ = xcl::time_unit::microseconds;
TimeUnit &TimeUnit::nanos_ = xcl::time_unit::nanoseconds;

TimeUnit &TimeUnit::days() { return days_; }
TimeUnit &TimeUnit::hours() { return hours_; }
TimeUnit &TimeUnit::micros() { return micros_; }
TimeUnit &TimeUnit::millis() { return millis_; }
TimeUnit &TimeUnit::nanos() { return nanos_; }
TimeUnit &TimeUnit::seconds() { return seconds_; }
TimeUnit &TimeUnit::minutes() { return minutes_; }
} // namespace xcl
