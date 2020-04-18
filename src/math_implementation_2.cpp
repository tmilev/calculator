// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "general_lists.h"
#include "math_general_implementation.h"
#include "math_general.h"
#include "math_extra_algebraic_numbers.h"

void LargeIntegerUnsigned::AssignString(const std::string& input) {
  if (input.size() < 10) {
    unsigned int x = static_cast<unsigned>(std::atoi(input.c_str()));
    this->AssignShiftedUInt(x, 0);
    return;
  }
  this->makeZero();
  for (unsigned int i = 0; i < input.size(); i ++) {
    this->operator*=(10);
    int whichDigit = input[i] - '0';
    if (whichDigit > 9 || whichDigit < 0) {
      global.fatal << "This is a programming error: LargeIntUnsigned::AssignString "
      << "called on the string " << input
      << " which does not consist entirely of digits. "
      << "Please note that LargeIntUnsigned::AssignString is a no-fail function, intended for "
      << "internal use only. If you want to parse arbitrary unsafe "
      << "expressions coming from the user, please use the big gun (a.k.a. Calculator). "
      << global.fatal;
    }
    this->operator+=(static_cast<unsigned>(whichDigit));
  }
}

bool LargeIntegerUnsigned::AssignStringFailureAllowed(const std::string& input, bool ignoreNonDigits) {
  MacroRegisterFunctionWithName("LargeIntUnsigned::AssignStringFailureAllowed");
  if (input.size() > 10000000) {
    //<- sorry folks, no more than 10 million digits.
    return false;
  }
  if (input.size() < 10) {
    unsigned int x = static_cast<unsigned>(std::atoi(input.c_str()));
    this->AssignShiftedUInt(x, 0);
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
  this->MultiplyBy(x, result);
  return result;
}

LargeIntegerUnsigned LargeIntegerUnsigned::operator/(unsigned int x) const {
  LargeIntegerUnsigned result;
  LargeIntegerUnsigned remainder;
  LargeIntegerUnsigned tempX;
  tempX.AssignShiftedUInt(x, 0);
  this->DivPositive(tempX, result, remainder);
  return result;
}

LargeIntegerUnsigned LargeIntegerUnsigned::operator/(const LargeIntegerUnsigned& x) const {
  LargeIntegerUnsigned result;
  LargeIntegerUnsigned remainder;
  this->DivPositive(x, result, remainder);
  return result;
}

int LargeIntegerUnsigned::operator%(unsigned int x) {
  LargeIntegerUnsigned quotient;
  LargeIntegerUnsigned remainder;
  LargeIntegerUnsigned divisor;
  divisor.AssignShiftedUInt(x, 0);
  this->DivPositive(divisor, quotient, remainder);
  int result = 0;
  remainder.IsIntegerFittingInInt(&result);
  return result;
}

void LargeIntegerUnsigned::GetHexBigEndian(
  int numberOfLeadingZeroesToPadWith, std::string& output
) const {
  List<char> result;
  LargeIntegerUnsigned digit, inputCopy = *this;
  while (inputCopy > 0) {
    digit = static_cast<unsigned char>(inputCopy % 256);
    inputCopy /= 256;
    int digitInt = 0;
    digit.IsIntegerFittingInInt(&digitInt);
    char digitChar = static_cast<char>(digitInt);
    result.AddOnTop(digitChar);
  }
  for (int i = 0; i < numberOfLeadingZeroesToPadWith; i ++) {
    result.AddOnTop(0);
  }
  result.ReverseOrderElements();
  output.assign(result.TheObjects, static_cast<unsigned>(result.size));
}

bool LargeIntegerUnsigned::WriteBigEndianFixedNumberOfBytes(
  List<unsigned char>& outputAppend,
  int desiredNumberOfBytes,
  std::stringstream* commentsOnFailure
) const {
  int startOffset = outputAppend.size;
  this->WriteBigEndianBytes(outputAppend, 0);
  int zeroesToPad = desiredNumberOfBytes - outputAppend.size - startOffset;
  if (zeroesToPad > 0) {
    outputAppend.ShiftUpExpandOnTopRepeated(startOffset, zeroesToPad);
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

void LargeIntegerUnsigned::WriteBigEndianBytes(
  List<unsigned char>& outputAppend, bool leadingZeroPad
) const {
  LargeIntegerUnsigned remainder = *this;
  List<unsigned char> digitsReveredOrder;
  while (remainder > 0) {
    int nextDigit = remainder % 256;
    digitsReveredOrder.AddOnTop(static_cast<unsigned char>(nextDigit));
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
  outputAppend.Reserve(outputAppend.size + digitsReveredOrder.size + padWithZero);
  if (padWithZero == 1) {
    outputAppend.AddOnTop(0);
  }
  for (int i = digitsReveredOrder.size - 1; i >= 0; i --) {
    outputAppend.AddOnTop(digitsReveredOrder[i]);
  }
}

void LargeIntegerUnsigned::makeOne() {
  this->theDigits.SetSize(1);
  this->theDigits[0] = 1;
}

void LargeIntegerUnsigned::makeZero() {
  this->theDigits.SetSize(1);
  this->theDigits[0] = 0;
}

void LargeIntegerUnsigned::MultiplyByUInt(unsigned int x) {
  LargeIntegerUnsigned tempLI;
  tempLI.AssignShiftedUInt(x, 0);
  this->MultiplyBy(tempLI);
}

void LargeIntegerUnsigned::MultiplyBy(const LargeIntegerUnsigned& x) {
  LargeIntegerUnsigned tempInt;
  this->MultiplyBy(x, tempInt);
  *this = tempInt;
}

void LargeIntegerUnsigned::AddUInt(unsigned int x) {
  LargeIntegerUnsigned tempI;
  tempI.AssignShiftedUInt(x, 0);
  (*this) += tempI;
}

void LargeIntegerUnsigned::operator*=(const LargeIntegerUnsigned& right) {
  this->MultiplyBy(right);
}

void LargeIntegerUnsigned::operator*=(unsigned int x) {
  this->MultiplyByUInt(x);
}

void LargeIntegerUnsigned::operator+=(unsigned int x) {
  this->AddUInt(x);
}

LargeIntegerUnsigned LargeIntegerUnsigned::operator+(const LargeIntegerUnsigned& other) {
  LargeIntegerUnsigned result = *this;
  result += other;
  return result;
}

void LargeIntegerUnsigned::operator++(int) {
  this->AddUInt(1);
}

bool LargeIntegerUnsigned::IsIntegerFittingInInt(int* whichInt) {
  LargeIntegerUnsigned twoToThe31 = 2;
  MathRoutines::RaiseToPower(twoToThe31, 31, LargeIntegerUnsigned(1));
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

bool LargeIntegerUnsigned::IsGreaterThanOrEqualTo(const LargeIntegerUnsigned& x) const {
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

bool LargeIntegerUnsigned::IsPossiblyPrimeMillerRabinOnce(
  unsigned int theBase,
  int theExponentOfThePowerTwoFactorOfNminusOne,
  const LargeIntegerUnsigned& theOddFactorOfNminusOne,
  std::stringstream* comments
) {
  MacroRegisterFunctionWithName("LargeIntUnsigned::IsPossiblyPrimeMillerRabinOnce");
  if (*this == theBase) {
    return true;
  }
  ElementZmodP thePower, theOne;
  thePower.theModulus = *this;
  thePower.theValue = theBase;
  theOne.theModulus = *this;
  theOne.theValue = 1;
  MathRoutines::RaiseToPower(thePower, theOddFactorOfNminusOne, theOne);
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
        << theOddFactorOfNminusOne.toString() << "} = " << thePower.theValue.toString() << " ~ mod ~"
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
  return false;
}

bool LargeIntegerUnsigned::TryToFindWhetherIsPower(bool& outputIsPower, LargeInteger& outputBase, int& outputPower) const {
  MacroRegisterFunctionWithName("LargeIntUnsigned::TryToFindWhetherIsPower");
  List<LargeInteger> theFactors;
  List<int> theMults;
  if (!this->FactorReturnFalseIfFactorizationIncomplete(theFactors, theMults, 0, nullptr)) {
    return false;
  }
  if (theMults.size == 0) {
    outputIsPower = true;
    outputBase = 1;
    outputPower = 0;
    return true;
  }
  if (theMults[0] <= 1) {
    outputIsPower = false;
    return true;
  }
  for (int i = 1; i < theFactors.size; i ++) {
    if (theMults[i] != theMults[0]) {
      outputIsPower = false;
      return true;
    }
  }
  outputIsPower = true;
  outputBase = 1;
  for (int i = 0; i < theFactors.size; i ++) {
    outputBase *= theFactors[i];
  }
  outputPower = theMults[0];
  return true;
}

bool LargeIntegerUnsigned::IsPossiblyPrimeMillerRabiN(int numberOfTries, std::stringstream* comments) {
  return this->IsPossiblyPrime(numberOfTries, false, comments);
}

bool LargeIntegerUnsigned::IsCompositePrimeDivision(
  List<unsigned int>& primesGenerated, std::stringstream* comments
) {
  MacroRegisterFunctionWithName("LargeIntUnsigned::IsCompositePrimeDivision");
  if (this->IsEven()) {
    return *this == 2;
  }
  LargeIntegerUnsigned::getPrimesEratosthenesSieve(
    100000, primesGenerated
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

bool LargeIntegerUnsigned::IsPossiblyPrime(
  int timesToRunMillerRabin,
  bool tryDivisionSetTrueFaster,
  std::stringstream* comments
) {
  MacroRegisterFunctionWithName("LargeIntUnsigned::IsPossiblyPrime");
  List<unsigned int> aFewPrimes;
  if (tryDivisionSetTrueFaster) {
    if (this->IsCompositePrimeDivision(aFewPrimes, comments)) {
      return false;
    }
  } else {
    LargeIntegerUnsigned::getPrimesEratosthenesSieve(100000, aFewPrimes);
  }
  if (timesToRunMillerRabin > aFewPrimes.size) {
    timesToRunMillerRabin = aFewPrimes.size;
  }
  LargeIntegerUnsigned theOddFactorOfNminusOne = *this;
  int theExponentOfThePowerTwoFactorOfNminusOne = 0;
  theOddFactorOfNminusOne --;
  while (theOddFactorOfNminusOne.IsEven()) {
    theOddFactorOfNminusOne /= 2;
    theExponentOfThePowerTwoFactorOfNminusOne ++;
  }
  ProgressReport theReport;
  for (int i = 0; i < timesToRunMillerRabin; i ++) {
    if (theReport.TickAndWantReport()) {
      std::stringstream reportStream;
      reportStream << "Testing whether " << this->ToStringAbbreviate()
      << " is prime using Miller-Rabin test " << i + 1 << " out of "
      << timesToRunMillerRabin << ". ";
      theReport.Report(reportStream.str());
    }
    if (!this->IsPossiblyPrimeMillerRabinOnce(
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
  output.Reserve(primesUpToInclusive / 2);
  output.size = 0;
  for (unsigned int i = 2; i <= primesUpToInclusive; i ++) {
    if (theSieve[static_cast<signed>(i)] == 0) {
      continue;
    }
    output.AddOnTop(i);
    for (unsigned int j = i; j <= primesUpToInclusive; j += i) {
      theSieve[static_cast<signed>(j)] = 0;
    }
  }
}

LargeIntegerUnsigned LargeIntegerUnsigned::GetOne() {
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
  return !this->IsGreaterThanOrEqualTo(other);
}

bool LargeIntegerUnsigned::operator>=(const LargeIntegerUnsigned& other) const {
  return this->IsGreaterThanOrEqualTo(other);
}

bool LargeIntegerUnsigned::operator>(const LargeIntegerUnsigned& other) const {
  return other < *this;
}

bool LargeIntegerUnsigned::IsEven() const {
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
  if (this->IsEqualToZero()) {
    global.fatal << "This is a programming error: attempting to subtract "
    << "1 from a large unsigned integer with value 0. " << global.fatal;
  }
  this->SubtractSmallerPositive(1);
}

void LargeIntegerUnsigned::operator%=(const LargeIntegerUnsigned& other) {
  if (&other == this) {
    this->makeZero();
    return;
  }
  LargeIntegerUnsigned copyMe = *this;
  LargeIntegerUnsigned temp1;
  copyMe.DivPositive(other, temp1, *this);
}

void LargeIntegerUnsigned::operator/=(const LargeIntegerUnsigned& other) {
  if (&other == this) {
    this->makeOne();
    return;
  }
  LargeIntegerUnsigned copyMe = *this;
  LargeIntegerUnsigned temp1;
  copyMe.DivPositive(other, *this, temp1);
}

void LargeIntegerUnsigned::toString(std::string& output) const {
  if (this->IsEqualToZero()) {
    output = "0";
    return;
  }
  if (this->theDigits.size > 1) {
    this->ElementToStringLargeElementDecimal(output);
    return;
  }
  std::stringstream out;
  out << this->theDigits[0];
  output = out.str();
}

bool LargeIntegerUnsigned::isDivisibleBy(const LargeIntegerUnsigned& divisor) {
  if (divisor > *this || this->IsEqualToZero()) {
    return false;
  }
  LargeIntegerUnsigned quotient, remainder;
  this->DivPositive(divisor, quotient, remainder);
  return remainder.IsEqualToZero();
}

void LargeIntegerUnsigned::DivPositive(
  const LargeIntegerUnsigned& divisor,
  LargeIntegerUnsigned& quotientOutput,
  LargeIntegerUnsigned& remainderOutput
) const {
  if (divisor.IsEqualToZero()) {
    global.fatal << "Division by zero. " << global.fatal;
  }
  if (divisor.theDigits.size == 1 && this->theDigits.size == 1) {
    unsigned quotientDigit = static_cast<unsigned>(this->theDigits[0] / divisor.theDigits[0]);
    unsigned remainderDigit = static_cast<unsigned>(this->theDigits[0] % divisor.theDigits[0]);
    quotientOutput.AssignShiftedUInt(quotientDigit, 0);
    remainderOutput.AssignShiftedUInt(remainderDigit, 0);
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
    thisCopy.DivPositive(divisorCopy, quotientOutput, remainderOutput);
    return;
  }
  remainderOutput = *this;
  quotientOutput.makeZero();
  int currentQuotientDigit = 0;
  int divisorLeadingDigit = *divisor.theDigits.LastObject();
  int lastRemainderSize = - 1;
  int numRunsNoDigitImprovement = 0;
  LargeIntegerUnsigned remainderBackup;
  int upperlimitNoImprovementRounds = this->SquareRootOfCarryOverBound * 2;
  while (remainderOutput.IsGreaterThanOrEqualTo(divisor)) {
    int quotientDigitIndex = remainderOutput.theDigits.size - divisor.theDigits.size;
    long long remainderLeadingDigit = *remainderOutput.theDigits.LastObject();
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
      remainderOutput.AddLargeIntUnsignedShiftedTimesDigit(divisor, quotientDigitIndex, - currentQuotientDigit);
      if (*remainderOutput.theDigits.LastObject() >= 0) {
        quotientOutput.AddShiftedUIntSmallerThanCarryOverBound(static_cast<unsigned>(currentQuotientDigit), quotientDigitIndex);
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

void LargeIntegerUnsigned::ElementToStringLargeElementDecimal(std::string& output) const {
  std::stringstream out;
  if (this->CarryOverBound == 1000000000  || this->CarryOverBound == 10) {
    std::string tempS;
    int numZeroesInCarryOver = 9;
    if (this->CarryOverBound == 10) {
      numZeroesInCarryOver = 1;
    }
    if (*this->theDigits.LastObject() < 0) {
      out << "[";
    }
    out << (*this->theDigits.LastObject());
    if (*this->theDigits.LastObject() < 0) {
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
  int sizeBufferPowersOfBase = MathRoutines::Minimum(initialNumDigitsEstimate, 10000);
  bufferPowersOfBase.SetSize(sizeBufferPowersOfBase);
  LargeIntegerUnsigned currentPower;
  LargeIntegerUnsigned Remainder = *this;
  int numRemainingDigits;
  while (!Remainder.IsEqualToZero()) {
    currentPower.makeOne();
    numRemainingDigits = 0;
    int highestBufferIndex = - 1;
    bufferPowersOfBase.TheObjects[0].makeOne();
    bool bufferFilled = false;
    while (Remainder.IsGreaterThanOrEqualTo(currentPower)) {
      numRemainingDigits ++;
      highestBufferIndex ++;
      highestBufferIndex %= sizeBufferPowersOfBase;
      bufferPowersOfBase.TheObjects[highestBufferIndex] = currentPower;
      if (highestBufferIndex == sizeBufferPowersOfBase - 1) {
        bufferFilled = true;
      }
      currentPower *= base;
    }
    int startIndex = highestBufferIndex;
    do {
      currentPower = bufferPowersOfBase.TheObjects[highestBufferIndex];
      unsigned int theDigit = 0;
      while (Remainder.IsGreaterThanOrEqualTo(currentPower)) {
        theDigit ++;
        currentPower += bufferPowersOfBase[highestBufferIndex];
      }
      out << theDigit;
      numRemainingDigits --;
      if (theDigit != 1) {
        bufferPowersOfBase[highestBufferIndex] *= theDigit;
      }
      Remainder.SubtractSmallerPositive(bufferPowersOfBase[highestBufferIndex]);
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
  this->theDigits.SetSize(1);
  this->theDigits[0] = 0;
}

LargeIntegerUnsigned::LargeIntegerUnsigned(unsigned int x) {
  this->AssignShiftedUInt(x, 0);
}

LargeIntegerUnsigned::LargeIntegerUnsigned(const LargeIntegerUnsigned& x) {
  (*this) = x;
}

void LargeIntegerUnsigned::AddShiftedUIntSmallerThanCarryOverBound(unsigned int x, int shift) {
  if (!(x < LargeIntegerUnsigned::CarryOverBound)) {
    global.fatal << "Digit too large. " << global.fatal;
  }
  while (x > 0) {
    if (shift >= this->theDigits.size) {
      int oldsize = this->theDigits.size;
      this->theDigits.SetSize(shift + 1);
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

unsigned int LargeIntegerUnsigned::LogarithmBaseNCeiling(unsigned int theBase) const {
  if (this->IsEqualToZero()) {
    return 0;
  }
  if (theBase <= 1) {
    global.fatal << "Base of logarithm needs to be larger than one. " << global.fatal;
  }
  LargeIntegerUnsigned current;
  List<LargeIntegerUnsigned> baseRaisedTo2ToPowerIndex;
  List<unsigned int> powersOfTwo;
  current = theBase;
  baseRaisedTo2ToPowerIndex.AddOnTop(current);
  powersOfTwo.AddOnTop(1);
  while(true) {
    current *= current;
    if (current > *this) {
      break;
    }
    baseRaisedTo2ToPowerIndex.AddOnTop(current);
    powersOfTwo.AddOnTop(*powersOfTwo.LastObject() * 2);
  }
  unsigned int result = *powersOfTwo.LastObject();
  current = *baseRaisedTo2ToPowerIndex.LastObject();
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

void LargeIntegerUnsigned::AssignUInt64(uint64_t x) {
  this->theDigits.SetSize(0);
  while (x > 0) {
    uint64_t nextDigit = x % LargeIntegerUnsigned::CarryOverBound;
    this->theDigits.AddOnTop(static_cast<int>(nextDigit));
    x /= LargeIntegerUnsigned::CarryOverBound;
  }
}

void LargeIntegerUnsigned::AssignUnsignedInt(unsigned int x) {
  this->AssignShiftedUInt(x, 0);
}

void LargeIntegerUnsigned::AssignShiftedUInt(unsigned int x, int shift) {
  if (x == 0) {
    this->makeZero();
    return;
  }
  this->theDigits.initializeFillInObject(shift, 0);
  while (x != 0) {
    unsigned int tempX = x % LargeIntegerUnsigned::CarryOverBound;
    this->theDigits.AddOnTop(static_cast<int>(tempX));
    x = x / LargeIntegerUnsigned::CarryOverBound;
  }
}

void LargeIntegerUnsigned::AddNoFitSize(const LargeIntegerUnsigned& x) {
  MacroIncrementCounter(Rational::TotalLargeAdditions);
  int oldsize = this->theDigits.size;
  this->theDigits.SetSize(MathRoutines::Maximum(this->theDigits.size, x.theDigits.size) + 1);
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
  this->AddNoFitSize(x);
  this->FitSize();
}

LargeIntegerUnsigned LargeIntegerUnsigned::operator-(const LargeIntegerUnsigned& other) const {
  LargeIntegerUnsigned result;
  result = *this;
  result.SubtractSmallerPositive(other);
  return result;
}

std::string LargeIntegerUnsigned::ToStringAbbreviate(FormatExpressions *theFormat) const {
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

void LargeIntegerUnsigned::PadWithZeroesToAtLeastNDigits(int desiredMinNumDigits) {
  if (this->theDigits.size >= desiredMinNumDigits) {
    return;
  }
  int i = this->theDigits.size;
  this->theDigits.SetSize(desiredMinNumDigits);
  for (; i < desiredMinNumDigits; i ++) {
    this->theDigits[i] = 0;
  }
}

void LargeIntegerUnsigned::AddLargeIntUnsignedShiftedTimesDigit(const LargeIntegerUnsigned& other, int digitShift, int theConst) {
  if (theConst >= this->CarryOverBound || (- theConst) <= (- this->CarryOverBound)) {
    global.fatal << "Digit: " << theConst << " is too large" << global.fatal;
  }
  int numDigits = MathRoutines::Maximum(other.theDigits.size + 1 + digitShift, this->theDigits.size + 1);
  this->PadWithZeroesToAtLeastNDigits(numDigits);
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
  this->FitSize();
  int lastDigit =* this->theDigits.LastObject();
  if (lastDigit >= this->CarryOverBound || (- lastDigit) <= (- this->CarryOverBound)) {
    global.fatal << "Leading digit: " << lastDigit << " is too large" << global.fatal;
  }
}

void LargeIntegerUnsigned::SubtractSmallerPositive(const LargeIntegerUnsigned& x) {
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
  this->FitSize();
}

void LargeIntegerUnsigned::MultiplyBy(const LargeIntegerUnsigned& x, LargeIntegerUnsigned& output) const {
  if (this == &output || &x == &output) {
    LargeIntegerUnsigned thisCopy = *this;
    LargeIntegerUnsigned xCopy = x;
    return thisCopy.MultiplyBy(xCopy, output);
  }
  MacroIncrementCounter(Rational::TotalLargeMultiplications);
  output.theDigits.SetSize(x.theDigits.size + this->theDigits.size);
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
    << this->ToStringAbbreviate()
    << "<br>\n" << x.ToStringAbbreviate();
    report1.GetElement().Report(reportStream.str());
    report2.GetElement().ticksPerReport = ticksPerReport;
  }
  for (int i = 0; i < this->theDigits.size; i ++) {
    for (int j = 0; j < x.theDigits.size; j ++) {
      unsigned long long tempLong = static_cast<unsigned>(this->theDigits[i]);
      unsigned long long tempLong2 = static_cast<unsigned>(x.theDigits[j]);
      tempLong = tempLong * tempLong2;
      unsigned long long lowPart = tempLong % LargeIntegerUnsigned::CarryOverBound;
      unsigned long long highPart = tempLong / LargeIntegerUnsigned::CarryOverBound;
      output.AddShiftedUIntSmallerThanCarryOverBound(static_cast<unsigned int>(lowPart), i + j);
      output.AddShiftedUIntSmallerThanCarryOverBound(static_cast<unsigned int>(highPart), i + j + 1);
      if (doReport) {
        if (report2.GetElement().TickAndWantReport()) {
          std::stringstream out;
          out << "<br>Crunching " << numCycles << " out of " << totalCycles
          << " pairs of large integer ``digits'' = "
          << this->theDigits.size << " x " << x.theDigits.size
          << " digits (base " << LargeIntegerUnsigned::CarryOverBound << ").";
          report2.GetElement().Report(out.str());
        }
      }
    }
  }
  output.FitSize();
}

LargeIntegerUnsigned LargeIntegerUnsigned::operator%(const LargeIntegerUnsigned& other) const {
  LargeIntegerUnsigned result, temp;
  this->DivPositive(other, temp, result);
  return result;
}

int LargeIntegerUnsigned::GetUnsignedIntValueTruncated() {
  return this->theDigits[0];
}

double LargeIntegerUnsigned::GetDoubleValue() const {
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
  MacroIncrementCounter(Rational::TotalLargeGCDcalls);
  LargeIntegerUnsigned p, q, r, temp;
  p = a;
  q = b;
  while (!q.IsEqualToZero()) {
    p.DivPositive(q, temp, r);
    p = q;
    q = r;
  }
  output = p;
}

void LargeIntegerUnsigned::FitSize() {
  int newSize = this->theDigits.size;
  for (int i = this->theDigits.size - 1; i >= 1; i --) {
    if (this->theDigits[i] == 0) {
      newSize --;
    } else {
      break;
    }
  }
  this->theDigits.SetSize(newSize);
}

void LargeIntegerUnsigned::AccountFactor(
  const LargeInteger& theP, List<LargeInteger>& outputPrimeFactors, List<int>& outputMultiplicities
) const {
  if (outputPrimeFactors.size == 0) {
    outputPrimeFactors.AddOnTop(theP);
    outputMultiplicities.AddOnTop(1);
    return;
  }
  if ((*outputPrimeFactors.LastObject()).operator==(theP)) {
    (*outputMultiplicities.LastObject()) ++;
  } else {
    outputPrimeFactors.AddOnTop(theP);
    outputMultiplicities.AddOnTop(1);
  }
}

bool LargeIntegerUnsigned::FactorLargeReturnFalseIfFactorizationIncomplete(
  List<LargeInteger>& outputFactors,
  List<int>& outputMultiplicites,
  int dontSearchForDivisorsLargerThan,
  std::stringstream* commentsOnFailure
) const {
  MacroRegisterFunctionWithName("LargeIntUnsigned::FactorLarge");
  int maximumNumberOfDigits = 1000;
  if (this->theDigits.size > maximumNumberOfDigits) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Number has too many digits: maximum " << maximumNumberOfDigits
      << " digits accepted for factorization attempt. ";
    }
    return false;
  }
  if (this->IsEqualToZero()) {
    global.fatal << "This is a programming error: it was requested that I factor 0, which is forbidden. " << global.fatal;
  }
  LargeIntegerUnsigned toBeFactored = *this;
  outputFactors.SetSize(0);
  outputMultiplicites.SetSize(0);
  if (dontSearchForDivisorsLargerThan <= 0) {
    dontSearchForDivisorsLargerThan = 100000;
  }
  List<bool> theSieve;
  theSieve.initializeFillInObject(dontSearchForDivisorsLargerThan + 1, true);
  for (int i = 2; i <= dontSearchForDivisorsLargerThan; i ++) {
    if (!theSieve[i]) {
      continue;
    }
    //LargeIntUnsigned current = toBeFactored % i;
    unsigned candidate = static_cast<unsigned>(i);
    while (toBeFactored % candidate == 0) {
      this->AccountFactor(i, outputFactors, outputMultiplicites);
      toBeFactored /= candidate;
    }
    for (int j = i; j <= dontSearchForDivisorsLargerThan; j += i) {
      theSieve[j] = false;
    }
  }
  if (toBeFactored > 1) {
    this->AccountFactor(toBeFactored, outputFactors, outputMultiplicites);
    if (!toBeFactored.IsPossiblyPrimeMillerRabiN(10, nullptr)) {
      *commentsOnFailure
      << "The largest remaining factor "
      << toBeFactored
      << " is known not to be prime (Miller-Rabin test) "
      << "but I could not factor it. "
      << "I checked all factors smaller than or equal to: "
      << dontSearchForDivisorsLargerThan;
      return false;
    }
  }
  return true;
}

bool LargeIntegerUnsigned::FactorReturnFalseIfFactorizationIncomplete(
  List<LargeInteger>& outputFactors,
  List<int>& outputMultiplicites,
  int dontSearchForDivisorsLargerThan,
  std::stringstream* commentsOnFailure
) const {
  MacroRegisterFunctionWithName("LargeIntUnsigned::Factor");
  if (this->theDigits.size > 1) {
    return this->FactorLargeReturnFalseIfFactorizationIncomplete(
      outputFactors, outputMultiplicites, dontSearchForDivisorsLargerThan, commentsOnFailure
    );
  }
  if (this->IsEqualToZero()) {
    global.fatal << "This is a programming error: it was requested that I factor 0, which is forbidden." << global.fatal;
  }
  unsigned int toBeFactored = static_cast<unsigned>(this->theDigits[0]);
  outputFactors.SetSize(0);
  outputMultiplicites.SetSize(0);
  int upperboundPrimeDivisors = static_cast<int>(FloatingPoint::Sqrt(static_cast<double>(toBeFactored)));
  List<bool> theSieve;
  theSieve.initializeFillInObject(upperboundPrimeDivisors + 1, true);
  for (int i = 2; i <= upperboundPrimeDivisors; i ++) {
    if (!theSieve[i]) {
      continue;
    }
    unsigned candidate = static_cast<unsigned>(i);
    while (toBeFactored % candidate == 0) {
      this->AccountFactor(i, outputFactors, outputMultiplicites);
      toBeFactored /= candidate;
      upperboundPrimeDivisors = static_cast<int>(FloatingPoint::Sqrt(static_cast<double>(toBeFactored)));
    }
    for (int j = i; j <= upperboundPrimeDivisors; j += i) {
      theSieve[j] = false;
    }
  }
  if (toBeFactored > 1) {
    this->AccountFactor(static_cast<int>(toBeFactored), outputFactors, outputMultiplicites);
  }
  return true;
}

void LargeIntegerUnsigned::lcm(
  const LargeIntegerUnsigned& a,
  const LargeIntegerUnsigned& b,
  LargeIntegerUnsigned& output
) {
  LargeIntegerUnsigned tempUI, tempUI2;
  if (a.IsEqualToZero() || b.IsEqualToZero()) {
    global.fatal << "This is a programming error: calling lcm on zero elements is not allowed. " << global.fatal;
  }
  LargeIntegerUnsigned::gcd(a, b, tempUI);
  a.MultiplyBy(b, tempUI2);
  output = tempUI2;
  output.DivPositive(tempUI, output, tempUI2);
  if (output.IsEqualToZero()) {
    global.fatal << "Least common multiple not allowed to be zero. " << global.fatal;
  }
}

void LargeIntegerUnsigned::operator=(const LargeIntegerUnsigned& x) {
  this->theDigits = x.theDigits;
}

void LargeIntegerUnsigned::operator=(unsigned int x) {
  this->AssignShiftedUInt(x, 0);
}

unsigned int LargeIntegerUnsigned::hashFunction() const {
  int numCycles = MathRoutines::Minimum(this->theDigits.size, SomeRandomPrimesSize);
  unsigned int result = 0;
  for (int i = 0; i < numCycles; i ++) {
    result += static_cast<unsigned>(this->theDigits[i]) * SomeRandomPrimes[i];
  }
  return result;
}

bool LargeIntegerUnsigned::IsPositive() const {
  return this->theDigits.size > 1 || this->theDigits[0] > 0;
}

bool LargeIntegerUnsigned::IsEqualToOne() const {
  return this->theDigits.size == 1 && this->theDigits[0] == 1;
}

bool LargeIntegerUnsigned::IsEqualToZero() const {
  return this->theDigits.size == 1 && this->theDigits[0] == 0;
}

void LargeIntegerUnsigned::AssignFactorial(unsigned int x) {
  this->makeOne();
  List<unsigned int> primesBelowX;
  LargeIntegerUnsigned::getPrimesEratosthenesSieve(x, primesBelowX);
  LargeIntegerUnsigned tempInt, tempOne;
  tempOne.makeOne();
  for (int i = 0; i < primesBelowX.size; i ++) {
    unsigned int thePrime = primesBelowX.TheObjects[i];
    unsigned int thePowerOfThePrime = 0;
    unsigned int currentPower = thePrime;
    do {
      thePowerOfThePrime += x / currentPower;
      currentPower *= thePrime;
    } while (currentPower <= x);
    tempInt.AssignShiftedUInt(thePrime, 0);
    MathRoutines::RaiseToPower(tempInt, thePowerOfThePrime, tempOne);
    *this *= tempInt;
  }
}

bool LargeInteger::IsEven() const {
  return this->value.IsEven();
}

void LargeInteger::WriteToFile(std::fstream& output) {
  std::string tempS;
  this->toString(tempS);
  output << tempS;
}

void LargeInteger::ReadFromFile(std::fstream& input) {
  std::string tempS;
  input >> tempS;
  this->AssignString(tempS);
}

void LargeInteger::AssignString(const std::string& input) {
  bool success = this->AssignStringFailureAllowed(input, nullptr);
  if (!success) {
    global.fatal
    << "LargeInt::AssignString is not allowed to fail." << global.fatal;
  }
}

bool LargeInteger::AssignStringFailureAllowed(
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
  if (!this->IsEqualToZero()) {
    if (input[0] == '-') {
      this->sign = - 1;
    }
  }
  return true;
}

void LargeInteger::RaiseToPower(int thePower) {
  MathRoutines::RaiseToPower(*this, thePower, LargeInteger(1));
}

bool LargeInteger::IsIntegerFittingInInt(int* whichInt) {
  if (!this->value.IsIntegerFittingInInt(whichInt)) {
    return false;
  }
  if (whichInt != nullptr) {
    *whichInt *= this->sign;
  }
  return true;
}

void LargeInteger::MultiplyByInt(int x) {
  LargeInteger tempI;
  tempI.AssignInt(x);
  *this *= tempI;
}

LargeInteger LargeInteger::zero() {
  LargeInteger result;
  return result;
}

bool LargeInteger::TryToFindWhetherIsPower(
  bool& outputIsPower, LargeInteger& outputBase, int& outputPower
) const {
  if (this->sign == - 1) {
    return false;
  }
  return this->value.TryToFindWhetherIsPower(outputIsPower, outputBase, outputPower);
}

bool LargeInteger::operator==(const LargeInteger& x) const {
  if (x.sign != this->sign) {
    if (x.IsEqualToZero() && this->IsEqualToZero()) {
      return true;
    } else {
      return false;
    }
  }
  return this->value == x.value;
}

bool LargeInteger::CheckForConsistensy() {
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

double LargeInteger::GetDoubleValue() const {
  return this->sign * this->value.GetDoubleValue();
}

void LargeInteger::toString(std::string& output) const {
  std::stringstream out;
  if (this->IsEqualToZero()) {
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

void LargeInteger::AssignUInt64(uint64_t x) {
  this->sign = 1;
  this->value.AssignUInt64(x);
}

void LargeInteger::AssignInt64(int64_t x) {
  this->sign = 1;
  if (x < 0) {
    this->sign = - 1;
    x = - x;
  }
  this->value.AssignUInt64(static_cast<uint64_t>(x));
}

void LargeInteger::AssignInt(int x) {
  if (x == 0) {
    this->makeZero();
    return;
  }
  this->sign = 1;
  if (x < 0) {
    this->sign = - 1;
    x = - x;
  }
  this->value.AssignShiftedUInt(static_cast<unsigned int>(x), 0);
}

bool LargeInteger::GetDivisors(List<int>& output, bool includeNegative) {
  if (this->value.theDigits.size > 1) {
    return false;
  }
  int val = this->value.theDigits[0];
  if (val > 50000) {
    return false;
  }
  output.SetSize(0);
  for (int i = 1; i <= val; i ++) {
    if (val % i == 0) {
      output.AddOnTop(i);
      if (includeNegative) {
        output.AddOnTop(- i);
      }
    }
  }
  return true;
}

void LargeInteger::AddLargeIntUnsigned(const LargeIntegerUnsigned& x) {
  if (this->sign == 1) {
    this->value += x;
    return;
  }
  if (this->value.IsGreaterThanOrEqualTo(x)) {
    this->value.SubtractSmallerPositive(x);
  } else {
    LargeIntegerUnsigned tempI = x;
    tempI.SubtractSmallerPositive(this->value);
    this->value = tempI;
    this->sign = 1;
  }
}

void LargeInteger::operator+=(const LargeInteger& x) {
  if (this->sign == x.sign) {
    this->value += x.value;
  } else {
    if (this->value.IsGreaterThanOrEqualTo(x.value)) {
      this->value.SubtractSmallerPositive(x.value);
    } else {
      LargeIntegerUnsigned tempI = this->value;
      this->value = x.value;
      this->value.SubtractSmallerPositive(tempI);
      this->sign = x.sign;
    }
  }
}

void LargeInteger::makeZero() {
  this->value.makeZero();
  this->sign = 1;
}

void LargeInteger::operator=(const Rational& other) {
  if (!other.IsInteger(this)) {
    global.fatal << "This is a programming error: converting implicitly rational number " << other.toString()
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
  tempX.AssignShiftedUInt(static_cast<unsigned>(absX), 0);
  this->value.DivPositive(tempX, result.value, remainder);
  result.sign = this->sign * signX;
  return result;
}

LargeInteger LargeInteger::operator/(LargeInteger& x) const {
  LargeInteger result;
  LargeInteger remainder;
  this->value.DivPositive(x.value, result.value, remainder.value);
  result.sign = this->sign * x.sign;
  if (!result.CheckForConsistensy()) {
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
  tempX.AssignShiftedUInt(static_cast<unsigned>(x), 0);
  this->value.DivPositive(tempX, result, remainder);
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
  if (this->Extended == nullptr) {
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
  global.fatal << "This piece of code should not be reached. " << global.fatal;
  return - 1;
}

void Rational::WriteToFile(std::fstream& output) {
  output << this->toString();
}

void Rational::RaiseToPower(int x) {
  Rational tempRat;
  tempRat.makeOne();
  if (x < 0) {
    x = - x;
    this->invert();
  }
  LargeInteger tempNum = this->GetNumerator();
  LargeIntegerUnsigned oneLI;
  oneLI.makeOne();
  MathRoutines::RaiseToPower(tempNum.value, x, oneLI);
  LargeIntegerUnsigned tempDen = this->GetDenominator();
  MathRoutines::RaiseToPower(tempDen, x, oneLI);
  char theSign = (this->IsPositive() || x % 2 == 0) ? 1 : - 1;
  this->AllocateExtended();
  this->Extended->num.sign = theSign;
  this->Extended->den = tempDen;
  this->Extended->num.value = tempNum.value;
  this->ShrinkExtendedPartIfPossible();
}

void Rational::invert() {
  if (this->Extended == nullptr) {
    int tempI = this->DenShort;
    if (tempI <= 0) {
      global.fatal << "Denominator not allowed to be negative. " << global.fatal;
    }
    if (this->NumShort < 0) {
      this->DenShort = - this->NumShort;
      this->NumShort = - tempI;
    } else {
      this->DenShort = this->NumShort;
      this->NumShort = tempI;
    }
    return;
  }
  MathRoutines::swap(this->Extended->den, this->Extended->num.value);
}

void Rational::ReadFromFile(std::istream& input) {
  std::string tempS;
  input >> tempS;
  this->AssignString(tempS);
}

void Rational::MultiplyByInt(int x) {
  Rational tempRat;
  tempRat.AssignInteger(x);
  this->MultiplyBy(tempRat);
}

void Rational::MultiplyBy(const Rational& r) {
  if (r.Extended == nullptr && this->Extended == nullptr) {
    if (this->TryToMultiplyQuickly(r.NumShort, r.DenShort)) {
      return;
    }
  }
  this->InitExtendedFromShortIfNeeded();
  if (r.Extended != nullptr) {
    this->Extended->num *= r.Extended->num;
    this->Extended->den *= r.Extended->den;
  } else {
    this->Extended->num.MultiplyByInt(r.NumShort);
    this->Extended->den.MultiplyByUInt( static_cast<unsigned int>(r.DenShort));
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
  tempRat.Assign(argument);
  tempRat.Minus();
  return tempRat;
}

void Rational::MultiplyByLargeInt(LargeInteger& x) {
  this->InitExtendedFromShortIfNeeded();
  this->Extended->num *= x;
  this->simplify();
}

void Rational::MultiplyByLargeIntUnsigned(LargeIntegerUnsigned& x) {
  this->InitExtendedFromShortIfNeeded();
  this->Extended->num.value.MultiplyBy(x);
  this->simplify();
}

void Rational::DivideBy(const Rational& r) {
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
  tempRat.Assign(*this);
  tempRat.DivideBy(right);
  return tempRat;
}

Rational Rational::operator*(const Rational& right) const {
  Rational tempRat;
  tempRat.Assign(*this);
  tempRat.MultiplyBy(right);
  return tempRat;
}

Rational Rational::operator+(const Rational& right) const {
  Rational tempRat;
  tempRat.Assign(*this);
  tempRat += right;
  return tempRat;
}

Rational Rational::operator-(const Rational& right) const {
  Rational tempRat;
  tempRat.Assign(*this);
  tempRat -= right;
  return tempRat;
}

void Rational::Assign(const Rational& r) {
  this->NumShort = r.NumShort;
  this->DenShort = r.DenShort;
  if (r.Extended == nullptr) {
    if (this->Extended == nullptr) {
      return;
    }
    this->FreeExtended();
    return;
  }
  this->InitExtendedFromShortIfNeeded();
  this->Extended->num = r.Extended->num;
  this->Extended->den = r.Extended->den;
}

void Rational::AssignFracValue() {
  if (this->Extended == nullptr) {
    if (this->NumShort == 0) {
      return;
    }
    if (this->DenShort == 1) {
      this->NumShort = 0;
      this->DenShort = 1;
      return;
    }
    this->NumShort = this->NumShort % this->DenShort;
    if (this->NumShort < 0) {
      this->NumShort += this->DenShort;
    }
    return;
  }
  if (this->IsEqualToZero()) {
    return;
  }
  if (this->Extended->den.IsEqualToOne()) {
    this->makeZero();
    return;
  }
  LargeIntegerUnsigned newNum, tempI;
  this->Extended->num.value.DivPositive(this->Extended->den, tempI, newNum);
  this->Extended->num.value = newNum;
  if (this->Extended->num.IsNegative()) {
    this->Extended->num.AddLargeIntUnsigned(this->Extended->den);
  }
  if (!this->Extended->num.IsPositiveOrZero()) {
    global.fatal << "Numerator must not be negative. " << global.fatal;
  }
  this->simplify();
}

void Rational::AssignLargeIntUnsigned(const LargeIntegerUnsigned& other) {
  LargeInteger tempInt;
  tempInt.AssignLargeIntUnsigned(other);
  this->AssignLargeInteger(tempInt);
}

void Rational::AssignLargeInteger(const LargeInteger& other){
  if (this->Extended == nullptr) {
    this->Extended = new LargeRationalExtended;
#ifdef AllocationLimitsSafeguard
ParallelComputing::GlobalPointerCounter ++;
ParallelComputing::CheckPointerCounters();
#endif
  }
  this->Extended->num = other;
  this->Extended->den.makeOne();
  this->ShrinkExtendedPartIfPossible();
}

void Rational::AddInteger(int x) {
  Rational tempRat;
  tempRat.AssignNumeratorAndDenominator(x, 1);
  this->operator+=(tempRat);
}

bool Rational::IsGreaterThan(const Rational& r) const {
  Rational tempRat;
  tempRat.Assign(*this);
  tempRat.Subtract(r);
  return tempRat.IsPositive();
}

void Rational::Subtract(const Rational& r) {
  Rational temp;
  temp.Assign(r);
  temp.Minus();
  this->operator+=(temp);
}

bool Rational::GetSquareRootIfRational(Rational& output) const {
  if (*this < 0) {
    return false;
  }
  LargeInteger theNum = this->GetNumerator();
  LargeIntegerUnsigned theDen = this->GetDenominator();
  List<LargeInteger> primeFactorsNum, primeFactorsDen;
  List<int> multsNum, multsDen;
  if (!theNum.value.FactorReturnFalseIfFactorizationIncomplete(primeFactorsNum, multsNum, 0, nullptr)) {
    return false;
  }
  if (!theDen.FactorReturnFalseIfFactorizationIncomplete(primeFactorsDen, multsDen, 0, nullptr)) {
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
    tempRat.RaiseToPower(currentMult / 2);
    output *= tempRat;
  }
  for (int i = 0; i < primeFactorsDen.size; i ++) {
    if (multsDen[i] % 2 != 0) {
      return false;
    }
    tempRat = primeFactorsDen[i];
    int currentMult = multsDen[i];
    tempRat.RaiseToPower(currentMult / 2);
    output /= tempRat;
  }
  return true;
}

bool Rational::TryToAddQuickly(int OtherNum, int OtherDen) {
  int OtherNumAbs, thisNumAbs;
  if (this->DenShort <= 0 || OtherDen <= 0) {
    global.fatal << "This is a programming error: trying to add corrupt rational number(s) with denominators "
    << this->DenShort << " and " << OtherDen
    << ". The cause of the error should be in some of the calling functions. " << global.fatal;
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
    this->Extended != nullptr ||
    thisNumAbs >= LargeIntegerUnsigned::SquareRootOfCarryOverBound ||
    this->DenShort >= LargeIntegerUnsigned::SquareRootOfCarryOverBound ||
    OtherNumAbs >= LargeIntegerUnsigned::SquareRootOfCarryOverBound ||
    OtherDen >= LargeIntegerUnsigned::SquareRootOfCarryOverBound
  ) {
    return false;
  }
  int N = this->NumShort * OtherDen + this->DenShort * OtherNum;
  int D = this->DenShort * OtherDen;
  if (N == 0) {
    this->NumShort = 0;
    this->DenShort = 1;
    MacroIncrementCounter(Rational::TotalSmallAdditions);
    return true;
  }
  int tempGCD = 0;
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

bool Rational::TryToMultiplyQuickly(int OtherNum, int OtherDen) {
  int OtherNumAbs, thisNumAbs;
  if (this->DenShort <= 0 || OtherDen <= 0) {
    if (DenShort == 0 || OtherDen == 0) {
      global.fatal << "This is a programming error: division by zero. ";
    } else {
      global.fatal << "This is a programming error during rational number multiplication: "
      << "corrupt rational number denominator. ";
    }
    global.fatal << global.fatal;
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
    this->Extended != nullptr ||
    thisNumAbs >= LargeIntegerUnsigned::SquareRootOfCarryOverBound ||
    this->DenShort >= LargeIntegerUnsigned::SquareRootOfCarryOverBound ||
    OtherNumAbs >= LargeIntegerUnsigned::SquareRootOfCarryOverBound ||
    OtherDen >= LargeIntegerUnsigned::SquareRootOfCarryOverBound
  ) {
    return false;
  }
  int N = this->NumShort * OtherNum;
  int D = this->DenShort * OtherDen;
  if (N == 0) {
    this->NumShort = 0;
    this->DenShort = 1;
  } else {
    int tempGCD = 0;
    if (N > 0) {
      tempGCD = Rational::gcd(N, D);
    } else {
      tempGCD = Rational::gcd(- N, D);
    }
    this->NumShort = N / static_cast<signed int>(tempGCD);
    this->DenShort = D / tempGCD;
  }
  MacroIncrementCounter(Rational::TotalSmallMultiplications);
  return true;
}

Rational Rational::scaleNormalizeIndex(
  List<Rational>& output, int indexNonZeroEntry
) {
  if (output.size == 0) {
    return 1;
  }
  LargeIntegerUnsigned denominatorLCM = output[0].GetDenominator();
  LargeIntegerUnsigned numeratorGCD = output[0].GetNumerator().value;
  for (int i = 1; i < output.size; i ++) {
    LargeIntegerUnsigned::lcm(
      denominatorLCM,
      output[i].GetDenominator(),
      denominatorLCM
    );
    LargeIntegerUnsigned::gcd(
      numeratorGCD,
      output[i].GetNumerator().value,
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

LargeIntegerUnsigned Rational::GetDenominator() const {
  LargeIntegerUnsigned result;
  if (this->Extended == nullptr) {
    unsigned int tempI = static_cast<unsigned int>(this->DenShort);
    result.AssignShiftedUInt(tempI, 0);
  } else {
    result = (this->Extended->den);
  }
  return result;
}

bool Rational::BeginsWithMinus() {
  return this->IsNegative();
}

LargeInteger Rational::GetNumerator() const {
  LargeInteger result;
  if (this->Extended == nullptr) {
    if (this->NumShort < 0) {
      result.value.AssignShiftedUInt(static_cast<unsigned int>(- this->NumShort), 0);
    } else {
      result.value.AssignShiftedUInt(static_cast<unsigned int>(this->NumShort), 0);
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

const Rational& Rational::GetComplexConjugate() const {
  return *this;
}

bool Rational::GetPrimeFactorsAbsoluteValue(
  List<LargeInteger>& numeratorPrimeFactors,
  List<int>& numeratorMultiplicities,
  List<LargeInteger>& denominatorPrimeFactors,
  List<int>& denominatorMultiplicities
) {
  MacroRegisterFunctionWithName("Rational::GetPrimeFactorsAbsoluteValue");
  if (!this->GetNumerator().value.FactorReturnFalseIfFactorizationIncomplete(
    numeratorPrimeFactors, numeratorMultiplicities, 0, nullptr
  )) {
    return false;
  }
  return this->GetDenominator().FactorReturnFalseIfFactorizationIncomplete(
    denominatorPrimeFactors, denominatorMultiplicities, 0, nullptr
  );
}

void Rational::AssignInteger(int x) {
  this->FreeExtended();
  this->DenShort = 1;
  this->NumShort = x;
}

bool Rational::ShrinkExtendedPartIfPossible() {
  if (this->Extended == nullptr) {
    return true;
  }
  if (
    this->Extended->num.value.theDigits.size > 1 ||
    this->Extended->den.theDigits.size > 1 ||
    this->Extended->num.value.theDigits[0] >= LargeIntegerUnsigned::SquareRootOfCarryOverBound ||
    this->Extended->den.theDigits[0] >= LargeIntegerUnsigned::SquareRootOfCarryOverBound
  ) {
    return false;
  }
  this->NumShort = this->Extended->num.GetIntValueTruncated();
  this->DenShort = this->Extended->den.GetUnsignedIntValueTruncated();
  this->FreeExtended();
  return true;
}

Rational Rational::Factorial(int n) {
  if (n < 0) {
    global.fatal << "This is a programming error: taking factorial of the negative number " << n << ". " << global.fatal;
    return 0;
  }
  LargeIntegerUnsigned result;
  result.AssignFactorial(static_cast<unsigned int>(n));
  Rational answer;
  answer = result;
  return answer;
}

Rational Rational::TwoToTheNth(int n) {
  Rational result = 1;
  if (n>= 0) {
    for (int i = 0; i < n; i ++) {
      result.MultiplyByInt(2);
    }
  } else {
    for (int i = 0; i > n; i --) {
      result.DivideByInteger(2);
    }
  }
  return result;
}

Rational Rational::NChooseK(const Rational& n, int k) {
  Rational result;
  result.makeOne();
  for (int i = 0; i < k; i ++) {
    result *= n - i;
    result.DivideByInteger(i + 1);
  }
  return result;
}

Rational Rational::NtoTheKth(int n, int k) {
  Rational result = n;
  result.RaiseToPower(k);
  return result;
}

bool Rational::IsInteger(LargeInteger* whichInteger) const {
  bool result = false;
  if (this->Extended == nullptr) {
    result = (this->DenShort == 1);
    if (whichInteger != nullptr) {
      *whichInteger = this->NumShort;
    }
  } else {
    if (this->Extended->den.IsEqualToOne()) {
      result = true;
      if (whichInteger != nullptr) {
        *whichInteger = this->Extended->num;
      }
    }
  }
  return result;
}

bool MathRoutines::IsInteger(Rational x) {
  return x.IsInteger();
}

double Rational::GetDoubleValue() const {
  if (this->Extended == nullptr) {
    return static_cast<double>(this->NumShort) / static_cast<double>(this->DenShort);
  } else {
    return this->Extended->num.GetDoubleValue() / this->Extended->den.GetDoubleValue();
  }
}

void Rational::simplify() {
  if (this->Extended == nullptr) {
    if (this->NumShort == 0) {
      this->DenShort = 1;
    } else {
      if (this->DenShort == 1) {
        return;
      }
      int tempGCD;
      if (this->NumShort > 0) {
        tempGCD = this->gcd(this->NumShort, this->DenShort);
      } else {
        tempGCD = this->gcd(- this->NumShort, this->DenShort);
      }
      this->NumShort /= tempGCD;
      this->DenShort /= tempGCD;
    }
    return;
  }
  if (this->Extended->num.IsEqualToZero()) {
    this->makeZero();
    return;
  }
  if (!this->Extended->den.IsEqualToOne()) {
    LargeIntegerUnsigned tempI;
    LargeIntegerUnsigned::gcd(this->Extended->den, this->Extended->num.value, tempI);
    /*if (Rational::flagAnErrorHasOccurredTimeToPanic) {
      std::string tempS1, tempS2, tempS3;
      tempI.toString(tempS1);
      this->toString(tempS2);
    }*/
    LargeIntegerUnsigned tempI2;
    this->Extended->den.DivPositive(tempI, this->Extended->den, tempI2);
    this->Extended->num.value.DivPositive(tempI, this->Extended->num.value, tempI2);
  }
  this->ShrinkExtendedPartIfPossible();
}

void Rational::operator=(const Polynomial<Rational>& other) {
  if (!other.IsConstant(this)) {
    global.fatal << "This is a programming error: attempting to assign "
    << "non-constant polynomial to a Rational number is not allowed. "
    << global.fatal;
  }
}

bool Rational::IsEqualTo(const Rational& b) const {
  if (this->Extended == nullptr && b.Extended == nullptr) {
    return (this->NumShort * b.DenShort == b.NumShort * this->DenShort);
  }
  Rational tempRat;
  tempRat.Assign(*this);
  tempRat.Subtract(b);
  return tempRat.IsEqualToZero();
}

bool Rational::IsGreaterThanOrEqualTo(const Rational& right) const {
  if (this->Extended == nullptr && right.Extended == nullptr) {
    return (this->NumShort * right.DenShort >= right.NumShort * this->DenShort);
  }
  Rational tempRat;
  tempRat.Assign(*this);
  tempRat.Subtract(right);
  return tempRat.IsPositiveOrZero();
}

std::string Rational::toString(FormatExpressions* theFormat) const {
  if (theFormat != nullptr) {
    if (theFormat->flagUseFrac) {
      return this->ToStringFrac();
    }
  }
  std::stringstream out;
  if (this->Extended == nullptr) {
    out << this->NumShort;
    if (this->DenShort != 1) {
      out << "/" << this->DenShort;
    }
  } else {
    std::string tempS;
    this->Extended->num.toString(tempS);
    out << tempS;
    this->Extended->den.toString(tempS);
    if (tempS != "1") {
      out << "/" << tempS;
    }
  }
  return out.str();
}

std::string Rational::ToStringForFileOperations(FormatExpressions* notUsed) const {
  (void) notUsed; //portable way of avoiding unused parameter warning
  std::stringstream out;
  if (this->Extended == nullptr) {
    if (this->NumShort < 0) {
      out << "-";
    }
    int numShortAbsoluteValue = this->NumShort < 0 ? - this->NumShort: this->NumShort;
    if (this->DenShort == 1) {
      out << numShortAbsoluteValue;
    } else {
      out << numShortAbsoluteValue << "_div_" << this->DenShort;
    }
    return out.str();
  }
  LargeInteger numAbsVal = this->Extended->num;
  if (numAbsVal < 0) {
    out << "-";
    numAbsVal.sign = 1;
  }
  if (this->Extended->den.IsEqualToOne()) {
    out << numAbsVal.toString();
  } else {
    out << numAbsVal.toString() << "_div_" << this->Extended->den.toString();
  }
  return out.str();
}

std::string Rational::ToStringFrac() const {
  std::stringstream out;
  if (this->Extended == nullptr) {
    if (this->NumShort < 0) {
      out << "-";
    }
    int numShortAbsoluteValue = this->NumShort < 0 ? - this->NumShort: this->NumShort;
    if (this->DenShort == 1) {
      out << numShortAbsoluteValue;
    } else {
      out << "\\frac{" << numShortAbsoluteValue << "}{" << this->DenShort << "}";
    }
    return out.str();
  }
  LargeInteger numAbsVal = this->Extended->num;
  if (numAbsVal < 0) {
    out << "-";
    numAbsVal.sign = 1;
  }
  if (this->Extended->den.IsEqualToOne()) {
    out << numAbsVal.toString();
  } else {
    out << "\\frac{" << numAbsVal.toString() << "}{" << this->Extended->den.toString() << "}";
  }
  return out.str();
}

void Rational::operator=(const AlgebraicNumber& other) {
  bool isGood = other.IsRational(this);
  if (!isGood) {
    global.fatal << "This is a programming error: attempting to assign the "
    << "non-rational algebraic number " << other.toString() << "to a rational number. " << global.fatal;
  }
}

bool Rational::AssignStringFailureAllowed(const std::string& input) {
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
  if (readerDen.IsEqualToZero()) {
    return false;
  }
  *this /= readerDen;
  *this *= sign;
  return true;
}

void Rational::AssignString(const std::string& input) {
  if (!Rational::AssignStringFailureAllowed(input)) {
    global.fatal << "Programming error: "
    << "Rational::AssignString failed (likely a zero denominator). "
    << global.fatal;
  }
}
