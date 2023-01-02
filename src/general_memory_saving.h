
#ifndef header_memory_saving_ALREADY_INCLUDED
#define header_memory_saving_ALREADY_INCLUDED

#include "macros.h"
#include "general_list_references.h"

// This is a wrapper-allocate-on-first-use around a regular object.
template <class Object>
class MemorySaving {
private:
  Object* value;
public:
  void operator=(const MemorySaving<Object>& other) {
    if (!other.isZeroPointer()) {
      this->getElement() = other.getElementConst();
    } else {
      this->freeMemory();
    }
  }
  const Object& getElementConst() const {
    if (this->value == 0) {
      fatalCrash("Attempt to access zero pointer. ");
    }
    return *this->value;
  }
  Object& getElement() {
    if (this->value == 0) {
      this->value = new Object;
#ifdef AllocationLimitsSafeguard
      GlobalStatistics::globalPointerCounter ++;
      GlobalStatistics::checkPointerCounters();
#endif
    }
    return *(this->value);
  }
  bool operator==(const MemorySaving<Object>& other) const {
    if (this->isZeroPointer() != other.isZeroPointer()) {
      return false;
    }
    if (this->isZeroPointer()) {
      return true;
    }
    return this->getElementConst() == other.getElementConst();
  }
  int hashFunction() const {
    if (this->isZeroPointer()) {
      return 0;
    }
    return this->getElementConst().hashFunction();
  }
  static inline int hashFunction(const MemorySaving<Object>& input) {
    return input.hashFunction();
  }
  bool isZeroPointer() const {
    return this->value == 0;
  }
  void freeMemory() {
    delete this->value;
    this->value = 0;
#ifdef AllocationLimitsSafeguard
    GlobalStatistics::globalPointerCounter --;
    GlobalStatistics::checkPointerCounters();
#endif
  }
  MemorySaving() {
    this->value = nullptr;
  }
  MemorySaving(const MemorySaving<Object>& other) {
    if (other == this) {
      // This does happen in practice
      return;
    }
    this->value = nullptr;
    this->getElement() == other.getElementConst();
  }
  ~MemorySaving() {
    this->freeMemory();
  }
};

#endif
