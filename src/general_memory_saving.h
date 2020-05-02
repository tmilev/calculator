// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader9MemorySaving_h_already_included
#define vpfHeader9MemorySaving_h_already_included

#include "macros.h"
#include "general_list_references.h"

// This is a wrapper-allocate-on-first-use around a regular object.
template <class Object>
class MemorySaving {
private:
  Object* theValue;
  MemorySaving(const MemorySaving<Object>& other) {
    (void) other;
    fatalCrash("This constructor should not be used. ");
  }
public:
  void operator=(const MemorySaving<Object>& other) {
    if (!other.isZeroPointer()) {
      this->getElement() = other.getElementConst();
    } else {
      this->freeMemory();
    }
  }
  const Object& getElementConst() const {
    if (this->theValue == 0) {
      fatalCrash("Attempt to access zero pointer. ");
    }
    return *this->theValue;
  }
  Object& getElement()  {
    if (this->theValue == 0) {
      this->theValue = new Object;
      #ifdef AllocationLimitsSafeguard
      GlobalStatistics::globalPointerCounter ++;
      GlobalStatistics::checkPointerCounters();
      #endif
    }
    return *(this->theValue);
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
    return this->theValue == 0;
  }
  void freeMemory() {
    delete this->theValue;
    this->theValue = 0;
    #ifdef AllocationLimitsSafeguard
    GlobalStatistics::globalPointerCounter --;
    GlobalStatistics::checkPointerCounters();
    #endif
  }
  MemorySaving() {
    this->theValue = 0;
  }
  ~MemorySaving() {
    this->freeMemory();
  }
};

#endif
