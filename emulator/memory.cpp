#include "memory.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include "memory_constant.hpp"
#include "../aux/exceptions.hpp"

std::unordered_map<uint32_t, Page*> Memory::pageTable;

void Memory::initializeMemory(const std::string& fileName)
{
  std::ifstream file(fileName);
  std::string line;

  while (std::getline(file, line))
  {
      size_t pos = line.find(':');
      if (pos == std::string::npos)
          continue;

      std::string addressStr = line.substr(0, pos);
      std::string bytesStr = line.substr(pos + 1);

      uint32_t address = std::stoul(addressStr, nullptr, 16);

      std::stringstream ss(bytesStr);
      std::string byteStr;

      while (ss >> byteStr)
      {
        uint8_t byte = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
        memWrite(address, byte);
        address++;
      }
  }
}

void Memory::memWrite(const uint32_t &address, const uint8_t &content)
{
  uint32_t pageNumber = address / PAGE_SIZE;
  auto it = pageTable.find(pageNumber);
  if(it == pageTable.end())
  {
    Page* newPage = new Page();
    pageTable[pageNumber] = newPage;
    newPage->write(address % PAGE_SIZE, content);
  }
  else
  {
    it->second->write(address % PAGE_SIZE, content);
  }
}

uint8_t Memory::memRead(const uint32_t &address)
{
  uint32_t pageNumber = address / PAGE_SIZE;
  auto it = pageTable.find(pageNumber);
  if(it == pageTable.end())
  {
    throw CPUErrors(ErrorType::ErrorUninitializedMemory, "Memory [" + std::to_string(address) + "] is not initialized");
  }
  return it->second->read(address % PAGE_SIZE);
}
