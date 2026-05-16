#include "string_table.hpp"
#include "../assembler.hpp"

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

std::string StringTable::getNameOfElement(const size_t &startIdx) const
{
  std::string name;
  for(size_t i = startIdx; tableOfNames[i] != '\0'; i++)
  {
    name.push_back(tableOfNames[i]);
  }
  return name;
}

void StringTable::makeSection(const std::string &name, const Section::SectionType& type)
{
  Section* newSection = new Section(name, type);
  std::vector<uint8_t> content;
  std::vector<std::string> textContent;

  for(size_t i = 0; i < tableOfNames.size(); i++)
  {
    content.push_back(static_cast<uint8_t>(tableOfNames[i]));
    newSection->incrementLocationCounter(1);
  }
  textContent.push_back(tableOfNames);
  newSection->setContent(content);
  newSection->setTextContent(textContent);
  
  Assembler::addSection(newSection);
}
