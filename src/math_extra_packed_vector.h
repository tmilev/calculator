// The current file is licensed under the license terms found in the main header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_math_extra_packed_vector_ALREADY_INCLUDED
#define header_math_extra_packed_vector_ALREADY_INCLUDED

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
  scalar scalarProduct(
    const PackedVector<scalar>& v, const Matrix<scalar>& B
  ) const;
  unsigned int hashFunction() const;
  static unsigned int hashFunction(const PackedVector<scalar>& in);
};

// In particular, scalar likes to be int
// Note by Todor: we have to think of a technical way to merge your code with
// WeylGroup.
// I think adding a templateVector like you did should be straightforward.
// For the time being I will not proceed with a merge.
template <typename scalar, typename templateVector>
class AnotherWeylGroup {
  // compat WeylGroup
  int sizePrivate;
public:
  Matrix<Rational> cartanSymmetric;
  templateVector rho;
  List<templateVector> rootSystem;
  HashedList<templateVector> rhoOrbit;
  // <-Note from Todor: I don't remember if I changed this to be a Hashed list.
  List<List<int> > conjugacyClasses;
  List<int> classMap;
  List<Matrix<Rational> > classMatrices;
  // needed
  List<Vector<Rational> > characterTable;
  AnotherWeylGroup(): sizePrivate(- 1) {}
  int getRank() const;
  // idk lol
  void simpleReflection(
    int i, const templateVector& v, templateVector& out
  ) const;
  void simpleReflection(int i, templateVector& v) const;
  void getSimpleReflections(const templateVector& v, List<int>& out) const;
  void getWord(int i, List<int>& out) const;
  void applyReflectionList(
    const List<int>& simpleReflections, templateVector& v
  ) const;
  void actOn(int g, templateVector& v) const;
  void actOn(int g, const templateVector& v, templateVector& out) const;
  int multiplyElements(int g, int h) const;
  // <-multiply elements is better name than multiply (groups can be
  // tensored/direct product multiplied).
  // multiplyElements is unambiguous.
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
  void computeClassMatrices();
  // O(W.size)
  void computeTauSignatures();
  Rational getHermitianProduct(
    const Vector<Rational>& left, const Vector<Rational>& right
  ) const;
  int getRootReflection(int i) const;
  void getSignCharacter(Vector<Rational>& out);
  // own stuff
  int size() const {
    // useful b/c its nice to not accidentally depend on logically stuff
    return this->sizePrivate;
  }
  int rank;
  templateVector twiceRho;
  Matrix<scalar> unrationalCartanSymmetric;
  templateVector* ucsm;
};

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::makeArbitrarySimple(
  char letter, int number
) {
  DynkinType D;
  D.makeSimpleType(letter, number);
  D.getCartanSymmetric(this->cartanSymmetric);
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::simpleReflection(
  int i, templateVector& v
) const {
  scalar x = 0;
  for (int j = 0; j < this->rank; j ++) {
    x += this->unrationalCartanSymmetric.elements[i][j] * v[j];
  }
  // so, under ordinary circumstances, this is just out[i] -= x;
  // silent corruption occurs if UnrationalCartanSymmetric.elements[i][i] does
  // not belong to {1, 2}
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
void AnotherWeylGroup<scalar, templateVector>::getSimpleReflections(
  const templateVector& v, List<int>& out
) const {
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
void AnotherWeylGroup<scalar, templateVector>::getWord(
  int i, List<int>& out
) const {
  this->getSimpleReflections(this->rhoOrbit[i], out);
}

// copypaste another function into this so the autovectorizer will work
template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::applyReflectionList(
  const List<int>& simpleReflections, templateVector& v
) const {
  for (int ii = simpleReflections.size - 1; ii >= 0; ii --) {
    // this->simpleReflection(simpleReflections[i],v);
    int i =
    simpleReflections[ii];
    scalar x = 0;
    for (int j = 0; j < this->rank; j ++) {
      x += this->unrationalCartanSymmetric.elements[i][j] * v[j];
    }
    // so, under ordinary circumstances, this is just out[i] -= x;
    // silent corruption occurs if UnrationalCartanSymmetric.elements[i][i] !∈
    // {1,2}
    // and scalar can't deal with division properly
    // fortunately this only happens in G₂
    v[i] -= x * 2 / this->unrationalCartanSymmetric.elements[i][i];
  }
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::actOn(
  int i, templateVector& v
) const {
  List<int> simpleReflections;
  getSimpleReflections(this->rhoOrbit[i], simpleReflections);
  this->applyReflectionList(simpleReflections, v);
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::actOn(
  int i, const templateVector& v, templateVector& out
) const {
  out = v;
  this->actOn(i, out);
}

template <typename scalar, typename templateVector>
int AnotherWeylGroup<scalar, templateVector>::multiplyElements(int i, int j)
const {
  templateVector v = this->rhoOrbit[j];
  this->actOn(i, v);
  return this->rhoOrbit.getIndex(v);
}

template <typename scalar, typename templateVector>
int AnotherWeylGroup<scalar, templateVector>::invert(int i) const {
  List<int> srsl;
  getSimpleReflections(this->rhoOrbit[i], srsl);
  templateVector v = this->twiceRho;
  for (int i = srsl.size - 1; i >= 0; i --) {
    this->simpleReflection(srsl[i], v);
  }
  return this->rhoOrbit.getIndex(v);
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::computeRho() {
  if (
    this->unrationalCartanSymmetric.numberOfRows < cartanSymmetric.numberOfRows
  ) {
    int den = 1;
    for (int i = 0; i < this->cartanSymmetric.numberOfRows; i ++) {
      for (int j = 0; j < this->cartanSymmetric.numberOfColumns; j ++) {
        den =
        MathRoutines::leastCommonMultiple(
          den,
          cartanSymmetric.elements[i][j].getDenominator().
          getUnsignedIntValueTruncated()
        );
      }
    }
    unrationalCartanSymmetric.initialize(
      cartanSymmetric.numberOfRows, cartanSymmetric.numberOfColumns
    );
    // this->ucsm = malloc(cartanSymmetric.numberOfRows*sizeof(templateVector));
    // for (int i = 0; i <cartanSymmetric.numberOfRows; i ++)
    //  this->ucsm[i].setSize(cartanSymmetric.numberOfColumns);
    for (int i = 0; i < this->cartanSymmetric.numberOfRows; i ++) {
      for (int j = 0; j < this->cartanSymmetric.numberOfColumns; j ++) {
        this->unrationalCartanSymmetric.elements[i][j] = (
          cartanSymmetric.elements[i][j] * den
        ).getNumerator().getIntValueTruncated();
        // ucsm[i][j] = unrationalCartanSymmetric.elements[i][j];
      }
    }
  }
  global.comments << "symmetric Cartan matrix is" << "\n";
  for (int i = 0; i < this->unrationalCartanSymmetric.numberOfRows; i ++) {
    for (
      int j = 0; j < this->unrationalCartanSymmetric.numberOfColumns; j ++
    ) {
      global.comments << this->unrationalCartanSymmetric.elements[i][j] << '\t'
      ;
    }
    global.comments << '\n';
  }
  this->rank = this->unrationalCartanSymmetric.numberOfRows;
  for (int rvi = 0; rvi < rank; rvi ++) {
    templateVector vi;
    vi.makeEi(rank, rvi);
    if (this->rootSystem.contains(vi)) {
      continue;
    }
    this->rootSystem.addOnTop(vi);
    List<int> newelts;
    templateVector w;
    newelts.addOnTop(this->rootSystem.getIndex(vi));
    while (newelts.size > 0) {
      int i = newelts.popLastObject();
      for (int si = 0; si < this->rank; si ++) {
        this->simpleReflection(si, this->rootSystem[i], w);
        if (this->rootSystem.getIndex(w) == - 1) {
          this->rootSystem.addOnTop(w);
          int j = this->rootSystem.getIndex(w);
          newelts.addOnTop(j);
        }
      }
    }
  }
  global.comments << "root system is" << "\n";
  global.comments << this->rootSystem << "\n";
  this->rootSystem.quickSortAscending();
  this->twiceRho.makeZero(rank);
  for (int i = 0; i < this->rootSystem.size; i ++) {
    bool usethis = true;
    for (int j = 0; j < this->rootSystem[i].size; j ++) {
      if (this->rootSystem[i][j] < 0) {
        usethis = false;
        break;
      }
    }
    if (usethis) {
      this->twiceRho += this->rootSystem[i];
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
  if (this->rootSystem.size == 0) {
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
      this->simpleReflection(si, this->rhoOrbit[i], w);
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
  List<bool> accounted;
  accounted.initializeFillInObject(this->size(), false);
  this->conjugacyClasses.setSize(0);
  this->conjugacyClasses.reserve(50);
  HashedList<int, HashFunctions::hashFunction> stack;
  stack.setExpectedSize(this->size());
  int rank = this->getRank();
  templateVector rhoImage;
  global.comments << "number of conjugacy classes... ";
  for (int i = 0; i < this->size(); i ++) {
    if (!accounted[i]) {
      stack.clear();
      stack.addOnTop(i);
      for (int j = 0; j < stack.size; j ++) for (int k = 0; k < rank; k ++) {
        rhoImage = this->twiceRho;
        this->simpleReflection(k, rhoImage);
        this->actOn(stack[j], rhoImage);
        this->simpleReflection(k, rhoImage);
        int accountedIndex = this->rhoOrbit.getIndex(rhoImage);
        stack.addOnTopNoRepetition(accountedIndex);
        accounted[accountedIndex] = true;
      }
      this->conjugacyClasses.addOnTop(stack);
      this->conjugacyClasses.lastObject()->quickSortAscending();
      global.comments << this->conjugacyClassCount() << " ";
    }
  }
  this->conjugacyClasses.quickSortAscending();
  global.comments << this->conjugacyClassCount() << "\n";
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::computeClassMap() {
  this->classMap.setSize(this->size);
  for (int i = 0; i < this->conjugacyClassCount(); i ++) {
    for (int j = 0; j < this->conjugacyClasses[i].size; j ++) {
      this->classmap[this->conjugacyClasses[i][j]] = i;
    }
  }
}

template <typename scalar, typename templateVector>
Matrix<Rational>& AnotherWeylGroup<scalar, templateVector>::getClassMatrix(
  int cc
) {
  if (this->classMatrices.size == 0) {
    if (this->conjugacyClassCount() == 0) {
      this->computeConjugacyClasses();
    }
    this->classMatrices.setSize(this->conjugacyClassCount());
  }
  if (this->classMap.size == 0) {
    this->computeClassMap();
  }
  this->classMatrices[cc] =
  getClassMatrix(*this, cc, &(this->classmap));
  return this->classMatrices[cc];
}

template <typename scalar, typename templateVector>
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
  scalar x =
  this->rootSystem[i].scalarProduct(
    this->twiceRho, this->unrationalCartanSymmetric
  );
  x *= - 2;
  x /=
  this->rootSystem[i].scalarProduct(
    this->rootSystem[i], this->unrationalCartanSymmetric
  );
  return this->rhoOrbit.getIndexNoFail(twiceRho + this->rootSystem[i] * x);
}

template <typename scalar, typename templateVector>
void AnotherWeylGroup<scalar, templateVector>::getSignCharacter(
  Vector<Rational>& out
) {
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

// there are like 3 copies of this function with slightly different argument
// types
template <typename scalar, typename templateVector>
Rational AnotherWeylGroup<scalar, templateVector>::getHermitianProduct(
  const Vector<Rational>& left, const Vector<Rational>& right
) const {
  Rational acc = 0;
  for (int i = 0; i < left.size; i ++) {
    acc +=
    left[i].getComplexConjugate() * right[i] * this->conjugacyClasses[i].size;
  }
  return acc / this->size();
}

#endif // header_math_extra_packed_vector_ALREADY_INCLUDED

