#include "assembler.hpp"
#include "aux/exceptions.hpp"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <string>

int yyparse();
extern FILE* yyin;

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

int main(int argc, char* argv[])
{ 
  int idxOfOption = findOption(argc, argv);
  const char* inputName;
  std::string outputName;
  try 
  {
    if(argc < 4)
    {
      throw std::runtime_error("Too few arguments for assembler. Assembler expects ./assembler -o out.o in.s");
    }
    if(argc > 4)
    {
      throw std::runtime_error("Too many arguments for assembler. Assembler expects ./assembler -o out.o in.s");
    }
    if(idxOfOption == -1)
    {
      throw std::runtime_error("Option -o of the assembler is not set. Assembler expects ./assembler -o out.o in.s");
    
    }
    if(idxOfOption!= 1 && idxOfOption != 2)
    {
      throw std::runtime_error("Option -o is not used properly. Assembler expects ./assembler -o out.o in.s");
    
    }

    outputName = argv[idxOfOption + 1];
    if(idxOfOption == 1)
    {
      inputName = argv[argc - 1];
    }
    else
    {
      inputName = argv[1];
    }
    yyin = fopen(inputName, "r");
    if(!yyin)
    {
      throw std::runtime_error("Input file can't be open");
    }

    Assembler::initializeAssembler();
    //extern int yydebug;
    //yydebug = 1;
    yyparse();
    fclose(yyin);
    Assembler::afterFirstPass();
    Assembler::makeELFFiles(outputName);
    Assembler::cleanup();
  }
  catch(AssemblerErrors& err)
  {
    if(err.getErrorType() == ErrorType::ErrorUndefinedSymbol || err.getErrorType() == ErrorType::ErrorUndefinedMacro)
    {
      std::cout << "Error[" << err.toString(err.getErrorType()) << "]" << std::endl
      << err.what() << std::endl;
    }
    else 
    {
      std::cout << "Error[" << err.toString(err.getErrorType()) << "]" << std::endl
                << "In section name[offset]: " << err.getSectionName() << "; Byte Number: " << err.getLineInSection() << std::endl
                << err.what() << std::endl;
    }
    
    if(!err.getDetailMessage().empty())
    {
      std::cout << "(" << err.getDetailMessage() << ")" << std::endl; 

    }
    if(yyin)
    {
      fclose(yyin);
    }
  }
  catch(const std::runtime_error& err) 
  {
    std::cout << err.what() << std::endl;
  }
  return 0;
}