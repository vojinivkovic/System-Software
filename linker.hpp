#ifndef _LINKER_HPP_
#define _LINKER_HPP_
#include "symbol/symbol_table.hpp"
#include <vector>
#include "aux/string_table.hpp"
#include "section.hpp"
#include "symbol/symbol.hpp"
#include "aux/exceptions.hpp"
#include <map>
#include <utility>
#include "relocation/relocation_entry.hpp"

class Linker
{
public:
  Linker() = delete;
  Linker(const Linker& link) = delete;
  Linker& operator=(const Linker& link) = delete;
  static void readElfFile(const std::string& fileName);
  static void checkMultipleDefinitions();
  static void checkUnresolvedSymbols();
  static void makeLinkerSections();
  static void fixRelocationEntries();


private:

  static std::vector<StringTable*> arrayOfSymbolStringsTables;
  static std::vector<StringTable*> arrayOfSectionStringTables;
  static std::vector<std::vector<Section*>> arrayOfFilesSections;
  static std::vector<std::vector<Symbol*>> arrayOfSymbolTables;
  static std::vector<Section*> linkerSections;
  static std::vector<Symbol*> linkerSymbols;
  static std::vector<std::string> files;
  static std::map<std::pair<size_t, size_t>, std::pair<size_t, size_t>> mappingFileSectionToSectionOffset;
  static StringTable* sectionStringTable;
  static StringTable* symbolStringTable;
  static std::vector<std::vector<RelocationEntry*>> arrayOfRelocationEntryTables;
  static void adjustOffset();
  static size_t findValueOfSymbol(const size_t& currentFile, const std::string& symbolName);
};

#endif