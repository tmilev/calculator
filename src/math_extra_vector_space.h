// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfVectorSpaceHeader_alreadyIncluded
#define vpfVectorSpaceHeader_alreadyIncluded
#include "math_general.h"

static ProjectInformationInstance ProjectInfoVpfVectorSpace(
  __FILE__,
  "Header file, Vector spaces. "
  "Encodes global information about vector spaces. "
  "Utility of class not agreed upon yet. "
);

template <typename coefficient>
class Basis {
  public:
  //vectors are vector rows
  Matrix<coefficient> basis;
  Matrix<coefficient> gramMatrix;
  bool haveGramMatrix;

  void AddVector(const Vector<coefficient>& v);
  void ComputeGramMatrix();
  Vector<coefficient> PutInBasis(const Vector<coefficient>& input);
  Matrix<coefficient> PutInBasis(const Matrix<coefficient>& M);
};

template <typename coefficient>
class VectorSpace {
public:
   int degree;
   int rank;
   Matrix<coefficient> fastbasis;
   Basis<coefficient> basis;

   VectorSpace() {
     degree = - 1;
     rank = 0;
   }
   void MakeFullRank(int dim);
   // true if it wasn't already there
   bool AddVector(const Vector<coefficient>& v);
   bool AddVectorDestructively(Vector<coefficient>& v);
   bool AddVectorToBasis(const Vector<coefficient>& v);
   bool GetCoordinatesDestructively(Vector<coefficient>& v, Vector<coefficient>& out) const;
   VectorSpace<coefficient> Intersection(const VectorSpace<coefficient>& other) const;
   VectorSpace<coefficient> Union(const VectorSpace<coefficient>& other) const;
   VectorSpace<coefficient> OrthogonalComplement(VectorSpace<coefficient>* ambient = 0, Matrix<coefficient>* form = 0) const;
   Vector<coefficient> GetBasisVector(int i) const;
   Vector<coefficient> GetCanonicalBasisVector(int i) const;
//   unsigned int HashFunction() const {return this->HashFunction(*this);}
   static unsigned int HashFunction(const VectorSpace<coefficient>& input) {
     return input.fastbasis.HashFunction();
   }
   bool operator==(const VectorSpace<coefficient> &other) const;
};

template <typename coefficient>
void Basis<coefficient>::AddVector(const Vector<coefficient>& v) {
  if (basis.NumCols == 0) {
    basis.init(v.size, v.size);
    basis.NumRows = 0;
  }
  if (basis.NumRows == basis.NumCols) {
    global.fatal << "Programming error: attempting to add the " << basis.NumRows
    << " vector to a Basis of degree " << basis.NumCols;
    global.fatal << global.fatal;
  }
  haveGramMatrix = false;
  for (int i = 0; i < v.size; i ++) {
    basis.elements[basis.NumRows][i] = v[i];
  }
  basis.NumRows ++;
}

template <typename coefficient>
void Basis<coefficient>::ComputeGramMatrix() {
  int r = basis.NumRows;
  int d = basis.NumCols;
  gramMatrix.MakeZeroMatrix(r);
  for (int i = 0; i < r; i ++) {
    for (int j = 0; j < r; j ++) {
      for (int k = 0; k < d; k ++) {
        gramMatrix.elements[i][j] += basis.elements[i][k] * basis.elements[j][k];
      }
    }
  }
  gramMatrix.Invert();
  haveGramMatrix = true;
}

template <typename coefficient>
Vector<coefficient> Basis<coefficient>::PutInBasis(const Vector<coefficient>& input) {
  if (false) {
    Vectors<coefficient> theBasisVectorForm;
    this->basis.GetVectorsFromRows(theBasisVectorForm);
    Vector<coefficient> output;
    input.GetCoordsInBasiS(theBasisVectorForm, output);
    return output;
  } else {
    if (!haveGramMatrix) {
      ComputeGramMatrix();
    }
    return gramMatrix*(basis * input);
  }
}

template <typename coefficient>
Matrix<coefficient> Basis<coefficient>::PutInBasis(const Matrix<coefficient>& in) {
  if (!haveGramMatrix) {
    ComputeGramMatrix();
  }
  return gramMatrix * (basis * in);
}

template <typename coefficient>
bool VectorSpace<coefficient>::AddVector(const Vector<coefficient>& v) {
  Vector<coefficient> tmp = v;
  return AddVectorDestructively(tmp);
}

template <typename coefficient>
bool VectorSpace<coefficient>::AddVectorDestructively(Vector<coefficient>& v) {
  if (fastbasis.NumRows == 0) {
    this->fastbasis.MakeZeroMatrix(v.size);
    this->degree = v.size;
    int nzi = 0;
    for (; nzi < degree; nzi ++) {
      if (v[nzi] != 0) {
        break;
      }
    }
    if (nzi == degree) {
      rank = 0;
      fastbasis.NumRows = 0;
      return false;
    }
    for (int i = 0; i < v.size; i ++) {
      fastbasis.elements[0][i] = v[i] / v[nzi];
    }
    rank = 1;
    this->fastbasis.NumRows = 1;
    return true;
  }
  int jj = 0;
  for (int i = 0; i < fastbasis.NumRows; i ++) {
    while ((jj < v.size) && (v[jj] == 0)) {
      jj ++;
    }
    if (jj == v.size) {
      return false;
    }
    int j = i;
    for (; (j < fastbasis.NumCols) && (fastbasis.elements[i][j] == 0); j ++);
    if (jj < j) {
      if (fastbasis.ActualNumRows >= fastbasis.NumRows + 1) {
        fastbasis.NumRows ++;
      } else {
        fastbasis.Resize(fastbasis.NumRows + 1,fastbasis.NumCols, true);
      }
      coefficient* tmp = fastbasis.elements[fastbasis.NumRows - 1];
      for (int bi = fastbasis.NumRows - 1; bi > i; bi --) {
        fastbasis.elements[bi] = fastbasis.elements[bi - 1];
      }
      fastbasis.elements[i] = tmp;
      for (int bj = 0; bj < fastbasis.NumCols; bj ++) {
        fastbasis.elements[i][bj] = v[bj];
      }
      fastbasis.GaussianEliminationByRows();
      rank ++;
      return true;
    }
    if (jj > j) {
      continue;
    }
    coefficient x = - v[jj] / fastbasis.elements[i][j];
    for (int jjj = jj; jjj < v.size; jjj ++) {
      v[jjj] += x * fastbasis.elements[i][jjj];
    }
  }
  if (v.IsEqualToZero()) {
    return false;
  }
  // this should take the same amount of time either way
  if (fastbasis.ActualNumRows >= fastbasis.NumRows + 1) {
    fastbasis.NumRows ++;
  } else {
    fastbasis.Resize(fastbasis.NumRows + 1, fastbasis.NumCols, true);
  }
  for (int j = 0; j < fastbasis.NumCols; j ++) {
    fastbasis.elements[fastbasis.NumRows - 1][j] = v[j];
  }
  rank ++;
  return true;
}

template <typename coefficient>
bool VectorSpace<coefficient>::AddVectorToBasis(const Vector<coefficient>& v) {
  if (AddVector(v)) {
    basis.AddVector(v);
    return true;
  }
  return false;
}

template <typename coefficient>
bool VectorSpace<coefficient>::GetCoordinatesDestructively(Vector<coefficient>& v, Vector<coefficient>& out) const {
  out.MakeZero(this->rank);
  if (v.IsEqualToZero()) {
    if (this->rank == 0) {
      return false;
    }
    return true;
  }
  int i = 0;
  while (true) {
    int vi = v.GetIndexFirstNonZeroCoordinate();
    while (true) {
      if (i == this->rank) {
        return false;
      }
      int mi = 0;
      while (this->fastbasis(i, mi) == 0) {
        mi ++;
      }
      if (mi > vi) {
        return false;
      }
      if (mi == vi) {
        break;
      }
      i ++;
    }
    coefficient c = v[vi] / this->fastbasis(i, vi);
    out[i] = c;
    for (int k = 0; k < this->degree; k ++) {
      v[k] -= this->fastbasis(i, k) * c;
    }
    if (v.IsEqualToZero()) {
      return true;
    }
  }
}

template<typename coefficient>
VectorSpace<coefficient> VectorSpace<coefficient>::Intersection(const VectorSpace<coefficient>& other) const {
  // perhaps at some point it would be nice to ban intersections with spaces of unspecified degree
  if (this->degree != other.degree && ((this->degree != - 1) && (other.degree != - 1))) {
    global.fatal << "Attempting to intersect vector spaces of different degrees, "
    << this->degree << " and " << other.degree << ". " << global.fatal;
  }
  VectorSpace<coefficient> output;
  output.degree = this->degree;
  if ((this->rank == 0) or (other.rank == 0)) {
    return output;
  }

  if (this->rank == this->degree) {
    output.rank = other.rank;
    output.fastbasis = other.fastbasis;
    return output;
  }

  if (other.rank == other.degree) {
    output.rank = this->rank;
    output.fastbasis = this->fastbasis;
    return output;
  }


  Matrix<Rational> MV = this->fastbasis;
  List<Vector<coefficient> > Vperp;
  MV.GetZeroEigenSpaceModifyMe(Vperp);

  Matrix<coefficient> MW = other.fastbasis;
  List<Vector<coefficient> > Wperp;
  MW.GetZeroEigenSpaceModifyMe(Wperp);

  Matrix<coefficient> M;
  M.init(Vperp.size + Wperp.size, this->degree);
  int i = 0;
  for (; i < Vperp.size; i ++) {
    for (int j = 0; j < this->degree; j ++) {
      M.elements[i][j] = Vperp[i][j];
    }
  }
  for (; i < Vperp.size +Wperp.size; i ++) {
    for (int j = 0; j < this->degree; j ++) {
      M.elements[i][j] = Wperp[i - Vperp.size][j];
    }
  }
  List<Vector<coefficient> > outvecs;
  M.GetZeroEigenSpaceModifyMe(outvecs);
  output.rank = outvecs.size;
  if (outvecs.size == 0) {
    return output;
  }
  output.fastbasis.init(outvecs.size,this->degree);
  if (outvecs.size == 1) {
    int inz = 0;
    for (; inz < outvecs[0].size; inz ++) {
      if (outvecs[0][inz] != 0) {
        break;
      }
    }
    for (int i = 0; i < outvecs[0].size; i ++) {
      output.fastbasis.elements[0][i] = outvecs[0][i] / outvecs[0][inz];
    }
    return output;
  }
  for (int i = 0; i < outvecs.size; i ++) {
    for (int j = 0; j < this->degree; j ++) {
      output.fastbasis.elements[i][j] = outvecs[i][j];
    }
  }
  output.fastbasis.GaussianEliminationByRows();
  return output;
}

template <typename coefficient>
VectorSpace<coefficient> VectorSpace<coefficient>::OrthogonalComplement(
  VectorSpace<coefficient>* ambient, Matrix<coefficient>* form
) const {
  VectorSpace<coefficient> V;
  V.degree = this->degree;
  if (this->rank == this->degree) {
    V.rank = 0;
    return V;
  }
  if (this->rank == 0) {
    if (ambient) {
      V.rank = ambient->rank;
      V.fastbasis = ambient->fastbasis;
      return V;
    }
    V.rank = this->degree;
    return V;
  }
  Matrix<coefficient> M = this->fastbasis;
  if (form) {
    M.MultiplyOnTheRight(*form); // i can never tell which one is right or left :/
  }
  List<Vector<coefficient> > VVs;
  // this is where 'nullspace' and 'kernel' are conceptually different
  M.GetZeroEigenSpaceModifyMe(VVs);
  // this appears common enough to warrant a better method
  for (int i = 0; i < VVs.size; i ++) {
    V.AddVectorDestructively(VVs[i]);
  }
  if (ambient && ambient->rank < ambient->degree) {
    VectorSpace<coefficient> W = V.Intersection(*ambient);
    return W;
  }
  return V;
}

// part of the point of coming up with a VectorSpace class
// was to find a way to special case full rank spaces and not carry around
// identity matrices everywhere...
template <typename coefficient>
void VectorSpace<coefficient>::MakeFullRank(int d) {
  this->degree = d;
  this->rank = d;
}

template <typename coefficient>
bool VectorSpace<coefficient>::operator==(const VectorSpace<coefficient>& other) const {
  return fastbasis == other.fastbasis;
}

template <typename coefficient>
Vector<coefficient> VectorSpace<coefficient>::GetBasisVector(int i) const {
  if (i >= this->rank) {
    global.fatal << "Bad vector index. " << global.fatal;
  }
  Vector<coefficient> out;
  if (basis.basis.NumRows > i) {
    basis.basis.GetVectorFromRow(i, out);
    return out;
  }
  fastbasis.GetVectorFromRow(i, out);
  return out;
}

template <typename coefficient>
Vector<coefficient> VectorSpace<coefficient>::GetCanonicalBasisVector(int i) const {
  if (i >= this->rank) {
    global.fatal << "Vector index too large. " << global.fatal;
  }
  Vector<coefficient> out;
  fastbasis.GetVectorFromRow(i, out);
  return out;
}
#endif
