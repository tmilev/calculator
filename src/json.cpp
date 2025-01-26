#include "general_strings.h"
#include "json.h"
#include "math_large_integers.h"
#include <cassert>
#include <sys/stat.h>

void JSData::operator=(const LargeInteger& other) {
  this->reset();
  this->elementType = JSData::Type::tokenLargeInteger;
  this->integerValue.getElement() = other;
}

void JSData::operator=(int other) {
  this->reset();
  this->elementType = JSData::Type::tokenLargeInteger;
  this->integerValue.getElement() = other;
}

void JSData::operator=(const bool other) {
  this->reset();
  this->elementType = JSData::Type::tokenBool;
  this->booleanValue = other;
}

void JSData::operator=(const double other) {
  this->reset();
  this->elementType = JSData::Type::tokenFloat;
  this->floatValue = other;
}

void JSData::operator=(const char* other) {
  std::string otherString(other);
  this->operator=(otherString);
}

void JSData::operator=(const std::string& other) {
  this->reset();
  this->elementType = JSData::Type::tokenString;
  this->stringValue = other;
}

void JSData::operator=(const JSData& other) {
  this->elementType = other.elementType;
  this->booleanValue = other.booleanValue;
  this->floatValue = other.floatValue;
  this->integerValue = other.integerValue;
  this->stringValue = other.stringValue;
  this->listObjects = other.listObjects;
  this->objects = other.objects;
}

void JSData::operator=(const List<unsigned char>& other) {
  this->elementType = JSData::Type::tokenString;
  this->stringValue = other.toStringConcatenate();
  this->listObjects.setSize(0);
  this->objects.clear();
}

void JSData::operator=(const List<JSData>& other) {
  this->elementType = JSData::Type::tokenArray;
  this->listObjects.setSize(other.size);
  for (int i = 0; i < other.size; i ++) {
    this->listObjects[i] = other[i];
  }
  this->objects.clear();
}

void JSData::operator=(int64_t input) {
  this->elementType = JSData::Type::tokenLargeInteger;
  this->integerValue.getElement().assignInt64(input);
}

JSData& JSData::operator[](int i) {
  this->elementType = JSData::Type::tokenArray;
  if (this->listObjects.size < i + 1) {
    this->listObjects.setSizeAndExpectedSize(i + 1);
  }
  return this->listObjects[i];
}

JSData JSData::getValue(const std::string& key) const {
  int index = this->getKeyIndex(key);
  if (index != - 1) {
    return this->objects.values[index];
  }
  JSData result;
  result.elementType = JSData::Type::tokenUndefined;
  return result;
}

bool JSData::hasKey(const std::string& key) const {
  return this->getKeyIndex(key) != - 1;
}

bool JSData::hasCompositeKeyOfType(
  const std::string& key,
  LargeIntegerUnsigned& output,
  std::stringstream* commentsOnFailure
) const {
  JSData container;
  if (
    !this->hasCompositeKeyOfToken(
      key, &container, JSData::Type::tokenLargeInteger, commentsOnFailure
    )
  ) {
    return false;
  }
  if (container.integerValue.getElement() < 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Error: integer is negative. ";
    }
    return false;
  }
  output = container.integerValue.getElement().value;
  return true;
}

bool JSData::hasCompositeKeyOfType(
  const std::string& key,
  std::string& output,
  std::stringstream*
  commentsOnFailure
) const {
  JSData container;
  if (
    !this->hasCompositeKeyOfToken(
      key, &container, JSData::Type::tokenString, commentsOnFailure
    )
  ) {
    return false;
  }
  output = container.stringValue;
  return true;
}

bool JSData::hasCompositeKeyOfType(
  const std::string& key,
  List<unsigned char>& output,
  std::stringstream* commentsOnFailure
) const {
  JSData container;
  if (
    !this->hasCompositeKeyOfToken(
      key, &container, JSData::Type::tokenString, commentsOnFailure
    )
  ) {
    return false;
  }
  output = container.stringValue;
  return true;
}

bool JSData::hasCompositeKeyValueNull(
  const std::string& key, std::stringstream* commentsOnFailure
) const {
  return
  this->hasCompositeKeyOfToken(
    key, nullptr, JSData::Type::tokenNull, commentsOnFailure
  );
}

bool JSData::hasCompositeKeyOfToken(
  const std::string& key,
  JSData* whichValue,
  char targetType,
  std::stringstream* commentsOnFailure
) const {
  JSData container;
  if (whichValue == nullptr) {
    whichValue = &container;
  }
  if (!JSData::hasCompositeKey(key, whichValue, commentsOnFailure)) {
    return false;
  }
  if (whichValue->elementType != targetType) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Key: "
      << key
      << ", value: "
      << whichValue->toString(nullptr)
      << " is of type "
      << static_cast<int>(whichValue->elementType)
      << " instead of the target one: "
      << static_cast<int>(targetType)
      << ". ";
    }
    return false;
  }
  return true;
}

bool JSData::hasCompositeKey(
  const std::string& inputKeys,
  JSData* whichValue,
  std::stringstream* commentsOnFailure
) const {
  List<char> delimiters;
  delimiters.addOnTop('.');
  delimiters.addOnTop('[');
  delimiters.addOnTop(']');
  List<std::string> keys;
  StringRoutines::splitExcludeDelimiters(inputKeys, delimiters, keys);
  const JSData* currentData = this;
  if (keys.size == 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Could not extract any keys from your input: "
      << inputKeys;
    }
    return false;
  }
  for (int i = 0; i < keys.size; i ++) {
    // Will not work for integer indices larger than 9.
    int digit = - 1;
    if (MathRoutines::isDigit(keys[i], &digit)) {
      if (currentData->elementType != JSData::Type::tokenArray) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << "The sub-object located before the key: "
          << keys[i]
          << " [all keys: "
          << keys.toStringCommaDelimited()
          << "] "
          << "is not an array, but is instead: "
          << currentData->toString(&JSData::PrintOptions::HTML())
          << ". ";
        }
        return false;
      }
      if (currentData->listObjects.size <= digit) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << "Key index: "
          << digit
          << " is too large for current value: "
          << currentData->toString(&JSData::PrintOptions::HTML());
        }
        return false;
      }
      currentData = &currentData->listObjects[digit];
      continue;
    }
    if (currentData->elementType != JSData::Type::tokenObject) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Value preceding key: "
        << keys[i]
        << " is not of type object: "
        << currentData->toString(&JSData::PrintOptions::HTML());
      }
      return false;
    }
    if (!currentData->hasKey(keys[i])) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Key: "
        << keys[i]
        << " not present in: "
        << currentData->toString(&JSData::PrintOptions::HTML());
      }
      return false;
    }
    currentData = &currentData->objects.getValueNoFail(keys[i]);
  }
  if (whichValue != nullptr) {
    *whichValue = *currentData;
  }
  return true;
}

int JSData::getKeyIndex(const std::string& key) const {
  return this->objects.getIndex(key);
}

bool JSData::hasNestedKey(
  const List<std::string>& nestedKeys, JSData* whichValue
) const {
  const JSData* current = this;
  for (const std::string& label : nestedKeys) {
    if (!current->hasKey(label)) {
      return false;
    }
    current = &(*current).objects.getValueNoFail(label);
  }
  if (whichValue != nullptr) {
    *whichValue = *current;
  }
  return true;
}

void JSData::setKeyValue(const std::string& key, const JSData& value) {
  (*this)[key] = value;
}

JSData& JSData::operator[](const std::string& key) {
  this->elementType = JSData::Type::tokenObject;
  return this->objects.getValueCreateEmpty(key);
}

bool JSData::isLargeInteger(LargeInteger* whichInteger) const {
  if (this->elementType != JSData::Type::tokenLargeInteger) {
    return false;
  }
  if (whichInteger != nullptr) {
    *whichInteger = this->integerValue.getElementConst();
  }
  return true;
}

bool JSData::isIntegerFittingInInt(int* whichInteger) {
  if (this->elementType == JSData::Type::tokenLargeInteger) {
    return
    this->integerValue.getElement().isIntegerFittingInInt(whichInteger);
  }
  if (this->elementType == JSData::Type::tokenFloat) {
    double floatRounded = static_cast<double>(
      static_cast<int>(this->floatValue)
    );
    if (floatRounded - this->floatValue != 0.0) {
      return false;
    }
    if (whichInteger != nullptr) {
      *whichInteger = static_cast<int>(this->floatValue);
    }
    return true;
  }
  return false;
}

bool JSData::isEqualTo(const std::string& other) {
  if (this->elementType != JSData::Type::tokenString) {
    return false;
  }
  return this->stringValue == other;
}

bool JSData::isNullOrUndefined() {
  return
  this->elementType == JSData::Type::tokenNull ||
  this->elementType == JSData::Type::tokenUndefined;
}

bool JSData::isTrueRepresentationInJSON() {
  if (this->elementType == JSData::Type::tokenBool) {
    return this->booleanValue;
  }
  if (this->elementType != JSData::Type::tokenString) {
    return false;
  }
  return this->stringValue == "true";
}

JSData JSData::emptyArray() {
  JSData result;
  result.makeEmptyArray();
  return result;
}

JSData JSData::emptyObject() {
  JSData result;
  result.makeEmptyObject();
  return result;
}

void JSData::makeEmptyArray() {
  this->reset(JSData::Type::tokenArray);
}

void JSData::makeEmptyObject() {
  this->reset(JSData::Type::tokenObject);
}

bool JSData::isString(std::string* whichString) const {
  if (this->elementType != JSData::Type::tokenString) {
    return false;
  }
  if (whichString != nullptr) {
    *whichString = this->stringValue;
  }
  return true;
}

bool JSData::isListOfStrings(List<std::string>* whichStrings) {
  if (this->elementType != JSData::Type::tokenArray) {
    return false;
  }
  for (int i = 0; i < this->listObjects.size; i ++) {
    if (this->listObjects[i].elementType != JSData::Type::tokenString) {
      return false;
    }
  }
  if (whichStrings != nullptr) {
    whichStrings->setSize(this->listObjects.size);
    for (int i = 0; i < this->listObjects.size; i ++) {
      (*whichStrings)[i] = this->listObjects[i].stringValue;
    }
  }
  return true;
}

bool JSData::isValidElement() {
  return
  this->elementType == JSData::Type::tokenNull ||
  this->elementType == JSData::Type::tokenBool ||
  this->elementType == JSData::Type::tokenLargeInteger ||
  this->elementType == JSData::Type::tokenFloat ||
  this->elementType == JSData::Type::tokenString ||
  this->elementType == JSData::Type::tokenArray ||
  this->elementType == JSData::Type::tokenObject;
}
