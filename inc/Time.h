#ifndef __QUAE_MODICA_TIME_H__
#define __QUAE_MODICA_TIME_H__

#include <string>
#include <time.h>
#include <string.h>
#include <stdio.h>

namespace QuaeModicaUtil {
class Time {
public:
  Time() {
    time.tv_sec = time.tv_nsec = 0;
  }

  static Time now() {
    Time t;
    clock_gettime(CLOCK_REALTIME, &(t.time));
    return t;
  }

  std::string to_string() {
    char ans[32];
    // create a combined date and time string in ISO 8601 format
    time_t t = time.tv_sec;
    strftime(ans, sizeof(ans), "%F %T", gmtime(&t));
    sprintf(ans + strlen(ans), ".%031lu", time.tv_nsec / 1000000);
    return std::string(ans);
  }

private:
  timespec time;
}; // class Time
} // namespace QuaeModicaUtil
#endif // #ifndef __QUAE_MODICA_TIME_H__
