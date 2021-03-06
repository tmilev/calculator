// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfJson_h_already_included
#define vpfJson_h_already_included

#include "general_memory_saving.h"
#include "general_list_references.h"
#include "general_maps.h"


//struct JSHashData;
class Rational;
template <typename Coefficient>
class Matrix;
template <typename Coefficient>
class Vector;
class LargeInteger;

class JSData {
public:
  static const int numEmptyTokensAtStart = 6;
  struct token {
    static const char tokenUndefined                = 0;
    static const char tokenNull                     = 1;
    static const char tokenBool                     = 2;
    static const char tokenFloat                    = 3;
    static const char tokenString                   = 4;
    static const char tokenArray                    = 5;
    static const char tokenObject                   = 6;
    static const char tokenLargeInteger             = 7;
    static const char tokenOpenBrace                = 8;
    static const char tokenCloseBrace               = 9;
    static const char tokenOpenBracket              = 10;
    static const char tokenCloseBracket             = 11;
    static const char tokenColon                    = 12;
    static const char tokenComma                    = 13;
    static const char tokenError                    = 14;
    static const char tokenBackslash                = 15;
    static const char tokenQuoteUnclosedEscapeAtEnd = 16;
    static const char tokenQuoteUnclosedStandard    = 17;
    static const char tokenUnknown                  = 18;
  };
  char theType;
  bool theBoolean;
  double theFloat;
  std::string stringValue;
  MemorySaving<LargeInteger> theInteger;
  ListReferences<JSData> listObjects;
  MapReferences<std::string, JSData, MathRoutines::hashString> objects;
  unsigned int hashFunction() const;
  static unsigned int hashFunction(const JSData& input) {
    return input.hashFunction();
  }
  void operator=(const bool other);
  void operator=(int other);
  void operator=(int64_t other);
  void operator=(const double other);
  void operator=(const std::string& other);
  void operator=(const char* other);
  JSData& operator[](int i);
  JSData& operator[](const std::string& s);
  JSData getValue(const std::string& key);
  bool mergeInMe(const JSData& input, std::stringstream* commentsOnFailure);
  bool hasKey(const std::string& key) const;
  bool hasCompositeKey(const std::string& key, JSData* whichValue, std::stringstream* commentsOnFailure) const;
  bool hasCompositeKeyOfToken(const std::string& key, JSData* whichValue, char targetType, std::stringstream* commentsOnFailure) const;
  bool hasCompositeKeyOfType(const std::string& key, std::string& output, std::stringstream* commentsOnFailure) const;
  bool hasCompositeKeyOfType(const std::string& key, LargeIntegerUnsigned& output, std::stringstream* commentsOnFailure) const;
  bool hasCompositeKeyOfType(const std::string& key, List<unsigned char>& output, std::stringstream* commentsOnFailure) const;
  bool hasCompositeKeyValueNull(const std::string& key, std::stringstream* commentsOnFailure) const;

  void setKeyValue(const std::string& key, const JSData& value);
  int getKeyIndex(const std::string& key) const;
  static JSData makeEmptyArray();
  JSData();
  ~JSData();
  JSData(const JSData& other);
  bool isEqualTo(const std::string& other);
  void operator=(const JSData& other);
  void operator=(const List<JSData>& other);
  void operator=(const List<int>& other);
  void operator=(const List<unsigned char>& other);
  template <typename any>
  void operator=(const List<any>& other) {
    this->reset();
    this->theType = JSData::token::tokenArray;
    this->listObjects.setSize(other.size);
    for (int i = 0; i < other.size; i ++) {
      this->listObjects[i] = other[i];
    }
  }
  void operator=(const LargeInteger& input);
  template <typename Coefficient>
  void operator=(const Vector<Coefficient>& other);
  template <typename Coefficient>
  void operator=(const Matrix<Coefficient>& other);
  bool isNullOrUndefined();
  bool isIntegerFittingInInt(int* whichInteger);
  bool isTrueRepresentationInJSON();
  bool isListOfStrings(List<std::string>* whichStrings);
  bool isString(std::string* whichString);
  // parsing
  bool isValidElement();
  void reset(char inputType = JSData::token::tokenUndefined);
  class PrintOptions {
  public:
    bool useNewLine;
    bool hexEncodeNonAsciiStrings;
    bool useHTML;
    int indentation;
    PrintOptions();
    static const JSData::PrintOptions& newLine();
    static const JSData::PrintOptions& HTML();
    static const PrintOptions& hexEncodeNonASCII();
  };
  std::string toString(const JSData::PrintOptions* options = nullptr) const;
  template <typename somestream>
  somestream& intoStream(
    somestream& out,
    const JSData::PrintOptions* optionsIncoming
  ) const;
  bool tokenizePrependOneDummyElement(
    const std::string& input,
    List<JSData>& output,
    std::stringstream* commentsOnFailure
  );
  static bool convertTwoByteHexToChar(
    char inputLeft, char inputRight, char& output, std::stringstream* commentsOnFailure
  );
  static bool readstringConsumeFourHexAppendUnicode(
    std::string& output,
    unsigned& currentIndex,
    const std::string& input,
    std::stringstream* commentsOnFailure
  );
  bool readstringConsumeNextCharacter(
    List<JSData>& readingStack,
    unsigned& currentIndex,
    const std::string& input,
    std::stringstream* commentsOnFailure
  );
  bool operator==(const JSData& other) const;
  bool readstring(const std::string& json, std::stringstream* commentsOnFailure = nullptr);
  bool tryToComputeType(std::stringstream* commentsOnFailure);
  static void filterColumnsJSDataObjectList(List<JSData>& inputOutput, const List<std::string>& columnsToPreserve);
  class Test {
  public:
    static bool all();
    static bool testRecode();
    static bool testBadInput();
  };
};

std::ostream& operator<<(std::ostream& out, const JSData& data);

template <typename Coefficient>
void JSData::operator=(const Vector<Coefficient>& other) {
  for (int i = 0; i < other.size; i ++) {
    (*this)[i] = other[i];
  }
}

template <typename Coefficient>
void JSData::operator=(const Matrix<Coefficient>& other) {
  for (int i = 0; i < other.numberOfRows; i ++) {
    for (int j = 0; j < other.numberOfColumns; j ++) {
      (*this)[i][j] = other(i, j);
    }
  }
}

#endif
