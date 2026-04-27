#ifndef _SECTION_HPP_
#define _SECTION_HPP_


#include <cstdlib>
#include <cstdint>
#include "aux/string_table.hpp"
#include <vector>
#include <string>
#include "aux/directives.hpp"
#include "aux/instructions.hpp"

class Section
{
public:
  Section(const std::string& sectionName);
  size_t getLocationCounter() const { return locationCounter;}
  size_t getIdxOfSection() const { return idxSection; }
  int translateInstruction(const std::string& instruction, const std::vector<Argument>& arguments); 
  int executeDirective(const std::string& line);
  int copyContent();
  static StringTable* getStringTable() { return tableOfSectionString; }
  size_t getSectionName() const { return name; }
  std::vector<uint8_t> getContent() const { return content; }
  void callocMemory(size_t sizeOfAllocation);
  void insertString(const std::string& stringToInsert);

private:
  size_t name, locationCounter, idxSection;
  std::vector<uint8_t> content;
  static StringTable* tableOfSectionString;

};



#endif