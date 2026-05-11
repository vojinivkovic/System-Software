#ifndef _SECTION_HPP_
#define _SECTION_HPP_


#include <cstdlib>
#include <cstdint>
#include "aux/string_table.hpp"
#include <vector>
#include <string>
#include "aux/directives.hpp"
#include "aux/instructions.hpp"
#include "aux/directive_functions.hpp"

class Section
{
public:
  Section(const std::string& sectionName);
  size_t getLocationCounter() const { return locationCounter;}
  size_t getIdxOfSection() const { return idxSection; }
  int translateInstruction(const std::string& instruction, const std::vector<Argument>& arguments); 
  int executeDirective(const std::string& command, const std::vector<MacroParameter>& parameters);
  void defineSymbol(const std::string& symbolName);
  int copyContent();
  static StringTable* getStringTable() { return tableOfSectionString; }
  static std::string::size_type findSectionInStringTable(const std::string& sectionName) { return tableOfSectionString->findString(sectionName); }
  size_t getSectionName() const { return name; }
  std::vector<uint8_t> getContent() const { return content; }
  void callocMemory(size_t sizeOfAllocation);
  void insertString(const std::string& stringToInsert);
  void insertContent(const uint32_t& value);
  void insertValueInContent(const uint32_t& value, size_t offset);
  static StringTable* getTableOfSectionStrings() { return tableOfSectionString; }
private:
  size_t name, locationCounter, idxSection;
  std::vector<uint8_t> content;
  std::vector<std::string> textContent;
  static StringTable* tableOfSectionString;
  void textRepresentationOfInstruction(const std::string& command, const std::vector<Argument>& arguments);
};



#endif