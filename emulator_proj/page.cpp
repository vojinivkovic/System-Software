#include "page.hpp"

void Page::write(const uint32_t &idx, const uint8_t &content)
{
  pageMemory[idx] = content;
}