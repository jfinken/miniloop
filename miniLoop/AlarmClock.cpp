#include <cstring>
#include <ctime>
#include <map>
#include <string>

#include "Loop.h"

static const std::map<std::string, enum loop::AlarmClock::Alarm::Days>
  day_names {
      { "mon", loop::AlarmClock::Alarm::Days::DAY_MONDAY },
      { "tue", loop::AlarmClock::Alarm::Days::DAY_TUESDAY },
      { "wed", loop::AlarmClock::Alarm::Days::DAY_WEDNESDAY },
      { "thu", loop::AlarmClock::Alarm::Days::DAY_THURSDAY },
      { "fri", loop::AlarmClock::Alarm::Days::DAY_FRIDAY },
      { "sat", loop::AlarmClock::Alarm::Days::DAY_SATURDAY },
      { "sun", loop::AlarmClock::Alarm::Days::DAY_SUNDAY },
  };

static void
time_now(int& minuteOfTheDay, enum loop::AlarmClock::Alarm::Days& day)
{
    struct tm tm;
    time_t    t = time(nullptr);

    localtime_r(&t, &tm);
    minuteOfTheDay = tm.tm_hour * 60 + tm.tm_min;
    switch (tm.tm_wday) {
        case 0:
            day = loop::AlarmClock::Alarm::Days::DAY_SUNDAY;
            break;
        case 1:
            day = loop::AlarmClock::Alarm::Days::DAY_MONDAY;
            break;
        case 2:
            day = loop::AlarmClock::Alarm::Days::DAY_TUESDAY;
            break;
        case 3:
            day = loop::AlarmClock::Alarm::Days::DAY_WEDNESDAY;
            break;
        case 4:
            day = loop::AlarmClock::Alarm::Days::DAY_THURSDAY;
            break;
        case 5:
            day = loop::AlarmClock::Alarm::Days::DAY_FRIDAY;
            break;
        case 6:
            day = loop::AlarmClock::Alarm::Days::DAY_SATURDAY;
            break;
    }
}

namespace loop {

AlarmClock::Alarm::Alarm(const JSON::Object& alarmSpec)
  : Alarm()
{
    JSON::Object hours, minutes, on;

    hours = alarmSpec["hours"];
    minutes = alarmSpec["minutes"];
    on = alarmSpec["on"];

    time = hours.asInt() * 60 + minutes.asInt();

    valid = hours.isInt() && minutes.isInt();

    if (on.isArray()) {
        days = 0;
        for (auto day : on) {
            auto name = day_names.find(day.asString());
            if (name != day_names.end())
                days |= name->second;
        }
    }
}

bool
AlarmClock::Alarm::matches(int time, int day)
{
    return valid && this->time == time && (this->days & day);
}

std::shared_ptr<AlarmClock>
AlarmClock::create(const JSON::Object& spec) noexcept
{
    std::shared_ptr<AlarmClock> ret{ nullptr };

    JSON::Object type = spec["type"];
    JSON::Object params = spec["params"];

    if (!(type && type.isString() && params && params.isObject()))
        return nullptr;

    if (strcmp(type.asString(), "oneshot") == 0)
        ret = std::make_shared<OneShotAlarm>(params);
    else if (strcmp(type.asString(), "recurrent") == 0)
        ret = std::make_shared<RecurrentAlarm>(params);

    return (ret && ret.get()) ? ret : nullptr;
}

void
AlarmClock::onAlarm(std::function<void()> callback) noexcept
{
    this->callback = callback;
}

RecurrentAlarm::RecurrentAlarm() noexcept
{
    timeout.onTimeout([this]() {
        int         minuteOfTheDay;
        Alarm::Days currentDay = Alarm::Days::DAY_SUNDAY;

        time_now(minuteOfTheDay, currentDay);

        if (alarm.matches(minuteOfTheDay, currentDay)) {
            if (!triggered && callback) {
                triggered = true;
                callback();
            }
        } else
            triggered = false;
    });
}

RecurrentAlarm::RecurrentAlarm(const JSON::Object& params) noexcept
  : alarm(params)
{
    timeout.onTimeout([this]() {
        int         minuteOfTheDay;
        Alarm::Days currentDay = Alarm::Days::DAY_SUNDAY;

        time_now(minuteOfTheDay, currentDay);

        if (alarm.matches(minuteOfTheDay, currentDay)) {
            if (!triggered && callback) {
                triggered = true;
                callback();
            }
        } else
            triggered = false;
    });
}

RecurrentAlarm::~RecurrentAlarm() noexcept {}

void
RecurrentAlarm::set(Alarm a)
{
    alarm = a;
}

void
RecurrentAlarm::reset() noexcept
{
    alarm = Alarm();
}

OneShotAlarm::OneShotAlarm() noexcept
  : timeout{}
{
    timeout.onTimeout([this]() { callback(); });
}

OneShotAlarm::OneShotAlarm(const JSON::Object& params) noexcept
{
    int after_ms{ 0 };

    JSON::Object hours, minutes, seconds;
    hours = params["hours"];
    minutes = params["minutes"];
    seconds = params["seconds"];

    if (hours && hours.isInt())
        after_ms += 3600000 * hours.asInt();
    if (minutes && minutes.isInt())
        after_ms += 60000 * minutes.asInt();
    if (seconds && seconds.isInt())
        after_ms += 1000 * seconds.asInt();

    timeout.onTimeout([this]() { callback(); });

    timeout.set(after_ms);
}

OneShotAlarm::~OneShotAlarm() noexcept {}

void
OneShotAlarm::reset() noexcept
{
    timeout.cancel();
}

void
OneShotAlarm::set(int after_ms) noexcept
{
    timeout.set(after_ms);
}
}
