// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "general_lists.h"
#include "math_general_implementation.h"
#include "math_general.h"
#include "math_extra_algebraic_numbers.h"

void LargeIntegerUnsigned::assignString(const std::string& input) {
  if (input.size() < 10) {
    unsigned int x = static_cast<unsigned>(std::atoi(input.c_str()));
    this->assignShiftedUInt(x, 0);
    return;
  }
  this->makeZero();
  for (unsigned int i = 0; i < input.size(); i ++) {
    this->operator*=(10);
    int whichDigit = input[i] - '0';
    if (whichDigit > 9 || whichDigit < 0) {
      global.fatal << "This is a programming error: LargeIntUnsigned::assignString "
      << "called on the string " << input
      << " which does not consist entirely of digits. "
      << "Please note that LargeIntUnsigned::assignString is a no-fail function, intended for "
      << "internal use only. If you want to parse arbitrary unsafe "
      << "expressions coming from the user, please use the big gun (a.k.a. Calculator). "
      << global.fatal;
    }
    this->operator+=(static_cast<unsigned>(whichDigit));
  }
}

bool LargeIntegerUnsigned::assignStringFailureAllowed(const std::string& input, bool ignoreNonDigits) {
  MacroRegisterFunctionWithName("LargeIntUnsigned::assignStringFailureAllowed");
  if (input.size() > 10000000) {
    //<- sorry folks, no more than 10 million digits.
    return false;
  }
  if (input.size() < 10) {
    unsigned int x = static_cast<unsigned>(std::atoi(input.c_str()));
    this->assignShiftedUInt(x, 0);
    return true;
  }
  this->makeZero();
  for (unsigned int i = 0; i < input.size(); i ++) {
    this->operator*=(10);
    int whichDigit = input[i] - '0';
    if (whichDigit > 9 || whichDigit < 0) {
      if (!ignoreNonDigits) {
        return false;
      }
    } else {
      this->operator+=(static_cast<unsigned>(whichDigit));
    }
  }
  return true;
}

LargeIntegerUnsigned LargeIntegerUnsigned::operator*(const LargeIntegerUnsigned& x) const {
  LargeIntegerUnsigned result;
  this->multiplyBy(x, result);
  return result;
}

LargeIntegerUnsigned LargeIntegerUnsigned::operator/(unsigned int x) const {
  LargeIntegerUnsigned result;
  LargeIntegerUnsigned remainder;
  LargeIntegerUnsigned tempX;
  tempX.assignShiftedUInt(x, 0);
  this->divPositive(tempX, result, remainder);
  return result;
}

LargeIntegerUnsigned LargeIntegerUnsigned::operator/(const LargeIntegerUnsigned& x) const {
  LargeIntegerUnsigned result;
  LargeIntegerUnsigned remainder;
  this->divPositive(x, result, remainder);
  return result;
}

int LargeIntegerUnsigned::operator%(unsigned int x) {
  LargeIntegerUnsigned quotient;
  LargeIntegerUnsigned remainder;
  LargeIntegerUnsigned divisor;
  divisor.assignShiftedUInt(x, 0);
  this->divPositive(divisor, quotient, remainder);
  int result = 0;
  remainder.isIntegerFittingInInt(&result);
  return result;
}

void LargeIntegerUnsigned::getHexBigEndian(
  int numberOfLeadingZeroesToPadWith, std::string& output
) const {
  List<char> result;
  LargeIntegerUnsigned digit, inputCopy = *this;
  while (inputCopy > 0) {
    digit = static_cast<unsigned char>(inputCopy % 256);
    inputCopy /= 256;
    int digitInt = 0;
    digit.isIntegerFittingInInt(&digitInt);
    char digitChar = static_cast<char>(digitInt);
    result.addOnTop(digitChar);
  }
  for (int i = 0; i < numberOfLeadingZeroesToPadWith; i ++) {
    result.addOnTop(0);
  }
  result.reverseElements();
  output.assign(result.objects, static_cast<unsigned>(result.size));
}

bool LargeIntegerUnsigned::writeBigEndianFixedNumberOfBytes(
  List<unsigned char>& outputAppend,
  int desiredNumberOfBytes,
  std::stringstream* commentsOnFailure
) const {
  int startOffset = outputAppend.size;
  this->writeBigEndianBytes(outputAppend, 0);
  int zeroesToPad = desiredNumberOfBytes - outputAppend.size - startOffset;
  if (zeroesToPad > 0) {
    outputAppend.shiftUpExpandOnTopRepeated(startOffset, zeroesToPad);
    for (int i = 0; i < zeroesToPad; i ++) {
      outputAppend[startOffset + i] = 0;
    }
  }
  int bytesTotal = outputAppend.size - startOffset;
  if (bytesTotal != desiredNumberOfBytes) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Large integer ";
      *commentsOnFailure << "generated " << bytesTotal << " bytes instead of "
      << "the desired " << desiredNumberOfBytes << " bytes. ";
    }
    return false;
  }
  return true;
}

void LargeIntegerUnsigned::writeBigEndianBytes(
  List<unsigned char>& outputAppend, bool leadingZeroPad
) const {
  LargeIntegerUnsigned remainder = *this;
  List<unsigned char> digitsReveredOrder;
  while (remainder > 0) {
    int nextDigit = remainder % 256;
    digitsReveredOrder.addOnTop(static_cast<unsigned char>(nextDigit));
    remainder /= 256;
  }
  int padWithZero = 0;
  if (leadingZeroPad) {
    if (digitsReveredOrder.size > 0) {
      if (digitsReveredOrder[digitsReveredOrder.size - 1] >= 128) {
        padWithZero = 1;
      }
    }
  }
  outputAppend.reserve(outputAppend.size + digitsReveredOrder.size + padWithZero);
  if (padWithZero == 1) {
    outputAppend.addOnTop(0);
  }
  for (int i = digitsReveredOrder.size - 1; i >= 0; i --) {
    outputAppend.addOnTop(digitsReveredOrder[i]);
  }
}

void LargeIntegerUnsigned::makeOne() {
  this->theDigits.setSize(1);
  this->theDigits[0] = 1;
}

void LargeIntegerUnsigned::makeZero() {
  this->theDigits.setSize(1);
  this->theDigits[0] = 0;
}

void LargeIntegerUnsigned::multiplyByUInt(unsigned int x) {
  LargeIntegerUnsigned tempLI;
  tempLI.assignShiftedUInt(x, 0);
  this->multiplyBy(tempLI);
}

void LargeIntegerUnsigned::multiplyBy(const LargeIntegerUnsigned& x) {
  LargeIntegerUnsigned tempInt;
  this->multiplyBy(x, tempInt);
  *this = tempInt;
}

void LargeIntegerUnsigned::addUInt(unsigned int x) {
  LargeIntegerUnsigned tempI;
  tempI.assignShiftedUInt(x, 0);
  (*this) += tempI;
}

void LargeIntegerUnsigned::operator*=(const LargeIntegerUnsigned& right) {
  this->multiplyBy(right);
}

void LargeIntegerUnsigned::operator*=(unsigned int x) {
  this->multiplyByUInt(x);
}

void LargeIntegerUnsigned::operator+=(unsigned int x) {
  this->addUInt(x);
}

LargeIntegerUnsigned LargeIntegerUnsigned::operator+(const LargeIntegerUnsigned& other) {
  LargeIntegerUnsigned result = *this;
  result += other;
  return result;
}

void LargeIntegerUnsigned::operator++(int) {
  this->addUInt(1);
}

bool LargeIntegerUnsigned::isIntegerFittingInInt(int* whichInt) const {
  LargeIntegerUnsigned twoToThe31 = 2;
  MathRoutines::raiseToPower(twoToThe31, 31, LargeIntegerUnsigned(1));
  if (*this >= twoToThe31) {
    return false;
  }
  if (whichInt == nullptr) {
    return true;
  }
  *whichInt = 0;
  for (int i = this->theDigits.size - 1; i >= 0; i --) {
    *whichInt *= LargeIntegerUnsigned::CarryOverBound;
    *whichInt += this->theDigits[i];
  }
  return true;
}

bool LargeIntegerUnsigned::isGreaterThanOrEqualTo(const LargeIntegerUnsigned& x) const {
  if (this->theDigits.size > x.theDigits.size) {
    return true;
  }
  if (this->theDigits.size < x.theDigits.size) {
    return false;
  }
  for (int i = this->theDigits.size - 1; i >= 0; i --) {
    if (x.theDigits[i] > this->theDigits[i]) {
      return false;
    }
    if (x.theDigits[i] < this->theDigits[i]) {
      return true;
    }
  }
  return true;
}

bool LargeIntegerUnsigned::isPossiblyPrimeMillerRabinOnce(
  unsigned int theBase,
  int theExponentOfThePowerTwoFactorOfNminusOne,
  const LargeIntegerUnsigned& theOddFactorOfNminusOne,
  std::stringstream* comments
) {
  MacroRegisterFunctionWithName("LargeIntUnsigned::isPossiblyPrimeMillerRabinOnce");
  if (*this == theBase) {
    return true;
  }
  if (this->isEqualToOne()) {
    return false;
  }
  ElementZmodP thePower, theOne;
  thePower.modulus = *this;
  thePower.value = theBase;
  theOne.modulus = *this;
  theOne.value = 1;
  MathRoutines::raiseToPower(thePower, theOddFactorOfNminusOne, theOne);
  if (thePower == 1) {
    return true;
  }
  for (int i = 0; i < theExponentOfThePowerTwoFactorOfNminusOne; i ++) {
    if (thePower == - 1) {
      return true;
    }
    if (i == theExponentOfThePowerTwoFactorOfNminusOne - 1) {
      if (comments != nullptr) {
        std::stringstream theTwoPowerContraStream, theTwoPowerStream;
        if (i > 0) {
          theTwoPowerContraStream << "2";
          if (i > 1) {
            theTwoPowerContraStream << "^{ " << i << "} \\cdot ";
          }
        }
        theTwoPowerStream << "2";
        if (theExponentOfThePowerTwoFactorOfNminusOne > 1) {
          theTwoPowerStream << "^{" << theExponentOfThePowerTwoFactorOfNminusOne << "}";
        }
        *comments << this->toString() << " is not prime because \\(" << theBase << "^{"
        << theTwoPowerContraStream.str()
        << theOddFactorOfNminusOne.toString() << "} = " << thePower.value.toString() << " ~ mod ~"
        << this->toString() << " \\)"
        << "<br>If " << this->toString() << " were prime, we'd have to have that \\("
        << theBase << "^{" << theTwoPowerStream.str() << "\\cdot" << theOddFactorOfNminusOne
        << "} = " << theBase << "^{" << this->toString() << " - 1} = 1 ~mod ~" << this->toString() << "\\)"
        << "<br> which can be reasoned to contradict the first equality.";
      }
      return false;
    }
    thePower *= thePower;
  }
  if (comments != nullptr) {
    *comments << "It appears your number is even. ";
  }
  return false;
}

bool LargeIntegerUnsigned::tryIsPower(
  bool& outputIsPower, LargeInteger& outputBase, int& outputPower
) const {
  MacroRegisterFunctionWithName("LargeIntUnsigned::tryIsPower");
  List<LargeInteger> factors;
  List<int> multiplicities;
  if (!this->factor(factors, multiplicities, 0, 4, nullptr)) {
    return false;
  }
  if (multiplicities.size == 0) {
    outputIsPower = true;
    outputBase = 1;
    outputPower = 0;
    return true;
  }
  if (multiplicities[0] <= 1) {
    outputIsPower = false;
    return true;
  }
  for (int i = 1; i < factors.size; i ++) {
    if (multiplicities[i] != multiplicities[0]) {
      outputIsPower = false;
      return true;
    }
  }
  outputIsPower = true;
  outputBase = 1;
  for (int i = 0; i < factors.size; i ++) {
    outputBase *= factors[i];
  }
  outputPower = multiplicities[0];
  return true;
}

bool LargeIntegerUnsigned::isPossiblyPrimeMillerRabin(int numberOfTries, std::stringstream* comments) {
  return this->isPossiblyPrime(numberOfTries, false, comments);
}

bool LargeIntegerUnsigned::isCompositePrimeDivision(
  List<unsigned int>& primesGenerated, std::stringstream* comments
) {
  MacroRegisterFunctionWithName("LargeIntUnsigned::isCompositePrimeDivision");
  if (this->isEven()) {
    return *this == 2;
  }
  if (this->isEqualToOne()) {
    return false;
  }
  int maximumDivisor = this->maximumDivisorToTryWhenFactoring(- 1);
  LargeIntegerUnsigned::getPrimesEratosthenesSieve(
    static_cast<unsigned>(maximumDivisor), primesGenerated
  );
  for (int i = 0; i < primesGenerated.size; i ++) {
    LargeIntegerUnsigned current = primesGenerated[i];
    if (current >= *this) {
      return false;
    }
    if (!this->isDivisibleBy(current)) {
      continue;
    }
    if (comments != nullptr) {
      *comments << "Number not prime as it is divisible by "
      << primesGenerated[i] << ". ";
    }
    return true;
  }
  return false;
}

bool LargeIntegerUnsigned::isPossiblyPrime(
  int millerRabinTries,
  bool tryDivisionSetTrueFaster,
  std::stringstream* comments
) {
  MacroRegisterFunctionWithName("LargeIntUnsigned::isPossiblyPrime");
  if (this->isEqualToOne()) {
    if (comments != nullptr) {
      *comments << "1 is not prime by definition. ";
    }
    return false;
  }
  if (this->isEven()) {
    if (*this != 2 && comments != nullptr) {
      *comments << "Number " << *this << " is even but not two. ";
    }
    return *this == 2;
  }
  List<unsigned int> aFewPrimes;
  if (tryDivisionSetTrueFaster) {
    if (this->isCompositePrimeDivision(aFewPrimes, comments)) {
      return false;
    }
  } else {
    LargeIntegerUnsigned::getPrimesEratosthenesSieve(1000, aFewPrimes);
  }
  if (millerRabinTries > aFewPrimes.size) {
    millerRabinTries = aFewPrimes.size;
  }
  LargeIntegerUnsigned theOddFactorOfNminusOne = *this;
  int theExponentOfThePowerTwoFactorOfNminusOne = 0;
  theOddFactorOfNminusOne --;
  while (theOddFactorOfNminusOne.isEven()) {
    theOddFactorOfNminusOne /= 2;
    theExponentOfThePowerTwoFactorOfNminusOne ++;
  }
  ProgressReport theReport;
  for (int i = 0; i < millerRabinTries; i ++) {
    if (theReport.tickAndWantReport()) {
      std::stringstream reportStream;
      reportStream << "Testing whether " << this->toStringAbbreviate()
      << " is prime using Miller-Rabin test " << i + 1 << " out of "
      << millerRabinTries << ". ";
      theReport.report(reportStream.str());
    }
    if (!this->isPossiblyPrimeMillerRabinOnce(
      aFewPrimes[i],
      theExponentOfThePowerTwoFactorOfNminusOne,
      theOddFactorOfNminusOne,
      comments
    )) {
      return false;
    }
  }
  return true;
}

void LargeIntegerUnsigned::getPrimesEratosthenesSieve(
  unsigned int primesUpToInclusive, List<unsigned int>& output
) {
  List<int> theSieve;
  theSieve.initializeFillInObject(static_cast<signed>(primesUpToInclusive) + 1, 1);
  output.reserve(primesUpToInclusive / 2);
  output.size = 0;
  for (unsigned int i = 2; i <= primesUpToInclusive; i ++) {
    if (theSieve[static_cast<signed>(i)] == 0) {
      continue;
    }
    output.addOnTop(i);
    for (unsigned int j = i; j <= primesUpToInclusive; j += i) {
      theSieve[static_cast<signed>(j)] = 0;
    }
  }
}

LargeIntegerUnsigned LargeIntegerUnsigned::getOne() {
  LargeIntegerUnsigned tempI;
  tempI.makeOne();
  return tempI;
}

bool LargeIntegerUnsigned::operator<(int other) const {
  if (other < 0) {
    return false;
  }
  LargeIntegerUnsigned tempUI;
  tempUI = static_cast<unsigned>(other);
  return *this < tempUI;
}

bool LargeIntegerUnsigned::operator>(int other) const {
  if (other < 0) {
    return true;
  }
  LargeIntegerUnsigned tempUI;
  tempUI = static_cast<unsigned>(other);
  return *this > tempUI;
}

bool LargeIntegerUnsigned::operator<(const LargeIntegerUnsigned& other) const {
  return !this->isGreaterThanOrEqualTo(other);
}

bool LargeIntegerUnsigned::operator<(const LargeInteger& other) const {
  if (other.isNegative()) {
    return false;
  }
  return (*this) < other.value;
}

bool LargeIntegerUnsigned::operator>=(const LargeIntegerUnsigned& other) const {
  return this->isGreaterThanOrEqualTo(other);
}

bool LargeIntegerUnsigned::operator>(const LargeIntegerUnsigned& other) const {
  return other < *this;
}

bool LargeIntegerUnsigned::isEven() const {
  if (LargeIntegerUnsigned::CarryOverBound % 2 == 0) {
    bool result = (this->theDigits[0] % 2) == 0;
    return result;
  }
  return ((*this) % 2) == 0;
}

bool LargeIntegerUnsigned::operator==(const LargeIntegerUnsigned& other) const {
  return this->theDigits == other.theDigits;
}

bool LargeIntegerUnsigned::operator!=(const LargeIntegerUnsigned& other) const {
  return ! ((*this) == other);
}

void LargeIntegerUnsigned::operator--(int) {
  if (this->isEqualToZero()) {
    global.fatal << "This is a programming error: attempting to subtract "
    << "1 from a large unsigned integer with value 0. " << global.fatal;
  }
  this->subtractSmallerPositive(1);
}

void LargeIntegerUnsigned::operator%=(const LargeIntegerUnsigned& other) {
  if (&other == this) {
    this->makeZero();
    return;
  }
  LargeIntegerUnsigned copyMe = *this;
  LargeIntegerUnsigned temp1;
  copyMe.divPositive(other, temp1, *this);
}

void LargeIntegerUnsigned::operator/=(const LargeIntegerUnsigned& other) {
  if (&other == this) {
    this->makeOne();
    return;
  }
  LargeIntegerUnsigned copyMe = *this;
  LargeIntegerUnsigned temp1;
  copyMe.divPositive(other, *this, temp1);
}

void LargeIntegerUnsigned::toString(std::string& output) const {
  if (this->isEqualToZero()) {
    output = "0";
    return;
  }
  if (this->theDigits.size > 1) {
    this->toStringLargeElementDecimal(output);
    return;
  }
  std::stringstream out;
  out << this->theDigits[0];
  output = out.str();
}

bool LargeIntegerUnsigned::isDivisibleBy(const LargeIntegerUnsigned& divisor) {
  if (divisor > *this || this->isEqualToZero()) {
    return false;
  }
  LargeIntegerUnsigned quotient, remainder;
  this->divPositive(divisor, quotient, remainder);
  return remainder.isEqualToZero();
}

void LargeIntegerUnsigned::divPositive(
  const LargeIntegerUnsigned& divisor,
  LargeIntegerUnsigned& quotientOutput,
  LargeIntegerUnsigned& remainderOutput
) const {
  if (divisor.isEqualToZero()) {
    global.fatal << "Division by zero. " << global.fatal;
  }
  if (divisor.theDigits.size == 1 && this->theDigits.size == 1) {
    unsigned quotientDigit = static_cast<unsigned>(this->theDigits[0] / divisor.theDigits[0]);
    unsigned remainderDigit = static_cast<unsigned>(this->theDigits[0] % divisor.theDigits[0]);
    quotientOutput.assignShiftedUInt(quotientDigit, 0);
    remainderOutput.assignShiftedUInt(remainderDigit, 0);
    return;
  }
  if (
    &quotientOutput == &divisor ||
    &quotientOutput == this ||
    &remainderOutput == &divisor ||
    &remainderOutput == this
  ) {
    LargeIntegerUnsigned divisorCopy = divisor;
    LargeIntegerUnsigned thisCopy = *this;
    thisCopy.divPositive(divisorCopy, quotientOutput, remainderOutput);
    return;
  }
  remainderOutput = *this;
  quotientOutput.makeZero();
  int currentQuotientDigit = 0;
  int divisorLeadingDigit = *divisor.theDigits.lastObject();
  int lastRemainderSize = - 1;
  int numRunsNoDigitImprovement = 0;
  LargeIntegerUnsigned remainderBackup;
  int upperlimitNoImprovementRounds = this->SquareRootOfCarryOverBound * 2;
  while (remainderOutput.isGreaterThanOrEqualTo(divisor)) {
    int quotientDigitIndex = remainderOutput.theDigits.size - divisor.theDigits.size;
    long long remainderLeadingDigit = *remainderOutput.theDigits.lastObject();
    int divisorLeadingDigitPlusSlack = divisorLeadingDigit;
    for (;;) {
      if (remainderLeadingDigit < divisorLeadingDigitPlusSlack) {
        quotientDigitIndex --;
        remainderLeadingDigit *= LargeIntegerUnsigned::CarryOverBound;
        if (remainderOutput.theDigits.size > 1) {
          remainderLeadingDigit += remainderOutput.theDigits[remainderOutput.theDigits.size - 2];
        }
      }
      currentQuotientDigit = static_cast<int>(remainderLeadingDigit / divisorLeadingDigitPlusSlack);
      ///////////////////////////////////////////////////////////
      remainderBackup = remainderOutput;
      remainderOutput.addLargeIntegerUnsignedShiftedTimesDigit(divisor, quotientDigitIndex, - currentQuotientDigit);
      if (*remainderOutput.theDigits.lastObject() >= 0) {
        quotientOutput.addShiftedUIntSmallerThanCarryOverBound(static_cast<unsigned>(currentQuotientDigit), quotientDigitIndex);
        break;
      }
      remainderOutput = remainderBackup;
      divisorLeadingDigitPlusSlack ++;
      if (divisorLeadingDigitPlusSlack > divisorLeadingDigit + 1) {
        global.fatal << "Bad division algorithm: could not figure out currentQuotientDigit for more than 2 runs. " << global.fatal;
      }
    }
    if (lastRemainderSize == remainderOutput.theDigits.size && lastRemainderSize != 1) {
      numRunsNoDigitImprovement ++;
    } else {
      numRunsNoDigitImprovement = 0;
    }
    lastRemainderSize = remainderOutput.theDigits.size;
    if (numRunsNoDigitImprovement > upperlimitNoImprovementRounds) {
      global.fatal << "Bad division: while dividing " << this->toString() << " by "
      << divisor.toString()
      << " got too many algorithm steps without remainder size decrease. "
      << "." << global.fatal;
    }
  }
}

std::string LargeIntegerUnsigned::toString(FormatExpressions* theFormat) const {
  (void) theFormat; //to avoid unused paramater warning
  std::string tempS;
  this->toString(tempS);
  return tempS;
}

void LargeIntegerUnsigned::toStringLargeElementDecimal(std::string& output) const {
  std::stringstream out;
  if (this->CarryOverBound == 1000000000  || this->CarryOverBound == 10) {
    std::string tempS;
    int numZeroesInCarryOver = 9;
    if (this->CarryOverBound == 10) {
      numZeroesInCarryOver = 1;
    }
    if (*this->theDigits.lastObject() < 0) {
      out << "[";
    }
    out << (*this->theDigits.lastObject());
    if (*this->theDigits.lastObject() < 0) {
      out << "]";
    }
    for (int i = this->theDigits.size - 2; i >= 0; i --) {
      std::stringstream tempStream;
      tempStream << this->theDigits[i];
      tempS = tempStream.str();
      if (tempS[0] == '-') {
        out << "[";
      }
      int numZeroesToPad = numZeroesInCarryOver - static_cast<signed>(tempS.length());
      for (int j = 0; j < numZeroesToPad; j ++) {
        out << "0";
      }
      out << tempS;
      if (tempS[0] == '-') {
        out << "]";
      }
    }
    output = out.str();
    return;
  }
  unsigned int base = 10;
  int MaxNumIntegersPerCarryOverBound = 11;
  List<LargeIntegerUnsigned> bufferPowersOfBase;
  int initialNumDigitsEstimate = MaxNumIntegersPerCarryOverBound*this->theDigits.size;
  int sizeBufferPowersOfBase = MathRoutines::minimum(initialNumDigitsEstimate, 10000);
  bufferPowersOfBase.setSize(sizeBufferPowersOfBase);
  LargeIntegerUnsigned currentPower;
  LargeIntegerUnsigned Remainder = *this;
  int numRemainingDigits;
  while (!Remainder.isEqualToZero()) {
    currentPower.makeOne();
    numRemainingDigits = 0;
    int highestBufferIndex = - 1;
    bufferPowersOfBase.objects[0].makeOne();
    bool bufferFilled = false;
    while (Remainder.isGreaterThanOrEqualTo(currentPower)) {
      numRemainingDigits ++;
      highestBufferIndex ++;
      highestBufferIndex %= sizeBufferPowersOfBase;
      bufferPowersOfBase.objects[highestBufferIndex] = currentPower;
      if (highestBufferIndex == sizeBufferPowersOfBase - 1) {
        bufferFilled = true;
      }
      currentPower *= base;
    }
    int startIndex = highestBufferIndex;
    do {
      currentPower = bufferPowersOfBase.objects[highestBufferIndex];
      unsigned int theDigit = 0;
      while (Remainder.isGreaterThanOrEqualTo(currentPower)) {
        theDigit ++;
        currentPower += bufferPowersOfBase[highestBufferIndex];
      }
      out << theDigit;
      numRemainingDigits --;
      if (theDigit != 1) {
        bufferPowersOfBase[highestBufferIndex] *= theDigit;
      }
      Remainder.subtractSmallerPositive(bufferPowersOfBase[highestBufferIndex]);
      highestBufferIndex --;
      if (highestBufferIndex == - 1) {
        highestBufferIndex = sizeBufferPowersOfBase - 1;
        if (!bufferFilled) {
          break;
        }
      }
    } while (highestBufferIndex != startIndex);
  }
  for (int i = 0; i < numRemainingDigits; i ++) {
    out << "0";
  }
  output = out.str();
}

LargeIntegerUnsigned::LargeIntegerUnsigned() {
  this->theDigits.setSize(1);
  this->theDigits[0] = 0;
}

LargeIntegerUnsigned::LargeIntegerUnsigned(unsigned int x) {
  this->assignShiftedUInt(x, 0);
}

LargeIntegerUnsigned::LargeIntegerUnsigned(const LargeIntegerUnsigned& x) {
  (*this) = x;
}

void LargeIntegerUnsigned::addShiftedUIntSmallerThanCarryOverBound(unsigned int x, int shift) {
  if (!(x < LargeIntegerUnsigned::CarryOverBound)) {
    global.fatal << "Digit too large. " << global.fatal;
  }
  while (x > 0) {
    if (shift >= this->theDigits.size) {
      int oldsize = this->theDigits.size;
      this->theDigits.setSize(shift + 1);
      for (int i = oldsize; i < this->theDigits.size; i ++) {
        this->theDigits[i] = 0;
      }
    }
    this->theDigits[shift] += x;
    if (this->theDigits[shift] >= LargeIntegerUnsigned::CarryOverBound) {
      this->theDigits[shift] -= LargeIntegerUnsigned::CarryOverBound;
      x = 1;
      shift ++;
    } else {
      x = 0;
    }
  }
}

unsigned int LargeIntegerUnsigned::logarithmBaseNCeiling(unsigned int theBase) const {
  if (this->isEqualToZero()) {
    return 0;
  }
  if (theBase <= 1) {
    global.fatal << "Base of logarithm needs to be larger than one. " << global.fatal;
  }
  LargeIntegerUnsigned current;
  List<LargeIntegerUnsigned> baseRaisedTo2ToPowerIndex;
  List<unsigned int> powersOfTwo;
  current = theBase;
  baseRaisedTo2ToPowerIndex.addOnTop(current);
  powersOfTwo.addOnTop(1);
  while(true) {
    current *= current;
    if (current > *this) {
      break;
    }
    baseRaisedTo2ToPowerIndex.addOnTop(current);
    powersOfTwo.addOnTop(*powersOfTwo.lastObject() * 2);
  }
  unsigned int result = *powersOfTwo.lastObject();
  current = *baseRaisedTo2ToPowerIndex.lastObject();
  LargeIntegerUnsigned next;
  for (int i = baseRaisedTo2ToPowerIndex.size - 2; i >= 0; i --) {
    next = current * baseRaisedTo2ToPowerIndex[i];
    if (next > *this) {
      continue;
    }
    result += powersOfTwo[i];
    current = next;
  }
  return result;
}

void LargeIntegerUnsigned::assignUInt64(uint64_t x) {
  this->theDigits.setSize(0);
  while (x > 0) {
    uint64_t nextDigit = x % LargeIntegerUnsigned::CarryOverBound;
    this->theDigits.addOnTop(static_cast<int>(nextDigit));
    x /= LargeIntegerUnsigned::CarryOverBound;
  }
}

void LargeIntegerUnsigned::assignUnsignedInt(unsigned int x) {
  this->assignShiftedUInt(x, 0);
}

void LargeIntegerUnsigned::assignShiftedUInt(unsigned int x, int shift) {
  if (x == 0) {
    this->makeZero();
    return;
  }
  this->theDigits.initializeFillInObject(shift, 0);
  while (x != 0) {
    unsigned int tempX = x % LargeIntegerUnsigned::CarryOverBound;
    this->theDigits.addOnTop(static_cast<int>(tempX));
    x = x / LargeIntegerUnsigned::CarryOverBound;
  }
}

void LargeIntegerUnsigned::addNoFitSize(const LargeIntegerUnsigned& x) {
  MacroIncrementCounter(Rational::totalLargeAdditions);
  int oldsize = this->theDigits.size;
  this->theDigits.setSize(MathRoutines::maximum(this->theDigits.size, x.theDigits.size) + 1);
  for (int i = oldsize; i < this->theDigits.size; i ++) {
    this->theDigits[i] = 0;
  }
  int CarryOver = 0;
  for (int i = 0; i < x.theDigits.size; i ++) {
    this->theDigits[i] += x.theDigits[i] + CarryOver;
    if (this->theDigits[i] >= LargeIntegerUnsigned::CarryOverBound) {
      this->theDigits[i] -= LargeIntegerUnsigned::CarryOverBound;
      CarryOver = 1;
    } else {
      CarryOver = 0;
    }
  }
  if (CarryOver != 0) {
    for (int i = x.theDigits.size; i < this->theDigits.size; i ++) {
      this->theDigits[i] += 1;
      if (this->theDigits[i] >= LargeIntegerUnsigned::CarryOverBound) {
        this->theDigits[i] -= LargeIntegerUnsigned::CarryOverBound;
      } else {
        break;
      }
    }
  }
}

void LargeIntegerUnsigned::operator+=(const LargeIntegerUnsigned& x) {
  this->addNoFitSize(x);
  this->fitSize();
}

LargeIntegerUnsigned LargeIntegerUnsigned::operator-(const LargeIntegerUnsigned& other) const {
  LargeIntegerUnsigned result;
  result = *this;
  result.subtractSmallerPositive(other);
  return result;
}

std::string LargeIntegerUnsigned::toStringAbbreviate(FormatExpressions *theFormat) const {
  (void) theFormat;
  std::string result = this->toString(theFormat);
  if (result.size() > 100) {
    std::stringstream out;
    out << result.substr(0, 40)
    << "...(" << (result.size() - 80) << " omitted)..."
    << result.substr(result.size() - 40);
    result = out.str();
  }
  return result;
}

void LargeIntegerUnsigned::padWithZeroesToAtLeastNDigits(int desiredMinNumDigits) {
  if (this->theDigits.size >= desiredMinNumDigits) {
    return;
  }
  int i = this->theDigits.size;
  this->theDigits.setSize(desiredMinNumDigits);
  for (; i < desiredMinNumDigits; i ++) {
    this->theDigits[i] = 0;
  }
}

void LargeIntegerUnsigned::addLargeIntegerUnsignedShiftedTimesDigit(const LargeIntegerUnsigned& other, int digitShift, int theConst) {
  if (theConst >= this->CarryOverBound || (- theConst) <= (- this->CarryOverBound)) {
    global.fatal << "Digit: " << theConst << " is too large" << global.fatal;
  }
  int numDigits = MathRoutines::maximum(other.theDigits.size + 1 + digitShift, this->theDigits.size + 1);
  this->padWithZeroesToAtLeastNDigits(numDigits);
  long long nextDigit = 0;
  for (int j = 0; j < other.theDigits.size; j ++) {
    int currentIndex = j + digitShift;
    int nextIndex = currentIndex + 1;
    nextDigit = other.theDigits[j];
    nextDigit *= theConst;
    nextDigit += this->theDigits[currentIndex];
    this->theDigits[currentIndex] = (nextDigit % this->CarryOverBound);
    this->theDigits[nextIndex] += (nextDigit / this->CarryOverBound);
    if (this->theDigits[currentIndex] < 0) {
      this->theDigits[currentIndex] += this->CarryOverBound;
      this->theDigits[nextIndex] --;
    }
    if (this->theDigits[currentIndex] < 0) {
      global.fatal << "Non-positive non-leading digit." << global.fatal;
    }
  }
  this->fitSize();
  int lastDigit =* this->theDigits.lastObject();
  if (lastDigit >= this->CarryOverBound || (- lastDigit) <= (- this->CarryOverBound)) {
    global.fatal << "Leading digit: " << lastDigit << " is too large" << global.fatal;
  }
}

void LargeIntegerUnsigned::subtractSmallerPositive(const LargeIntegerUnsigned& x) {
  int CarryOver = 0;
  for (int i = 0; i < x.theDigits.size; i ++) {
    int nextDigit = x.theDigits[i] + CarryOver;
    if (this->theDigits[i] < nextDigit) {
      this->theDigits[i] += LargeIntegerUnsigned::CarryOverBound;
      this->theDigits[i] -= nextDigit;
      CarryOver = 1;
    } else {
      this->theDigits[i] -= nextDigit;
      CarryOver = 0;
    }
  }
  if (CarryOver != 0) {
    for (int i = x.theDigits.size; i < this->theDigits.size; i ++) {
      if (this->theDigits[i] > 0) {
        this->theDigits[i] --;
        break;
      } else {
        this->theDigits[i] = LargeIntegerUnsigned::CarryOverBound - 1;
      }
    }
  }
  this->fitSize();
}

void LargeIntegerUnsigned::multiplyBy(const LargeIntegerUnsigned& x, LargeIntegerUnsigned& output) const {
  if (this == &output || &x == &output) {
    LargeIntegerUnsigned thisCopy = *this;
    LargeIntegerUnsigned xCopy = x;
    return thisCopy.multiplyBy(xCopy, output);
  }
  MacroIncrementCounter(Rational::totalLargeMultiplications);
  output.theDigits.setSize(x.theDigits.size + this->theDigits.size);
  for (int i = 0; i < output.theDigits.size; i ++) {
    output.theDigits[i] = 0;
  }
  unsigned long long numCycles = 0;
  bool doReport = false;
  signed ticksPerReport = 1024;
  unsigned long long totalCycles = static_cast<unsigned long long>(this->theDigits.size) * static_cast<unsigned long long>(x.theDigits.size);
  MemorySaving<ProgressReport> report1, report2;
  if (totalCycles >= static_cast<unsigned>(ticksPerReport)) {
    doReport = true;
    std::stringstream reportStream;
    reportStream
    << "<br>Large integer multiplication: product of integers:<br>\n"
    << this->toStringAbbreviate()
    << "<br>\n" << x.toStringAbbreviate();
    report1.getElement().report(reportStream.str());
    report2.getElement().ticksPerReport = ticksPerReport;
  }
  for (int i = 0; i < this->theDigits.size; i ++) {
    for (int j = 0; j < x.theDigits.size; j ++) {
      unsigned long long tempLong = static_cast<unsigned>(this->theDigits[i]);
      unsigned long long tempLong2 = static_cast<unsigned>(x.theDigits[j]);
      tempLong = tempLong * tempLong2;
      unsigned long long lowPart = tempLong % LargeIntegerUnsigned::CarryOverBound;
      unsigned long long highPart = tempLong / LargeIntegerUnsigned::CarryOverBound;
      output.addShiftedUIntSmallerThanCarryOverBound(static_cast<unsigned int>(lowPart), i + j);
      output.addShiftedUIntSmallerThanCarryOverBound(static_cast<unsigned int>(highPart), i + j + 1);
      if (doReport) {
        if (report2.getElement().tickAndWantReport()) {
          std::stringstream out;
          out << "<br>Crunching " << numCycles << " out of " << totalCycles
          << " pairs of large integer ``digits'' = "
          << this->theDigits.size << " x " << x.theDigits.size
          << " digits (base " << LargeIntegerUnsigned::CarryOverBound << ").";
          report2.getElement().report(out.str());
        }
      }
    }
  }
  output.fitSize();
}

LargeIntegerUnsigned LargeIntegerUnsigned::operator%(const LargeIntegerUnsigned& other) const {
  LargeIntegerUnsigned result, temp;
  this->divPositive(other, temp, result);
  return result;
}

int LargeIntegerUnsigned::getUnsignedIntValueTruncated() {
  return this->theDigits[0];
}

double LargeIntegerUnsigned::getDoubleValue() const {
  double result = 0;
  for (int i = this->theDigits.size - 1; i >= 0; i --) {
    result = result * LargeIntegerUnsigned::CarryOverBound + this->theDigits[i];
  }
  return result;
}

LargeIntegerUnsigned LargeIntegerUnsigned::lcm(const LargeIntegerUnsigned& a, const LargeIntegerUnsigned& b) {
  LargeIntegerUnsigned output;
  LargeIntegerUnsigned::lcm(a, b, output);
  return output;
}

LargeIntegerUnsigned LargeIntegerUnsigned::gcd(const LargeIntegerUnsigned& a, const LargeIntegerUnsigned& b) {
  LargeIntegerUnsigned output;
  LargeIntegerUnsigned::gcd(a, b, output);
  return output;
}

void LargeIntegerUnsigned::gcd(
  const LargeIntegerUnsigned& a,
  const LargeIntegerUnsigned& b,
  LargeIntegerUnsigned& output
) {
  MacroIncrementCounter(Rational::totalLargeGreatestCommonDivisors);
  LargeIntegerUnsigned p, q, r, temp;
  p = a;
  q = b;
  while (!q.isEqualToZero()) {
    p.divPositive(q, temp, r);
    p = q;
    q = r;
  }
  output = p;
}

void LargeIntegerUnsigned::fitSize() {
  int newSize = this->theDigits.size;
  for (int i = this->theDigits.size - 1; i >= 1; i --) {
    if (this->theDigits[i] == 0) {
      newSize --;
    } else {
      break;
    }
  }
  this->theDigits.setSize(newSize);
}

void LargeIntegerUnsigned::accountFactor(
  const LargeInteger& prime,
  List<LargeInteger>& outputPrimeFactors,
  List<int>& outputMultiplicities
) {
  if (outputPrimeFactors.size == 0) {
    outputPrimeFactors.addOnTop(prime);
    outputMultiplicities.addOnTop(1);
    return;
  }
  if ((*outputPrimeFactors.lastObject()).operator==(prime)) {
    (*outputMultiplicities.lastObject()) ++;
  } else {
    outputPrimeFactors.addOnTop(prime);
    outputMultiplicities.addOnTop(1);
  }
}

int LargeIntegerUnsigned::maximumDivisorToTryWhenFactoring(int desiredByUser) const {
  int result = desiredByUser;
  if (result <= 0) {
    result = 100000;
  }
  if (*this < result) {
    if (*this < 100) {
      result = 10;
    } else if (*this < 1000) {
      result = 32;
    } else if (*this < 10000) {
      result = 100;
    } else {
      this->isIntegerFittingInInt(&result);
    }
  }
  if (result > 1000000000 || result < 0) {
    result = 1000000000;
  }
  return result;
}

bool LargeIntegerUnsigned::factor(
  List<LargeInteger>& outputFactors,
  List<int>& outputMultiplicites,
  int maximumDivisorToTry,
  int numberMillerRabinRuns,
  std::stringstream* commentsOnFailure
) const {
  MacroRegisterFunctionWithName("LargeIntUnsigned::factor");
  int maximumNumberOfDigits = 1000;
  if (this->theDigits.size > maximumNumberOfDigits) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Number has too many digits: maximum "
      << maximumNumberOfDigits
      << " digits accepted for factorization attempt. ";
    }
    return false;
  }
  if (this->isEqualToZero()) {
    global.fatal << "Factoring zero is forbidden. "
    << global.fatal;
  }
  LargeIntegerUnsigned toBeFactored = *this;
  outputFactors.setSize(0);
  outputMultiplicites.setSize(0);
  maximumDivisorToTry = this->maximumDivisorToTryWhenFactoring(maximumDivisorToTry);
  List<bool> theSieve;
  theSieve.initializeFillInObject(maximumDivisorToTry + 1, true);
  for (int i = 2; i <= maximumDivisorToTry; i ++) {
    if (!theSieve[i]) {
      continue;
    }
    //LargeIntUnsigned current = toBeFactored % i;
    unsigned candidate = static_cast<unsigned>(i);
    while (toBeFactored % candidate == 0) {
      this->accountFactor(i, outputFactors, outputMultiplicites);
      toBeFactored /= candidate;
    }
    for (int j = i; j <= maximumDivisorToTry; j += i) {
      theSieve[j] = false;
    }
  }
  if (toBeFactored > 1) {
    this->accountFactor(toBeFactored, outputFactors, outputMultiplicites);
    if (!toBeFactored.isPossiblyPrimeMillerRabin(numberMillerRabinRuns, nullptr)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "The largest remaining factor "
        << toBeFactored
        << " is known to be composite (Miller-Rabin test) "
        << "but I could not factor it. "
        << "I checked all factors smaller than or equal to: "
        << maximumDivisorToTry << ". ";
      }
      return false;
    }
  }
  return true;
}

void LargeIntegerUnsigned::lcm(
  const LargeIntegerUnsigned& a,
  const LargeIntegerUnsigned& b,
  LargeIntegerUnsigned& output
) {
  LargeIntegerUnsigned tempUI, tempUI2;
  if (a.isEqualToZero() || b.isEqualToZero()) {
    global.fatal << "This is a programming error: calling lcm on zero elements is not allowed. " << global.fatal;
  }
  LargeIntegerUnsigned::gcd(a, b, tempUI);
  a.multiplyBy(b, tempUI2);
  output = tempUI2;
  output.divPositive(tempUI, output, tempUI2);
  if (output.isEqualToZero()) {
    global.fatal << "Least common multiple not allowed to be zero. " << global.fatal;
  }
}

void LargeIntegerUnsigned::operator=(const LargeIntegerUnsigned& x) {
  this->theDigits = x.theDigits;
}

void LargeIntegerUnsigned::operator=(unsigned int x) {
  this->assignShiftedUInt(x, 0);
}

unsigned int LargeIntegerUnsigned::hashFunction() const {
  int numCycles = MathRoutines::minimum(this->theDigits.size, someRandomPrimesSize);
  unsigned int result = 0;
  for (int i = 0; i < numCycles; i ++) {
    result += static_cast<unsigned>(this->theDigits[i]) * someRandomPrimes[i];
  }
  return result;
}

bool LargeIntegerUnsigned::isPositive() const {
  return this->theDigits.size > 1 || this->theDigits[0] > 0;
}

bool LargeIntegerUnsigned::isEqualToOne() const {
  return this->theDigits.size == 1 && this->theDigits[0] == 1;
}

bool LargeIntegerUnsigned::isEqualToZero() const {
  return this->theDigits.size == 1 && this->theDigits[0] == 0;
}

void LargeIntegerUnsigned::assignFactorial(unsigned int x) {
  this->makeOne();
  List<unsigned int> primesBelowX;
  LargeIntegerUnsigned::getPrimesEratosthenesSieve(x, primesBelowX);
  LargeIntegerUnsigned tempInt, tempOne;
  tempOne.makeOne();
  for (int i = 0; i < primesBelowX.size; i ++) {
    unsigned int thePrime = primesBelowX.objects[i];
    unsigned int thePowerOfThePrime = 0;
    unsigned int currentPower = thePrime;
    do {
      thePowerOfThePrime += x / currentPower;
      currentPower *= thePrime;
    } while (currentPower <= x);
    tempInt.assignShiftedUInt(thePrime, 0);
    MathRoutines::raiseToPower(tempInt, thePowerOfThePrime, tempOne);
    *this *= tempInt;
  }
}

bool LargeInteger::isEven() const {
  return this->value.isEven();
}

void LargeInteger::writeToFile(std::fstream& output) {
  std::string tempS;
  this->toString(tempS);
  output << tempS;
}

void LargeInteger::readFromFile(std::fstream& input) {
  std::string tempS;
  input >> tempS;
  this->assignString(tempS);
}

void LargeInteger::assignString(const std::string& input) {
  bool success = this->assignStringFailureAllowed(input, nullptr);
  if (!success) {
    global.fatal
    << "LargeInt::assignString is not allowed to fail." << global.fatal;
  }
}

bool LargeInteger::assignStringFailureAllowed(
  const std::string& input, std::stringstream* commentsOnFailure
) {
  if (input.size() == 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Empty string is not interpretted as zero. ";
    }
    return false;
  }
  this->makeZero();
  unsigned int startingIndex = 0;
  if (input[0] == '-') {
    startingIndex = 1;
  }
  for (unsigned int i = startingIndex; i < input.size(); i ++) {
    int x = input[i] - '0';
    if (x < 0 || x > 9) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Encountered a digit character not between 0 and 9. ";
      }
      return false;
    }
    if (i > startingIndex) {
      this->value *= 10;
    }
    this->value += static_cast<unsigned>(x);
  }
  if (!this->isEqualToZero()) {
    if (input[0] == '-') {
      this->sign = - 1;
    }
  }
  return true;
}

void LargeInteger::raiseToPower(int thePower) {
  MathRoutines::raiseToPower(*this, thePower, LargeInteger(1));
}

bool LargeInteger::isIntegerFittingInInt(int* whichInt) {
  if (!this->value.isIntegerFittingInInt(whichInt)) {
    return false;
  }
  if (whichInt != nullptr) {
    *whichInt *= this->sign;
  }
  return true;
}

void LargeInteger::multiplyByInt(int x) {
  LargeInteger tempI;
  tempI.assignInteger(x);
  *this *= tempI;
}

LargeInteger LargeInteger::zero() {
  return LargeInteger::zeroStatic();
}

LargeInteger LargeInteger::zeroStatic() {
  LargeInteger result;
  return result;
}

bool LargeInteger::tryIsPower(
  bool& outputIsPower, LargeInteger& outputBase, int& outputPower
) const {
  if (this->sign == - 1) {
    return false;
  }
  return this->value.tryIsPower(outputIsPower, outputBase, outputPower);
}

bool LargeInteger::operator==(const LargeInteger& x) const {
  if (x.sign != this->sign) {
    if (x.isEqualToZero() && this->isEqualToZero()) {
      return true;
    } else {
      return false;
    }
  }
  return this->value == x.value;
}

bool LargeInteger::checkConsistensy() {
  if (this->sign != - 1 && this->sign != 1) {
    return false;
  }
  for (int i = 0; i < this->value.theDigits.size; i ++) {
    if (this->value.theDigits[i] >= LargeIntegerUnsigned::CarryOverBound) {
      return false;
    }
  }
  return true;
}

double LargeInteger::getDoubleValue() const {
  return this->sign * this->value.getDoubleValue();
}

void LargeInteger::toString(std::string& output) const {
  std::stringstream out;
  if (this->isEqualToZero()) {
    output.assign("0");
    return;
  }
  if (this->sign == - 1) {
    out << "-";
  }
  std::string tempS;
  this->value.toString(tempS);
  out << tempS;
  output = out.str();
}

void LargeInteger::assignUInt64(uint64_t x) {
  this->sign = 1;
  this->value.assignUInt64(x);
}

void LargeInteger::assignInt64(int64_t x) {
  this->sign = 1;
  if (x < 0) {
    this->sign = - 1;
    x = - x;
  }
  this->value.assignUInt64(static_cast<uint64_t>(x));
}

void LargeInteger::assignInteger(int x) {
  if (x == 0) {
    this->makeZero();
    return;
  }
  this->sign = 1;
  if (x < 0) {
    this->sign = - 1;
    x = - x;
  }
  this->value.assignShiftedUInt(static_cast<unsigned int>(x), 0);
}

bool LargeInteger::getDivisors(List<int>& output, bool includeNegative) {
  if (this->value.theDigits.size > 1) {
    return false;
  }
  int val = this->value.theDigits[0];
  if (val > 50000) {
    return false;
  }
  output.setSize(0);
  for (int i = 1; i <= val; i ++) {
    if (val % i == 0) {
      output.addOnTop(i);
      if (includeNegative) {
        output.addOnTop(- i);
      }
    }
  }
  return true;
}

void LargeInteger::addLargeIntUnsigned(const LargeIntegerUnsigned& x) {
  if (this->sign == 1) {
    this->value += x;
    return;
  }
  if (this->value.isGreaterThanOrEqualTo(x)) {
    this->value.subtractSmallerPositive(x);
  } else {
    LargeIntegerUnsigned tempI = x;
    tempI.subtractSmallerPositive(this->value);
    this->value = tempI;
    this->sign = 1;
  }
}

void LargeInteger::operator+=(const LargeInteger& x) {
  if (this->sign == x.sign) {
    this->value += x.value;
  } else {
    if (this->value.isGreaterThanOrEqualTo(x.value)) {
      this->value.subtractSmallerPositive(x.value);
    } else {
      LargeIntegerUnsigned tempI = this->value;
      this->value = x.value;
      this->value.subtractSmallerPositive(tempI);
      this->sign = x.sign;
    }
  }
}

void LargeInteger::makeZero() {
  this->value.makeZero();
  this->sign = 1;
}

void LargeInteger::operator=(const Rational& other) {
  if (!other.isInteger(this)) {
    global.fatal << "Attempt to convert implicitly rational number "
    << other.toString()
    << " to integer is not possible as the Rational number is not integral. " << global.fatal;
  }
}

void LargeInteger::operator=(const LargeInteger& x) {
  this->sign = x.sign;
  this->value = x.value;
}

LargeInteger LargeInteger::operator/(int x) const {
  LargeInteger result;
  LargeIntegerUnsigned remainder;
  LargeIntegerUnsigned tempX;
  int absX = x;
  signed char signX = 1;
  if (x < 0) {
    signX = - 1;
    absX = - absX;
  }
  tempX.assignShiftedUInt(static_cast<unsigned>(absX), 0);
  this->value.divPositive(tempX, result.value, remainder);
  result.sign = this->sign * signX;
  return result;
}

LargeInteger LargeInteger::operator/(LargeInteger& x) const {
  LargeInteger result;
  LargeInteger remainder;
  this->value.divPositive(x.value, result.value, remainder.value);
  result.sign = this->sign * x.sign;
  if (!result.checkConsistensy()) {
    global.fatal << "Large integer corrupt. " << global.fatal;
  }
  return result;
}

int LargeInteger::operator%(int x) {
  if (x <= 0) {
    global.fatal << "Bad mod operator output. " << global.fatal;
  }
  LargeIntegerUnsigned result;
  LargeIntegerUnsigned remainder;
  LargeIntegerUnsigned tempX;
  if (x < 0) {
    x = - x;
  }
  tempX.assignShiftedUInt(static_cast<unsigned>(x), 0);
  this->value.divPositive(tempX, result, remainder);
  if (remainder.theDigits.size == 0) {
    return 0;
  } else {
    if (this->sign == - 1) {
      return x - remainder.theDigits[0];
    } else {
      return remainder.theDigits[0];
    }
  }
}

int Rational::floorIfSmall() {
  if (this->extended == nullptr) {
    if (numeratorShort < 0) {
      if (denominatorShort != 1) {
        return (this->numeratorShort / this->denominatorShort) - 1;
      } else {
        return this->numeratorShort / this->denominatorShort;
      }
    } else {
      return this->numeratorShort / this->denominatorShort;
    }
  }
  global.fatal << "This piece of code should not be reached. " << global.fatal;
  return - 1;
}

void Rational::writeToFile(std::fstream& output) {
  output << this->toString();
}

void Rational::raiseToPower(int x) {
  Rational tempRat;
  tempRat.makeOne();
  if (x < 0) {
    x = - x;
    this->invert();
  }
  LargeInteger tempNum = this->getNumerator();
  LargeIntegerUnsigned oneLI;
  oneLI.makeOne();
  MathRoutines::raiseToPower(tempNum.value, x, oneLI);
  LargeIntegerUnsigned tempDen = this->getDenominator();
  MathRoutines::raiseToPower(tempDen, x, oneLI);
  char theSign = (this->isPositive() || x % 2 == 0) ? 1 : - 1;
  this->allocateExtended();
  this->extended->numerator.sign = theSign;
  this->extended->denominator = tempDen;
  this->extended->numerator.value = tempNum.value;
  this->shrinkExtendedPartIfPossible();
}

void Rational::invert() {
  if (this->extended == nullptr) {
    int tempI = this->denominatorShort;
    if (tempI <= 0) {
      global.fatal << "Denominator not allowed to be negative. " << global.fatal;
    }
    if (this->numeratorShort < 0) {
      this->denominatorShort = - this->numeratorShort;
      this->numeratorShort = - tempI;
    } else {
      this->denominatorShort = this->numeratorShort;
      this->numeratorShort = tempI;
    }
    return;
  }
  MathRoutines::swap(this->extended->denominator, this->extended->numerator.value);
}

void Rational::readFromFile(std::istream& input) {
  std::string tempS;
  input >> tempS;
  this->assignString(tempS);
}

void Rational::multiplyByInt(int x) {
  Rational tempRat;
  tempRat.assignInteger(x);
  this->multiplyBy(tempRat);
}

void Rational::multiplyBy(const Rational& r) {
  if (r.extended == nullptr && this->extended == nullptr) {
    if (this->tryToMultiplyQuickly(r.numeratorShort, r.denominatorShort)) {
      return;
    }
  }
  this->initializeExtendedFromShortIfNeeded();
  if (r.extended != nullptr) {
    this->extended->numerator *= r.extended->numerator;
    this->extended->denominator *= r.extended->denominator;
  } else {
    this->extended->numerator.multiplyByInt(r.numeratorShort);
    this->extended->denominator.multiplyByUInt( static_cast<unsigned int>(r.denominatorShort));
  }
  this->simplify();
}

Rational operator/(int left, const Rational& right) {
  Rational tempRat = left;
  tempRat /= right;
  return tempRat;
}

Rational operator-(const Rational& argument) {
  Rational tempRat;
  tempRat.assign(argument);
  tempRat.negate();
  return tempRat;
}

void Rational::multiplyByLargeInteger(LargeInteger& x) {
  this->initializeExtendedFromShortIfNeeded();
  this->extended->numerator *= x;
  this->simplify();
}

void Rational::multiplyByLargeIntUnsigned(LargeIntegerUnsigned& x) {
  this->initializeExtendedFromShortIfNeeded();
  this->extended->numerator.value.multiplyBy(x);
  this->simplify();
}

void Rational::divideBy(const Rational& r) {
  if (this == &r) {
    this->makeOne();
    return;
  }
  Rational tempRat = r;
  tempRat.invert();
  this->operator*=(tempRat);
}

Rational Rational::operator/(const Rational& right) const {
  Rational tempRat;
  tempRat.assign(*this);
  tempRat.divideBy(right);
  return tempRat;
}

Rational Rational::operator*(const Rational& right) const {
  Rational tempRat;
  tempRat.assign(*this);
  tempRat.multiplyBy(right);
  return tempRat;
}

Rational Rational::operator+(const Rational& right) const {
  Rational tempRat;
  tempRat.assign(*this);
  tempRat += right;
  return tempRat;
}

Rational Rational::operator-(const Rational& right) const {
  Rational tempRat;
  tempRat.assign(*this);
  tempRat -= right;
  return tempRat;
}

void Rational::assign(const Rational& r) {
  this->numeratorShort = r.numeratorShort;
  this->denominatorShort = r.denominatorShort;
  if (r.extended == nullptr) {
    if (this->extended == nullptr) {
      return;
    }
    this->freeExtended();
    return;
  }
  this->initializeExtendedFromShortIfNeeded();
  this->extended->numerator = r.extended->numerator;
  this->extended->denominator = r.extended->denominator;
}

void Rational::assignFractionalValue() {
  if (this->extended == nullptr) {
    if (this->numeratorShort == 0) {
      return;
    }
    if (this->denominatorShort == 1) {
      this->numeratorShort = 0;
      this->denominatorShort = 1;
      return;
    }
    this->numeratorShort = this->numeratorShort % this->denominatorShort;
    if (this->numeratorShort < 0) {
      this->numeratorShort += this->denominatorShort;
    }
    return;
  }
  if (this->isEqualToZero()) {
    return;
  }
  if (this->extended->denominator.isEqualToOne()) {
    this->makeZero();
    return;
  }
  LargeIntegerUnsigned newNum, tempI;
  this->extended->numerator.value.divPositive(this->extended->denominator, tempI, newNum);
  this->extended->numerator.value = newNum;
  if (this->extended->numerator.isNegative()) {
    this->extended->numerator.addLargeIntUnsigned(this->extended->denominator);
  }
  if (!this->extended->numerator.isPositiveOrZero()) {
    global.fatal << "Numerator must not be negative. " << global.fatal;
  }
  this->simplify();
}

void Rational::assignLargeIntUnsigned(const LargeIntegerUnsigned& other) {
  LargeInteger tempInt;
  tempInt.assignLargeIntUnsigned(other);
  this->assignLargeInteger(tempInt);
}

void Rational::assignLargeInteger(const LargeInteger& other){
  if (this->extended == nullptr) {
    this->extended = new LargeRationalExtended;
#ifdef AllocationLimitsSafeguard
GlobalStatistics::globalPointerCounter ++;
GlobalStatistics::checkPointerCounters();
#endif
  }
  this->extended->numerator = other;
  this->extended->denominator.makeOne();
  this->shrinkExtendedPartIfPossible();
}

void Rational::addInteger(int x) {
  Rational tempRat;
  tempRat.assignNumeratorAndDenominator(x, 1);
  this->operator+=(tempRat);
}

bool Rational::isGreaterThan(const Rational& r) const {
  Rational tempRat;
  tempRat.assign(*this);
  tempRat.subtract(r);
  return tempRat.isPositive();
}

void Rational::subtract(const Rational& r) {
  Rational temp;
  temp.assign(r);
  temp.negate();
  this->operator+=(temp);
}

bool Rational::getSquareRootIfRational(Rational& output) const {
  if (*this < 0) {
    return false;
  }
  LargeInteger theNum = this->getNumerator();
  LargeIntegerUnsigned theDen = this->getDenominator();
  List<LargeInteger> primeFactorsNum, primeFactorsDen;
  List<int> multsNum, multsDen;
  if (!theNum.value.factor(primeFactorsNum, multsNum, 0, 3, nullptr)) {
    return false;
  }
  if (!theDen.factor(primeFactorsDen, multsDen, 0, 3, nullptr)) {
    return false;
  }
  output = 1;
  Rational tempRat;
  for (int i = 0; i < primeFactorsNum.size; i ++) {
    if (multsNum[i] % 2 != 0) {
      return false;
    }
    tempRat = primeFactorsNum[i];
    int currentMult = multsNum[i];
    tempRat.raiseToPower(currentMult / 2);
    output *= tempRat;
  }
  for (int i = 0; i < primeFactorsDen.size; i ++) {
    if (multsDen[i] % 2 != 0) {
      return false;
    }
    tempRat = primeFactorsDen[i];
    int currentMult = multsDen[i];
    tempRat.raiseToPower(currentMult / 2);
    output /= tempRat;
  }
  return true;
}

bool Rational::tryToAddQuickly(int otherNumerator, int otherDenominator) {
  int otherNumeratorAbsoluteValue, thisNumAbs;
  if (this->denominatorShort <= 0 || otherDenominator <= 0) {
    global.fatal << "This is a programming error: trying to add corrupt rational number(s) with denominators "
    << this->denominatorShort << " and " << otherDenominator
    << ". The cause of the error should be in some of the calling functions. " << global.fatal;
  }
  if (otherNumerator < 0) {
    otherNumeratorAbsoluteValue = - otherNumerator;
  } else {
    otherNumeratorAbsoluteValue = otherNumerator;
  }
  if (this->numeratorShort < 0) {
    thisNumAbs = - this->numeratorShort;
  } else {
    thisNumAbs = this->numeratorShort;
  }
  if (
    this->extended != nullptr ||
    thisNumAbs >= LargeIntegerUnsigned::SquareRootOfCarryOverBound ||
    this->denominatorShort >= LargeIntegerUnsigned::SquareRootOfCarryOverBound ||
    otherNumeratorAbsoluteValue >= LargeIntegerUnsigned::SquareRootOfCarryOverBound ||
    otherDenominator >= LargeIntegerUnsigned::SquareRootOfCarryOverBound
  ) {
    return false;
  }
  int N = this->numeratorShort * otherDenominator + this->denominatorShort * otherNumerator;
  int D = this->denominatorShort * otherDenominator;
  if (N == 0) {
    this->numeratorShort = 0;
    this->denominatorShort = 1;
    MacroIncrementCounter(Rational::totalSmallAdditions);
    return true;
  }
  int tempGCD = 0;
  if (N > 0) {
    tempGCD = Rational::greatestCommonDivisor(N, D);
  } else {
    tempGCD = Rational::greatestCommonDivisor(- N, D);
  }
  this->numeratorShort = N / tempGCD;
  this->denominatorShort = D / tempGCD;
  MacroIncrementCounter(Rational::totalSmallAdditions);
  return true;
}

bool Rational::tryToMultiplyQuickly(int otherNumerator, int otherDenominator) {
  int otherNumeratorAbsoluteValue, thisNumAbs;
  if (this->denominatorShort <= 0 || otherDenominator <= 0) {
    if (denominatorShort == 0 || otherDenominator == 0) {
      global.fatal << "This is a programming error: division by zero. ";
    } else {
      global.fatal << "This is a programming error during rational number multiplication: "
      << "corrupt rational number denominator. ";
    }
    global.fatal << global.fatal;
  }
  if (otherNumerator < 0) {
    otherNumeratorAbsoluteValue = - otherNumerator;
  } else {
    otherNumeratorAbsoluteValue = otherNumerator;
  }
  if (this->numeratorShort < 0) {
    thisNumAbs = - this->numeratorShort;
  } else {
    thisNumAbs = this->numeratorShort;
  }
  if (
    this->extended != nullptr ||
    thisNumAbs >= LargeIntegerUnsigned::SquareRootOfCarryOverBound ||
    this->denominatorShort >= LargeIntegerUnsigned::SquareRootOfCarryOverBound ||
    otherNumeratorAbsoluteValue >= LargeIntegerUnsigned::SquareRootOfCarryOverBound ||
    otherDenominator >= LargeIntegerUnsigned::SquareRootOfCarryOverBound
  ) {
    return false;
  }
  int N = this->numeratorShort * otherNumerator;
  int D = this->denominatorShort * otherDenominator;
  if (N == 0) {
    this->numeratorShort = 0;
    this->denominatorShort = 1;
  } else {
    int tempGCD = 0;
    if (N > 0) {
      tempGCD = Rational::greatestCommonDivisor(N, D);
    } else {
      tempGCD = Rational::greatestCommonDivisor(- N, D);
    }
    this->numeratorShort = N / static_cast<signed int>(tempGCD);
    this->denominatorShort = D / tempGCD;
  }
  MacroIncrementCounter(Rational::totalSmallMultiplications);
  return true;
}

Rational Rational::scaleNormalizeIndex(
  List<Rational>& output, int indexNonZeroEntry
) {
  if (output.size == 0) {
    return 1;
  }
  LargeIntegerUnsigned denominatorLCM = output[0].getDenominator();
  LargeIntegerUnsigned numeratorGCD = output[0].getNumerator().value;
  for (int i = 1; i < output.size; i ++) {
    LargeIntegerUnsigned::lcm(
      denominatorLCM,
      output[i].getDenominator(),
      denominatorLCM
    );
    LargeIntegerUnsigned::gcd(
      numeratorGCD,
      output[i].getNumerator().value,
      numeratorGCD
    );
  }
  Rational result = denominatorLCM;
  result /= numeratorGCD;
  if (output[indexNonZeroEntry] < 0) {
    result *= - 1;
  }
  for (int i = 0; i < output.size; i ++) {
    output[i] *= result;
  }
  return result;
}

LargeIntegerUnsigned Rational::getDenominator() const {
  LargeIntegerUnsigned result;
  if (this->extended == nullptr) {
    unsigned int tempI = static_cast<unsigned int>(this->denominatorShort);
    result.assignShiftedUInt(tempI, 0);
  } else {
    result = (this->extended->denominator);
  }
  return result;
}

bool Rational::beginsWithMinus() {
  return this->isNegative();
}

LargeInteger Rational::getNumerator() const {
  LargeInteger result;
  if (this->extended == nullptr) {
    if (this->numeratorShort < 0) {
      result.value.assignShiftedUInt(static_cast<unsigned int>(- this->numeratorShort), 0);
    } else {
      result.value.assignShiftedUInt(static_cast<unsigned int>(this->numeratorShort), 0);
    }
  } else {
    result.value = this->extended->numerator.value;
  }
  result.sign = 1;
  if (this->isNegative()) {
    result.sign = - 1;
  }
  return result;
}

const Rational& Rational::getComplexConjugate() const {
  return *this;
}

bool Rational::getPrimeFactorsAbsoluteValue(
  List<LargeInteger>& numeratorPrimeFactors,
  List<int>& numeratorMultiplicities,
  List<LargeInteger>& denominatorPrimeFactors,
  List<int>& denominatorMultiplicities
) {
  MacroRegisterFunctionWithName("Rational::getPrimeFactorsAbsoluteValue");
  if (!this->getNumerator().value.factor(
    numeratorPrimeFactors, numeratorMultiplicities, 0, 3, nullptr
  )) {
    return false;
  }
  return this->getDenominator().factor(
    denominatorPrimeFactors, denominatorMultiplicities, 0, 3, nullptr
  );
}

void Rational::assignInteger(int x) {
  this->freeExtended();
  this->denominatorShort = 1;
  this->numeratorShort = x;
}

bool Rational::shrinkExtendedPartIfPossible() {
  if (this->extended == nullptr) {
    return true;
  }
  if (
    this->extended->numerator.value.theDigits.size > 1 ||
    this->extended->denominator.theDigits.size > 1 ||
    this->extended->numerator.value.theDigits[0] >= LargeIntegerUnsigned::SquareRootOfCarryOverBound ||
    this->extended->denominator.theDigits[0] >= LargeIntegerUnsigned::SquareRootOfCarryOverBound
  ) {
    return false;
  }
  this->numeratorShort = this->extended->numerator.getIntValueTruncated();
  this->denominatorShort = this->extended->denominator.getUnsignedIntValueTruncated();
  this->freeExtended();
  return true;
}

Rational Rational::factorial(int n) {
  if (n < 0) {
    global.fatal << "This is a programming error: taking factorial of the negative number " << n << ". " << global.fatal;
    return 0;
  }
  LargeIntegerUnsigned result;
  result.assignFactorial(static_cast<unsigned int>(n));
  Rational answer;
  answer = result;
  return answer;
}

Rational Rational::twoToTheNth(int n) {
  Rational result = 1;
  if (n>= 0) {
    for (int i = 0; i < n; i ++) {
      result.multiplyByInt(2);
    }
  } else {
    for (int i = 0; i > n; i --) {
      result.divideByInteger(2);
    }
  }
  return result;
}

Rational Rational::nChooseK(const Rational& n, int k) {
  Rational result;
  result.makeOne();
  for (int i = 0; i < k; i ++) {
    result *= n - i;
    result.divideByInteger(i + 1);
  }
  return result;
}

Rational Rational::NtoTheKth(int n, int k) {
  Rational result = n;
  result.raiseToPower(k);
  return result;
}

bool Rational::isInteger(LargeInteger* whichInteger) const {
  bool result = false;
  if (this->extended == nullptr) {
    result = (this->denominatorShort == 1);
    if (whichInteger != nullptr) {
      *whichInteger = this->numeratorShort;
    }
  } else {
    if (this->extended->denominator.isEqualToOne()) {
      result = true;
      if (whichInteger != nullptr) {
        *whichInteger = this->extended->numerator;
      }
    }
  }
  return result;
}

bool MathRoutines::isInteger(Rational x) {
  return x.isInteger();
}

double Rational::getDoubleValue() const {
  if (this->extended == nullptr) {
    return static_cast<double>(this->numeratorShort) / static_cast<double>(this->denominatorShort);
  } else {
    return this->extended->numerator.getDoubleValue() / this->extended->denominator.getDoubleValue();
  }
}

void Rational::simplify() {
  if (this->extended == nullptr) {
    if (this->numeratorShort == 0) {
      this->denominatorShort = 1;
    } else {
      if (this->denominatorShort == 1) {
        return;
      }
      int tempGCD;
      if (this->numeratorShort > 0) {
        tempGCD = this->greatestCommonDivisor(this->numeratorShort, this->denominatorShort);
      } else {
        tempGCD = this->greatestCommonDivisor(- this->numeratorShort, this->denominatorShort);
      }
      this->numeratorShort /= tempGCD;
      this->denominatorShort /= tempGCD;
    }
    return;
  }
  if (this->extended->numerator.isEqualToZero()) {
    this->makeZero();
    return;
  }
  if (!this->extended->denominator.isEqualToOne()) {
    LargeIntegerUnsigned tempI;
    LargeIntegerUnsigned::gcd(this->extended->denominator, this->extended->numerator.value, tempI);
    /*if (Rational::flagAnErrorHasOccurredTimeToPanic) {
      std::string tempS1, tempS2, tempS3;
      tempI.toString(tempS1);
      this->toString(tempS2);
    }*/
    LargeIntegerUnsigned tempI2;
    this->extended->denominator.divPositive(tempI, this->extended->denominator, tempI2);
    this->extended->numerator.value.divPositive(tempI, this->extended->numerator.value, tempI2);
  }
  this->shrinkExtendedPartIfPossible();
}

template <>
bool Polynomial<Rational>::isConstant(Rational* whichConstant) const;

void Rational::operator=(const Polynomial<Rational>& other) {
  if (!other.isConstant(this)) {
    global.fatal << "This is a programming error: attempting to assign "
    << "non-constant polynomial to a Rational number is not allowed. "
    << global.fatal;
  }
}

bool Rational::isEqualTo(const Rational& b) const {
  if (this->extended == nullptr && b.extended == nullptr) {
    return (this->numeratorShort * b.denominatorShort == b.numeratorShort * this->denominatorShort);
  }
  Rational tempRat;
  tempRat.assign(*this);
  tempRat.subtract(b);
  return tempRat.isEqualToZero();
}

bool Rational::isGreaterThanOrEqualTo(const Rational& right) const {
  if (this->extended == nullptr && right.extended == nullptr) {
    return (this->numeratorShort * right.denominatorShort >= right.numeratorShort * this->denominatorShort);
  }
  Rational tempRat;
  tempRat.assign(*this);
  tempRat.subtract(right);
  return tempRat.isPositiveOrZero();
}

std::string Rational::toString(FormatExpressions* theFormat) const {
  if (theFormat != nullptr) {
    if (theFormat->flagUseFrac) {
      return this->toStringFrac();
    }
  }
  std::stringstream out;
  if (this->extended == nullptr) {
    out << this->numeratorShort;
    if (this->denominatorShort != 1) {
      out << "/" << this->denominatorShort;
    }
  } else {
    std::string tempS;
    this->extended->numerator.toString(tempS);
    out << tempS;
    this->extended->denominator.toString(tempS);
    if (tempS != "1") {
      out << "/" << tempS;
    }
  }
  return out.str();
}

std::string Rational::toStringForFileOperations(FormatExpressions* notUsed) const {
  (void) notUsed; //portable way of avoiding unused parameter warning
  std::stringstream out;
  if (this->extended == nullptr) {
    if (this->numeratorShort < 0) {
      out << "-";
    }
    int numShortAbsoluteValue = this->numeratorShort < 0 ? - this->numeratorShort: this->numeratorShort;
    if (this->denominatorShort == 1) {
      out << numShortAbsoluteValue;
    } else {
      out << numShortAbsoluteValue << "_div_" << this->denominatorShort;
    }
    return out.str();
  }
  LargeInteger numAbsVal = this->extended->numerator;
  if (numAbsVal < 0) {
    out << "-";
    numAbsVal.sign = 1;
  }
  if (this->extended->denominator.isEqualToOne()) {
    out << numAbsVal.toString();
  } else {
    out << numAbsVal.toString() << "_div_" << this->extended->denominator.toString();
  }
  return out.str();
}

std::string Rational::toStringFrac() const {
  std::stringstream out;
  if (this->extended == nullptr) {
    if (this->numeratorShort < 0) {
      out << "-";
    }
    int numShortAbsoluteValue = this->numeratorShort < 0 ? - this->numeratorShort: this->numeratorShort;
    if (this->denominatorShort == 1) {
      out << numShortAbsoluteValue;
    } else {
      out << "\\frac{" << numShortAbsoluteValue << "}{" << this->denominatorShort << "}";
    }
    return out.str();
  }
  LargeInteger numAbsVal = this->extended->numerator;
  if (numAbsVal < 0) {
    out << "-";
    numAbsVal.sign = 1;
  }
  if (this->extended->denominator.isEqualToOne()) {
    out << numAbsVal.toString();
  } else {
    out << "\\frac{" << numAbsVal.toString() << "}{" << this->extended->denominator.toString() << "}";
  }
  return out.str();
}

void Rational::operator=(const AlgebraicNumber& other) {
  bool isGood = other.isRational(this);
  if (!isGood) {
    global.fatal << "This is a programming error: attempting to assign the "
    << "non-rational algebraic number " << other.toString()
    << "to a rational number. " << global.fatal;
  }
}

Rational Rational::one() {
  return 1;
}

Rational Rational::zero() {
  return 0;
}

Rational Rational::zeroStatic() {
  return 0;
}

bool Rational::assignStringFailureAllowed(const std::string& input) {
  this->makeZero();
  if (input == "") {
    return false;
  }
  if (input == "0") {
    return true;
  }
  int sign = 1;
  int ReaderDigit = - 1;
  Rational readerDen = 1;
  unsigned positionInString = 0;
  for (; positionInString < input.size(); positionInString ++) {
    if (input[positionInString] == '-') {
      sign = - 1;
      positionInString ++;
      break;
    }
    if (MathRoutines::isADigit(input[positionInString])) {
      break;
    }
    if (MathRoutines::isALatinLetter(input[positionInString])) {
      return false;
    }
  }
  for (; positionInString < input.size(); positionInString ++) {
    if (MathRoutines::isADigit(input[positionInString], &ReaderDigit)) {
      *this *= 10;
      *this += ReaderDigit;
    }
    if (input[positionInString] == '/') {
      positionInString ++;
      readerDen = 0;
      break;
    }
  }
  for (; positionInString < input.size(); positionInString ++) {
    if (MathRoutines::isADigit(input[positionInString], &ReaderDigit)) {
      readerDen *= 10;
      readerDen += ReaderDigit;
    }
  }
  if (readerDen.isEqualToZero()) {
    return false;
  }
  *this /= readerDen;
  *this *= sign;
  return true;
}

void Rational::assignString(const std::string& input) {
  if (!Rational::assignStringFailureAllowed(input)) {
    global.fatal << "Programming error: "
    << "Rational::assignString failed (likely a zero denominator). "
    << global.fatal;
  }
}
