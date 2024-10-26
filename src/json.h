#ifndef header_json_ALREADY_INCLUDED
#define header_json_ALREADY_INCLUDED

#include "general_list_references.h"
#include "general_maps.h"
#include "general_memory_saving.h"

// struct JSHashData;
class Rational;
template <typename Coefficient>
class Matrix;
template <typename Coefficient>
class Vector;
class LargeInteger;

class JSData {
public:
  enum Type {
    tokenUndefined,
    tokenNull,
    tokenBool,
    tokenFloat,
    tokenString,
    tokenArray,
    tokenObject,
    tokenLargeInteger,
  };
  JSData::Type elementType;
  bool booleanValue;
  double floatValue;
  std::string stringValue;
  MemorySaving<LargeInteger> integerValue;
  ListReferences<JSData> listObjects;
  MapReferences<std::string, JSData> objects;
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
  void operator=(const JSData& other);
  void operator=(const List<JSData>& other);
  void operator=(const List<int>& other);
  void operator=(const List<unsigned char>& other);
  template <typename any>
  void operator=(const List<any>& other) {
    this->reset();
    this->elementType = JSData::Type::tokenArray;
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
  bool operator==(const JSData& other) const;
  bool operator!=(const JSData& other) const;
  JSData& operator[](int i);
  JSData& operator[](const std::string& s);
  JSData getValue(const std::string& key) const;
  bool mergeInMe(
    const JSData& input, std::stringstream* commentsOnFailure
  );
  bool hasKey(const std::string& key) const;
  bool hasCompositeKey(
    const std::string& key,
    JSData* whichValue,
    std::stringstream* commentsOnFailure
  ) const;
  bool hasCompositeKeyOfToken(
    const std::string& key,
    JSData* whichValue,
    char targetType,
    std::stringstream* commentsOnFailure
  ) const;
  bool hasCompositeKeyOfType(
    const std::string& key,
    std::string& output,
    std::stringstream* commentsOnFailure
  ) const;
  bool hasCompositeKeyOfType(
    const std::string& key,
    LargeIntegerUnsigned& output,
    std::stringstream* commentsOnFailure
  ) const;
  bool hasCompositeKeyOfType(
    const std::string& key,
    List<unsigned char>& output,
    std::stringstream* commentsOnFailure
  ) const;
  bool hasCompositeKeyValueNull(
    const std::string& key, std::stringstream* commentsOnFailure
  ) const;
  bool hasNestedKey(
    const List<std::string>& nestedKeys, JSData* whichValue = nullptr
  ) const;
  void setKeyValue(const std::string& key, const JSData& value);
  int getKeyIndex(const std::string& key) const;
  void makeEmptyArray();
  static JSData emptyArray();
  JSData();
  ~JSData();
  JSData(const JSData& other);
  bool isEqualTo(const std::string& other);
  bool isNullOrUndefined();
  bool isIntegerFittingInInt(int* whichInteger);
  bool isTrueRepresentationInJSON();
  bool isListOfStrings(List<std::string>* whichStrings);
  bool isString(std::string* whichString = nullptr) const;
  // parsing
  bool isValidElement();
  void reset(Type inputType = JSData::Type::tokenUndefined);
  class PrintOptions {
  public:
    bool useNewLine;
    bool useHTML;
    int indentation;
    PrintOptions();
    static const JSData::PrintOptions& newLine();
    static const JSData::PrintOptions& HTML();
  };

  std::string toString(const JSData::PrintOptions* options = nullptr) const;
  std::string toStringPretty() const;
  std::stringstream& intoStream(
    std::stringstream& out, const JSData::PrintOptions* optionsIncoming
  ) const;
  static bool convertTwoByteHexToUnsignedChar(
    unsigned char inputLeft,
    unsigned char inputRight,
    unsigned char& output,
    std::stringstream* commentsOnFailure
  );
  void parseNoFail(const std::string& json, bool relaxedInput = false);
  bool parse(
    const std::string& json,
    bool relaxedInput = false,
    std::stringstream* commentsOnFailure = nullptr,
    bool useHTMLInComments = false
  );
  static void filterColumnsJSDataObjectList(
    List<JSData>& inputOutput,
    const List<std::string>& columnsToPreserve
  );
  class Test {
  public:
    static bool all();
    static bool recode();
    static bool recodeOnce(
      const List<std::string>& pair, bool relaxedInput
    );
    static bool recodeRelaxed();
    static bool decodeEscapedUnicode();
    static bool badInput();
    static bool loadLarger();
  };
};

// Helper class used when parsing jsons. Holds either a fully parsed json
// or a syntactic element.
class JSONWithTokens {
public:
  static const int numberOfEmptyTokensAtStart = 6;
  enum Token {
    tokenUnknown,
    tokenJSON,
    tokenOpenBrace,
    tokenCloseBrace,
    tokenOpenBracket,
    tokenCloseBracket,
    tokenColon,
    tokenComma,
    tokenParsingStart,
    tokenParsingEnd,
    tokenError,
    tokenBackslash,
    tokenQuoteUnclosedEscapeAtEnd,
    tokenQuoteUnclosedStandard,
  };
  MemorySaving<JSData> container;
  JSONWithTokens::Token token;
  std::string source;
  JSData& content();
  bool isJSON() const;
  JSONWithTokens();
  std::string toString() const;
  std::string toString(bool useHTML) const;
  std::string toStringToken(bool useHTML) const;
  bool tryToComputeType(std::stringstream* commentsOnFailure);
  static bool tokenize(
    const std::string& input,
    List<JSONWithTokens>& output,
    bool relaxedInput,
    std::stringstream* commentsOnFailure
  );
  static bool readstringConsumeNextCharacter(
    List<JSONWithTokens>& readingStack,
    unsigned& currentIndex,
    const std::string& input,
    std::stringstream* commentsOnFailure
  );
  static bool readstringConsumeUnicodeFourHexAppendUtf8(
    std::string& output,
    unsigned& currentIndex,
    const std::string& input,
    std::stringstream* commentsOnFailure
  );
  static std::string toStringTokens(const List<JSONWithTokens>& stack);
  static std::string toStringReadingStack(
    const List<JSONWithTokens>& stack
  );
};

std::ostream& operator<<(std::ostream& out, const JSData& data);
std::ostream& operator<<(std::ostream& out, const JSONWithTokens& data);

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

#endif // header_json_ALREADY_INCLUDED
