//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#ifndef vpfPackedVector_h_already_included
#define vpfPackedVector_h_already_included
#include "json.h"
#include "math_extra_vector_space.h"
#include "math_extra_finite_groups_implementation.h"

static ProjectInformationInstance vpfPackedVector(__FILE__, "Very small (RAM-wise) vectors for extreme computations with small built-in coefficients. ");

template <typename scalar>
class PackedVector {
public:
  scalar data[8];

  static const int size = 8;

  scalar& operator[](int i);
  scalar operator[](int i) const;
  bool operator!=(const PackedVector<scalar>& other) const;
  bool operator==(const PackedVector<scalar>& other) const;
  bool operator>(const PackedVector<scalar>& other) const;
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

// in particular, scalar likes to be int
//Note by Todor: we have to think of a technical way to merge your code with WeylGroup.
//I think adding a templateVector like you did should be straightforward.
//For the time being I will not proceed with a merge.
template <typename scalar, typename templateVector>
class AnotherWeylGroup {
  // compat WeylGroup
  int sizePrivate;
public:
  Matrix<Rational> CartanSymmetric;
  templateVector rho;
  List<templateVector> RootSystem;
  HashedList<templateVector> rhoOrbit; //<-Note from Todor: I don't remember if I changed this to be a Hashed list.
  List<List<int> > conjugacyClasses;
  List<int> classMap;
  List<Matrix<Rational> > classMatrices;

  // needed
  List<Vector<Rational> > characterTable;

  AnotherWeylGroup():sizePrivate(- 1){}
  int GetRank() const; // idk lol
  void SimpleReflection(int i, const templateVector& v, templateVector& out) const;
  void SimpleReflection(int i, templateVector& v) const;
  void GetSimpleReflections(const templateVector& v, List<int>& out) const;
  void GetWord(int i, List<int>& out) const;
  void ApplyReflectionList(const List<int>& simpleReflections, templateVector& v) const;
  void ActOn(int g, templateVector& v) const;
  void ActOn(int g, const templateVector& v, templateVector& out) const;
  int MultiplyElements(int g, int h) const;
  //<-multiply elements is better name than multiply (groups can be tensored/direct product multiplied.
  //MultiplyElements is unambiguous.
  int ConjugacyClassCount() const {
    return this->conjugacyClasses.size;
  }
  int Invert(int g) const;

  void MakeArbitrarySimple(char letter, int number);
  void ComputeRho();
  void ComputeAllElements();
  void ComputeCC();
  void ComputeClassMap();
  Matrix<Rational>& GetClassMatrix(int cc);
  void ComputeClassMatrices(); // O(W.size)
  void ComputeTauSignatures();
  Rational GetHermitianProduct(const Vector<Rational>& X1, const Vector<Rational>& X2) const;

  int GetRootReflection(int i) const;
  void GetSignCharacter(Vector<Rational>& out);

  // own stuff
  int size() const {// useful b/c its nice to not accidentally depend on logically stuff
    return this->sizePrivate;
  }
  int rank;
  templateVector twiceRho;
  Matrix<scalar> unrationalCartanSymmetric;
  templateVector* ucsm;

  void PrintCharTable(const char* filename) const;
};

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::MakeArbitrarySimple(char letter, int number) {
  DynkinType D;
  D.MakeSimpleType(letter, number);
  D.GetCartanSymmetric(this->CartanSymmetric);
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::SimpleReflection(int i, templateVector& v) const {
  // next i'll put some printfs in addition
  //stOutput << "Debugging simple reflections: templateVector " << v << " transformed by reflection " << i;
  scalar x = 0;
  for (int j = 0; j < this->rank; j ++)
    x += this->unrationalCartanSymmetric.elements[i][j] * v[j];
  // so, under ordinary circumstances, this is just out[i] -= x;
  // silent corruption occurs if UnrationalCartanSymmetric.elements[i][i] !∈ {1,2}
  // and scalar can't deal with division properly
  // fortunately this only happens in G₂
  v[i] -= x * 2 / unrationalCartanSymmetric.elements[i][i];
  //stOutput << " becomes " << v << "\n";
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::SimpleReflection(int i, const templateVector& v, templateVector& out) const {
  out = v;
  this->SimpleReflection(i,out);
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::GetSimpleReflections(const templateVector& v, List<int>& out) const {
  templateVector w = v;
  while (w != this->twiceRho) {
    for (int i = 0; i < this->rank; i ++) {
      scalar x = 0;
      for (int j = 0; j < this->rank; j ++) {
        if (this->unrationalCartanSymmetric.elements[i][j] != 0) {
          x += this->unrationalCartanSymmetric.elements[i][j] * w[j];
        }
      }
      if (x > 0) {
        continue;
      }
      out.AddOnTop(i);
      SimpleReflection(i, w);
    }
  }
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::GetWord(int i, List<int>& out) const {
  this->GetSimpleReflections(this->rhoOrbit[i], out);
}

// copypaste another function into this so the autovectorizer will work
template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::ApplyReflectionList(const List<int>& simpleReflections, templateVector& v) const {
  for (int ii = simpleReflections.size - 1; ii >= 0; ii --) {
    //this->SimpleReflection(simpleReflections[i],v);
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

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::ActOn(int i, templateVector& v) const {
  List<int> simpleReflections;
  GetSimpleReflections(this->rhoOrbit[i],simpleReflections);
  this->ApplyReflectionList(simpleReflections, v);
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::ActOn(int i, const templateVector& v, templateVector& out) const {
  out = v;
  this->ActOn(i,out);
}

template <typename scalar, typename templateVector>
int AnotherWeylGroup<scalar, templateVector>::MultiplyElements(int i, int j) const {
  templateVector v = this->rhoOrbit[j];
  this->ActOn(i, v);
  return this->rhoOrbit.GetIndex(v);
}

template <typename scalar, typename templateVector>
int AnotherWeylGroup<scalar, templateVector>::Invert(int i) const {
  List<int> srsl;
  GetSimpleReflections(this->rhoOrbit[i], srsl);
  templateVector v = twiceRho;
  for (int i = srsl.size - 1; i >= 0; i --) {
    this->SimpleReflection(srsl[i], v);
  }
  return this->rhoOrbit.GetIndex(v);
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::ComputeRho() {
  if (unrationalCartanSymmetric.NumRows < CartanSymmetric.NumRows) {
    int den = 1;
    for (int i = 0; i < CartanSymmetric.NumRows; i ++) {
      for (int j = 0; j < CartanSymmetric.NumCols; j ++) {
        den = MathRoutines::lcm(den,CartanSymmetric.elements[i][j].GetDenominator().GetUnsignedIntValueTruncated());
      }
    }
    unrationalCartanSymmetric.init(CartanSymmetric.NumRows, CartanSymmetric.NumCols);
    //this->ucsm = malloc(CartanSymmetric.NumRows*sizeof(templateVector));
    //for (int i = 0; i <CartanSymmetric.NumRows; i ++)
    //  this->ucsm[i].SetSize(CartanSymmetric.NumCols);
    for (int i = 0; i < CartanSymmetric.NumRows; i ++) {
      for (int j = 0; j < CartanSymmetric.NumCols; j ++) {
        unrationalCartanSymmetric.elements[i][j] = (CartanSymmetric.elements[i][j] * den).GetNumerator().GetIntValueTruncated();
        //ucsm[i][j] = unrationalCartanSymmetric.elements[i][j];
      }
    }
  }
  stOutput << "symmetric Cartan matrix is" << "\n";
  for (int i = 0; i < unrationalCartanSymmetric.NumRows; i ++) {
    for (int j = 0; j < unrationalCartanSymmetric.NumCols; j ++) {
      stOutput << unrationalCartanSymmetric.elements[i][j] << '\t';
    }
    stOutput << '\n';
  }
  this->rank = this->unrationalCartanSymmetric.NumRows;
  for (int rvi = 0; rvi < rank; rvi ++) {
    templateVector vi;
    vi.MakeEi(rank, rvi);
    if (this->RootSystem.Contains(vi)) {
      continue;
    }
    this->RootSystem.AddOnTop(vi);
    List<int> newelts;
    templateVector w;
    newelts.AddOnTop(this->RootSystem.GetIndex(vi));
    while (newelts.size > 0) {
      int i = newelts.PopLastObject();
      for (int si = 0; si < this->rank; si ++) {
        this->SimpleReflection(si, this->RootSystem[i], w);
        if (this->RootSystem.GetIndex(w) == - 1) {
          this->RootSystem.AddOnTop(w);
          int j = this->RootSystem.GetIndex(w);
          newelts.AddOnTop(j);
        }
      }
    }
  }
  stOutput << "root system is" << "\n";
  stOutput << this->RootSystem << "\n";
  this->RootSystem.QuickSortAscending();
  this->twiceRho.MakeZero(rank);
  for (int i = 0; i < this->RootSystem.size; i ++) {
    bool usethis = true;
    for (int j = 0; j < this->RootSystem[i].size; j ++) {
      if (this->RootSystem[i][j] < 0) {
        usethis = false;
        break;
      }
    }
    if (usethis) {
      this->twiceRho += this->RootSystem[i];
    }
  }
  this->rho.SetSize(this->twiceRho.size);
  for (int i = 0; i < this->twiceRho.size; i ++) {
    this->rho[i] = this->twiceRho[i] / 2;
  }
  stOutput << "half sum of positive roots is " << this->rho << "\n";
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::ComputeAllElements() {
  stOutput << "Getting elements...";
  if (this->RootSystem.size == 0) {
    this->ComputeRho();
  }
  stOutput << "(twiceRho is " << this->twiceRho << ")" << "\n";
  templateVector w;
  List<int> newelts;
  this->rhoOrbit.AddOnTop(twiceRho);
  newelts.AddOnTop(this->rhoOrbit.GetIndex(twiceRho));
  while (newelts.size > 0) {
    int i = newelts.PopLastObject();
    for (int si = 0; si < this->rank; si ++) {
      this->SimpleReflection(si,this->rhoOrbit[i],w);
      if (this->rhoOrbit.GetIndex(w) == - 1) {
        this->rhoOrbit.AddOnTop(w);
        int j = this->rhoOrbit.GetIndex(w);
        newelts.AddOnTop(j);
      }
    }
  }
  this->sizePrivate = this->rhoOrbit.size;
  stOutput << this->size() << "\n";
//  stOutput << rhoOrbit << "\n";
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::ComputeCC() {
  stOutput << "Getting conjugacy classes...";
  if (this->rhoOrbit.size == 0) {
    this->ComputeAllElements();
  }
  List<bool> Accounted;
  Accounted.initializeFillInObject(this->size(), false);
  this->conjugacyClasses.SetSize(0);
  this->conjugacyClasses.Reserve(50);
  HashedList<int, MathRoutines::IntUnsignIdentity> theStack;
  theStack.SetExpectedSize(this->size());
  int theRank = this->GetRank();
  templateVector theRhoImage;
  stOutput << "number of conjugacy classes... ";
  for (int i = 0; i < this->size(); i ++) {
    if (!Accounted[i]) {
      theStack.Clear();
      theStack.AddOnTop(i);
      for (int j = 0; j < theStack.size; j ++)
        for (int k = 0; k < theRank; k ++) {
          theRhoImage = this->twiceRho;
          this->SimpleReflection(k, theRhoImage);
          this->ActOn(theStack[j], theRhoImage);
          this->SimpleReflection(k, theRhoImage);
          int accountedIndex = this->rhoOrbit.GetIndex(theRhoImage);
          theStack.AddOnTopNoRepetition(accountedIndex);
          Accounted[accountedIndex] = true;
        }
      this->conjugacyClasses.AddOnTop(theStack);
      this->conjugacyClasses.LastObject()->QuickSortAscending();
      stOutput << this->ConjugacyClassCount() << " ";
    }
  }
  this->conjugacyClasses.QuickSortAscending();
  stOutput << this->ConjugacyClassCount() << "\n";
}

template<typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::ComputeClassMap() {
  this->classMap.SetSize(this->size);
  for (int i = 0; i < this->ConjugacyClassCount(); i ++) {
    for (int j = 0; j < this->conjugacyClasses[i].size; j ++) {
      this->classmap[this->conjugacyClasses[i][j]] = i;
    }
  }
}

template<typename scalar, typename templateVector>
Matrix<Rational>& AnotherWeylGroup<scalar, templateVector>::GetClassMatrix(int cc) {
  if (this->classMatrices.size == 0) {
    if (this->ConjugacyClassCount() == 0) {
      this->ComputeCC();
    }
    this->classMatrices.SetSize(this->ConjugacyClassCount());
  }
  if (this->classMap.size == 0) {
    this->ComputeClassMap();
  }
  this->classMatrices[cc] = GetClassMatrix(*this, cc, &(this->classmap));
  return this->classMatrices[cc];
}

template<typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::ComputeClassMatrices() {
  if (this->ConjugacyClassCount() == 0) {
    this->ComputeCC();
  }
  for (int i = 0; i < this->ConjugacyClassCount(); i ++) {
    stOutput << "//getting class matrix " << i << "\n";
    this->GetClassMatrix(i);
  }
}

template <typename scalar, typename templateVector>
int AnotherWeylGroup<scalar, templateVector>::GetRank() const {
  return this->CartanSymmetric.NumRows;
}

template <typename scalar, typename templateVector>
int AnotherWeylGroup<scalar, templateVector>::GetRootReflection(int i) const {
  scalar x = this->RootSystem[i].ScalarProduct(this->twiceRho,this->unrationalCartanSymmetric);
  x *= - 2;
  x /= this->RootSystem[i].ScalarProduct(this->RootSystem[i],this->unrationalCartanSymmetric);
  return this->rhoOrbit.GetIndexIMustContainTheObject(twiceRho+this->RootSystem[i] * x);
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::GetSignCharacter(Vector<Rational>& out) {
  if (this->ConjugacyClassCount() == 0) {
    this->ComputeCC();
  }
  out.SetSize(this->ConjugacyClassCount());
  for (int i = 0; i < this->ConjugacyClassCount(); i ++) {
    List<int> srs;
    this->GetWord(this->conjugacyClasses[i][0], srs);
    int yn = srs.size % 2;
    if (yn == 0) {
      out[i] = 1;
    } else {
      out[i] = - 1;
    }
  }
}

// there are like 3 copies of this function with slightly different argument types
template <typename scalar, typename templateVector>
Rational AnotherWeylGroup<scalar, templateVector>::GetHermitianProduct(const Vector<Rational>& X1, const Vector<Rational>& X2) const {
  Rational acc = 0;
  for (int i = 0; i < X1.size; i ++) {
    acc += X1[i].GetComplexConjugate() * X2[i] * this->conjugacyClasses[i].size;
  }
  return acc / this->size();
}

List<List<Vector<Rational> > > eigenspaces(const Matrix<Rational> &M, int checkDivisorsOf= 0);


template <typename coefficient>
List<VectorSpace<coefficient> > GetEigenspaces(const Matrix<coefficient> &M) {
  List<List<Vector<coefficient> > > es = eigenspaces(M);
  List<VectorSpace<coefficient> > vs;
  for (int spi = 0; spi < es.size; spi ++) {
    VectorSpace<coefficient> V;
    for (int j = 0; j < es[spi].size; j ++) {
      V.AddVector(es[spi][j]);
    }
    vs.AddOnTop(V);
  }
  return vs;
}

// As in Schneider, 1990
// well, so far not using any of his improvements
template <typename somegroup>
List<ClassFunction<somegroup, Rational> > ComputeCharacterTable(somegroup &G) {
  if (G.ConjugacyClassCount() == 0) {
    G.ComputeCCfromAllElements();
  }
  List<int> classmap;
  int sizeOfG = - 1;
  G.GetSize().IsIntegerFittingInInt(&sizeOfG);
  classmap.SetSize(sizeOfG);
//  classmap.SetSize(G.theElements.size);
  for (int i = 0; i < G.ConjugacyClassCount(); i ++) {
    for (int j = 0; j < G.conjugacyClasseS[i].size; j ++) {
      classmap[G.theElements.GetIndex(G.conjugacyClasseS[i].theElements[j])] = i;
    }
  }
  Matrix<Rational> form; // so inefficient
  form.MakeZeroMatrix(G.ConjugacyClassCount());
  for (int i = 0; i < G.ConjugacyClassCount(); i ++) {
    form.elements[i][i] = G.conjugacyClasseS[i].size;
  }
  List<VectorSpace<Rational> > spaces;
  if (G.characterTable.size > 0) {
    VectorSpace<Rational> allchars;
    for (int i = 0; i < G.characterTable.size; i ++) {
      VectorSpace<Rational> xspi;
      xspi.AddVector(G.characterTable[i].data);
      spaces.AddOnTop(xspi);
      allchars.AddVector(G.characterTable[i].data);
    }
    spaces.AddOnTop(allchars.OrthogonalComplement(nullptr, &form));
  } else {
    Vector<Rational> X1;
    X1.SetSize(G.ConjugacyClassCount());
    for (int i = 0; i < G.ConjugacyClassCount(); i ++) {
      X1[i] = 1;
    }
    VectorSpace<Rational> sp1;
    sp1.AddVector(X1);
    spaces.AddOnTop(sp1);
    spaces.AddOnTop(sp1.OrthogonalComplement(nullptr, &form));
  }
  bool foundEmAll = false;
  for (int i = 0; !foundEmAll && i < G.ConjugacyClassCount(); i ++) {
    Matrix<Rational> M;
    stOutput << "Getting class matrix " << i << "\n";
    M = GetClassMatrix(G,i,&classmap);
    List<VectorSpace<Rational> > es = GetEigenspaces(M);
    for (int esi = 0; !foundEmAll && esi < es.size; esi ++) {
      int spsize = spaces.size;
      for (int spi = 0; !foundEmAll &&spi < spsize; spi ++) {
        if (spaces[spi].rank == 1) {
          continue;
        }
        VectorSpace<Rational> V = spaces[spi].Intersection(es[esi]);
        if ((V.rank > 0) and (V.rank < spaces[spi].rank)) {
          VectorSpace<Rational> W = es[esi].OrthogonalComplement(&spaces[spi],&form);
          spaces[spi] = V;
          spaces.AddOnTop(W);
          if (spaces.size == G.ConjugacyClassCount()) {
            foundEmAll = true;
            break;
          }
        }
      }
    }
    int nchars = 0;
    for (int i = 0; i < spaces.size; i ++) {
      if (spaces[i].rank == 1) {
        nchars += 1;
      }
    }
    stOutput << "Have " << nchars << " chars" << "\n";
  }
  List<ClassFunction<somegroup, Rational> > chars;
  chars.SetSize(spaces.size);
  for (int i = 0; i < spaces.size; i ++) {
    chars[i].data = spaces[i].GetCanonicalBasisVector(0);
    chars[i].G = &G;
  }
  for (int i = 0; i < spaces.size; i ++) {
    Rational x = chars[i].InnerProduct(chars[i]);
    int x2 = x.GetDenominator().GetUnsignedIntValueTruncated();
    x2 = static_cast<int>(FloatingPoint::sqrt(x2));
    chars[i] *= x2;
    if (chars[i][0] < 0) {
      chars[i] *= - 1;
    }
    stOutput << x2 << "\n";
  }

  chars.QuickSortAscending(/*&CharacterComparator*/);
  for (int i = 0; i < chars.size; i ++) {
    for (int j = i; j < chars.size; j ++) {
      if (chars[i] > chars[j]) {
        stOutput << "error: " << i << j << "\n";
      }
    }
  }
  for (int i = 0; i < chars.size; i ++) {
    stOutput << chars[i] << "\n";
  }
  G.characterTable = chars;
  for (int i = 0; i < G.characterTable.size; i ++) {
    stOutput << G.characterTable[i] << "\n";
  }
  return chars;
}

template <typename somegroup>
Matrix<Rational> GetClassMatrix(const somegroup &G, int cci, List<int>* classmap = nullptr) {
  List<int> invl;
  int classSize = - 1;
  G.conjugacyClasseS[cci].size.IsIntegerFittingInInt(&classSize);
  invl.SetSize(classSize);
  for (int i = 0; i < G.conjugacyClasseS[cci].size; i ++) {
    invl[i] = G.Invert(G.theElements.GetIndex(G.conjugacyClasseS[cci].theElements[i]));
  }
  Matrix<int> M;
  M.MakeZeroMatrix(G.ConjugacyClassCount());
  for (int t = 0; t < G.ConjugacyClassCount(); t ++)
    for (int xi = 0; xi < invl.size; xi ++) {
      int yi = G.MultiplyElements(invl[xi], G.theElements.GetIndex(G.conjugacyClasseS[t].representative));
      int ci;
      if (classmap) {
        M.elements[t][(*classmap)[yi]] += 1;
      } else {
        for (ci = 0; ci < G.ConjugacyClassCount(); ci ++) {
          if (G.conjugacyClasseS[ci].indicesEltsInOwner.BSContains(yi)) {
            M.elements[t][ci] += 1;
            break;
          }
        }
      }
    }
  Matrix<Rational> out;
  out.init(M.NumRows, M.NumCols);
  for (int i = 0; i < M.NumRows; i ++) {
    for (int j = 0; j < M.NumCols; j ++) {
      out.elements[i][j] = M.elements[i][j];
    }
  }
  return out;
}

void GetTauSignaturesFromSubgroup(WeylGroupData& G, const List<ElementWeylGroup>& gens, List<bool>& out) {
  /*List<ElementWeylGroup<WeylGroup>> genes;
  genes.SetSize(gens.size);
  for (int i = 0; i <gens.size; i ++)
    genes[i] = G.theElements[gens[i]];
  FiniteGroup<ElementWeylGroup<WeylGroup>> H;
  H.MakeFrom(genes);
  Vector<Rational> HXs;
  H.GetSignCharacter(HXs);*/

  SubgroupData<FiniteGroup<ElementWeylGroup>, ElementWeylGroup> HD;
  HD.initFromGroupAndGenerators(G.theGroup, gens);
  FiniteGroup<ElementWeylGroup>& H = *HD.theSubgroup;
  H.ComputeAllElements(true, - 1);
  Vector<Rational> HXs;
  H.GetSignCharacter(HXs);

  List<int> ccPreimages;
  ccPreimages.SetSize(H.ConjugacyClassCount());
  for (int i = 0; i < H.ConjugacyClassCount(); i ++) {
    bool notFound = true;
    for (int ci = 0; notFound && ci < G.theGroup.ConjugacyClassCount(); ci ++) {
      for (int cj = 0; notFound && cj < G.theGroup.conjugacyClasseS[ci].size; cj ++) {
        if (G.theGroup.conjugacyClasseS[ci].theElements[cj] == H.conjugacyClasseS[i].representative) {
          ccPreimages[i] = ci;
          notFound = false;
        }
      }
    }
    if (notFound) {
      crash << "Something went very wrong: couldn't find preimage of conjugacy class of subgroup. " << crash;
    }
  }
  out.SetSize(G.theGroup.characterTable.size);
  Vector<Rational> HXi;
  HXi.SetSize(H.ConjugacyClassCount());
  for (int i = 0; i < G.theGroup.characterTable.size; i ++) {
    Vector<Rational> GXi = G.theGroup.characterTable[i].data;
    for (int j = 0; j < HXi.size; j ++) {
      HXi[j] = GXi[ccPreimages[j]];
    }
    if (H.GetHermitianProduct(HXs, HXi) == 0) {
      out[i] = false;
    } else {
      out[i] = true;
    }
  }
}

void ComputeTauSignatures(WeylGroupData* G, List<List<bool> >& tauSignatures, bool pseudo = false) {
  Selection sel;
  sel.init(G->CartanSymmetric.NumCols);
  int numCycles = MathRoutines::TwoToTheNth(sel.MaxSize);
  List<List<bool> > tss;
  tss.SetSize(numCycles);
  List<ElementWeylGroup> theGenerators;
  for (int i = 0; i < numCycles - 2; i ++) {
    sel.incrementSelection();
    theGenerators.SetSize(sel.CardinalitySelection);
    for (int j = 0; j < sel.CardinalitySelection; j ++) {
      theGenerators[j].MakeSimpleReflection(sel.elements[j], *G);
    }
    GetTauSignaturesFromSubgroup(*G,theGenerators,tss[i]);
  }
  Vector<Rational> Xs;
  G->GetSignCharacter(Xs);
  List<bool> tsg;
  tsg.SetSize(G->theGroup.characterTable.size);
  for (int i = 0; i < G->theGroup.characterTable.size; i ++) {
    tsg[i] = G->theGroup.characterTable[i].data == Xs;
  }
  tss.AddOnTop(tsg);

  if (pseudo) {
    stOutput << "pseudo-parabolics" << "\n";
    ElementWeylGroup hr = G->GetRootReflection(G->RootSystem.size- 1);
    sel.init(G->CartanSymmetric.NumCols);
    for (int i = 0; i < numCycles - 1; i ++) {
      theGenerators.SetSize(sel.CardinalitySelection);
      for (int j = 0; j < sel.CardinalitySelection; j ++) {
        theGenerators[j].MakeSimpleReflection(sel.elements[j], *G);
      }
      theGenerators.AddOnTop(hr);
      List<bool> ts;
      GetTauSignaturesFromSubgroup(*G,theGenerators,ts);
      tss.AddOnTop(ts);
      sel.incrementSelection();
    }
  }

  // we will need the sign character for the group


  tauSignatures.SetSize(G->theGroup.characterTable.size);
  for (int i = 0; i < G->theGroup.characterTable.size; i ++) {
    tauSignatures[i].SetSize(tss.size + 1);
    tauSignatures[i][0] = 1;
    for (int j = 1; j < tss.size + 1; j ++) {
      tauSignatures[i][j] = tss[j - 1][i];
    }
  }
  for (int i = 0; i < G->theGroup.characterTable.size; i ++) {
    stOutput << G->theGroup.characterTable[i] << "\n";
    for (int j = 0; j < tauSignatures[i].size; j ++) {
      stOutput << tauSignatures[i][j] << ' ';
    }
    stOutput << "\n";
  }
}

template <typename elementSomeGroup>
void ExportCharTable(FiniteGroup<elementSomeGroup>& G, JSData &data) {
  data.theType = JSData::token::tokenObject;
  JSData& representatives = data.objects.GetValueCreate("representatives");
  JSData& sizes = data.objects.GetValueCreate("sizes");
  JSData& characters = data.objects.GetValueCreate("characters");
  representatives.theType = JSData::token::tokenArray;
  representatives.theList.SetSize(G.ConjugacyClassCount());

  for (int i = 0; i < G.ConjugacyClassCount(); i ++) {
    List<int> reprefs;
    G.GetWord(G.conjugacyClasseS[i].representative, reprefs);
    representatives.theList[i].theType = JSData::token::tokenArray;
    representatives.theList[i].theList.SetSize(reprefs.size);
    for (int j = 0; j < reprefs.size; j ++) {
      representatives.theList[i].theList[j].theType = JSData::token::tokenLargeInteger;
      representatives.theList[i].theList[j].theInteger.GetElement() = reprefs[j];
    }
  }
  sizes.theType = JSData::token::tokenArray;
  sizes.theList.SetSize(G.ConjugacyClassCount());
  for (int i = 0; i < G.ConjugacyClassCount(); i ++) {
    sizes.theList[i].theType = JSData::token::tokenLargeInteger;
    sizes.theList[i].theInteger.GetElement() = G.conjugacyClasseS[i].size;
  }
  characters.theType = JSData::token::tokenArray;
  characters.theList.SetSize(G.characterTable.size);
  for (int i = 0; i < G.characterTable.size; i ++) {
    for (int j = 0; j < G.characterTable[i].data.size; j ++) {
      characters[i][j] = G.characterTable[i][j].GetNumerator();
    }
  }
}

void ExportTauSignatures(WeylGroupData& G, const List<List<bool> >& ts, JSData &data) {
  ExportCharTable(G.theGroup, data["chartable"]);
  for (int i = 0; i < ts.size; i ++) {
    for (int j = 0; j < ts[i].size; j ++) {
      data["tausigs"][i][j] = ts[i][j];
    }
  }
}

#endif
