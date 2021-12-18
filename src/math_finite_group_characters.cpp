// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general.h"
#include "math_extra_semisimple_lie_algebras_root_subalgebras.h" // WeylGroup::getSignSignatureAllRootSubsystems
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_symmetric_groups_and_generalizations.h"
#include "math_extra_graph.h"
#include "math_general_implementation.h"

template<>
List<ElementWeylGroup>::Comparator* FormatExpressions::getMonomialOrder<ElementWeylGroup>() {
  return nullptr;
}

Vector<Rational> WeylGroupData::applyReflectionList(const List<int>& rightReflectionsActFirst, const Vector<Rational>& vv) const {
  Vector<Rational> v = vv;
  for (int i = rightReflectionsActFirst.size - 1; i >= 0; i --) {
    Rational x = 0;
    for (int j = 0; j < this->getDimension(); j ++) {
      x += v[j] * cartanSymmetric(rightReflectionsActFirst[i], j);
    }
    //cartanSymmetric(i, j) is slower than cartanSymmetric.elements[i][j] but includes index checking
    //(we want to catch a rogue call to applyReflectionList
    v[rightReflectionsActFirst[i]] -= x * 2 / cartanSymmetric(rightReflectionsActFirst[i], rightReflectionsActFirst[i]);
  }
  return v;
}

void WeylGroupData::getSimpleReflectionMatrix(int indexSimpleRoot, Matrix<Rational>& output) const {
  MacroRegisterFunctionWithName("WeylGroup::getSimpleReflectionMatrix");
  int rank = this->getDimension();
  output.makeIdentityMatrix(rank, Rational::one(), Rational::zero());
  for (int j = 0; j < rank; j ++) {
    output(indexSimpleRoot, j) -= (this->cartanSymmetric(indexSimpleRoot, j) / cartanSymmetric(indexSimpleRoot, indexSimpleRoot)) * 2;
  }
}

Matrix<Rational> WeylGroupData::simpleReflectionMatrix(int i) const {
  Matrix<Rational> result;
  this->getSimpleReflectionMatrix(i, result);
  return result;
}

ElementWeylGroup WeylGroupData::simpleConjugation(int i, const ElementWeylGroup& vv) {
  (void) vv;
  ElementWeylGroup eltSimpleReflection;
  eltSimpleReflection.makeSimpleReflection(i, *this);
  return eltSimpleReflection * this->group.elements[i] * eltSimpleReflection;
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::computeSquaresCCReps() {
  MacroRegisterFunctionWithName("WeylGroup::computeSquares");
  if (!this->flagCCsComputed) {
    this->computeConjugacyClassesFromAllElements();
  }
  this->squaresCCReps.setExpectedSize(this->conjugacyClassCount());
  this->squaresCCReps.setSize(this->conjugacyClassCount());
  //elementSomeGroup currentSquare;
  for (int i = 0; i < this->conjugacyClassCount(); i ++) {
    this->squaresCCReps[i] = this->conjugacyClasses[i].representative * this->conjugacyClasses[i].representative;
  }
}

void WeylGroupData::computeInitialIrreducibleRepresentations() {
  MacroRegisterFunctionWithName("WeylGroup::computeInitialIrreducibleRepresentations");
  if (!this->group.flagCCsComputed) {
    this->group.computeConjugacyClassesFromAllElements();
  }
  if (this->group.squaresCCReps.size == 0) {
    this->group.computeSquaresCCReps();
  }
  this->group.irreducibleRepresentations.setSize(0);
  this->group.characterTable.setSize(0);
  this->group.irreps_grcam.setSize(0);
  this->group.characterTable.setExpectedSize(this->group.conjugacyClassCount());
  this->group.irreducibleRepresentations.setExpectedSize(this->group.conjugacyClassCount());
  this->group.irreps_grcam.setExpectedSize(this->group.conjugacyClassCount());
  GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup>, Rational> trivialRep, signRep, standardRep;
  this->getTrivialRepresentation(trivialRep);
  this->getSignRepresentation(signRep);
  this->getStandardRepresentation(standardRep);
  this->group.addIrreducibleRepresentation(trivialRep);
  this->group.addIrreducibleRepresentation(signRep);
  this->group.addIrreducibleRepresentation(standardRep);
}

template <typename Coefficient>
class SpaceTree {
public:
  List<Vector<Coefficient> > space;
  List<SpaceTree<Coefficient> > subspaces;
  void placeInTree(const List<Vector<Coefficient> >& vector);
  void getLeaves(List<List<Vector<Coefficient> > >& leaves) const;
  void displayTree() const;
  void getunion(
    const List<Vector<Coefficient> >& left,
    const List<Vector<Coefficient> >& right,
    List<Vector<Coefficient> >& output
  );
  void intersection(
    const List<Vector<Coefficient> > &left,
    const List<Vector<Coefficient> > &right,
    List<Vector<Coefficient> >& output
  );
};

template <typename Coefficient>
void SpaceTree<Coefficient>::placeInTree(const List<Vector<Coefficient> >& vector) {
  List<Vector<Coefficient> > U;
  for (int i = 0; i < subspaces.size; i ++) {
    intersection(vector, subspaces[i].space, U);
    if (U.size == 0) {
      continue;
    }
    if (U.size == subspaces[i].space.size) {
      return;
    }
    subspaces[i].PlaceInTree(U);
  }
  List<Vector<Coefficient> > W, tempVspace;
  for (int i = 0; i < subspaces.size; i ++) {
    getunion(W, subspaces[i].space, tempVspace);
    W = tempVspace;
  }
  intersection(W, vector, tempVspace);
  if (tempVspace.size != vector.size) {
    SpaceTree<Coefficient> vst;
    vst.space = vector;
    subspaces.addOnTop(vst);
  }
}

template <typename Coefficient>
void SpaceTree<Coefficient>::getLeaves(List<List<Vector<Coefficient> > >& leaves) const {
  if (subspaces.size > 0) {
    for (int i = 0; i < subspaces.size; i ++) {
      subspaces[i].GetLeaves(leaves);
    }
  } else {
    leaves.addOnTop(space);
  }
}

template <typename Coefficient>
void SpaceTree<Coefficient>::displayTree() const {
  global.comments << '[' << space.size << ',';
  for (int i = 0; i < subspaces.size; i ++) {
    subspaces[i].displayTree();
  }
}

class CharacterFunctions {
public:
  template <typename elementSomeGroup>
  static void exportCharTable(FiniteGroup<elementSomeGroup>& G, JSData& data);
  static void exportTauSignatures(
    WeylGroupData& groupData, const List<List<bool> >& ts, JSData& output
  );
  template <typename Somegroup>
  static List<ClassFunction<Somegroup, Rational> > computeCharacterTable(Somegroup& group);
  static void computeTauSignatures(WeylGroupData* group, List<List<bool> >& tauSignatures, bool pseudo = false);
  static List<List<Vector<Rational> > > eigenspaces(const Matrix<Rational>& M, int checkDivisorsOf = 0);
  template <typename Somegroup>
  static Matrix<Rational> getClassMatrix(
    const Somegroup &group, int cci, List<int>* classmap = nullptr
  );
  template <typename Coefficient>
  static List<VectorSpace<Coefficient> > getEigenspaces(const Matrix<Coefficient>& matrix);
  template <typename Coefficient>
  static Matrix<Coefficient> getMatrix(const ClassFunction<WeylGroupData::WeylGroupBase, Coefficient>& classFunction);
  static void getTauSignaturesFromSubgroup(WeylGroupData& G, const List<ElementWeylGroup>& gens, List<bool>& out);
  template <typename Coefficient>
  static bool isIsotypicComponent(WeylGroupData& group, const List<Vector<Coefficient> >& vector);
  static Matrix<Rational> matrixInBasis(
    const ClassFunction<FiniteGroup<ElementWeylGroup>, Rational>& X, const List<Vector<Rational> >& B
  );
  template <typename Coefficient>
  static Vector<Coefficient> putInBasis(const Vector<Coefficient>& v, const List<Vector<Coefficient> >& B);
  template <typename Coefficient>
  List<Vector<Coefficient> > orthogonalComplement(
    const List<Vector<Coefficient> >& V, const List<Vector<Coefficient> >& WW
  );
  template <typename Coefficient>
  bool spaceContains(
    const List<Vector<Coefficient> >& space,
    const Vector<Coefficient>& vector
  );
};


template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::multiplyBy(
  const GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>& other,
  GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>& output
) const {
  if (&output == this || &output == &other) {
    GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> thisCopy, otherCopy;
    thisCopy = *this;
    otherCopy = other;
    thisCopy.multiplyBy(otherCopy, output);
    return;
  }
  //////////////////////////////////
  if (this->ownerGroup != other.ownerGroup) {
    global.fatal << "Attempt to multiply "
    << "representations with different owner groups. " << global.fatal;
  }
  output.reset(this->G);
  int Vd = this->basis[0].size;
  int Wd = other.basis[0].size;
  int Ud = Vd * Wd;
  for (int vi = 0; vi < this->basis.size; vi ++) {
    for (int wi = 0; wi < other.basis.size; wi ++) {
      Vector<Coefficient> u;
      u.setSize(Ud);
      for (int i = 0; i < Vd; i ++) {
        for (int j = 0; j < Wd; j ++) {
          u[i * Wd + j] = this->basis[vi][i] * other.basis[wi][j];
        }
      }
      output.basis.addOnTop(u);
    }
  }
  output.generators.setSize(this->generators.size);
  for (int i = 0; i < this->generators.size; i ++) {
    output.generators[i].assignTensorProduct(this->generators[i],other.generators[i]);
  }
  output.ownerGroup = this->ownerGroup;
}

template <typename somegroup, typename Coefficient>
GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>
GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::reduced() const {
  int d = basis.size;
  Matrix<Coefficient> GM;
  GM.initialize(d, d);
  for (int i = 0; i < d; i ++) {
    for (int j = 0; j < d; j ++) {
      GM.elements[i][j] = this->basis[i].scalarEuclidean(this->basis[j]);
    }
  }
  GM.invert();
  GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> out;
  out.generators.setSize(this->generators.size);
  for (int i = 0; i < this->generators.size; i ++) {
  //     MatrixInBasisFast(out.generators[i], this->generators[i], BM);
    matrixInBasis(out.generators[i], this->generators[i], this->basis, GM);
  }
  out.ownerGroup = ownerGroup;
  out.basis.makeEiBasis(d);
  return out;
}

template <typename somegroup, typename Coefficient>
VectorSpace<Coefficient> GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::findBasis() const {
  VectorSpace<Coefficient> V;
  int d = this->generators[0].numberOfColumns;
  for (int geni = 0; geni < this->generators.size; geni ++) {
    List<Vector<Coefficient> > ess = eigenspaces(this->generators[geni]);
    for (int essi = 0; essi < ess.size; essi ++) {
      for (int esi = 0; esi < ess[essi].size; esi ++) {
        // the best laid coding practices of mice and men oft go astray
        if (!V.addVectorToBasis(ess[essi][esi])) {
          return V;
        }
      }
    }
  }
  // This should not be possible
  for (int i = 0; i < d; i ++) {
    Vector<Coefficient> v;
    v.makeEi(d, i);
    if (!V.addVectorToBasis(v)) {
      return V;
    }
  }
  // if it gets here, it deserves to crash
}


template <typename somegroup, typename Coefficient>
List<GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> >
  GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::decomposeThomasVersion() {
  MacroRegisterFunctionWithName("WeylGroupRepresentation::decomposeThomasVersion");
  Matrix<Coefficient> splittingOperatorMatrix;
  List<GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> > out;
  List<Vector<Rational> > splittingMatrixKernel;
  if (getNumberOfComponents() == 1) {
    if (this->ownerGroup->characterTable.getIndex(this->character) == - 1) {
      global.comments << "new irrep found, have " << this->ownerGroup->characterTable.size << "\n";
      this->ownerGroup->addIrreducibleRepresentation(*this);
    }
    out.addOnTop(*this);
    return out;
  }
  List<Vector<Coefficient> > Vb = this->basis;
  List<Vector<Coefficient> > tempVectors;
  for (int i = 0; i < this->ownerGroup->characterTable.size; i ++) {
    if (this->character.innerProduct(this->ownerGroup->characterTable[i]) != 0) {
      global.comments << "contains irrep " << i << "\n";
      this->classFunctionMatrix(this->ownerGroup->characterTable[i], splittingOperatorMatrix);
      splittingOperatorMatrix.getZeroEigenSpaceModifyMe(splittingMatrixKernel);
      intersection(Vb, splittingMatrixKernel, tempVectors);
      Vb = tempVectors;
    }
  }
  if ((Vb.size < this->basis.size) && (Vb.size > 0)) {
    global.comments << "calculating remaining subrep... ";
    GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> V;
    V.ownerGroup = this->ownerGroup;
    V.generators = this->generators;
    V.basis = Vb;
    V = V.reduced();
    global.comments << "done\n";
    global.comments << "Decomposing remaining subrep " << V.getCharacter();
    return V.decomposeThomasVersion();
  }
  if (Vb.size == 0) {
    return out;
  }
  List<List<Vector<Coefficient> > > es;
  for (int cfi = this->ownerGroup->conjugacyClassCount() - 1; cfi >= 0; cfi --) {
    ClassFunction<somegroup, Coefficient> cf;
    cf.makeZero(*this->ownerGroup);
    cf[cfi] = 1;
    global.comments << "getting matrix " << cf << "\n";
    Matrix<Coefficient> A;
    classFunctionMatrix(cf, A);
    es = eigenspaces(A);
    if (es.size > 1) {
      global.comments << "eigenspaces were ";
      for (int i = 0; i < es.size; i ++) {
        global.comments << es[i].size << " ";
      }
      global.comments << "\n";
      break;
    }
  }

  for (int i = 0; i < es.size; i ++) {
    GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> outeme;
    outeme.ownerGroup = this->ownerGroup;
    outeme.generators = this->generators;
    outeme.basis = es[i];
    out.addOnTop(outeme.reduced());
  }
  return out;
}

template <typename Coefficient>
DivisionResult<PolynomialUnivariateDense<Coefficient> > PolynomialUnivariateDense<Coefficient>::divideBy(const PolynomialUnivariateDense<Coefficient>& divisor) const {
  DivisionResult<PolynomialUnivariateDense<Coefficient> > out;
  out.remainder = *this;
  if (data.size < divisor.data.size) {
    return out;
  }
  int r = data.size - divisor.data.size + 1;
  out.quotient.data.setSize(r);
  for (int i = r - 1; i != - 1; i --) {
    if (out.remainder.data.size - divisor.data.size != i) {
      out.quotient[i] = 0;
      continue;
    }
    PolynomialUnivariateDense<Coefficient> p = divisor.timesXn(i);
    out.quotient[i] = out.remainder.data[out.remainder.data.size - 1] / divisor.data[divisor.data.size - 1];
    p *= out.quotient[i];
    out.remainder -= p;
  }
  return out;
}

template <typename Coefficient>
PolynomialUnivariateDense<Coefficient> PolynomialUnivariateDense<Coefficient>::operator/(const PolynomialUnivariateDense<Coefficient>& divisor) const {
  DivisionResult<PolynomialUnivariateDense<Coefficient> > tmp = this->divideBy(divisor);
  return tmp.quotient;
}

template <typename Coefficient>
PolynomialUnivariateDense<Coefficient> PolynomialUnivariateDense<Coefficient>::operator%(const PolynomialUnivariateDense<Coefficient>& divisor) const {
  DivisionResult<PolynomialUnivariateDense<Coefficient> > tmp = this->divideBy(divisor);
  return tmp.remainder;
}

template <typename Coefficient>
bool PolynomialUnivariateDense<Coefficient>::operator==(const int other) const {
  if (other == 0) {
    if (data.size == 0) {
      return true;
    }
    return false;
  }
  if (data.size != 1) {
    return false;
  }
  if (data[0] == other) {
    return true;
  }
  return false;
}

template <typename Coefficient>
//template <typename integral>
void PolynomialUnivariateDense<Coefficient>::clearDenominators() {
  int acc = 1;
  for (int i = 0; i < data.size; i ++) {
    acc = lcm(acc, data[i].getDenominator());
  }
  *this *= acc;
}

template <typename Coefficient>
void PolynomialUnivariateDense<Coefficient>::formalDerivative() {
  if (data.size < 2) {
    data.size = 0;
  }
  for (int i = 1; i < data.size; i ++) {
    data[i - 1] = i * data[i];
  }
  data.size --;
}

template <typename Coefficient>
bool PolynomialUnivariateDense<Coefficient>::operator<(const PolynomialUnivariateDense<Coefficient>& right) const {
  if (data.size < right.data.size) {
    return true;
  }
  if (data.size > right.data.size) {
    return false;
  }
  if (data.size == 0) {
    return false;
  }
  return data[data.size - 1] < right.data[data.size - 1];
}

template <typename Coefficient>
void PolynomialUnivariateDense<Coefficient>::squareFree() {
  global.comments << *this << "\n";
  PolynomialUnivariateDense<Coefficient> p = formalDerivative();
  global.comments << p << "\n";
  PolynomialUnivariateDense<Coefficient> q = gcd(*this, p);
  if (q.data.size > 1) {
    data = (*this / q).data;
  }
}

template <typename Coefficient>
bool CharacterFunctions::spaceContains(
  const List<Vector<Coefficient> >& space,
  const Vector<Coefficient>& vector
) {
  if (vector.isEqualToZero()) {
    if (space.size == 0) {
      return false;
    }
    return true;
  }
  int i = 0;
  while (true) {
    int vi = vector.getIndexFirstNonZeroCoordinate();
    while (true) {
      if (i == space.size) {
        return false;
      }
      int vii = space[i].getIndexFirstNonZeroCoordinate();
      if (vii > vi) {
        return false;
      }
      if (vii == vi) {
        break;
      }
      i ++;
    }
    vector -= space[i] * vector[vi] / space[i][vi];
    if (vector.isEqualToZero()) {
      return true;
    }
  }
}

template<typename Coefficient>
void SpaceTree<Coefficient>::getunion(
  const List<Vector<Coefficient> >& left,
  const List<Vector<Coefficient> >& right,
  List<Vector<Coefficient> >& output
) {
  if (&output == &left || &output == &right) {
    List<Vector<Coefficient> > newOutput;
    getunion(left, right, newOutput);
    output = newOutput;
    return;
  }
  if (left.size == 0) {
    output = right;
    return;
  }
  int d = left[0].size;
  Matrix<Coefficient> matrix;
  matrix.initialize(left.size + right.size,d);
  for (int i = 0; i < left.size; i ++) {
    for (int j = 0; j < d; j ++) {
      matrix.elements[i][j] = left[i][j];
    }
  }
  for (int i = 0; i < right.size; i ++) {
    for (int j = 0; j < d; j ++) {
      matrix.elements[left.size + i][j] = right[i][j];
    }
  }
  matrix.gaussianEliminationByRows(matrix);
  output.setSize(0);
  Vector<Coefficient> v;
  for (int i = 0; i < matrix.numberOfRows; i ++) {
    v.setSize(d);
    for (int j = 0; j < d; j ++) {
      v[j] = matrix.elements[i][j];
    }
    if (v.isEqualToZero()) {
      break;
    }
    output.addOnTop(v);
  }
}

template<typename Coefficient>
void SpaceTree<Coefficient>::intersection(
  const List<Vector<Coefficient> >& left,
  const List<Vector<Coefficient> >& right,
  List<Vector<Coefficient> >& output
) {
  if ((left.size == 0) or (right.size == 0)) {
    output.setSize(0);
    return;
  }
  int d = left[0].size;

  Matrix<Coefficient> MV;
  MV.initialize(left.size, d);
  for (int i = 0; i < left.size; i ++) {
    for (int j = 0; j < d; j ++) {
      MV.elements[i][j] = left[i][j];
    }
  }
  List<Vector<Coefficient> > Vperp;
  MV.getZeroEigenSpaceModifyMe(Vperp);

  Matrix<Coefficient> MW;
  MW.initialize(right.size, d);
  for (int i = 0; i < right.size; i ++) {
    for (int j = 0; j < d; j ++) {
      MW.elements[i][j] = right[i][j];
    }
  }
  List<Vector<Coefficient> > Wperp;
  MW.getZeroEigenSpaceModifyMe(Wperp);

  Matrix<Coefficient> M;
  M.initialize(Vperp.size + Wperp.size,d);
  int i = 0;
  for (; i < Vperp.size; i ++) {
    for (int j = 0; j < d; j ++) {
      M.elements[i][j] = Vperp[i][j];
    }
  }
  for (; i < Vperp.size + Wperp.size; i ++) {
    for (int j = 0; j < d; j ++) {
      M.elements[i][j] = Wperp[i - Vperp.size][j];
    }
  }
  M.getZeroEigenSpaceModifyMe(output);
}

template <typename Coefficient>
List<Vector<Coefficient> > CharacterFunctions::orthogonalComplement(
  const List<Vector<Coefficient> >& V, const List<Vector<Coefficient> >& WW
) {
  List<Vector<Coefficient> > W = intersection(V, WW);
  if (W.size == 0) {
    return V;
  }
  int d = W[0].size;
  Matrix<Coefficient> GM;
  GM.initialize(W.size, W.size);
  for (int i = 0; i < W.size; i ++) {
    for (int j = 0; j < W.size; j ++) {
      GM.elements[i][j] = W[i].scalarEuclidean(W[j]);
    }
  }
  GM.invert();
  Matrix<Coefficient> VM;
  VM.initialize(W.size,V.size);
  for (int i = 0; i < W.size; i ++) {
    for (int j = 0; j < V.size; j ++) {
      VM.elements[i][j] = W[i].scalarEuclidean(V[j]);
    }
  }
  VM.multiplyOnTheLeft(GM);
  Matrix<Coefficient> outm;
  outm.initialize(V.size, d);
  for (int i = 0; i < V.size; i ++) {
    for (int j = 0; j < d; j ++) {
      Coefficient r = V[i][j];
      outm.elements[i][j] = r;
      for (int k = 0; k < W.size; k ++) {
        outm.elements[i][j] -= VM.elements[k][i] * W[k][j];
      }
    }
  }
  Matrix<Coefficient> tmp;
  Selection s;
  outm.gaussianEliminationByRows(outm,tmp,s);
  List<Vector<Coefficient> > out;
  for (int i = 0; i < outm.numberOfRows; i ++) {
    Vector<Coefficient> v;
    v.setSize(d);
    for (int j = 0; j < d; j ++) {
      v[j] = outm.elements[i][j];
    }
    if (v.isEqualToZero()) {
      return out;
    }
    out.addOnTop(v);
  }
  return out;
}

List<List<Vector<Rational> > > CharacterFunctions::eigenspaces(
  const Matrix<Rational>& M, int checkDivisorsOf
) {
  (void) checkDivisorsOf;
  int n = M.numberOfColumns;
  List<List<Vector<Rational> > > spaces;
  int found = 0;
  PolynomialUnivariateDense<Rational> p;
  p.assignMinimalPolynomial(M);
  for (int ii = 0; ii < 2 * n + 2; ii ++) {
    int i = ((ii + 1) / 2) * (2 * (ii % 2) - 1); // 0,1,-1,2,-2,3,-3,...
    Rational r = i;
    if (p(r) == 0) {
      Matrix<Rational> M2 = M;
      List<Vector<Rational> > V;
      M2.getEigenspaceModifyMe(r, V);
      found += V.size;
      spaces.addOnTop(V);
      if (found == M.numberOfColumns) {
        break;
      }
    }
  }
  return spaces;
}

template <typename Coefficient>
Vector<Coefficient> CharacterFunctions::putInBasis(const Vector<Coefficient>& v, const List<Vector<Coefficient> >& B) {
  Vector<Coefficient> w;
  w.makeZero(B.size);
  Matrix<Coefficient> M;
  M.makeZeroMatrix(B.size);
  for (int i = 0; i < B.size; i ++) {
    for (int j = 0; j < v.size; j ++) {
      w[i] += B[i][j] * v[j];
    }
    for (int j = 0; j < B.size; j ++) {
      for (int k = 0; k < v.size; k ++) {
        M.elements[i][j] += B[i][k] * B[j][k];
      }
    }
  }
  M.invert();
  Vector<Coefficient> v2 = M * w;
  Vector<Coefficient> v3;
  v3.makeZero(v.size);
  for (int i = 0; i < B.size; i ++) {
    v3 += B[i] * v2[i];
  }
  if (!(v3 == v)) {
    Vector<Coefficient> out;
    return out;
  }
  return v2;
}

template <typename Coefficient>
bool CharacterFunctions::isIsotypicComponent(
  WeylGroupData& group, const List<Vector<Coefficient> >& vector
) {
  // pre-initial test: V isn't empty
  if (vector.size == 0) {
    return false;
  }
  // initial test: dimension of V is a square
  int n = sqrt(vector.size);
  if (n * n != vector.size) {
    return false;
  }
  // more expensive test: character of V has unit norm
  ClassFunction<FiniteGroup<ElementWeylGroup>, Coefficient> X;
  X.G = &group.group;
  X.data.setSize(group.group.conjugacyClassCount());
  for (int i = 0; i < group.group.conjugacyClassCount(); i ++) {
    ElementWeylGroup& g = group.group.conjugacyClasses[i].representative;
    Coefficient tr = 0;
    for (int j = 0; j < vector.size; j ++) {
      Vector<Coefficient> v = ActOnGroupRing(g, vector[j]);
      v = putInBasis(v, vector);
      tr += v[j];
    }
    X.data[i] = tr;
  }
  // (4, - 1, - 1/2, 1/2, - 1, 1/4, 1/4, 1, 1/2, -7/2)[1]
  if (X.norm() != n) {
    return false;
  }
  // okay now do the really hard test
  Matrix<Coefficient> M = getMatrix(X);
  List<List<Vector<Coefficient> > > spaces = eigenspaces(M);
  if (spaces.size != 2) {
    return false;
  }
  if (spaces[1].size != vector.size) {
    return false;
  }
  if (intersection(spaces[1], vector).size != vector.size) {
    return false;
  }
  return true;
}

Matrix<Rational> CharacterFunctions::matrixInBasis(
  const ClassFunction<FiniteGroup<ElementWeylGroup>, Rational>& X, const List<Vector<Rational> >& B
) {
  List<Vector<Rational> > rows;
  for (int i = 0; i < B.size; i ++) {
    Vector<Rational> v;
    v.makeZero(B[0].size);
    rows.addOnTop(v);
  }
  for (int i1 = 0; i1 < X.G->conjugacyClassCount(); i1 ++) {
    for (int i2 = 0; i2 < X.G->conjugacyClasses[i1].size; i2 ++) {
      for (int j = 0; j < X.G->getSize(); j ++) {
        for (int k = 0; k < B.size; k ++) {
          int l = X.G->elements.getIndex(
            X.G->conjugacyClasses[i1].elements[i2] * X.G->elements[j]
          );
          rows[k][l] = X.data[i1] * B[k][j];
        }
      }
    }
  }
  Matrix<Rational> M;
  M.initialize(rows.size, rows.size);
  for (int i = 0; i < rows.size; i ++) {
    Vector<Rational> v = putInBasis(rows[i], B);
    for (int j = 0; j < rows.size; j ++) {
      M.elements[i][j] = v[j];
    }
  }
  return M;
}

template <typename Coefficient>
Matrix<Coefficient> CharacterFunctions::getMatrix(
  const ClassFunction<WeylGroupData::WeylGroupBase, Coefficient>& classFunction
) {
  Matrix<Coefficient> M;
  M.makeZeroMatrix(classFunction.G->N);
  for (int i1 = 0; i1 < classFunction.G->ccCount; i1 ++) {
    for (int i2 = 0; i2 < classFunction.G->ccSizes[i1]; i2 ++) {
      int i = classFunction.G->conjugacyClasses[i1][i2];
      for (int j = 0; j < classFunction.G->N; j ++) {
        M(classFunction.G->multiplyElements(i, j), j) = classFunction.data[i1];
      }
    }
  }
  return M;
}

void SubgroupDataRootReflections::computeDynkinType() {
  WeylGroupData tempGroup;
  tempGroup.cartanSymmetric = this->SubCartanSymmetric;
  tempGroup.makeMeFromMyCartanSymmetric();
  this->dynkinType = tempGroup.dynkinType;
}

SubgroupDataWeylGroup::SubgroupDataWeylGroup() {
  this->weylData = nullptr;
}

SubgroupDataRootReflections::SubgroupDataRootReflections() {
  this->flagIsParabolic = false;
  this->flagIsExtendedParabolic = false;
}

bool SubgroupDataWeylGroup::checkInitialization() {
  if (this->weylData == nullptr) {
    global.fatal << "SubgroupDataWeylGroup: non-initialized theWeylData pointer. " << global.fatal;
  }
  return true;
}

template <class someGroup, class elementSomeGroup>
std::string SubgroupData<someGroup, elementSomeGroup>::toString(FormatExpressions* format) {
  MacroRegisterFunctionWithName("SubgroupData::toString");
  (void) format;
  if (this->groupContent == nullptr) {
    return "(not initialized (no owner group))";
  }
  if (this->subgroupContent == nullptr) {
    return "(not initialized (subgroup pointer is 0))";
  }
  std::stringstream out;
  out << "<br>\nSubgroup: " << this->subgroupContent->toString();

  out << "<br>\nOwner group: " << this->groupContent->toString();

  return out.str();
}

std::string SubgroupDataRootReflections::toString(FormatExpressions* format) {
  MacroRegisterFunctionWithName("SubgroupDataRootReflections::toString");
  std::stringstream out;
  out << subGroupDataContainer.toString(format);
  return out.str();
}

std::string SubgroupDataWeylGroup::toString(FormatExpressions* format) {
  MacroRegisterFunctionWithName("SubgroupDataWeylGroup::toString");
  std::stringstream out;
  out << this->subgroupData.toString(format);
  return out.str();
}

void SubgroupDataWeylGroup::ComputeTauSignature() {
  MacroRegisterFunctionWithName("SubgroupWeylGroup::ComputeTauSignature");
  this->checkInitialization();
  if (!this->subgroupData.groupContent->flagCCRepresentativesComputed) {
    this->subgroupData.groupContent->computeConjugacyClassSizesAndRepresentatives();
  }
  this->subgroupData.groupContent->checkConjugacyClassRepresentationsMatchCCSizes();
  this->subgroupData.groupContent->checkOrthogonalityCharacterTable();
  Vector<Rational> Xs;
  this->subgroupData.subgroupContent->getSignCharacter(Xs);
  this->subgroupData.computeCCRepresentativesPreimages();
  //if (global.printOutThisKindaThing)
  //{
  //global.Comments << "here is the character table of the group, the representatives and sizes for "
  //         << "conjugacy classes of the group and the subgroup, and for your convenience, "
  //         << "a mapping from subgroup conjugacy classes to their preimages in the group. ";
  //global.Comments << this->theSubgroupData.theGroup->prettyPrintCharacterTable();
  //global.Comments << Xs << " <- subgroup sign character\n";
  //global.Comments << this->theSubgroupData.theGroup->prettyPrintCCRepsSizes();
  //global.Comments << this->theSubgroupData.theSubgroup->prettyPrintCCRepsSizes();
  //global.Comments << this->theSubgroupData.ccRepresentativesPreimages.toStringCommaDelimited() << '\n';
 //}

  this->tauSignature.setSize(this->subgroupData.groupContent->conjugacyClassCount());

  Vector<Rational> XiS;
  XiS.makeZero(this->subgroupData.subgroupContent->conjugacyClasses.size);
  for (int i = 0; i < this->subgroupData.groupContent->conjugacyClasses.size; i ++) {
    ClassFunction<FiniteGroup<ElementWeylGroup>, Rational>& XiG =
    this->weylData->group.characterTable[i];
    //global.Comments << "Restricting character: " << Xip.toString() << "<br>";
    for (int j = 0; j < this->subgroupData.subgroupContent->conjugacyClasses.size; j ++) {
      XiS[j] = XiG[this->subgroupData.ccRepresentativesPreimages[j]];
    }
    this->tauSignature[i] = this->subgroupData.subgroupContent->getHermitianProduct(Xs, XiS);
    //global.Comments << "<br>Hermitian product of " << Xs.toString() << " and "
    //<< XiS.toString() << " = " << this->tauSignature[i];
    if (!this->tauSignature[i].isSmallInteger()) {
      global.fatal << " Tau signature is not integral, something is wrong. " << global.fatal ;
    }
  }
}

void SubgroupDataRootReflections::computeCCSizesRepresentativesPreimages() {
  MacroRegisterFunctionWithName("SubgroupRootReflections::computeCCSizesRepresentativesPreimages");
  if (this->dynkinType == this->weylData->dynkinType && this->weylData->group.flagCCRepresentativesComputed) {
    this->subgroupData.subgroupContent->conjugacyClasses.setSize(this->subgroupData.groupContent->conjugacyClasses.size);
    for (int i = 0; i < this->subgroupData.subgroupContent->conjugacyClasses.size; i ++) {
      this->subgroupData.subgroupContent->conjugacyClasses[i].flagRepresentativeComputed = true;
      this->subgroupData.subgroupContent->conjugacyClasses[i].representative = this->subgroupData.groupContent->conjugacyClasses[i].representative;
      this->subgroupData.subgroupContent->conjugacyClasses[i].size = this->subgroupData.groupContent->conjugacyClasses[i].size;
      this->subgroupData.subgroupContent->conjugacyClasses[i].flagElementsComputed = false;
    }
    this->subgroupData.ccRepresentativesPreimages.setSize(this->subgroupData.groupContent->conjugacyClasses.size);
    for (int i = 0; i < this->subgroupData.ccRepresentativesPreimages.size; i ++) {
      this->subgroupData.ccRepresentativesPreimages[i] = i;
    }
    this->subgroupData.subgroupContent->flagCCRepresentativesComputed = true;
  } else {
    if (this->dynkinType.getRank() <= 6) {
      this->subgroupData.subgroupContent->computeConjugacyClassesFromAllElements();
    } else {
      this->subgroupData.subgroupContent->computeConjugacyClassSizesAndRepresentatives();
    }

    this->subgroupData.computeCCRepresentativesPreimages();
  }
}

void SubgroupDataRootReflections::initializeGenerators() {
  MacroRegisterFunctionWithName("SubgroupRootReflections::initializeGenerators");
  if (this->dynkinType.getRank() == 0) {
    this->subgroupData.subgroupContent->generators.setSize(1);
    this->subgroupData.subgroupContent->generators[0].makeIdentity(*this->subgroupData.groupContent);
    return;
  }
  int d = this->SubCartanSymmetric.numberOfRows;
  this->subgroupData.generatorPreimages.setSize(d);
  this->subgroupData.subgroupContent->generators.setSize(d);
  ElementWeylGroup currentReflection;
  for (int i = 0; i < d; i ++) {
    currentReflection.MakeRootReflection(this->generatingSimpleRoots[i], *this->weylData);
    this->subgroupData.generatorPreimages[i] = this->subgroupData.groupContent->elements.getIndex(currentReflection);
    this->subgroupData.subgroupContent->generators[i] = currentReflection;
  }
}

void SubgroupDataRootReflections::makeParabolicSubgroup(WeylGroupData& G, const Selection& inputGeneratingSimpleRoots) {
  MacroRegisterFunctionWithName("SubgroupDataRootReflections::makeParabolicSubgroup");
  G.checkConsistency();
  this->weylData = &G;
  this->subgroupData.makeSubgroupOf(G.group);
  this->checkInitialization();
  this->flagIsParabolic = true;
  this->simpleRootsInLeviParabolic = inputGeneratingSimpleRoots;
  Vectors<Rational> EiBasis;
  EiBasis.makeEiBasis(G.getDimension());
  EiBasis.subSelection(inputGeneratingSimpleRoots, this->generatingSimpleRoots);
  int d = inputGeneratingSimpleRoots.cardinalitySelection;
  this->SubCartanSymmetric.initialize(d, d);
  for (int ii = 0; ii < d; ii ++) {
    for (int jj = 0; jj < d; jj ++) {
      this->SubCartanSymmetric(ii, jj) = G.rootScalarCartanRoot(
        this->generatingSimpleRoots[ii], this->generatingSimpleRoots[jj]
      );
    }
  }
  this->computeDynkinType();
  this->initializeGenerators();
}

void SubgroupDataRootReflections::makeFromRoots(WeylGroupData& G, const Vectors<Rational>& inputRootReflections) {
  MacroRegisterFunctionWithName("SubgroupDataRootReflections::makeFromRoots");
  this->subgroupData.makeSubgroupOf(G.group);
  this->weylData = &G;
  this->generatingSimpleRoots = inputRootReflections;
  DynkinDiagramRootSubalgebra diagram;
  diagram.computeDiagramTypeModifyInput(this->generatingSimpleRoots, G);
  int d = this->generatingSimpleRoots.size;
  this->SubCartanSymmetric.initialize(d, d);
  for (int ii = 0; ii < d; ii ++) {
    for (int jj = 0; jj < d; jj ++) {
      this->SubCartanSymmetric(ii, jj) = G.rootScalarCartanRoot(
        this->generatingSimpleRoots[ii], generatingSimpleRoots[jj]
      );
    }
  }
  this->computeDynkinType();
  DynkinType verificationType;
  diagram.getDynkinType(verificationType);
  if (this->dynkinType != verificationType) {
    global.fatal << "Two different comptuations of the Dynkin type a set of roots did not coincide. " << global.fatal;
  }
  this->initializeGenerators();
}

template <class elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::areConjugate_OLD_Deprecated_Version_By_Todor(
  const elementSomeGroup& left, const elementSomeGroup& right
) {
  MacroRegisterFunctionWithName("WeylGroup::areConjugate_OLD_Deprecated_Version_By_Todor");
  if (left.hasDifferentConjugacyInvariantsFrom(right)) {
    return false;
  }
  OrbitIteratorWeylGroup theIterator;
  theIterator.initialize(this->generators, left, ElementWeylGroup::conjugationAction);
  if (this->generators.size == 0) {
    global.fatal << "Generators not allowed to be 0. " << global.fatal;
  }
  do {
    //if (left.toString() == "s_{4}")
    //  global.Comments << "<br>" << theIterator.getCurrentElement().toString() << "=?=" << right.toString();
    if (theIterator.getCurrentElement() == right) {
      return true;
    }
  } while (theIterator.incrementReturnFalseIfPastLast());
  return false;
}

void WeylGroupData::getSignSignatureAllRootSubsystems(List<SubgroupDataRootReflections>& outputSubgroups) {
  MacroRegisterFunctionWithName("WeylGroup::getSignSignatureAllRootSubsystems");
  RootSubalgebras theRootSAs;
  SemisimpleLieAlgebra theSSlieAlg;
  theSSlieAlg.weylGroup.makeFromDynkinType(this->dynkinType);
//  theSSlieAlg.computeChevalleyConstants(global);
  theRootSAs.owner = &theSSlieAlg;
  theRootSAs.computeAllReductiveRootSubalgebrasUpToIsomorphism();
  List<Vectors<Rational> > theRootSAsBases;
  theRootSAsBases.setExpectedSize(theRootSAs.subalgebras.size);
  for (int i = theRootSAs.subalgebras.size - 1; i >= 0; i --) {
    theRootSAsBases.addOnTop(theRootSAs.subalgebras[i].simpleRootsReductiveSubalgebra);
  }
  this->getSignSignatureRootSubgroups(outputSubgroups, theRootSAsBases);
}

void WeylGroupData::getSignSignatureParabolics(List<SubgroupDataRootReflections>& outputSubgroups) {
  MacroRegisterFunctionWithName("WeylGroup::getSignSignatureParabolics");
//  this->computeIrreducibleRepresentationsThomasVersion();
  this->computeOrLoadCharacterTable();
  ClassFunction<WeylGroupData::WeylGroupBase, Rational> signRep;
  signRep.G = &(this->group);
  this->getSignCharacter(signRep.data);
  Selection sel;
  sel.initialize(this->getDimension());
  int numCycles = MathRoutines::twoToTheNth(sel.numberOfElements);
  outputSubgroups.setSize(numCycles);
  ElementWeylGroup g;
  g.owner = this;
//  global.Comments << "<hr>Meself is: " << this->toString();
  for (int i = 0; i < outputSubgroups.size; i ++, sel.incrementSelection()) {
    SubgroupDataRootReflections& currentParabolic = outputSubgroups[i];
    currentParabolic.makeParabolicSubgroup(*this, sel);
    currentParabolic.computeCCSizesRepresentativesPreimages();
//    global.Comments << "<hr>Current parabolic is: " << currentParabolic.toString();
    // ComputeInitialCharacters gets the character of the sign representation
    // as characterTable[1]
    //global.Comments << "<hr>before compute initial irreps";
  }
  this->group.checkConjugacyClassRepresentationsMatchCCSizes();
  for (int j = 0; j < outputSubgroups.size; j ++) {
    outputSubgroups[j].ComputeTauSignature();
  }
}

void WeylGroupData::getSignSignatureExtendedParabolics(List<SubgroupDataRootReflections>& outputSubgroups) {
  MacroRegisterFunctionWithName("WeylGroup::getSignSignatureExtendedParabolics");
//  this->computeIrreducibleRepresentationsThomasVersion();
  this->computeOrLoadCharacterTable();
  ClassFunction<WeylGroupData::WeylGroupBase, Rational> signRep;
  signRep.G = &(this->group);
  this->getSignCharacter(signRep.data);
  Selection parSelrootsAreInLevi;
  parSelrootsAreInLevi.initialize(this->getDimension() + 1);
  Vectors<Rational> extendedBasis, currentBasisExtendedParabolic;
  extendedBasis.makeEiBasis(this->getDimension());
  extendedBasis.addOnTop(this->rootSystem[0]);
  outputSubgroups.setExpectedSize(MathRoutines::twoToTheNth(this->getDimension()));
  outputSubgroups.setSize(0);
  SubgroupDataRootReflections theSG;
  do {
    extendedBasis.subSelection(parSelrootsAreInLevi, currentBasisExtendedParabolic);
    if (currentBasisExtendedParabolic.getRankElementSpan() == currentBasisExtendedParabolic.size) {
      theSG.makeFromRoots(*this, currentBasisExtendedParabolic);
      theSG.flagIsExtendedParabolic = true;
      theSG.simpleRootsInLeviParabolic = parSelrootsAreInLevi;
      outputSubgroups.addOnTop(theSG);
    }
  } while (parSelrootsAreInLevi.incrementReturnFalseIfPastLast());
  for (int i = 0; i < outputSubgroups.size; i ++) {
    outputSubgroups[i].computeCCSizesRepresentativesPreimages();
  }
  this->group.checkConjugacyClassRepresentationsMatchCCSizes();
  for (int j = 0; j < outputSubgroups.size; j ++) {
    outputSubgroups[j].ComputeTauSignature();
  }
}

void WeylGroupData::getSignSignatureRootSubgroups(
  List<SubgroupDataRootReflections>& outputSubgroups, const List<Vectors<Rational> >& rootsGeneratingReflections
) {
  MacroRegisterFunctionWithName("WeylGroup::getSignSignatureRootSubgroups");
  this->computeOrLoadCharacterTable();
  ClassFunction<WeylGroupData::WeylGroupBase, Rational> signRep;
  signRep.G = &(this->group);
  this->getSignCharacter(signRep.data);
  outputSubgroups.setSize(rootsGeneratingReflections.size);
  ElementWeylGroup g;
  g.owner = this;
  for (int i = 0; i < outputSubgroups.size; i ++) {
    SubgroupDataRootReflections& currentParabolic = outputSubgroups[i];
    currentParabolic.makeFromRoots(*this, rootsGeneratingReflections[i]);
    currentParabolic.computeCCSizesRepresentativesPreimages();
  }
  this->group.checkConjugacyClassRepresentationsMatchCCSizes();
  this->group.checkOrthogonalityCharacterTable();
  for (int j = 0; j < outputSubgroups.size; j ++) {
    outputSubgroups[j].ComputeTauSignature();
  }
}

void CharacterFunctions::computeTauSignatures(
  WeylGroupData* group, List<List<bool> >& tauSignatures, bool pseudo
) {
  Selection sel;
  sel.initialize(group->cartanSymmetric.numberOfColumns);
  int numCycles = MathRoutines::twoToTheNth(sel.numberOfElements);
  List<List<bool> > tss;
  tss.setSize(numCycles);
  List<ElementWeylGroup> generators;
  for (int i = 0; i < numCycles - 2; i ++) {
    sel.incrementSelection();
    generators.setSize(sel.cardinalitySelection);
    for (int j = 0; j < sel.cardinalitySelection; j ++) {
      generators[j].makeSimpleReflection(sel.elements[j], *group);
    }
    getTauSignaturesFromSubgroup(*group, generators, tss[i]);
  }
  Vector<Rational> Xs;
  group->getSignCharacter(Xs);
  List<bool> tsg;
  tsg.setSize(group->group.characterTable.size);
  for (int i = 0; i < group->group.characterTable.size; i ++) {
    tsg[i] = group->group.characterTable[i].data == Xs;
  }
  tss.addOnTop(tsg);

  if (pseudo) {
    global.comments << "pseudo-parabolics" << "\n";
    ElementWeylGroup hr = group->getRootReflection(group->rootSystem.size- 1);
    sel.initialize(group->cartanSymmetric.numberOfColumns);
    for (int i = 0; i < numCycles - 1; i ++) {
      generators.setSize(sel.cardinalitySelection);
      for (int j = 0; j < sel.cardinalitySelection; j ++) {
        generators[j].makeSimpleReflection(sel.elements[j], *group);
      }
      generators.addOnTop(hr);
      List<bool> ts;
      getTauSignaturesFromSubgroup(*group, generators, ts);
      tss.addOnTop(ts);
      sel.incrementSelection();
    }
  }

  // we will need the sign character for the group


  tauSignatures.setSize(group->group.characterTable.size);
  for (int i = 0; i < group->group.characterTable.size; i ++) {
    tauSignatures[i].setSize(tss.size + 1);
    tauSignatures[i][0] = 1;
    for (int j = 1; j < tss.size + 1; j ++) {
      tauSignatures[i][j] = tss[j - 1][i];
    }
  }
  for (int i = 0; i < group->group.characterTable.size; i ++) {
    global.comments << group->group.characterTable[i] << "\n";
    for (int j = 0; j < tauSignatures[i].size; j ++) {
      global.comments << tauSignatures[i][j] << ' ';
    }
    global.comments << "\n";
  }
}

template <typename elementSomeGroup>
void CharacterFunctions::exportCharTable(FiniteGroup<elementSomeGroup>& G, JSData& data) {
  data.elementType = JSData::token::tokenObject;
  JSData emptyArray;
  emptyArray.makeEmptyArray();
  JSData& representatives = data.objects.getValueCreate("representatives", emptyArray);
  JSData& sizes = data.objects.getValueCreate("sizes", emptyArray);
  JSData& characters = data.objects.getValueCreate("characters", emptyArray);
  representatives.elementType = JSData::token::tokenArray;
  representatives.listObjects.setSize(G.conjugacyClassCount());

  for (int i = 0; i < G.conjugacyClassCount(); i ++) {
    List<int> reprefs;
    G.getWord(G.conjugacyClasses[i].representative, reprefs);
    representatives.listObjects[i].elementType = JSData::token::tokenArray;
    representatives.listObjects[i].listObjects.setSize(reprefs.size);
    for (int j = 0; j < reprefs.size; j ++) {
      representatives.listObjects[i].listObjects[j].elementType = JSData::token::tokenLargeInteger;
      representatives.listObjects[i].listObjects[j].integerValue.getElement() = reprefs[j];
    }
  }
  sizes.elementType = JSData::token::tokenArray;
  sizes.listObjects.setSize(G.conjugacyClassCount());
  for (int i = 0; i < G.conjugacyClassCount(); i ++) {
    sizes.listObjects[i].elementType = JSData::token::tokenLargeInteger;
    sizes.listObjects[i].integerValue.getElement() = G.conjugacyClasses[i].size;
  }
  characters.elementType = JSData::token::tokenArray;
  characters.listObjects.setSize(G.characterTable.size);
  for (int i = 0; i < G.characterTable.size; i ++) {
    for (int j = 0; j < G.characterTable[i].data.size; j ++) {
      characters[i][j] = G.characterTable[i][j].getNumerator();
    }
  }
}

void CharacterFunctions::exportTauSignatures(
  WeylGroupData& groupData, const List<List<bool> >& ts, JSData& output
) {
  CharacterFunctions::exportCharTable(groupData.group, output["chartable"]);
  for (int i = 0; i < ts.size; i ++) {
    for (int j = 0; j < ts[i].size; j ++) {
      output["tausigs"][i][j] = ts[i][j];
    }
  }
}

template <typename Coefficient>
List<VectorSpace<Coefficient> > CharacterFunctions::getEigenspaces(
  const Matrix<Coefficient>& matrix
) {
  List<List<Vector<Coefficient> > > es = eigenspaces(matrix);
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

void CharacterFunctions::getTauSignaturesFromSubgroup(
  WeylGroupData& G, const List<ElementWeylGroup>& gens, List<bool>& out
) {
  SubgroupData<FiniteGroup<ElementWeylGroup>, ElementWeylGroup> HD;
  HD.initFromGroupAndGenerators(G.group, gens);
  FiniteGroup<ElementWeylGroup>& H = *HD.subgroupContent;
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

// Reference: Schneider, 1990.
// TODO(tmilev): apply improvements described in the reference
template <typename Somegroup>
List<ClassFunction<Somegroup, Rational> > CharacterFunctions::computeCharacterTable(Somegroup &group) {
  if (group.conjugacyClassCount() == 0) {
    group.computeConjugacyClassesFromAllElements();
  }
  List<int> classmap;
  int sizeOfG = - 1;
  group.getSize().isIntegerFittingInInt(&sizeOfG);
  classmap.setSize(sizeOfG);
  for (int i = 0; i < group.conjugacyClassCount(); i ++) {
    for (int j = 0; j < group.conjugacyClasses[i].size; j ++) {
      classmap[group.elements.getIndex(group.conjugacyClasses[i].elements[j])] = i;
    }
  }
  Matrix<Rational> form; // so inefficient
  form.makeZeroMatrix(group.conjugacyClassCount());
  for (int i = 0; i < group.conjugacyClassCount(); i ++) {
    form.elements[i][i] = group.conjugacyClasses[i].size;
  }
  List<VectorSpace<Rational> > spaces;
  if (group.characterTable.size > 0) {
    VectorSpace<Rational> allchars;
    for (int i = 0; i < group.characterTable.size; i ++) {
      VectorSpace<Rational> xspi;
      xspi.addVector(group.characterTable[i].data);
      spaces.addOnTop(xspi);
      allchars.addVector(group.characterTable[i].data);
    }
    spaces.addOnTop(allchars.orthogonalComplement(nullptr, &form));
  } else {
    Vector<Rational> X1;
    X1.setSize(group.conjugacyClassCount());
    for (int i = 0; i < group.conjugacyClassCount(); i ++) {
      X1[i] = 1;
    }
    VectorSpace<Rational> sp1;
    sp1.addVector(X1);
    spaces.addOnTop(sp1);
    spaces.addOnTop(sp1.orthogonalComplement(nullptr, &form));
  }
  bool foundEmAll = false;
  for (int i = 0; !foundEmAll && i < group.conjugacyClassCount(); i ++) {
    Matrix<Rational> M;
    global.comments << "Getting class matrix " << i << "\n";
    M = getClassMatrix(group,i,&classmap);
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
          if (spaces.size == group.conjugacyClassCount()) {
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
  List<ClassFunction<Somegroup, Rational> > chars;
  chars.setSize(spaces.size);
  for (int i = 0; i < spaces.size; i ++) {
    chars[i].data = spaces[i].getCanonicalBasisVector(0);
    chars[i].G = &group;
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
  group.characterTable = chars;
  for (int i = 0; i < group.characterTable.size; i ++) {
    global.comments << group.characterTable[i] << "\n";
  }
  return chars;
}

template <typename Somegroup>
Matrix<Rational> CharacterFunctions::getClassMatrix(
  const Somegroup &group, int cci, List<int>* classmap
) {
  List<int> invl;
  int classSize = - 1;
  group.conjugacyClasses[cci].size.isIntegerFittingInInt(&classSize);
  invl.setSize(classSize);
  for (int i = 0; i < group.conjugacyClasses[cci].size; i ++) {
    invl[i] = group.invert(group.elements.getIndex(group.conjugacyClasses[cci].elements[i]));
  }
  Matrix<int> M;
  M.makeZeroMatrix(group.conjugacyClassCount());
  for (int t = 0; t < group.conjugacyClassCount(); t ++)
    for (int xi = 0; xi < invl.size; xi ++) {
      int yi = group.multiplyElements(invl[xi], group.elements.getIndex(group.conjugacyClasses[t].representative));
      int ci;
      if (classmap) {
        M.elements[t][(*classmap)[yi]] += 1;
      } else {
        for (ci = 0; ci < group.conjugacyClassCount(); ci ++) {
          if (group.conjugacyClasses[ci].indicesEltsInOwner.BSContains(yi)) {
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
