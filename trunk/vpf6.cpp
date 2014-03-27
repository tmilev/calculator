//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
#include "vpfImplementationHeader2Math3_WeylAlgebra.h"
#include "vpfImplementationHeader2Math15_UniversalEnveloping.h"
#include "vpfImplementationHeader2Math6_ModulesSSLieAlgebras.h"
#include "vpfImplementationHeader2Math1_SemisimpleLieAlgebras.h"
#include "vpfImplementationHeader2Math3_FiniteGroups.h"


ProjectInformationInstance ProjectInfoVpf6cpp(__FILE__, "Calculator parser, implementation. ");

Calculator::Calculator()
{ this->theGlobalVariableS=0;
  this->numOutputFiles=0;
  this->flagHideLHS=false;
}

std::string Calculator::WriteDefaultLatexFileReturnHtmlLink
(const std::string& fileContent, bool useLatexDviPSpsTopdf)
{ std::fstream theFile;
  std::stringstream fileName;
  std::stringstream systemCommand1, systemCommand2, systemCommand3;

  fileName << this->PhysicalNameDefaultOutput << this->numOutputFiles;
  XML::OpenFileCreateIfNotPresent(theFile, fileName.str()+".tex", false, true, false);
  theFile << fileContent;
  theFile.flush();
  theFile.close();
  systemCommand1 << " latex -output-directory=" << this->PhysicalPathOutputFolder << " " << fileName.str()+".tex";
  //std::cout << "<br>system command:<br>" << systemCommand1.str();
  this->SystemCommands.AddOnTop(systemCommand1.str());
  if (useLatexDviPSpsTopdf)
  { systemCommand2 << " dvips -o " <<  fileName.str() << ".ps " << fileName.str() << ".dvi";
    //std::cout << "<br>system command:<br>" << systemCommand2.str();
    this->SystemCommands.AddOnTop(systemCommand2.str());
    systemCommand3 << " convert " << fileName.str() << ".ps " << fileName.str() << ".png";
    //std::cout << "<br>system command:<br>" << systemCommand3.str();
    this->SystemCommands.AddOnTop(systemCommand3.str());
  }
  std::stringstream out;
  out << "<img src=\"" << this->DisplayNameDefaultOutput << this->numOutputFiles << ".png\"></img><a href=\"" << this->DisplayNameDefaultOutput
  << this->numOutputFiles << ".png\">" << this->DisplayNameDefaultOutput << this->numOutputFiles << ".png</a>";
  this->numOutputFiles++;
  return out.str();
}

inline int IntIdentity(const int& x)
{ return x;
}

void Expression::MakeProducT(Calculator& owner, const Expression& left, const Expression& right)
{ this->MakeXOX(owner, owner.opTimes(), left, right);
}

void Expression::MakeFunction(Calculator& owner, const Expression& theFunction, const Expression& theArgument)
{ this->reset(owner, 2);
  this->AddChildOnTop(theFunction);
  this->AddChildOnTop(theArgument);
  this->format=this->formatFunctionUseUnderscore;
}

bool Expression::MakeEmptyContext(Calculator& owner)
{ this->reset(owner, 1);
  return this->AddChildAtomOnTop(owner.opContexT());
}

bool Expression::ContextMakeContextWithOnePolyVar(Calculator& owner, const Expression& inputPolyVarE)
{ this->MakeEmptyContext(owner);
  Expression thePolyVars;
  thePolyVars.reset(owner, 2);
  thePolyVars.AddChildAtomOnTop(owner.opPolynomialVariables());
  thePolyVars.AddChildOnTop(inputPolyVarE);
  return this->AddChildOnTop(thePolyVars);
}

bool Expression::MakeContextWithOnePolyVarOneDiffVar(Calculator& owner, const Expression& inputPolyVarE, const Expression& inputDiffVarE)
{ this->MakeEmptyContext(owner);
  return this->ContextSetDiffOperatorVar(inputPolyVarE, inputDiffVarE);
}

bool Expression::MakeContextSSLieAlg(Calculator& owner, const SemisimpleLieAlgebra& theSSLiealg)
{ this->MakeEmptyContext(owner);
  return this->ContextSetSSLieAlgebrA(owner.theObjectContainer.theLieAlgebras.AddNoRepetitionOrReturnIndexFirst(theSSLiealg), owner);
}

bool Expression::ContextMakeContextWithOnePolyVar(Calculator& owner, const std::string& inputPolyVarName)
{ Expression varNameE;
  varNameE.MakeAtom(owner.AddOperationNoRepetitionOrReturnIndexFirst(inputPolyVarName), owner);
  return this->ContextMakeContextWithOnePolyVar(owner, varNameE);
}

bool Expression::MakeSqrt(Calculator& owner, const Rational& argument, const Rational& radicalSuperIndex)
{ this->reset(owner,3);
  Expression radicalIndexE, argumentE;
  radicalIndexE.AssignValue(radicalSuperIndex, owner);
  argumentE.AssignValue(argument, owner);
  this->AddChildAtomOnTop(owner.opSqrt());
  this->AddChildOnTop(radicalIndexE);
  this->AddChildOnTop(argumentE);
  return true;
}

bool Expression::MakeXOX(Calculator& owner, int theOp, const Expression& left, const Expression& right)
{ if (&left==this || &right==this)
  { Expression leftCopy=left;
    Expression rightCopy=right;
    return this->MakeXOX(owner, theOp, leftCopy, rightCopy);
  }
  this->reset(owner, 3);
  this->theData=owner.opLisT();
  this->AddChildAtomOnTop(theOp);
  this->AddChildOnTop(left);
  return this->AddChildOnTop(right);
}

bool Expression::MakeOX(Calculator& owner, int theOp, const Expression& opArgument)
{ if (&opArgument==this)
  { Expression copyE=opArgument;
    return this->MakeOX(owner, theOp, copyE);
  }
  this->reset(owner);
  this->theData=owner.opLisT();
  this->AddChildAtomOnTop(theOp);
  return this->AddChildOnTop(opArgument);
}

bool Expression::Sequencefy()
{ this->CheckInitialization();
  if (this->IsSequenceNElementS())
    return true;
  return this->MakeOX(*this->theBoss, this->theBoss->opSequence(), *this);
}

bool Calculator::GetVectorExpressions(const Expression& input, List<Expression>& output, int targetDimNonMandatory)
{ MacroRegisterFunctionWithName("Calculator::GetVectorExpressions");
  output.ReservE(input.children.size);
  output.SetSize(0);
  if (!input.IsSequenceNElementS())
  { if (targetDimNonMandatory>0)
      if (targetDimNonMandatory!=1)
      { this->Comments << "<hr>GetVector failure: target dim is " << targetDimNonMandatory << " but the input " << input.ToString()
        << " can only be interpretted as a single element";
        return false;
      }
    output.AddOnTop(input);
    return true;
  }
  if (targetDimNonMandatory>0)
    if (targetDimNonMandatory!=input.children.size-1)
    { this->Comments << "<hr>Failed to GetVector: the input is required to have " << targetDimNonMandatory << " columns but it has "
      << input.children.size-1 << " columns instead. <hr>";
      return false;
    }
  targetDimNonMandatory=input.children.size-1;
  for (int i=0; i<targetDimNonMandatory; i++)
    output.AddOnTop(input[i+1]);
  return true;
}

std::string BoundVariablesSubstitution::ToString()
{ std::stringstream out;
  out << "the pairs: ";
  for (int i=0; i<this->theBoundVariables.size; i++)
    out << this->theBoundVariables[i].ToString() << "->" << this->variableImages[i].ToString() << "<br>";
  return out.str();
}

template <class coefficient>
bool ModuleSSalgebra<coefficient>::IsNotInParabolic(int theGeneratorIndex)
{ Vector<Rational> theWeight=  this->GetOwner().GetWeightOfGenerator(theGeneratorIndex);
  for (int j=0; j<this->parabolicSelectionNonSelectedAreElementsLevi.CardinalitySelection; j++)
    if (!theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[j]]<0)
      return true;
  return false;
}

template <class coefficient>
bool ModuleSSalgebra<coefficient>::IsNotInLevi(int theGeneratorIndex)
{ Vector<Rational> theWeight=  this->GetOwner().GetWeightOfGenerator(theGeneratorIndex);
  for (int j=0; j<this->parabolicSelectionNonSelectedAreElementsLevi.CardinalitySelection; j++)
    if (!theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[j]].IsEqualToZero())
      return true;
  return false;
}

template <class coefficient>
void ModuleSSalgebra<coefficient>::GetGenericUnMinusElt
 (bool shiftPowersByNumVarsBaseField, ElementUniversalEnveloping<RationalFunctionOld>& output, GlobalVariables& theGlobalVariables)
{ List<ElementUniversalEnveloping<coefficient> > eltsNilrad;
  this->GetElementsNilradical(eltsNilrad, true);
  RationalFunctionOld tempRF;
  output.MakeZero(*this->theAlgebras, this->indexAlgebra);
  MonomialUniversalEnveloping<RationalFunctionOld> tempMon;
  tempMon.MakeConst(*this->theAlgebras, this->indexAlgebra);
  int varShift=0;
  if (shiftPowersByNumVarsBaseField)
    varShift=this->GetMinNumVars();
  int numVars=varShift+eltsNilrad.size;
  for (int i=0; i<eltsNilrad.size; i++)
  { tempRF.MakeOneLetterMoN(i+varShift, 1, &theGlobalVariables);
    tempMon.MultiplyByGeneratorPowerOnTheRight(eltsNilrad[i][0].generatorsIndices[0], tempRF);
  }
  tempRF.MakeOne(&theGlobalVariables);
  output.AddMonomial(tempMon, tempRF);
}

template <class coefficient>
void ModuleSSalgebra<coefficient>::GetGenericUnMinusElt
(bool shiftPowersByNumVarsBaseField, ElementUniversalEnveloping<Polynomial<Rational> >& output, GlobalVariables& theGlobalVariables)
{ List<ElementUniversalEnveloping<coefficient> > eltsNilrad;
  this->GetElementsNilradical(eltsNilrad, true);
  Polynomial<Rational> tempRF;
  output.MakeZero(*this->owneR);
  MonomialUniversalEnveloping<Polynomial<Rational> > tempMon;
  tempMon.MakeOne(*this->owneR);
  int varShift=0;
  if (shiftPowersByNumVarsBaseField)
    varShift=this->GetMinNumVars();
  int numVars=varShift+eltsNilrad.size;
  for (int i=0; i<eltsNilrad.size; i++)
  { tempRF.MakeMonomiaL(i+varShift, 1, 1, numVars);
    tempMon.MultiplyByGeneratorPowerOnTheRight(eltsNilrad[i][0].generatorsIndices[0], tempRF);
  }
  tempRF.MakeOne(numVars);
  output.AddMonomial(tempMon, tempRF);
}

class quasiDiffMon
{
  friend std::ostream& operator << (std::ostream& output, const quasiDiffMon& theMon)
  { output << theMon.ToString();
    return output;
  }
  public:
  MonomialWeylAlgebra theWeylMon;
  MonomialMatrix theMatMon;
  static unsigned int HashFunction(const quasiDiffMon& input)
  { return input.theWeylMon.HashFunction()*SomeRandomPrimes[0]+input.theMatMon.HashFunction()*SomeRandomPrimes[1];
  }
  unsigned int HashFunction()const
  { return HashFunction(*this);
  }
  bool IsMonEqualToZero()const
  { return this->theMatMon.IsMonEqualToZero();
  }
  bool operator==(const quasiDiffMon& other)const
  { return this->theWeylMon==other.theWeylMon && this->theMatMon==other.theMatMon;
  }
  void operator=(const quasiDiffMon& other)
  { this->theWeylMon=other.theWeylMon;
    this->theMatMon=other.theMatMon;
  }
  bool operator>(const quasiDiffMon& other)const
  { if (this->theMatMon==other.theMatMon)
      return this->theWeylMon>other.theWeylMon;
    return this->theMatMon>other.theMatMon;
  }
  std::string ToString(FormatExpressions* theFormat=0)const;
};

std::string quasiDiffMon::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  out << this->theWeylMon.ToString(theFormat) << "\\otimes ";
  out << this->theMatMon.ToString(theFormat);
  return out.str();
}

template <class coefficient>
class quasiDiffOp : public MonomialCollection<quasiDiffMon, coefficient>
{
public:
  std::string ToString(FormatExpressions* theFormat=0)const;
  void GenerateBasisLieAlgebra(List<quasiDiffOp<coefficient> >& theElts, FormatExpressions* theFormat=0, GlobalVariables* theGlobalVariables=0);
  void operator*=(const quasiDiffOp<coefficient>& standsOnTheRight);
  void operator=(const  MonomialCollection<quasiDiffMon, coefficient>& other)
  { this->MonomialCollection<quasiDiffMon, coefficient>::operator=(other);
  }
  void LieBracketMeOnTheRight(const MonomialCollection<quasiDiffMon, coefficient>& standsOnTheRight)
  { quasiDiffOp<coefficient> tempRight;
    tempRight=standsOnTheRight;
    MathRoutines::LieBracket(*this, tempRight, *this);
  }
  void FourierTransformDiffPartOnly(quasiDiffOp<coefficient>& output)const;
  void GetEWAsetMatrixPartsToId(ElementWeylAlgebra<coefficient>& output)const;
};

template <class coefficient>
void quasiDiffOp<coefficient>::FourierTransformDiffPartOnly(quasiDiffOp<coefficient>& output)const
{ if (& output==this)
  { quasiDiffOp<coefficient> thisCopy;
    thisCopy.FourierTransformDiffPartOnly(output);
    return;
  }
  output.MakeZero();
  ElementWeylAlgebra<coefficient> startDO, finalDO;
  quasiDiffMon theMon;
  for (int i=0; i<this->size(); i++)
  { startDO.MakeZero();
    startDO.AddMonomial((*this)[i].theWeylMon, this->theCoeffs[i]);
    startDO.FourierTransform(finalDO);
    for (int j=0; j<finalDO.size(); j++)
    { theMon.theMatMon=(*this)[i].theMatMon;
      theMon.theWeylMon=finalDO[j];
      output.AddMonomial(theMon, finalDO.theCoeffs[j]);
    }
  }
}

template <class coefficient>
void quasiDiffOp<coefficient>::GetEWAsetMatrixPartsToId(ElementWeylAlgebra<coefficient>& output)const
{ output.MakeZero();
  for (int i=0; i<this->size(); i++)
    output.AddMonomial((*this)[i].theWeylMon, this->theCoeffs[i]);
}

template<class Element>
void MathRoutines::LieBracket(const Element& standsOnTheLeft, const Element& standsOnTheRight, Element& output)
{ if (&standsOnTheLeft==&output || &standsOnTheRight==&output)
  { Element standsOnTheLeftNew, standsOnTheRightNew;
    standsOnTheLeftNew=standsOnTheLeft;
    standsOnTheRightNew=standsOnTheRight;
    MathRoutines::LieBracket(standsOnTheLeftNew, standsOnTheRightNew, output);
    return;
  }
//  FormatExpressions tempFormat;
//  tempFormat.polyAlphabeT.SetSize(4);
//  tempFormat.polyAlphabeT[0]="x_1";
//  tempFormat.polyAlphabeT[1]="x_2";
//  tempFormat.polyAlphabeT[2]="\\partial_1";
//  tempFormat.polyAlphabeT[3]="\\partial_2";

  Element tempE;
//  std::cout << "<hr>[ " << standsOnTheLeft.ToString(&tempFormat);
//  std::cout << " , " << standsOnTheRight.ToString(&tempFormat) << " ] = ";
  output=standsOnTheLeft;
  output*=standsOnTheRight;
//  std::cout << "<br>intermediate: " << output.ToString(&tempFormat);
  tempE=standsOnTheRight;
  tempE*=standsOnTheLeft;
//  std::cout << "<br>intermediate2: " << tempE.ToString(&tempFormat);
  output-=tempE;
//  std::cout << "<br>finally:" << output.ToString(&tempFormat) << "<hr>";
}

template <class coefficient>
void quasiDiffOp<coefficient>::GenerateBasisLieAlgebra(List<quasiDiffOp<coefficient> >& theElts, FormatExpressions* theFormat, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("quasiDiffOp<coefficient>::GenerateBasisLieAlgebra");
  ProgressReport theReport (theGlobalVariables);
  HashedList<quasiDiffMon> bufferMons;
  //std::cout << "<br> the elts:" << theElts.ToString();
  List< MonomialCollection<quasiDiffMon, coefficient> > theEltsConverted;
  theEltsConverted=theElts;
  //std::cout << "<br>the elts converted: " << theEltsConverted.ToString();
  this->GaussianEliminationByRows(theEltsConverted);
  quasiDiffOp tempQDO;
  bool foundNew=true;
  int numTimesEliminationWasExecuted=0;
  while (foundNew)
  { foundNew=false;
    for(int i=0; i<theEltsConverted.size; i++)
      for (int j=i+1; j<theEltsConverted.size; j++)
      { tempQDO=theEltsConverted[i];
        std::stringstream report;
        report << "Lie bracketing elements " << " of indices " << i+1 << " and " << j+1 << " out of " << theEltsConverted.size << "<br> "
        << tempQDO.ToString(theFormat) << "<br> with element <br>" << theEltsConverted[j].ToString(theFormat) << " to get <br>";
        //      std::cout << "<hr>Lie bracketing " << tempQDO.ToString(&tempFormat)
  //      << " and " << theEltsConverted[j].ToString(&tempFormat);
        tempQDO.LieBracketMeOnTheRight(theEltsConverted[j]);
        theReport.Report(report.str());
        report << tempQDO.ToString(theFormat);
        theReport.Report(report.str());
  //      std::cout << ", to get " << tempQDO.ToString(&tempFormat);
        theEltsConverted.AddOnTop(tempQDO);
        quasiDiffOp::GaussianEliminationByRows(theEltsConverted, 0, &bufferMons);
        numTimesEliminationWasExecuted++;
        if (!theEltsConverted.LastObject()->IsEqualToZero())
          foundNew=true;
        for (int k=theEltsConverted.size-1; k>=0; k--)
          if (theEltsConverted[k].IsEqualToZero())
            theEltsConverted.RemoveIndexSwapWithLast(k);
          else
            break;
      }
  }
  std::cout << "<hr>" << "<b>Num times Gaussian Elimination was called = " << numTimesEliminationWasExecuted << ".</b>";
  theElts.SetSize(theEltsConverted.size);
  for (int i=0; i<theEltsConverted.size; i++)
    theElts[i]=theEltsConverted[i];
//  std::cout << "<hr>the elts at end: ";
//  theElts=theEltsConverted;
//  for (int i=0; i<theElts.size; i++)
//    std::cout << "<br>" << theElts[i].ToString();
}

template <class coefficient>
void quasiDiffOp<coefficient>::operator*=(const quasiDiffOp<coefficient>& standsOnTheRight)
{ quasiDiffOp<coefficient> output;
  ElementWeylAlgebra<Rational> leftElt, rightElt, tempElt;
  quasiDiffMon outputMon;
  output.MakeZero();
  for (int j=0; j<standsOnTheRight.size(); j++)
  { rightElt.MakeZero();
    rightElt.AddMonomial(standsOnTheRight[j].theWeylMon, standsOnTheRight.theCoeffs[j]);
    for (int i=0; i<this->size(); i++)
    { leftElt.MakeZero();
      leftElt.AddMonomial((*this)[i].theWeylMon, this->theCoeffs[i]);
      outputMon.theMatMon= (*this)[i].theMatMon;
      outputMon.theMatMon*=standsOnTheRight[j].theMatMon;
      leftElt*=rightElt;
      for (int k=0; k<leftElt.size(); k++)
      { outputMon.theWeylMon=leftElt[k];
        output.AddMonomial(outputMon, leftElt.theCoeffs[k]);
      }
    }
  }
  *this=output;
}

template <class coefficient>
std::string quasiDiffOp<coefficient>::ToString(FormatExpressions* theFormat)const
{ bool combineWeylPart=true;
  if (theFormat!=0)
    combineWeylPart=theFormat->flagQuasiDiffOpCombineWeylPart;
  if (!combineWeylPart)
    return this->MonomialCollection<quasiDiffMon, coefficient>::ToString(theFormat);
  MatrixTensor<ElementWeylAlgebra<Rational> > reordered;
  reordered.MakeZero();
  ElementWeylAlgebra<Rational> tempP;
  for (int i=0; i<this->size(); i++)
  { const quasiDiffMon& currentMon=(*this)[i];
    tempP.MakeZero();
    tempP.AddMonomial(currentMon.theWeylMon, this->theCoeffs[i]);
    reordered.AddMonomial(currentMon.theMatMon, tempP);
  }
  std::string result=reordered.ToString(theFormat);
  if (result=="0" && this->size()!=0)
    crash << "This is likely a programming error (crashing at any rate): I have a non-zero quasidifferential operator "
    << " with non-properly formatted LaTeX string " << this->MonomialCollection<quasiDiffMon, coefficient>::ToString(theFormat)
    << ", however its properly formatted string is 0. Probably there is something wrong with the initializations of the monomials of the qdo. " << crash;
  return result;
}

template <class coefficient>
bool ModuleSSalgebra<coefficient>::GetActionEulerOperatorPart(const MonomialP& theCoeff, ElementWeylAlgebra<Rational>& outputDO, GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("ModuleSSalgebra_CoefficientType::GetActionMonGenVermaModuleAsDiffOperator");
//  int varShift=this->GetMinNumVars();
//  std::cout << "<br>varShift for Euler operator: " << varShift;
  int powerMonCoeff=0;
  ElementWeylAlgebra<Rational> currentMonContribution;
  outputDO.MakeOne();
//  std::cout << "<br>Getting Euler part contribution of " << theCoeff.ToString()
//  <<  " with min num vars equal to " << theCoeff.GetMinNumVars();
  for (int i=0; i<theCoeff.GetMinNumVars(); i++)
  { if (!theCoeff(i).IsSmallInteger(&powerMonCoeff))
      crash << "This is a programming error. Getting euler operator part of action on generalized Verma module: I have an exponent with non-small integer entry. "
      << crash;
    currentMonContribution.Makexidj(i, i, 0);
    currentMonContribution.RaiseToPower(powerMonCoeff);
    outputDO*=currentMonContribution;
    //std::cout << "<br>Accounted index " << i+1 << "  out of " << theCoeff.GetMinNumVars()
    //<< ", power is " << powerMonCoeff << ", current output DO has "
    //<< outputDO.size << " monomials.";
  }
//  std::cout << ".. Final euler: "  << outputDO.ToString();
  return true;
}

template <class coefficient>
bool ModuleSSalgebra<coefficient>::GetActionGenVermaModuleAsDiffOperator
(ElementSemisimpleLieAlgebra<Rational>& inputElt, quasiDiffOp<Rational>& output, GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("ModuleSSalgebra_CoefficientType::GetActionGenVermaModuleAsDiffOperator");
  List<ElementUniversalEnveloping<coefficient> > eltsNilrad;
  List<int> indicesNilrad;
  this->GetElementsNilradical(eltsNilrad, true, &indicesNilrad);
  ElementUniversalEnveloping<Polynomial<Rational> > theGenElt, result;
  this->GetGenericUnMinusElt(true, theGenElt, theGlobalVariables);
//  Polynomial<Rational> Pone, Pzero;
  result.AssignElementLieAlgebra(inputElt, *this->owneR, 1, 0);
  std::stringstream out;
//  std::cout << "<br>the generic elt:" << CGI::GetHtmlMathSpanPure(theGenElt.ToString());
  theGenElt.Simplify(&theGlobalVariables);
//  std::cout << "<br>the generic elt simplified:" << CGI::GetHtmlMathSpanPure(theGenElt.ToString());
//  std::cout << "<br>" << CGI::GetHtmlMathSpanPure(result.ToString() ) << " times " << CGI::GetHtmlMathSpanPure(theGenElt.ToString()) << " = ";
  result*=(theGenElt);
  result.Simplify(&theGlobalVariables);
//  std::cout << " <br>" << CGI::GetHtmlMathSpanPure(result.ToString());
  MatrixTensor<Polynomial<Rational> > endoPart, tempMT, idMT;
  idMT.MakeIdSpecial();
  MatrixTensor<RationalFunctionOld> tempMat1;

  int varShift=this->GetMinNumVars();
//  std::cout  << "<br>Num elements nilrad: " << indicesNilrad.size;
  ElementWeylAlgebra<Rational> weylPartSummand, exponentContribution, oneIndexContribution,
  eulerOperatorContribution;
  Polynomial<Rational> tempP1, negativeExponentDenominatorContribution, theCoeff;
  quasiDiffMon monQDO, monQDO2;
  //static int problemCounter=0;
  //problemCounter++;
  Rational tempRat;
  output.MakeZero();
  Rational currentShift;
  for (int i=0; i<result.size(); i++)
  { //problemCounter++;
    const MonomialUniversalEnveloping<Polynomial<Rational> >& currentMon=result[i];
    endoPart=idMT;
    for (int j=currentMon.Powers.size-1; j>=indicesNilrad.size; j--)
    { int thePower=0;
      if (!currentMon.Powers[j].IsSmallInteger(&thePower))
        return false;
      tempMat1=this->GetActionGeneratorIndeX(currentMon.generatorsIndices[j], theGlobalVariables);
      tempMT.MakeZero();
      for (int k=0; k<tempMat1.size(); k++)
      { if (tempMat1.theCoeffs[k].expressionType==RationalFunctionOld::typeRationalFunction)
          return false;
        tempMat1.theCoeffs[k].GetNumerator(tempP1);
        tempMT.AddMonomial(tempMat1[k], tempP1);
      }
      MathRoutines::RaiseToPower(tempMT, thePower, idMT);
      endoPart*=tempMT;
    }
    exponentContribution.MakeOne();
    theCoeff=result.theCoeffs[i];
    for (int j=0; j<indicesNilrad.size; j++)
    { //if (problemCounter==249)
        //std::cout << "ere be problem!";
      //problemCounter++;
      currentMon.Powers[j].GetConstantTerm(currentShift);
      ElementWeylAlgebra<Rational>::GetStandardOrderDiffOperatorCorrespondingToNraisedTo
      (currentShift, j+varShift, oneIndexContribution, negativeExponentDenominatorContribution, theGlobalVariables);
//      std::cout << "<br>result from GetStandardOrderDiffOperatorCorrespondingToNraisedTo: "
//      << negativeExponentDenominatorContribution.ToString() << " divided by "
//      << oneIndexContribution.ToString();
      exponentContribution*=oneIndexContribution;
      theCoeff.DivideBy(negativeExponentDenominatorContribution, theCoeff, tempP1);
      if (!tempP1.IsEqualToZero())
        crash << "This is a mathematical error! Something is very wrong with embedding semisimple Lie algebras in Weyl algebras. " << crash;
    }
//    std::cout << "<br>Endo part of " << currentMon.ToString() << ": " << endoPart.ToString();
//    std::cout << "<br>Exponent contribution of " << currentMon.ToString() << ": "
//    << exponentContribution.ToString();
    for (int l=0; l<theCoeff.size(); l++)
    { //problemCounter++;
      //if (problemCounter==249)
        //std::cout << "ere be problem!";
      if (!this->GetActionEulerOperatorPart(theCoeff[l], eulerOperatorContribution, theGlobalVariables))
        return false;
//      std::cout << "<br>Euler operator contribution: " << eulerOperatorContribution.ToString();
      weylPartSummand=exponentContribution;
      weylPartSummand*=eulerOperatorContribution;
      weylPartSummand*=theCoeff.theCoeffs[l];
      for (int j=0; j<weylPartSummand.size(); j++)
        for (int k=0; k<endoPart.size(); k++)
        { monQDO.theMatMon=endoPart[k];
          monQDO.theWeylMon=weylPartSummand[j];
          Polynomial<Rational>& currentEndoCoeff=endoPart.theCoeffs[k];
          for (int m=0; m<currentEndoCoeff.size(); m++)
          { monQDO2=monQDO;
            monQDO2.theWeylMon.polynomialPart*=currentEndoCoeff[m];
            tempRat=currentEndoCoeff.theCoeffs[m];
            tempRat*=weylPartSummand.theCoeffs[j];
  //          if (problemCounter==5)
  //          std::cout << "<br>adding " << monQDO.ToString() << " times "
  //          << tempRat.ToString()  << " to "
  //          << output.ToString();
            output.AddMonomial(monQDO2, tempRat);
  //          if (problemCounter==5)
  //          std::cout << " to get " << output.ToString();
          }
        }
    }
  }
//  std::cout << "<br>result qdo: " << output.ToString();
  return true;
}

bool Calculator::innerWriteGenVermaModAsDiffOperatorInner
(Calculator& theCommands, const Expression& input, Expression& output, Vectors<Polynomial<Rational> >& theHws, Expression& hwContext,
 Selection& selInducing, SemisimpleLieAlgebra* owner, bool AllGenerators, std::string* xLetter, std::string* partialLetter, std::string* exponentVariableLetter)
{ MacroRegisterFunctionWithName("Calculator::innerWriteGenVermaModAsDiffOperatorInner");
   /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  if (theHws.size==0)
    return false;
  Expression tempExp;
  SemisimpleLieAlgebra& theSSalgebra=*owner;
  List<ElementUniversalEnveloping<Polynomial<Rational> > > elementsNegativeNilrad;
  ElementSemisimpleLieAlgebra<Rational> theGenerator;
  ElementUniversalEnveloping<Polynomial<Rational> > genericElt, actionOnGenericElt;
  List<ElementWeylAlgebra<Rational> > actionNilrad;
  List<int> vectorIndices, dualIndices;
  List<quasiDiffOp<Rational> > theQDOs;
  FormatExpressions theWeylFormat, theUEformat;
  std::stringstream out, latexReport, latexReport2;
  theWeylFormat.MaxLineLength=40;
  theWeylFormat.flagUseLatex=true;
  theUEformat.MaxLineLength=20;
  theUEformat.flagUseLatex=true;
  theUEformat.chevalleyGgeneratorLetter="g";
  theUEformat.chevalleyHgeneratorLetter="h";
  hwContext.ContextGetFormatExpressions(theUEformat);
  theUEformat.polyDefaultLetter= exponentVariableLetter==0  ? "a" : *exponentVariableLetter;
  theUEformat.MaxLineLength=178;
  theUEformat.NumAmpersandsPerNewLineForLaTeX=2;
  theWeylFormat.NumAmpersandsPerNewLineForLaTeX=2;
  hwContext.ContextGetFormatExpressions(theWeylFormat);
  List<ElementSemisimpleLieAlgebra<Rational> > theGeneratorsItry;
  if (!AllGenerators)
    for (int j=0; j<theSSalgebra.GetRank(); j++)
    { Vector<Rational> ei;
      ei.MakeEi(theSSalgebra.GetRank(), j);
      theGenerator.MakeGGenerator(ei, theSSalgebra);
      theGeneratorsItry.AddOnTop(theGenerator);
      theGenerator.MakeHgenerator(ei, theSSalgebra);
      theGeneratorsItry.AddOnTop(theGenerator);
      ei.Minus();
      theGenerator.MakeGGenerator(ei, theSSalgebra);
      theGeneratorsItry.AddOnTop(theGenerator);
    }
  else
    for (int j=0; j<theSSalgebra.GetNumGenerators(); j++)
    { theGenerator.MakeGenerator(j, theSSalgebra);
      theGeneratorsItry.AddOnTop(theGenerator);
    }
  theQDOs.SetSize(theGeneratorsItry.size);
/*  if (false)
    if (theSSalgebra.GetRank()==3 && theSSalgebra.theWeyl.WeylLetter=='B')
    { theGenerator=theGeneratorsItry[0];
      theGenerator+=theGeneratorsItry[2];
      theGeneratorsItry.AddOnTop(theGenerator);
    }*/
  out << "<table border=\"1\">";
  latexReport << "\\begin{longtable}{rll";
  for (int i =0; i<theGeneratorsItry.size; i++)
    latexReport << "l";
  latexReport << "}\\caption{\\label{tableDiffOps" << selInducing.ToString() << "} Differential operators corresponding to actions"
  << " of simple positive generators for the " << selInducing.ToString() << "-parabolic subalgebra.}\\\\<br>";
  List<ModuleSSalgebra<RationalFunctionOld > > theMods;
  theMods.SetSize(theHws.size);
  Vector<RationalFunctionOld> tempV;
  int numStartingVars=hwContext.ContextGetNumContextVariables();
  //std::cout << "<br>num starting vars:" << numStartingVars;
  std::stringstream reportFourierTransformedCalculatorCommands, reportCalculatorCommands;
  for (int i=0; i<theHws.size; i++)
  { ModuleSSalgebra<RationalFunctionOld>& theMod=theMods[i];
    tempV=theHws[i];
    if (!theMod.MakeFromHW(theSSalgebra, tempV, selInducing, *theCommands.theGlobalVariableS, 1, 0, 0, true))
      return output.SetError("Failed to create module.", theCommands);
    if (i==0)
    { theMod.GetElementsNilradical(elementsNegativeNilrad, true);
      Polynomial<Rational> Pone, Pzero;
      Pone.MakeOne(elementsNegativeNilrad.size+theMod.GetMinNumVars());
      Pzero.MakeZero();
      theMod.GetGenericUnMinusElt(true, genericElt, *theCommands.theGlobalVariableS);
      //std::cout << "<br>highest weight: " << tempV.ToString();
      //std::cout << "<br>generic elt: " <<  genericElt.ToString();

      //std::cout << "<br>theWeylFormat: ";
//      for (int k=0; k<theWeylFormat.polyAlphabeT.size; k++)
//        std::cout << theWeylFormat.polyAlphabeT[k] << ", ";
      theWeylFormat.polyAlphabeT.SetSize(numStartingVars+ elementsNegativeNilrad.size);
      theWeylFormat.weylAlgebraLetters.SetSize(numStartingVars+ elementsNegativeNilrad.size);
      theUEformat.polyAlphabeT.SetSize(numStartingVars+ elementsNegativeNilrad.size);
      for (int k=0; k<numStartingVars; k++)
        theWeylFormat.weylAlgebraLetters[k]="error";
      //std::cout << "<br>HW num context vars: " << hwContext.ContextGetNumContextVariables();
      std::string theFinalXletter= (xLetter==0) ? "x": *xLetter;
      std::string theFinalPartialLetter=(partialLetter==0) ? "\\partial" : *partialLetter;
      for (int k=numStartingVars; k<theUEformat.polyAlphabeT.size; k++)
      { std::stringstream tmpStream, tempstream2, tempstream3, tempStream4;
        tmpStream << theUEformat.polyDefaultLetter << "_{" << k-hwContext.ContextGetNumContextVariables()+1 << "}";
        theUEformat.polyAlphabeT[k] = tmpStream.str();
        tempstream2 << theFinalXletter << "_{" << k-numStartingVars+1 << "}";
        tempstream3 << theFinalXletter << "_" << k-numStartingVars+1;
        tempStream4 << theFinalPartialLetter << "_{" << k-numStartingVars+1 << "}";
        if (theWeylFormat.polyAlphabeT.Contains(tempstream2.str()) || theWeylFormat.polyAlphabeT.Contains(tempstream3.str()))
          return output.SetError
          ("Error: the variable "+ tempstream2.str()+" is reserved for me: you are not allowed to use it as a coordinate of the highest weight. ", theCommands);
        theWeylFormat.polyAlphabeT[k]=tempstream2.str();
        theWeylFormat.weylAlgebraLetters[k]=tempStream4.str();
      }
//      std::cout << "<br>theUEformat: ";
//      for (int k=0; k<theUEformat.polyAlphabeT.size; k++)
//        std::cout << theUEformat.polyAlphabeT[k] << ", ";
      out << "<tr><td>General monomial in U(n_-):</td><td>" << CGI::GetMathMouseHover(genericElt.ToString(&theUEformat)) << "</td> </tr>";
      latexReport << "& \\multicolumn{" << theGeneratorsItry.size << "}{c}{Element acting}\\\\<br>\n ";
      latexReport << "Action on ";
      out << "<tr><td></td><td colspan=\"" << theGeneratorsItry.size << "\"> Element acting</td></td></tr>";
      out << "<tr><td>Action on</td>";
      for (int j=0; j<theGeneratorsItry.size; j++)
      { out << "<td>" << theGeneratorsItry[j].ToString(&theUEformat) << "</td>";
        latexReport << "& $" << theGeneratorsItry[j].ToString(&theUEformat)  << "$";
      }
      latexReport << "\\endhead \\hline<br>";
      out << "</tr>";
      out << "<tr><td>" << CGI::GetMathMouseHover(genericElt.ToString(&theUEformat)) << "</td>";
      latexReport << "$" << genericElt.ToString(&theUEformat) << "$";
      for (int j=0; j<theGeneratorsItry.size; j++)
      { actionOnGenericElt.AssignElementLieAlgebra(theGeneratorsItry[j], theSSalgebra, Pone, Pzero);
        actionOnGenericElt*=(genericElt);
        theSSalgebra.OrderSetNilradicalNegativeMost(theMod.parabolicSelectionNonSelectedAreElementsLevi);
        theSSalgebra.OrderSetNilradicalNegativeMostReversed(theMod.parabolicSelectionNonSelectedAreElementsLevi);
        actionOnGenericElt.Simplify(theCommands.theGlobalVariableS);
        theUEformat.NumAmpersandsPerNewLineForLaTeX=2;
        out << "<td>" << CGI::GetMathMouseHover("\\begin{array}{rcl}&&" +actionOnGenericElt.ToString(&theUEformat)+"\\end{array}") << "</td>";
        theUEformat.NumAmpersandsPerNewLineForLaTeX=0;
        latexReport << "& $\\begin{array}{l} " << actionOnGenericElt.ToString(&theUEformat) << "\\end{array}$ ";
      }
      latexReport << "\\\\ \\hline\\hline<br>";
      out << "</tr>";
    }
    out << "<tr><td>" << CGI::GetMathMouseHover(theMod.theChaR.ToString()) << "</td>";
    latexReport2 << "\\begin{longtable}{rll}";
    latexReport2 << "$\\gog$& $n$& element of $\\mathbb W_n$ \\\\\\hline" << "\\multirow{" << theGeneratorsItry.size
    << "}{*}{$" << theSSalgebra.GetLieAlgebraName() << "$}" << " &  \\multirow{"  << theGeneratorsItry.size << "}{*}{"
    << elementsNegativeNilrad.size << "}&";

    latexReport << "$\\begin{array}{r}" << theMod.theChaR.ToString() << "(\\mathfrak{l}) \\\\ \\\\dim:~" << theMod.GetDim() << " \\end{array}$";
    //std::cout << "<hr>hw: " << theMod.theHWFundamentalCoordsBaseField.ToString() << " nilrad elts: " << elementsNegativeNilrad.ToString();
    //std::cout << "<br>generic element: " << genericElt.ToString();
    for (int j=0; j<theGeneratorsItry.size; j++)
    { theGenerator=theGeneratorsItry[j];
      theMod.GetActionGenVermaModuleAsDiffOperator(theGenerator, theQDOs[j], *theCommands.theGlobalVariableS);
      theWeylFormat.CustomCoeffMonSeparator="\\otimes ";
      theWeylFormat.NumAmpersandsPerNewLineForLaTeX=2;
      out << "<td>" << CGI::GetMathMouseHover("\\begin{array}{|r|c|l|}&&"+theQDOs[j].ToString(&theWeylFormat)+"\\end{array}")
      << "</td>";
      theWeylFormat.NumAmpersandsPerNewLineForLaTeX=0;
      theWeylFormat.MaxLineLength=300;
      latexReport << " & $\\begin{array}{l}" << theQDOs[j].ToString(&theWeylFormat) << "\\end{array}$";
      if (j!=0)
        latexReport2 << "&&";
      latexReport2 << " $\\begin{array}{l}" << theQDOs[j].ToString(&theWeylFormat) << "\\end{array}$\\\\ "
      << (j!=theGeneratorsItry.size-1 ? "\\cline{3-3}" : "\\hline" ) << "\n<br>";
      theWeylFormat.CustomCoeffMonSeparator="";
    }
    latexReport2 << "\\end{longtable}";
    latexReport << "\\\\\\hline<br>";
    out << "</tr>";
    if (theMod.GetDim()==1)
    { ElementWeylAlgebra<Rational> diffOpPart, transformedDO;
      reportFourierTransformedCalculatorCommands << "<hr>" << CGI::GetMathMouseHover(theMod.theChaR.ToString())
      << ", differential operators Fourier transformed - formatted for calculator input. <br><br>";
      reportFourierTransformedCalculatorCommands << "x_{{i}}:=ElementWeylAlgebraPoly{}(\\partial_i, x_i);\n<br>"
      << "\\partial_{{i}}:=ElementWeylAlgebraDO{}(\\partial_i, x_i);\n";
      reportCalculatorCommands << "<hr>" << CGI::GetMathMouseHover(theMod.theChaR.ToString())
      << ", differential operators - formatted for calculator input. <br><br>";
      reportCalculatorCommands << "x_{{i}}:=ElementWeylAlgebraPoly{}(\\partial_i, x_i);\n<br>"
      << "\\partial_{{i}}:=ElementWeylAlgebraDO{}(\\partial_i, x_i);\n";

      for (int j=0; j<theGeneratorsItry.size; j++)
      { theQDOs[j].GetEWAsetMatrixPartsToId(diffOpPart);
        diffOpPart.FourierTransform(transformedDO);
        reportFourierTransformedCalculatorCommands << "<br>" << theGeneratorsItry[j].ToString() << ":=" << transformedDO.ToString() << ";";
        reportCalculatorCommands << "<br>" << theGeneratorsItry[j].ToString() << ":=" << diffOpPart.ToString() << ";";
      }
      reportFourierTransformedCalculatorCommands << "<br>GenerateVectorSpaceClosedWRTLieBracket{}(248," ;
      reportCalculatorCommands << "<br>GenerateVectorSpaceClosedWRTLieBracket{}(248," ;
      for (int j=0; j<theGeneratorsItry.size; j++)
      { reportFourierTransformedCalculatorCommands << theGeneratorsItry[j].ToString();
        reportCalculatorCommands << theGeneratorsItry[j].ToString();
        if (j!=theGeneratorsItry.size-1)
        { reportFourierTransformedCalculatorCommands << ", ";
          reportCalculatorCommands << ", ";
        }
      }
      reportFourierTransformedCalculatorCommands << ");";
      reportFourierTransformedCalculatorCommands << "<hr>";
      reportCalculatorCommands << ");";
      reportCalculatorCommands << "<hr>";
    }
//    theQDOs[0].GenerateBasisLieAlgebra(theQDOs, &theWeylFormat, theCommands.theGlobalVariableS);
//    std::cout << "<br><b>Dimension generated Lie algebra: " << theQDOs.size << "</b>";
//    std::cout << "<br>The qdos: ";
//    for (int j=0; j<theQDOs.size; j++)
 //     std::cout << "<br>" << theQDOs[j].ToString();
  }
  latexReport << "\\end{longtable}";
  out << "</table>";
  out << reportCalculatorCommands.str();
  out << reportFourierTransformedCalculatorCommands.str();
  out << "<br>" << latexReport.str();
  out << "<br><br>" << latexReport2.str();
  return output.AssignValue<std::string>(out.str(), theCommands);
}

template <class coefficient>
std::string ModuleSSalgebra<coefficient>::ToString(FormatExpressions* theFormat)const
{ MacroRegisterFunctionWithName("ModuleSSalgebra::ToString");
  if (this->owneR==0)
    return "(Error: module not initialized)";
  SemisimpleLieAlgebra& theAlgebrA=*this->owneR;
  WeylGroup theWeyl;
  theWeyl=theAlgebrA.theWeyl;
  std::stringstream out;
  GlobalVariables theGlobalVariables;
  out << "<br>Semisimple Lie algebra acting on generalized Verma module: " << theAlgebrA.GetLieAlgebraName() << ".";
  out << "<br>Parabolic selection: " << this->parabolicSelectionNonSelectedAreElementsLevi.ToString();
  out << "<br>Highest weight of Generalized Verma module in fundamental coordinates: " << this->theHWFundamentalCoordsBaseField.ToString();
  out << "<br>In simple coordinates: " << this->theHWSimpleCoordSBaseField.ToString();
  out << "<br>Finite dimensional part h. w. fundamental coordinates: " << this->theHWFDpartFundamentalCoordS.ToString();
  out << "<br>Finite dimensinoal part h. w. simple coords: " << this->theHWFDpartSimpleCoordS.ToString();
  out << "<br>Inducing module character (over the Cartan subalgebra): ";
  FormatExpressions latexFormat;
  latexFormat.flagUseLatex=true;
  latexFormat.flagUseHTML=false;
  if (this->theCharOverH.size()<100)
    out << CGI::GetMathMouseHover(this->theCharOverH.ToString(&latexFormat));
  else
    out << this->theCharOverH.ToString();
  out << "<br>Dimensionn of the finite dimensional part of the module: " << this->GetDim();
  out << "<br>A module basis follows.";
  out << "<table><tr><td>Monomial label</td><td>Definition</td><td>Littelmann path string</td></tr>";
  ElementWeylGroup<WeylGroup> tempWelt;
  int wordCounter=0;
  for (int i=0; i<this->theGeneratingWordsGrouppedByWeight.size; i++)
  { List<MonomialUniversalEnveloping<coefficient> >& currentList=this->theGeneratingWordsGrouppedByWeight[i];
    List<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >& currentListInt=this->theGeneratingWordsIntGrouppedByWeight[i];
    for (int j=0; j<currentList.size; j++)
    { wordCounter++;
      tempWelt.generatorsLastAppliedFirst.SetSize(currentListInt[j].generatorsIndices.size);
      for (int k=0; k<currentListInt[j].generatorsIndices.size; k++)
        tempWelt.generatorsLastAppliedFirst[k]=theWeyl.RootsOfBorel.size-1 -currentListInt[j].generatorsIndices[k];
      out << "<tr><td>m_{ " << wordCounter << "} </td><td>" << currentList[j].ToString(&theGlobalVariables.theDefaultFormat)
      << "  v_\\lambda</td><td>" << tempWelt.ToString() << "</td> </tr>";
    }
  }
  out << "</table>";

  out << "<br>Character: " << this->theChaR.ToString();
  out << "<br>Computed generator actions (" << this->ComputedGeneratorActions.CardinalitySelection << " out of "
  << this->actionsGeneratorsMaT.size << " computed actions) follow. Note that generator actions are computed on demand, only the simple "
  << " Chevalley generators are computed by default. ";
  out << "<table><tr><td>Generator </td><td>Action</td></tr>";
  ElementSemisimpleLieAlgebra<Rational> tempSSElt;
  for (int i=0; i<this->actionsGeneratorsMaT.size; i++)
    if (this->ComputedGeneratorActions.selected[i])
    { tempSSElt.MakeGenerator(i, theAlgebrA);
      out << "<tr>";
      out << "<td>" << CGI::GetMathMouseHover(tempSSElt.ToString(theFormat)) << "</td>";
      out << "<td>";
      if (this->GetDim()<28)
      { Matrix<coefficient> outputMat;
        this->actionsGeneratorsMaT[i].GetMatrix(outputMat, this->GetDim());
        //std::cout << outputMat.ToString(&latexFormat);
        out << CGI::GetMathMouseHover(outputMat.ToString(&latexFormat), 5000) << " = ";
        out << this->actionsGeneratorsMaT[i].ToString();
      }
      else
        out << this->actionsGeneratorsMaT[i].ToString();
      out << "</td></tr>";
    }
  out << "</table>";
  std::string tempS;
  DrawingVariables theDV;
  this->theCharOverH.DrawMeAssumeCharIsOverCartan(theWeyl, theGlobalVariables, theDV);
  out << " A picture of the weight support follows. " << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyl.GetDim());

  bool isBad=false;
  for (int k=0; k<this->theBilinearFormsAtEachWeightLevel.size; k++)
  { Matrix<coefficient>& theBF=this->theBilinearFormsAtEachWeightLevel[k];
    Matrix<coefficient>& theBFinverted= this->theBilinearFormsInverted[k];
    out << "<hr>weight in simple coords: "
    << this->theModuleWeightsSimpleCoords[k].ToString();
    List<MonomialUniversalEnveloping<coefficient> >& currentList=this->theGeneratingWordsGrouppedByWeight[k];
    for (int i=0; i<currentList.size; i++)
    { MonomialUniversalEnveloping<coefficient>& currentElt=currentList[i];
      out << "<br>monomial " << i+1 << ": " << currentElt.ToString(&theGlobalVariables.theDefaultFormat);
    }
    out << "; Matrix of Shapovalov form associated to current weight level: <br> " << theBF.ToString(&theGlobalVariables.theDefaultFormat);
/*    if (!theBF.IsPositiveDefinite())
    { monomialDetailStream << "<b>Is not positive definite!</b>";
      this->flagConjectureCholds=false;
    }
    else
      monomialDetailStream << " (positive definite)";*/
    if (!theBF.IsNonNegativeAllEntries())
      out << "<b>Has negative entries</b>";
    else
      out << " (positive entries only )";
    out << " corresonding inverted matrix:<br>";
    if (theBFinverted.NumRows>0)
      out << theBFinverted.ToString(&theGlobalVariables.theDefaultFormat);
    else
    { out << "<b>The matrix of the bilinear form is not invertible!</b>";
      isBad=true;
    }
  }
  if (isBad)
    out << "<br>Error: the Littelmann-path induced monomials do not give a monomial basis. ";
  out << "<br>Cached Shapovalov products before generator action computation: " << this->cachedPairs.size << ". Dimension : " << this->GetDim();
  ElementSemisimpleLieAlgebra<Rational> tempSSelt;
  out << "<br>Cached Shapovalov products final: " << this->cachedPairs.size << "; dimension : " << this->GetDim();
  return out.str();
}

bool Calculator::innerHWVCommon
(Calculator& theCommands, Expression& output, Vector<RationalFunctionOld>& highestWeightFundCoords, Selection& selectionParSel,
 Expression& hwContext, SemisimpleLieAlgebra* owner, bool Verbose)
{ MacroRegisterFunctionWithName("Calculator::innerHWVCommon");
  RecursionDepthCounter therecursionIncrementer(&theCommands.RecursionDeptH);
  //  std::cout << "<br>highest weight in fundamental coords: " << highestWeightFundCoords.ToString() << "<br>";
//  std::cout << "<br>parabolic selection: " << parabolicSel.ToString();
  RationalFunctionOld RFOne, RFZero;
  RFOne.MakeOne(theCommands.theGlobalVariableS);
  RFZero.MakeZero(theCommands.theGlobalVariableS);
  std::string report;
  ElementTensorsGeneralizedVermas<RationalFunctionOld> theElt;
  //=theElementData.theElementTensorGenVermas.GetElement();
  ListReferences<ModuleSSalgebra<RationalFunctionOld> >& theMods= theCommands.theObjectContainer.theCategoryOmodules;
  int indexOfModule=-1;

  for (int i=0; i<theMods.size; i++)
  { ModuleSSalgebra<RationalFunctionOld>& currentMod=theMods[i];
    if (highestWeightFundCoords==currentMod.theHWFundamentalCoordsBaseField && selectionParSel==currentMod.parabolicSelectionNonSelectedAreElementsLevi && currentMod.owneR==owner)
    { indexOfModule=i;
      break;
    }
  }
  if (indexOfModule==-1)
  { indexOfModule=theMods.size;
    theMods.SetSize(theMods.size+1);
    theMods.LastObject().reset();
  }
  ModuleSSalgebra<RationalFunctionOld>& theMod=theMods[indexOfModule];
  if (!theMod.flagIsInitialized)
  { bool isGood=theMod.MakeFromHW(*owner, highestWeightFundCoords, selectionParSel, *theCommands.theGlobalVariableS, RFOne, RFZero, &report);
    if (Verbose)
      theCommands.Comments << theMod.ToString();
    if (!isGood)
      return output.SetError("Error while generating highest weight module. See comments for details. ", theCommands);
  }
  if (&theMod.GetOwner()!=owner)
    crash << "This is a programming error: module has owner that is not what it should be. " << crash;
  theElt.MakeHWV(theMod, RFOne);
  if (&theElt.GetOwnerSS()!=owner)
    crash << "This is a programming error: just created an ElementTensorsGeneralizedVermas whose owner is not what it should be. " << crash;
  return output.AssignValueWithContext<ElementTensorsGeneralizedVermas<RationalFunctionOld> >(theElt, hwContext, theCommands);
}

bool Calculator::innerCrash(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCrash");
  crash << "<hr>This is a test of the crashing mechanism of the calculator. Are log files created correctly? Check the /output/ directory." << crash;
  return output.AssignValue((std::string)"Crashed succesfully", theCommands);
}

bool Calculator::CheckConsistencyAfterInitializationExpressionStackEmpty()
{ this->theExpressionContainer.GrandMasterConsistencyCheck();
  this->ExpressionStack.GrandMasterConsistencyCheck();
  this->cachedExpressions.GrandMasterConsistencyCheck();
  if (this->cachedExpressions.size!=0 || this->imagesCachedExpressions.size!=0 || this->ExpressionStack.size!=0 || this->theExpressionContainer.size!=0)
    crash << "This is a programming error: cached expressions, images cached expressions, expression stack and expression container are supposed to be empty, but "
    << " instead they contain respectively " << this->cachedExpressions.size << ", " << this->imagesCachedExpressions.size << ", "
    << this->ExpressionStack.size << " and " << this->theExpressionContainer.size << " elements. " << crash;
  return true;
}

bool Calculator::innerFunctionToMatrix(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerFunctionToMatrix");
//  std::cout << input.ToString();
  if (!input.IsListNElements(4))
    return false;
  const Expression& leftE  =input[1];
  const Expression& middleE=input[2];
  const Expression& rightE =input[3];
//  std::cout << leftE.ToString() << ", " << rightE.ToString() << ", " << middleE.ToString();
  int numRows, numCols;
  if (!middleE.IsSmallInteger(&numRows) || !rightE.IsSmallInteger(&numCols))
    return false;
//  std::cout << "<br>Rows, cols: " << numRows << ", " << numCols;
  if (numRows<=0 || numCols<=0)
    return false;
  if (numRows>1000 || numCols>1000)
  { theCommands.Comments << "Max number of rows/columns is 1000. You requested " << numRows << " rows and " << numCols << " columns.<br>";
    return false;
  }
  output.reset(theCommands, numRows+1);
  output.AddChildAtomOnTop(theCommands.opSequence());
  output.format=output.formatMatrix;
  Expression theIndices, currentRow, currentE, leftIE, rightIE;
  for (int i=0; i<numRows; i++)
  { currentRow.reset(theCommands, numCols+1);
    currentRow.AddChildAtomOnTop(theCommands.opSequence());
    currentRow.format=currentRow.formatMatrixRow;
    for (int j=0; j<numCols; j++)
    { leftIE.AssignValue(i+1, theCommands);
      rightIE.AssignValue(j+1, theCommands);
      currentE.reset(theCommands, 3);
      currentE.AddChildOnTop(leftE);
      currentE.AddChildOnTop(leftIE);
      currentE.AddChildOnTop(rightIE);
      currentRow.AddChildOnTop(currentE);
    }
    output.AddChildOnTop(currentRow);
  }
  return true;
}

bool Calculator::innerGetChevGen(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerGetChevGen");
  if (!input.IsListNElements(3))
    return false;
  SemisimpleLieAlgebra* theSSalg;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorSerialization::innerSSLieAlgebra, input[1], theSSalg))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  int theIndex;
  if (!input[2].IsSmallInteger(&theIndex))
    return false;
  if (theIndex>theSSalg->GetNumPosRoots() || theIndex==0 || theIndex<-theSSalg->GetNumPosRoots())
    return output.SetError("Bad Chevalley-Weyl generator index.", theCommands);
  ElementSemisimpleLieAlgebra<Rational> theElt;
  if (theIndex>0)
    theIndex+=theSSalg->GetRank()-1;
  theIndex+=theSSalg->GetNumPosRoots();
  theElt.MakeGenerator(theIndex, *theSSalg);
  ElementUniversalEnveloping<RationalFunctionOld> theUE;
  theUE.AssignElementLieAlgebra(theElt, *theSSalg);
  Expression theContext;
  int indexInOwner=theCommands.theObjectContainer.theLieAlgebras.GetIndex(*theSSalg);
  //theCommands.ToString();
  theContext.ContextMakeContextSSLieAlgebrA(indexInOwner, theCommands);
  //theCommands.ToString();
  output.AssignValueWithContext(theUE, theContext, theCommands);
  //theCommands.ToString();
  return true;
}

bool Calculator::innerGetCartanGen(Calculator& theCommands, const Expression& input, Expression& output)
{ //std::cout << "<br>Here I am with input: " << input.ToString();
  if (!input.IsListNElements(3))
    return false;
  SemisimpleLieAlgebra* theSSalg=0;
//  std::cout << "<br>before calling inner ss: " << input.ToString();
//  std::cout.flush();
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorSerialization::innerSSLieAlgebra, input[1], theSSalg))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  if (theSSalg==0)
    crash << "This is a programming error: called conversion function successfully, but the output is a zero pointer to a semisimple Lie algebra. "
    << crash;
//  std::cout << "<br>Here I am at next phase: " << input.ToString() << ", ss alg: " << theSSalg->ToString();
//  std::cout.flush();
  int theIndex;
  if (!input[2].IsSmallInteger(&theIndex))
    return false;
  if (theIndex<1 || theIndex>theSSalg->GetRank() )
    return output.SetError("Bad Cartan subalgebra genrator index.", theCommands);
  ElementSemisimpleLieAlgebra<Rational> theElt;
  Vector<Rational> theH;
  theIndex--;
  theH.MakeEi(theSSalg->GetRank(), theIndex);
  theElt.MakeHgenerator(theH, *theSSalg);
//  std::cout << "<br>good before ue! " << input.ToString();
//  std::cout.flush();
  ElementUniversalEnveloping<RationalFunctionOld> theUE;
  theUE.AssignElementLieAlgebra(theElt, *theSSalg);
  Expression theContext;
  int theAlgIndex=theCommands.theObjectContainer.theLieAlgebras.GetIndex(*theSSalg);
  theContext.ContextMakeContextSSLieAlgebrA(theAlgIndex, theCommands);
  //std::cout << "<br>And the context is: " << theContext.ToString();
  return output.AssignValueWithContext(theUE, theContext, theCommands);
}

bool Calculator::fKLcoeffs(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::fKLcoeffs");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  SemisimpleLieAlgebra* theSSalgebra=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorSerialization::innerSSLieAlgebra, input, theSSalgebra))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  std::stringstream out;
  WeylGroup& theWeyl=theSSalgebra->theWeyl;
  if (theWeyl.GetGroupSizeByFormula()>192)
  { out << "I have been instructed to run only for Weyl groups that have at most 192 elements (i.e. no larger than D_4). "
    << theSSalgebra->GetLieAlgebraName() << " has " << theWeyl.GetGroupSizeByFormula().ToString() << ".";
    return output.AssignValue(out.str(), theCommands);
  }
  FormatExpressions theFormat;
  theFormat.polyAlphabeT.SetSize(1);
  theFormat.polyAlphabeT[0]="q";
  out << "Our notation follows that of the original Kazhdan-Lusztig paper, Representations of Coxeter Groups and Hecke Algebras.<br>";
  out << " The algebra: " << theSSalgebra->GetLieAlgebraName();
  KLpolys theKLpolys;
  theKLpolys.ComputeKLPolys(&theWeyl);
  theFormat.flagUseHTML=true;
  out << theKLpolys.ToString(&theFormat);
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerPrintSSLieAlgebra(Calculator& theCommands, const Expression& input, Expression& output, bool Verbose)
{ MacroRegisterFunctionWithName("Calculator::innerPrintSSLieAlgebra");
  SemisimpleLieAlgebra *tempSSpointer=0;
  input.CheckInitialization();
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorSerialization::innerSSLieAlgebra, input, tempSSpointer))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  SemisimpleLieAlgebra& theSSalgebra=*tempSSpointer;
  WeylGroup& theWeyl=theSSalgebra.theWeyl;
  std::stringstream out;
  FormatExpressions theFormat, latexFormat;
  latexFormat.flagUseLatex=true;
  latexFormat.flagUseHTML=false;
//      theFormat.chevalleyHgeneratorLetter="\\bar{h}";
//      theFormat.chevalleyGgeneratorLetter="\\bar{g}";
  out << "<hr>Lie algebra type: " << theWeyl.theDynkinType << ". ";
  out << "<br>Weyl group size: " << theWeyl.GetGroupSizeByFormula().ToString() << "." << "<br>To get extra details: ";
  std::stringstream tempStream;
  tempStream << "printSemisimpleLieAlgebra{}(" << theWeyl.theDynkinType << ")";
  out << theCommands.GetCalculatorLink(tempStream.str()) << "<br>";
  if (Verbose)
  { out << " The resulting Lie bracket pairing table follows. <hr> " << theSSalgebra.ToString(&theCommands.theGlobalVariableS->theDefaultFormat);
    out << "Ready for LaTeX consumption version of the first three columns: ";
    out << "<br>%Add to preamble: <br>\\usepackage{longtable} <br>%Add to body: <br>"
    << " \\begin{longtable}{ccc}generator & root simple coord. & root $\\varepsilon$-notation \\\\\\hline<br>\n";
    Vector<Rational> tempRoot, tempRoot2;
    ElementSemisimpleLieAlgebra<Rational> tempElt1;
    for (int i=0; i<theSSalgebra.GetNumGenerators(); i++)
    { tempElt1.MakeGenerator(i, theSSalgebra);
      tempRoot=theSSalgebra.GetWeightOfGenerator(i);
      theWeyl.GetEpsilonCoords(tempRoot, tempRoot2);
      out << "$" << tempElt1.ToString(&theFormat) << "$&$"<< tempRoot.ToString() << "$";
      out << "&$" << tempRoot2.ToStringLetterFormat("\\varepsilon") << "$";
      out << "\\\\\n";
    }
    out << "\\end{longtable}" << "<hr>";
  }
  out << "We define the symmetric Cartan matrix <br>by requesting that the entry in the i-th row and j-th column<br> "
  << " be the scalar product of the i^th and j^th roots. Symmetric Cartan matrix:<br>"
  << CGI::GetMathSpanPure(theWeyl.CartanSymmetric.ToString(&latexFormat));
  out << "Let the (i,j)^{th} entry of the symmetric Cartan matrix be a_{ij}. <br> Then we define the symmetric Cartan co-matrix as "
  << " the matrix whose (i,j)^{th} entry equals 4*a_{ij}/(a_{ii}*a_{jj}). In other words, the symmetric Cartan co-matrix is the "
  << "symmetric Cartan matrix of the dual root system. The symmetric Cartan co-matrix equals:<br>"
  << CGI::GetMathSpanPure(theWeyl.CoCartanSymmetric.ToStringLatex());
  out << "<br>The determinant of the symmetric Cartan matrix is: " << theWeyl.CartanSymmetric.GetDeterminant().ToString();
  /*  Rational theRatio;
    for (int j=0; j<theWeyl.GetDim(); j++)
    { theRatio=0;
      for (int i=0; i<theWeyl.RootSystem.size; i++)
      { Rational tempRat=theWeyl.RootScalarCartanRoot(theWeyl.RootSystem[i], theWeyl.RootSystem[j]);
        theRatio+=tempRat*tempRat;
      }
      theRatio.Invert();
      theRatio*=theWeyl.RootScalarCartanRoot(theWeyl.RootSystem[j], theWeyl.RootSystem[j]);
      Rational tempRat=theWeyl.GetKillingDivTraceRatio();
      tempRat.Invert();
  //    std::cout << "<br>" << j+1 << ": " << theRatio.ToString() << "=? " << tempRat.ToString();
    }*/
  //Lattice tempLattice;
  //theWeyl.GetIntegralLatticeInSimpleCoordinates(tempLattice);
  //out << "<br>The integral lattice in simple coordinates is (generated by): " << tempLattice.ToString(true, false);
  //Vectors<Rational> integralRoots, integralRootsEpsForm;
  Vectors<Rational> fundamentalWeights, fundamentalWeightsEpsForm;
  //integralRoots.AssignMatrixRows(tempLattice.basisRationalForm);
  //theWeyl.GetEpsilonCoords(integralRoots, integralRootsEpsForm, theGlobalVariables);
  //out << "<br>The integral lattice generators in epsilon format: " << integralRootsEpsForm.ElementToStringEpsilonForm();
  theWeyl.GetFundamentalWeightsInSimpleCoordinates(fundamentalWeights);
  Vectors<Rational> simpleBasis, simplebasisEpsCoords;
  out << "<hr> Half sum of positive roots: " << theWeyl.rho.ToString();
  Vector<Rational> tempRoot;
  theWeyl.GetEpsilonCoords(theWeyl.rho, tempRoot);
  out << "= " << CGI::GetMathSpanPure(tempRoot.ToStringLetterFormat("\\varepsilon"));
  out << "<hr>The fundamental weights (the j^th fundamental weight has scalar product 1 <br>with the j^th simple root times 2 divided by the root length squared,<br> "
  << " and 0 with the remaining simple roots): ";
  theWeyl.GetEpsilonCoords(fundamentalWeights, fundamentalWeightsEpsForm);
  out << "<table>";
  for (int i=0; i< fundamentalWeights.size; i++)
  { out << "<tr><td>" << fundamentalWeights[i].ToString() << "</td><td> =</td><td> " << CGI::GetMathSpanPure(fundamentalWeightsEpsForm[i].ToStringEpsilonFormat())
    << "</td></tr>";
  }
  out << "</table>";
  out << "<hr>Simple basis in epsilon coordinates: <table>";
  simpleBasis.MakeEiBasis(theWeyl.GetDim());
  theWeyl.GetEpsilonCoords(simpleBasis, simplebasisEpsCoords);
  for (int i=0; i< simplebasisEpsCoords.size; i++)
  { out << "<tr><td>" << simpleBasis[i].ToString() << " </td><td>=</td> <td>" << CGI::GetMathSpanPure(simplebasisEpsCoords[i].ToStringEpsilonFormat())
    << "</td></tr>";
  }
  out << "</table>";
  DynkinSimpleType tempSimpleType;
  if (theWeyl.theDynkinType.IsSimple(&tempSimpleType.theLetter, &tempSimpleType.theRank, &tempSimpleType.CartanSymmetricInverseScale))
    if (tempSimpleType.CartanSymmetricInverseScale==1)
    { //std::cout << "here i am";
      Matrix<Rational> tempM, tempM2;
      theWeyl.theDynkinType.GetEpsilonMatrix(tempM);
      tempM2=tempM;
      tempM2.Transpose();
      tempM2.MultiplyOnTheRight(tempM);
      if (!(tempM2==theWeyl.CartanSymmetric))
        crash << "This is a (non-critical) programming error: the epsilon coordinates of the vectors are incorrect. "
        << "Please fix function DynkinType::GetEpsilonMatrix. The matrix of the epsilon coordinates is " << tempM.ToString()
        << ", the Symmetric Cartan matrix is " << theWeyl.CartanSymmetric.ToString() << ", and the  "
        << "transpose of the epsilon matrix times the epsilon matrix:  " << tempM2.ToString() << ". " << crash;
    }
  if (Verbose)
  { out << theWeyl.ToStringRootsAndRootReflections();
    DrawingVariables theDV;
    theWeyl.DrawRootSystem(theDV, true, *theCommands.theGlobalVariableS, true, 0, true, 0);
    out << "<hr>Below a drawing of the root system in its corresponding Coxeter plane (computed as explained on John Stembridge's website). "
    << "<br>The darker red dots can be dragged with the mouse to rotate the picture.<br>The grey lines are the edges of the Weyl chamber.<br>"
    << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyl.GetDim());
  }
  return output.AssignValue<std::string>(out.str(), theCommands);
}

bool Expression::HasBoundVariables()const
{ if (this->theBoss==0)
    crash << "This is a programming error: calling function HasBoundVariables on non-initialized expression. " << crash;
  RecursionDepthCounter recursionCounter(&this->theBoss->RecursionDeptH);
  MacroRegisterFunctionWithName("Expression::HasBoundVariables");
  if (this->theBoss->RecursionDeptH>this->theBoss->MaxRecursionDeptH)
    crash << "This is a programming error: function HasBoundVariables has exceeded recursion depth limit. " << crash;
  if (this->IsListOfTwoAtomsStartingWith(this->theBoss->opBind()))
    return true;
  for (int i=0; i<this->children.size; i++)
    if ((*this)[i].HasBoundVariables())
      return true;
  return false;
}

bool Calculator::innerNot(Calculator& theCommands, const Expression& input, Expression& output)
{ int theInt;
  if (!input.IsSmallInteger(&theInt))
    return false;
  if (theInt==0)
    return output.AssignValue(1, theCommands);
  return output.AssignValue(0, theCommands);
}

bool Calculator::innerIsInteger(Calculator& theCommands, const Expression& input, Expression& output)
{ if (input.HasBoundVariables())
    return false;
  if (input.IsInteger())
    output.AssignValue(1, theCommands);
  else
    output.AssignValue(0, theCommands);
  return true;
}

bool Calculator::innerIsRational(Calculator& theCommands, const Expression& input, Expression& output)
{ if (input.HasBoundVariables())
    return false;
  if (input.IsOfType<Rational>())
    output.AssignValue(1, theCommands);
  else
    output.AssignValue(0, theCommands);
  return true;
}

bool Calculator::AppendOpandsReturnTrueIfOrderNonCanonical(const Expression& input, List<Expression>& output, int theOp)
{ RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  if (this->RecursionDeptH>this->MaxRecursionDeptH)
    return false;
  bool result=false;
  if (!input.IsListStartingWithAtom(theOp))
    output.AddOnTop(input);
  else
    for (int i=1; i<input.children.size; i++)
    { if (this->AppendOpandsReturnTrueIfOrderNonCanonical(input[i], output, theOp))
        result=true;
      if (i<input.children.size-1 && input[i].IsListStartingWithAtom(theOp) && input[i].children.size>2)
        result=true;
    }
  return result;
}

Expression::FunctionAddress Calculator::GetInnerFunctionFromOp(int theOp, const Expression& left, const Expression& right)
{ crash << "Function Calculator::GetfOp not implemented yet. " << crash;
  return 0;
}

bool Calculator::outerTensor(Calculator& theCommands, const Expression& input, Expression& output)
{ //std::cout << "<br>At start of evaluate standard times: " << theExpression.ToString();
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("Calculator::StandardTensor");
  if (theCommands.outerDistribute
      (theCommands, input, output, theCommands.opPlus(), theCommands.opTensor()))
    return true;
  //std::cout << "<br>After distribute: " << theExpression.ToString();
  if (theCommands.outerAssociate(theCommands, input, output))
    return true;
  if (theCommands.outerExtractBaseMultiplication(theCommands, input, output))
    return true;
//  if (theExpression.children.size!=2)
//    return false;
  //std::cout << "<br>After do associate: " << theExpression.ToString();
  return false;
}

bool Calculator::innerCollectMultiplicands(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerCollectMultiplicands");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
//  std::cout << "<hr>Collecting multiplicands. input: " << input.ToString();
  Expression constPower, thePower;
  const Expression* left= &input[1];
  const Expression* right=&input[2];
  if (*left==*right)
  { constPower.AssignValue(2, theCommands);
    output.MakeXOX(theCommands, theCommands.opThePower(), *left, constPower);
    //std::cout << "<br>output be at first place: " << output.ToString();
    return true;
  }
  for (int i=0; i<2; i++, MathRoutines::swap(left, right))
    if (left->IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
    { if ((*left)[1]==(*right))
      { constPower.AssignValue(1, theCommands);
        thePower.MakeXOX(theCommands, theCommands.opPlus(), (*left)[2], constPower);
        return output.MakeXOX(theCommands, theCommands.opThePower(), *right, thePower);
      }
      if (right->IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
        if ((*left)[1]==(*right)[1])
        { thePower.MakeXOX(theCommands, theCommands.opPlus(), (*left)[2], (*right)[2]);
          output.MakeXOX(theCommands, theCommands.opThePower(), (*left)[1], thePower);
          //std::cout << "<br>output be at second place: " << output.ToString();
          return true;
        }
    }
  return false;
}

bool Calculator::outerCombineFractions(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opPlus(), 3))
    return false;
  const Expression* quotientE=0;
  const Expression* summandE=0;
  if (input[1].IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
  { quotientE=&input[1];
    summandE=&input[2];
  } else if (input[2].IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
  { quotientE=&input[2];
    summandE=& input[1];
  } else
    return false;
  const Expression& quotientNumeratorE=(*quotientE)[1];
  const Expression& quotientDenominatorE=(*quotientE)[2];
  Expression outputSummandE, outputNumE;
  outputSummandE.MakeXOX(theCommands, theCommands.opTimes(), *summandE, quotientDenominatorE);
  outputNumE.MakeXOX(theCommands, theCommands.opPlus(), quotientNumeratorE, outputSummandE);
  return output.MakeXOX(theCommands, theCommands.opDivide(), outputNumE, quotientDenominatorE);
}

bool Calculator::outerCheckRule(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opDefine(), 3))
    return false;
  if (input[1]!=input[2])
    return false;
  std::stringstream out;
  out << "Bad rule: you are asking me to substitute " << input[1] << " by itself (" << input[2] << ")" << " which is an infinite substitution cycle. ";
  return output.SetError(out.str(), theCommands);
}

bool Calculator::innerSubZeroDivAnythingWithZero(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  if (input[1].IsEqualToZero())
    if (!input[2].IsEqualToZero())
      return output.AssignValue(0, theCommands);
  return false;
}

bool Calculator::innerCancelMultiplicativeInverse(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  if (!input[1].IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  if (input[1][2]==input[2])
  { output=input[1][1];
    return true;
  }
  return false;
}

bool Calculator::outerAssociateTimesDivision(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  if (!input[2].IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  Expression newLeftE;
  newLeftE.MakeXOX(theCommands, theCommands.opTimes(), input[1], input[2][1]);
  output.MakeXOX(theCommands, theCommands.opDivide(), newLeftE, input[2][2]);
  return true;
}

bool Calculator::outerAssociate(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(-1, 3))
    return false;
  int theOperation=input[0].theData;
  if (!input[1].IsListNElementsStartingWithAtom(theOperation, 3))
    return false;
  Expression newRight;
  newRight.MakeXOX(theCommands, theOperation, input[1][2], input[2]);
  output.MakeXOX(theCommands, theOperation, input[1][1], newRight);
  return true;
}

bool Calculator::StandardIsDenotedBy(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::StandardIsDenotedBy");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (!input.IsListNElementsStartingWithAtom(theCommands.opIsDenotedBy(), 3))
    return false;
  const Expression& withNotation=input[2];
  const Expression& theNotation=input[1];
  theCommands.Comments << "<br>Registering notation: globally, " << withNotation.ToString() << " will be denoted by "
  << theNotation.ToString();
  theCommands.theObjectContainer.ExpressionNotation.AddOnTop(theNotation.ToString());
  theCommands.theObjectContainer.ExpressionWithNotation.AddOnTop(withNotation);
  output=input;
  output.SetChildAtomValue(0, theCommands.opDefine());
  ////
  if (withNotation.IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
    if (withNotation.GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >().IsHWV())
    { MonomialGeneralizedVerma<RationalFunctionOld>& theElt=
      withNotation.GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >()[0].theMons[0];
      theElt.GetOwner().highestWeightVectorNotation=theNotation.ToString();
    }
  return true;
}

bool Calculator::innerMultiplyByOne(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListStartingWithAtom(theCommands.opTimes()) || input.children.size!=3)
    return false;
  if (!input[1].IsEqualToOne())
    return false;
  output=input[2];
  return true;
}

bool Calculator::outerTimesToFunctionApplication(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::outerTimesToFunctionApplication");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes()))
    return false;
  if (input.children.size<2)
    return false;
  const Expression& firstElt=input[1];
//  std::cout << " <hr>outer times to function ";
  if (!firstElt.IsBuiltInAtom())
  { if (!firstElt.IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
      return false;
    if (!firstElt[1].IsAtomWhoseExponentsAreInterprettedAsFunction())
      return false;
  }
  if (firstElt.IsAtomNotInterprettedAsFunction())
    return false;
  const Expression& secondElt=input[2];
  if (secondElt.IsSequenceNElementS())
  { output=secondElt;
    return output.SetChilD(0, firstElt);
  }
  output=input;
  output.children.RemoveIndexShiftDown(0);
  return true;
}

bool Calculator::outerDistribute(Calculator& theCommands, const Expression& input, Expression& output, int AdditiveOp, int multiplicativeOp)
{ if (theCommands.flagDontDistribute)
    return false;
  if (theCommands.outerLeftDistributeBracketIsOnTheLeft(theCommands, input, output, AdditiveOp, multiplicativeOp))
    return true;
  return theCommands.outerRightDistributeBracketIsOnTheRight(theCommands, input, output, AdditiveOp, multiplicativeOp);
}

bool Calculator::outerLeftDistributeBracketIsOnTheLeft(Calculator& theCommands, const Expression& input, Expression& output, int AdditiveOp, int multiplicativeOp)
{ if (!input.IsListNElementsStartingWithAtom(-1, 3))
    return false;
  int theAdditiveOp=theCommands.opPlus();
  int theMultiplicativeOP=input[0].theData;
  if (!input[1].IsListNElementsStartingWithAtom(theAdditiveOp,3))
    return false;
//  int theFormat=input.format;
  Expression leftE, rightE;
  leftE.MakeXOX(theCommands, theMultiplicativeOP, input[1][1], input[2]);
  rightE.MakeXOX(theCommands, theMultiplicativeOP, input[1][2], input[2]);
  return output.MakeXOX(theCommands, theAdditiveOp, leftE, rightE);
}

bool Calculator::outerRightDistributeBracketIsOnTheRight(Calculator& theCommands, const Expression& input, Expression& output, int AdditiveOp, int multiplicativeOp)
{ if (!input.IsListNElementsStartingWithAtom(-1, 3))
    return false;
  int theAdditiveOp=theCommands.opPlus();
  int theMultiplicativeOP=input[0].theData;
  if (!input[2].IsListNElementsStartingWithAtom(theAdditiveOp, 3))
    return false;
//  int theFormat=input.format;
  Expression leftE, rightE;
  leftE.MakeXOX(theCommands, theMultiplicativeOP, input[1], input[2][1]);
  rightE.MakeXOX(theCommands, theMultiplicativeOP, input[1], input[2][2]);
  return output.MakeXOX(theCommands, theAdditiveOp, leftE, rightE);
}

bool Calculator::CollectSummands(Calculator& theCommands, const Expression& input, MonomialCollection<Expression, Rational>& outputSum)
{ MacroRegisterFunctionWithName("Calculator::CollectSummands");
  List<Expression> summands;
  theCommands.AppendSummandsReturnTrueIfOrderNonCanonical(input, summands);
  Expression oneE; //used to record the constant term
  oneE.AssignValue<Rational>(1, theCommands);
  outputSum.MakeZero();
  for (int i=0; i<summands.size; i++)
  { const Expression* currentSummandNoCoeff;
    currentSummandNoCoeff=&summands[i];
    Rational theCoeff=1;
    if (currentSummandNoCoeff->IsListStartingWithAtom(theCommands.opTimes()))
    { if((*currentSummandNoCoeff)[1].IsOfType<Rational>(&theCoeff))
        currentSummandNoCoeff=&((*currentSummandNoCoeff)[2]);
    } else if (currentSummandNoCoeff->IsOfType<Rational>(&theCoeff))
      currentSummandNoCoeff=&oneE;
    outputSum.AddMonomial(*currentSummandNoCoeff, theCoeff);
  }
//  std::cout << "<hr> before mon sort, mon order: " << outputSum.theMonomials.ToString();
//  if (outputSum.theMonomials[0]>outputSum.theMonomials[1])
//    std::cout << outputSum.theMonomials[0].ToString() << " > " << outputSum.theMonomials[1].ToString();
//  else
//    std::cout << outputSum.theMonomials[0].ToString() << " < " << outputSum.theMonomials[1].ToString();
  outputSum.QuickSortDescending();

//  std::cout << " after mon sort: " << outputSum.theMonomials.ToString();
  return true;
}

bool Calculator::innerAssociateExponentExponent(Calculator& theCommands, const Expression& input, Expression& output)
{ int opPower=theCommands.opThePower();
  if (!input.IsListNElementsStartingWithAtom(opPower, 3))
    return false;
  if (!input[1].IsListNElementsStartingWithAtom(opPower, 3))
    return false;
  Expression tempE;
  tempE.MakeProducT(theCommands, input[1][2], input[2]);
  return output.MakeXOX(theCommands, opPower, input[1][1], tempE);
}

bool Calculator::outerPowerRaiseToFirst(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
    return false;
  if (input[2].IsEqualToOne())
  { std::string tempS;
//    if (input[1].IsOperation(&tempS))
//      if (tempS=="A" || tempS=="B" || tempS=="C" || tempS=="D" || tempS=="E" ||
//          tempS=="F" || tempS=="G")
//        return false;
    output=input[1];
    return true;
  }
  return false;
}

bool Expression::MakeXOdotsOX(Calculator& owner, int theOp, const List<Expression>& input)
{ MacroRegisterFunctionWithName("Expression::MakeXOdotsOX");
  if (input.size==0)
    crash << "This is a programming error: cannot create operation sequence from an empty list. " << crash;
  if (input.size==1)
  { *this=input[0];
    return true;
  }
  this->MakeXOX(owner, theOp, input[input.size-2], *input.LastObject());
  Expression result;
  for (int i=input.size-3; i>=0; i--)
  { result.reset(owner, 3);
    result.AddChildAtomOnTop(theOp);
    result.AddChildOnTop(input[i]);
    result.AddChildOnTop(*this);
    *this=result;
  }
  return true;
}

bool Expression::MakeIdMatrixExpressions(int theDim, Calculator& inputBoss)
{ Matrix<Expression> theMat;
  Expression oneE, zeroE;
  oneE.AssignValue(1, inputBoss);
  zeroE.AssignValue(0, inputBoss);
  theMat.MakeIdMatrix(theDim, oneE, zeroE);
  return this->AssignMatrixExpressions(theMat, inputBoss);
}

bool Expression::MakeSum(Calculator& theCommands, const MonomialCollection<Expression, Rational>& theSum)
{ MacroRegisterFunctionWithName("Expression::MakeSum");
  Expression oneE; //used to record the constant term
  oneE.AssignValue<Rational>(1, theCommands);
  if (theSum.IsEqualToZero())
    return this->AssignValue<Rational>(0, theCommands);
  List<Expression> summandsWithCoeff;
  summandsWithCoeff.SetSize(theSum.size());
  for (int i=0; i<theSum.size(); i++)
  { Expression& current=summandsWithCoeff[i];
    if (theSum[i]==oneE)
      current.AssignValue(theSum.theCoeffs[i], theCommands);
    else if (!theSum.theCoeffs[i].IsEqualToOne())
    { current.reset(theCommands, 3);
      current.AddChildAtomOnTop(theCommands.opTimes());
      current.AddChildValueOnTop(theSum.theCoeffs[i]);
      current.AddChildOnTop(theSum[i]);
    } else
      current=theSum[i];
  }
  if (summandsWithCoeff.size>=2)
    if (summandsWithCoeff[0]>summandsWithCoeff[1] && summandsWithCoeff[1]>summandsWithCoeff[0])
      crash << "This is a pgoramming error: bad comparison! " << crash;
  return this->MakeXOdotsOX(theCommands, theCommands.opPlus(), summandsWithCoeff);
}

bool Calculator::outerPlus(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::outerPlus");
//  theCommands.Comments << "<hr><hr>processing outer plus with input: " << input.Lispify();
  if (!input.IsListNElementsStartingWithAtom(theCommands.opPlus()))
    return false;
  MonomialCollection<Expression, Rational> theSum;
  theCommands.CollectSummands(theCommands, input, theSum);
  return output.MakeSum(theCommands, theSum);
}

bool Calculator::EvaluateIf(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opDefineConditional(), 4))
    return output.SetError("Error: operation :if := takes three arguments.", theCommands);
  Rational conditionRat;
  if (!input[1].IsOfType<Rational>(&conditionRat))
    return false;
  if (conditionRat.IsEqualToOne())
  { output=input[2];
    return true;
  }
  if (conditionRat.IsEqualToZero())
  { output=input[3];
    return true;
  }
  return false;
}

bool Calculator::outerGreaterThan(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElements(3))
    return false;
  const Expression& left=input[1];
  const Expression& right=input[2];
  Rational leftRat, rightRat;
  if (!left.IsOfType<Rational> (&leftRat)|| ! right.IsOfType<Rational> (&rightRat))
    return false;
  if (leftRat>rightRat)
    return output.AssignValue(1, theCommands);
  return output.AssignValue(0, theCommands);
}

bool Calculator::outerLessThan(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElements(3))
    return false;
  const Expression& left=input[1];
  const Expression& right=input[2];
  Rational leftRat, rightRat;
  if (!left.IsOfType<Rational> (&leftRat)|| ! right.IsOfType<Rational>(&rightRat))
    return false;
  if (rightRat>leftRat)
    return output.AssignValue(1, theCommands);
  return output.AssignValue(0, theCommands);
}

bool Calculator::outerEqualEqual(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElements(3))
    return false;
  const Expression& left=input[1];
  const Expression& right=input[2];
  if (left.HasBoundVariables() || right.HasBoundVariables())
    return false;
  if (left==right)
    return output.AssignValue(1, theCommands);
  else
    return output.AssignValue(0, theCommands);
}

bool Calculator::outerUnion(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opUnion()))
    return false;
  int numElts=1;
  for (int i=1; i<input.children.size; i++)
    if (!input[i].IsSequenceNElementS())
      return false;
    else
      numElts+=input[i].children.size-1;
  output.reset(theCommands, numElts);
  output.AddChildAtomOnTop(theCommands.opSequence());
  for (int i=1; i<input.children.size; i++)
    for (int j=1; j<input[i].children.size; j++)
      output.AddChildOnTop(input[i][j]);
  output.format=input.formatDefault;
  return true;
}

bool Calculator::outerUnionNoRepetition(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsLisT())
    return false;
  int numElts=1;
  for (int i=1; i<input.children.size; i++)
    if (!input[i].IsListStartingWithAtom(theCommands.opSequence()))
      return false;
    else
      numElts+=input[i].children.size-1;
  HashedList<Expression> theList;
  List<int> theIndices;
  theList.SetExpectedSize(numElts);
  for (int i=1; i<input.children.size; i++)
    for (int j=1; j<input[i].children.size; j++)
      theList.AddOnTopNoRepetition(input[i][j]);
  theIndices.SetSize(theList.size);
  for (int i=0; i<theList.size; i++)
    theIndices[i]=theCommands.theExpressionContainer.AddNoRepetitionOrReturnIndexFirst(theList[i]);
  output.children.ReservE(numElts);
  output.reset(theCommands, theIndices.size+1);
  output.AddChildAtomOnTop(theCommands.opSequence());
  output.children.AddOnTop(theIndices);
  output.format=output.formatDefault;
  return true;
}

bool Calculator::outerDivide(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::outerDivide");
//  std::cout << "<br>Now I'm here 1! input: " << input.ToString() << " lisp: "
//  << input.Lispify() ;
  if (!input.IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
//  std::cout << "<br>Now I'm here! 2";
  Rational tempRat;
  if (!input[2].IsOfType<Rational>(&tempRat))
    return false;
  if (tempRat.IsEqualToZero())
    return output.SetError("Division by zero. ", theCommands);
  output=input;
  tempRat.Invert();
  output.SetChildAtomValue(0, theCommands.opTimes());
  Expression tempE;
  tempE.AssignValue(tempRat, theCommands);
  return output.SetChilD(2, tempE);
}

bool Calculator::outerMinus(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!(input.IsListNElementsStartingWithAtom(theCommands.opMinus(), 3) || input.IsListNElementsStartingWithAtom(theCommands.opMinus(), 2)) )
    return false;
  Expression tempE, minusOne;
  minusOne.AssignValue(-1, theCommands);
  if (input.children.size==2)
    output.MakeXOX(theCommands, theCommands.opTimes(), minusOne, input[1]);
  else
  { tempE.MakeXOX(theCommands, theCommands.opTimes(), minusOne, input[2]);
    output.MakeXOX(theCommands, theCommands.opPlus(), input[1], tempE);
  }
  return true;
}

void Expression::operator+=(const Expression& other)
{ if (this->theBoss==0 && other.theBoss==0)
  { this->theData+=other.theData;
    if (this->theData!=1 && this->theData!=0)
      crash << "Attempting to add non-initialized expressions" << crash;
    return;
  }
  if (other.theBoss==0)
  { Expression otherCopy;
    otherCopy.AssignValue(other.theData, *this->theBoss);
    (*this)+=otherCopy;
    return;
  }
  if (this->theBoss==0)
    this->AssignValue(this->theData, *other.theBoss);
  if (this->theBoss!=other.theBoss)
    crash << "Error: adding expressions with different owners. " << crash;
  Expression resultE;
  resultE.MakeXOX(*this->theBoss, this->theBoss->opPlus(), *this, other);
  *this=resultE;
}

void Expression::operator*=(const Expression& other)
{ if (this->theBoss==0 && other.theBoss==0)
  { this->theData*=other.theData;
    if (this->theData!=1 && this->theData!=0)
      crash << "Attempting to add non-initialized expressions" << crash;
    return;
  }
  if (other.theBoss==0)
  { Expression otherCopy;
    otherCopy.AssignValue(other.theData, *this->theBoss);
    (*this)*=otherCopy;
    return;
  }
  if (this->theBoss==0)
    this->AssignValue(this->theData, *other.theBoss);
  if (this->theBoss!=other.theBoss)
    crash << "Error: adding expressions with different owners. " << crash;
  Expression resultE;
  resultE.MakeXOX(*this->theBoss, this->theBoss->opTimes(), *this, other);
  *this=resultE;
}

bool Expression::operator==(const Expression& other)const
{ if (this->theBoss!=other.theBoss)
    return false;
  return this->theData==other.theData && this->children==other.children;
}

SemisimpleLieAlgebra* Expression::GetAmbientSSAlgebraNonConstUseWithCaution()const
{ this->CheckInitialization();
  Expression myContext=this->GetContext();
  int indexSSalg=myContext.ContextGetIndexAmbientSSalg();
  if (indexSSalg==-1)
    return 0;
  return &this->theBoss->theObjectContainer.theLieAlgebras.GetElement(indexSSalg);
}

int Expression::ContextGetIndexAmbientSSalg()const
{ //std::cout << "<hr>trying to get ambient algebra from " << this->ToString();
  if (!this->IsContext() )
    return -1;
//  std::cout << ". I have " << this->children.size << " children. ";
  for (int i=1; i<this->children.size; i++)
    if ((*this)[i].IsListNElementsStartingWithAtom(this->theBoss->opSSLieAlg(), 3))
      return (*this)[i][2].theData;
  return -1;
}

void Expression::GetBaseExponentForm(Expression& outputBase, Expression& outputExponent)const
{ this->CheckInitialization();
  if (this->IsListNElementsStartingWithAtom(this->theBoss->opThePower(), 3))
  { outputBase=(*this)[1];
    outputExponent=(*this)[2];
    return;
  }
  outputBase=*this;
  outputExponent.AssignValue(1, *this->theBoss);
}

Expression Expression::GetContext()const
{ this->CheckInitialization();
  if (this->IsBuiltInType())
    return (*this)[1];
  crash << "This is a programming error: GetContext called on an Expression that is not a built-in data type.  "
  << " I can't display the expression as this may cause ``infinite'' recursion if the error is caused by the ToString method. Here is however the lisp form "
  << this->ToStringFull() << " of the expression. " << "Here's  a stack trace. " << crash;
  Expression output;
  output.MakeEmptyContext(*this->theBoss);
  return output;
}

int Expression::GetNumContextVariables()const
{ return this->GetContext().ContextGetPolynomialVariables().children.size-1;
}

int Expression::GetNumCols()const
{ if (!this->IsSequenceNElementS() || this->format!=this->formatMatrix)
    return -1;
  int theMax=1;
  for (int i=1; i<this->children.size; i++)
    if ((*this)[i].format==this->formatMatrixRow &&
        (*this)[i].IsSequenceNElementS())
      theMax=MathRoutines::Maximum((*this)[i].children.size-1, theMax);
  return theMax;
}

void Expression::GetCoefficientMultiplicandForm(Rational& outputCoeff, Expression& outputNoCoeff)const
{ this->CheckInitialization();
  if (this->IsListNElementsStartingWithAtom(this->theBoss->opTimes(), 3))
    if ((*this)[1].IsOfType(&outputCoeff))
    { outputNoCoeff=(*this)[2];
      return;
    }
  outputCoeff=1;
  outputNoCoeff=*this;
  return;
}

int Expression::GetExpressionTreeSize()const
{ this->CheckInitialization();
  RecursionDepthCounter theCounter(&this->theBoss->RecursionDeptH);
  if (this->theBoss->RecursionDeptH>this->theBoss->MaxRecursionDeptH)
    crash << "This is a run-time error, which may or may not be caused by a programming error. "
    << "While computing expression tree size, I exceeded the "
    << " recrsion depth limit. To increase the recursion depth limit, modify Calculator::MaxRecursionDeptH. "
    << "The current max recursion depth limit is: " << this->theBoss->MaxRecursionDeptH << crash;
  if (this->IsAtom())
    return 1;
  int result=0;
  for (int i=0; i<this->children.size; i++)
    result+=(*this)[i].GetExpressionTreeSize();
  return result;
}

bool Expression::operator>(const Expression& other)const
{ Rational leftCoeff, rightCoeff;
  Expression leftMon, rightMon;
  this->GetCoefficientMultiplicandForm(leftCoeff, leftMon);
  other.GetCoefficientMultiplicandForm(rightCoeff, rightMon);
  if (leftMon==rightMon)
    return leftCoeff>rightCoeff;
  return leftMon.GreaterThanNoCoeff(rightMon);
}

bool Expression::GreaterThanNoCoeff(const Expression& other)const
{ MacroRegisterFunctionWithName("Expression::GreaterThanNoCoeff");
  if (this->IsOfType<Rational>() && other.IsOfType<Rational>())
    return this->GetValue<Rational>()>other.GetValue<Rational>();
  if (this->IsBuiltInType() && !other.IsBuiltInType())
    return false;
  if (!this->IsBuiltInType() && other.IsBuiltInType())
    return true;
  int thisExpressionTreeSize=this->GetExpressionTreeSize();
  int otherExpressionTreeSize=other.GetExpressionTreeSize();
  if (thisExpressionTreeSize>otherExpressionTreeSize)
    return true;
  if (otherExpressionTreeSize>thisExpressionTreeSize)
    return false;
  if (this->children.size==0)
  { std::string leftS, rightS;
    if (this->IsAtom(&leftS))
      if (other.IsAtom(&rightS))
        return leftS>rightS;
    return this->theData>other.theData;
  }
  if (this->children.size>other.children.size)
    return true;
  if (this->children.size<other.children.size)
    return false;
  for (int i=0; i<this->children.size; i++)
    if ((*this)[i]>other[i])
      return true;
    else if (other[i]>(*this)[i])
      return false;
  return false;
}

bool Expression::ToStringData(std::string& output, FormatExpressions* theFormat)const
{ MacroRegisterFunctionWithName("Expression::ToStringData");
  std::stringstream out;
  bool result=false;
  bool isFinal=theFormat==0 ? false : theFormat->flagExpressionIsFinal;
  MemorySaving<FormatExpressions> contextFormat;
  bool showContext= theBoss==0 ? false : theBoss->flagDisplayContext;
  if (this->IsAtom())
  { if (this->theData< this->theBoss->GetOperations().size && this->theData>=0)
      out << this->theBoss->GetOperations()[this->theData];
    else
      out << "(unknown~ atom~ of~ value~ " << this->theData << ")";
    result=true;
  } else if (this->IsOfType<std::string>())
  { if (isFinal)
      out << this->GetValue<std::string>();
    else
    { out << "(string~ not~ shown~ to~ avoid~ javascript~ problems~ (in~ case~ the~ string~ has~ javascript))";
      //out << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    }
    result=true;
  } else if (this->IsOfType<Rational>())
  { if (this->HasNonEmptyContext())
      out << "Rational{}(" << (*this)[1].ToString() << ", ";
    if (!this->theBoss->flagUseFracInRationalLaTeX)
      out << this->GetValue<Rational>().ToString();
    else
      out << this->GetValue<Rational>().ToStringFrac();
    if (this->HasNonEmptyContext())
      out << ")";
    result=true;
  } else if (this->IsOfType<ElementZmodP>())
  { out << this->GetValue<ElementZmodP>().ToString();
    result=true;
  } else if (this->IsOfType<Polynomial<Rational> >())
  { this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    out << "Polynomial{}(";
    out << this->GetValue<Polynomial<Rational> >().ToString(&contextFormat.GetElement()) << ")";
    if (showContext)
      out << "[" << this->GetContext().ToString() << "]";
    result=true;
  } else if (this->IsOfType<Polynomial<AlgebraicNumber> >())
  { this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    out << "PolynomialAlgebraicNumbers{}(";
    out << this->GetValue<Polynomial<AlgebraicNumber> >().ToString(&contextFormat.GetElement()) << ")";
    if (showContext)
      out << "[" << this->GetContext().ToString() << "]";
    result=true;
  } else if (this->IsOfType<RationalFunctionOld>())
  { this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    out << "RationalFunction{}("
    << this->GetValue<RationalFunctionOld>().ToString(&contextFormat.GetElement()) << ")";
    if (showContext)
      out << "[" << this->GetContext().ToString() << "]";
    result=true;
  } else if (this->IsOfType<Weight<Polynomial<Rational> > >())
  { this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    contextFormat.GetElement().flagFormatWeightAsVectorSpaceIndex=false;
    out << this->GetValue<Weight<Polynomial<Rational> > >().ToString(&contextFormat.GetElement());
    result=true;
  } else if (this->IsOfType<SemisimpleLieAlgebra>())
  { out << "SSLieAlg{}("
    << this->GetValue<SemisimpleLieAlgebra>().GetLieAlgebraName()
    << ")";
    result=true;
  } else if (this->IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >())
  { this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    out << "UEE{}(" //<< this->GetContext().ToString() << ", "
    << this->GetValue<ElementUniversalEnveloping<RationalFunctionOld> >().ToString(&contextFormat.GetElement())
    << ")";
    result=true;
  } else if (this->IsOfType<MatrixTensor<Rational> >())
  { this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    contextFormat.GetElement().flagUseLatex=true;
    contextFormat.GetElement().flagUseHTML=false;
    if (this->GetValue<MatrixTensor<Rational> >().GetMinNumColsNumRows()<20)
      out << "MatrixRationalsTensorForm{}("
      << this->GetValue<MatrixTensor<Rational> > ().ToStringMatForm(&contextFormat.GetElement())
      << ")";
    else
      out << this->GetValue<MatrixTensor<Rational> >().ToString();
    result=true;
  } else if (this->IsOfType<Matrix<Rational> >())
  { this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    contextFormat.GetElement().flagUseLatex=true;
    contextFormat.GetElement().flagUseHTML=false;
    out << "innerMatrixRational{}(" << this->GetValue<Matrix<Rational> >().ToString(&contextFormat.GetElement()) << ")";
    result=true;
  } else if (this->IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
  { this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    out << "ETGVM{}(";
    out << this->GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >().ToString(&contextFormat.GetElement());
    out << ")";
    result=true;
  } else if (this->IsOfType<Plot>())
  { if (isFinal)
    { Plot& thePlot=this->GetValueNonConst<Plot>();
      out << this->theBoss->WriteDefaultLatexFileReturnHtmlLink(thePlot.GetPlotStringAddLatexCommands(false), true);
      out << "<br><b>LaTeX code used to generate the output. </b><br>" << thePlot.GetPlotStringAddLatexCommands(true);
    } else
      out << "(plot not shown)";
    result=true;
  } else if (this->IsOfType<WeylGroup>())
  { WeylGroup& theGroup=this->GetValueNonConst<WeylGroup>();
    contextFormat.GetElement().flagUseLatex=true;
    contextFormat.GetElement().flagUseHTML=false;
    contextFormat.GetElement().flagUseReflectionNotation=true;
    out << theGroup.ToString(&contextFormat.GetElement());
    result=true;
  } else if (this->IsOfType<ElementWeylGroup<WeylGroup> >())
  { const ElementWeylGroup<WeylGroup>& theElt=this->GetValue<ElementWeylGroup<WeylGroup> >();
    contextFormat.GetElement().flagUseLatex=true;
    contextFormat.GetElement().flagUseHTML=false;
    contextFormat.GetElement().flagUseReflectionNotation=true;
    out << theElt.ToString(&contextFormat.GetElement());
    result=true;
  } else if (this->IsOfType<WeylGroupRepresentation<Rational> >())
  { const WeylGroupRepresentation<Rational>& theElt=this->GetValue<WeylGroupRepresentation<Rational> >();
    contextFormat.GetElement().flagUseLatex=true;
    contextFormat.GetElement().flagUseHTML=false;
    contextFormat.GetElement().flagUseReflectionNotation=true;
    out << theElt.ToString(&contextFormat.GetElement());
    result=true;
  } else if (this->IsOfType<WeylGroupVirtualRepresentation<Rational> >())
  { const WeylGroupVirtualRepresentation<Rational>& theElt=this->GetValue<WeylGroupVirtualRepresentation<Rational> >();
    contextFormat.GetElement().flagUseLatex=true;
    contextFormat.GetElement().flagUseHTML=false;
    contextFormat.GetElement().flagUseReflectionNotation=true;
    out << theElt.ToString(&contextFormat.GetElement());
    result=true;
  } else if (this->IsOfType<charSSAlgMod<Rational> >())
  { charSSAlgMod<Rational> theElt=this->GetValue<charSSAlgMod<Rational> >();
    out << theElt.ToString();
    result=true;
  } else if (this->IsOfType<SemisimpleSubalgebras>())
  { SemisimpleSubalgebras& theSubalgebras=this->GetValueNonConst<SemisimpleSubalgebras>();
    contextFormat.GetElement().flagUseLatex=true;
    contextFormat.GetElement().flagUseHTML=true;
    contextFormat.GetElement().flagCandidateSubalgebraShortReportOnly=false;
    contextFormat.GetElement().flagIncludeMutableInformation=false;
    contextFormat.GetElement().flagUseMathSpanPureVsMouseHover=false;
    out << theSubalgebras.ToString(&contextFormat.GetElement());
    result=true;
  } else if (this->IsOfType<double>())
  { out << MathRoutines::DoubleToString(this->GetValue<double>());
    result=true;
  } else if (this->IsOfType<AlgebraicNumber>())
  { out << this->GetValue<AlgebraicNumber>().ToString();
    result=true;
  } else if (this->IsOfType<LittelmannPath>())
  { out << this->GetValue<LittelmannPath>().ToString();
    result=true;
  } else if (this->IsOfType<Matrix<RationalFunctionOld> >())
  { this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    contextFormat.GetElement().flagUseHTML=false;
    contextFormat.GetElement().flagUseLatex=true;
    out << this->GetValue<Matrix<RationalFunctionOld> >().ToString(&contextFormat.GetElement());
    result=true;
  } else if (this->IsOfType<ElementWeylAlgebra<Rational> >())
  { this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    contextFormat.GetElement().flagUseHTML=false;
    contextFormat.GetElement().flagUseLatex=true;
    out << "ElementWeylAlgebra{}(";
    out << this->GetValue<ElementWeylAlgebra<Rational> >().ToString(&contextFormat.GetElement());
    out << ")";
    if (showContext)
      out << "[" << this->GetContext().ToString() << "]";
    result=true;
  }
  output=out.str();
  return result;
}

std::string Expression::ToStringSemiFull()const
{ std::stringstream out;
  std::string tempS;
  if (this->ToStringData(tempS))
    out << tempS << " ";
  else
    if (this->children.size>0)
    { out << "(";
      for (int i=0; i<this->children.size; i++)
      { out << (*this)[i].ToStringSemiFull();
        if (i!=this->children.size-1)
          out << ", ";
      }
      out << ")";
    }
  return out.str();
}

std::string Expression::ToStringFull()const
{ std::stringstream out;
  if (this->IsAtom())
    out << this->theData << " ";
  if (this->children.size>0)
  { out << "(";
    for (int i=0; i<this->children.size; i++)
    { out << (*this)[i].ToStringFull();
      if (i!=this->children.size-1)
        out << ", ";
    }
    out << ")";
  }
  return out.str();
}

bool Expression::NeedsParenthesisForBaseOfExponent()const
{ if (this->theBoss==0)
    return false;
  if (this->IsListNElementsStartingWithAtom())
    return true;
//  if (this->IsListNElementsStartingWithAtom(this->theBoss->opPlus()) || this->IsListNElementsStartingWithAtom(this->theBoss->opMinus()) ||
//      this->IsListNElementsStartingWithAtom(this->theBoss->opTimes()) || this->IsListNElementsStartingWithAtom(this->theBoss->opDivide()) ||
//      this->IsListNElementsStartingWithAtom(this->theBoss->opThePower()))
//    return true;
  return false;
}

bool Expression::NeedsParenthesisForMultiplication()const
{ if (this->theBoss==0)
    return false;
  if (this->IsListNElementsStartingWithAtom(this->theBoss->opPlus()) || this->IsListNElementsStartingWithAtom(this->theBoss->opMinus()))
    return true;
  return false;
}

std::string Expression::ToString(FormatExpressions* theFormat, Expression* startingExpression, Expression* Context)const
{ MacroRegisterFunctionWithName("Expression::ToString");
  if (this->theBoss!=0)
  { if (this->theBoss->RecursionDeptH+1>this->theBoss->MaxRecursionDeptH)
      return "(...)";
  } else
    return "(Error:NoOwner)";
  RecursionDepthCounter theRecursionCounter(&this->theBoss->RecursionDeptH);
  this->CheckConsistency();
  int notationIndex=theBoss->theObjectContainer.ExpressionWithNotation.GetIndex(*this);
  if (notationIndex!=-1)
    return theBoss->theObjectContainer.ExpressionNotation[notationIndex];
  std::stringstream out;
//  AddBrackets=true;
//  if (this->theBoss->flagLogSyntaxRules && recursionDepth<=1)
//  { out << "(ContextIndex=" << this->IndexBoundVars << ")";
//  }
  bool isFinal=theFormat==0 ? false : theFormat->flagExpressionIsFinal;
  bool allowNewLine= (theFormat==0) ? false : theFormat->flagExpressionNewLineAllowed;
  bool oldAllowNewLine= (theFormat==0) ? false : theFormat->flagExpressionNewLineAllowed;

  if (this->theBoss->flagUseFracInRationalLaTeX)
    allowNewLine=false;
  int lineBreak=50;
  int charCounter=0;
  std::string tempS;
  if (this->ToStringData(tempS, theFormat))
    out << tempS;
  else if (this->IsListNElementsStartingWithAtom(this->theBoss->opDefine(), 3))
  { std::string firstE=(*this)[1].ToString(theFormat);
    std::string secondE=(*this)[2].ToString(theFormat);
    if ((*this)[1].IsListStartingWithAtom(this->theBoss->opDefine()))
      out << "(" << firstE << ")";
    else
      out << firstE;
    out << ":=";
    if ((*this)[2].IsListStartingWithAtom(this->theBoss->opDefine()))
      out << "(" << secondE << ")";
    else
      out << secondE;
  } else if (this->IsListStartingWithAtom(this->theBoss->opIsDenotedBy()))
    out << (*this)[1].ToString(theFormat) << ":=:" << (*this)[2].ToString(theFormat);
  else if (this->IsListStartingWithAtom(this->theBoss->opDefineConditional()))
    out << (*this)[1].ToString(theFormat) << " :if " << (*this)[2].ToString(theFormat) << ":=" << (*this)[3].ToString(theFormat);
  else if (this->IsListNElementsStartingWithAtom(this->theBoss->opDivide(), 3))
  { bool doUseFrac= this->formatUseFrac || this->theBoss->flagUseFracInRationalLaTeX;
    if(!doUseFrac)
    { std::string firstE= (*this)[1].ToString(theFormat);
      std::string secondE=(*this)[2].ToString(theFormat);
      bool firstNeedsBrackets= !((*this)[1].IsListStartingWithAtom(this->theBoss->opTimes())|| (*this)[1].IsListStartingWithAtom(this->theBoss->opDivide()));
      bool secondNeedsBrackets=true;
      if ((*this)[2].IsOfType<Rational>())
        if ((*this)[2].GetValue<Rational>().IsInteger())
          secondNeedsBrackets=false;
      if (firstNeedsBrackets)
        out << "(" << firstE << ")";
      else
        out << firstE;
      out << "/";
      if (secondNeedsBrackets)
        out << "(" << secondE << ")";
      else
        out << secondE;
    } else
    { if (theFormat!=0)
        theFormat->flagExpressionNewLineAllowed=false;
      std::string firstE= (*this)[1].ToString(theFormat);
      std::string secondE=(*this)[2].ToString(theFormat);
      out << "\\frac{" << firstE << "}{" << secondE << "}";
      if (theFormat!=0)
        theFormat->flagExpressionNewLineAllowed=oldAllowNewLine;
    }
  } else if (this->IsListNElementsStartingWithAtom(this->theBoss->opTensor(),3) )
    out << (*this)[1].ToString(theFormat) << "\\otimes " << (*this)[2].ToString(theFormat);
  else if (this->IsListNElementsStartingWithAtom(this->theBoss->opChoose(),3) )
    out << (*this)[1].ToString(theFormat) << "\\choose " << (*this)[2].ToString(theFormat);
  else if (this->IsListNElementsStartingWithAtom(this->theBoss->opTimes(), 3))
  { std::string secondE=(*this)[2].ToString(theFormat);
    if ((*this)[1].IsAtomGivenData(this->theBoss->opSqrt()))
      out << "\\sqrt{" << secondE << "}";
    else
    { std::string firstE= (*this)[1].ToString(theFormat);
      bool firstNeedsBrackets=(*this)[1].NeedsParenthesisForMultiplication();
      bool secondNeedsBrackets=(*this)[2].NeedsParenthesisForMultiplication();
      if (firstE=="-1" )
      { firstE="-";
        firstNeedsBrackets=false;
      }
      if (firstE=="1")
        firstE="";
      if (firstNeedsBrackets)
        out << "(" << firstE << ")";
      else
        out << firstE;
      if (this->format==this->formatTimesDenotedByStar && firstE!="-" && firstE!="")
        out << "*";
      else
        out << " ";
      if (secondNeedsBrackets)
        out << "(" << secondE << ")";
      else
        out << secondE;
    }
  } else if (this->IsListNElementsStartingWithAtom(this->theBoss->opSqrt(), 3))
  { int thePower=0;
    bool hasPowerTwo=(*this)[1].IsSmallInteger(&thePower);
    if (hasPowerTwo)
      hasPowerTwo=(thePower==2);
    if (hasPowerTwo)
      out << "\\sqrt{" << (*this)[2].ToString() << "}";
    else
      out << "\\sqrt[" << (*this)[1].ToString() << "]{" << (*this)[2].ToString() << "}";
  } else if (this->IsListStartingWithAtom(this->theBoss->opFactorial()))
    out << (*this)[1].ToString(theFormat) << "!";
  else if (this->IsListNElementsStartingWithAtom(this->theBoss->opThePower(),3))
  { bool involvesExponentsInterprettedAsFunctions=false;
    const Expression& firstE=(*this)[1];
    if (firstE.IsListNElementsStartingWithAtom(-1, 2))
      if (firstE[0].IsAtomWhoseExponentsAreInterprettedAsFunction())
      { involvesExponentsInterprettedAsFunctions=true;
        Expression newE, newFunE;
        newFunE.MakeXOX(*this->theBoss, this->theBoss->opThePower(), firstE[0], (*this)[2]);
        newE.reset(*this->theBoss, 2);
        newE.AddChildOnTop(newFunE);
        newE.AddChildOnTop(firstE[1]);
        this->CheckConsistency();
        newE.CheckConsistency();
        newFunE.CheckConsistency();
        //std::cout << "<br> tostringing a very special case: " << newE.ToString() << " lispified: " << this->ToStringFull();
        out << newE.ToString(theFormat);
      }
    if (!involvesExponentsInterprettedAsFunctions)
    { std::string secondEstr=(*this)[2].ToString(theFormat);
      std::string firstEstr=(*this)[1].ToString(theFormat);
      if ((*this)[1].NeedsParenthesisForBaseOfExponent())
      { bool useBigParenthesis=false;
        if ((*this)[1].IsListNElementsStartingWithAtom(this->theBoss->opDivide()))
          useBigParenthesis=true;
        if (useBigParenthesis)
          out << "\\left(";
        else
          out << "(";
        out << firstEstr;
        if (useBigParenthesis)
          out << "\\right)";
        else
          out << ")";
      }
      else
        out << firstEstr;
      out << "^{" << secondEstr << "}";
    }
//    std::cout << "<br>tostringing: " << out.str() << "   lispified: " << this->ToStringFull();
  } else if (this->IsListStartingWithAtom(this->theBoss->opPlus() ))
  { if (this->children.size<2)
      crash << crash;
    std::string tempS2= (*this)[1].ToString(theFormat);
    tempS=(*this)[2].ToString(theFormat);
    out << tempS2;
//    std::cout << "<br>here i am! tempS2.size=" << tempS2.size() << ", allowNewLine="
//    << allowNewLine;
    if (allowNewLine && tempS2.size()>(unsigned)lineBreak)
      out << "\\\\\n";
    if (tempS.size()>0)
      if (tempS[0]!='-')
        out << "+";
    out << tempS;
  } else if (this->IsListNElementsStartingWithAtom(this->theBoss->opMinus(), 2))
    out << "-" << (*this)[1].ToString(theFormat);
  else if (this->IsListNElementsStartingWithAtom(this->theBoss->opSqrt(), 2))
    out << "\\sqrt{" << (*this)[1].ToString(theFormat) << "}";
  else if (this->IsListNElementsStartingWithAtom(this->theBoss->opMinus(), 3))
  { if (!(this->children.size==3))
      crash << "This is a programming error: the minus function expects 1 or 2 arguments, instead there are " << this->children.size-1 << ". " << crash;
    out << (*this)[1].ToString(theFormat) << "-" << (*this)[2].ToString(theFormat);
  } else if (this->IsListNElementsStartingWithAtom(this->theBoss->opBind(), 2))
    out << "{{" << (*this)[1].ToString(theFormat) << "}}";
  else if (this->IsListStartingWithAtom(this->theBoss->opApplyFunction()))
  { if (this->children.size<2)
      crash << crash;
    switch(this->format)
    { case Expression::formatFunctionUseUnderscore:
        if (allowNewLine)
          theFormat->flagExpressionNewLineAllowed=false;
        out <<  (*this)[1].ToString(theFormat) << "_{" << (*this)[2].ToString(theFormat) << "}";
        if (allowNewLine)
          theFormat->flagExpressionNewLineAllowed=true;
        break;
      case Expression::formatFunctionUseCdot:
        out <<  (*this)[1].ToString(theFormat) << "\\cdot(" << (*this)[1].ToString(theFormat) << ")";
        break;
      default:
        out << (*this)[1].ToString(theFormat) << "{}(" << (*this)[2].ToString(theFormat) << ")";
        break;
    }
  } else if (this->IsListStartingWithAtom(this->theBoss->opEqualEqual()))
    out << (*this)[1].ToString(theFormat) << "==" << (*this)[2].ToString(theFormat);
  else if (this->IsListStartingWithAtom(this->theBoss->opGreaterThan()))
    out << (*this)[1].ToString(theFormat) << ">" << (*this)[2].ToString(theFormat);
  else if (this->IsListStartingWithAtom(this->theBoss->opLessThan()))
    out << (*this)[1].ToString(theFormat) << "<" << (*this)[2].ToString(theFormat);
  else if (this->IsListStartingWithAtom(this->theBoss->opSequence()))
  { switch (this->format)
    { case Expression::formatMatrixRow:
        for (int i=1; i<this->children.size; i++)
        { out << (*this)[i].ToString(theFormat);
          if (i!=this->children.size-1)
            out << "& ";
        }
        break;
      case Expression::formatMatrix:
        if (theFormat!=0)
          if (theFormat->flagUseLatex)
          out << "\\left(";
        out << "\\begin{array}{";
        for (int i=0; i<this->GetNumCols(); i++)
          out << "c";
        out << "} ";
        for (int i=1; i<this->children.size; i++)
        { out << (*this)[i].ToString(theFormat);
          if (i!=this->children.size-1)
            out << "\\\\ \n";
        }
        out << " \\end{array}";
        if (theFormat!=0)
          if (theFormat->flagUseLatex)
            out << "\\right)";
        break;
      default:
        if (this->children.size==2)
          out << "{Sequence{}";
        out << "(";
        for (int i=1; i<this->children.size; i++)
        { std::string currentChildString=(*this)[i].ToString(theFormat);
          out << currentChildString;
          charCounter+=currentChildString.size();
          if (i!=this->children.size-1)
          { out << ", ";
            if (allowNewLine && charCounter >50 )
              out << "\\\\\n";
          }
          charCounter%=50;
        }
        out << ")";
        if (this->children.size==2)
          out << "}";
        break;
    }
  }
  else if (this->IsListStartingWithAtom(this->theBoss->opLieBracket()))
    out << "[" << (*this)[1].ToString(theFormat) << "," << (*this)[2].ToString(theFormat) << "]";
  else if (this->IsListStartingWithAtom(this->theBoss->opMod()))
    out << (*this)[1].ToString(theFormat) << " mod " << (*this)[2].ToString(theFormat);
  else if (this->IsListStartingWithAtom(this->theBoss->opUnion()))
    out << (*this)[1].ToString(theFormat) << "\\cup " << (*this)[2].ToString(theFormat);
  else if (this->IsListStartingWithAtom(this->theBoss->opUnionNoRepetition()))
    out << (*this)[1].ToString(theFormat) << "\\sqcup " << (*this)[2].ToString(theFormat);
  else if (this->IsListStartingWithAtom(this->theBoss->opEndStatement()))
  { if (startingExpression==0)
      out << "<table>";
    for (int i=1; i<this->children.size; i++)
    { out << "<tr><td valign=\"top\">";
      bool createTable=(startingExpression!=0);
      if (createTable)
        createTable=(startingExpression->IsListStartingWithAtom(this->theBoss->opEndStatement()));
      if (createTable)
      { out << "<hr> ";
        if (!this->theBoss->flagHideLHS)
        { if (i<(*startingExpression).children.size)
            out << CGI::GetMathMouseHoverBeginArrayL((*startingExpression)[i].ToString(theFormat));
          else
            out << "No matching starting expression- possible use of the Melt keyword.";
        } else
          out << "...";
        if (i!=this->children.size-1)
          out << ";";
        out << "</td><td valign=\"top\"><hr>";
        if ((*this)[i].IsOfType<std::string>() && isFinal)
          out << (*this)[i].GetValue<std::string>();
        else if (((*this)[i].IsOfType<Plot> () || (*this)[i].IsOfType<SemisimpleSubalgebras>() || (*this)[i].IsOfType<WeylGroup>()
                  || (*this)[i].IsOfType<WeylGroupRepresentation<Rational> >()) && isFinal)
          out << (*this)[i].ToString(theFormat);
        else
          out << CGI::GetMathSpanBeginArrayL((*this)[i].ToString(theFormat));
        if (i!=this->children.size-1)
          out << ";";
      }
      else
      { out << (*this)[i].ToString(theFormat);
        if (i!=this->children.size-1)
          out << ";";
      }
//      out << "&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp Context index: " << this->IndexBoundVars;
      out << "</td></tr>";
    }
    if (startingExpression==0)
      out << "</table>";
  } else if (this->IsListNElementsStartingWithAtom(this->theBoss->opError(), 2))
  { this->theBoss->NumErrors++;
    out << "(Error~ " << this->theBoss->NumErrors << ":~ see~ comments)";
    this->theBoss->Comments << "<br>Error " << this->theBoss->NumErrors << ". " << (*this)[1].ToString(theFormat);
  } else if (this->children.size==1)
    out << (*this)[0].ToString(theFormat);
  else if (this->children.size>=2)
  { out << (*this)[0].ToString(theFormat);
    bool needParenthesis=true;
    if (this->children.size==2)
    { if ((*this)[0].IsAtomWhoseExponentsAreInterprettedAsFunction())
        needParenthesis=!(*this)[1].IsAtom();
      if ((*this)[0].IsPowerOfAtomWhoseExponentsAreInterprettedAsFunction())
        needParenthesis=!(*this)[1].IsAtom();
    }
    if (this->format==this->formatFunctionUseUnderscore)
      out << "_";
    else if (this->format==this->formatFunctionUseCdot)
      out << "\\cdot";
    else
      out << "{}";
    if(this->format== this->formatFunctionUseUnderscore)
      out << "{";
    else if (needParenthesis)
      out << "(";
    for (int i=1; i<this->children.size; i++)
    { out << (*this)[i].ToString(theFormat);
      if (i!=this->children.size-1)
        out << ", ";
    }
    if(this->format== this->formatFunctionUseUnderscore)
      out << "}";
    else if (needParenthesis)
      out << ")";
//    std::cout << "<br>tostringing: " << out.str() << "   lispified: " << this->ToStringFull();

  } else //<-not sure if this case is possible
    out << "(ProgrammingError:NotDocumented)" ;
  if (startingExpression!=0)
  { std::stringstream outTrue;
    outTrue << "<table>";
    outTrue << "<tr><th>Input in LaTeX</th><th>Result in LaTeX</th></tr>";
    outTrue << "<tr><td colspan=\"2\">Double click LaTeX image to get the LaTeX code. "
    << "Javascript LaTeXing courtesy of <a href=\"http://www.math.union.edu/~dpvc/jsmath/\">jsmath</a>: many thanks for your great work!</td></tr>";
   // std::cout << this->Lispify();
    if (this->IsListStartingWithAtom(this->theBoss->opEndStatement()))
      outTrue << out.str();
    else
    { outTrue << "<tr><td>" << CGI::GetMathSpanBeginArrayL(startingExpression->ToString(theFormat));
      if ((this->IsOfType<std::string>() || this->IsOfType<Plot>() ||
           this->IsOfType<SemisimpleSubalgebras>() || this->IsOfType<WeylGroup>()) && isFinal)
        outTrue << "</td><td>" << out.str() << "</td></tr>";
      else
        outTrue << "</td><td>" << CGI::GetMathSpanBeginArrayL(out.str()) << "</td></tr>";
    }
    outTrue << "</table>";
    return outTrue.str();
  }
//  if (useLatex && recursionDepth==0 && this->theOperation!=theBoss->opEndStatement())
//    return CGI::GetHtmlMathSpanFromLatexFormula(out.str());
  return out.str();
}

std::string Expression::Lispify()const
{ if (this->theBoss==0)
    return "Error: not initialized";
  RecursionDepthCounter theCounter(&this->theBoss->RecursionDeptH);
  if (this->theBoss->RecursionDeptH>this->theBoss->MaxRecursionDeptH)
    return "(error: max recursion depth ...)";
  std::string tempS;
  if (this->ToStringData(tempS))
    return tempS;
  if (this->children.size==0)
    return this->ToString();
  std::stringstream out;
  out << "(";
  for (int i=0; i<this->children.size; i++)
    out << " " << (*this)[i].Lispify();
  out << ")";
  return out.str();
}

bool Expression::IsLisT()const
{ if (this->theBoss==0)
    return false;
  if (this->children.size<=0)
    return false;
  if (this->theData!=this->theBoss->opLisT())
    crash << "This is a programming error. List expressions must have data valule equal to Calculator::opList(). " << crash;
  return true;
}

bool Expression::IsFrozen()const
{ std::string atomName;
  if (!this->IsListNElementsStartingWithAtom())
    return false;
  return (*this)[0].IsAtomThatFreezesArguments();
}

bool Expression::IsAtomThatFreezesArguments(std::string* outputWhichAtom)const
{ if (this->theBoss==0)
    return false;
  if (this->IsLisT())
    return false;
  if (this->theData<0 || this->theData>=this->theBoss->GetOperations().size)
    return false;
  if (outputWhichAtom!=0)
    *outputWhichAtom=this->theBoss->GetOperations()[this->theData];
  return this->theBoss->atomsThatFreezeArguments.Contains(this->theBoss->GetOperations()[this->theData]);
}

bool Expression::IsPowerOfAtomWhoseExponentsAreInterprettedAsFunction()const
{ if (this->theBoss==0)
    return false;
  if (!this->IsListNElementsStartingWithAtom(this->theBoss->opThePower(), 3))
    return false;
  return (*this)[1].IsAtomWhoseExponentsAreInterprettedAsFunction();
}

bool Expression::IsAtomWhoseExponentsAreInterprettedAsFunction(std::string* outputWhichAtom)const
{ if (this->theBoss==0)
    return false;
  if (this->IsLisT())
    return false;
  if (this->theData<0 || this->theData>=this->theBoss->GetOperations().size)
    return false;
  if (outputWhichAtom!=0)
    *outputWhichAtom=this->theBoss->GetOperations()[this->theData];
  return this->theBoss->atomsWhoseExponentsAreInterprettedAsFunctions.Contains(this->theBoss->GetOperations()[this->theData]);
}

bool Expression::IsAtomNotInterprettedAsFunction(std::string* outputWhichAtom)const
{ if (this->theBoss==0)
    return false;
  if (this->IsLisT())
    return false;
  if (this->theData<0 || this->theData>=this->theBoss->GetOperations().size)
    return false;
  if (outputWhichAtom!=0)
    *outputWhichAtom=this->theBoss->GetOperations()[this->theData];
  return this->theBoss->atomsNotInterprettedAsFunctions.Contains(this->theBoss->GetOperations()[this->theData]);
}

bool Expression::IsAtom(std::string* outputWhichOperation)const
{ if (this->theBoss==0)
    return false;
  if (this->IsLisT())
    return false;
  if (outputWhichOperation!=0)
  { if (this->theData<0 || this->theData>=this->theBoss->GetOperations().size)
    { std::stringstream out;
      out << "(data-atom:~" << this->theData << ")";
      *outputWhichOperation= out.str();
    }
    else
      *outputWhichOperation=this->theBoss->GetOperations()[this->theData];
  }
  return true;
}

bool Expression::IsAtomGivenData(int desiredDataUseMinusOneForAny)const
{ if (this->IsLisT())
    return false;
  if (desiredDataUseMinusOneForAny==-1)
    return true;
  return this->theData==desiredDataUseMinusOneForAny;
}

bool Expression::IsBuiltInAtom(std::string* outputWhichOperation)const
{ if (this->theBoss==0)
    return false;
  if (this->IsLisT())
    return false;
  if (this->theData<0 || this->theData>=this->theBoss->GetOperations().size)
    return false;
  if (this->theData>= this->theBoss->NumPredefinedAtoms)
    return false;
  if (outputWhichOperation!=0)
    *outputWhichOperation=this->theBoss->GetOperations()[this->theData];
  return true;
}

bool Expression::IsGoodForChainRuleFunction(std::string* outputWhichOperation)const
{ if (this->theBoss==0)
    return false;
  if (this->IsLisT())
    return false;
  if (this->theData<0 || this->theData>=this->theBoss->GetOperations().size)
    return false;
//  std::cout << "ere be i - checking whether operation " << this->theBoss->GetOperations()[this->theData] << " is good for chain rule. ";
  if (outputWhichOperation!=0)
    *outputWhichOperation=this->theBoss->GetOperations()[this->theData];
  return !this->theBoss->atomsNotAllowingChainRule.Contains(this->theBoss->GetOperations()[this->theData]);
}

bool Expression::RemoveContext()
{ this->CheckInitialization();
  if (!this->HasContext())
    return true;
  this->children.RemoveIndexShiftDown(1);
  if (this->children.size==1)
    this->AssignMeMyChild(0);
  return true;
}

bool Expression::HasContext()const
{ this->CheckInitialization();
  if (!this->IsBuiltInType() || !this->children.size==3)
    return false;
  //std::string debugString=(*this)[1].ToString();
  //std::cout << "<br>Trying to fetch context from: " << debugString ;
  return (*this)[1].IsListStartingWithAtom(this->theBoss->opContexT());
}

bool Expression::HasNonEmptyContext()const
{ if (!this->HasContext())
    return false;
  return !this->GetContext().IsListNElementsStartingWithAtom(this->theBoss->opContexT(), 1);
}

bool Expression::IsBuiltInScalar()const
{ return this->IsOfType<Rational>() || this->IsOfType<Polynomial<Rational> >()
  || this->IsOfType<RationalFunctionOld>();
}

bool Expression::IsBuiltInType(std::string* outputWhichOperation)const
{ std::string tempS;
  if (!this->IsListNElementsStartingWithAtom())
    return false;
//  if (this->children.size<2 || !(*this)[this->children.size-1].IsAtoM())
//    return false;
  if (!(*this)[0].IsAtom(&tempS))
    return false;
  if (this->theBoss->GetBuiltInTypes().Contains(tempS))
  { if (outputWhichOperation!=0)
      *outputWhichOperation=tempS;
    return true;
  }
  return false;
}

bool Expression::IsBuiltInType(int* outputWhichType)const
{ std::string theType;
  if (!this->IsBuiltInType(&theType))
    return false;
  if (outputWhichType!=0)
    *outputWhichType=this->theBoss->GetOperations().GetIndex(theType);
  return true;
}

int Calculator::AddOperationNoRepetitionOrReturnIndexFirst(const std::string& theOpName)
{ int result=this->theAtoms.GetIndex(theOpName);
  if (result==-1)
  { this->theAtoms.AddOnTop(theOpName);
    this->FunctionHandlers.SetSize(this->theAtoms.size);
    this->FunctionHandlers.LastObject()->SetSize(0);
    result=this->theAtoms.size-1;
  }
  return result;
}

void Calculator::AddOperationBuiltInType(const std::string& theOpName)
{ this->AddOperationNoRepetitionAllowed(theOpName);
  this->builtInTypes.AddOnTop(theOpName);
}

void Calculator::AddOperationNoRepetitionAllowed(const std::string& theOpName)
{ if (this->GetOperations().Contains(theOpName))
    crash << "This is a programming error: operation " << theOpName << " already created. " << crash;
  this->theAtoms.AddOnTop(theOpName);
  this->FunctionHandlers.SetSize(this->theAtoms.size);
  this->FunctionHandlers.LastObject()->SetSize(0);
}

void Calculator::AddOperationBinaryInnerHandlerWithTypes
(const std::string& theOpName, Expression::FunctionAddress innerHandler, int leftType, int rightType, const std::string& opDescription,
 const std::string& opExample, bool visible, bool experimental)
{ int indexOp=this->theAtoms.GetIndex(theOpName);
  if (indexOp==-1)
  { this->theAtoms.AddOnTop(theOpName);
    indexOp=this->theAtoms.size-1;
    this->FunctionHandlers.SetSize(this->theAtoms.size);
    this->FunctionHandlers.LastObject()->SetSize(0);
  }
  Function innerFunction(innerHandler, 0, opDescription, opExample, true, visible, experimental, true);
  innerFunction.theArgumentTypes.reset(*this, 2);
  innerFunction.theArgumentTypes.AddChildAtomOnTop(leftType);
  innerFunction.theArgumentTypes.AddChildAtomOnTop(rightType);
  this->FunctionHandlers[indexOp].ReservE(10);
  this->FunctionHandlers[indexOp].AddOnTop(innerFunction);
}

void Calculator::AddOperationHandler
(const std::string& theOpName, Expression::FunctionAddress handler, const std::string& opArgumentListIgnoredForTheTimeBeing,
 const std::string& opDescription, const std::string& opExample, bool isInner, bool visible, bool isExperimental)
{ int indexOp=this->theAtoms.GetIndex(theOpName);
  if (indexOp==-1)
  { this->theAtoms.AddOnTop(theOpName);
    indexOp=this->theAtoms.size-1;
    this->FunctionHandlers.SetSize(this->theAtoms.size);
    this->FunctionHandlers.LastObject()->SetSize(0);
  }
  if (opArgumentListIgnoredForTheTimeBeing!="")
    crash << "This section of code is not implemented yet. Crashing to let you know. " << crash;
  Function theFun(handler, 0, opDescription, opExample, isInner, visible, isExperimental);
  if (theOpName=="*" || theOpName=="+" || theOpName=="/" || theOpName=="\\otimes" || theOpName=="^")
    this->FunctionHandlers[indexOp].ReservE(100);
  else
    this->FunctionHandlers[indexOp].ReservE(10);
  this->FunctionHandlers[indexOp].AddOnTop(theFun);
}

void Calculator::AddOperationComposite
(const std::string& theOpName, Expression::FunctionAddress handler, const std::string& opArgumentListIgnoredForTheTimeBeing,
 const std::string& opDescription, const std::string& opExample, bool isInner, bool visible, bool isExperimental)
{ MacroRegisterFunctionWithName("Calculator::AddOperationComposite");
  int theIndex=this->operationsComposite.GetIndex(theOpName);
  if (opArgumentListIgnoredForTheTimeBeing!="")
    crash << "This section of code is not implemented yet. Crashing to let you know. " << crash;
  if (theIndex==-1)
  { theIndex=this->operationsComposite.size;
    this->operationsComposite.AddOnTop(theOpName);
    this->operationsCompositeHandlers.SetSize(this->operationsCompositeHandlers.size+1);
    this->operationsCompositeHandlers.LastObject()->SetSize(0);
  }
  Function theFun(handler, 0, opDescription, opExample, isInner, visible, isExperimental);
  this->operationsCompositeHandlers[theIndex].AddOnTop(theFun);
}

std::string Calculator::ElementToStringNonBoundVars()
{ std::stringstream out;
  std::string openTag1="<span style=\"color:#0000FF\">";
  std::string closeTag1="</span>";
  std::string openTag2="<span style=\"color:#FF0000\">";
  std::string closeTag2="</span>";
  out << "<br>\n" << this->theAtoms.size << " atoms " << " (= " << this->NumPredefinedAtoms << " predefined atoms+ "
  << this->theAtoms.size-this->NumPredefinedAtoms << " user-or-run-time defined). <br>Predefined: \n<br>\n";
  for (int i=0; i<this->theAtoms.size; i++)
  { out << openTag1 << this->theAtoms[i] << closeTag1;
    if (this->FunctionHandlers[i].size>0)
    { out << " [handled by: ";
      for (int j=0; j<this->FunctionHandlers[i].size; j++)
      { out << std::hex << (unsigned long) this->FunctionHandlers[i][j].theFunction;
        if (this->FunctionHandlers[i][j].flagIsInner)
          out << "(inner)";
        else
          out << "(outer)";
        if (j!=this->FunctionHandlers[i].size-1)
          out << ", ";
      }
      out << "]";
    }
    if (i!=this->theAtoms.size-1)
    { out << ", ";
      if (i==this->NumPredefinedAtoms-1 )
        out << "<br>user or run-time defined:\n<br>\n";
    }
  }
  return out.str();
}

bool Function::inputFitsMyInnerType(const Expression& input)
{ if (!this->flagIsInner)
    return false;
  if (this->theArgumentTypes.children.size!=2)
    return true;
  if (input.children.size!=3)
    return false;
  bool argument1good=this->theArgumentTypes[0].theData==-1 ? true :input[1].IsListStartingWithAtom(this->theArgumentTypes[0].theData);
  bool argument2good=this->theArgumentTypes[1].theData==-1 ? true :input[2].IsListStartingWithAtom(this->theArgumentTypes[1].theData);
  return argument1good && argument2good;
}

std::string Function::GetString(Calculator& theBoss)
{ if (!this->flagIamVisible)
    return "";
  std::stringstream out2;
  if (!this->flagIsExperimental)
  { std::stringstream out;
    out << this->theDescription;
//    out << " \nFunction memory address: " << std::hex << (int) this->theFunction << ". ";
    // use of unsigned long is correct on i386 and amd64
    // uintptr_t is only available in c++0x
    out << " Function memory address: " << std::hex << (unsigned long) this->theFunction << ". ";
    if (!this->flagIsInner)
      out << "This is a <b>``law''</b> - substitution takes place only if output expression is different from input. ";
    if (this->theExample!="")
      out << " <br> " << this->theExample << "&nbsp&nbsp&nbsp";
    out2 << CGI::GetHtmlSpanHidableStartsHiddeN(out.str());
    if (this->theExample!="")
      out2 << "<a href=\"" << theBoss.DisplayNameCalculator  << "? textType=Calculator&textDim=1&textInput="
      << CGI::UnCivilizeStringCGI(this->theExample) << "\"> " << " Example" << "</a>" ;
  } else
    out2 << "<b>Experimental, please don't use.</b>";
  return out2.str();
}

std::string ObjectContainer::ToString()
{ std::stringstream out;
  if (this->theLieAlgebras.size>0)
  { out << "Lie algebras created (" << this->theLieAlgebras.size << " total): ";
    for (int i=0; i<this->theLieAlgebras.size; i++)
    { out << this->theLieAlgebras[i].GetLieAlgebraName();
      if (i!=this->theLieAlgebras.size-1)
        out << ", ";
    }
  }
  if (this->theSSsubalgebras.size>0)
  { out << "<br>Lie semisimple subalgebras computation data structures (" << this->theLieAlgebras.size << " total): ";
    for (int i=0; i<this->theSSsubalgebras.size; i++)
    { out << " Type " << this->theSSsubalgebras[i].owneR->GetLieAlgebraName() << " with "
      << this->theSSsubalgebras[i].theSubalgebraCandidates.size << " candidates";
      if (i!=this->theLieAlgebras.size-1)
        out << ", ";
    }
  }
  return out.str();
}

std::string Calculator::ToString()
{ MacroRegisterFunctionWithName("Calculator::ToString");
  std::stringstream out, out2;
  std::string openTag1="<span style=\"color:#0000FF\">";
  std::string closeTag1="</span>";
  std::string openTag2="<span style=\"color:#FF0000\">";
  std::string closeTag2="</span>";
  std::string openTag3="<span style=\"color:#00FF00\">";
  std::string closeTag3="</span>";
  out2 << " Total number of pattern matches performed: " << this->TotalNumPatternMatchedPerformed << "";
  double elapsedSecs=this->theGlobalVariableS->GetElapsedSeconds();
  out2 << "<br>Computation time: " << elapsedSecs << " seconds (" << elapsedSecs*1000 << " milliseconds).<br>";
  std::stringstream tempStreamTime;
  tempStreamTime << " Of them " << this->StartTimeEvaluationInSecondS << " seconds (" << this->StartTimeEvaluationInSecondS*1000
  << " millisecond(s)) boot + " << elapsedSecs-this->StartTimeEvaluationInSecondS << " (" << (elapsedSecs-this->StartTimeEvaluationInSecondS)*1000
  << " milliseconds) user computation.<br>Boot time is measured from start of main() until evaluation start and excludes static initializations "
  << "+ executable load. Computation time excludes the time needed to compute the strings that follow below (which might take a while).";
  out2 << CGI::GetHtmlSpanHidableStartsHiddeN(tempStreamTime.str());
  out2 << "<br>Maximum computation time: " << this->theGlobalVariableS->MaxComputationTimeSecondsNonPositiveMeansNoLimit/2 << " seconds. ";
  if (this->DepthRecursionReached>0)
    out2 << "<br>Maximum recursion depth reached: " << this->DepthRecursionReached << ".";
  #ifdef MacroIncrementCounter
  out2 << "<br>Number of Lists created: " << NumListsCreated << "<br> Number of List resizes: " << NumListResizesTotal
  << "<br> Number HashedList hash resizing: " << NumHashResizes;
  if (Rational::TotalSmallAdditions>0)
    out2 << "<br>Number small rational number additions: " << Rational::TotalSmallAdditions << " (# successful calls Rational::TryToAddQuickly)";
  if (Rational::TotalSmallMultiplications>0)
    out2 << "<br>Number small rational number multiplications: " << Rational::TotalSmallMultiplications << " (# successful calls Rational::TryToMultiplyQuickly)";
  if (Rational::TotalSmallGCDcalls>0)
    out2 << "<br>Number small number gcd calls: " << Rational::TotalSmallGCDcalls << " (# calls of Rational::gcd)";
  if (Rational::TotalLargeAdditions>0)
    out2 << "<br>Number large integer additions: " << Rational::TotalLargeAdditions << " (# calls LargeIntUnsigned::AddNoFitSize)";
  if (Rational::TotalLargeMultiplications>0)
    out2 << "<br>Number large integer multiplications: " << Rational::TotalLargeMultiplications << " (# calls LargeIntUnsigned::MultiplyBy)";
  if (Rational::TotalLargeGCDcalls>0)
    out2 << "<br>Number large number gcd calls: " << Rational::TotalLargeGCDcalls << " (# calls LargeIntUnsigned::gcd)";
  #endif
  if (this->RuleStack.size>0)
  { out2 << "<hr><b>Predefined rules.</b><br>";
    for (int i=0; i<this->RuleStack.size; i++)
    { out2 << this->RuleStack[i].ToString();
      if (i!=this->RuleStack.size-1)
        out2 << "<br>";
    }
  }
  out2 << "<hr>" << this->ToStringFunctionHandlers() << "<hr><b>Further calculator details.</b>";
  out << "<br><b>Object container information</b>.The object container is the data structure storing all c++ built-in data types "
  << " requested by the user<br> " << this->theObjectContainer.ToString();
  out << "<hr>Control sequences (" << this->controlSequences.size << " total):\n<br>\n";
  for (int i=0; i<this->controlSequences.size; i++)
  { out << openTag1 << this->controlSequences[i] << closeTag1;
    if (i!=this->controlSequences.size)
      out << ", ";
  }
  out << "<br>\n User or run-time defined atoms = " << this->theAtoms.size << " (= " << this->NumPredefinedAtoms << " predefined + "
  << this->theAtoms.size-this->NumPredefinedAtoms << " user-defined):<br>\n";
  for (int i=0; i<this->theAtoms.size; i++)
  { out << "\n" << i << ": " << openTag1 << this->theAtoms[i] << closeTag1;
    if(i!=this->theAtoms.size-1)
      out << ", ";
  }
  out << this->ElementToStringNonBoundVars();
  out << "<hr>";
  out << "Children expressions (" << this->theExpressionContainer.size << " total):<br>";
  int numExpressionsToDisplay=this->theExpressionContainer.size;
  if (this->theExpressionContainer.size>1000)
  { numExpressionsToDisplay=1000;
    out << " <b>Diplaying first " << numExpressionsToDisplay << " only </b><br>";
  }
  for (int i=0; i< numExpressionsToDisplay; i++)
    out << this->theExpressionContainer[i].ToString() << ", ";
  out << "<hr>";
  out << "\n Cached expressions (" << this->cachedExpressions.size << " total):\n<br>\n";
  numExpressionsToDisplay=this->cachedExpressions.size;
  if (numExpressionsToDisplay>1000)
  { numExpressionsToDisplay=1000;
    out << "<b>Displaying first " << numExpressionsToDisplay << " expressions only.</b><br>";
  }
  for (int i=0; i<numExpressionsToDisplay; i++)
  { out << this->cachedExpressions[i].ToString() << " -> " << this->imagesCachedExpressions[i].ToString();
    if (i!=this->cachedExpressions.size-1)
      out << "<br>";
  }
  out2 << CGI::GetHtmlSpanHidableStartsHiddeN(out.str());//, "Expand/collapse.");
  return out2.str();
}

bool Calculator::ReplaceIntIntBy10IntPlusInt()
{ SyntacticElement& left=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& right=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Rational tempRat=left.theData.GetValue<Rational>();
  if (this->registerPositionAfterDecimalPoint==0)
  { tempRat*=10;
    tempRat+=right.theData.GetValue<Rational>();
  } else
  { Rational afterDecimal=right.theData.GetValue<Rational>();
    for (int i=0; i<this->registerPositionAfterDecimalPoint; i++)
      afterDecimal/=10;
    tempRat+=afterDecimal;
    this->registerPositionAfterDecimalPoint++;
  }
  left.theData.AssignValue(tempRat, *this);
  this->DecreaseStackSetCharacterRangeS(1);
  return true;
}

bool Calculator::ReplaceXEXEXByEusingO(int theControlIndex, int formatOptions)
{ SyntacticElement& lefT = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& result = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(theControlIndex));
  newExpr.AddChildOnTop(lefT.theData);
  newExpr.AddChildOnTop(right.theData);
  newExpr.format=formatOptions;
  result.theData=newExpr;
  result.controlIndex=this->conExpression();
  this->DecreaseStackSetCharacterRangeS(4);
  return true;
}

bool Calculator::ReplaceEXdotsXbySsXdotsX(int numDots)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numDots-1];
  Expression newExpr;
  newExpr.reset(*this);
  newExpr.children.ReservE(2);
  newExpr.AddChildAtomOnTop(this->opEndStatement());
  newExpr.AddChildOnTop(left.theData);
  left.theData=newExpr;
  left.controlIndex=this->conSequenceStatements();
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceSsSsXdotsXbySsXdotsX(int numDots)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numDots-2];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numDots-1];
  if (!left.theData.IsListNElementsStartingWithAtom(this->opEndStatement()))
    crash << "This is a programming error: ReplaceSsSsXdotsXbySsXdotsX called but left expression is not EndStatement." << crash;
  left.theData.children.ReservE(left.theData.children.size+ right.theData.children.size-1);
  for (int i=1; i<right.theData.children.size; i++)
    left.theData.AddChildOnTop(right.theData[i]);
  left.theData.format=Expression::formatDefault;
  left.controlIndex=this->conSequenceStatements();
  (*this->CurrentSyntacticStacK).PopIndexShiftDown((*this->CurrentSyntacticStacK).size-numDots-1);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceEXEByEusingO(int theControlIndex, int formatOptions)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(theControlIndex));
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(right.theData);
  newExpr.format=formatOptions;
  left.theData=newExpr;
  this->DecreaseStackSetCharacterRangeS(2);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceEXEXByEX(int formatOptions)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(right.theData);
  newExpr.format=formatOptions;
  left.theData=newExpr;
  this->DecreaseStackExceptLast(2);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

std::string Calculator::ElementToStringSyntacticStack()
{ std::stringstream out;
  if ((*this->CurrentSyntacticStacK).size<this->numEmptyTokensStart)
    return "Error: this is a programming error: not enough empty tokens in the start of the syntactic stack.";
  bool HasMoreThanOneSignificantEntriesOrIsNotExpression=((*this->CurrentSyntacticStacK).size>this->numEmptyTokensStart+1);
  SyntacticElement& lastSyntacticElt=* (*this->CurrentSyntacticStacK).LastObject();
  if ((*this->CurrentSyntacticStacK).size==this->numEmptyTokensStart+1)
    if (lastSyntacticElt.controlIndex!=this->conExpression())
      HasMoreThanOneSignificantEntriesOrIsNotExpression=true;
  if (HasMoreThanOneSignificantEntriesOrIsNotExpression)
    out << "<table border=\"1\"><tr><td>";
  for (int i=this->numEmptyTokensStart; i<(*this->CurrentSyntacticStacK).size; i++)
  { out << (*this->CurrentSyntacticStacK)[i].ToString(*this);
    if (HasMoreThanOneSignificantEntriesOrIsNotExpression)
    { out << "</td>";
      if (i!=(*this->CurrentSyntacticStacK).size-1)
        out << "<td>";
    }
  }
  if (HasMoreThanOneSignificantEntriesOrIsNotExpression)
    out << "</td></tr></table>";
  return out.str();
}

bool Calculator::ReplaceXXXByCon(int theCon)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].controlIndex=theCon;
  this->DecreaseStackSetCharacterRangeS(2);
  return true;
}

bool Calculator::ReplaceXXXByConCon(int con1, int con2, int inputFormat1, int inputFormat2)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].controlIndex=con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].controlIndex=con2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].theData.format=inputFormat2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].theData.format=inputFormat1;
  this->DecreaseStackSetCharacterRangeS(1);
  return true;
}

bool Calculator::ReplaceXXXXXByCon(int theCon, int inputFormat)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5].controlIndex=theCon;
  this->DecreaseStackSetCharacterRangeS(4);
  return true;
}

bool Calculator::ReplaceXXXXXByConCon(int con1, int con2, int inputFormat1, int inputFormat2)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].controlIndex=con2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5].controlIndex=con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].theData.format=inputFormat2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5].theData.format=inputFormat1;
  this->DecreaseStackSetCharacterRangeS(3);
  return true;
}

bool Calculator::ReplaceXXXXByConCon(int con1, int con2, int inputFormat1, int inputFormat2)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].controlIndex=con2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].controlIndex=con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].theData.format=inputFormat2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].theData.format=inputFormat1;
  this->DecreaseStackSetCharacterRangeS(2);
  return true;
}

bool Calculator::ReplaceXXXXByCon(int con1, int inputFormat1)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].controlIndex=con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].theData.format=inputFormat1;
  this->DecreaseStackSetCharacterRangeS(3);
  return true;
}

bool Calculator::ReplaceXByCon(int theCon, int theFormat)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].controlIndex=theCon;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].theData.format=theFormat;
  //    this->DecreaseStackSetCharacterRanges(2);
  return true;
}

bool Calculator::ReplaceXByEusingO(int theOperation)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].controlIndex=this->conExpression();
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].theData.MakeAtom(theOperation, *this);
  //    this->DecreaseStackSetCharacterRanges(2);
  return true;
}

bool Calculator::ReplaceXByConCon(int con1, int con2, int format1, int format2)
{ (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size+1);
  (*this->CurrentSyntacticStacK).LastObject()->theData.reset(*this);
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].controlIndex=con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].controlIndex=con2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].theData.format=format1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].theData.format=format2;
  //    this->DecreaseStackSetCharacterRanges(2);
  return true;
}

void ObjectContainer::reset()
{ this->theWeylGroupElements.Clear();
  this->theWeylGroupReps.Clear();
  this->theWeylGroupVirtualReps.Clear();
  this->theCategoryOmodules.SetSize(0);
  this->theLieAlgebras.Clear();
  this->theSSsubalgebras.SetSize(0);
  this->theTensorElts.Clear();
  this->thePolys.Clear();
  this->thePolysOverANs.Clear();
  this->theWeylAlgebraElements.Clear();
  this->theUEs.Clear();
  this->theRFs.Clear();
  this->theRationals.Clear();
  this->theCharsSSLieAlgFD.Clear();
  this->theDoubles.Clear();
  this->theStrings.Clear();
  this->ExpressionNotation.Clear();
  this->ExpressionWithNotation.Clear();
  this->theLSpaths.Clear();
  this->theMatRats.Clear();
  this->theMatTensorRats.Clear();
  this->theMatRFs.Clear();
  this->theEltsModP.Clear();
  this->thePlots.SetSize(0);
  theAlgebraicClosure.reset();
  this->theAlgebraicNumbers.Clear();
  this->theLittelmannOperators.Clear();
  this->theSltwoSAs.SetSize(0);
}

bool Calculator::IsBoundVarInContext(int inputOp)
{ for (int i=0; i<this->BoundVariablesStack.size; i++)
    if (this->BoundVariablesStack[i].Contains(inputOp))
      return true;
  return false;
}

bool Calculator::IsNonBoundVarInContext(int inputOp)
{ for (int i=0; i<this->NonBoundVariablesStack.size; i++)
    if (this->NonBoundVariablesStack[i].Contains(inputOp))
      return true;
  return false;
}

void Calculator::InitJavaScriptDisplayIndicator()
{ std::stringstream output;
  output << " <!>\n";
  output << " <script type=\"text/javascript\"> \n";
  output << " var timeOutCounter=0;\n";
//  output << " var newReportString=\"\";\n";
  output << " var showProgress=false;";
  output << " function progressReport()\n";
  output << "{ var el = document.getElementById(\"idProgressReport\");	\n";
  output << "  if (!showProgress) \n";
  output << "  { el.style.display = 'none';\n";
  output << "    return;";
  output << "  }\n";
  output << "  el.style.display = '';\n";
//  output << "  el.contentWindow.location.reload();";
  output << "  timeOutCounter++;\n";
  output << "  var oRequest = new XMLHttpRequest();\n";
  output << "  var sURL  = \"" << this->indicatorFileNameDisplaY << "\";\n";
  output << "  oRequest.open(\"GET\",sURL,false);\n";
  output << "  oRequest.setRequestHeader(\"Indicator\",navigator.userAgent);\n";
  output << "  oRequest.send(null)\n";
  output << "  if (oRequest.status==200)\n";
  output << "  { newReportString= oRequest.responseText;\n";
  output << "    el.innerHTML= \"<hr>Refreshing each second. Client time: ~\"+ timeOutCounter+\" second(s)<br>\" +newReportString+\"<hr>\";\n";
  output << "  }\n";
  output << "   window.setTimeout(\"progressReport()\",1000);\n";
  output << " }\n";
  output << " </script>\n";
  output << CGI::GetHtmlButton
  ("progressReportButton", "showProgress=!showProgress; progressReport()", "expand/collapse progress report");
  output << "<br><div "
  //<< "src=\"" << this->indicatorFileNameDisplaY << "\" "
  << "id=\"idProgressReport\" style=\"display:none\">\n";
  output << " </div>\n";
  output << " \n";
  output << " \n";
  this->javaScriptDisplayingIndicator=output.str();
}

void Calculator::initDefaultFolderAndFileNames(const std::string& inputPathBinaryBaseIsFolderBelow, const std::string& inputDisplayPathBase, const std::string& scrambledIP)
{ this->PhysicalPathServerBase = inputPathBinaryBaseIsFolderBelow + "../";
  this->DisplayPathServerBase = "/" + inputDisplayPathBase;

  this->PhysicalPathOutputFolder = this->PhysicalPathServerBase + "output/";
  this->DisplayPathOutputFolder = this->DisplayPathServerBase + "output/";

  this->userLabel=scrambledIP;

  this->PhysicalNameDefaultOutput = this->PhysicalPathOutputFolder + "default" + this->userLabel + "output";
  this->DisplayNameDefaultOutputNoPath = "default" + this->userLabel + "output";
  this->DisplayNameDefaultOutput = this->DisplayPathOutputFolder + this->DisplayNameDefaultOutputNoPath;

  this->indicatorFileNamE = this->PhysicalPathOutputFolder + "indicator" + this->userLabel + ".html" ;
  this->indicatorFileNameDisplaY = this->DisplayPathOutputFolder + "indicator" + this->userLabel + ".html" ;
  this->DisplayNameCalculator = this->DisplayPathServerBase +"cgi-bin/calculator";

}

bool Calculator::innerWriteGenVermaModAsDiffOperators(Calculator& theCommands, const Expression& input, Expression& output, bool AllGenerators)
{ MacroRegisterFunctionWithName("Calculator::innerWriteGenVermaModAsDiffOperators");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  Vectors<Polynomial<Rational> > theHWs;
  theHWs.SetSize(1);
  Expression theContext;
  Selection theParSel;
  SemisimpleLieAlgebra* theSSalgebra;
  Expression truncatedInput=input;
  if (truncatedInput.children.size>4)
  { int numEltsToCut=truncatedInput.children.size-4;
    for (int i=0; i<numEltsToCut; i++)
      truncatedInput.children.RemoveLastObject();
  }
  if (!theCommands.GetTypeHighestWeightParabolic<Polynomial<Rational> >
      (theCommands, truncatedInput, output, theHWs[0], theParSel, theContext, theSSalgebra, CalculatorSerialization::innerPolynomial<Rational>))
    return output.SetError("Failed to extract type, highest weight, parabolic selection", theCommands);
  if (output.IsError())
    return true;
  std::string letterString="x";
  std::string partialString="\\partial";
  std::string exponentLetterString="a";
  if (input.children.size>4)
    letterString=input[4].ToString();
  if (input.children.size>5)
    partialString=input[5].ToString();
  if (input.children.size>6)
    exponentLetterString=input[6].ToString();

//std::cout << "<br>theContext:" << theContext.ToString();
//std::cout << ", numvars: " << theContext.ContextGetNumContextVariables();
//  FormatExpressions theFormat;
//  theContext.ContextGetFormatExpressions(theFormat);
//  std::cout << "highest weights you are asking me for: " << theHws.ToString(&theFormat);
  return theCommands.innerWriteGenVermaModAsDiffOperatorInner
  (theCommands, input, output, theHWs, theContext, theParSel, theSSalgebra, AllGenerators, &letterString, &partialString, &exponentLetterString);
}

bool Calculator::innerFreudenthalFull(Calculator& theCommands, const Expression& input, Expression& output)
{ Vector<Rational> hwFundamental, hwSimple;
  Selection tempSel;
  SemisimpleLieAlgebra* theSSalg;
  Expression context;
  if (!theCommands.GetTypeHighestWeightParabolic<Rational>(theCommands, input, output, hwFundamental, tempSel, context, theSSalg, 0))
    return output.SetError("Failed to extract highest weight and algebra", theCommands);
  if (output.IsError())
    return true;
  if (tempSel.CardinalitySelection>0)
    return output.SetError("Failed to extract highest weight. ", theCommands);
  charSSAlgMod<Rational> startingChar, resultChar ;
  hwSimple=theSSalg->theWeyl.GetSimpleCoordinatesFromFundamental(hwFundamental);
  startingChar.MakeFromWeight(hwSimple, theSSalg);
  std::string reportString;
  if (!startingChar.FreudenthalEvalMeFullCharacter(resultChar, 10000, &reportString, theCommands.theGlobalVariableS))
    return output.SetError(reportString, theCommands);
  std::stringstream out;
  out << resultChar.ToString();
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerFreudenthalEval(Calculator& theCommands, const Expression& input, Expression& output)
{ Vector<Rational> hwFundamental, hwSimple;
  Selection tempSel;
  SemisimpleLieAlgebra* theSSalg=0;
  Expression context;
  if (!theCommands.GetTypeHighestWeightParabolic<Rational>(theCommands, input, output, hwFundamental, tempSel, context, theSSalg, 0))
    return output.SetError("Failed to extract highest weight and algebra", theCommands);
  if (output.IsError())
    return true;
  if (tempSel.CardinalitySelection>0)
    return output.SetError("Failed to extract highest weight. ", theCommands);
  charSSAlgMod<Rational> startingChar, resultChar ;
  hwSimple=theSSalg->theWeyl.GetSimpleCoordinatesFromFundamental(hwFundamental);
  startingChar.MakeFromWeight(hwSimple, theSSalg);
  std::string reportString;
  if (!startingChar.FreudenthalEvalMeDominantWeightsOnly(resultChar, 10000, &reportString, theCommands.theGlobalVariableS))
    return output.SetError(reportString, theCommands);
  return output.AssignValue(resultChar, theCommands);
}

bool Expression::IsMeltable(int* numResultingChildren)const
{ this->CheckInitialization();
  if (!this->IsListNElementsStartingWithAtom(this->theBoss->opMelt(), 2))
    return false;
  if (numResultingChildren!=0)
  { if (!(*this)[1].IsListNElementsStartingWithAtom(this->theBoss->opEndStatement()))
      *numResultingChildren=1;
    else
      *numResultingChildren=(*this)[1].children.size-1;
  }
  return true;
}

bool Expression::MergeContextsMyAruments(Expression& output)const
{ MacroRegisterFunctionWithName("Expression::MergeContextsMyAruments");
//  std::cout << "<hr>Merging contexts of expression " << this->ToString();
  this->CheckInitialization();
  if (this->children.size<2)
    return false;
//  std::cout << " ... continuing to merge..." ;
  for (int i=1; i< this->children.size; i++)
    if (!(*this)[i].IsBuiltInType())
    { this->theBoss->Comments << "<hr>Failed to merge the arguments of the expression" << this->ToString() << ": the argument "
      << (*this)[i].ToString() << "is not of built-in type";
      return false;
    }
  Expression commonContext=(*this)[1].GetContext();
  bool needsMerge=false;
  for (int i=2; i<this->children.size; i++)
    if (!(commonContext==(*this)[i].GetContext()))
    { needsMerge=true;
      break;
    }
//  if (needsMerge)
//    std::cout << "needs merge!"; else std::cout << "no need merge no nothin";
  if (!needsMerge)
  { output=*this;
    return true;
  }
  for (int i=2; i<this->children.size; i++)
  { if (!(*this)[i].IsBuiltInType())
    { this->theBoss->Comments << "<hr>Failed to merge contexts of arguments: an argument is not of built-in type";
      return false;
    }
//    std::cout << "<br>Merging context " << commonContext.ToString() << " with " << (*this)[i].GetContext().ToString();
    if (!commonContext.ContextMergeContexts(commonContext, (*this)[i].GetContext(), commonContext))
    { this->theBoss->Comments << "<hr>Failed to merge context " << commonContext.ToString() << " with " << (*this)[i].GetContext().ToString();
      return false;
    }
//    std::cout << " ...  to get context: " << commonContext.ToString();
  }
  output.reset(*this->theBoss, this->children.size);
  output.AddChildOnTop((*this)[0]);
  Expression convertedE;
  for (int i=1; i<this->children.size; i++)
  { convertedE=(*this)[i];
//    std::cout << "<hr>Setting context of " << convertedE.ToString() << " to be the context " << commonContext.ToString();
    if (!convertedE.SetContextAtLeastEqualTo(commonContext))
    { this->theBoss->Comments << "<hr>Failed to convert " << convertedE.ToString() << " to context " << commonContext.ToString();
      return false;
    }
//    std::cout << "... and the result is: " << convertedE.ToString();
    output.AddChildOnTop(convertedE);
  }
  return true;
}

bool Calculator::ConvertExpressionsToCommonContext(List<Expression>& inputOutputEs, Expression* inputOutputStartingContext)
{ MacroRegisterFunctionWithName("Calculator::ConvertExpressionsToCommonContext");
  Expression commonContext;
  commonContext.MakeEmptyContext(*this);
  if (inputOutputStartingContext!=0)
    if (inputOutputStartingContext->IsContext())
      commonContext=*inputOutputStartingContext;
  for (int i=0; i<inputOutputEs.size; i++)
  { if (!inputOutputEs[i].IsBuiltInType())
    { this->Comments << "<hr>Possible programming error: calling ConvertExpressionsToCommonContext on expressions without context. "
      << Crasher::GetStackTraceEtcErrorMessage();
      return false;
    }
    if (!commonContext.ContextMergeContexts(commonContext, inputOutputEs[i].GetContext(), commonContext))
    { this->Comments << "<hr>Failed to merge context " << commonContext.ToString() << " with " << inputOutputEs[i].GetContext().ToString();
      return false;
    }
  }
  for (int i=0; i<inputOutputEs.size; i++)
    if (!inputOutputEs[i].::Expression::SetContextAtLeastEqualTo(commonContext))
      return false;
  if (inputOutputStartingContext!=0)
    *inputOutputStartingContext=commonContext;
  return true;
}

bool Calculator::outerMeltBrackets(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::outerMeltBrackets");
  RecursionDepthCounter theCounter(&theCommands.RecursionDeptH);
  //std::cout << "outerMeltBrackets meldet sich!";
  if (!input.IsListNElementsStartingWithAtom(theCommands.opEndStatement()))
    return false;
  //std::cout << "<br>outerMeltBrackets meldet sich!";
  int tempInt;
  int ChildIncrease=0;
  bool found=false;
  for (int i=0; i<input.children.size; i++)
  { const Expression& currentChild=input[i];
    if (currentChild.IsMeltable(&tempInt))
    { found=true;
      ChildIncrease+=tempInt-1;
    }
  }
//  if (!found)
//    std::cout << "<br>not found!";
  if (!found)
    return false;
//  std::cout << "<br>ChildIncrease: " << ChildIncrease;
  output.reset(theCommands, input.children.size+ChildIncrease);
  output.AddChildAtomOnTop(theCommands.opEndStatement());
  for (int i=1; i<input.children.size; i++)
  { const Expression& currentChild=input[i];
    if (!currentChild.IsMeltable())
    { //output.SetChild(i+shift, input.children[i]);
      output.AddChildOnTop(input[i]);
      continue;
    }
//    std::cout << "<br>shift:" << shift;
    if (!currentChild[1].IsListNElementsStartingWithAtom(theCommands.opEndStatement()))
    { //output.SetChild(i+shift, currentChild.children[1]);
      output.AddChildOnTop(currentChild[1]);
      continue;
    }
    for (int j=1; j<currentChild[1].children.size; j++)
    { //output.SetChild(i+shift, currentChild[1].children[j]);
      output.AddChildOnTop(currentChild[1][j]);
    }
  }
//  std::cout << output.ToString();

  return true;
}
