#include "ram_usage_computation.h"

template < >
int64_t RamUsageComputation::byteSize(const int& object) {
  return sizeof(object);
}

template < >
int64_t RamUsageComputation::byteSize(const unsigned int& object) {
  return sizeof(object);
}
