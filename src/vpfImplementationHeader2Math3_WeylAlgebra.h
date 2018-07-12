//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfImplementationHeaderWeylAlgebras_already_included
#define vpfImplementationHeaderWeylAlgebras_already_included

#include "vpfHeader2Math0_General.h"
#include "vpfHeader2Math2_AlgebraicNumbers.h"
static ProjectInformationInstance ProjectInfovpfImplementationHeaderWeylAlgebras(__FILE__, "Implementation header, Weyl algebra routines. ");

template <class coefficient>
bool ElementWeylAlgebra<coefficient>::IsPolynomial(Polynomial<coefficient>* whichPoly) const
{ if (whichPoly != 0)
    whichPoly->MakeZero();
  for (int i = 0; i < this->size(); i ++)
  { if (!(*this)[i].differentialPart.IsConstant())
      return false;
    if (whichPoly != 0)
      whichPoly->AddMonomial((*this)[i].polynomialPart, this->theCoeffs[i]);
  }
  return true;
}

template <class coefficient>
bool ElementWeylAlgebra<coefficient>::HasNonSmallPositiveIntegerDerivation() const
{ for (int i = 0; i < this->size(); i ++)
    if ((*this)[i].HasNonSmallPositiveIntegerDerivation())
      return true;
  return false;
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::MultiplyTwoMonomials(const MonomialWeylAlgebra& left, const MonomialWeylAlgebra& right, ElementWeylAlgebra& output) const
{ SelectionWithDifferentMaxMultiplicities tempSel;
  int theDimensioN = MathRoutines::Maximum(left.GetMinNumVars(), right.GetMinNumVars());
  tempSel.Multiplicities.initFillInObject(theDimensioN, 0);
  tempSel.MaxMultiplicities.SetSize(theDimensioN);
  int theExpectedSize = 1;
  for (int i = 0; i < theDimensioN; i ++)
  { int powerDiffOp = 0;
    if (!left.differentialPart(i).IsSmallInteger(&powerDiffOp))
      crash << "This is a programming error. Requested operations with elements of Weyl algebra that have monomials of exponent "
      << left.differentialPart(i).ToString() << " which I cannot handle. If this is bad user input, it should have been caught at an earlier level. " << crash;
    tempSel.MaxMultiplicities[i] = powerDiffOp;
    theExpectedSize *= powerDiffOp;
  }
  tempSel.elements.initFillInObject(theDimensioN, 0);
  MonomialWeylAlgebra buffer;
  buffer.MakeOne(theDimensioN);
  output.MakeZero();
  int numCycles = tempSel.TotalNumSubsetsMustBeSmalInt();
  Rational coeffBuff;
  output.SetExpectedSize(theExpectedSize);
  for (int i = 0; i < numCycles; i ++)
  { coeffBuff = 1;
    for (int k = 0; k<theDimensioN; k ++)
    { int multDrop = tempSel.Multiplicities[k];
      Rational theDOPower = left.differentialPart(k);
      Rational thePolPower = right.polynomialPart(k);
      coeffBuff *= Rational::NChooseK(theDOPower, multDrop) * Rational::NChooseK(thePolPower, multDrop) * Rational::Factorial(multDrop);
      buffer.polynomialPart[k] = left.polynomialPart(k) + right.polynomialPart(k) - multDrop;
      buffer.differentialPart[k] = left.differentialPart(k) + right.differentialPart(k) - multDrop;
    }
    output.AddMonomial(buffer, coeffBuff);
    tempSel.IncrementReturnFalseIfPastLast();
  }
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::LieBracketOnTheLeftMakeReport(const ElementWeylAlgebra& standsOnTheLeft)
{ this->LieBracketOnTheLeft(standsOnTheLeft, theGlobalVariables);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::LieBracketOnTheRightMakeReport(const ElementWeylAlgebra& standsOnTheRight)
{ this->LieBracketOnTheRight(standsOnTheRight, theGlobalVariables);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::LieBracket(const ElementWeylAlgebra& left, const ElementWeylAlgebra& right, ElementWeylAlgebra& output)
{ if (&output == &right || &output == &left)
  { ElementWeylAlgebra leftCopy = left;
    ElementWeylAlgebra rightCopy = right;
    ElementWeylAlgebra::LieBracket(leftCopy, rightCopy, output);
    return;
  }
  output = right;
  output.MultiplyOnTheLeft(left);
  ElementWeylAlgebra subtracand;
  subtracand = left;
  subtracand.MultiplyOnTheLeft(right);
  output -= subtracand;
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::LieBracketOnTheLeft(const ElementWeylAlgebra& standsOnTheLeft)
{ ElementWeylAlgebra tempEl1, tempEl2;
  tempEl1 = *this;
  tempEl1.MultiplyOnTheLeft(standsOnTheLeft);
  //tempEl1.ComputeDebugString(false);
  tempEl2 = standsOnTheLeft;
  tempEl2.MultiplyOnTheLeft(*this);
  //tempEl2.ComputeDebugString(false);
  *this = tempEl1;
  *this -= tempEl2;
  //this->ComputeDebugString(false);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::LieBracketOnTheRight(const ElementWeylAlgebra& standsOnTheRight)
{ ElementWeylAlgebra tempEl1, tempEl2;
  tempEl1 = standsOnTheRight;
  tempEl1.MultiplyOnTheLeft(*this);
  //tempEl1.ComputeDebugString(false);
  tempEl2 = *this;
  tempEl2.MultiplyOnTheLeft(standsOnTheRight, theGlobalVariables);
  //tempEl2.ComputeDebugString(false);
  *this = tempEl1;
  *this -= tempEl2;
  //this->ComputeDebugString(false);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::MultiplyOnTheLeft(const  ElementWeylAlgebra& standsOnTheLeft)
{ ElementWeylAlgebra buffer;
  ElementWeylAlgebra Accum;
  Accum.MakeZero();
  coefficient currentCF;
  for (int j = 0; j < standsOnTheLeft.size(); j ++)
    for (int i = 0; i < this->size(); i ++)
    { this->MultiplyTwoMonomials(standsOnTheLeft[j], (*this)[i], buffer);
      currentCF = standsOnTheLeft.theCoeffs[j];
      currentCF *= this->theCoeffs[i];
      buffer *= currentCF;
      Accum += buffer;
    }
  *this = Accum;
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::operator*=(const ElementWeylAlgebra& standsOnTheRight)
{ ElementWeylAlgebra buffer;
  ElementWeylAlgebra Accum;
  Accum.MakeZero();
  coefficient currentCF;
  for (int j = 0; j < standsOnTheRight.size(); j ++)
    for (int i = 0; i < this->size(); i ++)
    { this->MultiplyTwoMonomials((*this)[i], standsOnTheRight[j], buffer);
      currentCF = this->theCoeffs[i];
      currentCF *= standsOnTheRight.theCoeffs[j];
      buffer *= currentCF;
      Accum += buffer;
    }
  *this = Accum;
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::RaiseToPower(int thePower)
{ ElementWeylAlgebra WeylOne;
  WeylOne.MakeOne(this->GetMinNumVars());
  MathRoutines::RaiseToPower(*this, thePower, WeylOne);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::MakeGEpsPlusEpsInTypeD(int i, int j, int NumVars)
{ this->MakeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.MakeOne(NumVars);
  tempMon.polynomialPart[i] = 1;
  tempMon.differentialPart[j + NumVars] = 1;
  this->AddMonomial(tempMon, 1);
  tempMon.MakeOne(NumVars);
  tempMon.polynomialPart[j] = 1;
  tempMon.differentialPart[i + NumVars] = 1;
  this->AddMonomial(tempMon, 1);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::MakeGEpsMinusEpsInTypeD(int i, int j, int NumVars)
{ this->MakeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.MakeOne(NumVars);
  tempMon.polynomialPart[i] = 1;
  tempMon.differentialPart[j] = 1;
  this->AddMonomial(tempMon, 1);
  tempMon.MakeOne(NumVars);
  tempMon.polynomialPart[j + NumVars] = 1;
  tempMon.differentialPart[i + NumVars] = 1;
  this->AddMonomial(tempMon, 1);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::MakeGMinusEpsMinusEpsInTypeD(int i, int j, int NumVars)
{ this->MakeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.MakeOne(NumVars);
  tempMon.polynomialPart[i + NumVars] = 1;
  tempMon.differentialPart[j] = 1;
  this->AddMonomial(tempMon, 1);
  tempMon.MakeOne(NumVars);
  tempMon.polynomialPart[j + NumVars] = 1;
  tempMon.differentialPart[i] = 1;
  this->AddMonomial(tempMon, 1);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::Makedidj(int i, int j, int NumVars)
{ this->MakeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.MakeOne(NumVars);
  tempMon.differentialPart[i] = 1;
  tempMon.differentialPart[j] = 1;
  this->AddMonomial(tempMon, 1);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::Makexixj(int i, int j, int NumVars)
{ this->MakeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.MakeOne(NumVars);
  tempMon.polynomialPart[i] = 1;
  tempMon.polynomialPart[j] = 1;
  this->AddMonomial(tempMon, 1);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::Makexi(int i, int NumVars)
{ this->MakeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.MakeOne(NumVars);
  tempMon.polynomialPart[i] = 1;
  this->AddMonomial(tempMon, 1);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::Makedi(int i, int NumVars)
{ this->MakeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.MakeOne(NumVars);
  tempMon.differentialPart[i] = 1;
  this->AddMonomial(tempMon, 1);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::Makexidj(int i, int j, int NumVars)
{ this->MakeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.MakeOne(NumVars);
  tempMon.polynomialPart[i] = 1;
  tempMon.differentialPart[j] = 1;
  this->AddMonomial(tempMon, 1);
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::GetStandardOrderDiffOperatorCorrespondingToNraisedTo
(const Rational& inputRationalPower, int indexVar, ElementWeylAlgebra& outputDO, Polynomial<Rational>& outputDenominator)
{ outputDenominator.MakeOne();
  MonomialWeylAlgebra tempMon;
  outputDO.MakeZero();
  int inputPower = 0;
  if (!inputRationalPower.IsSmallInteger(&inputPower))
    crash << "This is a programming error: " << " I can give you a differential operator only from integer exponent. " << crash;
  if (inputPower >= 0)
    tempMon.polynomialPart.MakeEi(indexVar, inputPower);
  else
    tempMon.differentialPart.MakeEi(indexVar, - inputPower);
  outputDO.AddMonomial(tempMon, 1);
  inputPower *= - 1;
  Polynomial<Rational> newMult;
  newMult.MakeDegreeOne(0, indexVar, 1);
  for (int i = 0; i < inputPower; i ++)
  { outputDenominator *= newMult;
    newMult -= 1;
  }
//  output/=den;
}

template <class coefficient>
bool ElementWeylAlgebra<coefficient>::Substitution(const PolynomialSubstitution<Rational>& SubPolyPart, const PolynomialSubstitution<Rational>& SubDiffPArt)
{ MacroRegisterFunctionWithName("ElementWeylAlgebra::Substitution");
  Polynomial<Rational> DOpart, polyPart;
  MonomialWeylAlgebra theMon;
  ElementWeylAlgebra output;
  output.MakeZero();
  coefficient theNewCoeff;
//  stOutput << "<hr>Substituting " << SubPolyPart.ToString() << " into " << this->ToString();
  for (int i = 0; i < this->size(); i ++)
  { const MonomialWeylAlgebra& currentMon = (*this)[i];
    //stOutput << "<hr>Substituting " << SubPolyPart.ToString() << " into " << currentMon.polynomialPart.ToString();
    if (!currentMon.polynomialPart.SubstitutioN(SubPolyPart, polyPart))
      return false;
    //stOutput << " to get " << polyPart.ToString();
    //stOutput << "<hr>Substituting " << SubPolyPart.ToString() << " into " << currentMon.differentialPart.ToString();
    if (!currentMon.differentialPart.SubstitutioN(SubDiffPArt, DOpart))
      return false;
    //stOutput << " to get " << DOpart.ToString();
    for (int j = 0; j < polyPart.size(); j ++)
      for (int k = 0; k < DOpart.size(); k ++)
      { theMon.polynomialPart = polyPart[j];
        theMon.differentialPart = DOpart[k];
        theNewCoeff = this->theCoeffs[i];
        theNewCoeff *= polyPart.theCoeffs[j];
        theNewCoeff *= DOpart.theCoeffs[k];
        output.AddMonomial(theMon, theNewCoeff);
      }
  }
  *this = output;
  return true;
}

template <class coefficient>
void ElementWeylAlgebra<coefficient>::FourierTransform(ElementWeylAlgebra<coefficient>& output) const
{ MacroRegisterFunctionWithName("ElementWeylAlgebra::FourierTransformMe");
  if (&output == this)
  { ElementWeylAlgebra thisCopy = *this;
    thisCopy.FourierTransform(output);
    return;
  }
  LargeInt totalDeg;
  coefficient theCoeff;
  output.MakeZero();
  MonomialWeylAlgebra theMon;
  for (int i = 0; i < this->size(); i ++)
  { const MonomialWeylAlgebra& currentMon = (*this)[i];
    if (!(currentMon.polynomialPart.TotalDegree() + currentMon.differentialPart.TotalDegree()).IsInteger(&totalDeg))
      crash << "This is a programming error: calling Fourier transoform on differential operator with non-integral exponents. " << crash;
//    stOutput << "<br>totalDeg: " << totalDeg.ToString() << ", is even =" << totalDeg.IsEven();
    theMon.differentialPart = currentMon.polynomialPart;
    theMon.polynomialPart = currentMon.differentialPart;
    theCoeff = this->theCoeffs[i];
    if (totalDeg.IsEven())
      theCoeff *= - 1;
//    totalDeg%=4;
//    if (totalDeg==2 || totalDeg==3)
//      theCoeff*= - 1;
    //if (totalDeg==1 || totalDeg==3)
      //theCoeff*=ImaginaryUnit;
    output.AddMonomial(theMon, theCoeff);
  }
}

template <class coefficient>
bool ElementWeylAlgebra<coefficient>::ActOnPolynomial(Polynomial<Rational>& thePoly) const
{ Polynomial<Rational> result;
  result.MakeZero();
  MonomialP resultMon;
  Rational coeff;
  for (int i = 0; i < this->size(); i ++)
    for (int j = 0; j < thePoly.size(); j ++)
    { const MonomialP& currentPolMon = thePoly[j];
      const MonomialWeylAlgebra& currentOpMon = (*this)[i];
      resultMon = currentPolMon;
      coeff = thePoly.theCoeffs[j];
      coeff *= this->theCoeffs[i];
      for (int k = 0; k < currentOpMon.GetMinNumVars(); k ++)
      { int numDiff = 0;
        if (!currentOpMon.differentialPart(k).IsSmallInteger(&numDiff))
          return false;
        for (; numDiff > 0; numDiff --)
        { coeff *= resultMon[k];
          if (coeff.IsEqualToZero())
            break;
          resultMon[k] -= 1;
        }
        if (coeff.IsEqualToZero())
          break;
      }
      resultMon *= currentOpMon.polynomialPart;
      result.AddMonomial(resultMon, coeff);
    }
  thePoly = result;
  return true;
}

#endif
