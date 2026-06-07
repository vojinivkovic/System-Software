#ifndef _TIMER_HPP_
#define _TIMER_HPP_
#include <cstdint>
#include <iostream>
#include <chrono>

class Timer
{
public:
  Timer() = delete;
  Timer(const Timer& time) = delete;
  Timer& operator=(const Timer& time) = delete;

  static void initializeTimer();
  static void changeConfig();
  static void poll();
private:
  static std::chrono::steady_clock::time_point lastTick;
  static std::chrono::milliseconds period;
  static uint32_t tableConfig[8];
};

#endif