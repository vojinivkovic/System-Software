#ifndef _LINKER_HPP_
#define _LINKER_HPP_
#include "symbol/symbol_table.hpp"
#include <vector>

class Linker
{
public:
  Linker() = delete;
  Linker(const Linker& link) = delete;
  Linker& operator=(const Linker& link) = delete;

private:

};

#endif