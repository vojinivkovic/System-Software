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
private:
  static std::vector<RelocationEntry*> table;
};

#endif