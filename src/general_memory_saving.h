//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#ifndef vpfHeader9MemorySaving_h_already_included
#define vpfHeader9MemorySaving_h_already_included

#include "macros.h"
#include "general_list_references.h"

static ProjectInformationInstance vpfHeader9MemorySaving(__FILE__, "Header, memory some basic templates. ");

// This is a wrapper-allocate-on-first-use around a regular object.
template <class Object>
class MemorySaving {
private:
  Object* theValue;
  MemorySaving(const MemorySaving<Object>& other) {
    (void) other;
    crash << "This constructor should not be used. " << crash;
  }
public:
  void operator=(const MemorySaving<Object>& other) {
    if (!other.IsZeroPointer()) {
      this->GetElement() = other.GetElementConst();
    } else {
      this->FreeMemory();
    }
  }
  const Object& GetElementConst() const {
    if (this->theValue == 0) {
      crash << "Programming error: attempting to access zero pointer. " << crash;
    }
    return *this->theValue;
  }
  Object& GetElement()  {
    if (this->theValue == 0) {
      this->theValue = new Object;
      #ifdef AllocationLimitsSafeguard
      ParallelComputing::GlobalPointerCounter ++;
      ParallelComputing::CheckPointerCounters();
      #endif
    }
    return *(this->theValue);
  }
  bool operator==(const MemorySaving<Object>& other) const {
    if (this->IsZeroPointer() != other.IsZeroPointer()) {
      return false;
    }
    if (this->IsZeroPointer()) {
      return true;
    }
    return this->GetElementConst() == other.GetElementConst();
  }
  int HashFunction() const {
    if (this->IsZeroPointer()) {
      return 0;
    }
    return this->GetElementConst().HashFunction();
  }
  static inline int HashFunction(const MemorySaving<Object>& input) {
    return input.HashFunction();
  }
  bool IsZeroPointer() const {
    return this->theValue == 0;
  }
  void FreeMemory();
  MemorySaving() {
    this->theValue = 0;
  }
  ~MemorySaving() {
    this->FreeMemory();
  }
};

template <typename Object>
void MemorySaving<Object>::FreeMemory() {
  delete this->theValue;
  this->theValue = 0;
  #ifdef AllocationLimitsSafeguard
  ParallelComputing::GlobalPointerCounter --;
  ParallelComputing::CheckPointerCounters();
  #endif
}

#endif
