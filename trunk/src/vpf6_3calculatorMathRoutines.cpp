//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
#include "vpfHeader3Calculator2_InnerFunctions.h"
#include "vpfImplementationHeader2Math3_WeylAlgebra.h"
#include "vpfImplementationHeader2Math051_PolynomialComputations_Basic.h"
#include "vpfImplementationHeader2Math15_UniversalEnveloping.h"
#include "vpfImplementationHeader2Math3_FiniteGroups.h"
#include "vpfImplementationHeader2Math6_ModulesSSLieAlgebras.h"
#include "vpfHeader5Crypto.h"
#include "vpfImplementationHeader2Math1_SemisimpleLieAlgebras.h" // undefined reference to `charSSAlgMod<RationalFunctionOld>::SplitCharOverRedSubalg(std::string*, charSSAlgMod<RationalFunctionOld>&, branchingData&, GlobalVariables&)'

ProjectInformationInstance ProjectInfoVpf6_3cpp(__FILE__, "Calculator built-in functions. ");

template <class theType>
bool MathRoutines::GenerateVectorSpaceClosedWRTOperation
(List<theType>& inputOutputElts, int upperDimensionBound, void (*theBinaryOperation)(const theType& left, const theType& right, theType& output))
{ MacroRegisterFunctionWithName("MathRoutines::GenerateVectorSpaceClosedWRTOperation");
  inputOutputElts[0].GaussianEliminationByRowsDeleteZeroRows(inputOutputElts);
  theType theOpResult;
  ProgressReport theReport1, theReport2;
  bool flagDoReport=theGlobalVariables.flagReportEverything|| theGlobalVariables.flagReportGaussianElimination;
  if (flagDoReport)
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
        //stOutput << "<hr>Operation between <br>" << inputOutputElts[i].ToString() << " <br> " << inputOutputElts[j].ToString() << " <br>=<br> "
        //<< theOpResult.ToString();
      if (upperDimensionBound>0 && inputOutputElts.size>upperDimensionBound)
        return false;
      if (flagDoReport)
      { std::stringstream reportStream;
        reportStream << "Accounted operation between elements " << i+1 << " and " << j+1 << " out of " << inputOutputElts.size;
        theReport2.Report(reportStream.str());
      }
    }
  inputOutputElts[0].GaussianEliminationByRowsDeleteZeroRows(inputOutputElts);
  return true;
}

bool CalculatorFunctionsGeneral::innerConstructCartanSA(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerConstructCartanSA");
  SubalgebraSemisimpleLieAlgebra theSA;
  ElementSemisimpleLieAlgebra<AlgebraicNumber> theElt;
  if (input.ConvertsToType(&theElt))
    theSA.theGenerators.AddOnTop(theElt);
  else
    for (int i=1; i<input.children.size; i++)
      if (input[i].ConvertsToType(&theElt))
        theSA.theGenerators.AddOnTop(theElt);
      else
        return theCommands << "Failed to extract element of a semisimple Lie algebra from " << input[i].ToString();
  for (int i=0; i<theSA.theGenerators.size; i++)
    if (!theSA.theGenerators[i].IsEqualToZero())
    { if (theSA.owner!=0)
        if (theSA.owner!=theSA.theGenerators[i].GetOwner())
          return theCommands << "The input elements in " << input.ToString()
          << " belong to different semisimple Lie algebras";
      theSA.owner=theSA.theGenerators[i].GetOwner();
    }
  if (theSA.owner==0)
    return theCommands << "Failed to extract input semisimple Lie algebra elements from the inputs of " << input.ToString();
  theSA.theGlobalVariables=&theGlobalVariables;
  theSA.ComputeBasis();
  theSA.ComputeCartanSA();
  return output.AssignValue(theSA.ToString(), theCommands);
}

bool CalculatorFunctionsGeneral::innerGenerateVectorSpaceClosedWRTLieBracket(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGenerateVectorSpaceClosedWRTLieBracket");
  Vector<ElementWeylAlgebra<Rational> > theOps;
  Expression theContext;
  if (!(input.children.size>1))
    return false;
  int upperBound=-1;
  if (!input[1].IsSmallInteger(&upperBound))
    return theCommands << "<hr>Failed to extract upper bound for the vector space dimension from the first argument. ";
  Expression inputModded=input;
  inputModded.children.RemoveIndexShiftDown(1);

  if (!theCommands.GetVectorFromFunctionArguments(inputModded, theOps, &theContext))
  { Vector<ElementUniversalEnveloping<RationalFunctionOld> > theLieAlgElts;
    theContext.MakeEmptyContext(theCommands);
    if (!theCommands.GetVectorFromFunctionArguments(inputModded, theLieAlgElts, &theContext))
      return theCommands << "<hr>Failed to extract elements of weyl algebra and failed to extract elements of UE algebra from input "
      << input.ToString();
    FormatExpressions theFormat;
    theContext.ContextGetFormatExpressions(theFormat);
    std::stringstream out;
    out << "Starting elements: <br>";
    for (int i=0; i<theLieAlgElts.size; i++)
      out << CGI::GetMathSpanPure(theLieAlgElts[i].ToString(&theFormat)) << "<br>";
    bool success=MathRoutines::GenerateVectorSpaceClosedWRTLieBracket(theLieAlgElts, upperBound);
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
  bool success=MathRoutines::GenerateVectorSpaceClosedWRTLieBracket(theOps, upperBound);
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

bool CalculatorFunctionsGeneral::innerX509certificateCrunch(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerX509certificateCrunch");
  std::string theCertificateFileNameNoFolder;
  if (!input.IsOfType<std::string>(&theCertificateFileNameNoFolder))
    return false;
  if (!FileOperations::IsFileNameWithoutDotsAndSlashes(theCertificateFileNameNoFolder))
    return theCommands << "The file name contains forbidden characters, computation aborted. ";
  std::string theCertificateFileName=theCertificateFileNameNoFolder;
  std::fstream theCertFile;
  if (!FileOperations::OpenFileVirtual(theCertFile, "output/"+ theCertificateFileName, false, false, false))
    return theCommands << "Failed to open file " << theCertificateFileName;
  theCertFile.seekg(0);
  List<std::string> theCerts, theShas, certsAndShas;
  List<List<unsigned char> > theCertsRAWuchars;
  List<std::string> theCertsRAWstrings;
  const int sampleSize=100;
  theCerts.SetSize(sampleSize);
  theShas.SetSize(sampleSize);
  certsAndShas.SetSize(sampleSize);
  theCertsRAWuchars.SetSize(sampleSize);
  theCertsRAWstrings.SetSize(sampleSize);
  std::stringstream out;
  for (int i=0; i<sampleSize; i++)
  { theCertFile >> certsAndShas[i];
    unsigned commaPosition=0;
    for (;commaPosition< certsAndShas[i].size(); commaPosition++)
      if (certsAndShas[i][commaPosition]==',')
        break;
    MathRoutines::SplitStringInTwo(certsAndShas[i], commaPosition+1, theShas[i], theCerts[i]);
    if (theShas[i].size()>0)
      theShas[i].resize(theShas[i].size()-1);
    out << "Raw cert+sha:<br>" << certsAndShas[i] << "<br>Certificate " << i+1
    << " (base64):<br>" << theCerts[i] << "<br>Sha1:<br>" << theShas[i]
    << "<br>Comments while extracting the raw certificate: ";
    Crypto::StringBase64ToBitStream(theCerts[i], theCertsRAWuchars[i], &out);
    Crypto::ConvertBitStreamToString(theCertsRAWuchars[i], theCertsRAWstrings[i]);
    out << "<br>Raw certificate: " << theCertsRAWstrings[i];
//    Crypto::GetUInt32FromCharBigendian(theCertsRAW[i], )
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerSha1OfString(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSha1OfString");
  if (!input.IsOfType<std::string>())
    return false;
  List<unsigned char> theBitStream;
  const std::string& inputString=input.GetValue<std::string>();
  theBitStream.SetSize(inputString.size());
  for (unsigned i =0; i<inputString.size(); i++)
    theBitStream[i]=inputString[i];
  std::stringstream out;
  out << "<br>Input: " << inputString;
  out << "<br>Base64 conversion: " << Crypto::CharsToBase64String(theBitStream);
  List<uint32_t> theSha1Uint;
  List<unsigned char> theSha1Uchar;
  std::string theSha1base64string;
  Crypto::computeSha1(inputString, theSha1Uint);
  Crypto::ConvertUint32ToUcharBigendian(theSha1Uint, theSha1Uchar);
  theSha1base64string=Crypto::CharsToBase64String(theSha1Uchar);
  out << "<br>Sha1 in base64: " << theSha1base64string;
  out << "<br>Sha1 hex: ";
  for (int i=0; i<theSha1Uint.size; i++)
    out << std::hex << theSha1Uint[i];
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerCharToBase64(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCharToBase64");
  if (!input.IsOfType<std::string>())
    return false;
  List<unsigned char> theBitStream;
  const std::string& inputString=input.GetValue<std::string>();
  theBitStream.SetSize(inputString.size());
  for (unsigned i =0; i<inputString.size(); i++)
    theBitStream[i]=inputString[i];
  return output.AssignValue(Crypto::CharsToBase64String(theBitStream), theCommands);
}

bool CalculatorFunctionsGeneral::innerBase64ToCharToBase64Test(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerBase64ToCharSequence");
  if (!input.IsOfType<std::string>())
    return false;
  List<unsigned char> theBitStream;
  if (!Crypto::StringBase64ToBitStream(input.GetValue<std::string>(), theBitStream, &theCommands.Comments))
    return false;
  std::stringstream out;
  std::string theConvertedBack=Crypto::CharsToBase64String(theBitStream);
  out << "Original string: " << input.GetValue<std::string>()
  << "<br>Converted to bitstream and back: " << theConvertedBack;
  if (theConvertedBack!=input.GetValue<std::string>())
    out << "<br><b>The input is not the same as the output!</b>";
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerURLKeyValuePairsToNormalRecursive(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerURLKeyValuePairsToNormalRecursive");
  std::string theString;
  if (!input.IsOfType<std::string>(&theString))
    return false;
  return output.AssignValue(CGI::URLKeyValuePairsToNormalRecursiveHtml(theString), theCommands);
}

bool CalculatorFunctionsGeneral::innerUrlStringToNormalString(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerUrlStringToNormalString");
  std::string theString;
  if (!input.IsOfType<std::string>(&theString))
    return false;
  return output.AssignValue(CGI::URLStringToNormal(theString), theCommands);
}

bool CalculatorFunctionsGeneral::innerExpressionToString(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerExpressionToString");
  return output.AssignValue(input.ToString(), theCommands);
}

bool CalculatorFunctionsGeneral::innerQuoteToString(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerQuoteToString");
  std::string atomString;
  if (input.IsAtom(&atomString))
    return output.AssignValue(atomString, theCommands);
  theCommands << "<b>Warning: this shouldn't happen: quote operation is applied to the non-atomic expression: "
  << input.ToString() << "."
  << " This may be a bug with the function Calculator::ParseFillDictionary. </b>";
  return output.AssignValue(input.ToString(), theCommands);
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
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, input[1], theSSalg))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  if (theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit<50)
    theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=50;
  Vector<Rational> leviSelection;
  if(!theCommands.GetVectoR(input[2], leviSelection, 0, theSSalg->GetRank()))
    return theCommands << "<hr>Failed to extract parabolic selection. ";
  Selection theParSel;
  theParSel=leviSelection;
  theParSel.InvertSelection();
  ElementUniversalEnveloping<RationalFunctionOld> theCasimir;
  theCasimir.MakeCasimirWRTLeviParabolic(*theSSalg, theParSel);
//  theCasimir.Simplify(theGlobalVariables);
  Expression contextE;
  contextE.MakeContextSSLieAlg(theCommands, *theSSalg);
  return output.AssignValueWithContext(theCasimir, contextE, theCommands);
}

bool CalculatorFunctionsGeneral::innerLogBase(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerLogBase");
  //stOutput << "reducing ...." << input.ToStringSemiFull();
  if (!input.StartsWith(theCommands.opLogBase(), 3))
    return false;
  Expression numE, denE;
  numE.MakeOX(theCommands, theCommands.opLog(), input[2]);
  denE.MakeOX(theCommands, theCommands.opLog(), input[1]);
  return output.MakeXOX(theCommands, theCommands.opDivide(), numE, denE);
}

bool CalculatorFunctionsGeneral::innerLog(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerLog");
  if (input.IsEqualToZero())
    return output.MakeError("Logarithm of zero is undefined.", theCommands);
  if (input.IsEqualToOne())
    return output.AssignValue(0, theCommands);
  if (theCommands.flagNoApproximations)
    return false;
  double theArgument;
  if (!input.EvaluatesToDouble(&theArgument))
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
  iE.MakeSqrt(theCommands, (Rational)-1, 2);
  piE.MakeAtom(theCommands.opPi(), theCommands);
  ipiE.MakeXOX(theCommands, theCommands.opTimes(), piE, iE);
  lnPart.AssignValue(FloatingPoint::log(theArgument), theCommands);
  return output.MakeXOX(theCommands, theCommands.opPlus(), lnPart, ipiE);
}

bool CalculatorFunctionsGeneral::innerFactorial(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerFactorial");
  int inputInt;
  if (!input.IsSmallInteger(&inputInt))
    return false;
  Rational result;
  return output.AssignValue(result.Factorial(inputInt), theCommands);
}

bool CalculatorFunctionsGeneral::innerArctan(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerArctan");
  double theArgument;
  if (!input.EvaluatesToDouble(&theArgument))
    return false;
  return output.AssignValue(FloatingPoint::arctan(theArgument), theCommands);
}

bool CalculatorFunctionsGeneral::innerArccos(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerArccos");
  double theArgument;
  if (!input.EvaluatesToDouble(&theArgument))
    return false;
  return output.AssignValue(FloatingPoint::arccos(theArgument), theCommands);
}

bool CalculatorFunctionsGeneral::innerArcsin(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerArcsin");
  double theArgument;
  if (!input.EvaluatesToDouble(&theArgument))
    return false;
  return output.AssignValue(FloatingPoint::arcsin(theArgument), theCommands);
}

bool CalculatorFunctionsGeneral::innerAbs(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerAbs");
  Rational theRat;
  if (input.IsRational(&theRat))
  { if (theRat<0)
      return output.AssignValue(-theRat, theCommands);
    return output.AssignValue(theRat, theCommands);
  }
  double theDouble=0;
  if (input.EvaluatesToDouble(&theDouble))
  { if (theDouble<0)
    { Expression moneE;
      moneE.AssignValue(-1, theCommands);
      output=input;
      output*=moneE;
      return true;
    }
    output=input;
    return true;
  }
  return false;
}

bool CalculatorFunctionsGeneral::innerSin(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSin");
  if (input.IsAtomGivenData(theCommands.opPi()))
    return output.AssignValue(0, theCommands);
  Rational piProportion;
  if (input.StartsWith(theCommands.opTimes(), 3))
    if (input[2].IsAtomGivenData(theCommands.opPi()))
      if (input[1].IsOfType<Rational>(&piProportion))
      { AlgebraicNumber algOutput;
        Rational ratOutput;
        if (algOutput.AssignSinRationalTimesPi(piProportion, theCommands.theObjectContainer.theAlgebraicClosure))
        { if (algOutput.IsRational(&ratOutput))
            return output.AssignValue(ratOutput, theCommands);
          return output.AssignValue(algOutput, theCommands);
        }
      }
  if (theCommands.flagNoApproximations)
    return false;
  double theArgument=0;
  if (!input.EvaluatesToDouble(&theArgument))
    return false;
  return output.AssignValue(FloatingPoint::sin(theArgument), theCommands);
}

bool CalculatorFunctionsGeneral::innerCos(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCos");
  if (input.IsAtomGivenData(theCommands.opPi()))
    return output.AssignValue(-1, theCommands);
  if (input.IsEqualToZero())
    return output.AssignValue(1, theCommands);
  Rational piProportion;
  if (input.StartsWith(theCommands.opTimes(), 3))
    if (input[2].IsAtomGivenData(theCommands.opPi()))
      if (input[1].IsOfType<Rational>(&piProportion))
      { AlgebraicNumber algOutput;
        Rational ratOutput;
        if (algOutput.AssignCosRationalTimesPi(piProportion, theCommands.theObjectContainer.theAlgebraicClosure))
        { if (algOutput.IsRational(&ratOutput))
            return output.AssignValue(ratOutput, theCommands);
          return output.AssignValue(algOutput, theCommands);
        }
      }
  if (theCommands.flagNoApproximations)
    return false;
  double theArgument=0;
  if (!input.EvaluatesToDouble(&theArgument))
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

bool Calculator::GetSumProductsExpressions(const Expression& inputSum, List<List<Expression> >& outputSumMultiplicands)
{ MacroRegisterFunctionWithName("Calculator::GetSumProductsExpressions");
  List<Expression> theSummands, currentMultiplicands;
  outputSumMultiplicands.SetSize(0);
  if (!this->CollectOpands(inputSum, this->opPlus(), theSummands))
    return false;
  for (int i=0; i<theSummands.size; i++)
  { this->CollectOpands(theSummands[i], this->opTimes(), currentMultiplicands);
    outputSumMultiplicands.AddOnTop(currentMultiplicands);
  }
  return true;
}

bool CalculatorFunctionsGeneral::innerCoefficientOf(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCoefficientOf");
  if (input.children.size!=3)
    return false;
  List<List<Expression> > theSummands;
  List<Expression> currentListMultiplicands, survivingSummands;
  Expression currentMultiplicand;
  if (input[2].StartsWith(theCommands.opDivide()))
  { Expression coefficientNumerator=input;
    coefficientNumerator.SetChilD(2, input[2][1]);
    if (!CalculatorFunctionsGeneral::innerCoefficientOf(theCommands, coefficientNumerator, output))
      return false;
    return output.MakeXOX(theCommands, theCommands.opDivide(), output, input[2][2]);
  }
  if (!theCommands.GetSumProductsExpressions(input[2], theSummands))
    return theCommands << "Failed to extract product of expressions from " << input[2].ToString();
  for(int i=0; i<theSummands.size; i++)
  { bool isGood=false;
    for (int j=0; j<theSummands[i].size; j++)
      if (theSummands[i][j]==input[1])
      { if (isGood)
        { isGood=false;
          break;
        }
        isGood=true;
      }
    if (!isGood)
      continue;
    currentListMultiplicands.SetSize(0);
    for (int j=0; j<theSummands[i].size; j++)
      if (theSummands[i][j]!=input[1])
        currentListMultiplicands.AddOnTop(theSummands[i][j]);
    currentMultiplicand.MakeProducT(theCommands, currentListMultiplicands);
    survivingSummands.AddOnTop(currentMultiplicand);
  }
  return output.MakeSum(theCommands, survivingSummands);
}

bool CalculatorFunctionsGeneral::innerDereferenceOperator(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDereferenceOperator");
  (void) theCommands;
  if (input.children.size!=2)
    return false;
//  if (!input[0].IsListStartingWithAtom(theCommands.opSequence()))
//    return false;
  int theIndex;
  if (!input[1].IsSmallInteger(&theIndex))
    return false;
  if (theIndex>0 && theIndex<input[0].children.size)
  { output=input[0][theIndex];
    return true;
  }
  return false;
}

bool CalculatorFunctionsGeneral::innerSolveSerreLikeSystem
(Calculator& theCommands, const Expression& input, Expression& output, bool useUpperLimit, bool startWithAlgebraicClosure)
{ MacroRegisterFunctionWithName("Calculator::innerSolveSerreLikeSystem");
  Vector<Polynomial<Rational> > thePolysRational;
  Expression theContext(theCommands);
  bool useArguments=
  input.StartsWith(theCommands.GetOperations().GetIndexIMustContainTheObject("FindOneSolutionSerreLikePolynomialSystem")) ||
  input.StartsWith(theCommands.GetOperations().GetIndexIMustContainTheObject("FindOneSolutionSerreLikePolynomialSystemAlgebraic")) ||
  input.StartsWith(theCommands.GetOperations().GetIndexIMustContainTheObject("FindOneSolutionSerreLikePolynomialSystemUpperLimit")) ||
  input.StartsWith(theCommands.GetOperations().GetIndexIMustContainTheObject("FindOneSolutionSerreLikePolynomialSystemAlgebraicUpperLimit"))
  ;

  if (useArguments)
  { if (!theCommands.GetVectorFromFunctionArguments(input, thePolysRational, &theContext, 0, CalculatorConversions::innerPolynomial<Rational>))
      return output.MakeError("Failed to extract list of polynomials. ", theCommands);
  } else
    if (!theCommands.GetVectoR(input, thePolysRational, &theContext, 0, CalculatorConversions::innerPolynomial<Rational>))
      return output.MakeError("Failed to extract list of polynomials. ", theCommands);
  GroebnerBasisComputation<AlgebraicNumber> theComputation;
  theContext.ContextGetFormatExpressions(theComputation.theFormat);
  int upperLimit=2001;
  if (useUpperLimit)
  { Rational upperLimitRat;
    if (!thePolysRational[0].IsConstant(&upperLimitRat))
      return theCommands << "Failed to extract a constant from the first argument "
      << thePolysRational[0].ToString(&theComputation.theFormat) << ". ";
    if (!upperLimitRat.IsIntegerFittingInInt(&upperLimit))
      return theCommands << "Failed to extract a small integer from the first argument "
      << upperLimitRat.ToString(&theComputation.theFormat) << ". ";
    thePolysRational.PopIndexShiftDown(0);
  }
  Vector<Polynomial<AlgebraicNumber> > thePolysAlgebraic;
  thePolysAlgebraic=thePolysRational;
  //int numVars=theContext.GetNumContextVariables();
  theComputation.MaxNumGBComputations=upperLimit;
  theComputation.MaxNumSerreSystemComputationsPreferred=upperLimit;
  theComputation.thePolynomialOrder.theMonOrder=
  MonomialP::LeftGreaterThanTotalDegThenLexicographicLastVariableStrongest;
//  stOutput << "Calling SolveSerreLikeSystem with upperLimit= " << upperLimit;
  theComputation.theAlgebraicClosurE=&theCommands.theObjectContainer.theAlgebraicClosure;
  theComputation.flagTryDirectlySolutionOverAlgebraicClosure=startWithAlgebraicClosure;
  theGlobalVariables.theDefaultFormat.GetElement()=theComputation.theFormat;
//  stOutput << "<br>the alphabet:" << theComputation.theFormat.polyAlphabeT;
//  stOutput << "<br>The context vars:<br>" << theContext.ToString();
  theComputation.flagUseTheMonomialBranchingOptimization=true;
  theComputation.SolveSerreLikeSystem(thePolysAlgebraic);
//  stOutput << "<br>Got to ere";
  std::stringstream out;
  out << "<br>The context vars:<br>" << theContext.ToString();
  out << "<br>The polynomials: " << thePolysAlgebraic.ToString(&theComputation.theFormat);
  out << "<br>Total number of polynomial computations: " << theComputation.NumberSerreSystemComputations;
  if (theComputation.flagSystemProvenToHaveNoSolution)
    out << "<br>The system does not have a solution. ";
  else if(theComputation.flagSystemProvenToHaveSolution)
    out << "<br>System proven to have solution.";
  if (!theComputation.flagSystemProvenToHaveNoSolution)
  { if (theComputation.flagSystemSolvedOverBaseField)
      out << "<br>One solution follows. " << theComputation.ToStringSerreLikeSolution();
    else
      out << " However, I was unable to find such a solution: my heuristics are not good enough.";
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerGetAlgebraicNumberFromMinPoly(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGetAlgebraicNumberFromMinPoly");
  Expression polyE;
  if (!CalculatorConversions::innerPolynomial<AlgebraicNumber>(theCommands, input, polyE) )
    return theCommands << "<hr>Failed to convert " << input.ToString() << " to polynomial. ";
  Polynomial<AlgebraicNumber> thePoly;
  if (!polyE.IsOfType<Polynomial<AlgebraicNumber> >(&thePoly))
   return theCommands << "<hr>Failed to convert " << input.ToString() << " to polynomial, instead got " << polyE.ToString();
  if (polyE.GetNumContextVariables()!=1)
    return theCommands << "<hr>After conversion, I got the polynomial " << polyE.ToString() << ", which is not in one variable.";
  AlgebraicNumber theAN;
  if (!theAN.ConstructFromMinPoly(thePoly, theCommands.theObjectContainer.theAlgebraicClosure))
    return false;
  return output.AssignValue(theAN, theCommands);
}

bool CalculatorFunctionsGeneral::innerCompositeApowerBevaluatedAtC(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCompositeApowerBevaluatedAtC");
  //stOutput << "<hr>input be: " << input.ToString();
  if (!input.IsListNElements())
    return false;
  const Expression& firstE=input[0];
  if (!firstE.StartsWith(theCommands.opThePower(), 3))
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
  (void) theCommands;//portable way of avoiding unused parameter warning
  if (!input.IsListNElements())
    return false;
  if (!input[0].IsConstantNumber())
    return false;
  output=input[0];
  return true;
}

bool CalculatorFunctionsGeneral::outerCombineFractionsCommutative(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::outerCombineFractionsCommutative");
  if (!input.StartsWith(theCommands.opPlus(), 3))
    return false;
  const Expression& leftE=input[1];
  const Expression& rightE=input[2];
  if (!leftE.StartsWith(theCommands.opDivide(), 3) ||
      !rightE.StartsWith(theCommands.opDivide(), 3))
    return false;
  theCommands << "<br><b>To do: make function outerCombineFractionsCommutative work much better.</b>";
  if (leftE[2]==rightE[2])
  { output=(leftE[1]+rightE[1])/leftE[2];
    return true;
  }
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
      else if (theLinPolys[i][j].IsConstant())
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

class IntegralRFComputation
{
public:
  RationalFunctionOld theRF;
  Polynomial<Rational> theDen, theNum;
  Polynomial<Rational> quotientRat, remainderRat;
//  Rational constantCoefficient;
  List<Polynomial<Rational> > theFactors;
  Polynomial<AlgebraicNumber> thePolyThatMustVanish;
  Polynomial<AlgebraicNumber> remainderRescaledAlgebraic;
  RationalFunctionOld transformedRF;
  FormatExpressions currentFormaT;
  Expression integrationSetE;
  Expression contextE;
  Expression inpuTE;
  Expression outputIntegralE;
  AlgebraicNumber additionalMultiple;
  Calculator* owner;
  bool allFactorsAreOfDegree2orless;
  bool needPolyDivision;
  int NumberOfSystemVariables;
  std::stringstream printoutPFsHtml;
  std::stringstream printoutPFsLatex;
  std::stringstream printoutIntegration;
  std::string stringPolyIndentityNonSimplifiedLatex;
  std::string stringRationalFunctionLatex;
  std::string stringRationalFunctionPartialFractionLatex;
  std::string stringFinalAnswer;
  std::string stringDenominatorFactors;
  MonomialCollection<Polynomial<AlgebraicNumber>, Rational> theDenominatorFactorsWithMults;
  List<List<Polynomial<AlgebraicNumber> > > theNumerators;
  List<Expression> theIntegralSummands;
  Expression theIntegralSum;
  bool ComputePartialFractionDecomposition();
  void PrepareFormatExpressions();
  void PrepareDenominatorFactors();
  void PrepareNumerators();
  void PrepareFinalAnswer();
  std::string ToStringRationalFunctionLatex();
  bool IntegrateRF();
  IntegralRFComputation(Calculator* inputOwner):owner(inputOwner){}
  bool CheckConsistency()const;
};

bool IntegralRFComputation::CheckConsistency()const
{ if (this->owner==0)
    crash << "Non-initialized rf computation" << crash;
  return true;
}

bool IntegralRFComputation::IntegrateRF()
{ MacroRegisterFunctionWithName("IntegralRFComputation::IntegrateRF");
  this->CheckConsistency();
  if (!this->ComputePartialFractionDecomposition())
  { printoutIntegration << "Failed to decompose rational function into partial fractions. " << this->printoutPFsHtml.str();
    return false;
  }
//  stOutput << "<hr>Context variable 1: " << this->contextE.ContextGetContextVariable(0);
  printoutIntegration << this->printoutPFsHtml.str();
  Expression polyE, currentNum, denExpE, currentDenNoPowerMonic, currentDen, currentIntegrand,
  currentIntegralNoCoeff, currentIntegralWithCoeff, coeffE;
  this->theIntegralSummands.SetSize(0);
  AlgebraicNumber theCoefficient;
  Polynomial<AlgebraicNumber> denRescaled, numRescaled;
  AlgebraicNumber currentCoefficient, numScale;
  for (int i=0; i<this->theNumerators.size; i++)
    for (int j=0; j<this->theNumerators[i].size; j++)
    { if (this->theNumerators[i][j].IsEqualToZero())
        continue;
      denRescaled=this->theDenominatorFactorsWithMults[i];
      numRescaled=this->theNumerators[i][j];
      currentCoefficient=denRescaled.theCoeffs[denRescaled.GetIndexMaxMonomial()];
      currentCoefficient.Invert();
      denRescaled*=currentCoefficient;
      MathRoutines::RaiseToPower(currentCoefficient, j+1, (AlgebraicNumber) 1);
      numScale=numRescaled.theCoeffs[numRescaled.GetIndexMaxMonomial()];
      numRescaled/=numScale;
      currentCoefficient*=numScale;
      polyE.AssignValueWithContext(numRescaled, this->contextE, *this->owner);

//      stOutput << "<br>polyE is: " << polyE.ToString();
      if (!CalculatorConversions::innerExpressionFromBuiltInType(*this->owner, polyE, currentNum))
        return false;
//      stOutput << "<br>currentNum is: " << currentNum.ToString();
      polyE.AssignValueWithContext(denRescaled, this->contextE, *this->owner);
      if(!CalculatorConversions::innerExpressionFromBuiltInType(*this->owner, polyE, currentDenNoPowerMonic))
        return false;
//      stOutput << "<br>currentDenNoPower is: " << currentDenNoPower.ToString();
      if (j!=0)
      { denExpE.AssignValue(j+1, *this->owner);
        currentDen.MakeXOX(*this->owner, this->owner->opThePower(), currentDenNoPowerMonic, denExpE);
      } else
        currentDen=currentDenNoPowerMonic;
      currentIntegrand=currentNum;
      currentIntegrand/=currentDen;
//      stOutput << "<br>CurrentIntegrand is: " << currentIntegrand.ToString();
      currentIntegralNoCoeff.MakeIntegral
      (*this->owner, this->integrationSetE, currentIntegrand,
       this->contextE.ContextGetContextVariable(0));
      coeffE.AssignValue(currentCoefficient, *this->owner);
      currentIntegralWithCoeff=coeffE*currentIntegralNoCoeff;
      currentIntegralWithCoeff.CheckConsistencyRecursively();
//      stOutput << "<br>Current integral is: " << currentIntegral.ToString();
      this->theIntegralSummands.AddOnTop(currentIntegralWithCoeff);
    }
  if (!this->quotientRat.IsEqualToZero())
  { Expression currentIntegrandPolyForm;
    currentIntegrandPolyForm.AssignValueWithContext(this->quotientRat, this->contextE, *this->owner);
    if (!CalculatorConversions::innerExpressionFromPoly<Rational>(*this->owner, currentIntegrandPolyForm, currentIntegrand))
    { *this->owner << "<br>Something is wrong: failed to convert polynomial " << currentIntegrandPolyForm.ToString()
      << " to expression. This shouldn't happen. ";
      return false;
    }
    currentIntegralWithCoeff.MakeIntegral
    (*this->owner, this->integrationSetE, currentIntegrand,
     this->contextE.ContextGetContextVariable(0));
    currentIntegralWithCoeff.CheckConsistencyRecursively();

    this->theIntegralSummands.AddOnTop(currentIntegralWithCoeff);
  }
//  if (this->the)
  this->theIntegralSum.MakeSum(*this->owner, this->theIntegralSummands);
//  stOutput << " got before consistency check";
  this->theIntegralSum.CheckConsistencyRecursively();
//  this->theIntegralSum.CheckInitializationRecursively();
//  stOutput << " passed consistency check";
  return true;
}

template <class coefficient>
bool Polynomial<coefficient>::FactorMeNormalizedFactors
(Rational& outputCoeff, List<Polynomial<Rational> >& outputFactors, std::stringstream* comments)const
{ MacroRegisterFunctionWithName("Polynomial::FactorMeNormalizedFactors");
  List<Polynomial<Rational> > factorsToBeProcessed;
  outputFactors.SetSize(0);
  factorsToBeProcessed.AddOnTop(*this);

  factorsToBeProcessed.LastObject()->ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
  Polynomial<Rational> currentFactor, divisor;
  while (factorsToBeProcessed.size>0)
  { currentFactor=factorsToBeProcessed.PopLastObject();
//    stOutput << "<hr>Factoring " << currentFactor.ToString() << "<br>";
    if(!currentFactor.FactorMeOutputIsADivisor(divisor, comments))
      return false;
    if (currentFactor.IsEqualToOne())
    { divisor.ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
      outputFactors.AddOnTop(divisor);
      continue;
    }
//    stOutput << "<br><b>Smallest divisor: " << divisor.ToString() << ", thepoly: "
//    << currentFactor.ToString() << "</b>";
    divisor.ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
    factorsToBeProcessed.AddOnTop(divisor);
    factorsToBeProcessed.AddOnTop(currentFactor);
  }
  outputFactors.QuickSortAscending();
  Polynomial<Rational> checkComputations;
  checkComputations.MakeOne();
  for (int i=0; i<outputFactors.size; i++)
    checkComputations*=outputFactors[i];
  if (!checkComputations.IsProportionalTo(*this, outputCoeff, 1))
    crash << "Error in polynomial factorization function." << crash;
  return true;
}

void IntegralRFComputation::PrepareFormatExpressions()
{ MacroRegisterFunctionWithName("IntegralRFComputation::PrepareFormatExpressions");
  std::stringstream rfStream, polyStream;
  rfStream << transformedRF.ToString(&this->currentFormaT) << " = ";
  polyStream << remainderRescaledAlgebraic.ToString(&this->currentFormaT) << " = ";
  int varCounter=0;
  for (int i =0; i<this->theDenominatorFactorsWithMults.size(); i++)
  { int tempSize=-1;
    this->theDenominatorFactorsWithMults.theCoeffs[i].IsSmallInteger(&tempSize);
    for (int k=0; k<this->theDenominatorFactorsWithMults.theCoeffs[i]; k++)
    { rfStream << "\\frac{";
      if (this->theDenominatorFactorsWithMults[i].TotalDegree()>1)
        polyStream << "(";
      for (int j=0; j<this->theDenominatorFactorsWithMults[i].TotalDegree(); j++)
      { varCounter++;
        std::stringstream varNameStream;
        varNameStream << "A_{" << varCounter << "} ";
        rfStream << varNameStream.str();
        polyStream << varNameStream.str();
        this->currentFormaT.polyAlphabeT.AddOnTop(varNameStream.str());
        if (j>0)
        { rfStream << "x";
          polyStream << "x";
        }
        if (j>1)
        { rfStream << "^{" << j << "}";
          polyStream << "^{" << j << "}";
        }
        if ((this->theDenominatorFactorsWithMults[i].TotalDegree()-1)!=j)
        { rfStream << " + ";
          polyStream << " + ";
        }
      }
      if (this->theDenominatorFactorsWithMults[i].TotalDegree()>1)
        polyStream << ")";
      for (int j=0; j<this->theDenominatorFactorsWithMults.size(); j++)
      { Rational theExp=this->theDenominatorFactorsWithMults.theCoeffs[j];
        if (j==i)
          theExp-=k+1;
        if (theExp==0)
          continue;
        polyStream << "(" << this->theDenominatorFactorsWithMults[j].ToString(&this->currentFormaT) << ")";
        if (theExp>1)
          polyStream << "^{" << theExp << "}";
      }
      rfStream << "}{";
      if (k>0)
        rfStream << "(";
      rfStream << this->theDenominatorFactorsWithMults[i].ToString(&this->currentFormaT);
      if (k>0)
        rfStream << ")^{" << k+1 << "}";
      rfStream << "}";
      if (((this->theDenominatorFactorsWithMults.theCoeffs[i]-1)!=k) || (i!=this->theDenominatorFactorsWithMults.size()-1))
      { rfStream << "+";
        polyStream << "+";
      }
    }
  }
  this->stringRationalFunctionLatex=rfStream.str();
  this->stringPolyIndentityNonSimplifiedLatex=polyStream.str();
}

void IntegralRFComputation::PrepareNumerators()
{ MacroRegisterFunctionWithName("IntegralRFComputation::PrepareNumerators");
//  stOutput << "DEBUG: Remainder rat: " << this->remainderRat.ToString();
  this->transformedRF=this->remainderRat;
  //stOutput << " denominator: " << this->theDen.ToString();
  this->transformedRF/=this->theDen;
  //stOutput << " transformed rf: " << this->transformedRF.ToString();
  this->remainderRescaledAlgebraic=this->remainderRat;
  this->remainderRescaledAlgebraic/=additionalMultiple;
  this->NumberOfSystemVariables=0;
  Polynomial<AlgebraicNumber> currentSummand;
  MonomialP currentMon;
  this->thePolyThatMustVanish.MakeZero();
  this->thePolyThatMustVanish-=remainderRescaledAlgebraic;
  this->theNumerators.SetSize(this->theDenominatorFactorsWithMults.size());
  for (int i =0; i<this->theDenominatorFactorsWithMults.size(); i++)
  { int tempSize=-1;
    //stOutput << "<br>DEBUG: Accounting denominator: " << this->theDenominatorFactorsWithMults[i].ToString() << " with coeff: "
    //<< this->theDenominatorFactorsWithMults.theCoeffs[i].ToString();
    this->theDenominatorFactorsWithMults.theCoeffs[i].IsSmallInteger(&tempSize);
    this->theNumerators[i].SetSize(tempSize);
    for (int k=0; k<this->theDenominatorFactorsWithMults.theCoeffs[i]; k++)
    { currentSummand.MakeZero();
      this->theNumerators[i][k].MakeZero();
      for (int j=0; j<this->theDenominatorFactorsWithMults[i].TotalDegree(); j++)
      { this->NumberOfSystemVariables++;
        currentMon.MakeEi(this->NumberOfSystemVariables);
        currentMon[0]=j;
        this->theNumerators[i][k].AddMonomial(currentMon, 1);
        currentSummand.AddMonomial(currentMon, 1);
      }
      for (int j=0; j<this->theDenominatorFactorsWithMults.size(); j++)
      { Rational theExp=this->theDenominatorFactorsWithMults.theCoeffs[j];
        if (j==i)
          theExp-=k+1;
        if (theExp==0)
          continue;
        for (int p=0; p<theExp; p++)
          currentSummand*=this->theDenominatorFactorsWithMults[j];
      }
      this->thePolyThatMustVanish+=currentSummand;
    }
  }
}

void IntegralRFComputation::PrepareFinalAnswer()
{ MacroRegisterFunctionWithName("IntegralRFComputation::PrepareFinalAnswer");
  std::stringstream rfComputedStream, answerFinalStream;
  for (int i =0; i<theDenominatorFactorsWithMults.size(); i++)
    for (int k=0; k<theDenominatorFactorsWithMults.theCoeffs[i]; k++)
    { rfComputedStream << "\\frac{" << this->theNumerators[i][k].ToString(&this->currentFormaT) << "}";
      rfComputedStream << "{";
      rfComputedStream << "(" << theDenominatorFactorsWithMults[i].ToString(&this->currentFormaT) << ")";
      if (k>0)
        rfComputedStream << "^{" << k+1 << "}";
      rfComputedStream << "}";
      if (((theDenominatorFactorsWithMults.theCoeffs[i]-1)!=k) || (i!=theDenominatorFactorsWithMults.size()-1))
        rfComputedStream << "+";
    }
  this->stringRationalFunctionPartialFractionLatex=rfComputedStream.str();
  answerFinalStream << this->theRF.ToString(&this->currentFormaT) << "=";
  if (!this->quotientRat.IsEqualToZero())
  { answerFinalStream << this->quotientRat.ToString(&this->currentFormaT) << "+ ";
    answerFinalStream << this->transformedRF.ToString(&this->currentFormaT) << "=";
  }
  if (!this->quotientRat.IsEqualToZero())
    answerFinalStream << this->quotientRat.ToString(&this->currentFormaT) << "+ ";
  answerFinalStream << this->stringRationalFunctionPartialFractionLatex;
  this->stringFinalAnswer=answerFinalStream.str();
}

void IntegralRFComputation::PrepareDenominatorFactors()
{ MacroRegisterFunctionWithName("IntegralRFComputation::PrepareDenominatorFactors");
  this->printoutPFsHtml << "The rational function is: " << CGI::GetMathSpanPure
  ("\\frac{" + this->theNum.ToString(&this->currentFormaT) + "}{" +this->theDen.ToString(&this->currentFormaT) +"}")
  << ".";
  this->printoutPFsHtml << "<br>The denominator factors are: ";
  this->printoutPFsLatex << "We aim to decompose into partial fractions the following function "
  << "(the denominator has been factored). \\[\\frac{"
  << this->theNum.ToString(&this->currentFormaT) << "}{" << this->theDen.ToString(&this->currentFormaT) << "}="
  << "\\frac{" << this->theNum.ToString(&this->currentFormaT)  << "}{ ";
  this->allFactorsAreOfDegree2orless =true;
  for (int i=0; i<this->theFactors.size; i++)
  { this->printoutPFsHtml << CGI::GetMathSpanPure(this->theFactors[i].ToString(&this->currentFormaT));
    bool needsParenthesis= this->theFactors[i].NeedsParenthesisForMultiplication();
    if (needsParenthesis)
      this->printoutPFsLatex << "\\left(";
    this->printoutPFsLatex << this->theFactors[i].ToString(&this->currentFormaT);
    if (needsParenthesis)
      this->printoutPFsLatex << "\\right)";
    if (i!=this->theFactors.size-1)
      this->printoutPFsHtml << ", ";
    if (this->theFactors[i].TotalDegree()>2)
      allFactorsAreOfDegree2orless=false;
  }
  this->printoutPFsLatex << "}";
  this->printoutPFsLatex << "\\]";
  this->printoutPFsHtml << ". <br>";
}

template<class coefficient>
std::string GroebnerBasisComputation<coefficient>::GetPolynomialStringSpacedMonomialsLaTeX
  (const Polynomial<coefficient>& thePoly, const HashedList<MonomialP>& theMonomialOrder,
   std::string* highlightColor, List<MonomialP>* theHighLightedMons)
{ MacroRegisterFunctionWithName("GroebnerBasisComputation::GetPolynomialStringSpacedMonomialsLaTeX");
  std::stringstream out;
  bool found=false;
  int countMons=0;
  for (int i=0; i<theMonomialOrder.size; i++)
  { int theIndex= thePoly.theMonomials.GetIndex(theMonomialOrder[i]);
    if (theIndex==-1)
    { if (i!=theMonomialOrder.size-1)
        out << "&";
      continue;
    }
    countMons++;
    bool useHighlightStyle=false;
    if (highlightColor!=0)
      if (theHighLightedMons!=0)
        if (theHighLightedMons->Contains(theMonomialOrder[i]))
          useHighlightStyle=true;
    out << "$";
    if (useHighlightStyle)
      out << "\\color{" << *highlightColor << "}{";
    out << Polynomial<Rational>::GetBlendCoeffAndMon(thePoly[theIndex], thePoly.theCoeffs[theIndex], found, &this->theFormat);
    found=true;
    if (useHighlightStyle)
      out << "}\\color{black}";
    out << "$ ";
    if (i!=theMonomialOrder.size-1)
      out << "& ";
  }
  if (countMons!=thePoly.size())
    out << " Programming ERROR!";
  return out.str();
}

template <class coefficient>
std::string GroebnerBasisComputation<coefficient>::GetDivisionStringLaTeX()
{ MacroRegisterFunctionWithName("GroebnerBasisComputation::GetDivisionStringLaTeX");
  std::stringstream out;
  List<Polynomial<Rational> >& theRemainders=this->intermediateRemainders.GetElement();
  List<Polynomial<Rational> >& theSubtracands=this->intermediateSubtractands.GetElement();
  this->theFormat.thePolyMonOrder=this->thePolynomialOrder.theMonOrder;
  HashedList<MonomialP> totalMonCollection;
  std::string HighlightedColor="red";
  totalMonCollection.AddOnTopNoRepetition(this->startingPoly.GetElement().theMonomials);
  for (int i=0; i<theRemainders.size; i++)
  { totalMonCollection.AddOnTopNoRepetition(theRemainders[i].theMonomials);
    totalMonCollection.AddOnTopNoRepetition(theSubtracands[i].theMonomials);
  }
  //List<std::string> basisColorStyles;
  //basisColorStyles.SetSize(this->theBasiS.size);
  totalMonCollection.QuickSortDescending(this->thePolynomialOrder.theMonOrder);
//  stOutput << "<hr>The monomials in play ordered: " << totalMonCollection.ToString(theFormat);
//  int numVars=this->GetNumVars();
  this->theFormat.flagUseLatex=true;
  out << this->ToStringLetterOrder(true);
  out << theRemainders.size << " division steps total.";
  out << "\\renewcommand{\\arraystretch}{1.2}";
  out << "\\begin{longtable}{|c";
  for (int i =0; i<totalMonCollection.size; i++)
    out << "c";
  out << "|} \\hline";
  out << "&" <<  "\\multicolumn{" << totalMonCollection.size << "}{|c|}{\\textbf{Remainder}}" << "\\\\";
  out << "\\multicolumn{1}{|c|}{} & ";
  out << this->GetPolynomialStringSpacedMonomialsLaTeX
  (this->remainderDivision, totalMonCollection, &HighlightedColor, &this->remainderDivision.theMonomials)
  << "\\\\\\hline";
  out << "\\textbf{Divisor(s)} &" << "\\multicolumn{" << totalMonCollection.size << "}{|c|}{\\textbf{Quotient(s)}}"
  << "\\\\";
  Polynomial<coefficient> currentQuotient;
  for (int i=0; i<this->theBasiS.size; i++)
  { out << "$";
    out << this->theBasiS[i].ToString(&this->theFormat);
    out << "$";
    out << "& \\multicolumn{" << totalMonCollection.size << "}{|l|}{";
    currentQuotient.MakeZero();
    for (int j=0; j<theRemainders.size; j++)
    { if (this->intermediateSelectedDivisors.GetElement()[j]!=i)
        continue;
      currentQuotient.AddMonomial(this->intermediateHighestMonDivHighestMon.GetElement()[j],
      this->intermediateCoeffs.GetElement()[j]);
    }
    out << "$" << currentQuotient.ToString(&this->theFormat) << "$" << "}\\\\\\hline";
  }
  out << "& \\multicolumn{" << totalMonCollection.size << "}{|c|}{\\textbf{Dividend}}\\\\";
  out << "\\multicolumn{1}{|c|}{";
  if (theRemainders.size>0)
    out << "$\\underline{~}$";
  out << "} &";
  out << this->GetPolynomialStringSpacedMonomialsLaTeX
  (this->startingPoly.GetElement(), totalMonCollection, &HighlightedColor,
   &this->intermediateHighlightedMons.GetElement()[0]);
  out << "\\\\";
  for (int i=0; i<theRemainders.size; i++)
  { out << "&";
    out << this->GetPolynomialStringSpacedMonomialsLaTeX(theSubtracands[i], totalMonCollection, &HighlightedColor)
    << "\\\\\\cline{2-" << totalMonCollection.size+1 << "}";
    if (i<theRemainders.size-1)
      out << "$\\underline{~}$";
    out << "&"
    << this->GetPolynomialStringSpacedMonomialsLaTeX
    (theRemainders[i], totalMonCollection, &HighlightedColor, &this->intermediateHighlightedMons.GetElement()[i+1])
    << "\\\\";
  }
  out << "\\hline";
  out << "\\end{longtable}";
  return out.str();
}

bool IntegralRFComputation::ComputePartialFractionDecomposition()
{ MacroRegisterFunctionWithName("IntegralRFComputation::ComputePartialFractionDecomposition");
  this->CheckConsistency();
  this->contextE=this->inpuTE.GetContext();
  //stOutput << "<hr>Context: " << this->contextE.ToString();
  //stOutput << "<br>Context variable 1: " << this->contextE.ContextGetContextVariable(0).ToString();
  //stOutput << "<br>Context variable 2: " << this->contextE.ContextGetContextVariable(1).ToString();

  this->contextE.ContextGetFormatExpressions(this->currentFormaT);
  if (this->theRF.GetMinNumVars()<1 || this->theRF.expressionType== this->theRF.typeRational ||
      this->theRF.expressionType==this->theRF.typePoly)
  { this->printoutPFsHtml << this->theRF.ToString(&this->currentFormaT) << " is already split into partial fractions. ";
    return true;
  }
  this->theRF.GetDenominator(this->theDen);
  this->theRF.GetNumerator(this->theNum);
  this->theNum*= this->theDen.ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
  //stOutput << "<br>The den: " << this->theDen << " the num: " << this->theNum.ToString();
  Rational theConstantCoeff;
  if (!this->theDen.FactorMeNormalizedFactors(theConstantCoeff, this->theFactors, &this->printoutPFsHtml))
  { this->printoutPFsHtml << "<hr>Failed to factor the denominator of the rational function, I surrender.";
    return false;
  }
//  stOutput << "<br>The factors: " << this->theFactors.ToString() << "; <br> the const coeff: " << theConstantCoeff;
  this->theNum/=theConstantCoeff;
  this->theDen/=theConstantCoeff;
  Polynomial<Rational> tempP;
  tempP.MakeConst(1);
  for (int i=0; i<this->theFactors.size; i++)
    tempP*=this->theFactors[i];
  if (tempP!=this->theDen)
    crash << "Something is very wrong: product of denominator factors is " << tempP.ToString(&this->currentFormaT)
    << ", but the denominator equals: " << this->theDen.ToString(&this->currentFormaT);
  this->printoutPFsLatex << "\\documentclass{article}\\usepackage{longtable}\\usepackage{xcolor}\\usepackage{multicol} \\begin{document}";
  this->PrepareDenominatorFactors();
  if (!allFactorsAreOfDegree2orless)
  { this->printoutPFsHtml << "There were factors (over the rationals) of degree greater than 2. I surrender. ";
    return false;
  }
  this->currentFormaT.flagUseFrac=true;
  this->theNum.DivideBy(this->theDen, this->quotientRat, this->remainderRat);
//  stOutput << "this->TheDen= " << this->theDen.ToString(&this->currentFormaT)
//  << "<br>this->quotientRat= " << this->quotientRat.ToString(&this->currentFormaT)
//  << "<br>this->remainderRat= " << this->remainderRat.ToString(&this->currentFormaT);
  needPolyDivision=!this->quotientRat.IsEqualToZero();
  if (needPolyDivision)
  { this->printoutPFsHtml << "<br>The numerator " << CGI::GetMathSpanPure(this->theNum.ToString(&this->currentFormaT))
    << " divided by the denominator " << CGI::GetMathSpanPure(theDen.ToString(&this->currentFormaT)) << " yields "
    << CGI::GetMathSpanPure(this->quotientRat.ToString(&this->currentFormaT)) << " with remainder "
    << CGI::GetMathSpanPure(this->remainderRat.ToString(&this->currentFormaT)) << ". ";
    GroebnerBasisComputation<Rational> theGB;
    theGB.flagDoLogDivision=true;
    theGB.theBasiS.SetSize(1);
    theGB.theBasiS[0]= this->theDen;
    theGB.theFormat=this->currentFormaT;
    //  Polynomial<Rational> outputRemainder;
    theGB.thePolynomialOrder.theMonOrder=MonomialP::LeftGreaterThanTotalDegThenLexicographicLastVariableStrongest;
    theGB.initForDivisionAlone(theGB.theBasiS);
    Polynomial<Rational> theNumCopy=this->theNum;
    theGB.RemainderDivisionWithRespectToBasis(theNumCopy, &theGB.remainderDivision, -1);
    this->printoutPFsLatex << "Here is a detailed long polynomial division. ";
    this->printoutPFsLatex << theGB.GetDivisionStringLaTeX();
    this->printoutPFsHtml << "<br>Here is a detailed long polynomial division:<br> ";
    this->printoutPFsHtml << theGB.GetDivisionStringHtml();
  }
  MonomialCollection<Polynomial<Rational>, Rational> theDenominatorFactorsWithMultsCopy;
  theDenominatorFactorsWithMultsCopy.MakeZero();
  for (int i=0; i<this->theFactors.size; i++)
  { theDenominatorFactorsWithMultsCopy.AddMonomial(this->theFactors[i], 1);
    //this->currentFormaT.flagSuppressOneIn1overXtimesY=false;
//    stOutput << "<br>Factor: " << theFactors[i].ToString(&this->currentFormaT);
  }
  theDenominatorFactorsWithMultsCopy.QuickSortAscending();
  Polynomial<Rational> currentSecondDegreePoly;
  this->theDenominatorFactorsWithMults.MakeZero();
  Polynomial<AlgebraicNumber> currentLinPoly, currentSecondDegreePolyAlgebraic;
  this->additionalMultiple=1;
  for (int i=0; i<theDenominatorFactorsWithMultsCopy.size(); i++)
  { currentSecondDegreePoly=theDenominatorFactorsWithMultsCopy[i];
    currentSecondDegreePolyAlgebraic=currentSecondDegreePoly;
    if (currentSecondDegreePoly.TotalDegree()!=2)
    { this->theDenominatorFactorsWithMults.AddMonomial(currentSecondDegreePolyAlgebraic, theDenominatorFactorsWithMultsCopy.theCoeffs[i]);
      continue;
    }
    Rational theDiscriminant=currentSecondDegreePoly.GetDiscriminant();
//    stOutput << "The discriminant: " << theDiscriminant.ToString();
    if (theDiscriminant<0)
    { this->theDenominatorFactorsWithMults.AddMonomial(currentSecondDegreePolyAlgebraic, theDenominatorFactorsWithMultsCopy.theCoeffs[i]);
      continue;
    }
    AlgebraicNumber theDiscriminantSqrt;
    if (!theDiscriminantSqrt.AssignRationalQuadraticRadical(theDiscriminant, this->owner->theObjectContainer.theAlgebraicClosure))
    { this->printoutPFsHtml << "Failed to take radical of " << theDiscriminant.ToString() << " (radical too large?).";
      return false;
    }
//    stOutput << "<br>sqrt of discriminant: " << theDiscriminantSqrt.ToString();
    theDiscriminantSqrt.CheckConsistency();
    AlgebraicNumber a=currentSecondDegreePoly.GetMonomialCoefficient(MonomialP(0,2));
    AlgebraicNumber b=currentSecondDegreePoly.GetMonomialCoefficient(MonomialP(0,1));
    a.CheckConsistency();
    b.CheckConsistency();
    currentLinPoly.MakeMonomiaL(0,1);
    currentLinPoly-=(-b+theDiscriminantSqrt)/(a*2);
    this->theDenominatorFactorsWithMults.AddMonomial(currentLinPoly, theDenominatorFactorsWithMultsCopy.theCoeffs[i]);
    currentLinPoly.MakeMonomiaL(0,1);
    currentLinPoly-=(-b-theDiscriminantSqrt)/(a*2);
    this->theDenominatorFactorsWithMults.AddMonomial(currentLinPoly, theDenominatorFactorsWithMultsCopy.theCoeffs[i]);
    additionalMultiple*=a;
//    Rational c=currentSecondDegreePoly.GetMonomialCoefficient(MonomialP(0,0));
  }
  this->theDenominatorFactorsWithMults.QuickSortAscending();
//  this->printoutPFsHtml << "<hr>this->theDenominatorFactorsWithMults: " << this->theDenominatorFactorsWithMults.ToString();
  this->printoutPFsHtml << "<br><br>I need to find " << CGI::GetMathSpanPure("A_i") << "'s so that I have the equality of rational functions: ";
  this->printoutPFsLatex << "We need to find $A_i$'s so that we have the following equality of rational functions. ";
  this->PrepareNumerators();
  this->PrepareFormatExpressions();
  this->printoutPFsHtml << CGI::GetMathSpanPure(this->stringRationalFunctionLatex, -1);
  this->printoutPFsHtml << "<br><br>After clearing denominators, we get the equality: ";
  this->printoutPFsLatex << "After clearing denominators, we get the following equality. ";
  this->printoutPFsHtml << "<br><br>" << CGI::GetMathSpanPure(this->stringPolyIndentityNonSimplifiedLatex, -1);
  this->printoutPFsLatex << "\\[" << this->stringPolyIndentityNonSimplifiedLatex << "\\]";
  Polynomial<Polynomial<AlgebraicNumber> > univariateThatMustDie;
  thePolyThatMustVanish.GetPolyUnivariateWithPolyCoeffs(0, univariateThatMustDie);
  this->printoutPFsHtml << "<br><br>After rearranging we get that the following polynomial must vanish: "
  << CGI::GetMathSpanPure(univariateThatMustDie.ToString(&this->currentFormaT));
  this->printoutPFsLatex << "After rearranging we get that the following polynomial must vanish. Here, by ``vanish'' "
  << "we mean that the coefficients of the powers of $x$ must be equal to zero."
  << "\\[" << univariateThatMustDie.ToString(&this->currentFormaT) << "\\]";
  this->printoutPFsHtml << "<br>Here, by ``vanish'', we mean that the coefficients in front of the powers of x must vanish.";
  Matrix<AlgebraicNumber> theSystemHomogeneous, theSystemHomogeneousForLaTeX, theConstTerms, theConstTermsForLaTeX;
  Polynomial<AlgebraicNumber>::GetLinearSystemFromLinearPolys(univariateThatMustDie.theCoeffs, theSystemHomogeneous, theConstTerms);
  theSystemHomogeneousForLaTeX=theSystemHomogeneous;
  theConstTermsForLaTeX=theConstTerms;
  this->currentFormaT.flagFormatMatrixAsLinearSystem=true;
  this->printoutPFsHtml << "<br>In other words, we need to solve the system: "
  << CGI::GetMathSpanPure(theSystemHomogeneous.ToStringSystemLatex(&theConstTerms, &this->currentFormaT),-1);
  this->printoutPFsLatex << "In other words, we need to solve the following system. "
  << "\\[" << theSystemHomogeneous.ToStringSystemLatex(&theConstTerms, &this->currentFormaT) << "\\]";
  this->currentFormaT.flagUseHTML=true;
  theSystemHomogeneous.GaussianEliminationByRows(&theConstTerms, 0,0, &this->printoutPFsHtml, &this->currentFormaT);
  this->currentFormaT.flagUseHTML=false;
  theSystemHomogeneousForLaTeX.GaussianEliminationByRows(&theConstTermsForLaTeX, 0,0, &this->printoutPFsLatex, &this->currentFormaT);
  PolynomialSubstitution<AlgebraicNumber> theSub;
  theSub.MakeIdSubstitution(this->NumberOfSystemVariables+1);
  for (int i=1; i<theSub.size; i++)
    theSub[i].MakeConst(theConstTerms(i-1,0));
  //this->printoutPFsHtml << "The sub is: " << theSub.ToString();
  for (int i =0; i<theDenominatorFactorsWithMults.size(); i++)
    for (int k=0; k<theDenominatorFactorsWithMults.theCoeffs[i]; k++)
      this->theNumerators[i][k].Substitution(theSub);
  this->PrepareFinalAnswer();
  this->printoutPFsHtml << "<br>Therefore, the final partial fraction decomposition is: "
  << CGI::GetMathSpanPure(this->stringFinalAnswer);
  this->printoutPFsLatex << "Therefore, the final partial fraction decomposition is the following. "
  << "\\[" << this->stringFinalAnswer << "\\]";
//  static bool tempBool=false;
//  if (!tempBool)
//    stOutput << "<hr>Here be the printout: " << this->printoutPFsHtml.str();
//  tempBool=true;

  this->printoutPFsLatex << "\\end{document}";
  this->printoutPFsHtml << "<hr>The present printout, in latex format, in ready form for copy+paste to your latex editor, follows<hr> ";
  this->printoutPFsHtml << this->printoutPFsLatex.str();
  return true;
}

bool CalculatorFunctionsGeneral::innerSplitToPartialFractionsOverAlgebraicReals(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSplitToPartialFractionsOverAlgebraicReals");
  IntegralRFComputation theComputation(&theCommands);
  bool isGood=CalculatorConversions::innerRationalFunction(theCommands, input, theComputation.inpuTE);
  if (isGood)
    isGood=theComputation.inpuTE.IsOfType<RationalFunctionOld>();
  if (!isGood)
    return theCommands << "CalculatorFunctionsGeneral::innerSplitToPartialFractionsOverAlgebraicReals: Failed to convert "
    << input.ToString() << " to rational function. ";
  theComputation.theRF=theComputation.inpuTE.GetValue<RationalFunctionOld>();
  if (theComputation.theRF.GetMinNumVars()>1)
    return theCommands << "The input rational function is of " << theComputation.theRF.GetMinNumVars() << " variables and "
    << " I can handle only 1.";
  theComputation.ComputePartialFractionDecomposition();
  return output.AssignValue(theComputation.printoutPFsHtml.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerGaussianEliminationMatrix(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("innerGaussianEliminationMatrix");
  Expression theConverted;
  if (!CalculatorConversions::innerMatrixAlgebraic(theCommands, input, theConverted))
    return theCommands << "<hr>Failed to extract algebraic number matrix from " << input.ToString();
  Matrix<AlgebraicNumber> theMat;
  if (!theConverted.IsOfType<Matrix<AlgebraicNumber> >(&theMat))
    return theCommands << "<hr>Failed to extract algebraic number matrix, got intermediate conversion to: "
    << theConverted.ToString();
  if (theMat.NumRows<2)
    return theCommands << "<hr>The matrix I got as input had only 1 row. Possible user typo?";
  std::stringstream out;
  FormatExpressions theFormat;
  theFormat.flagUseFrac=true;
  theMat.GaussianEliminationByRows(0, 0, 0, &out, & theFormat);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerCompositeConstTimesAnyActOn(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCompositeConstTimesAnyActOn");
  if (!input.IsListNElements())
    return false;
  if (!input[0].StartsWith(theCommands.opTimes(), 3))
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
    return theCommands << "<hr> I cannot apply " << theEWA.ToString() << " onto " << theArgumentPoly.ToString() << " as "
    << "the differential operator contains non-integral differential operator exponents. ";
  if (!theEWA.ActOnPolynomial(theArgumentPoly))
  { std::stringstream out;
    out << "Failed to act by operator " << theEWA.ToString() << " on polynomial " << theArgumentPoly.ToString()
    << " (possibly the weyl algebra element has non-integral exponents)";
    return output.MakeError(out.str(), theCommands);
  }
  return output.AssignValueWithContext(theArgumentPoly, theEWAE.GetContext(), theCommands);
}

bool CalculatorFunctionsGeneral::innerMakeMakeFile(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerMakeMakeFile");
  (void) input;
  List<std::string> cppFilesNoExtension;
  for (int i=0; i<theGlobalVariables.theSourceCodeFiles().size; i++)
  { std::string theFileNameWithPath=theGlobalVariables.theSourceCodeFiles()[i].FileName;
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
  FileOperations::OpenFileCreateIfNotPresentVirtual(theFileStream, "output/makefile", false, true, false);
  std::stringstream outHtml;
  theFileStream << "all: directories calculator\n\n";
  theFileStream << "directories: bin\n";
  theFileStream << "bin:\n" << "\tmkdir ./bin\n";
  theFileStream << "calculator: ";
  for (int i=0; i<cppFilesNoExtension.size; i++)
    theFileStream << cppFilesNoExtension[i] << ".o ";
  theFileStream << "\n\tg++ -std=c++11 -pthread ";
  for (int i=0; i<cppFilesNoExtension.size; i++)
    theFileStream << cppFilesNoExtension[i] << ".o ";
  theFileStream << "-o ./bin/calculator\n\n";
  for (int i=0; i<cppFilesNoExtension.size; i++)
    theFileStream << cppFilesNoExtension[i] << ".o: " << cppFilesNoExtension[i] << ".cpp\n\tg++ -std=c++0x -pthread -c " << cppFilesNoExtension[i] << ".cpp\n\n";
  outHtml << "<a href=\" " << theGlobalVariables.DisplayPathOutputFolder << "makefile" << "\"> makefile </a>";
  return output.AssignValue(outHtml.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerIntersection(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntersection");
  if (!input.StartsWith(theCommands.opIntersection()))
    return false;
  int numElts=1;
  for (int i=1; i<input.children.size; i++)
    if (!input[i].IsSequenceNElementS())
      return false;
    else
      numElts+=input[i].children.size-1;
  List<List<Expression> > theListsToBeIntersected;
  theListsToBeIntersected.SetSize(input.children.size-1);
  for (int i=1; i<input.children.size; i++)
  { theListsToBeIntersected[i-1].Reserve(input[i].children.size);
    for (int j=1; j<input[i].children.size; j++)
      theListsToBeIntersected[i-1].AddOnTop(input[i][j]);
  }
  if (theListsToBeIntersected.size==0)
    return false;
  List<Expression> outputList=theListsToBeIntersected[0];
  for (int i=1; i<theListsToBeIntersected.size; i++)
    outputList.IntersectWith(theListsToBeIntersected[i], outputList);
  return output.MakeSequence(theCommands, &outputList);
}


bool CalculatorFunctionsGeneral::innerUnion(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerUnion");
  if (!input.StartsWith(theCommands.opUnion()))
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

bool CalculatorFunctionsGeneral::innerUnionNoRepetition(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerUnionNoRepetition");
  if (!input.IsLisT())
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
  output.children.Reserve(numElts);
  output.reset(theCommands, theIndices.size+1);
  output.AddChildAtomOnTop(theCommands.opSequence());
  output.children.AddOnTop(theIndices);
  output.format=output.formatDefault;
  return true;
}

bool CalculatorFunctionsGeneral::innerCrossProduct(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCrossProduct");
  if (!input.IsListStartingWithAtom(theCommands.opCrossProduct()) || input.children.size!=3)
    return false;
  const Expression& leftE=input[1];
  const Expression& rightE=input[2];
  if (!leftE.IsSequenceNElementS(3) || !rightE.IsSequenceNElementS(3))
  { std::stringstream out;
    out << "Can't compute cross product of the non-3d vectors " << input[1].ToString() << " and "
    << input[2].ToString() << ". ";
    return output.MakeError(out.str(), theCommands);
  }
  List<Expression> outputSequence;
  outputSequence.SetSize(3);
  outputSequence[0]=leftE[2]*rightE[3]- leftE[3]*rightE[2];
  outputSequence[1]=leftE[3]*rightE[1]- leftE[1]*rightE[3];
  outputSequence[2]=leftE[1]*rightE[2]- leftE[2]*rightE[1];
  return output.MakeSequence(theCommands, &outputSequence);
}

bool CalculatorFunctionsGeneral::innerDifferentiateConstPower(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDifferentiateConstPower");
  //////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1];
  const Expression& theArgument=input[2];
  //////////////////////
  if (!theArgument.StartsWith(theCommands.opThePower(), 3))
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
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1];
  const Expression& theArgument=input[2];
  //////////////////////
  //d/dx a^b= d/dx(e^{b\\ln a}) = a^b d/dx(b\\log a)
  if (!theArgument.StartsWith(theCommands.opThePower(), 3))
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
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
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
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
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
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString() << " - possible user typo?";
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
  if (theArgument.IsAtomGivenData(theCommands.opTan()))
  { Expression secE, twoE;
    secE.MakeAtom(theCommands.opSec(), theCommands);
    twoE.AssignValue(2, theCommands);
    return output.MakeXOX(theCommands, theCommands.opThePower(), secE, twoE);
  }
  if (theArgument.IsAtomGivenData(theCommands.opCot()))
  { Expression cscE, twoE, cscSquared, mOneE;
    cscE.MakeAtom(theCommands.opCsc(), theCommands);
    twoE.AssignValue(2, theCommands);
    cscSquared.MakeXOX(theCommands, theCommands.opThePower(), cscE, twoE);
    mOneE.AssignValue(-1, theCommands);
    return output.MakeXOX(theCommands, theCommands.opTimes(), mOneE, cscSquared);
  }
  if (theArgument.IsAtomGivenData(theCommands.opSec()))
  { Expression tanE, secE;
    tanE.MakeAtom(theCommands.opTan(), theCommands);
    secE.MakeAtom(theCommands.opSec(), theCommands);
    return output.MakeXOX(theCommands, theCommands.opTimes(), tanE, secE);
  }
  if (theArgument.IsAtomGivenData(theCommands.opCsc()))
  { Expression cotE, cscE, mOneE, cotTimesCscE;
    cotE.MakeAtom(theCommands.opCot(), theCommands);
    cscE.MakeAtom(theCommands.opCsc(), theCommands);
    mOneE.AssignValue(-1, theCommands);
    cotTimesCscE.MakeXOX(theCommands, theCommands.opTimes(), cotE, cscE);
    return output.MakeXOX(theCommands, theCommands.opTimes(), mOneE, cotTimesCscE);
  }
  if (theArgument.IsAtomGivenData(theCommands.opArcTan()))
  { Polynomial<Rational> onePlusXsquared;
    RationalFunctionOld oneOverOnePlusXsquared;
    onePlusXsquared.MakeMonomiaL(0,2);
    onePlusXsquared+=1;
    oneOverOnePlusXsquared.MakeOne();
    oneOverOnePlusXsquared/=onePlusXsquared;
    Expression theContext;
    theContext.ContextMakeContextWithOnePolyVar(theCommands, "x");
    return output.AssignValueWithContext(oneOverOnePlusXsquared, theContext, theCommands);
  }
  if (theArgument.IsAtomGivenData(theCommands.opArcSin()))
  { Expression mhalfE, denE, theContext;
    theContext.ContextMakeContextWithOnePolyVar(theCommands, "x");
    RationalFunctionOld oneMinusXsquared;
    oneMinusXsquared.MakeMonomiaL(0,2);
    oneMinusXsquared*=-1;
    oneMinusXsquared+=1;
    mhalfE.AssignValue(Rational(-1,2), theCommands);
    denE.AssignValueWithContext(oneMinusXsquared, theContext, theCommands);
    return output.MakeXOX(theCommands, theCommands.opThePower(), denE, mhalfE);
  }
  return false;
}

bool CalculatorFunctionsGeneral::innerCompositeDifferentiateLog(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCompositeDifferentiateLog");
  /////////////////////
//  stOutput << "<hr>input composite: " << input.ToString();
  if (input.children.size!=2)
    return false;
  if (!input[0].StartsWith(theCommands.opDifferentiate(), 3))
    return false;
  if (!input[0][2].IsAtomGivenData(theCommands.opLog()))
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  Expression OneE;
  OneE.AssignValue(1, theCommands);
  output.reset(theCommands, 2);
  return output.MakeXOX(theCommands, theCommands.opDivide(), OneE, input[1]);
}

bool CalculatorFunctionsGeneral::outerDivideByNumber
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::outerDivide");
//  stOutput << "<br>Now I'm here 1! input: " << input.ToString() << " lisp: "
//  << input.Lispify() ;
  if (!input.StartsWith(theCommands.opDivide(), 3))
    return false;
  if (input[2].IsEqualToZero())
    return output.MakeError("Division by zero. ", theCommands);

//  stOutput << "<br>Now I'm here! 2";
  Rational theRatValue;
  AlgebraicNumber theAlgValue;
  double theDoubleValue;
  Expression theInvertedE;
  bool result=false;
  if (input[2].IsOfType<Rational>(&theRatValue))
  { theRatValue.Invert();
    theInvertedE.AssignValue(theRatValue, theCommands);
    result=true;
  }
  if (input[2].IsOfType<AlgebraicNumber>(&theAlgValue))
  { theAlgValue.Invert();
    theInvertedE.AssignValue(theAlgValue, theCommands);
    result=true;
  }
  if (input[2].IsOfType<double>(&theDoubleValue))
  { theDoubleValue=1/theDoubleValue;
    theInvertedE.AssignValue(theDoubleValue, theCommands);
    result=true;
  }
  if (!result)
    return false;
  output=theInvertedE*input[1];
  return true;
}

bool CalculatorFunctionsGeneral::outerEqualEqual(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElements(3))
    return false;
  const Expression& left=input[1];
  const Expression& right=input[2];
  if (left.HasBoundVariables() || right.HasBoundVariables())
    return false;
  if (left==right)
    return output.AssignValue(1, theCommands);
  if (left.IsEqualToMathematically(right))
    return output.AssignValue(1, theCommands);
  else
    return output.AssignValue(0, theCommands);
}

bool CalculatorFunctionsGeneral::outerAssociateAdivBdivCpowerD(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::outerAssociateAdivBdivCpowerD");
  if (!input.StartsWith(theCommands.opDivide(), 3))
    return false;
  if (!input[2].StartsWith(theCommands.opThePower(), 3))
    return false;
  if (!input[2][1].StartsWith(theCommands.opDivide(), 3))
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
  if (!input.StartsWith(theCommands.opDivide(), 3))
    return false;
  if (!input[1].StartsWith(theCommands.opDivide(), 3))
    return false;
  if (!input[2].StartsWith(theCommands.opDivide(), 3))
    return false;
  if (input[1][2]==input[2][2])
    return output.MakeXOX(theCommands, theCommands.opDivide(), input[1][1], input[2][1]);
  if (input[1][1]==input[2][1])
    return output.MakeXOX(theCommands, theCommands.opDivide(), input[2][2], input[1][2]);
  return false;
}

bool CalculatorFunctionsGeneral::outerAssociateDivisionDivision(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::outerAssociateDivisionDivision");
  if (!input.StartsWith(theCommands.opDivide(), 3))
    return false;
  if (input[1].StartsWith(theCommands.opDivide(), 3))
  { Expression newRightE;
    newRightE.MakeXOX(theCommands, theCommands.opTimes(), input[2], input[1][2]);
    return output.MakeXOX(theCommands, theCommands.opDivide(), input[1][1], newRightE);
  }
  if (input[2].StartsWith(theCommands.opDivide(), 3))
  { Expression newLeftE;
    newLeftE.MakeXOX(theCommands, theCommands.opTimes(), input[1], input[2][2]);
    return output.MakeXOX(theCommands, theCommands.opDivide(), newLeftE, input[2][1]);
  }
  return false;
}

bool CalculatorFunctionsGeneral::innerDifferentiateChainRule(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDifferentiateChainRule");
  /////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1], theArgument=input[2];
  //////////////////////
//  stOutput << "ere be chain rule! Argument be: " << theArgument.ToString() << " Argument[0] be: " << theArgument[0].ToString();
  if (!theArgument.StartsWith(-1, 2))
    return false;
//  stOutput << " continues to rule! The argument be: " << theArgument.ToString() << " theArgument.IsGoodForChainRuleFunction()= "
//  << theArgument.IsGoodForChainRuleFunction() << " theArgument[0].IsGoodForChainRuleFunction()="
//  << theArgument[0].IsGoodForChainRuleFunction() << "; ";
  if (!theArgument.IsGoodForChainRuleFunction() && !theArgument[0].IsGoodForChainRuleFunction())
    return false;
//  stOutput << " here be ruler #1!";
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
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString() << " - possible user typo?";
  const Expression& theDOvar=input[1], theArgument=input[2];
  //////////////////////
  if (!theArgument.StartsWith(theCommands.opPlus(), 3))
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
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1], theArgument=input[2];
  //////////////////////
  if (!theArgument.StartsWith(theCommands.opTimes(), 3))
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
  if (!input.StartsWith(theCommands.opThePower(), 3))
    return false;
  if (!input[2].IsEqualToZero())
    return false;
//  stOutput << "input[1]: "<< input[1].ToString() << ", input[2]: " << input[2].ToString();
  if (input[1].IsEqualToZero())
    return output.MakeError("Error: expression of the form 0^0 is illegal.", theCommands);
  return output.AssignValue<Rational>(1, theCommands);
}

bool CalculatorFunctionsGeneral::innerDifferentiateAdivideBCommutative(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDifferentiateAdivideBCommutative");
    //////////////////////
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression " << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1], theArgument=input[2];
  //////////////////////
  //Quotient rule (commutative): (a/b^n)'=(a'b-n a b')/b^{n+1}
  if (!theArgument.StartsWith(theCommands.opDivide(), 3))
    return false;
  const Expression& numeratorE=theArgument[1];
  const Expression& denominatorE=theArgument[2];
  if (numeratorE.StartsWith(theCommands.opPlus()))
  { Expression leftE(theCommands), rightE(theCommands);
    leftE.AddChildAtomOnTop(theCommands.opDifferentiate());
    leftE.AddChildOnTop(theDOvar);
    rightE=leftE;
    leftE.AddChildOnTop(numeratorE[1]/denominatorE);
    rightE.AddChildOnTop(numeratorE[2]/denominatorE);
    output=leftE+rightE;
    return true;
  }
  theCommands.CheckInputNotSameAsOutput(input, output);
  output.reset(theCommands);
  Expression theDenominatorBase, eOne, theDenominatorExponentPlusOne, theDenominatorExponent, changedMultiplicand,
  leftSummand, rightSummand, theDenominatorFinal, numerator;
  eOne.AssignValue(1, theCommands);
  bool denBaseFound=false;
  if (theArgument[2].StartsWith(theCommands.opThePower(), 3))
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
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression " << input[1].ToString()
    << " - possible user typo?";
  const Expression& theDOvar=input[1], theArgument=input[2];
  //////////////////////
  //Quotient rule (non-commutative): (a/b)'=(ab^{-1})'=a' b - a b^{-1} b' b^{-1}
  if (!theArgument.StartsWith(theCommands.opDivide(), 3))
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
  if (!input.StartsWith(theCommands.opTimes(), 3))
    return false;
  if (input[1].IsConstantNumber())
    return false;
  HashedListSpecialized<Expression> theList;
  input.GetBlocksOfCommutativity(theList);
  if (theList.size!=1)
    return false;
  if (input[2]>input[1] || input[2]==input[1])
    return false;
//  stOutput << "<hr>" << input[2].ToString() << " less than " << input[1].ToString();
//  stOutput << "ere be i, number 1!";
  output=input;
  output.children.SwapTwoIndices(1,2);
  return true;
}

bool CalculatorFunctionsGeneral::outerCommuteAtimesBtimesCifUnivariate(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::outerCommuteAtimesBtimesCifUnivariate");
  if (!input.StartsWith(theCommands.opTimes(),3))
    return false;
  const Expression& leftE=input[1];
  if (leftE.IsConstantNumber())
    return false;
  if (!input[2].StartsWith(theCommands.opTimes(),3))
    return false;
  const Expression& rightE=input[2][1];
  HashedListSpecialized<Expression> theList;

  leftE.GetBlocksOfCommutativity(theList);
  rightE.GetBlocksOfCommutativity(theList);
  if (theList.size!=1)
    return false;
  if (rightE>leftE || leftE==rightE)
    return false;
//  stOutput << "<hr>" << rightE.ToString() << " less than " << leftE.ToString();
  Expression leftMultiplicand;
  leftMultiplicand.MakeXOX(theCommands, theCommands.opTimes(), rightE, leftE);
//  stOutput << "Left multiplicand: " << leftE.ToString() << ", right: " << rightE.ToString() << " ";
  return output.MakeXOX(theCommands, theCommands.opTimes(), leftMultiplicand, input[2][2]);
}

bool CalculatorFunctionsGeneral::innerGetFreeVariablesIncludeNamedConstants
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGetFreeVariables");
  HashedList<Expression> outputList;
  if (!input.GetFreeVariables(outputList, false))
    return theCommands << "Function GetFreeVariables failed, this shouldn't happen.";
  return output.MakeSequence(theCommands, &outputList);
}

bool CalculatorFunctionsGeneral::innerGetFreeVariablesExcludeNamedConstants
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGetFreeVariables");
  HashedList<Expression> outputList;
  if (!input.GetFreeVariables(outputList, true))
    return theCommands << "Function GetFreeVariables failed, this shouldn't happen.";
  return output.MakeSequence(theCommands, &outputList);
}

bool CalculatorFunctionsGeneral::innerCompareFunctionsNumerically
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCompareFunctionsNumerically");
  if (input.size()<5)
    return theCommands << "Comparing functions takes as input at least 4 arguments (two functions to compare and interval to compare over).";
  Expression theFunE=input[1];
  theFunE-=input[2];
  //stOutput << "Finding free vars in: " << theFunE.ToString();
  HashedList<Expression> theVars;
  if (!theFunE.GetFreeVariables(theVars, true))
    return theCommands << "Was not able to extract the function argument of your function. " ;
  if (theVars.size<=0)
  { Expression zeroE;
    zeroE.AssignValue(0, theCommands);
    return output.MakeXOX(theCommands, theCommands.opEqualEqual(), theFunE, zeroE);
  }
  //stOutput << "The vars: " << theVars.ToStringCommaDelimited();
  if (theVars.size>1)
    return theCommands << "I cannot compare the functions as they appear to depend on more than one variable, namely, on: "
    << theVars.ToStringCommaDelimited();
  double leftBoundary=0;
  double rightBoundary=0;
  if (!input[3].EvaluatesToDouble(&leftBoundary))
    return theCommands << "Failed to extract the left endpoint of the comparison interval. ";
  if (!input[4].EvaluatesToDouble(&rightBoundary))
    return theCommands << "Failed to extract the right endpoint of the comparison interval. ";
  int numPoints=50;
  if (input.size()>5)
    if (!input[5].IsSmallInteger(&numPoints))
      return theCommands << "Failed to convert argument: " << input[5].ToString() << " to a small integer. ";
  double minDiff=0, maxDiff=0;
  if (!theFunE.EvaluatesToDoubleInRange(theVars[0].ToString(), leftBoundary, rightBoundary, numPoints, &minDiff, &maxDiff, 0))
    return theCommands << "Failed to evaluate your function to a number. The sampling interval may be outside of the domain of the function. ";
  double tolerance=0.0001;
  if (input.size()>6)
    if (!input[6].EvaluatesToDouble(&tolerance))
      return theCommands << "Failed to evaluate the argument " << input[6].ToString() << " to a floating point number. ";
  if (minDiff<- tolerance || maxDiff> tolerance)
    return output.AssignValue(0, theCommands);
  return output.AssignValue(1, theCommands);
//  theFunE.
}

bool CalculatorFunctionsGeneral::innerCompareExpressionsNumericallyAtPoints
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCompareExpressionsNumericallyAtPoints");
  if (input.children.size<5)
    return theCommands << "Comparing functions at points takes as input at least 5 arguments "
    << "- two functions to compare, precision, variable belonging to an interval and number of sampling points).";
  Expression theFunE=input[1];
  theFunE-=input[2];
  HashedList<Expression> theFreeVars;
  if (!theFunE.GetFreeVariables(theFreeVars, true))
    return theCommands << "Was not able to extract the function argument of your function. " ;
  if (theFreeVars.size<=0)
  { Expression zeroE;
    zeroE.AssignValue(0, theCommands);
    return output.MakeXOX(theCommands, theCommands.opEqualEqual(), theFunE, zeroE);
  }
  double tolerance=0.0001;
  if (!input[3].EvaluatesToDouble(&tolerance))
    return theCommands << "Failed to evaluate tolerance limit from " << input[3].ToString()
    << " to a floating point number. ";
  if (tolerance<0)
    tolerance*=-1;
  if (!input[4].StartsWith(theCommands.opIn(), 3))
    return theCommands << "The fourth argument " << input[4].ToString() << " needs to be "
    << " of the form (x,y,...)\\in (...). ";
  const Expression& theVarsE=input[4][1];
  HashedList<Expression> varsGiven;
  if (!theVarsE.IsSequenceNElementS())
    varsGiven.AddOnTop(theVarsE);
  else
    for (int i=1; i< theVarsE.children.size; i++)
    { if (varsGiven.Contains(theVarsE[i]))
        return theCommands << theVarsE[i] << " given more than once. ";
      varsGiven.AddOnTop(theVarsE[i]);
    }
  for (int i=0; i<theFreeVars.size; i++)
    if (!varsGiven.Contains(theFreeVars[i]))
      return theCommands << "The expression depends on  " << theFreeVars[i].ToString()
      << " but I no value given for that expression. ";
  const Expression& thePointsE=input[4][2];
  Matrix<double> thePoints;
  if (!theCommands.GetMatrix(thePointsE, thePoints, 0, varsGiven.size, CalculatorFunctionsGeneral::innerEvaluateToDouble))
    return theCommands << "Failed to extract list of points from: " << thePointsE.ToString();
  HashedList<Expression> knownEs= theCommands.knownDoubleConstants;
  List<double> knownValues=theCommands.knownDoubleConstantValues;
  for (int i=0; i<varsGiven.size; i++)
    if (knownEs.Contains(varsGiven[i]))
      return theCommands << varsGiven[i]
      << " is an already known constant and cannot be used as a variable in this context. ";
    else
      knownEs.AddOnTop(varsGiven[i]);
  knownValues.SetSize(knownEs.size);
  int numFailedSamples=0;
  int totalSamples=thePoints.NumRows;
/*  stOutput << "<br>Total number of samples: " << totalSamples;
  stOutput << "<br>Variables: " << theBoundaryVars.ToStringCommaDelimited();
  stOutput << "<br>Boundaries left: " << leftBoundaries.ToStringCommaDelimited();
  stOutput << "<br>Boundaries right: " << rightBoundaries.ToStringCommaDelimited();*/
  for (int i=0; i<thePoints.NumRows; i++)
  { for (int j=0; j<thePoints.NumCols; j++)
      knownValues[j+theCommands.knownDoubleConstants.size]= thePoints(i,j);
    double floatingResult=0;
    if (!theFunE.EvaluatesToDoubleUnderSubstitutions(knownEs, knownValues, &floatingResult))
    { numFailedSamples++;
      if ((numFailedSamples*100)/totalSamples>20)
      { theCommands << "Failed to evaluate at least one of the functions in more than 20% of the sampling points. ";
        return output.AssignValue(0, theCommands);
      }
    }
    if (floatingResult> tolerance || floatingResult <-tolerance)
      return output.AssignValue(0, theCommands);
  }
  return output.AssignValue(1, theCommands);
}

bool CalculatorFunctionsGeneral::innerCompareExpressionsNumerically
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCompareFunctionsNumerically");
  if (input.children.size<6)
    return theCommands << "Comparing functions takes as input at least 5 arguments "
    << "- two functions to compare, precision, variable belonging to an interval and number of sampling points).";
  Expression theFunE=input[1];
  theFunE-=input[2];
  HashedList<Expression> theVars;
  if (!theFunE.GetFreeVariables(theVars, true))
    return theCommands << "Was not able to extract the function argument of your function. " ;
  if (theVars.size<=0)
  { Expression zeroE;
    zeroE.AssignValue(0, theCommands);
    return output.MakeXOX(theCommands, theCommands.opEqualEqual(), theFunE, zeroE);
  }
//  stOutput << "The vars: " << theVars.ToStringCommaDelimited();
  List<double> leftBoundaries;
  List<double> rightBoundaries;
  HashedList<Expression> theBoundaryVars;
  List<int> numSamples;
  for (int i=4; i<input.children.size; i+=2)
  { const Expression& currentIntervalWithVariable=input[i];
    if (!currentIntervalWithVariable.StartsWith(theCommands.opIn(), 3))
      return theCommands << "Expression " << currentIntervalWithVariable.ToString()
      << " is not a belonging relation (does not use the \\in symbol). ";
    if (!currentIntervalWithVariable[2].IsSequenceNElementS(2))
      return theCommands << "Could not get a two-element sequence from " << currentIntervalWithVariable[2].ToString();
    double currentLeft, currentRight;
    if (!currentIntervalWithVariable[2][1].EvaluatesToDouble(&currentLeft))
      return theCommands << "Could not get a double from " << currentIntervalWithVariable[2][1].ToString();
    if (!currentIntervalWithVariable[2][2].EvaluatesToDouble(&currentRight))
      return theCommands << "Could not get a double from " << currentIntervalWithVariable[2][2].ToString();
    if (theBoundaryVars.Contains(currentIntervalWithVariable[1]))
      return theCommands << "Expression " << currentIntervalWithVariable[1].ToString()
      << " specified an interval range more than once. ";
    theBoundaryVars.AddOnTop(currentIntervalWithVariable[1]);
    leftBoundaries.AddOnTop(currentLeft);
    rightBoundaries.AddOnTop(currentRight);
    numSamples.AddOnTop(10);
    int currentNumSamplingPoints=0;
    if (i+1<input.children.size)
    { if (input[i+1].IsSmallInteger(&currentNumSamplingPoints))
        *numSamples.LastObject()=currentNumSamplingPoints;
      else
        return theCommands << "Failed to extract a small integer for the number of sampling points from: "
        << input[i+1].ToString();
      if (*numSamples.LastObject()>1000)
        return theCommands << *numSamples.LastObject() << " sampling points requested for variable/expression "
        << theBoundaryVars.LastObject()->ToString() << "; this exceeds the hard-coded limit of 1000. ";
      if (*numSamples.LastObject()<1)
        return theCommands << *numSamples.LastObject() << " sampling points requested for variable/expression "
        << theBoundaryVars.LastObject()->ToString() << "; this is not allowed. ";
    }
  }
  for (int i=0; i<theVars.size; i++)
    if (!theBoundaryVars.Contains(theVars[i]))
      return theCommands << "Could not find an interval for variable/expression: " << theVars[i].ToString();
  HashedList<Expression> knownEs= theCommands.knownDoubleConstants;
  List<double> knownValues=theCommands.knownDoubleConstantValues;
  for (int i=0; i<theBoundaryVars.size; i++)
    if (knownEs.Contains(theBoundaryVars[i]))
      return theCommands << theBoundaryVars[i]
      << " is an already known constant and cannot be used as a variable in this context. ";
    else
      knownEs.AddOnTop(theBoundaryVars[i]);
  knownValues.SetSize(knownEs.size);
  SelectionWithDifferentMaxMultiplicities theSamplingSelector;
  theSamplingSelector.initFromInts(numSamples);
  if (theSamplingSelector.TotalNumSubsets()>1000000)
    return theCommands << "The total number of sampling points, "
    << theSamplingSelector.TotalNumSubsets().ToString() << " exceeds "
    << "the hard-coded limit of 1000000. ";
  double tolerance=0.0001;
  if (!input[3].EvaluatesToDouble(&tolerance))
    return theCommands << "Failed to evaluate tolerance limit from " << input[3].ToString()
    << " to a floating point number. ";
  if (tolerance<0)
    tolerance*=-1;
  Rational totalSamples=theSamplingSelector.TotalNumSubsetsMustBeSmalInt();
  Rational numFailedSamples=0;
/*  stOutput << "<br>Total number of samples: " << totalSamples;
  stOutput << "<br>Variables: " << theBoundaryVars.ToStringCommaDelimited();
  stOutput << "<br>Boundaries left: " << leftBoundaries.ToStringCommaDelimited();
  stOutput << "<br>Boundaries right: " << rightBoundaries.ToStringCommaDelimited();*/
  do
  { for (int i=0; i<theSamplingSelector.Multiplicities.size; i++)
    { double& currentValue=knownValues[i+theCommands.knownDoubleConstants.size];
      double& lBound=leftBoundaries[i];
      double& rBound=rightBoundaries[i];
      if (theSamplingSelector.MaxMultiplicities[i]==1)
      { currentValue=(lBound+rBound)/2;
        continue;
      }
      double paramBnZeroAndOne=theSamplingSelector.Multiplicities[i];
      paramBnZeroAndOne/=theSamplingSelector.MaxMultiplicities[i]-1;
      currentValue=lBound*(1-paramBnZeroAndOne)+rBound*paramBnZeroAndOne;
    }
    double floatingResult=0;
    if (!theFunE.EvaluatesToDoubleUnderSubstitutions(knownEs, knownValues, &floatingResult))
    { numFailedSamples++;
      if ((numFailedSamples*100)/totalSamples>20)
      { theCommands << "Failed to evaluate at least one of the functions in more than 20% of the sampling points. ";
        return output.AssignValue(0, theCommands);
      }
/*    stOutput << "<br>Substitutions: " << knownEs.ToStringCommaDelimited()
    << ", values: " << knownValues.ToStringCommaDelimited()
    << ".  Output: <span style=\"color:red\">failed.</span> ";*/
    }
/*    stOutput << "<br>Substitutions: " << knownEs.ToStringCommaDelimited()
    << ", values: " << knownValues.ToStringCommaDelimited()
    << ".  Output: " << floatingResult;*/
    if (floatingResult> tolerance || floatingResult <-tolerance)
      return output.AssignValue(0, theCommands);
  } while(theSamplingSelector.IncrementReturnFalseIfPastLast());
  return output.AssignValue(1, theCommands);
//  theFunE.
}

bool CalculatorFunctionsGeneral::innerCompositeArithmeticOperationEvaluatedOnArgument
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCompositeArithmeticOperationEvaluatedOnArgument");
  if (input.children.size!=2)
    return false;
  if (input[0].children.size!=3)
    return false;
  if (!input[0][0].IsArithmeticOperation())
    return false;
  Expression leftE(theCommands);
  Expression rightE(theCommands);
  leftE.AddChildOnTop(input[0][1]);
  leftE.AddChildOnTop(input[1]);
  rightE.AddChildOnTop(input[0][2]);
  rightE.AddChildOnTop(input[1]);
  return output.MakeXOX(theCommands, input[0][0].theData, leftE, rightE);
}

bool CalculatorFunctionsGeneral::innerIsEven(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIsEven");
  if (input.HasBoundVariables())
    return false;
  LargeInt theInt;
  if (!input.IsInteger(&theInt))
    return output.AssignValue(0, theCommands);
  if (theInt.IsEven())
    return output.AssignValue(1, theCommands);
  return output.AssignValue(0, theCommands);
}

bool CalculatorFunctionsGeneral::innerIsConstant(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIsConstant");
  //stOutput << "<br>Evaluating isconstant on: " << input.ToString();
  int result=(int) input.IsConstantNumber();
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsGeneral::innerIsNonEmptySequence(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIsNonEmptySequence");
  //stOutput << "<br>Evaluating isconstant on: " << input.ToString();
  if (input.HasBoundVariables())
    return false;
  if (!input.IsSequenceNElementS() || input.children.size<2)
    return output.AssignValue(0, theCommands);
  return output.AssignValue(1, theCommands);
}

bool CalculatorFunctionsGeneral::innerIsDifferentialOneFormOneVariable(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIsDifferentialOneFormOneVariable");
  return output.AssignValue((int) input.IsDifferentialOneFormOneVariablE(), theCommands);
}

bool CalculatorFunctionsGeneral::innerInterpretAsDifferential
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerInterpretAsDifferential");
  if (!input.StartsWith(theCommands.opTimes(), 3))
    return false;
  if (!input[1].StartsWith(theCommands.opIntegral()))
    return false;
  const Expression& theDiffFormCandidateE=input[2];
  std::string theDiff;
  if (theDiffFormCandidateE.IsAtom(&theDiff))
    if (theDiff.size()>1)
      if (theDiff[0]=='d')
      { Expression variableE, diffFormE;
        variableE.MakeAtom(theDiff.substr(1, std::string::npos), theCommands);
        diffFormE.reset(theCommands);
        diffFormE.AddChildAtomOnTop(theCommands.opDifferential());
        diffFormE.AddChildOnTop(variableE);
        diffFormE.AddChildRationalOnTop(1);
        output=input;
        return output.SetChilD(2, diffFormE);
      }
  return false;
}

bool CalculatorFunctionsGeneral::innerIntegralOperator(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCompositeMultiplyIntegralFbyDx");
  //stOutput << "called innerCompositeMultiplyIntegralFbyDx with input: " << input.ToString();
  int theOp=theCommands.opTimes();
  if (!input.StartsWith(theOp, 3))
  { theOp=theCommands.opDivide();
    if (!input.StartsWith(theOp, 3))
      return false;
  }
  const Expression& integralE=input[1];
  if (integralE.IsAtomGivenData(theCommands.opIntegral()))
  { Expression integralOperatorE;
    integralOperatorE.reset(theCommands, 2);
    integralOperatorE.AddChildAtomOnTop(theCommands.opIntegral());
    integralOperatorE.AddChildAtomOnTop(theCommands.opIndefiniteIntegralIndicator());
    output=input;
    return output.SetChilD(1, integralOperatorE);
  }
  if (!integralE.StartsWith(theCommands.opIntegral()))
    return false;
  if (integralE.size()<3)
  { output=integralE;
    return output.AddChildOnTop(input[2]);
  }
  if (integralE.size()!=3)
    return false;
  const Expression& startingIntegrand=integralE[2];
  if (startingIntegrand.IsDifferentialOneFormOneVariablE())
    return false;
  const Expression& incomingIntegrand=input[2];
  Expression newIntegrand;
  Expression newFun;
  if (incomingIntegrand.IsDifferentialOneFormOneVariablE())
  { newFun.MakeXOX(theCommands, theOp, startingIntegrand, incomingIntegrand[2]);
    newIntegrand.MakeXOX
    (theCommands, theCommands.opDifferential(), incomingIntegrand[1], newFun);
    output=integralE;
    return output.SetChilD(2, newIntegrand);
  }
  newIntegrand.MakeXOX(theCommands, theOp, startingIntegrand,incomingIntegrand);
  output=integralE;
  return output.SetChilD(2, newIntegrand);
}

bool CalculatorFunctionsGeneral::innerRationalFunctionSubstitution(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerRationalFunctionSubstitution");
//stOutput << "input of innerRationalFunctionSubstitution: " << input.ToString();
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
  return CalculatorConversions::innerExpressionFromRF(theCommands, ResultRationalForm, output);
}

bool CalculatorFunctionsGeneral::innerInvertMatrixRFsVerbose(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerInvertMatrixVerbose");
  Matrix<RationalFunctionOld> mat, outputMat, tempMat;
  Expression theContext;
  if (!input.IsOfType<Matrix<RationalFunctionOld> >(& mat))
    return output.MakeError("Failed to extract matrix. ", theCommands);
  theContext=input.GetContext();
  if (mat.NumRows!=mat.NumCols || mat.NumCols<1)
  { std::stringstream out;
    out << "The matrix " << mat.ToString( ) << " has " << mat.NumCols << " columns and " << mat.NumRows << " rows. "
    << "The matrix is not square.";
    return output.MakeError(out.str(), theCommands);
  }
  outputMat.MakeIdMatrix(mat.NumRows);
  int tempI;
  int NumFoundPivots = 0;
  std::stringstream out, outLaTeX;

  RationalFunctionOld tempElement;
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
  theFormat.flagUseLatex=true;
  theFormat.flagUseHTML=false;
  theFormat.flagUseFrac=true;
  theFormat.MatrixColumnVerticalLineIndex=mat.NumCols-1;
  out << "Computing " << CGI::GetMathSpanPure(mat.ToString(&theFormat) + "^{-1}");
  tempMat=mat;
  tempMat.AppendMatrixOnTheRight(outputMat);
  out << "<br>" << CGI::GetMathSpanPure(tempMat.ToString(&theFormat)) ;
  outLaTeX << "\\begin{tabular}{ll}";
  outLaTeX << "$" << tempMat.ToString(& theFormat) << "$";

  for (int i=0; i<mat.NumCols; i++)
  { tempI = mat.FindPivot(i, NumFoundPivots);
    if (tempI!=-1)
    { if (tempI!=NumFoundPivots)
      { mat.SwitchTwoRows(NumFoundPivots, tempI);
        outputMat.SwitchTwoRows (NumFoundPivots, tempI);
        out << "<br>Swap row " << NumFoundPivots+1 << " and row " << tempI+1 << ": ";
        outLaTeX << "& Swap row " << NumFoundPivots+1 << " and row " << tempI+1 << ". ";
        tempMat=mat;
        tempMat.AppendMatrixOnTheRight(outputMat);
        out << "<br>" << CGI::GetMathSpanPure(outputMat.ToString(&theFormat));
        outLaTeX << "\\\\" << "$" << outputMat.ToString(&theFormat) << "$";
      }
      tempElement=mat.elements[NumFoundPivots][i];
      tempElement.Invert();
      if (tempElement!=1)
      { out << "<br> multiply row number " << NumFoundPivots+1 << " by "
        << tempElement.ToString(&theFormat) << ": ";
        outLaTeX << "& multiply row number " << NumFoundPivots+1 << " by $"
        << tempElement.ToString(&theFormat) << "$. \\\\";
      }
      mat.RowTimesScalar(NumFoundPivots, tempElement);
      outputMat.RowTimesScalar(NumFoundPivots, tempElement);
      if (tempElement!=1)
      { tempMat=mat;
        tempMat.AppendMatrixOnTheRight(outputMat);
        out << CGI::GetMathSpanPure(tempMat.ToString(&theFormat));
        outLaTeX << "$" << tempMat.ToString(&theFormat) << "$";
      }
      bool found = false;
      for (int j = 0; j<mat.NumRows; j++)
        if (j!=NumFoundPivots)
          if (!mat.elements[j][i].IsEqualToZero())
          { tempElement=(mat.elements[j][i]);
            tempElement.Minus();
            mat.AddTwoRows(NumFoundPivots, j, i, tempElement);
            outputMat.AddTwoRows(NumFoundPivots, j, 0, tempElement);
            if(!found)
            { out << "<br>";
              outLaTeX << "&";
              outLaTeX << "\\begin{tabular}{l}";
            } else
            { out << ", ";
              outLaTeX << ", ";
            }
            found =true;
            out << " Row index " << NumFoundPivots+1 << " times "
            << tempElement.ToString(&theFormat) << " added to row index " << j+1;
            outLaTeX << " Row index " << NumFoundPivots+1 << " times $"
            << tempElement.ToString(&theFormat) << "$ added to row index " << j+1 << "\\\\";
          }
      if (found)
      { out << ": <br> ";
        outLaTeX << "\\end{tabular}";
        outLaTeX << "\\\\";
        tempMat=mat;
        tempMat.AppendMatrixOnTheRight(outputMat);
        out << CGI::GetMathSpanPure(tempMat.ToString(&theFormat));
        outLaTeX << "$" << tempMat.ToString(&theFormat) << "$";
      }
      NumFoundPivots++;
    }
  }

  outLaTeX << "\\end{tabular}";
  theFormat.MatrixColumnVerticalLineIndex=-1;
  if (NumFoundPivots<mat.NumRows)
  { out << "<br>Matrix to the right of the vertical line not transformed to the identity matrix => starting matrix is not invertible. ";
    outLaTeX << "Matrix to the right of the vertical line not transformed to the identity matrix => starting matrix is not invertible. ";
  } else
  { out << "<br>The inverse of the starting matrix can be read off on the matrix to the left of the id matrix: "
    << CGI::GetMathSpanPure(outputMat.ToString(&theFormat));
    outLaTeX << " The inverse matrix can now be read off as the matrix to the left of the identity matrix: $"
    << outputMat.ToString(&theFormat) << "$";
  }
  out << "Output in LaTeX: <br><br>" << outLaTeX.str();
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerInvertMatrixVerbose(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerInvertMatrixVerbose");
  Matrix<Rational> mat, outputMat, tempMat;
  if (!theCommands.GetMatriXFromArguments<Rational>(input, mat, 0, -1, 0))
    return CalculatorFunctionsGeneral::innerInvertMatrixRFsVerbose(theCommands, input, output);
  if (mat.NumRows!=mat.NumCols || mat.NumCols<1)
    return output.MakeError("The matrix is not square", theCommands);
  outputMat.MakeIdMatrix(mat.NumRows);
  int tempI;
  int NumFoundPivots = 0;
  std::stringstream out;
  Rational tempElement;
  FormatExpressions theFormat;
  theFormat.flagUseLatex=true;
  theFormat.flagUseHTML=false;
  theFormat.MatrixColumnVerticalLineIndex=mat.NumCols-1;
  out << "Computing " << CGI::GetMathSpanPure(mat.ToString(&theFormat) + "^{-1}");
  tempMat=mat;
  tempMat.AppendMatrixOnTheRight(outputMat);
  out << "<br>" << CGI::GetMathSpanPure(tempMat.ToString(&theFormat)) ;
  for (int i=0; i<mat.NumCols; i++)
  { tempI = mat.FindPivot(i, NumFoundPivots);
    if (tempI!=-1)
    { if (tempI!=NumFoundPivots)
      { mat.SwitchTwoRows(NumFoundPivots, tempI);
        outputMat.SwitchTwoRows (NumFoundPivots, tempI);
        out << "<br>switch row " << NumFoundPivots+1 << " and row " << tempI+1 << ": ";
        tempMat=mat;
        tempMat.AppendMatrixOnTheRight(outputMat);
        out << "<br>" << CGI::GetMathSpanPure(outputMat.ToString(&theFormat));
      }
      tempElement=mat.elements[NumFoundPivots][i];
      tempElement.Invert();
      if (tempElement!=1)
        out << "<br> multiply row " << NumFoundPivots+1 << " by " << tempElement << ": ";
      mat.RowTimesScalar(NumFoundPivots, tempElement);
      outputMat.RowTimesScalar(NumFoundPivots, tempElement);
      if (tempElement!=1)
      { tempMat=mat;
        tempMat.AppendMatrixOnTheRight(outputMat);
        out << CGI::GetMathSpanPure(tempMat.ToString(&theFormat));
      }
      bool found = false;
      for (int j = 0; j<mat.NumRows; j++)
        if (j!=NumFoundPivots)
          if (!mat.elements[j][i].IsEqualToZero())
          { tempElement=(mat.elements[j][i]);
            tempElement.Minus();
            mat.AddTwoRows(NumFoundPivots, j, i, tempElement);
            outputMat.AddTwoRows(NumFoundPivots, j, 0, tempElement);
            if(!found)
              out << "<br>";
            else
              out << ", ";
            found =true;
            out << " Row index " << NumFoundPivots+1 << " times "
            << tempElement << " added to row index " << j+1;
          }
      if (found)
      { out << ": <br> ";
        tempMat=mat;
        tempMat.AppendMatrixOnTheRight(outputMat);
        out << CGI::GetMathSpanPure(tempMat.ToString(&theFormat));
      }
      NumFoundPivots++;
    }
  }
  if (NumFoundPivots<mat.NumRows)
    out << "<br>Matrix to the right of the vertical line not transformed to the identity matrix => starting matrix is not invertible. ";
  else
    out << "<br>The inverse of the starting matrix can be read off on the matrix to the left of the id matrix: "
    << CGI::GetMathSpanPure(output.ToString(&theFormat));
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerPolynomialRelations
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerGroebner");
 /* if (input.IsSequenceNElementS())
  { output=input;
    if (!useGr && !useRevLex)
      return output.SetChildAtomValue(0, "GroebnerLexUpperLimit");
    if (!useGr && useRevLex)
      return output.SetChildAtomValue(0, "GroebnerRevLexUpperLimit");
    if (useGr)
      return output.SetChildAtomValue(0, "GroebnerGrLexUpperLimit");
  }*/
  Vector<Polynomial<Rational> > inputVector;
  Expression theContext;
  if (input.children.size<3)
    return output.MakeError("Function takes at least two arguments. ", theCommands);
  const Expression& numComputationsE=input[1];
  Rational upperBound=0;
  if (!numComputationsE.IsOfType(&upperBound))
    return output.MakeError("Failed to convert the first argument of the expression to rational number.", theCommands);
  if (upperBound>1000000)
    return output.MakeError
    ("Error: your upper limit of polynomial operations exceeds 1000000, which is too large.\
     You may use negative or zero number give no computation bound, but please don't. ", theCommands);
//  int upperBoundComputations=(int) upperBound.GetDoubleValue();
  output.reset(theCommands);
  for (int i=1; i<input.children.size; i++)
    output.children.AddOnTop(input.children[i]);
  if (!theCommands.GetVectorFromFunctionArguments<Polynomial<Rational> >
      (output, inputVector, &theContext, -1, CalculatorConversions::innerPolynomial<Rational>))
    return output.MakeError("Failed to extract polynomial expressions", theCommands);
  Vector<Polynomial<Rational> > theRels, theGens;
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
  for (char i=0; i<26; i++)
  { char currentLetter='a'+i;
    std::string currentStr;
    currentStr= currentLetter;
    if (!theFormat.polyAlphabeT.Contains(currentStr))
      theFormat.polyAlphabeT.AddOnTop(currentStr);
  }
  if (!RationalFunctionOld::GetRelations(inputVector, theGens, theRels, theCommands.Comments))
    return theCommands << "Failed to extract relations. ";
  std::stringstream out;
  out << "Polynomials:";
  for (int i=0; i<theGens.size; i++)
    out << "<br>" << theGens[i].ToString(&theFormat) << "=" << inputVector[i].ToString(&theFormat);

  out << "<br>Relations: ";
  for (int i=0; i<theRels.size; i++)
  { theRels[i].ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
    out << "<br>" << theRels[i].ToString(&theFormat);
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::outerPolynomialize(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPolynomialize");
  Expression thePolyE;
  if (!CalculatorConversions::innerPolynomial<Rational>(theCommands, input, thePolyE))
    return false;
  if (!CalculatorConversions::innerExpressionFromBuiltInType(theCommands, thePolyE, output))
    return false;
  return true;
}

bool CalculatorFunctionsGeneral::innerIntegrateRationalFunctionSplitToBuidingBlocks(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntegrateRationalFunctionSplitToBuidingBlocks");
  Expression theFunctionE, theVariableE, integrationSetE;
  if (!input.IsIntegralfdx(&theVariableE, &theFunctionE, &integrationSetE))
    return false;
  //stOutput << "<br>Calling CalculatorFunctionsGeneral::innerIntegrateRationalFunctionSplitToBuidingBlocks, input: " << input.ToString();
  IntegralRFComputation theComputation(&theCommands);
  if(!CalculatorConversions::innerRationalFunction(theCommands, theFunctionE, theComputation.inpuTE))
    return theCommands << "<hr>Call of function CalculatorConversions::innerRationalFunction failed, input was: "
    << theFunctionE.ToString();
  if (!theComputation.inpuTE.IsOfType<RationalFunctionOld>())
    return theCommands << "<hr>CalculatorFunctionsGeneral::innerIntegrateRationalFunctionSplitToBuidingBlocks: failed to convert "
    << theFunctionE.ToString() << " to rational function. Attempt to converted expression yielded: " << theComputation.inpuTE.ToString();
  if (theComputation.inpuTE.GetNumContextVariables()>1)
    return theCommands << "<hr>I converted " << theFunctionE.ToString() << " to rational function, but it is of "
    << theComputation.inpuTE.GetNumContextVariables() << " variables. I have been taught to work with 1 variable only. "
    << "<br>The context of the rational function is: " << theComputation.inpuTE.GetContext().ToString();
  if (theComputation.inpuTE.GetNumContextVariables()==1)
    if (theComputation.inpuTE.GetContext().ContextGetContextVariable(0)!=theVariableE)
      return theCommands << "<hr>The univariate rational function was in variable " << theComputation.inpuTE.GetContext().ToString()
      << " but the variable of integration is " << theVariableE.ToString();
  theComputation.integrationSetE=integrationSetE;
  theComputation.theRF=theComputation.inpuTE.GetValue<RationalFunctionOld>();
  theComputation.theRF.GetDenominator(theComputation.theDen);
  theComputation.theRF.GetNumerator(theComputation.theNum);
//  stOutput << "<br>partial fraction decompo called, the den is: " << theComputation.theDen.ToString();
  if (theComputation.theDen.TotalDegree()<1)
    return false;
//  stOutput << "<br>calling integration... ";
  if (!theComputation.IntegrateRF())
    return theCommands << theComputation.printoutIntegration.str();
  //stOutput << "got before check consistency 2";
  theComputation.theIntegralSum.CheckConsistencyRecursively();
  //stOutput << "got before check consistency 3";
  //stOutput << "result of "
  output=theComputation.theIntegralSum;
  if (output.StartsWith(theCommands.opIntegral()))
    if (output[1]==input)
      return false;
  output.CheckConsistencyRecursively();
  output.CheckInitializationRecursively();
//  stOutput << "<hr>Transforming " << input.ToString() << " to " << output[1].ToString() << "<hr>";
  return true;
}

bool CalculatorFunctionsGeneral::innerCoefficientsPowersOf
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCoefficientsPowersOf");
  if (input.size()!=3)
    return false;
  const Expression& theVarE=input[1];
  const Expression& theExpressionE=input[2];
  VectorSparse<Expression> theCFs;
  if (!theCommands.CollectCoefficientsPowersVar(theExpressionE, theVarE, theCFs))
    return theCommands << "<hr>Failed to evaluate Calculator::CollectCoefficientsPowersVar";
  int highestPowerPlus1=theCFs.GetLargestParticipatingBasisIndex()+1;
  List<Expression> theCFsIncludingZeros;
  Expression currentCF;
  for (int i =0; i<highestPowerPlus1; i++)
  { int theIndex=theCFs.theMonomials.GetIndex(MonomialVector(i));
    if (theIndex==-1)
      currentCF.AssignValue(0, theCommands);
    else
      currentCF=theCFs.theCoeffs[theIndex];
//    stOutput << "<br>cf: " << theCFs.theCoeffs[theIndex].ToString();
    theCFsIncludingZeros.AddOnTop(currentCF);
  }
  return output.MakeSequence(theCommands, &theCFsIncludingZeros);
}

bool CalculatorFunctionsGeneral::innerIntegrateRationalFunctionBuidingBlockIa
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntegrateRationalFunctionBuidingBlockIa");
  Expression theFunctionE, theVariableE, integralE(theCommands);
  if (!input.IsIntegralfdx(&theVariableE, &theFunctionE))
    return false;
//  stOutput << "<br>Calling CalculatorFunctionsGeneral::innerIntegrateRationalFunctionBuidingBlockI, input: " << input.ToString();
  if (!theFunctionE.StartsWith(theCommands.opDivide(), 3))
    return false;
  const Expression& A=theFunctionE[1];
  Expression a,b;
  const Expression& axPlusb=theFunctionE[2];
  if (!A.IsConstantNumber())
    return false;
  if (!CalculatorFunctionsGeneral::extractLinearCoeffsWRTvariable(axPlusb, theVariableE, a, b))
    return false;
  if (!a.IsConstantNumber() || !b.IsConstantNumber())
    return false;
  Expression logaxPlusb;
  logaxPlusb.reset(theCommands);
  logaxPlusb.AddChildAtomOnTop(theCommands.opLog());
  logaxPlusb.AddChildOnTop(axPlusb);
  output=A;
  output/=a;
  output*=logaxPlusb;
  output.CheckConsistencyRecursively();
  output.CheckInitializationRecursively();
  return true;
}

bool CalculatorFunctionsGeneral::innerIntegrateRationalFunctionBuidingBlockIb(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntegrateRationalFunctionBuidingBlockIa");
  Expression theFunctionE, theVariableE, integralE(theCommands);
  if (!input.IsIntegralfdx(&theVariableE, &theFunctionE))
    return false;
//  stOutput << "<br>Calling CalculatorFunctionsGeneral::innerIntegrateRationalFunctionBuidingBlockI, input: " << input.ToString();
  if (!theFunctionE.StartsWith(theCommands.opDivide(), 3))
    return false;
  const Expression& A=theFunctionE[1];
  const Expression& axPlusbPowerN=theFunctionE[2];
  if (!A.IsConstantNumber())
    return false;
  if (!axPlusbPowerN.StartsWith(theCommands.opThePower(), 3))
    return false;
  Expression N= axPlusbPowerN[2];
  if (!N.IsConstantNumber())
    return false;
  Expression a,b;
  const Expression& axPlusb=axPlusbPowerN[1];
  if (!CalculatorFunctionsGeneral::extractLinearCoeffsWRTvariable(axPlusb, theVariableE, a, b))
    return false;
  if (!a.IsConstantNumber() || !b.IsConstantNumber())
    return false;
  Expression base, OneMinusN;
  OneMinusN=N;
  OneMinusN+=-1;
  OneMinusN*=-1;
  base.MakeXOX(theCommands, theCommands.opThePower(), axPlusb, OneMinusN);
  output=A;
  output/=a;
  output/=OneMinusN;
  output*=base;
  output.CheckConsistencyRecursively();
  output.CheckInitializationRecursively();
  return true;
}

bool CalculatorFunctionsGeneral::extractQuadraticCoeffsWRTvariable
  (const Expression& theQuadratic, const Expression& theVariable,
   Expression& outputCoeffVarSquared, Expression& outputCoeffLinTerm, Expression& outputConstTerm)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::extractQuadraticCoeffsWRTvariable");
  VectorSparse<Expression> theCoeffs;
  if (theQuadratic.owner==0)
    return false;
  Calculator& theCommands=*theQuadratic.owner;
  if(!theCommands.CollectCoefficientsPowersVar(theQuadratic, theVariable, theCoeffs))
    return false;
  if (theCoeffs.GetLargestParticipatingBasisIndex()!=2)
    return false;
  outputCoeffVarSquared.AssignValue(0, theCommands);
  outputCoeffLinTerm.AssignValue(0, theCommands);
  outputConstTerm.AssignValue(0, theCommands);
  if (theCoeffs.theMonomials.Contains(MonomialVector(0)))
    outputConstTerm=theCoeffs.GetMonomialCoefficient(MonomialVector(0));
  if (theCoeffs.theMonomials.Contains(MonomialVector(1)))
    outputCoeffLinTerm=theCoeffs.GetMonomialCoefficient(MonomialVector(1));
  if (theCoeffs.theMonomials.Contains(MonomialVector(2)))
    outputCoeffVarSquared=theCoeffs.GetMonomialCoefficient(MonomialVector(2));
  return true;
}

bool CalculatorFunctionsGeneral::extractLinearCoeffsWRTvariable
(const Expression& theLinearExpression, const Expression& theVariable,
  Expression& outputCoeffLinTerm, Expression& outputConstTerm)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::extractLinearCoeffsWRTvariable");
  VectorSparse<Expression> theCoeffs;
  //stOutput << "extracting lin expression from : " << theLinearExpression.ToString();
  if (theLinearExpression.owner==0)
    return false;
  Calculator& theCommands=*theLinearExpression.owner;
  theCommands.CollectCoefficientsPowersVar(theLinearExpression, theVariable, theCoeffs);
  if (theCoeffs.GetLargestParticipatingBasisIndex()>1)
    return false;
  outputCoeffLinTerm.AssignValue(0, theCommands);
  outputConstTerm.AssignValue(0, theCommands);
  if (theCoeffs.theMonomials.Contains(MonomialVector(1)))
    outputCoeffLinTerm=theCoeffs.GetMonomialCoefficient(MonomialVector(1));
  if (theCoeffs.theMonomials.Contains(MonomialVector(0)))
    outputConstTerm=theCoeffs.GetMonomialCoefficient(MonomialVector(0));
  return true;
}

bool CalculatorFunctionsGeneral::innerIntegrateRationalFunctionBuidingBlockIIaandIIIa(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntegrateRationalFunctionBuidingBlockIIaandIIIa");
  Expression theFunctionE, x, integralE(theCommands);
  if (!input.IsIntegralfdx(&x, &theFunctionE))
    return false;
//  stOutput << "<br>Calling CalculatorFunctionsGeneral::innerIntegrateRationalFunctionBuidingBlockIIaandIIIa, input: " << input.ToString();
  if (!theFunctionE.StartsWith(theCommands.opDivide(), 3))
    return false;
  Expression denNoPower=theFunctionE[2];
  std::stringstream out;
  Expression a, b, c, A, B;
  if (!CalculatorFunctionsGeneral::extractQuadraticCoeffsWRTvariable(denNoPower, x, a, b, c))
  { //stOutput << "<br>couldn't extract quadratic from denominator.";
    return false;
  }
  if (!CalculatorFunctionsGeneral::extractLinearCoeffsWRTvariable(theFunctionE[1], x, A, B))
  { //stOutput << "<br>couldn't extract linear expression from numerator.";
    return false;
  }
  if (!a.IsConstantNumber() || !b.IsConstantNumber() || !c.IsConstantNumber()||
      !A.IsConstantNumber() || !B.IsConstantNumber())
    return theCommands << "<hr>Failed to evaluate to constant the coefficients of the block IIa and IIIa integral."
    << "The coefficients are: " << a.ToString() << ", " << b.ToString() << ", " << c.ToString() << ", "
    << A.ToString() << ", " << B.ToString() << ". ";
  double approxa, approxb, approxc;
  if (!a.EvaluatesToDouble(&approxa) || !b.EvaluatesToDouble(&approxb) || !c.EvaluatesToDouble(&approxc))
    return theCommands << "<hr>Failed to evaluate variable coefficients in denominator " << denNoPower.ToString()
    << " to double. Possible user typo. ";
  if (approxb*approxb>=approxa* approxc*4)
    return false;
  Expression xSquared, bSquared, aSquared;
  Expression twoE, oneE, fourE;
  oneE.AssignValue(1, theCommands);
  twoE.AssignValue(2, theCommands);
  fourE.AssignValue(4, theCommands);
  xSquared.MakeXOX(theCommands, theCommands.opThePower(), x, twoE);
  bSquared.MakeXOX(theCommands, theCommands.opThePower(), b, twoE);
  aSquared.MakeXOX(theCommands, theCommands.opThePower(), a, twoE);

  Expression theQuadraticDiva=xSquared+(b/a)*x+c/a;
  Expression xplusbdiv2a = x+b/(twoE*a);
  Expression D=(fourE*a*c-bSquared)/(fourE*aSquared);
  Expression sqrtD;
  sqrtD.MakeSqrt(theCommands, D, 2);
  Expression arcTanArgument= xplusbdiv2a/sqrtD;
  Expression theArcTan(theCommands);
  theArcTan.AddChildAtomOnTop(theCommands.opArcTan());
  theArcTan.AddChildOnTop(arcTanArgument);
  Expression theLog(theCommands);
  theLog.AddChildAtomOnTop(theCommands.opLog());
  theLog.AddChildOnTop(theQuadraticDiva);
  Expression C=B-(A*b)/(twoE*a);
  output= (oneE/a)*((A/twoE )*theLog+(C/sqrtD)*theArcTan);
  output.CheckConsistencyRecursively();
  output.CheckInitializationRecursively();
  return true;
}

bool CalculatorFunctionsGeneral::innerIntegrateRationalFunctionBuidingBlockIIIb(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntegrateRationalFunctionBuidingBlockIIIb");
  Expression theFunctionE, x, integrationSetE;
  if (!input.IsIntegralfdx(&x, &theFunctionE, &integrationSetE))
    return false;
  //stOutput << "<br>Calling CalculatorFunctionsGeneral::innerIntegrateRationalFunctionBuidingBlockI, input: " << input.ToString();
  if (!theFunctionE.StartsWith(theCommands.opDivide(), 3))
    return false;
  if (!theFunctionE[2].StartsWith(theCommands.opThePower(), 3))
    return false;
  Expression numPowerE=theFunctionE[2][2];
  int numPower=0;
  if (!numPowerE.IsSmallInteger(&numPower))
    return false;
  if (numPower<=1)
    return false;
  Expression denNoPower=theFunctionE[2][1];
  std::stringstream out;
  Expression a, b, c;
  if (!theFunctionE[1].IsEqualToOne())
    return false;
  if (!CalculatorFunctionsGeneral::extractQuadraticCoeffsWRTvariable(denNoPower, x, a, b, c))
    return false;
  if (!a.IsEqualToOne() || !b.IsConstantNumber() || !c.IsConstantNumber())
  { theCommands << "<hr>Failed to evaluate to constant the coefficients of the block IIIb integral."
    << "The coefficients are: " << a.ToString() << ", " << b.ToString() << ", " << c.ToString() << ". ";
    return false;
  }
  double approxa, approxb, approxc;
  if (!a.EvaluatesToDouble(&approxa) || !b.EvaluatesToDouble(&approxb) || !c.EvaluatesToDouble(&approxc))
  { theCommands << "Failed to evaluate variable coefficients in denominator " << denNoPower.ToString()
    << " to double. Possible user typo. ";
    return false;
  }
  if (approxb*approxb>=approxa* approxc*4)
    return false;
  Expression xSquared, bSquared;
  Expression twoE, oneE, threeE, fourE;
  oneE.AssignValue(1, theCommands);
  twoE.AssignValue(2, theCommands);
  threeE.AssignValue(3, theCommands);
  fourE.AssignValue(4, theCommands);

  xSquared.MakeXOX(theCommands, theCommands.opThePower(), x, twoE);
  bSquared.MakeXOX(theCommands, theCommands.opThePower(), b, twoE);

  Expression theMonicQuadratic=xSquared+b*x+c;
  Expression D=c-bSquared/(fourE);
  Expression remainingIntegral, functionRemainingToIntegrate, theQuadraticPowerOneMinusN, theQuadraticPowerNMinusOne;
  theQuadraticPowerOneMinusN.MakeXOX(theCommands, theCommands.opThePower(), theMonicQuadratic, oneE-numPowerE);
  theQuadraticPowerNMinusOne.MakeXOX(theCommands, theCommands.opThePower(), theMonicQuadratic, numPowerE-oneE);
  functionRemainingToIntegrate=oneE/theQuadraticPowerNMinusOne;
  remainingIntegral.MakeIntegral(theCommands, integrationSetE, functionRemainingToIntegrate, x);
  output =oneE/D *
  ((x+b/twoE)/(twoE*numPowerE-twoE) * theQuadraticPowerOneMinusN+
  (twoE*numPowerE-threeE)/(twoE*numPowerE-twoE)*remainingIntegral);
  output.CheckConsistencyRecursively();
  output.CheckInitializationRecursively();
  //stOutput << " <hr>innerIntegrateRationalFunctionBuidingBlockIIIb: replacing " << CGI::GetMathSpanPure(input.ToString() ) << " by "
  //<< CGI::GetMathSpanPure(output.ToString());
  return true;
}

bool CalculatorFunctionsGeneral::innerIntegrateRationalFunctionBuidingBlockIIb(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntegrateRationalFunctionBuidingBlockIIb");
  Expression theFunctionE, x, integrationSetE;
  if (!input.IsIntegralfdx(&x, &theFunctionE, &integrationSetE))
    return false;
//  stOutput << "<br>Calling CalculatorFunctionsGeneral::innerIntegrateRationalFunctionBuidingBlockI, input: " << input.ToString();
  if (!theFunctionE.StartsWith(theCommands.opDivide(), 3))
    return false;
  if (!theFunctionE[2].StartsWith(theCommands.opThePower(), 3))
    return false;
  Expression nE=theFunctionE[2][2];
  int numPower=0;
  if (!nE.IsSmallInteger(&numPower))
    return false;
  if (numPower<=1)
    return false;
  Expression denNoPower=theFunctionE[2][1];
  std::stringstream out;
  Expression a, b, c, A, B;
  if (!CalculatorFunctionsGeneral::extractQuadraticCoeffsWRTvariable(denNoPower, x, a, b, c))
    return false;
  if (!CalculatorFunctionsGeneral::extractLinearCoeffsWRTvariable(theFunctionE[1], x, A, B))
    return false;
  if (A.IsEqualToZero())//building block is of type IIIb
    return false;
  if (!a.IsConstantNumber() || !b.IsConstantNumber() || !c.IsConstantNumber()||
      !A.IsConstantNumber() || !B.IsConstantNumber())
  { theCommands << "<hr>Failed to evaluate to constant the coefficients of the block IIb integral."
    << "The coefficients are: " << a.ToString() << ", " << b.ToString() << ", " << c.ToString() << ", "
    << A.ToString() << ", " << B.ToString() << ". ";
    return false;
  }
  double approxa, approxb, approxc;
  if (!a.EvaluatesToDouble(&approxa) || !b.EvaluatesToDouble(&approxb) || !c.EvaluatesToDouble(&approxc))
  { theCommands << "Failed to evaluate variable coefficients in denominator " << denNoPower.ToString()
    << " to double. Possible user typo. ";
    return false;
  }
  if (approxb*approxb>=approxa* approxc*4)
    return false;
  Expression xSquared, bSquared, aSquared, apowerN;
  Expression twoE, oneE, fourE;
  Expression remainingIntegral, remainingFunctionToIntegrate, quadraticPowerN, quadraticPowerOneMinusN;
  oneE.AssignValue(1, theCommands);
  twoE.AssignValue(2, theCommands);
  fourE.AssignValue(4, theCommands);
  apowerN.MakeXOX(theCommands, theCommands.opThePower(), a, nE);
  xSquared.MakeXOX(theCommands, theCommands.opThePower(), x, twoE);
  bSquared.MakeXOX(theCommands, theCommands.opThePower(), b, twoE);
  aSquared.MakeXOX(theCommands, theCommands.opThePower(), a, twoE);
  Expression theQuadraticDiva=xSquared+(b/a)*x+c/a;
  quadraticPowerN.MakeXOX(theCommands, theCommands.opThePower(), theQuadraticDiva, nE);
  quadraticPowerOneMinusN.MakeXOX(theCommands, theCommands.opThePower(), theQuadraticDiva, oneE-nE);
  remainingFunctionToIntegrate=oneE/quadraticPowerN;
  remainingIntegral.MakeIntegral(theCommands, integrationSetE, remainingFunctionToIntegrate, x);
  Expression xplusbdiv2a = x+b/(twoE*a);
  Expression D=(fourE*a*c-bSquared)/(fourE*aSquared);
  Expression C=B-(A*b)/(twoE*a);
  oneE.CheckInitializationRecursively();
  apowerN.CheckInitializationRecursively();
  A.CheckInitializationRecursively();
  twoE.CheckInitializationRecursively();
  nE.CheckInitializationRecursively();
//  stOutput << "quadratic power one minus n: " << quadraticPowerOneMinusN.ToString();
  quadraticPowerOneMinusN.CheckInitializationRecursively();
  C.CheckInitializationRecursively();
  remainingIntegral.CheckInitializationRecursively();

  output=(oneE/ apowerN)*(A/(twoE*(oneE-nE))*quadraticPowerOneMinusN +C*remainingIntegral);

  output.CheckConsistencyRecursively();
  output.CheckInitializationRecursively();
  //stOutput << " <hr>innerIntegrateRationalFunctionBuidingBlockIIb: replacing " << input.ToString() << " by " << output.ToString();
  return true;
}

bool CalculatorFunctionsGeneral::innerIntegratePowerByUncoveringParenthesisFirst(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntegratePowerByUncoveringParenthesisFirst");
  Expression theFunctionE, integrandE, newIntegralE, theVariableE, integrationSetE;
  if (!input.IsIntegralfdx(&theVariableE, &theFunctionE, &integrationSetE))
    return false;
//  stOutput << "<br>innerIntegratePowerByUncoveringParenthesisFirst: Integrating function " << theFunctionE.ToString();
  if (!theFunctionE.StartsWith(theCommands.opThePower()))
    return false;
  if (!CalculatorFunctionsGeneral::outerPolynomialize(theCommands, theFunctionE, integrandE))
    return false;
//  stOutput << "<br>integrand polynomialized: " << integrandE.ToString();
  if (integrandE==theFunctionE)
    return false;
  newIntegralE.MakeIntegral(theCommands, integrationSetE, integrandE, theVariableE);
  if (!theCommands.EvaluateExpression(theCommands, newIntegralE, output))
    return false;
  if (output.ContainsAsSubExpression(theCommands.opIntegral()))
    return false;
  output.CheckConsistencyRecursively();
  output.CheckInitializationRecursively();
  return true;
}

bool Expression::MakeIntegral
(Calculator& theCommands, const Expression& integrationSet, const Expression& theFunction,
 const Expression& theVariable)
{ MacroRegisterFunctionWithName("Expression::MakeIntegral");
  if (this==&theFunction || this==&theVariable || this==&integrationSet)
  { Expression theFunCopy=theFunction;
    Expression theVarCopy=theVariable;
    Expression integrationSetCopy=integrationSet;
    return this->MakeIntegral(theCommands, integrationSetCopy, theFunCopy, theVarCopy);
  }
  Expression theDiffForm;
  theDiffForm.MakeXOX(theCommands, theCommands.opDifferential(), theVariable, theFunction);
  this->reset(theCommands);
  this->AddChildAtomOnTop(theCommands.opIntegral());
  this->AddChildOnTop(integrationSet);
  return this->AddChildOnTop(theDiffForm);
}

bool CalculatorFunctionsGeneral::innerIntegrateSum(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntegrateSum");
  Expression theFunctionE, theVariableE, integrationSetE;
  if (!input.IsIntegralfdx(&theVariableE, &theFunctionE, &integrationSetE))
    return false;
//  stOutput << "<br>innerIntegrateSum: Integrating function " << theFunctionE.ToString();
  if (!theFunctionE.StartsWith(theCommands.opPlus()))
    return false;
  List<Expression> integralsOfSummands;
  integralsOfSummands.SetSize(theFunctionE.children.size-1);
  Expression newIntegralE, result, newSummand;
  for (int i=1; i<theFunctionE.children.size; i++)
  { newIntegralE.MakeIntegral(theCommands, integrationSetE, theFunctionE[i], theVariableE);
//    stOutput << "New integral: " << newIntegralE.ToString();
    if (!theCommands.EvaluateExpression(theCommands, newIntegralE, newSummand))
      return false;
    if (newSummand.ContainsAsSubExpression(theCommands.opIntegral()))
      return false;
    if (i==1)
      result=newSummand;
    else
      result+=newSummand;
  }
  output=result;
  output.CheckConsistencyRecursively();
  output.CheckInitializationRecursively();
  return true;
}

bool CalculatorFunctionsGeneral::innerIntegrateXnDiffX(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntegrateXnDiffX");
  Expression theFunctionE, theVariableE;
  if (!input.IsIntegralfdx(&theVariableE, &theFunctionE))
    return false;
//  stOutput << "<br>innerIntegrateXnDiffX: Integrating function " << theFunctionE.ToString();
  Expression theFunCoeff, theFunNoCoeff, outputNoCoeff;
  if (theFunctionE.IsConstantNumber())
  { output=theFunctionE*theVariableE;
    return true;
  }
  theFunctionE.GetCoefficientMultiplicandForm(theFunCoeff, theFunNoCoeff);
  if (theFunNoCoeff==theVariableE)
  { output=theFunCoeff*theVariableE*theVariableE;
//    stOutput << "<br>output: " << output.ToString();
    output/=2;
//    stOutput << "<br>output: " << output.ToString();
    return true;
  }
  if (!theFunNoCoeff.StartsWith(theCommands.opThePower(), 3))
    return false;
  if (theFunNoCoeff[1]!=theVariableE)
    return false;
  if (!theFunNoCoeff[2].IsConstantNumber())
    return false;
  if (theFunNoCoeff[2].IsEqualToMOne())
  { outputNoCoeff.reset(theCommands, 2);
    outputNoCoeff.AddChildAtomOnTop(theCommands.opLog());
    outputNoCoeff.AddChildOnTop(theVariableE);
    output=theFunCoeff*outputNoCoeff;
    return true;
  }
  Expression outputPower=theFunNoCoeff[2];
  outputPower+=1;
  outputNoCoeff.MakeXOX(theCommands, theCommands.opThePower(), theVariableE, outputPower);
  outputNoCoeff/=outputPower;
  output=theFunCoeff*outputNoCoeff;
  output.CheckConsistency();
  output.CheckInitializationRecursively();
  return true;
}

bool CalculatorFunctionsGeneral::innerDifferentiateSqrt(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDifferentiateSqrt");
  if (input.children.size!=3)
    return false;
  if (!input[1].IsAtom())
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].ToString() << " - possible user typo?";
  const Expression& theArgument=input[2];
  if (!theArgument.IsAtomGivenData(theCommands.opSqrt()))
    return false;
  Expression twoE(theCommands);
  Expression oneOverSqrtE(theCommands);
  twoE= Rational(1,2);
  oneOverSqrtE=1;
  oneOverSqrtE/=theArgument;
  return output.MakeXOX(theCommands, theCommands.opTimes(), twoE, oneOverSqrtE);
}

bool CalculatorFunctionsGeneral::outerDifferentiateWRTxTimesAny(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::outerDifferentiateWRTxTimesAny");
  if (!input.StartsWith(theCommands.opTimes(), 3))
    return false;
  if (!input[1].StartsWith(theCommands.opDifferentiate(), 2))
    return false;
  if (input[2].IsBuiltInAtom())
    return false;
  theCommands.CheckInputNotSameAsOutput(input, output);
  output=input[1];
  return output.AddChildOnTop(input[2]);
}

bool CalculatorFunctionsGeneral::innerDiffdivDiffxToDifferentiation(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDiffdivDiffxToDifferentiation");
  if (!input.StartsWith(theCommands.opDivide(), 3))
    return false;
  if (input[1]!="\\diff")
    return false;
  if (input[2].size()!=2)
    return false;
  if (input[2][0]!="\\diff")
    return false;
  output.reset(theCommands, 2);
  output.AddChildAtomOnTop(theCommands.opDifferentiate());
  return output.AddChildOnTop(input[2][1]);
}

bool CalculatorFunctionsGeneral::innerDdivDxToDiffDivDiffx(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDdivDxToDifferentiation");
  if (!input.StartsWith(theCommands.opDivide(), 3))
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
  Expression numeratorE, denominatorE(theCommands), rightDenE;
  numeratorE.MakeAtom(theCommands.opDifferential(), theCommands);
  rightDenE.MakeAtom(theCommands.AddOperationNoRepetitionOrReturnIndexFirst(denominatorString), theCommands);
  denominatorE.AddChildOnTop(numeratorE);
  denominatorE.AddChildOnTop(rightDenE);
  return output.MakeXOX(theCommands, theCommands.opDivide(), numeratorE, denominatorE);
}

bool CalculatorFunctionsGeneral::outerMergeConstantRadicals(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::outerCommuteConstants");
  if (!input.StartsWith(theCommands.opTimes(), 3))
    return false;
  if (!input[1].StartsWith(theCommands.opThePower(), 3) ||
      !input[2].StartsWith(theCommands.opThePower(), 3))
    return false;
  if (input[1][2]!=input[2][2])
    return false;
  if (!input[1][1].IsConstantNumber() || !input[2][1].IsConstantNumber())
    return false;
  Expression theProduct;
  theProduct.MakeProducT(theCommands, input[1][2], input[2][1]);
  return output.MakeXOX(theCommands, theCommands.opThePower(), theProduct, input[1][2]);
}

bool CalculatorFunctionsGeneral::outerCommuteConstants(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::outerCommuteConstants");
  if (!input.StartsWith(theCommands.opTimes(), 3))
    return false;
  if (!input[2].IsConstantNumber())
    return false;
  if (input[1].IsConstantNumber())
    return false;
  output.MakeProducT(theCommands, input[2], input[1]);
  output.CheckInitializationRecursively();
  return true;
}

bool CalculatorFunctionsGeneral::outerDivideReplaceAdivBpowerItimesBpowerJ(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::outerDivideReplaceAdivBpowerItimesBpowerJ");
  if (!input.StartsWith(theCommands.opTimes(), 3))
    return false;
  if (!input[1].StartsWith(theCommands.opDivide(),3))
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
  this->owner->AppendOpandsReturnTrueIfOrderNonCanonical(*this, theMultiplicandsLeft, this->owner->opTimes());
  if (theMultiplicandsLeft.size<=numDesiredMultiplicandsLeft)
    return false;
  theMultiplicandsRight.SetExpectedSize(theMultiplicandsLeft.size-numDesiredMultiplicandsLeft);
  for (int i=numDesiredMultiplicandsLeft; i<theMultiplicandsLeft.size; i++)
    theMultiplicandsRight.AddOnTop(theMultiplicandsLeft[i]);
  theMultiplicandsLeft.SetSize(numDesiredMultiplicandsLeft);
  outputLeftMultiplicand.MakeOXdotsX(*this->owner, this->owner->opTimes(), theMultiplicandsLeft);
  return outputRightMultiplicand.MakeOXdotsX(*this->owner, this->owner->opTimes(), theMultiplicandsRight);
}

bool CalculatorFunctionsGeneral::outerAtimesBpowerJplusEtcDivBpowerI(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::outerAtimesBpowerJplusEtcDivBpowerI");
  if (!input.StartsWith(theCommands.opDivide(),3))
    return false;
//  stOutput << "ere be i!";
  Expression denominatorBase, denominatorExponent;
  input[2].GetBaseExponentForm(denominatorBase, denominatorExponent);
  //if (!denominatorBase.IsAtom())
  //  return false;
  if (!denominatorBase.DivisionByMeShouldBeWrittenInExponentForm())
    return false;
  MonomialCollection<Expression, Rational> numerators, numeratorsNew;
  theCommands.CollectSummands(theCommands, input[1], numerators);
//  stOutput << "Collected numerators: " << numerators.ToString();
  numeratorsNew.SetExpectedSize(numerators.size());
  numeratorsNew.MakeZero();
  Expression numeratorMultiplicandLeft, numeratorMultiplicandRight, numeratorBaseRight, numeratorExponentRight;
  Expression newNumSummand, newNumSummandRightPart, newNumExponent, mOneE;
  mOneE.AssignValue(-1, theCommands);
  for (int i=0; i<numerators.size(); i++)
  { if (numerators[i].IsConstantNumber())
    { newNumSummandRightPart.MakeXOX(theCommands, theCommands.opThePower(), denominatorBase, mOneE* denominatorExponent);
      newNumSummand= numerators[i]*newNumSummandRightPart;
//      stOutput << "<br>Adding monomial: " << newNumSummand.ToString() << " with cf: "
//      << numerators.theCoeffs[i].ToString();
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

bool CalculatorFunctionsGeneral::innerGrowDynkinType(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGrowDynkinType");
  if (input.children.size!=3)
    return false;
  const Expression& theSmallerTypeE=input[1];
  DynkinType theSmallDynkinType;
  if (!CalculatorConversions::innerDynkinType(theCommands, theSmallerTypeE, theSmallDynkinType))
    return false;
  SemisimpleLieAlgebra* theSSalg=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, input[2], theSSalg))
    return output.MakeError("Error extracting ambient Lie algebra.", theCommands);
  SemisimpleSubalgebras tempSas;
  tempSas.initHookUpPointers
  (*theSSalg, &theCommands.theObjectContainer.theAlgebraicClosure,
   &theCommands.theObjectContainer.theSSLieAlgebras,
   &theCommands.theObjectContainer.theSltwoSAs);
  tempSas.ComputeSl2sInitOrbitsForComputationOnDemand();
  if (!tempSas.RanksAndIndicesFit(theSmallDynkinType))
    return output.MakeError("Error: type "+theSmallDynkinType.ToString()+" does not fit inside "+theSSalg->theWeyl.theDynkinType.ToString(), theCommands);
  List<DynkinType> largerTypes;
  List<List<int> > imagesSimpleRoots;
  if (!tempSas.GrowDynkinType(theSmallDynkinType, largerTypes, &imagesSimpleRoots))
    return output.MakeError("Error: growing type "+theSmallDynkinType.ToString()+" inside "+theSSalg->theWeyl.theDynkinType.ToString() + " failed. ", theCommands);
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
      out << CGI::GetMathSpanPure
      (tempSas.GetHighestWeightFundNewComponentFromImagesOldSimpleRootsAndNewRoot
       (largerTypes[i], imagesSimpleRoots[i], tempCandidate).ToStringLetterFormat("\\omega"));
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
    if (this->owner->atomsThatAllowCommutingOfCompositesStartingWithThem.Contains(whichOperation))
    { for (int i=1; i<this->children.size; i++)
        (*this)[i].GetBlocksOfCommutativity(inputOutputList);
      return;
    }
//  stOutput << "<hr>which operation=" << whichOperation;
  inputOutputList.AddOnTopNoRepetition(*this);
}

bool Expression::MakeSequence(Calculator& owner, List<Expression>* inputSequence)
{ MacroRegisterFunctionWithName("Expression::MakeSequence");
  this->reset(owner, inputSequence==0 ? 1 : inputSequence->size+1);
  this->AddChildAtomOnTop(owner.opSequence());
//  stOutput << "Making sequence from: " << inputSequence.ToString();
  if (inputSequence!=0)
    for (int i=0; i<inputSequence->size; i++)
      this->AddChildOnTop((*inputSequence)[i]);
  return true;
}

bool Expression::MakeSequenceCommands(Calculator& owner, List<std::string>& inputKeys, List<Expression>& inputValues)
{ MacroRegisterFunctionWithName("Expression::MakeSequenceCommands");
  List<Expression> theStatements;
  Expression currentStatement, currentKey;
  if (inputValues.size!=inputKeys.size)
    crash << "This is a programming error: I am asked to create a sequence of statements but I was given different"
    << " number of keys and expressions." << crash;
  for (int i =0; i<inputValues.size; i++)
  { currentKey.MakeAtom(inputKeys[i], owner);
    currentStatement.MakeXOX(owner, owner.opDefine(), currentKey, inputValues[i] );
    theStatements.AddOnTop(currentStatement);
  }
  return this->MakeSequenceStatements(owner, &theStatements);
}

bool Expression::MakeSequenceStatements(Calculator& owner, List<Expression>* inputStatements)
{ MacroRegisterFunctionWithName("Expression::MakeSequenceStatements");
  this->reset(owner, inputStatements==0 ? 1 : inputStatements->size+1);
  this->AddChildAtomOnTop(owner.opEndStatement());
//  stOutput << "Making sequence statements from: " << inputStatements.ToString();
  if (inputStatements!=0)
    for (int i=0; i<inputStatements->size; i++)
      this->AddChildOnTop((*inputStatements)[i]);
  return true;
}

bool CalculatorFunctionsGeneral::innerGetUserDefinedSubExpressions(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGetUserDefinedSubExpressions");
  HashedListSpecialized<Expression> theList;
  input.GetBlocksOfCommutativity(theList);
  return output.MakeSequence(theCommands, &theList);
}

bool CalculatorFunctionsGeneral::innerComputeSemisimpleSubalgebras(Calculator& theCommands, const Expression& input, Expression& output)
{ //bool showIndicator=true;
  MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerComputeSemisimpleSubalgebras");
  SemisimpleLieAlgebra* ownerSSPointer;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, input, ownerSSPointer))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  SemisimpleLieAlgebra& ownerSS=*ownerSSPointer;
  std::stringstream out;
  if (ownerSS.GetRank()>6)
  { out << "<b>This code is completely experimental and has been set to run up to rank 6. As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
    return output.AssignValue(out.str(), theCommands);
  } else
    out << "<b>This code is completely experimental. Use the following printouts on your own risk</b>";
  SemisimpleSubalgebras& theSSsubalgebras=
  theCommands.theObjectContainer.GetSemisimpleSubalgebrasCreateIfNotPresent(ownerSS.theWeyl.theDynkinType);
  theSSsubalgebras.flagComputePairingTable=false;
  theSSsubalgebras.flagComputeNilradicals=false;
  theSSsubalgebras.FindTheSSSubalgebrasFromScratch(ownerSS);
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
    if (!CalculatorConversions::innerMatrixRational(theCommands, input, output))
      return false;
  Matrix<Rational> theMat;
  if (!output.IsOfType<Matrix<Rational> >(&theMat))
    return theCommands << "<hr>Minimal poly computation: could not convert " << input.ToString() << " to rational matrix.";
  if (theMat.NumRows!=theMat.NumCols || theMat.NumRows<=0)
    return output.MakeError("Error: matrix is not square!", theCommands);
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
    if (!CalculatorConversions::innerMatrixRational(theCommands, input, output))
      return false;
  Matrix<Rational> theMat;
  if (!output.IsOfType<Matrix<Rational> >(&theMat))
    return theCommands << "<hr>Characteristic poly computation: could not convert " << input.ToString() << " to rational matrix.";
  if (theMat.NumRows!=theMat.NumCols || theMat.NumRows<=0)
    return output.MakeError("Error: matrix is not square!", theCommands);
  FormatExpressions tempF;
  tempF.polyAlphabeT.SetSize(1);
  tempF.polyAlphabeT[0]="q";
  Polynomial<Rational> theCharPoly;
  theCharPoly.AssignCharPoly(theMat);
  return output.AssignValue(theCharPoly.ToString(&tempF), theCommands);
}

bool CalculatorFunctionsGeneral::innerTrace(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerTrace");
//  stOutput << "<hr>getting matrix from: " << input.ToString();
  if (input.IsOfType<Matrix<Rational> >())
  { if (!input.GetValue<Matrix<Rational> >().IsSquare())
      return output.MakeError("Error: attempting to get trace of non-square matrix.", theCommands);
    return output.AssignValue(input.GetValue<Matrix<Rational> >().GetTrace(), theCommands);
  }
  if (input.IsOfType<Matrix<RationalFunctionOld> >())
  { if (!input.GetValue<Matrix<RationalFunctionOld> >().IsSquare())
      return output.MakeError("Error: attempting to get trace of non-square matrix.", theCommands);
    return output.AssignValue(input.GetValue<Matrix<RationalFunctionOld> >().GetTrace(), theCommands);
  }
  Matrix<Expression> theMat;
  if (!theCommands.GetMatrixExpressionsFromArguments(input, theMat))
    return false;
  if (!theMat.IsSquare())
    return output.MakeError("Error: attempting to get trace of non-square matrix.", theCommands);
  if (theMat.NumRows==1)
  { theCommands << "Requesting trace of 1x1 matrix: possible interpretation of a scalar as a 1x1 matrix. Trace not taken";
    return false;
  }
  output=theMat.GetTrace();
  return true;
}

bool CalculatorFunctionsGeneral::innerLastElement(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerLastElement");
  if (input.HasBoundVariables())
    return false;
//  stOutput << "<hr>calling last element with input: " << input.ToString() << ", longer printout: " << input.ToStringSemiFull();
  if (input.IsAtom())
  { std::stringstream out;
    out << "Error: requesting the last element of the atom " << input.ToString();
    return output.MakeError(out.str(), theCommands);
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
//  stOutput << "<hr>calling remove last element with input: " << input.ToString() << ", longer printout: " << input.ToStringSemiFull();
  if (input.IsAtom() || input.children.size==1)
  { std::stringstream out;
    if (input.IsAtom())
      out << "Error: requesting to remove the last element of the atom " << input.ToString();
    else
      out << "Error: requesting to remove the last element of the zero-element list " << input.ToString();
    return output.MakeError(out.str(), theCommands);
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

bool ElementZmodP::operator==(int other)const
{ this->CheckIamInitialized();
  //std::cout << "comparing " << this->ToString() << " to " << other;
  //std::cout.flush();
  ElementZmodP tempElt;
  tempElt.theModulo=this->theModulo;
  tempElt=(LargeInt) other;
  return *this==tempElt;
}

bool ElementZmodP ::operator==(const ElementZmodP& other)const
{ this->CheckIamInitialized();
  other.CheckIamInitialized();
  return this->theModulo==other.theModulo && this->theValue==other.theValue;
}

void ElementZmodP ::operator*=(const ElementZmodP& other)
{ if (this==&other)
  { ElementZmodP other=*this;
    *this*=other;
    return;
  }
  this->CheckEqualModuli(other);
  this->theValue*=other.theValue;
  this->theValue%=this->theModulo;
}

bool LargeIntUnsigned::IsPossiblyPrimeMillerRabinOnce
(unsigned int theBase, int theExponentOfThePowerTwoFactorOfNminusOne,
 const LargeIntUnsigned& theOddFactorOfNminusOne)
{ MacroRegisterFunctionWithName("LargeIntUnsigned::IsPossiblyPrimeMillerRabin");
  ElementZmodP thePower, theOne;
  thePower.theModulo=*this;
  thePower.theValue=theBase;
  theOne.theModulo=*this;
  theOne.theValue=1;
  std::cout << "got to here 4\n";
  MathRoutines::RaiseToPower(thePower, theOddFactorOfNminusOne, theOne);
  std::cout << "got to here 5\n thePower: " << thePower.ToString() << "\n";
  std::cout.flush();
  if (thePower==1 )
    return true;
  std::cout << "got to here 5.5\n ";
  std::cout.flush();
  for (int i=0; i<theExponentOfThePowerTwoFactorOfNminusOne; i++)
  { if (thePower==-1)
      return true;
    if (i==theExponentOfThePowerTwoFactorOfNminusOne-1)
      return false;
    thePower*=thePower;
  }
  return false;
}

bool LargeIntUnsigned::IsPossiblyPrimeMillerRabin(int numTimesToRun)
{ MacroRegisterFunctionWithName("LargeIntUnsigned::IsPossiblyPrimeMillerRabin");
  if (this->IsEven())
    return *this==2;
  List<unsigned int> aFewPrimes;
  std::cout << "got to here 1 \n";
  std::cout.flush();
  LargeIntUnsigned::GetAllPrimesSmallerThanOrEqualToUseEratosthenesSieve(100000, aFewPrimes);
  if (numTimesToRun>aFewPrimes.size)
    numTimesToRun=aFewPrimes.size;
  LargeIntUnsigned theOddFactorOfNminusOne=*this;
  int theExponentOfThePowerTwoFactorOfNminusOne=0;
  theOddFactorOfNminusOne--;
  std::cout << "got to here 2 \n";
  std::cout.flush();
  while (theOddFactorOfNminusOne.IsEven())
  { theOddFactorOfNminusOne/=2;
    theExponentOfThePowerTwoFactorOfNminusOne++;
  }
  std::cout << "got to here 3\n";
  std::cout.flush();
  for (int i=0; i<numTimesToRun; i++)
    if (!this->IsPossiblyPrimeMillerRabinOnce
        (aFewPrimes[i], theExponentOfThePowerTwoFactorOfNminusOne, theOddFactorOfNminusOne))
      return false;
  return true;
}

bool CalculatorFunctionsGeneral::innerIsPrimeMillerRabin(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIsPrimeMillerRabin");
  LargeInt theInt;
  if (!input.IsInteger(&theInt))
    return false;
  bool resultBool=theInt.value.IsPossiblyPrimeMillerRabin(10);
  std::cout << "got to here";
  std::cout.flush();
  Rational result=1;
  if (!resultBool)
    result=0;
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsGeneral::innerIsNilpotent(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIsNilpotent");
  Matrix<Rational> theMat;
  MatrixTensor<Rational> theMatTensor;
  bool found=false;
  if (input.IsOfType<Matrix<Rational> >(&theMat))
  { found=true;
    theMatTensor=theMat;
  } else if (input.IsOfType<MatrixTensor<Rational> >(&theMatTensor))
    found =true;
  else if (theCommands.GetMatriXFromArguments<Rational>(input, theMat, 0, -1, 0))
  { theMatTensor=theMat;
    found=true;
  }
  if (!found)
    return output.MakeError("Failed to extract matrix with rational coefficients", theCommands);
//  stOutput << "mat tensor: " << theMatTensor.ToString();
  if (theMatTensor.IsNilpotent())
    return output.AssignValue(1, theCommands);
  return output.AssignValue(0, theCommands);
}

bool CalculatorFunctionsGeneral::innerInvertMatrix(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerInvertMatrix");
  Matrix<Rational> theMat;
//  stOutput << "Lispified: " << input.ToString();
  if (!input.IsOfType<Matrix<Rational> >(&theMat))
    if (!theCommands.GetMatriXFromArguments<Rational>(input, theMat, 0, -1, 0))
      return output.MakeError("Failed to extract matrix with rational coefficients", theCommands);
  if (theMat.NumRows!=theMat.NumCols || theMat.NumCols<1)
    return output.MakeError("The matrix is not square", theCommands);
  if (theMat.GetDeterminant()==0)
    return output.MakeError("Matrix determinant is zero.", theCommands);
  theMat.Invert();
  return output.AssignValue(theMat, theCommands);
}

bool CalculatorFunctionsGeneral::innerPlotWedge(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotWedge");
  if (input.children.size!=6)
    return theCommands << "<hr>innerPlotWedge takes as input 4 arguments: x,y coordinates of center, starting angle, final angle. ";
  Rational xCoord, yCoord, radius, startAngle, endAngle;
  if (!input[1].IsOfType<Rational>(&xCoord) || !input[2].IsOfType<Rational>(&yCoord) || !input[3].IsOfType<Rational>(&radius) ||
      !input[4].IsOfType<Rational>(&startAngle) || !input[5].IsOfType<Rational>(&endAngle))
    return theCommands << "<hr>Failed to extract x, y coordinate, radius, start angle, end angle from " << input.ToString() << ". ";
  std::stringstream out;
  double x1wedge= MathRoutines::ReducePrecision(xCoord.GetDoubleValue()+ radius.GetDoubleValue() *FloatingPoint::cos(startAngle.GetDoubleValue()));
  double y1wedge= MathRoutines::ReducePrecision(yCoord.GetDoubleValue()+ radius.GetDoubleValue() *FloatingPoint::sin(startAngle.GetDoubleValue()));
  double x2wedge= MathRoutines::ReducePrecision(xCoord.GetDoubleValue()+ radius.GetDoubleValue() *FloatingPoint::cos(endAngle.GetDoubleValue()));
  double y2wedge= MathRoutines::ReducePrecision(yCoord.GetDoubleValue()+ radius.GetDoubleValue() *FloatingPoint::sin(endAngle.GetDoubleValue()));
  double xCoordDouble=MathRoutines::ReducePrecision(xCoord.GetDoubleValue());
  double yCoordDouble=MathRoutines::ReducePrecision(yCoord.GetDoubleValue());
  double startAngleDouble=MathRoutines::ReducePrecision(startAngle.GetDoubleValue());
  double radiusDouble=MathRoutines::ReducePrecision(radius.GetDoubleValue());
  double endAngleDouble=MathRoutines::ReducePrecision(endAngle.GetDoubleValue());
  out << "\\pscustom*[linecolor=cyan]{ \\psparametricplot[algebraic,linecolor=\\fcColorGraph]{" << startAngleDouble << "}{" << endAngleDouble
  << "}{" << xCoordDouble << "+" << radiusDouble << "*cos(t)| " << yCoordDouble << "+" << radiusDouble << "*sin(t)} \\psline("
  << x2wedge << ", " << y2wedge << ")(" << xCoordDouble << ", " << yCoordDouble << ")" << "(" << x1wedge << ", " << y1wedge << ")}";
  out << "\\pscustom[linecolor=blue]{ \\psparametricplot[algebraic,linecolor=\\fcColorGraph]{" << startAngleDouble << "}{" << endAngleDouble
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
  theCommands.flagWriteLatexPlots=true;
  return output.AssignValue(thePlot, theCommands);
}

bool CalculatorFunctionsGeneral::innerPlotIntegralOf(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotIntegralOf");
  if (input.children.size<4)
    return output.MakeError("Plotting integral of takes at least three arguments: function, lower and upper bound. ", theCommands);
  const Expression& lowerE=input[2];
  const Expression& upperE=input[3];
  Expression functionE;
  double upperBound, lowerBound;
  if (!lowerE.EvaluatesToDouble(&upperBound) || !upperE.EvaluatesToDouble(&lowerBound))
    return output.MakeError("Failed to convert upper and lower bounds of drawing function to rational numbers.", theCommands);
  if (upperBound<lowerBound)
    MathRoutines::swap(upperBound, lowerBound);
  if (!theCommands.CallCalculatorFunction(theCommands.innerSuffixNotationForPostScript, input[1], functionE))
  { std::stringstream out;
    out << "Failed to convert expression " << input[1].ToString() << " to postfix notation. ";
    return output.MakeError(out.str(), theCommands);
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
  theCommands.flagWriteLatexPlots=true;
  plotFinal.AddFunctionPlotOnTop
  (input[1], functionE.GetValue<std::string>(), lowerBound, upperBound, -4,  4, 0);
  return output.AssignValue(plotFinal, theCommands);
}

bool CalculatorFunctionsGeneral::innerDFQsEulersMethod(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDFQsEulersMethod");
  if (input.children.size!=7)
    return theCommands << "Euler method function takes 6 arguments";
  double xInitial, yInitial, leftEndpoint, rightEndpoint;
  int numPoints;
  if (!input[2].EvaluatesToDouble(&xInitial) || !input[3].EvaluatesToDouble(&yInitial))
    return theCommands << "Failed to extract initial x,y values from " << input.ToString();
//  stOutput << "xInitial=" << xInitial << " yInitial=" << yInitial;
  if (!input[4].IsSmallInteger(&numPoints) )
    return theCommands << "Failed to extract number of points from " << input.ToString();
  if (!input[5].EvaluatesToDouble(&leftEndpoint) || !input[6].EvaluatesToDouble(&rightEndpoint))
    return theCommands << "Failed to extract left and right endpoints from " << input.ToString();
  if (leftEndpoint>rightEndpoint)
    MathRoutines::swap(leftEndpoint, rightEndpoint);
  if (xInitial>rightEndpoint || xInitial<leftEndpoint)
    return theCommands << "The initial value for x, " << xInitial << ", does not lie between the left and right endpoints "
    << leftEndpoint << ", " << rightEndpoint << ". I am aborting Euler's method.";
  HashedList<Expression> knownConsts;
  List<double> knownValues;
  knownConsts.AddOnTop(theCommands.knownDoubleConstants);
  knownValues.AddListOnTop(theCommands.knownDoubleConstantValues);
  Expression xE, yE;
  xE.MakeAtom("x", theCommands);
  yE.MakeAtom("y", theCommands);
  if (knownConsts.Contains(xE) || knownConsts.Contains(yE))
    return theCommands << "The letters x, y appear to be already used to denote known constants, I cannot run Euler's method.";
  knownConsts.AddOnTop(xE);
  knownConsts.AddOnTop(yE);
  knownValues.AddOnTop(0);
  knownValues.AddOnTop(0);
  if (numPoints<2)
    return theCommands << "The number of points for Euler's method is " << numPoints << ", too few. ";
  if (numPoints>10001)
    return theCommands << "The number of points for Euler's method is " << numPoints
    << ", I am not allowed to handle that many. ";
  if (leftEndpoint==rightEndpoint)
    return theCommands << "Whlie doing Euler's method: right endpoint equals left!";
//  stOutput << "numpoints: " << numPoints << ", rightendpt: " << rightEndpoint << ", left end pt: " << leftEndpoint;

  double delta= (rightEndpoint-leftEndpoint)/numPoints;
//  stOutput << "delta: " << delta;
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
//  stOutput << "Xvalues: ";
//  for (int i=0; i<XValues.size; i++)
//    stOutput << XValues[i] << ", ";
  int lastGoodXIndex=indexXinitial;
  int firstGoodXIndex=indexXinitial;
  double maxYallowed=(rightEndpoint-leftEndpoint)*2+yInitial;
  double minYallowed=-(rightEndpoint-leftEndpoint)*2+yInitial;
  //stOutput << "maxYallowed=" << maxYallowed;
  //stOutput << "minYallowed=" << minYallowed;
  for (int direction=-1; direction<2; direction+=2)
    for (int i=indexXinitial+direction; i>=0 && i<XValues.size; i+=direction)
    { knownValues[knownValues.size-2]=XValues[i];
      bool isGood=true;
      for (int counter=0; ; counter++)
      { knownValues[knownValues.size-1]=YValues[i];
        if (!functionE.EvaluatesToDoubleUnderSubstitutions(knownConsts, knownValues, &currentYprimeApprox))
          return theCommands << "Failed to evaluate yPrime approximation at x=" << XValues[i];
        double adjustment=delta*direction*currentYprimeApprox;
        double ynew=YValues[i-direction]+adjustment;
        double difference= ynew-YValues[i];
        YValues[i]=ynew;
        if (counter>20) //<- we run Euler algorithm at least 20 times.
          if (difference>-0.01 && difference<0.01)//<-if changes are smaller than 0.01 we assume success.
            break;
        if (counter>200) //<- we ran Euler algorithm 100 times, but the difference is still greater than 0.01. Something is wrong, we abort
        { theCommands << "Euler method: no convergence. At x=" << XValues[i] << ", y="
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
        { theCommands << "Euler method: y values outside of the bounding box. At x=" << XValues[i]
          << ", the y value is: " << YValues[i] << ". Max y is  " << maxYallowed << " and min y is " << minYallowed << ". ";
        }
      }
      if (direction==-1)
        firstGoodXIndex=i+1;
      else
        lastGoodXIndex=i-1;
      if (!isGood)
        break;
//      stOutput << "evaluated at " << XValues[i] << " to get " << YValues[i];
    }
//  stOutput << "first good index: " << firstGoodXIndex << " last good index: " << lastGoodXIndex;
  PlotObject thePlot;
  Vector<double> currentPt;
  currentPt.SetSize(2);
  for (int i=firstGoodXIndex; i<=lastGoodXIndex; i++)
  { currentPt[0]=XValues[i];
    currentPt[1]=YValues[i];
    thePlot.thePoints.AddOnTop(currentPt);
  }
  thePlot.xLow=XValues[0];
  thePlot.xHigh=*XValues.LastObject();
  thePlot.yLow=-0.5;
  thePlot.yHigh=0.5;
  thePlot.thePlotElement=input;
  std::stringstream outLatex, outHtml;
  outLatex << "\n\n%calculator input:" << input.ToString() << "\n\n"
  << "\\psline[linecolor=\\fcColorGraph]";
  outHtml << "\n<br>\n%calculator input:" << input.ToString() << "\n<br>\n"
  << "\\psline[linecolor=\\fcColorGraph]";
  for(int i=firstGoodXIndex; i<=lastGoodXIndex; i++)
  { outLatex << std::fixed << "(" << XValues[i] << ", " << YValues[i] << ")";
    outHtml << std::fixed << "(" << XValues[i] << ", " << YValues[i] << ")";
    thePlot.yLow=MathRoutines::Minimum(YValues[i], thePlot.yLow);
    thePlot.yHigh=MathRoutines::Maximum(YValues[i], thePlot.yHigh);
  }
//  stOutput << "final yhigh: " << thePlot.yHigh << ", ylow: " << thePlot.yLow;
  thePlot.thePlotString=outLatex.str();
  thePlot.thePlotStringWithHtml=outHtml.str();
  return output.AssignValue(thePlot, theCommands);
}

bool CalculatorFunctionsGeneral::innerPlotViewRectangle
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotViewRectangle");
  if (input.children.size<3)
    return false;
  Vector<double> lowerLeft, upperRight;
  if (!theCommands.GetVectorDoubles(input[1], lowerLeft, 2) || !theCommands.GetVectorDoubles(input[2], upperRight, 2))
    return theCommands << "Failed to extract two pairs of floating point numbers from: "
    << input[1].ToString() << " and " << input[2].ToString();
  Plot emptyPlot;
  emptyPlot.theLowerBoundAxes=lowerLeft[0];
  emptyPlot.lowBoundY=lowerLeft[1];
  emptyPlot.theUpperBoundAxes=upperRight[0];
  emptyPlot.highBoundY=upperRight[1];
  emptyPlot.viewWindowPriority=1;
  emptyPlot.DesiredHtmlHeightInPixels=100;
  emptyPlot.DesiredHtmlWidthInPixels=100;
  return output.AssignValue(emptyPlot, theCommands);
}

bool CalculatorFunctionsGeneral::innerPlotFill(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotFill");
  //stOutput << input.ToString();
  if (input.size()<3)
    return false;
  const Expression& thePlotE=input[1];
  const Expression& colorE=input[2];
  Plot outputPlot, startPlot;
  PlotObject theFilledPlot;
  if (!thePlotE.IsOfType<Plot>(&startPlot))
    return false;
  std::string colorString;
  if (!colorE.IsOfType<std::string>(&colorString))
    colorString=colorE.ToString();
  if (!DrawingVariables::GetColorIntFromColorString(colorString, theFilledPlot.fillColorRGB))
    theCommands << "Failed to extract color from: " << colorE.ToString() << "; using default color value. ";
  for (int i=0; i<startPlot.thePlots.size; i++)
    theFilledPlot.thePoints.AddListOnTop(startPlot.thePlots[i].thePoints);
  theFilledPlot.fillStyle="filled";
  outputPlot.DesiredHtmlHeightInPixels=startPlot.DesiredHtmlHeightInPixels;
  outputPlot.DesiredHtmlWidthInPixels=startPlot.DesiredHtmlWidthInPixels;
  outputPlot.thePlots.AddOnTop(theFilledPlot);
  outputPlot.thePlots.AddListOnTop(startPlot.thePlots);
  return output.AssignValue(outputPlot, theCommands);
}

bool CalculatorFunctionsGeneral::innerPlot2D(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlot2D");
  //stOutput << input.ToString();
  if (input.children.size<4)
    return output.MakeError
    ("Plotting coordinates takes at least three arguments: function, lower and upper bound. ", theCommands);
  if (input.HasBoundVariables())
    return false;
  const Expression& lowerE=input[2];
  const Expression& upperE=input[3];
  int desiredHtmlHeightPixels=400;
  int desiredHtmlWidthPixels=600;
  if (input.children.size>=6)
  { if (!input[4].IsSmallInteger(&desiredHtmlWidthPixels))
      desiredHtmlWidthPixels=600;
    if (!input[5].IsSmallInteger(&desiredHtmlHeightPixels))
      desiredHtmlHeightPixels=400;
  }
  std::string colorString;
  int colorTripleRGB=CGI::RedGreenBlue(255, 0, 0);
  if (input.children.size>=7)
    if (input[6].IsOfType<std::string>(&colorString))
      DrawingVariables::GetColorIntFromColorString(colorString, colorTripleRGB);
  double linewidth=1;
  if (input.children.size>=8)
  { if (input[7].EvaluatesToDouble(&linewidth))
      DrawingVariables::GetColorIntFromColorString(colorString, colorTripleRGB);
    else
      linewidth=1;
  }
  int numIntervals=500;
  if (input.children.size>=9)
    if (!input[8].IsSmallInteger(&numIntervals))
      numIntervals=500;
  if (numIntervals<2)
    numIntervals=2;
  Expression functionE;
  double upperBound, lowerBound;
  if (!lowerE.EvaluatesToDouble(&lowerBound) || !upperE.EvaluatesToDouble(&upperBound))
    return output.MakeError("Failed to convert upper and lower bounds of drawing function to rational numbers.", theCommands);
//  if (upperBound<lowerBound)//no bounds swapping! this messes up filling commands.
//    MathRoutines::swap(upperBound, lowerBound);
  if (!theCommands.CallCalculatorFunction(theCommands.innerSuffixNotationForPostScript, input[1], functionE))
  { std::stringstream out;
    out << "Failed to convert expression " << input[1].ToString() << " to postfix notation. ";
    return output.MakeError(out.str(), theCommands);
  }
  double yLow, yHigh;
  Vectors<double> thePoints;
  if (!input[1].EvaluatesToDoubleInRange("x", lowerBound, upperBound, numIntervals, &yLow, &yHigh, &thePoints))
  { bool hasOneGoodPoint=false;
    for (int i=0; i<thePoints.size; i++)
      if (!std::isnan(thePoints[i][1]))
      { hasOneGoodPoint=true;
        break;
      }
    if (!hasOneGoodPoint)
      return theCommands << "<hr>I failed to evaluate the input function at all points, "
      << "perhaps your expression is not a function of x.";
    theCommands << "<hr>I failed to evaluate your function in a number of points. ";
  }
  Plot thePlot;
  thePlot.DesiredHtmlHeightInPixels=desiredHtmlHeightPixels;
  thePlot.DesiredHtmlWidthInPixels=desiredHtmlWidthPixels;
  thePlot.AddFunctionPlotOnTop
  (input[1], functionE.GetValue<std::string>(), lowerBound, upperBound, yLow, yHigh, &thePoints,
   &colorTripleRGB, linewidth);
  return output.AssignValue(thePlot, theCommands);
}

bool CalculatorFunctionsGeneral::innerPlotPoint(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotPoint");
  //stOutput << input.ToString();
  if (input.children.size<3)
    return output.MakeError
    ("Plotting a point takes at least two arguments, namely the x and y coordinate of the point. ", theCommands);
  Vector<double> theV;
  theV.initFillInObject(2,0);
  if (!input[1].EvaluatesToDouble(&theV[0]) || !input[2].EvaluatesToDouble(&theV[1]))
    return theCommands << "<hr>Failed to extract x, y coordinates from: " << input.ToString();
  Plot theFinalPlot;
  PlotObject thePlot;
  thePlot.colorRGB=CGI::RedGreenBlue(0,0,0);
  if (input.children.size>3)
    if (input[3].IsOfType<std::string>())
      DrawingVariables::GetColorIntFromColorString(input[3].GetValue<std::string>(), thePlot.colorRGB);
  thePlot.thePoints.AddOnTop(theV);
  thePlot.thePlotType="point";
  theFinalPlot.thePlots.AddOnTop(thePlot);
  theFinalPlot.DesiredHtmlHeightInPixels=100;
  theFinalPlot.DesiredHtmlWidthInPixels=100;
//  stOutput << "DEBUG: plotting point with coords: " << theV;
  return output.AssignValue(theFinalPlot, theCommands);
}

bool CalculatorFunctionsGeneral::innerPlot2DWithBars(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlot2DWithBars");
  //stOutput << input.ToString();
  if (input.children.size<6)
    return output.MakeError("Plotting coordinates takes the following arguments: lower function, upper function, lower and upper bound, delta x. ", theCommands);
  Expression lowerEplot=input, upperEplot=input;
  lowerEplot.children.RemoveIndexShiftDown(2);
  upperEplot.children.RemoveIndexShiftDown(1);
  Plot outputPlot;
  bool tempB=CalculatorFunctionsGeneral::innerPlot2D(theCommands, lowerEplot, output);
  if (!tempB || !output.IsOfType<Plot>(&outputPlot))
    return theCommands << "<hr>Failed to get a plot from " << lowerEplot.ToString() << ", not proceding with bar plot.";
  tempB=CalculatorFunctionsGeneral::innerPlot2D(theCommands, upperEplot, output);
  if (!tempB || !output.IsOfType<Plot>())
    return theCommands << "<hr>Failed to get a plot from " << upperEplot.ToString() << ", not proceding with bar plot.";
  outputPlot+=output.GetValue<Plot>();
  const Expression& lowerFunctionE=input[1];
  const Expression& upperFunctionE=input[2];
  const Expression& lowerE=input[3];
  const Expression& upperE=input[4];
  const Expression& deltaE=input[5];
  double theDeltaNoSign, theDeltaWithSign;
  if (!deltaE.EvaluatesToDouble(&theDeltaWithSign))
    return false;
  theDeltaNoSign=theDeltaWithSign;
  if (theDeltaNoSign<0)
    theDeltaNoSign*=-1;
  if (theDeltaNoSign==0)
    theDeltaNoSign=1;
  double upperBound, lowerBound;
  if (!lowerE.EvaluatesToDouble(&lowerBound) || !upperE.EvaluatesToDouble(&upperBound))
    return output.MakeError("Failed to convert upper and lower bounds of drawing function to rational numbers.", theCommands);
  if (upperBound<lowerBound)
    MathRoutines::swap(upperBound, lowerBound);
  Expression xValueE, xExpression, theFunValueEnonEvaluated, theFunValueFinal;
  xExpression.MakeAtom(theCommands.AddOperationNoRepetitionOrReturnIndexFirst("x"), theCommands);
  List<double> xValues;
  List<double> fValuesLower;
  List<double> fValuesUpper;
  if (theDeltaNoSign==0)
    return output.MakeError("Delta equal to zero is not allowed", theCommands);
  if ((upperBound-lowerBound)/theDeltaNoSign>10000)
    return output.MakeError("More than 10000 intervals needed for the plot, this is not allowed.", theCommands);
  List<Rational> rValues;
  Rational lowerBoundRat, upperBoundRat, deltaRat;
  if (lowerE.IsOfType<Rational>(&lowerBoundRat) && upperE.IsOfType<Rational>(&upperBoundRat) && deltaE.IsOfType<Rational>(&deltaRat))
  { if (upperBoundRat<lowerBoundRat)
      MathRoutines::swap(upperBoundRat, lowerBoundRat);
    if (deltaRat<0)
      deltaRat*=-1;
    if (deltaRat==0)
      return output.MakeError("Delta equal to zero is not allowed", theCommands);
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
  //    stOutput << "<br>substitution result:" << tempE2.ToString();
      if (!theCommands.EvaluateExpression(theCommands, theFunValueEnonEvaluated, theFunValueFinal))
        return false;
  //    stOutput << "and after evaluation: " << theFunValueFinal.ToString();
      double finalResultDouble;
      if (!theFunValueFinal.EvaluatesToDouble(&finalResultDouble))
        return theCommands << "<hr>Failed to evaluate your function at point " << i << ", instead " << "I evaluated to " << theFunValueFinal.ToString();
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
      { outTex << "\\psline*[linecolor=\\fcColorNegativeAreaUnderGraph, linewidth=0.1pt]";
        outHtml << "<br>\\psline*[linecolor=\\fcColorNegativeAreaUnderGraph, linewidth=0.1pt]";
      }
      if (k==0 && !useNegativePattern)
      { outTex << "\\psline*[linecolor=\\fcColorAreaUnderGraph, linewidth=0.1pt]";
        outHtml << "<br>\\psline*[linecolor=\\fcColorAreaUnderGraph, linewidth=0.1pt]";
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
    tempStream << "\\rput[t](" << std::fixed << rValues[i].GetDoubleValue() << ",-0.03)" << "{$";
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
    return output.MakeError("Drawing polar coordinates takes three arguments: function, lower angle bound and upper angle bound. ", theCommands);
  const Expression& lowerE=input[2];
  const Expression& upperE=input[3];
  Expression functionE;
  double lowerBound=0, upperBound=0;
  PlotObject thePlotCartesian;
  if (!lowerE.EvaluatesToDouble(&lowerBound) || !upperE.EvaluatesToDouble(&upperBound))
    return output.MakeError("Failed to convert upper and lower bounds of drawing function to rational numbers.", theCommands);
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
  Vectors<double> theXCoords, theYCoords;
  if (!xCoordE.EvaluatesToDoubleInRange("t", lowerBound, upperBound, 500, &thePlotCartesian.xLow, &thePlotCartesian.xHigh, &theXCoords))
    return false;
  if (!yCoordE.EvaluatesToDoubleInRange("t", lowerBound, upperBound, 500, &thePlotCartesian.yLow, &thePlotCartesian.yHigh, &theYCoords))
    return false;
  thePlotCartesian.thePoints.SetSize(theXCoords.size);
  for (int i=0; i<thePlotCartesian.thePoints.size; i++)
  { thePlotCartesian.thePoints[i].SetSize(2);
    thePlotCartesian.thePoints[i][0]=theXCoords[i][1];
    thePlotCartesian.thePoints[i][1]=theYCoords[i][1];
  }
  std::stringstream outCartesianLatex, outCartesianHtml;
  outCartesianLatex << "%Calculator command: " << input.ToString() << "\n";
  outCartesianHtml << "%Calculator command: " << input.ToString() << "\n<br>\n";

  outCartesianLatex << "\\parametricplot[linecolor=\\fcColorGraph, plotpoints=1000, algebraic=false]{" << lowerBound << "}{"
  << upperBound << "}{";
  outCartesianHtml << "\\parametricplot[linecolor=\\fcColorGraph, plotpoints=1000, algebraic=false]{" << lowerBound << "}{"
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
    return output.MakeError("Drawing polar coordinates takes three arguments: function, lower angle bound and upper angle bound. ", theCommands);
  const Expression& lowerE=input[2];
  const Expression& upperE=input[3];
  Expression functionE;
  PlotObject thePlotPolar;
  if (!lowerE.EvaluatesToDouble(&thePlotPolar.xLow) || !upperE.EvaluatesToDouble(&thePlotPolar.xHigh))
    return output.MakeError("Failed to convert upper and lower bounds of drawing function to rational numbers.", theCommands);
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
  outPolarLatex << "\\parametricplot[linecolor=\\fcColorGraph, plotpoints=1000, algebraic=false]{" << thePlotPolar.xLow << "}{"
  << thePlotPolar.xHigh << "}{";
  outPolarHtml << "\\parametricplot[linecolor=\\fcColorGraph, plotpoints=1000, algebraic=false]{" << thePlotPolar.xLow << "}{"
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
    return theCommands
    << "Parametric curve plots take 4+ arguments, first arguments stand for the functions in the various coordinates, "
    << " the last two arguments stands for the variable range.";
  if (input.HasBoundVariables())
    return false;
  int desiredHtmlHeightPixels=400;
  int desiredHtmlWidthPixels=400;
  if (input.children.size>=7)
  { if (!input[5].IsSmallInteger(&desiredHtmlWidthPixels))
      desiredHtmlWidthPixels=400;
    if (!input[6].IsSmallInteger(&desiredHtmlHeightPixels))
      desiredHtmlHeightPixels=400;
  }
  std::string colorString;
  int colorTripleRGB=CGI::RedGreenBlue(255, 0, 0);
  if (input.children.size>=8)
    if (!input[7].IsOfType<std::string>(&colorString))
      colorString=input[7].ToString();
  DrawingVariables::GetColorIntFromColorString(colorString, colorTripleRGB);
  double lineWidth=1;
  if (input.size()>=9)
    if (!input[8].EvaluatesToDouble(&lineWidth))
      lineWidth=1;
  int numPoints=1000;
  if (input.size()>=10)
    if (!input[9].IsSmallInteger(&numPoints))
    { numPoints=1000;
      theCommands << "<hr>Could not extract number of points from " << input[9].ToString();
    }
  if (numPoints<2 || numPoints>30000)
  { numPoints=1000;
    theCommands << "<hr>Extracted " << numPoints << " point but that is not valid. Changing to 1000. ";
  }
  List<Expression> theConvertedExpressions;
  theConvertedExpressions.SetSize(input.children.size-3);
  for (int i=1; i<3; i++)
    if (!theCommands.CallCalculatorFunction(Calculator::innerSuffixNotationForPostScript, input[i], theConvertedExpressions[i-1]))
      return theCommands << "Failed to extract suffix notation from argument " << input[i].ToString();
  double leftEndPoint, rightEndPoint;
  if (!input[3].EvaluatesToDouble(&leftEndPoint) ||
      !input[4].EvaluatesToDouble(&rightEndPoint))
    return theCommands << "Failed to convert " << input[3].ToString() << " and "
    << input[4].ToString() << " to left and right endpoint of parameter interval. ";
  std::stringstream outLatex, outHtml;
  outLatex << "\\parametricplot[linecolor=\\fcColorGraph, plotpoints=" << numPoints << "]{" << leftEndPoint << "}{" << rightEndPoint << "}{"
  << theConvertedExpressions[0].GetValue<std::string>() << theConvertedExpressions[1].GetValue<std::string>() << "}";
  outHtml << "<br>%Calculator input: " << input.ToString()
  << "<br>\\parametricplot[linecolor=\\fcColorGraph, plotpoints=" << numPoints << "]{" << leftEndPoint << "}{" << rightEndPoint << "}{"
  << theConvertedExpressions[0].GetValue<std::string>() << theConvertedExpressions[1].GetValue<std::string>() << "}";
  PlotObject thePlot;
  thePlot.colorRGB=colorTripleRGB;
  thePlot.lineWidth=lineWidth;
  Vectors<double> theXs, theYs;
  if (!input[1].EvaluatesToDoubleInRange
      ("t", leftEndPoint, rightEndPoint, numPoints, &thePlot.xLow, &thePlot.xHigh, &theXs))
    return theCommands << "<hr>Failed to evaluate curve function. ";
  if (!input[2].EvaluatesToDoubleInRange
      ("t", leftEndPoint, rightEndPoint, numPoints, &thePlot.yLow, &thePlot.yHigh, &theYs))
    return theCommands << "<hr>Failed to evaluate curve function. ";
  thePlot.thePoints.SetSize(theXs.size);
  for (int i=0; i<theXs.size; i++)
  { thePlot.thePoints[i].SetSize(2);
    thePlot.thePoints[i][0]=theXs[i][1];
    thePlot.thePoints[i][1]=theYs[i][1];
  }
  thePlot.thePlotElement=input;
  thePlot.thePlotString=outLatex.str();
  thePlot.thePlotStringWithHtml=outHtml.str();
  Plot outputPlot;
  outputPlot.DesiredHtmlHeightInPixels=desiredHtmlHeightPixels;
  outputPlot.DesiredHtmlWidthInPixels=desiredHtmlWidthPixels;
  outputPlot.thePlots.AddOnTop(thePlot);
  return output.AssignValue(outputPlot, theCommands);
}

bool CalculatorFunctionsGeneral::innerPlotConeUsualProjection(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotConeUsualProjection");
  if (input.children.size!=5)
    return theCommands << "<hr>PlotConeUsualProjection takes 4 arguments. ";
  double radius, height, distance, viewPointHeight;
  if (!input[1].EvaluatesToDouble(&radius) || !input[2].EvaluatesToDouble(&height) || !input[3].EvaluatesToDouble(&distance) || !input[4].EvaluatesToDouble(&viewPointHeight) )
    return theCommands << "<hr> failed to extract radius, height, distance, viewpoint height from " << input.ToString();
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
  //stOutput << "<br>projection1: " << projection1.ToString();
  //stOutput << "<br>projection2: " << projection2.ToString();
  Vector<double> XdiameterVectorProjection=projection1-projection2;
  double theProjYradius= FloatingPoint::sqrt(XdiameterVectorProjection.ScalarEuclidean(XdiameterVectorProjection))/2;
  double theProjXradius=radius;
  //stOutput << "<br>theProjYradius: " << theProjYradius;
  //stOutput << "<br>theProjXradius: " << theProjXradius;
  std::stringstream out;
  Vector<double> tipOfTheCone, centerOfTheCone;
  centerOfTheCone.MakeZero(3);
  tipOfTheCone.MakeZero(3);
  tipOfTheCone[2]=height;
//  stOutput << "<br>radius, height, distance, view height: " << radius << ", " << height << ", " << distance << ", " << viewPointHeight;
//  stOutput << "<br>projectionPane.ProjectOnMe(tipOfTheCone): " << projectionPane.ProjectOnMe(tipOfTheCone);
  Vector<double> coneHeightVectorProjectedShifted= projectionPane.ProjectOnMe(tipOfTheCone)-projectionPane.ProjectOnMe(centerOfTheCone);
//  stOutput << "<br>coneHeightVectorProjectedShifted: " << coneHeightVectorProjectedShifted.ToString();
  double theConeProjectionHeight=FloatingPoint::sqrt(coneHeightVectorProjectedShifted.ScalarEuclidean(coneHeightVectorProjectedShifted));
//  stOutput << "<br>theConeProjectionHeight: " << theConeProjectionHeight;
  out << "\\psline[linecolor=black](0,0)(0," << MathRoutines::ReducePrecision(theConeProjectionHeight) << ")";
  if (theConeProjectionHeight>theProjYradius)
  { double yCoordPointTangency= theProjYradius*theProjYradius/theConeProjectionHeight;
    double xCoordPointTangency=theProjXradius*FloatingPoint::sqrt(1- yCoordPointTangency/theConeProjectionHeight);
    out << "\\psline[linecolor=\\fcColorGraph](" << MathRoutines::ReducePrecision(xCoordPointTangency) << ", "
    << MathRoutines::ReducePrecision(yCoordPointTangency) << ")(0, " << MathRoutines::ReducePrecision(theConeProjectionHeight) << ")";
    out << "\\psline[linecolor=\\fcColorGraph](" << MathRoutines::ReducePrecision(-xCoordPointTangency) << ", "
    << MathRoutines::ReducePrecision(yCoordPointTangency) << ")(0, " << MathRoutines::ReducePrecision(theConeProjectionHeight) << ")";
    double theAngleVisibleEnd=MathRoutines::Pi()/2;
    double theAngleVisibleStart=-theAngleVisibleEnd;
    double theAngleHiddenEnd=MathRoutines::Pi()/2;
    if (theProjXradius!=0)
    { theAngleVisibleEnd=FloatingPoint::arctan(yCoordPointTangency*theProjXradius/(xCoordPointTangency*theProjYradius));
      theAngleVisibleStart = -MathRoutines::Pi()-theAngleVisibleEnd;
      theAngleHiddenEnd=MathRoutines::Pi()-theAngleVisibleEnd;
    }
    out << "\\psparametricplot[algebraic,linecolor=\\fcColorGraph]{" << MathRoutines::ReducePrecision(theAngleVisibleStart)
    << "}{" << MathRoutines::ReducePrecision(theAngleVisibleEnd) << "}{" << MathRoutines::ReducePrecision(theProjXradius)
    << "*cos(t) |" << MathRoutines::ReducePrecision(theProjYradius) << "*sin(t)}";
    out << "\\psparametricplot[algebraic, linestyle=dashed, linecolor=\\fcColorGraph]{" << MathRoutines::ReducePrecision(theAngleVisibleEnd)
    << "}{" << MathRoutines::ReducePrecision(theAngleHiddenEnd) << "}{" << MathRoutines::ReducePrecision(theProjXradius)
    << "*cos(t) |" << MathRoutines::ReducePrecision(theProjYradius) << "*sin(t)}";
  } else
  { theCommands << "<hr>Cone is not high enough and therefore has no tip. ";
    out << "\\psparametricplot[algebraic,linecolor=\\fcColorGraph]{0}{6.283185307}{cos(t)*" << MathRoutines::ReducePrecision(theProjXradius)
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
    return theCommands << "<hr>Input of ComputeFKFT must be of type semisimple subalgebras. ";
  SemisimpleSubalgebras& theSAs=input.GetValueNonConst<SemisimpleSubalgebras>();
  theSAs.flagComputePairingTable=true;
  theSAs.flagComputeNilradicals=true;
  theSAs.ComputePairingTablesAndFKFTtypes();
  output=input;
  std::fstream theFile;
  std::string theFileName;
  theFileName="FKFTcomputation.html";
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML=true;
  tempFormat.flagUseLatex=true;
  tempFormat.flagUseHTML=true;
  tempFormat.flagCandidateSubalgebraShortReportOnly=false;
  FileOperations::OpenFileCreateIfNotPresentVirtual(theFile, "output/"+theFileName, false, true, false);
  theFile << theSAs.ToString(&tempFormat);
  std::stringstream out;
  out << "<a href=\"" << theGlobalVariables.DisplayPathOutputFolder << "FKFTcomputation.html\">FKFTcomputation.html</a>";
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerGetCentralizerChainsSemisimpleSubalgebras(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGetCentralizerChainsSemisimpleSubalgebras");
  if (!input.IsOfType<SemisimpleSubalgebras>())
    return theCommands << "<hr>Input of GetCentralizerChains must be of type semisimple subalgebras. ";
  SemisimpleSubalgebras& theSAs=input.GetValueNonConst<SemisimpleSubalgebras>();
  List<List<int> > theChains;
  std::stringstream out;
  theSAs.GetCentralizerChains(theChains);
  Expression currentChainE;
  out << theChains.size << " chains total. <br>";
  for (int i=0; i<theChains.size; i++)
  { out << "<br>Chain " << i+1 << ": LoadSemisimpleSubalgebras{}( " << theSAs.owner->theWeyl.theDynkinType.ToString() << ", (";
    for (int j=0; j<theChains[i].size; j++)
    { CalculatorConversions::innerStoreCandidateSA
      (theCommands, theSAs.theSubalgebras.theValues[theChains[i][j]], currentChainE);
      out << currentChainE.ToString();
      if (j!=theChains[i].size-1)
        out << ", ";
    }
    out << ")  )";
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerEvaluateToDouble(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Expression::innerEvaluateToDouble");
  double theValue=0;
  if (!input.EvaluatesToDouble(&theValue))
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
  if (!CalculatorConversions::innerDynkinType(theCommands, input[1], smallType) ||
      !CalculatorConversions::innerDynkinType(theCommands, input[2], targetType))
    return theCommands << "Failed to convert arguments of " << input.ToString() << " to two DynkinType's.";
  return output.AssignValue((int)smallType.CanBeExtendedParabolicallyTo(targetType), theCommands);
}

bool CalculatorFunctionsGeneral::innerGetSymmetricCartan(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGetSymmetricCartan");
  DynkinType theType;
  if (!CalculatorConversions::innerDynkinType(theCommands, input, theType))
    return theCommands << "Failed to convert " << input.ToString() << " to DynkinType.";
  std::stringstream out;
  Matrix<Rational> outputMat, outputCoMat;
  theType.GetCartanSymmetric(outputMat);
  theType.GetCoCartanSymmetric(outputCoMat);
  out << "Symmetric Cartan matrix: " << CGI::GetMathMouseHover(outputMat.ToStringLatex(),10000)
  << "<br>Co-symmetric Cartan matrix: " << CGI::GetMathMouseHover(outputCoMat.ToStringLatex(), 10000);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerGetPrincipalSl2Index(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGetPrincipalSl2Index");
  DynkinType theType;
  if (!CalculatorConversions::innerDynkinType(theCommands, input, theType))
    return theCommands << "Failed to convert " << input.ToString() << " to DynkinType.";
  return output.AssignValue(theType.GetPrincipalSlTwoCSInverseScale(), theCommands);
}

bool CalculatorFunctionsGeneral::innerGetDynkinIndicesSlTwoSubalgebras(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGetDynkinIndicesSlTwoSubalgebras");
  DynkinType theType;
  if (!CalculatorConversions::innerDynkinType(theCommands, input, theType))
    return theCommands << "Failed to convert " << input.ToString() << " to DynkinType.";
  if (theType.GetRank()>20)
    return theCommands << "Getting absolute Dynkin indices of sl(2)-subalgebras is restricted up to rank 20 "
    << "(for computational feasibility reasons).";
  List<List<Rational> > bufferIndices;
  HashedList<DynkinSimpleType> bufferTypes;
  HashedList<Rational> theIndices;
  theType.GetDynkinIndicesSl2Subalgebras(bufferIndices, bufferTypes, theIndices);
  std::stringstream out;
  out << "There are " << theIndices.size << " absolute Dynkin indices. The indices are: "
  << theIndices.ToStringCommaDelimited();
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerEmbedSSalgInSSalg(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerEmbedSSalgInSSalg");
  if (!input.IsListNElements(3))
    return output.MakeError("I expect two arguments - the two semisimple subalgebras.", theCommands);
  const Expression& EsmallSA=input[1];
  const Expression& ElargeSA=input[2];

  SemisimpleLieAlgebra* theSmallSapointer=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, EsmallSA, theSmallSapointer))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  SemisimpleLieAlgebra* thelargeSapointer=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, ElargeSA, thelargeSapointer))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  SemisimpleLieAlgebra& ownerSS=*thelargeSapointer;
  std::stringstream out;
  if (ownerSS.GetRank()>8)
  { out << "<b>This code is has been set to run up to ambient Lie algebra of rank 8. </b>";
    return output.AssignValue(out.str(), theCommands);
  } else
    out << "<b>This code is completely experimental. Use the following printouts on your own risk</b>";
  SemisimpleSubalgebras& theSSsubalgebras=
  theCommands.theObjectContainer.GetSemisimpleSubalgebrasCreateIfNotPresent(ownerSS.theWeyl.theDynkinType);
  theSSsubalgebras.ToStringExpressionString=CalculatorConversions::innerStringFromSemisimpleSubalgebras;

  out << "Attempting to embed " << theSmallSapointer->theWeyl.theDynkinType.ToString() << " in " << ownerSS.GetLieAlgebraName();
  theSSsubalgebras.FindTheSSSubalgebrasFromScratch(ownerSS, &theSmallSapointer->theWeyl.theDynkinType);
  return output.AssignValue(theSSsubalgebras, theCommands);
}

bool CalculatorFunctionsGeneral::innerWeylDimFormula(Calculator& theCommands, const Expression& input, Expression& output)
{ RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(3))
    return output.MakeError("This function takes 2 arguments", theCommands);
  SemisimpleLieAlgebra* theSSowner;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, input[1], theSSowner))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  Vector<RationalFunctionOld> theWeight;
  Expression newContext(theCommands);
//  stOutput << "<br>input[2] is: " << input[2].ToString();
  if (!theCommands.GetVectoR<RationalFunctionOld>(input[2], theWeight, &newContext, theSSowner->GetRank(), CalculatorConversions::innerRationalFunction))
    return output.MakeError("Failed to convert the argument of the function to a highest weight vector", theCommands);
  RationalFunctionOld rfOne;
  rfOne.MakeOne();
  Vector<RationalFunctionOld> theWeightInSimpleCoords;
  FormatExpressions theFormat;
  newContext.ContextGetFormatExpressions(theFormat);
  theWeightInSimpleCoords = theSSowner->theWeyl.GetSimpleCoordinatesFromFundamental(theWeight);
  //stOutput << "The fundamental coords: " << theWeight.ToString();
  theCommands << "<br>Weyl dim formula input: simple coords: " << theWeightInSimpleCoords.ToString(&theFormat) << ", fundamental coords: " << theWeight.ToString(&theFormat);
  RationalFunctionOld tempRF= theSSowner->theWeyl.WeylDimFormulaSimpleCoords(theWeightInSimpleCoords);
  //stOutput << "<br>The result: " << tempRF.ToString();
  return output.AssignValueWithContext(tempRF, newContext, theCommands);
}

bool CalculatorFunctionsGeneral::innerDecomposeFDPartGeneralizedVermaModuleOverLeviPart(Calculator& theCommands, const Expression& input, Expression& output)
{ RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(5))
    return output.MakeError("Function fDecomposeFDPartGeneralizedVermaModuleOverLeviPart expects 4 arguments.", theCommands);
  const Expression& typeNode=input[1];
  const Expression& weightNode=input[2];
  const Expression& inducingParNode=input[3];
  const Expression& splittingParNode=input[4];
  SemisimpleLieAlgebra* ownerSSPointer=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, typeNode, ownerSSPointer))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  Vector<RationalFunctionOld> theWeightFundCoords;
  Vector<Rational> inducingParSel, splittingParSel;
  SemisimpleLieAlgebra& ownerSS=*ownerSSPointer;
  WeylGroupData& theWeyl=ownerSS.theWeyl;
  int theDim=ownerSS.GetRank();
  Expression finalContext;
  if (!theCommands.GetVectoR<RationalFunctionOld>(weightNode, theWeightFundCoords, &finalContext, theDim, CalculatorConversions::innerRationalFunction))
    return output.MakeError("Failed to extract highest weight from the second argument.", theCommands);
  if (!theCommands.GetVectoR<Rational>(inducingParNode, inducingParSel, &finalContext, theDim, 0))
    return output.MakeError("Failed to extract parabolic selection from the third argument", theCommands);
  if (!theCommands.GetVectoR<Rational>(splittingParNode, splittingParSel, &finalContext, theDim, 0))
    return output.MakeError("Failed to extract parabolic selection from the fourth argument", theCommands);
  theCommands << "Your input weight in fundamental coordinates: " << theWeightFundCoords.ToString();
  theCommands << "<br>Your input weight in simple coordinates: " << theWeyl.GetSimpleCoordinatesFromFundamental(theWeightFundCoords).ToString()
  << "<br>Your inducing parabolic subalgebra: " << inducingParSel.ToString() << "." <<"<br>The parabolic subalgebra I should split over: " << splittingParSel.ToString() << ".";
  ModuleSSalgebra<RationalFunctionOld> theMod;
  Selection selInducing= inducingParSel;
  Selection selSplittingParSel=splittingParSel;
  theMod.MakeFromHW(ownerSS, theWeightFundCoords, selInducing, 1, 0, 0, false);
  std::string report;
  theMod.SplitOverLevi(&report, selSplittingParSel);
  return output.AssignValue(report, theCommands);
}

bool CalculatorFunctionsGeneral::innerSplitFDpartB3overG2Init(Calculator& theCommands, const Expression& input, Expression& output, branchingData& theG2B3Data, Expression& outputContext)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSplitFDpartB3overG2Init");
  if (!input.IsListNElements(4))
    return output.MakeError("Splitting the f.d. part of a B_3-representation over G_2 requires 3 arguments", theCommands);
  //stOutput << input.ToString();
  if (!theCommands.GetVectorFromFunctionArguments<RationalFunctionOld>(input, theG2B3Data.theWeightFundCoords, &outputContext, 3, CalculatorConversions::innerRationalFunction))
    output.MakeError("Failed to extract highest weight in fundamental coordinates. ", theCommands);
  theCommands.MakeHmmG2InB3(theG2B3Data.theHmm);
  theG2B3Data.selInducing.init(3);
  for (int i=0; i<theG2B3Data.theWeightFundCoords.size; i++)
    if (!theG2B3Data.theWeightFundCoords[i].IsSmallInteger())
      theG2B3Data.selInducing.AddSelectionAppendNewIndex(i);
  theG2B3Data.initAssumingParSelAndHmmInittedPart1NoSubgroups();
  return true;
}

bool CalculatorFunctionsGeneral::innerParabolicWeylGroups(Calculator& theCommands, const Expression& input, Expression& output)
{ Selection selectionParSel;
  SemisimpleLieAlgebra* theSSPointer;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, input, theSSPointer))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  SemisimpleLieAlgebra& theSSalgebra=*theSSPointer;
  int numCycles=MathRoutines::TwoToTheNth(selectionParSel.MaxSize);
  SubgroupWeylGroupOLD theSubgroup;
  std::stringstream out;
  for (int i=0; i<numCycles; i++, selectionParSel.incrementSelection())
  { theSubgroup.MakeParabolicFromSelectionSimpleRoots(theSSalgebra.theWeyl, selectionParSel, 2000);
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
  if(!theCommands.GetTypeHighestWeightParabolic(theCommands, input, output, theHWfundcoords, parabolicSel, hwContext, theSSalgPointer, CalculatorConversions::innerRationalFunction))
    return output.MakeError("Failed to extract highest weight vector data", theCommands);
  else
    if (output.IsError())
      return true;
  SemisimpleLieAlgebra& theSSalgebra=*theSSalgPointer;

  WeylGroupData& theAmbientWeyl=theSSalgebra.theWeyl;
  SubgroupWeylGroupOLD theSubgroup;
  std::stringstream out;
  std::stringstream outputFileContent, outputFileContent2;
  if (!theSubgroup.MakeParabolicFromSelectionSimpleRoots(theAmbientWeyl, parabolicSel, 500))
    return output.MakeError("<br><br>Failed to generate Weyl subgroup, 500 elements is the limit", theCommands);
  theSubgroup.FindQuotientRepresentatives(2000);
  out << "<br>Number elements of the coset: " << theSubgroup.RepresentativesQuotientAmbientOrder.size;
  out << "<br>Number of elements of the Weyl group of the Levi part: " << theSubgroup.size;
  out << "<br>Number of elements of the ambient Weyl: " << theSubgroup.AmbientWeyl->theGroup.theElements.size;
  outputFileContent << "\\documentclass{article}\\usepackage[all,cmtip]{xy}\\begin{document}\n";
  outputFileContent2 << "\\documentclass{article}\\usepackage[all,cmtip]{xy}\\begin{document}\n";
  FormatExpressions theFormat;
  hwContext.ContextGetFormatExpressions(theFormat);
  if (theSubgroup.size>498)
  { if (theSubgroup.AmbientWeyl->SizeByFormulaOrNeg1('E', 6) <= theSubgroup.AmbientWeyl->theGroup.GetSize())
      out << "Even I can't handle the truth, when it is so large. <br>";
    else
      out << "LaTeX can't handle handle the truth, when it is so large. <br>";
  } else
  { bool useJavascript=theSubgroup.size<100;
    outputFileContent << "\\[" << theSubgroup.ElementToStringBruhatGraph() << "\\]";
    outputFileContent << "\n\\end{document}";
    outputFileContent2 << "\\[" << theSubgroup.ElementToStringCosetGraph() << "\\]";
    outputFileContent2 << "\n\\end{document}";
    out << "<hr>"
    << " The Hasse graph of the Weyl group of the Levi part follows. <a href=\""
    << theGlobalVariables.DisplayNameExtraOutputNoPath << "1.tex\"> "
    << theGlobalVariables.DisplayNameExtraOutputNoPath << "1.tex</a>";
    out << ", <iframe src=\"" << theGlobalVariables.DisplayNameExtraOutputNoPath
    << "1.png\" width=\"800\" height=\"600\">"
    << theGlobalVariables.DisplayNameExtraOutputNoPath << "1.png</iframe>";
    out << "<hr> The coset graph of the Weyl group of the Levi part follows. The cosets are right, i.e. a coset "
    << " of the subgroup X is written in the form Xw, where w is one of the elements below. "
    << "<a href=\""
    << theGlobalVariables.DisplayNameExtraOutputNoPath
    << "2.tex\"> " << theGlobalVariables.DisplayNameExtraOutputNoPath << "2.tex</a>";
    out << ", <iframe src=\"" << theGlobalVariables.DisplayNameExtraOutputNoPath
    << "2.png\" width=\"800\" height=\"600\"> "
    << theGlobalVariables.DisplayNameExtraOutputNoPath << "2.png</iframe>";
    out <<"<b>The .png file might be bad if LaTeX crashed while trying to process it; "
    << "please check whether the .tex corresponds to the .png.</b>";
    out << "<hr>Additional printout follows.<br> ";
    out << "<br>Representatives of the coset follow. Below them you can find the elements of the subgroup. <br>";
    out << "<table><tr><td>Element</td><td>weight simple coords</td><td>weight fund. coords</td></tr>";
    theFormat.fundamentalWeightLetter="\\omega";
    for (int i=0; i<theSubgroup.RepresentativesQuotientAmbientOrder.size; i++)
    { ElementWeylGroup<WeylGroupData>& current=theSubgroup.RepresentativesQuotientAmbientOrder[i];
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
  out << theCommands.WriteDefaultLatexFileReturnHtmlLink(outputFileContent.str(), true);
  out << theCommands.WriteDefaultLatexFileReturnHtmlLink(outputFileContent2.str(), true);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerAllPartitions(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerAllPartitions");
  int theRank=-1;
  if (!input.IsSmallInteger(&theRank))
    return false;
  if (theRank>33 || theRank<0)
    return theCommands << "Partitions printouts are limited from n=0 to n=33, your input was: " << input.ToString();
  List<Partition> thePartitions;
  Partition::GetPartitions(thePartitions, theRank);
  std::stringstream out;
  out << "The partitions of " << theRank << " (total: " << thePartitions.size << ")"
  << ": ";
  for (int i=0; i<thePartitions.size; i++)
    out << "<br>" << thePartitions[i].ToString();
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerAllVectorPartitions(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerAllVectorPartitions");
  if (input.children.size!=3)
    return theCommands << "<hr>AllVectorPartitions function takes 3 arguments.";
  VectorPartition thePartition;
  const Expression& theVectorE=input[1];
  const Expression& thePartitioningVectorsE=input[2];
  if (!theCommands.GetVectoR(theVectorE, thePartition.goalVector))
    return theCommands << "<hr>Failed to extract vector from " << theVectorE.ToString();
  Matrix<Rational> vectorsMatForm;
  if (!theCommands.GetMatrix(thePartitioningVectorsE, vectorsMatForm, 0, thePartition.goalVector.size))
    return theCommands << "<hr>Failed to extract list of vectors from " << thePartitioningVectorsE.ToString();
  Vectors<Rational> theInputVectors;
  theInputVectors.AssignMatrixRows(vectorsMatForm);
  for (int i=0; i<theInputVectors.size; i++)
    if (!theInputVectors[i].IsPositive())
      return theCommands << "<hr>Input vector " << theInputVectors[i].ToString() << " is non-positive";
  if (!thePartition.init(theInputVectors, thePartition.goalVector))
    return theCommands << "<hr>Failed to initialize vector partition object";
//  stOutput << "<br>at start: " << thePartition.ToStringPartitioningVectors();
  std::stringstream out;
  int numFound=0;
  ProgressReport theReport;
  out << thePartition.ToStringPartitioningVectors();
  while (thePartition.IncrementReturnFalseIfPastLast())
  { out << "<br>" << thePartition.ToStringOnePartition(thePartition.currentPartition);
    numFound++;
    if (numFound%1000==0)
    { std::stringstream reportStream;
      reportStream << "Found " << numFound << " partitions of " << thePartition.goalVector.ToString()
      << "<br>Current partition: " << thePartition.currentPartition;
      theReport.Report(reportStream.str());
    }
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerDeterminant
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDeterminant");
  Matrix<Rational> matRat;
  bool isMatRat=input.IsOfType(&matRat);
  if (!isMatRat)
   isMatRat= theCommands.GetMatriXFromArguments(input, matRat, 0, 0, 0);
  if (isMatRat)
  { if (matRat.NumRows==matRat.NumCols)
    { if (matRat.NumRows>100)
        return theCommands << "<hr>I have been instructed not to compute "
        << "determinants of rational matrices larger than 100 x 100 "
        << ", and your matrix had " << matRat.NumRows << " rows. "
        << "To lift the restriction "
        << "edit function located in file " << __FILE__ << ", line " << __LINE__ << ". ";
      //stOutput << " <br> ... and the matRat is: " << matRat.ToString();
      return output.AssignValue(matRat.GetDeterminant(), theCommands);
    } else
    { theCommands << "Requesting to compute determinant of the non-square "
      << matRat.NumRows << " by "
      << matRat.NumCols << " matrix: " << input.ToString();
      return output.MakeError
      ("Requesting to compute determinant of non-square matrix. ", theCommands);
    }
  }
  Matrix<AlgebraicNumber> matAlg;
  bool isMatAlg=input.IsOfType(&matAlg);
  if (!isMatAlg)
    isMatAlg=theCommands.GetMatriXFromArguments(input, matAlg, 0,0,0);
  if (isMatAlg)
  { if (matAlg.NumRows==matAlg.NumCols)
    { if (matAlg.NumRows>100)
        return theCommands << "<hr>I have been instructed not to compute determinants of algebraic number matrices larger than 100 x 100 "
        << ", and your matrix had " << matAlg.NumRows << " rows. " << "To lift the restriction "
        << "edit function located in file " << __FILE__ << ", line " << __LINE__ << ". ";
      //stOutput << " <br> ... and the matRat is: " << matRat.ToString();
      return output.AssignValue(matAlg.GetDeterminant(), theCommands);
    } else
    { theCommands << "Requesting to compute determinant of the non-square "
      << matRat.NumRows << " by "
      << matRat.NumCols << " matrix: " << input.ToString();
      return output.MakeError("Requesting to compute determinant of non-square matrix. ", theCommands);
    }
  }
//  stOutput << "<hr>got to here";
  Matrix<RationalFunctionOld> matRF;
  Expression theContext;
  if (!input.IsOfType(&matRF))
  { if (!theCommands.GetMatriXFromArguments(input, matRF, &theContext, -1, CalculatorConversions::innerRationalFunction))
      return theCommands << "<hr>I have been instructed to only compute determinants of matrices whose entries are "
      << " rational functions or rationals, and I failed to convert your matrix to either type. "
      << " If this is not how you expect this function to act, correct it: the code is located in  "
      << " file " << __FILE__ << ", line " << __LINE__ << ". ";
  } else
    theContext= input.GetContext();
  //  stOutput << "<hr>got to here 2";
  if (matRF.NumRows==matRF.NumCols)
  { if (matRF.NumRows>50)
      return theCommands << "I have been instructed not to compute determinants of matrices of rational functions larger than "
      << " 50 x 50, and your matrix had " << matRF.NumRows << " rows. To lift the restriction edit function located in file "
      << __FILE__ << ", line " << __LINE__ << ". ";
    //stOutput << "<hr>DEBUG: determinant of " << matRF.ToString();
    RationalFunctionOld theDet=matRF.GetDeterminant();
    //stOutput << "<hr>Equals: " << theDet.ToString();
    return output.AssignValueWithContext(theDet, theContext, theCommands);
  } else
  { theCommands << "Requesting to compute determinant of the non-square "
    << matRat.NumRows << " by "
    << matRat.NumCols << " matrix: " << input.ToString();
    return output.MakeError("Requesting to compute determinant of non-square matrix given by:  "+input.ToString(), theCommands);
  }
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
      (theCommands, input, output, theHWfundcoords, parSel, theContext, theSSlieAlg, CalculatorConversions::innerRationalFunction))
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
  WeylGroupData& theWeyl=theSSlieAlg->theWeyl;
  if (!theKLpolys.ComputeKLPolys(&theWeyl))
    return output.MakeError("failed to generate Kazhdan-Lusztig polynomials (output too large?)", theCommands);
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
  if (!theSub.MakeParabolicFromSelectionSimpleRoots(theWeyl, parSel, 1000))
    return output.MakeError("Failed to generate Weyl subgroup of Levi part (possibly too large? element limit is 1000).", theCommands);
  theHWsimpCoords=theWeyl.GetSimpleCoordinatesFromFundamental(theHWfundcoords);
  List<ElementWeylGroup<WeylGroupData> > theWeylElements;
  theSub.GetGroupElementsIndexedAsAmbientGroup(theWeylElements);
  Vector<RationalFunctionOld> currentHW;
  out << "<br>Orbit modified with small rho: <table><tr><td>Simple coords</td><td>Fund coords</td></tr>";
  for (int i=0; i<theWeyl.theGroup.theElements.size; i++)
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
  { ElementWeylGroup<WeylGroupData>& currentElt=theWeylElements[i];
    out << "<tr><td>" << currentElt.ToString() << "</td>";
    int indexInWeyl=theKLpolys.TheWeylGroup->theGroup.theElements.GetIndex(currentElt);
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
        int sign= (currentElt.generatorsLastAppliedFirst.size- theWeyl.theGroup.theElements[j].generatorsLastAppliedFirst.size)%2==0 ? 1 :-1;
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
      (theCommands, input, output, theHWfundcoords, selectionParSel, hwContext, theSSalgebra, CalculatorConversions::innerRationalFunction))
    return output.MakeError("Failed to extract highest weight vector data", theCommands);
  else
    if (output.IsError())
      return true;
  if (!theCommands.innerHWVCommon(theCommands, output, theHWfundcoords, selectionParSel, hwContext, theSSalgebra, false))
    return output.MakeError("Failed to create Generalized Verma module", theCommands);
  if (output.IsError())
    return true;
  ElementTensorsGeneralizedVermas<RationalFunctionOld> theElt=output.GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
  ModuleSSalgebra<RationalFunctionOld>& theModule= *theElt[0].theMons[0].owner;
  return output.AssignValue(theModule.ToString(), theCommands);
}

bool CalculatorFunctionsGeneral::innerWriteGenVermaModAsDiffOperatorUpToLevel(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerWriteGenVermaModAsDiffOperatorUpToLevel");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(4))
    return output.MakeError("Function innerSplitGenericGenVermaTensorFD is expected to have three arguments: SS algebra type, Number, List{}. ", theCommands);
  const Expression& leftE=input[1];
  const Expression& genVemaWeightNode=input[3];
  const Expression& levelNode=input[2];
  Expression resultSSalgebraE;
  resultSSalgebraE=leftE;
  SemisimpleLieAlgebra* theSSalgebra;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, leftE, theSSalgebra))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  int theRank=theSSalgebra->GetRank();
  Vector<Polynomial<Rational> > highestWeightFundCoords;
  Expression hwContext(theCommands), emptyContext(theCommands);
  if (!theCommands.GetVectoR(genVemaWeightNode, highestWeightFundCoords, &hwContext, theRank, CalculatorConversions::innerPolynomial<Rational>))
    return theCommands
    << "Failed to convert the third argument of innerSplitGenericGenVermaTensorFD to a list of " << theRank
    << " polynomials. The second argument you gave is " << genVemaWeightNode.ToString() << ".";
  int desiredHeight;
  if (!levelNode.IsSmallInteger(&desiredHeight))
    return output.MakeError("second argument of " + input.ToString() + " must be a small integer", theCommands);

//  stOutput << "<br>highest weight in fundamental coords: " << highestWeightFundCoords.ToString() << "<br>";
//  stOutput << "<br>parabolic selection: " << parabolicSel.ToString();

//  int theNumVars=hwContext.VariableImages.size;
  RationalFunctionOld RFOne, RFZero;
  RFOne.MakeOne();
  RFZero.MakeZero();
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
//  stOutput << "Your input so far: " << theSSalgebra.GetLieAlgebraName() << " with hw: " << highestWeightFundCoords.ToString()
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
//  stOutput << "highest weights you are asking me for: " << theHws.ToString(&theFormat);
  return theCommands.innerWriteGenVermaModAsDiffOperatorInner
  (theCommands, input, output, theHws, hwContext, selInducing, theSSalgebra, false, 0, 0, 0, true, true);
}

bool CalculatorFunctionsGeneral::innerHWV(Calculator& theCommands, const Expression& input, Expression& output)
{ Selection selectionParSel;
  Vector<RationalFunctionOld> theHWfundcoords;
  Expression hwContext(theCommands);
  SemisimpleLieAlgebra* theSSalgebra=0;
  if(!theCommands.GetTypeHighestWeightParabolic(theCommands, input, output, theHWfundcoords, selectionParSel, hwContext, theSSalgebra, CalculatorConversions::innerRationalFunction) )
    return output.MakeError("Failed to extract highest weight vector data", theCommands);
  else
    if (output.IsError())
      return true;
  return theCommands.innerHWVCommon(theCommands, output, theHWfundcoords, selectionParSel, hwContext, theSSalgebra);
}

bool CalculatorFunctionsGeneral::innerSplitGenericGenVermaTensorFD(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSplitGenericGenVermaTensorFD");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(4))
    return output.MakeError
    ("Function innerSplitGenericGenVermaTensorFD is expected to have three arguments: SS algebra type, weight, weight. ", theCommands);
  const Expression& leftE=input[1];
  const Expression& genVemaWeightNode=input[3];
  const Expression& fdWeightNode=input[2];
  SemisimpleLieAlgebra* theSSalgebra;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, leftE, theSSalgebra))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  int theRank=theSSalgebra->GetRank();
  Vector<RationalFunctionOld> highestWeightFundCoords;
  Expression hwContext(theCommands);
  if (!theCommands.GetVectoR<RationalFunctionOld>(genVemaWeightNode, highestWeightFundCoords, &hwContext, theRank, CalculatorConversions::innerRationalFunction))
    return theCommands
    << "Failed to convert the third argument of innerSplitGenericGenVermaTensorFD to a list of " << theRank
    << " polynomials. The second argument you gave is " << genVemaWeightNode.ToString() << ".";
  Vector<Rational> theFDhw;
  if (!theCommands.GetVectoR<Rational>(fdWeightNode, theFDhw, 0, theRank, 0))
    return theCommands
    << "Failed to convert the second argument of innerSplitGenericGenVermaTensorFD to a list of " << theRank
    << " rationals. The second argument you gave is " << fdWeightNode.ToString() << ".";
//  stOutput << "<br>highest weight in fundamental coords: " << highestWeightFundCoords.ToString() << "<br>";
//  stOutput << "<br>parabolic selection: " << parabolicSel.ToString();
  int theNumVars=hwContext.ContextGetNumContextVariables();
  RationalFunctionOld RFOne, RFZero;
  RFOne.MakeOne();
  RFZero.MakeZero();
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
      return output.MakeError("Error: the third argument of innerSplitGenericGenVermaTensorFD must be a list of small non-negative integers.", theCommands);
  }
  theSSalgebra->flagHasNilradicalOrder=true;
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
  if (theGenMod.owner!=theFDMod.owner ||
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
  (&report, theGenMod.parabolicSelectionNonSelectedAreElementsLevi,
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
    currentChar.ModOutVermaRelations(& currentHWdualcoords, RFOne, RFZero);
    theCentralCharacters.AddOnTop(currentChar);
    out << "<tr><td>" << theFDLeviSplitShifteD[i].weightFundamentalCoordS.ToStringLetterFormat("\\psi") << "</td><td>"
    << currentChar.ToString(&tempFormat) << "</td></tr>";
    latexReport1 << " $" << theFDLeviSplitShifteD[i].weightFundamentalCoordS.ToStringLetterFormat("\\psi", &tempFormat) << "$"
    << "&$p_{" << i+1 <<"}=$ $" << currentChar.ToString(&tempFormat) << "$\\\\<br>";
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
  //stOutput << theGenMod.theGeneratingWordsNonReduced.ToString();
  for (int i=0; i<theCentralCharacters.size; i++)
  { Vector<RationalFunctionOld> currentWeightSimpleCoords=
    theSSalgebra->theWeyl.GetSimpleCoordinatesFromFundamental(theEigenVectorWeightsFund[i]);
    tempElt.MakeHWV(theFDMod, RFOne);
    tempElt.MultiplyOnTheLeft(theLeviEigenVectors[i], theElt, *theSSalgebra, RFOne);
    tempElt.MakeHWV(theGenMod, RFOne);
//      tempElt.MultiplyOnTheLeft
//      (theGenMod.theGeneratingWordsNonReduced[0], tempElt2, theMods, theSSalgebra,
//         theGlobalVariables,
//       RFOne, RFZero);
    theElt.TensorOnTheRight(tempElt);
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
        theElt.MultiplyOnTheLeft(theCasimirMinusChar, tempElt2, *theSSalgebra, RFOne);
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
//      stOutput << "<hr><hr>(" << theElt.ToString();
    Rational tempRat= theElt.ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy();
    currentHWsimplecoords=theGenMod.theHWSimpleCoordSBaseField;
    currentHWsimplecoords+=theFDMod.theModuleWeightsSimpleCoords[i];
//      stOutput << ") * " << tempRat.ToString()  << "<hr>=" << theElt.ToString() << "<hr><hr>";
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
    return output.MakeError("Function expects three arguments.", theCommands);
  Expression leftMerged=input[1];
  Expression rightMerged=input[2];
  if (!Expression::MergeContexts(leftMerged, rightMerged))
    return theCommands << "Could not get elements of universal enveloping algebra from inputs: " << input[1].ToString()
    << " and " << input[2].ToString();
  Expression finalContext=rightMerged.GetContext();
  int algebraIndex=finalContext.ContextGetIndexAmbientSSalg();
  if (algebraIndex==-1)
    return output.MakeError("I couldn't extract a Lie algebra to compute hwtaabf.", theCommands);

  SemisimpleLieAlgebra& constSSalg= theCommands.theObjectContainer.theSSLieAlgebras.theValues[algebraIndex];
  const Expression& weightExpression=input[3];
  Vector<RationalFunctionOld> weight;
  if (!theCommands.GetVectoR<RationalFunctionOld>(weightExpression, weight, &finalContext, constSSalg.GetRank(), CalculatorConversions::innerRationalFunction))
    return theCommands << "<hr>Failed to obtain highest weight from the third argument which is " << weightExpression.ToString();
  if (!leftMerged.SetContextAtLeastEqualTo(finalContext) || !rightMerged.SetContextAtLeastEqualTo(finalContext))
    return output.MakeError("Failed to merge the contexts of the highest weight and the elements of the Universal enveloping. ", theCommands);
  Expression leftConverted, rightConverted;
  if (!leftMerged.ConvertToType<ElementUniversalEnveloping<RationalFunctionOld> >(leftConverted))
    return false;
  if (!rightMerged.ConvertToType<ElementUniversalEnveloping<RationalFunctionOld> >(rightConverted))
    return false;
  const ElementUniversalEnveloping<RationalFunctionOld>& leftUE=leftConverted.GetValue<ElementUniversalEnveloping<RationalFunctionOld> >();
  const ElementUniversalEnveloping<RationalFunctionOld>& rightUE=rightConverted.GetValue<ElementUniversalEnveloping<RationalFunctionOld> >();
  WeylGroupData& theWeyl=constSSalg.theWeyl;
  std::stringstream out;
  Vector<RationalFunctionOld> hwDualCoords;
  constSSalg.OrderSSalgebraForHWbfComputation();
  hwDualCoords=theWeyl.GetDualCoordinatesFromFundamental(weight);
  RationalFunctionOld outputRF;
  //stOutput << "<br>The highest weight in dual coordinates, as I understand it:" << hwDualCoords.ToString();
  if(!leftUE.HWTAAbilinearForm(rightUE, outputRF, &hwDualCoords, 1, 0, &theCommands.Comments))
    return output.MakeError("Error: couldn't compute Shapovalov form, see comments.", theCommands);
  constSSalg.OrderStandardAscending();
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
  startingChar.SplitCharOverRedSubalg(&report, tempChar, theG2B3Data);
  out << report;
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerPrintB3G2branchingTableInit
(Calculator& theCommands, const Expression& input, Expression& output, branchingData& theG2B3data, int& desiredHeight, Expression& outputContext)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPrintB3G2branchingTableInit");
  if (input.children.size!=3)
    return output.MakeError("I need two arguments: first is height, second is parabolic selection. ", theCommands);
  desiredHeight=0;
  if (!input[1].IsSmallInteger(&desiredHeight))
    return output.MakeError("the first argument must be a small integer", theCommands);
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
  return theCommands.innerPrintB3G2branchingIntermediate(theCommands, input, output, theHWs, theG2B3Data, theContext);
}

bool Calculator::innerSplitFDpartB3overG2CharsOnly(Calculator& theCommands, const Expression& input, Expression& output)
{ branchingData theG2B3Data;
  return CalculatorFunctionsGeneral::innerSplitFDpartB3overG2CharsOutput(theCommands, input, output, theG2B3Data);
}

bool Calculator::innerPrintB3G2branchingTableCommon
(Calculator& theCommands, const Expression& input, Expression& output, Vectors<RationalFunctionOld>& outputHWs, branchingData& theG2B3Data, Expression& theContext)
{ MacroRegisterFunctionWithName("Calculator::innerPrintB3G2branchingTableCommon");
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
//  stOutput << " <br>the highest weights: " << outputHWs.ToString();
  return true;
}

bool Calculator::innerSplitFDpartB3overG2old(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerSplitFDpartB3overG2old");
  branchingData theG2B3Data;
  CalculatorFunctionsGeneral::innerSplitFDpartB3overG2CharsOutput(theCommands, input, output, theG2B3Data);
  if (output.IsError())
    return true;
  std::stringstream out;
  theCommands.innerSplitFDpartB3overG2inner(theCommands, theG2B3Data, output);
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
  out << "<br>Time final: " << theGlobalVariables.GetElapsedSeconds();
  return output.AssignValue(out.str(), theCommands);
}

bool Expression::EvaluatesToDoubleInRange
(const std::string& varName, double lowBound, double highBound, int numIntervals,
 double* outputYmin, double* outputYmax, Vectors<double>* outputPoints)const
{ MacroRegisterFunctionWithName("Expression::EvaluatesToDoubleInRange");
  if (numIntervals<1 || this->owner==0)
    return false;
  HashedList<Expression> knownEs= this->owner->knownDoubleConstants;
  List<double> knownValues=this->owner->knownDoubleConstantValues;
  Expression theVarNameE;
  theVarNameE.MakeAtom(varName, *this->owner);
  if (knownEs.Contains(theVarNameE))
    return *(this->owner) << "Variable name is an already known constant, variable name is bad.";
  knownEs.AddOnTop(theVarNameE);
  knownValues.AddOnTop(0);
  int numPoints=numIntervals+1;
  double delta=(highBound-lowBound)/(numIntervals);
  *knownValues.LastObject()=lowBound;
  double currentValue=0;
  if (outputPoints!=0)
    outputPoints->SetSize(numPoints);
  bool result=true;
  int numFailedEvaluations=0;
  for (int i=0; i<numPoints; i++)
  { //stOutput << "<br>Debug: evaluating to double under subs: ";
    if (i==numPoints-1)
      *knownValues.LastObject()=highBound; //correcting for floating point errors.
    if (!this->EvaluatesToDoubleUnderSubstitutions(knownEs, knownValues, &currentValue))
    { numFailedEvaluations++;
      if (numFailedEvaluations<5)
        *(this->owner) << "<br>Failed to evaluate " << this->ToString() << " at " << varName << "="
        << *knownValues.LastObject() << ". ";
      if (numFailedEvaluations==5)
        *(this->owner) << "<br>...";
      result=false;
      if (outputPoints!=0)
      { (*outputPoints)[i].SetSize(2);
        (*outputPoints)[i][0]=*knownValues.LastObject();
        (*outputPoints)[i][1]=NAN;
      }
      *knownValues.LastObject()+=delta;
      continue;
    }
    if (outputPoints!=0)
    { (*outputPoints)[i].SetSize(2);
      (*outputPoints)[i][0]=*knownValues.LastObject();
      (*outputPoints)[i][1]=currentValue;
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
  if (numFailedEvaluations>=5)
    (*this->owner) << "<br>" << numFailedEvaluations << " evaluations total failed. ";
  return result;
}

bool Expression::EvaluatesToDouble(double* whichDouble)const
{ return this->EvaluatesToDoubleUnderSubstitutions
  (this->owner->knownDoubleConstants, this->owner->knownDoubleConstantValues, whichDouble);
}

bool Expression::EvaluatesToDoubleUnderSubstitutions
(const HashedList<Expression>& knownEs, const List<double>& valuesKnownEs, double* whichDouble)const
{ MacroRegisterFunctionWithName("Expression::EvaluatesToDoubleUnderSubstitutions");
  if (this->owner==0)
    stOutput << "<br>Evaluating, but owner is zero!!! " << this->ToString();
  if (this->owner==0)
    return false;
//  stOutput << "<br>DEBUG: Evaluating to double: " << this->ToString();
  Calculator& theCommands=*this->owner;
  if (this->IsOfType<double>(whichDouble))
    return true;
  if (this->IsOfType<Rational>())
  { if (whichDouble!=0)
      *whichDouble=this->GetValue<Rational>().GetDoubleValue();
    return true;
  }
  if (this->IsOfType<AlgebraicNumber>())
    if (this->GetValue<AlgebraicNumber>().EvaluatesToDouble(whichDouble))
      return true;
  RecursionDepthCounter theCounter(&this->owner->RecursionDeptH);
  if (this->owner->RecursionDeptH >this->owner->MaxRecursionDeptH)
    return *(this->owner) << "<hr>Recursion depth exceeded while evaluating innerEvaluateToDouble."
    << " This may be a programming error. ";
  if (knownEs.Contains(*this))
  { if (whichDouble!=0)
      *whichDouble=valuesKnownEs[knownEs.GetIndex(*this)];
    return true;
  }

/*  if (this->IsAtomGivenData(theCommands.opE()))
    return output.AssignValue(MathRoutines::E(), theCommands);
  if (input.IsAtomGivenData(theCommands.opPi()))
  { //stOutput << "Evaluates to 3.1415...!";
    return output.AssignValue(MathRoutines::Pi(), theCommands);
  }*/
  bool isArithmeticOperationTwoArguments=
  this->StartsWith(theCommands.opTimes(),3) ||
  this->StartsWith(theCommands.opPlus(),3) ||
  this->StartsWith(theCommands.opMinus(),3) ||
  this->StartsWith(theCommands.opThePower(),3) ||
  this->StartsWith(theCommands.opDivide(),3) ||
  this->StartsWith(theCommands.opSqrt(),3)
  ;
  //stOutput << " is arithmetic operation two arguments! ";
  //if (input.StartsWith(theCommands.opSqrt()))
  //  stOutput << "Starting with sqrt: " << input.ToStringFull();
  if (isArithmeticOperationTwoArguments)
  { double leftD, rightD;
    if (!(*this)[1].EvaluatesToDoubleUnderSubstitutions(knownEs, valuesKnownEs, &leftD) ||
        !(*this)[2].EvaluatesToDoubleUnderSubstitutions(knownEs, valuesKnownEs, &rightD))
      return false;
    if ((*this).StartsWith(theCommands.opTimes(),3))
    { if (whichDouble!=0)
        *whichDouble=leftD*rightD;
      return true;
    }
    if ((*this).StartsWith(theCommands.opPlus(),3))
    { if (whichDouble!=0)
        *whichDouble=leftD+rightD;
      return true;
    }
    if ((*this).StartsWith(theCommands.opMinus(),3))
    { if (whichDouble!=0)
        *whichDouble=leftD-rightD;
      return true;
    }
    if ((*this).StartsWith(theCommands.opThePower(),3))
    { bool signChange=false;
      if (leftD<0)
      { Rational theRat;
        if ((*this)[2].IsRational(&theRat))
          if (!theRat.GetDenominator().IsEven())
          { if (!theRat.GetNumerator().IsEven())
              signChange=true;
            leftD*=-1;
          }
      }
      double tempDouble=0;
      if (whichDouble==0)
        whichDouble=&tempDouble;
      if (leftD==0 && rightD>0)
        *whichDouble=0;
      else
        *whichDouble=FloatingPoint::power(leftD, rightD);
      if (signChange)
        *whichDouble*=-1;
//      stOutput << "debug: here i am power " << leftD << " ^ " << rightD << " = " << *whichDouble << "<br>";
      return !std::isnan(*whichDouble);
    }
    if ((*this).StartsWith(theCommands.opSqrt(),3))
    { bool signChange=false;
      if (rightD<0)
      { Rational theRat;
        if ((*this)[1].IsRational(&theRat))
          if (!theRat.GetNumerator().IsEven())
          { if (!theRat.GetDenominator().IsEven())
              signChange=true;
            rightD*=-1;
          }
      }
//      stOutput << "debug: here i am";
      double tempDouble=0;
      if (whichDouble==0)
        whichDouble=&tempDouble;
      if (rightD==0 && leftD>0)
        *whichDouble=0;
      else
        *whichDouble=FloatingPoint::power(rightD,1/leftD);
      if (signChange)
        *whichDouble*=-1;
      return !std::isnan(*whichDouble);
    }
    if ((*this).StartsWith(theCommands.opDivide(),3))
    { if (rightD==0)
      { if (whichDouble!=0)
          *whichDouble=NAN;
        return false;
      }
      if (whichDouble!=0)
        *whichDouble=leftD/rightD;
      return true;
    }
    crash << "This is a piece of code which should never be reached. " << crash;
  }
  bool isKnownFunctionOneArgument=
  this->StartsWith(theCommands.opSin(),2) ||
  this->StartsWith(theCommands.opCos(),2) ||
  this->StartsWith(theCommands.opTan(),2) ||
  this->StartsWith(theCommands.opCot(),2) ||
  this->StartsWith(theCommands.opCsc(),2) ||
  this->StartsWith(theCommands.opSec(),2) ||
  this->StartsWith(theCommands.opArcTan(),2) ||
  this->StartsWith(theCommands.opArcCos(),2) ||
  this->StartsWith(theCommands.opArcSin(),2) ||
  this->StartsWith(theCommands.opSqrt(),2) ||
  this->StartsWith(theCommands.opLog(),2) ||
  this->StartsWith(theCommands.opAbsoluteValue(),2)
  ;

  if(isKnownFunctionOneArgument)
  { //stOutput << "GOT TO HJERE!";
    double argumentD;
    if (!(*this)[1].EvaluatesToDoubleUnderSubstitutions(knownEs, valuesKnownEs, &argumentD))
      return false;
    if (this->StartsWith(theCommands.opSqrt()))
    { if (argumentD<0)
        return false;
      if (whichDouble!=0)
        *whichDouble= FloatingPoint::sqrt(argumentD);
    }
    if (this->StartsWith(theCommands.opAbsoluteValue()))
      if (whichDouble!=0)
      { if (argumentD<0)
          *whichDouble=-argumentD;
        else
          *whichDouble=argumentD;
      }
    if (this->StartsWith(theCommands.opArcCos()))
    { if (argumentD>1 || argumentD<-1)
        return false;
      if (whichDouble!=0)
        *whichDouble= FloatingPoint::arccos(argumentD);
    }
    if (this->StartsWith(theCommands.opArcSin()))
    { if (argumentD>1 || argumentD<-1)
        return false;
      if (whichDouble!=0)
        *whichDouble= FloatingPoint::arcsin(argumentD);
    }
    if (this->StartsWith(theCommands.opSin()))
      if (whichDouble!=0)
        *whichDouble= FloatingPoint::sin(argumentD);
    if (this->StartsWith(theCommands.opCos()))
      if (whichDouble!=0)
        *whichDouble=FloatingPoint::cos(argumentD);
    if (this->StartsWith(theCommands.opTan()))
      if (whichDouble!=0)
      { double denominator=FloatingPoint::cos(argumentD);
        if (denominator==0)
          return false;
        *whichDouble=FloatingPoint::sin(argumentD)/denominator;
      }
    if (this->StartsWith(theCommands.opCot()))
      if (whichDouble!=0)
      { double denominator=FloatingPoint::sin(argumentD);
        if (denominator==0)
          return false;
        *whichDouble=FloatingPoint::cos(argumentD)/denominator;
      }
    if (this->StartsWith(theCommands.opCsc()))
      if (whichDouble!=0)
      { double denominator=FloatingPoint::sin(argumentD);
        if (denominator==0)
          return false;
        *whichDouble=1/denominator;
      }
    if (this->StartsWith(theCommands.opSec()))
      if (whichDouble!=0)
      { double denominator=FloatingPoint::cos(argumentD);
        if (denominator==0)
          return false;
        *whichDouble=1/denominator;
      }
    if (this->StartsWith(theCommands.opArcTan()))
      if (whichDouble!=0)
        *whichDouble=FloatingPoint::arctan(argumentD);
    if (this->StartsWith(theCommands.opLog()))
    { if (argumentD<=0)
        return false;
      if (whichDouble!=0)
        *whichDouble=FloatingPoint::log(argumentD);
    }
    return true;
  }
/*  stOutput << "<br>" << this->ToString() << " aint no double! constants are:";
  for (int i=0; i<knownEs.size; i++)
    stOutput << "<br>" << knownEs[i].ToString() << " -> " << valuesKnownEs[i];
*/
  return false;
}

bool CalculatorFunctionsGeneral::innerTestStandardOutput(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerTestStandardOutput");
  FormatExpressions tempFormat;
  tempFormat.flagExpressionIsFinal=true;
  stOutput << "<b>Standard output test. Input string bounced back on the following line.</b><br>" << input.ToString(&tempFormat);
  return output.AssignValue((std::string) "Standard output tested, verify your input is displayed correctly. ", theCommands);
}

bool CalculatorFunctionsGeneral::innerTestTopCommand
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerTestTopCommand");
  (void)input;  (void) theCommands;//portable way of avoiding unused parameter warning
  return output.AssignValue(theGlobalVariables.ToStringHTMLTopCommandLinuxSystem(), theCommands);
}

bool CalculatorFunctionsGeneral::innerTestIndicator(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerTestIndicator");
//  stOutput << "Report html return!";
  int numRuns=-1;
  if (!input.IsIntegerFittingInInt(&numRuns))
    return theCommands << "Argument of CalculatorFunctionsGeneral::innerTestIndicator is not a small integer, instead it equals: " << numRuns << ".";
  if (numRuns>200000)
  { theCommands << "The argument " << numRuns << " of CalculatorFunctionsGeneral::innerTestIndicator larger than 200000, trimming down to 200000.";
    numRuns=200000;
  }
  if (numRuns<0)
    numRuns=0;
  if (theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection!=0)
    theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection();
  else
    theCommands << "WebServerReturnDisplayIndicatorCloseConnection is zero.";
  ProgressReport theReport;

  for (int i=0; i<numRuns; i++)
  { std::stringstream reportStream;
    reportStream << " Running indicator test, " << i+1 << " out of " << numRuns << ".";
    theReport.Report(reportStream.str());
    if (theGlobalVariables.sleepFunction==0)
      crash << "fall asleep function is zero!" << crash;
    theGlobalVariables.FallAsleep(4000);
  }
  std::stringstream out;
  out << numRuns << " iterations of the indicator test executed. ";
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerRootSAsAndSltwos
(Calculator& theCommands, const Expression& input, Expression& output, bool showSLtwos, bool MustRecompute)
{ MacroRegisterFunctionWithName("Calculator::innerRootSAsAndSltwos");
  //bool showIndicator=true;
  SemisimpleLieAlgebra* ownerSS;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, input, ownerSS))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=10000;
  ownerSS->ComputeFolderNames();
  FormatExpressions theFormat;
  theFormat.flagUseHTML=true;
  theFormat.flagUseLatex=false;
  theFormat.flagUsePNG=true;

  std::stringstream outSltwoPath, outSltwoDisplayPath;
  std::stringstream outRootHtmlFileName, outRootHtmlDisplayName, outSltwoMainFile, outSltwoFileDisplayName;
  outSltwoPath << ownerSS->RelativePhysicalNameSSAlgOutputFolder << "sl2s/";
  outSltwoDisplayPath << ownerSS->DisplayNameSSalgOutputFolder << "sl2s/";
  outSltwoMainFile << outSltwoPath.str() << "sl2s.html";
  outSltwoFileDisplayName << outSltwoDisplayPath.str() << "sl2s.html";
  outRootHtmlFileName << ownerSS->RelativePhysicalNameSSAlgOutputFolder << "rootSubalgebras.html";
  outRootHtmlDisplayName << ownerSS->DisplayNameSSalgOutputFolder << "rootSubalgebras.html";
  bool NeedToCreateFolders=!FileOperations::FileExistsVirtual("output/"+outSltwoMainFile.str());
  if (NeedToCreateFolders)
  { std::stringstream outMkDirCommand1, outMkDirCommand2;
    outMkDirCommand1 << "mkdir " << theGlobalVariables.PhysicalPathHtmlFolder
    << ownerSS->RelativePhysicalNameSSAlgOutputFolder;
    outMkDirCommand2 << "mkdir " << theGlobalVariables.PhysicalPathHtmlFolder
    << outSltwoPath.str();
    theGlobalVariables.CallSystemNoOutput(outMkDirCommand1.str());
    theGlobalVariables.CallSystemNoOutput(outMkDirCommand2.str());
  }
  theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit =1000;
  if (!FileOperations::FileExistsVirtual("output/"+outSltwoMainFile.str()) ||
      !FileOperations::FileExistsVirtual("output/"+outRootHtmlFileName.str()))
    MustRecompute=true;
  std::stringstream out;
  if (MustRecompute)
  { //stOutput << theCommands.javaScriptDisplayingIndicator;
/*    if (theGlobalVariables.Fork())
      if (theGlobalVariables.ProcessIDforForking>0)//parent
      { theCommands.flagAbortComputationASAP=true;
        out << "<br>Computation of the root subalgebras is in progress. Below is a progress report. "
        << "Evaluation of all other commands has been stopped. " << theCommands.javaScriptDisplayingIndicator;
        stOutput << "parent is exiting...";
        stOutput.flush();
        return output.AssignValue(out.str(), theCommands);
      }
    stOutput << "child is computing ... ";
    stOutput.flush();*/
//    stOutput << "<br>The computation is in progress. <b><br>Please do not click back/refresh button: it will cause broken links in the calculator. "
//    << "<br>Appologies for this technical (Apache server configuration) problem. <br>Alleviating it is around the bottom of a very long to-do list.</b>"
//    << "<br> The computation is slow, up to around 10 minutes for E_8.<br>";
    SltwoSubalgebras theSl2s(*ownerSS);
    theSl2s.theRootSAs.flagPrintParabolicPseudoParabolicInfo=true;
//    stOutput << "ChangeME";
//    theSl2s.theRootSAs.flagPrintParabolicPseudoParabolicInfo=false;
    ownerSS->FindSl2Subalgebras(*ownerSS, theSl2s);
    std::string PathSl2= outSltwoPath.str();
    std::string DisplayPathSl2=outSltwoDisplayPath.str();
    theSl2s.ToHTML(&theFormat);
  } else
    out << "The table is precomputed and served from the hard disk. <br>";
//  out << "The full file name: " << outSltwoFileDisplayName.str();
  out << "<a href=\"" << (showSLtwos ? outSltwoFileDisplayName.str() : outRootHtmlDisplayName.str()) << "\">"
  << (showSLtwos ? outSltwoFileDisplayName.str() : outRootHtmlDisplayName.str()) << " </a>";
//  out << "<meta http-equiv=\"refresh\" content=\"1; url=" << (showSLtwos ? outSltwoFileDisplayName.str() : outRootHtmlDisplayName.str()) << "\">";
  return output.AssignValue(out.str(), theCommands);
}

class LaTeXcrawler
{
  public:
  int recursionDepth;
  std::string theFileToCrawlPhysical;
  std::string theFileToCrawlRelative;
  std::string baseFolderStartFilePhysical;
  std::string theFileToCrawlNoPathPhysical;
  std::string theFileWorkingCopy;
  Calculator* owner;
  bool ExtractFileNamesFromRelativeFileName();
  void Crawl();
  void BuildFreecalc();
  void CrawlRecursive(const std::string& currentFileName);
  std::stringstream crawlingResult;
  std::stringstream displayResult;
  std::stringstream errorStream;
  std::string ToString();
};

bool LaTeXcrawler::ExtractFileNamesFromRelativeFileName()
{ MacroRegisterFunctionWithName("LaTeXcrawler::ExtractFileNamesFromRelativeFileName");
  if (!FileOperations::IsOKfileNameVirtual(this->theFileToCrawlRelative))
  { this->displayResult << "The folders below the file name contain dots. This is not allowed. ";
    return false;
  }
  this->theFileToCrawlPhysical=theGlobalVariables.PhysicalPathProjectBase+ "../freecalc/" +
  this->theFileToCrawlRelative;
  this->baseFolderStartFilePhysical=FileOperations::GetPathFromFileNameWithPath(this->theFileToCrawlPhysical);
  MathRoutines::StringBeginsWith
  (this->theFileToCrawlPhysical, this->baseFolderStartFilePhysical, &this->theFileToCrawlNoPathPhysical);
  return true;
}

void LaTeXcrawler::BuildFreecalc()
{ MacroRegisterFunctionWithName("LaTeXcrawler::BuildFreecalc");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
  { this->displayResult << "Build freecalc command allowed only for logged-in admins.";
    return;
  }
  theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=50000;
  if (!this->ExtractFileNamesFromRelativeFileName())
    return;
  std::fstream inputFile;
  if (!FileOperations::OpenFileUnsecure(inputFile, this->theFileToCrawlPhysical, false, false, false))
  { this->displayResult << "Failed to open input file: " << this->theFileToCrawlPhysical << ", aborting.";
    return;
  }
  List<std::string> theLectureNumbers, theLectureDesiredNames;
  inputFile.seekg(0);
  std::string buffer;
  ProgressReport theReport;
  std::stringstream reportStream;
  reportStream << "Processing input file: extracting lecture numbers...";
  theReport.Report(reportStream.str());
//  stOutput << "Processing input file: extracting lecture numbers.";
  bool isLecturE =false;
  bool isHW=false;
  while (!inputFile.eof())
  { std::getline(inputFile, buffer);
    bool isInput=false;
    if (MathRoutines::StringBeginsWith(buffer, "\\lect"))
    { isInput=true;
      isLecturE=true;
    }
    if (MathRoutines::StringBeginsWith(buffer, "\\homeworkOnATopic"))
    { isInput=true;
      isHW=true;
    }
    if (!isInput)
      continue;
    int leftBracketsMinusRight=0;
    int numBallancedBracketGroups=0;
    std::string lectureNumber;
    bool recordingLectureNumber=false;
    for (unsigned i=0; i<buffer.size(); i++)
    { if (buffer[i]=='}')
      { leftBracketsMinusRight--;
        if (leftBracketsMinusRight==0)
        { numBallancedBracketGroups++;
          if (recordingLectureNumber)
            break;
        }
      }
      if (buffer[i]=='{')
      { leftBracketsMinusRight++;
        if (leftBracketsMinusRight==1 && numBallancedBracketGroups==2)
          recordingLectureNumber=true;
      }
      if (recordingLectureNumber)
        lectureNumber.push_back(buffer[i]);
    }
    if (lectureNumber.size()>0)
      theLectureNumbers.AddOnTop(lectureNumber.substr(1, lectureNumber.size()-1));
    else
    { this->displayResult << "Failed to extract lecture/homework number from line: " << buffer;
      return;
    }
    std::getline(inputFile, buffer);
    std::string desiredName;
    if (!MathRoutines::StringBeginsWith(buffer, "%DesiredLectureName: ", &desiredName))
      if(!MathRoutines::StringBeginsWith(buffer, "%DesiredHomeworkName: ", &desiredName))
        desiredName="";
    for (unsigned i=0; i<desiredName.size(); i++)
      if (desiredName[i]== '.' || desiredName[i]=='/' || desiredName[i]=='\\')
        desiredName="";
    if (desiredName=="")
      this->displayResult << "Failed to extract desired homework/lecture name from: " << buffer
      << "<br>This is the line immediately after the \\lect command. It should begin with the string \"%DesiredLectureName: \""
      << "(<-has space bar in the end). The name itself should not contain the characters . / or \\. "
      << "I am assigning an automatic file name. <br>";
    theLectureDesiredNames.AddOnTop(desiredName);
  }
  reportStream << " done. Extracted: " << theLectureNumbers.size << " homework/lecture numbers. Preparing Homework/Lecture content ... ";
  theReport.Report(reportStream.str());
//  stOutput << reportStream.str();
  if (isLecturE && isHW)
  { this->displayResult << "I was not able to determine whether the file is a homework or a lecture file. Aborting.";
    return;
  }
  if (!isLecturE && !isHW)
  { this->displayResult << "Could not find any lecture or homework entries. Aborting.";
    return;
  }
  std::stringstream LectureContentNoDocumentClassNoCurrentLecture;
  inputFile.clear();
  inputFile.seekg(0);
  if (isLecturE)
    while (!inputFile.eof())
    { std::getline(inputFile, buffer);
      if (!MathRoutines::StringBeginsWith(buffer, "\\documentclass") &&
          !MathRoutines::StringBeginsWith(buffer, "[handout]") &&
          !MathRoutines::StringBeginsWith(buffer, "{beamer}") &&
          !MathRoutines::StringBeginsWith(buffer, "\\newcommand{\\currentLecture}")
         )
        LectureContentNoDocumentClassNoCurrentLecture << buffer << "\n";
    }
  else
    while (!inputFile.eof())
    { std::getline(inputFile, buffer);
      if (!MathRoutines::StringBeginsWith(buffer, "\\newcommand{\\currentHW}"))
        LectureContentNoDocumentClassNoCurrentLecture << buffer << "\n";
    }
  reportStream << " done. Proceding to compile homeworks/lectures. ";
  theReport.Report(reportStream.str());
//  stOutput << reportStream2.str();
  if(isLecturE)
    this->displayResult << "<table><tr><td>Lecture number</td><td>Lecture name</td><td>Lecture pdf</td>"
    << "<td>Lecture handout pdf</td><td>Comments</td></tr>";
  else
    this->displayResult << "<table><tr><td>Homework number</td><td>Homework name</td><td>Homework pdf</td>"
    << "<td>Homework handout pdf</td><td>Comments</td></tr>";
  std::string lectureFileNameEnd;
  if (!MathRoutines::StringBeginsWith(this->theFileToCrawlNoPathPhysical, "Lecture_", &lectureFileNameEnd))
    if (!MathRoutines::StringBeginsWith(this->theFileToCrawlNoPathPhysical, "Homework_", &lectureFileNameEnd))
      lectureFileNameEnd="";
  std::string folderEnd=this->theFileToCrawlNoPathPhysical.substr(0, this->theFileToCrawlNoPathPhysical.size()-4);
  std::string printableFolder;
  std::string lectureProjectorFolder="lectures_projector_" + folderEnd + "/";
  if (!isLecturE)
    printableFolder="homework_" + folderEnd + "/";
  else
    printableFolder="lectures_printable_" + folderEnd + "/";
  if (lectureFileNameEnd.size()>4)
    lectureFileNameEnd=lectureFileNameEnd.substr(0, lectureFileNameEnd.size()-4);
  std::stringstream executedCommands, resultTable;
  std::string currentSysCommand; //"ch " + this->baseFolderStartFilePhysical+"\n\n\n\n";
  //executedCommands << "<br>" << currentSysCommand;
  theGlobalVariables.ChDir(this->baseFolderStartFilePhysical);
  executedCommands << "Commands executed: "
  << "<br>Directory changed: " << this->baseFolderStartFilePhysical;
  reportStream << "<br>Directory changed: " << this->baseFolderStartFilePhysical;
  theGlobalVariables.CallSystemNoOutput("mkdir "+printableFolder);
  if (isLecturE)
    theGlobalVariables.CallSystemNoOutput("mkdir "+lectureProjectorFolder);
  //this->theFileWorkingCopy=this->baseFolderStartFilePhysical+ "working_file_"+ this->theFileToCrawlNoPathPhysical;
  //std::string theFileWorkingCopyPDF=this->baseFolderStartFilePhysical+ "working_file_"
  //+this->theFileToCrawlNoPathPhysical.substr(0, this->theFileToCrawlNoPathPhysical.size()-3)+"pdf";
  this->theFileWorkingCopy= "working_file_"+ this->theFileToCrawlNoPathPhysical;
  std::string theFileWorkingCopyPDF= "working_file_"
  +this->theFileToCrawlNoPathPhysical.substr(0, this->theFileToCrawlNoPathPhysical.size()-3)+"pdf";
  for (int i=0; i<
//1
  theLectureNumbers.size
  ; i++)
  { reportStream << "<br>Processing lecture " << i+1 << " out of " << theLectureNumbers.size << ". ";
    resultTable << "<tr>";
    resultTable << "<td>" << theLectureNumbers[i] << "</td>";
    resultTable << "<td>" << theLectureDesiredNames[i] << "</td>";
    std::fstream workingFile;
    if (!FileOperations::OpenFileCreateIfNotPresentUnsecure(workingFile, this->theFileWorkingCopy, false, true, false))
    { resultTable << "<td>-</td><td>-</td><td>Failed to open working file: "
      << this->theFileWorkingCopy << ", aborting.</td> </tr>";
      break;
    }
    if (isLecturE)
      workingFile << "\\documentclass[handout]{beamer}\n\\newcommand{\\currentLecture}{"
      << theLectureNumbers[i] << "}\n"
      << LectureContentNoDocumentClassNoCurrentLecture.str();
    else
      workingFile << "\\newcommand{\\currentHW}{" << theLectureNumbers[i] << "}\n"
      << LectureContentNoDocumentClassNoCurrentLecture.str();
    currentSysCommand="pdflatex -shell-escape "+this->theFileWorkingCopy;
    executedCommands << "<br>" << currentSysCommand;
    reportStream << currentSysCommand;
    theReport.Report(reportStream.str());
    theGlobalVariables.CallSystemNoOutput(currentSysCommand);
    reportStream << "<b>[x2]</b>";
    theReport.Report(reportStream.str());
    theGlobalVariables.CallSystemNoOutput(currentSysCommand);
    std::stringstream thePdfFileNameHandout;
    if (isLecturE)
    { thePdfFileNameHandout << "./" << printableFolder << "Lecture" << theLectureNumbers[i] << "Handout_"
      << theLectureDesiredNames[i];
      if (lectureFileNameEnd!="")
        thePdfFileNameHandout << "_" << lectureFileNameEnd;
      thePdfFileNameHandout << ".pdf";
    } else
    { thePdfFileNameHandout << "./" << printableFolder << "Homework" << theLectureNumbers[i] << "_" << theLectureDesiredNames[i];
      if (lectureFileNameEnd!="")
        thePdfFileNameHandout << "_" << lectureFileNameEnd;
      thePdfFileNameHandout << ".pdf";
    }
    currentSysCommand="mv " +theFileWorkingCopyPDF+" " + thePdfFileNameHandout.str();
    executedCommands << "<br>" << currentSysCommand;
    reportStream << "<br>Lecture/Homework " << i+1 << " handout compiled, renaming file ... ";
    theReport.Report(reportStream.str());
    theGlobalVariables.CallSystemNoOutput(currentSysCommand);
    reportStream << " done.";
    theReport.Report(reportStream.str());
    workingFile.close();
    if (!isLecturE)
    { theReport.Report(reportStream.str());
      resultTable << "<td>" << thePdfFileNameHandout.str() << "</td>";
      resultTable << "</tr>";
      continue;
    }
    if (!FileOperations::OpenFileUnsecure(workingFile, this->theFileWorkingCopy, false, true, false))
    { resultTable << "<td>-</td><td>-</td><td>Failed to open working file: "
      << this->theFileWorkingCopy << ", aborting.</td> </tr>";
      break;
    }
    workingFile << "\\documentclass{beamer}\n\\newcommand{\\currentLecture}{"
    << theLectureNumbers[i] << "}\n";
    workingFile << LectureContentNoDocumentClassNoCurrentLecture.str();
    currentSysCommand="pdflatex -shell-escape "+this->theFileWorkingCopy;
    executedCommands << "<br>" << currentSysCommand;
    reportStream << currentSysCommand;
    theReport.Report(reportStream.str());
    theGlobalVariables.CallSystemNoOutput(currentSysCommand);
    std::stringstream thePdfFileNameNormal;
    thePdfFileNameNormal << "./" << lectureProjectorFolder << "Lecture"
    << theLectureNumbers[i] << "_" << theLectureDesiredNames[i] << "_"
    << lectureFileNameEnd << ".pdf";
    currentSysCommand= "mv " +theFileWorkingCopyPDF + " " + thePdfFileNameNormal.str();
    executedCommands << "<br>" << currentSysCommand;
    reportStream << "<br>Lecture " << i+1 << " regular slides compiled, renaming file ... ";
    theReport.Report(reportStream.str());
    theGlobalVariables.CallSystemNoOutput(currentSysCommand);
    reportStream << " done.";
    theReport.Report(reportStream.str());
    resultTable << "<td>" << thePdfFileNameNormal.str() << "</td>" << "<td>" << thePdfFileNameHandout.str() << "</td>";
    resultTable << "</tr>";
  }
  resultTable << "</table>";
  this->displayResult << executedCommands.str() << "<br>" << resultTable.str();
  theGlobalVariables.ChDir("../");
}

void LaTeXcrawler::Crawl()
{ MacroRegisterFunctionWithName("LaTeXcrawler::Crawl");
  if (!this->ExtractFileNamesFromRelativeFileName())
    return;
  this->recursionDepth=0;
  this->CrawlRecursive(this->theFileToCrawlPhysical);
  std::fstream outputFile;
  std::string outputFileName = "latexOutput.tex";
  if (!FileOperations::OpenFileCreateIfNotPresentVirtual(outputFile,"output/"+ outputFileName, false, true, false))
  { this->displayResult << "Failed to open output file: " << outputFileName << ", check write permissions. ";
    //this->displayResult << "The crawling appears to have been successful, below is the crawled file.<hr>"
    //<< this->crawlingResult.str();
    return;
  }
  outputFile << this->crawlingResult.str();
  if (this->errorStream.str()!="")
    this->displayResult << "Errors encountered. " << this->errorStream.str();
  this->displayResult << "Output file: <a href=\"" << theGlobalVariables.DisplayPathOutputFolder
  << "latexOutput.tex\">" << "latexOutput.tex" << "</a>";
}

void LaTeXcrawler::CrawlRecursive(const std::string& currentFileName)
{ MacroRegisterFunctionWithName("LaTeXcrawler::CrawlRecursive");
  RecursionDepthCounter theCounter(&this->recursionDepth);
  if (this->recursionDepth>1000)
  { this->errorStream << "While crawling theFileToCrawl, reached max recursion depth of 1000";
    return;
  }
  if (this->errorStream.str()!="")
    return;
  std::fstream theFile;
  if (!FileOperations::OpenFileUnsecure(theFile, currentFileName, false, false, false))
  { this->errorStream << "Failed to open file " << currentFileName << ", aborting.";
    return;
  }
  std::string buffer;
  while (!theFile.eof())
  { std::getline(theFile, buffer);
    std::stringstream newFileName;
    int foundInput= buffer.find("\\input");
    if (((unsigned) foundInput)<buffer.size() )
    { this->crawlingResult << buffer.substr(0, foundInput);
      buffer=buffer.substr(foundInput);
      newFileName << this->baseFolderStartFilePhysical;
      unsigned i=0;
      for (i=7; buffer[i]!='}' && i< buffer.size();i++)
        newFileName << buffer[i];
      newFileName << ".tex";
      this->crawlingResult << "%input from file: " << newFileName.str() << "\n";
      this->CrawlRecursive(newFileName.str());
      this->crawlingResult << "\n";
      if (i+1<buffer.size())
        buffer = buffer.substr(i+1);
      else
        continue;
    }
    this->crawlingResult << buffer;
    this->crawlingResult << "\n";
  }
}

bool CalculatorFunctionsGeneral::innerCrawlTexFile(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCrawlTexFile");
  if (!input.IsOfType<std::string>())
    return theCommands << "<hr>Input " << input.ToString() << " is not of type string. ";
  LaTeXcrawler theCrawler;
  theCrawler.owner=&theCommands;
  theCrawler.theFileToCrawlRelative=input.GetValue<std::string>();
  theCrawler.Crawl();
  return output.AssignValue(theCrawler.displayResult.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerBuildFreecalc(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCrawlTexFile");
  if (!input.IsOfType<std::string>())
    return theCommands << "<hr>Input " << input.ToString() << " is not of type string. ";
  LaTeXcrawler theCrawler;
  theCrawler.owner=&theCommands;
  theCrawler.theFileToCrawlRelative=input.GetValue<std::string>();
  theCrawler.BuildFreecalc();
  return output.AssignValue(theCrawler.displayResult.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerSetOutputFile(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSetOutputFile");
  std::string theFileName;
  std::stringstream out;
  if (!input.IsOfType<std::string>(&theFileName))
    return theCommands << "<hr>Input " << input.ToString() << " is not of type string. ";
  for (unsigned i=0; i<theFileName.size(); i++)
    if (theFileName[i]=='.' || theFileName[i]=='\\' || theFileName[i]=='/')
    { out << theFileName << " rejected as a file name as it contains the characters one of the three characters .\\/  ";
      return output.AssignValue(out.str(), theCommands);
    }
  theGlobalVariables.initOutputReportAndCrashFileNames(theFileName, theCommands.inputString);
  out << "The default output filename has been changed to " << theFileName << ".";
  return output.AssignValue(out.str(), theCommands);

}

bool CalculatorFunctionsGeneral::innerFindProductDistanceModN(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerFindProductDistanceModN");
  if (input.children.size!=3)
    return theCommands << "<hr>Product distance f-n takes as input 2 arguments, modulo and a list of integers";
  const Expression& theModuloE= input[1];
  const Expression& theIntegersE=input[2];
  int theSize;
  if (!theModuloE.IsIntegerFittingInInt(&theSize))
    return theCommands << " <hr> Failed to extract modulus from " << theModuloE.ToString();
  if (theSize==0)
    return theCommands << "<hr>Zero modulus not allowed.";
  if (theSize<0)
    theSize*=-1;
  if (theSize>10000000)
    return theCommands << "<hr>I've been instructed to compute with moduli no larger than 10000000.";
  List<int> theInts, theIntsReduced;
  if (!theCommands.GetVectoRInt(theIntegersE, theInts))
    return theCommands << "<hr>Failed to extract integer list from " << theIntegersE.ToString();
  theIntsReduced.SetSize(theInts.size);
  for (int i=0; i<theInts.size; i++)
  { if (theInts[i]<=0)
      return theCommands << "<hr>The integer list " << theInts << " contains non-positive numbers.";
    theIntsReduced[i]=theInts[i]% theSize;
  }
  List<LargeIntUnsigned> theList;
  List<int> theIndexStack;
  theList.initFillInObject(theSize, 0);
  theIndexStack.Reserve(theSize);
  LargeIntUnsigned theMod;
  theMod=theSize;
  int numElementsCovered=0;
  for (int i=0; i<theInts.size; i++)
  { if (theList[theIntsReduced[i]]==0)
      numElementsCovered++;
    theList[theIntsReduced[i]]=theInts[i];
    theIndexStack.AddOnTop(theIntsReduced[i]);
  }
  LargeIntUnsigned currentIndexLarge, currentDistance, maxDistanceGenerated;
  int currentIndex;
  ProgressReport theReport;
  ProgressReport theReport0;
  std::stringstream reportstream;
  reportstream << "Finding product distance mod " << theMod.ToString() << " w.r.t. elements "
  << theInts;
  int numElementsNotAddedToStack=0;
  maxDistanceGenerated=0;
  for (int i=0; i<theIndexStack.size; i++)
  { for (int j=0; j<theIntsReduced.size; j++)
    { currentIndexLarge=theIndexStack[i];
      currentIndexLarge *=theIntsReduced[j];
      currentIndexLarge%=theMod;
      if (!currentIndexLarge.IsIntegerFittingInInt(&currentIndex))
        return theCommands << "An internal check has failed. This shouldn't happen, this is possibly a programming bug.";
      currentDistance=theList[theIndexStack[i]];
      currentDistance+=theIntsReduced[j];
      if (theList[currentIndex]>0 )
        if (theList[currentIndex]<currentDistance )
        { numElementsNotAddedToStack++;
          if (numElementsNotAddedToStack%50000==0)
          { std::stringstream out;
            out << "While computing product distance, explored " << i+1 << " out of "
            << theIndexStack.size << " indices. " << numElementsNotAddedToStack << " candidates were not added to the stack. "
            << "Number of elements reached: " << numElementsCovered << ". "
            << "Max distance generated while searching: " << maxDistanceGenerated.ToString();
            theReport.Report(out.str());
          }
          continue;
        }
      if (theList[currentIndex]==0)
        numElementsCovered++;
      theList[currentIndex]=currentDistance;
      if (currentDistance>maxDistanceGenerated)
        maxDistanceGenerated=currentDistance;
      theIndexStack.AddOnTop(currentIndex);
      if (theIndexStack.size%10000==0)
      { std::stringstream out;
        out << "While computing product distance, explored " << i+1 << " out of "
        << theIndexStack.size << " indices. " << numElementsNotAddedToStack << " candidates were not added to the stack. "
        << "Number of elements reached: " << numElementsCovered << ". "
        << "Max distance generated while searching: " << maxDistanceGenerated.ToString();
        theReport.Report(out.str());
      }
    }
    if (theIndexStack.size>1000000000)
      return theCommands << "While computing product distance, exceeded allowed stack size of 1000000000";
  }
  std::stringstream out;
  for (int i=0; i<theList.size; i++)
    out << "<br>" << i << ": " << theList[i].ToString();
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerSolveProductSumEquationOverSetModN(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSolveProductSumEquationOverSetModN");
  Expression theModuloE, theIntegersE, theProductE, theSumE;
  if (!CalculatorConversions::innerLoadKey(theCommands, input, "theMod", theModuloE))
    return theCommands << "<hr>Value theMod not found.";
  int theMod;
  if (!theModuloE.IsIntegerFittingInInt(&theMod))
    return theCommands << " <hr> Failed to extract modulus from " << theModuloE.ToString();
  if (theMod==0)
    return theCommands << "<hr>Zero modulus not allowed.";
  if (theMod<0)
    theMod*=-1;
  if (theMod>10000000)
    return theCommands << "<hr>I've been instructed to compute with moduli no larger than 10000000.";
  if (!CalculatorConversions::innerLoadKey(theCommands, input, "theSet", theIntegersE))
    return theCommands << "<hr>Value theSet not found.";
  List<int> theInts;
  if (!theCommands.GetVectoRInt(theIntegersE, theInts))
    return theCommands << "<hr>Failed to extract integer list from " << theIntegersE.ToString();
  for (int i=0; i<theInts.size; i++)
    if (theInts[i]<=0)
      return theCommands << "<hr>The integer list " << theInts << " contains non-positive numbers.";
  if (!CalculatorConversions::innerLoadKey(theCommands, input, "theProduct", theProductE))
    return theCommands << "<hr>Value theProduct not found.";
  LargeInt goalProduct;
  if (!theProductE.IsInteger(&goalProduct))
    return theCommands << "<hr>Failed to extract integer from " << theProductE.ToString();
  if (goalProduct.IsNegative())
    return theCommands << "<hr>I am expecting a positive product as input. ";
  if (!CalculatorConversions::innerLoadKey(theCommands, input, "theSum", theSumE))
    return theCommands << "<hr>Value theSum not found.";
  int theSum=-1;
  if (!theSumE.IsSmallInteger(&theSum))
    return theCommands << "Failed to extract small integer from " << theSumE.ToString();
  VectorPartition thePartition;
  Vectors<Rational> theOneDimVectors;
  theOneDimVectors.SetSize(theInts.size);
  for (int i=0; i<theInts.size; i++)
  { theOneDimVectors[i].MakeZero(1);
    theOneDimVectors[i][0]=theInts[i];
  }
  thePartition.goalVector.MakeZero(1);
  thePartition.goalVector[0]=theSum;
  if(!thePartition.init(theOneDimVectors, thePartition.goalVector))
    return theCommands << "Failed to initialize the computation. ";
  LargeIntUnsigned theModLarge;
  theModLarge=theMod;
  int numTestedSoFar=0;
  ProgressReport theReport;
  LargeIntUnsigned oneUI=1;
  while (thePartition.IncrementReturnFalseIfPastLast())
  { LargeIntUnsigned theProduct=1;
    for (int i=0; i<thePartition.currentPartition.size; i++)
    { LargeIntUnsigned theNumber=theInts[i];
      MathRoutines::RaiseToPower(theNumber, thePartition.currentPartition[i], oneUI);
      theProduct*=theNumber;
      theProduct%=theModLarge;
    }
    if (theProduct==goalProduct.value)
    { std::stringstream out;
      out << "Found one solution: ";
      for (int i=0; i<thePartition.currentPartition.size; i++)
        if (thePartition.currentPartition[i]>0)
        { out << theInts[i];
          if (thePartition.currentPartition[i]>1)
            out << "^{" << thePartition.currentPartition[i] << "}";
          out << " ";
        }
      return output.AssignValue(out.str(), theCommands);
    }
    numTestedSoFar++;
    std::stringstream reportStream;
    reportStream << numTestedSoFar << " tested so far ...";
    theReport.Report(reportStream.str());
  }
  return output.AssignValue((std::string)"Couldn't find solution", theCommands);
}

void Calculator::AutomatedTestRun
(List<std::string>& outputCommandStrings, List<std::string>& outputResultsWithInit, List<std::string>& outputResultsNoInit)
{ MacroRegisterFunctionWithName("Calculator::AutomatedTestRun");
  Calculator theTester;
  int numFunctionsToTest=this->GetNumBuiltInFunctions();
  outputCommandStrings.SetExpectedSize(numFunctionsToTest);
  outputCommandStrings.SetSize(0);
  for (int i=0; i<this->FunctionHandlers.size; i++)
    for (int j=0; j<this->FunctionHandlers[i].size; j++)
      if (this->FunctionHandlers[i][j].theFunction!=Calculator::innerAutomatedTest &&
          this->FunctionHandlers[i][j].theFunction!=Calculator::innerAutomatedTestSetKnownGoodCopy &&
          this->FunctionHandlers[i][j].theFunction!=CalculatorFunctionsGeneral::innerCrash &&
          this->FunctionHandlers[i][j].theFunction!=CalculatorFunctionsGeneral::innerCrashByListOutOfBounds &&
          ! this->FunctionHandlers[i][j].flagIsExperimental)
        outputCommandStrings.AddOnTop(this->FunctionHandlers[i][j].theExample);
  for (int i=0; i<this->operationsCompositeHandlers.size; i++)
    for (int j=0; j<this->operationsCompositeHandlers[i].size; j++)
      outputCommandStrings.AddOnTop(this->operationsCompositeHandlers[i][j].theExample);
  outputResultsWithInit.SetSize(outputCommandStrings.size);
  outputResultsNoInit.SetSize(outputCommandStrings.size);
  ProgressReport theReport;
  FormatExpressions theFormat;
  theFormat.flagExpressionIsFinal=true;
  for (int i=0; i<outputCommandStrings.size; i++)
  { double startingTime=theGlobalVariables.GetElapsedSeconds();
    std::stringstream reportStream;
    reportStream << "<br>Testing expression:<br> " << outputCommandStrings[i]
    << "<br>Test progress: testing " << i+1 << " out of " << outputCommandStrings.size << ". ";
    theReport.Report(reportStream.str());
    theTester.reset();
    theTester.CheckConsistencyAfterInitializationExpressionStackEmpty();
    theTester.init();
    theTester.Evaluate(outputCommandStrings[i]);
    outputResultsWithInit[i]=theTester.theProgramExpression.ToString(&theFormat);
    reportStream << "<br>Result: " << theTester.theProgramExpression.ToString();
    reportStream << "<br>Done in: " << theGlobalVariables.GetElapsedSeconds()-startingTime << " seconds. ";
    theReport.Report(reportStream.str());
  }
}

bool CalculatorFunctionsGeneral::innerPrintRuleStack
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCrash");
  (void) input;//portable way of avoiding unused parameter warning
  return output.AssignValue(theCommands.RuleStack.ToString(), theCommands);
}

bool CalculatorFunctionsGeneral::innerCrash
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCrash");
  (void) input;//portable way of avoiding unused parameter warning
  crash << "<hr>This is a test of the crashing mechanism of the calculator. Are log files created correctly? Check the /output/ directory." << crash;
  return output.AssignValue((std::string)"Crashed succesfully", theCommands);
}

#include <vector>
bool CalculatorFunctionsGeneral::innerCrashByListOutOfBounds
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCrashByListOutOfBounds");
  (void) input;//portable way of avoiding unused parameter warning
  List<int> theList;
  std::vector<int> theVector;
  for (int i=0; i<5; i++)
  { theList.AddOnTop(0);
    theVector.push_back(0);
  }
  theList.SetSize(0);
  theVector.resize(0);
  theVector[1]=1;
  theList[1]=1;
  return output.AssignValue((std::string) "Crashing: list out of bounds.", theCommands);
}

bool CalculatorFunctionsGeneral::innerCrashByVectorOutOfBounds
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerCrashByVectorOutOfBounds");
  (void) input;//portable way of avoiding unused parameter warning
  std::vector<int> theVector;
  theVector[1]=1;
  return output.AssignValue((std::string) "Crashing: std::vector out of bounds.", theCommands);
}

bool CalculatorFunctionsGeneral::innerDrawWeightSupportWithMults(Calculator& theCommands, const Expression& input, Expression& output)
{ //theNode.owner->theHmm.MakeG2InB3(theParser);
  if (!input.IsListNElements(3))
    return output.MakeError("Error: the function for drawing weight support takes two  arguments (type and highest weight)", theCommands);
  const Expression& typeNode=input[1];
  const Expression& hwNode=input[2];
  SemisimpleLieAlgebra* theSSalgpointer=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, typeNode, theSSalgpointer))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  Vector<Rational> highestWeightFundCoords;
  Expression theContext;
  if (!theCommands.GetVectoR<Rational>  (hwNode, highestWeightFundCoords, &theContext, theSSalgpointer->GetRank(), 0))
    return output.MakeError("Failed to extract highest weight vector", theCommands);
  Vector<Rational> highestWeightSimpleCoords;
  WeylGroupData& theWeyl=theSSalgpointer->theWeyl;
  highestWeightSimpleCoords= theWeyl.GetSimpleCoordinatesFromFundamental(highestWeightFundCoords);
  //Vectors<Rational> theWeightsToBeDrawn;
  std::stringstream out;
  charSSAlgMod<Rational> theChar;
  theChar.MakeFromWeight(highestWeightSimpleCoords, theSSalgpointer);
  DrawingVariables theDV;
  std::string report;
  theChar.DrawMeWithMults(report, theDV, 10000);
  out << report << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyl.GetDim());
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerDrawRootSystem(Calculator& theCommands, const Expression& input, Expression& output)
{ //theNode.owner->theHmm.MakeG2InB3(theParser);
  bool hasPreferredProjectionPlane= input.IsListNElements(4);
  const Expression& typeNode= hasPreferredProjectionPlane ? input[1] : input;
  SemisimpleLieAlgebra* theAlgPointer;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, typeNode, theAlgPointer))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  SemisimpleLieAlgebra& theAlg=*theAlgPointer;
  WeylGroupData& theWeyl=theAlg.theWeyl;
  Vectors<Rational> preferredProjectionPlane;
  if (hasPreferredProjectionPlane)
  { preferredProjectionPlane.SetSize(2);
    bool isGood=
    theCommands.GetVectoR(input[2], preferredProjectionPlane[0], 0, theWeyl.GetDim(), 0) && theCommands.GetVectoR(input[3], preferredProjectionPlane[1], 0, theWeyl.GetDim(), 0);
    if (!isGood)
      return output.MakeError("Failed to convert second or third argument to vector of desired dimension", theCommands);
  }
  std::stringstream out;
  DrawingVariables theDV;
  theWeyl.DrawRootSystem(theDV, true, false, 0, true, 0);
  if (hasPreferredProjectionPlane)
  { theDV.flagFillUserDefinedProjection=true;
    theDV.FillUserDefinedProjection=preferredProjectionPlane;
  }
  out << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyl.GetDim());
  return output.AssignValue(out.str(), theCommands);
}

template <class coefficient>
int charSSAlgMod<coefficient>::GetPosNstringSuchThatWeightMinusNalphaIsWeight
(const Weight<coefficient>& theWeightInFundCoords, const Vector<coefficient>& theAlphaInFundCoords)
{ MacroRegisterFunctionWithName("charSSAlgMod_coefficient::GetMaxNSuchThatWeightMinusNalphaIsAWeight");
  int result=-1;
  Weight<coefficient> currentWeight;
  currentWeight=theWeightInFundCoords;
  for (;this->theMonomials.Contains(currentWeight);
       result++, currentWeight.weightFundamentalCoordS-=theAlphaInFundCoords){}
//  if (result==-1)
//    crash << "temporary wrong check" <<crash;
  return result;
}

template <class coefficient>
std::string charSSAlgMod<coefficient>::ToStringFullCharacterWeightsTable()
{ MacroRegisterFunctionWithName("charSSAlgMod_CoefficientType::ToStringFullCharacterWeightsTable");
  std::stringstream out;
  charSSAlgMod<coefficient> outputChar;
  if (!this->FreudenthalEvalMeFullCharacter(outputChar, 10000, 0))
  { out << "Failed to compute the character with highest weight " << this->ToString()
    << " I used Fredenthal's formula; likely the computation was too large. ";
    return out.str();
  }
  out << "<table><tr><td>Weight in fund. coords</td><td>simple coords.</td>"
  << "<td>Simple strings</td><td>Simple half-strings</td></tr>";
  Vector<coefficient> outputSimpleStringCoords, outputSimpleHalfStringCoords;
  Vector<coefficient> theSimpleRoot;
  Vector<coefficient> theSimpleRootFundCoords;
  for (int k=0; k<outputChar.size(); k++)
  { out << "<tr>";
    out << "<td>" << outputChar[k].weightFundamentalCoordS.ToString() << "</td>";
    Vector<coefficient> weightSimple=this->GetOwner()->theWeyl.GetSimpleCoordinatesFromFundamental
    (outputChar[k].weightFundamentalCoordS);
    out << "<td>" << weightSimple.ToString() << "</td>";
    outputSimpleStringCoords.MakeZero(this->GetOwner()->GetRank());
    outputSimpleHalfStringCoords.MakeZero(this->GetOwner()->GetRank());
    for (int j=0; j<this->GetOwner()->GetRank(); j++)
    { theSimpleRoot.MakeEi(this->GetOwner()->GetRank(), j);
      theSimpleRootFundCoords=
      this->GetOwner()->theWeyl.GetFundamentalCoordinatesFromSimple(theSimpleRoot);
      outputSimpleStringCoords[j]=outputChar.GetPosNstringSuchThatWeightMinusNalphaIsWeight
      (outputChar[k], theSimpleRootFundCoords)-
      outputChar.GetPosNstringSuchThatWeightMinusNalphaIsWeight
      (outputChar[k], -theSimpleRootFundCoords);
      outputSimpleHalfStringCoords[j]=outputChar.GetPosNstringSuchThatWeightMinusNalphaIsWeight
      (outputChar[k], theSimpleRootFundCoords);
    }
    if (outputSimpleStringCoords!=outputChar[k].weightFundamentalCoordS)
      out << "<td><span style=\"color:#FF0000\"><b>" << outputSimpleStringCoords.ToString() << "</b></span></td>" ;
    else
      out << "<td>" << outputSimpleStringCoords.ToString() << "</td>";
    if (outputSimpleHalfStringCoords!=outputChar[k].weightFundamentalCoordS)
      out << "<td><span style=\"color:#FF0000\"><b>" << outputSimpleHalfStringCoords.ToString() << "</b></span></td>" ;
    else
      out << "<td>" << outputSimpleHalfStringCoords.ToString() << "</td>";
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

class ExpressionTreeDrawer
{
public:
  int MaxDepth;
  int MaxAllowedWidth;
  int MaxDisplayedNodes;
  int indexInCurrentLayer;
  int indexCurrentChild;
  Rational widthMaxLayer;
  int numLayers;
  int maxNumCharsInString;
  bool flagUseFullTree;
  Expression baseExpression;
  HashedList<std::string, MathRoutines::hashString> DisplayedEstrings;
  List<bool> DisplayedStringIsLeaf;
  List<Vector<Rational> > NodePositions;
  List<int> LayerSizes;
  List<int> LayerFirstIndices;
  List<List<int> > arrows;
  DrawingVariables theDV;
  List<Expression> currentLayer;
  List<Expression> nextLayer;
  List<Expression> currentEchildrenTruncated;
  Calculator* owner;
  Rational charWidth, padding, layerHeight, charHeight;
  ExpressionTreeDrawer()
  { this->MaxDepth=10;
    this->MaxAllowedWidth=10;
    this->MaxDisplayedNodes=1000;
    this->flagUseFullTree=false;
    this->indexInCurrentLayer=-1;
    this->indexCurrentChild=-1;
    this->maxNumCharsInString=100;
    this->numLayers=0;
    this->owner=0;
    this->charWidth.AssignNumeratorAndDenominator(1,20);
    this->padding=1;
    this->layerHeight=2;
    this->widthMaxLayer=0;
    this->charHeight.AssignNumeratorAndDenominator(1,5);
  }
  Expression& GetCurrentE()
  { MacroRegisterFunctionWithName("ExpressionTreeDrawer::GetCurrentE");
    return this->currentLayer[this->indexInCurrentLayer];
  }
  void ComputeCurrentEchildrenTruncated()
  { MacroRegisterFunctionWithName("ExpressionTreeDrawer::ComputeCurrentEchildrenTruncated");
    this->currentEchildrenTruncated.SetSize(0);
    if (!this->flagUseFullTree)
      if (this->GetCurrentE().IsBuiltInType())
        return;
    for (int i=0; i< this->GetCurrentE().children.size; i++)
    { this->currentEchildrenTruncated.AddOnTop(this->GetCurrentE()[i]);
      if (i+1+this->indexCurrentChild>this->MaxDisplayedNodes || i>this->MaxAllowedWidth)
      { Expression dotsAtom;
        dotsAtom.MakeAtom((std::string)"...", *this->owner);
        this->currentEchildrenTruncated.AddOnTop(dotsAtom);
        break;
      }
    }
  }
  bool isLeaf(const Expression& input)
  { if (this->flagUseFullTree)
      return input.IsAtom();
    if (input.IsAtom() || input.IsBuiltInType())
      return true;
    return false;
  }
  std::string GetDisplayString(const Expression& input)
  { MacroRegisterFunctionWithName("ExpressionTreeDrawer::GetDisplayString");
    std::stringstream out;
    if (this->flagUseFullTree)
    { std::string atomName;
      if (input.IsAtom(&atomName))
      { if (atomName!="...")
          out << input.theData;
        else
          out << "...";
      } else
        out << input.ToString();
    } else
      out << input.ToString();
    return out.str();
  }
  void ComputeCurrentEContributionToNextLayer()
  { MacroRegisterFunctionWithName("ExpressionTreeDrawer::ComputeCurrentEContributionToNextLayer");
    this->ComputeCurrentEchildrenTruncated();
    this->nextLayer.AddListOnTop(this->currentEchildrenTruncated);
    List<int> emptyArrows;
    for (int i=0; i<this->currentEchildrenTruncated.size; i++)
    { this->arrows[this->indexCurrentChild].AddOnTop(this->DisplayedEstrings.size);
      this->AddStringTruncate
      (this->GetDisplayString(this->currentEchildrenTruncated[i]),
       this->isLeaf(this->currentEchildrenTruncated[i]));
      this->arrows.AddOnTop(emptyArrows);
    }
  }
  void init()
  { this->indexInCurrentLayer=0;
    this->indexCurrentChild=0;
    this->currentLayer.SetSize(1);
    this->LayerFirstIndices.SetSize(1);
    this->LayerFirstIndices[0]=0;
    this->LayerSizes.SetSize(1);
    this->LayerSizes[0]=1;
    List<int> emptyArrows;
    this->arrows.AddOnTop(emptyArrows);
    this->currentLayer[0]=this->baseExpression;
    this->DisplayedEstrings.Clear();
    this->AddStringTruncate(this->baseExpression.ToString(), this->isLeaf(this->baseExpression));
    this->ComputeCurrentEContributionToNextLayer();
    this->DisplayedEstrings.SetExpectedSize(this->MaxDisplayedNodes);
    this->arrows.SetExpectedSize(this->MaxDisplayedNodes);
  }
  std::string ToString()
  { std::stringstream out;
    out << "<br>Index in current layer: " << this->indexInCurrentLayer;
    out << "<br>Index in displayed strings: " << this->indexCurrentChild;
    out << "<br>Current layer: " << this->currentLayer.ToStringCommaDelimited();
    out << "<br>Next layer: " << this->nextLayer.ToStringCommaDelimited();
    out << "<br>Displayed strings: " << this->DisplayedEstrings.ToStringCommaDelimited() ;
    out << "<br>Node positions: " << this->NodePositions.ToStringCommaDelimited() ;
    out << "<br>Arrows: " << this->arrows.ToStringCommaDelimited() ;
    out << "<br>Width max layer: " << this->widthMaxLayer;
    out << "<br>DrawOps centerX, centerY: " << this->theDV.theBuffer.centerX[0] << ", "
    << this->theDV.theBuffer.centerY[0];
    return out.str();
  }
  void AddStringTruncate(const std::string& input, bool isLeaf)
  { this->DisplayedStringIsLeaf.AddOnTop(isLeaf);
    if (input.size()<= (unsigned) this->maxNumCharsInString)
    { this->DisplayedEstrings.AddOnTop(input);
      return;
    }
    std::string truncatedInput=input;
    truncatedInput.resize(this->maxNumCharsInString-3);
    truncatedInput+="...";
    this->DisplayedEstrings.AddOnTop(truncatedInput);
  }
  bool IncrementReturnFalseIfPastLast()
  { MacroRegisterFunctionWithName("ExpressionTreeDrawer::IncrementReturnFalseIfPastLast");
    //stOutput << "At start of incrementing function: " << this->ToString() << "<hr>";
    this->indexInCurrentLayer++;
    this->indexCurrentChild++;
    if (this->indexInCurrentLayer>=this->currentLayer.size)
    { this->indexInCurrentLayer=0;
      this->currentLayer=this->nextLayer;
      if (this->currentLayer.size==0)
        return false;
      this->LayerFirstIndices.AddOnTop(this->indexCurrentChild);
      this->LayerSizes.AddOnTop(this->nextLayer.size);
      this->nextLayer.SetSize(0);
      this->ComputeCurrentEContributionToNextLayer();
      return this->currentLayer.size>0;
    }
    this->ComputeCurrentEContributionToNextLayer();
    return true;
  }
  Rational GetStringWidthTruncated(int theIndex)
  { return this->charWidth*
    MathRoutines::Minimum(this->maxNumCharsInString, (signed) this->DisplayedEstrings[theIndex].size());
  }
  Rational GetLayerWidth(int layerIndex)
  { MacroRegisterFunctionWithName("ExpressionTreeDrawer::GetLayerWidth");
    Rational result=0;
    for (int i=this->LayerFirstIndices[layerIndex]; i<this->LayerFirstIndices[layerIndex]+this->LayerSizes[layerIndex]; i++)
      result+=this->GetStringWidthTruncated(i) +this->padding;
    result-=this->padding;
    if (result>this->widthMaxLayer)
      this->widthMaxLayer=result;
    return result;
  }
  void ComputeLayerPositions(int layerIndex)
  { MacroRegisterFunctionWithName("ExpressionTreeDrawer::ComputeLayerPositions");
    Rational currentX =-this->GetLayerWidth(layerIndex)/2;
    for (int i=this->LayerFirstIndices[layerIndex]; i<this->LayerFirstIndices[layerIndex]+this->LayerSizes[layerIndex]; i++)
    { this->NodePositions[i].SetSize(2);
      this->NodePositions[i][0]=currentX+this->GetStringWidthTruncated(i)/2;
      this->NodePositions[i][1]=this->layerHeight* layerIndex*(-1);
      currentX+=this->charWidth*this->DisplayedEstrings[i].size()+this->padding;
    }
  }
  void DrawToDV()
  { MacroRegisterFunctionWithName("ExpressionTreeDrawer::ExtractDisplayedExpressions");
    this->init();
    while (this->IncrementReturnFalseIfPastLast())
    {}
    this->NodePositions.SetSize(this->DisplayedEstrings.size);
    for (int i=0; i<this->LayerFirstIndices.size; i++)
      this->ComputeLayerPositions(i);
    //stOutput << this->ToString();
    Vector<Rational> arrowBase, arrowHead;
    for (int i=0; i<this->DisplayedEstrings.size; i++)
    { if (this->DisplayedEstrings[i]!="")
      { int theColor= this->DisplayedStringIsLeaf[i] ? CGI::RedGreenBlue(255, 0,0) : CGI::RedGreenBlue(100,100,100);
        theDV.drawTextAtVectorBufferRational
        (this->NodePositions[i],
         CGI::clearNewLines(CGI::backslashQuotes(
         CGI::DoubleBackslashes( this->DisplayedEstrings[i]) )),
         theColor, theDV.TextStyleNormal);
      } else
        theDV.drawCircleAtVectorBufferRational
        (this->NodePositions[i], 0.04, theDV.PenStyleNormal, CGI::RedGreenBlue(0,0,0));
      for (int j=0; j<this->arrows[i].size; j++)
      { arrowBase= this->NodePositions[i];
        arrowHead=this->NodePositions[this->arrows[i][j]];
        arrowHead[1]+=this->charHeight/2;
        //arrowHead=arrowHead*nineTenths+arrowBase*oneTenth;
        theDV.drawLineBetweenTwoVectorsBufferRational
        (arrowBase, arrowHead, theDV.PenStyleNormal, CGI::RedGreenBlue(0,0,0),1);
      }
    }
    double& theGraphicsUnit =theDV.theBuffer.GraphicsUnit[0];
    theGraphicsUnit=100;
    theDV.DefaultHtmlHeight=(int)( (this->layerHeight* (this->LayerSizes.size-1)*theGraphicsUnit+100).GetDoubleValue());
    theDV.DefaultHtmlWidth=(int)( (this->widthMaxLayer*theGraphicsUnit+40).GetDoubleValue());
    theDV.theBuffer.centerX.SetSize(1);
    theDV.theBuffer.centerY.SetSize(1);
    theDV.theBuffer.centerX[0]= (this->widthMaxLayer/2).GetDoubleValue()*theGraphicsUnit;
    theDV.theBuffer.centerY[0]= 40;
//    stOutput << this->ToString();
  }
};

bool CalculatorFunctionsGeneral::innerDrawExpressionGraphWithOptions
(Calculator& theCommands, const Expression& input, Expression& output, bool useFullTree)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDrawExpressionGraph");
  ExpressionTreeDrawer theEdrawer;
  theEdrawer.flagUseFullTree=useFullTree;
  theEdrawer.owner=&theCommands;
  theEdrawer.baseExpression=input;
  theEdrawer.DrawToDV();
  std::stringstream out;
  out << theEdrawer.theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(2);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerDrawWeightSupport(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDrawWeightSupport");
  //theNode.owner->theHmm.MakeG2InB3(theParser);
  if (!input.IsListNElements(3))
    return output.MakeError("Wrong number of arguments, must be 2. ", theCommands);
  const Expression& typeNode=input[1];
  const Expression& hwNode=input[2];
  SemisimpleLieAlgebra* theAlgPointer;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, typeNode, theAlgPointer))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  SemisimpleLieAlgebra& theAlg=*theAlgPointer;
  Vector<Rational> highestWeightFundCoords;
  Expression tempContext;
  if (!theCommands.GetVectoR<Rational>(hwNode, highestWeightFundCoords, &tempContext, theAlg.GetRank(), 0))
    return false;
  Vector<Rational> highestWeightSimpleCoords;
  WeylGroupData& theWeyl=theAlg.theWeyl;
  highestWeightSimpleCoords= theWeyl.GetSimpleCoordinatesFromFundamental(highestWeightFundCoords);
  //Vectors<Rational> theWeightsToBeDrawn;
  std::stringstream out;
  charSSAlgMod<Rational> theChar;
  theChar.MakeFromWeight(highestWeightSimpleCoords, theAlgPointer);
  DrawingVariables theDV;
  std::string report;
  theChar.DrawMeNoMults(report, theDV, 10000);
  out << report << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyl.GetDim());
  out << "<br>A table with the weights of the character follows. <br>";
  out << theChar.ToStringFullCharacterWeightsTable();
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerSetRandomSeed
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerRandomInteger");
  int theInt=-1;
  if (!input.IsIntegerFittingInInt(& theInt))
    return theCommands << "Failed to extract small integer";
  std::stringstream out;
  theCommands.theObjectContainer.CurrentRandomSeed=theInt;
  srand((unsigned) theInt);
  out << "Successfully set random seed to: " << (unsigned) theInt;
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerAnd
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerAnd");
  if (input.children.size!=3)
    return false;
  if (input[1].IsEqualToZero())
    return output.AssignValue(0, theCommands);
  if (input[2].IsEqualToZero())
    return output.AssignValue(0, theCommands);
  if (input[1].IsEqualToOne() && input[2].IsEqualToOne())
    return output.AssignValue(1, theCommands);
  return false;
}

bool CalculatorFunctionsGeneral::innerOr
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerOr");
  if (input.children.size!=3)
    return false;
  if (input[1].IsEqualToOne())
    return output.AssignValue(1, theCommands);
  if (input[2].IsEqualToOne())
    return output.AssignValue(1, theCommands);
  if (input[1].IsEqualToZero() && input[2].IsEqualToZero())
    return output.AssignValue(0, theCommands);
  return false;
}

bool CalculatorFunctionsGeneral::innerIf
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerOr");
  (void) theCommands;//portable way of avoiding unused parameter warning
  if (input.children.size!=4)
    return false;
  if (input[1].IsEqualToOne())
  { output=input[2];
    return true;
  }
  if (input[1].IsEqualToZero())
  { output=input[3];
    return true;
  }
  return false;
}

bool CalculatorFunctionsGeneral::innerTurnRulesOnOff
(Calculator& theCommands, const Expression& input, Expression& output, bool turnOff)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerTurnRulesOnOff");
  List<std::string> rulesToConsider;
  std::string currentRule;
  if (!input.StartsWith(theCommands.opTurnOffRules()) && !input.StartsWith(theCommands.opTurnOnRules()))
  { if (input.IsOfType<std::string>(&currentRule))
      rulesToConsider.AddOnTop(currentRule);
    else if (input.IsAtom(&currentRule))
      rulesToConsider.AddOnTop(currentRule);
    else
      return theCommands << "Could not extract rule to turn off from " << input.ToString() << ". ";
  } else
    for (int i=1; i<input.children.size; i++)
      if (input[i].IsOfType<std::string>(&currentRule))
        rulesToConsider.AddOnTop(currentRule);
      else if (input[i].IsAtom(&currentRule))
        rulesToConsider.AddOnTop(currentRule);
      else
        return theCommands << "Could not extract rule to turn off from " << input[i].ToString() << ". ";
  HashedList<std::string, MathRoutines::hashString> rulesToSwitch;
  rulesToSwitch.Reserve(rulesToConsider.size);
  for (int i=0; i<rulesToConsider.size; i++)
    if (! theCommands.namedRules.Contains(rulesToConsider[i]))
      return theCommands << "Can't find named rule: " << rulesToConsider[i]
      << ". Turn-off rules command failed. "
//      << theCommands.namedRules.ToStringCommaDelimited()
      ;
    else
      rulesToSwitch.AddOnTopNoRepetition(rulesToConsider[i]);
  output.reset(theCommands, rulesToSwitch.size+1);
  if (turnOff)
    output.AddChildAtomOnTop(theCommands.opRulesOff());
  else
    output.AddChildAtomOnTop(theCommands.opRulesOn());
  Expression currentRuleE;
  for (int i=0; i<rulesToSwitch.size; i++)
  { currentRuleE.AssignValue(rulesToSwitch[i], theCommands);
    output.AddChildOnTop(currentRuleE);
  }
  return true;
}

bool CalculatorFunctionsGeneral::innerTurnOffRules
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerTurnOffRules");
  return CalculatorFunctionsGeneral::innerTurnRulesOnOff(theCommands, input, output, true);
}

bool CalculatorFunctionsGeneral::innerTurnOnRules
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerTurnOnRules");
  return CalculatorFunctionsGeneral::innerTurnRulesOnOff(theCommands, input, output, false);
}

bool CalculatorFunctionsGeneral::innerEqualityToArithmeticExpression
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerEqualityToArithmeticExpression");
  if (!input.StartsWith(theCommands.opDefine(), 3))
    return false;
  return output.MakeXOX(theCommands, theCommands.opMinus(), input[1], input[2]);
}

bool CalculatorFunctionsGeneral::innerRandomInteger
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerRandomInteger");
  Matrix<Expression> theMat;
  if (!theCommands.GetMatrixExpressionsFromArguments(input, theMat, -1, 2))
    return theCommands << "<hr>Failed to extract a Nx2 matrix giving the integer intervals";
  if (theMat.NumRows==0)
    return theCommands << "<hr>Failed to extract a Nx2 matrix giving the integer intervals";
  List<List<int> > theIntervals;
  theIntervals.SetSize(theMat.NumRows);
  for (int i=0; i<theMat.NumRows; i++)
  { theIntervals[i].SetSize(theMat.NumCols);
    for (int j=0; j<theMat.NumCols; j++)
      if (!theMat(i,j).IsIntegerFittingInInt(& theIntervals[i][j]))
        return theCommands << "<hr>Failed to convert " << theMat(i,j).ToString() << " to an integer. ";
  }
  int accum=0;
  for (int i=0; i<theIntervals.size; i++)
  { int currentContribution= theIntervals[i][1]-theIntervals[i][0];
    if (currentContribution<0)
      currentContribution*=-1;
    accum+=currentContribution+1;
  }
  if (accum==0)
    crash << "This shouldn't happen: accum should not be zero at this point. " << crash;
  int generatedRandomInt = rand()%accum;
  int resultRandomValue=theIntervals[0][0];
  bool found=false;
  //stOutput << "<br>The intervals: " << theIntervals.ToStringCommaDelimited()
  //<< "<br>The intervals size: " << theIntervals.size
  //<< "<br>accum: " << accum << " generatedRandomInt: " << generatedRandomInt;
  accum=0;
  for (int i=0; i<theIntervals.size; i++)
  { int currentContribution= theIntervals[i][1]-theIntervals[i][0];
    int sign=1;
    if (currentContribution<0)
    { currentContribution*=-1;
      sign=-1;
    }
    currentContribution++;
    int nextAccum=accum+currentContribution;
    if (accum <= generatedRandomInt && generatedRandomInt <nextAccum)
    { resultRandomValue= theIntervals[i][0]+sign*(generatedRandomInt-accum);
      found=true;
    }
    accum=nextAccum;
  }
  if (!found)
    return theCommands << "<hr>Failed to generate a random number: this shouldn't happen - perhaps the requested "
    << " interval was too large. ";
  return output.AssignValue(resultRandomValue, theCommands);
}

bool CalculatorFunctionsGeneral::innerSelectAtRandom
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSelectAtRandom");
  if (!input.StartsWith(theCommands.theAtoms.GetIndex("selectAtRandom")))
  { output=input; //only one item to select from: returning the item
    return true;
  }
  if (input.children.size<2)
    return false;
  if (input.children.size==2)
  { output=input[1]; //only one item to select from: return that item
    return true;
  }
  int randomIndex= (rand()% (input.children.size-1))+1;
  if (randomIndex<0 || randomIndex> input.children.size-1)
    randomIndex=input.children.size-1;
  //<-the line above should never be executed if the % operator works as it should,
  //but having an extra check never hurts (may be a life saver if I change the code above).
  output=input[randomIndex];
  return true;
}
