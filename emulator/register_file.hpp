#ifndef _REGISTER_FILE_HPP_
#define _REGISTER_FILE_HPP_
#include <cstdint>

class RegisterFile
{
public:
  RegisterFile() = delete;
  RegisterFile(const RegisterFile& regFile) = delete;
  RegisterFile& operator=(const RegisterFile& regFile) = delete;
  static void writeToGPRegister(const uint8_t& regNum, const uint32_t& value) { registers[regNum] = value; }
  static void writeToSPRegister(const uint8_t& regNum, const uint32_t& value) { specialPurposeRegisters[regNum] = value; }
  static uint32_t readFromGPRegister(const uint8_t& regNum) { return registers[regNum]; }
  static uint32_t readFromSPRegister(const uint8_t& regNum) { return specialPurposeRegisters[regNum]; }
private:
  static uint32_t registers[15], specialPurposeRegisters[3];

};

#endif