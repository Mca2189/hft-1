#ifndef TIME_CONTROLLER_H_
#define TIME_CONTROLLER_H_

#include "struct/market_snapshot.h"
#include "struct/order.h"
#include "util/zmq_sender.hpp"
#include "util/common_tools.h"
#include "struct/exchange_info.h"
#include "struct/order_status.h"
#include "struct/time_status.h"
#include <unordered_map>
#include <time.h>
#include <libconfig.h++>
#include <sys/time.h>

#include <cmath>
#include <vector>
#include <iostream>
#include <string>

class TimeController {
 public:
  TimeController(string time_config);
  TimeController();
  TimeController(const TimeController & t);
  ~TimeController();

  TimeStatus::Enum ShotStatus(const MarketSnapshot& shot);
  TimeStatus::Enum CurrentStatus();
  TimeStatus::Enum IntStatus(int check_time) const;

  int TimevalInt(timeval t = {0,0}) const;
  int CurrentInt() const;
  std::string TimevalStr(timeval t = {0, 0}, bool show_time = false) const;
  std::string IntStr(int sec) const;

  void StartTimer();
  void EndTimer(const std::string & label="");

 private:
  int last_sec;
  int timezone_diff;
  std::vector<int>sleep_start;
  std::vector<int>sleep_stop;
  std::vector<int>close_start;
  std::vector<int>close_stop;
  std::vector<int>force_close_start;
  std::vector<int>force_close_stop;
  std::string mode;
  long int timer_sec;
  long int timer_usec;
  bool is_timer_on;

  bool IsMix(int s1, int e1, int s2, int e2) const;
  int Translate(const std::string & time) const;
  bool Check() const;
  void Push(std::vector<std::string> timestr, std::vector<int>& a, std::vector<int>& b);
};

#endif  // TIME_CONTROLLER_H_
