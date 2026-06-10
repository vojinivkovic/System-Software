#include "timer.hpp"
#include "register_file.hpp"
#include "memory.hpp"
#include "../aux/exceptions.hpp"


std::chrono::steady_clock::time_point Timer::lastTick;
std::chrono::milliseconds Timer::period;
uint32_t Timer::tableConfig[8] = {500, 1000, 1500, 2000, 5000, 10000, 30000, 60000}; 

void Timer::initializeTimer()
{
  lastTick = std::chrono::steady_clock::now();
  period = std::chrono::milliseconds(500);
}

void Timer::changeConfig()
{
  size_t optionIdx;
  optionIdx = Memory::memRead4bytes(0xFFFFFF10);
  if(optionIdx > 7)
  {
    throw CPUErrors(ErrorType::ErrorUndefinedTimerConfig, "There are only 8 timer configs available.");
  }
  std::cout << "option: " << optionIdx << std::endl;
  period = std::chrono::milliseconds(tableConfig[optionIdx]);

} 

void Timer::poll()
{
  std::chrono::steady_clock::time_point currentTick = std::chrono::steady_clock::now();
  if(currentTick - lastTick >= period)
  {
    // std::cout << "Timer interrupt" << std::endl;
    // std::cout << "STATUS REG [TIMER] = " << RegisterFile::readFromSPRegister(0) << std::endl;
    lastTick = currentTick;
    RegisterFile::writeToSPRegister(2, 2);
  }
}
