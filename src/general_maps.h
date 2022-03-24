// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_general_maps_ALREADY_INCLUDED
#define header_general_maps_ALREADY_INCLUDED

#include "general_lists.h"
#include "general_list_references.h"

template <
  class ListType,
  class Key,
  class Value,
  unsigned int keyHashFunction(const Key&) = Key::hashFunction
>
class MapTemplate {
public:
  HashedList<Key, keyHashFunction> keys;
  ListType values;
  unsigned int hashFunction() const {
    unsigned int result = 0;
    for (int i = 0; i < this->keys.size; i ++) {
      result +=
      keyHashFunction(this->keys[i]) *
      HashFunctions::hashFunction(this->values[i]);
    }
    return result;
  }
  int getIndex(const Key& input) const {
    return this->keys.getIndex(input);
  }
  int getIndexNoFail(const Key& input) const {
    return this->keys.getIndexNoFail(input);
  }
  bool contains(const Key& inputKey) const {
    return this->getIndex(inputKey) != - 1;
  }
  void removeKey(const Key& inputKey) {
    int index = this->keys.getIndex(inputKey);
    if (index == - 1) {
      return;
    }
    this->keys.removeIndexSwapWithLast(index);
    this->values.removeIndexSwapWithLast(index);
  }
  const Value& getValueNoFail(const Key& input) const {
    int index = this->keys.getIndex(input);
    if (index == - 1) {
      fatalCrash(
        "Map does not contain key at a place where that is not allowed. "
      );
    }
    return this->values[index];
  }
  Value getValue(const Key& input, const Value& resultIfMissing) const {
    int index = this->keys.getIndex(input);
    if (index == - 1) {
      return resultIfMissing;
    }
    return this->values[index];
  }
  Value& getValueCreateEmpty(const Key& input) {
    int index = this->keys.getIndex(input);
    if (index == - 1) {
      Value empty;
      index = this->keys.size;
      this->keys.addOnTop(input);
      this->values.addOnTop(empty);
    }
    return this->values[index];
  }
  Value& getValueCreate(const Key& input, const Value& initialValue) {
    int index = this->keys.getIndex(input);
    if (index == - 1) {
      index = this->keys.size;
      this->keys.addOnTop(input);
      this->values.addOnTop(initialValue);
    }
    return this->values[index];
  }
  Value& getValueCreateNoInitialization(const Key& input) {
    int index = this->keys.getIndex(input);
    if (index == - 1) {
      index = this->keys.size;
      this->keys.addOnTop(input);
      this->values.setSize(this->values.size + 1);
    }
    return this->values[index];
  }
  void setKeyValue(const Key& inputKey, const Value& inputValue) {
    if (this->contains(inputKey)) {
      this->values[this->keys.getIndex(inputKey)] = inputValue;
      return;
    }
    this->values.addOnTop(inputValue);
    this->keys.addOnTop(inputKey);
  }
  void setExpectedSize(int expectedSize) {
    this->keys.setExpectedSize(expectedSize);
    this->values.setExpectedSize(expectedSize);
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
  // Fetches a value with a given key.
  // If not present, creates an empty object with the given key.
  // If you need to use object lookup with a const function, use
  // function:
  //
  // getValueNoFail() const
  //
  // instead (this function will crash if the object's not there).
  inline Value& operator[](const Key& key) {
    return this->getValueCreateNoInitialization(key);
  }
  bool operator==(
    const MapTemplate<ListType, Key, Value, keyHashFunction>& other
  ) const {
    if (this->size() != other.size()) {
      return false;
    }
    for (int i = 0; i < this->size(); i ++) {
      const Key& key = this->keys[i];
      int otherIndex = other.getIndex(key);
      if (otherIndex == - 1) {
        return false;
      }
      if (!(this->values[i] == other.values[otherIndex])) {
        return false;
      }
    }
    return true;
  }
};

// using C++ 11, not sure if that is a good idea:
// In case this does not compile, please see the commented code below.
template <
  class key,
  class value,
  unsigned int hashFunction(const key&) = key::hashFunction
>
using MapReferences =
MapTemplate<ListReferences<value>, key, value, hashFunction>;
template <
  class key,
  class value,
  unsigned int hashFunction(const key&) = key::hashFunction
>
using MapList =
MapTemplate<List<value>, key, value, hashFunction>;
#endif // header_general_maps_ALREADY_INCLUDED
