#ifndef _INSTRUCTIONS_HPP_
#define _INSTRUCTIONS_HPP_

#include <unordered_map>
#include <vector>
#include <string>
#include "instructions_functions.hpp"
 

class Instructions
{
public:
  Instructions() = delete;
  Instructions(const Instructions& instr) = delete;
  Instructions& operator=(const Instructions& instr) = delete;
  static std::vector<uint8_t> translate(const std::string& instruction_name, const std::vector<Argument>& arguments);
  static bool resolveSymbol(const std::string& symbol, uint32_t* value, bool acknowledgeSymbol = true);

private:
  static std::unordered_map<std::string, std::vector<uint8_t> (*)(const std::vector<Argument>& arguments)> tableOfInstructions;
};

#endif