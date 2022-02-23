// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_math_large_integers_ALREADY_INCLUDED
#define header_math_large_integers_ALREADY_INCLUDED

#include "general_lists.h"

class LargeIntegerUnsigned {
  void addNoFitSize(const LargeIntegerUnsigned& x);
  void fitSize();
  static List<unsigned int> precomputedPrimesSmallerThan15Bits;
public:
  // The zero element is assumed to have length one array with a zero entry.
  //
  // CarryOverBound is the "base" over which we work.
  // Requirements on the CarryOverBound:
  // 1.  +/-(CarryOverBound*2)- 1 must fit inside an (int)
  //     on the system
  // 2. (CarryOverBound*2)^2- 1 must fit inside (long long)
  //     on the system.
  // On a 32 bit machine any number smaller than or equal to 2^30 will work.
  // If you got no clue what to put just leave CarryOverBound as it is below.
  List<int32_t> digits;
  // static const int CarryOverBound =10; //<-for extreme "corner case" testing
  static const int carryOverBound = 1000000000;
  // the above choice of CarryOverBound facilitates very quick conversions of Large integers into decimal, with
  // relatively small loss of speed and RAM memory.
  // static const unsigned int CarryOverBound =2147483648UL; //=2^31
  // The following must be less than or equal to the square root of CarryOverBound.
  // It is used for quick multiplication of Rational-s.
  // static const int SquareRootOfCarryOverBound =3;//<-for extreme "corner case" testing
  static const int squareRootOfCarryOverBound = 31000; //31000*31000=961000000<1000000000
  // static const int SquareRootOfCarryOverBound =32768; //=2^15
  friend bool operator<(int left, const LargeIntegerUnsigned& right) {
    return right > left;
  }
  friend std::ostream& operator<<(std::ostream& output, const LargeIntegerUnsigned& largeIntegerUnsigned) {
    output << largeIntegerUnsigned.toString();
    return output;
  }
  static const List<unsigned int>& allPrimesSmallerThan15Bits();
  void padWithZeroesToAtLeastNDigits(int desiredMinNumDigits);
  void addLargeIntegerUnsignedShiftedTimesDigit(
    const LargeIntegerUnsigned& other, int digitShift, int constantTerm
  );
  void subtractSmallerPositive(const LargeIntegerUnsigned& x);
  void toString(std::string& output) const;
  void toStringLargeElementDecimal(std::string& output) const;
  std::string toString(FormatExpressions* format = nullptr) const;
  std::string toStringAbbreviate(FormatExpressions* format = nullptr) const;
  void dividePositive(
    const LargeIntegerUnsigned& divisor,
    LargeIntegerUnsigned& quotientOutput,
    LargeIntegerUnsigned& remainderOutput
  ) const;
  bool isDivisibleBy(const LargeIntegerUnsigned& divisor);
  void makeOne();
  void addUInt(unsigned int x);
  void makeZero();
  bool isEqualToZero() const;
  bool isEven() const;
  bool isPositive() const;
  bool tryIsPower(bool& outputIsPower, LargeInteger& outputBase, int& outputPower) const;
  bool isCompositePrimeDivision(
    List<unsigned int>& primesGenerated,
    bool& outputGuaranteedPrime,
    std::stringstream* comments
  );
  bool isPossiblyPrime(
    int millerRabinTries,
    bool tryDivisionSetTrueFaster = true,
    std::stringstream* comments = nullptr
  );
  bool isPossiblyPrimeMillerRabin(
    int numberOfTimesToRun = 1, std::stringstream* comments = nullptr
  );
  bool isPossiblyPrimeMillerRabinOnce(
    unsigned int base,
    int exponentOfThePowerTwoFactorOfNminusOne,
    const LargeIntegerUnsigned& oddFactorOfNminusOne,
    std::stringstream* comments
  );
  bool isEqualToOne() const;
  bool isGreaterThanOrEqualTo(const LargeIntegerUnsigned& x) const;
  static void getPrimesEratosthenesSieve(
    unsigned int primesUpToInclusive, List<unsigned int>& output
  );
  static void greatestCommonDivisor(const LargeIntegerUnsigned& a, const LargeIntegerUnsigned& b, LargeIntegerUnsigned& output);
  static LargeIntegerUnsigned greatestCommonDivisor(const LargeIntegerUnsigned& a, const LargeIntegerUnsigned& b);
  static LargeIntegerUnsigned leastCommonMultiple(const LargeIntegerUnsigned& a, const LargeIntegerUnsigned& b);
  static void leastCommonMultiple(const LargeIntegerUnsigned& a, const LargeIntegerUnsigned& b, LargeIntegerUnsigned& output);
  unsigned int hashFunction() const;
  void multiplyBy(const LargeIntegerUnsigned& right);
  void operator*=(const LargeIntegerUnsigned& right);
  void operator*=(unsigned int x);
  void operator+=(unsigned int x);
  void operator++(int);
  bool isIntegerFittingInInt(int* whichInt) const;
  void assignFactorial(unsigned int x);
  void multiplyBy(const LargeIntegerUnsigned& x, LargeIntegerUnsigned& output) const;
  void multiplyByUInt(unsigned int x);
  void addShiftedUIntSmallerThanCarryOverBound(unsigned int x, int shift);
  void assignShiftedUInt(unsigned int x, int shift);
  void assignUnsignedInt(unsigned int x);
  void assignUInt64(uint64_t x);
  // returns ceiling of the logarithm base two of the number,
  // i.e., the smallest x such that this <= 2^x.
  unsigned int logarithmBaseNCeiling(unsigned int base) const;

  int maximumDivisorToTryWhenFactoring(int desiredByUser) const;

  static void accountFactor(
    const LargeInteger& prime,
    List<LargeInteger>& outputPrimeFactors,
    List<int>& outputMultiplicities
  );
  // Product of resulting factors equals the original number,
  // even when factorization fails.
  //
  bool factor(
    List<LargeInteger>& outputFactors,
    List<int>& outputMultiplicites,
    int maximumDivisorToTry,
    int numberMillerRabinRuns,
    std::stringstream* commentsOnFailure
  ) const;
  void assignString(const std::string& input);
  bool assignStringFailureAllowed(const std::string& input, bool ignoreNonDigits);
  int getUnsignedIntValueTruncated();
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
  // LargeIntUnsigned(unsigned int x) {this->assignShiftedUInt(x,0);}
  static LargeIntegerUnsigned getOne();
  bool operator<(int other) const;
  bool operator<=(int other) const;
  bool operator>(int other) const;
  bool operator<(const LargeIntegerUnsigned& other) const;
  bool operator<(const LargeInteger& other) const;
  bool operator>=(const LargeIntegerUnsigned& other) const;
  bool operator>(const LargeIntegerUnsigned& other) const;
  void writeBigEndianBytes(List<unsigned char>& outputAppend, bool leadingZeroPad) const;
  bool writeBigEndianFixedNumberOfBytes(
    List<unsigned char>& outputAppend,
    int desiredNumberOfBytes,
    std::stringstream* commentsOnFailure
  ) const;
  void getHexBigEndian(int numberOfLeadingZeroesToPadWith, std::string& output) const;
  //must be rewritten:
  double getDoubleValue() const;
  class Test {
    public:
    static bool all();
    static bool serializationToHex(const LargeIntegerUnsigned& input);
    static bool serializationToHex();
    static bool comparisons();
    static bool isPossiblyPrime();
    static bool guaranteedPrime();
    static bool isPossiblyPrimeFast(
      const List<LargeIntegerUnsigned>& input,
      bool mustBeTrue,
      int millerRabinTries,
      int64_t maximumRunningTimeMilliseconds
    );
    static bool isPossiblyPrimeMillerRabinOnly(
      const List<LargeIntegerUnsigned>& input,
      bool mustBeTrue,
      int millerRabinTries
    );
    static bool factor();
    static bool factorSmall(
      const LargeIntegerUnsigned& input,
      const std::string& expectedFactors,
      const std::string& expectedMultiplicities,
      int maximumDivisorToTry,
      int numberMillerRabinRuns,
      int64_t maximumRunningTime
    );
  };
};

class LargeInteger {
  friend class Rational;
  friend LargeInteger operator*(const LargeInteger& left, const LargeInteger& right) {
    LargeInteger result;
    result = left;
    result *= right;
    return result;
  }
  friend bool operator<(int left, const LargeInteger& right) {
    LargeInteger leftCopy = left;
    return right > leftCopy;
  }
  friend LargeInteger operator*(const LargeInteger& left, const LargeIntegerUnsigned& right) {
    LargeInteger rightCopy;
    rightCopy = right;
    return left * rightCopy;
  }
  friend std::ostream& operator<<(std::ostream& output, const LargeInteger& largeInteger) {
    output << largeInteger.toString();
    return output;
  }
public:
  signed char sign;
  LargeIntegerUnsigned value;
  void operator*=(const LargeInteger& x) {
    this->sign *= x.sign;
    this->value.multiplyBy(x.value);
  }
  void multiplyByInt(int x);
  void toString(std::string& output) const;
  std::string toString(FormatExpressions* format = nullptr) const {
    (void) format;
    std::string tempS;
    this->toString(tempS);
    return tempS;
  }
  bool isIntegerFittingInInt(int* whichInt);
  bool isPositive() const {
    return this->sign == 1 && (this->value.isPositive());
  }
  bool isNegative() const {
    return this->sign == - 1 && (this->value.isPositive());
  }
  bool beginsWithMinus() {
    return this->isNegative();
  }
  bool isPositiveOrZero() const {
    return !this->isNegative();
  }
  bool isNonPositive() const {
    return !this->isPositive();
  }
  static LargeInteger zero();
  static LargeInteger zeroStatic();
  bool tryIsPower(bool& outputIsPower, LargeInteger& outputBase, int& outputPower) const;
  bool needsParenthesisForMultiplication(FormatExpressions* unused) const {
    (void) unused;
    return false;
  }
  bool operator==(const LargeInteger& x) const;
  bool isEqualToZero() const {
    return this->value.isEqualToZero();
  }
  bool isEven() const;
  bool isEqualToOne() const {
    return this->value.isEqualToOne() && this->sign == 1;
  }
  void assignLargeIntUnsigned(const LargeIntegerUnsigned& x) {
    this->value = x;
    this->sign = 1;
  }
  void assignInteger(int x);
  void assignInt64(int64_t x);
  void assignUInt64(uint64_t x);
  void addLargeIntUnsigned(const LargeIntegerUnsigned& x);
  void addInt(int x) {
    LargeInteger converted;
    converted.assignInteger(x);
    *this += converted;
  }
  //bool IsGEQ(LargeInt& x);
  bool checkConsistensy();
  void writeToFile(std::fstream& output);
  void assignString(const std::string& input);
  bool assignStringFailureAllowed(const std::string& input, std::stringstream* commentsOnFailure);
  void readFromFile(std::fstream& input);
  void checkConsistency(){}
  void makeZero();
  bool getDivisors(List<int>& output, bool includeNegative);
  void makeOne() {
    this->value.makeOne();
    this->sign = 1;
  }
  void makeMinusOne() {
    this->value.makeOne();
    this->sign = - 1;
  }
  static unsigned int hashFunction(const LargeInteger& input) {
    return input.hashFunction();
  }
  unsigned int hashFunction() const {
    return this->value.hashFunction() + static_cast<unsigned int>(this->sign) + 3;
  }
  int getIntValueTruncated() {
   return this->sign * this->value.getUnsignedIntValueTruncated();
  }
  double getDoubleValue() const;
  int operator %(int x);
  inline void assignFloor(){}
  void operator=(const Rational& x);
  void operator=(const LargeInteger& x);
  inline void operator=(int x) {
    this->assignInteger(x);
  }
  inline void operator=(const LargeIntegerUnsigned& other) {
    this->value = other;
    this->sign = 1;
  }
  void operator*=(const LargeIntegerUnsigned& other) {
    if (other.isEqualToZero()) {
      this->makeZero();
      return;
    }
    this->value *= other;
  }
  inline void operator++(int) {
    *this += 1;
  }
  inline void operator*=(int x) {
    this->multiplyByInt(x);
  }
  void negate() {
    if (!this->isEqualToZero()) {
      this->sign *= - 1;
    }
  }
  void operator+=(const LargeInteger& other);
  inline void operator+=(const LargeIntegerUnsigned& other) {
    this->addLargeIntUnsigned(other);
  }
  inline void operator+=(int x) {
    this->addInt(x);
  }
  inline bool operator!=(const LargeInteger& other) const {
    return !(*this == other);
  }
  void operator-=(const LargeInteger& other) {
    this->negate();
    *this += (other);
    this->negate();
  }
  inline bool operator<=(const LargeInteger& other) const {
    return !(other<*this);
  }
  inline bool operator<(const LargeInteger& other) const {
    if (other.isPositiveOrZero()) {
      if (this->isPositiveOrZero()) {
        return this->value<other.value;
      }
      return true;
    }
    if (this->isNegative()) {
      return other.value < this->value;
    }
    return false;
  }
  inline LargeInteger operator+(const LargeInteger& other) const {
    LargeInteger result;
    result = *this;
    result += other;
    return result;
  }
  inline LargeInteger operator-(const LargeInteger& other) const {
    LargeInteger result = *this;
    result -= (other);
    return result;
  }
  inline LargeInteger operator+(int x) const {
    LargeInteger result = *this;
    result.addInt(x);
    return result;
  }
  LargeInteger operator*(int x) {
    LargeInteger result = *this;
    result.multiplyByInt(x);
    return result;
  }
  LargeInteger operator/(int x) const;
  LargeInteger operator/(LargeInteger& x) const;
  void operator/=(const LargeInteger& other) {
    if (this->isEqualToZero()) {
      return;
    }
    this->sign*= other.sign;
    LargeIntegerUnsigned quotient, remainder;
    this->value.dividePositive(other.value, quotient, remainder);
    this->value =quotient;
  }
  inline bool operator>(const LargeInteger& other) const {
    return other < *this;
  }
  void operator%=(const LargeInteger& other) {
    if (this->isEqualToZero()) {
      return;
    }
    LargeIntegerUnsigned quotient, remainder;
    this->value.dividePositive(other.value, quotient, remainder);
    this->value = remainder;
    if (this->isNegative()) {
      *this += other.value;
    }
  }
  void raiseToPower(int power);
  LargeInteger operator%(const LargeInteger& other) const {
    LargeInteger result = *this;
    result %= other;
    return result;
  }
  LargeInteger(const LargeInteger& x) {
    this->operator=(x);
  }
  LargeInteger(int x) {
    this->assignInteger(x);
  }
  LargeInteger(int64_t x) {
    this->assignInt64(x);
  }
  LargeInteger(uint64_t x) {
    this->assignUInt64(x);
  }
  LargeInteger(const LargeIntegerUnsigned& other) : sign(1), value(other) {
  }
  LargeInteger(): sign(1) {}
};

Rational operator-(const Rational& argument);
Rational operator/(int left, const Rational& right);

class Rational {
private:
  struct LargeRationalExtended {
  public:
    LargeInteger numerator;
    LargeIntegerUnsigned denominator;
  };
  friend Rational operator-(const Rational& argument);
  friend Rational operator/(int left, const Rational& right);
  friend std::ostream& operator << (std::ostream& output, const Rational& rational) {
    output << rational.toString();
    return output;
  }
  friend Rational operator*(int left, const Rational& right) {
    return Rational (left) * right;
  }
  friend bool operator<(int left, const Rational& right) {
    return Rational(left) < right;
  }
  bool tryToAddQuickly(int otherNumerator, int otherDenominator);
  bool tryToMultiplyQuickly(int otherNumerator, int otherDenominator);
  void allocateExtended() {
    if (this->extended != nullptr) {
      return;
    }
    this->extended = new LargeRationalExtended;
#ifdef AllocationLimitsSafeguard
  GlobalStatistics::globalPointerCounter ++;
  GlobalStatistics::checkPointerCounters();
#endif
  }
  bool initializeExtendedFromShortIfNeeded() {
    if (this->extended != nullptr) {
      return false;
    }
    this->extended = new LargeRationalExtended;
#ifdef AllocationLimitsSafeguard
  GlobalStatistics::globalPointerCounter ++;
  GlobalStatistics::checkPointerCounters();
#endif
    this->extended->denominator.assignShiftedUInt( static_cast<unsigned int>(this->denominatorShort), 0);
    this->extended->numerator.assignInteger(this->numeratorShort);
    return true;
  }
  inline void freeExtended() {
    if (this->extended == nullptr) {
      return;
    }
    delete this->extended;
    this->extended = nullptr;
#ifdef AllocationLimitsSafeguard
  GlobalStatistics::globalPointerCounter ++;
  GlobalStatistics::checkPointerCounters();
#endif
  }
  bool shrinkExtendedPartIfPossible();
public:
  int numeratorShort;
  int denominatorShort;
  LargeRationalExtended* extended;
  static unsigned long long int totalSmallAdditions;
  static unsigned long long int totalLargeAdditions;
  static unsigned long long int totalSmallMultiplications;
  static unsigned long long int totalLargeMultiplications;
  static unsigned long long int totalSmallGreatestCommonDivisors;
  static unsigned long long int totalLargeGreatestCommonDivisors;
  static std::string className() {
    return "Rational";
  }
  bool needsParenthesisForMultiplicationWhenSittingOnTheRightMost() const {
    return this->isNegative();
  }
  bool needsParenthesisForMultiplication(FormatExpressions* unused) const {
    (void) unused;
    return false;
    //return this->isNegative();
  }
  bool getSquareRootIfRational(Rational& output) const;
  // Scales a vector of rationals so as to make all coordinates
  // relatively prime integers such that the non-zero coefficient is positive.
  // Returns the number by which the vector was multiplied.
  static Rational scaleNormalizeIndex(List<Rational>& inputOutput, int indexNonZeroEntry);
  static Rational scaleNoSignChange(List<Rational>& inputOutput);
  LargeIntegerUnsigned getDenominator() const;
  bool beginsWithMinus();
  LargeInteger getNumerator() const;
  bool getPrimeFactorsAbsoluteValue(
    List<LargeInteger>& numeratorPrimeFactors,
    List<int>& numeratorMultiplicities,
    List<LargeInteger>& denominatorPrimeFactors,
    List<int>& denominatorMultiplicities
  );
  const Rational& getComplexConjugate() const;
  Rational rationalValue() {
    return *this;
  }
  void subtract(const Rational& r);
  void addInteger(int x);
  void assignLargeIntUnsigned(const LargeIntegerUnsigned& other);
  void assignLargeInteger(const LargeInteger& other);
  void assignString(const std::string& input);
  bool assignStringFailureAllowed(const std::string& input);
  static Rational zero();
  static Rational zeroStatic();
  static Rational one();
  static Rational oneStatic();
  void assignFractionalValue();
  void assignFloor() {
    Rational result = *this;
    result.assignFractionalValue();
    *this -= result;
  }
  void multiplyBy(const Rational& r);
  // The Hash function of zero must be equal to zero.
  // See Note on Hashes before the definition of someRandomPrimes;
  unsigned int hashFunction() const {
    if (this->extended == nullptr) {
      if (this->numeratorShort == 0) {
        return 0;
      }
      return static_cast<unsigned int>(this->numeratorShort) * HashConstants::constant0 +
      static_cast<unsigned int>(this->denominatorShort) * HashConstants::constant1;
    }
    return this->extended->numerator.hashFunction() * HashConstants::constant0 +
    this->extended->denominator.hashFunction() * HashConstants::constant1;
  }
  static inline unsigned int hashFunction(const Rational& input) {
    return input.hashFunction();
  }
  void multiplyByInteger(int x);
  void multiplyByLargeInteger(LargeInteger& x);
  void multiplyByLargeIntUnsigned(LargeIntegerUnsigned& x);
  void assign(const Rational& r);
  void assignInteger(int x);
  bool isInteger(LargeInteger* whichInteger = nullptr) const;
  bool isIntegerFittingInInt(int* whichInteger) const {
    LargeInteger resultInteger;
    if (!this->isInteger(&resultInteger)) {
      return false;
    }
    return resultInteger.isIntegerFittingInInt(whichInteger);
  }
  bool isSmallInteger(int* whichInteger = nullptr) const {
    if (this->extended != nullptr) {
      return false;
    }
    if (this->denominatorShort != 1) {
      return false;
    }
    if (whichInteger != nullptr) {
      *whichInteger = this->numeratorShort;
    }
    return true;
  }
  bool isGreaterThan(const Rational& r) const;
  inline void assignNumeratorAndDenominator(int n, int d) {
    if (d < 0) {
      d = - d;
      n = - n;
    }
    this->numeratorShort = n;
    this->denominatorShort = d;
    this->freeExtended();
    this->simplify();
  }
  void divideBy(const Rational& r);
  void divideByInteger(int x) {
    int denominator = 0;
    signed char tempSign;
    if (x < 0) {
      denominator = - x;
      tempSign = - 1;
    } else {
      denominator = x;
      tempSign = 1;
    }
    if (this->tryToMultiplyQuickly(tempSign, denominator)) {
      return;
    }
    this->initializeExtendedFromShortIfNeeded();
    this->extended->denominator.multiplyByUInt(static_cast<unsigned int>(denominator));
    this->extended->numerator.sign *= tempSign;
    this->simplify();
  }
  void divideByLargeInteger(LargeInteger& x) {
    this->initializeExtendedFromShortIfNeeded();
    this->extended->denominator.multiplyBy(x.value);
    this->extended->numerator.sign *= x.sign;
    this->simplify();
  }
  void divideByLargeIntegerUnsigned(LargeIntegerUnsigned& x) {
    this->initializeExtendedFromShortIfNeeded();
    this->extended->denominator.multiplyBy(x);
    this->simplify();
  }
  std::string toString(FormatExpressions* format = nullptr) const;
  std::string toStringFrac() const;
  std::string toStringForFileOperations(FormatExpressions* notUsed = nullptr) const;
  bool isEqualTo(const Rational& r) const;
  bool isGreaterThanOrEqualTo(const Rational& right) const;
  bool isEven() const {
    Rational thisCopy = *this;
    thisCopy /= 2;
    return thisCopy.isInteger();
  }
  inline bool isEqualToOne() const {
    if (this->extended == nullptr) {
      return (this->numeratorShort == 1 && this->denominatorShort == 1);
    } else {
      return (this->extended->numerator.isEqualToOne() && this->extended->denominator.isEqualToOne());
    }
  }
  inline bool isEqualToZero() const {
    if (this->extended == nullptr) {
      return this->numeratorShort == 0;
    } else {
      return this->extended->numerator.isEqualToZero();
    }
  }
  inline bool operator<=(const Rational& other) const {
    return !(other < *this);
  }
  inline bool isPositiveOrZero() const {
    if (this->extended == nullptr) {
      return this->numeratorShort >= 0;
    } else {
      return this->extended->numerator.isPositiveOrZero();
    }
  }
  bool isNegative() const {
    if (this->extended == nullptr) {
      return this->numeratorShort < 0;
    } else {
      return this->extended->numerator.isNegative();
    }
  }
  bool isNonPositive() const {
    if (this->extended == nullptr) {
      return this->numeratorShort <= 0;
    } else {
      return this->extended->numerator.isNonPositive();
    }
  }
  bool isPositive() const {
    if (this->extended == nullptr) {
      return this->numeratorShort > 0;
    } else {
      return this->extended->numerator.isPositive();
    }
  }
  void simplify();
  void invert();
  void negate() {
    if (this->extended == nullptr) {
      this->numeratorShort *= - 1;
    } else {
      this->extended->numerator.sign *= - 1;
    }
  }
  double getDoubleValue() const;
  int floorIfSmall();
  void makeZero() {
    this->numeratorShort = 0;
    this->denominatorShort = 1;
    this->freeExtended();
  }
  void makeOne() {
    this->numeratorShort = 1;
    this->denominatorShort = 1;
    this->freeExtended();
  }
  void makeMinusOne() {
    this->numeratorShort = - 1;
    this->denominatorShort = 1;
    this->freeExtended();
  }
  void writeToFile(std::fstream& output);
  void readFromFile(std::istream& input);
  inline void AssignAbsoluteValue() {
    if (this->isNegative()) {
      this->negate();
    }
  }
  static long long int totalAdditions() {
    return static_cast<long long int>(Rational::totalLargeAdditions + Rational::totalSmallAdditions);
  }
  static long long int totalMultiplications() {
    return static_cast<long long int>(Rational::totalLargeMultiplications + Rational::totalSmallMultiplications);
  }
  static long long int totalArithmeticOperations() {
    return Rational::totalAdditions() + Rational::totalMultiplications();
  }
  static Rational nChooseK(const Rational& n, int k);
  static Rational factorial(int n);
  static Rational twoToTheNth(int n);
  static Rational NtoTheKth(int n, int k);
  void raiseToPower(int x);
  // Calling the following constructor on an already initialized object will leak the
  // extended pointer.
  Rational(int n, int d) {
    this->extended = nullptr;
    this->assignNumeratorAndDenominator(n, d);
  }
  Rational(const LargeInteger& other) {
    this->extended = nullptr;
    *this = other;
  }
  Rational(const LargeIntegerUnsigned& other) {
    this->extended = nullptr;
    *this = other;
  }
  Rational(): numeratorShort(0), denominatorShort(0), extended(nullptr) {
  }
  Rational(int n) {
    this->extended = nullptr;
    this->assignNumeratorAndDenominator(n, 1);
  }
  Rational(const Rational& right) {
    this->extended = nullptr;
    this->assign(right);
  }
  Rational(const std::string& input): numeratorShort(0), denominatorShort(0), extended(nullptr) {
    this->assignString(input);
  }
  ~Rational() {
    this->freeExtended();
  }
  //the below must be called only with positive arguments!
  static inline int greatestCommonDivisor(int a, int b) {
    MacroIncrementCounter(Rational::totalSmallGreatestCommonDivisors);
    return MathRoutines::greatestCommonDivisor(a, b);
  }
  static int greatestCommonDivisorSigned(int a, int b) {
    if (a < 0) {
      a *= - 1;
    }
    if (b < 0) {
      b *= - 1;
    }
    return Rational::greatestCommonDivisor(a, b);
  }
  bool checkConsistency() {
    if (this->extended == nullptr) {
      return this->denominatorShort > 0;
    }
    return true;
  }
  void operator=(const AlgebraicNumber& other);
  void operator=(const LargeIntegerUnsigned& right) {
    LargeInteger rightCopy;
    rightCopy = right;
    this->operator=(rightCopy);
  }
  void operator=(const Rational& right) {
    this->assign(right);
  }
  void operator=(const Polynomial<Rational>& other);
  bool operator==(const int other) const {
    if (other == 0) {
      return this->isEqualToZero();
    }
    return this->isEqualTo(other);
  }
  inline bool operator==(const Rational& right) const {
    return this->isEqualTo(right);
  }
  inline void operator+=(const Rational& r) {
    if (r.extended == nullptr && this->extended == nullptr) {
      if (this->tryToAddQuickly(r.numeratorShort, r.denominatorShort)) {
        return;
      }
    }
    if (this == &r) {
      this->multiplyByInteger(2);
      return;
    }
    MacroIncrementCounter(Rational::totalLargeAdditions);
    this->initializeExtendedFromShortIfNeeded();
    Rational rightCopy;
    rightCopy.assign(r);
    rightCopy.initializeExtendedFromShortIfNeeded();
    LargeInteger numeratorCopy;
    numeratorCopy = rightCopy.extended->numerator;
    numeratorCopy.value.multiplyBy(this->extended->denominator);
    this->extended->numerator.value.multiplyBy(rightCopy.extended->denominator);
    this->extended->numerator += numeratorCopy;
    this->extended->denominator.multiplyBy(rightCopy.extended->denominator);
    this->simplify();
  }
  inline void operator-=(const Rational& right) {
    this->subtract(right);
  }
  inline void operator*=(const Rational& right) {
    this->multiplyBy(right);
  }
  void operator/=(const Rational& right) {
    this->divideBy(right);
  }
  void operator+=(int right) {
    this->addInteger(right);
  }
  void operator-=(int right) {
    Rational converted = right;
    this->subtract(converted);
  }
  void operator=(int right) {
    this->assignInteger(right);
  }
  inline void operator=(const LargeInteger& other) {
    this->assignLargeInteger(other);
  }
  Rational operator*(const Rational& right) const;
  void operator++(int) {
    *this += 1;
  }
  Rational operator*(int right) const {
    Rational rightCopy;
    rightCopy.assign(*this);
    rightCopy.multiplyByInteger(right);
    return rightCopy;
  }
  Rational operator/(int right) const {
    Rational rightCopy;
    rightCopy.assign(*this);
    rightCopy.divideByInteger(right);
    return rightCopy;
  }
  Vector<Rational> operator*(const Vector<Rational>& right) const;
  Rational operator+(const Rational& right) const;
  Rational operator-(const Rational& right) const;
  Rational operator/(const Rational& right) const;
  inline bool operator!=(const Rational& right) const {
    return !this->isEqualTo(right);
  }
  inline bool operator!=(const int& right) const {
    return !((*this) == right);
  }
  inline bool operator>(const Rational& right) const {
    return this->isGreaterThan(right);
  }
  inline bool operator<(const Rational& right) const {
    return right.isGreaterThan(*this);
  }
  inline bool operator>(const int right) const {
    Rational rightCopy;
    rightCopy.assignInteger(right);
    return this->isGreaterThan(rightCopy);
  }
  inline bool operator>=(const Rational& right) const {
    return this->isGreaterThanOrEqualTo(right);
  }
  inline bool operator<(const int right) const {
    Rational rightCopy;
    rightCopy.assignInteger(right);
    return rightCopy.isGreaterThan(*this);
  }
  class Test {
  public:
    static bool all();
    static bool testScale();
  };
};

#endif // header_math_large_integers_ALREADY_INCLUDED
