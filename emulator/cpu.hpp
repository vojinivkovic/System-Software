#ifndef _CPU_HPP_
#define _CPU_HPP_
#include <cstdint>

class CPU
{
public:
  CPU() = delete;
  CPU(const CPU& processor) = delete;
  CPU& operator= (const CPU& processor) = delete;
  enum class InstructionOPCodes
  {
    HALT, 
    INT, 
    CALL,
    JUMP, 
    EXCHG, 
    ARITH, 
    LOG, 
    SHIFT,
    ST, 
    LD
  };
  static void initializeCPU();
  static void runProgram();
private:
  static void fetchInstruction();
  static InstructionOPCodes decode();
  static void executeInstruction(InstructionOPCodes code);
  static uint8_t instructionRegister[4];
  static void (*instructionSet[10])();
  static void instructionSoftwareInterrupt();
  static void instructionCall();
  static void instructionJump();
  static void instructionExchg();
  static void instructionArithmetic();
  static void instructionLogic();
  static void instructionShift();
  static void instructionStore();
  static void instructionLoad();

  static uint8_t extractRegisterA();
  static uint8_t extractRegisterB();
  static uint8_t extractRegisterC();
  static uint16_t extractDisplay();
  static void extractInstructionFiels(uint8_t& regA, uint8_t& regB, uint8_t& regC, uint16_t& disp);
};
#endif