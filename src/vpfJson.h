//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfJson_h_already_included
#define vpfJson_h_already_included

#include "vpfHeader1General1_ListReferences.h"
#include "vpfHeader1General6Maps.h"
static ProjectInformationInstance vpfJson(__FILE__, "Implementation of JSON, work in progress.");

//struct JSHashData;
class Rational;
template <typename coefficient>
class Matrix;
template <typename coefficient>
class Vector;

class JSData {
public:
  static const int numEmptyTokensAtStart = 6;
  static const char JSUndefined    = 0;
  static const char JSnull         = 1;
  static const char JSbool         = 2;
  static const char JSnumber       = 3;
  static const char JSstring       = 4;
  static const char JSarray        = 5;
  static const char JSObject       = 6;
  static const char JSopenBrace    = 7;
  static const char JScloseBrace   = 8;
  static const char JSopenBracket  = 9;
  static const char JScloseBracket = 10;
  static const char JScolon        = 11;
  static const char JScomma        = 12;
  static const char JSerror        = 13;

  char theType;
  bool theBoolean;
  double number;
  std::string theString;
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
  void SetKeyValue(const std::string& key, const JSData& value);
  int GetKeyIndex(const std::string& key) const;
  JSData() {
    this->reset();
  }
  JSData(const JSData& other) {
    this->operator=(other);
  }
  JSData(const char* other) {
    this->reset();
    this->operator=(other);
  }
  void operator=(const JSData& other) {
    this->theType = other.theType;
    this->theBoolean = other.theBoolean;
    this->number = other.number;
    this->theString = other.theString;
    this->theList = other.theList;
    this->objects = other.objects;
  }
  void operator=(const List<JSData>& other);
  template <typename any>
  void operator=(const List<any>& other) {
    this->reset();
    this->theType = this->JSarray;
    this->theList.SetSize(other.size);
    for (int i = 0; i < other.size; i ++) {
      this->theList[i] = other[i];
    }
  }
  void operator=(const Rational& other);

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
  void reset(char inputType = JSData::JSUndefined);
  std::string ToString(bool percentEncodeKeysIncludingDotsExcludingDollarSigns, bool useNewLine = false, bool useHTML = false) const;
  static std::string EncodeKeyForMongo(const std::string& input);
  template <typename somestream>
  somestream& IntoStream(
    somestream& out, bool percentEncodeStrings, int indentation = 0, bool useNewLine = false, bool useHTML = false
  ) const;
  bool readstring(const std::string& json, bool stringsWerePercentEncoded, std::stringstream* commentsOnFailure = 0);
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
