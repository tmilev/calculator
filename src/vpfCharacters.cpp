//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader2Math0_General.h"
#include "vpfHeader2Math1_2SemisimpleLieAlgebras_RootSubalgebras.h" // WeylGroup::GetSignSignatureAllRootSubsystems
#include "vpfImplementationHeader2Math3_FiniteGroups.h"
#include "vpfHeader2Math3_SymmetricGroupsAndGeneralizations.h"
#include "vpfHeader2Math4_Graph.h"
#include "vpfImplementationHeader2Math0_General.h"
static ProjectInformationInstance ProjectInfoVpfCharacters(__FILE__, "Finite group characters sandbox. Work in progress by Thomas. ");

extern FormatExpressions consoleFormat;

template<>
List<ElementWeylGroup<WeylGroupData> >::OrderLeftGreaterThanRight FormatExpressions::GetMonOrder<ElementWeylGroup<WeylGroupData> >() {
  return 0;
}

Vector<Rational> WeylGroupData::ApplyReflectionList(const List<int>& rightReflectionsActFirst, const Vector<Rational>& vv) const {
  Vector<Rational> v = vv;
  for (int i = rightReflectionsActFirst.size - 1; i >= 0; i --) {
    Rational x = 0;
    for (int j = 0; j < this->GetDim(); j ++) {
      x += v[j] * CartanSymmetric(rightReflectionsActFirst[i], j);
    }
    //CartanSymmetric(i, j) is slower than CartanSymmetric.elements[i][j] but includes index checking
    //(we want to catch a rogue call to ApplyReflectionList
    v[rightReflectionsActFirst[i]] -= x * 2 / CartanSymmetric(rightReflectionsActFirst[i], rightReflectionsActFirst[i]);
  }
  return v;
}

void WeylGroupData::GetSimpleReflectionMatrix(int indexSimpleRoot, Matrix<Rational>& output) const {
  MacroRegisterFunctionWithName("WeylGroup::GetSimpleReflectionMatrix");
  int rank = this->GetDim();
  output.MakeIdMatrix(rank);
  for (int j = 0; j < rank; j ++) {
    output(indexSimpleRoot, j) -= (this->CartanSymmetric(indexSimpleRoot, j) / CartanSymmetric(indexSimpleRoot, indexSimpleRoot)) * 2;
  }
}

Matrix<Rational> WeylGroupData::SimpleReflectionMatrix(int i) const {
  Matrix<Rational> result;
  this->GetSimpleReflectionMatrix(i, result);
  return result;
}

ElementWeylGroup<WeylGroupData> WeylGroupData::SimpleConjugation(int i, const ElementWeylGroup<WeylGroupData>& vv) {
  (void) vv;//avoid unused parameter warning, portable.
  ElementWeylGroup<WeylGroupData> eltSimpleReflection;
  eltSimpleReflection.MakeSimpleReflection(i, *this);
  return eltSimpleReflection*this->theGroup.theElements[i] * eltSimpleReflection;
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeSquaresCCReps() {
  MacroRegisterFunctionWithName("WeylGroup::ComputeSquares");
  if (!this->flagCCsComputed) {
    this->ComputeCCfromAllElements();
  }
  this->squaresCCReps.SetExpectedSize(this->ConjugacyClassCount());
  this->squaresCCReps.SetSize(this->ConjugacyClassCount());
  //elementSomeGroup currentSquare;
  for (int i = 0; i < this->ConjugacyClassCount(); i ++) {
    this->squaresCCReps[i] = this->conjugacyClasseS[i].representative * this->conjugacyClasseS[i].representative;
  }
}

void WeylGroupData::ComputeInitialIrreps() {
  MacroRegisterFunctionWithName("WeylGroup::ComputeInitialIrreps");
  if (!this->theGroup.flagCCsComputed) {
    this->theGroup.ComputeCCfromAllElements();
  }
  if (this->theGroup.squaresCCReps.size == 0) {
    this->theGroup.ComputeSquaresCCReps();
  }
  this->theGroup.irreps.SetSize(0);
  this->theGroup.characterTable.SetSize(0);
  this->theGroup.irreps_grcam.SetSize(0);
  this->theGroup.characterTable.SetExpectedSize(this->theGroup.ConjugacyClassCount());
  this->theGroup.irreps.SetExpectedSize(this->theGroup.ConjugacyClassCount());
  this->theGroup.irreps_grcam.SetExpectedSize(this->theGroup.ConjugacyClassCount());
  GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> trivialRep, signRep, standardRep;
  this->GetTrivialRepresentation(trivialRep);
  this->GetSignRepresentation(signRep);
  this->GetStandardRepresentation(standardRep);
  this->theGroup.AddIrreducibleRepresentation(trivialRep);
  this->theGroup.AddIrreducibleRepresentation(signRep);
  this->theGroup.AddIrreducibleRepresentation(standardRep);
}

// This is dumb, but i couldnt figure out what else to do
template<> // haha wat
unsigned int VectorSpace<Rational>::HashFunction(const VectorSpace<Rational>& in) {
  return in.fastbasis.HashFunction();
}
// whenever i figure out cxx i can get rid of this.  but im probably jst
// going to define this for every other field


template <typename coefficient>
void MatrixInBasis(
  Matrix<coefficient>& out,
  const Matrix<coefficient>& in,
  const List<Vector<coefficient> >& basis,
  const Matrix<coefficient>& gramMatrix
) {
  int d = basis.size;
  out.init(d, d);
  for (int i = 0; i < d; i ++) {
    for (int j = 0; j < d; j ++) {
      out.elements[i][j] = basis[i].ScalarEuclidean(in * basis[j]);
    }
  }
  out.MultiplyOnTheLeft(gramMatrix);
}

template <typename coefficient>
void MatrixInBasisFast(Matrix<coefficient>& out, const Matrix<coefficient>& in, const Matrix<coefficient>& BM) {
  int d = BM.NumRows;
  Matrix<coefficient> M = BM;
  Matrix<coefficient> inT = in;
  inT.Transpose();
  M.MultiplyOnTheRight(inT);
  out.MakeZeroMatrix(d);
  for (int i = 0; i < d; i ++) {
    int jj = 0;
    for (int j = 0; j < d; j ++) {
      while ((jj < M.NumCols) and (M.elements[i][jj] == 0)) {
        jj ++;
      }
      if (jj == M.NumCols) {
        out.elements[i][j] = 0;
        continue;
      }
      if (BM.elements[j][jj] == 0) {
        out.elements[i][j] = 0;
        continue;
      }
      out.elements[i][j] = M.elements[i][jj] / BM.elements[j][jj];
      for (int k = jj; k < M.NumCols; k ++) {
        M.elements[i][k] -= BM.elements[j][k] * out.elements[i][j];
      }
    }
  }
}

template <typename coefficient>
class SpaceTree {
public:
   List<Vector<coefficient> > space;
   List<SpaceTree<coefficient> > subspaces;

   void PlaceInTree(const List<Vector<coefficient> >& V);
   void GetLeaves(List<List<Vector<coefficient> > >& leaves) const;
   void DisplayTree() const;
};

template <typename coefficient>
void SpaceTree<coefficient>::PlaceInTree(const List<Vector<coefficient> > &V) {
  List<Vector<coefficient> > U;
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
  List<Vector<coefficient> > W, tempVspace;
  for (int i = 0; i < subspaces.size; i ++) {
    getunion(W, subspaces[i].space, tempVspace);
    W = tempVspace;
  }
  intersection(W, V, tempVspace);
  if (tempVspace.size != V.size) {
    SpaceTree<coefficient> vst;
    vst.space = V;
    subspaces.AddOnTop(vst);
  }
}

template <typename coefficient>
void SpaceTree<coefficient>::GetLeaves(List<List<Vector<coefficient> > >& leaves) const {
  if (subspaces.size > 0) {
    for (int i = 0; i < subspaces.size; i ++) {
      subspaces[i].GetLeaves(leaves);
    }
  } else {
    leaves.AddOnTop(space);
  }
}

template <typename coefficient>
void SpaceTree<coefficient>::DisplayTree() const {
  stOutput << '[' << space.size << ',';
  for (int i = 0; i < subspaces.size; i ++) {
    subspaces[i].DisplayTree();
  }
  stOutput << ']';
}

template <typename somegroup, typename coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::MultiplyBy(
  const GroupRepresentationCarriesAllMatrices<somegroup, coefficient>& other,
  GroupRepresentationCarriesAllMatrices<somegroup, coefficient>& output
) const {
  //lazy programmers handling://////
  if (&output == this || &output == &other) {
    GroupRepresentationCarriesAllMatrices<somegroup, coefficient> thisCopy, otherCopy;
    thisCopy = *this;
    otherCopy = other;
    thisCopy.MultiplyBy(otherCopy, output);
    return;
  }
  //////////////////////////////////
  if (this->ownerGroup != other.ownerGroup) {
    crash << "This is a programming error: attempting to multiply "
    << "representations with different owner groups. " << crash;
  }
  output.reset(this->G);
  int Vd = this->basis[0].size;
  int Wd = other.basis[0].size;
  int Ud = Vd * Wd;
  for (int vi = 0; vi < this->basis.size; vi ++) {
    for (int wi = 0; wi < other.basis.size; wi ++) {
      Vector<coefficient> u;
      u.SetSize(Ud);
      for (int i = 0; i < Vd; i ++) {
        for (int j = 0; j < Wd; j ++) {
          u[i * Wd + j] = this->basis[vi][i] * other.basis[wi][j];
        }
      }
      output.basis.AddOnTop(u);
    }
  }
  output.generators.SetSize(this->generators.size);
  for (int i = 0; i < this->generators.size; i ++) {
    output.generators[i].AssignTensorProduct(this->generators[i],other.generators[i]);
  }
  /* would that it be this simple
  if ((classFunctionMatrices.size > 0) && (other.classFunctionMatrices.size > 0)) {
    U.classFunctionMatrices.SetSize(G->ccCount);
    for (int i = 0; i <classFunctionMatrices.size; i ++) {
      if ((classFunctionMatrices[i].NumCols > 0) && (other.classFunctionMatrices[i].NumCols > 0))
      U.classFunctionMatrices[i].AssignTensorProduct(classFunctionMatrices[i],other.classFunctionMatrices[i]);
    }
  }*/
  output.ownerGroup = this->ownerGroup;
}

template <typename somegroup, typename coefficient>
GroupRepresentationCarriesAllMatrices<somegroup, coefficient>
GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::Reduced() const {
  int d = basis.size;
  Matrix<coefficient> GM;
  GM.init(d, d);
  for (int i = 0; i < d; i ++) {
    for (int j = 0; j < d; j ++) {
      GM.elements[i][j] = this->basis[i].ScalarEuclidean(this->basis[j]);
    }
  }
  GM.Invert();
  GroupRepresentationCarriesAllMatrices<somegroup, coefficient> out;
  out.generatorS.SetSize(this->generatorS.size);
  for (int i = 0; i < this->generatorS.size; i ++) {
  //     MatrixInBasisFast(out.generatorS[i], this->generatorS[i], BM);
    MatrixInBasis(out.generatorS[i], this->generatorS[i], this->basis, GM);
  }
  out.ownerGroup = ownerGroup;
  out.basis.MakeEiBasis(d);

/*
   if (classFunctionMatrices.size > 0) {
     out.classFunctionMatrices.SetSize(G->ccCount);
     for (int i = 0; i <classFunctionMatrices.size; i ++) {
        if (classFunctionMatrices[i].NumRows > 0) {
          //MatrixInBasisFast(out.classFunctionMatrices[i],classFunctionMatrices[i],BM);
          MatrixInBasis(out.classFunctionMatrices[i],classFunctionMatrices[i],basis,GM);
        }
      }
   }
*/
   return out;
}

template <typename somegroup, typename coefficient>
VectorSpace<coefficient> GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::FindDecentBasis() const {
  VectorSpace<coefficient> V;
  int d = this->generators[0].NumCols;
  for (int geni = 0; geni < this->generators.size; geni ++) {
    List<Vector<coefficient> > ess = eigenspaces(this->generators[geni]);
    for (int essi = 0; essi < ess.size; essi ++) {
      for (int esi = 0; esi < ess[essi].size; esi ++) {
        // the best laid coding practices of mice and men oft go astray
        if (!V.AddVectorToBasis(ess[essi][esi])) {
          return V;
        }
      }
    }
  }
  // This should not be possible
  for (int i = 0; i < d; i ++) {
    Vector<coefficient> v;
    v.MakeEi(d, i);
    if (!V.AddVectorToBasis(v)) {
      return V;
    }
  }
  // if it gets here, it deserves to crash
}


template <typename somegroup, typename coefficient>
List<GroupRepresentationCarriesAllMatrices<somegroup, coefficient> >
  GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::DecomposeThomasVersion() {
  MacroRegisterFunctionWithName("WeylGroupRepresentation::DecomposeThomasVersion");
  Matrix<coefficient> splittingOperatorMatrix;
  List<GroupRepresentationCarriesAllMatrices<somegroup, coefficient> > out;
  List<Vector<Rational> > splittingMatrixKernel;
  if (GetNumberOfComponents() == 1) {
    if (this->ownerGroup->characterTable.GetIndex(this->theCharacteR) == - 1) {
      stOutput << "new irrep found, have " << this->ownerGroup->characterTable.size << "\n";
      this->ownerGroup->AddIrreducibleRepresentation(*this);
    }
    out.AddOnTop(*this);
    return out;
  }
  List<Vector<coefficient> > Vb = this->basis;
  List<Vector<coefficient> > tempVectors;
  for (int i = 0; i < this->ownerGroup->characterTable.size; i ++) {
    if (this->theCharacteR.InnerProduct(this->ownerGroup->characterTable[i]) != 0) {
      stOutput << "contains irrep " << i << "\n";
      this->ClassFunctionMatrix(this->ownerGroup->characterTable[i], splittingOperatorMatrix);
      splittingOperatorMatrix.GetZeroEigenSpaceModifyMe(splittingMatrixKernel);
      intersection(Vb, splittingMatrixKernel, tempVectors);
      Vb = tempVectors;
    }
  }
  if ((Vb.size < this->basis.size) && (Vb.size > 0)) {
    stOutput << "calculating remaining subrep... ";
    GroupRepresentationCarriesAllMatrices<somegroup, coefficient> V;
    V.ownerGroup = this->ownerGroup;
    V.generatorS = this->generatorS;
    V.basis = Vb;
    V = V.Reduced();
    stOutput << "done\n";
    stOutput << "Decomposing remaining subrep " << V.GetCharacter();
    return V.DecomposeThomasVersion();
  }
  if (Vb.size == 0) {
    return out;
  }
/*  SpaceTree<Rational> st;
  st.space = basis;
  for (int cfi = 0; cfi <G->ccCount; cfi ++) {
     ClassFunction<coefficient> cf;
      cf.G = G;
      cf.MakeZero();
      cf[cfi] = 1;
      stOutput << "getting matrix " << cf << "\n";
      Matrix<coefficient> A;
      ClassFunctionMatrix(cf, A);
      List<List<Vector<coefficient> > > es = eigenspaces(A);
      stOutput << "eigenspaces were ";
      for (int i = 0; i <es.size; i ++)
         stOutput << es[i].size << " ";
      stOutput << "\n";
      for (int i = 0; i <es.size; i ++)
         st.PlaceInTree(es[i]);
      stOutput << "tree is ";
      st.DisplayTree();
      stOutput << "\n";
  }
  List<List<Vector<coefficient> > > leaves;
  st.GetLeaves(leaves);
  stOutput << "leaves are ";
  for (int i = 0; i <leaves.size; i ++)
    stOutput << leaves[i].size << " ";
  stOutput << "\n";
*/
  List<List<Vector<coefficient> > > es;
  for (int cfi = this->ownerGroup->ConjugacyClassCount() - 1; cfi >= 0; cfi --) {
    ClassFunction<somegroup, coefficient> cf;
    cf.MakeZero(*this->ownerGroup);
    cf[cfi] = 1;
    stOutput << "getting matrix " << cf << "\n";
    Matrix<coefficient> A;
    ClassFunctionMatrix(cf, A);
    es = eigenspaces(A);
    if (es.size > 1) {
      stOutput << "eigenspaces were ";
      for (int i = 0; i < es.size; i ++) {
        stOutput << es[i].size << " ";
      }
      stOutput << "\n";
      break;
    }
  }

  for (int i = 0; i < es.size; i ++) {
    GroupRepresentationCarriesAllMatrices<somegroup, coefficient> outeme;
    outeme.ownerGroup = this->ownerGroup;
    outeme.generatorS = this->generatorS;
    outeme.basis = es[i];
    out.AddOnTop(outeme.Reduced());
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

template <typename coefficient>
struct DivisionResult<UDPolynomial<coefficient> > UDPolynomial<coefficient>::DivideBy(const UDPolynomial<coefficient>& divisor) const {
  struct DivisionResult<UDPolynomial<coefficient> > out;
  out.remainder = *this;
  if (data.size < divisor.data.size) {
    return out;
  }
  int r = data.size - divisor.data.size + 1;
  out.quotient.data.SetSize(r);
  for (int i = r- 1; i != - 1; i --) {
    if (out.remainder.data.size - divisor.data.size != i) {
      out.quotient[i] = 0;
      continue;
    }
    UDPolynomial<coefficient> p = divisor.TimesXn(i);
    out.quotient[i] = out.remainder.data[out.remainder.data.size - 1] / divisor.data[divisor.data.size - 1];
    p *= out.quotient[i];
    out.remainder -= p;
  }
  return out;
}

template <typename coefficient>
UDPolynomial<coefficient> UDPolynomial<coefficient>::operator/(const UDPolynomial<coefficient>& divisor) const {
  struct DivisionResult<UDPolynomial<coefficient> > tmp = this->DivideBy(divisor);
  return tmp.quotient;
}

template <typename coefficient>
UDPolynomial<coefficient> UDPolynomial<coefficient>::operator%(const UDPolynomial<coefficient>& divisor) const {
  struct DivisionResult<UDPolynomial<coefficient> > tmp = this->DivideBy(divisor);
  return tmp.remainder;
}

template <typename coefficient>
bool UDPolynomial<coefficient>::operator==(const int other) const {
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

template <typename coefficient>
//template <typename integral>
void UDPolynomial<coefficient>::ClearDenominators() {
  int acc = 1;
  for (int i = 0; i < data.size; i ++) {
    acc = lcm(acc, data[i].GetDenominator());
  }
  *this *= acc;
}

template <typename coefficient>
void UDPolynomial<coefficient>::FormalDerivative() {
  if (data.size < 2) {
    data.size = 0;
  }
  for (int i = 1; i < data.size; i ++) {
    data[i - 1] = i * data[i];
  }
  data.size --;
}

template <typename coefficient>
bool UDPolynomial<coefficient>::operator<(const UDPolynomial<coefficient>& right) const {
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

template <typename coefficient>
void UDPolynomial<coefficient>::SquareFree() {
  stOutput << *this << "\n";
  UDPolynomial<coefficient> p = FormalDerivative();
  stOutput << p << "\n";
  UDPolynomial<coefficient> q = gcd(*this, p);
  if (q.data.size > 1) {
    data = (*this / q).data;
  }
}

template <typename coefficient>
bool space_contains(const List<Vector<coefficient> >& V, Vector<coefficient> v) {
  if (v.IsEqualToZero()) {
    if (V.size == 0) {
      return false;
    }
    return true;
  }
  int i = 0;
  while (true) {
    int vi = v.GetIndexFirstNonZeroCoordinate();
    while (true) {
      if (i == V.size) {
        return false;
      }
      int vii = V[i].GetIndexFirstNonZeroCoordinate();
      if (vii > vi) {
        return false;
      }
      if (vii == vi) {
        break;
      }
      i ++;
    }
    v -= V[i] * v[vi] / V[i][vi];
    if (v.IsEqualToZero()) {
      return true;
    }
  }
}

template<typename coefficient>
void getunion(const List<Vector<coefficient> >& V, const List<Vector<coefficient> >& W, List<Vector<coefficient> >& output) {
  if (&output == &V || &output == &W) {
    List<Vector<coefficient> > newOutput;
    getunion(V, W, newOutput);
    output = newOutput;
    return;
  }
  if (V.size == 0) {
    output = W;
    return;
  }
  int d = V[0].size;
  Matrix<coefficient> M;
  M.init(V.size + W.size,d);
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
  M.GaussianEliminationByRows(M);
  output.SetSize(0);
  Vector<coefficient> v;
  for (int i = 0; i < M.NumRows; i ++) {
    v.SetSize(d);
    for (int j = 0; j < d; j ++) {
      v[j] = M.elements[i][j];
    }
    if (v.IsEqualToZero()) {
      break;
    }
    output.AddOnTop(v);
  }
}

template<typename coefficient>
void intersection(
  const List<Vector<coefficient> > &V,
  const List<Vector<coefficient> > &W,
  List<Vector<coefficient> >& output
) {
  if ((V.size == 0) or (W.size == 0)) {
    output.SetSize(0);
    return;
  }
  int d = V[0].size;

  Matrix<coefficient> MV;
  MV.init(V.size, d);
  for (int i = 0; i < V.size; i ++) {
    for (int j = 0; j < d; j ++) {
      MV.elements[i][j] = V[i][j];
    }
  }
  List<Vector<coefficient> > Vperp;
  MV.GetZeroEigenSpaceModifyMe(Vperp);

  Matrix<coefficient> MW;
  MW.init(W.size, d);
  for (int i = 0; i < W.size; i ++) {
    for (int j = 0; j < d; j ++) {
      MW.elements[i][j] = W[i][j];
    }
  }
  List<Vector<coefficient> > Wperp;
  MW.GetZeroEigenSpaceModifyMe(Wperp);

  Matrix<coefficient> M;
  M.init(Vperp.size + Wperp.size,d);
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

template <typename coefficient>
List<Vector<coefficient> > orthogonal_complement(
  const List<Vector<coefficient> >& V, const List<Vector<coefficient> >& WW
) {
  List<Vector<coefficient> > W = intersection(V, WW);
  if (W.size == 0) {
    return V;
  }
  int d = W[0].size;
  Matrix<coefficient> GM;
  GM.init(W.size, W.size);
  for (int i = 0; i < W.size; i ++) {
    for (int j = 0; j < W.size; j ++) {
      GM.elements[i][j] = W[i].ScalarEuclidean(W[j]);
    }
  }
  GM.Invert();
  Matrix<coefficient> VM;
  VM.init(W.size,V.size);
  for (int i = 0; i < W.size; i ++) {
    for (int j = 0; j < V.size; j ++) {
      VM.elements[i][j] = W[i].ScalarEuclidean(V[j]);
    }
  }
  VM.MultiplyOnTheLeft(GM);
  Matrix<coefficient> outm;
  outm.init(V.size, d);
  for (int i = 0; i < V.size; i ++) {
    for (int j = 0; j < d; j ++) {
      coefficient r = V[i][j];
      outm.elements[i][j] = r;
      for (int k = 0; k < W.size; k ++) {
        outm.elements[i][j] -= VM.elements[k][i] * W[k][j];
      }
    }
  }
  Matrix<coefficient> tmp;
  Selection s;
  outm.GaussianEliminationByRows(outm,tmp,s);
  List<Vector<coefficient> > out;
  for (int i = 0; i < outm.NumRows; i ++) {
    Vector<coefficient> v;
    v.SetSize(d);
    for (int j = 0; j < d; j ++) {
      v[j] = outm.elements[i][j];
    }
    if (v.IsEqualToZero()) {
      return out;
    }
    out.AddOnTop(v);
  }
  return out;
}

// we're going to guess at integers
template <typename coefficient>
bool pdiv(List<coefficient> &p, int a) {
  List<coefficient> q;
  coefficient lastround = p[0];
  for (int i = 1; i < p.size; i ++) {
    q.AddOnTop(lastround);
    lastround = p[i] - lastround * a;
  }
  if (lastround == 0) {
    p = q;
    return true;
  }
  return false;
}

template <typename coefficient>
List<int> factorpoly(List<coefficient> p, int maxfac) {
  List<int> factors;
  for (int i1 = 0; i1 < maxfac; i1 ++) {
    for (int i2 = 1; i2 >= - 1; i2 -= 2) {
      int i = i1 * i2;
      while (pdiv(p, i)) {
        if (!factors.Contains(i)) {
          factors.AddOnTop(i);
        }
        if (p.size == 1) {
          return factors;
        }
      }
    }
  }
  return factors;
}

template <typename coefficient>
List<Vector<coefficient> > DestructiveColumnSpace(Matrix<coefficient>& M) {
  M.Transpose();
  Matrix<coefficient> dummy1;
  Selection dummy2;
  M.GaussianEliminationByRows(M, dummy1, dummy2);
  List<Vector<coefficient> > out;
  bool zerov;
  for (int i = 0; i < M.NumRows; i ++) {
    Vector<coefficient> v;
    v.MakeZero(M.NumCols); // initializing is not necessary
    zerov = true;
    for (int j = 0; j < M.NumCols; j ++) {
      v[j] = M.elements[i][j];
      if (zerov && M.elements[i][j] != 0) {
        zerov = false;
      }
    }
    if (zerov) {
      return out;
    }
    out.AddOnTop(v);
  }
}

// guess at integers
List<List<Vector<Rational> > > eigenspaces(const Matrix<Rational>& M, int checkDivisorsOf = 0) {
  (void) checkDivisorsOf;//avoid unused parameter warning, portable.
  int n = M.NumCols;
  List<List<Vector<Rational> > > spaces;
  int found = 0;
// for (int i = 0; found < n; i ++){
//   if ((i != 0) && (checkDivisorsOf%i != 0))
//     continue;
  UDPolynomial<Rational> p;
  p.AssignMinPoly(M);
  for (int ii = 0; ii < 2 * n + 2; ii ++) {
    // lol, this did end up working though
    int i = ((ii + 1) / 2) * (2 * (ii % 2) - 1); // 0,1,- 1,2,-2,3,-3,...
    Rational r = i;
    if (p(r) == 0) {
      Matrix<Rational> M2 = M;
      List<Vector<Rational> > V;
      M2.GetEigenspaceModifyMe(r, V);
      found += V.size;
      spaces.AddOnTop(V);
      if (found == M.NumCols) {
        break;
      }
    }
  }
  return spaces;
}

template <typename coefficient>
Vector<coefficient> PutInBasis(const Vector<coefficient>& v, const List<Vector<coefficient> >& B) {
  Vector<coefficient> w;
  w.MakeZero(B.size);
  Matrix<coefficient> M;
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
  M.Invert();
  Vector<coefficient> v2 = M * w;
  Vector<coefficient> v3;
  v3.MakeZero(v.size);
  for (int i = 0; i < B.size; i ++) {
    v3 += B[i] * v2[i];
  }
  if (!(v3 == v)) {
    Vector<coefficient> out;
    return out;
  }
  return v2;
}

template <typename coefficient>
ElementWeylGroupRing<coefficient> ActOnGroupRing(
  ElementWeylGroup<WeylGroupData>& theElement, const ElementWeylGroupRing<coefficient>& v
) {
  ElementWeylGroupRing<coefficient> out;
  out.MakeZero();
  for (int i = 0; i < v.size(); i ++) {
    out.AddMonomial(v.theCoeffs[i], theElement * v[i]);
  }
  return out;
}

// this function name is a lie
template <typename coefficient>
bool is_isotypic_component(WeylGroupData& G, const List<Vector<coefficient> >& V) {
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
  ClassFunction<FiniteGroup<ElementWeylGroup<WeylGroupData> >, coefficient> X;
  X.G = &G.theGroup;
  X.data.SetSize(G.theGroup.ConjugacyClassCount());
  for (int i = 0; i < G.theGroup.ConjugacyClassCount(); i ++) {
    ElementWeylGroup<WeylGroupData>& g = G.theGroup.conjugacyClasseS[i].representative;
    coefficient tr = 0;
    for (int j = 0; j < V.size; j ++) {
      Vector<coefficient> v = ActOnGroupRing(g,V[j]);
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
  Matrix<coefficient> M = GetMatrix(X);
  List<List<Vector<coefficient> > > spaces = eigenspaces(M);
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

Matrix<Rational> MatrixInBasis(
  const ClassFunction<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational>& X, const List<Vector<Rational> >& B
) {
  List<Vector<Rational> > rows;
  for (int i = 0; i < B.size; i ++) {
    Vector<Rational> v;
    v.MakeZero(B[0].size);
    rows.AddOnTop(v);
  }
  for (int i1 = 0; i1 < X.G->ConjugacyClassCount(); i1 ++) {
    for (int i2 = 0; i2 < X.G->conjugacyClasseS[i1].size; i2 ++) {
      for (int j = 0; j < X.G->GetSize(); j ++) {
        for (int k = 0; k < B.size; k ++) {
          int l = X.G->theElements.GetIndex
          (X.G->conjugacyClasseS[i1].theElements[i2] * X.G->theElements[j]);
          rows[k][l] = X.data[i1] * B[k][j];
        }
      }
    }
  }
  Matrix<Rational> M;
  M.init(rows.size, rows.size);
  for (int i = 0; i < rows.size; i ++) {
    Vector<Rational> v = PutInBasis(rows[i], B);
    for (int j = 0; j < rows.size; j ++) {
      M.elements[i][j] = v[j];
    }
  }
  return M;
}

ElementMonomialAlgebra<ElementWeylGroup<WeylGroupData>, Rational> FromClassFunction(
  const ClassFunction<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational>& X
) {
  ElementMonomialAlgebra<ElementWeylGroup<WeylGroupData>, Rational> out;
  for (int i = 0; i < X.G->ConjugacyClassCount(); i ++) {
    for (int j = 0; j < X.G->conjugacyClasseS[i].size; j ++) {
      out.AddMonomial(X.G->conjugacyClasseS[i].theElements[j], X.data[i]);
    }
  }
  return out;
}

template <typename coefficient>
Matrix<coefficient> GetMatrix(const ClassFunction<WeylGroupData::WeylGroupBase, coefficient>& X) {
  Matrix<coefficient> M;
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
  tempGroup.CartanSymmetric = this->SubCartanSymmetric;
  tempGroup.MakeMeFromMyCartanSymmetric();
  this->theDynkinType = tempGroup.theDynkinType;
}

template <class elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::CheckOrthogonalityCharTable() {
  MacroRegisterFunctionWithName("FiniteGroup::CheckOrthogonalityCharTable");
  for (int i = 0; i < this->characterTable.size; i ++) {
    for (int j = i; j < this->characterTable.size; j ++) {
      ClassFunction<FiniteGroup, Rational>& leftChar = this->characterTable[i];
      ClassFunction<FiniteGroup, Rational>& rightChar = this->characterTable[j];
      Rational theScalarProd = this->GetHermitianProduct(leftChar.data, rightChar.data);
      if (j != i) {
        if (theScalarProd != 0) {
          crash << "Error: the character table is not orthonormal: char number " << i + 1 << " = "
          << leftChar.ToString() << " is not orthogonal to char number "
          << j+ 1 << " = " << rightChar.ToString() << ". <br>The entire char table is: "
          << this->PrettyPrintCharacterTable() << crash;
        }
      }
      if (j == i) {
        if (theScalarProd != 1) {
          crash << "Error: the character table is not orthonormal: char number " << i + 1 << " = "
          << leftChar.ToString() << " is not of norm 1. "
          << "<br>The entire char table is: " << this->PrettyPrintCharacterTable() << crash;
        }
      }
    }
  }
  return true;
}

SubgroupDataWeylGroup::SubgroupDataWeylGroup() {
  this->theWeylData = 0;
}

bool SubgroupDataWeylGroup::CheckInitialization() {
  if (this->theWeylData == 0) {
    crash << "SubgroupDataWeylGroup: non-initialized theWeylData pointer. " << crash;
  }
  return true;
}

template <class someGroup, class elementSomeGroup>
std::string SubgroupData<someGroup, elementSomeGroup>::ToString(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("SubgroupData::ToString");
  (void) theFormat;//avoid unused parameter warning, portable.
  if (this->theGroup == 0) {
    return "(not initialized (no owner group))";
  }
  if (this->theSubgroup == 0) {
    return "(not initialized (subgroup pointer is 0))";
  }
  std::stringstream out;
  out << "<br>\nSubgroup: " << this->theSubgroup->ToString();

  out << "<br>\nOwner group: " << this->theGroup->ToString();

  return out.str();
}

std::string SubgroupDataRootReflections::ToString(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("SubgroupDataRootReflections::ToString");
  std::stringstream out;
  out << subGroupDataContainer.ToString(theFormat);
  return out.str();
}

std::string SubgroupDataWeylGroup::ToString(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("SubgroupDataWeylGroup::ToString");
  std::stringstream out;
  out << this->theSubgroupData.ToString(theFormat);
  return out.str();
}

void SubgroupDataWeylGroup::ComputeTauSignature() {
  MacroRegisterFunctionWithName("SubgroupWeylGroup::ComputeTauSignature");
  this->CheckInitialization();
  if (!this->theSubgroupData.theGroup->flagCCRepresentativesComputed) {
    this->theSubgroupData.theGroup->ComputeCCSizesAndRepresentatives();
  }
  this->theSubgroupData.theGroup->CheckConjugacyClassRepsMatchCCsizes();
  this->theSubgroupData.theGroup->CheckOrthogonalityCharTable();
  Vector<Rational> Xs;
  this->theSubgroupData.theSubgroup->GetSignCharacter(Xs);
  this->theSubgroupData.ComputeCCRepresentativesPreimages();
  //if (theGlobalVariables.printOutThisKindaThing)
  //{
  //stOutput << "here is the character table of the group, the representatives and sizes for "
  //         << "conjugacy classes of the group and the subgroup, and for your convenience, "
  //         << "a mapping from subgroup conjugacy classes to their preimages in the group. ";
  //stOutput << this->theSubgroupData.theGroup->PrettyPrintCharacterTable();
  //stOutput << Xs << " <- subgroup sign character\n";
  //stOutput << this->theSubgroupData.theGroup->PrettyPrintCCRepsSizes();
  //stOutput << this->theSubgroupData.theSubgroup->PrettyPrintCCRepsSizes();
  //stOutput << this->theSubgroupData.ccRepresentativesPreimages.ToStringCommaDelimited() << '\n';
 //}

  this->tauSignature.SetSize(this->theSubgroupData.theGroup->ConjugacyClassCount());

  Vector<Rational> XiS;
  XiS.MakeZero(this->theSubgroupData.theSubgroup->conjugacyClasseS.size);
  for (int i = 0; i < this->theSubgroupData.theGroup->conjugacyClasseS.size; i ++) {
    ClassFunction<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational>& XiG =
    this->theWeylData->theGroup.characterTable[i];
    //stOutput << "Restricting character: " << Xip.ToString() << "<br>";
    for (int j = 0; j < this->theSubgroupData.theSubgroup->conjugacyClasseS.size; j ++) {
      XiS[j] = XiG[this->theSubgroupData.ccRepresentativesPreimages[j]];
    }
    this->tauSignature[i] = this->theSubgroupData.theSubgroup->GetHermitianProduct(Xs, XiS);
    //stOutput << "<br>Hermitian product of " << Xs.ToString() << " and "
    //<< XiS.ToString() << " = " << this->tauSignature[i];
    if (!this->tauSignature[i].IsSmallInteger()) {
      crash << " Tau signature is not integral, something is wrong. " << crash ;
    }
  }
}

void SubgroupDataRootReflections::ComputeCCSizesRepresentativesPreimages() {
  MacroRegisterFunctionWithName("SubgroupRootReflections::ComputeCCSizesRepresentativesPreimages");
  if (this->theDynkinType == this->theWeylData->theDynkinType && this->theWeylData->theGroup.flagCCRepresentativesComputed) {
    this->theSubgroupData.theSubgroup->conjugacyClasseS.SetSize(this->theSubgroupData.theGroup->conjugacyClasseS.size);
    for (int i = 0; i < this->theSubgroupData.theSubgroup->conjugacyClasseS.size; i ++) {
      this->theSubgroupData.theSubgroup->conjugacyClasseS[i].flagRepresentativeComputed = true;
      this->theSubgroupData.theSubgroup->conjugacyClasseS[i].representative = this->theSubgroupData.theGroup->conjugacyClasseS[i].representative;
      this->theSubgroupData.theSubgroup->conjugacyClasseS[i].size = this->theSubgroupData.theGroup->conjugacyClasseS[i].size;
      this->theSubgroupData.theSubgroup->conjugacyClasseS[i].flagElementsComputed = false;
    }
    this->theSubgroupData.ccRepresentativesPreimages.SetSize(this->theSubgroupData.theGroup->conjugacyClasseS.size);
    for (int i = 0; i < this->theSubgroupData.ccRepresentativesPreimages.size; i ++) {
      this->theSubgroupData.ccRepresentativesPreimages[i] = i;
    }
    this->theSubgroupData.theSubgroup->flagCCRepresentativesComputed = true;
  } else {
    if (this->theDynkinType.GetRank() <= 6) {
      this->theSubgroupData.theSubgroup->ComputeCCfromAllElements();
    } else {
      this->theSubgroupData.theSubgroup->ComputeCCSizesAndRepresentatives();
    }

    this->theSubgroupData.ComputeCCRepresentativesPreimages();
  }
}

void SubgroupDataRootReflections::InitGenerators() {
  MacroRegisterFunctionWithName("SubgroupRootReflections::InitGenerators");
  if (this->theDynkinType.GetRank() == 0) {
    this->theSubgroupData.theSubgroup->generators.SetSize(1);
    this->theSubgroupData.theSubgroup->generators[0].MakeID(*this->theSubgroupData.theGroup);
    return;
  }
  int d = this->SubCartanSymmetric.NumRows;
  this->theSubgroupData.generatorPreimages.SetSize(d);
  this->theSubgroupData.theSubgroup->generators.SetSize(d);
  ElementWeylGroup<WeylGroupData> currentReflection;
  for (int i = 0; i < d; i ++) {
    currentReflection.MakeRootReflection(this->generatingSimpleRoots[i], *this->theWeylData);
    this->theSubgroupData.generatorPreimages[i] = this->theSubgroupData.theGroup->theElements.GetIndex(currentReflection);
    this->theSubgroupData.theSubgroup->generators[i] = currentReflection;
  }
}

void SubgroupDataRootReflections::MakeParabolicSubgroup(WeylGroupData& G, const Selection& inputGeneratingSimpleRoots) {
  MacroRegisterFunctionWithName("SubgroupDataRootReflections::MakeParabolicSubgroup");
  G.CheckConsistency();
  this->theWeylData = &G;
  this->theSubgroupData.MakeSubgroupOf(G.theGroup);
  this->CheckInitialization();
  this->flagIsParabolic = true;
  this->simpleRootsInLeviParabolic = inputGeneratingSimpleRoots;
  Vectors<Rational> EiBasis;
  EiBasis.MakeEiBasis(G.GetDim());
  EiBasis.SubSelection(inputGeneratingSimpleRoots, this->generatingSimpleRoots);
  int d = inputGeneratingSimpleRoots.CardinalitySelection;
  this->SubCartanSymmetric.init(d, d);
  for (int ii = 0; ii < d; ii ++) {
    for (int jj = 0; jj < d; jj ++) {
      this->SubCartanSymmetric(ii, jj) = G.RootScalarCartanRoot(
        this->generatingSimpleRoots[ii], this->generatingSimpleRoots[jj]
      );
    }
  }
  this->ComputeDynkinType();
  this->InitGenerators();
}

void SubgroupDataRootReflections::MakeFromRoots(WeylGroupData& G, const Vectors<Rational>& inputRootReflections) {
  MacroRegisterFunctionWithName("SubgroupDataRootReflections::MakeFromRoots");
  this->theSubgroupData.MakeSubgroupOf(G.theGroup);
  this->theWeylData = &G;
  this->generatingSimpleRoots = inputRootReflections;
  DynkinDiagramRootSubalgebra theDiagram;
  theDiagram.ComputeDiagramTypeModifyInput(this->generatingSimpleRoots, G);
  int d = this->generatingSimpleRoots.size;
  this->SubCartanSymmetric.init(d, d);
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
    crash << "Two different comptuations of the Dynkin type a set of roots did not coincide. " << crash;
  }
  this->InitGenerators();
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
  theIterator.init(this->generators, left, ElementWeylGroup<WeylGroupData>::ConjugationAction);
  if (this->generators.size == 0) {
    crash << "generators not allowed to be 0. " << crash;
  }
  do {
    //if (left.ToString() == "s_{4}")
    //  stOutput << "<br>" << theIterator.GetCurrentElement().ToString() << "=?=" << right.ToString();
    if (theIterator.GetCurrentElement() == right) {
      return true;
    }
  } while (theIterator.IncrementReturnFalseIfPastLast());
  return false;
}

void WeylGroupData::GetSignSignatureAllRootSubsystems(List<SubgroupDataRootReflections>& outputSubgroups) {
  MacroRegisterFunctionWithName("WeylGroup::GetSignSignatureAllRootSubsystems");
  rootSubalgebras theRootSAs;
  SemisimpleLieAlgebra theSSlieAlg;
  theSSlieAlg.theWeyl.MakeFromDynkinType(this->theDynkinType);
//  theSSlieAlg.ComputeChevalleyConstants(theGlobalVariables);
  theRootSAs.owner = &theSSlieAlg;
  theRootSAs.ComputeAllReductiveRootSubalgebrasUpToIsomorphism();
  List<Vectors<Rational> > theRootSAsBases;
  theRootSAsBases.SetExpectedSize(theRootSAs.theSubalgebras.size);
  for (int i = theRootSAs.theSubalgebras.size - 1; i >= 0; i --) {
    theRootSAsBases.AddOnTop(theRootSAs.theSubalgebras[i].SimpleBasisK);
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
  sel.init(this->GetDim());
  int numCycles = MathRoutines::TwoToTheNth(sel.MaxSize);
  outputSubgroups.SetSize(numCycles);
  ElementWeylGroup<WeylGroupData> g;
  g.owner = this;
//  stOutput << "<hr>Meself is: " << this->ToString();
  for (int i = 0; i < outputSubgroups.size; i ++, sel.incrementSelection()) {
    SubgroupDataRootReflections& currentParabolic = outputSubgroups[i];
    currentParabolic.MakeParabolicSubgroup(*this, sel);
    currentParabolic.ComputeCCSizesRepresentativesPreimages();
//    stOutput << "<hr>Current parabolic is: " << currentParabolic.ToString();
    // ComputeInitialCharacters gets the character of the sign representation
    // as characterTable[1]
    //stOutput << "<hr>before compute initial irreps";
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
  parSelrootsAreInLevi.init(this->GetDim() + 1);
  Vectors<Rational> extendedBasis, currentBasisExtendedParabolic;
  extendedBasis.MakeEiBasis(this->GetDim());
  extendedBasis.AddOnTop(this->RootSystem[0]);
  outputSubgroups.SetExpectedSize(MathRoutines::TwoToTheNth(this->GetDim()));
  outputSubgroups.SetSize(0);
  SubgroupDataRootReflections theSG;
  do {
    extendedBasis.SubSelection(parSelrootsAreInLevi, currentBasisExtendedParabolic);
    if (currentBasisExtendedParabolic.GetRankOfSpanOfElements() == currentBasisExtendedParabolic.size) {
      theSG.MakeFromRoots(*this, currentBasisExtendedParabolic);
      theSG.flagIsExtendedParabolic = true;
      theSG.simpleRootsInLeviParabolic = parSelrootsAreInLevi;
      outputSubgroups.AddOnTop(theSG);
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
  outputSubgroups.SetSize(rootsGeneratingReflections.size);
  ElementWeylGroup<WeylGroupData> g;
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
