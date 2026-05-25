#ifndef _STRING_TABLE_HPP_
#define _STRING_TABLE_HPP_


#include <string>
#include "../section.hpp"

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
  StringTable(STType tableType, const std::string& content) : type(tableType), tableOfNames(content) {}
  void addString(const std::string& name);
  size_t getOffset() { return tableOfNames.size(); }
  std::string::size_type findString(const std::string& name) const;
  std::string getNames() const { return tableOfNames; }
  void removeName(const size_t& start, const size_t& end) { tableOfNames.erase(start, end-start); }
  std::string getNameOfElement(const size_t& startIdx) const;
  void makeSection(const std::string& name, const Section::SectionType& type);
  void makeContentOfSection();

private:
  STType type;
  std::string tableOfNames;
  Section* sectionOfStrings;
  
};

#endif