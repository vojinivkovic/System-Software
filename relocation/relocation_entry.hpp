#ifndef _RELOCATION_ENTRY_HPP_
#define _RELOCATION_ENTRY_HPP_
#include <cstdlib>

enum class RelocationType
{
  Absolute
};

class RelocationEntry
{
public:
  RelocationEntry(size_t offset_, size_t idxSection_, size_t idxSymbol_, RelocationType type_ = RelocationType::Absolute) :
  offset(offset_), idxSection(idxSection_), idxSymbol(idxSymbol_) 
  {
    
  }

private:
  size_t offset, idxSection, idxSymbol;
  RelocationType typeOfRelocations;
};

#endif