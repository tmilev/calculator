//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfJsons_h_already_included
#define vpfJson_h_already_included

#include "vpfHeader2Math01_LargeIntArithmetic.h"
static ProjectInformationInstance vpfJson(__FILE__, "Header for incomplete and broken JSON library");


/*The best kind of misleading documentation is aspirational
  JSData data;
  data["name"] = "JSLibrary";
  data["version"] = 3.14;
  data["types"][0] = "null";
  data["types"][1] = "boolean";
  data["types"][2] = "number";
  data.writefile("jslib");
  std::cout << data << std::endl;
  JSData data2;
  data2.readfile("jslib");
  std::cout << data2 << std::endl;
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
  void writefile(const char* filename) const;
};

struct JSHashData
{ std::string key;
  struct JSData value;
};

std::ostream& operator<<(std::ostream& out, const JSData& data);

#endif
