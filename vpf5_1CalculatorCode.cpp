//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
ProjectInformationInstance ProjectInfoVpf5_1cpp(__FILE__, "Implementation file for the calculator parser part 3: meant for built-in functions. ");



std::string rootSubalgebras::ToString()
{ std::stringstream out;
  for (int i=0; i<this->size; i++)
  { out << (*this)[i].ToString();
  }
  return out.str();
}

void rootSubalgebras::GenerateAllReductiveRootSubalgebrasUpToIsomorphism
(GlobalVariables& theGlobalVariables, bool sort, bool computeEpsCoords)
{ MacroRegisterFunctionWithName("rootSubalgebras::GenerateAllReductiveRootSubalgebrasUpToIsomorphism");
  this->size=0;
  this->AmbientWeyl.ComputeRho(true);
  //this->initDynkinDiagramsNonDecided(this->AmbientWeyl, WeylLetter, WeylRank);
  rootSubalgebras rootSAsGenerateAll;
  rootSAsGenerateAll.SetSize(this->AmbientWeyl.CartanSymmetric.NumRows*2+1);
  rootSAsGenerateAll[0].genK.size=0;
  rootSAsGenerateAll[0].AmbientWeyl=(this->AmbientWeyl);
  rootSAsGenerateAll[0].ComputeAll();
  this->GenerateAllReductiveRootSubalgebrasContainingInputUpToIsomorphism
  (rootSAsGenerateAll, 1, theGlobalVariables)
  ;
//  std::cout << this->ToString();
  if (sort)
    this->SortDescendingOrderBySSRank();
  if(computeEpsCoords)
    for(int i=0; i<this->size; i++)
      (*this)[i].ComputeEpsCoordsWRTk(theGlobalVariables);
}

bool CommandList::fSSsubalgebras
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ //bool showIndicator=true;
  MacroRegisterFunctionWithName("CommandList::fSSsubalgebras");
  if (!theCommands.CallCalculatorFunction(theCommands.fSSAlgebra, inputIndexBoundVars, theExpression, comments))
    return false;
  SemisimpleLieAlgebra& ownerSS=theExpression.GetAtomicValue().GetAmbientSSAlgebra();
  char weylLetter=ownerSS.theWeyl.WeylLetter;
  int theRank=ownerSS.theWeyl.GetDim();
  WeylGroup& theWeyl=ownerSS.theWeyl;
  std::stringstream out;
  out << "not implemented";
  SemisimpleSubalgebras theSSsubalgebras(ownerSS.owner, ownerSS.indexInOwner);
  theSSsubalgebras.FindTheSSSubalgebras
  (ownerSS.owner, ownerSS.indexInOwner, theCommands.theGlobalVariableS);
  out << "<br>" << theSSsubalgebras.ToString();

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
