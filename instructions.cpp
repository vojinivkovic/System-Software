#include "instructions.hpp"
#include "instructions_functions.hpp"

std::unordered_map<std::string, std::vector<uint8_t> (*)(const std::vector<Argument>& arguments)> Instructions::tableOfInstructions = {
  {"halt", instructionHalt},
  {"int", instructrionSoftwareInterrupt}
};

std::vector<uint8_t> Instructions::translate(const std::string &instruction_name, const std::vector<Argument> &arguments)
{

  return tableOfInstructions[instruction_name](arguments);
}
