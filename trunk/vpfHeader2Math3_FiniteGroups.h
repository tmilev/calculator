//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfCharacterHeader_alreadyIncluded
#define vpfCharacterHeader_alreadyIncluded
#include "vpfHeader1General0_General.h"
#include "vpfHeader2Math0_General.h"
#include "vpfHeader2Math8_VectorSpace.h"

static ProjectInformationInstance ProjectInfoVpfFiniteGroups(__FILE__, "Header file, finite groups. Work in progress.");

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

class WeylGroupVirtualRepresentation
{
  public:
  WeylGroup* ownerGroup;
  Vector<Rational> coefficientsIrreps;
  WeylGroupVirtualRepresentation():ownerGroup(0){}
  std::string ToString(FormatExpressions* theFormat)const;
  void AssignWeylGroupRep(const WeylGroupRepresentation<Rational>& input, GlobalVariables* theGlobalVariables=0);
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  static unsigned int HashFunction(const WeylGroupVirtualRepresentation& input)
  { unsigned int result=0;
    if (input.ownerGroup!=0)
      result+=input.ownerGroup->HashFunction();
    result+=input.coefficientsIrreps.HashFunction();
    return result;
  }
  void operator+=(const WeylGroupVirtualRepresentation& other);
  void operator*=(const WeylGroupVirtualRepresentation& other);
  bool operator==(const WeylGroupVirtualRepresentation& other)const
  { return this->ownerGroup==other.ownerGroup &&
    this->coefficientsIrreps==other.coefficientsIrreps;
  }
};

template <typename ElementEuclideanDomain>
struct DivisionResult
{  ElementEuclideanDomain quotient;
   ElementEuclideanDomain remainder;
};

class CoxeterGroup;
class CoxeterElement;

template <typename coefficient>
class CoxeterRepresentation;

template<typename coefficient>
class ClassFunction{
    public:
    CoxeterGroup *G;
    List<coefficient> data;

    ClassFunction():G(0){} //the syntax :G(0) initializes the pointer G with 0.
    //Although there may be a minor speed penalty
    //(such a speed penalty is system dependent and possibly
    //even hardware dependent),
    //it is worth it to initialize all pointers with 0.

    void MakeZero();
    coefficient IP(const ClassFunction &other) const;
    coefficient norm() const;
    ClassFunction<coefficient> operator*(const ClassFunction<coefficient> &other) const;
    ClassFunction<coefficient> Sym2() const;
    ClassFunction<coefficient> Alt2() const;
    ClassFunction<coefficient> operator+(const ClassFunction<coefficient> &other) const;
    ClassFunction<coefficient> operator-(const ClassFunction<coefficient> &other) const;
    ClassFunction<coefficient> ReducedWithChars(const List<ClassFunction<coefficient> > chars = 0);
    coefficient& operator[](int i) const;
    std::string ToString(FormatExpressions* theFormat=0) const;
    static unsigned int HashFunction(const ClassFunction<coefficient>& input);
    inline unsigned int HashFunction()const
    { return this->HashFunction(*this);
    }
    bool operator==(const ClassFunction<coefficient>& other) const;
    bool operator>(const ClassFunction<coefficient>& right) const;
};

template <typename coefficient>
std::ostream& operator<<(std::ostream& out, const ClassFunction<coefficient> X);

class CoxeterGroup
{ public:
  // these are things any finite group should have
  int N;
  int ccCount;
  List<int> ccSizes;
  List<List<int> > conjugacyClasses;
  List<int> squares;
  List<ClassFunction<Rational> > characterTable;
  List<CoxeterRepresentation<Rational> > irreps;
  // these are specific to this type of finite group
  Matrix<Rational> CartanSymmetric;
  int nGens;
  HashedList<Vector<Rational> > rootSystem;
  Vector<Rational> rho;
  HashedList<Vector<Rational> > rhoOrbit;
  std::string ToString(FormatExpressions* theFormat=0) const;
  CoxeterGroup()
  { this->nGens=-1;
  }
  CoxeterGroup(const DynkinType& D)
  { this->MakeFrom(D);
  }
  void MakeFrom(const DynkinType& D);
  void MakeFrom(const Matrix<Rational>& M);
  bool operator==(const CoxeterGroup& other) const
  { return this->CartanSymmetric==other.CartanSymmetric;
  }
  static unsigned int HashFunction(const CoxeterGroup& input)
  { return input.CartanSymmetric.HashFunction();
  }
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  Vector<Rational> SimpleReflection(int i, const Vector<Rational> &right) const;
  HashedList<Vector<Rational> > GetOrbit(const Vector<Rational> &v) const;
  void ComputeRhoOrbit();
  Matrix<Rational> SimpleReflectionMatrix(int i) const;
  Matrix<Rational> TodorsVectorToInvMatrix(const Vector<Rational> &v) const;
  Matrix<Rational> TodorsVectorToMatrix(const Vector<Rational> &v) const;
  CoxeterElement GetCoxeterElement(int i) const;
  int GetIndexOfElement(const CoxeterElement& g) const;
  // note: the list is backwards

  List<int> DecomposeTodorsVector(const Vector<Rational> &v) const;
  Vector<Rational> ComposeTodorsVector(const List<int> &l) const;
  Vector<Rational> ApplyList(const List<int> &l, const Vector<Rational> &v) const;
  Vector<Rational> SimpleConjugation(int i, const Vector<Rational> &v) const;
  int MultiplyElements(int i, int j) const;
  int operator()(int i, int j) const;
  CoxeterRepresentation<Rational> StandardRepresentation();
  void ComputeElements();
  void ComputeConjugacyClasses();
  void ComputeSquares();
  void ComputeInitialCharacters();
  void ComputeIrreducibleRepresentations();

  List<List<bool> > GetTauSignatures();
};

class CoxeterElement{
  public:
  friend std::ostream& operator << (std::ostream& output, const CoxeterElement& theMon)
  { output << theMon.ToString();
    return output;
  }

  List<int> reflections;
  const CoxeterGroup* owner;
  CoxeterElement():owner(0){}
  CoxeterElement(const CoxeterGroup* owner, const List<int>& inputReflections){
    this->owner = owner; this->reflections = inputReflections;
  }
  void canonicalize();
  static unsigned int HashFunction(const CoxeterElement& input);
  std::string ToString(FormatExpressions* theFormat=0) const;
  void operator*=(const CoxeterElement& other);
  bool operator==(const CoxeterElement& other) const;
  bool operator>(const CoxeterElement& other) const
  { if(this->owner!=other.owner)
    { crash << "This may or may not be a programming error: comparing elements of different coxeter groups. "
      << "Crashing to let you know. " << crash;
    }
    return this->reflections>other.reflections;
  }
  static inline const bool IsEqualToZero()
  { return false;
  }

};

template <typename coefficient>
class GroupRingElement
{
  public:
  Vector<coefficient> data;
  CoxeterGroup* G;

  GroupRingElement(){}
  void MakeEi(CoxeterGroup* G, int i);
  void MakeFromClassFunction(CoxeterGroup* G, const List<coefficient>& l);
  void MakeFromClassFunction(const ClassFunction<coefficient>& l);
  GroupRingElement operator+(const GroupRingElement& right) const;
  GroupRingElement operator-(const GroupRingElement& right) const;
  GroupRingElement operator*(const GroupRingElement& right) const;
};
template <typename coefficient>
std::ostream& operator<<(std::ostream& out, const GroupRingElement<coefficient> g);

template <typename coefficient>
GroupRingElement<coefficient> GroupRingElement<coefficient>::operator+(const GroupRingElement<coefficient>& right) const
{ GroupRingElement<coefficient> out;
  out.G = G;
  out.data = data + right.data;
  return out;
}

template <typename coefficient>
GroupRingElement<coefficient> GroupRingElement<coefficient>::operator-(const GroupRingElement<coefficient>& right) const
{ GroupRingElement<coefficient> out;
  out.G = G;
  out.data = data - right.data;
  return out;
}

template <typename coefficient>
GroupRingElement<coefficient> GroupRingElement<coefficient>::operator*(const GroupRingElement<coefficient>& right) const
{ GroupRingElement<coefficient> out;
  out.G = G;
  out.data.MakeZero(G->N);
  for(int i=0; i<G->N; i++)
  { if(data[i]==0)
      continue;
    for(int j=0; j<G->N; j++)
    { if(right.data[j]==0)
        continue;
      out.data[G->MultiplyElements(i,j)] = data[i]*right.data[j];
    }
  }
  return out;
}

template <typename coefficient>
void GroupRingElement<coefficient>::MakeEi(CoxeterGroup *GG, int i)
{ G = GG;
  data.MakeEi(G->N,i);
}

template <typename coefficient>
void GroupRingElement<coefficient>::MakeFromClassFunction(const ClassFunction<coefficient>& l)
{ MakeFromClassFunction(l.G,l.data);
}

template <typename coefficient>
void GroupRingElement<coefficient>::MakeFromClassFunction(CoxeterGroup* GG, const List<coefficient>& l)
{ G = GG;
  data.MakeZero(G->N);
  for(int i=0; i<G->ccCount; i++){
    for(int j=0; j<G->ccSizes[i]; j++){
      data[G->conjugacyClasses[i][j]] = l[i];
    }
  }
}

//Matrix<Element>
template <typename coefficient>
std::ostream& operator<<(std::ostream& out, const GroupRingElement<coefficient> g)
{ out << g.data;
  return out;
}

//---------------------------------------------------------------------------Characters --------------------------------
//--------------------------------Characters----------------------------

template<typename coefficient>
coefficient ClassFunction<coefficient>::IP(const ClassFunction<coefficient> &other) const{
    coefficient acc = 0;
    for(int i=0;i<G->ccCount;i++)
        acc +=  this->data[i].GetComplexConjugate() * other[i].GetComplexConjugate() * G->ccSizes[i];
    return acc/G->N;
}

template<typename coefficient>
coefficient ClassFunction<coefficient>::norm() const {
    return this->IP(*this);
}

// The next three functions are practically identical
template<typename coefficient>
ClassFunction<coefficient> ClassFunction<coefficient>::operator*(const ClassFunction<coefficient> &other) const{
    ClassFunction<coefficient> l;
    l.G = G;
    l.data.SetExpectedSize(G->ccCount);
    for(int i=0; i<G->ccCount; i++)
        l.data.AddOnTop(this->data[i] * other[i]);
    return l;
}

template<typename coefficient>
ClassFunction<coefficient> ClassFunction<coefficient>::Sym2() const{
    ClassFunction<coefficient> l;
    l.G = G;
    l.data.SetExpectedSize(G->ccCount);
    for(int i=0; i<G->ccCount; i++){
        l.data.AddOnTop((this->data[i] * this->data[i] + this->data[G->squares[i]])/2);
    }
    return l;
}

template<typename coefficient>
ClassFunction<coefficient> ClassFunction<coefficient>::Alt2() const{
    ClassFunction<coefficient> l;
    l.G = G;
    l.data.SetExpectedSize(G->ccCount);
    for(int i=0; i<G->ccCount; i++){
        l.data.AddOnTop((this->data[i] * this->data[i] - this->data[G->squares[i]])/2);
    }
    return l;
}

template<typename coefficient>
ClassFunction<coefficient> ClassFunction<coefficient>::operator+(const ClassFunction<coefficient> &other) const{
    ClassFunction<coefficient> l;
    l.G = G;
    l.data.SetExpectedSize(G->ccCount);
    for(int i=0; i<G->ccCount; i++){
        l.data.AddOnTop(this->data[i] + other[i]);
    }
    return l;
}

template<typename coefficient>
ClassFunction<coefficient> ClassFunction<coefficient>::operator-(const ClassFunction &other) const{
    ClassFunction<coefficient> l;
    l.G = G;
    l.data.SetExpectedSize(G->ccCount);
    for(int i=0; i<G->ccCount; i++){
        l.data.AddOnTop(this->data[i] - other[i]);
    }
    return l;
}

template <typename coefficient>
ClassFunction<coefficient> ClassFunction<coefficient>::ReducedWithChars(const List<ClassFunction<coefficient> > cchars)
{ ClassFunction<coefficient> X = *this;
  if(X.norm() == 0)
    return X;
  List<ClassFunction<coefficient> > chars;
  if(cchars == 0)
    chars = this->G->characterTable;
  else
    chars = cchars;
  bool outerChanged = false;
  bool innerChanged = false;
  ClassFunction X2;
  int n;
  do
  { for(int i=0; i<chars.size; i++)
    { do
      { innerChanged = false;
        X2 = X - chars[i];
        // No virtual characters allowed
        coefficient n = X2[0];
        if(n<0)
          continue;
        bool bad = false;
        for(int iii=0; iii<X2.data.size; iii++){
          if(n<X2[iii]||n<-X2[iii])
            bad = true;
        }
        if(bad)
          continue;
        if(X2.norm() < X.norm())
        { X = X2;
          if(X.norm() == 0)
            return X;
          innerChanged = true;
//          if(X[0]<0) // negative virtual character alert
//            for(int i=0; i<X.data.size; i++)
//              X.data[i] = -X.data[i];
        }
      } while(innerChanged);
    }
  } while(outerChanged);
  return X;
}

template <typename coefficient>
void ClassFunction<coefficient>::MakeZero()
{ this->data.SetSize(this->G->ccCount);
  for(int i=0; i<this->G->ccCount; i++)
    this->data[i] = 0;
}

template <typename coefficient>
coefficient& ClassFunction<coefficient>::operator[](int i) const
{ return this->data[i];
}

template <typename coefficient>
std::string ClassFunction<coefficient>::ToString(FormatExpressions* theFormat) const
{ if (this->G==0)
    return "(not initialized)";
  std::stringstream out;
  out << "(";
  for(int i=0;i<this->data.size;i++){
    out << this->data[i];
    if(i<this->data.size-1)
      out << ", ";
  }
  out << ")[";
  out << this->norm();
  out << "]";
  return out.str();
}

template <typename coefficient>
std::ostream& operator<<(std::ostream& out, const ClassFunction<coefficient> X)
{ out << X.ToString();
  return out;
}

 //   static unsigned int HashFunction(const Character& input);
template <typename coefficient>
unsigned int ClassFunction<coefficient>::HashFunction(const ClassFunction<coefficient>& input)
{
  unsigned int acc;
  int N = (input.data.size < SomeRandomPrimesSize) ? input.data.size : SomeRandomPrimesSize;
  for(int i=0; i<N; i++){
    acc = input.data[i].HashFunction()*SomeRandomPrimes[i];
  }
  return acc;
}

// this should probably check if G is the same, but idk how to make that happen
template <typename coefficient>
bool ClassFunction<coefficient>::operator==(const ClassFunction<coefficient>& other)const
{ if(this->data == other.data)
    return true;
  return false;
}

template <typename coefficient>
bool ClassFunction<coefficient>::operator>(const ClassFunction<coefficient>& right) const
{ for(int i=0; i<this->data.size; i++)
    if(!(this->data[i] == right.data[i]))
      return this->data[i] > right.data[i];
  return false;
}

template <typename coefficient>
class TrixTree
{ public:
  Matrix<coefficient> M;
  // Would be nice to make this a pointer
  // and malloc() it to an appropriate size
  List<TrixTree<coefficient> > others;
  void reset()
  { this->others.SetSize(0);
  }
  Matrix<coefficient> GetElement(CoxeterElement &g, const List<Matrix<coefficient> > &gens);
};

// CoxeterRepresentation has all const operations because it is a lightweight wrapper
// of a list of matrices and a list of vectors
// well, not anymore :)
template <typename coefficient>
class CoxeterRepresentation
{
public:
  CoxeterGroup *G;
  Vectors<coefficient> basis;
  List<Matrix<coefficient> > gens;

  ClassFunction<coefficient> character;
  List<Matrix<coefficient> > classFunctionMatrices;
  TrixTree<coefficient> elements;

  CoxeterRepresentation():G(0){}
  void reset(CoxeterGroup* inputG=0)
  { this->G=inputG;
    this->basis.SetSize(0);
    this->gens.SetSize(0);
    this->character.G=this->G;
    this->character.MakeZero();
    this->classFunctionMatrices.SetSize(0);
    this->elements.reset();
  }
  CoxeterRepresentation<coefficient> operator*(const CoxeterRepresentation<coefficient>& other)const
  { CoxeterRepresentation<coefficient> output;
    this->MultiplyBy(other, output);
    return output;
  }
  void MultiplyBy
  (const CoxeterRepresentation<coefficient>& other, CoxeterRepresentation<coefficient>& output)const;
  void operator*=(const CoxeterRepresentation<coefficient>& other)
  { CoxeterRepresentation<coefficient> output;
    this->MultiplyBy(other, output);
    *this=output;
  }
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  static unsigned int HashFunction(const CoxeterRepresentation& input)
  { if (input.G==0)
      return 0;
    return SomeRandomPrimes[0]*input.G->HashFunction()+SomeRandomPrimes[1]*input.character.HashFunction();
  }
  bool operator==(const CoxeterRepresentation<coefficient>& other)const
  { return this->G==other.G && this->character==other.character;
  }
  Matrix<coefficient> MatrixOfElement(int g);
  ClassFunction<coefficient> GetCharacter();
  coefficient GetNumberOfComponents();
  bool CheckRepresentationConsistency();
  void ClassFunctionMatrix(ClassFunction<coefficient>& inputCF, Matrix<coefficient>& outputMat);
  List<CoxeterRepresentation<coefficient> > Decomposition();
  CoxeterRepresentation<coefficient> Reduced() const;
  VectorSpace<coefficient> FindDecentBasis() const;
  bool operator>(CoxeterRepresentation<coefficient>& right);
  std::string ToString(FormatExpressions* theFormat=0)const;
};


// Univariate dense polynomials.
template <typename coefficient>
class UDPolynomial
{
public:
   // "So the last shall be first, and the first last" -- Matthew 20:12
  List<coefficient> data;
  UDPolynomial(){}
  UDPolynomial(const UDPolynomial<coefficient>& other)
  { this->data=other.data;
  }
//  UDPolynomial<coefficient> operator+(const UDPolynomial<coefficient>& right) const;
  void operator+=(const UDPolynomial<coefficient>& right);
//  UDPolynomial<coefficient> operator-(const UDPolynomial<coefficient>& right) const;
  void operator-=(const UDPolynomial<coefficient>& right);
  UDPolynomial<coefficient> operator*(const UDPolynomial<coefficient>& right) const;
//  UDPolynomial<coefficient> operator*(const coefficient& right) const;
  void operator*=(const coefficient& right);
  void operator*=(const UDPolynomial<coefficient>& other)
  { *this=(*this)*other;
  }
  UDPolynomial<coefficient> TimesXn(int n) const;
// Quick divisibility test
// bool DivisibleBy(const UDPolynomial<coefficient>& divisor) const;
  struct DivisionResult<UDPolynomial<coefficient> > DivideBy(const UDPolynomial<coefficient>& right) const;
  UDPolynomial<coefficient> operator/(const UDPolynomial<coefficient>& divisor) const;
  UDPolynomial<coefficient> operator%(const UDPolynomial<coefficient>& divisor) const;
  void operator/=(const coefficient& right);
  void operator/=(const UDPolynomial<coefficient>& right)
  { *this=(*this/right);
  }
  coefficient operator()(const coefficient& x) const;
  void ClearDenominators();
  void FormalDerivative();
  void SquareFree();
  List<coefficient> GetRoots() const;
  void DoKronecker() const;
// static List<UDPolynomial<coefficient> > LagrangeInterpolants(List<coefficient> xs);
  coefficient& operator[](int i) const;
  bool operator<(const UDPolynomial<coefficient>& right) const;
  bool operator==(int other) const;
  std::string ToString(FormatExpressions* theFormat=0)const;
  void AssignMinPoly(const Matrix<coefficient>& input);
  void AssignCharPoly(const Matrix<coefficient>& input); // method due to Urbain Le Verrier
};

template <typename coefficient>
void UDPolynomial<coefficient>::AssignMinPoly(const Matrix<coefficient>& input)
{ int n = input.NumCols;
  this->data.SetSize(1);
  this->data[0] = 1;
  for(int col = 0; col < n; col++)
  { VectorSpace<coefficient> vs;
    Vector<coefficient> v,w;
    v.MakeEi(n,col);
    vs.AddVectorToBasis(v);
    for(int i=0; i<n; i++)
    { w = input*v;
      if(!vs.AddVectorToBasis(w))
        break;
      v = w;
    }
    Vector<coefficient> p = vs.basis.PutInBasis(w);
    UDPolynomial<coefficient> out;
    out.data.SetSize(p.size+1);
    for(int i=0; i<p.size; i++)
      out.data[i] = -p[i];
    out.data[p.size] = 1;
    *this = MathRoutines::lcm (*this, out);
  }
}

template <typename coefficient>
void UDPolynomial<coefficient>::AssignCharPoly(const Matrix<coefficient>& input)
{ int n = input.NumCols;
  this->data.SetSize(n+1);
  this->data[0] = 1;
  Matrix<coefficient> acc;
  acc = input;
  for(int i=1; i<n; i++)
  { this->data[i] = -acc.GetTrace()/i;
    for(int j=0; j<n; j++)
      acc.elements[j][j] += this->data[i];
    acc.MultiplyOnTheLeft(input);
  }
  this->data[n] = -acc.GetTrace()/n;
}

template <typename coefficient>
coefficient& UDPolynomial<coefficient>::operator[](int i) const
{ return data[i];
}

template <typename coefficient>
coefficient UDPolynomial<coefficient>::operator()(const coefficient &x) const
{  coefficient acc = 0;
   coefficient y = 1;
   for(int i=0; i<data.size; i++)
   {  acc += y*data[i];
      y *= x;
   }
   return acc;
}

template <typename coefficient>
void UDPolynomial<coefficient>::operator+=(const UDPolynomial<coefficient>& right)
{  int t = min(right.data.size, data.size);
   for(int i=0; i<t; i++)
      data[i] += right.data[i];

   if(right.data.size > data.size)
   {  int n = data.size;
      data.SetSize(right.data.size);
      for(int i=n; i<right.data.size; i++)
         data[i] = right.data[i];
   }
   else
      while((data.size != 0) and (data[data.size-1] != 0))
         data.size--;
}

template <typename coefficient>
void UDPolynomial<coefficient>::operator-=(const UDPolynomial<coefficient>& right)
{  // int t = min(right.data.size, data.size); // wtf lol
  int t = right.data.size;
  if(data.size < t)
    t = data.size;

  for(int i=0; i<t; i++)
    data[i] -= right.data[i];

  if(right.data.size > data.size)
  {  int n = data.size;
     data.SetSize(right.data.size);
     for(int i=n; i<right.data.size; i++)
       data[i] = -right.data[i];
  }
  else
    while((data.size != 0) and (data[data.size-1] == 0))
      data.size--;
}

template <typename coefficient>
UDPolynomial<coefficient> UDPolynomial<coefficient>::operator*(const UDPolynomial<coefficient>& right) const
{  UDPolynomial<coefficient> out;
   out.data.SetSize(data.size+right.data.size-1);
   for(int i=0; i<out.data.size; i++)
      out.data[i] = 0;
   for(int i=0; i<data.size; i++)
      for(int j=0; j<right.data.size; j++)
         out.data[i+j] += data[i]*right.data[j];
   return out;
}

template <typename coefficient>
UDPolynomial<coefficient> UDPolynomial<coefficient>::TimesXn(int n) const
{  UDPolynomial<coefficient> out;
   out.data.SetSize(data.size+n);
   for(int i=0; i<n; i++)
     out.data[i] = 0;
   // not memcpy()
   for(int i=0; i<data.size; i++)
      out.data[i+n] = data[i];

   return out;
}

template <typename coefficient>
void UDPolynomial<coefficient>::operator*=(const coefficient& right)
{  for(int i=0; i<data.size; i++)
      data[i] *= right;
}

template <class coefficient>
std::string UDPolynomial<coefficient>::ToString(FormatExpressions* theFormat)const
{ Polynomial<coefficient> tempP;
  tempP.MakeZero();
  MonomialP tempM;
  for (int i=0; i<this->data.size; i++)
  { tempM.MakeEi(0, i, 1);
    tempP.AddMonomial(tempM, this->data[i]);
  }
  return tempP.ToString(theFormat);
}

template <typename coefficient>
std::ostream& operator<<(std::ostream& out, const UDPolynomial<coefficient>& p)
{ FormatExpressions tempFormat;
  tempFormat.polyAlphabeT.SetSize(1);
  tempFormat.polyAlphabeT[0]="q";
  return out << p.ToString(&tempFormat);
}

#endif

