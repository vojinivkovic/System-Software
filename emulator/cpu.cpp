#include "cpu.hpp"
#include "register_file.hpp"
#include "memory.hpp"

uint8_t CPU::instructionRegister[4];

void CPU::initializeCPU()
{
}

void CPU::fetchInstruction()
{
  uint32_t pc = RegisterFile::readFromGPRegister(15);

  for(size_t i = 4; i-- > 0; )
  {
    instructionRegister[i] = Memory::memRead(pc);
    pc++;
  }

}

void CPU::decodeAndExecute()
{
}
