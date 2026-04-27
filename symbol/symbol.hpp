#ifndef _SYMBOL_HPP_
#define _SYMBOL_HPP_

#include <cstdlib>
#include <vector>
#include "../aux/forward_reference.hpp"


class Symbol
{  
public:
  enum class Binding
  {
    NoBinding,
    Import,
    Export
  };
  enum class Scope 
  {
    NoScope,
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

  Symbol(const size_t& idx_, 
         const size_t& name_, 
         const size_t& size_, 
         const size_t& value_, 
         const size_t& section_, 
         const Binding& bind_, 
         const Type& type_,
         const Scope& scope_, 
         const bool& defined_,
         ForwardReference* fReference_ = nullptr) 
         : idx(idx_),
           name(name_),
           size(size_),
           value(value_),
           section(section_),
           bind(bind_),
           type(type_),
           scope(scope_),
           defined(defined_) 
{
  if(fReference_)
  {
    tableForwardReference.push_back(fReference_);
  }


}
  size_t getName() const { return name; }
  
  void setBinding(Binding bind_) { bind = bind_; }
  Binding getBinding() const { return bind; } 

  void setScope(Scope scope_) { scope = scope_; }
  Scope getScope() const { return scope; }

  bool getDefined() const { return defined; }
  size_t getValue() const { return value; }
  size_t getIdx() const { return idx; }

  void addForwardReference(ForwardReference* newReference) { tableForwardReference.push_back(newReference); }
private: 
  size_t idx, name, size, value, section;
  Binding bind;
  Type type;
  Scope scope;
  bool defined;
  std::vector<ForwardReference*> tableForwardReference;
};

#endif