#include "assembler.hpp"


Section* Assembler::currentSection = nullptr;
std::vector<Section*> Assembler::arrayOfSections;

Section *Assembler::findSection(size_t nameOfSection)
{
  for(auto iSection: arrayOfSections)
  {
    if(iSection->getSectionName() == nameOfSection)
    {
      return iSection;
    }
  }  
  return nullptr;
}
