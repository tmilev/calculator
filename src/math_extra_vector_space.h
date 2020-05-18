// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfVectorSpaceHeader_alreadyIncluded
#define vpfVectorSpaceHeader_alreadyIncluded
#include "math_general.h"

template <typename Coefficient>
class Basis {
  public:
  //vectors are vector rows
  Matrix<Coefficient> basis;
  Matrix<Coefficient> gramMatrix;
  bool haveGramMatrix;

  void addVector(const Vector<Coefficient>& v);
  void computeGramMatrix();
  Vector<Coefficient> putInBasis(const Vector<Coefficient>& input);
  Matrix<Coefficient> putInBasis(const Matrix<Coefficient>& M);
};

template <typename Coefficient>
class VectorSpace {
public:
   int degree;
   int rank;
   Matrix<Coefficient> fastbasis;
   Basis<Coefficient> basis;

   VectorSpace() {
     degree = - 1;
     rank = 0;
   }
   void makeFullRank(int dim);
   // true if it wasn't already there
   bool addVector(const Vector<Coefficient>& v);
   bool addVectorDestructively(Vector<Coefficient>& v);
   bool addVectorToBasis(const Vector<Coefficient>& v);
   bool getCoordinatesDestructively(Vector<Coefficient>& v, Vector<Coefficient>& out) const;
   VectorSpace<Coefficient> intersection(const VectorSpace<Coefficient>& other) const;
   VectorSpace<Coefficient> Union(const VectorSpace<Coefficient>& other) const;
   VectorSpace<Coefficient> orthogonalComplement(VectorSpace<Coefficient>* ambient = 0, Matrix<Coefficient>* form = 0) const;
   Vector<Coefficient> getBasisVector(int i) const;
   Vector<Coefficient> getCanonicalBasisVector(int i) const;
   //unsigned int hashFunction() const {return this->hashFunction(*this);}
   static unsigned int hashFunction(const VectorSpace<Coefficient>& input) {
     return input.fastbasis.hashFunction();
   }
   bool operator==(const VectorSpace<Coefficient> &other) const;
};

template <typename Coefficient>
void Basis<Coefficient>::addVector(const Vector<Coefficient>& v) {
  if (basis.numberOfColumns == 0) {
    basis.initialize(v.size, v.size);
    basis.numberOfRows = 0;
  }
  if (basis.numberOfRows == basis.numberOfColumns) {
    global.fatal << "Programming error: attempting to add the "
    << basis.numberOfRows
    << " vector to a Basis of degree " << basis.numberOfColumns
    << global.fatal;
  }
  haveGramMatrix = false;
  for (int i = 0; i < v.size; i ++) {
    basis.elements[basis.numberOfRows][i] = v[i];
  }
  basis.numberOfRows ++;
}

template <typename Coefficient>
void Basis<Coefficient>::computeGramMatrix() {
  int r = basis.numberOfRows;
  int d = basis.numberOfColumns;
  gramMatrix.makeZeroMatrix(r);
  for (int i = 0; i < r; i ++) {
    for (int j = 0; j < r; j ++) {
      for (int k = 0; k < d; k ++) {
        gramMatrix.elements[i][j] += basis.elements[i][k] * basis.elements[j][k];
      }
    }
  }
  gramMatrix.invert();
  haveGramMatrix = true;
}

template <typename Coefficient>
Vector<Coefficient> Basis<Coefficient>::putInBasis(const Vector<Coefficient>& input) {
  if (false) {
    Vectors<Coefficient> theBasisVectorForm;
    this->basis.getVectorsFromRows(theBasisVectorForm);
    Vector<Coefficient> output;
    input.getCoordinatesInBasis(theBasisVectorForm, output);
    return output;
  } else {
    if (!haveGramMatrix) {
      computeGramMatrix();
    }
    return gramMatrix * (basis * input);
  }
}

template <typename Coefficient>
Matrix<Coefficient> Basis<Coefficient>::putInBasis(const Matrix<Coefficient>& in) {
  if (!haveGramMatrix) {
    computeGramMatrix();
  }
  return gramMatrix * (basis * in);
}

template <typename Coefficient>
bool VectorSpace<Coefficient>::addVector(const Vector<Coefficient>& v) {
  Vector<Coefficient> tmp = v;
  return addVectorDestructively(tmp);
}

template <typename Coefficient>
bool VectorSpace<Coefficient>::addVectorDestructively(Vector<Coefficient>& v) {
  if (fastbasis.numberOfRows == 0) {
    this->fastbasis.makeZeroMatrix(v.size);
    this->degree = v.size;
    int nzi = 0;
    for (; nzi < degree; nzi ++) {
      if (v[nzi] != 0) {
        break;
      }
    }
    if (nzi == degree) {
      rank = 0;
      fastbasis.numberOfRows = 0;
      return false;
    }
    for (int i = 0; i < v.size; i ++) {
      fastbasis.elements[0][i] = v[i] / v[nzi];
    }
    rank = 1;
    this->fastbasis.numberOfRows = 1;
    return true;
  }
  int jj = 0;
  for (int i = 0; i < fastbasis.numberOfRows; i ++) {
    while ((jj < v.size) && (v[jj] == 0)) {
      jj ++;
    }
    if (jj == v.size) {
      return false;
    }
    int j = i;
    for (; (j < fastbasis.numberOfColumns) && (fastbasis.elements[i][j] == 0); j ++);
    if (jj < j) {
      if (fastbasis.ActualNumRows >= fastbasis.numberOfRows + 1) {
        fastbasis.numberOfRows ++;
      } else {
        fastbasis.resize(fastbasis.numberOfRows + 1,fastbasis.numberOfColumns, true);
      }
      Coefficient* tmp = fastbasis.elements[fastbasis.numberOfRows - 1];
      for (int bi = fastbasis.numberOfRows - 1; bi > i; bi --) {
        fastbasis.elements[bi] = fastbasis.elements[bi - 1];
      }
      fastbasis.elements[i] = tmp;
      for (int bj = 0; bj < fastbasis.numberOfColumns; bj ++) {
        fastbasis.elements[i][bj] = v[bj];
      }
      fastbasis.gaussianEliminationByRows();
      rank ++;
      return true;
    }
    if (jj > j) {
      continue;
    }
    Coefficient x = - v[jj] / fastbasis.elements[i][j];
    for (int jjj = jj; jjj < v.size; jjj ++) {
      v[jjj] += x * fastbasis.elements[i][jjj];
    }
  }
  if (v.isEqualToZero()) {
    return false;
  }
  // this should take the same amount of time either way
  if (fastbasis.ActualNumRows >= fastbasis.numberOfRows + 1) {
    fastbasis.numberOfRows ++;
  } else {
    fastbasis.resize(fastbasis.numberOfRows + 1, fastbasis.numberOfColumns, true);
  }
  for (int j = 0; j < fastbasis.numberOfColumns; j ++) {
    fastbasis.elements[fastbasis.numberOfRows - 1][j] = v[j];
  }
  rank ++;
  return true;
}

template <typename Coefficient>
bool VectorSpace<Coefficient>::addVectorToBasis(const Vector<Coefficient>& v) {
  if (addVector(v)) {
    basis.addVector(v);
    return true;
  }
  return false;
}

template <typename Coefficient>
bool VectorSpace<Coefficient>::getCoordinatesDestructively(Vector<Coefficient>& v, Vector<Coefficient>& out) const {
  out.makeZero(this->rank);
  if (v.isEqualToZero()) {
    if (this->rank == 0) {
      return false;
    }
    return true;
  }
  int i = 0;
  while (true) {
    int vi = v.getIndexFirstNonZeroCoordinate();
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
    Coefficient c = v[vi] / this->fastbasis(i, vi);
    out[i] = c;
    for (int k = 0; k < this->degree; k ++) {
      v[k] -= this->fastbasis(i, k) * c;
    }
    if (v.isEqualToZero()) {
      return true;
    }
  }
}

template<typename Coefficient>
VectorSpace<Coefficient> VectorSpace<Coefficient>::intersection(const VectorSpace<Coefficient>& other) const {
  // perhaps at some point it would be nice to ban intersections with spaces of unspecified degree
  if (this->degree != other.degree && ((this->degree != - 1) && (other.degree != - 1))) {
    global.fatal << "Attempting to intersect vector spaces of different degrees, "
    << this->degree << " and " << other.degree << ". " << global.fatal;
  }
  VectorSpace<Coefficient> output;
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
  List<Vector<Coefficient> > Vperp;
  MV.getZeroEigenSpaceModifyMe(Vperp);

  Matrix<Coefficient> MW = other.fastbasis;
  List<Vector<Coefficient> > Wperp;
  MW.getZeroEigenSpaceModifyMe(Wperp);

  Matrix<Coefficient> M;
  M.initialize(Vperp.size + Wperp.size, this->degree);
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
  List<Vector<Coefficient> > outvecs;
  M.getZeroEigenSpaceModifyMe(outvecs);
  output.rank = outvecs.size;
  if (outvecs.size == 0) {
    return output;
  }
  output.fastbasis.initialize(outvecs.size,this->degree);
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
  output.fastbasis.gaussianEliminationByRows();
  return output;
}

template <typename Coefficient>
VectorSpace<Coefficient> VectorSpace<Coefficient>::orthogonalComplement(
  VectorSpace<Coefficient>* ambient, Matrix<Coefficient>* form
) const {
  VectorSpace<Coefficient> V;
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
  Matrix<Coefficient> M = this->fastbasis;
  if (form) {
    M.multiplyOnTheRight(*form); // i can never tell which one is right or left :/
  }
  List<Vector<Coefficient> > VVs;
  // this is where 'nullspace' and 'kernel' are conceptually different
  M.getZeroEigenSpaceModifyMe(VVs);
  // this appears common enough to warrant a better method
  for (int i = 0; i < VVs.size; i ++) {
    V.addVectorDestructively(VVs[i]);
  }
  if (ambient && ambient->rank < ambient->degree) {
    VectorSpace<Coefficient> W = V.intersection(*ambient);
    return W;
  }
  return V;
}

// part of the point of coming up with a VectorSpace class
// was to find a way to special case full rank spaces and not carry around
// identity matrices everywhere...
template <typename Coefficient>
void VectorSpace<Coefficient>::makeFullRank(int d) {
  this->degree = d;
  this->rank = d;
}

template <typename Coefficient>
bool VectorSpace<Coefficient>::operator==(const VectorSpace<Coefficient>& other) const {
  return fastbasis == other.fastbasis;
}

template <typename Coefficient>
Vector<Coefficient> VectorSpace<Coefficient>::getBasisVector(int i) const {
  if (i >= this->rank) {
    global.fatal << "Bad vector index. " << global.fatal;
  }
  Vector<Coefficient> out;
  if (basis.basis.numberOfRows > i) {
    basis.basis.getVectorFromRow(i, out);
    return out;
  }
  fastbasis.getVectorFromRow(i, out);
  return out;
}

template <typename Coefficient>
Vector<Coefficient> VectorSpace<Coefficient>::getCanonicalBasisVector(int i) const {
  if (i >= this->rank) {
    global.fatal << "Vector index too large. " << global.fatal;
  }
  Vector<Coefficient> out;
  fastbasis.getVectorFromRow(i, out);
  return out;
}
#endif
