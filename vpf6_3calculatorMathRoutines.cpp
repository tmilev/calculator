//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
#include "vpfHeader3Calculator2_InnerFunctions.h"
#include "vpfImplementationHeader2Math3_WeylAlgebra.h"
#include "vpfImplementationHeader2Math051_PolynomialComputations_Basic.h"
#include "vpfImplementationHeader2Math15_UniversalEnveloping.h"
#include "vpfImplementationHeader2Math3_FiniteGroups.h"
#include "vpfImplementationHeader2Math6_ModulesSSLieAlgebras.h"

ProjectInformationInstance ProjectInfoVpf6_3cpp(__FILE__, "Calculator built-in functions. ");

template <class theType>
bool MathRoutines::GenerateVectorSpaceClosedWRTOperation
(List<theType>& inputOutputElts, int upperDimensionBound, void (*theBinaryOperation)(const theType& left, const theType& right, theType& output),
 GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("MathRoutines::GenerateVectorSpaceClosedWRTOperation");
  inputOutputElts[0].GaussianEliminationByRowsDeleteZeroRows(inputOutputElts);
  theType theOpResult;
  ProgressReport theReport1(theGlobalVariables), theReport2(theGlobalVariables);
  if (theGlobalVariables!=0)
    theReport1.Report("Extending vector space to closed with respect to binary operation. ");
  List<theType> theEltsForGaussianElimination=inputOutputElts;
  for (int i=0; i<inputOutputElts.size; i++)
    for (int j=i; j<inputOutputElts.size; j++)
    { theBinaryOperation(inputOutputElts[i], inputOutputElts[j], theOpResult);
      //int oldNumElts=inputOutputElts.size;
      theEltsForGaussianElimination.AddOnTop(theOpResult);
      theEltsForGaussianElimination[0].GaussianEliminationByRowsDeleteZeroRows(theEltsForGaussianElimination);
      if (theEltsForGaussianElimination.size>inputOutputElts.size)
        inputOutputElts.AddOnTop(theOpResult);
      //if (oldNumElts<inputOutputElts.size)
        //std::cout << "<hr>Operation between <br>" << inputOutputElts[i].ToString() << " <br> " << inputOutputElts[j].ToString() << " <br>=<br> "
        //<< theOpResult.ToString();
      if (upperDimensionBound>0 && inputOutputElts.size>upperDimensionBound)
        return false;
      if (theGlobalVariables!=0)
      { std::stringstream reportStream;
        reportStream << "Accounted operation between elements " << i+1 << " and " << j+1 << " out of " << inputOutputElts.size;
        theReport2.Report(reportStream.str());
      }
    }
  inputOutputElts[0].GaussianEliminationByRowsDeleteZeroRows(inputOutputElts);
  return true;
}

bool CalculatorFunctionsGeneral::innerGenerateVectorSpaceClosedWRTLieBracket(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGenerateVectorSpaceClosedWRTLieBracket");
  Vector<ElementWeylAlgebra<Rational> > theOps;
  Expression theContext;
  if (!input.children.size>1)
    return false;
  int upperBound=-1;
  if (!input[1].IsSmallInteger(&upperBound))
  { theCommands.Comments << "<hr>Failed to extract upper bound for the vector space dimension from the first argument. ";
    return false;
  }
  Expression inputModded=input;
  inputModded.children.RemoveIndexShiftDown(1);

  if (!theCommands.GetVectorFromFunctionArguments(inputModded, theOps, &theContext))
  { Vector<ElementUniversalEnveloping<RationalFunctionOld> > theLieAlgElts;
    theContext.MakeEmptyContext(theCommands);
    if (!theCommands.GetVectorFromFunctionArguments(inputModded, theLieAlgElts, &theContext))
    { theCommands.Comments << "<hr>Failed to extract elements of weyl algebra and failed to extract elements of UE algebra from input "
      << input.ToString();
      return false;
    }
    FormatExpressions theFormat;
    theContext.ContextGetFormatExpressions(theFormat);
    std::stringstream out;
    out << "Starting elements: <br>";
    for (int i=0; i<theLieAlgElts.size; i++)
      out << CGI::GetMathSpanPure(theLieAlgElts[i].ToString(&theFormat)) << "<br>";
    bool success=MathRoutines::GenerateVectorSpaceClosedWRTLieBracket(theLieAlgElts, upperBound, theCommands.theGlobalVariableS);
    if (!success)
      out << "<br>Did not succeed with generating vector space, instead got a vector space with basis " << theLieAlgElts.size << " exceeding the limit. "
      << "The basis generated before exceeding the limit was: " << theLieAlgElts.ToString();
    else
    { out << "<br>Lie bracket generates vector space of dimension " << theLieAlgElts.size << " with basis:";
      for (int i=0; i<theLieAlgElts.size; i++)
      { out << "<br>";
        if (theLieAlgElts.size>50)
          out << theLieAlgElts[i].ToString(&theFormat);
        else
          out << CGI::GetMathSpanPure(theLieAlgElts[i].ToString(&theFormat));
      }
    }
    return output.AssignValue(out.str(), theCommands);
  }
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
  std::stringstream out;
  out << "Starting elements: <br>";
  for (int i=0; i<theOps.size; i++)
    out << CGI::GetMathSpanPure(theOps[i].ToString(&theFormat)) << "<br>";
  bool success=MathRoutines::GenerateVectorSpaceClosedWRTLieBracket(theOps, upperBound, theCommands.theGlobalVariableS);
  if (!success)
    out << "<br>Did not succeed with generating vector space, instead got a vector space with basis " << theOps.size << " exceeding the limit. "
    << "The basis generated before exceeding the limit was: " << theOps.ToString();
  else
  { out << "<br>Lie bracket generates vector space of dimension " << theOps.size << " with basis:";
    for (int i=0; i<theOps.size; i++)
    { out << "<br>";
      if (theOps.size>50)
        out << theOps[i].ToString(&theFormat);
      else
        out << CGI::GetMathSpanPure(theOps[i].ToString(&theFormat));
    }
  }

  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerFourierTransformEWA(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerFourierTransformEWA");
  if (!input.IsOfType<ElementWeylAlgebra<Rational> >())
    return false;
  ElementWeylAlgebra<Rational> theElt;
  input.GetValue<ElementWeylAlgebra<Rational> >().FourierTransform(theElt);
  return output.AssignValueWithContext(theElt, input.GetContext(), theCommands);
}

bool CalculatorFunctionsGeneral::innerCasimirWRTlevi(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCasimir");
  RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(3))
    return false;
  SemisimpleLieAlgebra* theSSalg=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorSerialization::innerSSLieAlgebra, input[1], theSSalg))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  if (theCommands.theGlobalVariableS->MaxComputationTimeSecondsNonPositiveMeansNoLimit<50)
    theCommands.theGlobalVariableS->MaxComputationTimeSecondsNonPositiveMeansNoLimit=50;
  Vector<Rational> leviSelection;
  if(!theCommands.GetVectoR(input[2], leviSelection, 0, theSSalg->GetRank()))
  { theCommands.Comments << "<hr>Failed to extract parabolic selection. ";
    return false;
  }
  Selection theParSel;
  theParSel=leviSelection;
  theParSel.InvertSelection();
  ElementUniversalEnveloping<RationalFunctionOld> theCasimir;
  theCasimir.MakeCasimirWRTLeviParabolic(*theSSalg, theParSel);
//  theCasimir.Simplify(*theCommands.theGlobalVariableS);
  Expression contextE;
  contextE.MakeContextSSLieAlg(theCommands, *theSSalg);
  return output.AssignValueWithContext(theCasimir, contextE, theCommands);
}

bool CalculatorFunctionsGeneral::innerLog(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerLog");
  double theArgument;
  if (input.IsEqualToZero())
    return output.SetError("Logarithm of zero is undefined.", theCommands);
  if (!input.EvaluatesToRealDouble(&theArgument))
  { if (input.IsAtomGivenData(theCommands.opE()))
      return output.AssignValue((Rational) 1, theCommands);
    return false;
  }
  if (theArgument>0)
  { if (input.IsAtomGivenData(theCommands.opE()))
      return output.AssignValue((Rational) 1, theCommands);
    return output.AssignValue(FloatingPoint::log(theArgument), theCommands);
  }
  theArgument*=-1;
  Expression iE, ipiE, piE, lnPart;
  iE.MakeSqrt(theCommands, -1);
  piE.MakeAtom(theCommands.opPi(), theCommands);
  ipiE.MakeXOX(theCommands, theCommands.opTimes(), piE, iE);
  lnPart.AssignValue(FloatingPoint::log(theArgument), theCommands);
  return output.MakeXOX(theCommands, theCommands.opPlus(), lnPart, ipiE);
}

bool CalculatorFunctionsGeneral::innerSin(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSin");
  if (input.IsAtomGivenData(theCommands.opPi()))
    return output.AssignValue(0, theCommands);
  double theArgument;
  if (!input.EvaluatesToRealDouble(&theArgument))
    return false;
  return output.AssignValue(FloatingPoint::sin(theArgument), theCommands);
}

bool CalculatorFunctionsGeneral::innerFactorial(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSin");
  int inputInt;
  if (!input.IsSmallInteger(&inputInt))
    return false;
  Rational result;
  return output.AssignValue(result.Factorial(inputInt), theCommands);
}

bool CalculatorFunctionsGeneral::innerArctan(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerArctan");
  double theArgument;
  if (!input.EvaluatesToRealDouble(&theArgument))
    return false;
  return output.AssignValue(FloatingPoint::arctan(theArgument), theCommands );
}

bool CalculatorFunctionsGeneral::innerArccos(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerArccos");
  double theArgument;
  if (!input.EvaluatesToRealDouble(&theArgument))
    return false;
  return output.AssignValue(FloatingPoint::arccos(theArgument), theCommands );
}

bool CalculatorFunctionsGeneral::innerArcsin(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerArcsin");
  double theArgument;
  if (!input.EvaluatesToRealDouble(&theArgument))
    return false;
  return output.AssignValue(FloatingPoint::arcsin(theArgument), theCommands );
}

bool CalculatorFunctionsGeneral::innerCos(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCos");
  if (input.IsAtomGivenData(theCommands.opPi()))
    return output.AssignValue(-1, theCommands);
  double theArgument;
  if (!input.EvaluatesToRealDouble(&theArgument))
    return false;
  return output.AssignValue(FloatingPoint::cos(theArgument), theCommands );
}

bool CalculatorFunctionsGeneral::innerTan(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerTan");
  Expression num, den;
  num.MakeOX(theCommands, theCommands.opSin(), input);
  den.MakeOX(theCommands, theCommands.opCos(), input);
  return output.MakeXOX(theCommands, theCommands.opDivide(), num, den);
}

bool CalculatorFunctionsGeneral::innerCot(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerTan");
  Expression num, den;
  num.MakeOX(theCommands, theCommands.opCos(), input);
  den.MakeOX(theCommands, theCommands.opSin(), input);
  return output.MakeXOX(theCommands, theCommands.opDivide(), num, den);
}

bool CalculatorFunctionsGeneral::innerSec(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSec");
  Expression num, den;
  num.AssignValue(1, theCommands);
  den.MakeOX(theCommands, theCommands.opCos(), input);
  return output.MakeXOX(theCommands, theCommands.opDivide(), num, den);
}

bool CalculatorFunctionsGeneral::innerCsc(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCsc");
  Expression num, den;
  num.AssignValue(1, theCommands);
  den.MakeOX(theCommands, theCommands.opSin(), input);
  return output.MakeXOX(theCommands, theCommands.opDivide(), num, den);
}

bool CalculatorFunctionsGeneral::innerCompositeSequenceDereference(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCompositeSequenceDereference");
  if (input.children.size!=2)
    return false;
  if (!input[0].IsListStartingWithAtom(theCommands.opSequence()))
    return false;
  int theIndex;
  if (!input[1].IsSmallInteger(&theIndex))
    return false;
  if (theIndex>0 && theIndex<input[0].children.size)
  { output=input[0][theIndex];
    return true;
  }
  return false;
}

bool CalculatorFunctionsGeneral::innerGetAlgebraicNumberFromMinPoly(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGetAlgebraicNumberFromMinPoly");
  Expression polyE;
  if (!CalculatorSerialization::innerPolynomial<AlgebraicNumber>(theCommands, input, polyE) )
  { theCommands.Comments << "<hr>Failed to convert " << input.ToString() << " to polynomial. ";
    return false;
  }
  Polynomial<AlgebraicNumber> thePoly;
  if (!polyE.IsOfType<Polynomial<AlgebraicNumber> >(&thePoly))
  { theCommands.Comments << "<hr>Failed to convert " << input.ToString() << " to polynomial, instead got " << polyE.ToString();
    return false;
  }
  if (polyE.GetNumContextVariables()!=1)
  { theCommands.Comments << "<hr>After conversion, I got the polynomial " << polyE.ToString() << ", which is not in one variable.";
    return false;
  }
  AlgebraicNumber theAN;
  if (!theAN.ConstructFromMinPoly(thePoly, theCommands.theObjectContainer.theAlgebraicClosure, theCommands.theGlobalVariableS))
    return false;
  return output.AssignValue(theAN, theCommands);
}

bool CalculatorFunctionsGeneral::innerCompositeApowerBevaluatedAtC(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCompositeApowerBevaluatedAtC");
  //std::cout << "<hr>input be: " << input.ToString();
  if (!input.IsListNElements())
    return false;
  const Expression& firstE=input[0];
  if (!firstE.IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  Expression finalBase;
  finalBase.reset(theCommands, input.children.size);
  finalBase.AddChildOnTop(input[0][1]);
  for (int i=1; i<input.children.size; i++)
    finalBase.AddChildOnTop(input[i]);
  return output.MakeXOX(theCommands, theCommands.opThePower(), finalBase, input[0][2]);
}

bool CalculatorFunctionsGeneral::innerConstantFunction(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerConstantFunction");
  if (!input.IsListNElements())
    return false;
  if (!input[0].IsConstantNumber())
    return false;
  output=input[0];
  return true;
}

bool CalculatorFunctionsGeneral::innerExpressionFromBuiltInType(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerExpressionFromBuiltInType");
  if (input.IsOfType<Polynomial<Rational> >())
    return CalculatorFunctionsGeneral::innerExpressionFromPoly(theCommands, input, output);
  if (input.IsOfType<RationalFunctionOld>())
    return CalculatorFunctionsGeneral::innerExpressionFromRF(theCommands, input, output);
  return false;
}

bool CalculatorFunctionsGeneral::innerExpressionFromRF(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerExpressionFromRF");
  if (!input.IsOfType<RationalFunctionOld>() )
    return false;
  const RationalFunctionOld& theRF=input.GetValue<RationalFunctionOld>();
  Rational aConst;
  if (theRF.IsConstant(&aConst))
    return output.AssignValue(aConst, theCommands);
  Expression numPolyE, denPolyE, numE, denE;
  Polynomial<Rational> numP, denP;
  theRF.GetNumerator(numP);
  numPolyE.AssignValueWithContext(numP, input.GetContext(), theCommands);
  if (theRF.expressionType==theRF.typePoly)
    return CalculatorFunctionsGeneral::innerExpressionFromPoly(theCommands, numPolyE, output);
  theRF.GetDenominator(denP);
  denPolyE.AssignValueWithContext(denP, input.GetContext(), theCommands);
  CalculatorFunctionsGeneral::innerExpressionFromPoly(theCommands, numPolyE, numE);
  CalculatorFunctionsGeneral::innerExpressionFromPoly(theCommands, denPolyE, denE);
  return output.MakeXOX(theCommands, theCommands.opDivide(), numE, denE);
}

bool CalculatorFunctionsGeneral::innerExpressionFromPoly(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerExpressionFromPoly");
  if (!input.IsOfType<Polynomial<Rational> >() )
    return false;
  const Polynomial<Rational>& thePoly=input.GetValue<Polynomial<Rational> >();
  MonomialCollection<Expression, Rational> theTerms;
  Expression currentBase, currentPower, currentTerm, currentLetterE;
  Expression theContext=input.GetContext();
  for (int i=0; i<thePoly.size(); i++)
  { if (thePoly[i].IsAConstant())
    { currentTerm.AssignValue(1, theCommands);
      theTerms.AddMonomial(currentTerm, thePoly.theCoeffs[i]);
      continue;
    }
    bool found=false;
    for(int j=0; j<thePoly[i].GetMinNumVars(); j++)
      if (thePoly[i](j)!=0)
      { if (thePoly[i](j)==1)
          currentLetterE=theContext.ContextGetContextVariable(j);
        else
        { currentBase=theContext.ContextGetContextVariable(j);
          currentPower.AssignValue(thePoly[i](j), theCommands);
          currentLetterE.MakeXOX(theCommands, theCommands.opThePower(), currentBase, currentPower);
        }
        if (!found)
          currentTerm=currentLetterE;
        else
          currentTerm*=currentLetterE;
        found=true;
      }
    theTerms.AddMonomial(currentTerm, thePoly.theCoeffs[i]);
  }
//  std::cout << "Extracted expressions: " << theTerms.ToString();
  return output.MakeSum(theCommands, theTerms);
}

bool CalculatorFunctionsGeneral::outerCombineFractionsCommutative(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::outerCombineFractionsCommutative");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opPlus(), 3))
    return false;
  const Expression& leftE=input[1];
  const Expression& rightE=input[2];
  if (!leftE.IsListNElementsStartingWithAtom(theCommands.opDivide(), 3) ||
      !rightE.IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  output=(leftE[1]*rightE[2]+rightE[1]*leftE[2])/(leftE[2]*rightE[2]);
  return true;
}

template<class coefficient>
void Polynomial<coefficient>::GetPolyWithPolyCoeff
(Selection& theNonCoefficientVariables, Polynomial<Polynomial<coefficient> >& output)const
{ MacroRegisterFunctionWithName("Polynomial::GetPolyWithPolyCoeff");
  if (theNonCoefficientVariables.MaxSize!=this->GetMinNumVars())
    crash << "GetPolyWithPolyCoeff called with selection which has selects the wrong number of variables. " << crash;
  output.MakeZero();
  MonomialP coeffPart, polyPart;
  Polynomial<coefficient> currentCF;
  for (int i=0; i<this->size(); i++)
  { coeffPart.MakeOne();
    polyPart.MakeOne();
    for (int j=0; j<(*this)[i].GetMinNumVars(); j++)
      if (theNonCoefficientVariables.selected[j])
        polyPart[j]=(*this)[i](j);
      else
        coeffPart[j]=(*this)[i](j);
    currentCF.MakeZero();
    currentCF.AddMonomial(coeffPart, this->theCoeffs[i]);
    output.AddMonomial(polyPart, currentCF);
  }
}

template<class coefficient>
void Polynomial<coefficient>::GetPolyUnivariateWithPolyCoeffs
(int theVar, Polynomial<Polynomial<coefficient> >& output)const
{ Selection theVars;
  theVars.init(this->GetMinNumVars());
  theVars.AddSelectionAppendNewIndex(theVar);
  this->GetPolyWithPolyCoeff(theVars, output);
}

template <class coefficient>
bool Polynomial<coefficient>::GetLinearSystemFromLinearPolys
(const List<Polynomial<coefficient> >& theLinPolys, Matrix<coefficient>& homogenousPart,
 Matrix<coefficient>& constTerms)
{ MacroRegisterFunctionWithName("Polynomial::GetLinearSystemFromLinearPolys");
  int theLetter;
  int numVars=0;
  for (int i=0; i<theLinPolys.size; i++)
    numVars=MathRoutines::Maximum(theLinPolys[i].GetMinNumVars(), numVars);
  homogenousPart.init(theLinPolys.size, numVars);
  homogenousPart.MakeZero();
  constTerms.init(theLinPolys.size,1);
  constTerms.MakeZero();
  for (int i=0; i<theLinPolys.size; i++)
    for (int j=0; j<theLinPolys[i].size(); j++)
      if (theLinPolys[i][j].IsLinearNoConstantTerm(&theLetter))
        homogenousPart(i,theLetter)=theLinPolys[i].theCoeffs[j];
      else if (theLinPolys[i][j].IsAConstant())
      { constTerms(i,0)=theLinPolys[i].theCoeffs[j];
        constTerms(i,0)*=-1;
      } else
        return false;
  return true;
}

template <class coefficient>
coefficient Polynomial<coefficient>::GetDiscriminant()
{ MacroRegisterFunctionWithName("Polynomial::GetDiscriminant");
  if (this->GetMinNumVars()>1)
    crash << "I do not have a definition of discriminant for more than one variable. " << crash;
  if (this->TotalDegree()!=2)
    crash << "Discriminant not implemented for polynomial of degree not equal to 1." << crash;
  coefficient a=this->GetMonomialCoefficient(MonomialP(0,2));
  coefficient b=this->GetMonomialCoefficient(MonomialP(0,1));
  coefficient c=this->GetMonomialCoefficient(MonomialP(0,0));
  return b*b-a*c*4;
}

bool CalculatorFunctionsGeneral::innerSplitToPartialFractionsOverAlgebraicReals(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSplitToPartialFractionsOverAlgebraicReals");
  Expression convertedInput;
  bool isGood=CalculatorSerialization::innerRationalFunction(theCommands, input, convertedInput);
  if (isGood)
    isGood=convertedInput.IsOfType<RationalFunctionOld>();
  if (!isGood)
  { theCommands.Comments << "Failed to convert " << input.ToString() << " to rational function. ";
    return false;
  }
  const RationalFunctionOld& inputRF=convertedInput.GetValue<RationalFunctionOld>();
  if (inputRF.GetMinNumVars()>1)
  { theCommands.Comments << "The input rational function is of " << inputRF.GetMinNumVars() << " variables and "
    << " I can handle only 1.";
    return false;
  }
  std::stringstream out;
  FormatExpressions theFormat;
  convertedInput.GetContext().ContextGetFormatExpressions(theFormat);
  if (inputRF.GetMinNumVars()<1 || inputRF.expressionType== inputRF.typeRational ||
      inputRF.expressionType==inputRF.typePoly)
  { out << inputRF.ToString(&theFormat) << " is already split into partial fractions. ";
    return output.AssignValue(out.str(), theCommands);
  }
  Polynomial<Rational> theDen, theNum;
  inputRF.GetDenominator(theDen);
  inputRF.GetNumerator(theNum);
  List<Polynomial<Rational> > theFactors;
  if (!theDen.FactorMe(theFactors, &theCommands.Comments))
  { theCommands.Comments << "<hr>I failed to factor the denominator of the rational function, I surrender.";
    return false;
  }
  theFormat.flagUseFrac=true;
  out << "The rational function is: " << CGI::GetMathSpanPure(inputRF.ToString(&theFormat)) << ".";
  out << "<br>The denominator factors are: ";
  bool allFactorsAreOfDegree2orless=true;
  for (int i=0; i<theFactors.size; i++)
  { out << CGI::GetMathSpanPure(theFactors[i].ToString(&theFormat));
    if (i!=theFactors.size-1)
      out << ", ";
    if (theFactors[i].TotalDegree()>2)
      allFactorsAreOfDegree2orless=false;
  }
  out << ". <br>";
  if (!allFactorsAreOfDegree2orless)
  { out << "There were factors (over the rationals) of degree greater than 2. I surrender. ";
    return output.AssignValue(out.str(), theCommands);
  }
  Polynomial<Rational> quotientRat, remainderRat;
  Polynomial<AlgebraicNumber> quotientAlg, remainderAlg;

  theNum.DivideBy(theDen, quotientRat, remainderRat);
  quotientAlg=quotientRat;
  remainderAlg=remainderRat;
  if (!quotientRat.IsEqualToZero())
  { out << "<br>The numerator " << CGI::GetMathSpanPure(theNum.ToString(&theFormat))
    << " divided by the denominator "
    << CGI::GetMathSpanPure(theDen.ToString(&theFormat)) << " yields "
    << CGI::GetMathSpanPure(quotientRat.ToString(&theFormat))
    << " with remainder "
    << CGI::GetMathSpanPure(remainderRat.ToString(&theFormat)) << ". ";
    Expression theDivStringArguments(theCommands), thePolyDivStringE, numE, denE;
    theDivStringArguments.AddChildAtomOnTop("PolyDivStringGrLex");
    numE.AssignValueWithContext(theNum, convertedInput.GetContext(), theCommands);
    denE.AssignValueWithContext(theDen, convertedInput.GetContext(), theCommands);
    theDivStringArguments.AddChildOnTop(numE);
    theDivStringArguments.AddChildOnTop(denE);
    theCommands.innerPolynomialDivisionVerboseGrLex(theCommands, theDivStringArguments, thePolyDivStringE);
    if(thePolyDivStringE.IsOfType<std::string>())
    { out << "<br>Here is a detailed long polynomial division:<br> ";
      out << thePolyDivStringE.GetValue<std::string>();
    }
  }
  MonomialCollection<Polynomial<Rational>, Rational> theDenominatorFactorsWithMultsCopy;
  MonomialCollection<Polynomial<AlgebraicNumber>, Rational> theDenominatorFactorsWithMults;
  List<List<Polynomial<AlgebraicNumber> > > theNumerators;
  theDenominatorFactorsWithMultsCopy.MakeZero();
  for (int i=0; i<theFactors.size; i++)
    theDenominatorFactorsWithMultsCopy.AddMonomial(theFactors[i], 1);
  theDenominatorFactorsWithMultsCopy.QuickSortAscending();
  Polynomial<Rational> currentSecondDegreePoly;
  theDenominatorFactorsWithMults.MakeZero();
  Polynomial<AlgebraicNumber> currentLinPoly, currentSecondDegreePolyAlgebraic;
  for (int i=0; i<theDenominatorFactorsWithMultsCopy.size(); i++)
  { currentSecondDegreePoly=theDenominatorFactorsWithMultsCopy[i];
    currentSecondDegreePolyAlgebraic=currentSecondDegreePoly;
    if (currentSecondDegreePoly.TotalDegree()!=2)
    { theDenominatorFactorsWithMults.AddMonomial(currentSecondDegreePolyAlgebraic, theDenominatorFactorsWithMultsCopy.theCoeffs[i]);
      continue;
    }
    Rational theDiscriminant=currentSecondDegreePoly.GetDiscriminant();
//    std::cout << "The discriminant: " << theDiscriminant.ToString();
    if (theDiscriminant<0)
    { theDenominatorFactorsWithMults.AddMonomial(currentSecondDegreePolyAlgebraic, theDenominatorFactorsWithMultsCopy.theCoeffs[i]);
      continue;
    }
    AlgebraicNumber theDiscriminantSqrt;
    if (!theDiscriminantSqrt.AssignRationalQuadraticRadical(theDiscriminant, theCommands.theObjectContainer.theAlgebraicClosure))
    { theCommands.Comments << "Failed to take radical of " << theDiscriminant.ToString()
      << " (radical too large?).";
      return false;
    }
//    std::cout << "<br>sqrt of discriminant: " << theDiscriminantSqrt.ToString();
    theDiscriminantSqrt.CheckConsistency();
    AlgebraicNumber a=currentSecondDegreePoly.GetMonomialCoefficient(MonomialP(0,2));
    AlgebraicNumber b=currentSecondDegreePoly.GetMonomialCoefficient(MonomialP(0,1));
    a.CheckConsistency();
    b.CheckConsistency();
    currentLinPoly.MakeMonomiaL(0,1);
    currentLinPoly-= (-b+theDiscriminantSqrt)/(a*2);
    theDenominatorFactorsWithMults.AddMonomial(currentLinPoly, theDenominatorFactorsWithMultsCopy.theCoeffs[i]);
    currentLinPoly.MakeMonomiaL(0,1);
    currentLinPoly-= (-b-theDiscriminantSqrt)/(a*2);
    theDenominatorFactorsWithMults.AddMonomial(currentLinPoly, theDenominatorFactorsWithMultsCopy.theCoeffs[i]);
//    Rational c=currentSecondDegreePoly.GetMonomialCoefficient(MonomialP(0,0));

  }
  theDenominatorFactorsWithMults.QuickSortAscending();
  out << "<br><br>I need to find " << CGI::GetMathSpanPure("A_i")
  << "'s so that I have the equality of rational functions: ";
  RationalFunctionOld transformedRF;
  transformedRF=remainderRat;
  transformedRF/=theDen;
  std::stringstream rfStream, polyStream;
  rfStream << transformedRF.ToString(&theFormat) << " = ";
  polyStream << remainderRat.ToString(&theFormat) << "=";
  int varCounter=0;
  Polynomial<AlgebraicNumber> thePolyThatMustVanish, currentSummand;
  MonomialP currentMon;
  thePolyThatMustVanish.MakeZero();
  thePolyThatMustVanish-=remainderAlg;
  theNumerators.SetSize(theDenominatorFactorsWithMults.size());
  for (int i =0; i<theDenominatorFactorsWithMults.size(); i++)
  { int tempSize=-1;
    theDenominatorFactorsWithMults.theCoeffs[i].IsSmallInteger(&tempSize);
    theNumerators[i].SetSize(tempSize);
    for (int k=0; k<theDenominatorFactorsWithMults.theCoeffs[i]; k++)
    { rfStream << "\\frac{";
      if (theDenominatorFactorsWithMults[i].TotalDegree()>1)
        polyStream << "(";
      currentSummand.MakeZero();
      theNumerators[i][k].MakeZero();
      for (int j=0; j<theDenominatorFactorsWithMults[i].TotalDegree(); j++)
      { varCounter++;
        std::stringstream varNameStream;
        varNameStream << "A_{" << varCounter << "} ";
        currentMon.MakeEi(varCounter);
        currentMon[0]=j;
        theNumerators[i][k].AddMonomial(currentMon, 1);
        currentSummand.AddMonomial(currentMon, 1);
        rfStream << varNameStream.str();
        polyStream << varNameStream.str();
        theFormat.polyAlphabeT.AddOnTop(varNameStream.str());
        if (j>0)
        { rfStream << "x";
          polyStream << "x";
        }
        if (j>1)
        { rfStream << "^{" << j << "}";
          polyStream << "^{" << j << "}";
        }
        if ((theDenominatorFactorsWithMults[i].TotalDegree()-1)!=j)
        { rfStream << " + ";
          polyStream << " + ";
        }
      }
      if (theDenominatorFactorsWithMults[i].TotalDegree()>1)
        polyStream << ")";
      for (int j=0; j<theDenominatorFactorsWithMults.size(); j++)
      { Rational theExp=theDenominatorFactorsWithMults.theCoeffs[j];
        if (j==i)
          theExp-=k+1;
        if (theExp==0)
          continue;
        polyStream << "(" << theDenominatorFactorsWithMults[j].ToString(&theFormat) << ")";
        if (theExp>1)
          polyStream << "^{" << theExp << "}";
        for (int p=0; p<theExp; p++)
          currentSummand*=theDenominatorFactorsWithMults[j];
      }
      rfStream << "}{";
      if (k>0)
        rfStream << "(";
      rfStream << theDenominatorFactorsWithMults[i].ToString(&theFormat);
      if (k>0)
        rfStream << ")^{" << k+1 << "}";
      rfStream << "}";
      if (((theDenominatorFactorsWithMults.theCoeffs[i]-1)!=k) || (i!=theDenominatorFactorsWithMults.size()-1))
      { rfStream << "+";
        polyStream << "+";
      }
      thePolyThatMustVanish+=currentSummand;
    }
  }
  out << CGI::GetMathSpanPure(rfStream.str(), -1);
  out << "<br><br>After clearing denominators, we get the equality: ";
  out << "<br><br>" << CGI::GetMathSpanPure(polyStream.str());
  Polynomial<Polynomial<AlgebraicNumber> > univariateThatMustDie;
  thePolyThatMustVanish.GetPolyUnivariateWithPolyCoeffs(0, univariateThatMustDie);
  out << "<br><br>After rearanging we get that the following polynomial must vanish: " << CGI::GetMathSpanPure(univariateThatMustDie.ToString(&theFormat));
  out << "<br>Here, by ``vanish'', we mean that the coefficients in front of the powers of x must vanish.";
  Matrix<AlgebraicNumber> theSystemHomogeneous, theConstTerms;
  Polynomial<AlgebraicNumber>::GetLinearSystemFromLinearPolys(univariateThatMustDie.theCoeffs, theSystemHomogeneous, theConstTerms);
  theFormat.flagFormatMatrixAsLinearSystem=true;
  out << "<br>In other words, we need to solve the system: "
  << CGI::GetMathSpanPure(theSystemHomogeneous.ToStringSystemLatex(&theConstTerms, &theFormat),-1);
  theSystemHomogeneous.GaussianEliminationByRows(&theConstTerms, 0,0,0, &out, &theFormat);
  PolynomialSubstitution<AlgebraicNumber> theSub;
  theSub.MakeIdSubstitution(varCounter+1);
  for (int i=1; i<theSub.size; i++)
    theSub[i].MakeConst(theConstTerms(i-1,0));
//  out << "The sub is: " << theSub.ToString();
  std::stringstream rfComputedStream;
  for (int i =0; i<theDenominatorFactorsWithMults.size(); i++)
    for (int k=0; k<theDenominatorFactorsWithMults.theCoeffs[i]; k++)
    { theNumerators[i][k].Substitution(theSub);
      rfComputedStream << "\\frac{" << theNumerators[i][k].ToString(&theFormat) << "}";
      rfComputedStream << "{";
      rfComputedStream << "(" << theDenominatorFactorsWithMults[i].ToString(&theFormat) << ")";
      if (k>0)
        rfComputedStream << "^{" << k+1 << "}";
      rfComputedStream << "}";
      if (((theDenominatorFactorsWithMults.theCoeffs[i]-1)!=k) || (i!=theDenominatorFactorsWithMults.size()-1))
        rfComputedStream << "+";
    }
  out << "<br>Therefore, the final partial fraction decomposition is: ";
  std::stringstream answerFinalStream;
  answerFinalStream << inputRF.ToString(&theFormat) << "=";
  if (!quotientRat.IsEqualToZero())
    answerFinalStream << quotientRat.ToString(&theFormat) << "+ ";
  answerFinalStream << transformedRF.ToString(&theFormat) << "=";
  if (!quotientRat.IsEqualToZero())
    answerFinalStream << quotientRat.ToString(&theFormat) << "+ ";
  answerFinalStream << rfComputedStream.str();
  out << CGI::GetMathSpanPure(answerFinalStream.str());
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerGaussianEliminationMatrix(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("innerGaussianEliminationMatrix");
  Expression theConverted;
  if (!Calculator::innerMatrixRational(theCommands, input, theConverted))
  { theCommands.Comments << "<hr>Failed to extract rational matrix from " << input.ToString();
    return false;
  }
  Matrix<Rational> theMat;
  if (!theConverted.IsOfType<Matrix<Rational> >(&theMat))
  { theCommands.Comments << "<hr>Failed to extract rational matrix, got intermediate conversion to: " << theConverted.ToString();
    return false;
  }
  if (theMat.NumRows<2)
  { theCommands.Comments << "<hr>The matrix I got as input had only 1 row. Possible user typo?";
    return false;
  }
  std::stringstream out;
  theMat.GaussianEliminationByRows(0,0,0,0,&out);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerIntegrateRF(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntegrateRF");
  if (input.children.size!=3)
    return false;
  Expression theConvertedE;
  bool isGood=CalculatorSerialization::innerRationalFunction(theCommands, input[2],theConvertedE);
  if (isGood)
    if (theConvertedE.IsOfType<RationalFunctionOld>())
      isGood=false;
  if (!isGood)
  { theCommands.Comments << "<hr>Failed to convert " << input[2].ToString() << " to rational function. ";
    return false;
  }
  if (theConvertedE.GetNumContextVariables()>1)
  { theCommands.Comments << "<hr>I converted " << input[2].ToString() << " to rational function, but it is of " << theConvertedE.GetNumContextVariables()
    << " variables. I have been taught to work with 1 variable only. ";
    return false;
  }
  if (theConvertedE.GetNumContextVariables()==1)
    if (theConvertedE.GetContext().ContextGetContextVariable(0)!=input[1])
    { theCommands.Comments << "<hr>The univariate rational function was in variable " << theConvertedE.GetContext().ToString()
      << " but the variable of integration is " << input[1].ToString();
      return false;
    }
  RationalFunctionOld theRF=theConvertedE.GetValue<RationalFunctionOld>();
  crash << "not implemented yet" << crash ;

//  if (!CalculatorSerialization::inner)
  return false;
}

bool CalculatorFunctionsGeneral::innerRationalFunctionSubstitution(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerRationalFunctionSubstitution");
//std::cout << "input of innerRationalFunctionSubstitution: " << input.ToString();
  if (input.children.size!=2)
    return false;
  if (!input[0].IsOfType<RationalFunctionOld>())
    return false;
  if (input[0].GetValue<RationalFunctionOld>().GetMinNumVars()>1)
    return false;
  Expression ResultRationalForm;
  Expression finalContext;
  finalContext.ContextMakeContextWithOnePolyVar(theCommands, input[1]);
  ResultRationalForm.AssignValueWithContext(input[0].GetValue<RationalFunctionOld>(), finalContext, theCommands);
  return CalculatorFunctionsGeneral::innerExpressionFromRF(theCommands, ResultRationalForm, output);
}

bool CalculatorFunctionsGeneral::innerCompositeConstTimesAnyActOn(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCompositeConstTimesAnyActOn");
  if (!input.IsListNElements())
    return false;
  if (!input[0].IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  if (!input[0][1].IsConstantNumber())
    return false;
  Expression functionActsOnE;
  functionActsOnE.reset(theCommands);
  functionActsOnE.AddChildOnTop(input[0][2]);
  functionActsOnE.AddChildOnTop(input[1]);
  theCommands.CheckInputNotSameAsOutput(input, output);
  return output.MakeXOX(theCommands, theCommands.opTimes(), input[0][1], functionActsOnE);
}

bool CalculatorFunctionsGeneral::innerCompositeEWAactOnPoly(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCompositeEWAactOnPoly");
  if (input.children.size!=2)
    return false;
  Expression theEWAE=input[0];
  Expression theArgument=input[1];
  if (!theEWAE.IsListStartingWithAtom(theCommands.opElementWeylAlgebra()))
    return false;
  if (!theArgument.IsBuiltInType())
    return false;
  if (!theEWAE.MergeContexts(theEWAE, theArgument))
    return false;
  Polynomial<Rational> theArgumentPoly;
  Expression theArgumentConverted;
  if (theArgument.ConvertToType<Polynomial<Rational> >(theArgumentConverted))
    theArgumentPoly=theArgumentConverted.GetValue<Polynomial<Rational> >();
  else if (theArgument.ConvertToType<ElementWeylAlgebra<Rational> >(theArgumentConverted))
  { if(!theArgumentConverted.GetValue<ElementWeylAlgebra<Rational> >().IsPolynomial(&theArgumentPoly))
      return false;
  } else
    return false;
  const ElementWeylAlgebra<Rational>& theEWA=theEWAE.GetValue<ElementWeylAlgebra<Rational> >();
  if (theEWA.HasNonSmallPositiveIntegerDerivation())
  { theCommands.Comments << "<hr> I cannot apply " << theEWA.ToString() << " onto " << theArgumentPoly.ToString() << " as "
    << "the differential operator contains non-integral differential operator exponents. ";
    return false;
  }
  if (!theEWA.ActOnPolynomial(theArgumentPoly))
  { std::stringstream out;
    out << "Failed to act by operator " << theEWA.ToString() << " on polynomial " << theArgumentPoly.ToString()
    << " (possibly the weyl algebra element has non-integral exponents)";
    return output.SetError(out.str(), theCommands);
  }
  return output.AssignValueWithContext(theArgumentPoly, theEWAE.GetContext(), theCommands);
}

bool CalculatorFunctionsGeneral::innerMakeMakeFile(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerMakeMakeFile");
  ProjectInformation& theProjectInfo=ProjectInformation::GetMainProjectInfo();
  List<std::string> cppFilesNoExtension;
  for (int i=0; i<theProjectInfo.theFiles.size; i++)
  { std::string theFileNameWithPath=theProjectInfo.theFiles[i].FileName;
    if (theFileNameWithPath[theFileNameWithPath.size()-1]=='h')
      continue;
    theFileNameWithPath.resize(theFileNameWithPath.size()-4);
    std::string theFileNameNoPathNoExtensionReversed, theFileNameNoPathNoExtension;
    for (int j=theFileNameWithPath.size()-1; j>=0; j--)
      if (theFileNameWithPath[j]=='/')
        break;
      else
        theFileNameNoPathNoExtensionReversed.push_back(theFileNameWithPath[j]);
    for (int j=theFileNameNoPathNoExtensionReversed.size()-1; j>=0; j--)
      theFileNameNoPathNoExtension.push_back(theFileNameNoPathNoExtensionReversed[j]);
    cppFilesNoExtension.AddOnTop(theFileNameNoPathNoExtension);
  }
  std::fstream theFileStream;
  XML::OpenFileCreateIfNotPresent(theFileStream, theCommands.PhysicalPathOutputFolder+"makefile", false, true, false);
  std::stringstream outHtml;
  theFileStream << "all: directories calculator\n\n";
  theFileStream << "directories: Debug\n";
  theFileStream << "Debug:\n" << "\tmkdir ./Debug\n";
  theFileStream << "calculator: ";
  for (int i=0; i<cppFilesNoExtension.size; i++)
    theFileStream << cppFilesNoExtension[i] << ".o ";
  theFileStream << "\n\tg++ -std=c++11 -pthread ";
  for (int i=0; i<cppFilesNoExtension.size; i++)
    theFileStream << cppFilesNoExtension[i] << ".o ";
  theFileStream << "-o ./Debug/calculator\n\n";
  for (int i=0; i<cppFilesNoExtension.size; i++)
    theFileStream << cppFilesNoExtension[i] << ".o: " << cppFilesNoExtension[i] << ".cpp\n\tg++ -std=c++0x -pthread -c " << cppFilesNoExtension[i] << ".cpp\n\n";
  outHtml << "<a href=\" " << theCommands.DisplayPathOutputFolder << "makefile" << "\"> makefile </a>";
  return output.AssignValue(outHtml.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerDifferentiateConstPower(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDifferentiateConstPower");
  //////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1];
  const Expression& theArgument=input[2];
  //////////////////////
  if (!theArgument.IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
    return false;
  if (!theArgument[2].IsConstantNumber())
    return false;
  Expression theMonomial, theTerm, theExponent, basePrime, minusOne;
  minusOne.AssignValue<Rational>(-1, theCommands);
  theExponent.MakeXOX(theCommands, theCommands.opPlus(), theArgument[2], minusOne);
  theMonomial.MakeXOX(theCommands, theCommands.opThePower(), theArgument[1], theExponent);
  basePrime.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[1]);
  theTerm.MakeXOX(theCommands, theCommands.opTimes(), theArgument[2], theMonomial);
  return output.MakeXOX(theCommands, theCommands.opTimes(), theTerm, basePrime);
}

bool CalculatorFunctionsGeneral::innerDifferentiateAPowerB(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDifferentiateAPowerB");
  //////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1];
  const Expression& theArgument=input[2];
  //////////////////////
  //d/dx a^b= d/dx(e^{b\\ln a}) = a^b d/dx(b\\log a)
  if (!theArgument.IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
    return false;
  Expression logBase, exponentTimesLogBase, derivativeExponentTimesLogBase;
  logBase.reset(theCommands, 2);
  logBase.AddChildAtomOnTop(theCommands.opLog());
  logBase.AddChildOnTop(theArgument[1]);
  exponentTimesLogBase.MakeXOX(theCommands, theCommands.opTimes(), theArgument[2], logBase);
  derivativeExponentTimesLogBase.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, exponentTimesLogBase);
  return output.MakeXOX(theCommands, theCommands.opTimes(), theArgument, derivativeExponentTimesLogBase);
}

bool CalculatorFunctionsGeneral::innerDifferentiateConstant(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDifferentiateConstant");
  //////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theArgument=input[2];
  //////////////////////
  if (!theArgument.IsConstantNumber())
    return false;
  return output.AssignValue<Rational>(0, theCommands);
}

bool CalculatorFunctionsGeneral::innerDifferentiateX(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDifferentiateX");
  //////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1];
  const Expression& theArgument=input[2];
  //////////////////////
  if (theArgument!=theDOvar)
    return false;
  return output.AssignValue<Rational>(1, theCommands);
}

bool CalculatorFunctionsGeneral::innerDifferentiateTrigAndInverseTrig(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDifferentiateTrigAndInverseTrig");
  /////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString() << " - possible user typo?";
  const Expression& theArgument=input[2];
  //////////////////////
  if (theArgument.IsAtomGivenData(theCommands.opSin()))
    return output.MakeAtom(theCommands.opCos(), theCommands);
  if (theArgument.IsAtomGivenData(theCommands.opCos()))
  { Expression mOneE, sinE;
    mOneE.AssignValue<Rational>(-1, theCommands);
    sinE.MakeAtom(theCommands.opSin(), theCommands);
    return output.MakeXOX(theCommands, theCommands.opTimes(), mOneE, sinE);
  }
  if (theArgument.IsAtomGivenData(theCommands.opArcTan()))
  { Polynomial<Rational> onePlusXsquared;
    RationalFunctionOld oneOverOnePlusXsquared;
    onePlusXsquared.MakeMonomiaL(0,2);
    onePlusXsquared+=1;
    oneOverOnePlusXsquared.MakeOne(theCommands.theGlobalVariableS);
    oneOverOnePlusXsquared/=onePlusXsquared;
    Expression theContext;
    theContext.ContextMakeContextWithOnePolyVar(theCommands, "x");
    return output.AssignValueWithContext(oneOverOnePlusXsquared, theContext, theCommands);
  }
  return false;
}

bool CalculatorFunctionsGeneral::innerCompositeDifferentiateLog(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCompositeDifferentiateLog");
  /////////////////////
//  std::cout << "<hr>input composite: " << input.ToString();
  if (input.children.size!=2)
    return false;
  if (!input[0].IsListNElementsStartingWithAtom(theCommands.opDifferentiate(), 3))
    return false;
  if (!input[0][2].IsAtomGivenData(theCommands.opLog()))
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  Expression OneE;
  OneE.AssignValue(1, theCommands);
  output.reset(theCommands, 2);
  return output.MakeXOX(theCommands, theCommands.opDivide(), OneE, input[1]);
}

bool CalculatorFunctionsGeneral::outerAssociateAdivBdivCpowerD(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::outerAssociateAdivBdivCpowerD");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  if (!input[2].IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
    return false;
  if (!input[2][1].IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  Expression numeratorE, numeratorLeftE, denominatorE;
  output.reset(theCommands, 3);
  numeratorLeftE.MakeXOX(theCommands, theCommands.opThePower(), input[2][1][2], input[2][2]);
  numeratorE.MakeXOX(theCommands, theCommands.opTimes(), input[1], numeratorLeftE);
  denominatorE.MakeXOX(theCommands, theCommands.opThePower(), input[2][1][1], input[2][2]);
  return output.MakeXOX(theCommands, theCommands.opDivide(), numeratorE, denominatorE);
}

bool CalculatorFunctionsGeneral::outerDivCancellations(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::outerDivCancellations");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  if (!input[1].IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  if (!input[2].IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  if (input[1][2]==input[2][2])
    return output.MakeXOX(theCommands, theCommands.opDivide(), input[1][1], input[2][1]);
  if (input[1][1]==input[2][1])
    return output.MakeXOX(theCommands, theCommands.opDivide(), input[2][2], input[1][2]);
  return false;
}

bool CalculatorFunctionsGeneral::outerAssociateDivisionDivision(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::outerAssociateDivisionDivision");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  if (input[1].IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
  { Expression newRightE;
    newRightE.MakeXOX(theCommands, theCommands.opTimes(), input[2], input[1][2]);
    return output.MakeXOX(theCommands, theCommands.opDivide(), input[1][1], newRightE);
  }
  if (input[2].IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
  { Expression newLeftE;
    newLeftE.MakeXOX(theCommands, theCommands.opTimes(), input[1], input[2][2]);
    return output.MakeXOX(theCommands, theCommands.opDivide(), newLeftE, input[2][1]);
  }
  return false;
}

bool CalculatorFunctionsGeneral::innerDifferentiateChainRule(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDifferentiateChainRule");
  /////////////////////
//  std::cout << "here be i!";
  if (input.children.size!=3)
    return false;
//  std::cout << "here be i number 2!";
  if (!input[1].IsAtom())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1], theArgument=input[2];
  //////////////////////
//  std::cout << "ere be chain rule! Argument be: " << theArgument.ToString() << " Argument[0] be: " << theArgument[0].ToString();
  if (!theArgument.IsListNElementsStartingWithAtom(-1, 2))
    return false;
//  std::cout << " continues to rule! The argument be: " << theArgument.ToString() << " theArgument.IsGoodForChainRuleFunction()= "
//  << theArgument.IsGoodForChainRuleFunction() << " theArgument[0].IsGoodForChainRuleFunction()="
//  << theArgument[0].IsGoodForChainRuleFunction() << "; ";
  if (!theArgument.IsGoodForChainRuleFunction() && !theArgument[0].IsGoodForChainRuleFunction())
    return false;
//  std::cout << " here be ruler #1!";
  theCommands.CheckInputNotSameAsOutput(input, output);
  output.reset(theCommands);
  Expression multiplicandleft, multiplicandleftFunction, multiplicandright;
  multiplicandleftFunction.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[0]);
  multiplicandleft.reset(theCommands);
  multiplicandleft.AddChildOnTop(multiplicandleftFunction);
  multiplicandleft.AddChildOnTop(theArgument[1]);
  multiplicandright.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[1]);
  return output.MakeXOX(theCommands, theCommands.opTimes(), multiplicandleft, multiplicandright);
}

bool CalculatorFunctionsGeneral::innerDifferentiateAplusB(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDifferentiateAplusB");
  /////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString() << " - possible user typo?";
  const Expression& theDOvar=input[1], theArgument=input[2];
  //////////////////////
  if (!theArgument.IsListNElementsStartingWithAtom(theCommands.opPlus(), 3))
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  output.reset(theCommands);
  Expression leftSummand, rightSummand;
  leftSummand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[1]);
  rightSummand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[2]);
  return output.MakeXOX(theCommands, theCommands.opPlus(), leftSummand, rightSummand);
}

bool CalculatorFunctionsGeneral::innerDifferentiateAtimesB(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDifferentiateAtimesB");
    //////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1], theArgument=input[2];
  //////////////////////
  if (!theArgument.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  output.reset(theCommands);
  Expression changedMultiplicand, leftSummand, rightSummand;
  changedMultiplicand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[1]);
  leftSummand.MakeXOX(theCommands, theCommands.opTimes(), changedMultiplicand, theArgument[2]);
  changedMultiplicand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[2]);
  rightSummand.MakeXOX(theCommands, theCommands.opTimes(), theArgument[1], changedMultiplicand );
  return output.MakeXOX(theCommands, theCommands.opPlus(), leftSummand, rightSummand);
}

bool CalculatorFunctionsGeneral::innerPowerAnyToZero(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPowerAnyToZero");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
    return false;
  if (!input[2].IsEqualToZero())
    return false;
//  std::cout << "input[1]: "<< input[1].ToString() << ", input[2]: " << input[2].ToString();
  if (input[1].IsEqualToZero())
    return output.SetError("Error: expression of the form 0^0 is illegal.", theCommands);
  return output.AssignValue<Rational>(1, theCommands);
}

bool CalculatorFunctionsGeneral::innerDifferentiateAdivideBCommutative(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDifferentiateAdivideBCommutative");
    //////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression " << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1], theArgument=input[2];
  //////////////////////
  //Quotient rule (commutative): (a/b^n)':=(a'b-n a b')/b^{n+1}
  if (!theArgument.IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  output.reset(theCommands);
  Expression theDenominatorBase, eOne, theDenominatorExponentPlusOne, theDenominatorExponent, changedMultiplicand,
  leftSummand, rightSummand, theDenominatorFinal, numerator;
  eOne.AssignValue(1, theCommands);
  bool denBaseFound=false;
  if (theArgument[2].IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
    if (theArgument[2][2].IsConstantNumber())
    { denBaseFound=true;
      theDenominatorBase=theArgument[2][1];
      theDenominatorExponent=theArgument[2][2];
      theDenominatorExponentPlusOne.MakeXOX(theCommands, theCommands.opPlus(), theDenominatorExponent, eOne);
    }
  if (!denBaseFound)
  { theDenominatorBase=theArgument[2];
    theDenominatorExponentPlusOne.AssignValue(2, theCommands);
    theDenominatorExponent.AssignValue(1, theCommands);
  }
  theDenominatorFinal.MakeXOX(theCommands, theCommands.opThePower(), theDenominatorBase, theDenominatorExponentPlusOne);
  changedMultiplicand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[1]);
  leftSummand.MakeXOX(theCommands, theCommands.opTimes(), changedMultiplicand, theDenominatorBase);
  rightSummand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theDenominatorBase);
  changedMultiplicand.MakeXOX(theCommands, theCommands.opTimes(), theArgument[1], rightSummand);
  rightSummand.MakeXOX(theCommands, theCommands.opTimes(), theDenominatorExponent, changedMultiplicand);
  numerator.MakeXOX(theCommands, theCommands.opMinus(), leftSummand, rightSummand);
  return output.MakeXOX(theCommands, theCommands.opDivide(), numerator, theDenominatorFinal);
}

bool CalculatorFunctionsGeneral::innerDifferentiateAdivideBNONCommutative(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDifferentiateAdivideBNONCommutative");
    //////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands.Comments << "<hr>Warning: differentiating with respect to the non-atomic expression " << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1], theArgument=input[2];
  //////////////////////
  //Quotient rule (non-commutative): (a/b)'=(ab^{-1})'=a' b - a b^{-1} b' b^{-1}
  if (!theArgument.IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  output.reset(theCommands);
  Expression changedMultiplicand, leftSummand, rightSummand;
  Expression bInverse, bPrime, eMOne;
  eMOne.AssignValue<Rational>(-1, theCommands);
  changedMultiplicand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[1]);
  leftSummand.MakeXOX(theCommands, theCommands.opDivide(), changedMultiplicand, theArgument[2]);
  bPrime.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[2]);
  bInverse.MakeXOX(theCommands, theCommands.opThePower(), theArgument[2], eMOne);
  rightSummand.MakeXOX(theCommands, theCommands.opTimes(), bPrime, bInverse); //rightSummand= b' b^{-1}
  changedMultiplicand.MakeXOX(theCommands, theCommands.opTimes(), bInverse, rightSummand); //changedMultiplicand= b^-1 b' b^-1
  rightSummand.MakeXOX(theCommands, theCommands.opTimes(), theArgument[1], changedMultiplicand);
  return output.MakeXOX(theCommands, theCommands.opMinus(), leftSummand, rightSummand);
}

bool CalculatorFunctionsGeneral::outerCommuteAtimesBifUnivariate(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::outerCommuteAtimesBifUnivariate");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  if (input[1].IsConstantNumber())
    return false;
  HashedListSpecialized<Expression> theList;
  input.GetBlocksOfCommutativity(theList);
  if (theList.size!=1)
    return false;
  if (input[2]>input[1] || input[2]==input[1])
    return false;
//  std::cout << "<hr>" << input[2].ToString() << " less than " << input[1].ToString();
//  std::cout << "ere be i, number 1!";
  output=input;
  output.children.SwapTwoIndices(1,2);
  return true;
}

bool CalculatorFunctionsGeneral::outerCommuteAtimesBtimesCifUnivariate(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::outerCommuteAtimesBtimesCifUnivariate");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(),3))
    return false;
  const Expression& leftE=input[1];
  if (leftE.IsConstantNumber())
    return false;
  if (!input[2].IsListNElementsStartingWithAtom(theCommands.opTimes(),3))
    return false;
  const Expression& rightE=input[2][1];
  HashedListSpecialized<Expression> theList;

  leftE.GetBlocksOfCommutativity(theList);
  rightE.GetBlocksOfCommutativity(theList);
  if (theList.size!=1)
    return false;
  if (rightE>leftE || leftE==rightE)
    return false;
//  std::cout << "<hr>" << rightE.ToString() << " less than " << leftE.ToString();
  Expression leftMultiplicand;
  leftMultiplicand.MakeXOX(theCommands, theCommands.opTimes(), rightE, leftE);
//  std::cout << "Left multiplicand: " << leftE.ToString() << ", right: " << rightE.ToString() << " ";
  return output.MakeXOX(theCommands, theCommands.opTimes(), leftMultiplicand, input[2][2]);
}

bool CalculatorFunctionsGeneral::outerDifferentiateWRTxTimesAny(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::outerDifferentiateWRTxTimesAny");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  if (!input[1].IsListNElementsStartingWithAtom(theCommands.opDifferentiate(), 2))
    return false;
  if (input[2].IsBuiltInAtom())
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  output=input[1];
  return output.AddChildOnTop(input[2]);
}

bool CalculatorFunctionsGeneral::innerDdivDxToDifferentiation(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDdivDxToDifferentiation");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  std::string denominatorString,numeratorString;
  if (!input[1].IsAtom(&numeratorString))
    return false;
  if (!input[2].IsAtom(&denominatorString))
    return false;
  if (numeratorString!="d")
    return false;
  if (denominatorString.size()<2)
    return false;
  if (denominatorString[0]!='d')
    return false;
  for (int i=0; i<((signed) denominatorString.size())-1; i++)
    denominatorString[i]=denominatorString[i+1];
  denominatorString.resize(denominatorString.size()-1);
  output.reset(theCommands, 2);
  output.AddChildAtomOnTop(theCommands.opDifferentiate());
  return output.AddChildAtomOnTop(theCommands.AddOperationNoRepetitionOrReturnIndexFirst(denominatorString));
}

bool CalculatorFunctionsGeneral::outerAdivBpowerItimesBpowerJ(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::outerAdivBpowerItimesBpowerJ");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  if (!input[1].IsListNElementsStartingWithAtom(theCommands.opDivide(),3))
    return false;
  Expression denominatorBase, denominatorExponent;
  Expression numeratorBase, numeratorExponent;
  input[1][2].GetBaseExponentForm(denominatorBase, denominatorExponent);
  input[2].GetBaseExponentForm(numeratorBase, numeratorExponent);
  if (denominatorBase!=numeratorBase)
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  Expression rightMultiplicand, rightMultiplicandExponent;
  rightMultiplicandExponent.MakeXOX(theCommands, theCommands.opMinus(), numeratorExponent, denominatorExponent);
  rightMultiplicand.MakeXOX(theCommands, theCommands.opThePower(), denominatorBase, rightMultiplicandExponent);
  return output.MakeXOX(theCommands, theCommands.opTimes(), input[1][1], rightMultiplicand);
}

bool Expression::SplitProduct(int numDesiredMultiplicandsLeft, Expression& outputLeftMultiplicand, Expression& outputRightMultiplicand)const
{ MacroRegisterFunctionWithName("Expression::SplitProduct");
  if (numDesiredMultiplicandsLeft<=0)
    return false;
  this->CheckInitialization();
  List<Expression> theMultiplicandsLeft, theMultiplicandsRight;
  this->theBoss->AppendOpandsReturnTrueIfOrderNonCanonical(*this, theMultiplicandsLeft, this->theBoss->opTimes());
  if (theMultiplicandsLeft.size<=numDesiredMultiplicandsLeft)
    return false;
  theMultiplicandsRight.SetExpectedSize(theMultiplicandsLeft.size-numDesiredMultiplicandsLeft);
  for (int i=numDesiredMultiplicandsLeft; i<theMultiplicandsLeft.size; i++)
    theMultiplicandsRight.AddOnTop(theMultiplicandsLeft[i]);
  theMultiplicandsLeft.SetSize(numDesiredMultiplicandsLeft);
  outputLeftMultiplicand.MakeXOdotsOX(*this->theBoss, this->theBoss->opTimes(), theMultiplicandsLeft);
  return outputRightMultiplicand.MakeXOdotsOX(*this->theBoss, this->theBoss->opTimes(), theMultiplicandsRight);
}

bool CalculatorFunctionsGeneral::outerAtimesBpowerJplusEtcDivBpowerI(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::outerAtimesBpowerJplusEtcDivBpowerI");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opDivide(),3))
    return false;
//  std::cout << "ere be i!";
  Expression denominatorBase, denominatorExponent;
  input[2].GetBaseExponentForm(denominatorBase, denominatorExponent);
  if (!denominatorBase.IsAtom())
    return false;
  MonomialCollection<Expression, Rational> numerators, numeratorsNew;
  theCommands.CollectSummands(theCommands, input[1], numerators);
  numeratorsNew.SetExpectedSize(numerators.size());
  numeratorsNew.MakeZero();
  Expression numeratorMultiplicandLeft, numeratorMultiplicandRight, numeratorBaseRight, numeratorExponentRight;
  Expression newNumSummand, newNumSummandRightPart, newNumExponent, mOneE;
  mOneE.AssignValue(-1, theCommands);
  for (int i=0; i<numerators.size(); i++)
  { if (numerators[i].IsConstantNumber())
    { newNumExponent.MakeXOX(theCommands, theCommands.opTimes(), mOneE, denominatorExponent);
      newNumSummand.MakeXOX(theCommands, theCommands.opThePower(), denominatorBase, newNumExponent);
      numeratorsNew.AddMonomial(newNumSummand, numerators.theCoeffs[i]);
      continue;
    }
    numerators[i].GetBaseExponentForm(numeratorBaseRight, numeratorExponentRight);
    if (numeratorBaseRight==denominatorBase)
    { newNumExponent.MakeXOX(theCommands, theCommands.opMinus(), numeratorExponentRight, denominatorExponent);
      newNumSummand.MakeXOX(theCommands, theCommands.opThePower(), denominatorBase, newNumExponent);
      numeratorsNew.AddMonomial(newNumSummand, numerators.theCoeffs[i]);
      continue;
    }
    bool isGood=false;
    for (int j=1; numerators[i].SplitProduct(j, numeratorMultiplicandLeft, numeratorMultiplicandRight); j++)
    { numeratorMultiplicandRight.GetBaseExponentForm(numeratorBaseRight, numeratorExponentRight);
      if (numeratorBaseRight!=denominatorBase)
        continue;
      newNumExponent.MakeXOX(theCommands, theCommands.opMinus(), numeratorExponentRight, denominatorExponent);
      newNumSummandRightPart.MakeXOX(theCommands, theCommands.opThePower(), denominatorBase, newNumExponent);
      newNumSummand.MakeXOX(theCommands, theCommands.opTimes(), numeratorMultiplicandLeft, newNumSummandRightPart);
      numeratorsNew.AddMonomial(newNumSummand, numerators.theCoeffs[i]);
      isGood=true;
      break;
    }
//    if (!isGood)
//      std:: cout << "- oh no: " << numerators[i] << " is no good! ";
    if (!isGood)
      return false;
  }
  return output.MakeSum(theCommands, numeratorsNew);
}

bool CalculatorFunctionsGeneral::innerSort(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSort");
  if (!input.IsSequenceNElementS())
    return false;
  List<Expression> sortedExpressions;
  sortedExpressions.ReservE(input.children.size-1);
  for (int i=1; i<input.children.size; i++)
    sortedExpressions.AddOnTop(input[i]);
  sortedExpressions.QuickSortAscending();
  output.reset(theCommands, sortedExpressions.size+1);
  output.AddChildAtomOnTop(theCommands.opSequence());
  for (int i=0; i<sortedExpressions.size; i++)
    output.AddChildOnTop(sortedExpressions[i]);
  return true;
}

bool CalculatorFunctionsGeneral::innerGrowDynkinType(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGrowDynkinType");
  if (input.children.size!=3)
    return false;
  const Expression& theSmallerTypeE=input[1];
  DynkinType theSmallDynkinType;
  if (!CalculatorSerialization::innerLoadDynkinType(theCommands, theSmallerTypeE, theSmallDynkinType))
    return false;
  SemisimpleLieAlgebra* theSSalg=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorSerialization::innerSSLieAlgebra, input[2], theSSalg))
    return output.SetError("Error extracting ambient Lie algebra.", theCommands);
  SemisimpleSubalgebras tempSas;
  tempSas.owneR=theSSalg;
  tempSas.ownerField=&theCommands.theObjectContainer.theAlgebraicClosure;
  tempSas.theGlobalVariables=theCommands.theGlobalVariableS;
  tempSas.ComputeSl2sInitOrbitsForComputationOnDemand();
  if (!tempSas.RanksAndIndicesFit(theSmallDynkinType))
    return output.SetError("Error: type "+theSmallDynkinType.ToString()+" does not fit inside "+theSSalg->theWeyl.theDynkinType.ToString(), theCommands);
  List<DynkinType> largerTypes;
  List<List<int> > imagesSimpleRoots;
  if (!tempSas.GrowDynkinType(theSmallDynkinType, largerTypes, &imagesSimpleRoots))
    return output.SetError("Error: growing type "+theSmallDynkinType.ToString()+" inside "+theSSalg->theWeyl.theDynkinType.ToString() + " failed. ", theCommands);
  std::stringstream out;
  out << "Inside " << theSSalg->theWeyl.theDynkinType.ToString() << ", input type " << theSmallDynkinType.ToString();
  if (largerTypes.size==0)
    out << " cannot grow any further. ";
  else
  { CandidateSSSubalgebra tempCandidate;
    out << " can grow to the following types. <br>";
    out << "<table border=\"1\"><td>Larger type</td><td>Root injection</td><td>Highest weight module containing new simple generator</td></tr>";
    for(int i=0; i<largerTypes.size; i++)
    { out << "<tr><td>" << largerTypes[i].ToString() << "</td>";
      out << "<td>";
      for (int j=0; j<imagesSimpleRoots[i].size; j++)
      { out << "r_" << j+1 << " -> " << "r_" << imagesSimpleRoots[i][j]+1;
        if (j!=imagesSimpleRoots[i].size)
          out << ", ";
      }
      out << "</td><td>";
      out << CGI::GetMathSpanPure(tempSas.GetHighestWeightFundNewComponentFromImagesOldSimpleRootsAndNewRoot(largerTypes[i], imagesSimpleRoots[i], tempCandidate).ToStringLetterFormat("\\omega"));
      out << "</td></tr>";
    }
    out << "</table>";
  }
  return output.AssignValue(out.str(), theCommands);
}

void Expression::GetBlocksOfCommutativity(HashedListSpecialized<Expression>& inputOutputList)const
{ MacroRegisterFunctionWithName("Expression::GetBlocksOfCommutativity");
  this->CheckInitialization();
  if (this->IsAtom())
  { inputOutputList.AddOnTopNoRepetition(*this);
    return;
  }
  if (this->IsConstantNumber())
    return;
  std::string whichOperation;
  if ((*this)[0].IsAtom(&whichOperation))
    if (this->theBoss->atomsThatAllowCommutingOfCompositesStartingWithThem.Contains(whichOperation))
    { for (int i=1; i<this->children.size; i++)
        (*this)[i].GetBlocksOfCommutativity(inputOutputList);
      return;
    }
//  std::cout << "<hr>which operation=" << whichOperation;
  inputOutputList.AddOnTopNoRepetition(*this);
}

bool Expression::MakeSequence(Calculator& owner, List<Expression>& inputSequence)
{ this->reset(owner, inputSequence.size+1);
  this->AddChildAtomOnTop(owner.opSequence());
  for (int i=0; i<inputSequence.size; i++)
    this->AddChildOnTop(inputSequence[i]);
  return true;
}

bool CalculatorFunctionsGeneral::innerGetUserDefinedSubExpressions(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGetUserDefinedSubExpressions");
  HashedListSpecialized<Expression> theList;
  input.GetBlocksOfCommutativity(theList);
  return output.MakeSequence(theCommands, theList);
}

bool CalculatorFunctionsGeneral::innerComputeSemisimpleSubalgebras(Calculator& theCommands, const Expression& input, Expression& output)
{ //bool showIndicator=true;
  MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerComputeSemisimpleSubalgebras");
  SemisimpleLieAlgebra* ownerSSPointer;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorSerialization::innerSSLieAlgebra, input, ownerSSPointer))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  SemisimpleLieAlgebra& ownerSS=*ownerSSPointer;
  std::stringstream out;
  if (ownerSS.GetRank()>6)
  { out << "<b>This code is completely experimental and has been set to run up to rank 6. As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
    return output.AssignValue(out.str(), theCommands);
  } else
    out << "<b>This code is completely experimental. Use the following printouts on your own risk</b>";
  SemisimpleSubalgebras tempSSsas
  (ownerSS, &theCommands.theObjectContainer.theAlgebraicClosure, &theCommands.theObjectContainer.theLieAlgebras, &theCommands.theObjectContainer.theSltwoSAs, theCommands.theGlobalVariableS);
  SemisimpleSubalgebras& theSSsubalgebras=theCommands.theObjectContainer.theSSsubalgebras[theCommands.theObjectContainer.theSSsubalgebras.AddNoRepetitionOrReturnIndexFirst(tempSSsas)];
  theSSsubalgebras.flagComputePairingTable=false;
  theSSsubalgebras.flagComputeNilradicals=false;
  theSSsubalgebras.FindTheSSSubalgebras(ownerSS);
  return output.AssignValue(theSSsubalgebras, theCommands);
}

bool CalculatorFunctionsGeneral::innerLispify(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerLispify");
  return output.AssignValue(input.ToStringSemiFull(), theCommands);
}

bool CalculatorFunctionsGeneral::innerLispifyFull(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerLispifyFull");
  return output.AssignValue(input.ToStringFull(), theCommands);
}

bool CalculatorFunctionsGeneral::innerMinPolyMatrix(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerMinPolyMatrix");
  if (!output.IsOfType<Matrix<Rational> >())
    if (!theCommands.innerMatrixRational(theCommands, input, output))
      return false;
  Matrix<Rational> theMat;
  if (!output.IsOfType<Matrix<Rational> >(&theMat))
  { theCommands.Comments << "<hr>Minimal poly computation: could not convert " << input.ToString() << " to rational matrix.";
    return true;
  }
  if (theMat.NumRows!=theMat.NumCols || theMat.NumRows<=0)
    return output.SetError("Error: matrix is not square!", theCommands);
  FormatExpressions tempF;
  tempF.polyAlphabeT.SetSize(1);
  tempF.polyAlphabeT[0]="q";
  Polynomial<Rational> theMinPoly;
  theMinPoly.AssignMinPoly(theMat);
  return output.AssignValue(theMinPoly.ToString(&tempF), theCommands);
}

bool CalculatorFunctionsGeneral::innerCharPolyMatrix(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerMinPolyMatrix");
  if (!output.IsOfType<Matrix<Rational> >())
    if (!theCommands.innerMatrixRational(theCommands, input, output))
      return false;
  Matrix<Rational> theMat;
  if (!output.IsOfType<Matrix<Rational> >(&theMat))
  { theCommands.Comments << "<hr>Characteristic poly computation: could not convert " << input.ToString() << " to rational matrix.";
    return true;
  }
  if (theMat.NumRows!=theMat.NumCols || theMat.NumRows<=0)
    return output.SetError("Error: matrix is not square!", theCommands);
  FormatExpressions tempF;
  tempF.polyAlphabeT.SetSize(1);
  tempF.polyAlphabeT[0]="q";
  Polynomial<Rational> theCharPoly;
  theCharPoly.AssignCharPoly(theMat);
  return output.AssignValue(theCharPoly.ToString(&tempF), theCommands);
}

bool CalculatorFunctionsGeneral::innerTrace(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerTrace");
//  std::cout << "<hr>getting matrix from: " << input.ToString();
  if (input.IsOfType<Matrix<Rational> >())
  { if (!input.GetValue<Matrix<Rational> >().IsSquare())
      return output.SetError("Error: attempting to get trace of non-square matrix.", theCommands);
    return output.AssignValue(input.GetValue<Matrix<Rational> >().GetTrace(), theCommands);
  }
  if (input.IsOfType<Matrix<RationalFunctionOld> >())
  { if (!input.GetValue<Matrix<RationalFunctionOld> >().IsSquare())
      return output.SetError("Error: attempting to get trace of non-square matrix.", theCommands);
    return output.AssignValue(input.GetValue<Matrix<RationalFunctionOld> >().GetTrace(), theCommands);
  }
  Matrix<Expression> theMat;
  if (!theCommands.GetMatrixExpressionsFromArguments(input, theMat))
    return false;
  if (!theMat.IsSquare())
    return output.SetError("Error: attempting to get trace of non-square matrix.", theCommands);
  if (theMat.NumRows==1)
  { theCommands.Comments << "Requesting trace of 1x1 matrix: possible interpretation of a scalar as a 1x1 matrix. Trace not taken";
    return false;
  }
  output=theMat.GetTrace();
  return true;
}

bool CalculatorFunctionsGeneral::innerLastElement(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerLastElement");
  if (input.HasBoundVariables())
    return false;
//  std::cout << "<hr>calling last element with input: " << input.ToString() << ", longer printout: " << input.ToStringSemiFull();
  if (input.IsAtom())
  { std::stringstream out;
    out << "Error: requesting the last element of the atom " << input.ToString();
    return output.SetError(out.str(), theCommands);
  }
  std::string firstAtom;
  if (input.children.size==2)
    if (input[0].IsAtom(&firstAtom) )
      if (firstAtom=="last")
        return CalculatorFunctionsGeneral::innerLastElement(theCommands, input[1], output);
  output=input[input.children.size-1];
  return true;
}

bool CalculatorFunctionsGeneral::innerRemoveLastElement(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerRemoveLastElement");
  if (input.HasBoundVariables())
    return false;
//  std::cout << "<hr>calling remove last element with input: " << input.ToString() << ", longer printout: " << input.ToStringSemiFull();
  if (input.IsAtom() || input.children.size==1)
  { std::stringstream out;
    if (input.IsAtom())
      out << "Error: requesting to remove the last element of the atom " << input.ToString();
    else
      out << "Error: requesting to remove the last element of the one-element list " << input.ToString();
    return output.SetError(out.str(), theCommands);
  }
  std::string firstAtom;
  if (input.children.size==2)
    if (input[0].IsAtom(&firstAtom) )
      if (firstAtom=="removeLast")
        return CalculatorFunctionsGeneral::innerRemoveLastElement(theCommands, input[1], output);

  output=input;
  output.children.RemoveLastObject();
  return output.SetChildAtomValue(0, theCommands.opSequence());
}

bool CalculatorFunctionsGeneral::innerInvertMatrix(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerInvertMatrix");
  Matrix<Rational> theMat;
  std::cout << "Lispified: " << input.ToString();
  if (!input.IsOfType<Matrix<Rational> >(&theMat))
    if (!theCommands.GetMatriXFromArguments<Rational>(input, theMat, 0, -1, 0))
      return output.SetError("Failed to extract matrix with rational coefficients", theCommands);
  if (theMat.NumRows!=theMat.NumCols || theMat.NumCols<1)
    return output.SetError("The matrix is not square", theCommands);
  if (theMat.GetDeterminant()==0)
    return output.SetError("Matrix determinant is zero.", theCommands);
  theMat.Invert(theCommands.theGlobalVariableS);
  return output.AssignValue(theMat, theCommands);
}

bool CalculatorFunctionsGeneral::innerPlotWedge(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotWedge");
  if (input.children.size!=6)
  { theCommands.Comments << "<hr>innerPlotWedge takes as input 4 arguments: x,y coordinates of center, starting angle, final angle. ";
    return false;
  }
  Rational xCoord, yCoord, radius, startAngle, endAngle;
  if (!input[1].IsOfType<Rational>(&xCoord) || !input[2].IsOfType<Rational>(&yCoord) || !input[3].IsOfType<Rational>(&radius) ||
      !input[4].IsOfType<Rational>(&startAngle) || !input[5].IsOfType<Rational>(&endAngle))
  { theCommands.Comments << "<hr>Failed to extract x, y coordinate, radius, start angle, end angle from " << input.ToString() << ". ";
    return false;
  }
  std::stringstream out;
  double x1wedge= MathRoutines::ReducePrecision(xCoord.DoubleValue()+ radius.DoubleValue() *FloatingPoint::cos(startAngle.DoubleValue()));
  double y1wedge= MathRoutines::ReducePrecision(yCoord.DoubleValue()+ radius.DoubleValue() *FloatingPoint::sin(startAngle.DoubleValue()));
  double x2wedge= MathRoutines::ReducePrecision(xCoord.DoubleValue()+ radius.DoubleValue() *FloatingPoint::cos(endAngle.DoubleValue()));
  double y2wedge= MathRoutines::ReducePrecision(yCoord.DoubleValue()+ radius.DoubleValue() *FloatingPoint::sin(endAngle.DoubleValue()));
  double xCoordDouble=MathRoutines::ReducePrecision(xCoord.DoubleValue());
  double yCoordDouble=MathRoutines::ReducePrecision(yCoord.DoubleValue());
  double startAngleDouble=MathRoutines::ReducePrecision(startAngle.DoubleValue());
  double radiusDouble=MathRoutines::ReducePrecision(radius.DoubleValue());
  double endAngleDouble=MathRoutines::ReducePrecision(endAngle.DoubleValue());
  out << "\\pscustom*[linecolor=cyan]{ \\psparametricplot[algebraic,linecolor=\\psColorGraph]{" << startAngleDouble << "}{" << endAngleDouble
  << "}{" << xCoordDouble << "+" << radiusDouble << "*cos(t)| " << yCoordDouble << "+" << radiusDouble << "*sin(t)} \\psline("
  << x2wedge << ", " << y2wedge << ")(" << xCoordDouble << ", " << yCoordDouble << ")" << "(" << x1wedge << ", " << y1wedge << ")}";
  out << "\\pscustom[linecolor=blue]{ \\psparametricplot[algebraic,linecolor=\\psColorGraph]{" << startAngleDouble << "}{" << endAngleDouble
  << "}{" << xCoordDouble << "+" << radiusDouble << "*cos(t)| " << yCoordDouble << "+" << radiusDouble << "*sin(t)} \\psline("
  << x2wedge << ", " << y2wedge << ")(" << xCoordDouble << ", " << yCoordDouble << ")" << "(" << x1wedge << ", " << y1wedge << ")}";
  PlotObject thePlot;
  thePlot.xLow=(-5);
  thePlot.xHigh=(5);
  thePlot.yLow=-2;
  thePlot.yHigh=5;
  thePlot.thePlotElement=(input);
  thePlot.thePlotString=(out.str());
  thePlot.thePlotStringWithHtml=(out.str());
  thePlot.thePlotType="plotFunction";
  return output.AssignValue(thePlot, theCommands);
}

bool CalculatorFunctionsGeneral::innerPlotIntegralOf(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotIntegralOf");
  if (input.children.size<4)
    return output.SetError("Plotting integral of takes at least three arguments: function, lower and upper bound. ", theCommands);
  const Expression& lowerE=input[2];
  const Expression& upperE=input[3];
  Expression functionE;
  double upperBound, lowerBound;
  if (!lowerE.EvaluatesToRealDouble(&upperBound) || !upperE.EvaluatesToRealDouble(&lowerBound))
    return output.SetError("Failed to convert upper and lower bounds of drawing function to rational numbers.", theCommands);
  if (upperBound<lowerBound)
    MathRoutines::swap(upperBound, lowerBound);
  if (!theCommands.CallCalculatorFunction(theCommands.innerSuffixNotationForPostScript, input[1], functionE))
  { std::stringstream out;
    out << "Failed to convert expression " << input[1].ToString() << " to postfix notation. ";
    return output.SetError(out.str(), theCommands);
  }
  std::stringstream theShadedRegion, theShadedRegionHTML;
  PlotObject thePlot;
  theShadedRegion << "\\pscustom*[linecolor=cyan]{"
  << thePlot.GetPlotStringFromFunctionStringAndRanges(false, functionE.GetValue<std::string>(), input[1].ToString(), lowerBound, upperBound)
  << "\\psline(" << std::fixed << upperBound << ", 0)(" << std::fixed << lowerBound << ", 0)}";
  theShadedRegionHTML << "\\pscustom*[linecolor=cyan]{"
  << thePlot.GetPlotStringFromFunctionStringAndRanges(true, functionE.GetValue<std::string>(), input[1].ToString(), lowerBound, upperBound)
  << "\\psline(" << std::fixed << upperBound << ", 0)(" << std::fixed << lowerBound << ", 0)}";
  thePlot.thePlotString=theShadedRegion.str();
  thePlot.thePlotStringWithHtml=theShadedRegionHTML.str();
  thePlot.xLow=lowerBound;
  thePlot.xHigh=upperBound;
  thePlot.yLow=-4;
  thePlot.yHigh=4;
  thePlot.thePlotElement=(input[1]);
  Plot plotFinal;
  plotFinal+=thePlot;
  plotFinal.AddFunctionPlotOnTop(input[1], functionE.GetValue<std::string>(), lowerBound, upperBound, -4,  4);
  return output.AssignValue(plotFinal, theCommands);
}

bool CalculatorFunctionsGeneral::innerDFQsEulersMethod(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDFQsEulersMethod");
  if (input.children.size!=7)
  { theCommands.Comments << "Euler method function takes 6 arguments";
    return false;
  }
  double xInitial, yInitial, leftEndpoint, rightEndpoint;
  int numPoints;
  if (!input[2].EvaluatesToRealDouble(&xInitial) || !input[3].EvaluatesToRealDouble(&yInitial))
  { theCommands.Comments << "Failed to extract initial x,y values from " << input.ToString();
    return false;
  }
//  std::cout << "xInitial=" << xInitial << " yInitial=" << yInitial;
  if (!input[4].IsSmallInteger(&numPoints) )
  { theCommands.Comments << "Failed to extract number of points from " << input.ToString();
    return false;
  }
  if (!input[5].EvaluatesToRealDouble(&leftEndpoint) || !input[6].EvaluatesToRealDouble(&rightEndpoint))
  { theCommands.Comments << "Failed to extract left and right endpoints from " << input.ToString();
    return false;
  }
  if (leftEndpoint>rightEndpoint)
    MathRoutines::swap(leftEndpoint, rightEndpoint);
  if (xInitial>rightEndpoint || xInitial<leftEndpoint)
  { theCommands.Comments << "The initial value for x, " << xInitial << ", does not lie between the left and right endpoints "
    << leftEndpoint << ", " << rightEndpoint << ". I am aborting Euler's method.";
    return false;
  }
  HashedList<Expression> knownConsts;
  List<double> knownValues;
  knownConsts.AddOnTop(theCommands.knownDoubleConstants);
  knownValues.AddListOnTop(theCommands.knownDoubleConstantValues);
  Expression xE, yE;
  xE.MakeAtom("x", theCommands);
  yE.MakeAtom("y", theCommands);
  if (knownConsts.Contains(xE) || knownConsts.Contains(yE))
  { theCommands.Comments << "The letters x, y appear to be already used to denote known constants, I cannot run Euler's method.";
    return false;
  }
  knownConsts.AddOnTop(xE);
  knownConsts.AddOnTop(yE);
  knownValues.AddOnTop(0);
  knownValues.AddOnTop(0);
  if (numPoints<2)
  { theCommands.Comments << "The number of points for Euler's method is " << numPoints << ", too few. ";
    return false;
  }
  if (numPoints>10001)
  { theCommands.Comments << "The number of points for Euler's method is " << numPoints
    << ", I am not allowed to handle that many. ";
    return false;
  }
  if (leftEndpoint==rightEndpoint)
  { theCommands.Comments << "Whlie doing Euler's method: right endpoint equals left!";
    return false;
  }
//  std::cout << "numpoints: " << numPoints << ", rightendpt: " << rightEndpoint << ", left end pt: " << leftEndpoint;

  double delta= (rightEndpoint-leftEndpoint)/numPoints;
//  std::cout << "delta: " << delta;
  List<double> XValues, YValues;
  XValues.SetExpectedSize(numPoints+5);
  YValues.SetExpectedSize(numPoints+5);
  int pointsCounter=0;
  for (double currentX=xInitial; currentX>leftEndpoint-delta; currentX-=delta)
  { XValues.AddOnTop(currentX);
    pointsCounter++;
    if (pointsCounter>numPoints)
      break; //<-in case floating point arithmetic is misbehaving
  }
  XValues.ReverseOrderElements();
  int indexXinitial=XValues.size-1;
  pointsCounter=0;
  for (double currentX=xInitial+delta; currentX<rightEndpoint+delta; currentX+=delta)
  { XValues.AddOnTop(currentX);
    pointsCounter++;
    if (pointsCounter>numPoints)
      break; //<-in case floating point arithmetic is misbehaving
  }
  YValues.initFillInObject(XValues.size, 0);
  YValues[indexXinitial]=yInitial;
  Expression functionE=input[1];
  double currentYprimeApprox=0;
//  std::cout << "Xvalues: ";
//  for (int i=0; i<XValues.size; i++)
//    std::cout << XValues[i] << ", ";
  int lastGoodXIndex=indexXinitial;
  int firstGoodXIndex=indexXinitial;
  double maxYallowed=(rightEndpoint-leftEndpoint)*2+yInitial;
  double minYallowed=-(rightEndpoint-leftEndpoint)*2+yInitial;
  //std::cout << "maxYallowed=" << maxYallowed;
  //std::cout << "minYallowed=" << minYallowed;
  for (int direction=-1; direction<2; direction+=2)
    for (int i=indexXinitial+direction; i>=0 && i<XValues.size; i+=direction)
    { knownValues[knownValues.size-2]=XValues[i];
      bool isGood=true;
      for (int counter=0; ; counter++)
      { knownValues[knownValues.size-1]=YValues[i];
        if (!functionE.EvaluatesToRealDoubleUnderSubstitutions(knownConsts, knownValues, &currentYprimeApprox))
        { theCommands.Comments << "Failed to evaluate yPrime approximation at x=" << XValues[i];
          return false;
        }
        double adjustment=delta*direction*currentYprimeApprox;
        double ynew=YValues[i-direction]+adjustment;
        double difference= ynew-YValues[i];
        YValues[i]=ynew;
        if (counter>20) //<- we run Euler algorithm at least 20 times.
          if (difference>-0.01 && difference<0.01)//<-if changes are smaller than 0.01 we assume success.
            break;
        if (counter>200) //<- we ran Euler algorithm 100 times, but the difference is still greater than 0.01. Something is wrong, we abort
        { theCommands.Comments << "Euler method: no convergence. At x=" << XValues[i] << ", y="
          << YValues[i] << ", the change in y is: " << difference << " after " << counter << " iterations. ";
          if (counter>205)
          { isGood=false;
            break;
          }
        }
      }
      if (isGood)
      { isGood= (YValues[i]<=maxYallowed && YValues[i]>=minYallowed);
        if (!isGood)
        { theCommands.Comments << "Euler method: y values outside of the bounding box. At x=" << XValues[i]
          << ", the y value is: " << YValues[i] << ". Max y is  " << maxYallowed << " and min y is " << minYallowed << ". ";
        }
      }
      if (direction==-1)
        firstGoodXIndex=i+1;
      else
        lastGoodXIndex=i-1;
      if (!isGood)
        break;
//      std::cout << "evaluated at " << XValues[i] << " to get " << YValues[i];
    }
//  std::cout << "first good index: " << firstGoodXIndex << " last good index: " << lastGoodXIndex;
  PlotObject thePlot;
  thePlot.xLow=XValues[0];
  thePlot.xHigh=*XValues.LastObject();
  thePlot.yLow=-0.5;
  thePlot.yHigh=0.5;
  thePlot.thePlotElement=input;
  std::stringstream outLatex, outHtml;
  outLatex << "\n\n%calculator input:" << input.ToString() << "\n\n"
  << "\\psline[linecolor=\\psColorGraph]";
  outHtml << "\n<br>\n%calculator input:" << input.ToString() << "\n<br>\n"
  << "\\psline[linecolor=\\psColorGraph]";
  for(int i=firstGoodXIndex; i<=lastGoodXIndex; i++)
  { outLatex << std::fixed << "(" << XValues[i] << ", " << YValues[i] << ")";
    outHtml << std::fixed << "(" << XValues[i] << ", " << YValues[i] << ")";
    thePlot.yLow=MathRoutines::Minimum(YValues[i], thePlot.yLow);
    thePlot.yHigh=MathRoutines::Maximum(YValues[i], thePlot.yHigh);
  }
//  std::cout << "final yhigh: " << thePlot.yHigh << ", ylow: " << thePlot.yLow;
  thePlot.thePlotString=outLatex.str();
  thePlot.thePlotStringWithHtml=outHtml.str();
  return output.AssignValue(thePlot, theCommands);
}

bool CalculatorFunctionsGeneral::innerPlot2D(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlot2D");
  //std::cout << input.ToString();
  if (input.children.size<4)
    return output.SetError("Plotting coordinates takes at least three arguments: function, lower and upper bound. ", theCommands);
  if (input.HasBoundVariables())
    return false;
  const Expression& lowerE=input[2];
  const Expression& upperE=input[3];
  Expression functionE;
  double upperBound, lowerBound;
  if (!lowerE.EvaluatesToRealDouble(&lowerBound) || !upperE.EvaluatesToRealDouble(&upperBound))
    return output.SetError("Failed to convert upper and lower bounds of drawing function to rational numbers.", theCommands);
  if (upperBound<lowerBound)
    MathRoutines::swap(upperBound, lowerBound);
  if (!theCommands.CallCalculatorFunction(theCommands.innerSuffixNotationForPostScript, input[1], functionE))
  { std::stringstream out;
    out << "Failed to convert expression " << input[1].ToString() << " to postfix notation. ";
    return output.SetError(out.str(), theCommands);
  }
  Plot thePlot;
  double yLow, yHigh;
  if (!input[1].EvaluatesToRealDoubleInRange("x", lowerBound, upperBound, 500, &yLow, &yHigh))
  { theCommands.Comments << "<hr>I failed to evaluate the input function, something is wrong!";
    return false;
  }
  thePlot.AddFunctionPlotOnTop(input[1], functionE.GetValue<std::string>(), lowerBound, upperBound, yLow, yHigh);
  return output.AssignValue(thePlot, theCommands);
}

bool CalculatorFunctionsGeneral::innerPlot2DWithBars(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlot2DWithBars");
  //std::cout << input.ToString();
  if (input.children.size<6)
    return output.SetError("Plotting coordinates takes the following arguments: lower function, upper function, lower and upper bound, delta x. ", theCommands);
  Expression lowerEplot=input, upperEplot=input;
  lowerEplot.children.RemoveIndexShiftDown(2);
  upperEplot.children.RemoveIndexShiftDown(1);
  Plot outputPlot;
  bool tempB=CalculatorFunctionsGeneral::innerPlot2D(theCommands, lowerEplot, output);
  if (!tempB || !output.IsOfType<Plot>(&outputPlot))
  { theCommands.Comments << "<hr>Failed to get a plot from " << lowerEplot.ToString() << ", not proceding with bar plot.";
    return false;
  }
  tempB=CalculatorFunctionsGeneral::innerPlot2D(theCommands, upperEplot, output);
  if (!tempB || !output.IsOfType<Plot>())
  { theCommands.Comments << "<hr>Failed to get a plot from " << upperEplot.ToString() << ", not proceding with bar plot.";
    return false;
  }
  outputPlot+=output.GetValue<Plot>();
  const Expression& lowerFunctionE=input[1];
  const Expression& upperFunctionE=input[2];
  const Expression& lowerE=input[3];
  const Expression& upperE=input[4];
  const Expression& deltaE=input[5];
  double theDeltaNoSign, theDeltaWithSign;
  if (!deltaE.EvaluatesToRealDouble(&theDeltaWithSign))
    return false;
  theDeltaNoSign=theDeltaWithSign;
  if (theDeltaNoSign<0)
    theDeltaNoSign*=-1;
  if (theDeltaNoSign==0)
    theDeltaNoSign=1;
  double upperBound, lowerBound;
  if (!lowerE.EvaluatesToRealDouble(&lowerBound) || !upperE.EvaluatesToRealDouble(&upperBound))
    return output.SetError("Failed to convert upper and lower bounds of drawing function to rational numbers.", theCommands);
  if (upperBound<lowerBound)
    MathRoutines::swap(upperBound, lowerBound);
  Expression xValueE, xExpression, theFunValueEnonEvaluated, theFunValueFinal;
  xExpression.MakeAtom(theCommands.AddOperationNoRepetitionOrReturnIndexFirst("x"), theCommands);
  List<double> xValues;
  List<double> fValuesLower;
  List<double> fValuesUpper;
  if (theDeltaNoSign==0)
    return output.SetError("Delta equal to zero is not allowed", theCommands);
  if ((upperBound-lowerBound)/theDeltaNoSign>10000)
    return output.SetError("More than 10000 intervals needed for the plot, this is not allowed.", theCommands);
  List<Rational> rValues;
  Rational lowerBoundRat, upperBoundRat, deltaRat;
  if (lowerE.IsOfType<Rational>(&lowerBoundRat) && upperE.IsOfType<Rational>(&upperBoundRat) && deltaE.IsOfType<Rational>(&deltaRat))
  { if (upperBoundRat<lowerBoundRat)
      MathRoutines::swap(upperBoundRat, lowerBoundRat);
    if (deltaRat<0)
      deltaRat*=-1;
    if (deltaRat==0)
      return output.SetError("Delta equal to zero is not allowed", theCommands);
    for (Rational i=lowerBoundRat; i<=upperBoundRat; i+=deltaRat)
      rValues.AddOnTop(i);
  }
  double yMax=-0.5, yMin=0.5;
  for (double i=lowerBound; i<=upperBound; i+=theDeltaNoSign)
    for (int j=0; j<2; j++)
    { if (theDeltaWithSign<0 && i==lowerBound)
        continue;
      if (theDeltaWithSign>0 && i==upperBound)
        continue;
      xValueE.AssignValue(i, theCommands);
      theFunValueEnonEvaluated=(j==0) ? lowerFunctionE : upperFunctionE;
      theFunValueEnonEvaluated.Substitute(xExpression, xValueE);
  //    std::cout << "<br>substitution result:" << tempE2.ToString();
      BoundVariablesSubstitution tempSub;
      bool tempB;
      if (!theCommands.EvaluateExpression(theFunValueEnonEvaluated, theFunValueFinal, tempSub, tempB))
        return false;
  //    std::cout << "and after evaluation: " << theFunValueFinal.ToString();
      double finalResultDouble;
      if (!theFunValueFinal.EvaluatesToRealDouble(&finalResultDouble))
      { theCommands.Comments << "<hr>Failed to evaluate your function at point " << i << ", instead " << "I evaluated to " << theFunValueFinal.ToString();
        return false;
      }
      if (j==0)
      { xValues.AddOnTop(i);
        fValuesLower.AddOnTop(finalResultDouble);
      } else
        fValuesUpper.AddOnTop(finalResultDouble);
      yMin=MathRoutines::Minimum(yMin, finalResultDouble);
      yMax=MathRoutines::Maximum(yMax, finalResultDouble);
    }
  std::stringstream outTex, outHtml;
  for (int k=0; k<2; k++)
    for (int i=0; i<xValues.size; i++)
    { //bool includePsLine=false;
      bool useNegativePattern=(fValuesLower[i]>fValuesUpper[i]);
      if (k==0 && useNegativePattern)
      { outTex << "\\psline*[linecolor=\\psColorNegativeAreaUnderGraph, linewidth=0.1pt]";
        outHtml << "<br>\\psline*[linecolor=\\psColorNegativeAreaUnderGraph, linewidth=0.1pt]";
      }
      if (k==0 && !useNegativePattern)
      { outTex << "\\psline*[linecolor=\\psColorAreaUnderGraph, linewidth=0.1pt]";
        outHtml << "<br>\\psline*[linecolor=\\psColorAreaUnderGraph, linewidth=0.1pt]";
      }
      if (k>0 && useNegativePattern)
      { outTex << "\\psline[linecolor=brown, linewidth=0.1pt]";
        outHtml << "<br>\\psline[linecolor=brown, linewidth=0.1pt]";
      }
      if (k>0 && !useNegativePattern)
      { outTex << "\\psline[linecolor=blue, linewidth=0.1pt]";
        outHtml << "<br>\\psline[linecolor=blue, linewidth=0.1pt]";
      }
      outTex << "(" << std::fixed << xValues[i] << ", " << std::fixed << fValuesLower[i] << ")(" << std::fixed << xValues[i]  << ", "
      << std::fixed << fValuesUpper[i] << ")" << "(" << std::fixed << xValues[i]+theDeltaWithSign << ", " << std::fixed
      << fValuesUpper[i] << ")(" << std::fixed << xValues[i]+theDeltaWithSign << ", " << std::fixed << fValuesLower[i] << ")"
      << "(" << std::fixed << xValues[i] << ", " << std::fixed << fValuesLower[i] << ")";
      outHtml << "(" << std::fixed << xValues[i] << ", " << std::fixed << fValuesLower[i] << ")(" << std::fixed << xValues[i] << ", "
      << std::fixed << fValuesUpper[i] << ")" << "(" << std::fixed << xValues[i]+theDeltaWithSign << ", " << std::fixed
      << fValuesUpper[i] << ")(" << std::fixed << xValues[i]+theDeltaWithSign << ", " << std::fixed << fValuesLower[i] << ")"
      << "(" << std::fixed << xValues[i] << ", " << std::fixed << fValuesLower[i] << ")";
    }
  outHtml << "<br>";
  for (int i=0; i<rValues.size; i++)
  { std::stringstream tempStream;
    tempStream << "\\rput[t](" << std::fixed << rValues[i].DoubleValue() << ",-0.03)" << "{$";
    if (rValues[i].IsInteger())
      tempStream << rValues[i].ToString();
    else
      tempStream << "\\frac{" << rValues[i].GetNumerator().ToString() << "}" << "{" << rValues[i].GetDenominator().ToString() << "}";
    tempStream << "$}";
    outHtml << tempStream.str();
    outTex << tempStream.str();
  }
  outHtml << "<br>";
  PlotObject thePlot;
  thePlot.thePlotString=outTex.str();
  thePlot.thePlotStringWithHtml=outHtml.str();
  thePlot.xLow=lowerBound;
  thePlot.xHigh=upperBound;
  thePlot.yLow=yMin;
  thePlot.yHigh=yMax;

  thePlot.thePlotElement=input[1];
  Plot plotFinal;
  plotFinal+=thePlot;
  plotFinal+=outputPlot;
  return output.AssignValue(plotFinal, theCommands);
}

bool CalculatorFunctionsGeneral::innerPlotPolarRfunctionTheta(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotPolarRfunctionTheta");
  if (!input.IsListNElements(4))
    return output.SetError("Drawing polar coordinates takes three arguments: function, lower angle bound and upper angle bound. ", theCommands);
  const Expression& lowerE=input[2];
  const Expression& upperE=input[3];
  Expression functionE;
  double lowerBound=0, upperBound=0;
  PlotObject thePlotCartesian;
  if (!lowerE.EvaluatesToRealDouble(&lowerBound) || !upperE.EvaluatesToRealDouble(&upperBound))
    return output.SetError("Failed to convert upper and lower bounds of drawing function to rational numbers.", theCommands);
  if (upperBound<lowerBound)
    MathRoutines::swap(upperBound, lowerBound);
  if (!theCommands.CallCalculatorFunction(theCommands.innerSuffixNotationForPostScript, input[1], functionE))
    return false;
  Expression xCoordE, yCoordE, costE(theCommands), sintE(theCommands);
  costE.AddChildAtomOnTop(theCommands.opCos());
  costE.AddChildAtomOnTop("t");
  sintE.AddChildAtomOnTop(theCommands.opSin());
  sintE.AddChildAtomOnTop("t");
  xCoordE.MakeProducT(theCommands, input[1], costE);
  yCoordE.MakeProducT(theCommands, input[1], sintE);
  if (!xCoordE.EvaluatesToRealDoubleInRange("t", lowerBound, upperBound, 500, &thePlotCartesian.xLow, &thePlotCartesian.xHigh))
    return false;
  if (!yCoordE.EvaluatesToRealDoubleInRange("t", lowerBound, upperBound, 500, &thePlotCartesian.yLow, &thePlotCartesian.yHigh))
    return false;
  std::stringstream outCartesianLatex, outCartesianHtml;
  outCartesianLatex << "%Calculator command: " << input.ToString() << "\n";
  outCartesianHtml << "%Calculator command: " << input.ToString() << "\n<br>\n";

  outCartesianLatex << "\\parametricplot[linecolor=\\psColorGraph, plotpoints=1000, algebraic=false]{" << lowerBound << "}{"
  << upperBound << "}{";
  outCartesianHtml << "\\parametricplot[linecolor=\\psColorGraph, plotpoints=1000, algebraic=false]{" << lowerBound << "}{"
  << upperBound << "}{";
  std::string funString=functionE.GetValue<std::string>();
  outCartesianLatex << funString << " t 57.29578 mul cos mul " << funString << " t 57.29578 mul sin mul " << "}";
  outCartesianHtml << funString << " t 57.29578 mul cos mul " << funString << " t 57.29578 mul sin mul " << "}";
  thePlotCartesian.thePlotString=outCartesianLatex.str();
  thePlotCartesian.thePlotStringWithHtml=outCartesianHtml.str();
  return output.AssignValue(thePlotCartesian, theCommands);
}

bool CalculatorFunctionsGeneral::innerPlotPolarRfunctionThetaExtended(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotPolarRfunctionThetaExtended");
  if (!input.IsListNElements(4))
    return output.SetError("Drawing polar coordinates takes three arguments: function, lower angle bound and upper angle bound. ", theCommands);
  const Expression& lowerE=input[2];
  const Expression& upperE=input[3];
  Expression functionE;
  PlotObject thePlotPolar;
  if (!lowerE.EvaluatesToRealDouble(&thePlotPolar.xLow) || !upperE.EvaluatesToRealDouble(&thePlotPolar.xHigh))
    return output.SetError("Failed to convert upper and lower bounds of drawing function to rational numbers.", theCommands);
  if (thePlotPolar.xHigh<thePlotPolar.xLow)
    MathRoutines::swap(thePlotPolar.xHigh, thePlotPolar.xLow);
  if (!theCommands.CallCalculatorFunction(theCommands.innerSuffixNotationForPostScript, input[1], functionE))
    return false;
  Expression cartesianPlotE, polarPlotE;
  if (!theCommands.CallCalculatorFunction(CalculatorFunctionsGeneral::innerPlotPolarRfunctionTheta, input, cartesianPlotE))
    return false;
  std::stringstream  outPolarLatex, outPolarHtml;
  outPolarLatex << "%Calculator command: " << input.ToString() << "\n";
  outPolarHtml << "%Calculator command: " << input.ToString() << "\n<br>\n";
  outPolarLatex << "\\parametricplot[linecolor=\\psColorGraph, plotpoints=1000, algebraic=false]{" << thePlotPolar.xLow << "}{"
  << thePlotPolar.xHigh << "}{";
  outPolarHtml << "\\parametricplot[linecolor=\\psColorGraph, plotpoints=1000, algebraic=false]{" << thePlotPolar.xLow << "}{"
  << thePlotPolar.xHigh << "}{";
  std::string funString=functionE.GetValue<std::string>();
  outPolarLatex << "t " << funString << "}";
  outPolarHtml << "t " << funString << "}";
  thePlotPolar.thePlotString=outPolarLatex.str();
  thePlotPolar.thePlotStringWithHtml=outPolarHtml.str();
  polarPlotE.AssignValue(thePlotPolar, theCommands);
  return output.MakeXOX(theCommands, theCommands.opSequence(), cartesianPlotE, polarPlotE);
}

bool CalculatorFunctionsGeneral::innerPlotParametricCurve(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotParametricCurve");
  if (input.children.size<5)
  { theCommands.Comments
    << "Parametric curve plots take 4+ arguments, first arguments stand for the functions in the various coordinates, "
    << " the last two arguments stands for the variable range.";
    return false;
  }
  if (input.HasBoundVariables())
    return false;
  if (input.children.size!=5)
  { theCommands.Comments << "At the moment I can draw on curves sitting in 2d space. ";
    return false;
  }
  List<Expression> theConvertedExpressions;
  theConvertedExpressions.SetSize(input.children.size-3);
  for (int i=1; i<input.children.size-2; i++)
    if (!Calculator::innerSuffixNotationForPostScript(theCommands, input[i], theConvertedExpressions[i-1]))
    { theCommands.Comments << "Failed to extract suffix notation from argument " << input[i].ToString();
      return false;
    }
  double leftEndPoint, rightEndPoint;
  if (!input[input.children.size-2].EvaluatesToRealDouble(&leftEndPoint) ||
      !input[input.children.size-1].EvaluatesToRealDouble(&rightEndPoint))
  { theCommands.Comments << "Failed to convert " << input[input.children.size-2].ToString() << " and "
    << input[input.children.size-1].ToString() << " to left and right endpoint of parameter interval. ";
    return false;
  }
  std::stringstream outLatex, outHtml;
  outLatex << "\\parametricplot[linecolor=\\psColorGraph, plotpoints=1000]{" << leftEndPoint << "}{" << rightEndPoint << "}{"
  << theConvertedExpressions[0].GetValue<std::string>() << theConvertedExpressions[1].GetValue<std::string>() << "}";
  outHtml << "<br>%Calculator input: " << input.ToString()
  << "<br>\\parametricplot[linecolor=\\psColorGraph, plotpoints=1000]{" << leftEndPoint << "}{" << rightEndPoint << "}{"
  << theConvertedExpressions[0].GetValue<std::string>() << theConvertedExpressions[1].GetValue<std::string>() << "}";
  PlotObject thePlot;
  if (!input[1].EvaluatesToRealDoubleInRange("t", leftEndPoint, rightEndPoint, 1000, &thePlot.xLow, &thePlot.xHigh))
  { theCommands.Comments << "<hr>Failed to evaluate curve function. ";
    return false;
  }
  if (!input[2].EvaluatesToRealDoubleInRange("t", leftEndPoint, rightEndPoint, 1000, &thePlot.yLow, &thePlot.yHigh))
  { theCommands.Comments << "<hr>Failed to evaluate curve function. ";
    return false;
  }
  thePlot.thePlotElement=input;
  thePlot.thePlotString=outLatex.str();
  thePlot.thePlotStringWithHtml=outHtml.str();
  return output.AssignValue(thePlot, theCommands);
}

bool CalculatorFunctionsGeneral::innerPlotConeUsualProjection(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotConeUsualProjection");
  if (input.children.size!=5)
  { theCommands.Comments << "<hr>PlotConeUsualProjection takes 4 arguments. ";
    return false;
  }
  double radius, height, distance, viewPointHeight;
  if (!input[1].EvaluatesToRealDouble(&radius) || !input[2].EvaluatesToRealDouble(&height) || !input[3].EvaluatesToRealDouble(&distance) || !input[4].EvaluatesToRealDouble(&viewPointHeight) )
  { theCommands.Comments << "<hr> failed to extract radius, height, distance, viewpoint height from " << input.ToString();
    return false;
  }
  affineHyperplane<double> projectionPane;
  projectionPane.normal.MakeZero(3);
  projectionPane.normal[1]=-distance;
  projectionPane.normal[2]=-viewPointHeight;
  projectionPane.affinePoint.MakeZero(3);
  projectionPane.affinePoint[1]=distance;
  projectionPane.affinePoint[2]=viewPointHeight;
  Vector<double> pointCircle1, pointCircle2;
  Vector<double> axis1, axis2, axis3;

  pointCircle1.MakeZero(3);
  pointCircle2.MakeZero(3);
  pointCircle1[1]=radius;
  pointCircle2[1]=-radius;
  Vector<double> projection1= projectionPane.ProjectOnMe(pointCircle1);
  Vector<double> projection2= projectionPane.ProjectOnMe(pointCircle2);
  //std::cout << "<br>projection1: " << projection1.ToString();
  //std::cout << "<br>projection2: " << projection2.ToString();
  Vector<double> XdiameterVectorProjection=projection1-projection2;
  double theProjYradius= FloatingPoint::sqrt(XdiameterVectorProjection.ScalarEuclidean(XdiameterVectorProjection))/2;
  double theProjXradius=radius;
  //std::cout << "<br>theProjYradius: " << theProjYradius;
  //std::cout << "<br>theProjXradius: " << theProjXradius;
  std::stringstream out;
  Vector<double> tipOfTheCone, centerOfTheCone;
  centerOfTheCone.MakeZero(3);
  tipOfTheCone.MakeZero(3);
  tipOfTheCone[2]=height;
//  std::cout << "<br>radius, height, distance, view height: " << radius << ", " << height << ", " << distance << ", " << viewPointHeight;
//  std::cout << "<br>projectionPane.ProjectOnMe(tipOfTheCone): " << projectionPane.ProjectOnMe(tipOfTheCone);
  Vector<double> coneHeightVectorProjectedShifted= projectionPane.ProjectOnMe(tipOfTheCone)-projectionPane.ProjectOnMe(centerOfTheCone);
//  std::cout << "<br>coneHeightVectorProjectedShifted: " << coneHeightVectorProjectedShifted.ToString();
  double theConeProjectionHeight=FloatingPoint::sqrt(coneHeightVectorProjectedShifted.ScalarEuclidean(coneHeightVectorProjectedShifted));
//  std::cout << "<br>theConeProjectionHeight: " << theConeProjectionHeight;
  out << "\\psline[linecolor=black](0,0)(0," << MathRoutines::ReducePrecision(theConeProjectionHeight) << ")";
  if (theConeProjectionHeight>theProjYradius)
  { double yCoordPointTangency= theProjYradius*theProjYradius/theConeProjectionHeight;
    double xCoordPointTangency=theProjXradius*FloatingPoint::sqrt(1- yCoordPointTangency/theConeProjectionHeight);
    out << "\\psline[linecolor=\\psColorGraph](" << MathRoutines::ReducePrecision(xCoordPointTangency) << ", "
    << MathRoutines::ReducePrecision(yCoordPointTangency) << ")(0, " << MathRoutines::ReducePrecision(theConeProjectionHeight) << ")";
    out << "\\psline[linecolor=\\psColorGraph](" << MathRoutines::ReducePrecision(-xCoordPointTangency) << ", "
    << MathRoutines::ReducePrecision(yCoordPointTangency) << ")(0, " << MathRoutines::ReducePrecision(theConeProjectionHeight) << ")";
    double theAngleVisibleEnd=MathRoutines::Pi()/2;
    double theAngleVisibleStart=-theAngleVisibleEnd;
    double theAngleHiddenEnd=MathRoutines::Pi()/2;
    if (theProjXradius!=0)
    { theAngleVisibleEnd=FloatingPoint::arctan(yCoordPointTangency*theProjXradius/(xCoordPointTangency*theProjYradius));
      theAngleVisibleStart = -MathRoutines::Pi()-theAngleVisibleEnd;
      theAngleHiddenEnd=MathRoutines::Pi()-theAngleVisibleEnd;
    }
    out << "\\psparametricplot[algebraic,linecolor=\\psColorGraph]{" << MathRoutines::ReducePrecision(theAngleVisibleStart)
    << "}{" << MathRoutines::ReducePrecision(theAngleVisibleEnd) << "}{" << MathRoutines::ReducePrecision(theProjXradius)
    << "*cos(t) |" << MathRoutines::ReducePrecision(theProjYradius) << "*sin(t)}";
    out << "\\psparametricplot[algebraic, linestyle=dashed, linecolor=\\psColorGraph]{" << MathRoutines::ReducePrecision(theAngleVisibleEnd)
    << "}{" << MathRoutines::ReducePrecision(theAngleHiddenEnd) << "}{" << MathRoutines::ReducePrecision(theProjXradius)
    << "*cos(t) |" << MathRoutines::ReducePrecision(theProjYradius) << "*sin(t)}";
  } else
  { theCommands.Comments << "<hr>Cone is not high enough and therefore has no tip. ";
    out << "\\psparametricplot[algebraic,linecolor=\\psColorGraph]{0}{6.283185307}{cos(t)*" << MathRoutines::ReducePrecision(theProjXradius)
    << " |sin(t)*" << MathRoutines::ReducePrecision(theProjYradius) << "}";
  }
  PlotObject thePlot;
  thePlot.xLow=-5;
  thePlot.xHigh=5;
  thePlot.yLow=-5;
  thePlot.yHigh=5;
  thePlot.thePlotElement=input;
  thePlot.thePlotString=out.str();
  thePlot.thePlotStringWithHtml=out.str();
  return output.AssignValue(thePlot, theCommands);
}

bool CalculatorFunctionsGeneral::innerComputePairingTablesAndFKFTsubalgebras(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerComputePairingTablesAndFKFTsubalgebras");
  if (!input.IsOfType<SemisimpleSubalgebras>())
  { theCommands.Comments << "<hr>Input of ComputeFKFT must be of type semisimple subalgebras. ";
    return false;
  }
  SemisimpleSubalgebras& theSAs=input.GetValueNonConst<SemisimpleSubalgebras>();
  theSAs.flagComputePairingTable=true;
  theSAs.flagComputeNilradicals=true;
  theSAs.ComputePairingTablesAndFKFTtypes();
  output=input;
  std::fstream theFile;
  std::string theFileName;
  theFileName=theCommands.PhysicalPathOutputFolder + "FKFTcomputation.html";
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML=true;
  tempFormat.flagUseLatex=true;
  tempFormat.flagUseHTML=true;
  tempFormat.flagCandidateSubalgebraShortReportOnly=false;
  XML::OpenFileCreateIfNotPresent(theFile, theFileName, false, true, false);
  theFile << theSAs.ToString(&tempFormat);
  std::stringstream out;
  out << "<a href=\"" << theCommands.DisplayPathOutputFolder << "FKFTcomputation.html\">FKFTcomputation.html</a>";
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerGetCentralizerChainsSemisimpleSubalgebras(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGetCentralizerChainsSemisimpleSubalgebras");
  if (!input.IsOfType<SemisimpleSubalgebras>())
  { theCommands.Comments << "<hr>Input of GetCentralizerChains must be of type semisimple subalgebras. ";
    return false;
  }
  SemisimpleSubalgebras& theSAs=input.GetValueNonConst<SemisimpleSubalgebras>();
  List<List<int> > theChains;
  std::stringstream out;
  theSAs.GetCentralizerChains(theChains);
  Expression currentChainE;
  out << theChains.size << " chains total. <br>";
  for (int i=0; i<theChains.size; i++)
  { out << "<br>Chain " << i+1 << ": Load{}(Serialization{}(LoadSemisimpleSubalgebras, " << theSAs.owneR->theWeyl.theDynkinType.ToString() << ", (";
    for (int j=0; j<theChains[i].size; j++)
    { CalculatorSerialization::innerStoreCandidateSA(theCommands, theSAs.theSubalgebraCandidates[theChains[i][j]], currentChainE);
      out << currentChainE.ToString();
      if (j!=theChains[i].size-1)
        out << ", ";
    }
    out << ") ) )";
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerEvaluateToDouble(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Expression::innerEvaluateToDouble");
  double theValue=0;
  if (!input.EvaluatesToRealDouble(&theValue))
    return false;
  return output.AssignValue(theValue, theCommands);
}

bool CalculatorFunctionsGeneral::innerTestMathMouseHover(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerTestMathMouseHover");
  std::stringstream out;
  out << "Hover mouse to render: " << CGI::GetMathMouseHover(input.ToString());
//  out << "<br>Directly rendered: " << CGI::GetMathSpanPure(input.ToString());
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerCanBeExtendedParabolicallyTo(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCanBeExtendedParabolicallyTo");
  if (!input.IsListNElements(3))
    return false;
  DynkinType smallType, targetType;
  if (!CalculatorSerialization::DeSerializeMonCollection(theCommands, input[1], smallType) || !CalculatorSerialization::DeSerializeMonCollection(theCommands, input[2], targetType))
  { theCommands.Comments << "Failed to convert arguments of " << input.ToString() << " to two DynkinType's.";
    return false;
  }
  return output.AssignValue((int)smallType.CanBeExtendedParabolicallyTo(targetType), theCommands);
}

bool CalculatorFunctionsGeneral::innerGetSymmetricCartan(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGetSymmetricCartan");
  DynkinType theType;
  if (!CalculatorSerialization::DeSerializeMonCollection(theCommands, input, theType))
  { theCommands.Comments << "Failed to convert " << input.ToString() << " to DynkinType.";
    return false;
  }
  std::stringstream out;
  Matrix<Rational> outputMat, outputCoMat;
  theType.GetCartanSymmetric(outputMat);
  theType.GetCoCartanSymmetric(outputCoMat);
  out << "Symmetric Cartan matrix: " << CGI::GetMathMouseHover(outputMat.ToStringLatex(),10000)
  << "<br>Symmetric Cartan co-matrix: " << CGI::GetMathMouseHover(outputCoMat.ToStringLatex(), 10000);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerEmbedSSalgInSSalg(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerEmbedSSalgInSSalg");
  if (!input.IsListNElements(3))
    return output.SetError("I expect two arguments - the two semisimple subalgebras.", theCommands);
  const Expression& EsmallSA=input[1];
  const Expression& ElargeSA=input[2];

  SemisimpleLieAlgebra* theSmallSapointer=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorSerialization::innerSSLieAlgebra, EsmallSA, theSmallSapointer))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  SemisimpleLieAlgebra* thelargeSapointer=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorSerialization::innerSSLieAlgebra, ElargeSA, thelargeSapointer))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  SemisimpleLieAlgebra& ownerSS=*thelargeSapointer;
  std::stringstream out;
  if (ownerSS.GetRank()>6)
  { out << "<b>This code is completely experimental and has been set to run up to rank 4. As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
    return output.AssignValue(out.str(), theCommands);
  } else
    out << "<b>This code is completely experimental. Use the following printouts on your own risk</b>";
  SemisimpleSubalgebras tempSSsas
  (ownerSS, &theCommands.theObjectContainer.theAlgebraicClosure, &theCommands.theObjectContainer.theLieAlgebras,
   &theCommands.theObjectContainer.theSltwoSAs, theCommands.theGlobalVariableS);
  SemisimpleSubalgebras& theSSsubalgebras=
  theCommands.theObjectContainer.theSSsubalgebras[theCommands.theObjectContainer.theSSsubalgebras.AddNoRepetitionOrReturnIndexFirst(tempSSsas)];
  out << "Attempting to embed " << theSmallSapointer->theWeyl.theDynkinType.ToString() << " in " << ownerSS.GetLieAlgebraName();
  theSSsubalgebras.FindTheSSSubalgebras(ownerSS, &theSmallSapointer->theWeyl.theDynkinType);
  return output.AssignValue(theSSsubalgebras, theCommands);
}

bool CalculatorFunctionsGeneral::innerWeylDimFormula(Calculator& theCommands, const Expression& input, Expression& output)
{ RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(3))
    return output.SetError("This function takes 2 arguments", theCommands);
  SemisimpleLieAlgebra* theSSowner;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorSerialization::innerSSLieAlgebra, input[1], theSSowner))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  Vector<RationalFunctionOld> theWeight;
  Expression newContext(theCommands);
//  std::cout << "<br>input[2] is: " << input[2].ToString();
  if (!theCommands.GetVectoR<RationalFunctionOld>(input[2], theWeight, &newContext, theSSowner->GetRank(), CalculatorSerialization::innerRationalFunction))
    return output.SetError("Failed to convert the argument of the function to a highest weight vector", theCommands);
  RationalFunctionOld rfOne;
  rfOne.MakeOne(theCommands.theGlobalVariableS);
  Vector<RationalFunctionOld> theWeightInSimpleCoords;
  FormatExpressions theFormat;
  newContext.ContextGetFormatExpressions(theFormat);
  theWeightInSimpleCoords = theSSowner->theWeyl.GetSimpleCoordinatesFromFundamental(theWeight);
  //std::cout << "The fundamental coords: " << theWeight.ToString();
  theCommands.Comments << "<br>Weyl dim formula input: simple coords: " << theWeightInSimpleCoords.ToString(&theFormat) << ", fundamental coords: " << theWeight.ToString(&theFormat);
  RationalFunctionOld tempRF= theSSowner->theWeyl.WeylDimFormulaSimpleCoords(theWeightInSimpleCoords);
  //std::cout << "<br>The result: " << tempRF.ToString();
  return output.AssignValueWithContext(tempRF, newContext, theCommands);
}

bool CalculatorFunctionsGeneral::innerDecomposeFDPartGeneralizedVermaModuleOverLeviPart(Calculator& theCommands, const Expression& input, Expression& output)
{ RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(5))
    return output.SetError("Function fDecomposeFDPartGeneralizedVermaModuleOverLeviPart expects 4 arguments.", theCommands);
  const Expression& typeNode=input[1];
  const Expression& weightNode=input[2];
  const Expression& inducingParNode=input[3];
  const Expression& splittingParNode=input[4];
  SemisimpleLieAlgebra* ownerSSPointer=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorSerialization::innerSSLieAlgebra, typeNode, ownerSSPointer))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  Vector<RationalFunctionOld> theWeightFundCoords;
  Vector<Rational> inducingParSel, splittingParSel;
  SemisimpleLieAlgebra& ownerSS=*ownerSSPointer;
  WeylGroup& theWeyl=ownerSS.theWeyl;
  int theDim=ownerSS.GetRank();
  Expression finalContext;
  if (!theCommands.GetVectoR<RationalFunctionOld>(weightNode, theWeightFundCoords, &finalContext, theDim, CalculatorSerialization::innerRationalFunction))
    return output.SetError("Failed to extract highest weight from the second argument.", theCommands);
  if (!theCommands.GetVectoR<Rational>(inducingParNode, inducingParSel, &finalContext, theDim, 0))
    return output.SetError("Failed to extract parabolic selection from the third argument", theCommands);
  if (!theCommands.GetVectoR<Rational>(splittingParNode, splittingParSel, &finalContext, theDim, 0))
    return output.SetError("Failed to extract parabolic selection from the fourth argument", theCommands);
  theCommands.Comments << "Your input weight in fundamental coordinates: " << theWeightFundCoords.ToString();
  theCommands.Comments << "<br>Your input weight in simple coordinates: " << theWeyl.GetSimpleCoordinatesFromFundamental(theWeightFundCoords).ToString()
  << "<br>Your inducing parabolic subalgebra: " << inducingParSel.ToString() << "." <<"<br>The parabolic subalgebra I should split over: " << splittingParSel.ToString() << ".";
  ModuleSSalgebra<RationalFunctionOld> theMod;
  Selection selInducing= inducingParSel;
  Selection selSplittingParSel=splittingParSel;
  theMod.MakeFromHW(ownerSS, theWeightFundCoords, selInducing, *theCommands.theGlobalVariableS, 1, 0, 0, false);
  std::string report;
  theMod.SplitOverLevi(&report, selSplittingParSel, *theCommands.theGlobalVariableS, 1, 0);
  return output.AssignValue(report, theCommands);
}

bool CalculatorFunctionsGeneral::innerSplitFDpartB3overG2Init(Calculator& theCommands, const Expression& input, Expression& output, branchingData& theG2B3Data, Expression& outputContext)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSplitFDpartB3overG2Init");
  if (!input.IsListNElements(4))
    return output.SetError("Splitting the f.d. part of a B_3-representation over G_2 requires 3 arguments", theCommands);
  //std::cout << input.ToString();
  if (!theCommands.GetVectorFromFunctionArguments<RationalFunctionOld>(input, theG2B3Data.theWeightFundCoords, &outputContext, 3, CalculatorSerialization::innerRationalFunction))
    output.SetError("Failed to extract highest weight in fundamental coordinates. ", theCommands);
  theCommands.MakeHmmG2InB3(theG2B3Data.theHmm);
  theG2B3Data.selInducing.init(3);
  for (int i=0; i<theG2B3Data.theWeightFundCoords.size; i++)
    if (!theG2B3Data.theWeightFundCoords[i].IsSmallInteger())
      theG2B3Data.selInducing.AddSelectionAppendNewIndex(i);
  theG2B3Data.initAssumingParSelAndHmmInittedPart1NoSubgroups(*theCommands.theGlobalVariableS);
  return true;
}

bool CalculatorFunctionsGeneral::innerParabolicWeylGroups(Calculator& theCommands, const Expression& input, Expression& output)
{ Selection selectionParSel;
  SemisimpleLieAlgebra* theSSPointer;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorSerialization::innerSSLieAlgebra, input, theSSPointer))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  SemisimpleLieAlgebra& theSSalgebra=*theSSPointer;
  int numCycles=MathRoutines::TwoToTheNth(selectionParSel.MaxSize);
  SubgroupWeylGroupOLD theSubgroup;
  std::stringstream out;
  for (int i=0; i<numCycles; i++, selectionParSel.incrementSelection())
  { theSubgroup.MakeParabolicFromSelectionSimpleRoots(theSSalgebra.theWeyl, selectionParSel, *theCommands.theGlobalVariableS, 2000);
    out << "<hr>" << CGI::GetMathSpanPure(theSubgroup.ToString());
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerParabolicWeylGroupsBruhatGraph(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerParabolicWeylGroupsBruhatGraph");
  RecursionDepthCounter theRecursion(&theCommands.RecursionDeptH);
  Selection parabolicSel;
  Vector<RationalFunctionOld> theHWfundcoords, tempRoot, theHWsimplecoords;
  Expression hwContext(theCommands);
  SemisimpleLieAlgebra* theSSalgPointer;
  if(!theCommands.GetTypeHighestWeightParabolic(theCommands, input, output, theHWfundcoords, parabolicSel, hwContext, theSSalgPointer, CalculatorSerialization::innerRationalFunction))
    return output.SetError("Failed to extract highest weight vector data", theCommands);
  else
    if (output.IsError())
      return true;
  SemisimpleLieAlgebra& theSSalgebra=*theSSalgPointer;

  WeylGroup& theAmbientWeyl=theSSalgebra.theWeyl;
  SubgroupWeylGroupOLD theSubgroup;
  std::stringstream out;
  std::fstream outputFile, outputFile2;
  std::string fileName, filename2;
  fileName=theCommands.PhysicalNameDefaultOutput+"1";
  filename2=theCommands.PhysicalNameDefaultOutput+"2";
  XML::OpenFileCreateIfNotPresent(outputFile, fileName+".tex", false, true, false);
  XML::OpenFileCreateIfNotPresent(outputFile2, filename2+".tex", false, true, false);
  if (!theSubgroup.MakeParabolicFromSelectionSimpleRoots(theAmbientWeyl, parabolicSel, *theCommands.theGlobalVariableS, 500))
    return output.SetError("<br><br>Failed to generate Weyl subgroup, 500 elements is the limit", theCommands);
  theSubgroup.FindQuotientRepresentatives(2000);
  out << "<br>Number elements of the coset: " << theSubgroup.RepresentativesQuotientAmbientOrder.size;
  out << "<br>Number of elements of the Weyl group of the Levi part: " << theSubgroup.size;
  out << "<br>Number of elements of the ambient Weyl: " << theSubgroup.AmbientWeyl.theElements.size;
  outputFile << "\\documentclass{article}\\usepackage[all,cmtip]{xy}\\begin{document}\n";
  outputFile2 << "\\documentclass{article}\\usepackage[all,cmtip]{xy}\\begin{document}\n";
  FormatExpressions theFormat;
  hwContext.ContextGetFormatExpressions(theFormat);
  if (theSubgroup.size>498)
  { if (theSubgroup.AmbientWeyl.GetGroupSizeByFormula('E', 6) <= theSubgroup.AmbientWeyl.GetGroupSizeByFormula())
      out << "Even I can't handle the truth, when it is so large<br>";
    else
      out << "LaTeX can't handle handle the truth, when it is so large. <br>";
  } else
  { bool useJavascript=theSubgroup.size<100;
    outputFile << "\\[" << theSubgroup.ElementToStringBruhatGraph() << "\\]";
    outputFile << "\n\\end{document}";
    outputFile2 << "\\[" << theSubgroup.ElementToStringCosetGraph() << "\\]";
    outputFile2 << "\n\\end{document}";
    out << "<hr>"
    << " The Hasse graph of the Weyl group of the Levi part follows. <a href=\""
    << theCommands.DisplayNameDefaultOutput << "1.tex\"> "
    << theCommands.DisplayNameDefaultOutput << "1.tex</a>";
    out << ", <iframe src=\"" << theCommands.DisplayNameDefaultOutput
    << "1.png\" width=\"800\" height=\"600\">"
    << theCommands.DisplayNameDefaultOutput << "1.png</iframe>";
    out << "<hr> The coset graph of the Weyl group of the Levi part follows. The cosets are right, i.e. a coset "
    << " of the subgroup X is written in the form Xw, where w is one of the elements below. "
    << "<a href=\""
    << theCommands.DisplayNameDefaultOutput
    << "2.tex\"> " <<  theCommands.DisplayNameDefaultOutput << "2.tex</a>";
    out << ", <iframe src=\"" << theCommands.DisplayNameDefaultOutput
    << "2.png\" width=\"800\" height=\"600\"> "
    << theCommands.DisplayNameDefaultOutput << "2.png</iframe>";
    out <<"<b>The .png file might be bad if LaTeX crashed while trying to process it; "
    << "please check whether the .tex corresponds to the .png.</b>";
    out << "<hr>Additional printout follows.<br> ";
    out << "<br>Representatives of the coset follow. Below them you can find the elements of the subgroup. <br>";
    out << "<table><tr><td>Element</td><td>weight simple coords</td><td>weight fund. coords</td></tr>";
    theFormat.fundamentalWeightLetter="\\omega";
    for (int i=0; i<theSubgroup.RepresentativesQuotientAmbientOrder.size; i++)
    { ElementWeylGroup<WeylGroup>& current=theSubgroup.RepresentativesQuotientAmbientOrder[i];
      out << "<tr><td>"
      << (useJavascript ? CGI::GetMathSpanPure(current.ToString()) : current.ToString())
      << "</td>";
      theHWsimplecoords=
      theSSalgebra.theWeyl.GetSimpleCoordinatesFromFundamental(theHWfundcoords);
      theSSalgebra.theWeyl.ActOnRhoModified
      (theSubgroup.RepresentativesQuotientAmbientOrder[i], theHWsimplecoords);
      out << "<td>"
      << (useJavascript ? CGI::GetMathSpanPure(theHWsimplecoords.ToString(&theFormat))
      : theHWsimplecoords.ToString(&theFormat))
      << "</td>";
      tempRoot = theSSalgebra.theWeyl.GetFundamentalCoordinatesFromSimple(theHWsimplecoords);
      std::string theFundString=
      tempRoot.ToStringLetterFormat(theFormat.fundamentalWeightLetter, &theFormat);
      out << "<td>" << (useJavascript ? CGI::GetMathSpanPure(theFundString): theFundString)
      << "</td>";
      out << "</tr>";
    }
    out << "</table><hr>";
    out << theSubgroup.ToString();
  }
  outputFile.close();
  outputFile2.close();
  std::cout << "<!--";
  std::cout.flush();
  std::string command1="latex  -output-directory=" + theCommands.PhysicalPathOutputFolder + " " + fileName + ".tex";
  std::string command2="dvipng " + fileName + ".dvi -o " + fileName + ".png -T tight";
  std::string command3="latex  -output-directory=" + theCommands.PhysicalPathOutputFolder + " " + filename2 + ".tex";
  std::string command4="dvipng " + filename2 + ".dvi -o " + filename2 + ".png -T tight";
//  std::cout << "<br>" << command1;
//  std::cout << "<br>" << command2;
//  std::cout << "<br>" << command3;
//  std::cout << "<br>" << command4;
  theCommands.theGlobalVariableS->System(command1);
  theCommands.theGlobalVariableS->System(command2);
  theCommands.theGlobalVariableS->System(command3);
  theCommands.theGlobalVariableS->System(command4);
  std::cout << "-->";
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerDeterminant(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDeterminant");
  Matrix<Rational> matRat;
  Matrix<RationalFunctionOld> matRF;
  Expression theContext;
  if (theCommands.GetMatriXFromArguments(input, matRat, 0, 0, 0))
  { if (matRat.NumRows==matRat.NumCols)
    { if (matRat.NumRows>100)
      { theCommands.Comments << "<hr>I have been instructed not to compute determinants of rational matrices larger than 100 x 100 "
        << ", and your matrix had " << matRat.NumRows << " rows. " << "To lift the restriction "
        << "edit function located in file " << __FILE__ << ", line " << __LINE__ << ". ";
        return false;
      }
      //std::cout << " <br> ... and the matRat is: " << matRat.ToString();
      return output.AssignValue(matRat.GetDeterminant(), theCommands);
    } else
      return output.SetError("Requesting to compute determinant of non-square matrix. ", theCommands);
  }
  if (!theCommands.GetMatriXFromArguments(input, matRF, &theContext, -1, CalculatorSerialization::innerRationalFunction))
  { theCommands.Comments << "<hr>I have been instructed to only compute determinants of matrices whose entries are "
    << " rational functions or rationals, and I failed to convert your matrix to either type. "
    << " If this is not how you expect this function to act, correct it: the code is located in  "
    << " file " << __FILE__ << ", line " << __LINE__ << ". ";
    return false;
  }
  if (matRF.NumRows==matRF.NumCols)
  { if (matRF.NumRows>10)
    { theCommands.Comments << "I have been instructed not to compute determinants of matrices of rational functions larger than "
      << " 10 x 10, and your matrix had " << matRF.NumRows << " rows. To lift the restriction edit function located in file "
      << __FILE__ << ", line " << __LINE__ << ". ";
      return false;
    }
    return output.AssignValueWithContext(matRF.GetDeterminant(), theContext, theCommands);
  } else
    return output.SetError("Requesting to comptue determinant of non-square matrix. ", theCommands);
}

bool CalculatorFunctionsGeneral::innerMatrixRationalFunction(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerMatrixRationalFunction");
  Matrix<RationalFunctionOld> outputMat;
  Expression ContextE;
  if (!theCommands.GetMatriXFromArguments(input, outputMat, &ContextE, -1, CalculatorSerialization::innerRationalFunction))
  { theCommands.Comments << "<hr>Failed to get matrix of rational functions. ";
    return false;
  }
//  std::cout << "<hr>And the context is: " << ContextE.ToString();
  return output.AssignValueWithContext(outputMat, ContextE, theCommands);
}

bool CalculatorFunctionsGeneral::innerDecomposeCharGenVerma(Calculator& theCommands, const Expression& input, Expression& output)
{ RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDecomposeCharGenVerma");
  Expression theContext;
  Vector<RationalFunctionOld> theHWfundcoords, theHWsimpCoords, theHWFundCoordsFDPart, theHWSimpCoordsFDPart;
  Selection parSel, invertedParSel;
  SemisimpleLieAlgebra* theSSlieAlg=0;
  output.reset(theCommands);
  if (!theCommands.GetTypeHighestWeightParabolic<RationalFunctionOld>
      (theCommands, input, output, theHWfundcoords, parSel, theContext, theSSlieAlg, CalculatorSerialization::innerRationalFunction))
   return false;
  if (output.IsError())
    return true;
  std::stringstream out;
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
  out << "<br>Highest weight: " << theHWfundcoords.ToString(&theFormat) << "<br>Parabolic selection: " << parSel.ToString();
  theHWFundCoordsFDPart=theHWfundcoords;
  for (int i=0; i<parSel.CardinalitySelection; i++)
    theHWFundCoordsFDPart[parSel.elements[i]]=0;
  KLpolys theKLpolys;
  WeylGroup theWeyl;
  theWeyl=theSSlieAlg->theWeyl;
  if (!theKLpolys.ComputeKLPolys(&theWeyl))
    return output.SetError("failed to generate Kazhdan-Lusztig polynomials (output too large?)", theCommands);
//  Vectors<Polynomial<Rational> > tempVects;
//  tempVects.AddOnTop(theSSlieAlg.theWeyl.GetSimpleCoordinatesFromFundamental(theHWfundcoords) );
//  HashedList<Vector<Polynomial<Rational> > > theOrbit;
//  if (!theSSlieAlg.theWeyl.GenerateOrbit(tempVects, true, theOrbit, false, -1, 0, 1000))
//    out << "Error: failed to generate highest weight \rho-modified orbit (too large?)";
  theHWSimpCoordsFDPart=theWeyl.GetSimpleCoordinatesFromFundamental(theHWFundCoordsFDPart);
  theHWSimpCoordsFDPart+=theWeyl.rho;
//  ElementWeylGroup<WeylGroup> raisingElt;
//  theSSlieAlg.theWeyl.RaiseToDominantWeight
//  (theHWSimpCoordsFDPart, 0, 0, &raisingElt);
  SubgroupWeylGroupOLD theSub;
  if (!theSub.MakeParabolicFromSelectionSimpleRoots(theWeyl, parSel, *theCommands.theGlobalVariableS, 1000))
    return output.SetError("Failed to generate Weyl subgroup of Levi part (possibly too large? element limit is 1000).", theCommands);
  theHWsimpCoords=theWeyl.GetSimpleCoordinatesFromFundamental(theHWfundcoords);
  List<ElementWeylGroup<WeylGroup> > theWeylElements;
  theSub.GetGroupElementsIndexedAsAmbientGroup(theWeylElements);
  Vector<RationalFunctionOld> currentHW;
  out << "<br>Orbit modified with small rho: <table><tr><td>Simple coords</td><td>Fund coords</td></tr>";
  for (int i=0; i<theWeyl.theElements.size; i++)
  { currentHW=theHWsimpCoords;
    currentHW+=theSub.GetRho();
    theWeyl.ActOn(i, currentHW);
    currentHW-=theSub.GetRho();
    out << "<tr><td>" << currentHW.ToString() << "</td><td>" << theWeyl.GetFundamentalCoordinatesFromSimple(currentHW).ToString() << "</td></tr>";
  }
  out << "</table>";
  out << "<br>The rho of the Levi part is: " << theSub.GetRho().ToString() << "<br>Weyl group of Levi part follows. "
  << "<br><table><tr><td>Weyl element<td><td>Image hw under small rho modified action fund coords</td>"
  << "<td>Character Verma given h.w.</td></tr>";
  invertedParSel=parSel;
  invertedParSel.InvertSelection();
  charSSAlgMod<RationalFunctionOld> theChar, currentChar;
  Weight<RationalFunctionOld> theMon;
  theChar.MakeZero();
  FormatExpressions formatChars;
  formatChars.FDrepLetter="L";
  formatChars.fundamentalWeightLetter="\\omega";
  formatChars.flagUseLatex=true;
  for (int i=0; i<theWeylElements.size; i++)
  { ElementWeylGroup<WeylGroup>& currentElt=theWeylElements[i];
    out << "<tr><td>" << currentElt.ToString() << "</td>";
    int indexInWeyl=theKLpolys.TheWeylGroup->theElements.GetIndex(currentElt);
    if (indexInWeyl==-1)
      crash << "This is a programming error. Something is wrong: I am getting that an element of the Weyl group of the Levi part of "
      << "the parabolic does not lie in the ambient Weyl group, which is impossible. There is a bug somewhere; crashing in accordance. " << crash;
    currentChar.MakeZero();
    theMon.owner=theSSlieAlg;
    for (int j=0; j< theKLpolys.theKLcoeffs[indexInWeyl].size; j++)
      if (!theKLpolys.theKLcoeffs[indexInWeyl][j].IsEqualToZero())
      { currentHW=theHWsimpCoords;
        //currentHW+=theSub.GetRho();
        theWeyl.ActOnRhoModified(j, currentHW);
        //currentHW-=theSub.GetRho();
        theMon.weightFundamentalCoordS=theWeyl.GetFundamentalCoordinatesFromSimple(currentHW);
        int sign= (currentElt.generatorsLastAppliedFirst.size- theWeyl.theElements[j].generatorsLastAppliedFirst.size)%2==0 ? 1 :-1;
        currentChar.AddMonomial(theMon, theKLpolys.theKLcoeffs[indexInWeyl][j]*sign);
      }
    currentHW=theHWsimpCoords;
    currentHW+=theSub.GetRho();
    theWeyl.ActOn(indexInWeyl, currentHW);
    currentHW-=theSub.GetRho();
    out << "<td>" << theWeyl.GetFundamentalCoordinatesFromSimple(currentHW).ToStringLetterFormat("\\omega") << "</td>";
    out << "<td>" << CGI::GetMathMouseHover(currentChar.ToString(&formatChars)) << "</td>";
    if (currentElt.generatorsLastAppliedFirst.size%2==1)
      currentChar*=-1;
    theChar+=currentChar;
    out << "</tr>";
  }
  out << "</table>";
  out << "Final char: " << CGI::GetMathMouseHover(theChar.ToString(&formatChars));
  return output.AssignValue<std::string>(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerPrintGenVermaModule(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPrintGenVermaModule");
  Selection selectionParSel;
  Vector<RationalFunctionOld> theHWfundcoords;
  Expression hwContext(theCommands);
  SemisimpleLieAlgebra* theSSalgebra=0;
  if(!theCommands.GetTypeHighestWeightParabolic
      (theCommands, input, output, theHWfundcoords, selectionParSel, hwContext, theSSalgebra, CalculatorSerialization::innerRationalFunction))
    return output.SetError("Failed to extract highest weight vector data", theCommands);
  else
    if (output.IsError())
      return true;
  if (!theCommands.innerHWVCommon(theCommands, output, theHWfundcoords, selectionParSel, hwContext, theSSalgebra, false))
    return output.SetError("Failed to create Generalized Verma module", theCommands);
  if (output.IsError())
    return true;
  ElementTensorsGeneralizedVermas<RationalFunctionOld> theElt=output.GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
  ModuleSSalgebra<RationalFunctionOld>& theModule= *theElt[0].theMons[0].owneR;
  return output.AssignValue(theModule.ToString(), theCommands);
}

bool CalculatorFunctionsGeneral::innerWriteGenVermaModAsDiffOperatorUpToLevel(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerWriteGenVermaModAsDiffOperatorUpToLevel");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(4))
    return output.SetError("Function innerSplitGenericGenVermaTensorFD is expected to have three arguments: SS algebra type, Number, List{}. ", theCommands);
  const Expression& leftE=input[1];
  const Expression& genVemaWeightNode=input[3];
  const Expression& levelNode=input[2];
  Expression resultSSalgebraE;
  resultSSalgebraE=leftE;
  SemisimpleLieAlgebra* theSSalgebra;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorSerialization::innerSSLieAlgebra, leftE, theSSalgebra))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  int theRank=theSSalgebra->GetRank();
  Vector<Polynomial<Rational> > highestWeightFundCoords;
  Expression hwContext(theCommands), emptyContext(theCommands);
  if (!theCommands.GetVectoR(genVemaWeightNode, highestWeightFundCoords, &hwContext, theRank, CalculatorSerialization::innerPolynomial<Rational>))
  { theCommands.Comments
    << "Failed to convert the third argument of innerSplitGenericGenVermaTensorFD to a list of " << theRank
    << " polynomials. The second argument you gave is " << genVemaWeightNode.ToString() << ".";
    return false;
  }
  int desiredHeight;
  if (!levelNode.IsSmallInteger(&desiredHeight))
    return output.SetError("second argument of " + input.ToString()+ " must be a small integer", theCommands);

//  std::cout << "<br>highest weight in fundamental coords: " << highestWeightFundCoords.ToString() << "<br>";
//  std::cout << "<br>parabolic selection: " << parabolicSel.ToString();

//  int theNumVars=hwContext.VariableImages.size;
  RationalFunctionOld RFOne, RFZero;
  RFOne.MakeOne(theCommands.theGlobalVariableS);
  RFZero.MakeZero(theCommands.theGlobalVariableS);
  std::string report;
  ElementTensorsGeneralizedVermas<RationalFunctionOld> theElt;
  //=theElementData.theElementTensorGenVermas.GetElement();
  Selection selInducing;
  Expression hwvGenVerma, hwvFD;
  selInducing.MakeFullSelection(theRank);
  int theCoeff;
  for (int i=0; i<theRank; i++)
    if (highestWeightFundCoords[i].IsSmallInteger(&theCoeff))
      if (theCoeff==0)
        selInducing.RemoveSelection(i);
//  std::cout << "Your input so far: " << theSSalgebra.GetLieAlgebraName() << " with hw: " << highestWeightFundCoords.ToString()
//  << " parabolic selection: " << selInducing.ToString() << " degree: " << desiredHeight;
  Vectors<Polynomial<Rational> > theHws;
  Selection invertedSelInducing=selInducing;
  invertedSelInducing.InvertSelection();
  theHws.SetSize(0);
  SelectionWithMaxMultiplicity theHWenumerator;
  Vector<Polynomial<Rational> > theHWrf;
  for (int j=0; j<=desiredHeight; j++)
  { theHWenumerator.initMaxMultiplicity(theRank-selInducing.CardinalitySelection, j);
    theHWenumerator.IncrementSubsetFixedCardinality(j);
    int numCycles=theHWenumerator.NumCombinationsOfCardinality(j);
    for (int i=0; i<numCycles; i++, theHWenumerator.IncrementSubsetFixedCardinality(j))
    { theHWrf=highestWeightFundCoords;
      for (int k=0; k<invertedSelInducing.CardinalitySelection; k++)
        theHWrf[invertedSelInducing.elements[k]]+=theHWenumerator.Multiplicities[k];
      theHws.AddOnTop(theHWrf);
    }
  }
  FormatExpressions theFormat;
  hwContext.ContextGetFormatExpressions(theFormat);
//  std::cout << "highest weights you are asking me for: " << theHws.ToString(&theFormat);
  return theCommands.innerWriteGenVermaModAsDiffOperatorInner(theCommands, input, output, theHws, hwContext, selInducing, theSSalgebra, false);
}

bool CalculatorFunctionsGeneral::innerHWV(Calculator& theCommands, const Expression& input, Expression& output)
{ Selection selectionParSel;
  Vector<RationalFunctionOld> theHWfundcoords;
  Expression hwContext(theCommands);
  SemisimpleLieAlgebra* theSSalgebra=0;
  if(!theCommands.GetTypeHighestWeightParabolic(theCommands, input, output, theHWfundcoords, selectionParSel, hwContext, theSSalgebra, CalculatorSerialization::innerRationalFunction) )
    return output.SetError("Failed to extract highest weight vector data", theCommands);
  else
    if (output.IsError())
      return true;
  return theCommands.innerHWVCommon(theCommands, output, theHWfundcoords, selectionParSel, hwContext, theSSalgebra);
}

bool CalculatorFunctionsGeneral::innerSplitGenericGenVermaTensorFD(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSplitGenericGenVermaTensorFD");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(4))
    return output.SetError
    ("Function innerSplitGenericGenVermaTensorFD is expected to have three arguments: SS algebra type, weight, weight. ", theCommands);
  const Expression& leftE=input[1];
  const Expression& genVemaWeightNode=input[3];
  const Expression& fdWeightNode=input[2];
  SemisimpleLieAlgebra* theSSalgebra;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorSerialization::innerSSLieAlgebra, leftE, theSSalgebra))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  int theRank=theSSalgebra->GetRank();
  Vector<RationalFunctionOld> highestWeightFundCoords;
  Expression hwContext(theCommands);
  if (!theCommands.GetVectoR<RationalFunctionOld>(genVemaWeightNode, highestWeightFundCoords, &hwContext, theRank, CalculatorSerialization::innerRationalFunction))
  { theCommands.Comments
    << "Failed to convert the third argument of innerSplitGenericGenVermaTensorFD to a list of " << theRank
    << " polynomials. The second argument you gave is " << genVemaWeightNode.ToString() << ".";
    return false;
  }
  Vector<Rational> theFDhw;
  if (!theCommands.GetVectoR<Rational>(fdWeightNode, theFDhw, 0, theRank, 0))
  { theCommands.Comments
    << "Failed to convert the second argument of innerSplitGenericGenVermaTensorFD to a list of " << theRank
    << " rationals. The second argument you gave is " << fdWeightNode.ToString() << ".";
    return false;
  }
//  std::cout << "<br>highest weight in fundamental coords: " << highestWeightFundCoords.ToString() << "<br>";
//  std::cout << "<br>parabolic selection: " << parabolicSel.ToString();
  int theNumVars=hwContext.ContextGetNumContextVariables();
  RationalFunctionOld RFOne, RFZero;
  RFOne.MakeOne(theCommands.theGlobalVariableS);
  RFZero.MakeZero(theCommands.theGlobalVariableS);
  ElementTensorsGeneralizedVermas<RationalFunctionOld> theElt;
  //=theElementData.theElementTensorGenVermas.GetElement();
  Selection selParSel1, selFD;
  Expression hwvFD, hwvGenVerma;
  selParSel1.MakeFullSelection(theRank);
  selFD.init(theRank);
  int theCoeff;
  for (int i=0; i<theRank; i++)
  { if (highestWeightFundCoords[i].IsSmallInteger(&theCoeff))
      if (theCoeff>=0)
        selParSel1.RemoveSelection(i);
    bool isGood=false;
    if (theFDhw[i].IsSmallInteger(&theCoeff))
      if (theCoeff>=0)
        isGood=true;
    if (!isGood)
      return output.SetError("Error: the third argument of innerSplitGenericGenVermaTensorFD must be a list of small non-negative integers.", theCommands);
  }
  if (!theCommands.innerHWVCommon(theCommands, hwvGenVerma, highestWeightFundCoords, selParSel1, hwContext, theSSalgebra))
    return false;
  if (hwvGenVerma.IsError())
  { output=hwvGenVerma;
    return true;
  }
  Vector<RationalFunctionOld> theFDhwRF;
  theFDhwRF=theFDhw;
  if (!theCommands.innerHWVCommon(theCommands, hwvFD, theFDhwRF, selFD, hwContext, theSSalgebra))
    return false;
  if (hwvFD.IsError())
  { output=hwvFD;
    return true;
  }
  std::stringstream out;
  out << "hwv par sel: " << hwvGenVerma.ToString() << "hwv fd: " << hwvFD.ToString();
  const ElementTensorsGeneralizedVermas<RationalFunctionOld>& theHWgenVerma= hwvGenVerma.GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
  const ElementTensorsGeneralizedVermas<RationalFunctionOld>& theHWfd=hwvFD.GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();

  ModuleSSalgebra<RationalFunctionOld>& theGenMod=theHWgenVerma[0].theMons[0].GetOwner();
  //int indexGenMod=theHWgenVerma[0].theMons[0].indexInOwner;
  ModuleSSalgebra<RationalFunctionOld>& theFDMod=theHWfd[0].theMons[0].GetOwner();
  if (theGenMod.owneR!=theFDMod.owneR ||
      theGenMod.GetOwner().GetRank()!=theGenMod.parabolicSelectionNonSelectedAreElementsLevi.MaxSize ||
      theFDMod.GetOwner().GetRank()!=theFDMod.parabolicSelectionNonSelectedAreElementsLevi.MaxSize
      )
    crash << "This is a programming error: the two modules have owners, " << theFDMod.GetOwner().theWeyl.theDynkinType.ToString()
    << " and " << theGenMod.GetOwner().theWeyl.theDynkinType.ToString() << ", and parabolic selections of max size "
    << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.MaxSize << " and " << theFDMod.parabolicSelectionNonSelectedAreElementsLevi.MaxSize << crash;
  //int indexFDMod=theHWfd[0].theMons[0].indexInOwner;
  ElementUniversalEnveloping<RationalFunctionOld> theCasimir, theCasimirMinusChar;
  charSSAlgMod<RationalFunctionOld> theHWchar, theFDLeviSplit, theFDChaR, theFDLeviSplitShifteD;
  theHWchar.MakeFromWeight(theFDMod.theHWSimpleCoordSBaseField, theSSalgebra);
  List<ElementUniversalEnveloping<RationalFunctionOld> > theLeviEigenVectors;
  Vectors<RationalFunctionOld> theEigenVectorWeightsFund;
  if (theGenMod.parabolicSelectionNonSelectedAreElementsLevi.MaxSize!=theGenMod.GetOwner().GetRank())
    crash << "This is a programming error: module has parabolic selection with max size "
    << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.MaxSize << " but the ambient semisimple Lie algebra is of rank "
    << theGenMod.GetOwner().GetRank() << ". " << crash;
  std::string report;
  theFDMod.SplitOverLevi
  (&report, theGenMod.parabolicSelectionNonSelectedAreElementsLevi, *theCommands.theGlobalVariableS, RFOne, RFZero,
   &theLeviEigenVectors, &theEigenVectorWeightsFund, 0, &theFDLeviSplit);
  theFDMod.GetFDchar(theFDChaR);
  List<ElementUniversalEnveloping<RationalFunctionOld> > theCentralCharacters;
  theCasimir.MakeCasimir(*theSSalgebra);
  Vector<RationalFunctionOld> currentHWsimplecoords, currentHWdualcoords, currentWeightDiff;
  FormatExpressions tempFormat;
  tempFormat.MaxLineLength=60;
  tempFormat.flagUseLatex=true;
  tempFormat.fundamentalWeightLetter="\\psi";
  tempFormat.CustomPlusSign="\\oplus ";
  hwContext.ContextGetFormatExpressions(tempFormat);
  out << "<br>Character of finite dimensional module:" << CGI::GetMathMouseHover(theFDChaR.ToString());
  if (theGenMod.parabolicSelectionSelectedAreElementsLevi.CardinalitySelection>0)
    out << "<br>theFDChar split over levi:" << CGI::GetMathMouseHover(theFDLeviSplit.ToString(&tempFormat));
  //out << "<br> Splitting report: " << report;
  std::stringstream latexReport1;
  out << "<br><table><tr><td>weight in fundamental coords</td><td>Character</td></tr>";
  latexReport1 << " \\begin{longtable}{rl}\\caption{\\label{table" << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.ToString()
  << "GenVermatimes7DimCentralCharacters} $" << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.ToString()
  << "$- parabolic $\\bar{\\mathfrak{p}}$} \\\\ $\\mu+\\gamma$ & Action of $\\bar c$\\\\\\hline";
  tempFormat.CustomPlusSign="";
  tempFormat.chevalleyGgeneratorLetter="\\bar{g}";
  tempFormat.chevalleyHgeneratorLetter="\\bar{h}";
  theFDLeviSplitShifteD.MakeZero();
  Weight<RationalFunctionOld> tempMon;
  tempMon.owner=theSSalgebra;
  ElementUniversalEnveloping<RationalFunctionOld> currentChar;
  for (int i=0; i<theLeviEigenVectors.size; i++)
  { tempMon.weightFundamentalCoordS=theEigenVectorWeightsFund[i];
    tempMon.weightFundamentalCoordS+=theGenMod.theHWFundamentalCoordsBaseField;
    theFDLeviSplitShifteD.AddMonomial(tempMon, RFOne);
    currentHWdualcoords=theSSalgebra->theWeyl.GetDualCoordinatesFromFundamental(tempMon.weightFundamentalCoordS);
    currentChar=theCasimir;
    currentChar.ModOutVermaRelations(theCommands.theGlobalVariableS, & currentHWdualcoords, RFOne, RFZero);
    theCentralCharacters.AddOnTop(currentChar);
    out << "<tr><td>" << theFDLeviSplitShifteD[i].weightFundamentalCoordS.ToStringLetterFormat("\\psi") << "</td><td>"
    << currentChar.ToString(&tempFormat) << "</td></tr>";
    latexReport1 << " $" << theFDLeviSplitShifteD[i].weightFundamentalCoordS.ToStringLetterFormat("\\psi", &tempFormat) << "$"
    << "&$p_{" << i+1 <<"}:=$ $" << currentChar.ToString(&tempFormat) << "$\\\\<br>";
  }
  out << "</table>";
  latexReport1 << "\\end{longtable}<br>";
  ElementTensorsGeneralizedVermas<RationalFunctionOld> tempElt, tempElt2;
  theFDMod.highestWeightVectorNotation="v";
  theGenMod.highestWeightVectorNotation="w";
  out << "Let w be the highest weight vector of the generalized Verma component, and let v be the highest weight vector of the finite dimensional component";
  out << "<br><table><tr><td>weight in fundamental coords</td><td>Algebraic expression</td><td>Additional multiplier</td>";
  if (theNumVars==1)
    out << "<td>gcd polynomial coeffs</td>";
  out << "<td>the hwv</td>";
  if (theNumVars==1)
    out << "<td>gcd divided out</td>";
  out << "</tr>";
  std::stringstream latexReport2;
  latexReport2 << "\\begin{longtable}{p{2.5cm}p{2.5cm}p{1.5cm}l}\\caption{\\label{tableDecompo"
  << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.ToString() << "times7dim}Decomposition for the $"
  << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.ToString()
  << "$-parabolic subalgebra $\\bar{\\mathfrak{p}}$ } \\\\ Weight & Projector applied to &"
  << " Extra multiplier & Resulting $\\bar {\\mathfrak b}$-singular vector \\endhead\\hline";
  //std::cout << theGenMod.theGeneratingWordsNonReduced.ToString();
  for (int i=0; i<theCentralCharacters.size; i++)
  { Vector<RationalFunctionOld> currentWeightSimpleCoords=
    theSSalgebra->theWeyl.GetSimpleCoordinatesFromFundamental(theEigenVectorWeightsFund[i]);
    tempElt.MakeHWV(theFDMod, RFOne);
    tempElt.MultiplyOnTheLeft(theLeviEigenVectors[i], theElt, *theSSalgebra, *theCommands.theGlobalVariableS, RFOne, RFZero);
    tempElt.MakeHWV(theGenMod, RFOne);
//      tempElt.MultiplyOnTheLeft
//      (theGenMod.theGeneratingWordsNonReduced[0], tempElt2, theMods, theSSalgebra,
//         *theCommands.theGlobalVariableS,
//       RFOne, RFZero);
    theElt.TensorOnTheRight(tempElt, *theCommands.theGlobalVariableS, RFOne, RFZero);
    theElt*=-1;
    std::string startingEltString=theElt.ToString(&tempFormat);
    std::stringstream tempStream, tempStream2;
    tempStream << "\\begin{array}{l}";
    bool found=false;
    for (int j=0; j<theCentralCharacters.size; j++)
    { Vector<RationalFunctionOld> otherWeightSimpleCoords=
      theSSalgebra->theWeyl.GetSimpleCoordinatesFromFundamental(theEigenVectorWeightsFund[j]);
      if ((otherWeightSimpleCoords-currentWeightSimpleCoords).IsPositive())
      { theCasimirMinusChar=theCasimir;
        theCasimirMinusChar-=theCentralCharacters[j];
        theElt.MultiplyOnTheLeft(theCasimirMinusChar, tempElt2, *theSSalgebra, *theCommands.theGlobalVariableS, RFOne, RFZero);
        theElt=tempElt2;
        tempStream << "(i(\\bar c)- (" << theCentralCharacters[j].ToString() << ") )\\\\";
        tempStream2 << " $(\\bar c-p_" << j+1 << ") $ ";
        found=true;
      }
    }
    if (found)
    { tempStream << "\\cdot";
      tempStream2 << " $\\cdot$ ";
    }
    tempStream << "(" << startingEltString << ")";
    tempStream << "\\end{array}";
    tempStream2 << " $(" << startingEltString << ")$ ";
//      std::cout << "<hr><hr>(" << theElt.ToString();
    Rational tempRat= theElt.ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy();
    currentHWsimplecoords=theGenMod.theHWSimpleCoordSBaseField;
    currentHWsimplecoords+=theFDMod.theModuleWeightsSimpleCoords[i];
//      std::cout << ") * " << tempRat.ToString()  << "<hr>=" << theElt.ToString() << "<hr><hr>";
    out << "<tr><td>"
    << theSSalgebra->theWeyl.GetFundamentalCoordinatesFromSimple(currentHWsimplecoords).ToStringLetterFormat("\\psi")
    << "</td><td>" << CGI::GetMathMouseHover(tempStream.str()) << "</td><td>" << tempRat.ToString() << "</td>";
    latexReport2
    << "$" << theSSalgebra->theWeyl.GetFundamentalCoordinatesFromSimple(currentHWsimplecoords).ToStringLetterFormat("\\psi")
    << "$ &  " << tempStream2.str() << " &" << tempRat.ToString();
    Polynomial<Rational> tmpGCD, tmpRF;
    tempFormat.MaxLineLength=80;
    if (theNumVars==1)
    { tmpGCD= theElt.FindGCDCoefficientNumeratorsOverRationals();
      tmpGCD.ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy();
      out << "<td>" << CGI::GetMathMouseHover(tmpGCD.ToString(&tempFormat)) << "</td>";
    }
    out << "<td>" << CGI::GetMathMouseHover(theElt.ToString(&tempFormat)) << "</td>";
    latexReport2 << "&$\\begin{array}{l}" << theElt.ToString(&tempFormat) << "\\end{array}$\\\\<br>";
    if (theNumVars==1)
    { tmpRF=tmpGCD;
      theElt/=tmpRF;
      out << "<td>" << CGI::GetMathMouseHover("\\begin{array}{l}" + theElt.ToString(&tempFormat)+"\\end{array}") << "</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  latexReport2 << "\\end{longtable}";
  out << "<br>Ready LaTeX (table 1 and 2): <br><br><br>" << latexReport1.str() << "<br><br><br>" << latexReport2.str() << "<br>";
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerHWTAABF(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerHWTAABF");
  RecursionDepthCounter theRecursionCounter(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(4))
    return output.SetError("Function expects three arguments.", theCommands);
  Expression leftMerged=input[1];
  Expression rightMerged=input[2];
  if (!Expression::MergeContexts(leftMerged, rightMerged))
    return output.SetError("Failed to merge the contexts of the two UE elements ", theCommands);
  Expression finalContext=rightMerged.GetContext();
  int algebraIndex=finalContext.ContextGetIndexAmbientSSalg();
  if (algebraIndex==-1)
    return output.SetError("I couldn't extract a Lie algebra to compute hwtaabf.", theCommands);
  SemisimpleLieAlgebra* constSSalg= &theCommands.theObjectContainer.theLieAlgebras.GetElement(algebraIndex);
  const Expression& weightExpression=input[3];
  Vector<RationalFunctionOld> weight;
  if (!theCommands.GetVectoR<RationalFunctionOld>(weightExpression, weight, &finalContext, constSSalg->GetRank(), CalculatorSerialization::innerRationalFunction))
  { theCommands.Comments << "<hr>Failed to obtain highest weight from the third argument which is " << weightExpression.ToString();
    return false;
  }
  if (!leftMerged.SetContextAtLeastEqualTo(finalContext) || !rightMerged.SetContextAtLeastEqualTo(finalContext))
    return output.SetError("Failed to merge the contexts of the highest weight and the elements of the Universal enveloping. ", theCommands);
  Expression leftConverted, rightConverted;
  if (!leftMerged.ConvertToType<ElementUniversalEnveloping<RationalFunctionOld> >(leftConverted))
    return false;
  if (!rightMerged.ConvertToType<ElementUniversalEnveloping<RationalFunctionOld> >(rightConverted))
    return false;
  const ElementUniversalEnveloping<RationalFunctionOld>& leftUE=leftConverted.GetValue<ElementUniversalEnveloping<RationalFunctionOld> >();
  const ElementUniversalEnveloping<RationalFunctionOld>& rightUE=rightConverted.GetValue<ElementUniversalEnveloping<RationalFunctionOld> >();
  SemisimpleLieAlgebra theSSalgebra;
  theSSalgebra=*constSSalg;
  WeylGroup& theWeyl=theSSalgebra.theWeyl;
  std::stringstream out;
  Vector<RationalFunctionOld> hwDualCoords;
  theSSalgebra.OrderSSalgebraForHWbfComputation();
  hwDualCoords=theWeyl.GetDualCoordinatesFromFundamental(weight);
  RationalFunctionOld outputRF;
  //std::cout << "<br>The highest weight in dual coordinates, as I understand it:" << hwDualCoords.ToString();
  if(!leftUE.HWTAAbilinearForm(rightUE, outputRF, &hwDualCoords, *theCommands.theGlobalVariableS, 1, 0, &theCommands.Comments))
    return output.SetError("Error: couldn't compute Shapovalov form, see comments.", theCommands);
  return output.AssignValueWithContext(outputRF, finalContext, theCommands);
}

bool CalculatorFunctionsGeneral::innerSplitFDpartB3overG2CharsOutput(Calculator& theCommands, const Expression& input, Expression& output, branchingData& theG2B3Data)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSplitFDpartB3overG2CharsOutput");
  Expression theContext(theCommands);
  CalculatorFunctionsGeneral::innerSplitFDpartB3overG2Init(theCommands, input, output, theG2B3Data, theContext);
  if (output.IsError())
    return true;
  std::stringstream out;
  out << "<br>Highest weight: " << theG2B3Data.theWeightFundCoords.ToString() << "<br>Parabolic selection: " << theG2B3Data.selInducing.ToString();
  std::string report;
  Vectors<RationalFunctionOld> outputWeightsFundCoordS;
  Vectors<RationalFunctionOld> outputWeightsSimpleCoords;
  Vectors<RationalFunctionOld> leviEigenSpace;
  Vector<RationalFunctionOld> ih1, ih2;
  SubgroupWeylGroupOLD subGroupLarge, subGroupSmall;
  charSSAlgMod<RationalFunctionOld> tempChar;
  charSSAlgMod<RationalFunctionOld> startingChar;
  startingChar.MakeFromWeight(theG2B3Data.theHmm.theRange().theWeyl.GetSimpleCoordinatesFromFundamental(theG2B3Data.theWeightFundCoords), &theG2B3Data.theHmm.theRange());
  startingChar.SplitCharOverRedSubalg(&report, tempChar, theG2B3Data, *theCommands.theGlobalVariableS);
  out << report;
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerPrintB3G2branchingTableInit
(Calculator& theCommands, const Expression& input, Expression& output, branchingData& theG2B3data, int& desiredHeight, Expression& outputContext)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPrintB3G2branchingTableInit");
  if (input.children.size!=3)
    return output.SetError("I need two arguments: first is height, second is parabolic selection. ", theCommands);
  desiredHeight=0;
  if (!input[1].IsSmallInteger(&desiredHeight))
    return output.SetError("the first argument must be a small integer", theCommands);
  if (desiredHeight<0)
    desiredHeight=0;
  const Expression& weightNode= input[2];
  CalculatorFunctionsGeneral::innerSplitFDpartB3overG2Init(theCommands, weightNode, output, theG2B3data, outputContext);
  if (output.IsError())
    return true;
  return false;
}

bool CalculatorFunctionsGeneral::innerSplitFDpartB3overG2(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSplitFDpartB3overG2");
  branchingData theG2B3Data;
  Expression theContext(theCommands);
  CalculatorFunctionsGeneral::innerSplitFDpartB3overG2Init(theCommands, input, output, theG2B3Data, theContext);
  if (output.IsError())
    return true;
  std::stringstream out;
  Vectors<RationalFunctionOld> theHWs;
  theHWs.AddOnTop(theG2B3Data.theWeightFundCoords);
  return theCommands.fPrintB3G2branchingIntermediate(theCommands, input, output, theHWs, theG2B3Data, theContext);
}

bool Calculator::fSplitFDpartB3overG2CharsOnly(Calculator& theCommands, const Expression& input, Expression& output)
{ branchingData theG2B3Data;
  return CalculatorFunctionsGeneral::innerSplitFDpartB3overG2CharsOutput(theCommands, input, output, theG2B3Data);
}

bool Calculator::fPrintB3G2branchingTableCommon
(Calculator& theCommands, const Expression& input, Expression& output, Vectors<RationalFunctionOld>& outputHWs, branchingData& theG2B3Data, Expression& theContext)
{ MacroRegisterFunctionWithName("Calculator::fPrintB3G2branchingTableCommon");
  std::stringstream out, timeReport;
  Vector<Rational> theHW;
  Vector<RationalFunctionOld> theHWrf;
  SelectionWithMaxMultiplicity theHWenumerator;
  int desiredHeight=0;
  if (!CalculatorFunctionsGeneral::innerPrintB3G2branchingTableInit(theCommands, input, output, theG2B3Data, desiredHeight, theContext))
    return false;
  if (output.IsError())
    return true;
  Selection invertedSelInducing=theG2B3Data.selInducing;
  theContext.ContextGetFormatExpressions(theG2B3Data.theFormat);
  invertedSelInducing.InvertSelection();
  outputHWs.SetSize(0);
  for (int j=0; j<=desiredHeight; j++)
  { theHWenumerator.initMaxMultiplicity(3-theG2B3Data.selInducing.CardinalitySelection, j);
    theHWenumerator.IncrementSubsetFixedCardinality(j);
    int numCycles=theHWenumerator.NumCombinationsOfCardinality(j);
    for (int i=0; i<numCycles; i++, theHWenumerator.IncrementSubsetFixedCardinality(j))
    { theHWrf=theG2B3Data.theWeightFundCoords;
      for (int k=0; k<invertedSelInducing.CardinalitySelection; k++)
        theHWrf[invertedSelInducing.elements[k]]+=theHWenumerator.Multiplicities[k];
      outputHWs.AddOnTop(theHWrf);
    }
  }
//  std::cout << " <br>the highest weights: " << outputHWs.ToString();
  return true;
}

bool Calculator::fSplitFDpartB3overG2old(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::fSplitFDpartB3overG2old");
  branchingData theG2B3Data;
  CalculatorFunctionsGeneral::innerSplitFDpartB3overG2CharsOutput(theCommands, input, output, theG2B3Data);
  if (output.IsError())
    return true;
  std::stringstream out;
  theCommands.fSplitFDpartB3overG2inner(theCommands, theG2B3Data, output);
  out << "<br>Highest weight: " << theG2B3Data.theWeightFundCoords.ToString() << "<br>Parabolic selection: "
  << theG2B3Data.selInducing.ToString() << "<br>common Levi part of G_2 and B_3: "
  << theG2B3Data.selSmallParSel.ToString();
  out
  << "<table border=\"1\"><tr><td>word</td><td>B_3-weight simple coords</td><td>B_3-weight fund. coords </td>"
  << "<td>G_2 simple coordinates</td><td>G2-fund. coords</td><td>G2-dual coordinates</td><td>character</td></tr>";
  std::stringstream readyForLatexConsumptionTable1, readyForLatexConsumptionTable2;

  readyForLatexConsumptionTable1 << "\\hline\\multicolumn{3}{|c|}{Highest weight $ "
  <<  theG2B3Data.theWeightFundCoords.ToStringLetterFormat("\\omega")
  << "$}\\\\ weight fund. coord.& singular vector& weight proj. $\\bar h^*$ \\\\\\hline\n<br> ";
  for (int i=0; i< theG2B3Data.outputWeightsSimpleCoords.size; i++)
  { Vector<RationalFunctionOld>& currentWeightSimpleB3coords=theG2B3Data.outputWeightsSimpleCoords[i];
    Vector<RationalFunctionOld>& currentWeightFundB3coords=theG2B3Data.outputWeightsFundCoordS[i];
    Vector<RationalFunctionOld>& currentG2Weight=theG2B3Data.g2Weights[i];
    Vector<RationalFunctionOld>& currentG2DualWeight=theG2B3Data.g2DualWeights[i];
    readyForLatexConsumptionTable1 << "$" << currentWeightFundB3coords.ToStringLetterFormat("\\omega")
    << " $ & $" << theG2B3Data.leviEigenSpace[i].ToStringLetterFormat("m")
    << " $ & $ " << currentG2Weight.ToStringLetterFormat("\\alpha") << " $ \\\\\n<br>";
    out << "<tr><td>" << theG2B3Data.outputEigenWords[i].ToString() << "</td><td> "
    << currentWeightSimpleB3coords.ToString() << "</td><td> " << currentWeightFundB3coords.ToString()
    << "</td><td>" << currentG2Weight.ToStringLetterFormat("\\alpha") << "</td><td> "
    << theG2B3Data.theHmm.theDomain().theWeyl.GetFundamentalCoordinatesFromSimple(currentG2Weight).ToString()
    << "</td><td> " << currentG2DualWeight.ToString() << "</td>";
    out << "<td>" << CGI::GetMathSpanPure(theG2B3Data.theChars[i].ToString()) << "</td>";
    out << "</tr>";
  }
  readyForLatexConsumptionTable1 <<"\\hline \n";
  out << "</table>";
  out << "<br>Ready for LaTeX consumption: ";
  out << "<br><br>" << readyForLatexConsumptionTable1.str() << "<br><br>";
  out << "<table border=\"1\"><tr><td>weight</td><td>the elt closed form</td><td>the elt</td></tr>";
  Vector<RationalFunctionOld> weightDifference, weightDifferenceDualCoords;
  std::stringstream formulaStream1;
  for (int k=0; k<theG2B3Data.g2Weights.size; k++)
  { out << "<tr><td>" << theG2B3Data.g2Weights[k].ToString() << "</td><td>";
    for (int j=0; j<theG2B3Data.g2Weights.size; j++)
    { weightDifference=theG2B3Data.g2Weights[j] - theG2B3Data.g2Weights[k];
      if (weightDifference.IsPositive())
        formulaStream1 << "(12(i(\\bar c) - " << theG2B3Data.theChars[j].ToString() <<  "))";
    }
    formulaStream1 << "v_\\lambda";
    out << CGI::GetMathSpanPure(formulaStream1.str()) << "</td><td>" << CGI::GetMathSpanPure(theG2B3Data.theEigenVectorS[k].ToString()) << "</td></tr>";
  }
  out << "</table>";
  out << "<br>Time final: " << theCommands.theGlobalVariableS->GetElapsedSeconds();
  return output.AssignValue(out.str(), theCommands);
}

bool Expression::EvaluatesToRealDoubleInRange
  (const std::string& varName, double lowBound, double highBound, int numIntervals,
   double* outputYmin, double* outputYmax)const
{ MacroRegisterFunctionWithName("Expression::EvaluatesToRealDoubleInRange");
  if (numIntervals<1 || this->theBoss==0)
    return false;
  HashedList<Expression> knownEs= this->theBoss->knownDoubleConstants;
  List<double> knownValues=this->theBoss->knownDoubleConstantValues;
  Expression theVarNameE;
  theVarNameE.MakeAtom(varName, *this->theBoss);
  if (knownEs.Contains(theVarNameE))
  { this->theBoss->Comments << "Variable name is an already known constant, variable name is bad.";
    return false;
  }
  knownEs.AddOnTop(theVarNameE);
  knownValues.AddOnTop(0);
  int numPoints=numIntervals+1;
  double delta=(highBound-lowBound)/(numPoints);
  *knownValues.LastObject()=lowBound;
  double currentValue=0;
  for (int i=0; i<numIntervals; i++)
  { if (!this->EvaluatesToRealDoubleUnderSubstitutions(knownEs, knownValues, & currentValue))
    { this->theBoss->Comments << "<hr>Failed to evaluate " << this->ToString() << " at " << varName << "="
      << *knownValues.LastObject() << ". ";
      return false;
    }
    *knownValues.LastObject()+=delta;
    if (outputYmin!=0)
    { if (i==0)
        *outputYmin=currentValue;
      else
        *outputYmin=MathRoutines::Minimum(currentValue, *outputYmin);
    }
    if (outputYmax!=0)
    { if (i==0)
        *outputYmax=currentValue;
      else
        *outputYmax=MathRoutines::Maximum(currentValue, *outputYmax);
    }
  }
  return true;
}

bool Expression::EvaluatesToRealDouble(double* whichDouble)const
{ return this->EvaluatesToRealDoubleUnderSubstitutions
  (this->theBoss->knownDoubleConstants, this->theBoss->knownDoubleConstantValues, whichDouble);
}

bool Expression::EvaluatesToRealDoubleUnderSubstitutions
(const HashedList<Expression>& knownEs, const List<double>& valuesKnownEs, double* whichDouble)const
{ MacroRegisterFunctionWithName("Expression::EvaluatesToRealDoubleUnderSubstitutions");
  if (this->theBoss==0)
    return false;
//  std::cout << "<br>Evaluating to double: " << this->ToString();
  Calculator& theCommands=*this->theBoss;
  RecursionDepthCounter theRcounter(&this->theBoss->RecursionDeptH);
  //std::cout << "<br>evaluatetodouble: " << input.ToString();
  if (this->IsOfType<double>(whichDouble))
    return true;
  if (this->IsOfType<Rational>())
  { if (whichDouble!=0)
      *whichDouble=this->GetValue<Rational>().DoubleValue();
    return true;
  }
  RecursionDepthCounter theCounter(&this->theBoss->RecursionDeptH);
  if (this->theBoss->RecursionDeptH >this->theBoss->MaxRecursionDeptH)
  { this->theBoss->Comments << "<hr>Recursion depth exceeded while evaluating innerEvaluateToDouble. This may be a programming error. ";
    return false;
  }
  if (knownEs.Contains(*this))
  { if (whichDouble!=0)
      *whichDouble=valuesKnownEs[knownEs.GetIndex(*this)];
    return true;
  }

/*  if (this->IsAtomGivenData(theCommands.opE()))
    return output.AssignValue(MathRoutines::E(), theCommands);
  if (input.IsAtomGivenData(theCommands.opPi()))
  { //std::cout << "Evaluates to 3.1415...!";
    return output.AssignValue(MathRoutines::Pi(), theCommands);
  }*/
  bool isArithmeticOperationTwoArguments=
  this->IsListNElementsStartingWithAtom(theCommands.opTimes(),3) ||
  this->IsListNElementsStartingWithAtom(theCommands.opPlus(),3) ||
  this->IsListNElementsStartingWithAtom(theCommands.opThePower(),3) ||
  this->IsListNElementsStartingWithAtom(theCommands.opDivide(),3) ||
  this->IsListNElementsStartingWithAtom(theCommands.opSqrt(),3)
  ;
  //std::cout << " is arithmetic operation two arguments! ";
  //if (input.IsListNElementsStartingWithAtom(theCommands.opSqrt()))
  //  std::cout << "Starting with sqrt: " << input.ToStringFull();
  if (isArithmeticOperationTwoArguments)
  { double leftD, rightD;
    if (!(*this)[1].EvaluatesToRealDoubleUnderSubstitutions(knownEs, valuesKnownEs, &leftD) ||
        !(*this)[2].EvaluatesToRealDoubleUnderSubstitutions(knownEs, valuesKnownEs, &rightD))
      return false;
    if ((*this).IsListNElementsStartingWithAtom(theCommands.opTimes(),3))
    { if (whichDouble!=0)
        *whichDouble=leftD*rightD;
      return true;
    }
    if ((*this).IsListNElementsStartingWithAtom(theCommands.opPlus(),3))
    { if (whichDouble!=0)
        *whichDouble=leftD+rightD;
      return true;
    }
    if ((*this).IsListNElementsStartingWithAtom(theCommands.opThePower(),3))
    { if (whichDouble!=0)
        *whichDouble=FloatingPoint::power(leftD, rightD);
      return true;
    }
    if ((*this).IsListNElementsStartingWithAtom(theCommands.opSqrt(),3))
    { if (whichDouble!=0)
        *whichDouble=FloatingPoint::power(rightD,1/leftD);
      return true;
    }
    if ((*this).IsListNElementsStartingWithAtom(theCommands.opDivide(),3))
    { if (whichDouble!=0)
        *whichDouble=leftD/rightD;
      return true;
    }
    crash << "This is a piece of code which should never be reached. " << crash;
  }
  bool isKnownFunctionOneArgument=
  this->IsListNElementsStartingWithAtom(theCommands.opSin(),2) ||
  this->IsListNElementsStartingWithAtom(theCommands.opCos(),2) ||
  this->IsListNElementsStartingWithAtom(theCommands.opArcTan(),2) ||
  this->IsListNElementsStartingWithAtom(theCommands.opArcCos(),2) ||
  this->IsListNElementsStartingWithAtom(theCommands.opSqrt(),2)
  ;

  if(isKnownFunctionOneArgument)
  { double argumentD;
    if (!(*this)[1].EvaluatesToRealDoubleUnderSubstitutions(knownEs, valuesKnownEs, &argumentD))
      return false;
    if (this->IsListNElementsStartingWithAtom(theCommands.opSqrt()))
    { if (argumentD<0)
        return false;
      if (whichDouble!=0)
        *whichDouble= FloatingPoint::sqrt(argumentD);
    }
    if (this->IsListNElementsStartingWithAtom(theCommands.opArcCos()))
    { if (argumentD>1 ||argumentD<-1)
        return false;
      if (whichDouble!=0)
        *whichDouble= FloatingPoint::arccos(argumentD);
    }
    if (this->IsListNElementsStartingWithAtom(theCommands.opSin()))
      if (whichDouble!=0)
        *whichDouble= FloatingPoint::sin(argumentD);
    if (this->IsListNElementsStartingWithAtom(theCommands.opCos()))
      if (whichDouble!=0)
        *whichDouble=FloatingPoint::cos(argumentD);
    if (this->IsListNElementsStartingWithAtom(theCommands.opArcTan()))
      if (whichDouble!=0)
        *whichDouble=FloatingPoint::arctan(argumentD);
    return true;
  }
//  std::cout << "<br>" << this->ToString() << " aint no double! constants are:";
//  for (int i=0; i<knownEs.size; i++)
//    std::cout << "<br>" << knownEs[i].ToString() << " -> " << valuesKnownEs[i];

  return false;
}
