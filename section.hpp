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
  enum class SectionType 
  {
    NullSection, 
    ProgbitsSection, 
    SymTabSection,
    HeaderSectionTabSection,
    SymStrTabSection,
    SectionStrTabSection, 
    RelaSection
  };
  Section(const std::string& sectionName, const SectionType& type_);
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
  void setContent (const std::vector<uint8_t>& content_) { content = content_; }
  void setTextContent (const std::vector<std::string>& textContent_) { textContent = textContent_; }
  void incrementLocationCounter(size_t increment) { locationCounter += increment; }
  static void makeSectionOfSectionNames();

private:
  size_t name, locationCounter, idxSection;
  SectionType type;
  std::vector<uint8_t> content;
  std::vector<std::string> textContent;
  static StringTable* tableOfSectionString;
  void textRepresentationOfInstruction(const std::string& command, const std::vector<Argument>& arguments);
};



#endif