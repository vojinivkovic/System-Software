#ifndef _RELOCATION_ENTRY_HPP_
#define _RELOCATION_ENTRY_HPP_
#include <cstdlib>
#include <cstdint>
#include <vector>

enum class RelocationType
{
  Absolute
};

class RelocationEntry
{
public:
  RelocationEntry(size_t offset_, size_t idxSection_, size_t idxSymbol_, size_t addend_, RelocationType type_ = RelocationType::Absolute) :
  offset(offset_), idxSection(idxSection_), idxSymbol(idxSymbol_), addend(addend_), typeOfRelocations(type_) {}
  size_t getIdxSymbol() const { return idxSymbol;}
  void setIdxSymbol(size_t idx) { idxSymbol = idx; }
  void setOffset(size_t offset_) { offset = offset_; }
  void setAddend(size_t newAddend) { addend = newAddend; } 
  size_t getOffset() const { return offset;}
  size_t getIdxSection() const { return idxSection; }
  size_t getAddend() const { return addend; }
  std::vector<uint8_t> getLittleEndianFormatOfRE() const;

private:
  size_t offset, idxSection, idxSymbol, addend;
  RelocationType typeOfRelocations;
};

#endif