//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
ProjectInformationInstance ProjectInfoVpf5_1cpp(__FILE__, "Implementation file for the calculator parser part 3: meant for built-in functions. ");

bool CommandList::fSSsubalgebras
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ //bool showIndicator=true;
  MacroRegisterFunctionWithName("CommandList::fSSsubalgebras");
  if (!theCommands.CallCalculatorFunction(theCommands.fSSAlgebra, inputIndexBoundVars, theExpression, comments))
    return false;
  SemisimpleLieAlgebra& ownerSS=theExpression.GetAtomicValue().GetAmbientSSAlgebra();
  std::stringstream out;
  if (ownerSS.GetRank()>4)
  { out << "<b>This code is completely experimental and has been set to run up to rank 4."
    << " As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
    theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, out.str());
    return true;
  }
  else
    out << "<b>This code is completely experimental. Use the following printouts on "
    << "your own risk</b>";
  SemisimpleSubalgebras theSSsubalgebras(ownerSS.owner, ownerSS.indexInOwner);
  theSSsubalgebras.FindTheSSSubalgebras
  (ownerSS.owner, ownerSS.indexInOwner, theCommands.theGlobalVariableS);
  FormatExpressions theFormat;
  std::stringstream out1, out2;
  out1 << theCommands.PhysicalPathOutputFolder
  << ownerSS.theWeyl.WeylLetter << ownerSS.GetRank() << "/";
  out2 << theCommands.DisplayPathOutputFolder
  << ownerSS.theWeyl.WeylLetter << ownerSS.GetRank() << "/";
  theFormat.physicalPath=out1.str();
  theFormat.htmlPathServer=out2.str();
  out << "<br>" << theSSsubalgebras.ToString(&theFormat);
  theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, out.str());
  return true;
}

bool CommandList::fEmbedSSalgInSSalg
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ //bool showIndicator=true;
  MacroRegisterFunctionWithName("CommandList::fEmbedSSalgInSSalg");
  if (theExpression.children.size!=2)
    return theExpression.SetError("I expect two arguments - the two semisimple subalgebras.");
  Expression& EsmallSA=theExpression.children[0];
  Expression& ElargeSA=theExpression.children[1];
  if (!theCommands.CallCalculatorFunction(theCommands.fSSAlgebra, inputIndexBoundVars, EsmallSA, comments))
    return false;
  if (!theCommands.CallCalculatorFunction(theCommands.fSSAlgebra, inputIndexBoundVars, ElargeSA, comments))
    return false;

  SemisimpleLieAlgebra& ownerSS=ElargeSA.GetAtomicValue().GetAmbientSSAlgebra();
  SemisimpleLieAlgebra& smallSS=EsmallSA.GetAtomicValue().GetAmbientSSAlgebra();

  std::stringstream out;
  if (ownerSS.GetRank()>4)
  { out << "<b>This code is completely experimental and has been set to run up to rank 4."
    << " As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
    theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, out.str());
    return true;
  }
  else
    out << "<b>This code is completely experimental. Use the following printouts on "
    << "your own risk</b>";
  SemisimpleSubalgebras theSSsubalgebras(ownerSS.owner, ownerSS.indexInOwner);
  DynkinSimpleType theType;
  theType.theLetter=smallSS.theWeyl.WeylLetter;
  theType.theRank=smallSS.GetRank();
  theSSsubalgebras.FindAllEmbeddings
  (theType, ownerSS.owner, ownerSS.indexInOwner, theCommands.theGlobalVariableS);
  FormatExpressions theFormat;
  std::stringstream out1, out2;
  out1 << theCommands.PhysicalPathOutputFolder
  << ownerSS.theWeyl.WeylLetter << ownerSS.GetRank() << "/";
  out2 << theCommands.DisplayPathOutputFolder
  << ownerSS.theWeyl.WeylLetter << ownerSS.GetRank() << "/";
  theFormat.physicalPath=out1.str();
  theFormat.htmlPathServer=out2.str();
  out << "<br>" << theSSsubalgebras.ToString(&theFormat);
  theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, out.str());
  return true;
}

bool CommandList::fGroebnerBuchberger
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression,
 std::stringstream* comments, bool useGr)
{ MacroRegisterFunctionWithName("CommandList::fGroebnerBuchberger");
  Vector<Polynomial<Rational> > inputVector;
  Vector<Polynomial<ElementZmodP> > inputVectorZmodP;

  List<Polynomial<Rational> > outputGroebner, outputGroebner2;
  Context theContext;
  if (!theCommands.GetVector<Polynomial<Rational> >
      (theExpression, inputVector, &theContext, -1, theCommands.fPolynomial, comments))
    return theExpression.SetError("Failed to extract polynomial expressions");
  if (theExpression.errorString!="")
    return true;
  theContext.VariableImages.QuickSortAscending();
  theCommands.GetVector<Polynomial<Rational> >
  (theExpression, inputVector, &theContext, -1, theCommands.fPolynomial, comments);

  for (int i=0; i<inputVector.size; i++)
    inputVector[i].ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();

  FormatExpressions theFormat;
  theContext.GetFormatExpressions(theFormat);
//  int theNumVars=theContext.VariableImages.size;
  outputGroebner=inputVector;
  outputGroebner2=inputVector;
//  std::cout << outputGroebner.ToString(&theFormat);
  Polynomial<Rational> buffer1, buffer2, buffer3, buffer4;
  MonomialP bufferMon1, bufferMon2;
  theContext.GetFormatExpressions(theCommands.theGlobalVariableS->theDefaultFormat);

  MathRoutines::MonomialOrder theMonOrder=
  useGr ? MonomialP::LeftIsGEQTotalDegThenLexicographic :
  MonomialP::LeftIsGEQLexicographicLastVariableStrongest;
  RationalFunction<Rational>::TransformToReducedGroebnerBasisImprovedAlgorithm
(outputGroebner2, theMonOrder, theCommands.theGlobalVariableS
)
;
/*  RationalFunctionOld::TransformToReducedGroebnerBasis
  (outputGroebner, buffer1, buffer2, buffer3, buffer4, bufferMon1, bufferMon2, theMonOrder
   , theCommands.theGlobalVariableS);
*/
  std::stringstream out;
  out << "<br>Starting basis: ";
  std::stringstream out1, out2, out3;
  for(int i=0; i<inputVector.size; i++)
    out1 << inputVector[i].ToString(&theFormat) << ", ";
  out << CGI::GetHtmlMathDivFromLatexAddBeginArrayL(out1.str());
//  out << "<br>Minimal Groebner basis algorithm 1:";
//  for(int i=0; i<outputGroebner.size; i++)
//    out2 << outputGroebner[i].ToString(&theFormat) << ", ";
//  out << CGI::GetHtmlMathDivFromLatexAddBeginArrayL(out2.str());
  out << "<br>Minimal Groebner basis algorithm 2:";
  for(int i=0; i<outputGroebner2.size; i++)
    out3 << outputGroebner2[i].ToString(&theFormat) << ", ";
  out << CGI::GetHtmlMathDivFromLatexAddBeginArrayL(out3.str());
  theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, out.str());
  return true;
}
