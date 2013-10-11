//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
// ^ that is first for precompilation

#include "vpfHeader4SystemFunctionsGlobalObjects.h"

//#include "testlib.h"
#include <iostream>

#include <sys/stat.h>

// no need to lex first
//enum Symbol{bracket,brace,quote};
//class SymbolTree
//{ public:
//  Symbol symbol;
//  int start, end;
//  List<SymbolTree> children;
//};
/*int SymbolTree::ParseSymbol(std::string data, int begin)
{ bool havesymbol = false;
  for(int i = begin; i<data.size && !havesymbol; i++)
  { switch(data[i])
    { case '{':
        this->symbol = brace;
        havesymbol=true;
        break;
      case '[':
        this->symbol = bracket;
        havesymbol=true;
        break;
      case '"':
        this->symbol = quote;
        havesymbol=true;
        break;
    }
  }
  if(!havesymbol)
    return i;
  for(int i=this->begin, i<data.size; i++)
  {

  }

}*/

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

void JSData::operator=(const bool other)
{ this->type = JSBOOL;
  this->boolean = other;
}

void JSData::operator=(const double other)
{ this->type = JSNUM;
  this->number = other;
}

void JSData::operator=(const std::string& other)
{ this->type = JSSTR;
  this->string = other;
}

JSData& JSData::operator[](int i)
{ this->type = JSLIST;
  if(this->list.size < i+1)
    this->list.SetSize(i+1);
  return this->list[i];
}

JSData& JSData::operator[](const std::string& key)
{ this->type = JSOBJ;
  for(int i=0; i<this->obj.size; i++)
    if(this->obj[i].key == key)
      return this->obj[i].value;
  int i = this->obj.size;
  this->obj.SetSize(i+1);
  this->obj[i].key = key;
  return this->obj[i].value;
}

std::string JSData::GetString(const std::string& json, int begin, int end, int* actualend) const
{ // i don't expect this to be bigger then 2147482647 but smaller then 4294967296
  // the compiler whines about comparison between int i and json.size()
  unsigned int i;
  unsigned int minend;
  bool endlt;
  if(end>0 and ((unsigned int) end)<json.size())
  { endlt = true;
    minend = end;
  }
  else
  { endlt = false;
    minend = json.size();
  }
  for(i=begin; i<minend; i++)
    if(json[i] == '"')
    { i += 1;
      break;
    }
  if(i>=minend)
  { std::cout << "GetString: there does not appear to be a string between " << begin << " and " << minend << std::endl;
    if(actualend)
      *actualend = minend;
    return json.substr(i,0);
  }
  for(; i<minend; i++)
  { if(json[i] == '"')
    { if(actualend)
        *actualend = i;
      return json.substr(begin+1,i-begin-1);
    }
  }
  if(endlt)
  { std::cout << "string being extracted failed to terminate in a timely manner, ending it at position" << i << std::endl;
    if(actualend)
      *actualend = i;
    return json.substr(begin+1,i-begin);
  }
  std::cout << "json ended before string being extracted" << std::endl;
  if(actualend)
    *actualend = i;
  return json.substr(begin+1,i-begin);
}

int JSData::AcceptString(const std::string& json, int begin)
{ this->type = JSSTR;
  // i don't expect this to be bigger then 2147482647 but smaller then 4294967296
  // the compiler whines about comparison between int i and json.size()
  unsigned int i = begin+1;
  for(; i<json.size(); i++)
  { if(json[i] == '"')
    { this->string = json.substr(begin+1,i-begin-1);
      return i;
    }
  }
  std::cout << "parse error: input string ended but current data string is incomplete" << std::endl;
  this->string = json.substr(begin+1,json.size()-begin-1);
  return json.size();
}

void JSData::ExtractScalar(const std::string& json, int begin, int end)
{ if(begin==end)
    return;
  std::string sbs = json.substr(begin,end-begin);
  this->type = JSNUM;
  this->number = atof(sbs.c_str());
}

int JSData::AcceptList(const std::string& json, int begin)
{ this->type = JSLIST;
  unsigned int i=begin+1;
  unsigned int curobjbgn = i;
  bool havecurobj = false;
  for(; i<json.size(); i++)
  { if(json[i] == ']')
    { if(!havecurobj)
      { // hack: should instead test in extractScalar for any type of reasonable
        // scalar.  don't.  no need to figure that out yet.
        if(i>curobjbgn)
        { this->list.SetSize(this->list.size+1);
          this->list[this->list.size-1].ExtractScalar(json, curobjbgn, i);
        }
      }
      return i;
    }
    if(json[i] == '[')
    { this->list.SetSize(this->list.size+1);
      i = this->list[this->list.size-1].AcceptList(json,i);
      havecurobj = true;
      continue;
    }
    if(json[i] == '"')
    { this->list.SetSize(this->list.size+1);
      i = this->list[this->list.size-1].AcceptString(json,i);
      havecurobj = true;
      continue;
    }
    if(json[i] == '{')
    { this->list.SetSize(this->list.size+1);
      i = this->list[this->list.size-1].AcceptObject(json,i);
      havecurobj = true;
      continue;
    }
    if(json[i] == ',')
    { if(!havecurobj)
      { this->list.SetSize(this->list.size+1);
        this->list[this->list.size-1].ExtractScalar(json, curobjbgn, i);
      }
      havecurobj = false;
      curobjbgn = i+1;
      continue;
    }
  }
  std::cout << "parse error: string ended but list is incomplete" << std::endl;
  return i;
}

int JSData::AcceptObject(const std::string& json, int begin)
{ this->type = JSOBJ;
  unsigned int i=begin+1;
  unsigned int curobjbgn = i;
  bool isvaluetime = false;
  bool havecurobj = false;
  for(; i<json.size(); i++)
  { if(json[i] == '}')
    { if(!isvaluetime)
      { std::cout << "parse error: } in key, character " << i << " key will be ignored" << std::endl;
        return i;
      }
      if(!havecurobj)
        this->obj[this->obj.size-1].value.ExtractScalar(json,curobjbgn,i-1);
      return i;
    }
    if(json[i] == ':')
    { if(isvaluetime)
      { std::cout << "parse error: : in value, second key ignored" << std::endl;
        curobjbgn = i+1;
        continue;
      }
      isvaluetime = true;
      this->obj.SetSize(this->obj.size+1);
      this->obj[this->obj.size-1].key = this->GetString(json,curobjbgn,i);
      curobjbgn = i+1;
      havecurobj = false;
    }
    if(json[i] == ',')
    { if(!isvaluetime)
      { std::cout << "parse error: , in key, key ignored";
        curobjbgn = i+1;
        continue;
      }
      if(!havecurobj)
        this->obj[this->obj.size-1].value.ExtractScalar(json, curobjbgn, i-1);
      isvaluetime = false;
      curobjbgn = i+1;
    }
    if(isvaluetime)
    { if(json[i] == '[')
      { i = this->obj[this->obj.size-1].value.AcceptList(json,i);
        havecurobj = true;
        continue;
      }
      if(json[i] == '"')
      { i = this->obj[this->obj.size-1].value.AcceptString(json,i);
        havecurobj = true;
        continue;
      }
      if(json[i] == '{')
      { i = this->obj[this->obj.size-1].value.AcceptObject(json,i);
        havecurobj = true;
        continue;
      }
    }
  }
  std::cout << "parse error: string ended but object is incomplete" << std::endl;
  return i;
}

void JSData::readfile(const char* filename)
{ std::ifstream ifp(filename);
  if(!ifp.is_open())
    return;
  struct stat f;
  stat(filename,&f);
  std::string json;
  json.resize(f.st_size);
  ifp.read(&json[0], json.size());

  unsigned int i=0;
  for(; i<json.size(); i++)
  { if(json[i] == '"')
    { this->AcceptString(json,0);
      return;
    }
    if(json[i] == '[')
    { this->AcceptList(json,0);
      return;
    }
    if(json[i] == '{')
    { this->AcceptObject(json,0);
      return;
    }
  }
  this->ExtractScalar(json,0,json.size());
}

template <typename somestream>
somestream& JSData::IntoStream(somestream& out) const
{ switch(this->type)
  { case JSNULL:
      out << "null";
      return out;
    case JSNUM:
      out << this->number;
      return out;
    case JSBOOL:
      if(this->boolean == true)
        out << "true";
      else
        out << "false";
      return out;
    case JSSTR:
      out << '"' << this->string << '"';
      return out;
    case JSLIST:
      out << '[';
      for(int i=0; i<this->list.size; i++)
      { this->list[i].IntoStream(out);
        if(i!=this->list.size-1)
          out << ',';
      }
      out << ']';
      return out;
    case JSOBJ:
      out << '{';
      for(int i=0; i<this->obj.size; i++)
      { out << '"' << this->obj[i].key << '"';
        out << ':';
        this->obj[i].value.IntoStream(out);
        if(i!=this->obj.size-1)
          out << ',';
      }
      out << '}';
      return out;
  }
  //unreachable
  assert(false);
  return out;
}

void JSData::writefile(const char* filename) const
{ std::ofstream out;
  out.open(filename);
  this->IntoStream(out);
}

std::string JSData::ToString() const
{ std::stringstream out;
  this->IntoStream(out);
  return out.str();
}

std::ostream& operator<<(std::ostream& out, const JSData& data)
{ return data.IntoStream(out);
}



class CharacterTable
{
public:
  List<List<int> > representatives;
  List<int> sizes;
  List<Vector<Rational> > characters;

  void write(std::string filename);
  void read(std::string filename);
};


/*{ std::ofstream out;
  out.open(filename.c_str());
  // representatives
  out << '[';
  for(int i=0; i<this->representatives.size; i++)
  { out << '[';
    for(int j=0; j<this->representatives[i].size; j++)
    { out << this->representatives[i][j];
      if(j != this->representatives[i].size-1)
        out << ',';
    }
    out << ']';
    if(i != this->representatives.size-1)
      out << ',';
  }
  out << ']';
  out << '\n';

  // sizes
  out << '[';
  for(int i=0; i<this->sizes.size; i++)
  { out << this->sizes[i];
    if(i != this->sizes.size-1)
      out << ',';
  }
  out << ']';
  out << '\n';

  // characters
  out << '[';
  for(int i=0; i<this->characters.size; i++)
  { out << '[';
    for(int j=0; j<this->characters[i].size; j++)
    { out << '"' << this->characters[i][j] << '"';
      if(j != this->characters[i].size-1)
        out << ',';
    }
    out << ']' << '\n';
    if(i != this->characters.size-1)
      out << ',';
  }
  out << ']';
}*/

bool CharacterComparator(const Vector<Rational>& left, const Vector<Rational>& right)
{ if(left[0] > right[0])
    return true;
  if(left[0] < right[0])
    return false;
  for(int i=1; i<left.size; i++)
  { if(left[i] > right[i])
      return false;
    if(left[i] < right[i])
      return true;
  }
  return false;
}

template <>
unsigned int Vector<int>::HashFunction() const
{ unsigned int result=0;
  int theSize= MathRoutines::Minimum(this->size, SomeRandomPrimesSize);
  for (int i=0; i<theSize; i++)
    result+=  (unsigned int) this->TheObjects[i] *  SomeRandomPrimes[i];
  return result;
}

class PermutationR2
{
public:
  List<List<int> > cycles;

  //void MakeCanonical();
  //unsigned int HashFunction();

  void GetCycleStructure(List<int>& out);
};

void PermutationR2::GetCycleStructure(List<int>& out)
{ int N = 0;
  for(int i=0; i<this->cycles.size; i++)
    if(N < this->cycles[i].size)
      N = this->cycles[i].size;
  out.SetSize(N+1);
  for(int i=0; i<out.size; i++)
    out[i] = 0;
  for(int i=0; i<this->cycles.size; i++)
    out[this->cycles[i].size] += 1;
}

void WeylElementPermutesRootSystem(const ElementWeylGroup& g, PermutationR2& p)
{ int rss = g.owner->RootSystem.size;
  List<bool> accountedFor;
  accountedFor.SetSize(rss);
  for(int i=0; i<rss; i++)
    accountedFor[i] = false;
  for(int i=0; i<rss; i++)
  { if(accountedFor[i])
      continue;
    p.cycles.SetSize(p.cycles.size+1);
    p.cycles[p.cycles.size-1].size = 0;
    int j = i;
    do
    { p.cycles[p.cycles.size-1].AddOnTop(j);
      accountedFor[j] = true;
      j = g.owner->RootSystem.GetIndex(g*g.owner->RootSystem[j]);
    }
    while(j!=i);
    if(p.cycles[p.cycles.size-1].size == 1)
      p.cycles.SetSize(p.cycles.size-1);
  }
}

std::ostream& operator<<(std::ostream& out, const PermutationR2& p)
{ for(int i=0; i<p.cycles.size; i++)
  { out << '(';
    for(int j=0; j<p.cycles[i].size; j++)
    { out << p.cycles[i][j];
      if(j!=p.cycles[i].size-1)
        out << ' ';
    }
    out << ')';
  }
  return out;
}

template <typename scalar>
class PackedVector
{
public:
  scalar data[8];

  static const int size = 8;

  inline scalar& operator[](int i);
  inline scalar operator[](int i) const;
  bool operator!=(const PackedVector<scalar>& other) const;
  bool operator==(const PackedVector<scalar>& other) const;
  bool operator>(const PackedVector<scalar>&other) const;
  // passing in d for compatibility with Vector
  void MakeZero(int d);
  void MakeEi(int d, int i);
  // for compatibility
  void SetSize(int s);

  PackedVector<scalar> operator*(scalar x) const;
  PackedVector<scalar> operator+(const PackedVector<scalar>& w) const;
  void operator+=(const PackedVector<scalar>& w);
  scalar ScalarProduct(const PackedVector<scalar>& v, const Matrix<scalar>& B) const;

  unsigned int HashFunction() const;
  static unsigned int HashFunction(const PackedVector<scalar>& in);
};
template <typename scalar>
const int PackedVector<scalar>::size;

template <typename scalar>
PackedVector<scalar> PackedVector<scalar>::operator+(const PackedVector<scalar>& w) const
{ PackedVector<scalar> out;
  for(int i=0; i<this->size; i++)
    out[i] = this->data[i]+w[i];
  return out;
}

template <typename scalar>
void PackedVector<scalar>::operator+=(const PackedVector<scalar>& w)
{ for(int i=0; i<this->size; i++)
    this->data[i] += w[i];
}

template <typename scalar>
PackedVector<scalar> PackedVector<scalar>::operator*(scalar x) const
{ PackedVector<scalar> out;
  for(int i=0; i<this->size; i++)
    out[i] = this->data[i]*x;
  return out;
}

template <typename scalar>
scalar PackedVector<scalar>::ScalarProduct(const PackedVector<scalar>& v, const Matrix<scalar>& B) const
{ PackedVector<scalar> Bv;
  Bv.MakeZero(B.NumRows);
  for(int i=0; i<B.NumRows; i++)
    for(int j=0; j<B.NumCols; j++)
      Bv[i] += B.elements[i][j]*v[j];
  scalar wBv = 0;
  for(int i=0; i<B.NumRows; i++)
    wBv += this->data[i]*Bv[i];
  return wBv;
}

/*
template <typename scalar>
scalar PackedVector<scalar>::ScalarProduct(const PackedVector<scalar>& v, const PackedVector* B) const
{ PackedVector<scalar> Bv;
  Bv.MakeZero();
  for(int i=0; i<this->size; i++)
    for(int j=0; j<this->size; j++)
      Bv[i] += B[i][j]*v[j]
  scalar wBv = 0;
  for(int i=0; i<B.NumRows; i++)
    wBv += this->data[i]*Bv[i];
  return wBv;
}
*/



// seriously, this wasn't automatically inlined?
template <typename scalar>
inline scalar& PackedVector<scalar>::operator[](int i)
{ return this->data[i];
}

template <typename scalar>
inline scalar PackedVector<scalar>::operator[](int i) const
{ return this->data[i];
}

template <typename scalar>
void PackedVector<scalar>::SetSize(int s)
{ if(s>this->size)
  { std::cout << "if this was intentional, recompile PackedVector with size>=" << s << std::endl;
    assert(false);
  }
}

template <typename scalar>
bool PackedVector<scalar>::operator!=(const PackedVector<scalar>& w) const
{ for(int i=0; i<this->size; i++)
    if(w[i] != this->data[i])
      return true;
  return false;
}

template <typename scalar>
bool PackedVector<scalar>::operator==(const PackedVector<scalar>& w) const
{ return !(*this != w);
}

template <typename scalar>
bool PackedVector<scalar>::operator>(const PackedVector<scalar>&w) const
{ for(int i=0; i<this->size; i++)
  { if(this->data[i] > w[i])
      return true;
    if(this->data[i] < w[i])
      return false;
  }
  return false;
}


template <typename scalar>
void PackedVector<scalar>::MakeZero(int n)
{ for(int i=0; i<this->size; i++)
    this->data[i] = 0;
}

template <typename scalar>
void PackedVector<scalar>::MakeEi(int d, int ei)
{ for(int i=0; i<this->size; i++)
    this->data[i] = 0;
  this->data[ei] = 1;
}

template <typename scalar>
unsigned int PackedVector<scalar>::HashFunction(const PackedVector<scalar>& in)
{ return in.HashFunction();
}

template <typename scalar>
unsigned int PackedVector<scalar>::HashFunction() const
{ unsigned int result=0;
  for (int i=0; i<this->size; i++)
    result+=  (unsigned int) this->data[i] *  SomeRandomPrimes[i];
  return result;
}

template <typename scalar>
std::ostream& operator<<(std::ostream& out, const PackedVector<scalar>& v)
{ out << '(';
  for(int i=0; i<v.size; i++)
  { out << v[i];
    if(i!=v.size-1)
      out << ", ";
  }
  out << ')';
  return out;
}

// in particular, scalar likes to be int
template <typename scalar, typename vector>
class AnotherWeylGroup
{ // compat WeylGroup
public:
  Matrix<Rational> CartanSymmetric;
  vector rho;
  List<vector> RootSystem;
  HashedList<vector> rhoOrbit;
  List<List<int> > conjugacyClasses;
  List<int> classMap;
  List<Matrix<Rational> > classMatrices;

  // needed
  List<Vector<Rational> > characterTable;

  int GetRank() const; // idk lol
  void SimpleReflection(int i, const vector& v, vector& out) const;
  void SimpleReflection(int i, vector& v) const;
  void GetSimpleReflections(const vector& v, List<int>& out) const;
  void GetGeneratorList(int i, List<int>& out) const;
  void ApplyReflectionList(const List<int>& simpleReflections, vector& v) const;
  void ActOn(int g, vector& v) const;
  void ActOn(int g, const vector& v, vector& out) const;
  int Multiply(int g, int h) const;
  int Invert(int g) const;

  void ComputeRho();
  void ComputeAllElements();
  void ComputeConjugacyClasses();
  void ComputeClassMap();
  Matrix<Rational>& GetClassMatrix(int cc);
  void ComputeClassMatrices(); // O(W.size)
  void ComputeTauSignatures();
  Rational GetHermitianProduct(const Vector<Rational>& X1, const Vector<Rational>& X2) const;

  int GetRootReflection(int i) const;
  void GetSignCharacter(Vector<Rational>& out);

  // own stuff
  int size; // useful b/c its nice to not axcidently depend on logically stuff
  int rank;
  vector twiceRho;
  Matrix<scalar> unrationalCartanSymmetric;
  vector* ucsm;

  void PrintCharTable(const char* filename) const;
};

template <typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::SimpleReflection(int i, vector& v) const
{ // next i'll put some printfs in addition
  //std::cout << "Debugging simple reflections: vector " << v << " transformed by reflection " << i;
  scalar x=0;
  for(int j=0; j<this->rank; j++)
    x += this->unrationalCartanSymmetric.elements[i][j] * v[j];
  // so, under ordinary circumstances, this is just out[i] -= x;
  // silent corruption occurs if UnrationalCartanSymmetric.elements[i][i] !∈ {1,2}
  // and scalar can't deal with division properly
  // fortunately this only happens in G₂
  v[i] -= x * 2/ unrationalCartanSymmetric.elements[i][i];
  //std::cout << " becomes " << v << std::endl;
}

template <typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::SimpleReflection(int i, const vector& v, vector& out) const
{ out = v;
  this->SimpleReflection(i,out);
}

template <typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::GetSimpleReflections(const vector& v, List<int>& out) const
{ vector w = v;
  while(w != this->twiceRho)
  { for(int i=0; i<this->rank; i++)
    { scalar x=0;
      for(int j=0; j<this->rank; j++)
        if(this->unrationalCartanSymmetric.elements[i][j] != 0)
          x += this->unrationalCartanSymmetric.elements[i][j] * w[j];
      if(x>0)
        continue;
      out.AddOnTop(i);
      SimpleReflection(i,w);
    }
  }
}

template <typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::GetGeneratorList(int i, List<int>& out) const
{ this->GetSimpleReflections(this->rhoOrbit[i], out);
}

// copypaste another function into this so the autovectorizer will work
template <typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::ApplyReflectionList(const List<int>& simpleReflections, vector& v) const
{ for(int ii=simpleReflections.size-1; ii>=0; ii--)
  { //this->SimpleReflection(simpleReflections[i],v);
    int i = simpleReflections[ii];
    scalar x=0;
    for(int j=0; j<this->rank; j++)
      x += this->unrationalCartanSymmetric.elements[i][j] * v[j];
    // so, under ordinary circumstances, this is just out[i] -= x;
    // silent corruption occurs if UnrationalCartanSymmetric.elements[i][i] !∈ {1,2}
    // and scalar can't deal with division properly
    // fortunately this only happens in G₂
    v[i] -= x * 2/ unrationalCartanSymmetric.elements[i][i];
  }
}

template <typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::ActOn(int i, vector& v) const
{ List<int> simpleReflections;
  GetSimpleReflections(this->rhoOrbit[i],simpleReflections);
  this->ApplyReflectionList(simpleReflections,v);
}

template <typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::ActOn(int i, const vector& v, vector& out) const
{ out = v;
  this->ActOn(i,out);
}

template <typename scalar, typename vector>
int AnotherWeylGroup<scalar, vector>::Multiply(int i, int j) const
{ vector v = this->rhoOrbit[j];
  this->ActOn(i,v);
  return this->rhoOrbit.GetIndex(v);
}

template <typename scalar, typename vector>
int AnotherWeylGroup<scalar, vector>::Invert(int i) const
{ List<int> srsl;
  GetSimpleReflections(this->rhoOrbit[i], srsl);
  vector v = twiceRho;
  for(int i=srsl.size-1; i>=0; i--)
    this->SimpleReflection(srsl[i], v);
  return this->rhoOrbit.GetIndex(v);
}

template <typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::ComputeRho()
{ if(unrationalCartanSymmetric.NumRows < CartanSymmetric.NumRows)
  { int den = 1;
    for(int i=0; i<CartanSymmetric.NumRows; i++)
      for(int j=0; j<CartanSymmetric.NumCols; j++)
        den = MathRoutines::lcm(den,CartanSymmetric.elements[i][j].GetDenominator().GetUnsignedIntValueTruncated());
    unrationalCartanSymmetric.init(CartanSymmetric.NumRows, CartanSymmetric.NumCols);
    //this->ucsm = malloc(CartanSymmetric.NumRows*sizeof(vector));
    //for(int i=0; i<CartanSymmetric.NumRows; i++)
    //  this->ucsm[i].SetSize(CartanSymmetric.NumCols);
    for(int i=0; i<CartanSymmetric.NumRows; i++)
      for(int j=0; j<CartanSymmetric.NumCols; j++)
      { unrationalCartanSymmetric.elements[i][j] = (CartanSymmetric.elements[i][j]*den).GetNumerator().GetIntValueTruncated();
        //ucsm[i][j] = unrationalCartanSymmetric.elements[i][j];
      }

  }
  for(int i=0; i<unrationalCartanSymmetric.NumRows; i++)
  { for(int j=0; j<unrationalCartanSymmetric.NumCols; j++)
      std::cout << unrationalCartanSymmetric.elements[i][j] << '\t';
    std::cout << '\n';
  }
  this->rank = this->unrationalCartanSymmetric.NumRows;
  for(int rvi=0; rvi<rank; rvi++)
  { vector vi;
    vi.MakeEi(rank,rvi);
    if(this->RootSystem.Contains(vi))
      continue;
    this->RootSystem.AddOnTop(vi);
    List<int> newelts;
    vector w;
    newelts.AddOnTop(this->RootSystem.GetIndex(vi));
    while(newelts.size > 0)
    { int i = newelts.PopLastObject();
      for(int si=0; si<this->rank; si++)
      { this->SimpleReflection(si,this->RootSystem[i],w);
        if(this->RootSystem.GetIndex(w) == -1)
        { this->RootSystem.AddOnTop(w);
          int j = this->RootSystem.GetIndex(w);
          newelts.AddOnTop(j);
        }
      }
    }
  }
  this->RootSystem.QuickSortAscending();
  this->twiceRho.MakeZero(rank);
  for(int i=0; i<this->RootSystem.size; i++)
  { bool usethis = true;
    for(int j=0; j<this->RootSystem[i].size; j++)
      if(this->RootSystem[i][j] < 0)
      { usethis = false;
        break;
      }
    if(usethis)
      this->twiceRho += this->RootSystem[i];
  }
  this->rho.SetSize(this->twiceRho.size);
  for(int i=0; i<this->twiceRho.size; i++)
    this->rho[i] = this->twiceRho[i]/2;
}

template <typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::ComputeAllElements()
{ std::cout << "Getting elements...";
  if(this->RootSystem.size == 0)
    this->ComputeRho();
  std::cout << "(twiceRho is " << this->twiceRho << ")" << std::endl;
  vector w;
  List<int> newelts;
  this->rhoOrbit.AddOnTop(twiceRho);
  newelts.AddOnTop(this->rhoOrbit.GetIndex(twiceRho));
  while(newelts.size > 0)
  { int i = newelts.PopLastObject();
    for(int si=0; si<this->rank; si++)
    { this->SimpleReflection(si,this->rhoOrbit[i],w);
      if(this->rhoOrbit.GetIndex(w) == -1)
      { this->rhoOrbit.AddOnTop(w);
        int j = this->rhoOrbit.GetIndex(w);
        newelts.AddOnTop(j);
      }
    }
  }
  this->size = this->rhoOrbit.size;
  std::cout << this->size << std::endl;
//  std::cout << rhoOrbit << std::endl;
}

template <typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::ComputeConjugacyClasses()
{ std::cout << "Getting conjugacy classes...";
  if(this->rhoOrbit.size == 0)
    this->ComputeAllElements();
  List<bool> Accounted;
  Accounted.initFillInObject(this->size, false);
  this->conjugacyClasses.SetSize(0);
  this->conjugacyClasses.ReservE(50);
  HashedList<int, MathRoutines::IntUnsignIdentity> theStack;
  theStack.SetExpectedSize(this->size);
  int theRank=this->GetRank();
  vector theRhoImage;
  for (int i=0; i<this->size; i++)
    if (!Accounted[i])
    { theStack.Clear();
      theStack.AddOnTop(i);
      for (int j=0; j<theStack.size; j++)
        for (int k=0; k<theRank; k++)
        { theRhoImage=this->twiceRho;
          this->SimpleReflection(k, theRhoImage);
          this->ActOn(theStack[j], theRhoImage);
          this->SimpleReflection(k, theRhoImage);
          int accountedIndex=this->rhoOrbit.GetIndex(theRhoImage);
          theStack.AddOnTopNoRepetition(accountedIndex);
          Accounted[accountedIndex]=true;
        }
      this->conjugacyClasses.AddOnTop(theStack);
      this->conjugacyClasses.LastObject()->QuickSortAscending();
    }
  this->conjugacyClasses.QuickSortAscending();
  std::cout << conjugacyClasses.size << std::endl;
}

template<typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::ComputeClassMap()
{ this->classMap.SetSize(this->size);
  for(int i=0; i<this->conjugacyClasses.size; i++)
    for(int j=0; j<this->conjugacyClasses[i].size; j++)
      this->classmap[this->conjugacyClasses[i][j]] = i;
}

template<typename scalar, typename vector>
Matrix<Rational>& AnotherWeylGroup<scalar, vector>::GetClassMatrix(int cc)
{ if(this->classMatrices.size == 0)
  { if(this->conjugacyClasses.size == 0)
      this.ComputeConjugacyClasses();
    this->classMatrices.SetSize(this->conjugacyClasses.size);
  }
  if(this->classMap.size == 0)
    this->ComputeClassMap();
  this->classMatrices[cc] = GetClassMatrix(*this, cc, &(this->classmap));
  return this->classMatrices[cc];
}

template<typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::ComputeClassMatrices()
{ if(this->conjugacyClasses.size == 0)
    this->ComputeConjugacyClasses();
  for(int i=0; i<this->conjugacyClasses.size; i++)
  { std::cout << "//getting class matrix " << i << std::endl;
    this->GetClassMatrix(i);
  }
}


template <typename scalar, typename vector>
int AnotherWeylGroup<scalar, vector>::GetRank() const
{ return this->CartanSymmetric.NumRows;
}

template <typename scalar, typename vector>
int AnotherWeylGroup<scalar, vector>::GetRootReflection(int i) const
{ scalar x = this->RootSystem[i].ScalarProduct(this->twiceRho,this->unrationalCartanSymmetric);
  x *= -2;
  x /= this->RootSystem[i].ScalarProduct(this->RootSystem[i],this->unrationalCartanSymmetric);
  return this->rhoOrbit.GetIndexIMustContainTheObject(twiceRho+this->RootSystem[i]*x);
}

template <typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::GetSignCharacter(Vector<Rational>& out)
{ if(this->conjugacyClasses.size == 0)
    this->ComputeConjugacyClasses();
  out.SetSize(this->conjugacyClasses.size);
  for(int i=0; i<this->conjugacyClasses.size; i++)
  { List<int> srs;
    this->GetGeneratorList(this->conjugacyClasses[i][0], srs);
    int yn = srs.size % 2;
    if(yn == 0)
      out[i] = 1;
    else
      out[i] = -1;
  }
}

List<List<Vector<Rational> > > eigenspaces(const Matrix<Rational> &M, int checkDivisorsOf=0);

template <typename coefficient>
List<VectorSpace<coefficient> > GetEigenspaces(const Matrix<coefficient> &M)
{ List<List<Vector<coefficient> > > es = eigenspaces(M);
  List<VectorSpace<coefficient> > vs;
  for(int spi=0; spi<es.size; spi++)
  { VectorSpace<coefficient> V;
    for(int j=0; j<es[spi].size; j++)
      V.AddVector(es[spi][j]);
    vs.AddOnTop(V);
  }
  return vs;
}

// there are like 3 copies of this function with slightly different argument types
template <typename scalar, typename vector>
Rational AnotherWeylGroup<scalar, vector>::GetHermitianProduct(const Vector<Rational>& X1, const Vector<Rational>& X2) const
{ Rational acc = 0;
  for(int i=0; i<X1.size; i++)
    acc += X1[i].GetComplexConjugate() * X2[i] * this->conjugacyClasses[i].size;
  return acc / this->size;
}

template <typename somegroup>
void PrintCharTable(const somegroup& G, const char* filename)
{ JSData data;
  data.type = JSOBJ;
  data.obj.SetSize(3);
  data.obj[0].key = "representatives";
  data.obj[1].key = "sizes";
  data.obj[2].key = "characters";

  data.obj[0].value.type = JSLIST;
  data.obj[0].value.list.SetSize(G.conjugacyClasses.size);
  for(int i=0; i<G.conjugacyClasses.size; i++)
  { List<int> reprefs;
    G.GetGeneratorList(G.conjugacyClasses[i][0],reprefs);
    data.obj[0].value.list[i].type = JSLIST;
    data.obj[0].value.list[i].list.SetSize(reprefs.size);
    for(int j=0; j<reprefs.size; j++)
    { data.obj[0].value.list[i].list[j].type = JSNUM;
      data.obj[0].value.list[i].list[j].number = reprefs[j];
    }
  }

  data.obj[1].value.type = JSLIST;
  data.obj[1].value.list.SetSize(G.conjugacyClasses.size);
  for(int i=0; i<G.conjugacyClasses.size; i++)
  { data.obj[1].value.list[i].type = JSNUM;
    data.obj[1].value.list[i].number = G.conjugacyClasses[i].size;
  }

  data.obj[2].value.type = JSLIST;
  data.obj[2].value.list.SetSize(G.characterTable.size);
  for(int i=0; i<G.characterTable.size; i++)
  { for(int j=0; j<G.characterTable[i].size; j++)
    { data["characters"][i][j] = G.characterTable[i][j].DoubleValue();
    }
  }
  if(filename)
  { std::ofstream out;
    out.open(filename);
    data.IntoStream(out);
  }
  else
  { data.IntoStream(std::cout);
  }
}

template <typename somegroup>
void AddCharTable(JSData& chartable, somegroup& G)
{ std::string sizes = "sizes";

  // check sizes
  for(int i=0; i<G.conjugacyClasses.size; i++)
    if(chartable[sizes][i].number != G.conjugacyClasses[i].size)
      std::cout << "Size mismatch in conjugacy class " << i;
  // check representatives... well, not yet.
  // load characters

  G.characterTable.SetSize(chartable["characters"].list.size);
  for(int i=0; i<chartable["characters"].list.size; i++)
  { G.characterTable[i].SetSize(G.conjugacyClasses.size);
    for(int j=0; j<G.conjugacyClasses.size; j++)
      G.characterTable[i][j] = chartable["characters"][i][j].number;
  }
}

template <typename coefficient>
List<VectorSpace<coefficient> > GetLeftEigenspaces(Matrix<coefficient> M)
{ M.Transpose();
  return GetEigenspaces(M);
}

template <typename weylgroup>
List<int> FindConjugacyClassRepresentatives(weylgroup &G, int ncc)
{ if(G.theElements.size == 0)
    G.ComputeAllElements();
  std::cout << "Computing conjugacy class representatives... ";
  // hashing these might be nice
  // need to figure out how to pass MathRoutines::IntUnsignIdentity in
  List<List<int> > cycleTypes;
  List<int> representatives;
  for(int gi=0; gi<G.theElements.size; gi++)
  { PermutationR2 pi;
    WeylElementPermutesRootSystem(G.theElements[gi],pi);
    List<int> cycleType;
    pi.GetCycleStructure(cycleType);
    std::cout << pi;
    for(int i=1; i<cycleType.size; i++)
      std::cout << ' ' << cycleType[i];
    std::cout << std::endl;
    if(cycleTypes.GetIndex(cycleType) == -1)
    { cycleTypes.AddOnTop(cycleType);
      representatives.AddOnTop(gi);
      //std::cout << representatives.size << ' ';
      std::cout << "^ New cycle type, now have " << representatives.size << std::endl;
      if(representatives.size == ncc)
        break;
    }
  }
  return representatives;
}

// As in Schneider, 1990
// well, so far not using any of his improvements
template <typename somegroup>
List<Vector<Rational> > ComputeCharacterTable(somegroup &G)
{ if(G.conjugacyClasses.size == 0)
    G.ComputeConjugacyClasses();
  List<int> classmap;
  classmap.SetSize(G.size);
//  classmap.SetSize(G.theElements.size);
  for(int i=0; i<G.conjugacyClasses.size; i++)
    for(int j=0; j<G.conjugacyClasses[i].size; j++)
      classmap[G.conjugacyClasses[i][j]] = i;
  Matrix<Rational> form; // so inefficient
  form.MakeZeroMatrix(G.conjugacyClasses.size);
  for(int i=0; i<G.conjugacyClasses.size; i++)
    form.elements[i][i] = G.conjugacyClasses[i].size;
  List<VectorSpace<Rational> > spaces;
  if(G.characterTable.size > 0)
  { VectorSpace<Rational> allchars;
    for(int i=0; i<G.characterTable.size; i++)
    { VectorSpace<Rational> xspi;
      xspi.AddVector(G.characterTable[i]);
      spaces.AddOnTop(xspi);
      allchars.AddVector(G.characterTable[i]);
    }
    spaces.AddOnTop(allchars.OrthogonalComplement(0,&form));
  } else {
    Vector<Rational> X1;
    X1.SetSize(G.conjugacyClasses.size);
    for(int i=0; i<G.conjugacyClasses.size; i++)
      X1[i] = 1;
    VectorSpace<Rational> sp1;
    sp1.AddVector(X1);
    spaces.AddOnTop(sp1);
    spaces.AddOnTop(sp1.OrthogonalComplement(0,&form));
  }
  for(int i=0; i<G.conjugacyClasses.size; i++)
  { Matrix<Rational> M;
    std::cout << "Getting class matrix " << i << std::endl;
    M = GetClassMatrix(G,i,&classmap);
    //std::cout << M.ToString(&consoleFormat) << std::endl;
    List<VectorSpace<Rational> > es = GetEigenspaces(M);
    for(int esi=0; esi<es.size; esi++)
    { int spsize = spaces.size;
      for(int spi=0; spi<spsize; spi++)
      { if(spaces[spi].rank == 1)
          continue;
        VectorSpace<Rational> V = spaces[spi].Intersection(es[esi]);
        if((V.rank > 0) and (V.rank < spaces[spi].rank))
        { VectorSpace<Rational> W = es[esi].OrthogonalComplement(&spaces[spi],&form);
          spaces[spi] = V;
          spaces.AddOnTop(W);
          if(spaces.size == G.conjugacyClasses.size)
            goto got_chars;
        }
      }
    }
    int nchars = 0;
    for(int i=0; i<spaces.size; i++)
      if(spaces[i].rank == 1)
        nchars += 1;
    std::cout << "Have " << nchars << " chars" << std::endl;
  }
got_chars:
  List<Vector<Rational> > chars;
  chars.SetSize(spaces.size);
  for(int i=0; i<spaces.size; i++)
    chars[i] = spaces[i].GetCanonicalBasisVector(0);
  /*  HashedList<VectorSpace<Rational> > sps;
    List<Vector<Rational> > chars;
    for(int i=0; i<G.conjugacyClasses.size; i++)
    { Matrix<Rational> M;
      M = GetClassMatrix(G,i);
      std::cout << M.ToString(&consoleFormat) << std::endl;
      List<VectorSpace<Rational> > spsi = GetEigenspaces(M);
      for(int spi=0; spi<spsi.size; spi++)
        sps.AddOnTop(spsi[spi]);
    }
    for(int i=0; i<sps.size; i++)
      if(sps[i].rank == 1){
        Vector<Rational> X = sps[i].GetCanonicalBasisVector(0);
        if(!chars.Contains(X))
        { chars.AddOnTop(sps[i].GetCanonicalBasisVector(0));
          if(chars.size == G.conjugacyClasses.size)
            goto got_chars;
        }
      }
    std::cout << chars << std::endl;
    { // to scope newspaces so that the compiler doesn't complain about the goto
      // skipping its initialization
    List<VectorSpace<Rational> > newspaces = sps;
    while(newspaces.size > 0)
    { VectorSpace<Rational> sp = newspaces.PopLastObject();
      for(int i=0; i<sps.size; i++)
      { VectorSpace<Rational> sp2 = sp.Intersection(sps[i]);
        if(sps.GetIndex(sp2) == -1)
        { sps.AddOnTop(sp2);
          newspaces.AddOnTop(sp2);
          if(sp2.rank == 1)
          { Vector<Rational> X = sp2.GetCanonicalBasisVector(0);
            if(!chars.Contains(X))
            { chars.AddOnTop(X);
              std::cout << chars.size << ' ';
              if(chars.size == G.conjugacyClasses.size)
              { goto got_chars;
              }
            }
          }
        }
      }
    }
    }
    got_chars:
    */
//  std::cout << chars << std::endl;
  for(int i=0; i<chars.size; i++)
  { Rational x = G.GetHermitianProduct(chars[i],chars[i]);
    int x2 = x.GetDenominator().GetUnsignedIntValueTruncated();
    x2 = sqrt(x2);
    chars[i] *= x2;
    if(chars[i][0] < 0)
      chars[i] *= -1;
    std::cout << x2 << std::endl;
  }

  chars.QuickSortAscending(/*&CharacterComparator*/);
  for(int i=0; i<chars.size; i++)
    for(int j=i; j<chars.size; j++)
      if(chars[i] > chars[j])
        std::cout << "error: " << i << j << std::endl;
  for(int i=0; i<chars.size; i++)
    std::cout << chars[i] << std::endl;
  G.characterTable = chars;
  for(int i=0; i<G.characterTable.size; i++)
    std::cout << G.characterTable[i] << std::endl;
  return chars;
}

template <typename somegroup>
Matrix<Rational> GetClassMatrix(const somegroup &G, int cci, List<int>* classmap = 0)
{ List<int> invl;
  invl.SetSize(G.conjugacyClasses[cci].size);
  for(int i=0; i<G.conjugacyClasses[cci].size; i++)
    invl[i] = G.Invert(G.conjugacyClasses[cci][i]);
  Matrix<int> M;
  M.MakeZeroMatrix(G.conjugacyClasses.size);
  for(int t=0; t<G.conjugacyClasses.size; t++)
    for(int xi=0; xi<invl.size; xi++)
    { int yi = G.Multiply(invl[xi],G.conjugacyClasses[t][0]);
      int ci;
      if(classmap)
        M.elements[t][(*classmap)[yi]] += 1;
      else
        for(ci=0; ci<G.conjugacyClasses.size; ci++)
          if(G.conjugacyClasses[ci].BSContains(yi))
          { M.elements[t][ci] += 1;
            break;
          }
    }
  Matrix<Rational> out;
  out.init(M.NumRows, M.NumCols);
  for(int i=0; i<M.NumRows; i++)
    for(int j=0; j<M.NumCols; j++)
      out.elements[i][j] = M.elements[i][j];
  return out;
}

//template <typename somegroup>
//template <typename grouprep>
void ComputeIrreps(WeylGroup& G)
{ List<int> charactersWithIrreps;
  List<int> charactersWithoutIrreps;
  for(int i=0; i<G.irreps.size; i++)
  { charactersWithIrreps.AddOnTop(G.characterTable.BSGetIndex(G.irreps[i].GetCharacter()));
    std::cout << G.irreps[i].GetCharacter() << ": " << G.characterTable.BSExpectedIndex(G.irreps[i].GetCharacter()) << G.characterTable.BSGetIndex(G.irreps[i].GetCharacter()) << std::endl;
  }
  std::cout << "characters with irreps " << charactersWithIrreps << std::endl;
  for(int i=0; i<G.characterTable.size; i++)
    if(!charactersWithIrreps.BSContains(i))
      charactersWithoutIrreps.AddOnTop(i);
  std::cout << "need reps for chars" << charactersWithoutIrreps << std::endl;
  Matrix<Rational> M;
  M.init(G.characterTable.size, G.characterTable.size);
  for(int i=0; i<G.characterTable.size; i++)
    for(int j=0; j<G.characterTable.size; j++)
      M(j,i) = G.characterTable[j][i]*G.conjugacyClasses[i].size/G.size;
  for(int i=0; i<G.characterTable.size; i++)
    for(int j=i; j<G.characterTable.size; j++)
    { std::cout << i << "⊗" << j << ": " << M*G.GetCharacterProduct(G.characterTable[i], G.characterTable[j]) << std::endl;
    }
}

template <typename element>
class FiniteGroup
{
public:
  List<element> theElements;
  List<int> lengths; // sure why not
  bool MakeFrom(const List<element>& generators, int MaxElements=60000);

  List<int> generators;
  List<List<int> > conjugacyClasses;
  void ComputeConjugacyClasses();
  void GetSignCharacter(Vector<Rational>& out);
  Rational GetHermitianProduct(const Vector<Rational>& X1, const Vector<Rational>& X2) const;
};

template <typename element>
bool FiniteGroup<element>::MakeFrom(const List<element>& generators, int MaxElements)
{ List<int> newElements;
  for(int i=0; i<generators.size; i++)
  { this->theElements.AddOnTop(generators[i]);
    this->lengths.AddOnTop(1);
    int g = this->theElements.GetIndex(generators[i]);
    this->generators.AddOnTop(g);
    newElements.AddOnTop(g);
  }

  while(newElements.size > 0)
  { int gi = newElements.PopLastObject();
    element g = this->theElements[gi];
    for(int i=0; i<this->generators.size; i++)
    { element h = this->theElements[this->generators[i]] * g;
      if(this->theElements.GetIndex(h) == -1)
      { this->theElements.AddOnTop(h);
        this->lengths.AddOnTop(this->lengths[gi]+1);
        if(this->theElements.size > MaxElements)
          return false;
        int hi = this->theElements.GetIndex(h);
        newElements.AddOnTop(hi);
      }
    }
  }
  return true;
}

template <typename element>
void FiniteGroup<element>::ComputeConjugacyClasses()
{ List<bool> Accounted;
  Accounted.initFillInObject(this->theElements.size, false);
  this->conjugacyClasses.SetSize(0);
  this->conjugacyClasses.ReservE(50);
  HashedList<int, MathRoutines::IntUnsignIdentity> theStack;
  theStack.SetExpectedSize(this->theElements.size);
  List<int> inverseGenerators;
  inverseGenerators.SetSize(this->generators.size);
  for(int i=0; i<this->generators.size; i++)
    inverseGenerators[i] = this->theElements.GetIndex(this->theElements[this->generators[i]].Inverse());
  for(int i=0; i<this->theElements.size; i++)
    if (!Accounted[i])
    { theStack.Clear();
      theStack.AddOnTop(i);
      for (int j=0; j<theStack.size; j++)
        for (int k=0; k<this->generators.size; k++)
        { element g = this->theElements[inverseGenerators[k]] * this->theElements[theStack[j]] * this->theElements[this->generators[k]];
          int accountedIndex=this->theElements.GetIndex(g);
          theStack.AddOnTopNoRepetition(accountedIndex);
          Accounted[accountedIndex]=true;
        }
      this->conjugacyClasses.AddOnTop(theStack);
      this->conjugacyClasses.LastObject()->QuickSortAscending();
    }
  this->conjugacyClasses.QuickSortAscending();
}

template <typename element>
void FiniteGroup<element>::GetSignCharacter(Vector<Rational>& Xs)
{ if(this->conjugacyClasses.size == 0)
    this->ComputeConjugacyClasses();
  Xs.SetSize(this->conjugacyClasses.size);
  for(int i=0; i<Xs.size; i++)
  { int yn = this->lengths[conjugacyClasses[i][0]] % 2;
    if(yn == 0)
      Xs[i] = 1;
    else
      Xs[i] = -1;
  }
}

template <typename element>
Rational FiniteGroup<element>::GetHermitianProduct(const Vector<Rational>& X1, const Vector<Rational>& X2) const
{ Rational acc = 0;
  for(int i=0; i<X1.size; i++)
    acc += X1[i].GetComplexConjugate() * X2[i] * this->conjugacyClasses[i].size;
  return acc / this->theElements.size;
}

template <typename somegroup>
class Subgroup
{
public:
  somegroup *parent;
  HashedList<int, MathRoutines::IntUnsignIdentity> theElements;
  List<int> lengths; // sure why not
  // this used to have a maximum value.  but then after a while examining a crash
  // I realized that it's a subgroup of a finite group that was already in memory.
  bool MakeFrom(somegroup &G, const List<int>& generators, int MaxElements=-1);

  List<int> generators;
  List<List<int> > conjugacyClasses;
  void ComputeConjugacyClasses();
  void GetSignCharacter(Vector<Rational>& out);
  Rational GetHermitianProduct(const Vector<Rational>& X1, const Vector<Rational>& X2) const;
};


template <typename somegroup>
bool Subgroup<somegroup>::MakeFrom(somegroup &G, const List<int>& generators, int MaxElements)
{ // bookkeeping...
  // this class does things in such a bizarre way
  this->parent = &G;
  List<int> newElements;
  for(int i=0; i<generators.size; i++)
  { this->theElements.AddOnTop(generators[i]);
    this->lengths.AddOnTop(1);
    int g = this->theElements.GetIndex(generators[i]);
    this->generators.AddOnTop(g);
    newElements.AddOnTop(g);
  }

  // okay here is where the copy of dr milev's code is, for the 58th time
  while(newElements.size > 0)
  { int gi = newElements.PopLastObject();
    int g = this->theElements[gi];
    for(int i=0; i<this->generators.size; i++)
    { int h = this->parent->Multiply(this->theElements[this->generators[i]], g);
      // need a way of storing a sorted list...
      if(this->theElements.GetIndex(h) == -1)
      { this->theElements.AddOnTop(h);
        this->lengths.AddOnTop(this->lengths[gi]+1);
        if(MaxElements >=0 && this->theElements.size > MaxElements)
          return false;
        int hi = this->theElements.GetIndex(h);
        newElements.AddOnTop(hi);
      }
    }
  }
  return true;
}

template <typename somegroup>
void Subgroup<somegroup>::ComputeConjugacyClasses()
{ List<bool> Accounted;
  Accounted.initFillInObject(this->theElements.size, false);
  this->conjugacyClasses.SetSize(0);
  this->conjugacyClasses.ReservE(50);
  HashedList<int, MathRoutines::IntUnsignIdentity> theStack;
  theStack.SetExpectedSize(this->theElements.size);
  List<int> inverseGenerators;
  inverseGenerators.SetSize(this->generators.size);
  for(int i=0; i<this->generators.size; i++)
    inverseGenerators[i] = this->theElements.GetIndex(this->parent->Invert(this->theElements[this->generators[i]]));
  for(int i=0; i<this->theElements.size; i++)
    if (!Accounted[i])
    { theStack.Clear();
      theStack.AddOnTop(i);
      for (int j=0; j<theStack.size; j++)
        for (int k=0; k<this->generators.size; k++)
        { int g = this->parent->Multiply(this->parent->Multiply(this->theElements[inverseGenerators[k]], this->theElements[theStack[j]]), this->theElements[this->generators[k]]);
          int accountedIndex=this->theElements.GetIndex(g);
          theStack.AddOnTopNoRepetition(accountedIndex);
          Accounted[accountedIndex]=true;
        }
      this->conjugacyClasses.AddOnTop(theStack);
      this->conjugacyClasses.LastObject()->QuickSortAscending();
    }
  this->conjugacyClasses.QuickSortAscending();
}

template <typename somegroup>
void Subgroup<somegroup>::GetSignCharacter(Vector<Rational>& Xs)
{ if(this->conjugacyClasses.size == 0)
    this->ComputeConjugacyClasses();
  Xs.SetSize(this->conjugacyClasses.size);
  for(int i=0; i<Xs.size; i++)
  { int yn = this->lengths[conjugacyClasses[i][0]] % 2;
    if(yn == 0)
      Xs[i] = 1;
    else
      Xs[i] = -1;
  }
}

template <typename somegroup>
Rational Subgroup<somegroup>::GetHermitianProduct(const Vector<Rational>& X1, const Vector<Rational>& X2) const
{ Rational acc = 0;
  for(int i=0; i<X1.size; i++)
    acc += X1[i].GetComplexConjugate() * X2[i] * this->conjugacyClasses[i].size;
  return acc / this->theElements.size;
}

template <typename weylgroup>
void GetTauSignaturesFromSubgroup(weylgroup& G, const List<int>& gens, List<bool>& out)
{ /*List<ElementWeylGroup> genes;
  genes.SetSize(gens.size);
  for(int i=0; i<gens.size; i++)
    genes[i] = G.theElements[gens[i]];
  FiniteGroup<ElementWeylGroup> H;
  H.MakeFrom(genes);
  Vector<Rational> HXs;
  H.GetSignCharacter(HXs);*/

  Subgroup<weylgroup> H;
  H.MakeFrom(G, gens);
  Vector<Rational> HXs;
  H.GetSignCharacter(HXs);

  List<int> ccPreimages;
  ccPreimages.SetSize(H.conjugacyClasses.size);
  for(int i=0; i<H.conjugacyClasses.size; i++)
  { int gi = H.theElements[H.conjugacyClasses[i][0]];
    for(int ci=0; ci<G.conjugacyClasses.size; ci++)
      for(int cj=0; cj<G.conjugacyClasses[ci].size; cj++)
        if(G.conjugacyClasses[ci][cj] == gi)
        { ccPreimages[i] = ci;
          goto endloop;
        }
endloop:
    ;
  }

  out.SetSize(G.characterTable.size);
  Vector<Rational> HXi;
  HXi.SetSize(H.conjugacyClasses.size);
  for(int i=0; i<G.characterTable.size; i++)
  { Vector<Rational> GXi = G.characterTable[i];
    for(int j=0; j<HXi.size; j++)
      HXi[j] = GXi[ccPreimages[j]];
    if(H.GetHermitianProduct(HXs,HXi) == 0)
      out[i] = false;
    else
      out[i] = true;
  }
}



class WeylSubgroup: public WeylGroup
{
public:
  WeylGroup *parent;
  List<int> generatorPreimages;
  List<int> ccPreimages;
  List<bool> tauSignature;
  void ComputeTauSignature();
};

template <typename weylgroup>
void ParabolicSubgroup(weylgroup* G, const Selection sel, WeylSubgroup& out)
{ out.init();
  out.parent = G;
  int d = sel.CardinalitySelection;
  out.CartanSymmetric.init(d,d);
  for(int ii=0; ii<d; ii++)
    for(int jj=0; jj<d; jj++)
      out.CartanSymmetric.elements[ii][jj] = G->CartanSymmetric.elements[sel.elements[ii]][sel.elements[jj]];
  out.generatorPreimages.SetSize(d);
  for(int i=0; i<d; i++)
    out.generatorPreimages[i] = sel.elements[i] + 1;
}

// this is incorrect.  Do not use it.
void PseudoParabolicSubgroupNope(WeylGroup* G, const Selection& sel, WeylSubgroup& out)
{ out.init();
  out.parent = G;
  int d = sel.CardinalitySelection+1;
  out.generatorPreimages.SetSize(d);
  // not sure if this is actually how this works
  for(int i=0; i<d-1; i++)
    out.generatorPreimages[i] = i+1;

  out.CartanSymmetric.init(d,d);
  for(int ii=0; ii<d-1; ii++)
    for(int jj=0; jj<d-1; jj++)
      out.CartanSymmetric.elements[ii][jj] = G->CartanSymmetric.elements[sel.elements[ii]][sel.elements[jj]];

  // the highest root was the last one in the RootSystem
  Vector<Rational> hr = G->RootSystem[G->RootSystem.size-1];

  for(int i=0; i<d-1; i++)
  { Vector<Rational> v;
    G->CartanSymmetric.GetVectorFromRow(sel.elements[i],v);
    Rational x = v.ScalarEuclidean(hr);
    out.CartanSymmetric.elements[i][d-1] = x;
    out.CartanSymmetric.elements[d-1][i] = x;
  }
  out.CartanSymmetric.elements[d-1][d-1] = hr.ScalarProduct(hr,G->CartanSymmetric);
  out.generatorPreimages[d-1] = G->GetRootReflection(G->RootSystem.size-1);
}

void WeylSubgroup::ComputeTauSignature()
{ if(this->ccPreimages.size == 0)
  { this->ccPreimages.SetSize(this->conjugacyClasses.size);
    for(int i=0; i<this->conjugacyClasses.size; i++)
    { ElementWeylGroup g;
      g.owner = this->parent;
      for(int j=0; j<this->theElements[this->conjugacyClasses[i][0]].reflections.size; j++)
        g.reflections.AddListOnTop(this->parent->theElements[this->generatorPreimages[this->theElements[this->conjugacyClasses[i][0]].reflections[j]]].reflections);
      g.MakeCanonical();
      int gi = this->parent->theElements.GetIndex(g);
      for(int ci=0; ci<this->parent->conjugacyClasses.size; ci++)
        for(int cj=0; cj<this->parent->conjugacyClasses[ci].size; cj++)
          if(this->parent->conjugacyClasses[ci][cj] == gi)
          { this->ccPreimages[i] = ci;
            goto endloop;
          }
endloop:
      ;
    }
  }
  Vector<Rational> Xs;
  this->GetSignCharacter(Xs);
  Vector<Rational> Xi;
  Xi.SetSize(this->conjugacyClasses.size);
  this->tauSignature.SetSize(this->parent->irreps.size);
  for(int i=0; i<this->parent->irreps.size; i++)
  { Vector<Rational> Xip = this->parent->irreps[i].GetCharacter();
    for(int j=0; j<Xi.size; j++)
      Xi[j] = Xip[this->ccPreimages[j]];
    if(this->GetHermitianProduct(Xs,Xi) == 0)
      this->tauSignature[i] = false;
    else
      this->tauSignature[i] = true;
  }
}

template <typename weylgroup>
void AllTauSignatures(weylgroup* G, List<List<bool> >& tauSignatures, bool pseudo=false)
{ Selection sel;
  sel.init(G->CartanSymmetric.NumCols);
  int numCycles=MathRoutines::TwoToTheNth(sel.MaxSize);
  List<List<bool> > tss;
  tss.SetSize(numCycles-2);
  for(int i=0; i<numCycles-2; i++)
  { sel.incrementSelection();
    List<int> gens;
    for(int j=0; j<sel.CardinalitySelection; j++)
      gens.AddOnTop(sel.elements[j]+1);
    GetTauSignaturesFromSubgroup(*G,gens,tss[i]);
  }


  Vector<Rational> Xs;
  G->GetSignCharacter(Xs);
  List<bool> tsg;
  tsg.SetSize(G->characterTable.size);
  for(int i=0; i<G->characterTable.size; i++)
    tsg[i] =  G->characterTable[i] == Xs;
  tss.AddOnTop(tsg);

  if(pseudo)
  { std::cout << "pseudo-parabolics" << std::endl;
    int hr = G->GetRootReflection(G->RootSystem.size-1);
    sel.init(G->CartanSymmetric.NumCols);
    for(int i=0; i<numCycles-1; i++)
    { List<int> gens;
      for(int j=0; j<sel.CardinalitySelection; j++)
        gens.AddOnTop(sel.elements[j]+1);
      gens.AddOnTop(hr);
      List<bool> ts;
      GetTauSignaturesFromSubgroup(*G,gens,ts);
      tss.AddOnTop(ts);
      sel.incrementSelection();
    }
  }

  // we will need the sign character for the group


  tauSignatures.SetSize(G->characterTable.size);
  for(int i=0; i<G->characterTable.size; i++)
  { tauSignatures[i].SetSize(tss.size+1);
    tauSignatures[i][0] = 1;
    for(int j=1; j<tss.size+1; j++)
      tauSignatures[i][j] = tss[j-1][i];
  }

  for(int i=0; i<G->characterTable.size; i++)
  { std::cout << G->characterTable[i] << std::endl;
    for(int j=0; j<tauSignatures[i].size; j++)
      std::cout << tauSignatures[i][j] << ' ';
    std::cout << std::endl;
  }
}



template <typename weylgroup>
void PrettyPrintTauSignatures(weylgroup& G, JSData& data, bool pseudo = false)
{ if(G.characterTable.size == 0)
    ComputeCharacterTable(G);
  /* this doesn't belong here
     Selection sel;
     sel.MakeFullSelection(G.CartanSymmetric.NumCols);
     sel.RemoveSelection(0);
     WeylSubgroup H;
     ParabolicSubgroup(&G,sel,H);
     std::cout << G.CartanSymmetric.ToString(&consoleFormat) << std::endl;
     H.ComputeIrreducibleRepresentations();
     std::cout << "subgroup" << std::endl;
     std::cout << H.ToString(&consoleFormat) << std::endl;
     std::cout << "sigs" << std::endl;
     H.ComputeTauSignature();
     std::cout << H.tauSignature << std::endl;
  */
  List<List<bool> > ts;
  AllTauSignatures(&G,ts,true);


  for(int i=0; i<ts.size; i++)
    for(int j=0; j<ts[i].size; j++)
      data[i][j] = ts[i][j];
  /* this prints out everything for some reason
  for(int i=0; i<G.conjugacyClasses.size; i++)
  {std::cout << "conjugacy class " << i << std::endl;
   for(int j=0; j<G.conjugacyClasses[i].size; j++)
   {ElementWeylGroup g = G.theElements[G.conjugacyClasses[i][j]];
     for(int k=0; k<g.size; k++)
      std::cout << g[k] << ' ';
     std::cout << std::endl;
   }
   std::cout << std::endl;
  }*/

// lets print out the character table
  std::cout << "\\[ \\begin{array}{";
  for(int i=0; i<G.conjugacyClasses.size+1; i++)
    std::cout << 'c';
  std::cout << "}\n";

  for(int i=0; i<G.conjugacyClasses.size; i++)
  { List<int> g;
    G.GetGeneratorList(G.conjugacyClasses[i][0],g);
    std::cout << "&\\rotatebox{90}{";
    for(int k=0; k<g.size; k++)
      std::cout << g[k] << ' ';
    std::cout << "}\t";
  }
  std::cout << "\\\\" << std::endl;

  for(int i=0; i<G.conjugacyClasses.size; i++)
    std::cout << '&' << G.conjugacyClasses[i].size << '\t';
  std::cout << "\\\\\n\\hline\n";

  for(int i=0; i<G.characterTable.size; i++)
  { for(int j=0; j<G.conjugacyClasses.size; j++)
      std::cout << '&' << G.characterTable[i][j] << '\t';
    std::cout << "\\\\\n";
  }
  std::cout << "\\end{array} \\]" << std::endl;

// okay now the regular tau signatures
  Selection sel;
  sel.init(G.CartanSymmetric.NumCols);
  int numCycles=MathRoutines::TwoToTheNth(sel.MaxSize);
  std::cout << "\\[ \\begin{array}{";
  for(int i=0; i<numCycles+1; i++)
    std::cout << 'c';
  std::cout << "}\n";
  for(int i=0; i<numCycles; i++)
  { std::cout << "&\\rotatebox{90}{(";
    for(int j=0; j<sel.CardinalitySelection; j++)
    { std::cout << sel.elements[j];
      if(j+1<sel.elements.size)
        std::cout << ' ';
    }
    std::cout << ")}\t";
    sel.incrementSelection();
  }

  std::cout << "\\\\" << std::endl;
  for(int i=0; i<ts.size; i++)
  { for(int j=0; j<numCycles; j++)
    { std::cout << '&';
      std::cout << ts[i][j] << '\t';
    }
    std::cout << "\\\\\n";
  }
  std::cout << "\\end{array} \\]" << std::endl;

// now the pseudo tau signatures
  if(pseudo)
  {

    if(ts[0].size > numCycles)
    { std::cout << "hr is ";
// WeylGroup is alittle different from AnotherWeylGroup<derp>
//    ElementWeylGroup hr = G.theElements[G.GetRootReflection(G.RootSystem.size-1)];
      List<int> hr;
      G.GetGeneratorList(G.GetRootReflection(G.RootSystem.size-1), hr);
      for(int i=0; i<hr.size; i++)
        std::cout << hr[i] << ' ';
      std::cout << std::endl;

      // make enough columns
      std::cout << "\\[ \\begin{array}{";
      for(int i=0; i<numCycles; i++)
        std::cout << 'c';
      std::cout << "}\n";

      // labels at the top
      sel.init(G.CartanSymmetric.NumCols);
      for(int i=0; i<numCycles-1; i++)
      { std::cout << "&\\rotatebox{90}{(";
        for(int j=0; j<sel.CardinalitySelection; j++)
          std::cout << sel.elements[j] << ' ';
        std::cout << "hr";
        std::cout << ")}\t";
        sel.incrementSelection();
      }

      // data goes out here
      std::cout << "\\\\" << std::endl;
      for(int i=0; i<ts.size; i++)
      { for(int j=numCycles; j<numCycles+numCycles-1; j++)
        { std::cout << '&';
          std::cout << ts[i][j] << '\t';
        }
        std::cout << "\\\\\n";
      }
      std::cout << "\\end{array} \\]" << std::endl;
    }
  }
}

/*
  for(int j=0; j<PSGs[i].ccCount; j++)
  { CoxeterElement h = PSGs[i].GetCoxeterElement(PSGs[i].conjugacyClasses[j][0]);
    g.reflections.SetSize(h.reflections.size);
    for(int k=0; k<h.reflections.size; k++)
      g.reflections[k] = sel.elements[h.reflections[k]];
    int gi = this->GetIndexOfElement(g);
    for(int k=0; k<this->ccCount; k++)
      for(int l=0; l<this->conjugacyClasses[k].size; l++)
        if(this->conjugacyClasses[k][l] == gi)
        { ccBackMaps[i][j] = k;
          goto endloop;
        }
    endloop:;
  }
*/


/* dunno why i thought it was fun to spend so much time on this
class f127
{ public:
  int n;

  f127 operator+(const f127 right) const;
  void operator+=(const f127 right);
  void operator-();
  f127 operator-(const f127 right) const;
  void operator-=(const f127 right);
  f127 operator*(const f127 right) const;
  void operator*=(const f127 right);
  void Invert();
  f127 operator/(const f127 right) const;
  void operator/=(const f127 right);
  void operator=(const int rhs);
}

f127 f127::operator+(const f127 right) const
{ return (n+right.n)%127;
}

void f127::operator+=(const f127 right)
{ n += right.n;
  n %= 127;
}

f127 f127::operator-(const f127 right) const
{ int x = (n-right.n)%127;
  f127 out;
  out.n = x>=0?x:-x;
  return out;
}

void f127::operator-=(const f127 right)
{ n -= right.n;
  n %= 127;
  if(n<0)
    n = -n;
}

f127 f127::operator*(const f127 right) const
{ return (n*right.n)%127;
}

void f127::operator*=(const f127 right)
{ n *= right.n;
  n %= 127;
}

void f127::Invert()
{ int x = 0;
  int y = 1;
  int a = 127;
  int b = n;
  int m = 1
  n = 0;
  while(b != 0)
  { int q = a / b;
    int r = a % b;
    a = b;
    b = r;
    int s = m - q*x;
    int t = n - q*y;
    m = x;
    n = y;
    x = s;
    y = t;
  }
}

f127 operator/(const f127 right) const;
{ tmp = right;
  tmp.Invert();
  return *this *tmp;
}

void operator/=(const f127 right)
{ tmp = right;
  tmp.Invert();
  *this *= tmp;
}
*/





/*
template <typename coefficient>
List<CoxeterRepresentation<coefficient> > CoxeterRepresentation<coefficient>::Decomposition(List<ClassFunction<coefficient> >& ct, List<CoxeterRepresentation<coefficient> > &gr)
{  List<CoxeterRepresentation<coefficient> > out;
   if(GetNumberOfComponents() == 1)
   { if(ct.GetIndex(character) == -1)
     { std::cout << "new irrep found, have " << ct.size << std::endl;
       ct.AddOnTop(character);
       gr.AddOnTop(*this);
     }
     out.AddOnTop(*this);
     return out;
   }
   List<Vector<coefficient> > Vb = basis;
   for(int i=0; i<ct.size; i++)
   { if(character.IP(ct[i])!=0)
     { std::cout << "contains irrep " << i << std::endl;
       Matrix<coefficient> M = ClassFunctionMatrix(ct[i]);
       Vb = intersection(Vb,DestructiveKernel(M));
     }
   }
   if((Vb.size < basis.size) && (Vb.size > 0))
   { CoxeterRepresentation<coefficient> V;
     V.G = G;
     V.gens = gens;
     V.classFunctionMatrices = classFunctionMatrices;
     V.basis = Vb;
     V = V.Reduced();
     std::cout << "Decomposing remaining subrep " << V.GetCharacter() << std::endl;
     return V.Decomposition(ct,gr);
   }
   if(Vb.size == 0)
    return out;
   SpaceTree<Rational> st;
   st.space = basis;
   for(int cfi=0; cfi<G->ccCount; cfi++)
   {  ClassFunction<coefficient> cf;
      cf.G = G;
      cf.MakeZero();
      cf[cfi] = 1;
      std::cout << "getting matrix " << cf << std::endl;
      Matrix<coefficient> A = ClassFunctionMatrix(cf);
      List<List<Vector<coefficient> > > es = eigenspaces(A);
      std::cout << "eigenspaces were ";
      for(int i=0; i<es.size; i++)
         std::cout << es[i].size << " ";
      std::cout << std::endl;
      for(int i=0; i<es.size; i++)
         st.PlaceInTree(es[i]);
      std::cout << "tree is ";
      st.DisplayTree();
      std::cout << std::endl;
   }
   List<List<Vector<coefficient> > > leaves;
   st.GetLeaves(leaves);
   std::cout << "leaves are ";
   for(int i=0; i<leaves.size; i++)
      std::cout << leaves[i].size << " ";
   std::cout << std::endl;

   for(int i=0; i<leaves.size; i++)
   {  CoxeterRepresentation<coefficient> outeme;
      outeme.G = G;
      outeme.gens = gens;
      outeme.classFunctionMatrices = classFunctionMatrices;
      outeme.basis = leaves[i];
      out.AddOnTop(outeme.Reduced());
   }
   return out;
}
*/

/*template <typename coefficient>
List<CoxeterRepresentation<coefficient> > CoxeterRepresentation<coefficient>::Decomposition() const
{ List<CoxeterRepresentation<coefficient> > out;
  if(GetNumberOfComponents() == 1)
  { out.AddOnTop(*this);
    return out;
  }
  List<List<Vector<coefficient> > > spaces;
  spaces.AddOnTop(basis);
  List<bool> is_component;
  is_component.AddOnTop(false);
  int dim_components = 0;
  for(int cfi=0; cfi<G->ccCount; cfi++)
  { if(dim_components == basis.size)
      break;
    ClassFunction<coefficient> cf;
    cf.G = G;
    cf.MakeZero();
    cf[cfi] = 1;
    std::cout << "getting matrix" << cf << std::endl;
    Matrix<coefficient> A = ClassFunctionMatrix(cf);
    std::cout << A.ToString(&consoleFormat) << std::endl;
    std::cout << "getting eigenspaces" << std::endl;
    List<List<Vector<coefficient> > > es = eigenspaces(A);
    for(int i=0; i<es.size; i++)
    { if(dim_components == basis.size)
        break;
      int spaces_to_check = spaces.size;
      for(int j=0; j<spaces_to_check; j++)
      { if(is_component[j])
          continue;
        List<Vector<coefficient> > middle = intersection(es[i],spaces[j]);
        if(!((0<middle.size)&&(middle.size<spaces[j].size)))
          continue;
        std::cout << "eigenspace " << i << " intersects with space " << j << std::endl;
        List<Vector<coefficient> > right = relative_complement(spaces[j],middle);
        spaces[j] = middle;
        spaces.AddOnTop(right);
        CoxeterRepresentation<coefficient> midr;
        midr.G = G;
        midr.gens = gens;
        midr.basis = middle;
        if(midr.GetNumberOfComponents() == 1)
        { is_component[j] = true;
          dim_components += middle.size;
        }
        CoxeterRepresentation<coefficient> rightr;
        rightr.G = G;
        rightr.gens = gens;
        rightr.basis = right;
        if(rightr.GetNumberOfComponents() == 1)
        { is_component.AddOnTop(true);
          dim_components += right.size;
        } else
        { is_component.AddOnTop(false);
        }
      }
    }
  }
  std::cout << "spaces.size " << spaces.size << std::endl;
  for(int i=0; i<spaces.size; i++)
  { CoxeterRepresentation<coefficient> s;
    s.G = this->G;
    s.gens = this->gens;
    s.basis = spaces[i];
    out.AddOnTop(s);
  }
  std::cout << "decomposition might be complete, found " << out.size << " components" << std::endl;
  return out;
}*/


/*
template <typename coefficient>
class polynom1al
{
  public:
  List<coefficient> data;


  void AddMonomial(coefficient c, int i)
  { while(this->data.size < i+1)
    { this->data.AddOnTop(0);
    }
    this->data[i] += c;
  }


  // if only cxx had lisp macros...
  polynom1al<coefficient> operator+(const polynom1al &right) const
  { polynom1al<coefficient> out;
    out.data.SetSize((right.data.size>this->data.size)?right.data.size:this->data.size);
    int i=0;
    for(; i<this->data.size && i<right.data.size; i++)
      out.data[i] = this->data[i] + right.data[i];
    for(; i<this->data.size; i++)
      out.data[i] = this->data[i];
    for(; i<right.data.size; i++)
      out.data[i] = right->data[i];
    return out;
  }

  polynom1al<coefficient> operator-(const polynom1al &right) const
  { polynom1al<coefficient> out;
    out.data.SetSize((right.data.size>this->data.size)?right.data.size:this->data.size);
    int i=0;
    for(; i<this->data.size && i<right.data.size; i++)
      out.data[i] = this->data[i] - right.data[i];
    for(; i<this->data.size; i++)
      out.data[i] = this->data[i];
    for(; i<right.data.size; i++)
      out.data[i] = -right->data[i];
    return out;
  }

  polynom1al<coefficient> operator*(const polynom1al &right) const
  { polynom1al<coefficient> out;
    out.data.SetSize(tmp.data.size*right.data.size);
    for(int i=0; i<this->data.size; i++)
      for(int j=0; j<right.data.size; j++)
        out.data[i*j] = this->data[i]*right.data[j];
    return out;
  }
  void operator*=(const polynom1al &right)
  { polynom1al<coefficient> tmp = *this;
    this->data.SetSize(tmp.data.size*right.data.size)
    for(int i=0; i<tmp.data.size; i++)
      for(int j=0; j<right.data.size; j++)
        this->data[i*j] = tmp.data[i]*right.data[j];
  }

  void Invert()
  { for(int i=0; i<this->data.size; i++)
      this->data[i].Invert();
  }

  bool IsEqualToZero()
  { return this->data.size == 0;
  }


  static List<polynom1al<coefficient> > LagrangeInterpolants(List<int> points)
  {

  }

  polynom1al(){}
  polynom1al(int c){this->data.AddOnTop(c);}


  void operator=(int right)
  { this->data.SetSize(1);
    this->data[0] = right;

  }
  void operator=(coefficient right)
  { this->data.SetSize(1);
    this->data[0] = right;
  }

};

template <typename coefficient>
std::ostream& operator<<(std::ostream& out, const polynom1al<coefficient>& p)
{ out << p.data;
  return out;
}
*/
template <typename coefficient>
bool space_contains(const List<Vector<coefficient> > &V, Vector<coefficient> v)
{ if(v.IsEqualToZero())
  { if(V.size == 0)
      return false;
    return true;
  }
  int i=0;
  while(true)
  { int vi = v.GetIndexFirstNonZeroCoordinate();
    while(true)
    { if(i==V.size)
        return false;
      int vii = V[i].GetIndexFirstNonZeroCoordinate();
      if(vii > vi)
        return false;
      if(vii == vi)
        break;
      i++;
    }
    v -= V[i] * v[vi]/V[i][vi];
    if(v.IsEqualToZero())
      return true;
  }
}

template<typename coefficient>
List<Vector<coefficient> > getunion(const List<Vector<coefficient> > &V, const List<Vector<coefficient> > &W)
{ if(V.size == 0)
    return W;
  int d = V[0].size;
  Matrix<coefficient> M;
  M.init(V.size+W.size,d);
  for(int i=0; i<V.size; i++)
    for(int j=0; j<d; j++)
      M.elements[i][j] = V[i][j];
  for(int i=0; i<W.size; i++)
    for(int j=0; j<d; j++)
      M.elements[V.size+i][j] = W[i][j];
  M.GaussianEliminationByRows();
  List<Vector<coefficient> > out;
  for(int i=0; i<M.NumRows; i++)
  { Vector<coefficient> v;
    v.SetSize(d);
    for(int j=0; j<d; j++)
      v[j] = M.elements[i][j];
    if(v.IsEqualToZero())
      break;
    out.AddOnTop(v);
  }
  return out;
}

template<typename coefficient>
List<Vector<coefficient> > intersection(const List<Vector<coefficient> > &V, const List<Vector<coefficient> > &W)
{ if((V.size==0) or (W.size==0))
  { List<Vector<coefficient> > out;
    return out;
  }
  int d = V[0].size;

  Matrix<coefficient> MV;
  MV.init(V.size, d);
  for(int i=0; i<V.size; i++)
    for(int j=0; j<d; j++)
      MV.elements[i][j] = V[i][j];
  List<Vector<coefficient> > Vperp = DestructiveKernel(MV);

  Matrix<coefficient> MW;
  MW.init(W.size, d);
  for(int i=0; i<W.size; i++)
    for(int j=0; j<d; j++)
      MW.elements[i][j] = W[i][j];
  List<Vector<coefficient> > Wperp = DestructiveKernel(MW);

  Matrix<coefficient> M;
  M.init(Vperp.size+Wperp.size,d);
  int i=0;
  for(; i<Vperp.size; i++)
    for(int j=0; j<d; j++)
      M.elements[i][j] = Vperp[i][j];
  for(; i<Vperp.size+Wperp.size; i++)
    for(int j=0; j<d; j++)
      M.elements[i][j] = Wperp[i-Vperp.size][j];
  return DestructiveKernel(M);
}

template <typename coefficient>
List<Vector<coefficient> > orthogonal_complement(const List<Vector<coefficient> > &V, const List<Vector<coefficient> > &WW)
{ List<Vector<coefficient> > W = intersection(V,WW);
  if(W.size==0)
    return V;
  int d = W[0].size;
  Matrix<coefficient> GM;
  GM.init(W.size, W.size);
  for(int i=0; i<W.size; i++)
    for(int j=0; j<W.size; j++)
      GM.elements[i][j] = W[i].ScalarEuclidean(W[j]);
  GM.Invert();
  Matrix<coefficient> VM;
  VM.init(W.size,V.size);
  for(int i=0; i<W.size; i++)
    for(int j=0; j<V.size; j++)
      VM.elements[i][j] = W[i].ScalarEuclidean(V[j]);
  VM.MultiplyOnTheLeft(GM);
  Matrix<coefficient> outm;
  outm.init(V.size, d);
  for(int i=0; i<V.size; i++)
  { for(int j=0; j<d; j++)
    { coefficient r = V[i][j];
      outm.elements[i][j] = r;
      for(int k=0; k<W.size; k++)
        outm.elements[i][j] -= VM.elements[k][i] * W[k][j];
    }
  }
  outm.GaussianEliminationByRows();
  List<Vector<coefficient> > out;
  for(int i=0; i<outm.NumRows; i++)
  { Vector<coefficient> v;
    v.SetSize(d);
    for(int j=0; j<d; j++)
      v[j] = outm.elements[i][j];
    if(v.IsEqualToZero())
      return out;
    out.AddOnTop(v);
  }
  return out;
}

// we're going to guess at integers
template <typename coefficient>
bool pdiv(List<coefficient> &p, int a)
{ List<coefficient> q;
  coefficient lastround = p[0];
  for(int i=1; i<p.size; i++)
  { q.AddOnTop(lastround);
    lastround = p[i] - lastround*a;
  }
  if(lastround == 0)
  { p = q;
    return true;
  }
  return false;
}

template <typename coefficient>
List<int> factorpoly(List<coefficient> p, int maxfac)
{ List<int> factors;
  for(int i1=0; i1<maxfac; i1++)
  { for(int i2=1; i2>=-1; i2-=2)
    { int i = i1*i2;
      while(pdiv(p,i))
      { if(!factors.Contains(i))
        { factors.AddOnTop(i);
        }
        if(p.size == 1)
          return factors;
      }
    }
  }
  return factors;
}

template <typename coefficient>
List<Vector<coefficient> > DestructiveKernel(Matrix<coefficient> &M)
{ Matrix<coefficient> MM; // idk what this does
  Selection s;
//  std::cout << "DestructiveKernel: GaussianEliminationByRows" << std::endl;
  M.GaussianEliminationByRows(0,0,s);
//  std::cout << "DestructiveKernel: calculating kernel space" << std::endl;
  List<Vector<coefficient> > V;
  for(int i=0; i<s.selected.size; i++)
  { if(!s.selected[i])
      continue;
    Vector<coefficient> v;
    v.MakeEi(M.NumCols,i);
    int rowmap = 0;
    for(int j=0; j<M.NumCols; j++)
      if(!s.selected[j])
      { v[j] = -M.elements[rowmap][i];
        rowmap++;
      }
    V.AddOnTop(v);
  }
//  std::cout << "DestructiveKernel: done" << std::endl;
  return V;
}

template <typename coefficient>
List<Vector<coefficient> > DestructiveEigenspace(Matrix<coefficient> &M, const coefficient &l)
{ for(int i=0; i<M.NumCols; i++)
    M.elements[i][i] -= l;
  return DestructiveKernel(M);
}

template <typename coefficient>
List<Vector<coefficient> > DestructiveColumnSpace(Matrix<coefficient>& M)
{ M.Transpose();
  M.GaussianEliminationByRows();
  List<Vector<coefficient> > out;
  bool zerov;
  for(int i=0; i<M.NumRows; i++)
  { Vector<coefficient> v;
    v.MakeZero(M.NumCols); // initializing is not necessary
    zerov = true;
    for(int j=0; j<M.NumCols; j++)
    { v[j] = M.elements[i][j];
      if(zerov && M.elements[i][j]!=0)
        zerov = false;
    }
    if(zerov)
      return out;
    out.AddOnTop(v);
  }
}


template <typename coefficient>
Vector<coefficient> PutInBasis(const Vector<coefficient> &v, const List<Vector<coefficient> > &B)
{ Vector<coefficient> w;
  w.MakeZero(B.size);
  Matrix<coefficient> M;
  M.MakeZeroMatrix(B.size);
  for(int i=0; i<B.size; i++)
  { for(int j=0; j<v.size; j++)
      w[i] += B[i][j]*v[j];
    for(int j=0; j<B.size; j++)
      for(int k=0; k<v.size; k++)
        M.elements[i][j] += B[i][k] * B[j][k];
  }
  M.Invert();
  Vector<coefficient> v2 = M*w;
  Vector<coefficient> v3;
  v3.MakeZero(v.size);
  for(int i=0; i<B.size; i++)
    v3 += B[i] * v2[i];
  if(!(v3 == v))
  { Vector<coefficient> out;
    return out;
  }
  return v2;
}

template <typename coefficient>
Vector<coefficient> ActOnGroupRing(const CoxeterGroup& G, int g, const Vector<coefficient> &v)
{ Vector<coefficient> out;
  out.MakeZero(G.N);
  for(int i=0; i<G.N; i++)
    if(v[i] != 0)
      out[G.MultiplyElements(g,i)] = v[i];
  return out;
}

// this function name is a lie
template <typename coefficient>
bool is_isotypic_component(CoxeterGroup &G, const List<Vector<coefficient> > &V)
{ // pre-initial test: V isn't empty
  if(V.size == 0)
    return false;
  // initial test: dimension of V is a square
  int n = sqrt(V.size);
  if(n*n != V.size)
    return false;
  // more expensive test: character of V has unit norm
  ClassFunction<coefficient> X;
  X.G = &G;
  X.data.SetSize(G.ccCount);
  for(int i=0; i<G.ccCount; i++)
  { int g = G.conjugacyClasses[i][0];
    coefficient tr = 0;
    for(int j=0; j<V.size; j++)
    { Vector<coefficient> v = ActOnGroupRing(G,g,V[j]);
      v = PutInBasis(v,V);
      tr += v[j];
    }
    X.data[i] = tr;
  }
  // (4, -1, -1/2, 1/2, -1, 1/4, 1/4, 1, 1/2, -7/2)[1] haha yeah right
  if(X.norm() != n)
    return false;
  // okay now do the really hard test
  Matrix<coefficient> M = GetMatrix(X);
  List<List<Vector<coefficient> > > spaces = eigenspaces(M);
  if(spaces.size != 2)
    return false;
  if(spaces[1].size != V.size)
    return false;
  if(intersection(spaces[1],V).size != V.size)
    return false;
  return true;
}



template <typename coefficient>
Matrix<coefficient> GetMatrix(const ClassFunction<coefficient>& X)
{ Matrix<coefficient> M;
  M.MakeZeroMatrix(X.G->N);
  for(int i1=0; i1<X.G->ccCount; i1++)
  { for(int i2=0; i2<X.G->ccSizes[i1]; i2++)
    { int i=X.G->conjugacyClasses[i1][i2];
      for(int j=0; j<X.G->N; j++)
      { M.elements[X.G->MultiplyElements(i,j)][j] = X.data[i1];
      }
    }
  }
  return M;
}

int skipcount(int n, const List<int>& l)
{ int o = 0;
  for(int i=0; i<l.size; i++)
  { if(l[i] == n)
      return -1;
    if(l[i]<n)
    { o++;
    }
  }
  return n-o;
}

int popcnt(int i) // from the internet
{ i = i - ((i >> 1) & 0x55555555);
  i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
  return ((((i + (i >> 4)) & 0xF0F0F0F) * 0x1010101) & 0xffffffff) >> 24;
}

List<List<int> > powersetgrade(int G, const List<int>& l)
{ List<List<int> > out;
  for(int i=0; i<1<<l.size; i++)
  { if(popcnt(i)==G)
    { List<int> outi;
      for(int j=0; j<l.size; j++)
      { if(i&(1<<j))
          outi.AddOnTop(j);
      }
      out.AddOnTop(outi);
    }
  }
  return out;
}

template<typename Element>
Element minor_det(const Matrix<Element>& M, const List<int>& l)
{ Matrix<Element> MM;
  MM.init(M.NumRows-l.size,M.NumCols-l.size);
  for(int i=0; i<M.NumRows; i++)
  { int ii = skipcount(i,l);
    if(ii == -1)
      continue;
    for(int j=0; j<M.NumCols; j++)
    { int jj = skipcount(j,l);
      if(jj == -1)
        continue;
      MM.elements[ii][jj] = M.elements[i][j];
    }
  }
//  Element x;
//  MM.ComputeDeterminantOverwriteMatrix(&x);
//  return x;
  return MM.GetDeterminant();
}

template<typename Element>
List<Element> charpoly(const Matrix<Element>& M)
{ List<Element> p;
  p.SetSize(M.NumRows+1);
  List<int> rowids;
  for(int i=0; i<M.NumRows; i++)
    rowids.AddOnTop(i);
  for(int i=0; i<M.NumRows; i++)
  { Element acc = 0;
    List<List<int> > ps = powersetgrade(i,rowids);
    for(int si=0; si < ps.size; si++)
      acc += minor_det(M,ps[si]);
    if(i%2==0)
      acc = -acc;
    p[M.NumRows-i] = acc;
  }
  p[0] = ((M.NumRows%2)==0)? -1 : 1;

  return p;
}

void BSTest()
{ List<int> l;
  l.BSInsertDontDup(1);
  l.BSInsertDontDup(1);
  l.BSInsertDontDup(5);
  l.BSInsertDontDup(5);
  l.BSInsertDontDup(1);
  l.BSInsertDontDup(3);
  l.BSInsertDontDup(5);
  l.InsertAtIndexShiftElementsUp(1,1);
  l.AddOnTop(5);
  std::cout << "list contents " << l << std::endl;
  for(int i=0; i<7; i++)
    std::cout << i << ": " << l.BSExpectedIndex(i) << std::endl;
}

int chartable[10][10] =
{ {1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
  {1, -1, -1,  1,  1, -1,  1,  1, -1, -1},
  {3,  1,  1, -1,  1,  0,  0, -1, -1, -3},
  {3, -1, -1, -1,  1,  0,  0, -1,  1,  3},
  {2,  0, -2,  0,  0,  1, -1,  2,  0, -2},
  {2,  0,  2,  0,  0, -1, -1,  2,  0,  2},
  {1,  1, -1, -1, -1, -1,  1,  1,  1, -1},
  {1, -1,  1, -1, -1,  1,  1,  1, -1,  1},
  {3, -1,  1,  1, -1,  0,  0, -1,  1, -3},
  {3,  1, -1,  1, -1,  0,  0, -1, -1,  3}
};

template <typename coefficient>
bool GetCoordsInBasisInputIsGaussianEliminated
(const List<Polynomial<coefficient> > &inputReducedBasis,
 const Polynomial<coefficient> & input,
 Vector<coefficient>* outputCoordinatesInBasis=0)
{ MacroRegisterFunctionWithName("GetCoordsInBasiSPolynomial");
  Polynomial<coefficient> summand;
  Polynomial<coefficient> v = input;
  coefficient currentCoeff;
  if (outputCoordinatesInBasis!=0)
    outputCoordinatesInBasis->MakeZero(inputReducedBasis.size);
  for(int i=0; i<inputReducedBasis.size; i++)
  { currentCoeff=v.GetMonomialCoefficient(inputReducedBasis[i].GetMaxMonomial());
    if (!currentCoeff.IsEqualToZero())
    { summand=inputReducedBasis[i];
      if (outputCoordinatesInBasis!=0)
        (*outputCoordinatesInBasis)[i]=currentCoeff;
      currentCoeff*=-1;
      summand*=currentCoeff;
      v-=summand;
    }
  }
  return v.IsEqualToZero();
}

void make_macdonald_polynomial(const WeylGroup& G, const List<Vector<Rational> > roots, Polynomial<Rational>& macdonaldPoly)
{ List<MonomialP> vars;
  vars.SetSize(G.GetDim());
  for(int i=0; i<G.GetDim(); i++)
    vars[i].MakeEi(i);

  macdonaldPoly.MakeOne();
  for(int i=0; i<roots.size; i++)
  { Polynomial<Rational> pj;
    for(int j=0; j<G.GetDim(); j++)
      pj.AddMonomial(vars[j], roots[i][j]);
    macdonaldPoly *= pj;
  }
}

void matrix_acts_on_polynomial(const Matrix<Rational>& m,const Polynomial<Rational>& p, Polynomial<Rational>& q)
{ Rational one = 1;
  q = q.GetZero();
  List<MonomialP> vars;
  vars.SetSize(m.NumCols);
  for(int i=0; i<m.NumCols; i++)
    vars[i].MakeEi(i);
  for(int i=0; i<p.theMonomials.size; i++)
  { Polynomial<Rational> qi;
    qi = qi.GetOne();
    for(int j=0; j<p.theMonomials[i].GetMinNumVars(); j++)
      if(p.theMonomials[i](j) != 0)
      { Polynomial<Rational> qj;
        qj = qj.GetZero();
        for(int k=0; k<m.NumCols; k++)
          qj.AddMonomial(vars[k],m.elements[k][j]);
        qj.RaiseToPower((p.theMonomials[i](j)).floorIfSmall(),one);
        qi *= qj;
      }
    qi *= p.theCoeffs[i];
    q += qi;
  }
}

void get_macdonald_representations_of_weyl_group(SemisimpleLieAlgebra& theSSlieAlg)
{ WeylGroup& W = theSSlieAlg.theWeyl;


  rootSubalgebras theRootSAs;
  theRootSAs.owneR=&theSSlieAlg;
  DynkinSimpleType dt = W.theDynkinType.GetGreatestSimpleType();
  theRootSAs.GenerateAllReductiveRootSubalgebrasUpToIsomorphism(theGlobalVariables, true, false);
  List<Vector<Rational> > roots;

  for (int k=0; k<theRootSAs.size; k++)
  { rootSubalgebra& currentRootSA=theRootSAs[k];
    roots=currentRootSA.PositiveRootsK;
    Polynomial<Rational> macdonaldPoly;
    make_macdonald_polynomial(W,roots,macdonaldPoly);
    std::cout << macdonaldPoly << std::endl;
    Polynomial<Rational> p;
    Matrix<Rational> m;
    List<Polynomial<Rational> > module;
    std::cout << "I am processing root subalgebra of type "
              << currentRootSA.theDynkinDiagram.ToStringRelativeToAmbientType(W.theDynkinType[0]);
    module.AddOnTop(macdonaldPoly);
    int mcs=0;
    do
    { mcs = module.size;
      for(int i=1; i<W.GetDim()+1; i++)
      { W.GetStandardRepresentationMatrix(i,m);
        for(int j=0; j<mcs; j++){
          matrix_acts_on_polynomial(m,module[j],p);
          module.AddOnTop(p);
        }
      }
      Polynomial<Rational>::GaussianEliminationByRowsDeleteZeroRows(module);
    } while(module.size > mcs);
    std::cout << "...rank is " << module.size << std::endl;
    WeylGroupRepresentation<Rational> rep;
    rep.reset(&W);
    rep.names.SetSize(1);
    rep.names[0] = currentRootSA.theDynkinDiagram.ToStringRelativeToAmbientType(W.theDynkinType[0]);
    for(int i=1; i<W.GetDim()+1; i++)
    { Matrix<Rational> rm;
      rm.init(module.size, module.size);
      W.GetStandardRepresentationMatrix(i,m);
      for(int ji=0; ji<module.size; ji++) // would be neat if we
      { Vector<Rational> v;               // didnt need to go through
        matrix_acts_on_polynomial(m,module[ji],p); // stuff we've
        GetCoordsInBasisInputIsGaussianEliminated(module,p,&v); //
        for(int jj=0; jj<module.size; jj++) // already looked at, but
          rm(jj,ji) = v[jj]; // new dimensions can be inserted anywhere
      }
      rep.SetElementImage(i,rm);
    }
    std::cout << "has character ";
    std::cout << rep.GetCharacter() << std::endl;
    W.AddIrreducibleRepresentation(rep);
    std::cout << "elapsed seconds: " << theGlobalVariables.GetElapsedSeconds() << std::endl;
  }
}

class lennum
{ public:
  int num;
  Rational len;

  bool operator<(const lennum& y) const
  { return this->len < y.len;
  }
  bool operator>(const lennum& y) const
  { return this->len > y.len;
  }
};

WeylGroupRepresentation<Rational> get_macdonald_representation(WeylGroup& W, const List<Vector<Rational> >& roots)
{ std::cout << "starting with roots " << roots << std::endl;
  List<Vector<Rational> > monomial;
  List<List<Vector<Rational> > > monomials;
  Matrix<Rational> m;
  for(int i=0; i<W.size; i++)
  { W.GetStandardRepresentationMatrix(i,m);
    monomial.SetSize(roots.size);
    for(int j=0; j<roots.size; j++)
    { monomial[j] = m*roots[j];
      if(monomial[j].IsNegative())
        monomial[j] = -monomial[j];
    }
    monomial.QuickSortAscending();
    monomials.BSInsertDontDup(monomial);
  }
  std::cout << "have all " << monomials.size << " monomials";

  List<lennum> lens;
  List<Vector<Rational> > images;
  images.SetSize(monomials.size);
  lens.SetSize(monomials.size);
  for(int i=0; i<monomials.size; i++)
  { images[i].SetSize(W.RootSystem.size);
    Rational l = 0;
    for(int j=0; j<W.RootSystem.size; j++)
    { Rational p=1;
      for(int k1=0; k1<monomials[i].size; k1++)
      { Rational s=0;
        for(int k2=0; k2<monomials[i][k1].size; k2++)
          s += monomials[i][k1][k2] * W.RootSystem[j][k2];
        p *= s;
      }
      images[i][j] = p;
      l += p*p;
    }
    lens[i].len = l;
    lens[i].num = i;
  }
  lens.QuickSortAscending();
  std::cout << " ... sorted" << std::endl;

  List<int> monomials_used;
  VectorSpace<Rational> vs;
  for(int i=0; i<monomials.size; i++)
  { if(vs.AddVector(images[lens[i].num]))
      monomials_used.AddOnTop(lens[i].num);
  }
  Basis<Rational> B;
  for(int i=0; i<monomials_used.size; i++)
    B.AddVector(images[monomials_used[i]]);

  std::cout << "module rank is " << monomials_used.size << std::endl;

  WeylGroupRepresentation<Rational> rep;
  rep.reset(&W);
  for(int i=1; i<W.GetDim()+1; i++)
  { Matrix<Rational> m;
    W.GetStandardRepresentationMatrix(i,m);
    Matrix<Rational> rm;
    rm.init(monomials_used.size, monomials_used.size);
    for(int j=0; j<monomials_used.size; j++)
    { List<Vector<Rational> > monomial;
      monomial.SetSize(roots.size);
      for(int k=0; k<roots.size; k++)
        monomial[k] = m*monomials[monomials_used[j]][k];
      Vector<Rational> evaluated;
      evaluated.SetSize(W.RootSystem.size);
      for(int ei=0; ei<W.RootSystem.size; ei++)
      { Rational p = 1;
        for(int ej=0; ej<monomial.size; ej++)
        { Rational s = 0;
          for(int ek=0; ek<monomial[ej].size; ek++)
          { s += monomial[ej][ek] * W.RootSystem[ei][ek];
          }
          p *= s;
        }
        evaluated[ei] = p;
      }
      Vector<Rational> be = B.PutInBasis(evaluated);
      rm.AssignVectorToColumnKeepOtherColsIntactNoInit(j,be);
    }
    rep.SetElementImage(i,rm);
    std::cout << rm.ToString(&consoleFormat) << std::endl;
    if(!(rm*rm).IsIdMatrix())
      std::cout << "Error: this matrix is not an involution" << std::endl;
  }
  return rep;
}

// balanced ternary
Vector<int> pointi(int d, int i)
{ Vector<int> out;
  out.SetSize(d);
  for(int ii=0; ii<d; ii++)
  { int m = i%3;
    out[ii] = (m!=2)?m:-1;
    i /= 3;
  }
  return out;
}

WeylGroupRepresentation<Rational> get_macdonald_representation_v2(WeylGroup& W, const List<Vector<Rational> >& roots)
{ std::cout << "starting with roots " << roots << " at time " <<   theGlobalVariables.GetElapsedSeconds() << std::endl;
  List<Vector<Rational> > monomial;
  HashedList<List<Vector<Rational> > > monomials;
  Matrix<Rational> m;
  for(int i=0; i<W.size; i++)
  { W.GetStandardRepresentationMatrix(i,m);
    monomial.SetSize(roots.size);
    for(int j=0; j<roots.size; j++)
    { monomial[j] = m*roots[j];
      if(monomial[j].IsNegative())
        monomial[j] = -monomial[j];
    }
    monomial.QuickSortAscending();
    monomials.AddNoRepetitionOrReturnIndexFirst(monomial);
  }
  std::cout << "have all " << monomials.size << " monomials (" << theGlobalVariables.GetElapsedSeconds() << ")";

  // Σ(n choose k)x<<k
  int number_of_images = 1;
  for(int i=0; i<W.GetDim(); i++)
    number_of_images *= 3;


  List<lennum> lens;
  List<Vector<Rational> > images;
  images.SetSize(monomials.size);
  lens.SetSize(monomials.size);
  for(int i=0; i<monomials.size; i++)
  { images[i].SetSize(number_of_images);
    Rational l = 0;
    for(int j=0; j<number_of_images; j++)
    { Vector<int> point = pointi(W.GetDim(),j);
      Rational p=1;
      for(int k1=0; k1<monomials[i].size; k1++)
      { Rational s=0;
        for(int k2=0; k2<monomials[i][k1].size; k2++)
          s += monomials[i][k1][k2] * point[k2];
        p *= s;
      }
      images[i][j] = p;
      l += p*p;
    }
    lens[i].len = l;
    lens[i].num = i;
  }
  lens.QuickSortAscending();
  std::cout << " ... sorted (" << theGlobalVariables.GetElapsedSeconds() << ")" << std::endl;

  List<int> monomials_used;
  VectorSpace<Rational> vs;
  for(int i=0; i<monomials.size; i++)
  { if(vs.AddVector(images[lens[i].num]))
      monomials_used.AddOnTop(lens[i].num);
  }
  Basis<Rational> B;
  for(int i=0; i<monomials_used.size; i++)
    B.AddVector(images[monomials_used[i]]);

  std::cout << "module rank is " << monomials_used.size << " (" << theGlobalVariables.GetElapsedSeconds() << ")" << std::endl;

  WeylGroupRepresentation<Rational> rep;
  rep.reset(&W);
  for(int i=1; i<W.GetDim()+1; i++)
  { Matrix<Rational> m;
    W.GetStandardRepresentationMatrix(i,m);
    Matrix<Rational> rm;
    rm.init(monomials_used.size, monomials_used.size);
    for(int j=0; j<monomials_used.size; j++)
    { List<Vector<Rational> > monomial;
      monomial.SetSize(roots.size);
      for(int k=0; k<roots.size; k++)
        monomial[k] = m*monomials[monomials_used[j]][k];
      Vector<Rational> evaluated;
      evaluated.SetSize(number_of_images);
      for(int ei=0; ei<number_of_images; ei++)
      { Rational p = 1;
        for(int ej=0; ej<monomial.size; ej++)
        { Rational s = 0;
          for(int ek=0; ek<monomial[ej].size; ek++)
          { s += monomial[ej][ek] * pointi(W.GetDim(),ei)[ek];
          }
          p *= s;
        }
        evaluated[ei] = p;
      }
      Vector<Rational> be = B.PutInBasis(evaluated);
      for(int mnop=0; mnop<be.size; mnop++)
        rm.elements[j][mnop] = be[mnop];
    }
    rep.SetElementImage(i,rm);
    std::cout << rm.ToString(&consoleFormat) << std::endl;
    if((rm*rm).IsIdMatrix())
      std::cout << "passed idempotence test" << std::endl;
    else
      std::cout << "failed idempotence test" << std::endl;
  }


  Matrix<Rational> idr;
  idr.MakeIdMatrix(rep.GetDim());

  for(int i=0; i<W.GetDim(); i++)
  { for(int j=0; j<W.GetDim(); j++)
    { if(j == i)
        continue;
      int w = W.theDynkinType.GetCoxeterEdgeWeight(i,j);
      if(w <= 2)
        continue;
      Matrix<Rational> M1 = rep.GetElementImage(i+1);
      Matrix<Rational> M2 = rep.GetElementImage(j+1);
      Matrix<Rational> M3 = M1*M2;
      MathRoutines::RaiseToPower(M3,w,idr);
      if(M3.IsIdMatrix())
        std::cout << "passed Coxeter presentation test " << i << "," << j << std::endl;
      else
        std::cout << "failed Coxeter presentation test " << i << "," << j << std::endl;
    }
  }
  return rep;
}

void get_macdonald_representations_of_weyl_group_v2(SemisimpleLieAlgebra& theSSlieAlg)
{ WeylGroup& W = theSSlieAlg.theWeyl;

  GlobalVariables localGlobalVariables;
  localGlobalVariables.SetFeedDataToIndicatorWindowDefault(CGI::makeStdCoutReport);

  rootSubalgebras theRootSAs;
  theRootSAs.owneR=&theSSlieAlg;
  DynkinSimpleType dt = W.theDynkinType.GetGreatestSimpleType();
  theRootSAs.GenerateAllReductiveRootSubalgebrasUpToIsomorphism(localGlobalVariables, true, false);
  List<Vector<Rational> > roots;

  for (int k=0; k<theRootSAs.size; k++)
  { rootSubalgebra& currentRootSA=theRootSAs[k];
    roots=currentRootSA.PositiveRootsK;
    std::cout << "I am processing root subalgebra of type "
              << currentRootSA.theDynkinDiagram.ToStringRelativeToAmbientType(W.theDynkinType[0]);
    WeylGroupRepresentation<Rational> rep = get_macdonald_representation_v2(W,roots);
    rep.names.SetSize(1);
    rep.names[0] = currentRootSA.theDynkinDiagram.ToStringRelativeToAmbientType(W.theDynkinType[0]);
    std::cout << "has character ";
    std::cout << rep.GetCharacter() << std::endl;
    W.AddIrreducibleRepresentation(rep);
  }
}



template <class templateMonomial, class coefficient>
bool GetCoordsInBasisInputIsGaussianEliminated
(const List<MonomialCollection<templateMonomial, coefficient> > &inputReducedBasis,
 const MonomialCollection<templateMonomial, coefficient> & input,
 Vector<coefficient>* outputCoordinatesInBasis=0)
{ MacroRegisterFunctionWithName("GetCoordsInBasiS");
  MonomialCollection<templateMonomial, coefficient>  summand, v=input;
  coefficient currentCoeff;
  if (outputCoordinatesInBasis!=0)
    outputCoordinatesInBasis->MakeZero(inputReducedBasis.size);
  for(int i=0; i<inputReducedBasis.size; i++)
  { currentCoeff=v.GetMonomialCoefficient(inputReducedBasis[i].GetMaxMonomial());
    if (!currentCoeff.IsEqualToZero())
    { summand=inputReducedBasis[i];
      if (outputCoordinatesInBasis!=0)
        (*outputCoordinatesInBasis)[i]=currentCoeff;
      currentCoeff*=-1;
      summand*=currentCoeff;
      v-=summand;
    }
  }
  return v.IsEqualToZero();
}



int main(void)
{ InitializeGlobalObjects();
  //BSTest();

  /*
      Rational zero = Rational(0,1);
      Rational one = Rational(1,1);
      Rational two = Rational(2,1);
      std::cout << zero*one << "\n" << zero+one << one*two << one+two << two*two << std::endl;

  //    Vector<Rational> v;
  //    v.SetSize(3);
  //    v="(1/2,3,-1/5)";
  //    v[1].AssignString("1/3");
    //  v[2]=5;
      Vector<Rational> v;
      v = "(1/3,1,2)";
      std::cout << v << std::endl;

      Vector<Rational> w;
      w.SetSize(3);
      w[0] = 0;
      w[1] = 1;
      w[2] = 2;
      std::cout << w << std::endl;

      Matrix<Rational> M;
      M.Resize(3,3,false,&zero);
      M.ActOnVectorColumn(v,zero);
      std::cout << M << std::endl;
      std::cout << v << std::endl;
  */
  /*
      Rational one = 1;
      DynkinType D;
      D.MakeSimpleType('B',3,&one);
      WeylGroup G;
      G.MakeFromDynkinType(D);
      std::cout << G << std::endl;
      std::cout << G.CartanSymmetric << std::endl;
      G.ComputeAllElements();
      std::cout << G << std::endl;
  */
  /*
      Vector<Rational> v1; v1 = "(1,0)";
      Vector<Rational> v2; v2 = "(0,1)";

      std::cout << D << std::endl;
      D.GetCartanSymmetric(M);
      std::cout << M << std::endl;
      Vectors<Rational> V;
      G.ComputeWeylGroupAndRootsOfBorel(V);
      std::cout << G.GetReport() << std::endl;
      std::cout << "printing G " << G << std::endl;
      G.GetMatrixOfElement(0,M);


      Graph *g = new Graph(6,3);
      g->AddEdge(0,1);
      g->AddEdge(1,2);
      g->AddEdge(2,3);
      g->AddEdge(4,5);
      g->AddEdge(0,3);
      std::cout << "g" << g->edges << std::endl;
      std::cout << g->data << std::endl;
      List<List<int> > ccs;
      ccs = g->DestructivelyGetConnectedComponents();
      std::cout << ccs << std::endl;

      std::cout << G.RootsOfBorel << std::endl;

      std::cout << G[0] << '\n' << G[1] << '\n' << G[2] << '\n' << std::endl;
      M = GetMatrixOfElement(G,G[6]);
      std::cout << GetMatrixOfElement(G,G[6]) << std::endl;
      std::cout << G[6] << ' ' << M.GetDeterminant() << ' ' << M(0,0)+M(1,1)+M(2,2) << std::endl;
  */
  /*

     DynkinType D;
     D.MakeSimpleType('G',2);
     Matrix<Rational> M;
     D.GetCartanSymmetric(M);
     std::cout << M << std::endl;

  //  Matrix<f127> M2;
  //  M2.init(M.NumRows, M.NumCols);
  //  for(int i=0; i<M.NumRows; i++)
  //    for(int j=0; j<M.NumCols; j++)
  //     M2.elements[i][j].n = M.elements[i][j].GetNumerator().GetIntValueTruncated();
    CoxeterGroup G;
    G.CartanSymmetric = M;
    G.ComputeIrreducibleRepresentations();
    CoxeterRepresentation<Rational> r = G.irreps[0]*G.irreps[1];
    Matrix<Rational> cfm;
    ClassFunction<Rational> cf;
    cf = G.irreps[0].GetCharacter();
    r.ClassFunctionMatrix(cf,cfm);
    std::cout << cfm.ToString(&consoleFormat) << std::endl;
  */

  /*
   CoxeterGroup G;
   G.MakeFrom(M);
   G.ComputeInitialCharacters();
   for(int i=0; i<G.characterTable.size; i++)
      std::cout << G.characterTable[i] << std::endl;
  */
  /*
    for(int i=0;i<G.ccCount;i++){
      int n = G.conjugacyClasses[i][0];
      Matrix<Rational> M = G.TodorsVectorToMatrix(G.rhoOrbit[n]);
      std::cout << i << charpoly(M) << "\n\n";
    //  std::cout << M << '\n';
    //  std::cout << "\n\n";
    }

    G.characterTable.size = 0;
    for(int i=0; i<10; i++){
      ClassFunction<Rational> X;
      X.G = &G;
      for(int j=0; j<10; j++){
        X.data.AddOnTop(chartable[i][j]);
      }
      G.characterTable.AddOnTop(X);
    }

    std::cout << G.characterTable << std::endl;
  */
  /*
    GroupRingElement<Rational> XeP;
    XeP.MakeFromClassFunction(G.characterTable[0]);
    std::cout << XeP.data << std::endl;

    GroupRingElement<Rational> E;
    E.MakeEi(&G,0);
    std::cout << E << std::endl;
    std::cout << XeP*E << std::endl;

    GroupRingElement<Rational> XstdP;
    XstdP.MakeFromClassFunction(G.characterTable[2]);
    std::cout << XstdP << std::endl;

    std::cout << "\nhi there\n";
    for(int i=0; i<G.N; i++)
    { E.MakeEi(&G,i);
      std::cout << XstdP*E << std::endl;
    }

    std::cout << GetMatrix(G.characterTable[0]) << std::endl;
    std::cout << GetMatrix(G.characterTable[2]) << std::endl;

    E.MakeEi(&G,33);
    std::cout << XstdP*E << std::endl;
    std::cout << GetMatrix(G.characterTable[2])*E.data << std::endl;

    ElementMonomialAlgebra<CoxeterElement, Rational> Xstdp = FromClassFunction(G.characterTable[2]);
    std::cout << Xstdp << std::endl;
    std::cout << "here they are" << std::endl;
    List<ElementMonomialAlgebra<CoxeterElement, Rational> > l;
    for(int i=0; i<G.N; i++)
    { ElementMonomialAlgebra<CoxeterElement, Rational> tmp;
      tmp.AddMonomial(G.GetCoxeterElement(i),1);
      tmp *= Xstdp;
      l.AddOnTop(tmp);
      std::cout << tmp << '\n' << std::endl;
    }
    Xstdp.GaussianEliminationByRowsDeleteZeroRows(l);
    std::cout << l.size << std::endl;
  */
  /*
    Matrix<Rational> ct;
    ct.init(G.ccCount,G.ccCount);
    for(int i=0; i<G.ccCount; i++)
      for(int j=0; j<G.ccCount; j++)
      { ct.elements[i][j] = chartable[i][j];
        ct.elements[i][j] /= G.N;
        ct.elements[i][j] *= G.ccSizes[j];
      }
    */
  /*
  Vectors<Rational> powers;
  Vectors<Rational> powers2;
  Vector<Rational> v;
  v = G.characterTable[2].data;
  std::cout << v << '\n' << ct*v << '\n';
  powers.AddOnTop(v);
  for(int i=1; i<50; i++)
  { Vector<Rational> v;
    v = G.characterTable[2].data;
    for(int j=0; j<G.ccCount; j++)
      v[j] *= powers[i-1][j];
    Vector<Rational> w = ct*v;
    std::cout << v << '\n' << w << '\n';
    powers.AddOnTop(v);
    powers2.AddOnTop(w);
  }
  std::cout << powers2.GetRank() << std::endl;
  std::cout << powers.GetRank() << std::endl;

  ClassFunction<Rational> X;
  X.G = &G;
  X.data = powers[5];
  std::cout << X << std::endl;

  List<ClassFunction<Rational> > tstct;
  for(int i=0;i<G.ccCount;i++)
    std::cout << X.IP(G.characterTable[i]) << std::endl;

  Matrix<Rational> XP;
  XP = GetMatrix(G.characterTable[2]);
  GroupRingElement<Rational> XPP;
  Vector<Rational> vv;
  XPP.MakeFromClassFunction(G.characterTable[2]);
  GroupRingElement<Rational> vP;
  for(int i=0; i<G.N; i++)
  { vv.MakeEi(G.N,i);
    vP.MakeEi(&G,i);
    std::cout << (XP*vv == (XPP*vP).data) << ' ';
  }
  */
  //  Vectors<Rational> V;
  //  XP.GetZeroEigenSpace(V);
  //  std::cout << V << std::endl;
  //  std::cout << V.GetRank() << std::endl;

  /*
  Matrix<Rational> A;
  A.init(3,3);
  Rational one = 1;

  A(0,0) = 1; A(0,1) = 2; A(0,2) = 3;
  A(1,0) = 4; A(1,1) = 5; A(1,2) = 6;
  A(2,0) = 7; A(2,1) = 8; A(2,2) = 9;
  std::cout << Kernel(A) << std::endl;

  A(0,0) = 1; A(0,1) = 0; A(0,2) = 0;
  A(1,0) = 0; A(1,1) = 1; A(1,2) = 0;
  A(2,0) = 0; A(2,1) = 0; A(2,2) = 0;
  std::cout << Kernel(A) << std::endl;
  std::cout << Eigenspace(A,one) << std::endl;
  */
  /*
    Matrix<Rational> A;
    Rational one = 1;
    A.init(6,6);
    A(0,0) = 2; A(0,1) =-1; A(0,2) =-1; A(0,3) = 0; A(0,4) = 0; A(0,5) = 0;
    A(1,0) =-1; A(1,1) = 2; A(1,2) =-1; A(1,3) = 0; A(1,4) = 0; A(1,5) = 0;
    A(2,0) =-1; A(2,1) =-1; A(2,2) = 2; A(2,3) = 0; A(2,4) = 0; A(2,5) = 0;
    A(3,0) = 0; A(3,1) = 0; A(3,2) = 0; A(3,3) = 2; A(3,4) =-1; A(3,5) =-1;
    A(4,0) = 0; A(4,1) = 0; A(4,2) = 0; A(4,3) =-1; A(4,4) = 2; A(4,5) =-1;
    A(5,0) = 0; A(5,1) = 0; A(5,2) = 0; A(5,3) =-1; A(5,4) =-1; A(5,5) = 2;
    A /= 3;
    std::cout << DestructiveEigenspace(A,one) << std::endl;

    Rational zero = 0;
    Rational negone = -1;
    List<List<Rational> > ll;
    GlobalVariables g;
    A.FindZeroEigenSpacE(ll,one,negone,zero,g);
    std::cout << ll << std::endl;
  */
  /*
    Matrix<Rational> XP;
    for(int i=0; i<G.ccCount; i++)
    {
    XP = GetMatrix(G.characterTable[i]);
    XP *= G.characterTable[i].data[0];
    std::cout << G.characterTable[i] << std::endl;
    std::cout << DestructiveEigenspace(XP,one) << std::endl;
    }
  */
  /*
    Rational three = 3;
    List<Rational> p;
    p.AddOnTop(one); p.AddOnTop(three); p.AddOnTop(three); p.AddOnTop(one);
    std::cout << factorpoly(p,5) << std::endl;
    p.size = 0;
    p.AddOnTop(one); p.AddOnTop(three); p.AddOnTop(one); p.AddOnTop(three);
    std::cout << factorpoly(p,5) << std::endl;

    Vector<Rational> v;
    List<Vector<Rational> > B;
    v.AssignString("(1,1,0)");
    B.AddOnTop(v);
    v.AssignString("(1,0,1)");
    B.AddOnTop(v);
    v.AssignString("(0,1,1)");
    B.AddOnTop(v);
    v.AssignString("(2,2,2)");
    std::cout << PutInBasis(v,B) << std::endl;
    B.size = 0;
    v.AssignString("(1,1,0,0)");
    B.AddOnTop(v);
    v.AssignString("(1,0,0,1)");
    B.AddOnTop(v);
    v.AssignString("(0,0,1,1)");
    B.AddOnTop(v);
    std::cout << "4-vector tests" << std::endl;
    v.AssignString("(1,0,0,1)");
    std::cout << PutInBasis(v,B) << std::endl;
    v.AssignString("(0,1,1,0)");
    std::cout << PutInBasis(v,B) << std::endl;

    M = GetMatrix(G.characterTable[2]);
    M *= 3;
    B = DestructiveEigenspace(M,one);
    std::cout << B << std::endl;

    Matrix<Rational> BM;
    BM.init(B.size,B[0].size);
    for(int i=0; i<B.size; i++)
    { for(int j=0; j<B[0].size; j++)
      { BM.elements[i][j] = B[i][j];
      }
    }

    std::cout << BM.ToString(&consoleFormat) << std::endl;
    for(int i=0; i<BM.NumRows; i++)
    { std::cout << PutInBasis(B[i],B) << std::endl;
    }

    int ge = 37;
    M.init(B.size,B.size);
    for(int i=0; i<B.size; i++)
    { Vector<Rational> v;
      v.MakeZero(B[0].size);
      for(int j=0; j<B[0].size; j++)
      { v[G.MultiplyElements(ge,j)] = B[i][j];
      }
      v = PutInBasis(v,B);
      for(int j=0;j<B.size;j++)
      { M.elements[i][j] = v[j];
      }
    }
    std::cout << M.ToString(&consoleFormat) << std::endl;



    List<List<Vector<Rational> > > spaces;
    M = GetMatrix(G.characterTable[2]);
    B = DestructiveColumnSpace(M);
    spaces.AddOnTop(B);
    List<ClassFunction<Rational> > Xs;
    Xs.AddOnTop(G.characterTable[2]);
    std::cout << spaces << std::endl;
    { int i=1;
      Xs.AddOnTop(Xs[i-1]*Xs[i-1]);
      M = GetMatrix(Xs[i]);
  //    List<Rational> p = charpoly(M);
  //    std::cout << p << std::endl;
  //    List<int> ls = factorpoly(p,Xs[i].data[0].floorIfSmall());
  //    std::cout << ls << std::endl;
      B = DestructiveColumnSpace(M);
      std::cout << B << std::endl;
    }
  */
  /*

    Matrix<Rational> a;
    Matrix<Rational> b;
    a.init(2,2);
    b.init(2,2);
    a(0,0) = -1; a(0,1) = -1; a(1,0) =  1; a(1,1) =  0;
    b(0,0) =  1; b(0,1) =  0; b(1,0) = -1; b(1,1) = -1;
    M.AssignTensorProduct(b,b);
    std::cout << M.ToString(&consoleFormat) << std::endl;
  */
  /*
      std::cout << "this orbit has " << G.rhoOrbit.size << std::endl;
      std::cout << G.rhoOrbit << std::endl;
      Vector<Rational> v = G.rhoOrbit[34];
      M = G.TodorsVectorToMatrix(v);
      std::cout << M*G.rho << " derp " << v << std::endl;
  */
//    for(int i=0;i<G.rhoOrbit.size;i++){
//    std::cout << G.TodorsVectorToMatrix(G.rhoOrbit[i]) << "\n\n";
//    }
//    std::cout << std::endl;
  /*
      std::cout << v << G.DecomposeTodorsVector(v) << G.ComposeTodorsVector(G.DecomposeTodorsVector(v)) << std::endl;

      G.ComputeConjugacyClasses();
      std::cout << G.conjugacyClasses << std::endl;
      std::cout << G.ccSizes << std::endl;
      G.ComputeSquares();
      std::cout << G.squares << std::endl;
      G.ComputeInitialCharacters();
      std::cout << G.characterTable << std::endl;
      std::cout << G.characterTable[0].norm() << G.characterTable[1].norm() << G.characterTable[2].norm() << std::endl;

  //    ElementMonomialAlgebra<FiniteGroupElement, Rational> a;
      ClassFunction X = G.characterTable[2] - G.characterTable[0];
      std::cout << X << X.norm() << std::endl;

      List<Vector<Rational> > l;
      for(int i=0;i<G.rootSystem.size;i++){
          bool usethis = true;
          for(int j=0; j<4; j++)
              if(G.rootSystem[i][j] < 0){
                  usethis = false;
                  break;
              }
          if(usethis == true)
              l.AddOnTop(G.rootSystem[i]);
      }
      std::cout << l << std::endl;
  */
  /*
      std::string* S = [
      ""

      ]
  */
  /*    std::cout << "polynom1al" << std::endl;
      polynom1al<Rational> zeropoly;
      Matrix<polynom1al<Rational> > MP;
      MP.MakeZeroMatrix(G.N, zeropoly);
      for(int i1=0; i1<G.ccCount; i1++)
      { for(int i2=0; i2<G.ccSizes[i1]; i2++)
        { int i=G.conjugacyClasses[i1][i2];
          for(int j=0; j<G.N; j++)
          { M.elements[G.MultiplyElements(i,j)][j] = G.characterTable[2].data[i1]/G.N;
          }
        }
      }
      for(int i=0; i<G.N; i++)
      { MP.elements[i][i].AddMonomial(-1,1);
      }

      polynom1al<Rational> cp;

      cp = MP.GetDeterminant();

      std::cout << cp << std::endl;
  */
//    List<bool> isod;
//    isod.AddOnTop(false);
//    int isodd = 0;
  /*    List<Vector<Rational> > bigspace;
      List<List<Vector<Rational> > > spaces;
      for(int i=0; i<G.N; i++)
      {Vector<Rational> v;
        v.MakeEi(G.N,i);
        bigspace.AddOnTop(v);
      }
      spaces.AddOnTop(bigspace);
      Rational one = 1;
      Rational zero = 0;
      for(int ci=0; ci<G.ccCount; ci++)
      { ClassFunction<Rational> X;
        X.G = &G;
        for(int ii=0; ii<G.ccCount; ii++)
        { if(ii==ci)
            X.data.AddOnTop(one);
            continue;
          X.data.AddOnTop(zero);
        }

      }
      */
  /*    ClassFunction<Rational> Xs = G.characterTable[2];
      ClassFunction<Rational> Xcs = G.characterTable[1]*G.characterTable[2];
      List<List<ClassFunction<Rational> > > chars;
      List<ClassFunction<Rational> > charsi;
      charsi.AddOnTop(G.characterTable[0]);
      chars.AddOnTop(charsi);
      HashedList<ClassFunction<Rational> > usedchars;
      int ci = 0;
      int cj = 0;
      while(true)
      { if(!usedchars.Contains(chars[ci][cj]))
        { usedchars.AddOnTop(chars[ci][cj]);
          std::cout << "matrix for " << ci << " " << cj << " " << chars[ci][cj] << std::endl;
          Matrix<Rational> MM = GetMatrix(chars[ci][cj]);
          std::cout << "eigenspaces" << std::endl;
          List<List<Vector<Rational> > > es = eigenspaces(MM,G.N);
          std::cout << "splitting spaces" << std::endl;
          for(int i=0; i<es.size; i++)
          { int spaces_to_check = spaces.size;
            for(int j=0; j<spaces_to_check; j++)
            { //if(isod[j])
              //  continue;
              if(spaces[j].size < 9)
                continue;
              std::cout << "intersect eigenspace " << i << " with space " << j << std::endl;
              List<Vector<Rational> > middle = intersection(es[i], spaces[j]);
              std::cout << "result is " << middle.size << "dimensional" << std::endl;
              if(0 < middle.size and middle.size < spaces[j].size)
              { std::cout << "finding relative complement" << std::endl;
                List<Vector<Rational> > right = relative_complement(spaces[j],es[i]);
                std::cout << "relative complement is " << right.size << "dimensional" << std::endl;
                spaces[j] = middle;
                //if(is_isotypic_component(G,middle))
                //{ isod[j] = true;
                //  isodd += middle.size;
                //  std::cout << "intersection was an isotypic component" << std::endl;
                //}
                spaces.AddOnTop(right);
                //if(is_isotypic_component(G,right))
                //{ isod.AddOnTop(true);
                //  isodd += right.size;
                //  std::cout << "complement was an isotypic component" << std::endl;
                //} else
                //{ isod.AddOnTop(false);
                //}
                std::cout << "moving on" << std::endl;
              }
            }
          }
  //        if(isodd >= G.N)
  //          break;
          if(spaces.size == G.ccCount)
            break;
        }
        std::cout << "finding next character" << std::endl;
        if(ci!=0)
        { ci--;
          cj++;
          chars[ci].AddOnTop(chars[ci][cj-1]*Xcs);
        } else
        { ci=cj+1;
          cj=0;
          List<ClassFunction<Rational> > cil;
          cil.AddOnTop(chars[ci-1][0]*Xs);
          chars.AddOnTop(cil);
        }
      }
      std::cout << "spaces" << std::endl;
      std::cout << spaces.size << std::endl;
      for(int i=0; i<spaces.size; i++)
        std::cout << spaces[i].size << " ";
      std::cout << std::endl;
      //std::cout << isod << std::endl;
      //std::cout << isodd << std::endl;
  */
  /*    int i=0; j=0;
      while(spaces.size > 0)
      {

      }
  */

  /*    std::cout << V2.GetCharacter() << std::endl;
      CoxeterRepresentation<Rational> V2x2 = V2*V2;
      std::cout << V2x2.GetCharacter() << std::endl;
      for(int i=0; i<G.rank; i++)
      { std::cout << V2x2.gens[i].ToString(&consoleFormat);
        std::cout << V2x2.gens[i].GetDeterminant() << ' ' << V2x2.gens[i].GetTrace() << std::endl;
      }
      std::cout << "class function matrices" << std::endl;
      for(int cfi=0; cfi<G.ccCount; cfi++)
      { ClassFunction<Rational> cf;
        cf.G = &G;
        cf.MakeZero();
        cf[cfi] = 1;
        std::cout << cfi << std::endl;
        std::cout << V2x2.ClassFunctionMatrix(cf).ToString(&consoleFormat) << std::endl;
      }
      List<CoxeterRepresentation<Rational> > Vs;
      Vs = V2x2.Decomposition();
      for(int i=0; i<Vs.size; i++)
        std::cout << Vs[i].Reduced().GetCharacter() << std::endl;
    */

  /*Matrix<Rational> A;
  A.init(6,6);
  A(0,0) = 2; A(0,1) =-1; A(0,2) =-1; A(0,3) = 0; A(0,4) = 0; A(0,5) = 0;
  A(1,0) =-1; A(1,1) = 2; A(1,2) =-1; A(1,3) = 0; A(1,4) = 0; A(1,5) = 0;
  A(2,0) =-1; A(2,1) =-1; A(2,2) = 2; A(2,3) = 0; A(2,4) = 0; A(2,5) = 0;
  A(3,0) = 0; A(3,1) = 0; A(3,2) = 0; A(3,3) = 2; A(3,4) =-1; A(3,5) =-1;
  A(4,0) = 0; A(4,1) = 0; A(4,2) = 0; A(4,3) =-1; A(4,4) = 2; A(4,5) =-1;
  A(5,0) = 0; A(5,1) = 0; A(5,2) = 0; A(5,3) =-1; A(5,4) =-1; A(5,5) = 2;
  std::cout << "polynomial test" << std::endl;
  UDPolynomial<Rational> p = MinPoly(A);
  std::cout << p.data.size << std::endl;
  std::cout << p << std::endl;
  */

  /*std::cout << '[';
  for(int i=0; i<G.ccCount; i++)
    std::cout << G.conjugacyClasses[i][0] << ", ";
  std::cout << ']' << std::endl;
  for(int i=0; i<G.ccCount; i++)
    std::cout << i << ' ' << G.ccSizes[i] << ' ' << G.GetCoxeterElement(G.conjugacyClasses[i][0]) << std::endl;
  for(int i=0; i<G.ccCount; i++)
    std::cout << G.GetCoxeterElement(G.conjugacyClasses[i][0]) << ", ";*/
//  G.ComputeIrreducibleRepresentations();
  /*
  List<ClassFunction<Rational> > ct;
  List<CoxeterRepresentation<Rational> > gr;

  CoxeterRepresentation<Rational> sr = G.StandardRepresentation();
  CoxeterRepresentation<Rational> sr2 = sr*sr;

  List<CoxeterRepresentation<Rational> > sr2d = sr2.Decomposition(ct,gr);

  std::cout << "std (x) std = ";
  for(int i=0; i<sr2d.size; i++)
  { std::cout << sr2d[i].GetCharacter() << std::endl;
    for(int j=0; j<sr2d[i].gens.size; j++)
     std::cout << sr2d[i].gens[j].ToString(&consoleFormat) << std::endl;
  }
  */



  /*
     std::cout << "Building QG" << std::endl;
     WeylGroupRepresentation<Rational> QG;
     QG.reset(&G);
     for(int g=1; g<G.CartanSymmetric.NumRows+1; g++)
     { Matrix<Rational> M;
       M.MakeZeroMatrix(G.theElements.size);
       for(int i=0; i<G.theElements.size; i++)
       { ElementWeylGroup gg;
         gg = G.theElements[g];
         gg *= G.theElements[i];
         gg.MakeCanonical();
         M.elements[i][G.theElements.GetIndex(gg)] = 1;
       }
       QG.SetElementImage(g,M);
     }

     std::cout << "getting isotypic components of QG" << std::endl;
     List<WeylGroupRepresentation<Rational> > isocomps;
     isocomps.SetSize(G.irreps.size);
     for(int i=0; i<G.irreps.size; i++)
     { Matrix<Rational> M;
       QG.GetClassFunctionMatrix(G.irreps[i].GetCharacter(),M);
       VectorSpace<Rational> V;
       Vectors<Rational> B;
       for(int j=0; j<M.NumCols; j++)
       { Vector<Rational> v;
         M.GetVectorFromColumn(j,v);
         if(!V.AddVector(v))
           continue;
         B.AddOnTop(v);
       }
       Vector<Rational> v;
       QG.Restrict(B,v,isocomps[i]);
     }

     std::cout << "trying to break up one isotypic component" << std::endl;
     int cmpx = 2;
     std::cout << isocomps[cmpx].ToString(&consoleFormat) << std::endl;
     int d = isocomps[cmpx].GetRank();
     std::cout << "the smoothed-out inner product" << std::endl;
     Matrix<Rational> B;
     B.init(d,d);
     for(int i=0; i<d; i++)
       for(int j=0; j<d; j++)
         for(int g=0; g<G.theElements.size; g++)
         { Vector<Rational> v,w;
           isocomps[cmpx].GetElementImage(g).GetVectorFromColumn(i,v);
           isocomps[cmpx].GetElementImage(g).GetVectorFromColumn(j,w);
           B.elements[i][j] = v.ScalarEuclidean(w);
         }
    std::cout << B.ToString(&consoleFormat) << std::endl;

    Vector<Rational> v;
    v.MakeZero(B.NumRows);
    for(int i=0; i<B.NumCols; i++)
      for(int j=0; j<B.NumRows; j++)
        v[j] += B.elements[i][j];
    std::cout << v << std::endl;

    VectorSpace<Rational> WS;
    Vectors<Rational> W;
    for(int g=0; g<G.theElements.size; g++)
    { Vector<Rational> w = isocomps[cmpx].GetElementImage(g) * v;
      if(WS.AddVector(w))
        W.AddOnTop(w);
    }
    WeylGroupRepresentation<Rational> comp;
    Vector<Rational> derp;
    isocomps[cmpx].Restrict(W,derp,comp);

    std::cout << "This should be one component" << std::endl;
    std::cout << comp.ToString(&consoleFormat) << std::endl;
  */




  char letter = 'E';
  int number = 6;

  GlobalVariables localGlobalVariables;
  localGlobalVariables.SetFeedDataToIndicatorWindowDefault(CGI::makeStdCoutReport);
  SemisimpleLieAlgebra theSSlieAlg;
  theSSlieAlg.theWeyl.MakeArbitrarySimple(letter, number);
  theSSlieAlg.ComputeChevalleyConstantS(&localGlobalVariables);
  WeylGroup& W=theSSlieAlg.theWeyl;
  W.ComputeConjugacyClasses();

  get_macdonald_representations_of_weyl_group_v2(theSSlieAlg);
  for(int i=0; i<W.irreps.size; i++)
    std::cout << W.irreps[i].GetName() << '\t' << W.irreps[i].GetCharacter() << std::endl;

  ComputeCharacterTable(W);
  //ComputeIrreps(W);
/*UDPolynomial<Rational> p;
  Matrix<Rational> m;
  for(int i=0; i<W.conjugacyClasses.size; i++)
  { for(int j=0; j<W.conjugacyClasses[i].size; j++)
    { W.GetStandardRepresentationMatrix(W.conjugacyClasses[i][j],m);
      p.AssignCharPoly(m);
      std::cout << i << ',' << j << ": " << p << std::endl;
    }
  }*/

  /*
     JSData data;
     data.readfile("c7ct");
     if(data.type != JSNULL)
     { std::cout << data << std::endl;
       AddCharTable(data,G);
     } else
     { ComputeCharacterTable(G);
       PrintCharTable(G, NULL);
       PrintCharTable(G, "c7ct");
     }
     JSData ts;
     PrettyPrintTauSignatures(G,ts);
     ts.writefile("c7ts");
  */
//   G.ComputeConjugacyClasses();
//   std::cout << "Conjugacy class representatives" << std::endl;
//   for(int i=0; i<G.conjugacyClasses.size; i++)
//    std::cout << G.rhoOrbit[G.conjugacyClasses[i][0]] << std::endl;
  /*   List<Vector<Rational> > chars = ComputeCharacterTable(G);
     for(int i=0; i<chars.size; i++)
       std::cout << chars[i] << std::endl;
       */

//  WeylGroup G;
//  G.MakeArbitrarySimple('E',7);
//  std::cout <<
//  std::cout << FindConjugacyClassRepresentatives(G,500);

  std::cout << "Rational.TotalSmallAdditions: " << Rational::TotalSmallAdditions;
  std::cout << "\nRational.TotalLargeAdditions: " << Rational::TotalLargeAdditions;
  std::cout << "\nRational.TotalSmallMultiplications: " << Rational::TotalSmallMultiplications;
  std::cout << "\nRational.TotalLargeMultiplications: " << Rational::TotalLargeMultiplications << std::endl;


  /*  f127 a;
    a.n = 2;
    f127 b;
    b.n = 5;
    std::cout << (a*b).n << std::endl;
    std::cout << (a/b).n << std::endl;
  */

//   std::string s;
//   std::cin >> s;
  return 0;
}
