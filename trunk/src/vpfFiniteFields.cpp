#include "vpfFiniteFields.h"
#include <cassert>

f65521 f65521::operator+(const f65521 right) const
{ f65521 out;
  out.n = (n+right.n)%65521;
  return out;
}

void f65521::operator+=(const f65521 right)
{ n += right.n;
  n %= 65521;
}

void f65521::Minus()
{ n = 65521-n;
}

f65521 f65521::operator-() const
{ f65521 out;
  out.n = 65521-n;
  return out;
}

f65521 f65521::operator-(const f65521 right) const
{ return *this+(-right);
}

void f65521::operator-=(const f65521 right)
{ n += (-right).n;
  n %= 65521;
}

f65521 f65521::operator*(const f65521 right) const
{ f65521 out;
  out.n = (n*right.n)%65521;
  return out;
}

void f65521::operator*=(const f65521 right)
{ n *= right.n;
  n %= 65521;
}

void f65521::Invert()
{ int x = 0;
  int y = 1;
  int a = 65521;
  int b = n;
  int l = 1;
  int m = 0;
  while(b != 0)
  { int q = a / b;
    int r = a % b;
    a = b;
    b = r;
    int s = l - q*x;
    int t = m - q*y;
    l = x;
    m = y;
    x = s;
    y = t;
  }

  if(m>=0)
    n = m;
  else
    n = m+65521;
}

f65521 f65521::operator/(const f65521 right) const
{ f65521 tmp = right;
  tmp.Invert();
  return *this * tmp;
}

void f65521::operator/=(const f65521 right)
{ f65521 tmp = right;
  tmp.Invert();
  *this *= tmp;
}

bool f65521::IsEqualToZero() const
{ return n==0;
}

bool f65521::IsEqualToOne() const
{ return n==1;
}

bool f65521::operator==(const f65521 rhs) const
{ return n == rhs.n;
}

bool f65521::operator!=(const f65521 rhs) const
{ return n != rhs.n;
}

void f65521::operator=(const int rhs)
{ int tmp = rhs%65521;
  if(tmp<0)
    tmp += 65521;
  n = tmp;
}

void f65521::operator=(const Rational rhs)
{ n = rhs.GetNumerator()%65521;
  f65521 den = rhs.GetDenominator()%65521;
  *this /= den;
}

std::string f65521::ToString(FormatExpressions* f) const
{ (void) f; //avoid unused parameter warning, portable.
  return std::to_string(n);
}

std::ostream& operator<<(std::ostream& out, const f65521& data)
{ out << data.ToString();
  return out;
}

f211 f211::operator+(const f211 right) const
{ f211 out;
  int tmp = n;
  tmp += right.n;
  tmp %= 211;
  out.n = tmp;
  return out;
}

void f211::operator+=(const f211 right)
{ int tmp = n;
  tmp += right.n;
  tmp %= 211;
  n = tmp;
}

void f211::Minus()
{ n = 211-n;
}

f211 f211::operator-() const
{ f211 out;
  out.n = 211-n;
  return out;
}

f211 f211::operator-(const f211 right) const
{ return *this+(-right);
}

void f211::operator-=(const f211 right)
{ int tmp = n;
  tmp += (-right).n;
  tmp %= 211;
  n = tmp;
}

f211 f211::operator*(const f211 right) const
{ f211 out;
  int tmp;
  tmp = n;
  tmp *= right.n;
  tmp %= 211;
  out.n = tmp;
  return out;
}

void f211::operator*=(const f211 right)
{ int tmp;
  tmp = n;
  tmp *= right.n;
  tmp %= 211;
  n = tmp;
}

void f211::Invert()
{ int x = 0;
  int y = 1;
  int a = 211;
  int b = n;
  int l = 1;
  int m = 0;
  while(b != 0)
  { int q = a / b;
    int r = a % b;
    a = b;
    b = r;
    int s = l - q*x;
    int t = m - q*y;
    l = x;
    m = y;
    x = s;
    y = t;
  }

  if(m>=0)
    n = m;
  else
    n = m+211;
}

f211 f211::operator/(const f211 right) const
{ f211 tmp = right;
  tmp.Invert();
  return *this * tmp;
}

void f211::operator/=(const f211 right)
{ f211 tmp = right;
  tmp.Invert();
  *this *= tmp;
}

bool f211::IsEqualToZero() const
{ return n==0;
}

bool f211::IsEqualToOne() const
{ return n==1;
}

bool f211::operator==(const f211 rhs) const
{ return n == rhs.n;
}

bool f211::operator!=(const f211 rhs) const
{ return n != rhs.n;
}

void f211::operator=(const int rhs)
{ int tmp = rhs % 211;
  if(tmp<0)
    tmp += 211;
  n = tmp;
}

void f211::operator=(const Rational rhs)
{ n = rhs.GetNumerator()%211;
  f211 den = rhs.GetDenominator()%211;
  *this /= den;
}

bool f211::operator<(int rhs) const
{ if(rhs != 0)
  { stOutput << "finite fields are not ordered (if you think they are, modify around " << __FILE__ << ':' << __LINE__ << ", and may God have mercy on your soul)" << "\n";
    assert(false);
  }
  if(n>104)
    return true;
  else
    return false;
}

bool f211::operator>(int rhs) const
{ if(rhs != 0)
  { stOutput << "finite fields are not ordered (if you think they are, modify around " << __FILE__ << ':' << __LINE__ << ", and may God have mercy on your soul)" << "\n";
    assert(false);
  }
  if(n<105)
    return true;
  else
    return false;
}

bool f211::operator>(f211 rhs) const
{ if(this->n > rhs.n)
    return true;
  return false;
}

bool f211::operator<(f211 rhs) const
{ if(this->n < rhs.n)
    return true;
  return false;
}

unsigned int f211::HashFunction() const
{ return (unsigned int) this->n;
}

std::string f211::ToString(FormatExpressions* f) const
{ (void) f; //avoid unused parameter warning, portable.
  return std::to_string(n);
}

std::ostream& operator<<(std::ostream& out, const f211& data)
{ out << data.ToString();
  return out;
}
