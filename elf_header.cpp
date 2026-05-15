#include "elf_header.hpp"
#include <vector>


ELFHeader::ELFHeaderType ELFHeader::type;
size_t ELFHeader::entry;
size_t ELFHeader::phoff;
size_t ELFHeader::shoff;
size_t ELFHeader::size = 10 * sizeof(size_t);
size_t ELFHeader::phentsize;
size_t ELFHeader::phnum;
size_t ELFHeader::shentsize;
size_t ELFHeader::shnum;
size_t ELFHeader::shstrndx;

void ELFHeader::makeELFHeader(const ELFHeaderType& type_, const size_t &entry_, const size_t &phoff_, const size_t &shoff_,
   const size_t &phentsize_, const size_t &phnum_, const size_t &shentsize_, const size_t &shnum_, 
   const size_t &shstrndx_)
{
  type = type_;
  entry = entry_;
  phoff = phoff_;
  shoff = shoff_;
  phentsize = phentsize_;
  phnum = phnum_;
  shentsize = shentsize_;
  shnum = shnum_;
  shstrndx = shstrndx_;
}

static std::vector<uint8_t> convertToLittleEndiand(size_t classMember)
{
  std::vector<uint8_t> littleEndianFormat;
  for(size_t i = 0; i < sizeof(size_t); i++)
  {
    littleEndianFormat.push_back(static_cast<uint8_t>((classMember >> (i * 8)) & 0xFF));
  }
  return littleEndianFormat;
}

std::vector<uint8_t> ELFHeader::getLittleEndianFormat()
{
  std::vector<uint8_t> tempContent;
  std::vector<uint8_t> container;

  container = convertToLittleEndiand(static_cast<size_t>(type));
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  container = convertToLittleEndiand(entry);
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  container = convertToLittleEndiand(phoff);
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  container = convertToLittleEndiand(shoff);
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  container = convertToLittleEndiand(size);
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  container = convertToLittleEndiand(phentsize);
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  container = convertToLittleEndiand(phnum);
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  container = convertToLittleEndiand(shentsize);
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  container = convertToLittleEndiand(shnum);
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  container = convertToLittleEndiand(shstrndx);
  tempContent.insert(tempContent.end(), container.begin(), container.end());

  return tempContent;
}

std::string ELFHeader::getStringFormat()
{
  std::string stringFormat;
  stringFormat = "Type: " + std::to_string(static_cast<size_t>(type)) + 
  ", Entry: " + std::to_string(entry) + 
  ", Program Header Offset: " + std::to_string(phoff) + 
  ", Section Header Offset: " + std::to_string(shoff) + 
  ", Size of Header: " + std::to_string(size) + 
  ", Size of Entry in PH: " + std::to_string(phentsize) + 
  ", Number of Entries in PH: " + std::to_string(phnum) + 
  ", Size of Entry in SH: " + std::to_string(shentsize) + 
  ", Number of Entries in SH: " + std::to_string(shnum) + 
  ", Number of Entry for Section of Section Names: " + std::to_string(shstrndx); 

  return stringFormat;
}
