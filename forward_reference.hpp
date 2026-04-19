#ifndef _FORWARD_REFERENCE_HPP_
#define _FORWARD_REFERENCE_HPP_

#include <cstdlib>

class ForwardReference
{
public:
  ForwardReference(size_t section_, size_t offset_) : section(section_), offset(offset_) {}
private:
  size_t section, offset;
};

#endif