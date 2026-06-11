#include "relocation_table.hpp"
#include "../section.hpp"
#include "../assembler.hpp"
#include <string>
#include <iostream>
#include <fstream>

std::vector<RelocationEntry*> RelocationTable::table;

void RelocationTable::fixRelocationEntry(size_t idx, size_t newIdxOfSymbol, size_t addend)
{
  table[idx]->setIdxSymbol(newIdxOfSymbol);
  table[idx]->setAddend(addend);
}

void RelocationTable::addjustRelocationOffset(const size_t& idxSection, const size_t& offsetOfSymbol, const size_t& shift)
{
  for(auto iReloc : table)
  {
    if(iReloc->getIdxSection() == idxSection && iReloc->getOffset() > offsetOfSymbol)
    {
      iReloc->setOffset(iReloc->getOffset() + shift);
    }
  }
}

void RelocationTable::makeSection()
{
  Section* newSection = new Section(".rela", Section::SectionType::RelaSection);
  std::vector<uint8_t> content;
  std::vector<uint8_t> subContent;
  std::string subContentString;
  std::vector<std::string> textContent;
  for(auto iRelocationEntry: table)
  {
    subContent = iRelocationEntry->getLittleEndianFormatOfRE();
    content.insert(content.end(), subContent.begin(), subContent.end());
    newSection->incrementLocationCounter(4 * sizeof(size_t));
    subContentString = "Offset: " + std::to_string(iRelocationEntry->getOffset()) + 
      ", idxSection: " + std::to_string(iRelocationEntry->getIdxSection()) + 
      ", idxSymbol: " + std::to_string(iRelocationEntry->getIdxSymbol()) + 
      ", Addend: " + std::to_string(iRelocationEntry->getAddend());
    
      textContent.push_back(subContentString);
  }
  newSection->setContent(content);
  newSection->setTextContent(textContent);
  Assembler::addSection(newSection);
}
static size_t readAndConvertFromLittleEndian(const size_t& startIdx, const std::vector<uint8_t>& bytes)
{
  size_t value = 0;
  for(size_t i = 0; i < sizeof(size_t); i++)
  {
    value = (value << 8) | (bytes[startIdx + sizeof(size_t) - 1 - i]);
  }
  return value;
}

static RelocationEntry* readRelocationEntryFromTable(std::ifstream& inputFile)
{
  std::vector<uint8_t> bytes;
  std::string hexByte;
  size_t currIdx = 0, offset, idxSection, idxSymbol, addend;

  for(size_t i = 0; i < 4 * sizeof(size_t); i++)
  {
    inputFile >> hexByte;
    bytes.push_back(static_cast<uint8_t>(std::stoul(hexByte, nullptr, 16))); 
  }

  offset = readAndConvertFromLittleEndian(currIdx, bytes);
  currIdx += sizeof(size_t);

  idxSection = readAndConvertFromLittleEndian(currIdx, bytes);
  currIdx += sizeof(size_t);

  idxSymbol = readAndConvertFromLittleEndian(currIdx, bytes);
  currIdx += sizeof(size_t);

  addend = readAndConvertFromLittleEndian(currIdx, bytes);
  currIdx += sizeof(size_t);

  return new RelocationEntry(offset, idxSection, idxSymbol, addend);
}
std::vector<RelocationEntry *> RelocationTable::readRelocationTableFromElfFile(const std::string &fileName, const Section *relaTable)
{
  // std::cout << std::to_string(relaTable->getSizeOfEntry()) << std::endl;
  // std::cout << std::to_string(relaTable->getLocationCounter()) << std::endl;
  // std::cout << std::to_string(relaTable->getOffsetInFile()) << std::endl;

  std::vector<RelocationEntry*> arrayOfRelocationEntries;
  size_t offset = relaTable->getOffsetInFile();
  size_t offsetInFile, numOfRows = offset / 4, numOfCol = offset % 4;
  std::ifstream inputFile(fileName);
  offsetInFile = numOfRows * 13 + numOfCol * 3;

  // std::cout << std::to_string(offset) << std::endl;
  

  size_t numOfRelocationEntries = relaTable->getLocationCounter() / relaTable->getSizeOfEntry();

  inputFile.seekg(offsetInFile);
  for(size_t i = 0; i < numOfRelocationEntries; i++)
  {
    arrayOfRelocationEntries.push_back(readRelocationEntryFromTable(inputFile));
  }
  return arrayOfRelocationEntries;
}

void RelocationTable::deleteTableOfRelocationEntries()
{
  for(auto iReloc : table)
  {
    delete iReloc;
  }
}
