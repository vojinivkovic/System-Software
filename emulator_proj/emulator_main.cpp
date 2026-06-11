#include "cpu.hpp"
#include "../aux/exceptions.hpp"
#include <string>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
  std::string fileName = argv[1];
  try
  {
    if(argc < 2)
    {
      throw std::runtime_error("Too few arguments for emulator. Emulator expects ./emulator program.hex");
    }
    if(argc > 2)
    {
      throw std::runtime_error("Too many arguments for emulator. Emulator expects ./emulator program.hex");
    }
    CPU::initializeCPU(fileName);
    CPU::runProgram();
    CPU::cleanup();
  }
  catch(CPUErrors& err)
  {
    std::cout << "Error[" << err.toString(err.getErrorType()) << "]" << std::endl
                << err.what() << std::endl;
    
    if(!err.getDetailMessage().empty())
    {
      std::cout << "(" << err.getDetailMessage() << ")" << std::endl; 

    }
  }
  catch(const std::runtime_error& err)
  {
    std::cout << err.what() << std::endl;
  }
  return 0;
}