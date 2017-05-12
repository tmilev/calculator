//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfJson_h_already_included
#define vpfJson_h_already_included

#include "vpfHeader2Math0_General.h" // Matrix
#include "vpfHeader2Math01_LargeIntArithmetic.h" // Rational
static ProjectInformationInstance vpfJson(__FILE__, "Implementation of JSON, work in progress.");


/*The best kind of misleading documentation is aspirational
  JSData data;
  data["name"] = "JSLibrary";
  data["version"] = 3.14;
  data["types"][0] = "null";
  data["types"][1] = "boolean";
  data["types"][2] = "number";
  data.writefile("jslib");
  stOutput << data << "\n";
  JSData data2;
  data2.readfile("jslib");
  stOutput << data2 << "\n";
*/

//enum JSType{null, boolean, number, string, list, hash};
// error: 'JSData::number' cannot appear in a constant-expression
// or
// error: 'JSType' is not a cass or namespace

//struct JSHashData;
class JSData
{
public:
  static const char numEmptyTokensAtStart= 6;
  static const char JSUndefined   = 0;
  static const char JSnull        = 1;
  static const char JSbool        = 2;
  static const char JSnumber      = 3;
  static const char JSstring      = 4;
  static const char JSarray       = 5;
  static const char JSObject      = 6;
  static const char JSopenBrace   = 7;
  static const char JScloseBrace  = 8;
  static const char JSopenBracket = 9;
  static const char JScloseBracket= 10;
  static const char JScolon       = 11;
  static const char JScomma       = 12;
  static const char JSerror       = 13;

  char type;
  bool boolean;
  double number;
  std::string string;
  List<JSData> list;
  List<struct JSHashData> obj;

  void operator=(const bool other);
  void operator=(const double other);
  void operator=(const std::string& other);
  JSData& operator[](int i);
  JSData& operator[](const std::string& s);
  JSData GetValue(const std::string& key);
  bool HasKey(const std::string& key);
  int GetKeyIndex(const std::string& key);
  JSData()
  { this->reset();
  }
  // there has to be a better way to do this
  void operator=(const Rational& other)
  { this->type = this->JSnumber;
    this->number = other.GetDoubleValue();
  }

  template <typename coefficient>
  void operator=(const Vector<coefficient>& other);
  template <typename coefficient>
  void operator=(const Matrix<coefficient>& other);

  // parsing
  void ExtractScalar(const std::string& json, int begin, int end);
  bool IsValidElement();
  void reset();
  std::string ToString(bool useHTML) const;
  template <typename somestream>
  somestream& IntoStream(somestream& out, int indentation=0, bool useHTML=false) const;
  void readfile(const char* filename);
  bool readstring(const std::string& json, std::stringstream* commentsOnFailure=0);
  void TryToComputeType();
  static bool Tokenize(const std::string& input, List<JSData>& output);
  void writefile(const char* filename) const;
};

struct JSHashData
{ std::string key;
  JSData value;
};

std::ostream& operator<<(std::ostream& out, const JSData& data);

template <typename coefficient>
void JSData::operator=(const Vector<coefficient>& other)
{ for(int i=0; i<other.size;i++)
    (*this)[i] = other.TheObjects[i];
}

template <typename coefficient>
void JSData::operator=(const Matrix<coefficient>& other)
{ for(int i=0; i<other.NumRows; i++)
    for(int j=0; j<other.NumCols; j++)
      (*this)[i][j] = other(i,j);
}

#endif