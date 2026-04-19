#ifndef _SYMBOL_HPP_
#define _SYMBOL_HPP_

#include <cstdlib>
#include <vector>
#include "forward_reference.hpp"


class Symbol
{  
public:
  enum class Binding
  {
    NoBinding,
    Local,
    Global
  };
  enum class Type
  {
    NoType, 
    Object,
    Function, 
    Section
  };

  Symbol(size_t idx_, 
         size_t name_, 
         size_t size_, 
         size_t value_, 
         size_t section_, 
         Binding bind_, 
         Type type_,
         bool defined_,
         ForwardReference* fReference_ = nullptr) 
         : idx(idx_),
           name(name_),
           size(size_),
           value(value_),
           section(section_),
           bind(bind_),
           type(type_),
           defined(defined_) 
{
  if(fReference_)
  {
    tableForwardReference.push_back(fReference_);
  }
}

private:
  size_t idx, name, size, value, section;
  Binding bind;
  Type type;
  bool defined;
  std::vector<ForwardReference*> tableForwardReference;
};

#endif