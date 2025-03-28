#ifndef header_general_list_references_ALREADY_INCLUDED
#define header_general_list_references_ALREADY_INCLUDED

#include "general_lists.h"

template <class Object>
class ListReferences;

template <typename Object>
class ListReferencesIterator {
public:
  const ListReferences<Object>* iterated;
  int index;
  ListReferencesIterator(const ListReferences<Object>* input) {
    this->iterated = input;
    this->index = 0;
  }
  bool operator!=(const ListReferencesIterator<Object>& other) {
    return this->index != other.index;
  }
  Object& operator*() const;
  const ListReferencesIterator<Object>& operator++() {
    this->index ++;
    return *this;
  }
};

// class ListReferences is a list of objects used similarly to class List.
// The essential differences between ListReferences and List are:
// 1) ListReferences cannot be copied themselves.
// 2) the way the objects are
// stored in memory. A copy of each object of ListReferences
// is allocated with an individual copy constructor (call of new Object; rather
// than new Object[size];),
// and a pointer to the so allocated memory is stored.
// Motivation for this class: when a pointer/reference to an object is
// requested,
// the class returns a pointer to the individually allocated object.
// This piece of memory is allocated exactly once,
// i.e. the individual piece of memory never moves, and is thus completely safe
// to pass a reference of the object around. This reference will not expire,
// should the list be resized.
//
// Use guideline. ListReferences is slower, but pointers
// to data stored in ListReferences are safe to share.
// You should use ListReferences/HashedListReferences
// for objects that will be shared and passed accross functions.
// Those include mostly large
// objects and objects contained in the Object container of the calulator.
// Data that is not meant to be shared
// or that is otherwise small (lists of ints, monomials, Rationals, etc.)
// should be kept in simple Lists and copied around when needed.
// So long as a List is const, it is safe to pass references to data from the
// List. Whenever in doubt, use ListReferences.
template <class Object>
class ListReferences {
  // Copying a list of references with copy constructor is forbidden.
  // This would violate the basic code contract of the class: pointers
  // to objects allocated in this class can be safely passed around.
  // If you copied this class and removed the old copy,
  // and someone already has a pointer coming from the old copy,
  // that pointer will be used after free.
  ListReferences(const ListReferences<Object>&);
  friend std::ostream& operator<<(
    std::ostream& output, const ListReferences<Object>& input
  ) {
    output << "[";
    for (int i = 0; i < input.size; i ++) {
      output << input[i];
      if (i != input.size - 1) {
        output << ", ";
      }
    }
    output << "]";
    return output;
  }
public:
  bool flagDeallocated;
  List<Object*> references;
  int size;
  Object& operator[](int i) const {
    if (i < 0 || i >= this->size) {
      std::stringstream commentsOnCrash;
      commentsOnCrash
      << "Attempt to access element of index "
      << i
      << " in ListReferences that has only "
      << this->size
      << " elements. ";
      fatalCrash(commentsOnCrash.str());
    }
    if (this->references[i] == 0) {
      std::stringstream commentsOnCrash;
      commentsOnCrash
      << "Element of index "
      << i
      << " in ListReferences has zero pointer. ";
      fatalCrash(commentsOnCrash.str());
    }
    return *this->references[i];
  }
  unsigned int hashFunction() const {
    unsigned int result = 0;
    int j = 0;
    for (int i = 0; i < this->size; i ++) {
      result +=
      HashConstants::getConstantIncrementCounter(j) *
      HashFunctions::hashFunction((*this)[i]);
    }
    return result;
  }
  bool contains(const Object& inputObject) const {
    for (int i = 0; i < this->size; i ++) {
      if ((*this)[i] == inputObject) {
        return true;
      }
    }
    return false;
  }
  void removeIndexSwapWithLast(int index) {
    // This is not thread-safe
    if (index >= this->size) {
      std::stringstream errorStream;
      errorStream
      << "Attempt to remove index "
      << index
      << " in reference list with "
      << this->size
      << " elements.";
      fatalCrash(errorStream.str());
    }
    this->killElementIndex(index);
    this->references[index] = this->references[this->size - 1];
    this->references[this->size - 1] = 0;
    this->size --;
  }
  void addListOnTop(const List<Object>& input) {
    for (const Object& o : input) {
      this->addOnTop(o);
    }
  }
  void swapTwoIndices(int index1, int index2) {
    this->references.swapTwoIndices(index1, index2);
  }
  void allocateElements(int newSize);
  void reserve(int desiredSize) {
    this->allocateElements(desiredSize);
  }
  void clear() {
    this->setSize(0);
  }
  // Resizes the array.
  // Fills newly created objects with objects allocated with default
  // constructors.
  void setSize(int newSize) {
    this->allocateElements(newSize);
    this->size = newSize;
  }
  // Resizes the object and sets teh expected size.
  void setSizeAndExpectedSize(int newSize) {
    this->setExpectedSize(newSize);
    this->allocateElements(newSize);
    this->size = newSize;
  }
  // Resizes the array. When enlarged, the array will increase by a
  // fixed percent, which gurantees that N non-trivial resizes
  // will result in exponential in N growth.
  void setExpectedSize(int desiredSize) {
    int newSize = (desiredSize* 6) / 5;
    if (newSize > 0) {
      this->allocateElements(newSize);
    }
  }
  void killAllElements();
  void killElementIndex(int i) {
    delete this->references[i];
    // <- NOT thread safe!
    this->references[i] = 0;
  }
  void addOnTop(const Object& o);
  int getIndex(const Object& o) const;
  bool containsExactlyOnce(const Object& o) const {
    bool result = false;
    for (int i = 0; i < this->size; i ++) {
      if ((*this)[i] == o) {
        if (result) {
          return false;
        }
        result = true;
      }
    }
    return result;
  }
  int addNoRepetitionOrReturnIndexFirst(const Object& o) {
    int indexOfObject = this->getIndex(o);
    if (indexOfObject == - 1) {
      this->addOnTop(o);
      return this->size - 1;
    }
    return indexOfObject;
  }
  void operator=(const ListReferences<Object>& other) {
    if (this == &other) {
      return;
    }
    this->killAllElements();
    this->reserve(other.size);
    for (int i = 0; i < other.size; i ++) {
      this->addOnTop(other[i]);
    }
  }
  void operator=(const List<Object>& other) {
    this->setSize(other.size);
    for (int i = 0; i < other.size; i ++) {
      (*this)[i] = other[i];
    }
  }
  Object& lastObject() const {
    return (*this)[this->size - 1];
  }
  template <class otherList = List<Object> >
  void quickSortAscending(
    const typename List<Object>::Comparator* order = nullptr,
    otherList* carbonCopy = nullptr
  ) {
    List<Object>::quickSortAscending(*this, order, carbonCopy);
  }
  ListReferences(): flagDeallocated(false), size(0) {}
  ~ListReferences() {
    this->flagDeallocated = true;
    this->killAllElements();
  }
  std::string toString() const {
    std::stringstream out;
    out << *this;
    return out.str();
  }
  ListReferencesIterator<Object> begin() const {
    ListReferencesIterator<Object> result(this);
    return result;
  }
  ListReferencesIterator<Object> end() const {
    ListReferencesIterator<Object> result(this);
    result.index = this->size;
    return result;
  }
};

template <class Object>
void ListReferences<Object>::allocateElements(int newSize) {
  if (newSize < 0) {
    std::stringstream commentsOnCrash;
    commentsOnCrash
    << "Requested to set negative size "
    << newSize
    << " of List of References. If a "
    << "List is to be set empty, then one should call setSize(0), "
    << "rather than provide a negative argument to setSize.";
    fatalCrash(commentsOnCrash.str());
  }
  if (newSize <= this->references.size) {
    return;
  }
  int oldReferencesSize = this->references.size;
  this->references.setSize(newSize);
  for (int i = oldReferencesSize; i < newSize; i ++) {
    this->references[i] = (new Object);
  }
#ifdef AllocationLimitsSafeguard
  GlobalStatistics::globalPointerCounter += newSize - oldReferencesSize;
  GlobalStatistics::checkPointerCounters();
#endif
}

template <class Object>
void ListReferences<Object>::killAllElements() {
  for (int i = 0; i < this->references.size; i ++) {
    delete this->references[i];
#ifdef AllocationLimitsSafeguard
    GlobalStatistics::globalPointerCounter --;
    GlobalStatistics::checkPointerCounters();
#endif
    this->references[i] = 0;
  }
  this->references.size = 0;
  this->size = 0;
}

template <class Object>
void ListReferences<Object>::addOnTop(const Object& o) {
  if (this->size < 0) {
    std::stringstream out;
    out << "Negative list size not allowed: " << this->size;
    fatalCrash(out.str());
  }
  this->setSize(this->size + 1);
  (*this)[this->size - 1] = o;
}

template <class Object>
int ListReferences<Object>::getIndex(const Object& o) const {
  for (int i = 0; i < this->size; i ++) {
    if ((*this)[i] == o) {
      return i;
    }
  }
  return - 1;
}

template <typename Object>
Object&ListReferencesIterator<Object>::operator*() const {
  return (*this->iterated)[this->index];
}

template <
  class Object, unsigned int hashFunction(const Object&) = Object::hashFunction
>
class HashedListReferences: public HashTemplate<
  Object, ListReferences<Object>, hashFunction
> {
public:
  // Note The following function specializations are declared entirely in order
  // to facilitate autocomplete in my current IDE. If I find a better
  // autocompletion IDE the following should be removed.
  inline void addOnTopNoRepetition(const List<Object>& inputList) {
    this->::HashTemplate<Object, ListReferences<Object>, hashFunction>::
    addOnTopNoRepetition(inputList);
  }
  inline bool addOnTopNoRepetition(const Object& o) {
    return
    this->::HashTemplate<Object, ListReferences<Object>, hashFunction>::
    addOnTopNoRepetition(o);
  }
  inline void addOnTop(const Object& o) {
    this->::HashTemplate<Object, ListReferences<Object>, hashFunction>::
    addOnTop(o);
  }
  inline void addListOnTop(const List<Object>& inputList) {
    this->::HashTemplate<Object, ListReferences<Object>, hashFunction>::
    addListOnTop(inputList);
  }
  inline bool contains(const Object& o) const {
    return
    this->::HashTemplate<Object, ListReferences<Object>, hashFunction>::
    contains(o);
  }
  inline bool contains(const List<Object>& inputList) const {
    return
    this->::HashTemplate<Object, ListReferences<Object>, hashFunction>::
    contains(inputList);
  }
  Object& getElement(int objectIndex) const {
    return
    this->::HashTemplate<Object, ListReferences<Object>, hashFunction>::
    getElement(objectIndex);
  }
  int getIndex(const Object& o) const {
    return
    this->::HashTemplate<Object, ListReferences<Object>, hashFunction>::
    getIndex(o);
  }
  inline int getIndexNoFail(const Object& o) const {
    return
    this->::HashTemplate<Object, ListReferences<Object>, hashFunction>::
    getIndexNoFail(o);
  }
  inline int addNoRepetitionOrReturnIndexFirst(const Object& o) {
    return
    this->::HashTemplate<Object, ListReferences<Object>, hashFunction>::
    addNoRepetitionOrReturnIndexFirst(o);
  }
};

template <class Object>
void List<Object>::operator=(const ListReferences<Object>& other) {
  this->clear();
  for (const Object& object : other) {
    this->addOnTop(object);
  }
}

#endif // header_general_list_references_ALREADY_INCLUDED
