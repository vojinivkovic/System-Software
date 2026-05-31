#include "linker.hpp"
#include "aux/exceptions.hpp"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstring>

enum class OptionType
{
  Hex, 
  Relocatable, 
  Place, 
  Other
};

static int findOption(int argc, char* argv[])
{
  for(int i = 0; i < argc; i++)
  {
    if(std::strcmp(argv[i], "-o") == 0)
    {
      return i;
    }
  }
  return -1;
}
static OptionType inspectCommandArgument(char* argument)
{
  std::string arg(argument);

  if(arg == "-hex")
  {
    return OptionType::Hex;
  }

  if(arg == "-relocatable")
  {
    return OptionType::Relocatable;
  }

  if(arg.rfind("-place=", 0) == 0)
  {
    return OptionType::Place;
  }

  return OptionType::Other;
}


static void parsePlaceArgument(char* argument, std::string& sectionName, uint32_t& address)
{
  std::string arg(argument);

  arg = arg.substr(7);
  size_t atPos = arg.find('@');

  if(atPos == std::string::npos)
  {
    throw std::runtime_error("Invalid -place format. Linker expects -place=section@address");
  }

  sectionName = arg.substr(0, atPos);

  std::string addressStr = arg.substr(atPos + 1);

  address = std::stoul(addressStr, nullptr, 16);
}

int main(int argc, char* argv[])
{
  int idxOption = findOption(argc, argv);
  bool isHex = false, isRelocatable = false;
  std::string fileName;
  std::string outputName;

  try
  {
    if(idxOption == -1)
    {
      throw std::runtime_error("Option -o of the linker is not set. Linker expects -o flag.");
    
    }
    if(idxOption == argc - 1)
    {
      throw std::runtime_error("Option -o is not used properly. After flag -o name of the output file is expected");
    }

    for(size_t i = 1; i < argc; i++)
    {
      if(i == idxOption || i == idxOption + 1)
      {
        continue;
      }

      switch(inspectCommandArgument(argv[i]))
      {
        case OptionType::Hex:
          isHex = true;
          break;
        case OptionType::Relocatable:
          isRelocatable = true;
          break;
        case OptionType::Place:
        {
          std::string sectionName;
          uint32_t sectionAddress;
          parsePlaceArgument(argv[i], sectionName, sectionAddress);
          Linker::addSectionMapping(sectionName, sectionAddress);
          break;
        }
        
        case OptionType::Other:
          Linker::addFileName(argv[i]);
          break;
      }
    }
    if((isHex && isRelocatable) || (!isHex && !isRelocatable))
    {
      return 0;
    }

    outputName = argv[idxOption + 1];
    Linker::readInputFiles();
    Linker::checkMultipleDefinitions();
    if(isHex)
    {
      Linker::checkUnresolvedSymbols;
    }
    Linker::makeLinkerSections();
    if(isHex)
    {
      Linker::fixAddresses();
      Linker::fixRelocationEntries();
      Linker::execPipeline();
      Linker::makeExecElfFile(outputName);
    }
    else if(isRelocatable)
    {
      Linker::makeLinkerSymbolTable();
      Linker::makeLinkerRelocationEntries();
      Linker::relocPipeline();
      Linker::makeRelocElfFile(outputName);
    }

  }
  catch(LinkerErrors& err)
  {

  }
  catch(const std::runtime_error& err)
  {
    std::cout << err.what() << std::endl;
  }
  return 0;
}