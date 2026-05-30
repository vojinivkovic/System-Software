#ifndef _SYMBOL_HPP_
#define _SYMBOL_HPP_

#include <cstdlib>
#include <vector>
#include <cstdint>
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
    Section,
    String,
    WordArray
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

  Symbol(
         const size_t& name_, 
         const size_t& size_, 
         const size_t& value_, 
         const size_t& section_, 
         const Binding& bind_, 
         const Type& type_,
         const Scope& scope_) 
         : name(name_),
           size(size_),
           value(value_),
           section(section_),
           bind(bind_),
           type(type_),
           scope(scope_) 
           {
            if(scope == Symbol::Scope::Local || (scope == Symbol::Scope::Global && bind == Symbol::Binding::Export))
            {
              defined = true;
            }
            else
            {
              defined = false;
            }
           }

  size_t getName() const { return name; }
  size_t getSize() const { return size; }
  void setSize(size_t size_) { size = size_; }
  void setBinding(Binding bind_) { bind = bind_; }
  Binding getBinding() const { return bind; } 

  void setScope(Scope scope_) { scope = scope_; }
  Scope getScope() const { return scope; }

  Type getType() const { return type; }
  void setType(Type type_) { type = type_; } 
  bool getDefined() const { return defined; }
  void setDefined() { defined = true; }

  size_t getValue() const { return value; }
  void setValue(size_t value_) { value = value_; }

  void setSection(size_t idxSection) { section = idxSection; }
  size_t getSection() const { return section; }
  size_t getIdx() const { return idx; }

  std::vector<ForwardReference*> getForwardReference() const { return tableForwardReference; }

  void addForwardReference(ForwardReference* newReference) { tableForwardReference.push_back(newReference); }

  std::vector<uint8_t> getLittleEndianFormatOfSymbol() const;

private: 
  size_t idx, name, size, value, section;
  Binding bind;
  Type type;
  Scope scope;
  bool defined;
  std::vector<ForwardReference*> tableForwardReference;
};

#endif