#ifndef _CPU_HPP_
#define _CPU_HPP_
#include <cstdint>

class CPU
{
public:
  CPU() = delete;
  CPU(const CPU& processor) = delete;
  CPU& operator= (const CPU& processor) = delete;
  static void initializeCPU();
private:
  static void fetchInstruction();
  static void decodeAndExecute();
  static uint8_t instructionRegister[4];
};
#endif