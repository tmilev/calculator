#ifndef vpfCharacterHeader
#define vpfCharacterHeader
#include "vpf.h"

static ProjectInformationInstance ProjectInfoVpfCharactersHEader
(__FILE__, "Header file, experimental code by Thomas: finite group characters sandbox. Not fully implemented yet.");

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

    ClassFunction(){}

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
    void operator=(const ClassFunction<coefficient>& X);
    std::string ToString(FormatExpressions* theFormat) const;
    std::string ToString() const;
    static unsigned int HashFunction(const ClassFunction<coefficient>& input);
    bool operator==(const ClassFunction<coefficient>& other) const;
    bool operator>(const ClassFunction<coefficient>& right) const;
};

template <typename coefficient>
std::ostream& operator<<(std::ostream& out, const ClassFunction<coefficient> X);

class CoxeterGroup
{ public:
  CoxeterGroup()
  { this->nGens=-1;
  }
  CoxeterGroup(const DynkinType& D)
  { this->MakeFrom(D);
  }
  void MakeFrom(const DynkinType& D);

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
    bool operator==(const CoxeterGroup& other) const
    { return this->CartanSymmetric==other.CartanSymmetric;
    }
    void operator=(const CoxeterGroup& other)
    { this->CartanSymmetric=other.CartanSymmetric;
      this->rootSystem=other.rootSystem;
      this->rho = other.rho;
      this->rhoOrbit=other.rhoOrbit;
      this->conjugacyClasses=other.conjugacyClasses;
      this->nGens=other.nGens;
      this->N=other.N;
      this->ccCount=other.ccCount;
      this->ccSizes=other.ccSizes;
      this->squares=other.squares;
      this->characterTable=other.characterTable;
//      std::cout << "operator=; rho=" << this->rho << "\n" << std::endl;
    }
    Vector<Rational> SimpleReflection(int i, const Vector<Rational> &right) const;
    HashedList<Vector<Rational> > GetOrbit(const Vector<Rational> &v) const;
    void ComputeRhoOrbit();
    Matrix<Rational> SimpleReflectionMatrix(int i) const;
    Matrix<Rational> TodorsVectorToInvMatrix(const Vector<Rational> &v) const;
    Matrix<Rational> TodorsVectorToMatrix(const Vector<Rational> &v) const;
    CoxeterElement GetCoxeterElement(int i) const;
    // note: the list is backwards

    List<int> DecomposeTodorsVector(const Vector<Rational> &v) const;
    Vector<Rational> ComposeTodorsVector(const List<int> &l) const;
    Vector<Rational> ApplyList(const List<int> &l, const Vector<Rational> &v) const;
    Vector<Rational> SimpleConjugation(int i, const Vector<Rational> &v) const;
    int MultiplyElements(int i, int j) const;
    int operator()(int i, int j) const;
    CoxeterRepresentation<Rational> StandardRepresentation();
    void ComputeConjugacyClasses();
    void ComputeSquares();
    void ComputeInitialCharacters();
    void ComputeIrreducibleRepresentations();
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
  void operator=(const CoxeterElement& other);
  bool operator==(const CoxeterElement& other) const;
  bool operator>(const CoxeterElement& other) const
  { if(this->owner!=other.owner)
    { std::cout << "This may or may not be a programming error: comparing elements of different coxeter groups. "
      << "Crashing to let you know. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
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
//This will be incorrect if it's ever extended to a complex type
template<typename coefficient>
coefficient ClassFunction<coefficient>::IP(const ClassFunction<coefficient> &other) const{
    coefficient acc = 0;
    for(int i=0;i<G->ccCount;i++)
        acc +=  this->data[i] * other[i] * G->ccSizes[i];
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
{ //if (this->G==0)
  //  return "(not initialized)";
  // Check disabled because it shouldn't happen and doesn't work
  std::stringstream out;
  out << "(";
  for(int i=0;i<data.size;i++){
    out << data[i];
    if(i<data.size-1)
      out << ", ";
  }
  out << ")[";
  out << norm();
  out << "]";
  return out.str();
}

template <typename coefficient>
std::string ClassFunction<coefficient>::ToString() const
{ return ToString(0);
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

template <typename coefficient>
void ClassFunction<coefficient>::operator=(const ClassFunction<coefficient>& X)
{ this->G = X.G;
  this->data = X.data;
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
class Basis
{
  public:
  Matrix<coefficient> basis;
  Matrix<coefficient> gramMatrix;
  bool haveGramMatrix;

  void AddVector(const Vector<coefficient>& v);
  void ComputeGramMatrix();
  Vector<coefficient> PutInBasis(const Vector<coefficient>& v);
  Matrix<coefficient> PutInBasis(const Matrix<coefficient>& M);
};

template <typename coefficient>
class VectorSpace
{
public:
   int degree;
   int rank;
   Matrix<coefficient> fastbasis;
   Basis<coefficient> basis;

   VectorSpace(){degree=-1;rank=0;}
   void MakeFullRank(int dim);
   // true if it wasn't already there
   bool AddVector(const Vector<coefficient> &v);
   bool AddVectorDestructively(Vector<coefficient> &v);
   bool AddVectorToBasis(const Vector<coefficient> &v);
   bool Contains(const Vector<coefficient>& v) const;
   VectorSpace<coefficient> Intersection(const VectorSpace<coefficient>& other) const;
   VectorSpace<coefficient> Union(const VectorSpace<coefficient>& other) const;
   VectorSpace<coefficient> OrthogonalComplement() const;
};


template <typename coefficient>
class TrixTree
{ public:
  Matrix<coefficient> M;
  // Would be nice to make this a pointer
  // and malloc() it to an appropriate size
  List<TrixTree<coefficient> > others;

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
   List<Vector<coefficient> > basis;
   List<Matrix<coefficient> > gens;

   ClassFunction<coefficient> character;
   List<Matrix<coefficient> > classFunctionMatrices;
   TrixTree<coefficient> elements;

   CoxeterRepresentation() {};

   CoxeterRepresentation<coefficient> operator*(const CoxeterRepresentation<coefficient>& other) const;

   ClassFunction<coefficient> GetCharacter();
   coefficient GetNumberOfComponents();
   Matrix<coefficient> ClassFunctionMatrix(ClassFunction<coefficient> cf);
   List<CoxeterRepresentation<coefficient> > Decomposition(List<ClassFunction<coefficient> >& ct, List<CoxeterRepresentation<coefficient> >& gr);
   CoxeterRepresentation<coefficient> Reduced() const;
   VectorSpace<coefficient> FindDecentBasis() const;
   bool operator>(CoxeterRepresentation<coefficient>& right);
};


#endif
