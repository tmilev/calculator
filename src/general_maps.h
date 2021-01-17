// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader1General6Maps_already_included
#define vpfHeader1General6Maps_already_included

#include "general_lists.h"
#include "general_list_references.h"

template <class listType, class key, class value, unsigned int keyHashFunction(const key&) = key::hashFunction>
class MapTemplate {
public:
  HashedList<key, keyHashFunction> keys;
  listType values;
  unsigned int hashFunction() const {
    int j = - 1;
    unsigned int result = 0;
    for (int i = 0; i < this->keys.size; i ++) {
      j ++;
      if (j >= someRandomPrimesSize) {
        j = 0;
      }
      result += keyHashFunction(this->keys[i]) *
      HashFunctions::hashFunction(this->values[i]) *
      someRandomPrimes[j];
    }
    return result;
  }
  int getIndex(const key& input) const {
    return this->keys.getIndex(input);
  }
  int getIndexNoFail(const key& input) const {
    return this->keys.getIndexNoFail(input);
  }
  bool contains(const key& inputKey) const {
    return this->getIndex(inputKey) != - 1;
  }
  void removeKey(const key& theKey) {
    int theIndex = this->keys.getIndex(theKey);
    if (theIndex == - 1) {
      return;
    }
    this->keys.removeIndexSwapWithLast(theIndex);
    this->values.removeIndexSwapWithLast(theIndex);
  }
  const value& getValueNoFail(const key& input) const {
    int theIndex = this->keys.getIndex(input);
    if (theIndex == - 1) {
      fatalCrash("Map does not contain key at a place where that is not allowed. ");
    }
    return this->values[theIndex];
  }
  value getValue(const key& input, const value& resultIfMissing) const {
    int theIndex = this->keys.getIndex(input);
    if (theIndex == - 1) {
      return resultIfMissing;
    }
    return this->values[theIndex];
  }
  value& getValueCreate(const key& input) {
    int theIndex = this->keys.getIndex(input);
    if (theIndex == - 1) {
      theIndex = this->keys.size;
      this->keys.addOnTop(input);
      value object;
      this->values.addOnTop(object);
    }
    return this->values[theIndex];
  }
  value& getValueCreateNoInit(const key& input) {
    int theIndex = this->keys.getIndex(input);
    if (theIndex == - 1) {
      theIndex = this->keys.size;
      this->keys.addOnTop(input);
      this->values.setSize(this->values.size + 1);
    }
    return this->values[theIndex];
  }
  void setKeyValue(const key& inputKey, const value& inputValue) {
    if (this->contains(inputKey)) {
      this->values[this->keys.getIndex(inputKey)] = inputValue;
      return;
    }
    this->values.addOnTop(inputValue);
    this->keys.addOnTop(inputKey);
  }
  void setExpectedSize(int theSize) {
    this->keys.setExpectedSize(theSize);
    this->values.setExpectedSize(theSize);
  }
  void clear() {
    this->keys.clear();
    this->values.setSize(0);
  }
  int size() const {
    return this->values.size;
  }
  std::string toStringHtml() const {
    std::stringstream out;
    out << "{";
    for (int i = 0; i < this->size(); i ++) {
      out << "\"" << this->keys[i] << "\":\"" << this->values[i] << "\"";
      if (i != this->size() - 1) {
        out << ", ";
      }
    }
    out << "}";
    return out.str();
  }
  inline value& operator[](const key& index) {
    return this->getValueCreate(index);
  }
};

// using C++ 11, not sure if that is a good idea:
// In case this does not compile, please see the commented code below.
template <class key, class value, unsigned int hashFunction(const key&) = key::hashFunction>
using MapReferences = MapTemplate<ListReferences<value>, key, value, hashFunction>;
template <class key, class value, unsigned int hashFunction(const key&) = key::hashFunction>
using MapList = MapTemplate<List<value>, key, value, hashFunction>;
#endif
