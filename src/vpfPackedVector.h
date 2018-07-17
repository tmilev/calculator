#include "vpfJson.h"
#ifndef vpfPackedVector_h_already_included
#define vpfPackedVector_h_already_included

static ProjectInformationInstance vpfPackedVector(__FILE__, "So soon, and the filename is already inaccurate.  Has some packed vector code and code using packed vector code.");


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
  for (int i = 0; i < this->size; i ++)
    out[i] = this->data[i] + w[i];
  return out;
}

template <typename scalar>
void PackedVector<scalar>::operator+=(const PackedVector<scalar>& w)
{ for (int i = 0; i < this->size; i ++)
    this->data[i] += w[i];
}

template <typename scalar>
PackedVector<scalar> PackedVector<scalar>::operator*(scalar x) const
{ PackedVector<scalar> out;
  for (int i = 0; i < this->size; i ++)
    out[i] = this->data[i] * x;
  return out;
}

template <typename scalar>
scalar PackedVector<scalar>::ScalarProduct(const PackedVector<scalar>& v, const Matrix<scalar>& B) const
{ PackedVector<scalar> Bv;
  Bv.MakeZero(B.NumRows);
  for (int i = 0; i < B.NumRows; i ++)
    for (int j = 0; j < B.NumCols; j ++)
      Bv[i] += B.elements[i][j] * v[j];
  scalar wBv = 0;
  for (int i = 0; i < B.NumRows; i ++)
    wBv += this->data[i] * Bv[i];
  return wBv;
}

/*
template <typename scalar>
scalar PackedVector<scalar>::ScalarProduct(const PackedVector<scalar>& v, const PackedVector* B) const
{ PackedVector<scalar> Bv;
  Bv.MakeZero();
  for (int i = 0; i < this->size; i ++)
    for (int j = 0; j < this->size; j ++)
      Bv[i] += B[i][j]*v[j]
  scalar wBv = 0;
  for (int i = 0; i <B.NumRows; i ++)
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
{ if (s > this->size)
  { std::cout << "if this was intentional, recompile PackedVector with size>=" << s << std::endl;
    assert(false);
  }
}

template <typename scalar>
bool PackedVector<scalar>::operator!=(const PackedVector<scalar>& w) const
{ for (int i = 0; i < this->size; i ++)
    if (w[i] != this->data[i])
      return true;
  return false;
}

template <typename scalar>
bool PackedVector<scalar>::operator==(const PackedVector<scalar>& w) const
{ return !(*this != w);
}

template <typename scalar>
bool PackedVector<scalar>::operator>(const PackedVector<scalar>&w) const
{ for (int i = 0; i < this->size; i ++)
  { if (this->data[i] > w[i])
      return true;
    if (this->data[i] < w[i])
      return false;
  }
  return false;
}


template <typename scalar>
void PackedVector<scalar>::MakeZero(int n)
{ for (int i = 0; i < this->size; i ++)
    this->data[i] = 0;
}

template <typename scalar>
void PackedVector<scalar>::MakeEi(int d, int ei)
{ for (int i = 0; i < this->size; i ++)
    this->data[i] = 0;
  this->data[ei] = 1;
}

template <typename scalar>
unsigned int PackedVector<scalar>::HashFunction(const PackedVector<scalar>& in)
{ return in.HashFunction();
}

template <typename scalar>
unsigned int PackedVector<scalar>::HashFunction() const
{ unsigned int result = 0;
  for (int i = 0; i < this->size; i ++)
    result += (unsigned int) (this->data[i] * SomeRandomPrimes[i]);
  return result;
}

template <typename scalar>
std::ostream& operator<<(std::ostream& out, const PackedVector<scalar>& v)
{ out << '(';
  for (int i = 0; i < v.size; i ++)
  { out << v[i];
    if (i != v.size - 1)
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

  void MakeArbitrarySimple(char letter, int number);
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
void AnotherWeylGroup<scalar, vector>::MakeArbitrarySimple(char letter, int number)
{ DynkinType D;
  D.MakeSimpleType(letter, number);
  D.GetCartanSymmetric(this->CartanSymmetric);
}

template <typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::SimpleReflection(int i, vector& v) const
{ // next i'll put some printfs in addition
  //std::cout << "Debugging simple reflections: vector " << v << " transformed by reflection " << i;
  scalar x = 0;
  for (int j = 0; j < this->rank; j ++)
    x += this->unrationalCartanSymmetric.elements[i][j] * v[j];
  // so, under ordinary circumstances, this is just out[i] -= x;
  // silent corruption occurs if UnrationalCartanSymmetric.elements[i][i] !∈ {1,2}
  // and scalar can't deal with division properly
  // fortunately this only happens in G₂
  v[i] -= x * 2 / unrationalCartanSymmetric.elements[i][i];
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
  while (w != this->twiceRho)
  { for (int i = 0; i < this->rank; i ++)
    { scalar x = 0;
      for (int j = 0; j < this->rank; j ++)
        if (this->unrationalCartanSymmetric.elements[i][j] != 0)
          x += this->unrationalCartanSymmetric.elements[i][j] * w[j];
      if (x > 0)
        continue;
      out.AddOnTop(i);
      SimpleReflection(i, w);
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
{ for (int ii = simpleReflections.size - 1; ii >= 0; ii --)
  { //this->SimpleReflection(simpleReflections[i],v);
    int i = simpleReflections[ii];
    scalar x = 0;
    for (int j = 0; j < this->rank; j ++)
      x += this->unrationalCartanSymmetric.elements[i][j] * v[j];
    // so, under ordinary circumstances, this is just out[i] -= x;
    // silent corruption occurs if UnrationalCartanSymmetric.elements[i][i] !∈ {1,2}
    // and scalar can't deal with division properly
    // fortunately this only happens in G₂
    v[i] -= x * 2 / unrationalCartanSymmetric.elements[i][i];
  }
}

template <typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::ActOn(int i, vector& v) const
{ List<int> simpleReflections;
  GetSimpleReflections(this->rhoOrbit[i], simpleReflections);
  this->ApplyReflectionList(simpleReflections, v);
}

template <typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::ActOn(int i, const vector& v, vector& out) const
{ out = v;
  this->ActOn(i, out);
}

template <typename scalar, typename vector>
int AnotherWeylGroup<scalar, vector>::Multiply(int i, int j) const
{ vector v = this->rhoOrbit[j];
  this->ActOn(i, v);
  return this->rhoOrbit.GetIndex(v);
}

template <typename scalar, typename vector>
int AnotherWeylGroup<scalar, vector>::Invert(int i) const
{ List<int> srsl;
  GetSimpleReflections(this->rhoOrbit[i], srsl);
  vector v = twiceRho;
  for (int i = srsl.size - 1; i >= 0; i --)
    this->SimpleReflection(srsl[i], v);
  return this->rhoOrbit.GetIndex(v);
}

template <typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::ComputeRho()
{ if (unrationalCartanSymmetric.NumRows < CartanSymmetric.NumRows)
  { int den = 1;
    for (int i = 0; i < CartanSymmetric.NumRows; i ++)
      for (int j = 0; j < CartanSymmetric.NumCols; j ++)
        den = MathRoutines::lcm(den,CartanSymmetric.elements[i][j].GetDenominator().GetUnsignedIntValueTruncated());
    unrationalCartanSymmetric.init(CartanSymmetric.NumRows, CartanSymmetric.NumCols);
    //this->ucsm = malloc(CartanSymmetric.NumRows*sizeof(vector));
    //for (int i = 0; i <CartanSymmetric.NumRows; i ++)
    //  this->ucsm[i].SetSize(CartanSymmetric.NumCols);
    for (int i = 0; i < CartanSymmetric.NumRows; i ++)
      for (int j = 0; j < CartanSymmetric.NumCols; j ++)
      { unrationalCartanSymmetric.elements[i][j] = (CartanSymmetric.elements[i][j] * den).GetNumerator().GetIntValueTruncated();
        //ucsm[i][j] = unrationalCartanSymmetric.elements[i][j];
      }

  }
  std::cout << "symmetric Cartan matrix is" << std::endl;
  for (int i = 0; i < unrationalCartanSymmetric.NumRows; i ++)
  { for (int j = 0; j < unrationalCartanSymmetric.NumCols; j ++)
      std::cout << unrationalCartanSymmetric.elements[i][j] << '\t';
    std::cout << '\n';
  }
  this->rank = this->unrationalCartanSymmetric.NumRows;
  for (int rvi = 0; rvi < rank; rvi ++)
  { vector vi;
    vi.MakeEi(rank, rvi);
    if (this->RootSystem.Contains(vi))
      continue;
    this->RootSystem.AddOnTop(vi);
    List<int> newelts;
    vector w;
    newelts.AddOnTop(this->RootSystem.GetIndex(vi));
    while (newelts.size > 0)
    { int i = newelts.PopLastObject();
      for (int si = 0; si < this->rank; si ++)
      { this->SimpleReflection(si, this->RootSystem[i], w);
        if (this->RootSystem.GetIndex(w) == - 1)
        { this->RootSystem.AddOnTop(w);
          int j = this->RootSystem.GetIndex(w);
          newelts.AddOnTop(j);
        }
      }
    }
  }
  std::cout << "root system is" << std::endl;
  std::cout << this->RootSystem << std::endl;
  this->RootSystem.QuickSortAscending();
  this->twiceRho.MakeZero(rank);
  for (int i = 0; i < this->RootSystem.size; i ++)
  { bool usethis = true;
    for (int j = 0; j < this->RootSystem[i].size; j ++)
      if (this->RootSystem[i][j] < 0)
      { usethis = false;
        break;
      }
    if (usethis)
      this->twiceRho += this->RootSystem[i];
  }
  this->rho.SetSize(this->twiceRho.size);
  for (int i = 0; i < this->twiceRho.size; i ++)
    this->rho[i] = this->twiceRho[i] / 2;
  std::cout << "half sum of positive roots is " << this->rho << std::endl;
}

template <typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::ComputeAllElements()
{ std::cout << "Getting elements...";
  if (this->RootSystem.size == 0)
    this->ComputeRho();
  std::cout << "(twiceRho is " << this->twiceRho << ")" << std::endl;
  vector w;
  List<int> newelts;
  this->rhoOrbit.AddOnTop(twiceRho);
  newelts.AddOnTop(this->rhoOrbit.GetIndex(twiceRho));
  while (newelts.size > 0)
  { int i = newelts.PopLastObject();
    for (int si = 0; si < this->rank; si ++)
    { this->SimpleReflection(si,this->rhoOrbit[i],w);
      if (this->rhoOrbit.GetIndex(w) == - 1)
      { this->rhoOrbit.AddOnTop(w);
        int j = this->rhoOrbit.GetIndex(w);
        newelts.AddOnTop(j);
      }
    }
  }
  this->size = this->rhoOrbit.size;
  std::cout << this->size << std::endl;
  //std::cout << rhoOrbit << std::endl;
}

template <typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::ComputeConjugacyClasses()
{ std::cout << "Getting conjugacy classes...";
  if (this->rhoOrbit.size == 0)
    this->ComputeAllElements();
  List<bool> Accounted;
  Accounted.initFillInObject(this->size, false);
  this->conjugacyClasses.SetSize(0);
  this->conjugacyClasses.ReservE(50);
  HashedList<int, MathRoutines::IntUnsignIdentity> theStack;
  theStack.SetExpectedSize(this->size);
  int theRank = this->GetRank();
  vector theRhoImage;
  std::cout << "number of conjugacy classes... ";
  for (int i = 0; i < this->size; i ++)
    if (!Accounted[i])
    { theStack.Clear();
      theStack.AddOnTop(i);
      for (int j = 0; j < theStack.size; j ++)
        for (int k = 0; k < theRank; k ++)
        { theRhoImage = this->twiceRho;
          this->SimpleReflection(k, theRhoImage);
          this->ActOn(theStack[j], theRhoImage);
          this->SimpleReflection(k, theRhoImage);
          int accountedIndex = this->rhoOrbit.GetIndex(theRhoImage);
          theStack.AddOnTopNoRepetition(accountedIndex);
          Accounted[accountedIndex] = true;
        }
      this->conjugacyClasses.AddOnTop(theStack);
      this->conjugacyClasses.LastObject()->QuickSortAscending();
      std::cout << this->conjugacyClasses.size << " ";
    }
  this->conjugacyClasses.QuickSortAscending();
  std::cout << conjugacyClasses.size << std::endl;
}

template<typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::ComputeClassMap()
{ this->classMap.SetSize(this->size);
  for (int i = 0; i < this->conjugacyClasses.size; i ++)
    for (int j = 0; j < this->conjugacyClasses[i].size; j ++)
      this->classmap[this->conjugacyClasses[i][j]] = i;
}

template<typename scalar, typename vector>
Matrix<Rational>& AnotherWeylGroup<scalar, vector>::GetClassMatrix(int cc)
{ if (this->classMatrices.size == 0)
  { if (this->conjugacyClasses.size == 0)
      this.ComputeConjugacyClasses();
    this->classMatrices.SetSize(this->conjugacyClasses.size);
  }
  if (this->classMap.size == 0)
    this->ComputeClassMap();
  this->classMatrices[cc] = GetClassMatrix(*this, cc, &(this->classmap));
  return this->classMatrices[cc];
}

template<typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::ComputeClassMatrices()
{ if (this->conjugacyClasses.size == 0)
    this->ComputeConjugacyClasses();
  for (int i = 0; i < this->conjugacyClasses.size; i ++)
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
  x *= - 2;
  x /= this->RootSystem[i].ScalarProduct(this->RootSystem[i],this->unrationalCartanSymmetric);
  return this->rhoOrbit.GetIndexIMustContainTheObject(twiceRho + this->RootSystem[i] * x);
}

template <typename scalar, typename vector>
void AnotherWeylGroup<scalar, vector>::GetSignCharacter(Vector<Rational>& out)
{ if (this->conjugacyClasses.size == 0)
    this->ComputeConjugacyClasses();
  out.SetSize(this->conjugacyClasses.size);
  for (int i = 0; i < this->conjugacyClasses.size; i ++)
  { List<int> srs;
    this->GetGeneratorList(this->conjugacyClasses[i][0], srs);
    int yn = srs.size % 2;
    if (yn == 0)
      out[i] = 1;
    else
      out[i] = - 1;
  }
}


// there are like 3 copies of this function with slightly different argument types
template <typename scalar, typename vector>
Rational AnotherWeylGroup<scalar, vector>::GetHermitianProduct(const Vector<Rational>& X1, const Vector<Rational>& X2) const
{ Rational acc = 0;
  for (int i = 0; i < X1.size; i ++)
    acc += X1[i].GetComplexConjugate() * X2[i] * this->conjugacyClasses[i].size;
  return acc / this->size;
}

template <typename coefficient>
List<VectorSpace<coefficient> > GetEigenspaces(const Matrix<coefficient>& M)
{ List<List<Vector<coefficient> > > es = eigenspaces(M);
  List<VectorSpace<coefficient> > vs;
  for (int spi = 0; spi < es.size; spi ++)
  { VectorSpace<coefficient> V;
    for (int j = 0; j < es[spi].size; j ++)
      V.AddVector(es[spi][j]);
    vs.AddOnTop(V);
  }
  return vs;
}

// As in Schneider, 1990
// well, so far not using any of his improvements
template <typename somegroup>
List<Vector<Rational> > ComputeCharacterTable(somegroup &G)
{ if (G.conjugacyClasses.size == 0)
    G.ComputeConjugacyClasses();
  List<int> classmap;
  classmap.SetSize(G.size);
//  classmap.SetSize(G.theElements.size);
  for (int i = 0; i < G.conjugacyClasses.size; i ++)
    for (int j = 0; j < G.conjugacyClasses[i].size; j ++)
      classmap[G.conjugacyClasses[i][j]] = i;
  Matrix<Rational> form; // so inefficient
  form.MakeZeroMatrix(G.conjugacyClasses.size);
  for (int i = 0; i < G.conjugacyClasses.size; i ++)
    form.elements[i][i] = G.conjugacyClasses[i].size;
  List<VectorSpace<Rational> > spaces;
  if (G.characterTable.size > 0)
  { VectorSpace<Rational> allchars;
    for (int i = 0; i < G.characterTable.size; i ++)
    { VectorSpace<Rational> xspi;
      xspi.AddVector(G.characterTable[i]);
      spaces.AddOnTop(xspi);
      allchars.AddVector(G.characterTable[i]);
    }
    spaces.AddOnTop(allchars.OrthogonalComplement(0,&form));
  } else
  { Vector<Rational> X1;
    X1.SetSize(G.conjugacyClasses.size);
    for (int i = 0; i < G.conjugacyClasses.size; i ++)
      X1[i] = 1;
    VectorSpace<Rational> sp1;
    sp1.AddVector(X1);
    spaces.AddOnTop(sp1);
    spaces.AddOnTop(sp1.OrthogonalComplement(0,&form));
  }
  bool found = false;
  for (int i = 0; i < G.conjugacyClasses.size; i ++)
  { Matrix<Rational> M;
    std::cout << "Getting class matrix " << i << std::endl;
    M = GetClassMatrix(G,i,&classmap);
    //std::cout << M.ToString(&consoleFormat) << std::endl;
    List<VectorSpace<Rational> > es = GetEigenspaces(M);
    for (int esi = 0; esi < es.size; esi ++)
    { int spsize = spaces.size;
      for (int spi = 0; spi < spsize; spi ++)
      { if (spaces[spi].rank == 1)
          continue;
        VectorSpace<Rational> V = spaces[spi].Intersection(es[esi]);
        if ((V.rank > 0) and (V.rank < spaces[spi].rank))
        { VectorSpace<Rational> W = es[esi].OrthogonalComplement(&spaces[spi], &form);
          spaces[spi] = V;
          spaces.AddOnTop(W);
          if (spaces.size == G.conjugacyClasses.size)
          { found = true;
            break;
          }
        }
      }
      if (found)
        break;
    }
    if (found)
      break;
    int nchars = 0;
    for (int i = 0; i < spaces.size; i ++)
      if (spaces[i].rank == 1)
        nchars += 1;
    std::cout << "Have " << nchars << " chars" << std::endl;
  }
  List<Vector<Rational> > chars;
  chars.SetSize(spaces.size);
  for (int i = 0; i < spaces.size; i ++)
    chars[i] = spaces[i].GetCanonicalBasisVector(0);
  /*  HashedList<VectorSpace<Rational> > sps;
    List<Vector<Rational> > chars;
    for (int i = 0; i <G.conjugacyClasses.size; i ++)
    { Matrix<Rational> M;
      M = GetClassMatrix(G,i);
      std::cout << M.ToString(&consoleFormat) << std::endl;
      List<VectorSpace<Rational> > spsi = GetEigenspaces(M);
      for (int spi = 0; spi <spsi.size; spi ++)
        sps.AddOnTop(spsi[spi]);
    }
    for (int i = 0; i <sps.size; i ++)
      if (sps[i].rank == 1){
        Vector<Rational> X = sps[i].GetCanonicalBasisVector(0);
        if (!chars.Contains(X))
        { chars.AddOnTop(sps[i].GetCanonicalBasisVector(0));
          if (chars.size == G.conjugacyClasses.size)
            goto got_chars;
        }
      }
    std::cout << chars << std::endl;
    { // to scope newspaces so that the compiler doesn't complain about the goto
      // skipping its initialization
    List<VectorSpace<Rational> > newspaces = sps;
    while (newspaces.size > 0)
    { VectorSpace<Rational> sp = newspaces.PopLastObject();
      for (int i = 0; i <sps.size; i ++)
      { VectorSpace<Rational> sp2 = sp.Intersection(sps[i]);
        if (sps.GetIndex(sp2) == - 1)
        { sps.AddOnTop(sp2);
          newspaces.AddOnTop(sp2);
          if (sp2.rank == 1)
          { Vector<Rational> X = sp2.GetCanonicalBasisVector(0);
            if (!chars.Contains(X))
            { chars.AddOnTop(X);
              std::cout << chars.size << ' ';
              if (chars.size == G.conjugacyClasses.size)
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
  //std::cout << chars << std::endl;
  for (int i = 0; i < chars.size; i ++)
  { Rational x = G.GetHermitianProduct(chars[i], chars[i]);
    int x2 = x.GetDenominator().GetUnsignedIntValueTruncated();
    x2 = sqrt(x2);
    chars[i] *= x2;
    if (chars[i][0] < 0)
      chars[i] *= - 1;
    std::cout << x2 << std::endl;
  }

  chars.QuickSortAscending(/*&CharacterComparator*/);
  for (int i = 0; i < chars.size; i ++)
    for (int j = i; j < chars.size; j ++)
      if (chars[i] > chars[j])
        std::cout << "error: " << i << j << std::endl;
  for (int i = 0; i < chars.size; i ++)
    std::cout << chars[i] << std::endl;
  G.characterTable = chars;
  for (int i = 0; i < G.characterTable.size; i ++)
    std::cout << G.characterTable[i] << std::endl;
  return chars;
}

template <typename somegroup>
Matrix<Rational> GetClassMatrix(const somegroup &G, int cci, List<int>* classmap = 0)
{ List<int> invl;
  invl.SetSize(G.conjugacyClasses[cci].size);
  for (int i = 0; i < G.conjugacyClasses[cci].size; i ++)
    invl[i] = G.Invert(G.conjugacyClasses[cci][i]);
  Matrix<int> M;
  M.MakeZeroMatrix(G.conjugacyClasses.size);
  for (int t = 0; t < G.conjugacyClasses.size; t ++)
    for (int xi = 0; xi < invl.size; xi ++)
    { int yi = G.Multiply(invl[xi], G.conjugacyClasses[t][0]);
      int ci;
      if (classmap)
        M.elements[t][(*classmap)[yi]] += 1;
      else
        for (ci = 0; ci < G.conjugacyClasses.size; ci ++)
          if (G.conjugacyClasses[ci].BSContains(yi))
          { M.elements[t][ci] += 1;
            break;
          }
    }
  Matrix<Rational> out;
  out.init(M.NumRows, M.NumCols);
  for (int i = 0; i < M.NumRows; i ++)
    for (int j = 0; j < M.NumCols; j ++)
      out.elements[i][j] = M.elements[i][j];
  return out;
}

template <typename somegroup>
bool Subgroup<somegroup>::MakeFrom(somegroup &G, const List<int>& generators, int MaxElements)
{ // bookkeeping...
  // this class does things in such a bizarre way
  this->parent = &G;
  List<int> newElements;
  for (int i = 0; i < generators.size; i ++)
  { this->theElements.AddOnTop(generators[i]);
    this->lengths.AddOnTop(1);
    int g = this->theElements.GetIndex(generators[i]);
    this->generators.AddOnTop(g);
    newElements.AddOnTop(g);
  }

  // okay here is where the copy of dr milev's code is, for the 58th time
  while (newElements.size > 0)
  { int gi = newElements.PopLastObject();
    int g = this->theElements[gi];
    for (int i = 0; i < this->generators.size; i ++)
    { int h = this->parent->Multiply(this->theElements[this->generators[i]], g);
      // need a way of storing a sorted list...
      if (this->theElements.GetIndex(h) == - 1)
      { this->theElements.AddOnTop(h);
        this->lengths.AddOnTop(this->lengths[gi] + 1);
        if (MaxElements >= 0 && this->theElements.size > MaxElements)
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
  for (int i = 0; i < this->generators.size; i ++)
    inverseGenerators[i] = this->theElements.GetIndex(this->parent->Invert(this->theElements[this->generators[i]]));
  for (int i = 0; i < this->theElements.size; i ++)
    if (!Accounted[i])
    { theStack.Clear();
      theStack.AddOnTop(i);
      for (int j = 0; j < theStack.size; j ++)
        for (int k = 0; k < this->generators.size; k ++)
        { int g = this->parent->Multiply(this->parent->Multiply(this->theElements[inverseGenerators[k]], this->theElements[theStack[j]]), this->theElements[this->generators[k]]);
          int accountedIndex = this->theElements.GetIndex(g);
          theStack.AddOnTopNoRepetition(accountedIndex);
          Accounted[accountedIndex] = true;
        }
      this->conjugacyClasses.AddOnTop(theStack);
      this->conjugacyClasses.LastObject()->QuickSortAscending();
    }
  this->conjugacyClasses.QuickSortAscending();
}

template <typename somegroup>
void Subgroup<somegroup>::GetSignCharacter(Vector<Rational>& Xs)
{ if (this->conjugacyClasses.size == 0)
    this->ComputeConjugacyClasses();
  Xs.SetSize(this->conjugacyClasses.size);
  for (int i = 0; i < Xs.size; i ++)
  { int yn = this->lengths[conjugacyClasses[i][0]] % 2;
    if (yn == 0)
      Xs[i] = 1;
    else
      Xs[i] = - 1;
  }
}

template <typename somegroup>
Rational Subgroup<somegroup>::GetHermitianProduct(const Vector<Rational>& X1, const Vector<Rational>& X2) const
{ Rational acc = 0;
  for (int i = 0; i < X1.size; i ++)
    acc += X1[i].GetComplexConjugate() * X2[i] * this->conjugacyClasses[i].size;
  return acc / this->theElements.size;
}

template <typename weylgroup>
void GetTauSignaturesFromSubgroup(weylgroup& G, const List<int>& gens, List<bool>& out)
{ /*List<ElementWeylGroup> genes;
  genes.SetSize(gens.size);
  for (int i = 0; i <gens.size; i ++)
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
  for (int i = 0; i < H.conjugacyClasses.size; i ++)
  { int gi = H.theElements[H.conjugacyClasses[i][0]];
    for (int ci = 0; ci < G.conjugacyClasses.size; ci ++)
      for (int cj = 0; cj < G.conjugacyClasses[ci].size; cj ++)
        if (G.conjugacyClasses[ci][cj] == gi)
        { ccPreimages[i] = ci;
          cj = G.conjugacyClasses[ci].size; //to break from the loops
          ci = G.conjugacyClasses.size; // to break from the loops
        }
  }

  out.SetSize(G.characterTable.size);
  Vector<Rational> HXi;
  HXi.SetSize(H.conjugacyClasses.size);
  for (int i = 0; i < G.characterTable.size; i ++)
  { Vector<Rational> GXi = G.characterTable[i];
    for (int j = 0; j < HXi.size; j ++)
      HXi[j] = GXi[ccPreimages[j]];
    if (H.GetHermitianProduct(HXs, HXi) == 0)
      out[i] = false;
    else
      out[i] = true;
  }
}


template <typename weylgroup>
void ComputeTauSignatures(weylgroup* G, List<List<bool> >& tauSignatures, bool pseudo = false)
{ Selection sel;
  sel.init(G->CartanSymmetric.NumCols);
  int numCycles = MathRoutines::TwoToTheNth(sel.MaxSize);
  List<List<bool> > tss;
  tss.SetSize(numCycles - 2);
  for (int i = 0; i < numCycles - 2; i ++)
  { sel.incrementSelection();
    List<int> gens;
    for (int j = 0; j < sel.CardinalitySelection; j ++)
      gens.AddOnTop(sel.elements[j] + 1);
    GetTauSignaturesFromSubgroup(*G, gens, tss[i]);
  }


  Vector<Rational> Xs;
  G->GetSignCharacter(Xs);
  List<bool> tsg;
  tsg.SetSize(G->characterTable.size);
  for (int i = 0; i < G->characterTable.size; i ++)
    tsg[i] = G->characterTable[i] == Xs;
  tss.AddOnTop(tsg);

  if (pseudo)
  { std::cout << "pseudo-parabolics" << std::endl;
    int hr = G->GetRootReflection(G->RootSystem.size- 1);
    sel.init(G->CartanSymmetric.NumCols);
    for (int i = 0; i < numCycles - 1; i ++)
    { List<int> gens;
      for (int j = 0; j < sel.CardinalitySelection; j ++)
        gens.AddOnTop(sel.elements[j] + 1);
      gens.AddOnTop(hr);
      List<bool> ts;
      GetTauSignaturesFromSubgroup(*G, gens, ts);
      tss.AddOnTop(ts);
      sel.incrementSelection();
    }
  }

  // we will need the sign character for the group


  tauSignatures.SetSize(G->characterTable.size);
  for (int i = 0; i <G->characterTable.size; i ++)
  { tauSignatures[i].SetSize(tss.size + 1);
    tauSignatures[i][0] = 1;
    for (int j =1; j < tss.size + 1; j ++)
      tauSignatures[i][j] = tss[j- 1][i];
  }

  for (int i = 0; i < G->characterTable.size; i ++)
  { std::cout << G->characterTable[i] << std::endl;
    for (int j = 0; j < tauSignatures[i].size; j ++)
      std::cout << tauSignatures[i][j] << ' ';
    std::cout << std::endl;
  }
}

template <typename somegroup>
void ExportCharTable(const somegroup& G, JSData &data)
{ data.type = JSOBJ;
  data.obj.SetSize(3);
  data.obj[0].key = "representatives";
  data.obj[1].key = "sizes";
  data.obj[2].key = "characters";

  data.obj[0].value.type = JSLIST;
  data.obj[0].value.list.SetSize(G.conjugacyClasses.size);
  for (int i = 0; i < G.conjugacyClasses.size; i ++)
  { List<int> reprefs;
    G.GetGeneratorList(G.conjugacyClasses[i][0],reprefs);
    data.obj[0].value.list[i].type = JSLIST;
    data.obj[0].value.list[i].list.SetSize(reprefs.size);
    for (int j = 0; j < reprefs.size; j ++)
    { data.obj[0].value.list[i].list[j].type = JSNUM;
      data.obj[0].value.list[i].list[j].number = reprefs[j];
    }
  }

  data.obj[1].value.type = JSLIST;
  data.obj[1].value.list.SetSize(G.conjugacyClasses.size);
  for (int i = 0; i < G.conjugacyClasses.size; i ++)
  { data.obj[1].value.list[i].type = JSNUM;
    data.obj[1].value.list[i].number = G.conjugacyClasses[i].size;
  }

  data.obj[2].value.type = JSLIST;
  data.obj[2].value.list.SetSize(G.characterTable.size);
  for (int i = 0; i < G.characterTable.size; i ++)
    for (int j = 0; j < G.characterTable[i].size; j ++)
      data["characters"][i][j] = G.characterTable[i][j].DoubleValue();
}

template <typename somegroup>
void ExportTauSignatures(const somegroup& G, const List<List<bool> > ts, JSData &data)
{ ExportCharTable(G, data["chartable"]);
  for (int i = 0; i < ts.size; i ++)
    for (int j = 0; j < ts[i].size; j ++)
      data["tausigs"][i][j] = ts[i][j];
}

void LoadAndPrintTauSignatures(char letter, int number)
{ std::stringstream s;
  s << letter << number << "signsigs";
  JSData data;
  data.readfile(s.str().c_str());
  if (data.type == JSNULL)
  { AnotherWeylGroup<int,PackedVector<int> > G;
    G.MakeArbitrarySimple(letter, number);
    ComputeCharacterTable(G);
    List<List<bool> > ts;
    ComputeTauSignatures(&G, ts, true);
    ExportTauSignatures(G, ts, data);
    data.writefile(s.str().c_str());
  }

  // lets print out the character table
  std::cout << "<table>";

  std::cout << "<tr><td></td>";
  for (int i = 0; i < data["chartable"]["representatives"].list.size; i ++)
    std::cout << "<td>" << data["chartable"]["representatives"][i] << "</td>";
  std::cout << "</tr>" << std::endl;

  std::cout << "<tr><td></td>";
  for (int i = 0; i < data["chartable"]["sizes"].list.size; i ++)
    std::cout << "<td>" << data["chartable"]["sizes"][i] << "</td>";
  std::cout << "</tr>";

  for (int i = 0; i < data["chartable"]["characters"].list.size; i ++)
  { std::cout << "<tr><td>" << i << "</td>";
    for (int j = 0; j < data["chartable"]["characters"][i].list.size; j ++)
      std::cout << "<td>" << data["chartable"]["characters"][i][j] << "</td>";
    std::cout << "</tr>";
  }
  std::cout << "</table>" << std::endl;

// okay now the regular tau signatures
  Selection sel;
  sel.init(number);
  int numCycles = MathRoutines::TwoToTheNth(sel.MaxSize);
  std::cout << "<table>";
  std::cout << "<tr>";
  for (int i = 0; i < numCycles; i ++)
  { std::cout << "<td>";
    for (int j = 0; j < sel.CardinalitySelection; j ++)
    { std::cout << sel.elements[j];
      if (j + 1 < sel.elements.size)
        std::cout << ' ';
    }
    std::cout << "</td>";
    sel.incrementSelection();
  }
  std::cout << "</tr>";

  for (int i = 0; i < data["tausigs"].list.size; i ++)
  { std::cout << "<tr>";
    for (int j = 0; j < numCycles; j ++)
    { std::cout << "<td>";
      std::cout << data["tausigs"][i][j] << "</td>";
    }
    std::cout << "</tr>";
  }
  std::cout << "</table>" << std::endl;
/*
// now the pseudo tau signatures
  if (pseudo)
  {

    if (ts[0].size > numCycles)
    { std::cout << "hr is ";
// WeylGroup is alittle different from AnotherWeylGroup<derp>
//    ElementWeylGroup hr = G.theElements[G.GetRootReflection(G.RootSystem.size- 1)];
      List<int> hr;
      G.GetGeneratorList(G.GetRootReflection(G.RootSystem.size- 1), hr);
      for (int i = 0; i <hr.size; i ++)
        std::cout << hr[i] << ' ';
      std::cout << std::endl;

      // make enough columns
      std::cout << "\\[ \\begin{array}{";
      for (int i = 0; i <numCycles; i ++)
        std::cout << 'c';
      std::cout << "}\n";

      // labels at the top
      sel.init(G.CartanSymmetric.NumCols);
      for (int i = 0; i <numCycles- 1; i ++)
      { std::cout << "&\\rotatebox{90}{(";
        for (int j = 0; j<sel.CardinalitySelection; j ++)
          std::cout << sel.elements[j] << ' ';
        std::cout << "hr";
        std::cout << ")}\t";
        sel.incrementSelection();
      }

      // data goes out here
      std::cout << "\\\\" << std::endl;
      for (int i = 0; i < ts.size; i ++)
      { for (int j = numCycles; j<numCycles+numCycles- 1; j ++)
        { std::cout << '&';
          std::cout << ts[i][j] << '\t';
        }
        std::cout << "\\\\\n";
      }
      std::cout << "\\end{array} \\]" << std::endl;
    }
  }*/
}

#endif
