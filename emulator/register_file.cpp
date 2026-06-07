#include "register_file.hpp"
#include <iostream>
#include <iomanip>

uint32_t RegisterFile::registers[16] = {}; 
uint32_t RegisterFile::specialPurposeRegisters[3] = {};

void RegisterFile::getStateOfRegisterFile()
{
  std::cout << "----------------------------------" << std::endl;
  std::cout << "STATE OF REGISTERS" << std::endl;
  for(int i = 0; i < 16; i++)
  {
      std::cout << "r" << i
                << "=0x"
                << std::hex
                << std::uppercase
                << std::setw(8)
                << std::setfill('0')
                << registers[i];

      if(i % 4 == 3)
          std::cout << '\n';
      else
          std::cout << ' ';
  }

  std::cout << std::dec;
}
