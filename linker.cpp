#include "linker.hpp"
#include "elf_header.hpp"
#include "section.hpp"

void Linker::readElfFile(const std::string &fileName)
{
  ELFHeader::ELFHeaderType elfType; 
  size_t entry, phoff, shoff, phentsize, phnum, shentsize, shnum, shstrndx;
  std::vector<Section*> arrayOfSections;

  ELFHeader::readElfHeader(fileName, elfType, entry, phoff, shoff, phentsize, phnum, shentsize, shnum, shstrndx);

}