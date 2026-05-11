#include "relocation_table.hpp"

std::vector<RelocationEntry*> RelocationTable::table;

void RelocationTable::fixRelocationEntry(size_t idx, size_t newIdxOfSymbol, size_t addend)
{
  table[idx]->setIdxSymbol(newIdxOfSymbol);
  table[idx]->setAddend(addend);
}
