#ifndef _STRING_TABLE_HPP_
#define _STRING_TABLE_HPP_


#include <string>


class StringTable
{
public:

  enum class STType 
  {
    SectionName,
    SymbolName
  };

  StringTable(STType tableType) : type(tableType) {}
  void addString(const char* name);
  size_t getOffset() { return tableOfNames.size(); }

private:
  STType type;
  std::string tableOfNames;
  
};

#endif