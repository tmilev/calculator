#ifndef header_size_computer_ALREADY_INCLUDED
#define header_size_computer_ALREADY_INCLUDED

#include <cstdint>
#include <sstream>

namespace RamUsageComputation {
typedef int* IntPointer;
template <typename T>
int64_t byteSize(const T& object) {
  return sizeof(object) + object.byteSizeOwnedThroughPointers();
}
template < >
int64_t byteSize(const int& object);
template < >
int64_t byteSize(const IntPointer& object);
template < >
int64_t byteSize(const unsigned int& object);
}

#endif // header_size_computer_ALREADY_INCLUDED
