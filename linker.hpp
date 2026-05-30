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
#include <unordered_map>

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
  static void makeLinkerSymbolTable();
  static void makeLinkerRelocationEntries();
  static void addSectionMapping(const std::string& secionName, const uint32_t& memAddress);
  static void addFileName(const std::string& fileName) { files.push_back(fileName); }

private:

  static std::vector<StringTable*> arrayOfSymbolStringsTables;
  static std::vector<StringTable*> arrayOfSectionStringTables;
  static std::vector<std::vector<Section*>> arrayOfFilesSections;
  static std::vector<std::vector<Symbol*>> arrayOfSymbolTables;
  static std::vector<Section*> linkerSections;
  static std::vector<Symbol*> linkerSymbols;
  static std::vector<RelocationEntry*> linkerReloactionEntries;
  static std::vector<std::string> files;
  static std::map<std::pair<size_t, size_t>, std::pair<size_t, size_t>> mappingFileSectionToSectionOffset;
  static std::map<std::pair<size_t, size_t>, size_t> mappingOfSymbols;
  static StringTable* sectionStringTable;
  static StringTable* symbolStringTable;
  static std::vector<std::vector<RelocationEntry*>> arrayOfRelocationEntryTables;
  static void adjustOffset();
  static size_t findValueOfSymbol(const size_t& currentFile, const std::string& symbolName);
  static std::unordered_map<std::string, uint32_t> placeMapping;
  static void fixVirtualAddressOfSections();
  static void addOffsetToSections(std::map<std::pair<size_t, size_t>, std::pair<size_t, size_t>>& map, const size_t& idxFile, const size_t& idxSection);
  static void fixLinkerSymbolTable(const Symbol* symbol, const size_t& symbolName, const size_t& idxFile, const size_t& idxSymbol);

};

#endif