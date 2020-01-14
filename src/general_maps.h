// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader1General6Maps_already_included
#define vpfHeader1General6Maps_already_included

#include "general_lists.h"
#include "general_list_references.h"

template <class listType, class key, class value, unsigned int hashFunction(const key&) = key::HashFunction>
class MapTemplate {
public:
  HashedList<key, hashFunction> theKeys;
  listType theValues;
  int GetIndex(const key& input) const {
    return this->theKeys.GetIndex(input);
  }
  int GetIndexIMustContainTheObject(const key& input) const {
    return this->theKeys.GetIndexIMustContainTheObject(input);
  }
  bool Contains(const key& inputKey) const {
    return this->GetIndex(inputKey) != - 1;
  }
  void RemoveKey(const key& theKey) {
    int theIndex = this->theKeys.GetIndex(theKey);
    if (theIndex == - 1) {
      return;
    }
    this->theKeys.RemoveIndexSwapWithLast(theIndex);
    this->theValues.RemoveIndexSwapWithLast(theIndex);
  }
  const value& GetValueConstCrashIfNotPresent(const key& input) const {
    int theIndex = this->theKeys.GetIndex(input);
    if (theIndex == - 1) {
      fatalCrash("Map does not contain key at a place where that is not allowed. ");
    }
    return this->theValues[theIndex];
  }
  value GetValue(const key& input, const value& resultIfMissing) {
    int theIndex = this->theKeys.GetIndex(input);
    if (theIndex == - 1) {
      return resultIfMissing;
    }
    return this->theValues[theIndex];
  }
  value& GetValueCreate(const key& input) {
    int theIndex = this->theKeys.GetIndex(input);
    if (theIndex == - 1) {
      theIndex = this->theKeys.size;
      this->theKeys.AddOnTop(input);
      value object;
      this->theValues.AddOnTop(object);
    }
    return this->theValues[theIndex];
  }
  value& GetValueCreateNoInit(const key& input) {
    int theIndex = this->theKeys.GetIndex(input);
    if (theIndex == - 1) {
      theIndex = this->theKeys.size;
      this->theKeys.AddOnTop(input);
      this->theValues.SetSize(this->theValues.size + 1);
    }
    return this->theValues[theIndex];
  }
  void SetKeyValue(const key& inputKey, const value& inputValue) {
    if (this->Contains(inputKey)) {
      this->theValues[this->theKeys.GetIndex(inputKey)] = inputValue;
      return;
    }
    this->theValues.AddOnTop(inputValue);
    this->theKeys.AddOnTop(inputKey);
  }
  void SetExpectedSize(int theSize) {
    this->theKeys.SetExpectedSize(theSize);
    this->theValues.SetExpectedSize(theSize);
  }
  void Clear() {
    this->theKeys.Clear();
    this->theValues.SetSize(0);
  }
  int size() const {
    return this->theValues.size;
  }
  std::string ToStringHtml() const {
    std::stringstream out;
    out << "{";
    for (int i = 0; i < this->size(); i ++) {
      out << "\"" << this->theKeys[i] << "\":\"" << this->theValues[i] << "\"";
      if (i != this->size() - 1) {
        out << ", ";
      }
    }
    out << "}";
    return out.str();
  }
  inline value& operator[](const key& index) {
    return this->GetValueCreate(index);
  }
};

// using C++ 11, not sure if that is a good idea:
// In case this does not compile, please see the commented code below.
template <class key, class value, unsigned int hashFunction(const key&) = key::HashFunction>
using MapReferences = MapTemplate<ListReferences<value>, key, value, hashFunction>;
template <class key, class value, unsigned int hashFunction(const key&) = key::HashFunction>
using MapList = MapTemplate<List<value>, key, value, hashFunction>;

/*
template <class value, class key, unsigned int hashFunction(const key&)=key::HashFunction>
class MapReferences
{
public:
  HashedList<key, hashFunction> theKeys;
  ListReferences<value> theValues;
  int GetIndex(const key& input) const {
    return this->theKeys.GetIndex(input);
  }
  bool Contains(const key& input) const {
    return this->GetIndex(input) != - 1;
  }
  value& GetValueCreateIfNotPresent(const key& input) {
    int theIndex = this->theKeys.GetIndex(input);
    if (theIndex == - 1) {
      theIndex = this->theKeys.size;
      this->theKeys.AddOnTop(input);
      this->theValues.SetSize(this->theValues.size + 1);
    }
    return this->theValues[theIndex];
  }
  void SetValue(const value& inputValue, const key& inputKey) {
    if (this->Contains(inputKey)) {
      this->theValues[this->theKeys.GetIndex(inputKey)] = inputValue;
      return;
    }
    this->theValues.AddOnTop(inputValue);
    this->theKeys.AddOnTop(inputKey);
  }
  void SetExpectedSize(int theSize) {
    this->theKeys.SetExpectedSize(theSize);
    this->theValues.SetExpectedSize(theSize);
  }
  void Clear() {
    this->theKeys.Clear();
    this->theValues.SetSize(0);
  }
  value& operator[](int i) const {
    return this->theValues[i];
  }
};
*/
#endif
