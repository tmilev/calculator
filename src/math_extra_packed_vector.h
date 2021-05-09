// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfPackedVector_h_already_included
#define vpfPackedVector_h_already_included
#include "json.h"
#include "math_extra_vector_space.h"
#include "math_extra_finite_groups_implementation.h"

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
  void makeZero(int d);
  void makeEi(int d, int i);
  // for compatibility
  void setSize(int s);

  PackedVector<scalar> operator*(scalar x) const;
  PackedVector<scalar> operator+(const PackedVector<scalar>& w) const;
  void operator+=(const PackedVector<scalar>& w);
  scalar scalarProduct(const PackedVector<scalar>& v, const Matrix<scalar>& B) const;

  unsigned int hashFunction() const;
  static unsigned int hashFunction(const PackedVector<scalar>& in);
};

// In particular, scalar likes to be int
// Note by Todor: we have to think of a technical way to merge your code with WeylGroup.
// I think adding a templateVector like you did should be straightforward.
// For the time being I will not proceed with a merge.
template <typename scalar, typename templateVector>
class AnotherWeylGroup {
  // compat WeylGroup
  int sizePrivate;
public:
  Matrix<Rational> cartanSymmetric;
  templateVector rho;
  List<templateVector> RootSystem;
  HashedList<templateVector> rhoOrbit; //<-Note from Todor: I don't remember if I changed this to be a Hashed list.
  List<List<int> > conjugacyClasses;
  List<int> classMap;
  List<Matrix<Rational> > classMatrices;

  // needed
  List<Vector<Rational> > characterTable;

  AnotherWeylGroup() : sizePrivate(- 1){}
  int getRank() const; // idk lol
  void simpleReflection(int i, const templateVector& v, templateVector& out) const;
  void simpleReflection(int i, templateVector& v) const;
  void getSimpleReflections(const templateVector& v, List<int>& out) const;
  void getWord(int i, List<int>& out) const;
  void applyReflectionList(const List<int>& simpleReflections, templateVector& v) const;
  void actOn(int g, templateVector& v) const;
  void actOn(int g, const templateVector& v, templateVector& out) const;
  int multiplyElements(int g, int h) const;
  //<-multiply elements is better name than multiply (groups can be tensored/direct product multiplied.
  //multiplyElements is unambiguous.
  int conjugacyClassCount() const {
    return this->conjugacyClasses.size;
  }
  int invert(int g) const;

  void makeArbitrarySimple(char letter, int number);
  void computeRho();
  void computeAllElements();
  void computeConjugacyClasses();
  void computeClassMap();
  Matrix<Rational>& getClassMatrix(int cc);
  void computeClassMatrices(); // O(W.size)
  void computeTauSignatures();
  Rational getHermitianProduct(const Vector<Rational>& left, const Vector<Rational>& right) const;

  int getRootReflection(int i) const;
  void getSignCharacter(Vector<Rational>& out);

  // own stuff
  int size() const {// useful b/c its nice to not accidentally depend on logically stuff
    return this->sizePrivate;
  }
  int rank;
  templateVector twiceRho;
  Matrix<scalar> unrationalCartanSymmetric;
  templateVector* ucsm;
};

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::makeArbitrarySimple(char letter, int number) {
  DynkinType D;
  D.makeSimpleType(letter, number);
  D.getCartanSymmetric(this->cartanSymmetric);
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::simpleReflection(int i, templateVector& v) const {
  scalar x = 0;
  for (int j = 0; j < this->rank; j ++) {
    x += this->unrationalCartanSymmetric.elements[i][j] * v[j];
  }
  // so, under ordinary circumstances, this is just out[i] -= x;
  // silent corruption occurs if UnrationalCartanSymmetric.elements[i][i] does not belong to {1, 2}
  // and scalar can't deal with division properly
  // fortunately this only happens in G_2
  v[i] -= x * 2 / unrationalCartanSymmetric.elements[i][i];
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::simpleReflection(
  int i, const templateVector& v, templateVector& out
) const {
  out = v;
  this->simpleReflection(i, out);
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::getSimpleReflections(const templateVector& v, List<int>& out) const {
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
      out.addOnTop(i);
      simpleReflection(i, w);
    }
  }
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::getWord(int i, List<int>& out) const {
  this->getSimpleReflections(this->rhoOrbit[i], out);
}

// copypaste another function into this so the autovectorizer will work
template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::applyReflectionList(const List<int>& simpleReflections, templateVector& v) const {
  for (int ii = simpleReflections.size - 1; ii >= 0; ii --) {
    //this->simpleReflection(simpleReflections[i],v);
    int i = simpleReflections[ii];
    scalar x = 0;
    for (int j = 0; j < this->rank; j ++) {
      x += this->unrationalCartanSymmetric.elements[i][j] * v[j];
    }
    // so, under ordinary circumstances, this is just out[i] -= x;
    // silent corruption occurs if UnrationalCartanSymmetric.elements[i][i] !∈ {1,2}
    // and scalar can't deal with division properly
    // fortunately this only happens in G₂
    v[i] -= x * 2 / unrationalCartanSymmetric.elements[i][i];
  }
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::actOn(int i, templateVector& v) const {
  List<int> simpleReflections;
  getSimpleReflections(this->rhoOrbit[i],simpleReflections);
  this->applyReflectionList(simpleReflections, v);
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::actOn(int i, const templateVector& v, templateVector& out) const {
  out = v;
  this->actOn(i,out);
}

template <typename scalar, typename templateVector>
int AnotherWeylGroup<scalar, templateVector>::multiplyElements(int i, int j) const {
  templateVector v = this->rhoOrbit[j];
  this->actOn(i, v);
  return this->rhoOrbit.getIndex(v);
}

template <typename scalar, typename templateVector>
int AnotherWeylGroup<scalar, templateVector>::invert(int i) const {
  List<int> srsl;
  getSimpleReflections(this->rhoOrbit[i], srsl);
  templateVector v = twiceRho;
  for (int i = srsl.size - 1; i >= 0; i --) {
    this->simpleReflection(srsl[i], v);
  }
  return this->rhoOrbit.getIndex(v);
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::computeRho() {
  if (unrationalCartanSymmetric.numberOfRows < cartanSymmetric.numberOfRows) {
    int den = 1;
    for (int i = 0; i < cartanSymmetric.numberOfRows; i ++) {
      for (int j = 0; j < cartanSymmetric.numberOfColumns; j ++) {
        den = MathRoutines::leastCommonMultiple(den,cartanSymmetric.elements[i][j].getDenominator().getUnsignedIntValueTruncated());
      }
    }
    unrationalCartanSymmetric.initialize(cartanSymmetric.numberOfRows, cartanSymmetric.numberOfColumns);
    //this->ucsm = malloc(cartanSymmetric.numberOfRows*sizeof(templateVector));
    //for (int i = 0; i <cartanSymmetric.numberOfRows; i ++)
    //  this->ucsm[i].setSize(cartanSymmetric.numberOfColumns);
    for (int i = 0; i < cartanSymmetric.numberOfRows; i ++) {
      for (int j = 0; j < cartanSymmetric.numberOfColumns; j ++) {
        unrationalCartanSymmetric.elements[i][j] = (cartanSymmetric.elements[i][j] * den).getNumerator().getIntValueTruncated();
        //ucsm[i][j] = unrationalCartanSymmetric.elements[i][j];
      }
    }
  }
  global.comments << "symmetric Cartan matrix is" << "\n";
  for (int i = 0; i < unrationalCartanSymmetric.numberOfRows; i ++) {
    for (int j = 0; j < unrationalCartanSymmetric.numberOfColumns; j ++) {
      global.comments << unrationalCartanSymmetric.elements[i][j] << '\t';
    }
    global.comments << '\n';
  }
  this->rank = this->unrationalCartanSymmetric.numberOfRows;
  for (int rvi = 0; rvi < rank; rvi ++) {
    templateVector vi;
    vi.makeEi(rank, rvi);
    if (this->RootSystem.contains(vi)) {
      continue;
    }
    this->RootSystem.addOnTop(vi);
    List<int> newelts;
    templateVector w;
    newelts.addOnTop(this->RootSystem.getIndex(vi));
    while (newelts.size > 0) {
      int i = newelts.popLastObject();
      for (int si = 0; si < this->rank; si ++) {
        this->simpleReflection(si, this->RootSystem[i], w);
        if (this->RootSystem.getIndex(w) == - 1) {
          this->RootSystem.addOnTop(w);
          int j = this->RootSystem.getIndex(w);
          newelts.addOnTop(j);
        }
      }
    }
  }
  global.comments << "root system is" << "\n";
  global.comments << this->RootSystem << "\n";
  this->RootSystem.quickSortAscending();
  this->twiceRho.makeZero(rank);
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
  this->rho.setSize(this->twiceRho.size);
  for (int i = 0; i < this->twiceRho.size; i ++) {
    this->rho[i] = this->twiceRho[i] / 2;
  }
  global.comments << "half sum of positive roots is " << this->rho << "\n";
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::computeAllElements() {
  global.comments << "Getting elements...";
  if (this->RootSystem.size == 0) {
    this->computeRho();
  }
  global.comments << "(twiceRho is " << this->twiceRho << ")" << "\n";
  templateVector w;
  List<int> newelts;
  this->rhoOrbit.addOnTop(twiceRho);
  newelts.addOnTop(this->rhoOrbit.getIndex(twiceRho));
  while (newelts.size > 0) {
    int i = newelts.popLastObject();
    for (int si = 0; si < this->rank; si ++) {
      this->simpleReflection(si,this->rhoOrbit[i],w);
      if (this->rhoOrbit.getIndex(w) == - 1) {
        this->rhoOrbit.addOnTop(w);
        int j = this->rhoOrbit.getIndex(w);
        newelts.addOnTop(j);
      }
    }
  }
  this->sizePrivate = this->rhoOrbit.size;
  global.comments << this->size() << "\n";
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::computeConjugacyClasses() {
  global.comments << "Getting conjugacy classes...";
  if (this->rhoOrbit.size == 0) {
    this->computeAllElements();
  }
  List<bool> Accounted;
  Accounted.initializeFillInObject(this->size(), false);
  this->conjugacyClasses.setSize(0);
  this->conjugacyClasses.reserve(50);
  HashedList<int, HashFunctions::hashFunction> theStack;
  theStack.setExpectedSize(this->size());
  int theRank = this->getRank();
  templateVector theRhoImage;
  global.comments << "number of conjugacy classes... ";
  for (int i = 0; i < this->size(); i ++) {
    if (!Accounted[i]) {
      theStack.clear();
      theStack.addOnTop(i);
      for (int j = 0; j < theStack.size; j ++)
        for (int k = 0; k < theRank; k ++) {
          theRhoImage = this->twiceRho;
          this->simpleReflection(k, theRhoImage);
          this->actOn(theStack[j], theRhoImage);
          this->simpleReflection(k, theRhoImage);
          int accountedIndex = this->rhoOrbit.getIndex(theRhoImage);
          theStack.addOnTopNoRepetition(accountedIndex);
          Accounted[accountedIndex] = true;
        }
      this->conjugacyClasses.addOnTop(theStack);
      this->conjugacyClasses.lastObject()->quickSortAscending();
      global.comments << this->conjugacyClassCount() << " ";
    }
  }
  this->conjugacyClasses.quickSortAscending();
  global.comments << this->conjugacyClassCount() << "\n";
}

template<typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::computeClassMap() {
  this->classMap.setSize(this->size);
  for (int i = 0; i < this->conjugacyClassCount(); i ++) {
    for (int j = 0; j < this->conjugacyClasses[i].size; j ++) {
      this->classmap[this->conjugacyClasses[i][j]] = i;
    }
  }
}

template<typename scalar, typename templateVector>
Matrix<Rational>& AnotherWeylGroup<scalar, templateVector>::getClassMatrix(int cc) {
  if (this->classMatrices.size == 0) {
    if (this->conjugacyClassCount() == 0) {
      this->computeConjugacyClasses();
    }
    this->classMatrices.setSize(this->conjugacyClassCount());
  }
  if (this->classMap.size == 0) {
    this->computeClassMap();
  }
  this->classMatrices[cc] = getClassMatrix(*this, cc, &(this->classmap));
  return this->classMatrices[cc];
}

template<typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::computeClassMatrices() {
  if (this->conjugacyClassCount() == 0) {
    this->computeConjugacyClasses();
  }
  for (int i = 0; i < this->conjugacyClassCount(); i ++) {
    global.comments << "//getting class matrix " << i << "\n";
    this->getClassMatrix(i);
  }
}

template <typename scalar, typename templateVector>
int AnotherWeylGroup<scalar, templateVector>::getRank() const {
  return this->cartanSymmetric.numberOfRows;
}

template <typename scalar, typename templateVector>
int AnotherWeylGroup<scalar, templateVector>::getRootReflection(int i) const {
  scalar x = this->RootSystem[i].scalarProduct(this->twiceRho,this->unrationalCartanSymmetric);
  x *= - 2;
  x /= this->RootSystem[i].scalarProduct(this->RootSystem[i],this->unrationalCartanSymmetric);
  return this->rhoOrbit.getIndexNoFail(twiceRho+this->RootSystem[i] * x);
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::getSignCharacter(Vector<Rational>& out) {
  if (this->conjugacyClassCount() == 0) {
    this->computeConjugacyClasses();
  }
  out.setSize(this->conjugacyClassCount());
  for (int i = 0; i < this->conjugacyClassCount(); i ++) {
    List<int> srs;
    this->getWord(this->conjugacyClasses[i][0], srs);
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
Rational AnotherWeylGroup<scalar, templateVector>::getHermitianProduct(const Vector<Rational>& left, const Vector<Rational>& right) const {
  Rational acc = 0;
  for (int i = 0; i < left.size; i ++) {
    acc += left[i].getComplexConjugate() * right[i] * this->conjugacyClasses[i].size;
  }
  return acc / this->size();
}

List<List<Vector<Rational> > > eigenspaces(const Matrix<Rational>& M, int checkDivisorsOf = 0);

template <typename Coefficient>
List<VectorSpace<Coefficient> > getEigenspaces(const Matrix<Coefficient>& M) {
  List<List<Vector<Coefficient> > > es = eigenspaces(M);
  List<VectorSpace<Coefficient> > vs;
  for (int spi = 0; spi < es.size; spi ++) {
    VectorSpace<Coefficient> V;
    for (int j = 0; j < es[spi].size; j ++) {
      V.addVector(es[spi][j]);
    }
    vs.addOnTop(V);
  }
  return vs;
}

// As in Schneider, 1990
// well, so far not using any of his improvements
template <typename somegroup>
List<ClassFunction<somegroup, Rational> > computeCharacterTable(somegroup& G) {
  if (G.conjugacyClassCount() == 0) {
    G.computeConjugacyClassesFromAllElements();
  }
  List<int> classmap;
  int sizeOfG = - 1;
  G.getSize().isIntegerFittingInInt(&sizeOfG);
  classmap.setSize(sizeOfG);
//  classmap.setSize(G.theElements.size);
  for (int i = 0; i < G.conjugacyClassCount(); i ++) {
    for (int j = 0; j < G.conjugacyClasses[i].size; j ++) {
      classmap[G.theElements.getIndex(G.conjugacyClasses[i].theElements[j])] = i;
    }
  }
  Matrix<Rational> form; // so inefficient
  form.makeZeroMatrix(G.conjugacyClassCount());
  for (int i = 0; i < G.conjugacyClassCount(); i ++) {
    form.elements[i][i] = G.conjugacyClasses[i].size;
  }
  List<VectorSpace<Rational> > spaces;
  if (G.characterTable.size > 0) {
    VectorSpace<Rational> allchars;
    for (int i = 0; i < G.characterTable.size; i ++) {
      VectorSpace<Rational> xspi;
      xspi.addVector(G.characterTable[i].data);
      spaces.addOnTop(xspi);
      allchars.addVector(G.characterTable[i].data);
    }
    spaces.addOnTop(allchars.orthogonalComplement(nullptr, &form));
  } else {
    Vector<Rational> X1;
    X1.setSize(G.conjugacyClassCount());
    for (int i = 0; i < G.conjugacyClassCount(); i ++) {
      X1[i] = 1;
    }
    VectorSpace<Rational> sp1;
    sp1.addVector(X1);
    spaces.addOnTop(sp1);
    spaces.addOnTop(sp1.orthogonalComplement(nullptr, &form));
  }
  bool foundEmAll = false;
  for (int i = 0; !foundEmAll && i < G.conjugacyClassCount(); i ++) {
    Matrix<Rational> M;
    global.comments << "Getting class matrix " << i << "\n";
    M = getClassMatrix(G,i,&classmap);
    List<VectorSpace<Rational> > es = getEigenspaces(M);
    for (int esi = 0; !foundEmAll && esi < es.size; esi ++) {
      int spsize = spaces.size;
      for (int spi = 0; !foundEmAll &&spi < spsize; spi ++) {
        if (spaces[spi].rank == 1) {
          continue;
        }
        VectorSpace<Rational> V = spaces[spi].intersection(es[esi]);
        if ((V.rank > 0) and (V.rank < spaces[spi].rank)) {
          VectorSpace<Rational> W = es[esi].orthogonalComplement(&spaces[spi],&form);
          spaces[spi] = V;
          spaces.addOnTop(W);
          if (spaces.size == G.conjugacyClassCount()) {
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
    global.comments << "Have " << nchars << " chars" << "\n";
  }
  List<ClassFunction<somegroup, Rational> > chars;
  chars.setSize(spaces.size);
  for (int i = 0; i < spaces.size; i ++) {
    chars[i].data = spaces[i].getCanonicalBasisVector(0);
    chars[i].G = &G;
  }
  for (int i = 0; i < spaces.size; i ++) {
    Rational x = chars[i].innerProduct(chars[i]);
    int x2 = x.getDenominator().getUnsignedIntValueTruncated();
    x2 = static_cast<int>(FloatingPoint::sqrtFloating(x2));
    chars[i] *= x2;
    if (chars[i][0] < 0) {
      chars[i] *= - 1;
    }
    global.comments << x2 << "\n";
  }

  chars.quickSortAscending(/*&CharacterComparator*/);
  for (int i = 0; i < chars.size; i ++) {
    for (int j = i; j < chars.size; j ++) {
      if (chars[i] > chars[j]) {
        global.comments << "error: " << i << j << "\n";
      }
    }
  }
  for (int i = 0; i < chars.size; i ++) {
    global.comments << chars[i] << "\n";
  }
  G.characterTable = chars;
  for (int i = 0; i < G.characterTable.size; i ++) {
    global.comments << G.characterTable[i] << "\n";
  }
  return chars;
}

template <typename somegroup>
Matrix<Rational> getClassMatrix(const somegroup &G, int cci, List<int>* classmap = nullptr) {
  List<int> invl;
  int classSize = - 1;
  G.conjugacyClasses[cci].size.isIntegerFittingInInt(&classSize);
  invl.setSize(classSize);
  for (int i = 0; i < G.conjugacyClasses[cci].size; i ++) {
    invl[i] = G.invert(G.theElements.getIndex(G.conjugacyClasses[cci].theElements[i]));
  }
  Matrix<int> M;
  M.makeZeroMatrix(G.conjugacyClassCount());
  for (int t = 0; t < G.conjugacyClassCount(); t ++)
    for (int xi = 0; xi < invl.size; xi ++) {
      int yi = G.multiplyElements(invl[xi], G.theElements.getIndex(G.conjugacyClasses[t].representative));
      int ci;
      if (classmap) {
        M.elements[t][(*classmap)[yi]] += 1;
      } else {
        for (ci = 0; ci < G.conjugacyClassCount(); ci ++) {
          if (G.conjugacyClasses[ci].indicesEltsInOwner.BSContains(yi)) {
            M.elements[t][ci] += 1;
            break;
          }
        }
      }
    }
  Matrix<Rational> out;
  out.initialize(M.numberOfRows, M.numberOfColumns);
  for (int i = 0; i < M.numberOfRows; i ++) {
    for (int j = 0; j < M.numberOfColumns; j ++) {
      out.elements[i][j] = M.elements[i][j];
    }
  }
  return out;
}

void getTauSignaturesFromSubgroup(WeylGroupData& G, const List<ElementWeylGroup>& gens, List<bool>& out) {
  /*List<ElementWeylGroup<WeylGroup>> genes;
  genes.setSize(gens.size);
  for (int i = 0; i <gens.size; i ++)
    genes[i] = G.theElements[gens[i]];
  FiniteGroup<ElementWeylGroup<WeylGroup>> H;
  H.MakeFrom(genes);
  Vector<Rational> HXs;
  H.getSignCharacter(HXs);*/

  SubgroupData<FiniteGroup<ElementWeylGroup>, ElementWeylGroup> HD;
  HD.initFromGroupAndGenerators(G.group, gens);
  FiniteGroup<ElementWeylGroup>& H = *HD.theSubgroup;
  H.computeAllElements(true, - 1);
  Vector<Rational> HXs;
  H.getSignCharacter(HXs);

  List<int> ccPreimages;
  ccPreimages.setSize(H.conjugacyClassCount());
  for (int i = 0; i < H.conjugacyClassCount(); i ++) {
    bool notFound = true;
    for (int ci = 0; notFound && ci < G.group.conjugacyClassCount(); ci ++) {
      for (int cj = 0; notFound && cj < G.group.conjugacyClasses[ci].size; cj ++) {
        if (G.group.conjugacyClasses[ci].elements[cj] == H.conjugacyClasses[i].representative) {
          ccPreimages[i] = ci;
          notFound = false;
        }
      }
    }
    if (notFound) {
      global.fatal << "Something went very wrong: couldn't find preimage of conjugacy class of subgroup. " << global.fatal;
    }
  }
  out.setSize(G.group.characterTable.size);
  Vector<Rational> HXi;
  HXi.setSize(H.conjugacyClassCount());
  for (int i = 0; i < G.group.characterTable.size; i ++) {
    Vector<Rational> GXi = G.group.characterTable[i].data;
    for (int j = 0; j < HXi.size; j ++) {
      HXi[j] = GXi[ccPreimages[j]];
    }
    if (H.getHermitianProduct(HXs, HXi) == 0) {
      out[i] = false;
    } else {
      out[i] = true;
    }
  }
}

void computeTauSignatures(WeylGroupData* G, List<List<bool> >& tauSignatures, bool pseudo = false) {
  Selection sel;
  sel.initialize(G->cartanSymmetric.numberOfColumns);
  int numCycles = MathRoutines::twoToTheNth(sel.numberOfElements);
  List<List<bool> > tss;
  tss.setSize(numCycles);
  List<ElementWeylGroup> theGenerators;
  for (int i = 0; i < numCycles - 2; i ++) {
    sel.incrementSelection();
    theGenerators.setSize(sel.cardinalitySelection);
    for (int j = 0; j < sel.cardinalitySelection; j ++) {
      theGenerators[j].makeSimpleReflection(sel.elements[j], *G);
    }
    getTauSignaturesFromSubgroup(*G,theGenerators,tss[i]);
  }
  Vector<Rational> Xs;
  G->getSignCharacter(Xs);
  List<bool> tsg;
  tsg.setSize(G->group.characterTable.size);
  for (int i = 0; i < G->group.characterTable.size; i ++) {
    tsg[i] = G->group.characterTable[i].data == Xs;
  }
  tss.addOnTop(tsg);

  if (pseudo) {
    global.comments << "pseudo-parabolics" << "\n";
    ElementWeylGroup hr = G->getRootReflection(G->rootSystem.size- 1);
    sel.initialize(G->cartanSymmetric.numberOfColumns);
    for (int i = 0; i < numCycles - 1; i ++) {
      theGenerators.setSize(sel.cardinalitySelection);
      for (int j = 0; j < sel.cardinalitySelection; j ++) {
        theGenerators[j].makeSimpleReflection(sel.elements[j], *G);
      }
      theGenerators.addOnTop(hr);
      List<bool> ts;
      getTauSignaturesFromSubgroup(*G,theGenerators,ts);
      tss.addOnTop(ts);
      sel.incrementSelection();
    }
  }

  // we will need the sign character for the group


  tauSignatures.setSize(G->group.characterTable.size);
  for (int i = 0; i < G->group.characterTable.size; i ++) {
    tauSignatures[i].setSize(tss.size + 1);
    tauSignatures[i][0] = 1;
    for (int j = 1; j < tss.size + 1; j ++) {
      tauSignatures[i][j] = tss[j - 1][i];
    }
  }
  for (int i = 0; i < G->group.characterTable.size; i ++) {
    global.comments << G->group.characterTable[i] << "\n";
    for (int j = 0; j < tauSignatures[i].size; j ++) {
      global.comments << tauSignatures[i][j] << ' ';
    }
    global.comments << "\n";
  }
}

template <typename elementSomeGroup>
void ExportCharTable(FiniteGroup<elementSomeGroup>& G, JSData& data) {
  data.elementType = JSData::token::tokenObject;
  JSData& representatives = data.objects.getValueCreate("representatives");
  JSData& sizes = data.objects.getValueCreate("sizes");
  JSData& characters = data.objects.getValueCreate("characters");
  representatives.elementType = JSData::token::tokenArray;
  representatives.listObjects.setSize(G.conjugacyClassCount());

  for (int i = 0; i < G.conjugacyClassCount(); i ++) {
    List<int> reprefs;
    G.getWord(G.conjugacyClasses[i].representative, reprefs);
    representatives.listObjects[i].elementType = JSData::token::tokenArray;
    representatives.listObjects[i].listObjects.setSize(reprefs.size);
    for (int j = 0; j < reprefs.size; j ++) {
      representatives.listObjects[i].listObjects[j].elementType = JSData::token::tokenLargeInteger;
      representatives.listObjects[i].listObjects[j].theInteger.getElement() = reprefs[j];
    }
  }
  sizes.elementType = JSData::token::tokenArray;
  sizes.listObjects.setSize(G.conjugacyClassCount());
  for (int i = 0; i < G.conjugacyClassCount(); i ++) {
    sizes.listObjects[i].elementType = JSData::token::tokenLargeInteger;
    sizes.listObjects[i].theInteger.getElement() = G.conjugacyClasses[i].size;
  }
  characters.elementType = JSData::token::tokenArray;
  characters.listObjects.setSize(G.characterTable.size);
  for (int i = 0; i < G.characterTable.size; i ++) {
    for (int j = 0; j < G.characterTable[i].data.size; j ++) {
      characters[i][j] = G.characterTable[i][j].getNumerator();
    }
  }
}

void exportTauSignatures(WeylGroupData& G, const List<List<bool> >& ts, JSData &data) {
  ExportCharTable(G.group, data["chartable"]);
  for (int i = 0; i < ts.size; i ++) {
    for (int j = 0; j < ts[i].size; j ++) {
      data["tausigs"][i][j] = ts[i][j];
    }
  }
}

#endif
