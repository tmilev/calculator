//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
ProjectInformationInstance ProjectInfoVpf5_1cpp(__FILE__, "Implementation file for the calculator parser part 3: meant for built-in functions. ");

class SemisimpleSubalgebras
{
public:
  std::string ToString();
  SltwoSubalgebras theSl2s;
  List<SemisimpleLieAlgebra>* owner;
  int indexInOwner;
  List<Vectors<Rational> >  theHcandidates;
  int indexLowestUnexplored;
  void FindHCandidates
    (GlobalVariables* theGlobalVariables)
  ;
  void FindHCandidatesWithOneExtraHContaining
  (Vectors<Rational>& inpuT, GlobalVariables* theGlobalVariables)
  ;
  SemisimpleLieAlgebra& GetSSowner()
  { if (this->owner==0 || this->indexInOwner<0)
    { std::cout << "This is a programming error: attempted to access non-initialized "
      << " semisimple Lie subalgerbas. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    return (*owner)[this->indexInOwner];
  }
  SemisimpleSubalgebras(List<SemisimpleLieAlgebra>* inputOwners, int inputIndexInOwner)
  :owner(inputOwners), indexInOwner(inputIndexInOwner){}
};


std::string SemisimpleSubalgebras::ToString()
{ std::stringstream out;
  out << this->theHcandidates.ToString();
  return out.str();
}

void SemisimpleSubalgebras::FindHCandidates(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("SemisimpleSubalgebras::FindHCandidates");
  Vectors<Rational> emptyStart;
//  List<List<Vector<Rational>> >::ListActualSizeIncrement=10000;
  MemorySaving<GlobalVariables> tempGV;
  if (theGlobalVariables==0)
    theGlobalVariables=&tempGV.GetElement();
  this->GetSSowner().FindSl2Subalgebras
  (this->theSl2s, this->GetSSowner().theWeyl.WeylLetter, this->GetSSowner().GetRank(),
   *theGlobalVariables);
  this->theHcandidates.size=0;
  this->theHcandidates.AddOnTop(emptyStart);
  for (this->indexLowestUnexplored=0; this->indexLowestUnexplored<this->theHcandidates.size; this->indexLowestUnexplored++)
    this->FindHCandidatesWithOneExtraHContaining(this->theHcandidates.TheObjects[this->indexLowestUnexplored], theGlobalVariables);
}

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

void  SemisimpleSubalgebras::FindHCandidatesWithOneExtraHContaining
(Vectors<Rational>& inpuT, GlobalVariables* theGlobalVariables)
{ int theDimension= this->GetSSowner().theWeyl.CartanSymmetric.NumRows;
  SelectionWithMaxMultiplicity theSel;
  theSel.initMaxMultiplicity(theDimension, 2);
  int theCounter=MathRoutines::KToTheNth(3, theDimension);
  Vector<Rational> theRoot;
  WeylGroup& theWeyl=this->GetSSowner().theWeyl;
  Matrix<Rational> invertedCartan=this->GetSSowner().theWeyl.CartanSymmetric;
  invertedCartan.Invert();
  Vectors<Rational> tempRoots;
  tempRoots=inpuT;
  Vectors<Rational> inputCopy;
  inputCopy=inpuT;
  ProgressReport theReport(theGlobalVariables);
  for (int i=0; i<theCounter; i++, theSel.IncrementSubset())
  { //slTwo& currentSl2=this->theSl2s.TheObjects[i];
    theRoot=theSel;
    invertedCartan.ActOnVectorColumn(theRoot, theRoot);
    bool isGood=true;
    if (!inputCopy.LinSpanContainsRoot(theRoot))
    { for (int j=0; j<inputCopy.size; j++)
        if (theWeyl.RootScalarCartanRoot(inputCopy.TheObjects[j], theRoot).IsPositive())
        { isGood=false;
          break;
        }
      if (isGood)
      { tempRoots= inputCopy;
        tempRoots.AddOnTop(theRoot);
        for (int k=tempRoots.size-1; k>0; k--)
          if (tempRoots[k]< tempRoots[k-1])
            tempRoots.SwapTwoIndices(k, k-1);
        this->theHcandidates.AddOnTopNoRepetition(tempRoots);
        //this->th
      }
      if (theGlobalVariables!=0)
      { std::stringstream out;
        out << "index lowest non explored: " << this->indexLowestUnexplored+1 << " Total number found: " << this->theHcandidates.size;
        theReport.Report(out.str());
      }
    }
  }
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
  theSSsubalgebras.FindHCandidates(theCommands.theGlobalVariableS);
  out << "<br>" << theSSsubalgebras.ToString();

  theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, out.str());
  return true;
}

bool CommandList::fGroebnerBuchberger
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression,
 std::stringstream* comments, bool useGr)
{ MacroRegisterFunctionWithName("CommandList::fGroebnerBuchberger");
  Vector<Polynomial<Rational> > inputVector;
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
  RationalFunctionOld::TransformToReducedGroebnerBasis
  (outputGroebner, buffer1, buffer2, buffer3, buffer4, bufferMon1, bufferMon2, theMonOrder
   , theCommands.theGlobalVariableS);

  std::stringstream out;
  out << "<br>Starting basis: ";
  std::stringstream out1, out2, out3;
  for(int i=0; i<inputVector.size; i++)
    out1 << inputVector[i].ToString(&theFormat) << ", ";
  out << CGI::GetHtmlMathDivFromLatexAddBeginArrayL(out1.str());
  out << "<br>Minimal Groebner basis algorithm 1:";
  for(int i=0; i<outputGroebner.size; i++)
    out2 << outputGroebner[i].ToString(&theFormat) << ", ";
  out << CGI::GetHtmlMathDivFromLatexAddBeginArrayL(out2.str());
  out << "<br>Minimal Groebner basis algorithm 2:";
  for(int i=0; i<outputGroebner2.size; i++)
    out3 << outputGroebner2[i].ToString(&theFormat) << ", ";
  out << CGI::GetHtmlMathDivFromLatexAddBeginArrayL(out3.str());
  theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, out.str());
  return true;
}


