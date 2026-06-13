#include "string_table.hpp"
#include "../assembler.hpp"
#include <cstring>
#include <iostream>

void StringTable::addString(const std::string& name)
{
  tableOfNames += name;
  tableOfNames.push_back('\0');
}

std::string::size_type StringTable::findString(const std::string &name) const
{
  size_t pos = 0;
  while (pos < tableOfNames.size()) 
  {
    size_t len = std::strlen(tableOfNames.data() + pos);

    if (len == name.size() && tableOfNames.compare(pos, len, name) == 0) 
    {
      return pos;
    }

    pos += len + 1;
  }

  return std::string::npos; 
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
  sectionOfStrings = new Section(name, type);
  Assembler::addSection(sectionOfStrings);
}

void StringTable::makeContentOfSection()
{
  std::vector<uint8_t> content;
  std::vector<std::string> textContent;

  for(size_t i = 0; i < tableOfNames.size(); i++)
  {
    content.push_back(static_cast<uint8_t>(tableOfNames[i]));
    sectionOfStrings->incrementLocationCounter(1);
  }
  textContent.push_back(tableOfNames);
  sectionOfStrings->setContent(content);
  sectionOfStrings->setTextContent(textContent);
}
