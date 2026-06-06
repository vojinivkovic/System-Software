#ifndef _MEMORY_HPP_
#define _MEMORY_HPP_
#include "page.hpp"
#include <unordered_map>
#include <string>

class Memory
{
public:
  Memory() = delete;
  Memory(const Memory& mem) = delete;
  Memory& operator=(const Memory& mem) = delete;
  static void initializeMemory(const std::string& fileName);
  static void memWrite(const uint32_t& address, const uint8_t& content);
  static uint8_t memRead(const uint32_t& address);
private:
  static std::unordered_map<uint32_t, Page*> pageTable;
};

#endif