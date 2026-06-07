#include "cpu.hpp"
#include "register_file.hpp"
#include "memory.hpp"

uint8_t CPU::instructionRegister[4];

void CPU::initializeCPU()
{
}

void CPU::runProgram()
{
  while(true)
  {
    fetchInstruction();
    InstructionOPCodes opCode = decode();
    if(opCode == InstructionOPCodes::HALT)
    {
      break;
    }
    executeInstruction(opCode);
  }
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

CPU::InstructionOPCodes CPU::decode()
{
  uint8_t opCode = (instructionRegister[0] & 0xF0) >> 4;
  return static_cast<CPU::InstructionOPCodes>(opCode);
}

void CPU::executeInstruction(InstructionOPCodes code)
{
  instructionSet[static_cast<size_t>(code)]();
}

void CPU::instructionSoftwareInterrupt()
{
  uint32_t regStatusValue, regPCValue, regHandlerValue;

  regStatusValue = RegisterFile::readFromSPRegister(0);
  regPCValue = RegisterFile::readFromGPRegister(15);
  regHandlerValue = RegisterFile::readFromSPRegister(1);

  Memory::pushOnStack(regStatusValue);
  Memory::pushOnStack(regPCValue);
  RegisterFile::writeToSPRegister(2, 4);

  regStatusValue = regStatusValue | 0b100;
  RegisterFile::writeToGPRegister(15, regHandlerValue);
  
}

void CPU::instructionCall()
{
  //PUSH PC
  uint32_t sp = RegisterFile::readFromGPRegister(14);
  uint32_t pc = RegisterFile::readFromGPRegister(15);
  Memory::pushOnStack(pc);

  uint8_t regA, regB, regC;
  uint16_t disp;
  extractInstructionFiels(regA, regB, regC, disp);
  uint32_t newPC = RegisterFile::readFromGPRegister(regA) + RegisterFile::readFromGPRegister(regB) + disp;
  RegisterFile::writeToGPRegister(15, newPC);

}

void CPU::instructionJump()
{
  uint8_t mode, regA, regB, regC;
  uint16_t disp;
  uint32_t valueRegB, valueRegC, valueRegA, newPC;

  extractInstructionFiels(regA, regB, regC, disp);
  valueRegB = RegisterFile::readFromGPRegister(regB);
  valueRegC = RegisterFile::readFromGPRegister(regC);
  valueRegA = RegisterFile::readFromGPRegister(regA);

  mode = instructionRegister[0] & 0xF;
  switch(mode)
  {
    case 0x0:
      newPC = valueRegA + disp;
      RegisterFile::writeToGPRegister(15, newPC);
      break;
    case 0x1:
      if(valueRegB == valueRegC)
      {
        newPC = valueRegA + disp;
        RegisterFile::writeToGPRegister(15, newPC);
      }
      break;
    case 0x2:
      if(valueRegB != valueRegC)
      {
        newPC = valueRegA + disp;
        RegisterFile::writeToGPRegister(15, newPC);
      }
      break;
    case 0x3:
      if(static_cast<int>(valueRegB) > static_cast<int>(valueRegC))
      {
        newPC = valueRegA + disp;
        RegisterFile::writeToGPRegister(15, newPC);
      }
      break;
  }
  
}

void CPU::instructionExchg()
{
  uint8_t regA, regB, regC;
  uint16_t disp;
  uint32_t valueRegB, valueRegC;

  extractInstructionFiels(regA, regB, regC, disp);
  valueRegB = RegisterFile::readFromGPRegister(regB);
  valueRegC = RegisterFile::readFromGPRegister(regC);
  
  RegisterFile::writeToGPRegister(regB, valueRegC);
  RegisterFile::writeToGPRegister(regC, valueRegB);

  
}

void CPU::instructionArithmetic()
{
  uint8_t mode, regA, regB, regC;
  uint16_t disp;
  uint32_t valueRegB, valueRegC, valueRegA;

  extractInstructionFiels(regA, regB, regC, disp);
  valueRegB = RegisterFile::readFromGPRegister(regB);
  valueRegC = RegisterFile::readFromGPRegister(regC);
  

  mode = instructionRegister[0] & 0xF;
  switch(mode)
  {
    case 0x0:
      valueRegA = valueRegB + valueRegC;
      RegisterFile::writeToGPRegister(regA, valueRegA);
      break;
    case 0x1:
      valueRegA = valueRegB - valueRegC;
      RegisterFile::writeToGPRegister(regA, valueRegA);
      break;
    case 0x2:
      valueRegA = valueRegB * valueRegC;
      RegisterFile::writeToGPRegister(regA, valueRegA);
      break;
    case 0x3:
      valueRegA = valueRegB / valueRegC;
      RegisterFile::writeToGPRegister(regA, valueRegA);
      break;
  }
}

void CPU::instructionLogic()
{
  uint8_t mode, regA, regB, regC;
  uint16_t disp;
  uint32_t valueRegB, valueRegC, valueRegA;

  extractInstructionFiels(regA, regB, regC, disp);
  valueRegB = RegisterFile::readFromGPRegister(regB);
  valueRegC = RegisterFile::readFromGPRegister(regC);
  

  mode = instructionRegister[0] & 0xF;
  switch(mode)
  {
    case 0x0:
      valueRegA = ~valueRegB;
      RegisterFile::writeToGPRegister(regA, valueRegA);
      break;
    case 0x1:
      valueRegA = valueRegB & valueRegC;
      RegisterFile::writeToGPRegister(regA, valueRegA);
      break;
    case 0x2:
      valueRegA = valueRegB | valueRegC;
      RegisterFile::writeToGPRegister(regA, valueRegA);
      break;
    case 0x3:
      valueRegA = valueRegB ^ valueRegC;
      RegisterFile::writeToGPRegister(regA, valueRegA);
      break;
  }

}

void CPU::instructionShift()
{
  uint8_t mode, regA, regB, regC;
  uint16_t disp;
  uint32_t valueRegB, valueRegC, valueRegA;

  extractInstructionFiels(regA, regB, regC, disp);
  valueRegB = RegisterFile::readFromGPRegister(regB);
  valueRegC = RegisterFile::readFromGPRegister(regC);
  

  mode = instructionRegister[0] & 0xF;
  switch(mode)
  {
    case 0x0:
      valueRegA = valueRegB << valueRegC;
      RegisterFile::writeToGPRegister(regA, valueRegA);
      break;
    case 0x1:
      valueRegA = valueRegB >> valueRegC;
      RegisterFile::writeToGPRegister(regA, valueRegA);
      break;
  }
}

void CPU::instructionStore()
{
  uint8_t mode, regA, regB, regC;
  uint16_t disp;
  int16_t signedDisp;
  uint32_t valueRegB, valueRegC, valueRegA, address;

  extractInstructionFiels(regA, regB, regC, disp);
  if(disp & 0x800)
  {
    disp |= 0xF000;
  }
  signedDisp = static_cast<int16_t>(disp);

  valueRegB = RegisterFile::readFromGPRegister(regB);
  valueRegC = RegisterFile::readFromGPRegister(regC);
  valueRegA = RegisterFile::readFromGPRegister(regA);

  mode = instructionRegister[0] & 0xF;
  switch(mode)
  {
    case 0x0:
      address = valueRegA + valueRegB + signedDisp;
      Memory::memWrite4Bytes(address, valueRegC);
      break;
    case 0x1:
      valueRegA = valueRegA + signedDisp;
      Memory::memWrite4Bytes(valueRegA, valueRegC);
      RegisterFile::writeToGPRegister(regA, valueRegA);
      break;
    case 0x2:
      address = Memory::memRead4bytes(valueRegA + valueRegB + signedDisp);
      Memory::memWrite4Bytes(address, valueRegC);
      break;
  }

}

void CPU::instructionLoad()
{
  uint8_t mode, regA, regB, regC;
  uint16_t disp;
  int16_t signedDisp;
  uint32_t valueRegB, valueRegC, valueRegA, address;

  extractInstructionFiels(regA, regB, regC, disp);
  if(disp & 0x800)
  {
    disp |= 0xF000;
  }
  signedDisp = static_cast<int16_t>(disp);

  valueRegC = RegisterFile::readFromGPRegister(regC);
  valueRegA = RegisterFile::readFromGPRegister(regA);

  mode = instructionRegister[0] & 0xF;
  if(!mode || mode == 5)
  {
    valueRegB = RegisterFile::readFromSPRegister(regB);
  }
  else
  {
    valueRegB = RegisterFile::readFromGPRegister(regB);
  }

  switch(mode)
  {
    case 0x0:
      RegisterFile::writeToGPRegister(regA, valueRegB);
      break;
    case 0x1:
      valueRegA = valueRegB + signedDisp;
      RegisterFile::writeToGPRegister(regA, valueRegA);
      break;
    case 0x2:
      address = valueRegB + valueRegC + signedDisp;
      valueRegA = Memory::memRead4bytes(address);
      RegisterFile::writeToGPRegister(regA, valueRegA);
      break;
    case 0x3:
      valueRegA = Memory::memRead4bytes(valueRegB);
      RegisterFile::writeToGPRegister(regA, valueRegA);

      valueRegB = valueRegB + signedDisp;
      RegisterFile::writeToGPRegister(regB, valueRegB);
      break;
    case 0x4:
      RegisterFile::writeToSPRegister(regA, valueRegB);
      break;
    case 0x5:
      valueRegA = valueRegB | signedDisp;
      RegisterFile::writeToSPRegister(regA, valueRegA);
      break;
    case 0x6:
      address = valueRegB + valueRegC + signedDisp;
      valueRegA = Memory::memRead4bytes(address);
      RegisterFile::writeToSPRegister(regA, valueRegA);
      break;
    case 0x7:
      valueRegA = Memory::memRead4bytes(valueRegB);
      RegisterFile::writeToSPRegister(regA, valueRegA);
      
      valueRegB = valueRegB + signedDisp;
      RegisterFile::writeToGPRegister(regB, valueRegB);
      break;
  }
}

uint8_t CPU::extractRegisterA()
{
  uint8_t reg = (instructionRegister[1] >> 4) & 0xF;
  return reg;
}

uint8_t CPU::extractRegisterB()
{
  uint8_t reg = instructionRegister[1] & 0xF;
  return reg;
}

uint8_t CPU::extractRegisterC()
{
  uint8_t reg = (instructionRegister[2] >> 4) & 0xF;
  return reg;
}

uint16_t CPU::extractDisplay()
{
  uint16_t disp;
  disp = instructionRegister[2] & 0xF;
  disp <<= 8;
  disp |= instructionRegister[3];
  return disp; 
}

void CPU::extractInstructionFiels(uint8_t &regA, uint8_t &regB, uint8_t &regC, uint16_t &disp)
{
  regA = extractRegisterA();
  regB = extractRegisterB();
  regC = extractRegisterC();
  disp = extractDisplay();
}
