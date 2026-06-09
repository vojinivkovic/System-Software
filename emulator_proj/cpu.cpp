#include "cpu.hpp"
#include "register_file.hpp"
#include "memory.hpp"
#include "terminal.hpp"
#include "timer.hpp"
#include <string>
#include <iomanip>

uint8_t CPU::instructionRegister[4];
void (*CPU::instructionSet[10])();
size_t CPU::numOfInstructions = 0;

void CPU::initializeCPU(const std::string& fileName)
{
  Memory::initializeMemory(fileName);
  initializeTableOfInstructions();
  RegisterFile::writeToGPRegister(15, 0x40000000);
  Terminal::initializeTerminal();
  Timer::initializeTimer();
}

void CPU::runProgram()
{

  while(true)
  {
    //RegisterFile::getStateOfRegisterFile();
    // if(counter < 26)
    // {
    //   RegisterFile::getStateOfRegisterFile();
    //   counter++;
    // }
    fetchInstruction();
    InstructionOPCodes opCode = decode();
    if(opCode == InstructionOPCodes::HALT)
    {
      break;
    }
    executeInstruction(opCode);
    Terminal::pollKeyboard();
    Timer::poll();
    checkForInterrupt();
    numOfInstructions++;
  }
  Terminal::restoreTerminalSettings();
  RegisterFile::getStateOfRegisterFile();
}

void CPU::fetchInstruction()
{
  uint32_t pc = RegisterFile::readFromGPRegister(15);

  for(size_t i = 4; i-- > 0; )
  {
    instructionRegister[i] = Memory::memRead(pc);
    pc++;
  }
  RegisterFile::writeToGPRegister(15, pc);
}

CPU::InstructionOPCodes CPU::decode()
{
  uint8_t opCode = (instructionRegister[0] & 0xF0) >> 4;
  return static_cast<CPU::InstructionOPCodes>(opCode);
}

void CPU::executeInstruction(InstructionOPCodes code)
{
  size_t idxCode = static_cast<size_t>(code);
  if(idxCode < 0 || idxCode > 9)
  {
    RegisterFile::writeToSPRegister(2, 1);
    return;
  }
  instructionSet[idxCode]();
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
  RegisterFile::writeToSPRegister(0, regStatusValue);
  RegisterFile::writeToGPRegister(15, regHandlerValue);

}

void CPU::instructionCall()
{
  //PUSH PC
  uint32_t sp = RegisterFile::readFromGPRegister(14);
  uint32_t pc = RegisterFile::readFromGPRegister(15);
  Memory::pushOnStack(pc);

  //std::cout << "call instruction" << std::endl;
  uint8_t regA, regB, regC;
  uint16_t disp;
  extractInstructionFiels(regA, regB, regC, disp);
  uint32_t newPC = RegisterFile::readFromGPRegister(regA) + RegisterFile::readFromGPRegister(regB) + disp;
  RegisterFile::writeToGPRegister(15, newPC);

  // std::cout << "PC CHANGE[CALL]: 0x"
  //     << std::hex
  //     << std::setw(8)
  //     << std::setfill('0')
  //     << newPC
  //     << std::endl;
    

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
        // std::cout << "PC CHANGE[JUMP]: 0x"
        // << std::hex
        // << std::setw(8)
        // << std::setfill('0')
        // << newPC
        // << std::endl;
      break;
    case 0x1:
      if(valueRegB == valueRegC)
      {
        newPC = valueRegA + disp;
        RegisterFile::writeToGPRegister(15, newPC);
        // std::cout << "PC CHANGE[BEQ]: 0x"
        // << std::hex
        // << std::setw(8)
        // << std::setfill('0')
        // << newPC
        // << std::endl;
        // std::cout << "value B: " << std::to_string(valueRegB) << ", value C: " 
        // << std::to_string(valueRegC) << std::endl;
      }
      break;
    case 0x2:
      if(valueRegB != valueRegC)
      {
        newPC = valueRegA + disp;
        RegisterFile::writeToGPRegister(15, newPC);
        // std::cout << "PC CHANGE[BNEQ]: 0x"
        // << std::hex
        // << std::setw(8)
        // << std::setfill('0')
        // << newPC
        // << std::endl;
      }
      break;
    case 0x3:
      if(static_cast<int>(valueRegB) > static_cast<int>(valueRegC))
      {
        newPC = valueRegA + disp;
        // std::cout << "PC CHANGE[SIGNED]: 0x"
        // << std::hex
        // << std::setw(8)
        // << std::setfill('0')
        // << newPC
        // << std::endl;
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
  // std::cout << "mode: " << mode << std::endl;
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
      // if(regA == 15) 
      // {
      //   // std::cout << "PC CHANGE[POP]: 0x"
      //   // << std::hex
      //   // << std::setw(8)
      //   // << std::setfill('0')
      //   // << valueRegA
      //   // << std::endl;

      //   // std::cout << "STATUS REG: " << RegisterFile::readFromSPRegister(0) << std::endl;
      // }
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
      
      // std::cout << "WRITING TO SP REG [" << regA << "] = " << valueRegA << std::endl;

      valueRegB = valueRegB + signedDisp;
      RegisterFile::writeToGPRegister(regB, valueRegB);
      break;
  }
}

void CPU::initializeTableOfInstructions()
{
  instructionSet[static_cast<size_t>(InstructionOPCodes::INT)] = &instructionSoftwareInterrupt;
  instructionSet[static_cast<size_t>(InstructionOPCodes::CALL)] = &instructionCall;
  instructionSet[static_cast<size_t>(InstructionOPCodes::JUMP)] = &instructionJump; 
  instructionSet[static_cast<size_t>(InstructionOPCodes::EXCHG)] = &instructionExchg;
  instructionSet[static_cast<size_t>(InstructionOPCodes::ARITH)] = &instructionArithmetic;  
  instructionSet[static_cast<size_t>(InstructionOPCodes::LOG)] = &instructionLogic; 
  instructionSet[static_cast<size_t>(InstructionOPCodes::SHIFT)] = &instructionShift; 
  instructionSet[static_cast<size_t>(InstructionOPCodes::ST)] = &instructionStore; 
  instructionSet[static_cast<size_t>(InstructionOPCodes::LD)] = &instructionLoad;   
}

void CPU::checkForInterrupt()
{
  uint32_t regStatusValue, regHandlerValue;
  uint8_t regCauseValue, maskGlobalInterrupts, maskTimerInterrupt, maskTerminalInterrupt;
  regStatusValue = RegisterFile::readFromSPRegister(0);
  regHandlerValue = RegisterFile::readFromSPRegister(1);
  regCauseValue = static_cast<uint8_t>(RegisterFile::readFromSPRegister(2));
  
  maskGlobalInterrupts = regStatusValue & 0b100;
  maskTimerInterrupt = regStatusValue & 0b1;
  maskTerminalInterrupt = regStatusValue & 0b10;
  
  if(regCauseValue == 1 ||
  (!maskGlobalInterrupts && !maskTimerInterrupt && regCauseValue == 2) || 
  (!maskGlobalInterrupts && !maskTerminalInterrupt && regCauseValue == 3)) 
  {
    uint32_t regPCValue = RegisterFile::readFromGPRegister(15);

    // std::cout << "JUMP TO HANDLER" << std::endl;
    Memory::pushOnStack(regStatusValue);
    Memory::pushOnStack(regPCValue);

    regStatusValue = regStatusValue | 0b100;
    RegisterFile::writeToSPRegister(0, regStatusValue);
    RegisterFile::writeToGPRegister(15, regHandlerValue);
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
