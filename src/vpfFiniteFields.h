//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#ifndef vpfJsons_h_already_included
#define vpfJson_h_already_included

#include "math_2_large_integers.h"
static ProjectInformationInstance vpfFiniteField(__FILE__, "Prime fields");

/*static FiniteFieldFactory myFiniteFieldFactory = FiniteFieldFactory();
  myFiniteField = myFiniteFieldFactory.FiniteField(p);
*/


class f65521 {
public:
  unsigned int n;

  f65521(){}

  f65521(const int right) {
    *this = right;
  }

  f65521(const Rational right) {
    *this = right;
  }

  f65521 operator+(const f65521 right) const;
  void operator+=(const f65521 right);
  void Minus();
  f65521 operator-() const;
  f65521 operator-(const f65521 right) const;
  void operator-=(const f65521 right);
  f65521 operator*(const f65521 right) const;
  void operator*=(const f65521 right);
  void Invert();
  f65521 operator/(const f65521 right) const;
  void operator/=(const f65521 right);
  void operator=(const int rhs);
  void operator=(const Rational rhs);

  bool operator==(const f65521 rhs) const;
  bool operator!=(const f65521 rhs) const;
  bool IsEqualToZero() const;
  bool IsEqualToOne() const;
  bool NeedsParenthesisForMultiplication() {
    return false;
  }
  std::string ToString(FormatExpressions* f = nullptr) const;
};
std::ostream& operator<<(std::ostream& out, const f65521& data);


class f211 {
public:
  char n;

  f211(){}

  f211(const int right) {
    *this = right;
  }

  f211(const Rational right) {
    *this = right;
  }

  f211 operator+(const f211 right) const;
  void operator+=(const f211 right);
  void Minus();
  f211 operator-() const;
  f211 operator-(const f211 right) const;
  void operator-=(const f211 right);
  f211 operator*(const f211 right) const;
  void operator*=(const f211 right);
  void Invert();
  f211 operator/(const f211 right) const;
  void operator/=(const f211 right);
  void operator=(const int rhs);
  void operator=(const Rational rhs);

  bool operator==(const f211 rhs) const;
  bool operator!=(const f211 rhs) const;
  bool IsEqualToZero() const;
  bool IsEqualToOne() const;

  // these are nonsensical, and compatibility with
  // code that assumes an ordered ring is not desirable
  bool operator<(int rhs) const;
  bool operator>(int rhs) const;
  // hopefully these are only ever used for sorting
  // there's no way to use the type system to requite that
  bool operator<(f211 rhs) const;
  bool operator>(f211 rhs) const;

  unsigned int HashFunction() const;

  std::string ToString(FormatExpressions* f = nullptr) const;
};
std::ostream& operator<<(std::ostream& out, const f211& data);


#endif
