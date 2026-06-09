#include "memory.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include "memory_constant.hpp"
#include "../aux/exceptions.hpp"
#include "terminal.hpp"
#include "timer.hpp"
#include "register_file.hpp"
#include "cpu.hpp"

std::unordered_map<uint32_t, Page*> Memory::pageTable;

void Memory::initializeMemory(const std::string& fileName)
{
  std::ifstream file(fileName);
  std::string line;

  if (!file)
  {
    throw std::runtime_error("Failed to open file: " + fileName);
  }

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

void Memory::memWrite4Bytes(const uint32_t &address, const uint32_t &content)
{
  uint8_t byteContent;
  for(size_t i = 0; i < 4; i++)
  {
    byteContent = (content >> (i * 8)) & 0xFF;
    memWrite(address + i, byteContent);
  }
  if(address == 0xFFFFFF00)
  {
    //std::cout << "Display char" << std::endl;
    Terminal::displayCharacter();
  }
  else if(address == 0xFFFFFF10)
  {
    std::cout << "Change timer config" << std::endl;
    Timer::changeConfig();
  }
}

void Memory::pushOnStack(const uint32_t &value)
{
  uint32_t sp = RegisterFile::readFromGPRegister(14);
  for(size_t i = 0; i < 4; i++)
  {
    sp--;
    uint8_t byteValue = (value >> ((3 - i) * 8)) & 0xFF;
    memWrite(sp, byteValue); 
  }
  RegisterFile::writeToGPRegister(14, sp);
}

uint8_t Memory::memRead(const uint32_t &address)
{
  uint32_t pageNumber = address / PAGE_SIZE;
  auto it = pageTable.find(pageNumber);
  if(it == pageTable.end())
  {
    uint32_t pc = RegisterFile::readFromGPRegister(15);
    RegisterFile::getStateOfRegisterFile();
    throw CPUErrors(ErrorType::ErrorUninitializedMemory, "Memory [" + std::to_string(address) + "] is not initialized, PC: " \
    + std::to_string(CPU::getNumberOfInstructions()));

  }
  return it->second->read(address % PAGE_SIZE);
}

uint32_t Memory::memRead4bytes(const uint32_t &address)
{
  uint32_t value = 0;
  uint8_t byteContent;
  for(size_t i = 0; i < 4; i++)
  {
    byteContent = memRead(address + i);
    value |= static_cast<uint32_t>(byteContent) << 8 * i;
  }
  return value;
}
