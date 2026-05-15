#ifndef _ELF_HEADER_HPP_
#define _ELF_HEADER_HPP_
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <string>

class ELFHeader
{ 
  public:
    enum class ELFHeaderType
    {
      ELF_NONE, 
      ELF_REL, 
      ELF_EXEC
    };
    ELFHeader() = delete;
    ELFHeader(const ELFHeader& header) = delete;
    ELFHeader& operator=(const ELFHeader& header) = delete;

    static void makeELFHeader(const ELFHeaderType& tupe_, const size_t& entry_, 
      const size_t& phoff_, const size_t& shoff_, const size_t& phentsize_,
    const size_t& phnum_,const size_t& shentsize_,const size_t& shnum_, const size_t& shstrndx_);

    static std::vector<uint8_t> getLittleEndianFormat();
    static std::string getStringFormat();
  private:
    static ELFHeaderType type;
    static size_t entry, phoff, shoff, size, phentsize, phnum, shentsize, shnum, shstrndx;

};

#endif