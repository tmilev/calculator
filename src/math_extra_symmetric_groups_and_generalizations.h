// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfSymmetricGroupsAndGeneralizationsHeader_alreadyIncluded
#define vpfSymmetricGroupsAndGeneralizationsHeader_alreadyIncluded
#include "general_lists.h"
#include "math_general.h"
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_extra_vector_space.h"
#include "math_extra_graph.h"
#include "math_extra_finite_groups.h"
#include "math_extra_finite_groups_implementation.h"

// Dense bases for sparse spaces
// should this carry around the
// List<LinearCombination<TemplateMonomial, Coefficient> >
// it came from?
template <class templateVector, class TemplateMonomial, class Coefficient>
class SparseSubspaceBasis {
public:
  bool flagDeallocated;
  int rank;
  List<TemplateMonomial> involvedMonomials;
  Matrix<Coefficient> projectionOperator;

  void setBasis(const List<templateVector>& basis);
  void denseVectorInBasis(Vector<Coefficient>& out, const templateVector& in);
  bool checkConsistency() const {
    if (this->flagDeallocated) {
      global.fatal << "Use of SparseSubspaceBasis after free. " << global.fatal;
    }
    for (int i = 0; i < this->involvedMonomials.size; i ++) {
      this->involvedMonomials[i].checkConsistency();
    }
    this->projectionOperator.checkConsistency();
    return true;
  }
  SparseSubspaceBasis() {
    this->flagDeallocated = false;
  }
  ~SparseSubspaceBasis() {
    this->flagDeallocated = true;
  }
  template <typename somestream>
  somestream& intoStream(somestream& out) const;
  std::string toString() const;
};

template <class templateVector, class TemplateMonomial, class Coefficient>
void SparseSubspaceBasis<templateVector, TemplateMonomial, Coefficient>::setBasis(const List<templateVector>& basis) {
  this->checkConsistency();
  if (basis.size == 0) {
    return;
  }
  for (int i = 0; i < basis.size; i ++) {
    for (int j = 0; j < basis[i].monomials.size; j ++) {
      this->involvedMonomials.BSInsertDontDup(basis[i].monomials[j]);
    }
  }
  Matrix<Coefficient> basisMatrix;
  basisMatrix.initialize(this->involvedMonomials.size, basis.size);
  for (int j = 0; j < basis.size; j ++) {
    for (int i = 0; i < involvedMonomials.size; i ++) {
      basisMatrix.elements[i][j] = basis[j].getCoefficientOf(involvedMonomials[i]);
    }
  }
  Matrix<Coefficient> basisMatrixT = basisMatrix;
  basisMatrixT.transpose();
  basisMatrix.multiplyOnTheLeft(basisMatrixT,this->projectionOperator);
  this->projectionOperator.invert();
  this->projectionOperator *= basisMatrixT;
  this->rank = basis.size;
}

template <class templateVector, class TemplateMonomial, class Coefficient>
void SparseSubspaceBasis<templateVector, TemplateMonomial, Coefficient>::denseVectorInBasis(
  Vector<Coefficient>& out, const templateVector& in
) {
  Vector<Coefficient> inDense;
  inDense.setSize(this->involvedMonomials.size);
  for (int i = 0; i < involvedMonomials.size; i ++) {
    inDense[i] = in.getCoefficientOf(involvedMonomials[i]);
  }
  out = this->projectionOperator * inDense;
}

template <class templateVector, class TemplateMonomial, class Coefficient>
template <typename somestream>
somestream& SparseSubspaceBasis<templateVector, TemplateMonomial, Coefficient>::intoStream(somestream& out) const {
  out << "Sparse subspace basis object for basis of rank " << this->rank;
  out << " involving monomials " << this->involvedMonomials.toStringCommaDelimited() << '\n';
  out << " with projection operator\n" << this->projectionOperator.toStringPlainText() << '\n';
  return out;
}

template <class templateVector, class TemplateMonomial, class Coefficient>
std::string SparseSubspaceBasis<templateVector, TemplateMonomial, Coefficient>::toString() const {
  std::stringstream out;
  this->intoStream(out);
  return out.str();
}

template <class templateVector, class TemplateMonomial, class Coefficient>
std::ostream& operator<<(std::ostream& out, const SparseSubspaceBasis<templateVector, TemplateMonomial, Coefficient>& data) {
  return data.intoStream(out);
}

class Partition;
class Tableau;
class PermutationR2;
class PermutationGroup;

// Partitions are ordered from largest to smallest
class Partition {
public:
  int n;
  List<int> p;

  int& operator[](int i) const;
  void fromListInt(const List<int>& in, int lastElement = - 1);
  static void GetPartitions(List<Partition>& out, int n);
  void transpose();
  void fillTableau(Tableau& out, List<int>& stuffing) const;
  void fillTableauOrdered(Tableau& out) const;
  void getAllStandardTableaux(List<Tableau>& out) const;
  template <typename scalar>
  void spechtModuleMatricesOfTranspositions1j(List<Matrix<scalar> >& out) const;
  template <typename scalar>
  void spechtModuleMatricesOfTranspositionsjjplusone(List<Matrix<scalar> >& out) const;
  template <typename scalar>
  void spechtModuleMatricesOfPermutations(List<Matrix<scalar> >& out, const List<PermutationR2>& perms) const;
  template <typename scalar>
  void spechtModuleMatrixOfPermutation(List<Matrix<scalar> >& out, const PermutationR2& p) const;
  static void testAllSpechtModules(int n);
  // int might not be wide enough
  int fulton61z() const;
  bool operator== (const Partition& right) const;
  bool operator!= (const Partition& right) const;
  bool operator<(const Partition& right) const;
  bool operator>(const Partition& right) const;

  template <typename somestream>
  somestream& intoStream(somestream& out) const;
  std::string toString() const;
  std::string toStringForArticles(
    const std::string& leftParenthesis = "[",
    const std::string& rightParenthesis = "]"
  ) const;
  friend std::ostream& operator<<(std::ostream& out, const Partition& data) {
    return data.intoStream(out);
  }
  unsigned int hashFunction() const {
    return MathRoutines::hashListInts(this->p);
  }
  static unsigned int hashFunction(const Partition& input) {
    return input.hashFunction();
  }
};

// Tableau methods assume the partition ordering from largest to smallest
class Tableau {
public:
  List<List<int> > t;

  bool isStandard() const;
  void getColumns(List<List<int> >& output) const;
  void rowStabilizer(FiniteGroup<PermutationR2>& in) const;
  void columnStabilizer(FiniteGroup<PermutationR2>& in) const;
  template <typename Coefficient>
  void youngSymmetrizerAction(
    ElementMonomialAlgebra<MonomialTensor<int, MathRoutines::IntUnsignIdentity>, Coefficient>& out,
    const ElementMonomialAlgebra<MonomialTensor<int, MathRoutines::IntUnsignIdentity>, Coefficient>& in
  );
  List<int> turnIntoList() const;
  template <typename somestream>
  somestream& intoStream(somestream& out) const;
  std::string toString() const;
  friend std::ostream& operator<<(std::ostream& out, const Tableau& data) {
    return data.intoStream(out);
  }
};

// Permutations are expected to be represented as a list of cycles
// with the smallest numbers coming first.  each cycle is expected
// to be at least a transposition
// Permutations are also elements of the direct limit of the symmetric groups
class PermutationR2 {
public:
  List<List<int> > cycles;

  // the name makeCanonical clashes with makeFromMultiplicities and MakeFromTableau
  // but it seems to be typical of this project
  void makeCanonical();
  unsigned int hashFunction() const;
  static unsigned int hashFunction(const PermutationR2& in) {
    return in.hashFunction();
  }
  bool operator== (const PermutationR2& right) const;
  bool isIdentity() const;
  int largestOccurringNumber() const;
  int operator*(int i) const;
  int sign() const;
  void buildTransposition(int i, int j);
  void buildCycle(const List<int>& cycle);
  // Build operations leave the Permutation in a possibly not canonical state
  // you must makeCanonical() after using them to get virtually anything to work
  // properly.
  void addTransposition(int i, int j);
  void addCycle(const List<int>& cycle);
  void makeFromListOfCycles(const List<List<int> >& inCycles);
  // A jumbled up list of integers 0-n
  void makeFromActionDescription(const List<int>& actionDescription);
  void makeFromString(const std::string& in);
  // Cycle structure as a histogram of cycle lengths
  void getCycleStructure(List<int>& out) const;
  // Cycle structure as the related partition
  void getCycleStructure(Partition& out, int n_in_Sn = - 1) const;
  // Row by row
  void makeFromTableauRows(const Tableau& in);
  // this type is hard to *=
  // we are returing the smallest n such that this is an element of Sn
  int makeFromMultiplicities(const PermutationR2& left, const PermutationR2& right);
  void makeIdentity(const PermutationR2& unused);
  // the purpose of this is compat with other element classes that need
  // to see some things before they can be a proper identity element
  PermutationR2 inverse() {
    PermutationR2 copy = *this;
    copy.invert();
    return copy;
  }
  void invert();
  // some day i'm going to move the out to the first parameter everywhere
  // after i find out why it was put last this time
  PermutationR2 operator^(const PermutationR2& right) const;
  static void conjugationAction(const PermutationR2& conjugateWith, const PermutationR2& conjugateOn, PermutationR2& out);

  // Should this be operator*(), operator()(), or just actOn()?
  // I vote operator()() for an action, but I chose operator*() for some reason
  // and thanks to the nature of operator overloading, it's too late to change
  PermutationR2 operator*(const PermutationR2& right) const;

  static bool areConjugate(const PermutationR2& x, const PermutationR2& y);

  template <typename Object>
  void actOnList(List<Object>& in) const;
  //  MonomialTensor<T1, T2> operator*(MonomialTensor<T1,T2>& right) const;
  void actOnMonomialTensor(
    MonomialTensor<int, MathRoutines::IntUnsignIdentity>& out,
    const MonomialTensor<int, MathRoutines::IntUnsignIdentity>& in
  ) const;

  template <typename Coefficient>
  void actOnTensor(
    ElementMonomialAlgebra<MonomialTensor<int, MathRoutines::IntUnsignIdentity>, Coefficient>& out,
    const ElementMonomialAlgebra<MonomialTensor<int, MathRoutines::IntUnsignIdentity>, Coefficient>& in
  ) const;
  void getWordjjPlus1(List<int>& word) const;

  template <typename Coefficient>
  void getCharacteristicPolynomialStandardRepresentation(Polynomial<Coefficient>& out) {
    Matrix<Coefficient> matrix;
    int n = this->largestOccurringNumber() + 1;
    matrix.makeZeroMatrix(n);
    for (int i = 0; i < n; i ++) {
      matrix(i, (*this) * i) = 1;
    }
    out.assignCharacteristicPolynomial(matrix);
  }

  bool hasDifferentConjugacyInvariantsFrom(PermutationR2& other) {
    List<int> a, b;
    this->getCycleStructure(a);
    other.getCycleStructure(b);
    return !(a == b);
  }

  // this is strictly for sorting purposes, of course
  bool operator>(const PermutationR2& other) const {
    return this->cycles > other.cycles;
  }
  template <typename somestream>
  somestream& intoStream(somestream& out) const;
  std::string toString(FormatExpressions* format = nullptr) const;
  friend std::ostream& operator<<(std::ostream& out, const PermutationR2& data) {
    return data.intoStream(out);
  }
};

template <typename helt, typename kelt>
class TrivialOuterAutomorphism {
  kelt oa(helt& x, kelt& y) {
    (void) x;
    kelt z = y;
    return z;
  }

  template <typename somestream>
  somestream& intoStream(somestream& out) {
    out << "Identity function";
  }
  std::string toString(FormatExpressions* theFormat = nullptr) const {
    (void) theFormat;
    std::stringstream ss;
    ss << *this;
    return ss.str();
  }
  friend std::ostream& operator<<(std::ostream& out, const TrivialOuterAutomorphism<helt,kelt>& data) {
    out << data.toString();
    return out;
  }
};

// see operator* for how this is supposed to work
template <typename helt, typename kelt, typename oa>
class SemidirectProductElement {
public:
  helt h;
  kelt k;

  SemidirectProductElement<helt, kelt, oa> operator*(const SemidirectProductElement<helt, kelt, oa>& right) const {
    SemidirectProductElement<helt, kelt, oa> out;
    out.h = this->h * right.h;
    out.k = oa::oa(right.h, this->k) * right.k;
    return out;
  }

  void invert() {
    this->h.invert();
    this->k = oa::oa(this->h, this->k);
    this->k.invert();
  }
  SemidirectProductElement<helt,kelt,oa> inverse() {
    SemidirectProductElement<helt,kelt,oa> copy =*this;
    copy.invert();
    return copy;
  }

  SemidirectProductElement<helt,kelt,oa> operator^(const SemidirectProductElement<helt, kelt, oa>& right) const {
    auto inv = right;
    inv.invert();
    return right * (*this) * inv;
  }

  static void conjugationAction(
    const SemidirectProductElement<helt, kelt, oa>& conjugateWith,
    const SemidirectProductElement<helt, kelt, oa>& conjugateOn,
    SemidirectProductElement<helt, kelt, oa>& out
  ) {
    out = conjugateOn ^ conjugateWith;
  }

  bool operator==(const SemidirectProductElement<helt, kelt, oa> right) const {
    return (this->h == right.h) && (this->k == right.k);
  }
  bool operator>(const SemidirectProductElement<helt, kelt, oa> right) const {
    if (this->h > right.h) {
      return true;
    }
    if (right.h > this->h) {
      return false;
    }
    if (this->k > right.k) {
      return true;
    }
    if (right.k > this->k) {
      return false;
    }
    return false;
  }

  void makeIdentity(const SemidirectProductElement<helt, kelt, oa>& prototype) {
    this->k.makeIdentity(prototype.k);
    this->h.makeIdentity(prototype.h);
  }

  bool isIdentity() const {
    return this->k.isIdentity() && this->h.isIdentity();
  }

  std::string toString(FormatExpressions* format = nullptr) const {
    std::stringstream out;
    char leftDelimiter = '[';
    char rightDelimiter = ']';
    if (format != nullptr) {
      leftDelimiter ='(';
      rightDelimiter =')';
    }
    out << leftDelimiter;
    out << h.toString(format);
    out << ',';
    out << k.toString(format);
    out << rightDelimiter;
    return out.str();
  }

  void makeFromString(const std::string& in) {
    (void) in;
    global.fatal << "not implemented yet" << global.fatal;
  }

  friend std::ostream& operator<<(std::ostream& s, const SemidirectProductElement<helt, kelt, oa>& in) {
    return s << in.toString();
  }
  unsigned int hashFunction() const {
    return 2 * this->h.hashFunction() + 3 * this->k.hashFunction();
  }
  static unsigned int hashFunction(const SemidirectProductElement<helt, kelt, oa>& in) {
    return in.hashFunction();
  }
  bool hasDifferentConjugacyInvariantsFrom(const SemidirectProductElement& other) const {
    (void) other;
    return false;
  }
  template <typename Coefficient>
  void getCharacteristicPolynomialStandardRepresentation(Polynomial<Coefficient>& theCharPoly) {
    Polynomial<Rational> p;
    this->h.getCharacteristicPolynomialStandardRepresentation(theCharPoly);
    this->k.getCharacteristicPolynomialStandardRepresentation(p);
    theCharPoly *= p;
  }
};

template <typename helt, typename kelt>
class DirectProductElement: SemidirectProductElement<helt, kelt, TrivialOuterAutomorphism<helt, kelt> > {
};

template <typename hg, typename kg, typename helt, typename kelt, typename oa>
class SemidirectProductGroup: public FiniteGroup<SemidirectProductElement<helt, kelt, oa> > {
public:
  hg* H;
  kg* K;
  void initialize(hg* inH, kg* inK);
  void getWord(SemidirectProductElement<helt, kelt, oa>& g, List<int>& out);
};

template <typename hg, typename kg, typename helt, typename kelt, typename oa>
void SemidirectProductGroup<hg, kg, helt, kelt, oa>::initialize(hg* inH, kg* inK) {
  this->H = inH;
  this->K = inK;
  this->generators.setSize(inH->generators.size + inK->generators.size);
  int i = 0;
  for (; i < this->H.generators.size; i ++) {
    this->generators[i].h = this->H.generators[i];
    this->generators[i].k = this->K.makeIdentity();
  }
  for (; i < this->H.generators.size + this->K.generators.size; i ++) {
    this->generators[i].h = this->H.makeIdentity();
    this->generators[i].k = this->K.generators[i-this->H.generators.size];
  }
}

template <typename hg, typename kg, typename helt, typename kelt, typename oa>
void SemidirectProductGroup<hg, kg, helt, kelt, oa>::getWord(SemidirectProductElement<helt, kelt, oa>& g, List<int>& out) {
  this->H.getWord(g.h, out);
  List<int> kword;
  this->K.getWord(g.k, kword);
  out.addListOnTop(kword);
}

template <typename hg, typename kg, typename helt, typename kelt>
class DirectProductGroup: public SemidirectProductGroup<hg, kg, helt, kelt, TrivialOuterAutomorphism<helt, kelt> > {
};

class ElementZ2N {
public:
  List<bool> bits;
  void validate() const {
    for (int i = 0; i < bits.size; i ++) {
      if ((bits[i] != true) && (bits[i] != false)) {
        global.fatal << "invalid bit " << bits[i] << global.fatal;
      }
    }
  }

  ElementZ2N operator*(const ElementZ2N right) const {
    ElementZ2N out;
    out.bits.setSize(MathRoutines::maximum(this->bits.size, right.bits.size));
    int i = 0;
    for (; i < MathRoutines::minimum(this->bits.size, right.bits.size); i ++) {
      out.bits[i] = this->bits[i] != right.bits[i];
    }
    for (; i < this->bits.size; i ++) {
      out.bits[i] = this->bits[i];
    }
    for (; i < right.bits.size; i ++) {
      out.bits[i] = right.bits[i];
    }
    out.validate();
    return out;
  }

  void toggleBit(int i) {
    if (i < this->bits.size) {
      this->bits[i] = !this->bits[i];
    } else {
      int os = this->bits.size;
      this->bits.setSize(i + 1);
      for (int ii = os; ii < i; ii ++) {
        this->bits[ii] = false;
      }
      this->bits[i] = true;
    }
    this->validate();
  }

  // an ElementZ2N is its own inverse.  When it comes time to template ElementZmN
  // with class ElementZ2N: public ElementZmN<2>, this will need to be replaced with
  // x -> m - x
  void invert() {
  }

  bool operator== (const ElementZ2N& right) const {
    int i = 0;
    for (; i < MathRoutines::minimum(this->bits.size, right.bits.size); i ++) {
      if (this->bits[i] != right.bits[i]) {
        return false;
      }
    }
    for (; i < this->bits.size; i ++) {
      if (this->bits[i]) {
        return false;
      }
    }
    for (; i < right.bits.size; i ++) {
      if (right.bits[i]) {
        return false;
      }
    }
    return true;
  }
  void makeIdentity(const ElementZ2N& unused) {
    (void) unused;
    this->bits.setSize(0);
  }

  bool isIdentity() const {
    for (int i = 0; i < this->bits.size; i ++) {
      if (this->bits[i]) {
        return false;
      }
    }
    return true;
  }

  bool operator>(const ElementZ2N other) const {
    int i;
    int m = MathRoutines::minimum(this->bits.size, other.bits.size);
    for (i = 0; i < m; i ++) {
      if (this->bits[i] && !other.bits[i]) {
        return true;
      }
      if (!this->bits[i] && other.bits[i]) {
        return false;
      }
    }
    for (; i < this->bits.size; i ++) {
      if (this->bits[i]) {
        return true;
      }
    }
    return false;
  }

  std::string toString(FormatExpressions* format = nullptr) const {
    std::stringstream out;
    if (format == nullptr) {
      for (int i = 0; i < this->bits.size; i ++) {
        // parentheses are needed because << binds like a bitwise operator
        out << (this->bits[i] ? '1' : '0');
      }
    } else {
      for (int i = 0; i < this->bits.size; i ++) {
        out << (this->bits[i] ? '1' : '0');
        // parentheses are needed because << binds like a bitwise operator
        if (i != this->bits.size - 1) {
          out << ",";
        }
      }
    }
    return out.str();
  }

  void makeFromString(const std::string& in) {
    this->bits.setSize(0);
    for (unsigned i = 0; i < in.size(); i ++) {
      if (in[i] == '1') {
        this->toggleBit(static_cast<int>(i));
      }
    }
  }

  friend std::ostream& operator<<(std::ostream& s, const ElementZ2N& in) {
    return s << in.toString();
  }

  unsigned int hashFunction() const {
    return ElementZ2N::hashFunction(*this);
  }
  static unsigned int hashFunction(const ElementZ2N& in) {
    return in.bits.hashFunction();
  }
  template <typename Coefficient>
  void getCharacteristicPolynomialStandardRepresentation(Polynomial<Coefficient>& p) {
    int m = 1;
    for (int i = 0; i < this->bits.size; i ++) {
      if (this->bits[i]) {
        m *= - 1;
      }
    }
    MonomialP x;
    x.makeEi(0);
    p.addMonomial(x, 1);
    p.addConstant(- m);
  }
};

class HyperoctahedralBitsAutomorphism {
public:
  static ElementZ2N oa(const PermutationR2& h, const ElementZ2N& k) {
    ElementZ2N out = k;
    int bon = h.largestOccurringNumber();
    int os = out.bits.size;
    if (os <= bon) {
      out.bits.setSize(bon + 1);
      for (int i = os; i < bon + 1; i ++) {
        out.bits[i] = false;
      }
    }
    PermutationR2 hinv = h;
    hinv.invert();
    hinv.actOnList(out.bits);
    out.validate();
    return out;
  }
};

typedef SemidirectProductElement<PermutationR2, ElementZ2N, HyperoctahedralBitsAutomorphism> ElementHyperoctahedralGroupR2;

template <>
void ElementHyperoctahedralGroupR2::makeFromString(const std::string& in);

//class HyperoctahedralElementR2: public SemidirectProductElement<PermutationR2,ElementZ2N,HyperoctahedralBitsAutomorphism>
//{
//  unsigned int hashFunction() const {return SemidirectProductElement<PermutationR2,ElementZ2N,HyperoctahedralBitsAutomorphism>::hashFunction();}
//  static unsigned int hashFunction(const HyperoctahedralElementR2& in) {return in.hashFunction();}
//};

class HyperoctahedralGroupData {
public:
  int N;
  bool flagIsEntireHyperoctahedralGroup;
  bool flagIsEntireDn;
  FiniteGroup<ElementHyperoctahedralGroupR2>* theGroup;
  FiniteGroup<ElementHyperoctahedralGroupR2> theGroupMayBeHereNameIsLongToDiscourageUse;

  void makeHyperoctahedralGroup(int n) {
    this->theGroup = &theGroupMayBeHereNameIsLongToDiscourageUse;
    this->theGroup->specificDataPointer = this;
    this->N = n;
    this->theGroup->generators.setSize(n - 1 + n);
    for (int i = 0; i < n - 1; i ++) {
      this->theGroup->generators[i].h.addTransposition(i, i + 1);
    }
    for (int i = 0; i < n; i ++) {
      this->theGroup->generators[n - 1 + i].k.toggleBit(i);
    }
    this->flagIsEntireHyperoctahedralGroup = true;
    this->theGroup->getWordByFormula = this->getWordByFormulaImplementation;
    this->theGroup->getSizeByFormula = this->getSizeByFormulaImplementation;
  }

  static bool getWordByFormulaImplementation(
    FiniteGroup<ElementHyperoctahedralGroupR2>& G, const ElementHyperoctahedralGroupR2& g, List<int>& out
  );
  static LargeInteger getSizeByFormulaImplementation(FiniteGroup<ElementHyperoctahedralGroupR2>& G);
  void allSpechtModules();
  void spechtModuleOfPartititons(
    const Partition& positive,
    const Partition& negative,
    GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational>& out
  );
  bool operator==(const HyperoctahedralGroupData& other) const {
    if (!this->flagIsEntireHyperoctahedralGroup || !other.flagIsEntireHyperoctahedralGroup) {
      return false;
    }
    return this->N == other.N;
  }
  std::string toString() const;
  template <typename somestream>
  somestream& intoStream(somestream& outstream) const;
};
std::ostream& operator<<(std::ostream& out, const HyperoctahedralGroupData& data);


// superseded by SemidirectProductGroup<PermutationR2, ElementZ2N, HyperoctahedralBitsAutomorphism>
// A hyperoctahedral group is a semidirect product of a symmetric group and
// a group of bits.
// An unallocated ElementHyperoctahedralGroup does not have a bits field.
// Having no bits field is always acceptable and it simply means the bits
// are all unset.
// Multiplication is as folows: (p1, s1) * (p2, s2) = (p1 * p2, (p2 * s1) * s2)
// where the action of p on s comes from conjugation p⁻¹sp in the standard
// representation. For example, (0,1,2) conjugated with xyz gives yzx.
// An ElementHyperoctahedralGroup is not an element of any particular
// hyperoctahedral group, but rather of the direct limit of the hyperoctahedral
// groups.  It is the responsibility of the caller, of course, to ensure that
// all requested operations are defined.
// class ElementHyperoctahedralGroup

template <typename elementSomeGroup>
class ConjugacyClassR2 {
public:
  elementSomeGroup representative;
  int size;
  bool flagRepresentativeComputed;
  bool flagHaveRepresentativeWord;
  List<int> representativeWord;
  // the other members are not necessarily meaningful
  int representativeIndex;
  List<int> theElements;
  template <typename somestream>
  somestream& intoStream(somestream& out) const;
  std::string toString() const;
  ConjugacyClassR2() {
    this->flagRepresentativeComputed = false;
    this->flagHaveRepresentativeWord = false;
  }
};

template <typename object>
class GeneratorPermutationsOfList {
public:
  List<object> l;
  bool done_iterating;
  bool go_once;

  struct stack_frame {
    int c;
    int loop_i;
    int program_counter;
  };
  List<struct stack_frame> stack;
  int frame_pointer;

  enum pcpositions { beginning, loop, firstout, afterloop, end};

  void initialize(List<object>& theL) {
    if (theL.size == 0) {
      done_iterating = true;
      return;
    }
    this->l = theL;
    this->done_iterating = false;
    this->go_once = false;
    this->frame_pointer = 0;
    this->stack.setSize(this->l.size);
    this->stack[0].c = this->l.size - 1;
    this->stack[0].program_counter = 0;
    ++ (*this);
  }
  // this function is the only one that is mathematically interesting
  GeneratorPermutationsOfList& operator++() {
    while (true) {
      //varsout();
      if (frame_pointer == - 1) {
        if (this->l.size == 1) {
          if (!go_once) {
            go_once = true;
          } else {
            done_iterating = true;
          }
        } else {
          done_iterating = true;
        }
        return *this;
      }
      switch(stack[frame_pointer].program_counter) {
        case pcpositions::beginning:
          if (stack[frame_pointer].c == 0) {
            frame_pointer --;
            return *this;
          }
          stack[frame_pointer].loop_i = 0;
          break;
        case pcpositions::loop:
          if (stack[frame_pointer].loop_i == stack[frame_pointer].c) {
            stack[frame_pointer].program_counter = pcpositions::afterloop;
            break;
          }
          stack[frame_pointer].program_counter = pcpositions::firstout;
          frame_pointer ++;
          stack[frame_pointer].c = stack[frame_pointer - 1].c - 1;
          stack[frame_pointer].program_counter = pcpositions::beginning;
          break;
        case firstout:
          if (stack[frame_pointer].c % 2!= 0) {
            l.swapTwoIndices(l.size - 1 - stack[frame_pointer].loop_i, l.size - 1 - stack[frame_pointer].c);
          } else {
            l.swapTwoIndices(l.size - 1, l.size - 1 - stack[frame_pointer].c);
          }
          stack[frame_pointer].loop_i ++;
          stack[frame_pointer].program_counter = pcpositions::loop;
          break;
        case afterloop:
          stack[frame_pointer].program_counter = pcpositions::end;
          frame_pointer ++;
          stack[frame_pointer].c = stack[frame_pointer - 1].c - 1;
          stack[frame_pointer].program_counter = pcpositions::beginning;
          break;
        case end:
          frame_pointer --;
          break;
      }
    }
  }
  void initialize(int theN) {
    List<int> ll;
    ll.setSize(theN);
    for (int i = 0; i < theN; i ++) {
      ll[i] = i;
    }
    this->initialize(ll);
  }
  List<int>& operator*() {
    return l;
  }
  bool doneIterating() const {
    return done_iterating;
  }
  void varsout() const {
    global.comments << "stack: [";
    for (int i = 0; i < stack.size; i ++) {
      global.comments << "(";
      if (i == frame_pointer) {
        global.comments << "(";
      }
      global.comments << "pc =" << stack[i].program_counter;
      global.comments << ",li =" << stack[i].loop_i;
      global.comments << ",c =" << stack[i].c;
      if (i == frame_pointer) {
        global.comments << ")";
      }
      global.comments << ")";
      if (i != stack.size - 1) {
        global.comments << ", ";
      }
    }
    global.comments << "] l =" << l.toStringCommaDelimited() << "\n";
  }
};

class GeneratorPermutationR2sOnIndices {
public:
  List<int> replacements;
  GeneratorPermutationsOfList<int> pads;

  void initialize(const List<int>& l) {
    replacements = l;
    replacements.quickSortAscending();
    pads.initialize(l.size);
  }

  GeneratorPermutationR2sOnIndices& operator++() {
    ++ pads;
    return *this;
  }

  // this method name needs improvement
  PermutationR2 operator*() {
    List<int> l = *pads; // for this line, it can't be const, since the other method can't return
    PermutationR2 out;    // a const list& or even verify that the list isn't being modified
    out.makeFromActionDescription(l);
    for (int i = 0; i < out.cycles.size; i ++) {
      for (int j = 0; j < out.cycles[i].size; j ++) {
        out.cycles[i][j] = replacements[out.cycles[i][j]];
      }
    }
    return out;
  }
  void initialize(int n) {
    List<int> l;
    l.setSize(n);
    for (int i = 0; i < n; i ++) {
      l[i] = i;
    }
    initialize(l);
  }

  bool doneIterating() const {
    return pads.doneIterating();
  }

  void ResetIteration() {
    this->initialize(replacements);
  }
};

class GeneratorElementsSnxSnOnIndicesAndIndices {
public:
  List<GeneratorPermutationR2sOnIndices> generators;

  enum pcpositions {beginning, loop, midloop, end};
  struct frame {
    int program_counter;
    PermutationR2 subprod;
  };
  List<struct frame> stack;
  int frame_pointer;
  void initialize(List<GeneratorPermutationR2sOnIndices> theGenerators) {
    if (theGenerators.size == 0) {
      frame_pointer = - 1;
      return;
    }
    generators = theGenerators;
    stack.setSize(generators.size);
    frame_pointer = 0;
    stack[frame_pointer].program_counter = pcpositions::beginning;
    ++ (*this);
  }

  GeneratorElementsSnxSnOnIndicesAndIndices& operator++() {
    while (true) {
      if (this->frame_pointer == - 1) {
        return *this;
      }
      switch (stack[frame_pointer].program_counter) {
      case pcpositions::beginning:
        generators[frame_pointer].ResetIteration();
          break;
      case pcpositions::loop:
        if (generators[frame_pointer].doneIterating()) {
          stack[frame_pointer].program_counter = pcpositions::end;
          break;
        }
        {
          PermutationR2 permi = *(generators[frame_pointer]);
          if (frame_pointer == 0) {
            stack[frame_pointer].subprod = permi;
          } else {
            stack[frame_pointer].subprod.makeFromMultiplicities(stack[frame_pointer - 1].subprod, permi);
          }
          if (frame_pointer == generators.size - 1) {
            stack[frame_pointer].program_counter = pcpositions::midloop;
            return *this;
          } else {
            stack[frame_pointer].program_counter = pcpositions::midloop;
            frame_pointer ++;
            stack[frame_pointer].program_counter = pcpositions::beginning;
            break;
          }
        }
        case pcpositions::midloop:
          ++ generators[frame_pointer];
          stack[frame_pointer].program_counter = pcpositions::loop;
          break;
        case pcpositions::end:
          frame_pointer --;
          break;
      }
    }
  }

  PermutationR2 operator*() {
    return stack.lastObject()->subprod;
  }

  bool doneIterating() {
    if (frame_pointer == - 1) {
      return true;
    }
    return false;
  }

  void initialize(List<List<int> >& indiceses) {
    List<GeneratorPermutationR2sOnIndices> gens;
    gens.setSize(indiceses.size);
    for (int i = 0; i < indiceses.size; i ++) {
      gens[i].initialize(indiceses[i]);
    }
    this->initialize(gens);
  }
};

template<typename groupElement>
class GroupConjugacyImplementation{
  GroupConjugacyImplementation<groupElement> operator^(const GroupConjugacyImplementation<groupElement>& right) const {
    auto inv = right;
    inv.invert();
    return right * (*this) * inv;
  }
  static void conjugationAction(
    const GroupConjugacyImplementation<groupElement>& conjugateWith,
    const GroupConjugacyImplementation<groupElement>& conjugateOn,
    GroupConjugacyImplementation<groupElement>& out
  ) {
    out = conjugateOn ^ conjugateWith;
  }
};

// sample element class.  Functional, verifiable documentation
// give it a multiplication function, feed it to SimpleFiniteGroup, and...
class ElementFiniteGroup: public GroupConjugacyImplementation<ElementFiniteGroup> {
  void* data;

  void* (*mul)(void*, void*);
  void* (*inv)(void*);

  ElementFiniteGroup() {
    this->data = nullptr;
    this->mul = nullptr;
    this->inv = nullptr;
  }

  ElementFiniteGroup operator*(ElementFiniteGroup right) {
    ElementFiniteGroup out;
    out.data = this->mul(this->data, right.data);
    out.mul = this->mul;
    out.inv = this->inv;
    return out;
  }
  ElementFiniteGroup invert() {
    ElementFiniteGroup out;
    if (inv != nullptr) {
      out.data = this->inv(this->data);
      return out;
    }
    global.fatal << "Inversion function not available. " << global.fatal;
    return out;
  }
  ElementFiniteGroup makeIdentity(const ElementFiniteGroup& prototype) {
    ElementFiniteGroup out;
    out.mul = prototype.mul;
    out.inv = prototype.inv;
    return out;
  }
};

class PermutationGroupData {
public:
  FiniteGroup<PermutationR2>* theGroup;
  FiniteGroup<PermutationR2> theGroupMayBeHereNameIsLongToDiscourageUse;
  bool flagIsSymmetricGroup;
  bool flagHasGenerators1j;
  bool flagHasGeneratorsjjPlus1;

  void makeSymmetricGroup(int n);
  void makeSymmetricGroupGeneratorsjjPlus1(int n);

  //bool areConjugate(const PermutationR2& x, const PermutationR2& y);

  static void computeCCSizesAndRepresentativesByFormulaImplementation(FiniteGroup<PermutationR2>& G);
  static LargeInteger getSizeByFormulaImplementation(FiniteGroup<PermutationR2>& G);
  static bool getWordjjPlus1Implementation(FiniteGroup<PermutationR2>& G, const PermutationR2& g, List<int>& word);

  PermutationGroupData() {
    this->flagIsSymmetricGroup = false;
    this->flagHasGenerators1j = false;
    this->flagHasGeneratorsjjPlus1= false;
  }
  template <typename Coefficient>
  void spechtModuleOfPartition(const Partition& p, GroupRepresentation<FiniteGroup<PermutationR2>, Coefficient>& rep);
  void computeSpechtModules();
  template <typename somestream>
  somestream& intoStream(somestream& out);
  std::string toString();
  friend std::ostream& operator<<(std::ostream& out, PermutationGroupData& data) {
    return data.intoStream(out);
  }
};

template <typename scalar>
void Partition::spechtModuleMatricesOfTranspositions1j(List<Matrix<scalar> >& out) const {
  List<PermutationR2> perms;
  perms.setSize(this->n - 1);
  for (int i = 0; i < this->n - 1; i ++) {
    perms[i].buildTransposition(0, i + 1);
  }
  this->spechtModuleMatricesOfPermutations(out, perms);
}

template <typename scalar>
void Partition::spechtModuleMatricesOfTranspositionsjjplusone(List<Matrix<scalar> >& out) const {
  List<PermutationR2> perms;
  perms.setSize(this->n - 1);
  for (int i = 0; i < this->n - 1; i ++) {
    perms[i].buildTransposition(i, i + 1);
  }
  this->spechtModuleMatricesOfPermutations(out, perms);
}

template <typename scalar>
void Partition::spechtModuleMatrixOfPermutation(List<Matrix<scalar> >& out, const PermutationR2& p) const {
  List<PermutationR2> perms;
  perms.addOnTop(p);
  this->spechtModuleMatricesOfPermutations(out, perms);
}


template <typename scalar>
void Partition::spechtModuleMatricesOfPermutations(List<Matrix<scalar> >& out, const List<PermutationR2>& perms) const {
  Tableau initialTableau;
  List<int> stuffing;
  stuffing.setSize(this->n);
  for (int i = 0; i < this->n; i ++){
    stuffing[i] = i;
  }
  this->fillTableau(initialTableau, stuffing);
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> tm1;
  tm1.generatorsIndices.setSize(n);
  tm1.powers.setSize(n);
  for (int i = 0; i < n; i ++) {
    tm1.generatorsIndices[i] = i;
    tm1.powers[i] = 1;
  }
  ElementMonomialAlgebra<MonomialTensor<int, MathRoutines::IntUnsignIdentity>, scalar> t1, t2;
  t1.addMonomial(tm1,1);
  initialTableau.youngSymmetrizerAction(t2, t1);
  List<ElementMonomialAlgebra<MonomialTensor<int, MathRoutines::IntUnsignIdentity>, scalar> > basisvs;
  SparseSubspaceBasis<
    ElementMonomialAlgebra<MonomialTensor<int, MathRoutines::IntUnsignIdentity>, scalar>,
    MonomialTensor<int, MathRoutines::IntUnsignIdentity>, scalar
  > basis;
  List<Tableau> standardTableaux;
  this->getAllStandardTableaux(standardTableaux);
  basisvs.setSize(standardTableaux.size);
  for (int i = 0; i <standardTableaux.size; i ++) {
    PermutationR2 p;
    p.makeFromActionDescription(standardTableaux[i].turnIntoList());
    p.actOnTensor(basisvs[i], t2);
  }
  basis.setBasis(basisvs);
  out.setSize(perms.size);
  for (int permi = 0; permi < perms.size; permi ++) {
    out[permi].initialize(basis.rank, basis.rank);
    for (int bi = 0; bi < basis.rank; bi ++) {
      ElementMonomialAlgebra<MonomialTensor<int, MathRoutines::IntUnsignIdentity>, scalar> sparse;
      perms[permi].actOnTensor(sparse,basisvs[bi]);
      Vector<scalar> dense;
      basis.denseVectorInBasis(dense, sparse);
      // AssignColumnFromVector ?  oh well.
      for (int j = 0; j< basis.rank; j ++) {
        out[permi].elements[j][bi] = dense[j];
      }
    }
  }
}

template <typename somestream>
somestream& Partition::intoStream(somestream& out) const {
  out << this->n << ": ";
  for (int i = 0; i < this->p.size; i ++) {
    out << this->p[i];
    if (i != (this->p.size - 1)) {
      out << " ";
    }
  }
  return out;
}

template <typename Coefficient>
void Tableau::youngSymmetrizerAction(
  ElementMonomialAlgebra<MonomialTensor<int, MathRoutines::IntUnsignIdentity>, Coefficient>& out,
  const ElementMonomialAlgebra<MonomialTensor<int, MathRoutines::IntUnsignIdentity>, Coefficient>& in
) {
  GeneratorElementsSnxSnOnIndicesAndIndices rs,cs;
  ElementMonomialAlgebra<MonomialTensor<int, MathRoutines::IntUnsignIdentity>, Coefficient> rst;
  for (rs.initialize(this->t); !rs.doneIterating(); ++ rs) {
    ElementMonomialAlgebra<MonomialTensor<int, MathRoutines::IntUnsignIdentity>, Coefficient> tmp;
    (*rs).actOnTensor(tmp, in);
    rst += tmp;
  }
  out.makeZero();
  List<List<int> > columns;
  this->getColumns(columns);
  cs.initialize(columns);
  for (; !cs.doneIterating(); ++ cs) {
    ElementMonomialAlgebra<MonomialTensor<int, MathRoutines::IntUnsignIdentity>, Coefficient> tmp;
    PermutationR2 csi = *cs;
    csi.actOnTensor(tmp, rst);
    out += tmp * csi.sign();
  }
}

template <typename somestream>
somestream& Tableau::intoStream(somestream& out) const {
  out << "[";
  for (int i = 0; i < this->t.size; i ++) {
    out << "[";
    for (int j = 0; j < this->t[i].size - 1; j ++) {
      out << this->t[i][j] << " ";
    }
    out << this->t[i][this->t[i].size - 1];
    out << "]";
    if (i != this->t.size - 1) {
      out << ", ";
    }
  }
  out << "]";
  return out;
}

template <typename somestream>
somestream& PermutationR2::intoStream(somestream& out) const {
  out << "[";
  for (int i = 0; i < this->cycles.size; i ++) {
    out << "(";
    for (int j = 0; j < this->cycles[i].size; j ++) {
      out << this->cycles[i][j];
      if (j != this->cycles[i].size - 1) {
        out << ", ";
      }
    }
    out << ")";
  }
  out << "]";
  return out;
}

template <typename Object>
void PermutationR2::actOnList(List<Object>& in) const {
  in.permuteIndices(this->cycles);
}

template <typename Coefficient>
void PermutationR2::actOnTensor(
  ElementMonomialAlgebra<MonomialTensor<int, MathRoutines::IntUnsignIdentity>, Coefficient>& out,
  const ElementMonomialAlgebra<MonomialTensor<int, MathRoutines::IntUnsignIdentity>, Coefficient>& in
) const {
  for (int i = 0; i < in.monomials.size; i ++) {
    MonomialTensor<int, MathRoutines::IntUnsignIdentity> tmpout, tmpin;
    tmpin = in.monomials[i];
    this->actOnMonomialTensor(tmpout, tmpin);
    out.addMonomial(tmpout, in.coefficients[i]);
  }
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::possiblyConjugate(const elementSomeGroup& x, const elementSomeGroup& y) {
  (void) x, y;
  return true;
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::areConjugate(const elementSomeGroup& x, const elementSomeGroup& y) {
  this->checkConsistency();
  if (this->areConjugateByFormula != 0) {
    return this->areConjugateByFormula(x, y);
  }
  if (!this->flagCCsComputed) {
    this->computeConjugacyClassSizesAndRepresentatives();
  }
  int xi = this->theElements.getIndex(x);
  int yi = this->theElements.getIndex(y);
  for (int i = 0; i < this->conjugacyClasses.size; i ++) {
    if (this->conjugacyClasses[i].indicesEltsInOwner.BSContains(xi)) {
      if (this->conjugacyClasses[i].indicesEltsInOwner.BSContains(yi)) {
        return true;
      }
    }
  }
  return false;
}

/*
template <typename elementSomeGroup>
template <typename Coefficient>
coefficient FiniteGroup<elementSomeGroup>::getHermitianProduct(const Vector<Coefficient>& X1, const Vector<Coefficient>& X2) {
  if (!this->flagCCsComputed)
    this->computeConjugacyClassSizesAndRepresentatives(NULL);
  coefficient acc = 0;
  for (int i = 0; i < this->conjugacyClasses.size; i ++)
    acc += MathRoutines::complexConjugate(X1[i]) * X2[i] * this->conjugacyClasses[i].size;
  return  acc / this->getSize();
}
*/

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::isIdentity(elementSomeGroup& g) {
  return g.isIdentity();
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::makeIdentity(elementSomeGroup& e) {
  if (this->generators.size != 0) {
    e.makeIdentity(this->generators[0]);
  }
}

// this needs some better design
// the intention here is to do representation theory.  so we need the sizes,
// representatives, and words.
template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::computeConjugacyClassesSizesRepresentativesWords() {
  MacroRegisterFunctionWithName("FiniteGroup::computeConjugacyClassesSizesRepresentativesWords");
  this->checkConsistency();
  if (this->getWordByFormula != 0) {
    this->flagWordsComputed = true;
  }
  if (this->flagCCsComputed && this->flagWordsComputed) {
    return;
  }
  if (this->computeConjugacyClassSizesAndRepresentativesByFormula != 0) {
    this->computeConjugacyClassSizesAndRepresentativesByFormula(*this);
    return;
  }
  if (this->areConjugateByFormula != 0) {
    global.comments << "This needs a rewrite";
  }
  if (!this->flagWordsComputed || !this->flagAllElementsAreComputed) {
    this->computeAllElements(true, - 1);
  }
  GraphOLD conjugacygraph = GraphOLD(this->theElements.size, this->generators.size);
  for (int i = 0; i < this->theElements.size; i ++) {
    for (int j = 0; j < this->generators.size; j ++) {
      elementSomeGroup x = this->theElements[i] ^ this->generators[j];
      int xi = this->theElements.getIndex(x);
      conjugacygraph.addEdge(i, xi);
    }
  }
  List<List<int> > components = conjugacygraph.destructivelyGetConnectedComponents();
  // boxing and unboxing...
  this->conjugacyClasses.setSize(components.size);
  for (int i = 0; i < components.size; i ++) {
    this->conjugacyClasses[i].representative = this->theElements[components[i][0]];
    this->conjugacyClasses[i].size = components[i].size;
    this->conjugacyClasses[i].indicesEltsInOwner = components[i];
    this->conjugacyClasses[i].representativeIndex = components[i][0];
    if (this->flagWordsComputed) {
      this->getWord(conjugacyClasses[i].representative, conjugacyClasses[i].representativeWord);
    }
  }
  this->flagCCsComputed = true;
  this->flagCCRepresentativesComputed = true;
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::computeGeneratorCommutationRelations() {
  if (this->generatorCommutationRelations.numberOfRows == this->generators.size) {
    return;
  }
  this->generatorCommutationRelations.initialize(this->generators.size, this->generators.size);
  for (int i = 0; i < this->generators.size; i ++) {
    for (int j = i; j < this->generators.size; j ++) {
      elementSomeGroup g;
      if (i == j) {
        g = this->generators[i];
      } else {
        g = this->generators[i] * this->generators[j];
      }
      elementSomeGroup gi = g;
      int cr = 1;
      while (!this->isIdentity(gi)) {
        gi = gi * g;
        cr ++;
        if (cr > 1009) {
          global.comments << "Error: in computing commutation relations, generator";
          if (i == j) {
            global.comments << " " << i << " is found to have g_n";
          } else {
            global.comments << "s " << i << "," << j << " are found to have (g_m g_n)";
          }
          global.comments << "^t does not equal e for all t < 1009. "
          << "This usually happens if the specified expressions don't generate a group, "
          << "let alone a finite group, but, if this was intentional, "
          << "lift the limit near " << __FILE__ << ":" << __LINE__ << "\n";
          global.comments << "I am recording the exponent as 1009 as an unexpected value\n";
          cr = 1009;
          break;
        }
      }
      this->generatorCommutationRelations(i, j) = cr;
      this->generatorCommutationRelations(j, i) = cr;
    }
  }
}

template <typename elementSomeGroup>
std::string FiniteGroup<elementSomeGroup>::prettyPrintGeneratorCommutationRelations(bool andPrint) {
  this->computeGeneratorCommutationRelations();
  std::string crs = this->generatorCommutationRelations.toStringPlainText();
  List<char*> rows;
  rows.addOnTop(&crs[0]);
  int i;
  while ((i = crs.find('\n')) != - 1) {
    crs[static_cast<unsigned>(i)] = 0;
    rows.addOnTop(&crs[static_cast<unsigned>(i) + 1]);
  }
  bool generatorStringsTooLarge = false;
  bool generatorStringsHaveNewline = false;
  List<std::string> genstrings;
  genstrings.setSize(this->generators.size);
  for (int i = 0; i < this->generators.size; i ++) {
    std::stringstream geni;
    geni << this->generators[i];
    genstrings[i] = geni.str();
    if (genstrings[i].length() > 50) {
      generatorStringsTooLarge = true;
    }
  }
  if (!generatorStringsTooLarge) {
    std::stringstream out;
    for (int i = 0; i < this->generators.size; i ++) {
      out << i << " " << genstrings[i] << '\n';
    }
  }
  std::stringstream out;
  for (int i = 0; i < this->generators.size; i ++) {
    out << i;
    if (generatorStringsHaveNewline) {
      out << "\n";
    } else {
      out << " ";
    }
    out << genstrings[i] << '\n';
  }
  for (int i = 0; i < this->generators.size; i ++) {
    out << i << " " << rows[i] << '\n';
  }
  std::string outs = out.str();
  if (andPrint) {
    global.comments << outs << '\n';
  }
  return outs;
}

template <typename elementSomeGroup>
std::string FiniteGroup<elementSomeGroup>::prettyPrintCharacterTable(bool andPrint) {
  for (int i = 0; i < this->irreps.size; i ++) {
    this->irreps[i].computeCharacter();
  }
  std::stringstream out;
  out << this->getSize() << " elements. Representatives and sizes are ";
  for (int i = 0; i < this->conjugacyClasses.size; i ++) {
    out << this->conjugacyClasses[i].representative << " " << this->conjugacyClasses[i].size;
    if (i != this->conjugacyClasses.size - 1) {
      out << ", ";
    } else {
      out << "\n";
    }
  }

  // pad the numbers out front
  List<std::string> numbers;
  int numpad = 0;
  for (int i = 0; i < this->irreps.size; i ++) {
    std::stringstream ns;
    ns << i;
    numbers.addOnTop(ns.str());
    int nil = numbers.lastObject()->length();
    if (numpad < nil) {
      numpad = nil;
    }
  }

  // pad the character values
  List<List<std::string> > values;
  values.setSize(this->irreps.size);
  int columnsPerElement = 0;
  for (int i = 0; i < this->irreps.size; i ++) {
    values[i].setSize(this->irreps[i].theCharacter.data.size);
    for (int j = 0; j < this->irreps[i].theCharacter.data.size; j ++) {
      values[i][j] = irreps[i].theCharacter.data[j].toString();
      int vijcols = static_cast<int>(values[i][j].length());
      if (vijcols > columnsPerElement) {
        columnsPerElement = vijcols;
      }
    }
  }
  columnsPerElement ++;

  // ok print it all up
  for (int i = 0; i < values.size; i ++) {
    int padn = numpad - static_cast<signed>(numbers[i].length());
    for (int pp = 0; pp < padn; pp ++) {
      out << ' ';
    }
    out << numbers[i];
    out << ' ';
    out << '[';
    for (int j = 0; j < values[i].size; j ++) {
      int padl = columnsPerElement - static_cast<int>(values[i][j].length());
      for (int pp = 0; pp < padl; pp ++) {
        out << ' ';
      }
      out << values[i][j];
    }
    Rational x = this->irreps[i].theCharacter.norm();
    if (x != 1) {
      out << "][" << x;
    }
    out << "] " << this->irreps[i].identifyingString;
    out << '\n';
  }
  Rational x = 0;
  for (int i = 0; i < this->irreps.size; i ++) {
    x += irreps[i].theCharacter.data[0] * irreps[i].theCharacter.data[0];
  }
  out << "Sum of squares of first column: " << x << '\n';
  // print information about if anything's wrong
  for (int i = 0; i < this->irreps.size; i ++) {
    for (int j = i + 1; j < this->irreps.size; j ++) {
      Rational x = this->irreps[i].theCharacter.innerProduct(this->irreps[j].theCharacter);
      if (x != 0) {
        out << "characters " << i << ", " << j << " have inner product " << x << '\n';
      }
    }
  }
  std::string outs = out.str();
  if (andPrint) {
    global.comments << outs << '\n';
  }
  return outs;
}

template <typename elementSomeGroup>
std::string FiniteGroup<elementSomeGroup>::prettyPrintCCRepsSizes(bool andPrint) {
  std::stringstream out;
  // pad the numbers out front
  List<std::string> numbers;
  int numpad = 0;
  for (int i = 0; i < this->conjugacyClasses.size; i ++) {
    std::stringstream ns;
    ns << i;
    numbers.addOnTop(ns.str());
    int nilen = numbers.lastObject()->length();
    if (numpad < nilen) {
      numpad = nilen;
    }
  }

  // pad the sizes
  List<std::string> sizes;
  int sizepad = 0;
  for (int i = 0; i < this->conjugacyClasses.size; i ++) {
    std::stringstream ns;
    ns << this->conjugacyClasses[i].size;
    sizes.addOnTop(ns.str());
    int nilen = sizes.lastObject()->length();
    if (sizepad < nilen) {
      sizepad = nilen;
    }
  }

  for (int i = 0; i < this->conjugacyClasses.size; i ++) {
    int pad;
    pad = numpad - static_cast<int>(numbers[i].length());
    for (int j = 0; j < pad; j ++) {
      out << " ";
    }
    out << numbers[i];
    out << " ";
    pad = sizepad - static_cast<int>(sizes[i].length());
    for (int j = 0; j < pad; j ++) {
      out << " ";
    }
    out << sizes[i];
    out << " ";
    out << this->conjugacyClasses[i].representative;
    out << '\n';
  }

  std::string outs = out.str();
  if (andPrint) {
    global.comments << outs << '\n';
  }
  return outs;
}

template <typename elementSomeGroup>
JSData FiniteGroup<elementSomeGroup>::representationDataIntoJS() {
  JSData out;
  for (int i = 0; i < irreps.size; i ++) {
    out[i] = irreps[i].toJSON();
  }
  return out;
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::verifyCCSizesAndRepresentativesFormula() {
  this->computeConjugacyClassSizesAndRepresentatives();
  FiniteGroup<elementSomeGroup> GG;
  GG.generators = this->generators;
  //GG.areConjugateByFormula = this->areConjugateByFormula;
  GG.computeConjugacyClassSizesAndRepresentatives();
  global.comments << "Conjugacy class sizes by formula: ";
  for (int i = 0; i < this->conjugacyClasses.size; i ++) {
    global.comments << this->conjugacyClasses[i].size << ", ";
  }
  global.comments << '\n';
  for (int i = 0; i < this->conjugacyClasses.size; i ++) {
    global.comments << this->conjugacyClasses[i].representative;
    global.comments << '\n';
  }
  global.comments << "Conjugacy class sizes by brute force: ";
  for (int i = 0; i < GG.conjugacyClasses.size; i ++) {
    global.comments << GG.conjugacyClasses[i].size << ", ";
  }
  global.comments << '\n';
  for (int i = 0; i < GG.conjugacyClasses.size; i ++) {
    global.comments << GG.conjugacyClasses[i].representative << "\n";
  }
  global.comments << '\n';
  if (GG.conjugacyClasses.size != this->conjugacyClasses.size) {
    global.comments << "Error: wrong number of conjugacy classes, "
    << this->conjugacyClasses.size << " should be " << GG.conjugacyClasses.size << '\n';
  }
  List<int> classes_found;
  for (int i = 0; i < this->conjugacyClasses.size; i ++) {
    int gcc;
    if (GG.areConjugateByFormula) {
      for (int gci = 0; gci < GG.conjugacyClasses.size; gci ++) {
        if (GG.areConjugateByFormula(GG.conjugacyClasses[gci].representative, this->conjugacyClasses[i].representative)) {
          gcc = gci;
          break;
        }
      }
    } else {
      int cri = GG.theElements.getIndex(this->conjugacyClasses[i].representative);
      for (int gci = 0; gci < GG.conjugacyClasses.size; gci ++) {
        if (GG.conjugacyClasses[gci].indicesEltsInOwner.BSContains(cri)) {
          gcc = gci;
          break;
        }
      }
    }
    global.comments << "class " << i << " representative "
    << this->conjugacyClasses[i].representative << " belongs to cc "
    << gcc << " with representative " << GG.conjugacyClasses[gcc].representative << '\n';
    if (classes_found.BSInsertDontDup(gcc) == - 1) {
      global.comments << "error\n";
    }
  }
  global.comments << "classes found are " << classes_found.toStringCommaDelimited() << '\n';
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::verifyWords() {
  if (!this->flagAllElementsAreComputed) {
    this->ComputeAllElementsWordsConjugacyIfObvious(true);
  }
  for (int i = 0; i < this->theElements.size; i ++) {
    List<int> word;
    getWord(this->theElements[i], word);
    elementSomeGroup g;
    this->makeIdentity(g);
    for (int j = 0; j < word.size; j ++) {
      g = g * this->generators[word[j]];
    }
    if (!(g == this->theElements[i])) {
      global.comments << this->theElements[i] << " has word " << word.toStringCommaDelimited()
      << " which corresponds to " << g << "\n";
    }
  }
}

/*
template <typename elementSomeGroup>
template <typename somestream>
somestream& FiniteGroup<elementSomeGroup>::intoStream(somestream& out) const {
  out << "Finite Group with " << this->generators.size << " generators";
  if (this->haveElements)
    out << ", " << this->theElements.size << " elements";
  if (this->flagCCsComputed)
    out << ", " << this->conjugacyClasses.size << " conjugacy classes";
  out << ", generated by: ";
  for (int i = 0; i < this->generators.size; i ++) {
    out << this->generators[i];
    if (i != this->generators.size- 1)
      out << ", ";
  }
  return out;
}
*/

template <typename somestream>
somestream& PermutationGroupData::intoStream(somestream& out) {
  if (!this->flagIsSymmetricGroup) {
    out << "Permutation Group with " << this->theGroup->theElements.size << " elements, generated by: ";
    for (int i = 0; i < this->theGroup->generators.size; i ++) {
      out << this->theGroup->generators[i];
      if (i != this->theGroup->generators.size - 1) {
        out << ", ";
      }
    }
  }
  out << "Symmetric Group on " << this->theGroup->generators.size + 1 << " letters (";
  if (this->flagHasGenerators1j) {
    out << "generators are (1 j))";
  }
  if (this->flagHasGeneratorsjjPlus1) {
    out << "generators are (j j+ 1))";
  }
  out << " thus having " << this->theGroup->getSize() << " elements.";
  return out;
}

/*
template <typename elementSomeGroup>
std::string FiniteGroup<elementSomeGroup>::toString() const {
  std::stringstream out;
  this->intoStream(out);
  return out.str();
}
*/

template <typename Coefficient>
void PermutationGroupData::spechtModuleOfPartition(
  const Partition& p, GroupRepresentation<FiniteGroup<PermutationR2>, Coefficient>& rep
) {
  p.spechtModuleMatricesOfPermutations(rep.generators, this->theGroup->generators);
  rep.ownerGroup = this->theGroup;
}

/*
template <typename elementSomeGroup>
std::ostream& operator<<(std::ostream& out, const FiniteGroup<elementSomeGroup>& data) {
  return data.intoStream(out);
}
*/

/*template <typename somestream>
somestream& ElementHyperoctahedralGroup::intoStream(somestream& out) const {
  out << '[' << this->p << ',';
  for (int i = 0; i < this->s.size; i ++)
    if (this->s[i])
      out << '1';
    else
      out << '0';
  out << ']';
  return out;
}*/

/*
template <typename somestream>
somestream& HyperoctahedralGroup::intoStream(somestream& out) const {
  if (this->isEntireHyperoctahedralGroup) {
    out << "Hyperoctahedral Group of rank " << this->generators[0].s.size;
  } else if (this->isEntireDn) {
    out << "Dn group of rank" << this->generators[0].s.size;
  } else {
    out << "Finite group of hyperoctahedral elements, generated by ";
    for (int i = 0; i < this->generators.size; i ++)
      out << this->generators[i];
  }
  return out;
}
*/

template <typename elementSomeGroup>
template <typename somestream>
somestream& ConjugacyClassR2<elementSomeGroup>::intoStream(somestream& out) const {
  out << "[" << this->representative << ", " << this->theElements.size << "]";
  return out;
}

template <typename elementSomeGroup>
std::string ConjugacyClassR2<elementSomeGroup>::toString() const {
  std::stringstream out;
  this->intoStream(out);
  return out.str();
}

template <typename elementSomeGroup>
std::ostream& operator<<(std::ostream& out, const ConjugacyClassR2<elementSomeGroup>& data) {
  return data.intoStream(out);
}

template <typename someGroup, typename Coefficient>
bool GroupRepresentation<someGroup, Coefficient>::verifyRepresentation() {
  bool badrep = false;
  if (this->generators.size != this->ownerGroup->generatorCommutationRelations.numberOfRows) {
    this->ownerGroup->computeGeneratorCommutationRelations();
    for (int i = 0; i < this->generators.size; i ++) {
      for (int j = i; j < this->generators.size; j ++) {
        Matrix<Rational> M1;
        if (i != j) {
          M1 = this->generators[i] * this->generators[j];
        } else {
          M1 = this->generators[i];
        }
        Matrix<Rational> Mi = M1;
        for (int n = 1; n < this->ownerGroup->generatorCommutationRelations(i, j); n ++) {
          Mi *= M1;
        }
        if (!Mi.isIdentity()) {
          global.comments << "generators " << i << ", " << j << " i.e. elements ";
          global.comments << this->ownerGroup->generators[i] << ", " << this->ownerGroup->generators[j];
          global.comments << " are assigned matrices which fail to have commutation relations ";
          global.comments << this->ownerGroup->generatorCommutationRelations(i, j) << "\n";
          global.comments << this->generators[i].toStringPlainText() << ",\n";
          global.comments << this->generators[j].toStringPlainText() << "\n\n";
          badrep = true;
        }
      }
    }
  }
  if (badrep) {
    FiniteGroup<Matrix<Rational> > RG;
    RG.generators = this->generators;
    LargeInteger GS = this->ownerGroup->getSize();
    LargeInteger RGS = RG.getSize();
    if ((GS % RGS) != 0) {
      global.comments << "Violation of Lagrange's theorem (" << RGS << "∤" << GS << ")\n";
    }
    global.comments << "Group and \"representation\" generator commutation relations follow" << "\n";
    global.comments << this->ownerGroup->prettyPrintGeneratorCommutationRelations() << "\n";
    global.comments << RG.prettyPrintGeneratorCommutationRelations() << "\n";
  }
  if (!badrep) {
    global.comments << "verifyRepresentation: this has the proper commutation relations\n";
  }
  return !badrep;
}

template <typename somegroup, typename Coefficient>
std::string GroupRepresentation<somegroup, Coefficient>::describeAsDirectSum() {
  this->computeCharacter();
  std::stringstream out;
  bool firstone = true;
  for (int i = 0; i < this->ownerGroup->irreps.size; i ++) {
    this->ownerGroup->irreps[i].computeCharacter();
    Coefficient x;
    x = this->theCharacter.innerProduct(this->ownerGroup->irreps[i].theCharacter);
    if (x != 0) {
      if (firstone) {
        firstone = false;
      } else {
        out << " ⊕ ";
      }
      out << x << "×χ" << i;
    }
  }
  return out.str();
}

template <typename somestream>
somestream& HyperoctahedralGroupData::intoStream(somestream& out) const {
  if (this->flagIsEntireHyperoctahedralGroup) {
    out << "Hyperoctahedral group with " << this->theGroup->getSizeByFormula(*(this->theGroup)) << " elements";
  } else if (this->flagIsEntireDn) {
    out << "Half hyperoctahedral group with " << this->theGroup->getSizeByFormula(*(this->theGroup)) << " elemets";
  } else {
    out << this->theGroup;
  }
  return out;
}
#endif
