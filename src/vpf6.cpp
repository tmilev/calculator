//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
#include "vpfImplementationHeader2Math3_WeylAlgebra.h"
#include "vpfImplementationHeader2Math051_PolynomialComputations_Basic.h"
#include "vpfImplementationHeader2Math15_UniversalEnveloping.h"
#include "vpfImplementationHeader2Math6_ModulesSSLieAlgebras.h"
#include "vpfImplementationHeader2Math1_SemisimpleLieAlgebras.h"
#include "vpfImplementationHeader2Math3_FiniteGroups.h"

ProjectInformationInstance ProjectInfoVpf6cpp(__FILE__, "Calculator parser, implementation. ");

Calculator::Calculator()
{ this->numOutputFileS=0;
  this->flagHideLHS=false;
  this->flagUseHtml=true;
  this->flagShowCalculatorInternalStatus=false;
  this->flagShowCalculatorExamples=true;
  this->flagWriteLatexPlots=true;
  this->flagUseLnInsteadOfLog=false;
  this->flagPlotShowJavascriptOnly=false;
}

std::string Calculator::GetCalculatorLink(const std::string& input)
{ return CGI::GetCalculatorLink(theGlobalVariables.DisplayNameExecutable, input);
}

std::string Calculator::WriteDefaultLatexFileReturnHtmlLink
(const std::string& fileContent, bool useLatexDviPSpsToPNG)
{ std::fstream theFile;
  std::stringstream fileNameRelative;
  std::stringstream systemCommand1, systemCommand2, systemCommand3;
  unsigned int fileIdentifier=MathRoutines::hashString(fileContent);

  fileNameRelative << theGlobalVariables.RelativePhysicalNameExtraOutputWithPath << fileIdentifier;
  std::string fileNamePhysical=theGlobalVariables.PhysicalPathHtmlFolder + fileNameRelative.str();
  if (!FileOperations::OpenFileCreateIfNotPresentVirtual(theFile, "output/"+fileNameRelative.str()+".tex", false, true, false))
    return "failed to create file: " + fileNameRelative.str() + ".tex";
  theFile << fileContent;
  theFile.flush();
  theFile.close();
  systemCommand1 << " latex -output-directory=" << theGlobalVariables.PhysicalPathHtmlFolder << " "
  << fileNamePhysical << ".tex";
  //stOutput << "<br>system command:<br>" << systemCommand1.str();
  this->SystemCommands.AddOnTop(systemCommand1.str());
  if (useLatexDviPSpsToPNG)
  { systemCommand2 << " dvips -o " << fileNamePhysical << ".ps " << fileNamePhysical << ".dvi";
    //stOutput << "<br>system command:<br>" << systemCommand2.str();
    this->SystemCommands.AddOnTop(systemCommand2.str());
    systemCommand3 << " convert " << fileNamePhysical << ".ps " << fileNamePhysical << ".png";
    //stOutput << "<br>system command:<br>" << systemCommand3.str();
    this->SystemCommands.AddOnTop(systemCommand3.str());
  }
  std::stringstream out;
  out << "<img src=\"" << theGlobalVariables.DisplayPathOutputFolder
  << fileNameRelative.str() << ".png\"></img><a href=\"" << theGlobalVariables.DisplayPathOutputFolder
  << fileNameRelative.str() << ".tex\">" << theGlobalVariables.DisplayPathOutputFolder
  << fileNameRelative.str() << ".tex</a>";
  this->numOutputFileS++;
  return out.str();
}

int IntIdentity(const int& x)
{ return x;
}

bool Calculator::GetVectorExpressions(const Expression& input, List<Expression>& output, int targetDimNonMandatory)
{ MacroRegisterFunctionWithName("Calculator::GetVectorExpressions");
  output.Reserve(input.children.size);
  output.SetSize(0);
  if (!input.IsSequenceNElementS())
  { if (targetDimNonMandatory>0)
      if (targetDimNonMandatory!=1)
        return *this << "<hr>GetVector failure: target dim is " << targetDimNonMandatory << " but the input " << input.ToString()
        << " can only be interpreted as a single element";
    output.AddOnTop(input);
    return true;
  }
  if (targetDimNonMandatory>0)
    if (targetDimNonMandatory!=input.children.size-1)
      return *this << "<hr>Failed to GetVector: the input is required to have " << targetDimNonMandatory << " columns but it has "
      << input.children.size-1 << " columns instead. <hr>";
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
{ Vector<Rational> theWeight=this->GetOwner().GetWeightOfGenerator(theGeneratorIndex);
  for (int j=0; j<this->parabolicSelectionNonSelectedAreElementsLevi.CardinalitySelection; j++)
    if (!(theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[j]]<0))
      return true;
  return false;
}

template <class coefficient>
bool ModuleSSalgebra<coefficient>::IsNotInLevi(int theGeneratorIndex)
{ Vector<Rational> theWeight=this->GetOwner().GetWeightOfGenerator(theGeneratorIndex);
  for (int j=0; j<this->parabolicSelectionNonSelectedAreElementsLevi.CardinalitySelection; j++)
    if (!theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[j]].IsEqualToZero())
      return true;
  return false;
}

template <class coefficient>
void ModuleSSalgebra<coefficient>::GetGenericUnMinusElt
 (bool shiftPowersByNumVarsBaseField, ElementUniversalEnveloping<RationalFunctionOld>& output,
  bool useNilWeight, bool ascending)
{ MacroRegisterFunctionWithName("ModuleSSalgebra::GetGenericUnMinusElt");
  List<ElementUniversalEnveloping<coefficient> > eltsNilrad;
  this->GetElementsNilradical(eltsNilrad, true, useNilWeight, ascending);
  RationalFunctionOld tempRF;
  output.MakeZero(*this->theAlgebras, this->indexAlgebra);
  MonomialUniversalEnveloping<RationalFunctionOld> tempMon;
  tempMon.MakeConst(*this->theAlgebras, this->indexAlgebra);
  int varShift=0;
  if (shiftPowersByNumVarsBaseField)
    varShift=this->GetMinNumVars();
  int numVars=varShift+eltsNilrad.size;
  for (int i=0; i<eltsNilrad.size; i++)
  { tempRF.MakeOneLetterMoN(i+varShift, 1);
    tempMon.MultiplyByGeneratorPowerOnTheRight(eltsNilrad[i][0].generatorsIndices[0], tempRF);
  }
  tempRF.MakeOne();
  output.AddMonomial(tempMon, tempRF);
}

template <class coefficient>
void ModuleSSalgebra<coefficient>::GetGenericUnMinusElt
(bool shiftPowersByNumVarsBaseField, ElementUniversalEnveloping<Polynomial<Rational> >& output,
 bool useNilWeight, bool ascending)
{ MacroRegisterFunctionWithName("ModuleSSalgebra::GetGenericUnMinusElt");
  List<ElementUniversalEnveloping<coefficient> > eltsNilrad;
  this->GetElementsNilradical(eltsNilrad, true, 0, useNilWeight, ascending);
  Polynomial<Rational> tempRF;
  output.MakeZero(*this->owner);
  MonomialUniversalEnveloping<Polynomial<Rational> > tempMon;
  tempMon.MakeOne(*this->owner);
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
  void GenerateBasisLieAlgebra(List<quasiDiffOp<coefficient> >& theElts, FormatExpressions* theFormat=0);
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
//  stOutput << "<hr>[ " << standsOnTheLeft.ToString(&tempFormat);
//  stOutput << " , " << standsOnTheRight.ToString(&tempFormat) << " ] = ";
  output=standsOnTheLeft;
  output*=standsOnTheRight;
//  stOutput << "<br>intermediate: " << output.ToString(&tempFormat);
  tempE=standsOnTheRight;
  tempE*=standsOnTheLeft;
//  stOutput << "<br>intermediate2: " << tempE.ToString(&tempFormat);
  output-=tempE;
//  stOutput << "<br>finally:" << output.ToString(&tempFormat) << "<hr>";
}

template <class coefficient>
void quasiDiffOp<coefficient>::GenerateBasisLieAlgebra(List<quasiDiffOp<coefficient> >& theElts, FormatExpressions* theFormat)
{ MacroRegisterFunctionWithName("quasiDiffOp<coefficient>::GenerateBasisLieAlgebra");
  ProgressReport theReport (theGlobalVariables);
  HashedList<quasiDiffMon> bufferMons;
  //stOutput << "<br> the elts:" << theElts.ToString();
  List< MonomialCollection<quasiDiffMon, coefficient> > theEltsConverted;
  theEltsConverted=theElts;
  //stOutput << "<br>the elts converted: " << theEltsConverted.ToString();
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
        //      stOutput << "<hr>Lie bracketing " << tempQDO.ToString(&tempFormat)
  //      << " and " << theEltsConverted[j].ToString(&tempFormat);
        tempQDO.LieBracketMeOnTheRight(theEltsConverted[j]);
        theReport.Report(report.str());
        report << tempQDO.ToString(theFormat);
        theReport.Report(report.str());
  //      stOutput << ", to get " << tempQDO.ToString(&tempFormat);
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
  stOutput << "<hr>" << "<b>Num times Gaussian Elimination was called = " << numTimesEliminationWasExecuted << ".</b>";
  theElts.SetSize(theEltsConverted.size);
  for (int i=0; i<theEltsConverted.size; i++)
    theElts[i]=theEltsConverted[i];
//  stOutput << "<hr>the elts at end: ";
//  theElts=theEltsConverted;
//  for (int i=0; i<theElts.size; i++)
//    stOutput << "<br>" << theElts[i].ToString();
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
bool ModuleSSalgebra<coefficient>::GetActionEulerOperatorPart(const MonomialP& theCoeff, ElementWeylAlgebra<Rational>& outputDO)
{ MacroRegisterFunctionWithName("ModuleSSalgebra_CoefficientType::GetActionMonGenVermaModuleAsDiffOperator");
//  int varShift=this->GetMinNumVars();
//  stOutput << "<br>varShift for Euler operator: " << varShift;
  int powerMonCoeff=0;
  ElementWeylAlgebra<Rational> currentMonContribution;
  outputDO.MakeOne();
//  stOutput << "<br>Getting Euler part contribution of " << theCoeff.ToString()
//  <<  " with min num vars equal to " << theCoeff.GetMinNumVars();
  for (int i=0; i<theCoeff.GetMinNumVars(); i++)
  { if (!theCoeff(i).IsSmallInteger(&powerMonCoeff))
      crash << "This is a programming error. Getting euler operator part of action on generalized Verma module: I have an exponent with non-small integer entry. "
      << crash;
    currentMonContribution.Makexidj(i, i, 0);
    currentMonContribution.RaiseToPower(powerMonCoeff);
    outputDO*=currentMonContribution;
    //stOutput << "<br>Accounted index " << i+1 << "  out of " << theCoeff.GetMinNumVars()
    //<< ", power is " << powerMonCoeff << ", current output DO has "
    //<< outputDO.size << " monomials.";
  }
//  stOutput << ".. Final euler: "  << outputDO.ToString();
  return true;
}

template <class coefficient>
bool ModuleSSalgebra<coefficient>::GetActionGenVermaModuleAsDiffOperator
(ElementSemisimpleLieAlgebra<Rational>& inputElt, quasiDiffOp<Rational>& output,
 bool useNilWeight, bool ascending)
{ MacroRegisterFunctionWithName("ModuleSSalgebra_CoefficientType::GetActionGenVermaModuleAsDiffOperator");
  List<ElementUniversalEnveloping<coefficient> > eltsNilrad;
  List<int> indicesNilrad;
  this->GetElementsNilradical(eltsNilrad, true, &indicesNilrad, useNilWeight, ascending);
  ElementUniversalEnveloping<Polynomial<Rational> > theGenElt, result;
  this->GetGenericUnMinusElt(true, theGenElt, useNilWeight, ascending);
//  Polynomial<Rational> Pone, Pzero;
  result.AssignElementLieAlgebra(inputElt, *this->owner, 1);
  std::stringstream out;
//  stOutput << "<br>the generic elt:" << CGI::GetHtmlMathSpanPure(theGenElt.ToString());
  theGenElt.Simplify();
//  stOutput << "<br>the generic elt simplified:" << CGI::GetHtmlMathSpanPure(theGenElt.ToString());
//  stOutput << "<br>" << CGI::GetHtmlMathSpanPure(result.ToString() ) << " times " << CGI::GetHtmlMathSpanPure(theGenElt.ToString()) << " = ";
  result*=(theGenElt);
  result.Simplify();
//  stOutput << " <br>" << CGI::GetHtmlMathSpanPure(result.ToString());
  MatrixTensor<Polynomial<Rational> > endoPart, tempMT, idMT;
  idMT.MakeIdSpecial();
  MatrixTensor<RationalFunctionOld> tempMat1;

  int varShift=this->GetMinNumVars();
//  stOutput  << "<br>Num elements nilrad: " << indicesNilrad.size;
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
      tempMat1=this->GetActionGeneratorIndeX(currentMon.generatorsIndices[j]);
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
        //stOutput << "ere be problem!";
      //problemCounter++;
      currentMon.Powers[j].GetConstantTerm(currentShift);
      ElementWeylAlgebra<Rational>::GetStandardOrderDiffOperatorCorrespondingToNraisedTo
      (currentShift, j+varShift, oneIndexContribution, negativeExponentDenominatorContribution);
//      stOutput << "<br>result from GetStandardOrderDiffOperatorCorrespondingToNraisedTo: "
//      << negativeExponentDenominatorContribution.ToString() << " divided by "
//      << oneIndexContribution.ToString();
      exponentContribution*=oneIndexContribution;
      theCoeff.DivideBy(negativeExponentDenominatorContribution, theCoeff, tempP1);
      if (!tempP1.IsEqualToZero())
        crash << "This is a mathematical error! Something is very wrong with embedding semisimple Lie algebras in Weyl algebras. " << crash;
    }
//    stOutput << "<br>Endo part of " << currentMon.ToString() << ": " << endoPart.ToString();
//    stOutput << "<br>Exponent contribution of " << currentMon.ToString() << ": "
//    << exponentContribution.ToString();
    for (int l=0; l<theCoeff.size(); l++)
    { //problemCounter++;
      //if (problemCounter==249)
        //stOutput << "ere be problem!";
      if (!this->GetActionEulerOperatorPart(theCoeff[l], eulerOperatorContribution))
        return false;
//      stOutput << "<br>Euler operator contribution: " << eulerOperatorContribution.ToString();
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
  //          stOutput << "<br>adding " << monQDO.ToString() << " times "
  //          << tempRat.ToString()  << " to "
  //          << output.ToString();
            output.AddMonomial(monQDO2, tempRat);
  //          if (problemCounter==5)
  //          stOutput << " to get " << output.ToString();
          }
        }
    }
  }
//  stOutput << "<br>result qdo: " << output.ToString();
  return true;
}

bool Calculator::innerWriteGenVermaModAsDiffOperatorInner
(Calculator& theCommands, const Expression& input, Expression& output, Vectors<Polynomial<Rational> >& theHws, Expression& hwContext,
 Selection& selInducing, SemisimpleLieAlgebra* owner, bool AllGenerators, std::string* xLetter,
 std::string* partialLetter, std::string* exponentVariableLetter, bool useNilWeight, bool ascending)
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
//      theGenerator.MakeHgenerator(ei, theSSalgebra);
//      theGeneratorsItry.AddOnTop(theGenerator);
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
  //stOutput << "<br>num starting vars:" << numStartingVars;
  std::stringstream reportFourierTransformedCalculatorCommands, reportCalculatorCommands;
  long long totalAdditions=0;
  long long currentAdditions=0;
  long long totalMultiplications=0;
  long long currentMultiplications=0;
  double totalTime=0, currentTime=0;
  for (int i=0; i<theHws.size; i++)
  { ModuleSSalgebra<RationalFunctionOld>& theMod=theMods[i];
    tempV=theHws[i];
    if (!theMod.MakeFromHW(theSSalgebra, tempV, selInducing, 1, 0, 0, true))
      return output.MakeError("Failed to create module.", theCommands);
    if (i==0)
    { theMod.GetElementsNilradical(elementsNegativeNilrad, true,0, useNilWeight, ascending);
      Polynomial<Rational> Pone, Pzero;
      Pone.MakeOne(elementsNegativeNilrad.size+theMod.GetMinNumVars());
      Pzero.MakeZero();
      theMod.GetGenericUnMinusElt(true, genericElt, useNilWeight, ascending);
      //stOutput << "<br>highest weight: " << tempV.ToString();
      //stOutput << "<br>generic elt: " <<  genericElt.ToString();

      //stOutput << "<br>theWeylFormat: ";
//      for (int k=0; k<theWeylFormat.polyAlphabeT.size; k++)
//        stOutput << theWeylFormat.polyAlphabeT[k] << ", ";
      theWeylFormat.polyAlphabeT.SetSize(numStartingVars+ elementsNegativeNilrad.size);
      theWeylFormat.weylAlgebraLetters.SetSize(numStartingVars+ elementsNegativeNilrad.size);
      theUEformat.polyAlphabeT.SetSize(numStartingVars+ elementsNegativeNilrad.size);
      for (int k=0; k<numStartingVars; k++)
        theWeylFormat.weylAlgebraLetters[k]="error";
      //stOutput << "<br>HW num context vars: " << hwContext.ContextGetNumContextVariables();
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
          return output.MakeError
          ("Error: the variable "+ tempstream2.str()+" is Reserved for me: you are not allowed to use it as a coordinate of the highest weight. ", theCommands);
        theWeylFormat.polyAlphabeT[k]=tempstream2.str();
        theWeylFormat.weylAlgebraLetters[k]=tempStream4.str();
      }
//      stOutput << "<br>theUEformat: ";
//      for (int k=0; k<theUEformat.polyAlphabeT.size; k++)
//        stOutput << theUEformat.polyAlphabeT[k] << ", ";
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
      { actionOnGenericElt.AssignElementLieAlgebra(theGeneratorsItry[j], theSSalgebra, Pone);
        actionOnGenericElt*=(genericElt);
        theSSalgebra.OrderNilradical(theMod.parabolicSelectionNonSelectedAreElementsLevi, useNilWeight, ascending);
        actionOnGenericElt.Simplify();
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
    //stOutput << "<hr>hw: " << theMod.theHWFundamentalCoordsBaseField.ToString() << " nilrad elts: " << elementsNegativeNilrad.ToString();
    //stOutput << "<br>generic element: " << genericElt.ToString();
    for (int j=0; j<theGeneratorsItry.size; j++)
    { theGenerator=theGeneratorsItry[j];
      currentTime= theGlobalVariables.GetElapsedSeconds();
      currentAdditions=Rational::TotalAdditions();
      currentMultiplications=Rational::TotalMultiplications();
      theMod.GetActionGenVermaModuleAsDiffOperator(theGenerator, theQDOs[j], useNilWeight, ascending);
      totalAdditions+=Rational::TotalAdditions()-currentAdditions;
      totalMultiplications+=Rational::TotalMultiplications()-currentMultiplications;
      totalTime+=theGlobalVariables.GetElapsedSeconds()-currentTime;
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
      reportFourierTransformedCalculatorCommands << "x_{{i}}=ElementWeylAlgebraPoly{}(\\partial_i, x_i);\n<br>"
      << "\\partial_{{i}}=ElementWeylAlgebraDO{}(\\partial_i, x_i);\n";
      reportCalculatorCommands << "<hr>" << CGI::GetMathMouseHover(theMod.theChaR.ToString())
      << ", differential operators - formatted for calculator input. <br><br>";
      reportCalculatorCommands << "x_{{i}}=ElementWeylAlgebraPoly{}(\\partial_i, x_i);\n<br>"
      << "\\partial_{{i}}=ElementWeylAlgebraDO{}(\\partial_i, x_i);\n";

      for (int j=0; j<theGeneratorsItry.size; j++)
      { theQDOs[j].GetEWAsetMatrixPartsToId(diffOpPart);
        diffOpPart.FourierTransform(transformedDO);
        reportFourierTransformedCalculatorCommands << "<br>" << theGeneratorsItry[j].ToString() << "=" << transformedDO.ToString() << ";";
        reportCalculatorCommands << "<br>" << theGeneratorsItry[j].ToString() << "=" << diffOpPart.ToString() << ";";
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
//    theQDOs[0].GenerateBasisLieAlgebra(theQDOs, &theWeylFormat, &theGlobalVariables);
//    stOutput << "<br><b>Dimension generated Lie algebra: " << theQDOs.size << "</b>";
//    stOutput << "<br>The qdos: ";
//    for (int j=0; j<theQDOs.size; j++)
 //     stOutput << "<br>" << theQDOs[j].ToString();
  }
  latexReport << "\\end{longtable}";
  out << "</table>";
  out << "<br>Multiplications needed to embed generators: " << totalMultiplications << ". ";
  out << "<br>Additions needed to embed generators: " << totalAdditions << ". ";
  out << "<br>Total time to embed generators: see comments. ";
  //please keep this report in the comments section for reasons of reproducibility of the
  //string output!
  theCommands << "<hr>Total time to embed generators invoked by command: " << input.ToString()
  << ": <br>" << totalTime << " seconds. ";
  out << reportCalculatorCommands.str();
  out << reportFourierTransformedCalculatorCommands.str();
  out << "<br>" << latexReport.str();
  out << "<br><br>" << latexReport2.str();
  return output.AssignValue<std::string>(out.str(), theCommands);
}

template <class coefficient>
std::string ModuleSSalgebra<coefficient>::ToString(FormatExpressions* theFormat)const
{ MacroRegisterFunctionWithName("ModuleSSalgebra::ToString");
  if (this->owner==0)
    return "(Error: module not initialized)";
  SemisimpleLieAlgebra& theAlgebrA=*this->owner;
  WeylGroupData& theWeyl=theAlgebrA.theWeyl;
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
  ElementWeylGroup<WeylGroupData> tempWelt;
  int wordCounter=0;
  simpleReflectionOrOuterAuto aGen;
  for (int i=0; i<this->theGeneratingWordsGrouppedByWeight.size; i++)
  { List<MonomialUniversalEnveloping<coefficient> >& currentList=this->theGeneratingWordsGrouppedByWeight[i];
    List<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >& currentListInt=this->theGeneratingWordsIntGrouppedByWeight[i];
    for (int j=0; j<currentList.size; j++)
    { wordCounter++;
      tempWelt.generatorsLastAppliedFirst.SetSize(currentListInt[j].generatorsIndices.size);
      for (int k=0; k<currentListInt[j].generatorsIndices.size; k++)
      { aGen.MakeSimpleReflection(theWeyl.RootsOfBorel.size-1 -currentListInt[j].generatorsIndices[k]);
        tempWelt.generatorsLastAppliedFirst[k]=aGen;
      }
      out << "<tr><td>m_{ " << wordCounter << "} </td><td>" << currentList[j].ToString(&theGlobalVariables.theDefaultFormat.GetElement())
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
        //stOutput << outputMat.ToString(&latexFormat);
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
  this->theCharOverH.DrawMeAssumeCharIsOverCartan(theWeyl, theDV);
  out << " A picture of the weight support follows. " << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyl.GetDim());

  bool isBad=false;
  for (int k=0; k<this->theBilinearFormsAtEachWeightLevel.size; k++)
  { Matrix<coefficient>& theBF=this->theBilinearFormsAtEachWeightLevel[k];
    Matrix<coefficient>& theBFinverted= this->theBilinearFormsInverted[k];
    out << "<hr>weight in simple coords: " << this->theModuleWeightsSimpleCoords[k].ToString();
    List<MonomialUniversalEnveloping<coefficient> >& currentList=this->theGeneratingWordsGrouppedByWeight[k];
    for (int i=0; i<currentList.size; i++)
    { MonomialUniversalEnveloping<coefficient>& currentElt=currentList[i];
      out << "<br>monomial " << i+1 << ": " << currentElt.ToString(&theGlobalVariables.theDefaultFormat.GetElement());
    }
    out << "; Matrix of Shapovalov form associated to current weight level: <br> " << theBF.ToString(&theGlobalVariables.theDefaultFormat.GetElement());
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
      out << theBFinverted.ToString(&theGlobalVariables.theDefaultFormat.GetElement());
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
  //  stOutput << "<br>highest weight in fundamental coords: " << highestWeightFundCoords.ToString() << "<br>";
//  stOutput << "<br>parabolic selection: " << parabolicSel.ToString();
  RationalFunctionOld RFOne, RFZero;
  RFOne.MakeOne();
  RFZero.MakeZero();
  std::string report;
  ElementTensorsGeneralizedVermas<RationalFunctionOld> theElt;
  //=theElementData.theElementTensorGenVermas.GetElement();
  ListReferences<ModuleSSalgebra<RationalFunctionOld> >& theMods= theCommands.theObjectContainer.theCategoryOmodules;
  int indexOfModule=-1;

  for (int i=0; i<theMods.size; i++)
  { ModuleSSalgebra<RationalFunctionOld>& currentMod=theMods[i];
    if (highestWeightFundCoords==currentMod.theHWFundamentalCoordsBaseField && selectionParSel==currentMod.parabolicSelectionNonSelectedAreElementsLevi && currentMod.owner==owner)
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
  { bool isGood=theMod.MakeFromHW(*owner, highestWeightFundCoords, selectionParSel, RFOne, RFZero, &report);
    if (Verbose)
      theCommands << theMod.ToString();
    if (!isGood)
      return output.MakeError("Error while generating highest weight module. See comments for details. ", theCommands);
  }
  if (&theMod.GetOwner()!=owner)
    crash << "This is a programming error: module has owner that is not what it should be. " << crash;
  theElt.MakeHWV(theMod, RFOne);
  if (&theElt.GetOwnerSS()!=owner)
    crash << "This is a programming error: just created an ElementTensorsGeneralizedVermas whose owner is not what it should be. " << crash;
  return output.AssignValueWithContext<ElementTensorsGeneralizedVermas<RationalFunctionOld> >(theElt, hwContext, theCommands);
}

bool Calculator::RecursionDepthExceededHandleRoughly(const std::string& additionalErrorInfo)
{ if (this->RecursionDeptH<=this->MaxRecursionDeptH)
    return false;
  if (!this->flagMaxRecursionErrorEncountered)
    *this << additionalErrorInfo << "<span style=\"color:#FF0000\"><b> Maximum recursion depth of " << this->MaxRecursionDeptH
    << " exceeded. </b></span>" << "Aborting computation ASAP. ";
  this->flagAbortComputationASAP=true;
  this->flagMaxRecursionErrorEncountered=true;
  return true;
}

bool Calculator::CheckConsistencyAfterInitializationExpressionStackEmpty()
{ this->theExpressionContainer.GrandMasterConsistencyCheck();
  this->EvaluatedExpressionsStack.GrandMasterConsistencyCheck();
  this->cachedExpressions.GrandMasterConsistencyCheck();
  if (this->cachedExpressions.size!=0 || this->imagesCachedExpressions.size!=0 || this->EvaluatedExpressionsStack.size!=0 || this->theExpressionContainer.size!=0)
    crash << "This is a programming error: cached expressions, images cached expressions, expression stack and expression container are supposed to be empty, but "
    << " instead they contain respectively " << this->cachedExpressions.size << ", " << this->imagesCachedExpressions.size << ", "
    << this->EvaluatedExpressionsStack.size << " and " << this->theExpressionContainer.size << " elements. " << crash;
  return true;
}

bool Calculator::innerFunctionToMatrix(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerFunctionToMatrix");
//  stOutput << input.ToString();
  if (!input.IsListNElements(4))
    return false;
  const Expression& leftE  =input[1];
  const Expression& middleE=input[2];
  const Expression& rightE =input[3];
//  stOutput << leftE.ToString() << ", " << rightE.ToString() << ", " << middleE.ToString();
  int numRows, numCols;
  if (!middleE.IsSmallInteger(&numRows) || !rightE.IsSmallInteger(&numCols))
    return false;
//  stOutput << "<br>Rows, cols: " << numRows << ", " << numCols;
  if (numRows<=0 || numCols<=0)
    return false;
  if (numRows>1000 || numCols>1000)
  { theCommands << "Max number of rows/columns is 1000. You requested " << numRows << " rows and " << numCols << " columns.<br>";
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
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, input[1], theSSalg))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  int theIndex;
  if (!input[2].IsSmallInteger(&theIndex))
    return false;
  if (theIndex>theSSalg->GetNumPosRoots() || theIndex==0 || theIndex<-theSSalg->GetNumPosRoots())
    return output.MakeError("Bad Chevalley-Weyl generator index.", theCommands);
  ElementSemisimpleLieAlgebra<Rational> theElt;
  if (theIndex>0)
    theIndex+=theSSalg->GetRank()-1;
  theIndex+=theSSalg->GetNumPosRoots();
  theElt.MakeGenerator(theIndex, *theSSalg);
  ElementUniversalEnveloping<RationalFunctionOld> theUE;
  theUE.AssignElementLieAlgebra(theElt, *theSSalg);
  Expression theContext;
  int indexInOwner=theCommands.theObjectContainer.theSSLieAlgebras.GetIndex(theSSalg->theWeyl.theDynkinType);
  //theCommands.ToString();
  theContext.ContextMakeContextSSLieAlgebrA(indexInOwner, theCommands);
  //theCommands.ToString();
  output.AssignValueWithContext(theUE, theContext, theCommands);
  //theCommands.ToString();
  return true;
}

bool Calculator::innerGetCartanGen(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElements(3))
    return false;
  SemisimpleLieAlgebra* theSSalg=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, input[1], theSSalg))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  if (theSSalg==0)
    crash << "This is a programming error: called conversion function successfully, but the output is a zero pointer to a semisimple Lie algebra. "
    << crash;
  int theIndex;
  if (!input[2].IsSmallInteger(&theIndex))
    return false;
  if (theIndex==0 || theIndex>theSSalg->GetNumPosRoots() || theIndex <-theSSalg->GetNumPosRoots() )
    return output.MakeError("Bad Cartan subalgebra generator index.", theCommands);
  ElementSemisimpleLieAlgebra<Rational> theElt;
  Vector<Rational> theH=theSSalg->theWeyl.RootSystem[theSSalg->GetRootIndexFromDisplayIndex(theIndex)];
  theElt.MakeHgenerator(theH, *theSSalg);
  ElementUniversalEnveloping<RationalFunctionOld> theUE;
  theUE.AssignElementLieAlgebra(theElt, *theSSalg);
  Expression theContext;
  int theAlgIndex=theCommands.theObjectContainer.theSSLieAlgebras.GetIndex(theSSalg->theWeyl.theDynkinType);
  theContext.ContextMakeContextSSLieAlgebrA(theAlgIndex, theCommands);
  return output.AssignValueWithContext(theUE, theContext, theCommands);
}

bool Calculator::innerKLcoeffs(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerKLcoeffs");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  SemisimpleLieAlgebra* theSSalgebra=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, input, theSSalgebra))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  std::stringstream out;
  WeylGroupData& theWeyl=theSSalgebra->theWeyl;
  if (theWeyl.theGroup.GetSize()>192)
  { out << "I have been instructed to run only for Weyl groups that have at most 192 elements (i.e. no larger than D_4). "
    << theSSalgebra->GetLieAlgebraName() << " has " << theWeyl.theGroup.GetSize().ToString() << ".";
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
//  double startTimeDebug=theGlobalVariables.GetElapsedSeconds();
  SemisimpleLieAlgebra *tempSSpointer=0;
  input.CheckInitialization();
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, input, tempSSpointer))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  SemisimpleLieAlgebra& theSSalgebra=*tempSSpointer;
  WeylGroupData& theWeyl=theSSalgebra.theWeyl;
  std::stringstream out;
  FormatExpressions theFormat, latexFormat;
  latexFormat.flagUseLatex=true;
  latexFormat.flagUseHTML=false;
//      theFormat.chevalleyHgeneratorLetter="\\bar{h}";
//      theFormat.chevalleyGgeneratorLetter="\\bar{g}";
  out << "<hr>Lie algebra type: " << theWeyl.theDynkinType << ". ";
  out << "<br>Weyl group size: " << theWeyl.theGroup.GetSize().ToString() << "." << "<br>To get extra details: ";
  std::stringstream tempStream;
  tempStream << "printSemisimpleLieAlgebra{}(" << theWeyl.theDynkinType << ")";
  out << theCommands.GetCalculatorLink(tempStream.str()) << "<br>";
  if (Verbose)
  { DrawingVariables theDV;
    theWeyl.DrawRootSystem(theDV, true, true, 0, true, 0);
    out << "<hr>Below is a drawing of the root system in its corresponding Coxeter plane (computed as explained on John Stembridge's website). "
    << "<br>The darker red dots can be dragged with the mouse to rotate the picture.<br>The grey lines are the edges of the Weyl chamber.<br>"
    << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyl.GetDim());
    out << theWeyl.ToStringRootsAndRootReflections();
    out << " The resulting Lie bracket pairing table follows. <hr> " << theSSalgebra.ToString(&theGlobalVariables.theDefaultFormat.GetElement());
    if (theCommands.flagWriteLatexPlots)
    { out << "Ready for LaTeX consumption version of the first three columns: ";
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
  }
  out << "We define the symmetric Cartan matrix <br>by requesting that the entry in the i-th row and j-th column<br> "
  << " be the scalar product of the i^th and j^th roots. The symmetric Cartan matrix is:<br>"
  << CGI::GetMathSpanPure(theWeyl.CartanSymmetric.ToString(&latexFormat));
  out << "<br>Let the (i,j)^{th} entry of the symmetric Cartan matrix be a_{ij}. <br> Then we define the co-symmetric Cartan matrix as "
  << " the matrix whose (i,j)^{th} entry equals 4*a_{ij}/(a_{ii}*a_{jj}). In other words, the co-symmetric Cartan matrix is the "
  << "symmetric Cartan matrix of the dual root system. The co-symmetric Cartan matrix equals:<br>"
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
  //    stOutput << "<br>" << j+1 << ": " << theRatio.ToString() << "=? " << tempRat.ToString();
    }*/
  //Lattice tempLattice;
  //theWeyl.GetIntegralLatticeInSimpleCoordinates(tempLattice);
  //out << "<br>The integral lattice in simple coordinates is (generated by): " << tempLattice.ToString(true, false);
  //Vectors<Rational> integralRoots, integralRootsEpsForm;
//  stOutput << "<br>DEBUG: time before getting fund coords in simple coords: "
//  << theGlobalVariables.GetElapsedSeconds()-startTimeDebug;
  Vectors<Rational> fundamentalWeights, fundamentalWeightsEpsForm;
  //integralRoots.AssignMatrixRows(tempLattice.basisRationalForm);
  //theWeyl.GetEpsilonCoords(integralRoots, integralRootsEpsForm);
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
  out << "<hr>Below is the simple basis realized in epsilon coordinates. "
  << "Please note that the epsilon coordinate realizations "
  << "do not have long roots of length of 2 in types G and C. "
  << "This means that gramm matrix (w.r.t. the standard scalar product)"
  << " of the epsilon coordinate realizations in types G and C does not equal the  corresponding symmetric Cartan matrix."
  << "  <table>";
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
    { Matrix<Rational> tempM, tempM2;
      theWeyl.theDynkinType.GetEpsilonMatrix(tempM);
      tempM2=tempM;
      tempM2.Transpose();
      tempM2.MultiplyOnTheRight(tempM);
      tempM2*=2/tempSimpleType.GetEpsilonRealizationLongRootLengthSquared();
      if (!(tempM2==theWeyl.CartanSymmetric))
        crash << "This is a (non-critical) programming error: the epsilon coordinates of the vectors are incorrect. "
        << "Please fix function DynkinType::GetEpsilonMatrix. The matrix of the epsilon coordinates is " << tempM.ToString()
        << ", the Symmetric Cartan matrix is " << theWeyl.CartanSymmetric.ToString() << ", and the  "
        << "transpose of the epsilon matrix times the epsilon matrix:  " << tempM2.ToString() << ". " << crash;
    }
  return output.AssignValue<std::string>(out.str(), theCommands);
}

bool Expression::CheckInitializationRecursively()const
{ MacroRegisterFunctionWithName("Expression::CheckInitializationRecursively");
  this->CheckInitialization();
  for (int i=0; i<this->children.size; i++)
    (*this)[i].CheckInitializationRecursively();
  return true;
}

bool Expression::CheckInitialization()const
{ if (this->owner==0)
  { crash << "This is a programming error: " << "Expression has non-initialized owner. " << crash;
    return false;
  }
  return true;
}

bool Expression::HasInputBoxVariables(HashedList<std::string, MathRoutines::hashString>* boxNames)const
{ MacroRegisterFunctionWithName("Expression::HasInputBoxVariables");
  if (this->owner==0)
    return false;
  RecursionDepthCounter recursionCounter(&this->owner->RecursionDeptH);
  if (this->owner->RecursionDeptH>this->owner->MaxRecursionDeptH)
    crash << "This is a programming error: "
    << "function HasInputBoxVariables has exceeded "
    << "recursion depth limit. " << crash;
  bool result=false;
  InputBox tempBox;
  if (this->IsOfType<InputBox>(&tempBox))
  {// stOutput << "DEBUG: Processing: " << this->ToString()
   // << " lispified " << this->ToStringSemiFull();
    if (boxNames==0)
      return true;
    else
    { result=true;
      boxNames->AddOnTopNoRepetition(tempBox.name);
    }
  }
  for (int i=0; i<this->size(); i++)
    if ((*this)[i].HasInputBoxVariables(boxNames))
    { if (boxNames==0)
        return true;
      else
        result=true;
    }
  return result;
}

bool Expression::HasBoundVariables()const
{ if (this->owner==0)
    crash << "This is a programming error: calling function HasBoundVariables on non-initialized expression. " << crash;
  RecursionDepthCounter recursionCounter(&this->owner->RecursionDeptH);
  MacroRegisterFunctionWithName("Expression::HasBoundVariables");
  if (this->owner->RecursionDeptH>this->owner->MaxRecursionDeptH)
    crash << "This is a programming error: function HasBoundVariables has exceeded recursion depth limit. " << crash;
  if (this->IsListOfTwoAtomsStartingWith(this->owner->opBind()))
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
  if (input.IsRational())
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

//Expression::FunctionAddress Calculator::GetInnerFunctionFromOp(int theOp, const Expression& left, const Expression& right)
//{ crash << "Function Calculator::GetfOp not implemented yet. " << crash;
//  return 0;
//}

bool Calculator::outerTensor(Calculator& theCommands, const Expression& input, Expression& output)
{ //stOutput << "<br>At start of evaluate standard times: " << theExpression.ToString();
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("Calculator::StandardTensor");
  if (theCommands.outerDistribute(theCommands, input, output, theCommands.opPlus(), theCommands.opTensor()))
    return true;
  //stOutput << "<br>After distribute: " << theExpression.ToString();
  if (theCommands.outerAssociate(theCommands, input, output))
    return true;
  if (theCommands.outerExtractBaseMultiplication(theCommands, input, output))
    return true;
//  if (theExpression.children.size!=2)
//    return false;
  //stOutput << "<br>After do associate: " << theExpression.ToString();
  return false;
}

bool Calculator::innerMultiplyAtoXtimesAtoYequalsAtoXplusY(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerMultiplyAtoXtimesAtoYequalsAtoXplusY");
  if (!input.StartsWith(theCommands.opTimes(), 3))
    return false;
//  stOutput << "<hr>Collecting multiplicands. input: " << input.ToString();
  Expression constPower, thePower;
  const Expression* left= &input[1];
  const Expression* right=&input[2];
  if (*left==*right)
  { constPower.AssignValue(2, theCommands);
    output.MakeXOX(theCommands, theCommands.opThePower(), *left, constPower);
    //stOutput << "<br>output be at first place: " << output.ToString();
    return true;
  }
  for (int i=0; i<2; i++, MathRoutines::swap(left, right))
    if (right->StartsWith(theCommands.opThePower(), 3))
    { if ((*right)[1]==(*left))
      { bool isGood=true;
//        stOutput << "<br>Right is: " << right->ToString();
       // if ((*right)[2].IsOfType<Rational>())
       //   if (!(*right)[2].GetValue<Rational>().IsInteger())
       //   { Rational ratBase;
       //     if ((*right)[1].IsRational(&ratBase))
       //       if (ratBase<0)
       //         isGood=false;
       //   }
        if(isGood)
        { constPower.AssignValue(1, theCommands);
          thePower.MakeXOX(theCommands, theCommands.opPlus(), (*right)[2], constPower);
          return output.MakeXOX(theCommands, theCommands.opThePower(), *left, thePower);
        }
      }
      if (left->StartsWith(theCommands.opThePower(), 3))
        if ((*left)[1]==(*right)[1])
        { thePower.MakeXOX(theCommands, theCommands.opPlus(), (*left)[2], (*right)[2]);
          return output.MakeXOX(theCommands, theCommands.opThePower(), (*left)[1], thePower);
          //stOutput << "<br>output be at second place: " << output.ToString();
        }
    }
  return false;
}

bool Calculator::outerCombineFractions(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.StartsWith(theCommands.opPlus(), 3))
    return false;
  const Expression* quotientE=0;
  const Expression* summandE=0;
  if (input[1].StartsWith(theCommands.opDivide(), 3))
  { quotientE=&input[1];
    summandE=&input[2];
  } else if (input[2].StartsWith(theCommands.opDivide(), 3))
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
{ if (!input.StartsWith(theCommands.opDefine(), 3))
    return false;
  if (input[1]!=input[2])
    return false;
  std::stringstream out;
  out << "Bad rule: you are asking me to substitute " << input[1] << " by itself (" << input[2] << ")" << " which is an infinite substitution cycle. ";
  return output.MakeError(out.str(), theCommands);
}

bool Calculator::innerSubZeroDivAnythingWithZero(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.StartsWith(theCommands.opDivide(), 3))
    return false;
  if (input[1].IsEqualToZero())
    if (!input[2].IsEqualToZero())
      return output.AssignValue(0, theCommands);
  return false;
}

bool Calculator::innerCancelMultiplicativeInverse(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.StartsWith(theCommands.opTimes(), 3))
    return false;
  if (!input[1].StartsWith(theCommands.opDivide(), 3))
    return false;
  if (input[1][2]==input[2])
  { output=input[1][1];
    return true;
  }
  return false;
}

bool Calculator::outerAssociateTimesDivision(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.StartsWith(theCommands.opTimes(), 3))
    return false;
  if (!input[2].StartsWith(theCommands.opDivide(), 3))
    return false;
  Expression newLeftE;
  newLeftE.MakeXOX(theCommands, theCommands.opTimes(), input[1], input[2][1]);
  output.MakeXOX(theCommands, theCommands.opDivide(), newLeftE, input[2][2]);
  return true;
}

bool Calculator::outerAssociate(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.StartsWith(-1, 3))
    return false;
  int theOperation=input[0].theData;
  if (!input[1].StartsWith(theOperation, 3))
    return false;
  Expression newRight;
  newRight.MakeXOX(theCommands, theOperation, input[1][2], input[2]);
  output.MakeXOX(theCommands, theOperation, input[1][1], newRight);
  return true;
}

bool Calculator::StandardIsDenotedBy(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::StandardIsDenotedBy");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (!input.StartsWith(theCommands.opIsDenotedBy(), 3))
    return false;
  const Expression& withNotation=input[2];
  const Expression& theNotation=input[1];
  theCommands << "<br>Registering notation: globally, " << withNotation.ToString() << " will be denoted by "
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
{ MacroRegisterFunctionWithName("Calculator::innerMultiplyByOne");
  if (!input.IsListStartingWithAtom(theCommands.opTimes()) || input.children.size!=3)
    return false;
  if (!input[1].IsEqualToOne())
    return false;
  output=input[2];
  return true;
}

bool Calculator::GetVectorLargeIntFromFunctionArguments(const Expression& input, List<LargeInt>& output)
{ MacroRegisterFunctionWithName("Calculator::GetVectorLargeIntFromFunctionArguments");
  Vector<Rational> theRats;
  if (!this->GetVectorFromFunctionArguments(input, theRats))
    return false;
  output.initFillInObject(theRats.size,0);
  for (int i=0; i< theRats.size; i++)
    if (!theRats[i].IsInteger(&output[i]))
      return *this << "<hr>Succeeded to convert " << input.ToString() << " to the vector of rationals: "
      << theRats.ToString() << " but failed to convert that to list of integers. ";
  return true;
}

bool Calculator::GetVectoRInt(const Expression& input, List<int>& output)
{ MacroRegisterFunctionWithName("Calculator::GetVectoRInt");
  Vector<Rational> theRats;
  if (!this->GetVectoR(input, theRats))
    return false;
  output.initFillInObject(theRats.size,0);
  for (int i=0; i< theRats.size; i++)
    if (!theRats[i].IsSmallInteger(&output[i]))
      return *this << "<hr>Succeeded to convert " << input.ToString() << " to the vector of rationals: "
      << theRats.ToString() << " but failed to convert that to list of small integers. ";
  return true;
}

bool Calculator::outerTimesToFunctionApplication(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::outerTimesToFunctionApplication");
  if (!input.StartsWith(theCommands.opTimes()))
    return false;
  if (input.children.size<2)
    return false;
  const Expression& firstElt=input[1];
//  stOutput << " <hr>outer times to function ";
  if (!firstElt.IsBuiltInAtom())
  { if (!firstElt.StartsWith(theCommands.opThePower(), 3))
      return false;
    if (!firstElt[1].IsAtomWhoseExponentsAreInterpretedAsFunction())
      return false;
  }
  if (firstElt.IsAtomNotInterpretedAsFunction())
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

bool Calculator::outerDistribute(Calculator& theCommands, const Expression& input, Expression& output,
int theAdditiveOp, int theMultiplicativeOp)
{ if (theCommands.outerLeftDistributeBracketIsOnTheLeft(theCommands, input, output, theAdditiveOp, theMultiplicativeOp))
    return true;
  return theCommands.outerRightDistributeBracketIsOnTheRight(theCommands, input, output, theAdditiveOp, theMultiplicativeOp);
}

bool Calculator::outerDistributeTimes(Calculator& theCommands, const Expression& input, Expression& output)
{ return Calculator::outerDistribute(theCommands, input, output,theCommands.opPlus(), theCommands.opTimes());
}

bool Calculator::outerLeftDistributeBracketIsOnTheLeft
(Calculator& theCommands, const Expression& input, Expression& output,
 int theAdditiveOp, int theMultiplicativeOp)
{ MacroRegisterFunctionWithName("Calculator::outerLeftDistributeBracketIsOnTheLeft");
  if (theAdditiveOp==-1)
    theAdditiveOp= theCommands.opPlus();
  if(theMultiplicativeOp==-1)
    theMultiplicativeOp=theCommands.opTimes();
  if (!input.StartsWith(theMultiplicativeOp, 3))
    return false;
  if (!input[1].StartsWith(theAdditiveOp,3))
    return false;
//  int theFormat=input.format;
  Expression leftE, rightE;
  leftE.MakeXOX(theCommands, theMultiplicativeOp, input[1][1], input[2]);
  rightE.MakeXOX(theCommands, theMultiplicativeOp, input[1][2], input[2]);
  return output.MakeXOX(theCommands, theAdditiveOp, leftE, rightE);
}

bool Calculator::outerRightDistributeBracketIsOnTheRight
(Calculator& theCommands, const Expression& input, Expression& output,
 int theAdditiveOp, int theMultiplicativeOp)
{ MacroRegisterFunctionWithName("Calculator::outerRightDistributeBracketIsOnTheRight");
  if (theAdditiveOp==-1)
    theAdditiveOp= theCommands.opPlus();
  if (theMultiplicativeOp==-1)
    theMultiplicativeOp=theCommands.opTimes();
  if (!input.StartsWith(theMultiplicativeOp, 3))
    return false;
  if (!input[2].StartsWith(theAdditiveOp, 3))
    return false;
//  int theFormat=input.format;
  Expression leftE, rightE;
  leftE.MakeXOX(theCommands, theMultiplicativeOp, input[1], input[2][1]);
  rightE.MakeXOX(theCommands, theMultiplicativeOp, input[1], input[2][2]);
  return output.MakeXOX(theCommands, theAdditiveOp, leftE, rightE);
}

bool Calculator::CollectCoefficientsPowersVar
(const Expression& input, const Expression& theVariable, VectorSparse<Expression>& outputPositionIiscoeffXtoIth)
{ MacroRegisterFunctionWithName("Calculator::CollectCoefficientsPowersVar");
  List<Expression> theSummands, currentMultiplicands, remainingMultiplicands;
  Calculator& theCommands=*input.owner;
  theCommands.CollectOpands(input, theCommands.opPlus(), theSummands);
  Expression currentCoeff, constTerm;
  outputPositionIiscoeffXtoIth.MakeZero();
  for (int i=0; i<theSummands.size; i++)
  { theCommands.CollectOpands(theSummands[i], theCommands.opTimes(), currentMultiplicands);
    bool found=false;
    for (int j=0; j<currentMultiplicands.size; j++)
    { const Expression& currentE= currentMultiplicands[j];
      remainingMultiplicands=currentMultiplicands;
      remainingMultiplicands.RemoveIndexShiftDown(j);
      if (remainingMultiplicands.size==0)
        currentCoeff.AssignValue(1, theCommands);
      else
        currentCoeff.MakeProducT(theCommands, remainingMultiplicands);
      if (currentE==theVariable)
      { outputPositionIiscoeffXtoIth.AddMonomial(MonomialVector(1),currentCoeff);
        found=true;
        break;
      }
      if (currentE.StartsWith(theCommands.opThePower(), 3))
      { int thePower;
        if (currentE[1]==theVariable)
          if (currentE[2].IsSmallInteger(&thePower))
          { outputPositionIiscoeffXtoIth.AddMonomial(MonomialVector(thePower),currentCoeff);
            found=true;
            break;
          }
      }
    }
    if (!found)
      outputPositionIiscoeffXtoIth.AddMonomial(MonomialVector(0), theSummands[i]);
//    stOutput << "<br>Summand : " << theSummands[i] << ", vector status: " << outputPositionIiscoeffXtoIth.ToString();
  }
  return true;
}

bool Calculator::CollectOpands(const Expression& input, int theOp, List<Expression>& outputOpands)
{ MacroRegisterFunctionWithName("Calculator::CollectOpands");
  const Expression* currentE=&input;
  outputOpands.SetSize(0);
  while (currentE->StartsWith(theOp, 3))
  { outputOpands.AddOnTop((*currentE)[1]);
    currentE=&(*currentE)[2];
  }
  outputOpands.AddOnTop(*currentE);
  return true;
}

bool Calculator::CollectSummands
(Calculator& theCommands, const Expression& input, MonomialCollection<Expression, Rational>& outputSum)
{ MacroRegisterFunctionWithName("Calculator::CollectSummands");
  List<Expression> summands;
  theCommands.AppendSummandsReturnTrueIfOrderNonCanonical(input, summands);
  Expression oneE; //used to record the constant term
  oneE.AssignValue<Rational>(1, theCommands);
  outputSum.MakeZero();
  MonomialCollection<Expression, AlgebraicNumber> sumOverAlgNums;
  MonomialCollection<Expression, double> sumOverDoubles;
  Rational coeffRat=1;
  AlgebraicNumber coeffAlg=1;
  double coeffDouble=1;
//  stOutput << "The summands are: " << summands.ToStringCommaDelimited();
  for (int i=0; i<summands.size; i++)
  { if (summands[i].IsEqualToZero())
      continue;
    if (summands[i].StartsWith(theCommands.opTimes(), 3))
    { if (summands[i][1].IsOfType<Rational>(&coeffRat))
      { outputSum.AddMonomial(summands[i][2], coeffRat);
        continue;
      } else if (summands[i][1].IsOfType<AlgebraicNumber>(&coeffAlg))
      { if (coeffAlg.IsRational(&coeffRat))
        { outputSum.AddMonomial(summands[i][2], coeffRat);
          continue;
        }
        sumOverAlgNums.AddMonomial(summands[i][2], coeffAlg);
        continue;
      } else if (summands[i][1].IsOfType<double>(&coeffDouble))
      { sumOverDoubles.AddMonomial(summands[i][2], coeffDouble);
        continue;
      }
    }
    if (summands[i].IsRational(&coeffRat))
      outputSum.AddMonomial(oneE, coeffRat);
    else
      outputSum.AddMonomial(summands[i],1);
  }
  if (!sumOverDoubles.IsEqualToZero())
  { sumOverDoubles.QuickSortDescending();
    Expression doubleSum;
    doubleSum.MakeSum(theCommands, sumOverDoubles);
    outputSum.AddMonomial(doubleSum, 1);
  }
  if (!sumOverAlgNums.IsEqualToZero())
  { sumOverAlgNums.QuickSortDescending();
    Expression algSum;
    algSum.MakeSum(theCommands, sumOverAlgNums);
    outputSum.AddMonomial(algSum, 1);
  }
//  stOutput << "<hr> before mon sort, mon order: " << outputSum.theMonomials.ToString();
//  if (outputSum.theMonomials[0]>outputSum.theMonomials[1])
//    stOutput << outputSum.theMonomials[0].ToString() << " > " << outputSum.theMonomials[1].ToString();
//  else
//    stOutput << outputSum.theMonomials[0].ToString() << " < " << outputSum.theMonomials[1].ToString();
  outputSum.QuickSortDescending();
//  stOutput << " after mon sort: " << outputSum.theMonomials.ToString();
  return true;
}

bool Calculator::innerAssociateExponentExponent(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerAssociateExponentExponent");
  int opPower=theCommands.opThePower();
  if (!input.StartsWith(opPower, 3))
    return false;
  if (!input[1].StartsWith(opPower, 3))
    return false;
  Expression tempE;
  tempE.MakeProducT(theCommands, input[1][2], input[2]);
  output.MakeXOX(theCommands, opPower, input[1][1], tempE);
  return true;
}

bool Calculator::innerDistributeExponent(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerDistributeExponent");
  if (!input.StartsWith(theCommands.opThePower(), 3))
    return false;
  const Expression& base = input[1];
  const Expression& exponentE=input[2];
  if (!input[1].StartsWith(theCommands.opTimes(), 3))
    return false;
  if (!base[1].IsConstantNumber())
    return false;
  bool isGood=base[1].IsPositiveNumber() || base[2].IsPositiveNumber();
  if (!isGood)
  { if (exponentE.IsInteger())
      isGood=true;
    else
    { Rational exponentRat;
      if (exponentE.IsRational(&exponentRat))
        if (exponentRat.GetDenominator().IsEven())
          isGood=true;
    }
  }
  if (!isGood)
    return false;
  Expression leftE, rightE;
  leftE.MakeXOX(theCommands, theCommands.opThePower(), input[1][1], input[2]);
  rightE.MakeXOX(theCommands, theCommands.opThePower(), input[1][2], input[2]);
  return output.MakeXOX(theCommands, theCommands.opTimes(), leftE, rightE);
}

bool Calculator::outerPowerRaiseToFirst(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.StartsWith(theCommands.opThePower(), 3))
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

bool Expression::MakeXOXOdotsOX(Calculator& owner, int theOp, const List<Expression>& input)
{ MacroRegisterFunctionWithName("Expression::MakeOXdotsX");
  if (input.size==0)
    crash << "This is a programming error: cannot create operation sequence from an empty list. " << crash;
  if (input.size==1)
  { *this=input[0];
    return true;
  }
  this->MakeXOX(owner, theOp, input[input.size-2], *input.LastObject());
  Expression result;
  result.format=this->formatDefault;
  for (int i=input.size-3; i>=0; i--)
  { result.reset(owner, 3);
    result.AddChildAtomOnTop(theOp);
    result.AddChildOnTop(input[i]);
    result.AddChildOnTop(*this);
    *this=result;
  }
  this->format=this->formatDefault;
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

bool Calculator::outerPlus(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::outerPlus");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.StartsWith(theCommands.opPlus()))
    return false;
//  stOutput << "<hr><hr>DEBUG: processing outer plus with input: <br>" << input.Lispify();
  MonomialCollection<Expression, Rational> theSum;
  theCommands.CollectSummands(theCommands, input, theSum);
//  stOutput << "<br>Debug: collected summands: " << theSum.ToString();
  theSum.QuickSortDescending();
//  stOutput << "<br>Debug: sorted: " << theSum.ToString();
  /*if (theSum.size()==3)
  { if (theSum[0]>theSum[1])
      stOutput << "<br>" << theSum[0].ToString() << "&gt;" << theSum[1].ToString();
    else
      stOutput << "<br>" << theSum[1].ToString() << "&gt;" << theSum[0].ToString();
    if (theSum[0]>theSum[2])
      stOutput << "<br>" << theSum[0].ToString() << "&gt;" << theSum[2].ToString();
    else
      stOutput << "<br>" << theSum[2].ToString() << "&gt;" << theSum[0].ToString();
    if (theSum[1]>theSum[2])
      stOutput << "<br>" << theSum[1].ToString() << "&gt;" << theSum[2].ToString();
    else
      stOutput << "<br>" << theSum[2].ToString() << "&gt;" << theSum[1].ToString();
  }*/
  if (theSum.size()<5)
    for (int i=0; i<theSum.size(); i++)
      for (int j=i; j<theSum.size(); j++)
        if (theSum[i]>theSum[j] && theSum[j]>theSum[i])
          crash << "Faulty comparison: " << theSum[i].ToString() << " and " << theSum[j].ToString()
          << " are mutually greater than one another. " << crash;
  output.MakeSum(theCommands, theSum);
//  stOutput << "<br>to get output:<br>" << output.Lispify();
  return true;
}

bool Calculator::EvaluateIf(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.StartsWith(theCommands.opDefineConditional(), 4))
    return output.MakeError("Error: operation :if = takes three arguments.", theCommands);
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
  if (left.IsRational(&leftRat) && right.IsRational(&rightRat))
  { if (leftRat>rightRat)
      return output.AssignValue(1, theCommands);
    return output.AssignValue(0, theCommands);
  }
  double leftD, rightD;
//  stOutput << "DEBUG: Got to here, comparing " << left.ToString() << " to " << right.ToString();
  if (left.EvaluatesToDouble(&leftD) && right.EvaluatesToDouble(&rightD) )
  { if (leftD>rightD)
      return output.AssignValue(1, theCommands);
    return output.AssignValue(0, theCommands);
  }
  return false;
}

bool Calculator::outerLessThan(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElements(3))
    return false;
  Expression swappedE(theCommands);
  swappedE.AddChildOnTop(input[0]);
  swappedE.AddChildOnTop(input[2]);
  swappedE.AddChildOnTop(input[1]);
  return Calculator::outerGreaterThan(theCommands, swappedE, output);
}

bool Calculator::outerMinus(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!(input.StartsWith(theCommands.opMinus(), 3) || input.StartsWith(theCommands.opMinus(), 2)) )
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
{ MacroRegisterFunctionWithName("Expression::operator+=");
  if (this->owner==0 && other.owner==0)
  { this->theData+=other.theData;
    if (this->theData!=1 && this->theData!=0)
      crash << "Attempting to add non-initialized expressions" << crash;
    return;
  }
  if (other.owner==0)
  { Expression otherCopy;
    otherCopy.AssignValue(other.theData, *this->owner);
    (*this)+=otherCopy;
    return;
  }
  if (this->owner==0)
    this->AssignValue(this->theData, *other.owner);
  if (this->owner!=other.owner)
    crash << "Error: adding expressions with different owners. " << crash;
  Expression resultE;
  resultE.MakeXOX(*this->owner, this->owner->opPlus(), *this, other);
  *this=resultE;
}

void Expression::operator-=(const Expression& other)
{ MacroRegisterFunctionWithName("Expression::operator+=");
  if (this->owner==0 && other.owner==0)
  { this->theData-=other.theData;
    if (this->theData!=1 && this->theData!=0)
      crash << "Attempting to subtract non-initialized expressions" << crash;
    return;
  }
  if (other.owner==0)
  { Expression otherCopy;
    otherCopy.AssignValue(other.theData, *this->owner);
    (*this)-=otherCopy;
    return;
  }
  if (this->owner==0)
    this->AssignValue(this->theData, *other.owner);
  if (this->owner!=other.owner)
    crash << "Error: adding expressions with different owners. " << crash;
  Expression resultE;
  resultE.MakeXOX(*this->owner, this->owner->opMinus(), *this, other);
  *this=resultE;
}

Expression Expression::operator*(const Expression& other)
{ MacroRegisterFunctionWithName("Expression::operator*");
//  stOutput << "going nearby Expression::operator*";
  Expression result;
  result=*this;
  result*=other;
  return result;
}

Expression Expression::operator*(int other)
{ MacroRegisterFunctionWithName("Expression::operator*");
  //stOutput << "going nearby Expression::operator*";
  Expression result;
  if (this->owner==0)
  { //perhaps we should allow the code below for convenience: really
    //hard to judge if the convenience is worth it, or whether it will cause hard-to-detect bugs.
    //Rational resultRat=this->theData;
    //resultRat*=other;
    //if (resultRat.IsSmallInteger(&result.theData))
    //  return result;
    crash << "Multiplying non-initialized expression with data: "
    << this->theData << " by integer " << other << " results in a large integer. This is not allowed. "
    << crash;
  }
  Expression otherE;
  otherE.AssignValue(other, *this->owner);
  result=*this;
  result*=otherE;
  return result;
}

Expression Expression::operator+(const Expression& other)
{ Expression result;
  result=*this;
  result+=other;
  return result;
}

Expression Expression::operator-(const Expression& other)
{ Expression result;
  result=*this;
  result-=other;
  return result;
}

Expression Expression::operator/(const Expression& other)
{ MacroRegisterFunctionWithName("Expression::operator/");
  Expression result;
  result=*this;
  result/=other;
  return result;
}

void Expression::operator/=(const Expression& other)
{ MacroRegisterFunctionWithName("Expression::operator/=");
  if (this->owner==0 && other.owner==0)
  { this->theData/=other.theData;
    if (this->theData!=1 && this->theData!=0)
      crash << "Attempting to divide non-initialized expressions" << crash;
    return;
  }
  if (other.owner==0)
  { Expression otherCopy;
    otherCopy.AssignValue(other.theData, *this->owner);
    (*this)/=otherCopy;
    return;
  }
  if (this->owner==0)
    this->AssignValue(this->theData, *other.owner);
  if (this->owner!=other.owner)
    crash << "Error: dividing expressions with different owners. " << crash;
  Expression resultE;
  resultE.MakeXOX(*this->owner, this->owner->opDivide(), *this, other);
  *this=resultE;
}

void Expression::operator*=(const Expression& other)
{ MacroRegisterFunctionWithName("Expression::operator*=");
//  stOutput << "going nearby Expression::operator*=";
  if (this->owner==0 && other.owner==0)
  { this->theData*=other.theData;
    if (this->theData!=1 && this->theData!=0)
      crash << "Attempting to add non-initialized expressions" << crash;
    return;
  }
  if (other.owner==0)
  { Expression otherCopy;
    otherCopy.AssignValue(other.theData, *this->owner);
    (*this)*=otherCopy;
    return;
  }
  if (this->owner==0)
    this->AssignValue(this->theData, *other.owner);
  if (this->owner!=other.owner)
    crash << "Error: adding expressions with different owners. " << crash;
  Expression resultE;
  resultE.MakeXOX(*this->owner, this->owner->opTimes(), *this, other);
  *this=resultE;
}

bool Expression::operator==(const Expression& other)const
{ if (this->owner!=other.owner)
    return false;
  return this->theData==other.theData && this->children==other.children;
}

bool Expression::IsEqualToMathematically(const Expression& other)const
{ MacroRegisterFunctionWithName("Expression::IsEqualToMathematically");
  //stOutput << "<br>DEBUG: Calling Expression::IsEqualToMathematically with input: " << this->ToString() << " and " << other.ToString();
  if (this->owner==0 && other.owner==0)
    return this->theData==other.theData;
  if (this->owner==0)
    return false;
  if (*this==other)
    return true;
  Rational theRat;
  AlgebraicNumber theAlgebraic;
  if (this->IsOfType<Rational>(&theRat) && other.IsOfType<AlgebraicNumber>(&theAlgebraic))
    return theAlgebraic==theRat;
  if (other.IsOfType<Rational>(&theRat) && this->IsOfType<AlgebraicNumber>(&theAlgebraic))
    return theAlgebraic==theRat;
  double leftD=-1, rightD=-1;
//  stOutput << "<br>DEBUG: step 1: Calling Expression::IsEqualToMathematically with input: " << this->ToString() << " and " << other.ToString();
  if (this->EvaluatesToDouble(&leftD) && other.EvaluatesToDouble(&rightD) )
  { //stOutput << "<br>DEBUG: step 1.1: comparing leftD: " << leftD << " to rightD: " << rightD << ";<br>";
    return leftD==rightD;
  }
  Expression differenceE = *this;
  differenceE-=other;
  Expression differenceEsimplified;
  if (!this->owner->EvaluateExpression(*this->owner, differenceE, differenceEsimplified))
    return false;
 // stOutput << "<br>DEBUG: step 2: Calling Expression::IsEqualToMathematically with input: " << this->ToString() << " and " << other.ToString();
  if (differenceEsimplified.IsEqualToZero())
    return true;
 // stOutput << "<br>DEBUG: step 3: Calling Expression::IsEqualToMathematically with input: " << this->ToString() << " and " << other.ToString();
  if (this->size()!=other.size())
    return false;
  if (this->size()==0)
    return this->theData==other.theData;
  for (int i=0; i<this->size(); i++)
    if (! (*this)[i].IsEqualToMathematically(other[i]))
      return false;
//  stOutput << "<br>DEBUG: step 4: Calling Expression::IsEqualToMathematically with input: " << this->ToString() << " and " << other.ToString();
  return true;
}

SemisimpleLieAlgebra* Expression::GetAmbientSSAlgebraNonConstUseWithCaution()const
{ this->CheckInitialization();
  Expression myContext=this->GetContext();
  int indexSSalg=myContext.ContextGetIndexAmbientSSalg();
  if (indexSSalg==-1)
    return 0;
  return &this->owner->theObjectContainer.theSSLieAlgebras.theValues[indexSSalg];
}

Function& Calculator::GetFunctionHandlerFromNamedRule(const std::string& inputNamedRule)
{ int theIndex= this->namedRules.GetIndex(inputNamedRule);
  if (theIndex==-1)
    crash << "Named rule " << inputNamedRule << " does not exist." << crash;
  if (this->namedRulesLocations[theIndex][0]==0)
    return this->FunctionHandlers[this->namedRulesLocations[theIndex][1]][this->namedRulesLocations[theIndex][2]];
  return this->operationsCompositeHandlers[this->namedRulesLocations[theIndex][1]][this->namedRulesLocations[theIndex][2]];
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
(const std::string& theOpName, Expression::FunctionAddress innerHandler, int leftType, int rightType,
 const std::string& opDescription,
 const std::string& opExample, bool visible, bool experimental,
 const std::string& inputAdditionalIdentifier, const std::string& inputCalculatorIdentifier)
{ int indexOp=this->theAtoms.GetIndex(theOpName);
  if (indexOp==-1)
  { this->theAtoms.AddOnTop(theOpName);
    indexOp=this->theAtoms.size-1;
    this->FunctionHandlers.SetSize(this->theAtoms.size);
    this->FunctionHandlers.LastObject()->SetSize(0);
  }
  Function innerFunction(*this, indexOp, this->FunctionHandlers[indexOp].size, innerHandler, 0, opDescription, opExample, true, visible, experimental, true);
  innerFunction.theArgumentTypes.reset(*this, 2);
  innerFunction.theArgumentTypes.AddChildAtomOnTop(leftType);
  innerFunction.theArgumentTypes.AddChildAtomOnTop(rightType);
  innerFunction.additionalIdentifier=inputAdditionalIdentifier;
  innerFunction.calculatorIdentifier=inputCalculatorIdentifier;
  this->FunctionHandlers[indexOp].Reserve(10);
  this->FunctionHandlers[indexOp].AddOnTop(innerFunction);
  this->RegisterCalculatorFunctionIdentifier(innerFunction, indexOp, 0, this->FunctionHandlers[indexOp].size-1);
}

void Calculator::RegisterCalculatorFunctionIdentifier
(const Function& theFun, int indexOp, int functionType, int theFunIndex)
{ MacroRegisterFunctionWithName("Calculator::RegisterCalculatorFunctionIdentifier");
  if (theFun.calculatorIdentifier=="")
    return;
//  stOutput << "Registering: " << theFun.ToStringShort();
  int namedRuleIndex=this->namedRules.GetIndex(theFun.calculatorIdentifier);
  if (namedRuleIndex==-1)
  { this->namedRules.AddOnTop(theFun.calculatorIdentifier);
    this->namedRulesLocations.SetSize(this->namedRulesLocations.size+1);
    this->namedRulesLocations.LastObject()->SetSize(0);
    namedRuleIndex=this->namedRules.size-1;
  }
  List<int> triple;
  triple.SetSize(3);
  triple[0]=functionType;
  triple[1]=indexOp;
  triple[2]=theFunIndex;
  this->namedRulesLocations[namedRuleIndex]=(triple);
}

void Calculator::AddOperationHandler
(const std::string& theOpName, Expression::FunctionAddress handler, const std::string& opArgumentListIgnoredForTheTimeBeing, const std::string& opDescription,
 const std::string& opExample, bool isInner, bool visible, bool experimental,
 const std::string& inputAdditionalIdentifier, const std::string& inputCalculatorIdentifier)
{ int indexOp=this->theAtoms.GetIndex(theOpName);
  if (indexOp==-1)
  { this->theAtoms.AddOnTop(theOpName);
    indexOp=this->theAtoms.size-1;
    this->FunctionHandlers.SetSize(this->theAtoms.size);
    this->FunctionHandlers.LastObject()->SetSize(0);
  }
  if (opArgumentListIgnoredForTheTimeBeing!="")
    crash << "This section of code is not implemented yet. Crashing to let you know. " << crash;
  Function theFun
  (*this, indexOp, this->FunctionHandlers[indexOp].size, handler, 0, opDescription, opExample,
   isInner, visible, experimental);
  theFun.additionalIdentifier=inputAdditionalIdentifier;
  theFun.calculatorIdentifier=inputCalculatorIdentifier;
  if (theOpName=="*" || theOpName=="+" || theOpName=="/" || theOpName=="\\otimes" || theOpName=="^")
    this->FunctionHandlers[indexOp].Reserve(100);
  else
    this->FunctionHandlers[indexOp].Reserve(10);
  this->FunctionHandlers[indexOp].AddOnTop(theFun);
  this->RegisterCalculatorFunctionIdentifier(theFun, indexOp, 0, this->FunctionHandlers[indexOp].size-1);
}

void Calculator::AddOperationComposite
  (const std::string& theOpName, Expression::FunctionAddress handler,
   const std::string& opArgumentListIgnoredForTheTimeBeing, const std::string& opDescription,
   const std::string& opExample, bool isInner, bool visible, bool experimental,
   const std::string& inputAdditionalIdentifier, const std::string& inputCalculatorIdentifier)
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
  Function theFun
  (*this, theIndex, this->operationsCompositeHandlers[theIndex].size, handler, 0, opDescription,
   opExample, isInner, visible, experimental);
  theFun.additionalIdentifier=inputAdditionalIdentifier;
  theFun.calculatorIdentifier=inputCalculatorIdentifier;
  theFun.flagIsCompositeHandler=true;
  this->operationsCompositeHandlers[theIndex].AddOnTop(theFun);
  this->RegisterCalculatorFunctionIdentifier(theFun, theIndex, 1, this->operationsCompositeHandlers[theIndex].size-1);
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

std::string Function::ToStringShort()const
{ if (this->owner==0)
    return "(non-initialized)";
  std::stringstream out;
  if (this->flagIsCompositeHandler)
    out << "<span style=\"color:#FF0000\">" << this->owner->operationsComposite[this->indexOperation]
    << " </span><span style=\"color:#00FF00\">(composite)</span> (" << this->indexAmongOperationHandlers+1 << " out of "
    << this->owner->operationsCompositeHandlers[this->indexOperation].size << ") ";
  else
    out << "<span style=\"color:#FF0000\">" << this->owner->theAtoms[this->indexOperation]
    << "</span> (" << this->indexAmongOperationHandlers+1 << " out of "
    << this->owner->FunctionHandlers[this->indexOperation].size << "). ";
  return out.str();
}

std::string Function::ToStringSummary()const
{ if (this->owner==0)
    return "(non-initialized)";
  std::stringstream out;
  out << this->ToStringShort();
  if (this->calculatorIdentifier!="")
    out << "Rule name: <span style='color:blue'>" << this->calculatorIdentifier << "</span>. ";
  if (this->additionalIdentifier!="")
    out << "Handler: " << this->additionalIdentifier << ". ";
  return out.str();
}

std::string Function::ToStringFull()const
{ if (!this->flagIamVisible)
    return "";
  if (this->owner==0)
    return "(non-intialized)";
  std::stringstream out2;
  out2 << this->ToStringShort();
  if (!this->flagIsExperimental)
  { std::stringstream out;
    out << this->theDescription;
//    out << " \nFunction memory address: " << std::hex << (int) this->theFunction << ". ";
    // use of unsigned long is correct on i386 and amd64
    // uintptr_t is only available in c++0x
    if (this->calculatorIdentifier!="")
      out << "Rule name: " << this->calculatorIdentifier << ". ";
    if (this->additionalIdentifier!="")
      out << "Handler: " << this->additionalIdentifier << ". ";
    out << "Function memory address: " << std::hex << (unsigned long) this->theFunction << ". ";
    if (!this->flagIsInner)
      out << "This is a <b>``law''</b> - substitution takes place only if output expression is different from input. ";
    if (this->theExample!="")
      out << " <br> " << this->theExample << "&nbsp&nbsp&nbsp";
    out2 << CGI::GetHtmlSpanHidableStartsHiddeN(out.str());
    if (this->theExample!="")
      out2 << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
      << "?request=calculator&showExamples=true&mainInput="
      << CGI::StringToURLString(this->theExample, false)
      << "\"> " << " Example" << "</a>" ;
  } else
    out2 << "<b>Experimental, please don't use.</b>";
  return out2.str();
}

std::string ObjectContainer::ToString()
{ MacroRegisterFunctionWithName("ObjectContainer::ToString");
  std::stringstream out;
  if (this->theSSLieAlgebras.theValues.size>0)
  { out << "Lie algebras created (" << this->theSSLieAlgebras.theValues.size << " total): ";
    for (int i=0; i<this->theSSLieAlgebras.theValues.size; i++)
    { out << this->theSSLieAlgebras.theValues[i].GetLieAlgebraName();
      if (i!=this->theSSLieAlgebras.theValues.size-1)
        out << ", ";
    }
  }
  if (this->theSSSubalgebraS.theValues.size>0)
  { out << "<br>Lie semisimple subalgebras computation data structures (" << this->theSSSubalgebraS.theValues.size << " total): ";
    for (int i=0; i<this->theSSSubalgebraS.theValues.size; i++)
    { out << " Type " << this->theSSSubalgebraS.theValues[i].owner->GetLieAlgebraName() << " with "
      << this->theSSSubalgebraS.theValues[i].theSubalgebras.theValues.size << " candidates";
      if (i!=this->theSSSubalgebraS.theValues.size-1)
        out << ", ";
    }
  }
  return out.str();
}

std::string Calculator::ToStringOutputAndSpecials()
{ MacroRegisterFunctionWithName("Calculator::ToStringOutputSpecials");
  if (this->inputString=="")
    return "";
  std::stringstream out;
  std::string urledInput=CGI::StringToURLString(this->inputString, false);
  if (this->inputString!="")
    out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=calculator&mainInput="
    << urledInput << "\">Link to your input (reloads page).</a><br>";
  out <<  "<table style=\"vertical-align: top\"><tr><td style=\"vertical-align: top\">"
  << this->outputString << "</td>"
  << "<td style=\"vertical-align: top\">"
  ;

  out << this->ToStringPerformance();
  if(this->outputCommentsString!="")
    out << "<br><b>Comments.</b><br>" << this->outputCommentsString;
  out << "</td></tr></table>";
  if (this->flagProduceLatexLink)
    out << "<br>LaTeX link (\\usepackage{hyperref}):<br> "
    << CGI::GetLatexEmbeddableLinkFromCalculatorInput(urledInput, this->inputString)
    ;
  if (this->parsingLog!="")
    out << "<b> As requested, here is a calculator parsing log</b><br>"
    << this->parsingLog;
  return out.str();
}

void Calculator::ComputeAutoCompleteKeyWords()
{ MacroRegisterFunctionWithName("Calculator::ComputeAutoCompleteKeyWords");
  this->autoCompleteKeyWords.SetExpectedSize(this->theAtoms.size*2);
  for (int i=0; i<this->theAtoms.size; i++)
    this->autoCompleteKeyWords.AddOnTopNoRepetition(this->theAtoms[i]);
  for (int i=0; i<this->namedRules.size; i++)
    this->autoCompleteKeyWords.AddOnTopNoRepetition(this->namedRules[i]);
  autoCompleteKeyWords.AddOnTopNoRepetition("NoFrac");
  autoCompleteKeyWords.AddOnTopNoRepetition("NoApproximations");
  autoCompleteKeyWords.AddOnTopNoRepetition("ShowContext");
  autoCompleteKeyWords.AddOnTopNoRepetition("LogParsing");
  autoCompleteKeyWords.AddOnTopNoRepetition("LogEvaluation");
  autoCompleteKeyWords.AddOnTopNoRepetition("NumberColors");
  autoCompleteKeyWords.AddOnTopNoRepetition("LogRules");
  autoCompleteKeyWords.AddOnTopNoRepetition("LogCache");
  autoCompleteKeyWords.AddOnTopNoRepetition("LogFull");
  autoCompleteKeyWords.AddOnTopNoRepetition("LatexLink");
  autoCompleteKeyWords.AddOnTopNoRepetition("UseLnInsteadOfLog");
  autoCompleteKeyWords.AddOnTopNoRepetition("UseLnAbsInsteadOfLog");
  autoCompleteKeyWords.AddOnTopNoRepetition("CalculatorStatus");
  autoCompleteKeyWords.AddOnTopNoRepetition("FullTree");
  autoCompleteKeyWords.AddOnTopNoRepetition("HideLHS");
  autoCompleteKeyWords.AddOnTopNoRepetition("DontUsePredefinedWordSplits");
  autoCompleteKeyWords.AddOnTopNoRepetition("PlotShowJavascriptOnly");
  autoCompleteKeyWords.AddOnTopNoRepetition("PlotNoCoordinateDetails");

}

std::string Calculator::ToStringPerformance()
{ MacroRegisterFunctionWithName("Calculator::ToStringPerformance");
  std::stringstream out;
  double elapsedSecs=theGlobalVariables.GetElapsedSeconds();
  out << "<br>Time: computation: " << (elapsedSecs-this->StartTimeEvaluationInSecondS)
  << " (" << (elapsedSecs-this->StartTimeEvaluationInSecondS)*1000
  << " ms), total: " << elapsedSecs << " ("
  << ((elapsedSecs)*1000) << " ms).";
  out << "<br>";
  std::stringstream moreDetails;
  moreDetails << "Total number of pattern matches performed: "
  << this->TotalNumPatternMatchedPerformed << "";
  if (this->DepthRecursionReached>0)
    moreDetails << "<br>Maximum recursion depth reached: " << this->DepthRecursionReached << ".";
  #ifdef MacroIncrementCounter
  moreDetails << "<br>Lists created: " << "computation: "
  << (ParallelComputing::NumListsCreated-this->NumListsStart)
  << ", total: " << ParallelComputing::NumListsCreated;
  moreDetails << "<br> # List resizes: computation: "
  << (ParallelComputing::NumListResizesTotal -this->NumListResizesStart)
  << ", total: " << ParallelComputing::NumListResizesTotal
  << "<br> # hash resizing: computation: " << (ParallelComputing::NumHashResizes- this->NumHashResizesStart)
  << ", total: " << ParallelComputing::NumHashResizes;
  if (Rational::TotalSmallAdditions>0)
    moreDetails << "<br>Small rational additions: computation: "
    << Rational::TotalSmallAdditions- this->NumSmallAdditionsStart
    << ", total: " << Rational::TotalSmallAdditions;
  if (Rational::TotalSmallMultiplications>0)
    moreDetails << "<br>Small rational multiplications: computation: "
    << Rational::TotalSmallMultiplications - this->NumSmallMultiplicationsStart
    << ", total: " << Rational::TotalSmallMultiplications;
  if (Rational::TotalSmallGCDcalls>0)
    moreDetails << "<br>Small gcd calls: computation: "
    << Rational::TotalSmallGCDcalls - this->NumSmallGCDcallsStart
    << ", total: " << Rational::TotalSmallGCDcalls;
  if (Rational::TotalLargeAdditions>0)
    moreDetails << "<br>Large integer additions: "
    << Rational::TotalLargeAdditions - this->NumLargeAdditionsStart
    << ", total: "
    << Rational::TotalLargeAdditions;
  if (Rational::TotalLargeMultiplications>0)
    moreDetails << "<br>Large integer multiplications: computation: "
    << Rational::TotalLargeMultiplications - this->NumLargeMultiplicationsStart
    << ", total: " << Rational::TotalLargeMultiplications;
  if (Rational::TotalLargeGCDcalls>0)
    moreDetails << "<br>Large gcd calls: "
    << Rational::TotalLargeGCDcalls - this->NumLargeGCDcallsStart
    << ", total: " << Rational::TotalLargeGCDcalls;
  out << CGI::GetHtmlSpanHidableStartsHiddeN(moreDetails.str(), "More details");
  #endif
  return out.str();
}

std::string Calculator::ToString()
{ MacroRegisterFunctionWithName("Calculator::ToString");
  std::stringstream out2;
  std::string openTag1="<span style=\"color:#0000FF\">";
  std::string closeTag1="</span>";
  if (this->RuleStack.children.size>1)
  { out2 << "<b>Predefined rules.</b><br>";
    for (int i=1; i<this->RuleStack.children.size; i++)
    { out2 << this->RuleStack[i].ToString();
      if (i!=this->RuleStack.children.size-1)
        out2 << "<br>";
    }
  }
  if (this->flagShowCalculatorExamples)
    out2 << this->ToStringFunctionHandlers();
  else
  { std::stringstream theExampleInjector;
    theExampleInjector
    << "if (document.getElementById('calculatorExamples').innerHTML.length<300)"
    << "  InjectCalculatorResponse('calculatorExamples', '', 'calculatorExamples'); "
//    << "else alert('innerHTML: '+ document.getElementById('calculatorExamples').innerHTML); "
    << "switchMenu('calculatorExamples')"
    ;
    out2 << CGI::GetHtmlButton("ShowCalculatorExamplesButton", theExampleInjector.str(), "Examples.");
    out2 << "<span style=\"display:none\" id=\"calculatorExamples\"></span>";
  }
  if (!this->flagShowCalculatorInternalStatus)
    return out2.str();
  std::stringstream out;
  out2 << "<hr><b>Further calculator details.</b>";
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
  out << "Children expressions (" << this->theExpressionContainer.size << " total): <br>";
  int numExpressionsToDisplay=this->theExpressionContainer.size;
  if (this->theExpressionContainer.size>1000)
  { numExpressionsToDisplay=1000;
    out << " <b>Displaying first " << numExpressionsToDisplay << " only </b><br>";
  }
  for (int i=0; i< numExpressionsToDisplay; i++)
    out << this->theExpressionContainer[i].ToString() << ", ";
  out << "<hr>";
  out << "\n Cached expressions (" << this->cachedExpressions.size << " total):\n<br>\n";
  numExpressionsToDisplay=this->cachedExpressions.size;
  if (numExpressionsToDisplay>1000)
  { numExpressionsToDisplay=1000;
    out << "<b>Displaying first " << numExpressionsToDisplay << " expressions only. </b><br>";
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
  newExpr.children.Reserve(2);
  newExpr.AddChildAtomOnTop(this->opEndStatement());
  newExpr.AddChildOnTop(left.theData);
  left.theData=newExpr;
  left.controlIndex=this->conSequenceStatements();
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceSsSsXdotsXbySsXdotsX(int numDots)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numDots-2];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numDots-1];
  if (!left.theData.StartsWith(this->opEndStatement()))
    crash << "This is a programming error: ReplaceSsSsXdotsXbySsXdotsX called but left expression is not EndStatement." << crash;
  left.theData.children.Reserve(left.theData.children.size+ right.theData.children.size-1);
  for (int i=1; i<right.theData.children.size; i++)
    left.theData.AddChildOnTop(right.theData[i]);
  left.theData.format=Expression::formatDefault;
  left.controlIndex=this->conSequenceStatements();
  (*this->CurrentSyntacticStacK).PopIndexShiftDown((*this->CurrentSyntacticStacK).size-numDots-1);
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
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
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
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
//    stOutput << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

std::string Calculator::ToStringSyntacticStackHumanReadable(bool includeLispifiedExpressions)
{ MacroRegisterFunctionWithName("Calculator::ToStringSyntacticStackHumanReadable");
  std::stringstream out;
  if ((*this->CurrentSyntacticStacK).size<this->numEmptyTokensStart)
    return "Error: this is a programming error: not enough empty tokens in the start of the syntactic stack.";
  bool isBad=((*this->CurrentSyntacticStacK).size>this->numEmptyTokensStart+1);
  SyntacticElement& lastSyntacticElt=* (*this->CurrentSyntacticStacK).LastObject();
  if ((*this->CurrentSyntacticStacK).size==this->numEmptyTokensStart+1)
    if (lastSyntacticElt.controlIndex!=this->conExpression())
      isBad=true;
  if (!isBad)
  { out << this->CurrentSyntacticStacK->LastObject()->ToStringHumanReadable(*this, includeLispifiedExpressions);
    return out.str();
  }
  out << "<table style=\"vertical-align:top;border-spacing:0px 0px;\"><tr>";
  for (int i=this->numEmptyTokensStart; i<(*this->CurrentSyntacticStacK).size; i++)
    out
    << "<td style=\"vertical-align:top;background-color:" << ((i%2==0) ?"#FAFAFA" : "#F0F0F0" ) << "\">"
    << (*this->CurrentSyntacticStacK)[i].ToStringHumanReadable(*this, includeLispifiedExpressions)
    << "</td>";
  out << "</tr></table>";
  return out.str();
}

std::string Calculator::ToStringSyntacticStackHTMLTable()
{ return this->ToStringSyntacticStackHumanReadable(true);
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

bool Calculator::ReplaceXXXXXByCon(int theCon)
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

SemisimpleSubalgebras& ObjectContainer::GetSemisimpleSubalgebrasCreateIfNotPresent(const DynkinType& input)
{ MacroRegisterFunctionWithName("ObjectContainer::GetSemisimpleSubalgebrasCreateIfNotPresent");
  bool needToHookUpPointers=false;
  if (!this->theSSSubalgebraS.Contains(input))
    needToHookUpPointers=true;
  SemisimpleSubalgebras& currentSAs=this->theSSSubalgebraS.GetValueCreateIfNotPresent(input);
  if (needToHookUpPointers)
  { SemisimpleLieAlgebra& ownerSS=this->GetLieAlgebraCreateIfNotPresent(input);
    currentSAs.initHookUpPointers(ownerSS, &this->theAlgebraicClosure, &this->theSSLieAlgebras, &this->theSltwoSAs);
  }
  return currentSAs;
}

SemisimpleLieAlgebra& ObjectContainer::GetLieAlgebraCreateIfNotPresent(const DynkinType& input)
{ MacroRegisterFunctionWithName("ObjectContainer::GetLieAlgebraCreateIfNotPresent");
  bool needToInit=false;
  //  stOutput << "got to here pt1<br>";
  if (!this->theSSLieAlgebras.Contains(input))
    needToInit=true;
  //  stOutput << "got to here pt2<br>";
  SemisimpleLieAlgebra& theLA=this->theSSLieAlgebras.GetValueCreateIfNotPresent(input);
  //  stOutput << "got to here pt3<br>";
  if (needToInit)
    theLA.theWeyl.MakeFromDynkinType(input);
  //  stOutput << "got to here pt4<br>";
  return theLA;
}

WeylGroupData& ObjectContainer::GetWeylGroupDataCreateIfNotPresent(const DynkinType& input)
{ MacroRegisterFunctionWithName("ObjectContainer::GetWeylGroupDataCreateIfNotPresent");
  return this->GetLieAlgebraCreateIfNotPresent(input).theWeyl;
}

std::string ObjectContainer::ToStringJavascriptForUserInputBoxes()
{ std::stringstream out;
  out << "<script>\n";
  out << "calculatorInputBoxNames=[";
  for (int i=0; i<this->theUserInputTextBoxesWithValues.size(); i++)
  { InputBox& currentBox=this->theUserInputTextBoxesWithValues.theValues[i];
    out << "'" << currentBox.name << "'";
    if (i!=this->theUserInputTextBoxesWithValues.size()-1)
      out << ", ";
  }
  out << "];\n";
  out << "calculatorInputBoxToSliderUpdaters= new Object;";
  for (int i=0; i<this->theUserInputTextBoxesWithValues.size(); i++)
  { InputBox& currentBox=this->theUserInputTextBoxesWithValues.theValues[i];
    out << "calculatorInputBoxToSliderUpdaters['"
    << currentBox.name << "']='"
    << currentBox.GetSliderName() << "';\n";
  }
  out << "</script>";
  return out.str();
}

void ObjectContainer::resetSliders()
{ this->userInputBoxSliderDisplayed.initFillInObject(this->theUserInputTextBoxesWithValues.size(), false);
}

void ObjectContainer::reset()
{ MacroRegisterFunctionWithName("ObjectContainer::reset");
  this->theWeylGroupElements.Clear();
  this->theWeylGroupReps.Clear();
  this->theWeylGroupVirtualReps.Clear();
  this->theCategoryOmodules.SetSize(0);
  this->theSSLieAlgebras.Clear();
  this->theSSSubalgebraS.Clear();
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
  this->theMatDoubles.SetSize(0);
  this->theMatsAlgebraic.Clear();
  this->theMatPolyRational.Clear();
  this->theWeights.Clear();
  this->theWeightsPoly.Clear();
  this->theHyperOctahedralGroups.SetSize(0);
  this->theElementsHyperOctGroup.Clear();
  this->CurrentRandomSeed=  time(NULL);
  this->theUserInputTextBoxesWithValues.Clear();
   //Setting up a random seed.
  srand (this->CurrentRandomSeed);
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

bool Calculator::innerWriteGenVermaModAsDiffOperators
(Calculator& theCommands, const Expression& input, Expression& output, bool AllGenerators,
 bool useNilWeight, bool ascending)
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
      (theCommands, truncatedInput, output, theHWs[0], theParSel, theContext, theSSalgebra, CalculatorConversions::innerPolynomial<Rational>))
    return output.MakeError("Failed to extract type, highest weight, parabolic selection", theCommands);
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

//stOutput << "<br>theContext:" << theContext.ToString();
//stOutput << ", numvars: " << theContext.ContextGetNumContextVariables();
//  FormatExpressions theFormat;
//  theContext.ContextGetFormatExpressions(theFormat);
//  stOutput << "highest weights you are asking me for: " << theHws.ToString(&theFormat);
  return theCommands.innerWriteGenVermaModAsDiffOperatorInner
  (theCommands, input, output, theHWs, theContext, theParSel, theSSalgebra, AllGenerators, &letterString,
   &partialString, &exponentLetterString, useNilWeight, ascending);
}

bool Calculator::innerFreudenthalFull(Calculator& theCommands, const Expression& input, Expression& output)
{ Vector<Rational> hwFundamental, hwSimple;
  Selection tempSel;
  SemisimpleLieAlgebra* theSSalg;
  Expression context;
  if (!theCommands.GetTypeHighestWeightParabolic<Rational>(theCommands, input, output, hwFundamental, tempSel, context, theSSalg, 0))
    return output.MakeError("Failed to extract highest weight and algebra", theCommands);
  if (output.IsError())
    return true;
  if (tempSel.CardinalitySelection>0)
    return output.MakeError("Failed to extract highest weight. ", theCommands);
  charSSAlgMod<Rational> startingChar, resultChar ;
  hwSimple=theSSalg->theWeyl.GetSimpleCoordinatesFromFundamental(hwFundamental);
  startingChar.MakeFromWeight(hwSimple, theSSalg);
  std::string reportString;
  if (!startingChar.FreudenthalEvalMeFullCharacter(resultChar, 10000, &reportString))
    return output.MakeError(reportString, theCommands);
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
    return output.MakeError("Failed to extract highest weight and algebra", theCommands);
  if (output.IsError())
    return true;
  if (tempSel.CardinalitySelection>0)
    return output.MakeError("Failed to extract highest weight. ", theCommands);
  charSSAlgMod<Rational> startingChar, resultChar ;
  hwSimple=theSSalg->theWeyl.GetSimpleCoordinatesFromFundamental(hwFundamental);
  startingChar.MakeFromWeight(hwSimple, theSSalg);
  std::string reportString;
  if (!startingChar.FreudenthalEvalMeDominantWeightsOnly(resultChar, 10000, &reportString))
    return output.MakeError(reportString, theCommands);
  return output.AssignValue(resultChar, theCommands);
}

bool Expression::IsMeltable(int* numResultingChildren)const
{ this->CheckInitialization();
  if (!this->StartsWith(this->owner->opMelt(), 2))
    return false;
  if (numResultingChildren!=0)
  { if (!(*this)[1].StartsWith(this->owner->opEndStatement()))
      *numResultingChildren=1;
    else
      *numResultingChildren=(*this)[1].children.size-1;
  }
  return true;
}

bool Expression::MergeContextsMyAruments(Expression& output)const
{ MacroRegisterFunctionWithName("Expression::MergeContextsMyAruments");
//  stOutput << "<hr>Merging contexts of expression " << this->ToString();
  this->CheckInitialization();
  if (this->children.size<2)
    return false;
//  stOutput << " ... continuing to merge..." ;
  for (int i=1; i< this->children.size; i++)
    if (!(*this)[i].IsBuiltInType())
    { *this->owner << "<hr>Failed to merge the arguments of the expression" << this->ToString() << ": the argument "
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
//    stOutput << "needs merge!";
//  else
//    stOutput << "no need merge no nothin";
  if (!needsMerge)
  { output=*this;
    return true;
  }
  for (int i=2; i<this->children.size; i++)
  { if (!(*this)[i].IsBuiltInType())
    { *this->owner << "<hr>Failed to merge contexts of arguments: an argument is not of built-in type";
      return false;
    }
//    stOutput << "<br>Merging context " << commonContext.ToString() << " with " << (*this)[i].GetContext().ToString();
    if (!commonContext.ContextMergeContexts(commonContext, (*this)[i].GetContext(), commonContext))
    { *this->owner << "<hr>Failed to merge context " << commonContext.ToString() << " with " << (*this)[i].GetContext().ToString();
      return false;
    }
//    stOutput << " ...  to get context: " << commonContext.ToString();
  }
  output.reset(*this->owner, this->children.size);
  output.AddChildOnTop((*this)[0]);
  Expression convertedE;
  for (int i=1; i<this->children.size; i++)
  { convertedE=(*this)[i];
    //stOutput << "<hr>Setting context of " << convertedE.ToString() << " to be the context " << commonContext.ToString();
    if (!convertedE.SetContextAtLeastEqualTo(commonContext))
    { *this->owner << "<hr>Failed to convert " << convertedE.ToString() << " to context " << commonContext.ToString();
      return false;
    }
    //stOutput << "... and the result is: " << convertedE.ToString();
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
      return *this << "<hr>Possible programming error: calling ConvertExpressionsToCommonContext on expressions without context. "
      << Crasher::GetStackTraceEtcErrorMessage();
    if (!commonContext.ContextMergeContexts(commonContext, inputOutputEs[i].GetContext(), commonContext))
      return *this << "<hr>Failed to merge context " << commonContext.ToString() << " with " << inputOutputEs[i].GetContext().ToString();
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
  //stOutput << "outerMeltBrackets meldet sich!";
  if (!input.StartsWith(theCommands.opEndStatement()))
    return false;
  //stOutput << "<br>outerMeltBrackets meldet sich!";
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
//    stOutput << "<br>not found!";
  if (!found)
    return false;
//  stOutput << "<br>ChildIncrease: " << ChildIncrease;
  output.reset(theCommands, input.children.size+ChildIncrease);
  output.AddChildAtomOnTop(theCommands.opEndStatement());
  for (int i=1; i<input.children.size; i++)
  { const Expression& currentChild=input[i];
    if (!currentChild.IsMeltable())
    { //output.SetChild(i+shift, input.children[i]);
      output.AddChildOnTop(input[i]);
      continue;
    }
//    stOutput << "<br>shift:" << shift;
    if (!currentChild[1].StartsWith(theCommands.opEndStatement()))
    { //output.SetChild(i+shift, currentChild.children[1]);
      output.AddChildOnTop(currentChild[1]);
      continue;
    }
    for (int j=1; j<currentChild[1].children.size; j++)
    { //output.SetChild(i+shift, currentChild[1].children[j]);
      output.AddChildOnTop(currentChild[1][j]);
    }
  }
//  stOutput << output.ToString();

  return true;
}
