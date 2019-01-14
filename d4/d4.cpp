#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string_view>
#include <tuple>

// month, day, hour, minute
using when_t = std::tuple<short, short, short, short>;
struct guard_info {
  int sleep_total;
  std::array<int, 60> sleep_mins;
};
enum class event_t { shift, sleep, wake, unknown };

event_t parseEvent(std::string line) {
  if (line[19] == 'G') {
    return event_t::shift;
  }
  if (line[19] == 'f') {
    return event_t::sleep;
  }
  if (line[19] == 'w') {
    return event_t::wake;
  }
  return event_t::unknown;
}

int parseGuard(std::string line) {
  int g_end = line.find(" ", 26);
  std::stringstream ss_guard(line.substr(26, g_end - 26));
  int g_id = 0;
  ss_guard >> g_id;
  return g_id;
}

void handleWake(std::map<int, guard_info> &guards, int g_id, int sleep_start,
                int sleep_end) {
  for (int min = sleep_start; min < sleep_end; ++min) {
    guards[g_id].sleep_total += 1;
    guards[g_id].sleep_mins[min] += 1;
  }
}

void p2(std::map<int, guard_info> &guards) {
  int max_sleep = 0;
  int max_sleep_min = 0;
  int max_guard = 0;
  for (auto ginfo : guards) {
    for (int i = 0; i < 60; ++i) {
      if (ginfo.second.sleep_mins[i] > max_sleep) {
        max_guard = ginfo.first;
        max_sleep_min = i;
        max_sleep = ginfo.second.sleep_mins[i];
      }
    }
  }
  std::cout << "Minute: " << max_sleep_min
            << " was most slept on, by guard: " << max_guard << "."
            << std::endl;
}

void p1(std::map<int, guard_info> &guards) {

  int max_guard = 0;
  int max_sleep = 0;
  for (auto ginfo : guards) {
    if (ginfo.second.sleep_total > max_sleep) {
      max_sleep = ginfo.second.sleep_total;
      max_guard = ginfo.first;
    }
  }

  guard_info found = guards[max_guard];

  int max_minute = 0;
  int max_minute_amount = 0;
  int chk_min = 0;
  for (; chk_min < 60; ++chk_min) {
    if (found.sleep_mins[chk_min] > max_minute_amount) {
      max_minute_amount = found.sleep_mins[chk_min];
      max_minute = chk_min;
    }
  }
  std::cout << "Guard: " << max_guard
            << " slept most.  Sleepiest minute was: " << max_minute
            << std::endl;
}

int main() {

  std::map<when_t, std::string> events{};
  std::ifstream input("d4/input.txt");
  std::string line;
  while (std::getline(input, line)) {
    std::stringstream mon(line.substr(6, 2));
    std::stringstream day(line.substr(9, 2));
    std::stringstream hr(line.substr(12, 2));
    std::stringstream min(line.substr(15, 2));

    when_t when{};
    mon >> std::get<0>(when);
    day >> std::get<1>(when);
    hr >> std::get<2>(when);
    min >> std::get<3>(when);

    events.insert(std::pair<when_t, std::string>(when, line));
  }

  std::map<int, guard_info> guards{};
  int curr_guard = 0;
  int sleep_start = 0;

  for (auto elem : events) {
    switch (parseEvent(elem.second)) {
    case event_t::shift:
      curr_guard = parseGuard(elem.second);
      break;
    case event_t::sleep:
      sleep_start = std::get<3>(elem.first);
      break;
    case event_t::wake:
      handleWake(guards, curr_guard, sleep_start, std::get<3>(elem.first));
      break;
    default:
      std::cout << " ** what ** " << std::endl;
      break;
    }
  }

  p1(guards);
  p2(guards);
}
