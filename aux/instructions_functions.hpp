#ifndef _INSTRUCTIONS_FUNCTIONS_HPP_
#define _INSTRUCTIONS_FUNCTIONS_HPP_

#include <vector>
#include <string>

enum class ArgumentType
{
  Register,
  OperandSymbol,
  OperandLiteral,
  RegisterAndSymbol,
  RegisterAndLiteral

};

enum class AddressingType
{
  MemoryDirect, 
  Immediate, 
  RegisterDirect,
  RegisterIndirect,
};

struct Argument 
{
  ArgumentType type;
  AddressingType addressing;
  uint8_t registerNum;
  std::string variable;
};


std::vector<uint8_t> instructionHalt(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructrionSoftwareInterrupt(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionJump(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionCall(const std::vector<Argument>& arguments);

std::vector<uint8_t> instuctionJumpEqual(const std::vector<Argument>& arguments);

std::vector<uint8_t> instuctionJumpNotEqual(const std::vector<Argument>& arguments);

std::vector<uint8_t> instuctionJumpGreater(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionAtomicExchange(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionAdd(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionSub(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionMul(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionDiv(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionNot(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionAnd(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionOr(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionXor(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionShiftLeft(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionShiftRight(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionPush(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionPop(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionReturnFromInterrupt(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionReturnFromFunction(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionReadFromCSRegister(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionWriteToCSRegister(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionStore(const std::vector<Argument>& arguments);

std::vector<uint8_t> instructionLoad(const std::vector<Argument>& arguments);

std::vector<uint8_t> transformLoadInstruction(const uint8_t& destReg, uint32_t operand);

#endif