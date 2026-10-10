#include "ram_usage_computation.h"

namespace RamUsageComputation {

template < >
int64_t byteSize(const int& object) {
  return sizeof(object);
}

template < >
int64_t byteSize(const unsigned int& object) {
  return sizeof(object);
}

template < >
int64_t byteSize(const RamUsageComputation::IntPointer& object) {
  return sizeof(object);
}
}
