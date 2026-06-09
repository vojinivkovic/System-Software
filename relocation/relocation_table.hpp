#ifndef _RELOCATION_TABLE_HPP_
#define _RELOCATION_TABLE_HPP_

#include "relocation_entry.hpp"
#include <string>
#include "../section.hpp"
#include <vector>

class RelocationTable
{
public:
  RelocationTable() = delete;
  RelocationTable(const RelocationTable& rela) = delete;
  RelocationTable& operator=(const RelocationTable& rela) = delete;
  static void addRelocationEntry(RelocationEntry* newEntry) { table.push_back(newEntry); }
  static std::vector<RelocationEntry*> getTable() { return table; }
  static void fixRelocationEntry(size_t idx, size_t newIdxOfSymbol, size_t addend);
  static void addjustRelocationOffset(const size_t& idxSection, const size_t& offsetOfSymbol, const size_t& shift);
  static void makeSection();
  static std::vector<RelocationEntry*> readRelocationTableFromElfFile(const std::string& fileName, const Section* relaTable);
private:
  static std::vector<RelocationEntry*> table;
};

#endif