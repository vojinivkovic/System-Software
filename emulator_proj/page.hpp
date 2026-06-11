#ifndef _PAGE_HPP_
#define _PAGE_HPP_
#include <cstdint>
#include "memory_constant.hpp"

class Page
{
public:

  Page() = default;
  ~Page() = default;
  void write(const uint32_t& idx, const uint8_t& content);
  uint8_t read(const uint32_t& idx) { return pageMemory[idx]; }

private:
  uint8_t pageMemory[PAGE_SIZE];

};
#endif