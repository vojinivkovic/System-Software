#ifndef _LINKER_HPP_
#define _LINKER_HPP_
#include "symbol/symbol_table.hpp"
#include <vector>
#include "aux/string_table.hpp"
#include "section.hpp"

class Linker
{
public:
  Linker() = delete;
  Linker(const Linker& link) = delete;
  Linker& operator=(const Linker& link) = delete;
  static void readElfFile(const std::string& fileName);

private:

  static std::vector<StringTable*> arrayOfSymbolStringsTables;
  static std::vector<StringTable*> arrayOfSectionStringTables;
  static std::vector<std::vector<Section*>> arrayOfFilesSections;

};

#endif