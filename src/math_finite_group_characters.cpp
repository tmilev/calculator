// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general.h"
#include "math_extra_semisimple_Lie_algebras_root_subalgebras.h" // WeylGroup::GetSignSignatureAllRootSubsystems
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_symmetric_groups_and_generalizations.h"
#include "math_extra_graph.h"
#include "math_general_implementation.h"

template<>
List<ElementWeylGroup>::Comparator* FormatExpressions::GetMonOrder<ElementWeylGroup>() {
  return nullptr;
}

Vector<Rational> WeylGroupData::ApplyReflectionList(const List<int>& rightReflectionsActFirst, const Vector<Rational>& vv) const {
  Vector<Rational> v = vv;
  for (int i = rightReflectionsActFirst.size - 1; i >= 0; i --) {
    Rational x = 0;
    for (int j = 0; j < this->getDimension(); j ++) {
      x += v[j] * cartanSymmetric(rightReflectionsActFirst[i], j);
    }
    //cartanSymmetric(i, j) is slower than cartanSymmetric.elements[i][j] but includes index checking
    //(we want to catch a rogue call to ApplyReflectionList
    v[rightReflectionsActFirst[i]] -= x * 2 / cartanSymmetric(rightReflectionsActFirst[i], rightReflectionsActFirst[i]);
  }
  return v;
}

void WeylGroupData::GetSimpleReflectionMatrix(int indexSimpleRoot, Matrix<Rational>& output) const {
  MacroRegisterFunctionWithName("WeylGroup::GetSimpleReflectionMatrix");
  int rank = this->getDimension();
  output.MakeIdMatrix(rank);
  for (int j = 0; j < rank; j ++) {
    output(indexSimpleRoot, j) -= (this->cartanSymmetric(indexSimpleRoot, j) / cartanSymmetric(indexSimpleRoot, indexSimpleRoot)) * 2;
  }
}

Matrix<Rational> WeylGroupData::SimpleReflectionMatrix(int i) const {
  Matrix<Rational> result;
  this->GetSimpleReflectionMatrix(i, result);
  return result;
}

ElementWeylGroup WeylGroupData::SimpleConjugation(int i, const ElementWeylGroup& vv) {
  (void) vv;
  ElementWeylGroup eltSimpleReflection;
  eltSimpleReflection.MakeSimpleReflection(i, *this);
  return eltSimpleReflection * this->theGroup.theElements[i] * eltSimpleReflection;
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeSquaresCCReps() {
  MacroRegisterFunctionWithName("WeylGroup::ComputeSquares");
  if (!this->flagCCsComputed) {
    this->computeConjugacyClassesFromAllElements();
  }
  this->squaresCCReps.setExpectedSize(this->ConjugacyClassCount());
  this->squaresCCReps.setSize(this->ConjugacyClassCount());
  //elementSomeGroup currentSquare;
  for (int i = 0; i < this->ConjugacyClassCount(); i ++) {
    this->squaresCCReps[i] = this->conjugacyClasses[i].representative * this->conjugacyClasses[i].representative;
  }
}

void WeylGroupData::ComputeInitialIrreps() {
  MacroRegisterFunctionWithName("WeylGroup::ComputeInitialIrreps");
  if (!this->theGroup.flagCCsComputed) {
    this->theGroup.computeConjugacyClassesFromAllElements();
  }
  if (this->theGroup.squaresCCReps.size == 0) {
    this->theGroup.ComputeSquaresCCReps();
  }
  this->theGroup.irreps.setSize(0);
  this->theGroup.characterTable.setSize(0);
  this->theGroup.irreps_grcam.setSize(0);
  this->theGroup.characterTable.setExpectedSize(this->theGroup.ConjugacyClassCount());
  this->theGroup.irreps.setExpectedSize(this->theGroup.ConjugacyClassCount());
  this->theGroup.irreps_grcam.setExpectedSize(this->theGroup.ConjugacyClassCount());
  GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup>, Rational> trivialRep, signRep, standardRep;
  this->GetTrivialRepresentation(trivialRep);
  this->GetSignRepresentation(signRep);
  this->GetStandardRepresentation(standardRep);
  this->theGroup.AddIrreducibleRepresentation(trivialRep);
  this->theGroup.AddIrreducibleRepresentation(signRep);
  this->theGroup.AddIrreducibleRepresentation(standardRep);
}

// This is dumb, but i couldnt figure out what else to do
template<> // haha wat
unsigned int VectorSpace<Rational>::hashFunction(const VectorSpace<Rational>& in) {
  return in.fastbasis.hashFunction();
}
// whenever i figure out cxx i can get rid of this.  but im probably jst
// going to define this for every other field


template <typename Coefficient>
void matrixInBasis(
  Matrix<Coefficient>& out,
  const Matrix<Coefficient>& in,
  const List<Vector<Coefficient> >& basis,
  const Matrix<Coefficient>& gramMatrix
) {
  int d = basis.size;
  out.initialize(d, d);
  for (int i = 0; i < d; i ++) {
    for (int j = 0; j < d; j ++) {
      out.elements[i][j] = basis[i].ScalarEuclidean(in * basis[j]);
    }
  }
  out.multiplyOnTheLeft(gramMatrix);
}

template <typename Coefficient>
void MatrixInBasisFast(Matrix<Coefficient>& out, const Matrix<Coefficient>& in, const Matrix<Coefficient>& BM) {
  int d = BM.numberOfRows;
  Matrix<Coefficient> M = BM;
  Matrix<Coefficient> inT = in;
  inT.transpose();
  M.MultiplyOnTheRight(inT);
  out.MakeZeroMatrix(d);
  for (int i = 0; i < d; i ++) {
    int jj = 0;
    for (int j = 0; j < d; j ++) {
      while ((jj < M.numberOfColumns) and (M.elements[i][jj] == 0)) {
        jj ++;
      }
      if (jj == M.numberOfColumns) {
        out.elements[i][j] = 0;
        continue;
      }
      if (BM.elements[j][jj] == 0) {
        out.elements[i][j] = 0;
        continue;
      }
      out.elements[i][j] = M.elements[i][jj] / BM.elements[j][jj];
      for (int k = jj; k < M.numberOfColumns; k ++) {
        M.elements[i][k] -= BM.elements[j][k] * out.elements[i][j];
      }
    }
  }
}

template <typename Coefficient>
class SpaceTree {
public:
   List<Vector<Coefficient> > space;
   List<SpaceTree<Coefficient> > subspaces;

   void PlaceInTree(const List<Vector<Coefficient> >& V);
   void GetLeaves(List<List<Vector<Coefficient> > >& leaves) const;
   void DisplayTree() const;
};

template <typename Coefficient>
void SpaceTree<Coefficient>::PlaceInTree(const List<Vector<Coefficient> > &V) {
  List<Vector<Coefficient> > U;
  for (int i = 0; i < subspaces.size; i ++) {
    intersection(V, subspaces[i].space, U);
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
  intersection(W, V, tempVspace);
  if (tempVspace.size != V.size) {
    SpaceTree<Coefficient> vst;
    vst.space = V;
    subspaces.addOnTop(vst);
  }
}

template <typename Coefficient>
void SpaceTree<Coefficient>::GetLeaves(List<List<Vector<Coefficient> > >& leaves) const {
  if (subspaces.size > 0) {
    for (int i = 0; i < subspaces.size; i ++) {
      subspaces[i].GetLeaves(leaves);
    }
  } else {
    leaves.addOnTop(space);
  }
}

template <typename Coefficient>
void SpaceTree<Coefficient>::DisplayTree() const {
  global.comments << '[' << space.size << ',';
  for (int i = 0; i < subspaces.size; i ++) {
    subspaces[i].DisplayTree();
  }
//  global.Comments << ']';
}

template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::multiplyBy(
  const GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>& other,
  GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>& output
) const {
  //lazy programmers handling://////
  if (&output == this || &output == &other) {
    GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> thisCopy, otherCopy;
    thisCopy = *this;
    otherCopy = other;
    thisCopy.multiplyBy(otherCopy, output);
    return;
  }
  //////////////////////////////////
  if (this->ownerGroup != other.ownerGroup) {
    global.fatal << "This is a programming error: attempting to multiply "
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
  /* would that it be this simple
  if ((classFunctionMatrices.size > 0) && (other.classFunctionMatrices.size > 0)) {
    U.classFunctionMatrices.setSize(G->ccCount);
    for (int i = 0; i <classFunctionMatrices.size; i ++) {
      if ((classFunctionMatrices[i].numberOfColumns > 0) && (other.classFunctionMatrices[i].numberOfColumns > 0))
      U.classFunctionMatrices[i].assignTensorProduct(classFunctionMatrices[i],other.classFunctionMatrices[i]);
    }
  }*/
  output.ownerGroup = this->ownerGroup;
}

template <typename somegroup, typename Coefficient>
GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>
GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::Reduced() const {
  int d = basis.size;
  Matrix<Coefficient> GM;
  GM.initialize(d, d);
  for (int i = 0; i < d; i ++) {
    for (int j = 0; j < d; j ++) {
      GM.elements[i][j] = this->basis[i].ScalarEuclidean(this->basis[j]);
    }
  }
  GM.invert();
  GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> out;
  out.generatorS.setSize(this->generatorS.size);
  for (int i = 0; i < this->generatorS.size; i ++) {
  //     MatrixInBasisFast(out.generatorS[i], this->generatorS[i], BM);
    matrixInBasis(out.generatorS[i], this->generatorS[i], this->basis, GM);
  }
  out.ownerGroup = ownerGroup;
  out.basis.MakeEiBasis(d);

/*
   if (classFunctionMatrices.size > 0) {
     out.classFunctionMatrices.setSize(G->ccCount);
     for (int i = 0; i <classFunctionMatrices.size; i ++) {
        if (classFunctionMatrices[i].numberOfRows > 0) {
          //MatrixInBasisFast(out.classFunctionMatrices[i],classFunctionMatrices[i],BM);
          matrixInBasis(out.classFunctionMatrices[i],classFunctionMatrices[i],basis,GM);
        }
      }
   }
*/
   return out;
}

template <typename somegroup, typename Coefficient>
VectorSpace<Coefficient> GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::FindDecentBasis() const {
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
  GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::DecomposeThomasVersion() {
  MacroRegisterFunctionWithName("WeylGroupRepresentation::DecomposeThomasVersion");
  Matrix<Coefficient> splittingOperatorMatrix;
  List<GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> > out;
  List<Vector<Rational> > splittingMatrixKernel;
  if (GetNumberOfComponents() == 1) {
    if (this->ownerGroup->characterTable.getIndex(this->theCharacteR) == - 1) {
      global.comments << "new irrep found, have " << this->ownerGroup->characterTable.size << "\n";
      this->ownerGroup->AddIrreducibleRepresentation(*this);
    }
    out.addOnTop(*this);
    return out;
  }
  List<Vector<Coefficient> > Vb = this->basis;
  List<Vector<Coefficient> > tempVectors;
  for (int i = 0; i < this->ownerGroup->characterTable.size; i ++) {
    if (this->theCharacteR.InnerProduct(this->ownerGroup->characterTable[i]) != 0) {
      global.comments << "contains irrep " << i << "\n";
      this->ClassFunctionMatrix(this->ownerGroup->characterTable[i], splittingOperatorMatrix);
      splittingOperatorMatrix.GetZeroEigenSpaceModifyMe(splittingMatrixKernel);
      intersection(Vb, splittingMatrixKernel, tempVectors);
      Vb = tempVectors;
    }
  }
  if ((Vb.size < this->basis.size) && (Vb.size > 0)) {
    global.comments << "calculating remaining subrep... ";
    GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> V;
    V.ownerGroup = this->ownerGroup;
    V.generatorS = this->generatorS;
    V.basis = Vb;
    V = V.Reduced();
    global.comments << "done\n";
    global.comments << "Decomposing remaining subrep " << V.GetCharacter();
    return V.DecomposeThomasVersion();
  }
  if (Vb.size == 0) {
    return out;
  }
/*  SpaceTree<Rational> st;
  st.space = basis;
  for (int cfi = 0; cfi <G->ccCount; cfi ++) {
     ClassFunction<Coefficient> cf;
      cf.G = G;
      cf.makeZero();
      cf[cfi] = 1;
      global.Comments << "getting matrix " << cf << "\n";
      Matrix<Coefficient> A;
      ClassFunctionMatrix(cf, A);
      List<List<Vector<Coefficient> > > es = eigenspaces(A);
      global.Comments << "eigenspaces were ";
      for (int i = 0; i <es.size; i ++)
         global.Comments << es[i].size << " ";
      global.Comments << "\n";
      for (int i = 0; i <es.size; i ++)
         st.PlaceInTree(es[i]);
      global.Comments << "tree is ";
      st.DisplayTree();
      global.Comments << "\n";
  }
  List<List<Vector<Coefficient> > > leaves;
  st.GetLeaves(leaves);
  global.Comments << "leaves are ";
  for (int i = 0; i <leaves.size; i ++)
    global.Comments << leaves[i].size << " ";
  global.Comments << "\n";
*/
  List<List<Vector<Coefficient> > > es;
  for (int cfi = this->ownerGroup->ConjugacyClassCount() - 1; cfi >= 0; cfi --) {
    ClassFunction<somegroup, Coefficient> cf;
    cf.makeZero(*this->ownerGroup);
    cf[cfi] = 1;
    global.comments << "getting matrix " << cf << "\n";
    Matrix<Coefficient> A;
    ClassFunctionMatrix(cf, A);
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
    outeme.generatorS = this->generatorS;
    outeme.basis = es[i];
    out.addOnTop(outeme.Reduced());
  }
  return out;
}

int ithfactor(int i, List<int> f) {
  int acc = 1;
  for (int j = 0; j < f.size; j ++) {
    if (i & i << j) {
      acc = acc * f[j];
    }
  }
  return acc;
}

template <typename Coefficient>
struct DivisionResult<UDPolynomial<Coefficient> > UDPolynomial<Coefficient>::DivideBy(const UDPolynomial<Coefficient>& divisor) const {
  struct DivisionResult<UDPolynomial<Coefficient> > out;
  out.remainder = *this;
  if (data.size < divisor.data.size) {
    return out;
  }
  int r = data.size - divisor.data.size + 1;
  out.quotient.data.setSize(r);
  for (int i = r- 1; i != - 1; i --) {
    if (out.remainder.data.size - divisor.data.size != i) {
      out.quotient[i] = 0;
      continue;
    }
    UDPolynomial<Coefficient> p = divisor.TimesXn(i);
    out.quotient[i] = out.remainder.data[out.remainder.data.size - 1] / divisor.data[divisor.data.size - 1];
    p *= out.quotient[i];
    out.remainder -= p;
  }
  return out;
}

template <typename Coefficient>
UDPolynomial<Coefficient> UDPolynomial<Coefficient>::operator/(const UDPolynomial<Coefficient>& divisor) const {
  struct DivisionResult<UDPolynomial<Coefficient> > tmp = this->DivideBy(divisor);
  return tmp.quotient;
}

template <typename Coefficient>
UDPolynomial<Coefficient> UDPolynomial<Coefficient>::operator%(const UDPolynomial<Coefficient>& divisor) const {
  struct DivisionResult<UDPolynomial<Coefficient> > tmp = this->DivideBy(divisor);
  return tmp.remainder;
}

template <typename Coefficient>
bool UDPolynomial<Coefficient>::operator==(const int other) const {
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
void UDPolynomial<Coefficient>::clearDenominators() {
  int acc = 1;
  for (int i = 0; i < data.size; i ++) {
    acc = lcm(acc, data[i].getDenominator());
  }
  *this *= acc;
}

template <typename Coefficient>
void UDPolynomial<Coefficient>::FormalDerivative() {
  if (data.size < 2) {
    data.size = 0;
  }
  for (int i = 1; i < data.size; i ++) {
    data[i - 1] = i * data[i];
  }
  data.size --;
}

template <typename Coefficient>
bool UDPolynomial<Coefficient>::operator<(const UDPolynomial<Coefficient>& right) const {
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
void UDPolynomial<Coefficient>::SquareFree() {
  global.comments << *this << "\n";
  UDPolynomial<Coefficient> p = FormalDerivative();
  global.comments << p << "\n";
  UDPolynomial<Coefficient> q = gcd(*this, p);
  if (q.data.size > 1) {
    data = (*this / q).data;
  }
}

template <typename Coefficient>
bool space_contains(const List<Vector<Coefficient> >& V, Vector<Coefficient> v) {
  if (v.isEqualToZero()) {
    if (V.size == 0) {
      return false;
    }
    return true;
  }
  int i = 0;
  while (true) {
    int vi = v.getIndexFirstNonZeroCoordinate();
    while (true) {
      if (i == V.size) {
        return false;
      }
      int vii = V[i].getIndexFirstNonZeroCoordinate();
      if (vii > vi) {
        return false;
      }
      if (vii == vi) {
        break;
      }
      i ++;
    }
    v -= V[i] * v[vi] / V[i][vi];
    if (v.isEqualToZero()) {
      return true;
    }
  }
}

template<typename Coefficient>
void getunion(const List<Vector<Coefficient> >& V, const List<Vector<Coefficient> >& W, List<Vector<Coefficient> >& output) {
  if (&output == &V || &output == &W) {
    List<Vector<Coefficient> > newOutput;
    getunion(V, W, newOutput);
    output = newOutput;
    return;
  }
  if (V.size == 0) {
    output = W;
    return;
  }
  int d = V[0].size;
  Matrix<Coefficient> M;
  M.initialize(V.size + W.size,d);
  for (int i = 0; i < V.size; i ++) {
    for (int j = 0; j < d; j ++) {
      M.elements[i][j] = V[i][j];
    }
  }
  for (int i = 0; i < W.size; i ++) {
    for (int j = 0; j < d; j ++) {
      M.elements[V.size + i][j] = W[i][j];
    }
  }
  M.gaussianEliminationByRows(M);
  output.setSize(0);
  Vector<Coefficient> v;
  for (int i = 0; i < M.numberOfRows; i ++) {
    v.setSize(d);
    for (int j = 0; j < d; j ++) {
      v[j] = M.elements[i][j];
    }
    if (v.isEqualToZero()) {
      break;
    }
    output.addOnTop(v);
  }
}

template<typename Coefficient>
void intersection(
  const List<Vector<Coefficient> > &V,
  const List<Vector<Coefficient> > &W,
  List<Vector<Coefficient> >& output
) {
  if ((V.size == 0) or (W.size == 0)) {
    output.setSize(0);
    return;
  }
  int d = V[0].size;

  Matrix<Coefficient> MV;
  MV.initialize(V.size, d);
  for (int i = 0; i < V.size; i ++) {
    for (int j = 0; j < d; j ++) {
      MV.elements[i][j] = V[i][j];
    }
  }
  List<Vector<Coefficient> > Vperp;
  MV.GetZeroEigenSpaceModifyMe(Vperp);

  Matrix<Coefficient> MW;
  MW.initialize(W.size, d);
  for (int i = 0; i < W.size; i ++) {
    for (int j = 0; j < d; j ++) {
      MW.elements[i][j] = W[i][j];
    }
  }
  List<Vector<Coefficient> > Wperp;
  MW.GetZeroEigenSpaceModifyMe(Wperp);

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
  M.GetZeroEigenSpaceModifyMe(output);
}

template <typename Coefficient>
List<Vector<Coefficient> > orthogonal_complement(
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
      GM.elements[i][j] = W[i].ScalarEuclidean(W[j]);
    }
  }
  GM.invert();
  Matrix<Coefficient> VM;
  VM.initialize(W.size,V.size);
  for (int i = 0; i < W.size; i ++) {
    for (int j = 0; j < V.size; j ++) {
      VM.elements[i][j] = W[i].ScalarEuclidean(V[j]);
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

// we're going to guess at integers
template <typename Coefficient>
bool pdiv(List<Coefficient> &p, int a) {
  List<Coefficient> q;
  Coefficient lastround = p[0];
  for (int i = 1; i < p.size; i ++) {
    q.addOnTop(lastround);
    lastround = p[i] - lastround * a;
  }
  if (lastround == 0) {
    p = q;
    return true;
  }
  return false;
}

template <typename Coefficient>
List<int> factorpoly(List<Coefficient> p, int maxfac) {
  List<int> factors;
  for (int i1 = 0; i1 < maxfac; i1 ++) {
    for (int i2 = 1; i2 >= - 1; i2 -= 2) {
      int i = i1 * i2;
      while (pdiv(p, i)) {
        if (!factors.contains(i)) {
          factors.addOnTop(i);
        }
        if (p.size == 1) {
          return factors;
        }
      }
    }
  }
  return factors;
}

template <typename Coefficient>
List<Vector<Coefficient> > DestructiveColumnSpace(Matrix<Coefficient>& M) {
  M.transpose();
  Matrix<Coefficient> dummy1;
  Selection dummy2;
  M.gaussianEliminationByRows(M, dummy1, dummy2);
  List<Vector<Coefficient> > out;
  bool zerov;
  for (int i = 0; i < M.numberOfRows; i ++) {
    Vector<Coefficient> v;
    v.makeZero(M.numberOfColumns); // initializing is not necessary
    zerov = true;
    for (int j = 0; j < M.numberOfColumns; j ++) {
      v[j] = M.elements[i][j];
      if (zerov && M.elements[i][j] != 0) {
        zerov = false;
      }
    }
    if (zerov) {
      return out;
    }
    out.addOnTop(v);
  }
}

// guess at integers
List<List<Vector<Rational> > > eigenspaces(const Matrix<Rational>& M, int checkDivisorsOf = 0) {
  (void) checkDivisorsOf;
  int n = M.numberOfColumns;
  List<List<Vector<Rational> > > spaces;
  int found = 0;
// for (int i = 0; found < n; i ++){
//   if ((i != 0) && (checkDivisorsOf%i != 0))
//     continue;
  UDPolynomial<Rational> p;
  p.assignMinPoly(M);
  for (int ii = 0; ii < 2 * n + 2; ii ++) {
    // lol, this did end up working though
    int i = ((ii + 1) / 2) * (2 * (ii % 2) - 1); // 0,1,- 1,2,-2,3,-3,...
    Rational r = i;
    if (p(r) == 0) {
      Matrix<Rational> M2 = M;
      List<Vector<Rational> > V;
      M2.GetEigenspaceModifyMe(r, V);
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
Vector<Coefficient> PutInBasis(const Vector<Coefficient>& v, const List<Vector<Coefficient> >& B) {
  Vector<Coefficient> w;
  w.makeZero(B.size);
  Matrix<Coefficient> M;
  M.MakeZeroMatrix(B.size);
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
ElementWeylGroupRing<Coefficient> ActOnGroupRing(
  ElementWeylGroup& theElement, const ElementWeylGroupRing<Coefficient>& v
) {
  ElementWeylGroupRing<Coefficient> out;
  out.makeZero();
  for (int i = 0; i < v.size(); i ++) {
    out.addMonomial(v.coefficients[i], theElement * v[i]);
  }
  return out;
}

// this function name is a lie
template <typename Coefficient>
bool is_isotypic_component(WeylGroupData& G, const List<Vector<Coefficient> >& V) {
  // pre-initial test: V isn't empty
  if (V.size == 0) {
    return false;
  }
  // initial test: dimension of V is a square
  int n = sqrt(V.size);
  if (n * n != V.size) {
    return false;
  }
  // more expensive test: character of V has unit Norm
  ClassFunction<FiniteGroup<ElementWeylGroup>, Coefficient> X;
  X.G = &G.theGroup;
  X.data.setSize(G.theGroup.ConjugacyClassCount());
  for (int i = 0; i < G.theGroup.ConjugacyClassCount(); i ++) {
    ElementWeylGroup& g = G.theGroup.conjugacyClasses[i].representative;
    Coefficient tr = 0;
    for (int j = 0; j < V.size; j ++) {
      Vector<Coefficient> v = ActOnGroupRing(g, V[j]);
      v = PutInBasis(v, V);
      tr += v[j];
    }
    X.data[i] = tr;
  }
  // (4, - 1, - 1/2, 1/2, - 1, 1/4, 1/4, 1, 1/2, -7/2)[1] haha yeah right
  if (X.Norm() != n) {
    return false;
  }
  // okay now do the really hard test
  Matrix<Coefficient> M = GetMatrix(X);
  List<List<Vector<Coefficient> > > spaces = eigenspaces(M);
  if (spaces.size != 2) {
    return false;
  }
  if (spaces[1].size != V.size) {
    return false;
  }
  if (intersection(spaces[1], V).size != V.size) {
    return false;
  }
  return true;
}

Matrix<Rational> matrixInBasis(
  const ClassFunction<FiniteGroup<ElementWeylGroup>, Rational>& X, const List<Vector<Rational> >& B
) {
  List<Vector<Rational> > rows;
  for (int i = 0; i < B.size; i ++) {
    Vector<Rational> v;
    v.makeZero(B[0].size);
    rows.addOnTop(v);
  }
  for (int i1 = 0; i1 < X.G->ConjugacyClassCount(); i1 ++) {
    for (int i2 = 0; i2 < X.G->conjugacyClasses[i1].size; i2 ++) {
      for (int j = 0; j < X.G->getSize(); j ++) {
        for (int k = 0; k < B.size; k ++) {
          int l = X.G->theElements.getIndex(
            X.G->conjugacyClasses[i1].theElements[i2] * X.G->theElements[j]
          );
          rows[k][l] = X.data[i1] * B[k][j];
        }
      }
    }
  }
  Matrix<Rational> M;
  M.initialize(rows.size, rows.size);
  for (int i = 0; i < rows.size; i ++) {
    Vector<Rational> v = PutInBasis(rows[i], B);
    for (int j = 0; j < rows.size; j ++) {
      M.elements[i][j] = v[j];
    }
  }
  return M;
}

ElementMonomialAlgebra<ElementWeylGroup, Rational> FromClassFunction(
  const ClassFunction<FiniteGroup<ElementWeylGroup>, Rational>& X
) {
  ElementMonomialAlgebra<ElementWeylGroup, Rational> out;
  for (int i = 0; i < X.G->ConjugacyClassCount(); i ++) {
    for (int j = 0; j < X.G->conjugacyClasses[i].size; j ++) {
      out.addMonomial(X.G->conjugacyClasses[i].theElements[j], X.data[i]);
    }
  }
  return out;
}

template <typename Coefficient>
Matrix<Coefficient> GetMatrix(const ClassFunction<WeylGroupData::WeylGroupBase, Coefficient>& X) {
  Matrix<Coefficient> M;
  M.MakeZeroMatrix(X.G->N);
  for (int i1 = 0; i1 < X.G->ccCount; i1 ++) {
    for (int i2 = 0; i2 < X.G->ccSizes[i1]; i2 ++) {
      int i = X.G->conjugacyClasses[i1][i2];
      for (int j = 0; j < X.G->N; j ++) {
        M(X.G->MultiplyElements(i, j), j) = X.data[i1];
      }
    }
  }
  return M;
}

void SubgroupDataRootReflections::ComputeDynkinType() {
  WeylGroupData tempGroup;
  tempGroup.cartanSymmetric = this->SubCartanSymmetric;
  tempGroup.MakeMeFromMyCartanSymmetric();
  this->theDynkinType = tempGroup.theDynkinType;
}

SubgroupDataWeylGroup::SubgroupDataWeylGroup() {
  this->theWeylData = nullptr;
}

SubgroupDataRootReflections::SubgroupDataRootReflections() {
  this->flagIsParabolic = false;
  this->flagIsExtendedParabolic = false;
}

bool SubgroupDataWeylGroup::checkInitialization() {
  if (this->theWeylData == nullptr) {
    global.fatal << "SubgroupDataWeylGroup: non-initialized theWeylData pointer. " << global.fatal;
  }
  return true;
}

template <class someGroup, class elementSomeGroup>
std::string SubgroupData<someGroup, elementSomeGroup>::toString(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("SubgroupData::toString");
  (void) theFormat;
  if (this->theGroup == nullptr) {
    return "(not initialized (no owner group))";
  }
  if (this->theSubgroup == nullptr) {
    return "(not initialized (subgroup pointer is 0))";
  }
  std::stringstream out;
  out << "<br>\nSubgroup: " << this->theSubgroup->toString();

  out << "<br>\nOwner group: " << this->theGroup->toString();

  return out.str();
}

std::string SubgroupDataRootReflections::toString(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("SubgroupDataRootReflections::toString");
  std::stringstream out;
  out << subGroupDataContainer.toString(theFormat);
  return out.str();
}

std::string SubgroupDataWeylGroup::toString(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("SubgroupDataWeylGroup::toString");
  std::stringstream out;
  out << this->theSubgroupData.toString(theFormat);
  return out.str();
}

void SubgroupDataWeylGroup::ComputeTauSignature() {
  MacroRegisterFunctionWithName("SubgroupWeylGroup::ComputeTauSignature");
  this->checkInitialization();
  if (!this->theSubgroupData.theGroup->flagCCRepresentativesComputed) {
    this->theSubgroupData.theGroup->computeConjugacyClassSizesAndRepresentatives();
  }
  this->theSubgroupData.theGroup->CheckConjugacyClassRepsMatchCCsizes();
  this->theSubgroupData.theGroup->CheckOrthogonalityCharTable();
  Vector<Rational> Xs;
  this->theSubgroupData.theSubgroup->GetSignCharacter(Xs);
  this->theSubgroupData.ComputeCCRepresentativesPreimages();
  //if (global.printOutThisKindaThing)
  //{
  //global.Comments << "here is the character table of the group, the representatives and sizes for "
  //         << "conjugacy classes of the group and the subgroup, and for your convenience, "
  //         << "a mapping from subgroup conjugacy classes to their preimages in the group. ";
  //global.Comments << this->theSubgroupData.theGroup->PrettyPrintCharacterTable();
  //global.Comments << Xs << " <- subgroup sign character\n";
  //global.Comments << this->theSubgroupData.theGroup->PrettyPrintCCRepsSizes();
  //global.Comments << this->theSubgroupData.theSubgroup->PrettyPrintCCRepsSizes();
  //global.Comments << this->theSubgroupData.ccRepresentativesPreimages.toStringCommaDelimited() << '\n';
 //}

  this->tauSignature.setSize(this->theSubgroupData.theGroup->ConjugacyClassCount());

  Vector<Rational> XiS;
  XiS.makeZero(this->theSubgroupData.theSubgroup->conjugacyClasses.size);
  for (int i = 0; i < this->theSubgroupData.theGroup->conjugacyClasses.size; i ++) {
    ClassFunction<FiniteGroup<ElementWeylGroup>, Rational>& XiG =
    this->theWeylData->theGroup.characterTable[i];
    //global.Comments << "Restricting character: " << Xip.toString() << "<br>";
    for (int j = 0; j < this->theSubgroupData.theSubgroup->conjugacyClasses.size; j ++) {
      XiS[j] = XiG[this->theSubgroupData.ccRepresentativesPreimages[j]];
    }
    this->tauSignature[i] = this->theSubgroupData.theSubgroup->GetHermitianProduct(Xs, XiS);
    //global.Comments << "<br>Hermitian product of " << Xs.toString() << " and "
    //<< XiS.toString() << " = " << this->tauSignature[i];
    if (!this->tauSignature[i].isSmallInteger()) {
      global.fatal << " Tau signature is not integral, something is wrong. " << global.fatal ;
    }
  }
}

void SubgroupDataRootReflections::ComputeCCSizesRepresentativesPreimages() {
  MacroRegisterFunctionWithName("SubgroupRootReflections::ComputeCCSizesRepresentativesPreimages");
  if (this->theDynkinType == this->theWeylData->theDynkinType && this->theWeylData->theGroup.flagCCRepresentativesComputed) {
    this->theSubgroupData.theSubgroup->conjugacyClasses.setSize(this->theSubgroupData.theGroup->conjugacyClasses.size);
    for (int i = 0; i < this->theSubgroupData.theSubgroup->conjugacyClasses.size; i ++) {
      this->theSubgroupData.theSubgroup->conjugacyClasses[i].flagRepresentativeComputed = true;
      this->theSubgroupData.theSubgroup->conjugacyClasses[i].representative = this->theSubgroupData.theGroup->conjugacyClasses[i].representative;
      this->theSubgroupData.theSubgroup->conjugacyClasses[i].size = this->theSubgroupData.theGroup->conjugacyClasses[i].size;
      this->theSubgroupData.theSubgroup->conjugacyClasses[i].flagElementsComputed = false;
    }
    this->theSubgroupData.ccRepresentativesPreimages.setSize(this->theSubgroupData.theGroup->conjugacyClasses.size);
    for (int i = 0; i < this->theSubgroupData.ccRepresentativesPreimages.size; i ++) {
      this->theSubgroupData.ccRepresentativesPreimages[i] = i;
    }
    this->theSubgroupData.theSubgroup->flagCCRepresentativesComputed = true;
  } else {
    if (this->theDynkinType.GetRank() <= 6) {
      this->theSubgroupData.theSubgroup->computeConjugacyClassesFromAllElements();
    } else {
      this->theSubgroupData.theSubgroup->computeConjugacyClassSizesAndRepresentatives();
    }

    this->theSubgroupData.ComputeCCRepresentativesPreimages();
  }
}

void SubgroupDataRootReflections::initializeGenerators() {
  MacroRegisterFunctionWithName("SubgroupRootReflections::initializeGenerators");
  if (this->theDynkinType.GetRank() == 0) {
    this->theSubgroupData.theSubgroup->generators.setSize(1);
    this->theSubgroupData.theSubgroup->generators[0].makeIdentity(*this->theSubgroupData.theGroup);
    return;
  }
  int d = this->SubCartanSymmetric.numberOfRows;
  this->theSubgroupData.generatorPreimages.setSize(d);
  this->theSubgroupData.theSubgroup->generators.setSize(d);
  ElementWeylGroup currentReflection;
  for (int i = 0; i < d; i ++) {
    currentReflection.MakeRootReflection(this->generatingSimpleRoots[i], *this->theWeylData);
    this->theSubgroupData.generatorPreimages[i] = this->theSubgroupData.theGroup->theElements.getIndex(currentReflection);
    this->theSubgroupData.theSubgroup->generators[i] = currentReflection;
  }
}

void SubgroupDataRootReflections::MakeParabolicSubgroup(WeylGroupData& G, const Selection& inputGeneratingSimpleRoots) {
  MacroRegisterFunctionWithName("SubgroupDataRootReflections::MakeParabolicSubgroup");
  G.checkConsistency();
  this->theWeylData = &G;
  this->theSubgroupData.MakeSubgroupOf(G.theGroup);
  this->checkInitialization();
  this->flagIsParabolic = true;
  this->simpleRootsInLeviParabolic = inputGeneratingSimpleRoots;
  Vectors<Rational> EiBasis;
  EiBasis.MakeEiBasis(G.getDimension());
  EiBasis.SubSelection(inputGeneratingSimpleRoots, this->generatingSimpleRoots);
  int d = inputGeneratingSimpleRoots.CardinalitySelection;
  this->SubCartanSymmetric.initialize(d, d);
  for (int ii = 0; ii < d; ii ++) {
    for (int jj = 0; jj < d; jj ++) {
      this->SubCartanSymmetric(ii, jj) = G.RootScalarCartanRoot(
        this->generatingSimpleRoots[ii], this->generatingSimpleRoots[jj]
      );
    }
  }
  this->ComputeDynkinType();
  this->initializeGenerators();
}

void SubgroupDataRootReflections::MakeFromRoots(WeylGroupData& G, const Vectors<Rational>& inputRootReflections) {
  MacroRegisterFunctionWithName("SubgroupDataRootReflections::MakeFromRoots");
  this->theSubgroupData.MakeSubgroupOf(G.theGroup);
  this->theWeylData = &G;
  this->generatingSimpleRoots = inputRootReflections;
  DynkinDiagramRootSubalgebra theDiagram;
  theDiagram.ComputeDiagramTypeModifyInput(this->generatingSimpleRoots, G);
  int d = this->generatingSimpleRoots.size;
  this->SubCartanSymmetric.initialize(d, d);
  for (int ii = 0; ii < d; ii ++) {
    for (int jj = 0; jj < d; jj ++) {
      this->SubCartanSymmetric(ii, jj) = G.RootScalarCartanRoot(
        this->generatingSimpleRoots[ii], generatingSimpleRoots[jj]
      );
    }
  }
  this->ComputeDynkinType();
  DynkinType verificationType;
  theDiagram.GetDynkinType(verificationType);
  if (this->theDynkinType != verificationType) {
    global.fatal << "Two different comptuations of the Dynkin type a set of roots did not coincide. " << global.fatal;
  }
  this->initializeGenerators();
}

template <class elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::AreConjugate_OLD_Deprecated_Version_By_Todor(
  const elementSomeGroup& left, const elementSomeGroup& right
) {
  MacroRegisterFunctionWithName("WeylGroup::AreConjugate_OLD_Deprecated_Version_By_Todor");
  if (left.HasDifferentConjugacyInvariantsFrom(right)) {
    return false;
  }
  OrbitIteratorWeylGroup theIterator;
  theIterator.initialize(this->generators, left, ElementWeylGroup::conjugationAction);
  if (this->generators.size == 0) {
    global.fatal << "Generators not allowed to be 0. " << global.fatal;
  }
  do {
    //if (left.toString() == "s_{4}")
    //  global.Comments << "<br>" << theIterator.GetCurrentElement().toString() << "=?=" << right.toString();
    if (theIterator.GetCurrentElement() == right) {
      return true;
    }
  } while (theIterator.IncrementReturnFalseIfPastLast());
  return false;
}

void WeylGroupData::GetSignSignatureAllRootSubsystems(List<SubgroupDataRootReflections>& outputSubgroups) {
  MacroRegisterFunctionWithName("WeylGroup::GetSignSignatureAllRootSubsystems");
  RootSubalgebras theRootSAs;
  SemisimpleLieAlgebra theSSlieAlg;
  theSSlieAlg.theWeyl.MakeFromDynkinType(this->theDynkinType);
//  theSSlieAlg.ComputeChevalleyConstants(global);
  theRootSAs.owner = &theSSlieAlg;
  theRootSAs.ComputeAllReductiveRootSubalgebrasUpToIsomorphism();
  List<Vectors<Rational> > theRootSAsBases;
  theRootSAsBases.setExpectedSize(theRootSAs.theSubalgebras.size);
  for (int i = theRootSAs.theSubalgebras.size - 1; i >= 0; i --) {
    theRootSAsBases.addOnTop(theRootSAs.theSubalgebras[i].SimpleBasisK);
  }
  this->GetSignSignatureRootSubgroups(outputSubgroups, theRootSAsBases);
}

void WeylGroupData::GetSignSignatureParabolics(List<SubgroupDataRootReflections>& outputSubgroups) {
  MacroRegisterFunctionWithName("WeylGroup::GetSignSignatureParabolics");
//  this->ComputeIrreducibleRepresentationsThomasVersion();
  this->ComputeOrLoadCharacterTable();
  ClassFunction<WeylGroupData::WeylGroupBase, Rational> signRep;
  signRep.G = &(this->theGroup);
  this->GetSignCharacter(signRep.data);
  Selection sel;
  sel.initialize(this->getDimension());
  int numCycles = MathRoutines::TwoToTheNth(sel.MaxSize);
  outputSubgroups.setSize(numCycles);
  ElementWeylGroup g;
  g.owner = this;
//  global.Comments << "<hr>Meself is: " << this->toString();
  for (int i = 0; i < outputSubgroups.size; i ++, sel.incrementSelection()) {
    SubgroupDataRootReflections& currentParabolic = outputSubgroups[i];
    currentParabolic.MakeParabolicSubgroup(*this, sel);
    currentParabolic.ComputeCCSizesRepresentativesPreimages();
//    global.Comments << "<hr>Current parabolic is: " << currentParabolic.toString();
    // ComputeInitialCharacters gets the character of the sign representation
    // as characterTable[1]
    //global.Comments << "<hr>before compute initial irreps";
  }
  this->theGroup.CheckConjugacyClassRepsMatchCCsizes();
  for (int j = 0; j < outputSubgroups.size; j ++) {
    outputSubgroups[j].ComputeTauSignature();
  }
}

void WeylGroupData::GetSignSignatureExtendedParabolics(List<SubgroupDataRootReflections>& outputSubgroups) {
  MacroRegisterFunctionWithName("WeylGroup::GetSignSignatureExtendedParabolics");
//  this->ComputeIrreducibleRepresentationsThomasVersion();
  this->ComputeOrLoadCharacterTable();
  ClassFunction<WeylGroupData::WeylGroupBase, Rational> signRep;
  signRep.G = &(this->theGroup);
  this->GetSignCharacter(signRep.data);
  Selection parSelrootsAreInLevi;
  parSelrootsAreInLevi.initialize(this->getDimension() + 1);
  Vectors<Rational> extendedBasis, currentBasisExtendedParabolic;
  extendedBasis.MakeEiBasis(this->getDimension());
  extendedBasis.addOnTop(this->RootSystem[0]);
  outputSubgroups.setExpectedSize(MathRoutines::TwoToTheNth(this->getDimension()));
  outputSubgroups.setSize(0);
  SubgroupDataRootReflections theSG;
  do {
    extendedBasis.SubSelection(parSelrootsAreInLevi, currentBasisExtendedParabolic);
    if (currentBasisExtendedParabolic.GetRankOfSpanOfElements() == currentBasisExtendedParabolic.size) {
      theSG.MakeFromRoots(*this, currentBasisExtendedParabolic);
      theSG.flagIsExtendedParabolic = true;
      theSG.simpleRootsInLeviParabolic = parSelrootsAreInLevi;
      outputSubgroups.addOnTop(theSG);
    }
  } while (parSelrootsAreInLevi.IncrementReturnFalseIfPastLast());
  for (int i = 0; i < outputSubgroups.size; i ++) {
    outputSubgroups[i].ComputeCCSizesRepresentativesPreimages();
  }
  this->theGroup.CheckConjugacyClassRepsMatchCCsizes();
  for (int j = 0; j < outputSubgroups.size; j ++) {
    outputSubgroups[j].ComputeTauSignature();
  }
}

void WeylGroupData::GetSignSignatureRootSubgroups(
  List<SubgroupDataRootReflections>& outputSubgroups, const List<Vectors<Rational> >& rootsGeneratingReflections
) {
  MacroRegisterFunctionWithName("WeylGroup::GetSignSignatureRootSubgroups");
  this->ComputeOrLoadCharacterTable();
  ClassFunction<WeylGroupData::WeylGroupBase, Rational> signRep;
  signRep.G = &(this->theGroup);
  this->GetSignCharacter(signRep.data);
  outputSubgroups.setSize(rootsGeneratingReflections.size);
  ElementWeylGroup g;
  g.owner = this;
  for (int i = 0; i < outputSubgroups.size; i ++) {
    SubgroupDataRootReflections& currentParabolic = outputSubgroups[i];
    currentParabolic.MakeFromRoots(*this, rootsGeneratingReflections[i]);
    currentParabolic.ComputeCCSizesRepresentativesPreimages();
  }
  this->theGroup.CheckConjugacyClassRepsMatchCCsizes();
  this->theGroup.CheckOrthogonalityCharTable();
  for (int j = 0; j < outputSubgroups.size; j ++) {
    outputSubgroups[j].ComputeTauSignature();
  }
}
