//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader2Math_01LargeIntegersInstance_already_included
#define vpfHeader2Math_01LargeIntegersInstance_already_included

#include "vpfHeader1General0_General.h"
static ProjectInformationInstance vpfHeader2Math_01LargeIntegersInstance(
  __FILE__, "Header, large integer/rational number arithmetic. "
);

class LargeIntUnsigned {
  void AddNoFitSize(const LargeIntUnsigned& x);
public:
  //The zero element is assumed to have length one array with a zero entry.
  //
  //CarryOverBound is the "base" over which we work.
  //Requirements on the CarryOverBound:
  //1.  +/-(CarryOverBound*2)- 1 must fit inside an (int)
  //    on the system
  //2. (CarryOverBound*2)^2- 1 must fit inside (long long)
  //    on the system.
  ////////////////////////////////////////////////////////
  //On a 32 bit machine any number smaller than or equal to 2^30 will work.
  //If you got no clue what to put just leave CarryOverBound as it is below.
  List<int32_t> theDigits;
  //static const int CarryOverBound =10; //<-for extreme "corner case" testing
  static const int CarryOverBound = 1000000000;
  //the above choice of CarryOverBound facilitates very quick conversions of Large integers into decimal, with
  //relatively small loss of speed and RAM memory.
//  static const unsigned int CarryOverBound =2147483648UL; //=2^31
  //The following must be less than or equal to the square root of CarryOverBound.
  //It is used for quick multiplication of Rational-s.
  //static const int SquareRootOfCarryOverBound =3;//<-for extreme "corner case" testing
  static const int SquareRootOfCarryOverBound = 31000; //31000*31000=961000000<1000000000
//  static const int SquareRootOfCarryOverBound =32768; //=2^15
  friend bool operator<(int left, const LargeIntUnsigned& right) {
    return right > left;
  }
  friend std::ostream& operator << (std::ostream& output, const LargeIntUnsigned& theLIU) {
    output << theLIU.ToString();
    return output;
  }
  void PadWithZeroesToAtLeastNDigits(int desiredMinNumDigits);
  void AddLargeIntUnsignedShiftedTimesDigit(const LargeIntUnsigned& other, int digitShift, int theConst);
  void SubtractSmallerPositive(const LargeIntUnsigned& x);
  void ToString(std::string& output) const;
  void ElementToStringLargeElementDecimal(std::string& output) const;
  std::string ToString(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringAbbreviate(FormatExpressions* theFormat = nullptr) const;
  void DivPositive(const LargeIntUnsigned& divisor, LargeIntUnsigned& quotientOutput, LargeIntUnsigned& remainderOutput) const;
  void MakeOne();
  void AddUInt(unsigned int x);
  void MakeZero();
  bool IsEqualToZero() const;
  bool IsEven() const;
  bool IsPositive() const;
  bool TryToFindWhetherIsPower(bool& outputIsPower, LargeInt& outputBase, int& outputPower) const;
  bool IsPossiblyPrimeMillerRabin(int numTimesToRun = 1, std::stringstream* comments = 0);
  bool IsPossiblyPrimeMillerRabinOnce(
    unsigned int theBase,
    int theExponentOfThePowerTwoFactorOfNminusOne,
    const LargeIntUnsigned& theOddFactorOfNminusOne,
    std::stringstream* comments
  );
  bool IsEqualToOne() const;
  bool IsGEQ(const LargeIntUnsigned& x) const;
  static void GetAllPrimesSmallerThanOrEqualToUseEratosthenesSieve(unsigned int n, List<unsigned int>& output);
  static void gcd(const LargeIntUnsigned& a, const LargeIntUnsigned& b, LargeIntUnsigned& output);
  static LargeIntUnsigned gcd(const LargeIntUnsigned& a, const LargeIntUnsigned& b);
  static LargeIntUnsigned lcm(const LargeIntUnsigned& a, const LargeIntUnsigned& b);
  static void lcm(const LargeIntUnsigned& a, const LargeIntUnsigned& b, LargeIntUnsigned& output);
  unsigned int HashFunction() const;
  void MultiplyBy(const LargeIntUnsigned& right);
  void operator*=(const LargeIntUnsigned& right);
  void operator*=(unsigned int x);
  void operator+=(unsigned int x);
  void operator++(int);
  bool IsIntegerFittingInInt(int* whichInt);
  void AssignFactorial(unsigned int x);
  void MultiplyBy(const LargeIntUnsigned& x, LargeIntUnsigned& output) const;
  void MultiplyByUInt(unsigned int x);
  void AddShiftedUIntSmallerThanCarryOverBound(unsigned int x, int shift);
  void AssignShiftedUInt(unsigned int x, int shift);
  void AccountFactor(const LargeInt& theP, List<LargeInt>& outputPrimeFactors, List<int>& outputMultiplicities) const;
  bool FactorReturnFalseIfFactorizationIncomplete(
    List<LargeInt>& outputFactors,
    List<int>& outputMultiplicites,
    int dontSearchForDivisorsLargerThan,
    std::stringstream *commentsOnFailure
  ) const;
  bool FactorLargeReturnFalseIfFactorizationIncomplete(
    List<LargeInt>& outputFactors,
    List<int>& outputMultiplicites,
    int dontSearchForDivisorsLargerThan,
    std::stringstream *commentsOnFailure
  ) const;
  void AssignString(const std::string& input);
  bool AssignStringFailureAllowed(const std::string& input, bool ignoreNonDigits);
  int GetUnsignedIntValueTruncated();
  int operator%(unsigned int x);
  void operator=(const LargeIntUnsigned& x);
  //void operator=(LargeIntUnsigned&& other);
  void operator=(unsigned int x);
  void operator+=(const LargeIntUnsigned& other);
  bool operator==(const LargeIntUnsigned& other) const;
  bool operator!=(const LargeIntUnsigned& other) const;
  void operator--(int);
  void operator%=(const LargeIntUnsigned& other);
  void operator/=(const LargeIntUnsigned& other);
  LargeIntUnsigned operator+(const LargeIntUnsigned& other);
  LargeIntUnsigned operator%(const LargeIntUnsigned& other) const;
  LargeIntUnsigned operator-(const LargeIntUnsigned& other) const;
  LargeIntUnsigned operator/(unsigned int x) const;
  LargeIntUnsigned operator/(const LargeIntUnsigned& x) const;
  LargeIntUnsigned operator*(const LargeIntUnsigned& x) const;
  LargeIntUnsigned(unsigned int x);
  LargeIntUnsigned(const LargeIntUnsigned& x);
  //  LargeIntUnsigned(LargeIntUnsigned x);
  LargeIntUnsigned();
  //  LargeIntUnsigned(unsigned int value){this->operator=(value); }
  //  LargeIntUnsigned(unsigned int x) {this->AssignShiftedUInt(x,0);}
  static LargeIntUnsigned GetOne();
  bool operator<(int other) const;
  bool operator>(int other) const;
  bool operator<(const LargeIntUnsigned& other) const;
  bool operator>=(const LargeIntUnsigned& other) const;
  bool operator>(const LargeIntUnsigned& other) const;
  //must be rewritten:
  double GetDoubleValue() const;
  void FitSize();
};

class LargeInt {
  friend class Rational;
  friend LargeInt operator*(const LargeInt& left, const LargeInt& right) {
    LargeInt tempI;
    tempI = left;
    tempI *= right;
    return tempI;
  }
  friend bool operator<(int left, const LargeInt& right) {
    LargeInt leftCopy = left;
    return right > leftCopy;
  }
  friend LargeInt operator*(const LargeInt& left, const LargeIntUnsigned& right) {
    LargeInt tempI;
    tempI = right;
    return left * tempI;
  }
  friend std::ostream& operator << (std::ostream& output, const LargeInt& theLI) {
    output << theLI.ToString();
    return output;
  }
public:
  inline static const std::string GetXMLClassName() {
    return "LInt";
  }
  signed char sign;
  LargeIntUnsigned value;
  void operator*=(const LargeInt& x) {
    this->sign *= x.sign;
    this->value.MultiplyBy(x.value);
  }
  void MultiplyByInt(int x);
  void ToString(std::string& output) const;
  std::string ToString(FormatExpressions* theFormat = nullptr) const {
    (void) theFormat;//avoid unused parameter warning, portable
    std::string tempS;
    this->ToString(tempS);
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
  bool TryToFindWhetherIsPower(bool& outputIsPower, LargeInt& outputBase, int& outputPower) const;
  bool NeedsParenthesisForMultiplication() const {
    return false;
  }
  bool operator==(const LargeInt& x) const;
  bool IsEqualToZero() const {
    return this->value.IsEqualToZero();
  }
  bool IsEven() const;
  bool IsEqualToOne() const {
    return this->value.IsEqualToOne() && this->sign == 1;
  }
  void AssignLargeIntUnsigned(const LargeIntUnsigned& x) {
    this->value = x;
    this->sign = 1;
  }
  void AssignInt(int x);
  void AddLargeIntUnsigned(const LargeIntUnsigned& x);
  inline void AddInt(int x) {
    LargeInt tempInt;
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
  void MakeZero();
  bool GetDivisors(List<int>& output, bool includeNegative);
  void MakeOne() {
    this->value.MakeOne();
    this->sign = 1;
  }
  void MakeMOne() {
    this->value.MakeOne();
    this->sign = - 1;
  }
  static unsigned int HashFunction(const LargeInt& input) {
    return input.HashFunction();
  }
  unsigned int HashFunction() const {
    return this->value.HashFunction() + this->sign + 3;
  }
  int GetIntValueTruncated() {
   return this->sign * this->value.GetUnsignedIntValueTruncated();
  }
  double GetDoubleValue() const;
  int operator %(int x);
  inline void AssignFloor(){}
  void operator=(const Rational& x);
  void operator=(const LargeInt& x);
  inline void operator=(int x) {
    this->AssignInt(x);
  }
  inline void operator=(const LargeIntUnsigned& other) {
    this->value = other;
    this->sign = 1;
  }
  void operator*=(const LargeIntUnsigned& other) {
    if (other.IsEqualToZero()) {
      this->MakeZero();
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
  void operator+=(const LargeInt& other);
  inline void operator+=(const LargeIntUnsigned& other) {
    this->AddLargeIntUnsigned(other);
  }
  inline void operator+=(int x) {
    this->AddInt(x);
  }
  inline bool operator!=(const LargeInt& other) const {
    return !(*this == other);
  }
  inline void operator-=(const LargeInt& other) {
    this->Minus();
    *this += (other);
    this->Minus();
  }
  inline bool operator<=(const LargeInt& other) const {
    return !(other<*this);
  }
  inline bool operator<(const LargeInt& other) const {
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
  inline LargeInt operator+(const LargeInt& other) const {
    LargeInt tempInt;
    tempInt = *this;
    tempInt += other;
    return tempInt;
  }
  inline LargeInt operator-(const LargeInt& other) const {
    LargeInt result = *this;
    result -= (other);
    return result;
  }
  inline LargeInt operator+(int x) const {
    LargeInt result = *this;
    result.AddInt(x);
    return result;
  }
  LargeInt operator*(int x) {
    LargeInt result = *this;
    result.MultiplyByInt(x);
    return result;
  }
  LargeInt operator/(int x) const;
  LargeInt operator/(LargeInt& x) const;
  void operator/=(const LargeInt& other) {
    if (this->IsEqualToZero()) {
      return;
    }
    this->sign*= other.sign;
    LargeIntUnsigned quotient, remainder;
    this->value.DivPositive(other.value, quotient, remainder);
    this->value =quotient;
  }
  inline bool operator>(const LargeInt& other) const {
    return other < *this;
  }
  void operator%=(const LargeInt& other) {
    if (this->IsEqualToZero()) {
      return;
    }
    LargeIntUnsigned quotient, remainder;
    this->value.DivPositive(other.value, quotient, remainder);
    this->value = remainder;
    if (this->IsNegative()) {
      *this += other.value;
    }
  }
  void RaiseToPower(int thePower) {
    MathRoutines::RaiseToPower(*this, thePower, (LargeInt) 1);
  }
  LargeInt operator%(const LargeInt& other) const {
    LargeInt result = *this;
    result %= other;
    return result;
  }
  LargeInt(const LargeInt& x) {
    this->operator=(x);
  }
  LargeInt(int x) {
    this->AssignInt(x);
  }
  LargeInt(const LargeIntUnsigned& other) : sign(1), value(other) {
  }
  LargeInt(): sign(1) {}
};

Rational operator-(const Rational& argument);
Rational operator/(int left, const Rational& right);

class Rational {
private:
//The following doesn't compile, when it should.
//  friend int operator=(int& left, const Rational& right)
//  { if (!right.IsSmallInteger(&left))
//      crash << "This is a programming error. I am asked to assign a rational number to a small integer, but the rational "
//      << " number is either too large or is not an integer. Namely, the rational number equals " << this->ToString()
//      << ". The programmer is supposed to write something of the sort int = rational only on condition that "
//      << " the programmer is sure that the rational contains is a small int. "
//      << crash;
//    return left;
//  }
  struct LargeRationalExtended {
  public:
    LargeInt num;
    LargeIntUnsigned den;
  };
  friend Rational operator-(const Rational& argument);
  friend Rational operator/(int left, const Rational& right);
  friend std::ostream& operator << (std::ostream& output, const Rational& theRat) {
    output << theRat.ToString();
    return output;
  }
  friend std::istream& operator >> (std::istream& input, const Rational& output) {
    (void) output;
    crash << "This line of code should not be running. " << crash;
    return input;
  }
  friend Rational operator*(int left, const Rational& right) {
    return (Rational) left * right;
  }
  friend bool operator<(int left, const Rational& right) {
    return (Rational) left < right;
  }
  inline bool TryToAddQuickly(int OtherNum, int OtherDen) {
    register int OtherNumAbs, thisNumAbs;
    if (this->DenShort <= 0 || OtherDen <= 0) {
      crash << "This is a programming error: trying to add corrupt rational number(s) with denominators "
      << this->DenShort << " and " << OtherDen
      << ". The cause of the error should be in some of the calling functions. " << crash;
    }
    if (OtherNum < 0) {
      OtherNumAbs = - OtherNum;
    } else {
      OtherNumAbs = OtherNum;
    }
    if (this->NumShort < 0) {
      thisNumAbs = - this->NumShort;
    } else {
      thisNumAbs = this->NumShort;
    }
    if (
      this->Extended != 0 ||
      thisNumAbs >= LargeIntUnsigned::SquareRootOfCarryOverBound ||
      this->DenShort >= LargeIntUnsigned::SquareRootOfCarryOverBound ||
      OtherNumAbs >= LargeIntUnsigned::SquareRootOfCarryOverBound ||
      OtherDen >= LargeIntUnsigned::SquareRootOfCarryOverBound
    ) {
      return false;
    }
    register int N = this->NumShort * OtherDen + this->DenShort * OtherNum;
    register int D = this->DenShort * OtherDen;
    if (N == 0) {
      this->NumShort = 0;
      this->DenShort = 1;
      MacroIncrementCounter(Rational::TotalSmallAdditions);
      return true;
    }
    register int tempGCD;
    if (N > 0) {
      tempGCD = Rational::gcd(N, D);
    } else {
      tempGCD = Rational::gcd(- N, D);
    }
    this->NumShort = N / tempGCD;
    this->DenShort = D / tempGCD;
    MacroIncrementCounter(Rational::TotalSmallAdditions);
    return true;
  }
  inline bool TryToMultiplyQuickly(int OtherNum, int OtherDen) {
    register int OtherNumAbs, thisNumAbs;
    if (this->DenShort <= 0 || OtherDen <= 0) {
      if (DenShort == 0 || OtherDen == 0) {
        crash << "This is a programming error: division by zero. ";
      } else {
        crash << "This is a programming error during rational number multiplication: "
        << "corrupt rational number denominator. ";
      }
      crash << crash;
    }
    if (OtherNum < 0) {
      OtherNumAbs = - OtherNum;
    } else {
      OtherNumAbs = OtherNum;
    }
    if (this->NumShort < 0) {
      thisNumAbs = - this->NumShort;
    } else {
      thisNumAbs = this->NumShort;
    }
    if (
      this->Extended != 0 ||
      thisNumAbs >= LargeIntUnsigned::SquareRootOfCarryOverBound ||
      this->DenShort >= LargeIntUnsigned::SquareRootOfCarryOverBound ||
      OtherNumAbs >= LargeIntUnsigned::SquareRootOfCarryOverBound ||
      OtherDen >= LargeIntUnsigned::SquareRootOfCarryOverBound
    ) {
      return false;
    }
    register int N = this->NumShort * OtherNum;
    register int D = this->DenShort * OtherDen;
    if (N == 0) {
      this->NumShort = 0;
      this->DenShort = 1;
    } else {
      register int tempGCD;
      if (N > 0) {
        tempGCD = Rational::gcd(N, D);
      } else {
        tempGCD = Rational::gcd(- N, D);
      }
      this->NumShort = N / ((signed int) tempGCD);
      this->DenShort = D / tempGCD;
    }
    MacroIncrementCounter(Rational::TotalSmallMultiplications);
    return true;
  }
  void AllocateExtended() {
    if (this->Extended != 0) {
      return;
    }
    this->Extended = new LargeRationalExtended;
#ifdef AllocationLimitsSafeguard
  ParallelComputing::GlobalPointerCounter ++;
  ParallelComputing::CheckPointerCounters();
#endif
  }
  bool InitExtendedFromShortIfNeeded() {
    if (this->Extended != 0) {
      return false;
    }
    this->Extended = new LargeRationalExtended;
#ifdef AllocationLimitsSafeguard
  ParallelComputing::GlobalPointerCounter ++;
  ParallelComputing::CheckPointerCounters();
#endif
    this->Extended->den.AssignShiftedUInt((unsigned int)this->DenShort, 0);
    this->Extended->num.AssignInt(this->NumShort);
    return true;
  }
  inline void FreeExtended() {
    if (this->Extended == 0) {
      return;
    }
    delete this->Extended;
    this->Extended = 0;
#ifdef AllocationLimitsSafeguard
  ParallelComputing::GlobalPointerCounter ++;
  ParallelComputing::CheckPointerCounters();
#endif
  }
  bool ShrinkExtendedPartIfPossible();
public:
  inline static std::string GetXMLClassName(){
    return "Rational";
  }
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
/*  operator int() const {
    int result = 0;
    if (!this->IsSmallInteger(&result))
      crash << "This is a programming error. I am asked to assign a rational number to a small integer, but the rational "
      << " number is either too large or is not an integer. Namely, the rational number equals " << this->ToString()
      << ". The programmer is supposed to write something of the sort int = rational only on condition that "
      << " the programmer is sure that the rational contains is a small int. "
      << crash;
    return result;
  }*/
  bool NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost() const {
    return this->IsNegative();
  }
  bool NeedsParenthesisForMultiplication() const {
    return false;
    //return this->IsNegative();
  }
  bool GetSquareRootIfRational(Rational& output) const;
  inline Rational GetDenominatorRationalPart() {
    return this->GetDenominator();
  }
  Rational GetNumeratorRationalPart() {
    return this->GetNumerator();
  }
  inline LargeIntUnsigned GetDenominator() const {
    LargeIntUnsigned result;
    if (this->Extended == 0) {
      unsigned int tempI = (unsigned int) this->DenShort;
      result.AssignShiftedUInt(tempI, 0);
    } else {
      result = (this->Extended->den);
    }
    return result;
  }
  bool BeginsWithMinus() {
    return this->IsNegative();
  }
  LargeInt GetNumerator() const {
    LargeInt result;
    if (this->Extended == 0) {
      if (this->NumShort < 0) {
        result.value.AssignShiftedUInt((unsigned int)(- this->NumShort), 0);
      } else {
        result.value.AssignShiftedUInt((unsigned int) this->NumShort, 0);
      }
    } else {
      result.value = this->Extended->num.value;
    }
    result.sign = 1;
    if (this->IsNegative()) {
      result.sign = - 1;
    }
    return result;
  }
  bool GetPrimeFactorsAbsoluteValue(
    List<LargeInt>& numeratorPrimeFactors,
    List<int> &numeratorMultiplicities,
    List<LargeInt>& denominatorPrimeFactors,
    List<int> &denominatorMultiplicities
  );
  inline const Rational& GetComplexConjugate() const {
    return *this;
  }
//  inline void SetDynamicSubtype(int dummyParameter)
//  {(void) dummyParameter;
//  }
  Rational RationalValue() {
    return *this;
  }
  void Subtract(const Rational& r);
  void AdD(const Rational& r) {
    this->operator+=(r);
  }
  void AddInteger(int x);
  void AssignLargeIntUnsigned(const LargeIntUnsigned& other) {
    LargeInt tempInt;
    tempInt.AssignLargeIntUnsigned(other);
    this->AssignLargeInteger(tempInt);
  }
  void AssignLargeInteger(const LargeInt& other) {
    if (this->Extended == 0) {
      this->Extended = new LargeRationalExtended;
#ifdef AllocationLimitsSafeguard
  ParallelComputing::GlobalPointerCounter ++;
  ParallelComputing::CheckPointerCounters();
#endif
    }
    this->Extended->num = other;
    this->Extended->den.MakeOne();
    this->ShrinkExtendedPartIfPossible();
  }
  void AssignString(const std::string& input);
  bool AssignStringFailureAllowed(const std::string& input);
  Rational GetZero() {
    return 0;
  }
  Rational GetOne() {
    return 1;
  }
  void AssignFracValue();
  void AssignFloor() {
    Rational tempRat = *this;
    tempRat.AssignFracValue();
    *this -= tempRat;
  }
  void MultiplyBy(const Rational& r);
  //IMPORTANT NOTE ON HASHFUNCTIONS
  //The Hash function of zero MUST be equal to zero.
  //See Note on Hashes before the definition of SomeRandomPrimes;
  unsigned int HashFunction() const {
    if (this->Extended == 0) {
      if (this->NumShort == 0) {
        return 0;
      }
      return this->NumShort*SomeRandomPrimes[0] + this->DenShort * ::SomeRandomPrimes[1];
    }
    return this->Extended->num.HashFunction() * SomeRandomPrimes[0] +
    this->Extended->den.HashFunction() * SomeRandomPrimes[1];
  }
  static inline unsigned int HashFunction(const Rational& input) {
    return input.HashFunction();
  }
  //void MultiplyByLargeRational(int num, int den);
  void MultiplyByInt(int x);
  void MultiplyByLargeInt(LargeInt& x);
  void MultiplyByLargeIntUnsigned(LargeIntUnsigned& x);
  void Assign(const Rational& r);
  void AssignInteger(int x);
  bool IsInteger(LargeInt* whichInteger = 0) const;
  bool IsIntegerFittingInInt(int* whichInt) const {
    LargeInt theInt;
    if (!this->IsInteger(&theInt)) {
      return false;
    }
    return theInt.IsIntegerFittingInInt(whichInt);
  }
  bool IsSmallInteger(int* whichInteger = 0) const {
    if (this->Extended != 0) {
      return false;
    }
    if (this->DenShort != 1) {
      return false;
    }
    if (whichInteger != 0) {
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
    this->Simplify();
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
    this->Extended->den.MultiplyByUInt(((unsigned int)tempDen));
    this->Extended->num.sign *= tempSign;
    this->Simplify();
  }
  void DivideByLargeInteger(LargeInt& x) {
    this->InitExtendedFromShortIfNeeded();
    this->Extended->den.MultiplyBy(x.value);
    this->Extended->num.sign *= x.sign;
    this->Simplify();
  }
  void DivideByLargeIntegerUnsigned(LargeIntUnsigned& x) {
    this->InitExtendedFromShortIfNeeded();
    this->Extended->den.MultiplyBy(x);
    this->Simplify();
  }
  std::string ToString(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringFrac() const;
  std::string ToStringForFileOperations(FormatExpressions* notUsed = 0) const;
  bool IsEqualTo(const Rational& r) const;
  bool IsGreaterThanOrEqualTo(const Rational& right) const;
  bool IsEven() const {
    Rational tempRat = *this;
    tempRat /= 2;
    return tempRat.IsInteger();
  }
  inline bool IsEqualToOne() const {
    if (this->Extended == 0) {
      return (this->NumShort == 1 && this->DenShort == 1);
    } else {
      return (this->Extended->num.IsEqualToOne() && this->Extended->den.IsEqualToOne());
    }
  }
  inline bool IsEqualToZero() const {
    if (this->Extended == 0) {
      return this->NumShort == 0;
    } else {
      return this->Extended->num.IsEqualToZero();
    }
  }
  inline bool operator<=(const Rational& other) const {
    return !(other < *this);
  }
  inline bool IsPositiveOrZero() const {
    if (this->Extended == 0) {
      return this->NumShort >= 0;
    } else {
      return this->Extended->num.IsPositiveOrZero();
    }
  }
  bool IsNegative() const {
    if (this->Extended == 0) {
      return this->NumShort < 0;
    } else {
      return this->Extended->num.IsNegative();
    }
  }
  bool IsNonPositive() const {
    if (this->Extended == 0) {
      return this->NumShort <= 0;
    } else {
      return this->Extended->num.IsNonPositive();
    }
  }
  bool IsPositive() const {
    if (this->Extended == 0) {
      return this->NumShort > 0;
    } else {
      return this->Extended->num.IsPositive();
    }
  }
  void Simplify();
  void Invert();
  void Minus() {
    if (this->Extended == 0) {
      this->NumShort *= - 1;
    } else {
      this->Extended->num.sign *= - 1;
    }
  }
  double GetDoubleValue() const;
  int floorIfSmall() {
    if (this->Extended == 0) {
      if (NumShort < 0) {
        if (DenShort != 1) {
          return (this->NumShort / this->DenShort) - 1;
        } else {
          return this->NumShort / this->DenShort;
        }
      } else {
        return this->NumShort / this->DenShort;
      }
    }
    crash << "This piece of code should not be reached. " << crash;
    return - 1;
  }
  void MakeZero() {
    this->NumShort = 0;
    this->DenShort = 1;
    this->FreeExtended();
  }
  void MakeOne() {
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
    return Rational::TotalLargeAdditions + Rational::TotalSmallAdditions;
  }
  static long long int TotalMultiplications() {
    return Rational::TotalLargeMultiplications + Rational::TotalSmallMultiplications;
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
    this->Extended = 0;
    this->AssignNumeratorAndDenominator(n, d);
    ParallelComputing::SafePointDontCallMeFromDestructors();
  }
  Rational(const LargeInt& other) {
    this->Extended = 0;
    *this = other;
  }
  Rational(const LargeIntUnsigned& other) {
    this->Extended = 0;
    *this = other;
  }
  Rational(): NumShort(0), DenShort(0), Extended(0) {
    ParallelComputing::SafePointDontCallMeFromDestructors();
  }
  Rational(int n) {
    this->Extended = 0;
    this->AssignNumeratorAndDenominator(n, 1);
    ParallelComputing::SafePointDontCallMeFromDestructors();
  }
  Rational(const Rational& right) {
    this->Extended = 0;
    this->Assign(right);
    ParallelComputing::SafePointDontCallMeFromDestructors();
  }
  Rational(const std::string& input): NumShort(0), DenShort(0), Extended(0) {
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
    return this->Extended == 0;
  }
  inline bool checkConsistency() {
    return this->CheckConsistency();
  }
  inline bool CheckConsistency() {
    if (this->Extended == 0) {
      return this->DenShort > 0;
    }
    return true;
  }
  void operator=(const AlgebraicNumber& other);
  void operator=(const LargeIntUnsigned& right) {
    LargeInt tempI;
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
    if (r.Extended == 0 && this->Extended == 0) {
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
    LargeInt tempI;
    tempI = tempRat.Extended->num;
    tempI.value.MultiplyBy(this->Extended->den);
    this->Extended->num.value.MultiplyBy(tempRat.Extended->den);
    this->Extended->num += (tempI);
    this->Extended->den.MultiplyBy(tempRat.Extended->den);
    this->Simplify();
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
  inline void operator=(const LargeInt& other) {
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
};
#endif
