#ifndef _INSTRUCTIONS_FUNCTIONS_HPP_
#define _INSTRUCTIONS_FUNCTIONS_HPP_

#include <vector>
#include <string>

enum class ArgumentType
{
  Register,
  Operand
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

#endif