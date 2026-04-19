#include "string_table.hpp"

void StringTable::addString(const char *name)
{
  tableOfNames += name;
  tableOfNames.push_back('\0');
}
