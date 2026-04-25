#include "instructions_functions.hpp"
#include "assembler.hpp"

std::vector<uint8_t> instructionHalt(const std::vector<Argument> &arguments)
{
  return std::vector<uint8_t>{0x00, 0x00, 0x00, 0x00};
}

std::vector<uint8_t> instructrionSoftwareInterrupt(const std::vector<Argument> &arguments)
{
  return std::vector<uint8_t>{0x10, 0x00, 0x00, 0x00};
}
