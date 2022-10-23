
#ifndef header_math_extra_packed_vector_implementation_ALREADY_INCLUDED
#define header_math_extra_packed_vector_implementation_ALREADY_INCLUDED

#include "math_extra_packed_vector.h"
#include "assert.h"

template <typename scalar>
const int PackedVector<scalar>::size;

template <typename scalar>
PackedVector<scalar> PackedVector<scalar>::operator+(
  const PackedVector<scalar>& w
) const {
  PackedVector<scalar> out;
  for (int i = 0; i < this->size; i ++) {
    out[i] = this->data[i] + w[i];
  }
  return out;
}

template <typename scalar>
void PackedVector<scalar>::operator+=(const PackedVector<scalar>& w) {
  for (int i = 0; i < this->size; i ++) {
    this->data[i] += w[i];
  }
}

template <typename scalar>
PackedVector<scalar> PackedVector<scalar>::operator*(scalar x) const {
  PackedVector<scalar> out;
  for (int i = 0; i < this->size; i ++) {
    out[i] = this->data[i] * x;
  }
  return out;
}

template <typename scalar>
scalar PackedVector<scalar>::scalarProduct(
  const PackedVector<scalar>& v, const Matrix<scalar>& B
) const {
  PackedVector<scalar> Bv;
  Bv.makeZero(B.numberOfRows);
  for (int i = 0; i < B.numberOfRows; i ++) {
    for (int j = 0; j < B.numberOfColumns; j ++) {
      Bv[i] += B.elements[i][j] * v[j];
    }
  }
  scalar wBv = 0;
  for (int i = 0; i < B.numberOfRows; i ++) {
    wBv += this->data[i] * Bv[i];
  }
  return wBv;
}

/*
template <typename scalar>
scalar PackedVector<scalar>::scalarProduct(const PackedVector<scalar>& v, const PackedVector* B) const {
  PackedVector<scalar> Bv;
  Bv.makeZero();
  for (int i = 0; i < this->size; i ++)
    for (int j = 0; j < this->size; j ++)
      Bv[i] += B[i][j]*v[j]
  scalar wBv = 0;
  for (int i = 0; i <B.numberOfRows; i ++)
    wBv += this->data[i]*Bv[i];
  return wBv;
}
*/
template <typename scalar>
scalar&PackedVector<scalar>::operator[](int i) {
  return this->data[i];
}

template <typename scalar>
scalar PackedVector<scalar>::operator[](int i) const {
  return this->data[i];
}

template <typename scalar>
void PackedVector<scalar>::setSize(int s) {
  if (s > this->size) {
    global.comments
    << "if this was intentional, recompile PackedVector with size>="
    << s
    << "\n";
    global.fatal << "Bad template argument." << global.fatal;
  }
}

template <typename scalar>
bool PackedVector<scalar>::operator!=(const PackedVector<scalar>& w) const {
  for (int i = 0; i < this->size; i ++) {
    if (w[i] != this->data[i]) {
      return true;
    }
  }
  return false;
}

template <typename scalar>
bool PackedVector<scalar>::operator==(const PackedVector<scalar>& w) const {
  return !(*this != w);
}

template <typename scalar>
bool PackedVector<scalar>::operator>(const PackedVector<scalar>& w) const {
  for (int i = 0; i < this->size; i ++) {
    if (this->data[i] > w[i]) {
      return true;
    }
    if (this->data[i] < w[i]) {
      return false;
    }
  }
  return false;
}

template <typename scalar>
void PackedVector<scalar>::makeZero(int n) {
  (void) n;
  for (int i = 0; i < this->size; i ++) {
    this->data[i] = 0;
  }
}

template <typename scalar>
void PackedVector<scalar>::makeEi(int d, int ei) {
  (void) d;
  for (int i = 0; i < this->size; i ++) {
    this->data[i] = 0;
  }
  this->data[ei] = 1;
}

template <typename scalar>
unsigned int PackedVector<scalar>::hashFunction(
  const PackedVector<scalar>& in
) {
  return in.hashFunction();
}

template <typename scalar>
unsigned int PackedVector<scalar>::hashFunction() const {
  unsigned int result = 0;
  int j = 0;
  for (int i = 0; i < this->size; i ++) {
    result +=
    this->data[i].hashFunction() *
    HashConstants::getConstantIncrementCounter(j);
  }
  return result;
}

template <typename scalar>
std::ostream& operator<<(
  std::ostream& out, const PackedVector<scalar>& v
) {
  out << '(';
  for (int i = 0; i < v.size; i ++) {
    out << v[i];
    if (i != v.size - 1) {
      out << ", ";
    }
  }
  out << ')';
  return out;
}

#endif // header_math_extra_packed_vector_implementation_ALREADY_INCLUDED
