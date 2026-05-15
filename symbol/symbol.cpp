#include "symbol.hpp"


static std::vector<uint8_t> convertToLittleEndiand(size_t classMember)
{
  std::vector<uint8_t> littleEndianFormat;
  for(size_t i = 0; i < sizeof(size_t); i++)
  {
    littleEndianFormat.push_back(static_cast<uint8_t>((classMember >> (i * 8)) & 0xFF));
  }
  return littleEndianFormat;
}

std::vector<uint8_t> Symbol::getLittleEndianFormatOfSymbol() const
{
  std::vector<uint8_t> tempContent;
  std::vector<uint8_t> container;

  container = convertToLittleEndiand(name);
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  container = convertToLittleEndiand(size);
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  container = convertToLittleEndiand(value);
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  container = convertToLittleEndiand(section);
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  container = convertToLittleEndiand(static_cast<size_t>(bind));
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  container = convertToLittleEndiand(static_cast<size_t>(type));
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  container = convertToLittleEndiand(static_cast<size_t>(scope));
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  return tempContent;
}