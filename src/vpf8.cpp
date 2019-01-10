//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1General0_General.h"
#include "vpfImplementationHeader2Math0_General.h"
#include "vpfHeader2Math0_General.h"
#include "vpfHeader2Math2_AlgebraicNumbers.h"

ProjectInformationInstance ProjectInfoVpf8cpp(__FILE__, "Math routines implementation. ");

void LargeIntUnsigned::AssignString(const std::string& input)
{ if (input.size() < 10)
  { unsigned int x = std::atoi(input.c_str());
    this->AssignShiftedUInt(x, 0);
    return;
  }
  this->MakeZero();
  for (unsigned int i = 0; i < input.size(); i ++)
  { this->operator*=(10);
    int whichDigit = input[i] - '0';
    if (whichDigit > 9 || whichDigit < 0)
      crash << "This is a programming error: LargeIntUnsigned::AssignString" << " called on the string " << input
      << " which does not consist entirely of digits. Please note that LargeIntUnsigned::AssignString is a no-fail function, intended for "
      << " internal use only. If you want to parse arbitrary unsafe expressions coming from the user, please use the big gun (a.k.a. Calculator). "
      << crash;
    this->operator+=((unsigned) whichDigit);
  }
}

bool LargeIntUnsigned::AssignStringFailureAllowed(const std::string& input, bool ignoreNonDigits)
{ MacroRegisterFunctionWithName("LargeIntUnsigned::AssignStringFailureAllowed");
  if (input.size() > 10000000) //<- sorry folks, no more than 10 million digits.
    return false;
  if (input.size() < 10)
  { unsigned int x = std::atoi(input.c_str());
    this->AssignShiftedUInt(x, 0);
    return true;
  }
  this->MakeZero();
  for (unsigned int i = 0; i < input.size(); i ++)
  { this->operator*=(10);
    int whichDigit = input[i] - '0';
    if (whichDigit > 9 || whichDigit < 0)
    { if (!ignoreNonDigits)
        return false;
    } else
      this->operator+=((unsigned) whichDigit);
  }
  return true;
}

LargeIntUnsigned LargeIntUnsigned::operator*(const LargeIntUnsigned& x) const
{ LargeIntUnsigned result;
  this->MultiplyBy(x, result);
  return std::move(result);
}

LargeIntUnsigned LargeIntUnsigned::operator/(unsigned int x) const
{ LargeIntUnsigned result;
  LargeIntUnsigned remainder;
  LargeIntUnsigned tempX;
  tempX.AssignShiftedUInt(x, 0);
  this->DivPositive(tempX, result, remainder);
//  if (!result.CheckForConsistensy()) crash << crash;
  return std::move(result);
}

LargeIntUnsigned LargeIntUnsigned::operator/(const LargeIntUnsigned& x) const
{ LargeIntUnsigned result;
  LargeIntUnsigned remainder;
  this->DivPositive(x, result, remainder);
  return std::move(result);
}

int LargeIntUnsigned::operator%(unsigned int x)
{ LargeIntUnsigned quotient;
  LargeIntUnsigned remainder;
  LargeIntUnsigned divisor;
  //stOutput << "<br>DEBIG:Calling mod operator with this: " << this->ToString()
  //<< " and x: " << x;
  divisor.AssignShiftedUInt(x, 0);
  this->DivPositive(divisor, quotient, remainder);
  int result = 0;
  remainder.IsIntegerFittingInInt(&result);
  return result;
}

void LargeIntUnsigned::MakeOne()
{ this->theDigits.SetSize(1);
  this->theDigits[0] = 1;
}

void LargeIntUnsigned::MakeZero()
{ this->theDigits.SetSize(1);
  this->theDigits[0] = 0;
}

void LargeIntUnsigned::MultiplyByUInt(unsigned int x)
{ LargeIntUnsigned tempLI;
  tempLI.AssignShiftedUInt(x, 0);
  this->MultiplyBy(tempLI);
}

void LargeIntUnsigned::MultiplyBy(const LargeIntUnsigned& x)
{ LargeIntUnsigned tempInt;
  this->MultiplyBy(x, tempInt);
  *this = tempInt;
//  if (!this->CheckForConsistensy())crash << crash;
}

void LargeIntUnsigned::AddUInt(unsigned int x)
{ LargeIntUnsigned tempI;
  tempI.AssignShiftedUInt(x, 0);
  (*this) += tempI;
}

void LargeIntUnsigned::operator*=(const LargeIntUnsigned& right)
{ this->MultiplyBy(right);
}

void LargeIntUnsigned::operator*=(unsigned int x)
{ this->MultiplyByUInt(x);
}

void LargeIntUnsigned::operator+=(unsigned int x)
{ this->AddUInt(x);
}

LargeIntUnsigned LargeIntUnsigned::operator+(const LargeIntUnsigned& other)
{ LargeIntUnsigned result = *this;
  result += other;
  return std::move(result);
}

void LargeIntUnsigned::operator++(int)
{ this->AddUInt(1);
}

bool LargeIntUnsigned::IsIntegerFittingInInt(int* whichInt)
{ LargeIntUnsigned twoToThe31 = 2;
  MathRoutines::RaiseToPower(twoToThe31, 31, (LargeIntUnsigned) 1);
  if (*this >= twoToThe31)
    return false;
  if (whichInt == 0)
    return true;
  *whichInt = 0;
  for (int i = this->theDigits.size - 1; i >= 0; i --)
  { *whichInt *= LargeIntUnsigned::CarryOverBound;
    *whichInt += this->theDigits[i];
  }
  return true;
}

bool LargeIntUnsigned::IsGEQ(const LargeIntUnsigned& x) const
{ if (this->theDigits.size > x.theDigits.size)
    return true;
  if (this->theDigits.size < x.theDigits.size)
    return false;
  for (int i = this->theDigits.size - 1; i >= 0; i --)
  { if (x.theDigits[i] > this->theDigits[i])
      return false;
    if (x.theDigits[i] < this->theDigits[i])
      return true;
  }
  return true;
}

void LargeIntUnsigned::GetAllPrimesSmallerThanOrEqualToUseEratosthenesSieve(unsigned int n, List<unsigned int>& output)
{ List<int> theSieve;
  theSieve.initFillInObject(n + 1, 1);
  output.Reserve(n / 2);
  output.size = 0;
  for (unsigned int i = 2; i <= n; i ++)
    if (theSieve.TheObjects[i] != 0)
    { output.AddOnTop(i);
//        stOutput << i << ",";
      for (unsigned int j = i; j <= n; j += i)
        theSieve[j] = 0;
    }
}

LargeIntUnsigned LargeIntUnsigned::GetOne()
{ LargeIntUnsigned tempI;
  tempI.MakeOne();
  return tempI;
}

bool LargeIntUnsigned::operator<(int other) const
{ if (other < 0)
    return false;
  LargeIntUnsigned tempUI;
  tempUI = (unsigned) other;
  return *this < tempUI;
}

bool LargeIntUnsigned::operator>(int other) const
{ if (other < 0)
    return true;
  LargeIntUnsigned tempUI;
  tempUI = (unsigned) other;
  return *this > tempUI;
}

bool LargeIntUnsigned::operator<(const LargeIntUnsigned& other) const
{ return !this->IsGEQ(other);
}

bool LargeIntUnsigned::operator>=(const LargeIntUnsigned& other) const
{ return this->IsGEQ(other);
}

bool LargeIntUnsigned::operator>(const LargeIntUnsigned& other) const
{ return other < *this;
}

bool LargeIntUnsigned::IsEven() const
{ //stOutput << "<br>DEBUG: remainder by 2 is " << ((*this) % 2).ToString()
  //<< " and ((*this)%2) == 0 is " << (((*this) % 2) == 0);
  if (LargeIntUnsigned::CarryOverBound % 2 == 0)
  { //stOutput << "<br>DEBUG: (* this->theDigits.LastObject()) % 2: " << ((* this->theDigits.LastObject()) % 2);
    bool result = (this->theDigits[0] % 2) == 0;
    //stOutput << "<br>DEBUG: result is: " << result << "<br>Last digit: " << (* (this->theDigits.LastObject())) << "<br>"
    //<< "and so " << this->ToString() << " is even.<br>";
    return result;
  }
  return ((*this) % 2) == 0;
}

bool LargeIntUnsigned::operator==(const LargeIntUnsigned& other) const
{ return this->theDigits == other.theDigits;
}

bool LargeIntUnsigned::operator!=(const LargeIntUnsigned& other) const
{ return ! ((*this) == other);
}

void LargeIntUnsigned::operator--(int)
{ if (this->IsEqualToZero())
    crash << "This is a programming error: attempting to subtract 1 from a large unsigned integer with value 0. " << crash;
  this->SubtractSmallerPositive(1);
}

void LargeIntUnsigned::operator%=(const LargeIntUnsigned& other)
{ if (&other == this)
  { this->MakeZero();
    return;
  }
  LargeIntUnsigned copyMe = *this;
  LargeIntUnsigned temp1;
  copyMe.DivPositive(other, temp1, *this);
}

void LargeIntUnsigned::operator/=(const LargeIntUnsigned& other)
{ if (&other == this)
  { this->MakeOne();
    return;
  }
  LargeIntUnsigned copyMe = *this;
  LargeIntUnsigned temp1;
  copyMe.DivPositive(other, *this, temp1);
}

void LargeIntUnsigned::ToString(std::string& output) const
{ if (this->IsEqualToZero())
  { output = "0";
    return;
  }
  if (this->theDigits.size > 1)
  { this->ElementToStringLargeElementDecimal(output);
    return;
  }
  std::stringstream out;
  out << this->theDigits[0];
  output = out.str();
}

void LargeIntUnsigned::DivPositive(const LargeIntUnsigned& divisor, LargeIntUnsigned& quotientOutput, LargeIntUnsigned& remainderOutput) const
{ if (divisor.IsEqualToZero())
    crash << "Division by zero. " << crash;
  if (divisor.theDigits.size == 1 && this->theDigits.size == 1)
  { quotientOutput.AssignShiftedUInt(this->theDigits[0] / divisor.theDigits[0], 0);
    remainderOutput.AssignShiftedUInt(this->theDigits[0] % divisor.theDigits[0], 0);
//    stOutput << "Dividing " << this->ToString() << " by " << x.ToString() << " yields quotient " << quotientOutput.ToString()
//    << " and remainder " << remainderOutput.ToString();
    return;
  }
  if (&quotientOutput == &divisor ||
      &quotientOutput == this ||
      &remainderOutput == &divisor ||
      &remainderOutput == this)
  { LargeIntUnsigned divisorCopy = divisor;
    LargeIntUnsigned thisCopy = *this;
    thisCopy.DivPositive(divisorCopy, quotientOutput, remainderOutput);
    return;
  }
  //stOutput << "<hr><hr>DEBUG: dividing: " << this->ToString() << " by: " << divisor.ToString()
  //<< "<hr><hr>\n";
  remainderOutput = *this;
  quotientOutput.MakeZero();
  int currentQuotientDigit = 0;
  int divisorLeadingDigit = *divisor.theDigits.LastObject();
  int lastRemainderSize = - 1;
  int numRunsNoDigitImprovement = 0;
  LargeIntUnsigned remainderBackup;
  int upperlimitNoImprovementRounds = this->SquareRootOfCarryOverBound * 2;
  while (remainderOutput.IsGEQ(divisor))
  { int quotientDigitIndex = remainderOutput.theDigits.size - divisor.theDigits.size;
    long long remainderLeadingDigit = *remainderOutput.theDigits.LastObject();
    //stOutput << "<br>DEBUG: Remainder so far: " << remainderOutput.ToString() << "; "
    //<< " quotient digit: " << quotientDigitIndex << "; quotient: "
    //<< quotientOutput.ToString() << "<hr>";
    int divisorLeadingDigitPlusSlack = divisorLeadingDigit;
    for (;;)
    { if (remainderLeadingDigit < divisorLeadingDigitPlusSlack)
      { quotientDigitIndex --;
        remainderLeadingDigit *= LargeIntUnsigned::CarryOverBound;
        if (remainderOutput.theDigits.size > 1)
          remainderLeadingDigit += remainderOutput.theDigits[remainderOutput.theDigits.size - 2];
      }
      currentQuotientDigit = (int) (remainderLeadingDigit / divisorLeadingDigitPlusSlack);
      ///////////////////////////////////////////////////////////
      remainderBackup = remainderOutput;
      remainderOutput.AddLargeIntUnsignedShiftedTimesDigit(divisor, quotientDigitIndex, - currentQuotientDigit);
      if (*remainderOutput.theDigits.LastObject() >= 0)
      { quotientOutput.AddShiftedUIntSmallerThanCarryOverBound((unsigned)currentQuotientDigit, quotientDigitIndex);
        break;
      }
      //stOutput << "<br>Restoring divisor";
      remainderOutput = remainderBackup;
      divisorLeadingDigitPlusSlack ++;
      if (divisorLeadingDigitPlusSlack > divisorLeadingDigit + 1)
        crash << "Bad division algorithm: could not figure out currentQuotientDigit for more than 2 runs. " << crash;
    }
    if (lastRemainderSize == remainderOutput.theDigits.size && lastRemainderSize != 1)
      numRunsNoDigitImprovement ++;
    else
      numRunsNoDigitImprovement = 0;
    lastRemainderSize = remainderOutput.theDigits.size;
    if (numRunsNoDigitImprovement > upperlimitNoImprovementRounds)
      crash << "Bad division: while dividing " << this->ToString() << " by "
      << divisor.ToString()
      << " got too many algorithm steps without remainder size decrease. "
      << "." << crash;
  }
}

std::string LargeIntUnsigned::ToString(FormatExpressions* theFormat) const
{ (void) theFormat;//to avoid unused paramater warning
  std::string tempS;
  this->ToString(tempS);
  return tempS;
}

void LargeIntUnsigned::ElementToStringLargeElementDecimal(std::string& output) const
{ std::stringstream out;
  if (this->CarryOverBound == 1000000000  || this->CarryOverBound == 10)
  { std::string tempS;
    int numZeroesInCarryOver = 9;
    if (this->CarryOverBound == 10)
      numZeroesInCarryOver = 1;
    if (*this->theDigits.LastObject() < 0)
      out << "[";
    out << (*this->theDigits.LastObject());
    if (*this->theDigits.LastObject() < 0)
      out << "]";
    for (int i = this->theDigits.size - 2; i >= 0; i --)
    { std::stringstream tempStream;
      tempStream << this->theDigits[i];
      tempS = tempStream.str();
      if (tempS[0] == '-')
        out << "[";
      int numZeroesToPad = numZeroesInCarryOver - tempS.length();
      for (int j = 0; j < numZeroesToPad; j ++)
        out << "0";
      out << tempS;
      if (tempS[0] == '-')
        out << "]";
    }
    output = out.str();
    return;
  }
  unsigned int base = 10;
  int MaxNumIntegersPerCarryOverBound = 11;
  List<LargeIntUnsigned> bufferPowersOfBase;
  int initialNumDigitsEstimate = MaxNumIntegersPerCarryOverBound*this->theDigits.size;
  int sizeBufferPowersOfBase = MathRoutines::Minimum(initialNumDigitsEstimate, 10000);
  bufferPowersOfBase.SetSize(sizeBufferPowersOfBase);
  LargeIntUnsigned currentPower;
  LargeIntUnsigned Remainder = *this;
  int numRemainingDigits;
  while (!Remainder.IsEqualToZero())
  { currentPower.MakeOne();
    numRemainingDigits = 0;
    int highestBufferIndex = - 1;
    bufferPowersOfBase.TheObjects[0].MakeOne();
    bool bufferFilled = false;
    while (Remainder.IsGEQ(currentPower))
    { numRemainingDigits ++;
      highestBufferIndex ++;
      highestBufferIndex %= sizeBufferPowersOfBase;
      bufferPowersOfBase.TheObjects[highestBufferIndex] = currentPower;
      if (highestBufferIndex == sizeBufferPowersOfBase - 1)
        bufferFilled = true;
      currentPower *= base;
    }
    int startIndex = highestBufferIndex;
    do
    { currentPower = bufferPowersOfBase.TheObjects[highestBufferIndex];
      unsigned int theDigit = 0;
      while (Remainder.IsGEQ(currentPower))
      { theDigit ++;
        currentPower += bufferPowersOfBase[highestBufferIndex];
      }
      out << theDigit;
      numRemainingDigits --;
      if (theDigit != 1)
        bufferPowersOfBase[highestBufferIndex] *= theDigit;
      Remainder.SubtractSmallerPositive(bufferPowersOfBase[highestBufferIndex]);
      highestBufferIndex --;
      if (highestBufferIndex == - 1)
      { highestBufferIndex = sizeBufferPowersOfBase - 1;
        if (!bufferFilled)
          break;
      }
    } while (highestBufferIndex != startIndex);
  }
  for (int i = 0; i < numRemainingDigits; i ++)
    out << "0";
  output = out.str();
}

LargeIntUnsigned::LargeIntUnsigned()
{ this->theDigits.SetSize(1);
  this->theDigits[0] = 0;
}

LargeIntUnsigned::LargeIntUnsigned(unsigned int x)
{ this->AssignShiftedUInt(x, 0);
//  stOutput << "<br>DEBUG: constructor, assigning: " << x << " to large int to get: " << this->ToString();
}

LargeIntUnsigned::LargeIntUnsigned(const LargeIntUnsigned& x)
{ (*this) = x;
//  stOutput << "<br>Copy constructor";
}

void LargeIntUnsigned::AddShiftedUIntSmallerThanCarryOverBound(unsigned int x, int shift)
{ if (!(x < LargeIntUnsigned::CarryOverBound))
    crash << "Digit too large. " << crash;
  while (x > 0)
  { if (shift >= this->theDigits.size)
    { int oldsize = this->theDigits.size;
      this->theDigits.SetSize(shift + 1);
      for (int i = oldsize; i < this->theDigits.size; i ++)
        this->theDigits[i] = 0;
    }
    this->theDigits[shift] += x;
    if (this->theDigits[shift] >= LargeIntUnsigned::CarryOverBound)
    { this->theDigits[shift] -= LargeIntUnsigned::CarryOverBound;
      x = 1;
      shift ++;
    } else
      x = 0;
  }
//  this->FitSize();
}

void LargeIntUnsigned::AssignShiftedUInt(unsigned int x, int shift)
{ if (x == 0)
  { this->MakeZero();
    return;
  }
  this->theDigits.SetSize(shift + 1);
  for (int i = 0; i < shift; i ++)
    this->theDigits[i] = 0;
  unsigned int tempX = x % LargeIntUnsigned::CarryOverBound;
  this->theDigits[shift] = tempX;
  x = x / LargeIntUnsigned::CarryOverBound;
  while (x != 0)
  { tempX = x % LargeIntUnsigned::CarryOverBound;
    this->theDigits.AddOnTop(tempX);
    x = x / LargeIntUnsigned::CarryOverBound;
  }
}

void LargeIntUnsigned::AddNoFitSize(const LargeIntUnsigned& x)
{ MacroIncrementCounter(Rational::TotalLargeAdditions);
  int oldsize = this->theDigits.size;
  this->theDigits.SetSize(MathRoutines::Maximum(this->theDigits.size, x.theDigits.size) + 1);
  for (int i = oldsize; i < this->theDigits.size; i ++)
    this->theDigits[i] = 0;
  unsigned int CarryOver = 0;
  for (int i = 0; i < x.theDigits.size; i ++)
  { this->theDigits[i] += x.theDigits[i] + CarryOver;
    if (this->theDigits[i] >= LargeIntUnsigned::CarryOverBound)
    { this->theDigits[i] -= LargeIntUnsigned::CarryOverBound;
      CarryOver = 1;
    } else
      CarryOver = 0;
  }
  if (CarryOver != 0)
    for (int i = x.theDigits.size; i < this->theDigits.size; i ++)
    { this->theDigits[i] += 1;
      if (this->theDigits[i] >= LargeIntUnsigned::CarryOverBound)
        this->theDigits[i] -= LargeIntUnsigned::CarryOverBound;
      else
        break;
    }
}

void LargeIntUnsigned::operator+=(const LargeIntUnsigned& x)
{ this->AddNoFitSize(x);
  this->FitSize();
}

LargeIntUnsigned LargeIntUnsigned::operator-(const LargeIntUnsigned& other) const
{ LargeIntUnsigned result;
  result = *this;
  result.SubtractSmallerPositive(other);
  return std::move(result);
}

std::string LargeIntUnsigned::ToStringAbbreviate(FormatExpressions *theFormat) const
{ (void) theFormat;
  std::string result = this->ToString(theFormat);
  if (result.size() > 90)
  { std::stringstream out;
    out << result.substr(0, 40)
    << "...(" << (result.size() - 80) << " omitted)"
    << result.substr(result.size() - 40);
    result = out.str();
  }
  return result;
}

void LargeIntUnsigned::PadWithZeroesToAtLeastNDigits(int desiredMinNumDigits)
{ if (this->theDigits.size >= desiredMinNumDigits)
    return;
  int i = this->theDigits.size;
  this->theDigits.SetSize(desiredMinNumDigits);
  for (; i < desiredMinNumDigits; i ++)
    this->theDigits[i] = 0;
}

void LargeIntUnsigned::AddLargeIntUnsignedShiftedTimesDigit(const LargeIntUnsigned& other, int digitShift, int theConst)
{ if (theConst >= this->CarryOverBound || (- theConst) <= (- this->CarryOverBound))
    crash << "Digit: " << theConst << " is too large" << crash;
  //stOutput << "<br>DEBUG: Adding " << other.ToString();
  //for (int i = 0; i <digitShift; i ++)
  //  stOutput << "0";
  //stOutput << " times " << theConst
  //<< " to " << this->ToString() << "\n";
  int numDigits = MathRoutines::Maximum(other.theDigits.size + 1 + digitShift, this->theDigits.size + 1);
  this->PadWithZeroesToAtLeastNDigits(numDigits);
  long long nextDigit = 0;
  for (int j = 0; j < other.theDigits.size; j ++)
  { int currentIndex = j + digitShift;
    int nextIndex = currentIndex + 1;
    nextDigit = other.theDigits[j];
    nextDigit *= theConst;
    nextDigit += this->theDigits[currentIndex];
    this->theDigits[currentIndex] = (int) (nextDigit % this->CarryOverBound);
    this->theDigits[nextIndex] += (int) (nextDigit / this->CarryOverBound);
    if (this->theDigits[currentIndex] < 0)
    { this->theDigits[currentIndex] += this->CarryOverBound;
      this->theDigits[nextIndex] --;
    }
    if (this->theDigits[currentIndex] < 0)
      crash << "Non-positive non-leading digit." << crash;
  }
  this->FitSize();
  int lastDigit =* this->theDigits.LastObject();
  if (lastDigit >= this->CarryOverBound || (- lastDigit) <= (- this->CarryOverBound))
    crash << "Leading digit: " << lastDigit << " is too large" << crash;
  //stOutput << " to get: "
  //<< this->ToString() << "\n";

//  if (!this->CheckForConsistensy())crash << crash;
}

void LargeIntUnsigned::SubtractSmallerPositive(const LargeIntUnsigned& x)
{ int CarryOver = 0;
  //if (!this->IsGEQ(x))
  //  crash << "This is a programming error: attempting to subtract a larger LargeIntUnsigned from a smaller one. " << crash;
  for (int i = 0; i < x.theDigits.size; i ++)
  { int nextDigit = x.theDigits[i] + CarryOver;
    if (this->theDigits[i] < nextDigit)
    { this->theDigits[i] += LargeIntUnsigned::CarryOverBound;
      this->theDigits[i] -= nextDigit;
      CarryOver = 1;
    } else
    { this->theDigits[i] -= nextDigit;
      CarryOver = 0;
    }
  }
  if (CarryOver != 0)
  { for (int i = x.theDigits.size; i < this->theDigits.size; i ++)
      if (this->theDigits[i] > 0)
      { this->theDigits[i] --;
        break;
      } else
        this->theDigits[i] = LargeIntUnsigned::CarryOverBound - 1;
  }
  this->FitSize();
//  if (!this->CheckForConsistensy())crash << crash;
}

void LargeIntUnsigned::MultiplyBy(const LargeIntUnsigned& x, LargeIntUnsigned& output) const
{ if (this == &output || &x == &output)
  { LargeIntUnsigned thisCopy = *this;
    LargeIntUnsigned xCopy = x;
    return thisCopy.MultiplyBy(xCopy, output);
  }
  MacroIncrementCounter(Rational::TotalLargeMultiplications);
  output.theDigits.SetSize(x.theDigits.size + this->theDigits.size);
  for (int i = 0; i < output.theDigits.size; i ++)
    output.theDigits[i] = 0;
  unsigned long long numCycles = 0;
  bool doProgressReporT = theGlobalVariables.flagReportEverything || theGlobalVariables.flagReportLargeIntArithmetic;
  ProgressReport theReport1, theReport2;
  unsigned long long totalCycles = 0;
  if (doProgressReporT)
  { totalCycles = ((unsigned long long) this->theDigits.size) * ((unsigned long long) x.theDigits.size);
    doProgressReporT = totalCycles > 2000;
  }
  if (doProgressReporT)
  { std::stringstream reportStream;
    reportStream
    << "<br>Large integer multiplication: product of integers:<br>\n"
    << this->ToStringAbbreviate()
    << "<br>\n" << x.ToStringAbbreviate();
    theReport1.Report(reportStream.str());
  }
  for (int i = 0; i < this->theDigits.size; i ++)
    for (int j = 0; j < x.theDigits.size; j ++)
    { unsigned long long tempLong = this->theDigits[i];
      unsigned long long tempLong2 = x.theDigits[j];
      tempLong = tempLong * tempLong2;
      unsigned long long lowPart = tempLong % LargeIntUnsigned::CarryOverBound;
      unsigned long long highPart = tempLong / LargeIntUnsigned::CarryOverBound;
      output.AddShiftedUIntSmallerThanCarryOverBound((unsigned int) lowPart, i + j);
      output.AddShiftedUIntSmallerThanCarryOverBound((unsigned int) highPart, i + j + 1);
      if (doProgressReporT)
      { numCycles ++;
        if ((numCycles % 1024) == 0)
        { std::stringstream out;
          out << "<br>Crunching " << numCycles << " out of " << totalCycles
          << " pairs of large integer ``digits'' = "
          << this->theDigits.size << " x " << x.theDigits.size
          << " digits (base " << LargeIntUnsigned::CarryOverBound << ").";
          theReport2.Report(out.str());
        }
      }
    }
  output.FitSize();
//  if (!this->CheckForConsistensy())crash << crash;
}

LargeIntUnsigned LargeIntUnsigned::operator%(const LargeIntUnsigned& other) const
{ LargeIntUnsigned result, temp;
  this->DivPositive(other, temp, result);
  return std::move(result);
}

int LargeIntUnsigned::GetUnsignedIntValueTruncated()
{ return (int) this->theDigits[0];
}

double LargeIntUnsigned::GetDoubleValue() const
{ double result = 0;
  for (int i = this->theDigits.size - 1; i >= 0; i --)
    result = result * LargeIntUnsigned::CarryOverBound + this->theDigits[i];
  return result;
}

LargeIntUnsigned LargeIntUnsigned::lcm(const LargeIntUnsigned& a, const LargeIntUnsigned& b)
{ LargeIntUnsigned output;
  LargeIntUnsigned::lcm(a, b, output);
  return output;
}

LargeIntUnsigned LargeIntUnsigned::gcd(const LargeIntUnsigned& a, const LargeIntUnsigned& b)
{ LargeIntUnsigned output;
  LargeIntUnsigned::gcd(a, b, output);
  return output;
}

void LargeIntUnsigned::gcd(const LargeIntUnsigned& a, const LargeIntUnsigned& b, LargeIntUnsigned& output)
{ MacroIncrementCounter(Rational::TotalLargeGCDcalls);
  LargeIntUnsigned p, q, r, temp;
  p = a;
  q = b;
  while (!q.IsEqualToZero())
  { p.DivPositive(q, temp, r);
    p = q;
    q = r;
  }
  output = p;
//  if (!output.CheckForConsistensy())crash << crash;
}

void LargeIntUnsigned::FitSize()
{ int newSize = this->theDigits.size;
  for (int i = this->theDigits.size - 1; i >= 1; i --)
    if (this->theDigits[i] == 0)
      newSize --;
    else
      break;
  this->theDigits.SetSize(newSize);
//  if (!this->CheckForConsistensy())crash << crash;
}

void LargeIntUnsigned::AccountFactor(const LargeInt& theP, List<LargeInt>& outputPrimeFactors, List<int>& outputMultiplicities) const
{ if (outputPrimeFactors.size == 0)
  { outputPrimeFactors.AddOnTop(theP);
    outputMultiplicities.AddOnTop(1);
    return;
  }
  if ((*outputPrimeFactors.LastObject()).operator==(theP))
    (*outputMultiplicities.LastObject()) ++;
  else
  { outputPrimeFactors.AddOnTop(theP);
    outputMultiplicities.AddOnTop(1);
  }
}

bool LargeIntUnsigned::FactorLargeReturnFalseIfFactorizationIncomplete
(List<LargeInt>& outputFactors, List<int>& outputMultiplicites, int dontSearchForDivisorsLargerThan,
 std::stringstream* commentsOnFailure) const
{ MacroRegisterFunctionWithName("LargeIntUnsigned::FactorLarge");
  int maximumNumberOfDigits = 1000;
  if (this->theDigits.size > maximumNumberOfDigits)
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Number has too many digits: maximum " << maximumNumberOfDigits
      << " digits accepted for factorization attempt. ";
    return false;
  }
  if (this->IsEqualToZero())
    crash << "This is a programming error: it was requested that I factor 0, which is forbidden. " << crash;
  LargeIntUnsigned toBeFactored = *this;
  outputFactors.SetSize(0);
  outputMultiplicites.SetSize(0);
  if (dontSearchForDivisorsLargerThan <= 0)
    dontSearchForDivisorsLargerThan = 100000;
  List<bool> theSieve;
  theSieve.initFillInObject(dontSearchForDivisorsLargerThan + 1, true);
  for (unsigned int i = 2; i <= (unsigned) dontSearchForDivisorsLargerThan; i ++)
  { if (!theSieve[i])
      continue;
    //LargeIntUnsigned current = toBeFactored % i;
    while (toBeFactored % i == 0)
    { this->AccountFactor(i, outputFactors, outputMultiplicites);
      toBeFactored /= i;
    }
    for (unsigned int j = i; j <= (unsigned) dontSearchForDivisorsLargerThan; j += i)
      theSieve[j] = false;
  }
  if (toBeFactored > 1)
  { this->AccountFactor(toBeFactored, outputFactors, outputMultiplicites);
    if (!toBeFactored.IsPossiblyPrimeMillerRabin(10, 0))
    { *commentsOnFailure
      << "The largest remaining factor "
      << toBeFactored
      << " is known not to be prime (Miller-Rabin test) "
      << "but I could not factor it. I checked all factors smaller than or equal to: "
      << dontSearchForDivisorsLargerThan;
      return false;
    }
  }
  return true;
}

bool LargeIntUnsigned::FactorReturnFalseIfFactorizationIncomplete
(List<LargeInt>& outputFactors, List<int>& outputMultiplicites,
 int dontSearchForDivisorsLargerThan, std::stringstream* commentsOnFailure) const
{ MacroRegisterFunctionWithName("LargeIntUnsigned::Factor");
  if (this->theDigits.size > 1)
    return this->FactorLargeReturnFalseIfFactorizationIncomplete(outputFactors, outputMultiplicites, dontSearchForDivisorsLargerThan, commentsOnFailure);
  if (this->IsEqualToZero())
    crash << "This is a programming error: it was requested that I factor 0, which is forbidden." << crash;
  //stOutput << "Factoring: " << this->ToString();
  unsigned int toBeFactored = this->theDigits[0];
  outputFactors.SetSize(0);
  outputMultiplicites.SetSize(0);
  int upperboundPrimeDivisors = (unsigned int) FloatingPoint::sqrt((double) toBeFactored);
  List<bool> theSieve;
  theSieve.initFillInObject(upperboundPrimeDivisors + 1, true);
  for (unsigned int i = 2; i <= (unsigned) upperboundPrimeDivisors; i ++)
  { if (!theSieve[i])
      continue;
    while (toBeFactored % i == 0)
    { this->AccountFactor(i, outputFactors, outputMultiplicites);
      toBeFactored /= i;
      upperboundPrimeDivisors = (unsigned int) FloatingPoint::sqrt((double) toBeFactored);
    }
    for (unsigned int j = i; j <= (unsigned) upperboundPrimeDivisors; j += i)
      theSieve[j] = false;
  }
  if (toBeFactored > 1)
    this->AccountFactor(toBeFactored, outputFactors, outputMultiplicites);
  //stOutput << " ... and the factors are: " << outputPrimeFactors
  //<< " with mults: " << outputMultiplicites;
  return true;
}

void LargeIntUnsigned::lcm(const LargeIntUnsigned& a, const LargeIntUnsigned& b, LargeIntUnsigned& output)
{ LargeIntUnsigned tempUI, tempUI2;
  if (a.IsEqualToZero() || b.IsEqualToZero())
    crash << "This is a programming error: calling lcm on zero elements is not allowed. " << crash;
  LargeIntUnsigned::gcd(a, b, tempUI);
  a.MultiplyBy(b, tempUI2);
  output = tempUI2;
  output.DivPositive(tempUI, output, tempUI2);
  if (output.IsEqualToZero())
    crash << crash;
}

void LargeIntUnsigned::operator=(const LargeIntUnsigned& x)
{ this->theDigits = x.theDigits;
  //stOutput << "<br>Copy assignment" << crash.GetStackTraceShort();
}

void LargeIntUnsigned::operator=(unsigned int x)
{ this->AssignShiftedUInt(x, 0);
}

unsigned int LargeIntUnsigned::HashFunction() const
{ int numCycles = MathRoutines::Minimum(this->theDigits.size, SomeRandomPrimesSize);
  unsigned int result = 0;
  for (int i = 0; i < numCycles; i ++)
    result += this->theDigits[i] * SomeRandomPrimes[i];
  return result;
}

bool LargeIntUnsigned::IsPositive() const
{ return this->theDigits.size > 1 || this->theDigits[0] > 0;
}

bool LargeIntUnsigned::IsEqualToOne() const
{ return this->theDigits.size == 1 && this->theDigits[0] == 1;
}

bool LargeIntUnsigned::IsEqualToZero() const
{ return this->theDigits.size == 1 && this->theDigits[0] == 0;
}

void LargeIntUnsigned::AssignFactorial(unsigned int x)
{ this->MakeOne();
  List<unsigned int> primesBelowX;
  LargeIntUnsigned::GetAllPrimesSmallerThanOrEqualToUseEratosthenesSieve(x, primesBelowX);
  LargeIntUnsigned tempInt, tempOne;
  tempOne.MakeOne();
  for (int i = 0; i < primesBelowX.size; i ++)
  { unsigned int thePrime = primesBelowX.TheObjects[i];
    unsigned int thePowerOfThePrime = 0;
    unsigned int currentPower = thePrime;
    do
    { thePowerOfThePrime += x / currentPower;
      currentPower *= thePrime;
    }
    while (currentPower <= x);
    tempInt.AssignShiftedUInt(thePrime, 0);
    MathRoutines::RaiseToPower(tempInt, thePowerOfThePrime, tempOne);
    *this *= tempInt;
/*    if (theGlobalVariables != 0)
    { std::stringstream out;
      out << "processing prime " << thePrime << " (" << i + 1 << " out of " << primesBelowX.size << ").";
      theGlobalVariables->theIndicatorVariables.ProgressReportStrings[0] = out.str();
      theGlobalVariables->theIndicatorVariables.ProgressReportStringsNeedRefresh = true;
      theGlobalVariables->MakeReport();
    }*/
  }
}

bool LargeInt::IsEven() const
{ return this->value.IsEven();
}

void LargeInt::WriteToFile(std::fstream& output)
{ std::string tempS;
  this->ToString(tempS);
  output << tempS;
}

void LargeInt::ReadFromFile(std::fstream& input)
{ std::string tempS;
  input >> tempS;
  this->AssignString(tempS);
}

void LargeInt::AssignString(const std::string& input)
{ if (input.size() == 0)
    return;
  this->MakeZero();
  unsigned int startingIndex = 0;
  if (input[0] == '-')
    startingIndex = 1;
  for (unsigned int i = startingIndex; i < input.size(); i ++)
  { int x = input[i] - '0';
    if (x < 0 || x > 9)
      crash << "LargeInt::AssignString is not allowed to fail." << crash;
    if (i > startingIndex)
      this->value *= 10;
    this->value += (unsigned) x;
  }
  if (!this->IsEqualToZero())
    if (input[0] == '-')
      this->sign = - 1;
}

bool LargeInt::IsIntegerFittingInInt(int* whichInt)
{ if (!this->value.IsIntegerFittingInInt(whichInt))
    return false;
  if (whichInt != 0)
    *whichInt *= this->sign;
  return true;
}

void LargeInt::MultiplyByInt(int x)
{//  if (this->value.size == 0) return;
  LargeInt tempI;
  tempI.AssignInt(x);
  *this *= tempI;
}

bool LargeInt::TryToFindWhetherIsPower(bool& outputIsPower, LargeInt& outputBase, int& outputPower) const
{ if (this->sign == - 1)
    return false;
  return this->value.TryToFindWhetherIsPower(outputIsPower, outputBase, outputPower);
}

bool LargeInt::operator==(const LargeInt& x) const
{ if (x.sign != this->sign)
  { if (x.IsEqualToZero() && this->IsEqualToZero())
      return true;
    else
      return false;
  }
  return this->value == x.value;
}

bool LargeInt::CheckForConsistensy()
{ if (this->sign != - 1 && this->sign != 1)
    return false;
  for (int i = 0; i < this->value.theDigits.size; i ++)
    if (this->value.theDigits[i] >= LargeIntUnsigned::CarryOverBound)
      return false;
  return true;
}

double LargeInt::GetDoubleValue() const
{ return this->sign * this->value.GetDoubleValue();
}

void LargeInt::ToString(std::string& output) const
{ std::stringstream out;
  if (this->IsEqualToZero())
  { output.assign("0");
    return;
  }
  if (this->sign == - 1)
    out << "-";
  std::string tempS;
  this->value.ToString(tempS);
  out << tempS;
  output = out.str();
}

void LargeInt::AssignInt(int x)
{ if (x == 0)
  { this->MakeZero();
    return;
  }
  this->sign = 1;
  if (x < 0)
  { this->sign = - 1;
    x = - x;
  }
  this->value.AssignShiftedUInt((unsigned int) x, 0);
//  if (!this->CheckForConsistensy())crash << crash;
}

bool LargeInt::GetDivisors(List<int>& output, bool includeNegative)
{ if (this->value.theDigits.size > 1)
    return false;
  int val = this->value.theDigits[0];
  if (val > 50000)
    return false;
  output.SetSize(0);
  for (int i = 1; i <= val; i ++)
    if (val % i == 0)
    { output.AddOnTop(i);
      if (includeNegative)
        output.AddOnTop(- i);
    }
  return true;
}

void LargeInt::AddLargeIntUnsigned(const LargeIntUnsigned& x)
{ if (this->sign == 1)
  { this->value += x;
    return;
  }
  if (this->value.IsGEQ(x))
    this->value.SubtractSmallerPositive(x);
  else
  { LargeIntUnsigned tempI = x;
    tempI.SubtractSmallerPositive(this->value);
    this->value = tempI;
    this->sign = 1;
  }
}

void LargeInt::operator+=(const LargeInt& x)
{ if (this->sign == x.sign)
    this->value += x.value;
  else
  { if (this->value.IsGEQ(x.value))
      this->value.SubtractSmallerPositive(x.value);
    else
    { LargeIntUnsigned tempI = this->value;
      this->value = x.value;
      this->value.SubtractSmallerPositive(tempI);
      this->sign = x.sign;
    }
  }
//  if (!this->CheckForConsistensy())crash << crash;
}

void LargeInt::MakeZero()
{ this->value.MakeZero();
  this->sign = 1;
}

void LargeInt::operator=(const Rational& other)
{ if (!other.IsInteger(this))
    crash << "This is a programming error: converting implicitly rational number " << other.ToString()
    << " to integer is not possible as the Rational number is not integral. " << crash;
//  if (!this->CheckForConsistensy())crash << crash;
}

void LargeInt::operator=(const LargeInt& x)
{ this->sign = x.sign;
  this->value = x.value;
//  if (!this->CheckForConsistensy())crash << crash;
}

LargeInt LargeInt::operator/(int x) const
{ LargeInt result;
  LargeIntUnsigned remainder;
  LargeIntUnsigned tempX;
  int absX = x;
  signed char signX = 1;
  if (x < 0)
  { signX = - 1;
    absX = - absX;
  }
  tempX.AssignShiftedUInt(absX, 0);
  this->value.DivPositive(tempX, result.value, remainder);
//  if (!result.CheckForConsistensy()) crash << crash;
  result.sign = this->sign * signX;
  return result;
}

LargeInt LargeInt::operator/(LargeInt& x) const
{ LargeInt result;
  LargeInt remainder;
  this->value.DivPositive(x.value, result.value, remainder.value);
  result.sign = this->sign * x.sign;
  if (!result.CheckForConsistensy())
    crash << crash;
  return result;
}

int LargeInt::operator%(int x)
{ if (x <= 0)
    crash << crash;
  LargeIntUnsigned result;
  LargeIntUnsigned remainder;
  LargeIntUnsigned tempX;
  if (x < 0)
    x = - x;
  tempX.AssignShiftedUInt(x, 0);
  this->value.DivPositive(tempX, result, remainder);
  if (remainder.theDigits.size == 0)
    return 0;
  else
  { if (this->sign == - 1)
      return x - remainder.theDigits[0];
    else
      return remainder.theDigits[0];
  }
}

void Rational::WriteToFile(std::fstream& output)
{ output << this->ToString();
}

void Rational::RaiseToPower(int x)
{ Rational tempRat;
  tempRat.MakeOne();
  if (x < 0)
  { x = - x;
    this->Invert();
  }
  LargeInt tempNum = this->GetNumerator();
  LargeIntUnsigned oneLI;
  oneLI.MakeOne();
  MathRoutines::RaiseToPower(tempNum.value, x, oneLI);
  LargeIntUnsigned tempDen = this->GetDenominator();
  MathRoutines::RaiseToPower(tempDen, x, oneLI);
  int theSign = (this->IsPositive() || x % 2 == 0) ? 1 :- 1;
  this->AllocateExtended();
  this->Extended->num.sign = (signed char) theSign;
  this->Extended->den = tempDen;
  this->Extended->num.value = tempNum.value;
  this->ShrinkExtendedPartIfPossible();
}

void Rational::Invert()
{ if (this->Extended == 0)
  { int tempI = this->DenShort;
    if (tempI <= 0)
      crash << crash;
    if (this->NumShort < 0)
    { this->DenShort = - this->NumShort;
      this->NumShort = - tempI;
    } else
    { this->DenShort = this->NumShort;
      this->NumShort = tempI;
    }
    return;
  }
  MathRoutines::swap(this->Extended->den, this->Extended->num.value);
}

void Rational::ReadFromFile(std::istream& input)
{ std::string tempS;
  input >> tempS;
  this->AssignString(tempS);
}

void Rational::MultiplyByInt(int x)
{ Rational tempRat;
  tempRat.AssignInteger(x);
  this->MultiplyBy(tempRat);
}

void Rational::MultiplyBy(const Rational& r)
{ if (r.Extended == 0 && this->Extended == 0)
    if (this->TryToMultiplyQuickly(r.NumShort, r.DenShort))
      return;
  this->InitExtendedFromShortIfNeeded();
  if (r.Extended != 0)
  { this->Extended->num *= r.Extended->num;
    this->Extended->den *= r.Extended->den;
  } else
  { this->Extended->num.MultiplyByInt(r.NumShort);
    this->Extended->den.MultiplyByUInt((unsigned int) r.DenShort);
  }
  this->Simplify();
}

Rational operator/(int left, const Rational& right)
{ Rational tempRat = left;
  tempRat /= right;
  return tempRat;
}

Rational operator-(const Rational& argument)
{ Rational tempRat;
  tempRat.Assign(argument);
  tempRat.Minus();
  return tempRat;
}

void Rational::MultiplyByLargeInt(LargeInt& x)
{ this->InitExtendedFromShortIfNeeded();
  this->Extended->num *= x;
  this->Simplify();
}

void Rational::MultiplyByLargeIntUnsigned(LargeIntUnsigned& x)
{ this->InitExtendedFromShortIfNeeded();
  this->Extended->num.value.MultiplyBy(x);
  this->Simplify();
}

void Rational::DivideBy(const Rational& r)
{ if (this == &r)
  { this->MakeOne();
    return;
  }
  Rational tempRat = r;
  tempRat.Invert();
  this->operator*=(tempRat);
}

Rational Rational::operator/(const Rational& right) const
{ Rational tempRat;
  tempRat.Assign(*this);
  tempRat.DivideBy(right);
  return tempRat;
}

Rational Rational::operator*(const Rational& right) const
{ Rational tempRat;
  tempRat.Assign(*this);
  tempRat.MultiplyBy(right);
  return tempRat;
}

Rational Rational::operator+(const Rational& right) const
{ Rational tempRat;
  tempRat.Assign(*this);
  tempRat += right;
  return tempRat;
}

Rational Rational::operator-(const Rational& right) const
{ Rational tempRat;
  tempRat.Assign(*this);
  tempRat -= right;
  return tempRat;
}

void Rational::Assign(const Rational& r)
{ this->NumShort = r.NumShort;
  this->DenShort = r.DenShort;
  if (r.Extended == 0)
  { if (this->Extended == 0)
      return;
    this->FreeExtended();
    return;
  }
  this->InitExtendedFromShortIfNeeded();
  this->Extended->num = r.Extended->num;
  this->Extended->den = r.Extended->den;
}

void Rational::AssignFracValue()
{ if (this->Extended == 0)
  { if (this->NumShort == 0)
      return;
    if (this->DenShort == 1)
    { this->NumShort = 0;
      this->DenShort = 1;
      return;
    }
    this->NumShort = this->NumShort % this->DenShort;
    if (this->NumShort < 0)
      this->NumShort += this->DenShort;
    return;
  }
  if (this->IsEqualToZero())
    return;
  if (this->Extended->den.IsEqualToOne())
  { this->MakeZero();
    return;
  }
  LargeIntUnsigned newNum, tempI;
  this->Extended->num.value.DivPositive(this->Extended->den, tempI, newNum);
  this->Extended->num.value = newNum;
  if (this->Extended->num.IsNegative())
    this->Extended->num.AddLargeIntUnsigned(this->Extended->den);
  if (!this->Extended->num.IsPositiveOrZero())
    crash << crash;
  this->Simplify();
}

void Rational::AddInteger(int x)
{ Rational tempRat;
  tempRat.AssignNumeratorAndDenominator(x, 1);
  this->operator+=(tempRat);
}

bool Rational::IsGreaterThan(const Rational& r) const
{ Rational tempRat;
  tempRat.Assign(*this);
  tempRat.Subtract(r);
  return tempRat.IsPositive();
}

void Rational::Subtract(const Rational& r)
{ Rational temp;
  temp.Assign(r);
  temp.Minus();
  this->operator+=(temp);
}

bool Rational::GetSquareRootIfRational(Rational& output) const
{ if (*this < 0)
    return false;
  LargeInt theNum = this->GetNumerator();
  LargeIntUnsigned theDen = this->GetDenominator();
  List<LargeInt> primeFactorsNum, primeFactorsDen;
  List<int> multsNum, multsDen;
  if (!theNum.value.FactorReturnFalseIfFactorizationIncomplete(primeFactorsNum, multsNum, 0, 0))
    return false;
  if (!theDen.FactorReturnFalseIfFactorizationIncomplete(primeFactorsDen, multsDen, 0, 0))
    return false;
  output = 1;
  Rational tempRat;
  for (int i = 0; i < primeFactorsNum.size; i ++)
  { if (multsNum[i] % 2 != 0)
      return false;
    tempRat = primeFactorsNum[i];
    int currentMult = multsNum[i];
    tempRat.RaiseToPower(currentMult / 2);
    output *= tempRat;
  }
  for (int i = 0; i < primeFactorsDen.size; i ++)
  { if (multsDen[i] % 2 != 0)
      return false;
    tempRat = primeFactorsDen[i];
    int currentMult = multsDen[i];
    tempRat.RaiseToPower(currentMult / 2);
    output /= tempRat;
  }
  return true;
}

bool Rational::GetPrimeFactorsAbsoluteValue
(List<LargeInt>& numeratorPrimeFactors, List<int>& numeratorMultiplicities,
 List<LargeInt>& denominatorPrimeFactors, List<int>& denominatorMultiplicities)
{ MacroRegisterFunctionWithName("Rational::GetPrimeFactorsAbsoluteValue");
  if (!this->GetNumerator().value.FactorReturnFalseIfFactorizationIncomplete(numeratorPrimeFactors, numeratorMultiplicities, 0, 0))
    return false;
  return this->GetDenominator().FactorReturnFalseIfFactorizationIncomplete(denominatorPrimeFactors, denominatorMultiplicities, 0, 0);
}

void Rational::AssignInteger(int x)
{ this->FreeExtended();
  this->DenShort = 1;
  this->NumShort = x;
}

bool Rational::ShrinkExtendedPartIfPossible()
{ if (this->Extended == 0)
    return true;
  if (this->Extended->num.value.theDigits.size > 1 ||
      this->Extended->den.theDigits.size > 1 ||
      this->Extended->num.value.theDigits[0] >= LargeIntUnsigned::SquareRootOfCarryOverBound ||
      this->Extended->den.theDigits[0] >= LargeIntUnsigned::SquareRootOfCarryOverBound)
    return false;
  this->NumShort = this->Extended->num.GetIntValueTruncated();
  this->DenShort = this->Extended->den.GetUnsignedIntValueTruncated();
  this->FreeExtended();
  return true;
}

Rational Rational::Factorial(int n)
{ if (n < 0)
  { crash << "This is a programming error: taking factorial of the negative number " << n << ". " << crash;
    return 0;
  }
  LargeIntUnsigned result;
  result.AssignFactorial((unsigned int) n);
  Rational answer;
  answer = result;
  return answer;
}

Rational Rational::TwoToTheNth(int n)
{ Rational result = 1;
  if (n>= 0)
    for (int i = 0; i < n; i ++)
      result.MultiplyByInt(2);
  else
    for (int i = 0; i > n; i --)
      result.DivideByInteger(2);
  return result;
}

Rational Rational::NChooseK(const Rational& n, int k)
{ Rational result;
  result.MakeOne();
  for (int i = 0; i < k; i ++)
  { //stOutput << "<br>" << result.ToString() << " * " << (n-i).ToString() << "/" << i + 1;
    result *= n - i;
    result.DivideByInteger(i + 1);
    //stOutput << "=" << result.ToString();
  }
  return result;
}

Rational Rational::NtoTheKth(int n, int k)
{ Rational result = n;
  result.RaiseToPower(k);
  return result;
}

bool Rational::IsInteger(LargeInt* whichInteger) const
{ bool result = false;
  if (this->Extended == 0)
  { result = (this->DenShort == 1);
    if (whichInteger != 0)
      *whichInteger = this->NumShort;
  } else
    if (this->Extended->den.IsEqualToOne())
    { result = true;
      if (whichInteger != 0)
        *whichInteger = this->Extended->num;
    }
  return result;
}

bool MathRoutines::IsInteger(Rational x)
{ return x.IsInteger();
}

double Rational::GetDoubleValue() const
{ if (this->Extended == 0)
    return (double) this->NumShort / (double) this->DenShort;
  else
    return this->Extended->num.GetDoubleValue() / this->Extended->den.GetDoubleValue();
}

void Rational::Simplify()
{ if (this->Extended == 0)
  { if (this->NumShort == 0)
      this->DenShort = 1;
    else
    { if (this->DenShort == 1)
        return;
      int tempGCD;
      if (this->NumShort > 0)
        tempGCD = this->gcd(this->NumShort, this->DenShort);
      else
        tempGCD = this->gcd(- this->NumShort, this->DenShort);
      this->NumShort /= tempGCD;
      this->DenShort /= tempGCD;
    }
    return;
  }
  if (this->Extended->num.IsEqualToZero())
  { this->MakeZero();
    return;
  }
  if (!this->Extended->den.IsEqualToOne())
  { LargeIntUnsigned tempI;
    LargeIntUnsigned::gcd(this->Extended->den, this->Extended->num.value, tempI);
    /*if (Rational::flagAnErrorHasOccurredTimeToPanic)
    { std::string tempS1, tempS2, tempS3;
      tempI.ToString(tempS1);
      this->ToString(tempS2);
    }*/
    LargeIntUnsigned tempI2;
    this->Extended->den.DivPositive(tempI, this->Extended->den, tempI2);
    this->Extended->num.value.DivPositive(tempI, this->Extended->num.value, tempI2);
  }
  this->ShrinkExtendedPartIfPossible();
}

/*
void Rational::DrawElement(DrawElementInputOutput& theDrawData)
{ std::string tempS;
  tempS = this->ToString();
  theGlobalVariables.theDrawingVariables.theBuffer.drawTextBuffer
  (theDrawData.TopLeftCornerX, theDrawData.TopLeftCornerY, tempS, 0, theGlobalVariables.theDrawingVariables.fontSizeNormal, theGlobalVariables.theDrawingVariables.TextStyleNormal);
  theDrawData.outputHeight =10;
  theDrawData.outputWidth =10*tempS.size();
}*/

void Rational::operator=(const Polynomial<Rational>& other)
{ if (!other.IsConstant(this))
    crash << "This is a programming error: attempting to assign non-constant polynomial to a Rational number is not allowed. "
    << crash;
}

bool Rational::IsEqualTo(const Rational& b) const
{ if (this->Extended == 0 && b.Extended == 0)
    return (this->NumShort * b.DenShort == b.NumShort * this->DenShort);
  Rational tempRat;
  tempRat.Assign(*this);
  tempRat.Subtract(b);
  return tempRat.IsEqualToZero();
}

bool Rational::IsGreaterThanOrEqualTo(const Rational& right) const
{ if (this->Extended == 0 && right.Extended == 0)
    return (this->NumShort * right.DenShort >= right.NumShort * this->DenShort);
  Rational tempRat;
  tempRat.Assign(*this);
  tempRat.Subtract(right);
  return tempRat.IsPositiveOrZero();
}

std::string Rational::ToString(FormatExpressions* theFormat) const
{ if (theFormat != 0)
    if (theFormat->flagUseFrac)
      return this->ToStringFrac();
  std::stringstream out;
  if (this->Extended == 0)
  { out << this->NumShort;
    if (this->DenShort != 1)
      out << "/" << this->DenShort;
  } else
  { std::string tempS;
    this->Extended->num.ToString(tempS);
    out << tempS;
    this->Extended->den.ToString(tempS);
    if (tempS != "1")
      out << "/" << tempS;
  }
  return out.str();
}

std::string Rational::ToStringForFileOperations(FormatExpressions* notUsed) const
{ (void) notUsed; //portable way of avoiding unused parameter warning
  std::stringstream out;
  if (this->Extended == 0)
  { if (this->NumShort < 0)
      out << "-";
    int numShortAbsoluteValue = this->NumShort < 0 ? - this->NumShort: this->NumShort;
    if (this->DenShort == 1)
      out << numShortAbsoluteValue;
    else
      out << numShortAbsoluteValue << "_div_" << this->DenShort;
    return out.str();
  }
  LargeInt numAbsVal = this->Extended->num;
  if (numAbsVal < 0)
  { out << "-";
    numAbsVal.sign = 1;
  }
  if (this->Extended->den.IsEqualToOne())
    out << numAbsVal.ToString();
  else
    out << numAbsVal.ToString() << "_div_" << this->Extended->den.ToString();
  return out.str();
}

std::string Rational::ToStringFrac() const
{ std::stringstream out;
  if (this->Extended == 0)
  { if (this->NumShort < 0)
      out << "-";
    int numShortAbsoluteValue = this->NumShort < 0 ? - this->NumShort: this->NumShort;
    if (this->DenShort == 1)
      out << numShortAbsoluteValue;
    else
      out << "\\frac{" << numShortAbsoluteValue << "}{" << this->DenShort << "}";
    return out.str();
  }
  LargeInt numAbsVal = this->Extended->num;
  if (numAbsVal < 0)
  { out << "-";
    numAbsVal.sign = 1;
  }
  if (this->Extended->den.IsEqualToOne())
    out << numAbsVal.ToString();
  else
    out << "\\frac{" << numAbsVal.ToString() << "}{" << this->Extended->den.ToString() << "}";
  return out.str();
}

void Rational::operator=(const AlgebraicNumber& other)
{ bool isGood = other.IsRational(this);
  if (!isGood)
    crash << "This is a programming error: attempting to assign the non-rational algebraic number " << other.ToString() << "to a rational number. " << crash;
}

bool Rational::AssignStringFailureAllowed(const std::string& input)
{ this->MakeZero();
  if (input == "")
    return false;
  if (input == "0")
    return true;
  int sign = 1;
  int ReaderDigit = - 1;
  Rational readerDen = 1;
  int positionInString = 0;
  for (; positionInString < (signed) input.size(); positionInString ++)
  { if (input[positionInString] == '-')
    { sign = - 1;
      positionInString ++;
      break;
    }
    if (MathRoutines::isADigit(input[positionInString]))
      break;
  }
  for (; positionInString < (signed) input.size(); positionInString ++)
  { if (MathRoutines::isADigit(input[positionInString], &ReaderDigit))
    { *this *= 10;
      *this += ReaderDigit;
    }
    if (input[positionInString] == '/')
    { positionInString ++;
      readerDen = 0;
      break;
    }
  }
  for (; positionInString < (signed) input.size(); positionInString ++)
    if (MathRoutines::isADigit(input[positionInString], &ReaderDigit))
    { readerDen *= 10;
      readerDen += ReaderDigit;
    }
  if (readerDen.IsEqualToZero())
    return false;
  *this /= readerDen;
  *this *= sign;
  return true;
}

void Rational::AssignString(const std::string& input)
{ if (!Rational::AssignStringFailureAllowed(input))
    crash << "Programming error: Rational::AssignString failed (likely a zero denominator). " << crash;
}

// haha, qtcreator doesn't understand this cxx11 code.  guess it wasn't such
// a good idea to use it.  anyway "3/5"_R is now a rational literal.
Rational operator"" _R(const char* in, size_t insize)
{ (void) insize;//remove unused variable warning, portable.
  Rational x;
  x.AssignString(in);
  return x;
}
