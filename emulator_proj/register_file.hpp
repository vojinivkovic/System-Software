#ifndef _REGISTER_FILE_HPP_
#define _REGISTER_FILE_HPP_
#include <cstdint>
#include <iomanip>
#include <iostream>

class RegisterFile
{
public:
  RegisterFile() = delete;
  RegisterFile(const RegisterFile& regFile) = delete;
  RegisterFile& operator=(const RegisterFile& regFile) = delete;
  static void writeToGPRegister(const uint8_t& regNum, const uint32_t& value) 
  { if(!regNum)
    {
      return;
    }
    registers[regNum] = value; 
  }
  static void writeToSPRegister(const uint8_t& regNum, const uint32_t& value) 
  { 
   // std::cout << "write to SP REG[" << std::to_string(regNum) << "]: " << value << std::endl;
    specialPurposeRegisters[regNum] = value; 
  }
  static uint32_t readFromGPRegister(const uint8_t& regNum) { return registers[regNum]; }
  static uint32_t readFromSPRegister(const uint8_t& regNum) { return specialPurposeRegisters[regNum]; }
  static void getStateOfRegisterFile();
private:
  static uint32_t registers[16], specialPurposeRegisters[3];

};

#endif