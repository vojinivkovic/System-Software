#ifndef _STRING_TABLE_HPP_
#define _STRING_TABLE_HPP_


#include <string>


class StringTable
{
public:

  enum class STType 
  {
    SectionName,
    SymbolName,
    MacroName
  };

  StringTable(STType tableType) : type(tableType) {}
  void addString(const std::string& name);
  size_t getOffset() { return tableOfNames.size(); }
  std::string::size_type findString(const std::string& name);
  std::string getNames() const { return tableOfNames; }
  void removeName(const size_t& start, const size_t& end) { tableOfNames.erase(start, end-start); }

private:
  STType type;
  std::string tableOfNames;
  
};

#endif