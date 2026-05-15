#ifndef _RELOCATION_TABLE_HPP_
#define _RELOCATION_TABLE_HPP_

#include "relocation_entry.hpp"
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
  static void makeSection();
private:
  static std::vector<RelocationEntry*> table;
};

#endif