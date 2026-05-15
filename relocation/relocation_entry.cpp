#include "relocation_entry.hpp"

static std::vector<uint8_t> convertToLittleEndiand(size_t classMember)
{
  std::vector<uint8_t> littleEndianFormat;
  for(size_t i = 0; i < sizeof(size_t); i++)
  {
    littleEndianFormat.push_back(static_cast<uint8_t>((classMember >> (i * 8)) & 0xFF));
  }
  return littleEndianFormat;
}

std::vector<uint8_t> RelocationEntry::getLittleEndianFormatOfRE() const
{
  std::vector<uint8_t> tempContent;
  std::vector<uint8_t> container;

  container = convertToLittleEndiand(offset);
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  container = convertToLittleEndiand(idxSection);
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  container = convertToLittleEndiand(idxSymbol);
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  container = convertToLittleEndiand(addend);
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  return tempContent;
  
}