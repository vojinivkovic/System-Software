#include "auxiliary_func.hpp"

std::vector<std::string> parseArguments(const std::string& arguments)
{
  std::vector<std::string> listOfArguments;
  std::string::size_type currentIdx = 0, limitIdx = arguments.find("/", currentIdx);
  std::string argument;

  while(limitIdx != std::string::npos)
  {
    argument = arguments.substr(currentIdx, limitIdx - currentIdx);
    listOfArguments.push_back(argument);
    currentIdx = limitIdx + 1;
    limitIdx = arguments.find("/", currentIdx); 
  }
  argument = arguments.substr(currentIdx);
  listOfArguments.push_back(argument);

  return listOfArguments;
}