// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader2Math_01LargeIntegersInstance_already_included
#define vpfHeader2Math_01LargeIntegersInstance_already_included

#include "general_lists.h"

class LargeIntegerUnsigned {
  void AddNoFitSize(const LargeIntegerUnsigned& x);
public:
  // The zero element is assumed to have length one array with a zero entry.
  //
  // CarryOverBound is the "base" over which we work.
  // Requirements on the CarryOverBound:
  // 1.  +/-(CarryOverBound*2)- 1 must fit inside an (int)
  //     on the system
  // 2. (CarryOverBound*2)^2- 1 must fit inside (long long)
  //     on the system.
  ////////////////////////////////////////////////////////
  // On a 32 bit machine any number smaller than or equal to 2^30 will work.
  // If you got no clue what to put just leave CarryOverBound as it is below.
  List<int32_t> theDigits;
  // static const int CarryOverBound =10; //<-for extreme "corner case" testing
  static const int CarryOverBound = 1000000000;
  // the above choice of CarryOverBound facilitates very quick conversions of Large integers into decimal, with
  // relatively small loss of speed and RAM memory.
  // static const unsigned int CarryOverBound =2147483648UL; //=2^31
  // The following must be less than or equal to the square root of CarryOverBound.
  // It is used for quick multiplication of Rational-s.
  // static const int SquareRootOfCarryOverBound =3;//<-for extreme "corner case" testing
  static const int SquareRootOfCarryOverBound = 31000; //31000*31000=961000000<1000000000
  // static const int SquareRootOfCarryOverBound =32768; //=2^15
  friend bool operator<(int left, const LargeIntegerUnsigned& right) {
    return right > left;
  }
  friend std::ostream& operator<<(std::ostream& output, const LargeIntegerUnsigned& theLIU) {
    output << theLIU.toString();
    return output;
  }
  class Test {
    public:
    static bool All();
    static bool SerializationToHex(const LargeIntegerUnsigned& input);
    static bool SerializationToHex();
    static bool Comparisons();
  };
  void PadWithZeroesToAtLeastNDigits(int desiredMinNumDigits);
  void AddLargeIntUnsignedShiftedTimesDigit(const LargeIntegerUnsigned& other, int digitShift, int theConst);
  void SubtractSmallerPositive(const LargeIntegerUnsigned& x);
  void toString(std::string& output) const;
  void ElementToStringLargeElementDecimal(std::string& output) const;
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringAbbreviate(FormatExpressions* theFormat = nullptr) const;
  void DivPositive(const LargeIntegerUnsigned& divisor, LargeIntegerUnsigned& quotientOutput, LargeIntegerUnsigned& remainderOutput) const;
  bool isDivisibleBy(const LargeIntegerUnsigned& divisor);
  void makeOne();
  void AddUInt(unsigned int x);
  void makeZero();
  bool IsEqualToZero() const;
  bool IsEven() const;
  bool IsPositive() const;
  bool TryToFindWhetherIsPower(bool& outputIsPower, LargeInteger& outputBase, int& outputPower) const;
  bool IsCompositePrimeDivision(List<unsigned int>& primesGenerated, std::stringstream* comments = nullptr);
  bool IsPossiblyPrime(int MmillerRabinTries, bool tryDivisionSetTrueFaster = true, std::stringstream* comments = nullptr);
  bool IsPossiblyPrimeMillerRabiN(int numTimesToRun = 1, std::stringstream* comments = nullptr);
  bool IsPossiblyPrimeMillerRabinOnce(
    unsigned int theBase,
    int theExponentOfThePowerTwoFactorOfNminusOne,
    const LargeIntegerUnsigned& theOddFactorOfNminusOne,
    std::stringstream* comments
  );
  bool IsEqualToOne() const;
  bool IsGreaterThanOrEqualTo(const LargeIntegerUnsigned& x) const;
  static void getPrimesEratosthenesSieve(
    unsigned int primesUpToInclusive, List<unsigned int>& output
  );
  static void gcd(const LargeIntegerUnsigned& a, const LargeIntegerUnsigned& b, LargeIntegerUnsigned& output);
  static LargeIntegerUnsigned gcd(const LargeIntegerUnsigned& a, const LargeIntegerUnsigned& b);
  static LargeIntegerUnsigned lcm(const LargeIntegerUnsigned& a, const LargeIntegerUnsigned& b);
  static void lcm(const LargeIntegerUnsigned& a, const LargeIntegerUnsigned& b, LargeIntegerUnsigned& output);
  unsigned int hashFunction() const;
  void MultiplyBy(const LargeIntegerUnsigned& right);
  void operator*=(const LargeIntegerUnsigned& right);
  void operator*=(unsigned int x);
  void operator+=(unsigned int x);
  void operator++(int);
  bool IsIntegerFittingInInt(int* whichInt);
  void AssignFactorial(unsigned int x);
  void MultiplyBy(const LargeIntegerUnsigned& x, LargeIntegerUnsigned& output) const;
  void MultiplyByUInt(unsigned int x);
  void AddShiftedUIntSmallerThanCarryOverBound(unsigned int x, int shift);
  void AssignShiftedUInt(unsigned int x, int shift);
  void AssignUnsignedInt(unsigned int x);
  void AssignUInt64(uint64_t x);
  // returns ceiling of the logarithm base two of the number,
  // i.e., the smallest x such that this <= 2^x.
  unsigned int LogarithmBaseNCeiling(unsigned int theBase) const;
  void AccountFactor(const LargeInteger& theP, List<LargeInteger>& outputPrimeFactors, List<int>& outputMultiplicities) const;
  bool FactorReturnFalseIfFactorizationIncomplete(
    List<LargeInteger>& outputFactors,
    List<int>& outputMultiplicites,
    int dontSearchForDivisorsLargerThan,
    std::stringstream *commentsOnFailure
  ) const;
  bool FactorLargeReturnFalseIfFactorizationIncomplete(
    List<LargeInteger>& outputFactors,
    List<int>& outputMultiplicites,
    int dontSearchForDivisorsLargerThan,
    std::stringstream *commentsOnFailure
  ) const;
  void AssignString(const std::string& input);
  bool AssignStringFailureAllowed(const std::string& input, bool ignoreNonDigits);
  int GetUnsignedIntValueTruncated();
  int operator%(unsigned int x);
  void operator=(const LargeIntegerUnsigned& x);
  //void operator=(LargeIntUnsigned&& other);
  void operator=(unsigned int x);
  void operator+=(const LargeIntegerUnsigned& other);
  bool operator==(const LargeIntegerUnsigned& other) const;
  bool operator!=(const LargeIntegerUnsigned& other) const;
  void operator--(int);
  void operator%=(const LargeIntegerUnsigned& other);
  void operator/=(const LargeIntegerUnsigned& other);
  LargeIntegerUnsigned operator+(const LargeIntegerUnsigned& other);
  LargeIntegerUnsigned operator%(const LargeIntegerUnsigned& other) const;
  LargeIntegerUnsigned operator-(const LargeIntegerUnsigned& other) const;
  LargeIntegerUnsigned operator/(unsigned int x) const;
  LargeIntegerUnsigned operator/(const LargeIntegerUnsigned& x) const;
  LargeIntegerUnsigned operator*(const LargeIntegerUnsigned& x) const;
  LargeIntegerUnsigned(unsigned int x);
  LargeIntegerUnsigned(const LargeIntegerUnsigned& x);
  // LargeIntUnsigned(LargeIntUnsigned x);
  LargeIntegerUnsigned();
  // LargeIntUnsigned(unsigned int value){this->operator=(value); }
  // LargeIntUnsigned(unsigned int x) {this->AssignShiftedUInt(x,0);}
  static LargeIntegerUnsigned GetOne();
  bool operator<(int other) const;
  bool operator>(int other) const;
  bool operator<(const LargeIntegerUnsigned& other) const;
  bool operator>=(const LargeIntegerUnsigned& other) const;
  bool operator>(const LargeIntegerUnsigned& other) const;
  void WriteBigEndianBytes(List<unsigned char>& outputAppend, bool leadingZeroPad) const;
  bool WriteBigEndianFixedNumberOfBytes(
    List<unsigned char>& outputAppend,
    int desiredNumberOfBytes,
    std::stringstream* commentsOnFailure
  ) const;
  void GetHexBigEndian(int numberOfLeadingZeroesToPadWith, std::string& output) const;
  //must be rewritten:
  double GetDoubleValue() const;
  void FitSize();
};

class LargeInteger {
  friend class Rational;
  friend LargeInteger operator*(const LargeInteger& left, const LargeInteger& right) {
    LargeInteger tempI;
    tempI = left;
    tempI *= right;
    return tempI;
  }
  friend bool operator<(int left, const LargeInteger& right) {
    LargeInteger leftCopy = left;
    return right > leftCopy;
  }
  friend LargeInteger operator*(const LargeInteger& left, const LargeIntegerUnsigned& right) {
    LargeInteger tempI;
    tempI = right;
    return left * tempI;
  }
  friend std::ostream& operator << (std::ostream& output, const LargeInteger& theLI) {
    output << theLI.toString();
    return output;
  }
public:
  signed char sign;
  LargeIntegerUnsigned value;
  void operator*=(const LargeInteger& x) {
    this->sign *= x.sign;
    this->value.MultiplyBy(x.value);
  }
  void MultiplyByInt(int x);
  void toString(std::string& output) const;
  std::string toString(FormatExpressions* theFormat = nullptr) const {
    (void) theFormat;//avoid unused parameter warning, portable
    std::string tempS;
    this->toString(tempS);
    return tempS;
  }
  bool IsIntegerFittingInInt(int* whichInt);
  bool IsPositive() const {
    return this->sign == 1 && (this->value.IsPositive());
  }
  bool IsNegative() const {
    return this->sign == - 1 && (this->value.IsPositive());
  }
  bool BeginsWithMinus() {
    return this->IsNegative();
  }
  bool IsPositiveOrZero() const {
    return !this->IsNegative();
  }
  bool IsNonPositive() const {
    return !this->IsPositive();
  }
  static LargeInteger zero();
  bool TryToFindWhetherIsPower(bool& outputIsPower, LargeInteger& outputBase, int& outputPower) const;
  bool NeedsParenthesisForMultiplication(FormatExpressions* unused) const {
    (void) unused;
    return false;
  }
  bool operator==(const LargeInteger& x) const;
  bool IsEqualToZero() const {
    return this->value.IsEqualToZero();
  }
  bool IsEven() const;
  bool IsEqualToOne() const {
    return this->value.IsEqualToOne() && this->sign == 1;
  }
  void AssignLargeIntUnsigned(const LargeIntegerUnsigned& x) {
    this->value = x;
    this->sign = 1;
  }
  void AssignInt(int x);
  void AssignInt64(int64_t x);
  void AssignUInt64(uint64_t x);
  void AddLargeIntUnsigned(const LargeIntegerUnsigned& x);
  inline void AddInt(int x) {
    LargeInteger tempInt;
    tempInt.AssignInt(x);
    *this += (tempInt);
  }
  //bool IsGEQ(LargeInt& x);
  bool CheckForConsistensy();
  void WriteToFile(std::fstream& output);
  void AssignString(const std::string& input);
  bool AssignStringFailureAllowed(const std::string& input, std::stringstream* commentsOnFailure);
  void ReadFromFile(std::fstream& input);
  void checkConsistency(){}
  void makeZero();
  bool GetDivisors(List<int>& output, bool includeNegative);
  void makeOne() {
    this->value.makeOne();
    this->sign = 1;
  }
  void MakeMOne() {
    this->value.makeOne();
    this->sign = - 1;
  }
  static unsigned int hashFunction(const LargeInteger& input) {
    return input.hashFunction();
  }
  unsigned int hashFunction() const {
    return this->value.hashFunction() + static_cast<unsigned int>(this->sign) + 3;
  }
  int GetIntValueTruncated() {
   return this->sign * this->value.GetUnsignedIntValueTruncated();
  }
  double GetDoubleValue() const;
  int operator %(int x);
  inline void AssignFloor(){}
  void operator=(const Rational& x);
  void operator=(const LargeInteger& x);
  inline void operator=(int x) {
    this->AssignInt(x);
  }
  inline void operator=(const LargeIntegerUnsigned& other) {
    this->value = other;
    this->sign = 1;
  }
  void operator*=(const LargeIntegerUnsigned& other) {
    if (other.IsEqualToZero()) {
      this->makeZero();
      return;
    }
    this->value *= other;
  }
  inline void operator++(int) {
    *this += 1;
  }
  inline void operator*=(int x) {
    this->MultiplyByInt(x);
  }
  inline void Minus() {
    if (!this->IsEqualToZero()) {
      this->sign *= - 1;
    }
  }
  void operator+=(const LargeInteger& other);
  inline void operator+=(const LargeIntegerUnsigned& other) {
    this->AddLargeIntUnsigned(other);
  }
  inline void operator+=(int x) {
    this->AddInt(x);
  }
  inline bool operator!=(const LargeInteger& other) const {
    return !(*this == other);
  }
  inline void operator-=(const LargeInteger& other) {
    this->Minus();
    *this += (other);
    this->Minus();
  }
  inline bool operator<=(const LargeInteger& other) const {
    return !(other<*this);
  }
  inline bool operator<(const LargeInteger& other) const {
    if (other.IsPositiveOrZero()) {
      if (this->IsPositiveOrZero()) {
        return this->value<other.value;
      }
      return true;
    }
    if (this->IsNegative()) {
      return other.value < this->value;
    }
    return false;
  }
  inline LargeInteger operator+(const LargeInteger& other) const {
    LargeInteger tempInt;
    tempInt = *this;
    tempInt += other;
    return tempInt;
  }
  inline LargeInteger operator-(const LargeInteger& other) const {
    LargeInteger result = *this;
    result -= (other);
    return result;
  }
  inline LargeInteger operator+(int x) const {
    LargeInteger result = *this;
    result.AddInt(x);
    return result;
  }
  LargeInteger operator*(int x) {
    LargeInteger result = *this;
    result.MultiplyByInt(x);
    return result;
  }
  LargeInteger operator/(int x) const;
  LargeInteger operator/(LargeInteger& x) const;
  void operator/=(const LargeInteger& other) {
    if (this->IsEqualToZero()) {
      return;
    }
    this->sign*= other.sign;
    LargeIntegerUnsigned quotient, remainder;
    this->value.DivPositive(other.value, quotient, remainder);
    this->value =quotient;
  }
  inline bool operator>(const LargeInteger& other) const {
    return other < *this;
  }
  void operator%=(const LargeInteger& other) {
    if (this->IsEqualToZero()) {
      return;
    }
    LargeIntegerUnsigned quotient, remainder;
    this->value.DivPositive(other.value, quotient, remainder);
    this->value = remainder;
    if (this->IsNegative()) {
      *this += other.value;
    }
  }
  void RaiseToPower(int thePower);
  LargeInteger operator%(const LargeInteger& other) const {
    LargeInteger result = *this;
    result %= other;
    return result;
  }
  LargeInteger(const LargeInteger& x) {
    this->operator=(x);
  }
  LargeInteger(int x) {
    this->AssignInt(x);
  }
  LargeInteger(int64_t x) {
    this->AssignInt64(x);
  }
  LargeInteger(uint64_t x) {
    this->AssignUInt64(x);
  }
  LargeInteger(const LargeIntegerUnsigned& other) : sign(1), value(other) {
  }
  LargeInteger(): sign(1) {}
};

Rational operator-(const Rational& argument);
Rational operator/(int left, const Rational& right);

class Rational {
private:
//The following doesn't compile, when it should.
//  friend int operator=(int& left, const Rational& right)
//  { if (!right.IsSmallInteger(&left))
//      global.fatal << "This is a programming error. I am asked to assign a rational number to a small integer, but the rational "
//      << " number is either too large or is not an integer. Namely, the rational number equals " << this->toString()
//      << ". The programmer is supposed to write something of the sort int = rational only on condition that "
//      << " the programmer is sure that the rational contains is a small int. "
//      << global.fatal;
//    return left;
//  }
  struct LargeRationalExtended {
  public:
    LargeInteger num;
    LargeIntegerUnsigned den;
  };
  friend Rational operator-(const Rational& argument);
  friend Rational operator/(int left, const Rational& right);
  friend std::ostream& operator << (std::ostream& output, const Rational& theRat) {
    output << theRat.toString();
    return output;
  }
  friend Rational operator*(int left, const Rational& right) {
    return Rational (left) * right;
  }
  friend bool operator<(int left, const Rational& right) {
    return Rational(left) < right;
  }
  bool TryToAddQuickly(int OtherNum, int OtherDen);
  bool TryToMultiplyQuickly(int OtherNum, int OtherDen);
  void AllocateExtended() {
    if (this->Extended != nullptr) {
      return;
    }
    this->Extended = new LargeRationalExtended;
#ifdef AllocationLimitsSafeguard
  ParallelComputing::GlobalPointerCounter ++;
  ParallelComputing::CheckPointerCounters();
#endif
  }
  bool InitExtendedFromShortIfNeeded() {
    if (this->Extended != nullptr) {
      return false;
    }
    this->Extended = new LargeRationalExtended;
#ifdef AllocationLimitsSafeguard
  ParallelComputing::GlobalPointerCounter ++;
  ParallelComputing::CheckPointerCounters();
#endif
    this->Extended->den.AssignShiftedUInt( static_cast<unsigned int>(this->DenShort), 0);
    this->Extended->num.AssignInt(this->NumShort);
    return true;
  }
  inline void FreeExtended() {
    if (this->Extended == nullptr) {
      return;
    }
    delete this->Extended;
    this->Extended = nullptr;
#ifdef AllocationLimitsSafeguard
  ParallelComputing::GlobalPointerCounter ++;
  ParallelComputing::CheckPointerCounters();
#endif
  }
  bool ShrinkExtendedPartIfPossible();
public:
  int NumShort;
  //the requirement that the below be unsigned caused a huge problem, so I
  //changed it back to int. Grrrrr.
  int DenShort;
  LargeRationalExtended *Extended;
  static unsigned long long int TotalSmallAdditions;
  static unsigned long long int TotalLargeAdditions;
  static unsigned long long int TotalSmallMultiplications;
  static unsigned long long int TotalLargeMultiplications;
  static unsigned long long int TotalSmallGCDcalls;
  static unsigned long long int TotalLargeGCDcalls;
  bool NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost() const {
    return this->IsNegative();
  }
  bool NeedsParenthesisForMultiplication(FormatExpressions* unused) const {
    (void) unused;
    return false;
    //return this->IsNegative();
  }
  bool GetSquareRootIfRational(Rational& output) const;
  // Scales a vector of rationals so as to make all coordinates
  // relatively prime integers such that the non-zero coefficient is positive.
  // Returns the number by which the vector was multiplied.
  static Rational scaleNormalizeIndex(List<Rational>& output, int indexNonZeroEntry);
  LargeIntegerUnsigned GetDenominator() const;
  bool BeginsWithMinus();
  LargeInteger GetNumerator() const;
  bool GetPrimeFactorsAbsoluteValue(
    List<LargeInteger>& numeratorPrimeFactors,
    List<int>& numeratorMultiplicities,
    List<LargeInteger>& denominatorPrimeFactors,
    List<int>& denominatorMultiplicities
  );
  const Rational& GetComplexConjugate() const;
  Rational RationalValue() {
    return *this;
  }
  void Subtract(const Rational& r);
  void AdD(const Rational& r) {
    this->operator+=(r);
  }
  void AddInteger(int x);
  void AssignLargeIntUnsigned(const LargeIntegerUnsigned& other);
  void AssignLargeInteger(const LargeInteger& other);
  void AssignString(const std::string& input);
  bool AssignStringFailureAllowed(const std::string& input);
  static Rational zero() {
    return 0;
  }
  static Rational one() {
    return 1;
  }
  void AssignFracValue();
  void AssignFloor() {
    Rational tempRat = *this;
    tempRat.AssignFracValue();
    *this -= tempRat;
  }
  void MultiplyBy(const Rational& r);
  // The Hash function of zero must be equal to zero.
  // See Note on Hashes before the definition of SomeRandomPrimes;
  unsigned int hashFunction() const {
    if (this->Extended == nullptr) {
      if (this->NumShort == 0) {
        return 0;
      }
      return static_cast<unsigned int>(this->NumShort) * SomeRandomPrimes[0] + static_cast<unsigned int>(this->DenShort) * ::SomeRandomPrimes[1];
    }
    return this->Extended->num.hashFunction() * SomeRandomPrimes[0] +
    this->Extended->den.hashFunction() * SomeRandomPrimes[1];
  }
  static inline unsigned int hashFunction(const Rational& input) {
    return input.hashFunction();
  }
  //void MultiplyByLargeRational(int num, int den);
  void MultiplyByInt(int x);
  void MultiplyByLargeInt(LargeInteger& x);
  void MultiplyByLargeIntUnsigned(LargeIntegerUnsigned& x);
  void Assign(const Rational& r);
  void AssignInteger(int x);
  bool IsInteger(LargeInteger* whichInteger = nullptr) const;
  bool IsIntegerFittingInInt(int* whichInt) const {
    LargeInteger theInt;
    if (!this->IsInteger(&theInt)) {
      return false;
    }
    return theInt.IsIntegerFittingInInt(whichInt);
  }
  bool IsSmallInteger(int* whichInteger = nullptr) const {
    if (this->Extended != nullptr) {
      return false;
    }
    if (this->DenShort != 1) {
      return false;
    }
    if (whichInteger != nullptr) {
      *whichInteger = this->NumShort;
    }
    return true;
  }
  bool IsGreaterThan(const Rational& r) const;
  inline void AssignNumeratorAndDenominator(int n, int d) {
    if (d < 0) {
      d = - d;
      n = - n;
    }
    this->NumShort = n;
    this->DenShort = d;
    this->FreeExtended();
    this->simplify();
  }
  void DivideBy(const Rational& r);
  void DivideByInteger(int x) {
    int tempDen; signed char tempSign;
    if (x < 0) {
      tempDen = - x;
      tempSign = - 1;
    } else {
      tempDen = x;
      tempSign = 1;
    }
    if (this->TryToMultiplyQuickly(tempSign, tempDen)) {
      return;
    }
    this->InitExtendedFromShortIfNeeded();
    this->Extended->den.MultiplyByUInt(static_cast<unsigned int>(tempDen));
    this->Extended->num.sign *= tempSign;
    this->simplify();
  }
  void DivideByLargeInteger(LargeInteger& x) {
    this->InitExtendedFromShortIfNeeded();
    this->Extended->den.MultiplyBy(x.value);
    this->Extended->num.sign *= x.sign;
    this->simplify();
  }
  void DivideByLargeIntegerUnsigned(LargeIntegerUnsigned& x) {
    this->InitExtendedFromShortIfNeeded();
    this->Extended->den.MultiplyBy(x);
    this->simplify();
  }
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringFrac() const;
  std::string ToStringForFileOperations(FormatExpressions* notUsed = nullptr) const;
  bool IsEqualTo(const Rational& r) const;
  bool IsGreaterThanOrEqualTo(const Rational& right) const;
  bool IsEven() const {
    Rational tempRat = *this;
    tempRat /= 2;
    return tempRat.IsInteger();
  }
  inline bool IsEqualToOne() const {
    if (this->Extended == nullptr) {
      return (this->NumShort == 1 && this->DenShort == 1);
    } else {
      return (this->Extended->num.IsEqualToOne() && this->Extended->den.IsEqualToOne());
    }
  }
  inline bool IsEqualToZero() const {
    if (this->Extended == nullptr) {
      return this->NumShort == 0;
    } else {
      return this->Extended->num.IsEqualToZero();
    }
  }
  inline bool operator<=(const Rational& other) const {
    return !(other < *this);
  }
  inline bool IsPositiveOrZero() const {
    if (this->Extended == nullptr) {
      return this->NumShort >= 0;
    } else {
      return this->Extended->num.IsPositiveOrZero();
    }
  }
  bool IsNegative() const {
    if (this->Extended == nullptr) {
      return this->NumShort < 0;
    } else {
      return this->Extended->num.IsNegative();
    }
  }
  bool IsNonPositive() const {
    if (this->Extended == nullptr) {
      return this->NumShort <= 0;
    } else {
      return this->Extended->num.IsNonPositive();
    }
  }
  bool IsPositive() const {
    if (this->Extended == nullptr) {
      return this->NumShort > 0;
    } else {
      return this->Extended->num.IsPositive();
    }
  }
  void simplify();
  void invert();
  void Minus() {
    if (this->Extended == nullptr) {
      this->NumShort *= - 1;
    } else {
      this->Extended->num.sign *= - 1;
    }
  }
  double GetDoubleValue() const;
  int floorIfSmall();
  void makeZero() {
    this->NumShort = 0;
    this->DenShort = 1;
    this->FreeExtended();
  }
  void makeOne() {
    this->NumShort = 1;
    this->DenShort = 1;
    this->FreeExtended();
  }
  void MakeMOne() {
    this->NumShort = - 1;
    this->DenShort = 1;
    this->FreeExtended();
  }
  void WriteToFile(std::fstream& output);
  void ReadFromFile(std::istream& input);
  void DrawElement(DrawElementInputOutput& theDrawData);
  inline void AssignAbsoluteValue() {
    if (this->IsNegative()) {
      this->Minus();
    }
  }
  static long long int TotalAdditions() {
    return static_cast<long long int>(Rational::TotalLargeAdditions + Rational::TotalSmallAdditions);
  }
  static long long int TotalMultiplications() {
    return static_cast<long long int>(Rational::TotalLargeMultiplications + Rational::TotalSmallMultiplications);
  }
  static long long int TotalArithmeticOperations() {
    return Rational::TotalAdditions() + Rational::TotalMultiplications();
  }
  static Rational NChooseK(const Rational& n, int k);
  static Rational Factorial(int n);
  static Rational TwoToTheNth(int n);
  static Rational NtoTheKth(int n, int k);
  void RaiseToPower(int x);
  //don't ever call the below manually or you can get memory leak (extended must be nullified here by
  //default!
  Rational(int n, int d) {
    this->Extended = nullptr;
    this->AssignNumeratorAndDenominator(n, d);
    ParallelComputing::SafePointDontCallMeFromDestructors();
  }
  Rational(const LargeInteger& other) {
    this->Extended = nullptr;
    *this = other;
  }
  Rational(const LargeIntegerUnsigned& other) {
    this->Extended = nullptr;
    *this = other;
  }
  Rational(): NumShort(0), DenShort(0), Extended(nullptr) {
    ParallelComputing::SafePointDontCallMeFromDestructors();
  }
  Rational(int n) {
    this->Extended = nullptr;
    this->AssignNumeratorAndDenominator(n, 1);
    ParallelComputing::SafePointDontCallMeFromDestructors();
  }
  Rational(const Rational& right) {
    this->Extended = nullptr;
    this->Assign(right);
    ParallelComputing::SafePointDontCallMeFromDestructors();
  }
  Rational(const std::string& input): NumShort(0), DenShort(0), Extended(nullptr) {
    this->AssignString(input);
  }
  ~Rational() {
    this->FreeExtended();
  }
  //the below must be called only with positive arguments!
  static inline int gcd(int a, int b) {
    MacroIncrementCounter(Rational::TotalSmallGCDcalls);
    return MathRoutines::gcd(a, b);
  }
  static int gcdSigned(int a, int b) {
    if (a < 0) {
      a *= - 1;
    }
    if (b < 0) {
      b *= - 1;
    }
    return Rational::gcd(a, b);
  }
  inline bool CheckForElementSanity() {
    return this->Extended == nullptr;
  }
  inline bool checkConsistency() {
    return this->CheckConsistency();
  }
  inline bool CheckConsistency() {
    if (this->Extended == nullptr) {
      return this->DenShort > 0;
    }
    return true;
  }
  void operator=(const AlgebraicNumber& other);
  void operator=(const LargeIntegerUnsigned& right) {
    LargeInteger tempI;
    tempI = right;
    this->operator=(tempI);
  }
  void operator=(const Rational& right) {
    this->Assign(right);
  }
  void operator=(const Polynomial<Rational>& other);
  bool operator==(const int other) const {
    if (other == 0) {
      return this->IsEqualToZero();
    }
    return this->IsEqualTo(other);
  }
  inline bool operator==(const Rational& right) const {
    return this->IsEqualTo(right);
  }
  inline void operator+=(const Rational& r) {
    //static std::string tempS1, tempS2, tempS3, tempS4, tempS5, tempS6, tempS7;
    if (r.Extended == nullptr && this->Extended == nullptr) {
      if (this->TryToAddQuickly(r.NumShort, r.DenShort)) {
        return;
      }
    }
    if (this == &r) {
      this->MultiplyByInt(2);
      return;
    }
    MacroIncrementCounter(Rational::TotalLargeAdditions);
    this->InitExtendedFromShortIfNeeded();
    Rational tempRat;
    tempRat.Assign(r);
    tempRat.InitExtendedFromShortIfNeeded();
    LargeInteger tempI;
    tempI = tempRat.Extended->num;
    tempI.value.MultiplyBy(this->Extended->den);
    this->Extended->num.value.MultiplyBy(tempRat.Extended->den);
    this->Extended->num += (tempI);
    this->Extended->den.MultiplyBy(tempRat.Extended->den);
    this->simplify();
  }
  inline void operator-=(const Rational& right) {
    this->Subtract(right);
  }
  inline void operator*=(const Rational& right) {
    this->MultiplyBy(right);
  }
  void operator/=(const Rational& right) {
    this->DivideBy(right);
  }
  void operator+=(int right) {
    this->AddInteger(right);
  }
  void operator-=(int right) {
    Rational tempRat = right;
    this->Subtract(tempRat);
  }
  void operator=(int right) {
    this->AssignInteger(right);
  }
  inline void operator=(const LargeInteger& other) {
    this->AssignLargeInteger(other);
  }
  Rational operator*(const Rational& right) const;
  void operator++(int) {
    *this += 1;
  }
  Rational operator*(int right) const {
    Rational tempRat;
    tempRat.Assign(*this);
    tempRat.MultiplyByInt(right);
    return tempRat;
  }
  Rational operator/(int right) const {
    Rational tempRat;
    tempRat.Assign(*this);
    tempRat.DivideByInteger(right);
    return tempRat;
  }
  Vector<Rational> operator*(const Vector<Rational> & right) const;
  Rational operator+(const Rational& right) const;
  Rational operator-(const Rational& right) const;
  Rational operator/(const Rational& right) const;
  inline bool operator!=(const Rational& right) const {
    return !this->IsEqualTo(right);
  }
  inline bool operator!=(const int& right) const {
    return !((*this) == right);
  }
  inline bool operator>(const Rational& right) const {
    return this->IsGreaterThan(right);
  }
  inline bool operator<(const Rational& right) const {
    return right.IsGreaterThan(*this);
  }
  inline bool operator>(const int right) const {
    Rational tempRat;
    tempRat.AssignInteger(right);
    return this->IsGreaterThan(tempRat);
  }
  inline bool operator>=(const Rational& right) const {
    return this->IsGreaterThanOrEqualTo(right);
  }
  inline bool operator<(const int right) const {
    Rational tempRat;
    tempRat.AssignInteger(right);
    return tempRat.IsGreaterThan(*this);
  }
  class Test {
  public:
    static bool All();
    static bool TestScale();
  };
};
#endif
