//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#ifndef vpfJson_h_already_included
#define vpfJson_h_already_included

#include "general_memory_saving.h"
#include "general_list_references.h"
#include "general_maps.h"


static ProjectInformationInstance vpfJson(__FILE__, "Implementation of JSON, work in progress.");

//struct JSHashData;
class Rational;
template <typename coefficient>
class Matrix;
template <typename coefficient>
class Vector;
class LargeInt;

class JSData {
public:
  static const int numEmptyTokensAtStart = 6;
  struct token {
    static const char tokenUndefined    = 0;
    static const char tokenNull         = 1;
    static const char tokenBool         = 2;
    static const char tokenFloat        = 3;
    static const char tokenString       = 4;
    static const char tokenArray        = 5;
    static const char tokenObject       = 6;
    static const char tokenOpenBrace    = 7;
    static const char tokenCloseBrace   = 8;
    static const char tokenOpenBracket  = 9;
    static const char tokenCloseBracket = 10;
    static const char tokenColon        = 11;
    static const char tokenComma        = 12;
    static const char tokenError        = 13;
    static const char tokenLargeInteger = 14;
  };
  char theType;
  bool theBoolean;
  double theFloat;
  std::string theString;
  MemorySaving<LargeInt> theInteger;
  List<JSData> theList;
  MapReferenceS<std::string, JSData, MathRoutines::HashString> objects;
  void operator=(const bool other);
  void operator=(int other);
  void operator=(int64_t other);
  void operator=(const double other);
  void operator=(const std::string& other);
  void operator=(const char* other);
  JSData& operator[](int i);
  JSData& operator[](const std::string& s);
  JSData GetValue(const std::string& key);
  bool HasKey(const std::string& key) const;
  bool HasCompositeKey(const std::string& key, JSData* whichValue, std::stringstream* commentsOnFailure) const;
  bool HasCompositeKeyOfTokeN(const std::string& key, JSData* whichValue, char targetType, std::stringstream* commentsOnFailure) const;
  bool HasCompositeKeyOfType(const std::string& key, std::string& output, std::stringstream* commentsOnFailure) const;
  bool HasCompositeKeyOfType(const std::string& key, LargeIntUnsigned& output, std::stringstream* commentsOnFailure) const;
  bool HasCompositeKeyOfType(const std::string& key, List<unsigned char>& output, std::stringstream* commentsOnFailure) const;
  bool HasCompositeKeyValueNull(const std::string& key, std::stringstream* commentsOnFailure) const;

  void SetKeyValue(const std::string& key, const JSData& value);
  int GetKeyIndex(const std::string& key) const;
  JSData();
  ~JSData();
  JSData(const JSData& other);
  JSData(const char* other);
  void operator=(const JSData& other);
  void operator=(const List<JSData>& other);
  template <typename any>
  void operator=(const List<any>& other) {
    this->reset();
    this->theType = JSData::token::tokenArray;
    this->theList.SetSize(other.size);
    for (int i = 0; i < other.size; i ++) {
      this->theList[i] = other[i];
    }
  }
  void operator=(const LargeInt& input);
  void operator=(const List<int>& other);
  template <typename coefficient>
  void operator=(const Vector<coefficient>& other);
  template <typename coefficient>
  void operator=(const Matrix<coefficient>& other);
  bool isIntegerFittingInInt(int* whichInteger);
  bool isTrueRepresentationInJSON();
  bool isListOfStrings(List<std::string>* whichStrings);
  // parsing
  void ExtractScalar(const std::string& json, int begin, int end);
  bool IsValidElement();
  void reset(char inputType = JSData::token::tokenUndefined);
  std::string ToString(
    bool percentEncodeKeysIncludingDotsExcludingDollarSigns,
    bool useNewLine = false,
    bool useHTML = false,
    bool convertNonASCIIStringsToHex = false
  ) const;
  static std::string EncodeKeyForMongo(const std::string& input);
  template <typename somestream>
  somestream& IntoStream(
    somestream& out,
    bool percentEncodeStrings,
    int indentation = 0,
    bool useNewLine = false,
    bool useHTML = false,
    bool convertNonASCIIStringsToHex = false
  ) const;
  bool readstring(const std::string& json, bool stringsWerePercentEncoded, std::stringstream* commentsOnFailure = nullptr);
  void TryToComputeType();
  static bool Tokenize(const std::string& input, List<JSData>& output);
  static void FilterColumnsJSDataObjectList(List<JSData>& inputOutput, const List<std::string>& columnsToPreserve);
};

std::ostream& operator<<(std::ostream& out, const JSData& data);

template <typename coefficient>
void JSData::operator=(const Vector<coefficient>& other) {
  for (int i = 0; i < other.size; i ++) {
    (*this)[i] = other[i];
  }
}

template <typename coefficient>
void JSData::operator=(const Matrix<coefficient>& other) {
  for (int i = 0; i < other.NumRows; i ++) {
    for (int j = 0; j < other.NumCols; j ++) {
      (*this)[i][j] = other(i, j);
    }
  }
}

#endif
