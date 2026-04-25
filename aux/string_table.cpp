#include "string_table.hpp"

void StringTable::addString(const std::string& name)
{
  tableOfNames += name;
  tableOfNames.push_back('\0');
}

std::string::size_type StringTable::findString(const std::string &name)
{
  std::string::size_type findPosition = tableOfNames.find(name);
  return findPosition;
}
