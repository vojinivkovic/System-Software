#include "instructions.hpp"
#include "instructions_functions.hpp"

std::unordered_map<std::string, void (*)(const std::vector<std::string>& arguments)> Instructions::tableOfInstructions = {
  {"halt", instruction_halt}
};

void Instructions::translate(const std::string &instruction_name, const std::vector<std::string> &arguments)
{
  tableOfInstructions[instruction_name](arguments);
}
