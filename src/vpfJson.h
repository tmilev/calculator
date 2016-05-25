//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfJson_h_already_included
#define vpfJson_h_already_included

#include "vpfHeader2Math0_General.h" // Matrix
#include "vpfHeader2Math01_LargeIntArithmetic.h" // Rational
static ProjectInformationInstance vpfJson(__FILE__, "Implementation of JSON, work in progress, Thomas.");


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
#define JSNULL 0
#define JSBOOL 1
#define JSNUM 2
#define JSSTR 3
#define JSLIST 4
#define JSOBJ 5

//struct JSHashData;
class JSData
{
public:
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

  // there has to be a better way to do this
  void operator=(const Rational& other)
  { this->type = JSNUM;
    this->number = other.GetDoubleValue();
  }

  template <typename coefficient>
  void operator=(const Vector<coefficient>& other);
  template <typename coefficient>
  void operator=(const Matrix<coefficient>& other);

  // parsing
  void ExtractScalar(const std::string& json, int begin, int end);
  std::string GetString(const std::string& json, int begin, int end, int* actualend = NULL) const;
  int AcceptString(const std::string& json, int begin);
  int AcceptList(const std::string& json, int begin);
  int AcceptObject(const std::string& json, int begin);

  JSData()
  { type=JSNULL;
  }
  std::string ToString() const;
  template <typename somestream>
  somestream& IntoStream(somestream& out) const;
  void readfile(const char* filename);
  void readstring(const std::string& json);
  void writefile(const char* filename) const;
};

struct JSHashData
{ std::string key;
  struct JSData value;
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
