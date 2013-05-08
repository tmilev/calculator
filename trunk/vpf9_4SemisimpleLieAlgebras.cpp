//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1_4SemisimpleLieAlgebras.h"
#include "vpf1_5SubsetsSelections.h"
ProjectInformationInstance ProjectInfoVpf9_4cpp
(__FILE__, "Implementation of semisimple subalgebra routines. ");

template<class CoefficientType>
void SemisimpleLieAlgebra::GenerateLieSubalgebra
(List<ElementSemisimpleLieAlgebra<CoefficientType> >& inputOutputGenerators)
{ ElementSemisimpleLieAlgebra<CoefficientType> theBracket;
  HashedList<ChevalleyGenerator> seedMons;
  for (int i=0; i<inputOutputGenerators.size; i++)
    for (int j=0; j<inputOutputGenerators.size; j++)
    { this->LieBracket(inputOutputGenerators[i], inputOutputGenerators[j], theBracket);
      inputOutputGenerators.AddOnTop(theBracket);
      if (theBracket.GetRankOfSpanOfElements
          (inputOutputGenerators, &seedMons)<inputOutputGenerators.size)
        inputOutputGenerators.RemoveLastObject();
    }
}

void WeylGroup::operator+=(const WeylGroup& other)
{ if (this==&other)
  { WeylGroup tempW;
    tempW=*this;
    *this+=tempW;
    return;
  }
  DynkinType theType=this->theDynkinType;
  theType+=other.theDynkinType;
  this->MakeFromDynkinType(theType);
}

int SemisimpleSubalgebras::GetDisplayIndexFromActual(int ActualIndexSubalgebra)const
{ int result=0;
  for (int i=0; i<=ActualIndexSubalgebra; i++)
    if (!this->Hcandidates[i].flagSystemProvedToHaveNoSolution)
      result++;
  return result;
}

std::string SemisimpleSubalgebras::GetPhysicalFileName
(int ActualIndexSubalgebra, FormatExpressions* theFormat)const
{ std::stringstream out;
  out << (theFormat==0 ? "./" : theFormat->PathPhysicalOutputFolder);
  out << this->owneR->theWeyl.theDynkinType.ToString() << "_subalgebra_"
  << this->GetDisplayIndexFromActual(ActualIndexSubalgebra) << ".html";
  return out.str();
}

std::string SemisimpleSubalgebras::GetDisplayFileName
(int ActualIndexSubalgebra, FormatExpressions* theFormat)const
{ std::stringstream out;
  out << (theFormat==0 ? "./" : theFormat->PathDisplayOutputFolder);
  out << this->owneR->theWeyl.theDynkinType.ToString() << "_subalgebra_"
  << this->GetDisplayIndexFromActual(ActualIndexSubalgebra) << ".html";
  return out.str();
}

std::string SemisimpleSubalgebras::ToString(FormatExpressions* theFormat)
{ MacroRegisterFunctionWithName("SemisimpleSubalgebras::ToString");
  bool writingToHD=theFormat==0? false: theFormat->flagUseHtmlAndStoreToHD;
  std::stringstream out;
  int candidatesRealized=0;
  int candidatesNotRealizedNotProvenImpossible=0;
  int candidatesProvenImpossible=0;
  for (int i=0; i<this->Hcandidates.size; i++)
  { CandidateSSSubalgebra& currentSA=this->Hcandidates[i];
    if (currentSA.flagSystemProvedToHaveNoSolution)
      candidatesProvenImpossible++;
    if (currentSA.flagSystemSolved)
      candidatesRealized++;
  }
  out << "<b>Please note that the code generating the tables is experimental. "
  << "While the code performs a number of self-test routines, "
  << " it is possible that there still exist programming bugs. "
  << "We will remove this message as soon "
  << "as we are confident in the accuracy of all tables.  "
  << " If you see any errors in the tables, we would be very grateful if "
  << "you email us with a simple explanation of the issue!</b><br>";
  if (!writingToHD)
  { candidatesNotRealizedNotProvenImpossible=
    this->Hcandidates.size-candidatesRealized- candidatesProvenImpossible;
    out << candidatesRealized << " subalgebras realized.";
    out << "<br>Total, there are " << this->Hcandidates.size << " = " << candidatesRealized
    << " realized + " << candidatesProvenImpossible << " proven impossible + "
    << candidatesNotRealizedNotProvenImpossible << " neither realized nor proven impossible. \n<hr>\n ";
  } else
  { out << "Total, there are " << candidatesRealized << " subalgebras";
    if (candidatesNotRealizedNotProvenImpossible!=0)
      out << " and "  << candidatesNotRealizedNotProvenImpossible << " subalgebra candidates which were "
      << " not realized (but not proven impossible)";
    out << ". ";
  }

  out << "The subalgebras are ordered by "
  << "(rank, dimensions of simple constituents, Dynkin indices of simple constituents). "
  << "The upper index stands for the length of the first co-root squared. "
  << " In type F_4, the upper index of a subalgebra coincides with its Dynkin index. "
  << "<hr> ";
  if (!writingToHD)
    for (int i=0; i<this->Hcandidates.size; i++)
    { if (!this->Hcandidates[i].flagSystemProvedToHaveNoSolution)
        out << "Subalgebra number " << this->GetDisplayIndexFromActual(i) << ".<br>";
      out << this->Hcandidates[i].ToString(theFormat) << "\n<hr>\n ";
    }
  else
  { theFormat->flagCandidateSubalgebraShortReportOnly=true;
    theFormat->flagUseHtmlAndStoreToHD=true;
    for (int i=0; i<this->Hcandidates.size; i++)
      if (!this->Hcandidates[i].flagSystemProvedToHaveNoSolution)
      { if (!this->Hcandidates[i].flagSystemProvedToHaveNoSolution)
          out << "Subalgebra number " << this->GetDisplayIndexFromActual(i) << ".<br>";
        out << this->Hcandidates[i].ToString(theFormat) << "\n<hr>\n ";
      }
    theFormat->flagCandidateSubalgebraShortReportOnly=false;
    for (int i=0; i<this->Hcandidates.size; i++)
      if (!this->Hcandidates[i].flagSystemProvedToHaveNoSolution)
      { std::fstream outputFile;
        if (! CGI::OpenFileCreateIfNotPresent
            (outputFile, this->GetPhysicalFileName(i, theFormat), false, true, false))
        { std::cout << "<br>This may or may not be a programming error. "
          << "While processing subalgebra of actual index " << i << " and display index "
          << this->GetDisplayIndexFromActual(i) << ", I requested to create file "
          << this->GetPhysicalFileName(i, theFormat) << " for output. However, the file failed to create. "
          << " Possible explanations: 1. Programming error. 2. The calculator has no write permission to the"
          << " folder in which the file is located. "
          << "3. The folder does not exist for some reason lying outside of the calculator. "
          << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
          assert(false);
        }
        outputFile << "<html>" << "<script src=\"" << theFormat->PathDisplayServerBaseFolder
        << "jsmath/easy/load.js\"></script> "
        << "<body>Subalgebra number " << this->GetDisplayIndexFromActual(i) << ".<br>";
        outputFile << this->Hcandidates[i].ToString(theFormat) << "\n</body></html>\n ";
      }
  }
  if (this->theSl2s.size!=0)
    out << this->theSl2s.ToString(theFormat);
  return out.str();
}

void SemisimpleSubalgebras::FindAllEmbeddings
(DynkinSimpleType& theType, SemisimpleLieAlgebra& theOwner, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("SemisimpleSubalgebras::FindAllEmbeddings");
  this->owneR=&theOwner;
  this->GetSSowner().FindSl2Subalgebras(theOwner, this->theSl2s, *theGlobalVariables);
  CandidateSSSubalgebra theCandidate;
  theCandidate.owner=this;
  this->ExtendOneComponentOneTypeAllLengthsRecursive
  (theCandidate, theType, false, theGlobalVariables);
}

void SemisimpleSubalgebras::FindTheSSSubalgebras
(SemisimpleLieAlgebra& newOwner, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("SemisimpleSubalgebras::FindTheSSSubalgebras");
  this->owneR=&newOwner;
  this->GetSSowner().FindSl2Subalgebras(newOwner, this->theSl2s, *theGlobalVariables);
  this->FindTheSSSubalgebrasPart2(theGlobalVariables);
}

void SemisimpleSubalgebras::FindTheSSSubalgebrasPart2
(GlobalVariables* theGlobalVariables)
{ CandidateSSSubalgebra emptyCandidate;
  emptyCandidate.owner=this;
  this->ExtendCandidatesRecursive(emptyCandidate, true, theGlobalVariables);
}

void SemisimpleSubalgebras::AddCandidatesSubalgebra
(CandidateSSSubalgebra& input, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("SemisimpleSubalgebras::AddCandidatesSubalgebra");
  SemisimpleLieAlgebra tempSA;
  tempSA.theWeyl=input.theWeylNonEmbeddeDdefaultScale;
  bool needToComputeConstants=this->theSubalgebrasNonEmbedded.Contains(tempSA);
  int theIndex=this->theSubalgebrasNonEmbedded.AddNoRepetitionOrReturnIndexFirst(tempSA);
  if (needToComputeConstants)
    this->theSubalgebrasNonEmbedded.GetElement(theIndex).ComputeChevalleyConstantS(theGlobalVariables);
  input.indexInOwnersOfNonEmbeddedMe=theIndex;
}

void DynkinType::GetDynkinTypeWithDefaultLengths(DynkinType& output)
{ if (&output==this)
  { DynkinType thisCopy=*this;
    thisCopy.GetDynkinTypeWithDefaultLengths(output);
    return;
  }
  output.MakeZero();
  DynkinSimpleType tempType;
  for (int i =0; i<this->size; i++)
  { tempType.MakeArbitrary((*this)[i].theLetter, (*this)[i].theRank);
    tempType.lengthFirstCoRootSquared=tempType.GetDefaultCoRootLengthSquared(0);
    output.AddMonomial(tempType, this->theCoeffs[i]);
  }
}

void DynkinSimpleType::GetAutomorphismActingOnVectorROWSwhichStandOnTheRight(Matrix<Rational>& output, int AutoIndex)const
{ MacroRegisterFunctionWithName("DynkinSimpleType::GetAutomorphismActingOnVectorROWSwhichStandOnTheRight");
  if (AutoIndex==0 || this->theLetter=='B' || this->theLetter=='C' || this->theLetter=='G' ||
      this->theLetter=='F' ||
      (this->theRank==1) ||
      (this->theLetter=='E' && this->theRank!=6))
  { output.MakeIdMatrix(this->theRank);
    return;
  }
  output.init(this->theRank, this->theRank);
  output.NullifyAll();
  if (this->theLetter=='A' && this->theRank!=1)
    for (int i=0; i<this->theRank; i++)
      output(i, this->theRank-i-1)=1;
  if (this->theLetter=='D')
  { if (this->theRank==4)
    { //Here be D_4 triality.
      //The automorphism group of Dynkin Diagram D_4 is S3
      //The triple node is always fixed:
      output(1,1)=1;
      if (AutoIndex==1)
      { //permutation (12), AutoIndex=1
        output(0,2)=1; output(2,0)=1; output(3,3)=1;
      } else if (AutoIndex==2)
      { //permutation (23), AutoIndex=2
        output(0,0)=1; output(2,3)=1; output(3,2)=1;
      } else if (AutoIndex==3)
      { //permutation (12)(23)=(123), AutoIndex=3
        output(0,2)=1; output(2,3)=1; output(3,0)=1;
      } else if (AutoIndex==4)
      { //permutation (23)(12)=(132), AutoIndex=4
        output(0,3)=1; output(2,0)=1; output(3,2)=1;
      } else if (AutoIndex==5)
      { //permutation (12)(23)(12)=(13), AutoIndex=5
        output(0,3)=1; output(2,2)=1; output(3,0)=1;
      } else
      { std::cout << "This is a programming error: requesting triality automorphism with "
        << "index not in the range 0-5. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }
    } else
    { output.MakeIdMatrix(this->theRank);
      output(this->theRank-1, this->theRank-1)=0;
      output(this->theRank-2, this->theRank-2)=0;
      output(this->theRank-1, this->theRank-2)=1;
      output(this->theRank-2, this->theRank-1)=1;
    }
  }
  if (this->theLetter=='E' && this->theRank==6)
  { //We gotta flip the Dynkin diagram of E_6. Note that the extra 1-length sticking root comes second and
    //and the triple node comes fourth.
    //Therefore, we must swap the 1st root with the 6th and the third root
    //with the 5th. Conventions, conventions... no way around 'em!
     output(1,1)=1; output(3,3)=1;
     output(0,5)=1; output(5,0)=1;
     output(2,4)=1; output(4,2)=1;
  }
  Rational tempRat=output.GetDeterminant();
  if (tempRat!=1 && tempRat!=-1)
  { std::cout << "This is a programming error: the determinant of the automorphism matrix of the Dynkin "
    << "graph must be +/-1, it is instead " << tempRat << ". "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__,__LINE__);
    assert(false);
  }
}

DynkinSimpleType DynkinType::GetGreatestSimpleType()const
{ if (this->size==0)
  { std::cout << "This is a programming error: asking for the greatest simple type "
    << " of a 0 dynkin type. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  DynkinSimpleType result=(*this)[0];
  for (int i=1; i<this->size; i++)
    if ((*this)[i]>result)
      result=(*this)[i];
  return result;
}

void SemisimpleSubalgebras::ExtendOneComponentRecursive
(const CandidateSSSubalgebra& baseCandidate, bool propagateRecursion,
GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("SemisimpleSubalgebras::ExtendOneComponentRecursive");
  baseCandidate.CheckInitialization();
  RecursionDepthCounter theCounter(&this->theRecursionCounter);
  int numVectorsFound=baseCandidate.CartanSAsByComponent.LastObject()->size;
  CandidateSSSubalgebra newCandidate;
  DynkinSimpleType theNewTypE=baseCandidate.theWeylNonEmbeddeD.theDynkinType.GetGreatestSimpleType();
  ProgressReport theReport(theGlobalVariables);
  ProgressReport theReport0(theGlobalVariables);
  ProgressReport theReport1(theGlobalVariables);
  ProgressReport theReport2(theGlobalVariables);
  if (numVectorsFound==theNewTypE.theRank)
  { newCandidate=baseCandidate;
    this->AddCandidatesSubalgebra(newCandidate, theGlobalVariables);
    if (!newCandidate.ComputeChar(theGlobalVariables))
    { theReport.Report("Candidate " + newCandidate.ToString() + " ain't no good");
      //std::cout << newCandidate.ToString() << " is bad<br>";
      return;
    }
    if (!newCandidate.ComputeSystem(theGlobalVariables))
    { theReport.Report("Candidate " + newCandidate.ToString() + " ain't no good");
      return;
    }
    for (int i=0; i<this->Hcandidates.size; i++)
      if (newCandidate.theWeylNonEmbeddeD.theDynkinType==
          this->Hcandidates[i].theWeylNonEmbeddeD.theDynkinType)
      { if (newCandidate.IsDirectSummandOf(this->Hcandidates[i], true))
          return;
        std::cout << "<hr><hr>Found two candidates with identical Dynkin types, equal to "
        << newCandidate.theWeylNonEmbeddeD.theDynkinType.ToString();
      }
//    std::cout << newCandidate.ToString() << "<b> is good</b><br>";
    newCandidate.indexInOwner=this->Hcandidates.size;
    this->Hcandidates.AddOnTop(newCandidate);
    if (!this->Hcandidates.LastObject()->indexInOwner==this->Hcandidates.size-1)
    { std::cout << "<hr>wtf? ";
      assert(false);
    }
    if (propagateRecursion)
      this->ExtendCandidatesRecursive(newCandidate, propagateRecursion, theGlobalVariables);
    return;
  }
  int indexFirstWeight=
  baseCandidate.theWeylNonEmbeddeD.CartanSymmetric.NumRows - theNewTypE.theRank;
  int indexCurrentWeight=indexFirstWeight+numVectorsFound;
  Rational desiredLengthSquared=theNewTypE.lengthFirstCoRootSquared;
  desiredLengthSquared*=theNewTypE.GetDefaultRootLengthSquared(0);
  desiredLengthSquared/=theNewTypE.GetDefaultRootLengthSquared(numVectorsFound);
  Vectors<Rational> startingVector;
  HashedList<Vector<Rational> > theOrbit;
  HashedList<ElementWeylGroup> theOrbitGenerators;
  Vector<Rational> Hrescaled;
  startingVector.SetSize(1);
  std::stringstream out0;
  if (theGlobalVariables!=0)
  { out0 << "Attempting to complete component " << theNewTypE.ToString()
    << ", root " << baseCandidate.CartanSAsByComponent.LastObject()->size+1
    << " out of " << theNewTypE.theRank << ".";
    theReport0.Report(out0.str());
  }
  for (int i=0; i<this->theSl2s.size; i++)
  { if (theGlobalVariables!=0)
    { std::stringstream reportStreamX;
      reportStreamX
      << "Extending  " << baseCandidate.theWeylNonEmbeddeD.theDynkinType.ToString() << ", "
      << numVectorsFound << " out of "
      << theNewTypE.theRank << ", desired length squared= " << desiredLengthSquared << " ."
      << " Current length is " << this->theSl2s[i].LengthHsquared;
      if (this->theSl2s[i].LengthHsquared!=desiredLengthSquared)
        reportStreamX << " which is no good.<br> ";
      else
        reportStreamX << " which is all nice and dandy.<br>";
      theReport1.Report(reportStreamX.str());
//      std::cout << "<br>" << reportStreamX.str();
    }
    if (this->theSl2s[i].LengthHsquared!=desiredLengthSquared)
      continue;
    startingVector[0]=this->theSl2s[i].theH.GetCartanPart();
    std::stringstream out;
    if (theGlobalVariables!=0)
    { out << "Generating orbit of " << startingVector[0].ToString() << "...";
      theReport.Report(out.str());
    }
    if (indexCurrentWeight!=0)
    { if (! this->GetSSowner().theWeyl.GenerateOrbit
        (startingVector, false, theOrbit, false, 1152, &theOrbitGenerators, 100000))
      { std::cout
        << "Failed to generate weight orbit: orbit has more than hard-coded limit of 100000 elements. "
        << " This is not a programming error, but I am crashing in flames to let you know "
        << " you hit the computational limits. You might wanna work on improving the algorithm "
        << " for generating semisimple subalgebras. "
        << " Here is a stack trace for you. "
        << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }
    } else
    { theOrbit=startingVector;
      ElementWeylGroup theId;
      theOrbitGenerators.Clear();
      theOrbitGenerators.AddOnTop(theId);
    }
    if (theGlobalVariables!=0)
    { out << " done. The size of the orbit is " << theOrbit.size;
      theReport.Report(out.str());
    }
    for (int j=0; j<theOrbit.size; j++)
    { std::stringstream out2;
      if (theGlobalVariables!=0)
      { out2 << "Exploring orbit element of index  " << j+1 << " out of "
        << theOrbit.size << ".";
        theReport2.Report(out2.str());
      }
      Hrescaled=theOrbit[j];
      Hrescaled*=theNewTypE.GetDefaultRootLengthSquared(numVectorsFound);
      Hrescaled/=2;
      if (baseCandidate.isGoodForTheTop(this->GetSSowner().theWeyl, Hrescaled))
      { if (theGlobalVariables!=0)
        { out2 << " the candidate is good. Attempting to extend it by recursion. ";
          theReport2.Report(out2.str());
        }
        //std::cout << "Is good!";
        newCandidate=baseCandidate;
        newCandidate.AddHincomplete(Hrescaled, theOrbitGenerators[j], i);
        this->ExtendOneComponentRecursive(newCandidate, propagateRecursion, theGlobalVariables);
      } else
        if (theGlobalVariables!=0)
        { out2 << " the candidate is no good. ";
          theReport2.Report(out2.str());
//            std::cout << "No good: " << baseCandidate.theTypes.ToString() << ", "
//            << baseCandidate.CartanSAsByComponent.LastObject()->ToString()
//            << " extra weight  " << Hrescaled.ToString()
//            << "<br>";
        }
    }
  }
}

bool CandidateSSSubalgebra::CheckInitialization()const
{ if (this->owner==0)
  { std::cout << "This is a programming error: use of non-initialized semisimple subalgebra "
    << " candidate. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }

  return true;
}

WeylGroup& CandidateSSSubalgebra::GetAmbientWeyl()const
{ this->CheckInitialization();
  return this->owner->GetSSowner().theWeyl;
}

SemisimpleLieAlgebra& CandidateSSSubalgebra::GetAmbientSS()
{ this->CheckInitialization();
  return this->owner->GetSSowner();
}

void CandidateSSSubalgebra::AddHincomplete
  (const Vector<Rational>& theH, const ElementWeylGroup& theWE, int indexOfOrbit)
{ MacroRegisterFunctionWithName("CandidateSSSubalgebra::AddHincomplete");
  this->CheckInitialization();
  if (this->CartanSAsByComponent.size==1)
    if (this->CartanSAsByComponent.LastObject()->size==0)
      this->PosRootsPerpendicularPrecedingWeights=this->GetAmbientWeyl().RootsOfBorel;
  for (int i=0; i<this->PosRootsPerpendicularPrecedingWeights.size; i++)
    if (this->GetAmbientWeyl().RootScalarCartanRoot(this->PosRootsPerpendicularPrecedingWeights[i], theH)!=0)
    { this->PosRootsPerpendicularPrecedingWeights.RemoveIndexShiftDown(i);
      i--;
    }
  this->CartanSAsByComponent.LastObject()->AddOnTop(theH);
  this->theHorbitIndices.LastObject()->AddOnTop(indexOfOrbit);
//  this->theHWeylGroupElts.LastObject()->AddOnTop(theWE);
  this->theInvolvedPosGenerators.LastObject()->SetSize
  (this->theInvolvedPosGenerators.LastObject()->size+1);
  this->theInvolvedNegGenerators.LastObject()->SetSize
  (this->theInvolvedNegGenerators.LastObject()->size+1);
}

void CandidateSSSubalgebra::AddTypeIncomplete(const DynkinSimpleType& theNewType)
{ if (theNewType.theRank<=0)
  { std::cout << "This is a programming error: I am given a simple Dynkin type of non-positive "
    << " rank. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  WeylGroup tempWeyl, tempWeylnonScaled;
  Rational two=2;
  tempWeyl.MakeArbitrarySimple(theNewType.theLetter, theNewType.theRank, &theNewType.lengthFirstCoRootSquared);
  tempWeylnonScaled.MakeArbitrarySimple(theNewType.theLetter, theNewType.theRank);
  this->theWeylNonEmbeddeD+=tempWeyl;
  this->theWeylNonEmbeddeDdefaultScale+=tempWeylnonScaled;

  this->CartanSAsByComponent.SetSize(this->CartanSAsByComponent.size+1);
  this->CartanSAsByComponent.LastObject()->size=0;
  this->theHorbitIndices.SetSize(this->theHorbitIndices.size+1);
  this->theHorbitIndices.LastObject()->size=0;
//  this->theHWeylGroupElts.SetSize(this->theHWeylGroupElts.size+1);
//  this->theHWeylGroupElts.LastObject()->size=0;
  this->theInvolvedNegGenerators.SetSize(this->theInvolvedNegGenerators.size+1);
  this->theInvolvedNegGenerators.LastObject()->size=0;
  this->theInvolvedPosGenerators.SetSize(this->theInvolvedPosGenerators.size+1);
  this->theInvolvedPosGenerators.LastObject()->size=0;
}

bool CandidateSSSubalgebra::isGoodForTheTop
  (WeylGroup& ownerWeyl, const Vector<Rational>& HneW)const
{ MacroRegisterFunctionWithName("CandidateSSSubalgebra::isGoodForTheTop");
  Rational theScalarProd;
  int counter=-1;
  int indexHnewInSmallType=this->CartanSAsByComponent.LastObject()->size;
  DynkinSimpleType currentType=this->theWeylNonEmbeddeD.theDynkinType.GetGreatestSimpleType();
  int indexHnew=
  this->theWeylNonEmbeddeD.CartanSymmetric.NumRows-currentType.theRank+indexHnewInSmallType;

  for (int k=0; k<this->CartanSAsByComponent.size; k++)
    for (int l=0; l<this->CartanSAsByComponent[k].size; l++)
    { counter++;
      theScalarProd= ownerWeyl.RootScalarCartanRoot
      (HneW, this->CartanSAsByComponent[k][l]);
      if (theScalarProd!=this->theWeylNonEmbeddeD.CartanSymmetric(indexHnew, counter))
        return false;
    }
  for (int i=0; i<this->PosRootsPerpendicularPrecedingWeights.size; i++)
    if (ownerWeyl.RootScalarCartanRoot
        (HneW, this->PosRootsPerpendicularPrecedingWeights[i])<0)
      return false;
  for (int i=0; i<this->CartanSAsByComponent.size; i++)
    if (this->CartanSAsByComponent[i].Contains(HneW))
    { std::cout << "This is a programming error: I am told that "
      << HneW.ToString() << " is an OK weight to extend the weight subsystem, "
      << " but the weight subsystem contains that weight already: "
      << this->CartanSAsByComponent[i].ToString()
      << ". "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
  Vectors<Rational> tempVectors;
  tempVectors.AssignListList(this->CartanSAsByComponent);
  tempVectors.AddOnTop(HneW);
  if (tempVectors.GetRankOfSpanOfElements()<tempVectors.size)
    return false;
  if (indexHnew==0)
    return ownerWeyl.IsDominantWeight(HneW);
  return true;
}

template <class CoefficientType>
int charSSAlgMod<CoefficientType>::GetIndexExtremeWeightRelativeToWeyl
(WeylGroup& theWeyl)const
{ HashedList<Vector<CoefficientType> > weightsSimpleCoords;
  weightsSimpleCoords.SetExpectedSize(this->size);
  for (int i=0; i<this->size; i++)
    weightsSimpleCoords.AddOnTop
    (theWeyl.GetSimpleCoordinatesFromFundamental((*this)[i].weightFundamentalCoords));
  for (int i=0; i<weightsSimpleCoords.size; i++)
  { bool isGood=true;
    for (int j=0; j<theWeyl.RootsOfBorel.size; j++)
      if(weightsSimpleCoords.Contains((weightsSimpleCoords[i]+theWeyl.RootsOfBorel[j])))
      { isGood=false;
        break;
      }
    if (isGood)
      return i;
  }
  return -1;
}

bool CandidateSSSubalgebra::IsWeightSystemSpaceIndex
(int theIndex, const Vector<Rational>& AmbientRootTestedForWeightSpace)
{ //std::cout << "<br>h's: " << this->theHs << " testing: "
  //<< AmbientRootTestedForWeightSpace << " for being e-possibility for "
  //<< this->theHs[theIndex];
  for (int k=0; k<this->theHs.size; k++)
  { Rational desiredScalarProd=
    this->theWeylNonEmbeddeDdefaultScale.CartanSymmetric(theIndex, k);
//    if (AmbientRootTestedForWeightSpace.ToString()=="(1, 1, 2)")
    /*{ std::cout << "<br>Desired scalar product of "
      << AmbientRootTestedForWeightSpace.ToString() << " and "
      << this->theHs[k].ToString() << ", determined by indices "
      << k << " and " << theIndex << ", is " << desiredScalarProd
      << " = "
      << this->GetAmbientWeyl().RootScalarCartanRoot(this->theHs[theIndex], this->theHs[k])
      << "*2/"
      << this->GetAmbientWeyl().RootScalarCartanRoot(this->theHs[theIndex], this->theHs[theIndex]);
    }*/
    Rational actualScalar=
    this->GetAmbientWeyl().RootScalarCartanRoot(this->theHs[k], AmbientRootTestedForWeightSpace);
    if (desiredScalarProd!=actualScalar)
    { //if (AmbientRootTestedForWeightSpace.ToString()=="(1, 1, 2)")
      //{ std::cout << ", instead got " << actualScalar;
      //}
      return false;
    }
  }
  return true;
}

bool CandidateSSSubalgebra::ComputeSystem
(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("CandidateSSSubalgebra::ComputeSystem");
  ChevalleyGenerator currentGen, currentOpGen;
  this->theHs.AssignListList(this->CartanSAsByComponent);
  this->theCoRoots.SetSize(this->theHs.size);
  int counter=-1;
  DynkinType dynkinTypeDefaultLengths;
  this->theWeylNonEmbeddeD.theDynkinType.GetDynkinTypeWithDefaultLengths(dynkinTypeDefaultLengths);
  this->theWeylNonEmbeddeDdefaultScale.MakeFromDynkinType(dynkinTypeDefaultLengths);
  this->theWeylNonEmbeddeDdefaultScale.ComputeRho(true);
  List<DynkinSimpleType> theTypes;
  this->theWeylNonEmbeddeD.theDynkinType.GetTypesWithMults(theTypes);
  for (int i=0; i<this->CartanSAsByComponent.size; i++)
    for (int j=0; j< this->CartanSAsByComponent[i].size; j++)
    { counter++;
      this->theCoRoots[counter]=
      (this->theHs[counter]/theTypes[i].GetDefaultRootLengthSquared(j))*2;
    }
  this->theInvolvedNegGenerators.SetSize(this->theCoRoots.size);
  this->theInvolvedPosGenerators.SetSize(this->theCoRoots.size);
  for (int i=0; i<this->theCoRoots.size; i++)
  { List<ChevalleyGenerator>& currentInvolvedPosGens= this->theInvolvedPosGenerators[i];
    List<ChevalleyGenerator>& currentInvolvedNegGens= this->theInvolvedNegGenerators[i];
    currentInvolvedNegGens.SetExpectedSize(this->GetAmbientWeyl().GetDim()*2);
    currentInvolvedPosGens.SetExpectedSize(this->GetAmbientWeyl().GetDim()*2);
    currentInvolvedNegGens.SetSize(0);
    currentInvolvedPosGens.SetSize(0);
    for (int j=0; j<this->GetAmbientWeyl().RootSystem.size; j++)
    { const Vector<Rational>& currentAmbRoot=this->GetAmbientWeyl().RootSystem[j];
      int indexCurGen=this->GetAmbientSS().GetGeneratorFromRootIndex(j);
      int opIndex= this->GetAmbientSS().GetGeneratorFromRoot(-currentAmbRoot);
      currentGen.MakeGenerator(*this->owner->owneR, indexCurGen);
      currentOpGen.MakeGenerator(*this->owner->owneR, opIndex);
      if (this->IsWeightSystemSpaceIndex(i, this->GetAmbientWeyl().RootSystem[j]))
      { currentInvolvedPosGens.AddOnTop(currentGen);
        currentInvolvedNegGens.AddOnTop(currentOpGen);
        //std::cout << "<br>Generator " << currentGen.ToString() << " is good";
      } //else
        //std::cout << "<br>Generator " << currentGen.ToString() << " ain't no good";
    }
    if (currentInvolvedNegGens.size==0)
      return false;
  }
  return this->ComputeSystemPart2(theGlobalVariables);
}

bool CandidateSSSubalgebra::ComputeSystemPart2
(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("CandidateSSSubalgebra::ComputeSystemPart2");
  theSystemToSolve.SetSize(0);
  ElementSemisimpleLieAlgebra<Polynomial<Rational> >
  lieBracketMinusGoalValue, goalValue;
  Vector<Polynomial<Rational> > desiredHpart;
  this->CheckInitialization();
//  if (this->indexInOwnersOfNonEmbeddedMe<0 || this->indexInOwnersOfNonEmbeddedMe >=this->owner->theSubalgebrasNonEmbedded
  const SemisimpleLieAlgebra& nonEmbeddedMe=
  this->owner->theSubalgebrasNonEmbedded[this->indexInOwnersOfNonEmbeddedMe];
  this->totalNumUnknowns=0;
  if (this->theHs.size==0)
  { std::cout << "This is a programming error: the number of involved H's cannot be zero. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  if (this->theInvolvedNegGenerators.size!=this->theHs.size)
  { std::cout << "This is a programming error: the number of involved negative generators, which is "
    << this->theInvolvedNegGenerators.size << " is not equal to the subalgebra rank, which is "
    << this->theHs.size << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  for (int i=0; i<this->theInvolvedNegGenerators.size; i++)
    this->totalNumUnknowns+=this->theInvolvedNegGenerators[i].size;
  if (this->theWeylNonEmbeddeD.RootSystem.size==0)
  { std::cout << "This is a programming error: the root system of the "
    << " candidate subalgebra has not been computed "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  this->totalNumUnknowns*=2;
  this->theUnknownNegGens.SetSize(this->theInvolvedNegGenerators.size);
  this->theUnknownPosGens.SetSize(this->theInvolvedPosGenerators.size);
  for (int i=0; i<this->theInvolvedNegGenerators.size; i++)
  { this->GetGenericNegGenLinearCombination(i, this->theUnknownNegGens[i]);
    this->GetGenericPosGenLinearCombination(i, this->theUnknownPosGens[i]);
    //std::cout << "<hr>Unknown generator index " << i << ": " << this->theUnknownNegGens[i].ToString();
  }
  for (int i=0; i<this->theInvolvedNegGenerators.size; i++)
  { desiredHpart=this->theCoRoots[i];//<-implicit type conversion here!
    goalValue.MakeHgenerator(desiredHpart, *this->owner->owneR);
    this->GetAmbientSS().LieBracket
    (this->theUnknownPosGens[i], this->theUnknownNegGens[i], lieBracketMinusGoalValue);
    //std::cout << "<hr>[" << this->theUnknownPosGens[i].ToString() << ", "
    //<< this->theUnknownNegGens[i].ToString() << "] = " << lieBracketMinusGoalValue.ToString();
    lieBracketMinusGoalValue-=goalValue;
    this->AddToSystem(lieBracketMinusGoalValue);
    for (int j=0; j<this->theInvolvedPosGenerators.size; j++)
      if (i!=j)
      { this->GetAmbientSS().LieBracket
        (this->theUnknownNegGens[i], this->theUnknownPosGens[j], lieBracketMinusGoalValue);
        //std::cout << "<hr>[" << this->theUnknownNegGens[i].ToString() << ", "
        //<< this->theUnknownPosGens[j].ToString() << "] = " << lieBracketMinusGoalValue.ToString();
        this->AddToSystem(lieBracketMinusGoalValue);

        Vector<Rational> posRoot1, posRoot2;
        posRoot1.MakeEi(this->theWeylNonEmbeddeD.GetDim(), i);
        posRoot2.MakeEi(this->theWeylNonEmbeddeD.GetDim(), j);
        int q;
        if (!nonEmbeddedMe.GetMaxQForWhichBetaMinusQAlphaIsARoot(posRoot1, -posRoot2, q))
        { std::cout << "This is a programming error: the alpha-string along "
          << posRoot1.ToString() << " through " << (-posRoot2).ToString()
          << " does not contain any root, which is impossible. "
          << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
          assert(false);
        }
        lieBracketMinusGoalValue=this->theUnknownPosGens[j];
        for (int k=0; k<q+1; k++)
          this->GetAmbientSS().LieBracket
          (this->theUnknownNegGens[i], lieBracketMinusGoalValue, lieBracketMinusGoalValue);
        //std::cout << "<hr>adjoint element, with power " << q+1 << ": " << lieBracketMinusGoalValue;
        this->AddToSystem(lieBracketMinusGoalValue);
      }
  }
  this->AttemptToSolveSytem(theGlobalVariables);
  this->theBasis=this->theNegGens;
  this->theBasis.AddListOnTop(this->thePosGens);
  if (this->theBasis.size>0)
  { this->owner->owneR->GenerateLieSubalgebra(this->theBasis);
    this->owner->owneR->GetCommonCentralizer(this->thePosGens, this->highestVectorsModules);
    this->ComputeCartanOfCentralizer(theGlobalVariables);
    this->ComputeCentralizinglySplitModuleDecomposition(theGlobalVariables);
  }
  return !this->flagSystemProvedToHaveNoSolution;
}

void CandidateSSSubalgebra::ExtendToModule
(List<ElementSemisimpleLieAlgebra<Rational> >& inputOutput, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("CandidateSSSubalgebra::ExtendToModule");
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  List<ElementSemisimpleLieAlgebra<Rational> > theVectorSpace;
  HashedList<ChevalleyGenerator> bufferList;
  theVectorSpace=inputOutput;
  ProgressReport theReport(theGlobalVariables);
  for (int i=0; i<inputOutput.size; i++)
    for (int j=0; j<this->theNegGens.size; j++)
    { std::stringstream reportStream;
      reportStream << "ExtendToModule: Lie bracket of element of index " << i+1 << " and negative generator index "
      << j+1 << ".";
      theReport.Report(reportStream.str());
      this->GetAmbientSS().LieBracket(this->theNegGens[j], inputOutput[i], tempElt);
      theVectorSpace.AddOnTop(tempElt);
      tempElt.GaussianEliminationByRowsDeleteZeroRows(theVectorSpace, 0, &bufferList);
      if (theVectorSpace.size>inputOutput.size)
        inputOutput.AddOnTop(tempElt);
    }
}

void CandidateSSSubalgebra::ComputePairingTablePreparation
(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("CandidateSSSubalgebra::ComputePairingTablePreparation");
  int totalDim=0;
  for (int i=0; i<this->modulesGrouppedByWeight.size; i++)
    for (int j=0; j<this->modulesGrouppedByWeight[i].size; j++)
    { this->ExtendToModule(this->modulesGrouppedByWeight[i][j], theGlobalVariables);
      totalDim+=this->modulesGrouppedByWeight[i][j].size;
    }
  if (totalDim!=this->GetAmbientSS().GetNumGenerators())
  { std::cout << "<br><b>Something went very wrong with candidate "
    << this->theWeylNonEmbeddeD.theDynkinType.ToString() << ": dimensions DONT FIT!!! More precisely, "
    << "I am getting total module dimension sum  " << totalDim << " instead of "
    << this->GetAmbientSS().GetNumGenerators()
    << ".</b>";
  }
  this->modulesGrouppedByPrimalType.SetSize(this->modulesGrouppedByWeight.size);
  for (int i=0; i<this->modulesGrouppedByWeight.size; i++)
  { this->modulesGrouppedByPrimalType[i].SetSize(0);
    for (int j=0; j<this->modulesGrouppedByWeight[i].size; j++)
      this->modulesGrouppedByPrimalType[i].AddListOnTop(this->modulesGrouppedByWeight[i][j]);
  }
  this->candidateSubalgebraModules.SetSize(0);
  this->primalSubalgebraModules.SetSize(0);
  for (int i=0; i<this->modulesGrouppedByPrimalType.size; i++)
    if (MonomialCollection<ChevalleyGenerator, Rational>::VectorSpacesIntersectionIsNonTrivial
        (this->theBasis, this->modulesGrouppedByPrimalType[i]))
    { this->candidateSubalgebraModules.AddOnTop(i);
      this->primalSubalgebraModules.AddOnTop(i);
    }
  for (int i=0; i<this->modulesGrouppedByPrimalType.size; i++)
    if (this->modulesGrouppedByPrimalType[i].size==1 &&
        this->modulesGrouppedByPrimalType[i][0].IsElementCartan())
      this->primalSubalgebraModules.AddOnTop(i);
  this->weightsOfModules.SetSize(this->modulesGrouppedByWeight.size);
  this->weightsOfPrimallySplitModules.SetSize(this->modulesGrouppedByWeight.size);
  Vector<Rational> theProjection, thePrimalProjection;
  for (int i=0; i<this->modulesGrouppedByWeight.size; i++)
    for (int k=0; k<this->modulesGrouppedByWeight[i].size; k++)
    { if (k==0)
      { this->weightsOfModules[i].SetSize(this->modulesGrouppedByWeight[i][0].size);
        this->weightsOfPrimallySplitModules[i].SetSize(this->modulesGrouppedByWeight[i][0].size);
      }
      for (int j=0; j<this->modulesGrouppedByWeight[i][k].size; j++)
      { ElementSemisimpleLieAlgebra<Rational>& currentElt=this->modulesGrouppedByWeight[i][k][j];
        Vector<Rational> currentRoot=this->GetAmbientSS().GetWeightOfGenerator(currentElt[0].theGeneratorIndex);
        this->GetWeightProjectionFundCoords(currentRoot, theProjection);
        this->GetPrimalWeightProjectionFundCoords(currentRoot, thePrimalProjection);
        if (k==0)
        { this->weightsOfModules[i][j]=theProjection;
          this->weightsOfPrimallySplitModules[i][j]=thePrimalProjection;
        } else
          if (this->weightsOfModules[i][j]!=theProjection ||
              this->weightsOfPrimallySplitModules[i][j]!=thePrimalProjection)
          { std::cout << "This is a programming or mathematical error. Given two isomorphic modules over "
            << "the semisimple subalgebra (i.e., same highest weights), "
            << "and the same order of generation of weights, I got different order "
            << " of the lower weights of the modules. Something is very wrong. "
            << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
            assert(false);
          }
      }
    }
}

void CandidateSSSubalgebra::ComputeSinglePair
(int leftIndex, int rightIndex, List<int>& output, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("CandidateSSSubalgebra::ComputeSinglePair");
  output.SetSize(0);
  List<ElementSemisimpleLieAlgebra<Rational> >& leftModule=this->modulesGrouppedByPrimalType[leftIndex];
  List<ElementSemisimpleLieAlgebra<Rational> >& rightModule=this->modulesGrouppedByPrimalType[rightIndex];
  ElementSemisimpleLieAlgebra<Rational> theLieBracket;
  ProgressReport theReport(theGlobalVariables);
  for (int i=0; i<leftModule.size; i++)
    for (int j=0; j<rightModule.size; j++)
    { this->GetAmbientSS().LieBracket(leftModule[i], rightModule[j], theLieBracket);
      std::stringstream reportStream;
      reportStream << "Bracketing indices " << i+1 << " and " << j+1 << ". ";
      theReport.Report(reportStream.str());
      if (!theLieBracket.IsEqualToZero())
        for (int k=0; k<this->modulesGrouppedByPrimalType.size; k++)
          if (theLieBracket.LinSpanContains(this->modulesGrouppedByPrimalType[k], theLieBracket))
          { output.AddOnTopNoRepetition(k);
            break;
          }
    }
}

void CandidateSSSubalgebra::ComputePairingTable
(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("CandidateSSSubalgebra::ComputePairingTable");
  ProgressReport theReport(theGlobalVariables);
  this->ComputePairingTablePreparation(theGlobalVariables);
  this->NilradicalPairingTable.SetSize(this->modulesGrouppedByPrimalType.size);
  for (int i=0; i<this->NilradicalPairingTable.size; i++)
    this->NilradicalPairingTable[i].SetSize(this->modulesGrouppedByPrimalType.size);
  for (int i=0; i<this->NilradicalPairingTable.size; i++)
    for (int j=i; j<this->NilradicalPairingTable[i].size; j++)
    { std::stringstream reportStream;
      reportStream << "Pairing indices " << i+1 << " and " << j+1 << " out of "
      << this->NilradicalPairingTable.size << ".";
      theReport.Report(reportStream.str());
      this->ComputeSinglePair(i, j, this->NilradicalPairingTable[i][j], theGlobalVariables);
      if (j>i)
        this->NilradicalPairingTable[j][i]=this->NilradicalPairingTable[i][j];
    }
  this->modulesWithZeroWeights.Clear();
  for (int i=0; i<this->NilradicalPairingTable.size; i++)
  { List<ElementSemisimpleLieAlgebra<Rational> >& currentMod=
    this->modulesGrouppedByPrimalType[i];
    for (int j=0; j<currentMod.size; j++)
      if (this->GetAmbientSS().GetWeightOfGenerator(currentMod[j][0].theGeneratorIndex).IsEqualToZero())
      { this->modulesWithZeroWeights.AddOnTop(i);
        break;
      }
  }
//  std::cout << "Nilradical pairing table size: " << this->NilradicalPairingTable.size;
  this->OppositeModules.SetSize(this->NilradicalPairingTable.size);
  for (int i=0; i<this->NilradicalPairingTable.size; i++)
    for (int j=0; j<this->NilradicalPairingTable[i].size; j++)
      for (int k=0; k<this->NilradicalPairingTable[i][j].size; k++)
        if (this->modulesWithZeroWeights.Contains(this->NilradicalPairingTable[i][j][k]))
          if (!(this->primalSubalgebraModules.Contains(i) &&
                this->primalSubalgebraModules.Contains(j)))
            this->OppositeModules[i].AddOnTopNoRepetition(j);
}

void CandidateSSSubalgebra::GetTheTwoCones
  (int inputFKIndex, Vectors<Rational>& outputNilradicalWeights, Vectors<Rational>& outputNonFKhws)
{ MacroRegisterFunctionWithName("CandidateSSSubalgebra::GetTheTwoCones");
  const List<int>& currentFKNilradicalCandidates= this->FKNilradicalCandidates[inputFKIndex];
  outputNilradicalWeights.SetSize(0);
  outputNonFKhws.SetSize(0);
  Vector<Rational> tempV;
  for (int i=0; i<currentFKNilradicalCandidates.size; i++)
    if (!this->primalSubalgebraModules.Contains(i))
    { if (currentFKNilradicalCandidates[i]==0)
        outputNonFKhws.AddOnTop(this->highestWeightsCartanCentralizerSplitModules[i]);
      else if (currentFKNilradicalCandidates[i]==1)
        for (int j=0; j<this->modulesGrouppedByPrimalType[i].size; j++)
        { ElementSemisimpleLieAlgebra<Rational>& curEl=this->modulesGrouppedByPrimalType[i][j];
          this->GetPrimalWeightProjectionFundCoords
          (this->GetAmbientSS().GetWeightOfGenerator(curEl[0].theGeneratorIndex), tempV);
          outputNilradicalWeights.AddOnTop(tempV);
        }
    }
}

void CandidateSSSubalgebra::EnumerateAllNilradicals(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("CandidateSSSubalgebra::EnumerateAllNilradicals");
  ProgressReport theReport(theGlobalVariables);
  theReport.Report("Enumerating recursively nilradicals...");
  List<int> theSel;
  this->RecursionDepthCounterForNilradicalGeneration=0;
  //0 stands for not selected, 1 for selected, 2 stands for unknown.
  theSel.initFillInObject(this->NilradicalPairingTable.size, 2);
  for(int i=0; i<this->primalSubalgebraModules.size; i++)
    theSel[this->primalSubalgebraModules[i]]=1;
  std::stringstream out;
  this->EnumerateNilradicalsRecursively(theSel, theGlobalVariables, &out);
  this->NilradicalConesIntersect.SetSize(this->FKNilradicalCandidates.size);
  Vector<Rational> emptyVector;
  this->ConeIntersections.initFillInObject(this->FKNilradicalCandidates.size, emptyVector);
  this->ConeSeparatingNormals.initFillInObject(this->FKNilradicalCandidates.size, emptyVector);
  this->theNilradicalWeights.SetSize(this->FKNilradicalCandidates.size);
  this->theNonFKhws.SetSize(this->FKNilradicalCandidates.size);
  for (int i=0; i<this->NilradicalConesIntersect.size; i++)
  { this->GetTheTwoCones(i, this->theNilradicalWeights[i], this->theNonFKhws[i]);
    this->NilradicalConesIntersect[i]=this->theNilradicalWeights[i].ConesIntersect
    (this->theNilradicalWeights[i], this->theNonFKhws[i], &this->ConeIntersections[i],
     &this->ConeSeparatingNormals[i], theGlobalVariables);
  }
//  this->nilradicalGenerationLog=out.str();
  //std::cout << out.str();
}

std::string CandidateSSSubalgebra::ToStringNilradicalSelection(const List<int>& theSelection)
{ Vector<Rational> undecided, included, excluded, tempV;
  included.MakeZero(this->NilradicalPairingTable.size);
  undecided.MakeZero(this->NilradicalPairingTable.size);
  excluded.MakeZero(this->NilradicalPairingTable.size);
  for (int i=0; i<theSelection.size; i++)
  { tempV.MakeEi(this->NilradicalPairingTable.size, i);
    if (theSelection[i]==0)
      excluded+=tempV;
    else if(theSelection[i]==1)
      included+=tempV;
    else
      undecided+=tempV;
  }
  std::stringstream out;
  out << "included modules: " << included.ToStringLetterFormat("V")
  << ", excluded modules: " << excluded.ToStringLetterFormat("V")
  << ", undecided modulues: " << undecided.ToStringLetterFormat("V");
  return out.str();
}

bool CandidateSSSubalgebra::IsPossibleNilradicalCarryOutSelectionImplications
(List<int>& theSelection, GlobalVariables* theGlobalVariables, std::stringstream* logStream)
{ if (this->FKNilradicalCandidates.size>100)
  { if (logStream!=0)
    { std::string tempS=logStream->str();
      if (tempS.size()>2)
        if (tempS[tempS.size()-1]!='.' || tempS[tempS.size()-2]!='.')
          *logStream << "<br>... number of nilradicals found exceeds 100, I will no longer log "
          << " the computations ...";
    }
    logStream=0;
  }
  if (logStream!=0)
    *logStream << "<br>********Considering selection " << this->ToStringNilradicalSelection(theSelection)
    << "********";
  HashedList<int, MathRoutines::IntUnsignIdentity> selectedIndices;
  for (int i=0; i<theSelection.size; i++)
    if (theSelection[i]==1)
      selectedIndices.AddOnTop(i);
  MemorySaving<List<int> > oldSelection;
  if (logStream!=0)
    oldSelection.GetElement()=theSelection;
  this->ExtendNilradicalSelectionToMultFreeOverSSpartSubalgebra(selectedIndices, theGlobalVariables, logStream);
  for (int i=0; i<theSelection.size; i++)
    if (theSelection[i]==0 && selectedIndices.Contains(i))
    { if (logStream!=0)
      { *logStream << " <br>The selection " << this->ToStringNilradicalSelection(theSelection)
        << " is contradictory, as the only way to extend it to a subalgebra (i.e., closed under Lie bracket)"
        << " is by requesting that module V_{" << i+1 << "} be included, but at the same time "
        << " we have already decided to exclude that module in one of our preceding choices. ";
      }
      return false;
    }
  for (int i=0; i<selectedIndices.size; i++)
    theSelection[selectedIndices[i]]=1;
  if (logStream!=0)
    if (oldSelection.GetElement()!=theSelection)
    { *logStream << "<br>In order to be closed w.r.t. the Lie bracket, I extend the nilradical selection "
      << this->ToStringNilradicalSelection(oldSelection.GetElement()) << " to the following. "
      << this->ToStringNilradicalSelection(theSelection);
    }
  for (int i=0; i<selectedIndices.size; i++)
    for (int j=0; j<this->OppositeModules[selectedIndices[i]].size; j++)
    { if (theSelection[this->OppositeModules[selectedIndices[i]][j]]==1)
      { if (logStream!=0)
        { *logStream << "<br>The subalgebra selection " << this->ToStringNilradicalSelection(theSelection)
          << " contains opposite modules and is therefore not allowed. ";
        }
        return false;
      }
      theSelection[this->OppositeModules[selectedIndices[i]][j]]=0;
    }
  return true;
}

void CandidateSSSubalgebra::ExtendNilradicalSelectionToMultFreeOverSSpartSubalgebra
(HashedList<int, MathRoutines::IntUnsignIdentity>& inputOutput, GlobalVariables* theGlobalVariables,
 std::stringstream* logStream)
{ for (int i=0; i<inputOutput.size; i++)
    for (int j=0; j<inputOutput.size; j++)
      for (int k=0; k<this->NilradicalPairingTable[inputOutput[i] ][inputOutput[j]].size; k++)
        inputOutput.AddOnTopNoRepetition(this->NilradicalPairingTable[inputOutput[i] ][inputOutput[j]][k]);
}

void CandidateSSSubalgebra::EnumerateNilradicalsRecursively
(List<int>& theSelection, GlobalVariables* theGlobalVariables, std::stringstream* logStream)
{ RecursionDepthCounter theCounter(&this->RecursionDepthCounterForNilradicalGeneration);
  if (this->RecursionDepthCounterForNilradicalGeneration>this->NilradicalPairingTable.size+1)
  { std::cout << "<br>oh no... something went very wrong! the nilradical generation recursion depth cannot "
    << "exceed the number of nilradicals! " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  if (!this->IsPossibleNilradicalCarryOutSelectionImplications(theSelection, theGlobalVariables, logStream))
    return;
  List<int> newSelection;
  bool found=false;
  for (int i=0; i<theSelection.size; i++)
    if (theSelection[i]==2)
    { found=true;
      newSelection=theSelection;
      newSelection[i]=0;
      this->EnumerateNilradicalsRecursively(newSelection, theGlobalVariables, logStream);
      newSelection=theSelection;
      newSelection[i]=1;
      this->EnumerateNilradicalsRecursively(newSelection, theGlobalVariables, logStream);
      break;
      //newSelection
    }
  if (found)
    return;
  this->FKNilradicalCandidates.AddOnTop(theSelection);
}

void CandidateSSSubalgebra::ComputeCentralizinglySplitModuleDecompositionWeightsOnly
(GlobalVariables* theGlobalVariables, HashedList<Vector<Rational> >& outputHWsDualCoords)
{ MacroRegisterFunctionWithName("CandidateSSSubalgebra::ComputeCentralizinglySplitModuleDecompositionWeightsOnly");
  outputHWsDualCoords.Clear();
  Vector<Rational> currentWeight, currentRootSpace;
  for (int i=0; i<this->highestVectorsModules.size; i++)
  { ElementSemisimpleLieAlgebra<Rational>& currentVector=this->highestVectorsModules[i];
    for (int j=0; j<currentVector.size; j++)
    { currentRootSpace=
      this->GetAmbientSS().GetWeightOfGenerator(currentVector[j].theGeneratorIndex);
      currentWeight.SetSize(this->theHs.size+ this->CartanOfCentralizer.size);
      for (int k=0; k<this->theHs.size; k++)
        currentWeight[k]=
        this->GetAmbientWeyl().RootScalarCartanRoot(currentRootSpace, this->theHs[k]);
      for (int k=0; k<this->CartanOfCentralizer.size; k++)
        currentWeight[k+this->theHs.size]=
        this->GetAmbientWeyl().RootScalarCartanRoot(currentRootSpace, this->CartanOfCentralizer[k]);
      outputHWsDualCoords.AddOnTopNoRepetition(currentWeight);
    }
  }
  outputHWsDualCoords.QuickSortAscending();
}

void CandidateSSSubalgebra::ComputeCentralizinglySplitModuleDecomposition
(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("CandidateSSSubalgebra::ComputeCentralizinglySplitModuleDecomposition");
  HashedList<Vector<Rational> > theWeightsCartanRestrictedDualCoords;
  this->ComputeCentralizinglySplitModuleDecompositionWeightsOnly
  (theGlobalVariables, theWeightsCartanRestrictedDualCoords);
  this->ComputeCentralizinglySplitModuleDecompositionHWVsOnly
  (theGlobalVariables, theWeightsCartanRestrictedDualCoords);
  this->ComputeCentralizinglySplitModuleDecompositionLastPart(theGlobalVariables);
  if (this->owner->flagDoComputePairingTable)
  { this->ComputePairingTable(theGlobalVariables);
    if (this->owner->flagDoComputeNilradicals)
      this->EnumerateAllNilradicals(theGlobalVariables);
  }
}

void CandidateSSSubalgebra::GetWeightProjectionFundCoords
(const Vector<Rational>& inputAmbientweight, Vector<Rational>& output)
{ MacroRegisterFunctionWithName("CandidateSSSubalgebra::GetWeightProjectionFundCoords");
  output.SetSize(this->theHs.size);
  for (int j=0; j<this->theHs.size; j++)
    output[j]= this->GetAmbientWeyl().RootScalarCartanRoot(inputAmbientweight, this->theHs[j])*2/
    this->theWeylNonEmbeddeDdefaultScale.CartanSymmetric(j,j);
}

void CandidateSSSubalgebra::GetPrimalWeightProjectionFundCoords
(const Vector<Rational>& inputAmbientweight, Vector<Rational>& output)
{ MacroRegisterFunctionWithName("CandidateSSSubalgebra::GetPrimalWeightProjectionFundCoords");
  output.SetSize(this->theHs.size+this->CartanOfCentralizer.size);
  for (int j=0; j<this->theHs.size; j++)
    output[j]= this->GetAmbientWeyl().RootScalarCartanRoot(inputAmbientweight, this->theHs[j])*2/
    this->theWeylNonEmbeddeDdefaultScale.CartanSymmetric(j,j);
  for (int j=0; j<this->CartanOfCentralizer.size; j++)
    output[j+this->theHs.size]=
    this->GetAmbientWeyl().RootScalarCartanRoot(inputAmbientweight, this->CartanOfCentralizer[j]);
}

void CandidateSSSubalgebra::ComputeCentralizinglySplitModuleDecompositionLastPart
(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("CandidateSSSubalgebra::ComputeCentralizinglySplitModuleDecompositionLastPart");
  this->highestWeightsCartanCentralizerSplitModules.SetSize(this->highestVectorsModules.size);
  this->highestWeightsModules.SetSize(this->highestVectorsModules.size);
  this->theCharOverCartanPlusCartanCentralizer.MakeZero(0);
  MonomialChar<Rational> theWeight;
  for (int i=0; i< this->highestVectorsModules.size; i++)
  { Vector<Rational> currentRoot=this->GetAmbientSS().GetWeightOfGenerator
    (this->highestVectorsModules[i][0].theGeneratorIndex);
    Vector<Rational>& currentHWrelative=this->highestWeightsModules[i];
    Vector<Rational>& currentHWCentralizerRelative=
    this->highestWeightsCartanCentralizerSplitModules[i];
    this->GetWeightProjectionFundCoords(currentRoot, currentHWrelative);
    this->GetPrimalWeightProjectionFundCoords(currentRoot, currentHWCentralizerRelative);
    theWeight.weightFundamentalCoords=currentHWCentralizerRelative;
    this->theCharOverCartanPlusCartanCentralizer.AddMonomial(theWeight, 1);
  }
  this->highestVectorsGrouppedByWeight.SetSize(this->theCharOverCartanPlusCartanCentralizer.size);
  this->modulesGrouppedByWeight.SetSize(this->theCharOverCartanPlusCartanCentralizer.size);
  for (int i=0; i<this->theCharOverCartanPlusCartanCentralizer.size; i++)
  { this->highestVectorsGrouppedByWeight[i].SetSize(0);
    this->modulesGrouppedByWeight[i].SetSize(0);
  }
  for (int i=0; i<this->highestVectorsModules.size; i++)
  { theWeight.weightFundamentalCoords=this->highestWeightsCartanCentralizerSplitModules[i];
    int theModuleIndex=this->theCharOverCartanPlusCartanCentralizer.GetIndex(theWeight);
    this->highestVectorsGrouppedByWeight[theModuleIndex].AddOnTop(this->highestVectorsModules[i]);
    this->modulesGrouppedByWeight[theModuleIndex].SetSize(this->modulesGrouppedByWeight[theModuleIndex].size+1);
    this->modulesGrouppedByWeight[theModuleIndex].LastObject()->AddOnTop(this->highestVectorsModules[i]);
  }
}

void CandidateSSSubalgebra::ComputeCentralizinglySplitModuleDecompositionHWVsOnly
(GlobalVariables* theGlobalVariables, HashedList<Vector<Rational> >& inputHws)
{ MacroRegisterFunctionWithName("CandidateSSSubalgebra::ComputeCentralizinglySplitModuleDecompositionHWVsOnly");
  List<Matrix<Rational> > theAdsOfHs;
  Matrix<Rational> tempAd, temp, commonAd, adIncludingCartanActions;
  for (int i=0; i<this->thePosGens.size; i++)
  { this->GetAmbientSS().GetAd(tempAd, this->thePosGens[i]);
    commonAd.AppendMatrixToTheBottom(tempAd);
  }
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  Vectors<Rational> allHs;
  allHs.AddListOnTop(this->theHs);
  allHs.AddListOnTop(this->CartanOfCentralizer);
  theAdsOfHs.SetSize(allHs.size);
  for (int j=0; j<allHs.size; j++)
  { tempElt.MakeHgenerator(allHs[j], this->GetAmbientSS());
    this->GetAmbientSS().GetAd(theAdsOfHs[j], tempElt);
  }
  Vectors<Rational> outputV;
  this->highestVectorsModules.size=0;
  for (int i=0; i<inputHws.size; i++)
  { adIncludingCartanActions=commonAd;
    for (int j=0; j<allHs.size; j++)
    { tempAd=theAdsOfHs[j];
      temp.MakeIdMatrix(this->GetAmbientSS().GetNumGenerators());
      temp*=inputHws[i][j];
      tempAd-=temp;
      adIncludingCartanActions.AppendMatrixToTheBottom(tempAd);
    }
    adIncludingCartanActions.GetZeroEigenSpace(outputV);
    //  std::cout << "<br>Common ad: " << commonAd.ToString();
    //  std::cout << "<br>Eigenvectors: " << outputV.ToString();
    for (int j=0; j<outputV.size; j++)
    { outputV[j].ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
      tempElt.AssignVectorNegRootSpacesCartanPosRootSpaces(outputV[j], this->GetAmbientSS());
      this->highestVectorsModules.AddOnTop(tempElt);
    }
  }
}

void SemisimpleSubalgebras::reset()
{ this->owneR=0;
  this->theRecursionCounter=0;
  this->theSl2s.owner=0;
  this->flagAttemptToSolveSystems=true;
  this->flagDoComputePairingTable=true;
}

bool CandidateSSSubalgebra::AttemptToSolveSytem
(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("CandidateSSSubalgebra::AttemptToSolveSystem");
  this->CheckInitialization();
  this->flagSystemSolved=false;
  this->flagSystemProvedToHaveNoSolution=false;
  this->transformedSystem=this->theSystemToSolve;
  GroebnerBasisComputation<Rational> theComputation;
  std::cout << "<br>System before transformation: " << this->transformedSystem.ToString();
  theComputation.SolveSerreLikeSystem(this->transformedSystem, theGlobalVariables);
  std::cout << " <br>And after: " << this->transformedSystem.ToString();
  this->flagSystemSolved=theComputation.flagSystemSolvedOverBaseField;
  this->flagSystemProvedToHaveNoSolution=theComputation.flagSystemProvenToHaveNoSolution;
  this->flagSystemGroebnerBasisFound=this->flagSystemSolved;
  if (this->flagSystemSolved)
  { this->theNegGens.SetSize(this->theUnknownNegGens.size);
    this->thePosGens.SetSize(this->theUnknownPosGens.size);
    PolynomialSubstitution<Rational> theSub;
    theSub.SetSize(theComputation.systemSolution.GetElement().size);
    for (int i=0; i<theSub.size; i++)
      theSub[i].MakeConst(theComputation.systemSolution.GetElement()[i]);
    ElementSemisimpleLieAlgebra<Polynomial<Rational> > currentNegElt;
    ElementSemisimpleLieAlgebra<Polynomial<Rational> > currentPosElt;
    for (int i=0; i<this->theUnknownNegGens.size; i++)
    { currentNegElt=this->theUnknownNegGens[i];
      currentPosElt=this->theUnknownPosGens[i];
      currentNegElt.SubstitutionCoefficients(theSub);
      currentPosElt.SubstitutionCoefficients(theSub);
      this->theNegGens[i]=currentNegElt;
      this->thePosGens[i]=currentPosElt;
    }
  }
  return this->flagSystemSolved;
}

void CandidateSSSubalgebra::GetGenericNegGenLinearCombination
  (int indexNegGens, ElementSemisimpleLieAlgebra<Polynomial<Rational> >& output)
{/* if (indexNegGens==0)
  { Polynomial<Rational> tempP;
    slTwoSubalgebra& curSl2=this->owner->theSl2s[this->theHorbitIndices[0][0]];
    output.MakeZero(*this->owner->owners, this->owner->indexInOwners);
    for (int j=0; j<curSl2.theF.size; j++)
    { tempP.MakeConst(this->totalNumUnknowns, curSl2.theF.theCoeffs[j]);
      output.AddMonomial(curSl2.theF[j], tempP);
    }
    return;
  }*/
  int offsetIndex=0;
  for (int i=0; i<indexNegGens; i++)
    offsetIndex+=this->theInvolvedNegGenerators[i].size;
  this->GetGenericLinearCombination
  (this->totalNumUnknowns, offsetIndex, this->theInvolvedNegGenerators[indexNegGens], output);
}

void CandidateSSSubalgebra::GetGenericPosGenLinearCombination
  (int indexPosGens, ElementSemisimpleLieAlgebra<Polynomial<Rational> >& output)
{ /*if (indexPosGens==0)
  { Polynomial<Rational> tempP;
    slTwoSubalgebra& curSl2=this->owner->theSl2s[this->theHorbitIndices[0][0]];
    output.MakeZero(*this->owner->owners, this->owner->indexInOwners);
    for (int j=0; j<curSl2.theE.size; j++)
    { tempP.MakeConst(this->totalNumUnknowns, curSl2.theE.theCoeffs[j]);
      output.AddMonomial(curSl2.theE[j], tempP);
    }
    return;
  }*/
  int offsetIndex=0;
  for (int i=0; i<indexPosGens; i++)
    offsetIndex+=this->theInvolvedPosGenerators[i].size;
  offsetIndex+=this->totalNumUnknowns/2;
  this->GetGenericLinearCombination
  (this->totalNumUnknowns, offsetIndex, this->theInvolvedPosGenerators[indexPosGens], output);
}

void CandidateSSSubalgebra::AddToSystem
  (const ElementSemisimpleLieAlgebra<Polynomial<Rational> >& elementThatMustVanish)
{ Polynomial<Rational> thePoly;
//  std::cout << "<hr>I must vanish: " << elementThatMustVanish.ToString();
  for (int i=0; i<elementThatMustVanish.size; i++)
  { thePoly=elementThatMustVanish.theCoeffs[i];
    thePoly.ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
    this->theSystemToSolve.AddOnTopNoRepetition(thePoly);
  }
}

void CandidateSSSubalgebra::GetGenericLinearCombination
(int numVars, int varOffset, List<ChevalleyGenerator>& involvedGens,
 ElementSemisimpleLieAlgebra<Polynomial<Rational> >& output)
{ Polynomial<Rational> theCF;
  ElementSemisimpleLieAlgebra<Polynomial<Rational> > tempMon;
  output.MakeZero(*this->owner->owneR);
  for (int i=0; i<involvedGens.size; i++)
  { theCF.MakeDegreeOne(numVars, varOffset+i, 1);
    tempMon.MakeGenerator(involvedGens[i].theGeneratorIndex, *this->owner->owneR)
    ;
    tempMon*=theCF;
    output+=tempMon;
  }
}

bool CandidateSSSubalgebra::ComputeChar
(GlobalVariables* theGlobalVariables)
{ if (this->indexInOwnersOfNonEmbeddedMe==-1)
  { std::cout << "This is a programming error: attempting to compute char "
    << "of candidate subalgebra that has not been initialized properly. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  MacroRegisterFunctionWithName("CandidateSSSubalgebra::ComputeChar");
  this->CheckInitialization();
  this->theWeylNonEmbeddeD.ComputeRho(true);
  MonomialChar<Rational> tempMon;
  tempMon.weightFundamentalCoords.MakeZero(this->theWeylNonEmbeddeD.GetDim());
  this->theCharFundamentalCoordsRelativeToCartan.Reset();
  this->theCharFundamentalCoordsRelativeToCartan.AddMonomial
  (tempMon, this->GetAmbientSS().GetRank());
  List<DynkinSimpleType> theTypes;
  this->theWeylNonEmbeddeD.theDynkinType.GetTypesWithMults(theTypes);
  for (int k=0; k<this->GetAmbientWeyl().RootSystem.size; k++)
  { int counter=-1;
    for (int i=0; i<this->CartanSAsByComponent.size; i++)
      for (int j=0; j<this->CartanSAsByComponent[i].size; j++)
      { counter++;
        tempMon.weightFundamentalCoords[counter]=
        this->GetAmbientWeyl().RootScalarCartanRoot
        (this->GetAmbientWeyl().RootSystem[k], this->CartanSAsByComponent[i][j])
        /theTypes[i].GetDefaultRootLengthSquared(j)*2
        ;
      }
    this->theCharFundamentalCoordsRelativeToCartan.AddMonomial(tempMon, 1);
  }
  charSSAlgMod<Rational> accumChar, freudenthalChar, outputChar;
  accumChar=this->theCharFundamentalCoordsRelativeToCartan;

  this->theCharFundCoords.MakeZero
  (&this->owner->theSubalgebrasNonEmbedded.GetElement(this->indexInOwnersOfNonEmbeddedMe))
  ;
  while (accumChar.size>0)
  { int currentIndex=
    accumChar.GetIndexExtremeWeightRelativeToWeyl(this->theWeylNonEmbeddeD);
    if (currentIndex==-1)
    { std::cout << "This is a programming error: while decomposing ambient Lie algebra "
      << "over the candidate subalgebra, I got that there is no extreme weight. This is "
      << " impossible: something has gone very wrong. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    if (accumChar.theCoeffs[currentIndex]<0)
      return false;
    for (int i=0; i<accumChar[currentIndex].weightFundamentalCoords.size; i++)
      if (accumChar[currentIndex].weightFundamentalCoords[i]<0)
        return false;
    freudenthalChar.MakeZero
    (&this->owner->theSubalgebrasNonEmbedded.GetElement(this->indexInOwnersOfNonEmbeddedMe));
    freudenthalChar.AddMonomial(accumChar[currentIndex], accumChar.theCoeffs[currentIndex]);
    this->theCharFundCoords.AddMonomial(accumChar[currentIndex], accumChar.theCoeffs[currentIndex]);
    std::string tempS;
    bool tempBool=freudenthalChar.FreudenthalEvalMeFullCharacter
    (outputChar, -1, &tempS, theGlobalVariables);
    if (!tempBool)
    { std::cout << "This is a programming error: failed to evaluate full character via "
      << " the Freudenthal formula on "
      << " a relatively small example, namely " << freudenthalChar.ToString()
      << ". The failure message was: " << tempS
      << ". This shouldn't happen. "
      <<  CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    accumChar-=outputChar;
  }
  return true;
}

void SemisimpleSubalgebras::ExtendOneComponentOneTypeAllLengthsRecursive
(const CandidateSSSubalgebra& baseCandidate, DynkinSimpleType& theType, bool propagateRecursion,
 GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("SemisimpleSubalgebras::ExtendOneComponentOneTypeAllLengthsRecursive");
  baseCandidate.CheckInitialization();
  CandidateSSSubalgebra theCandidate;
  SemisimpleLieAlgebra tempAlgebra;
  ProgressReport theProgressReport1(theGlobalVariables);
  ProgressReport theProgressReport2(theGlobalVariables);
  ProgressReport theProgressReport3(theGlobalVariables);
  int currentRank=baseCandidate.theWeylNonEmbeddeD.CartanSymmetric.NumRows;
  MonomialChar<Rational> tempMon;
  tempMon.weightFundamentalCoords.MakeZero(currentRank);
  Rational dimCentralizerBase=this->GetSSowner().GetNumGenerators();
  if (!baseCandidate.theCharFundCoords.Contains(tempMon))
  { if (currentRank>0)
      return;
  } else
    dimCentralizerBase=
    baseCandidate.theCharFundCoords.theCoeffs[baseCandidate.theCharFundCoords.GetIndex(tempMon)];
  Rational baseLength=-1;
  List<DynkinSimpleType> theTypes;
  baseCandidate.theWeylNonEmbeddeD.theDynkinType.GetTypesWithMults(theTypes);
  if (theTypes.size!=0)
    if (theType.theRank==theTypes.LastObject()->theRank &&
        theType.theLetter==theTypes.LastObject()->theLetter)
      baseLength=theTypes.LastObject()->lengthFirstCoRootSquared;
  //if (this->GetSSowner().GetRank()-(+)
  std::stringstream consideringStream;
  consideringStream << "\nThe centralizer dimension is " << dimCentralizerBase;
  consideringStream << "<br>\nCurrent rank: " << currentRank
  << "<br>\nConsidering: " << theType.ToString();
//  std::cout << "<br>Considering: " << theType.ToString();
  theProgressReport1.Report(consideringStream.str());
  if (currentRank!=0 && dimCentralizerBase < theType.GetSSAlgDim())
  { consideringStream << " turns out to be no good as the desired root system size is "
    << theType.GetRootSystemSize() << " but I have only "
    << 2*baseCandidate.PosRootsPerpendicularPrecedingWeights.size
    << " weights to use. ";
    theProgressReport1.Report(consideringStream.str());
    return;
  }
  if (theType.theRank+ baseCandidate.theWeylNonEmbeddeD.GetDim()==this->owneR->GetRank())
    for (int i=0; i<theTypes.size; i++)
      if (!this->owneR->theWeyl.theDynkinType.IsPossibleCoRootLength
          (theTypes[i].lengthFirstCoRootSquared))
        return;
  for (int k=0; k<this->theSl2s.size; k++)
  { theType.lengthFirstCoRootSquared=this->theSl2s[k].LengthHsquared;
    if (theType.theRank+ baseCandidate.theWeylNonEmbeddeD.GetDim()==this->owneR->GetRank())
      if (!this->owneR->theWeyl.theDynkinType.IsPossibleCoRootLength(theType.lengthFirstCoRootSquared))
        continue;
    if (theType.lengthFirstCoRootSquared<baseLength)
      continue;
    theCandidate=baseCandidate;
    tempAlgebra.theWeyl.MakeArbitrarySimple(theType.theLetter, theType.theRank);
    int indexSubalgebra=this->SimpleComponentsSubalgebras.GetIndex(tempAlgebra);
    bool mustComputeSSalgebra=(indexSubalgebra==-1);
    if (mustComputeSSalgebra)
    { indexSubalgebra=this->SimpleComponentsSubalgebras.size;
      this->SimpleComponentsSubalgebras.AddOnTop(tempAlgebra);
      this->theSl2sOfSubalgebras.SetSize(this->theSl2sOfSubalgebras.size+1);
    }
    SemisimpleLieAlgebra& theSmallAlgebra=this->SimpleComponentsSubalgebras[indexSubalgebra];
    SltwoSubalgebras& theSmallSl2s=this->theSl2sOfSubalgebras[indexSubalgebra];
    if (mustComputeSSalgebra)
    { std::stringstream tempStream;
      tempStream << "\nGenerating simple Lie algebra "
      << theType.ToString()
      << " (total " << this->SimpleComponentsSubalgebras.size << ")...";
      theProgressReport2.Report(tempStream.str());
      theSmallAlgebra.ComputeChevalleyConstantS(theGlobalVariables);
      theSmallAlgebra.FindSl2Subalgebras
      (theSmallAlgebra, theSmallSl2s, *theGlobalVariables);
      tempStream << " done.";
      theProgressReport2.Report(tempStream.str());
    }
    bool isGood=true;
    for (int i=0; i<theSmallSl2s.size; i++)
    { isGood=false;
      for (int j=0; j<this->theSl2s.size; j++)
      { if (!theSmallSl2s[i].ModuleDecompositionFitsInto(this->theSl2s[j]))
          continue;
        isGood=true;
        break;
      }
      if (!isGood)
        break;
    }
    if (theGlobalVariables!=0)
    { std::stringstream out;
      out << " \n" << theType.ToString();
      if (isGood)
      { out << "<br>" << theType.ToString() << " has fitting sl(2) decompositions.";
        //std::cout  << "<br>" << theType.ToString() << " has fitting sl(2) decompositions.";
      } else
      { out << "<br>" << theType.ToString() << " does not have fitting sl(2) decompositions.";
        //std::cout  << "<br>" << theType.ToString() << " does not have fitting sl(2) decompositions.";
      }
      theProgressReport3.Report(out.str());
    }
    if (isGood)
    { theCandidate.AddTypeIncomplete(theType);
      this->ExtendOneComponentRecursive(theCandidate, propagateRecursion, theGlobalVariables);
    }
  }
}

void SemisimpleSubalgebras::ExtendCandidatesRecursive
  (const CandidateSSSubalgebra& baseCandidate, bool propagateRecursion,
   GlobalVariables* theGlobalVariables)
{ RecursionDepthCounter theCounter(&this->theRecursionCounter);
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::ExtendCandidatesRecursive");
  baseCandidate.CheckInitialization();
  DynkinSimpleType theType;
  ProgressReport theProgressReport1(theGlobalVariables);
  DynkinSimpleType myType;
  myType.theLetter='F';
  myType.theRank=this->GetSSowner().GetRank();
  myType.lengthFirstCoRootSquared=this->theSl2s[0].LengthHsquared;
  if(theGlobalVariables!=0)
  { std::stringstream out;
    out << "\nExploring extensions of " << baseCandidate.ToString();
    theProgressReport1.Report(out.str());
    //std::cout << "Exploring extensions of " << baseCandidate.ToString() << " by: ";
  }
  List<DynkinSimpleType> theTypes;
  baseCandidate.theWeylNonEmbeddeD.theDynkinType.GetTypesWithMults(theTypes);
  if (theTypes.size==0)
    theType.MakeAone();
  else
    theType=*theTypes.LastObject();
  for (; theType<myType; theType++, theType.lengthFirstCoRootSquared=-1)
    this->ExtendOneComponentOneTypeAllLengthsRecursive
      (baseCandidate, theType, propagateRecursion, theGlobalVariables);
}

void slTwoSubalgebra::ElementToStringModuleDecompositionMinimalContainingRegularSAs
(bool useLatex, bool useHtml, SltwoSubalgebras& owner, std::string& output)const
{ std::stringstream out;
  std::string tempS;
  if (useLatex)
    out << "$";
  if (useHtml)
  { out << "<table><tr><td align=\"center\">Char.</td>";
    for (int i=0; i<this->IndicesMinimalContainingRootSA.size; i++)
    { rootSubalgebra& theSA= owner.theRootSAs[this->IndicesMinimalContainingRootSA[i]];
      out << "<td align=\"center\">Decomp. " << theSA.theDynkinDiagram.ToString() << "</td>";
    }
    out << "</tr>\n";
  }
  out << "<tr><td align=\"center\"> " << this->hCharacteristic.ToString() << "</td>";
  for (int k=0; k<this->IndicesMinimalContainingRootSA.size; k++)
  { rootSubalgebra& theSA= owner.theRootSAs[this->IndicesMinimalContainingRootSA[k]];
    tempS=theSA.theDynkinDiagram.ToString();
    if (useHtml)
      out << "<td align=\"center\">";
    for (int i=0; i<this->HighestWeightsDecompositionMinimalContainingRootSA[k].size; i++)
    { if (this->MultiplicitiesDecompositionMinimalContainingRootSA[k][i]>1)
        out << this->MultiplicitiesDecompositionMinimalContainingRootSA[k][i];
      out << "V_{"<<this->HighestWeightsDecompositionMinimalContainingRootSA[k][i] << "}";
      if (i!=this->HighestWeightsDecompositionMinimalContainingRootSA[k].size-1)
        out << "+";
    }
    if (useHtml)
      out << "</td>";
    out << "\n";
  }
  if (useHtml)
    out << "</tr></table>";
  if (useLatex)
    out << "$";
  output=out.str();
}

void slTwoSubalgebra::ElementToHtmlCreateFormulaOutputReference
(const std::string& formulaTex, std::stringstream& output, bool usePNG, bool useHtml,
 SltwoSubalgebras& container, std::string* physicalPath, std::string* htmlPathServer)const
{ if (!usePNG)
  { output << formulaTex;
    //if (useHtml)
      //output<<"\n<br>\n";
    return;
  }
  std::stringstream tempStream;
  container.texFileNamesForPNG.SetSize(container.texFileNamesForPNG.size+1);
  container.texStringsEachFile.SetSize(container.texFileNamesForPNG.size);
  (*container.texStringsEachFile.LastObject())=formulaTex;
  tempStream << (*physicalPath) << "fla";
  tempStream << container.texFileNamesForPNG.size << ".tex";
  container.texFileNamesForPNG.TheObjects[container.texFileNamesForPNG.size-1]=tempStream.str();
  output << "<img src=\"" << (*htmlPathServer) << "fla" << container.texFileNamesForPNG.size
  << ".png\">";
  if (useHtml)
    output << "\n<br>\n";
}

WeylGroup& slTwoSubalgebra::GetOwnerWeyl()
{ return this->GetOwnerSSAlgebra().theWeyl;
}

bool slTwoSubalgebra::operator==(const slTwoSubalgebra& right)const
{// See Dynkin, Semisimple Lie subalgebras of semisimple Lie algebras, chapter 7-10
  if (this->owneR!=right.owneR)
  { std::cout << "This is a programming error: comparing sl(2) subalgebras"
    << "that have different ambient Lie algebras. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->hCharacteristic==(right.hCharacteristic);
}

std::string slTwoSubalgebra::ToString(FormatExpressions* theFormat)const
{ if (this->container==0)
    return "sl(2) subalgebra not initialized.";
  std::stringstream out;  std::string tempS;
  out << "<a name=\"sl2index" << indexInContainer << "\">h-characteristic: "
  << this->hCharacteristic.ToString() << "</a>";
  out << "<br>Length of the weight dual to h: "
  << this->LengthHsquared;
  tempS=this->preferredAmbientSimpleBasis.ToString();
  std::string physicalPath="";
  std::string htmlPathServer="";
  bool usePNG=false;
  bool useHtml=true;
  bool useLatex=false;
  if (theFormat!=0)
    if (theFormat->PathPhysicalOutputFolder!="")
    { physicalPath=theFormat->PathPhysicalOutputFolder+ "sl2s/";
      htmlPathServer=theFormat->PathDisplayNameCalculator + "sl2s/";
      usePNG=theFormat->flagUsePNG;
    }
  if (physicalPath=="" || htmlPathServer=="")
  { usePNG=false;
    useHtml=false;
  }
  if (useHtml)
    out << "<br>";
  out << "\nSimple basis ambient algebra w.r.t defining h: " << tempS;
  Matrix<Rational> tempMat;
  if (useHtml)
    out << "<br>";
  if (this->IndicesContainingRootSAs.size>1)
  { out << "Number of containing regular semisimple subalgebras: "
    << this->IndicesContainingRootSAs.size;
    if (useHtml)
      out << "<br>";
  }
  FormatExpressions localFormat, latexFormat;
  localFormat.flagUseHTML=useHtml;
  localFormat.flagUseLatex=useLatex;
  latexFormat.flagUseHTML=false;
  latexFormat.flagUseLatex=true;
  for (int i=0; i<this->IndicesContainingRootSAs.size; i++)
  { out << "\nContaining regular semisimple subalgebra number " << i+1 << ": ";
    rootSubalgebra& currentSA= this->container->theRootSAs[this->IndicesContainingRootSAs[i]];
    if (useHtml)
    { out << "<a href=\"" << htmlPathServer << "../rootHtml_rootSA"
      << this->IndicesContainingRootSAs[i] << ".html\">";
      tempS=currentSA.theDynkinDiagram.ToString();
    }
    out << tempS;
    if (useHtml)
      out << "</a>";
  }
  if (useHtml)
    out << "<br>";
  out << "\nsl(2)-module decomposition of the ambient Lie algebra: ";
  this->ElementToStringModuleDecomposition(useLatex, useHtml, tempS);
  this->ElementToHtmlCreateFormulaOutputReference
  ("$"+tempS+"$", out, usePNG, useHtml, *this->container, &physicalPath, &htmlPathServer);
  if (indexInContainer!=-1)
  { this->container->IndicesSl2decompositionFlas.SetSize(this->container->size);
    this->container->IndicesSl2decompositionFlas[indexInContainer]=
    this->container->texFileNamesForPNG.size-1;
  }
  out << "\nThe below list one possible realization of the sl(2) subalgebra.";
  if (useHtml)
    out << "\n<br>\n";
  std::stringstream tempStreamH, tempStreamE, tempStreamF, tempStreamHE, tempStreamHF, tempStreamEF;
  tempS=this->theH.ToString(theFormat);
  tempStreamH << "\n$h=$ $" << tempS << "$";
  tempS= tempStreamH.str();
  this->ElementToHtmlCreateFormulaOutputReference
  (tempS, out, usePNG, useHtml, *this->container, &physicalPath, &htmlPathServer);
  tempStreamE << "\n$e=$ $" << this->theE.ToString(theFormat) << "$";
  tempS= tempStreamE.str();
  this->ElementToHtmlCreateFormulaOutputReference
  (tempS, out, usePNG, useHtml, *this->container, &physicalPath, &htmlPathServer);
  tempStreamF << "\n$f=$ $" << this->theF.ToString(theFormat) << "$";
  tempS= tempStreamF.str();
  this->ElementToHtmlCreateFormulaOutputReference
  (tempS, out, usePNG, useHtml, *this->container, &physicalPath, &htmlPathServer);
  out << "\n\nThe below are the Lie brackets of the above elements. Printed for debugging.";
  if (useHtml)
    out << "\n<br>\n";
  tempStreamEF << "\n$[e, f]=$ $" <<  this->bufferEbracketF.ToString(theFormat) << "$";
  tempS= tempStreamEF.str();
  this->ElementToHtmlCreateFormulaOutputReference
  (tempS, out, usePNG, useHtml, *this->container, &physicalPath, &htmlPathServer);
  tempStreamHE << "\n$[h, e]=$ $" << this->bufferHbracketE.ToString(theFormat) << "$";
  tempS= tempStreamHE.str();
  this->ElementToHtmlCreateFormulaOutputReference
  (tempS, out, usePNG, useHtml, *this->container, &physicalPath, &htmlPathServer);
  tempStreamHF << "\n$[h, f]=$ $" << this->bufferHbracketF.ToString(theFormat) << "$";
  tempS= tempStreamHF.str();
  this->ElementToHtmlCreateFormulaOutputReference
  (tempS, out, usePNG, useHtml, *this->container, &physicalPath, &htmlPathServer);
  //this->theSystemMatrixForm.ToString(tempS);
  //out <<"\nSystem matrix form we try to solve:\n"<< tempS;
  //this->theSystemColumnVector.ToString(tempS);
  //out <<"\nColumn vector of the system:\n"<<tempS;
  std::stringstream tempStreamActual;
  for (int i=0; i<this->theSystemToBeSolved.size; i++)
  { tempS=this->theSystemToBeSolved[i].ToString(theFormat);
    if (tempS=="")
    { if (useLatex || usePNG)
        tempStreamActual << "~\\\\";
      else
        tempStreamActual << "\n\n";
    }
    else
      tempStreamActual << "\\noindent \\begin{eqnarray*}&& " << tempS << "=0\\end{eqnarray*}\n\n";
  }
  out << "\nThe system we need to solve:\n";
  if (useHtml)
    out << "\n<br>\n";
  tempS= tempStreamActual.str();
  this->ElementToHtmlCreateFormulaOutputReference
  (tempS, out, usePNG, useHtml, *this->container, &physicalPath, &htmlPathServer);
  return out.str();
}

void slTwoSubalgebra::ComputeDynkinsEpsilon(WeylGroup& theWeyl)
{//outdates, must be erased as soon as I implement an equivalent
  this->DynkinsEpsilon = this->DiagramM.NumRootsGeneratedByDiagram()+this->DiagramM.RankTotal();
  int r=0;
  for (int i=0; i<this->hCharacteristic.size; i++)
    if (!this->hCharacteristic[i].IsEqualToZero())
      r++;
  this->DynkinsEpsilon+= r;
  for (int i=0; i<theWeyl.RootSystem.size; i++)
  { int NumNonZeroFound=0;
    for (int j=0; j<this->hCharacteristic.size; j++)
    { if (theWeyl.RootSystem[i][j]==1)
        NumNonZeroFound++;
      if(NumNonZeroFound>1)
        break;
    }
    if (NumNonZeroFound==1)
      this->DynkinsEpsilon--;
  }
  this->DynkinsEpsilon=0;
  for (int i=0; i<this->hCharacteristic.size; i++)
    if (this->hCharacteristic[i]==1)
      this->DynkinsEpsilon=-1;
  this->DynkinsEpsilon=0;
}

bool slTwoSubalgebra::ModuleDecompositionFitsInto(const slTwoSubalgebra& other)const
{ return this->ModuleDecompositionFitsInto
  (this->highestWeights, this->multiplicitiesHighestWeights, other.highestWeights,
   other.multiplicitiesHighestWeights);
}

bool slTwoSubalgebra::ModuleDecompositionFitsInto
(const List<int>& highestWeightsLeft, const List<int>& multiplicitiesHighestWeightsLeft,
 const List<int>& highestWeightsRight, const List<int>& multiplicitiesHighestWeightsRight)
{ for (int i=0; i<highestWeightsLeft.size; i++)
  { int theIndex= highestWeightsRight.GetIndex(highestWeightsLeft[i]);
    if (theIndex==-1)
      return false;
    else
      if (multiplicitiesHighestWeightsLeft[i]>multiplicitiesHighestWeightsRight[theIndex])
        return false;
  }
  return true;
}

void slTwoSubalgebra::ElementToHtml(std::string& filePath)
{ std::fstream theFile;
  std::string theFileName=filePath;
  theFileName.append("theSlTwo.txt");
  CGI::OpenFileCreateIfNotPresent(theFile, filePath, false, true, false);
}

void SltwoSubalgebras::init(SemisimpleLieAlgebra& inputOwner)
{ this->owner=& inputOwner;
  this->theRootSAs.owneR=&inputOwner;
}

void SemisimpleLieAlgebra::FindSl2Subalgebras
(SemisimpleLieAlgebra& inputOwner, SltwoSubalgebras& output, GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("SemisimpleLieAlgebra::FindSl2Subalgebras");
  output.init(inputOwner);
  output.GetOwner().ComputeChevalleyConstantS(&theGlobalVariables);
  output.theRootSAs.GenerateAllReductiveRootSubalgebrasUpToIsomorphism
  (theGlobalVariables, true, true);
  //output.theRootSAs.ComputeDebugString(false, false, false, 0, 0, theGlobalVariables);
  output.IndicesSl2sContainedInRootSA.SetSize(output.theRootSAs.size);
  output.IndicesSl2sContainedInRootSA.ReservE(output.theRootSAs.size*2);
  for (int i=0; i<output.IndicesSl2sContainedInRootSA.size; i++)
    output.IndicesSl2sContainedInRootSA[i].size=0;
  ProgressReport theReport(&theGlobalVariables);
  for (int i=0; i<output.theRootSAs.size-1; i++)
  { std::stringstream tempStream;
    tempStream << "\nExploring root subalgebra "
    << output.theRootSAs[i].theDynkinDiagram.ToString() << "(" << (i+1)
    << " out of " << output.theRootSAs.size-1 << " non-trivial)\n";
    theReport.Report(tempStream.str());
    output.theRootSAs[i].GetSsl2SubalgebrasAppendListNoRepetition
    (output, i, theGlobalVariables);
  }
  for (int i=0; i<output.size; i++)
  { slTwoSubalgebra& theSl2= output.GetElement(i);
    theSl2.IndicesMinimalContainingRootSA.ReservE(theSl2.IndicesContainingRootSAs.size);
    theSl2.IndicesMinimalContainingRootSA.size=0;
    for (int j=0; j<theSl2.IndicesContainingRootSAs.size; j++)
    { bool isMinimalContaining=true;
//      rootSubalgebra& currentRootSA = output.theRootSAs.TheObjects[];
      for (int k=0; k<theSl2.IndicesContainingRootSAs.size; k++)
      { rootSubalgebra& theOtherRootSA =
        output.theRootSAs[theSl2.IndicesContainingRootSAs[k]];
        if (theOtherRootSA.indicesSubalgebrasContainingK.Contains(theSl2.IndicesContainingRootSAs[j]))
        { isMinimalContaining=false;
          break;
        }
      }
      if (isMinimalContaining)
        theSl2.IndicesMinimalContainingRootSA.AddOnTopNoRepetition(theSl2.IndicesContainingRootSAs[j]);
    }
    output.ComputeModuleDecompositionsOfAmbientLieAlgebra(theGlobalVariables);
  }
//  tempRootSA.GetSsl2Subalgebras(tempSl2s, theGlobalVariables, *this);
}

std::string rootSubalgebras::ToString()
{ std::stringstream out;
  for (int i=0; i<this->size; i++)
    out << (*this)[i].ToString();
  return out.str();
}

WeylGroup& rootSubalgebras::GetOwnerWeyl()
{ return this->GetOwnerSSalgebra().theWeyl;
}

SemisimpleLieAlgebra& rootSubalgebras::GetOwnerSSalgebra()
{ if (this->owneR==0)
  { std::cout << "This is a programming error. Attempting to access the "
    << " ambient Lie algebra of a non-initialized collection of root subalgebras. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return *this->owneR;
}

void rootSubalgebras::GenerateAllReductiveRootSubalgebrasUpToIsomorphism
(GlobalVariables& theGlobalVariables, bool sort, bool computeEpsCoords)
{ MacroRegisterFunctionWithName("rootSubalgebras::GenerateAllReductiveRootSubalgebrasUpToIsomorphism");
  this->size=0;
  this->GetOwnerWeyl().ComputeRho(true);
  //this->initDynkinDiagramsNonDecided(this->AmbientWeyl, WeylLetter, WeylRank);
  rootSubalgebras rootSAsGenerateAll;
  rootSAsGenerateAll.SetSize(this->GetOwnerSSalgebra().GetRank()*2+1);
  rootSAsGenerateAll[0].genK.size=0;
  rootSAsGenerateAll[0].owneR=this->owneR;
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

void rootSubalgebra::GetSsl2SubalgebrasAppendListNoRepetition
(SltwoSubalgebras& output, int indexInContainer, GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("rootSubalgebra::GetSsl2SubalgebrasAppendListNoRepetition");
  //reference: Dynkin, semisimple Lie algebras of simple lie algebras, theorems 10.1-10.4
  Selection theRootsWithZeroCharacteristic;
  Selection simpleRootsChar2;
  Vectors<Rational> RootsWithCharacteristic2;
  Vectors<Rational> reflectedSimpleBasisK;
  RootsWithCharacteristic2.ReservE(this->PositiveRootsK.size);
  ElementWeylGroup raisingElt;
  DynkinDiagramRootSubalgebra tempDiagram;
  int theRelativeDimension= this->SimpleBasisK.size;
  theRootsWithZeroCharacteristic.init(theRelativeDimension);
  Matrix<Rational> InvertedRelativeKillingForm;
  InvertedRelativeKillingForm.init(theRelativeDimension, theRelativeDimension);
  for (int k=0; k<theRelativeDimension; k++)
    for (int j=0; j<theRelativeDimension; j++)
      InvertedRelativeKillingForm.elements[k][j]=this->GetAmbientWeyl().RootScalarCartanRoot
      (this->SimpleBasisK[k], this->SimpleBasisK[j]);
  InvertedRelativeKillingForm.Invert();
  int numCycles= MathRoutines::TwoToTheNth(theRootsWithZeroCharacteristic.MaxSize);
  ProgressReport theReport(&theGlobalVariables);
  Vectors<Rational> tempRoots;
  tempRoots.ReservE(theRootsWithZeroCharacteristic.MaxSize);
  Vectors<Rational> relativeRootSystem, bufferVectors;
  Matrix<Rational> tempMat;
//  Selection tempSel;
  this->PositiveRootsK.GetCoordsInBasis
  (this->SimpleBasisK, relativeRootSystem, bufferVectors, tempMat);
  slTwoSubalgebra theSl2;
  theSl2.container=&output;
  theSl2.owneR=this->owneR;
  SemisimpleLieAlgebra& theLieAlgebra= this->GetOwnerSSalg();
  for (int i=0; i<numCycles; i++, theRootsWithZeroCharacteristic.incrementSelection())
  { tempRoots.size=0;
    for (int j=0; j<theRootsWithZeroCharacteristic.CardinalitySelection; j++)
      tempRoots.AddOnTop(this->SimpleBasisK[theRootsWithZeroCharacteristic.elements[j]]);
    tempDiagram.ComputeDiagramTypeModifyInput(tempRoots, this->GetAmbientWeyl());
    int theSlack=0;
    RootsWithCharacteristic2.size=0;
    simpleRootsChar2=theRootsWithZeroCharacteristic;
    simpleRootsChar2.InvertSelection();
    Vector<Rational> simpleRootsChar2Vect;
    simpleRootsChar2Vect=simpleRootsChar2;
    for (int j=0; j<relativeRootSystem.size; j++)
      if (simpleRootsChar2Vect.ScalarEuclidean(relativeRootSystem[j])==1)
      { theSlack++;
        RootsWithCharacteristic2.AddOnTop(this->PositiveRootsK[j]);
      }
    int theDynkinEpsilon =
    tempDiagram.NumRootsGeneratedByDiagram() + theRelativeDimension - theSlack;
    //if Dynkin's epsilon is not zero the subalgebra cannot be an S sl(2) subalgebra.
    //otherwise, as far as I understand, it always is //
    //except for G_2 (go figure!).
    //(but generatorootsWithZeroCharVectorrs still have to be found)
    //this is done in the below code.
//    std::cout << "Simple basis k: " << this->SimpleBasisK.ToString();
    if (theDynkinEpsilon==0)
    { Vector<Rational> tempRoot, tempRoot2;
      tempRoot.MakeZero(theRelativeDimension);
      for (int k=0; k<theRelativeDimension; k++)
        if(!theRootsWithZeroCharacteristic.selected[k])
          tempRoot[k]=2;
      InvertedRelativeKillingForm.ActOnVectorColumn(tempRoot, tempRoot2);
      Vector<Rational> characteristicH;
      characteristicH.MakeZero(theLieAlgebra.GetRank());
      for(int j=0; j<theRelativeDimension; j++)
        characteristicH+=this->SimpleBasisK[j]*tempRoot2[j];
      this->GetAmbientWeyl().RaiseToDominantWeight(characteristicH, 0, 0, &raisingElt);
      ////////////////////
      reflectedSimpleBasisK=this->SimpleBasisK;
      for (int k=0; k<reflectedSimpleBasisK.size; k++)
        this->GetAmbientWeyl().ActOn(raisingElt, reflectedSimpleBasisK[k]);
      ////////////////////
      theSl2.RootsWithScalar2WithH=RootsWithCharacteristic2;
      for (int k=0; k<theSl2.RootsWithScalar2WithH.size; k++)
        this->GetAmbientWeyl().ActOn(raisingElt, theSl2.RootsWithScalar2WithH[k]);

      theSl2.theH.MakeHgenerator(characteristicH, theLieAlgebra);
      theSl2.theE.MakeZero(theLieAlgebra);
      theSl2.theF.MakeZero(theLieAlgebra);
      //theSl2.ComputeDebugString(false, false, theGlobalVariables);
//      std::cout << "<br>accounting " << characteristicH.ToString();
      if(theLieAlgebra.AttemptExtendingHEtoHEFWRTSubalgebra
         (theSl2.RootsWithScalar2WithH, theRootsWithZeroCharacteristic,
          reflectedSimpleBasisK, characteristicH, theSl2.theE, theSl2.theF,
          theSl2.theSystemMatrixForm, theSl2.theSystemToBeSolved,
          theSl2.theSystemColumnVector, theGlobalVariables))
      { int indexIsoSl2;
        theSl2.MakeReportPrecomputations
        (theGlobalVariables, output, output.size, indexInContainer, *this);
        if(output.ContainsSl2WithGivenHCharacteristic(theSl2.hCharacteristic, &indexIsoSl2))
        { output.GetElement(indexIsoSl2).IndicesContainingRootSAs.AddOnTop(indexInContainer);
          output.IndicesSl2sContainedInRootSA[indexInContainer].AddOnTop(indexIsoSl2);
        } else
        { output.IndicesSl2sContainedInRootSA[indexInContainer].AddOnTop(output.size);
          theSl2.indexInContainer=output.size;
          output.AddOnTop(theSl2);
        }
      }
      else
      { output.BadHCharacteristics.AddOnTop(characteristicH);
/*        std::cout << "<br>obtained bad characteristic "
        << characteristicH.ToString() << ". The zero diagram is "
        << tempDiagram.ElementToStrinG()
        << "; the Dynkin epsilon is " << theDynkinEpsilon
        << "= the num roots generated by diagram " << tempDiagram.NumRootsGeneratedByDiagram()
        << " + the relative dimension " << theRelativeDimension
        << " - the slack " << theSlack
        << "<br>The relative root system is: "
        << relativeRootSystem.ToString();
        std::cout << "<br> I was exploring " << this->ToString();*/
      }
    }
    std::stringstream out;
    out << "Exploring Dynkin characteristics case " << i+1 << " out of " << numCycles;
//    std::cout << "<br>" << out.str();
    theReport.Report(out.str());
  }
//  std::cout << "Bad chracteristics: " << output.BadHCharacteristics.ToString();
}

bool SltwoSubalgebras::ContainsSl2WithGivenH(Vector<Rational>& theH, int* outputIndex)
{ if (outputIndex!=0)
    *outputIndex=-1;
  ElementSemisimpleLieAlgebra<Rational> tempH;
  this->CheckForCorrectInitializationCrashIfNot();
  tempH.MakeHgenerator(theH, *this->owner);
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].theH==tempH)
    { if (outputIndex!=0)
        *outputIndex=i;
      return true;
    }
  return false;
}

bool SltwoSubalgebras::ContainsSl2WithGivenHCharacteristic
(Vector<Rational>& theHCharacteristic, int* outputIndex)
{ if (outputIndex!=0)
    *outputIndex=-1;
  for (int i=0; i<this->size; i++)
    if ((*this)[i].hCharacteristic==theHCharacteristic)
    { if (outputIndex!=0)
        *outputIndex=i;
      return true;
    }
  return false;
}

void slTwoSubalgebra::ElementToStringModuleDecomposition(bool useLatex, bool useHtml, std::string& output)const
{ std::stringstream out;
  for (int i=0; i<this->highestWeights.size; i++)
  { if (this->multiplicitiesHighestWeights[i]>1)
      out << this->multiplicitiesHighestWeights[i];
    out << "V_{" << this->highestWeights[i] << "}";
    if (i!=this->highestWeights.size-1)
      out << "+";
  }
  output=out.str();
}

void slTwoSubalgebra::ComputeModuleDecompositionAmbientLieAlgebra(GlobalVariables& theGlobalVariables)
{ this->ComputeModuleDecomposition
  (this->GetOwnerWeyl().RootsOfBorel, this->GetOwnerWeyl().CartanSymmetric.NumRows,
   this->highestWeights, this->multiplicitiesHighestWeights, this->weightSpaceDimensions,
   theGlobalVariables);
}

void slTwoSubalgebra::ComputeModuleDecompositionOfMinimalContainingRegularSAs(SltwoSubalgebras& owner, int IndexInOwner, GlobalVariables& theGlobalVariables)
{ this->MultiplicitiesDecompositionMinimalContainingRootSA.SetSize
  (this->IndicesMinimalContainingRootSA.size);
  this->HighestWeightsDecompositionMinimalContainingRootSA.SetSize
  (this->IndicesMinimalContainingRootSA.size);
  List<int> buffer;
  for (int i=0; i<this->IndicesMinimalContainingRootSA.size; i++)
  { rootSubalgebra& theSA= owner.theRootSAs[this->IndicesMinimalContainingRootSA[i]];
    this->ComputeModuleDecomposition
    (theSA.PositiveRootsK, theSA.SimpleBasisK.size,
     this->HighestWeightsDecompositionMinimalContainingRootSA[i],
     this->MultiplicitiesDecompositionMinimalContainingRootSA[i], buffer, theGlobalVariables);
  }
}

void slTwoSubalgebra::MakeReportPrecomputations
(GlobalVariables& theGlobalVariables, SltwoSubalgebras& container, int indexInContainer,
 int indexMinimalContainingRegularSA, rootSubalgebra& MinimalContainingRegularSubalgebra)
{ MacroRegisterFunctionWithName("slTwoSubalgebra::MakeReportPrecomputations");
  int theDimension=this->GetOwnerSSAlgebra().GetRank();
  this->IndicesContainingRootSAs.size=0;
  Vectors<Rational> tempRoots;
  tempRoots=(MinimalContainingRegularSubalgebra.SimpleBasisK);
  this->GetOwnerSSAlgebra().theWeyl.TransformToSimpleBasisGeneratorsWRTh
  (tempRoots, this->theH.GetCartanPart());
  DynkinDiagramRootSubalgebra theDiagram;
  theDiagram.ComputeDiagramTypeKeepInput(tempRoots, this->GetOwnerSSAlgebra().theWeyl);
  this->IndicesContainingRootSAs.AddOnTop(indexMinimalContainingRegularSA);
  tempRoots.MakeEiBasis(theDimension);
  this->GetOwnerSSAlgebra().theWeyl.TransformToSimpleBasisGeneratorsWRTh
  (tempRoots, this->theH.GetCartanPart());
  DynkinDiagramRootSubalgebra tempDiagram;
  tempDiagram.ComputeDiagramTypeKeepInput(tempRoots, this->GetOwnerSSAlgebra().theWeyl);
  this->preferredAmbientSimpleBasis=tempRoots;
  this->hCharacteristic.SetSize(theDimension);
  for (int i=0; i<theDimension; i++)
    this->hCharacteristic[i]=
    this->GetOwnerSSAlgebra().theWeyl.RootScalarCartanRoot
    (this->theH.GetCartanPart(), this->preferredAmbientSimpleBasis[i]);
  this->LengthHsquared=
  this->GetOwnerSSAlgebra().theWeyl.RootScalarCartanRoot
  (this->theH.GetCartanPart(), this->theH.GetCartanPart());
  //this->hCharacteristic.ComputeDebugString();
//  if (this->theE.NonZeroElements.MaxSize==this->owner->theWeyl.RootSystem.size
//      && this->theF.NonZeroElements.MaxSize==this->owner->theWeyl.RootSystem.size
//      && this->theH.NonZeroElements.MaxSize==this->owner->theWeyl.RootSystem.size)
  this->GetOwnerSSAlgebra().LieBracket(this->theE, this->theF, this->bufferEbracketF);
//  std:: cout << "[" << this->theE.ToString() << ", " << this->theF.ToString() << "]="
//  << this->bufferEbracketF.ToString();
  this->GetOwnerSSAlgebra().LieBracket(this->theH, this->theE, this->bufferHbracketE);
  this->GetOwnerSSAlgebra().LieBracket(this->theH, this->theF, this->bufferHbracketF);

  //theSl2.hCharacteristic.ComputeDebugString();
//  this->ComputeModuleDecomposition();
}

//The below code is related to sl(2) subalgebras of simple Lie algebras
void slTwoSubalgebra::ComputeModuleDecomposition
(Vectors<Rational>& positiveRootsContainingRegularSA, int dimensionContainingRegularSA,
 List<int>& outputHighestWeights, List<int>& outputMultiplicitiesHighestWeights,
 List<int>& outputWeightSpaceDimensions, GlobalVariables& theGlobalVariables)
{ int IndexZeroWeight=positiveRootsContainingRegularSA.size*2;
  outputWeightSpaceDimensions.initFillInObject(4*positiveRootsContainingRegularSA.size+1, 0);
  outputWeightSpaceDimensions[IndexZeroWeight]=dimensionContainingRegularSA;
  List<int> BufferHighestWeights;
  Rational tempRat;
  Vectors<Rational> coordsInPreferredSimpleBasis, tempRoots2;
  Matrix<Rational> tempMat;
  positiveRootsContainingRegularSA.GetCoordsInBasis
  (this->preferredAmbientSimpleBasis, coordsInPreferredSimpleBasis, tempRoots2, tempMat);
  for (int k=0; k<positiveRootsContainingRegularSA.size; k++)
  { tempRat=this->hCharacteristic.ScalarEuclidean(coordsInPreferredSimpleBasis[k]);
    assert(tempRat.DenShort==1);
    if (tempRat>positiveRootsContainingRegularSA.size*2)
    { std::cout << "This is a programming error. The scalar product of the h-Characteristic "
      << this->hCharacteristic.ToString()
      << " with the simple root " << coordsInPreferredSimpleBasis[k].ToString()
      << " is larger than " << positiveRootsContainingRegularSA.size*2
      << ". The affected sl(2) subalgebra is " << this->ToString()
      << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
      break;
    }
    outputWeightSpaceDimensions[IndexZeroWeight+tempRat.NumShort]++;
    outputWeightSpaceDimensions[IndexZeroWeight-tempRat.NumShort]++;
  }
  BufferHighestWeights=(outputWeightSpaceDimensions);
  outputHighestWeights.ReservE(positiveRootsContainingRegularSA.size*2);
  outputMultiplicitiesHighestWeights.ReservE(positiveRootsContainingRegularSA.size*2);
  outputHighestWeights.size=0;
  outputMultiplicitiesHighestWeights.size=0;
//  this->hCharacteristic.ComputeDebugString();
//  std::cout << "Starting weights:  " << BufferHighestWeights;
  for (int j=BufferHighestWeights.size-1; j>=IndexZeroWeight; j--)
  { int topMult = BufferHighestWeights[j];
    if (topMult<0)
    { std::cout << "This is a programming error: "
      << " the sl(2)-module decomposition "
      << " shows an sl(2)-module with highest weight "
      << topMult << " which is impossible. Here is the sl(2) subalgebra. "
      << this->ToString() << "."
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    if (topMult>0)
    { outputHighestWeights.AddOnTop(j-IndexZeroWeight);
      outputMultiplicitiesHighestWeights.AddOnTop(topMult);
      if (j!=IndexZeroWeight)
        BufferHighestWeights[IndexZeroWeight*2-j]-=topMult;
      for (int k=j-2; k>=IndexZeroWeight; k-=2)
      { BufferHighestWeights[k]-=topMult;
        if (k!=IndexZeroWeight)
          BufferHighestWeights[IndexZeroWeight*2-k]-=topMult;
        if(BufferHighestWeights[k]<0 ||
           !(BufferHighestWeights[k]==BufferHighestWeights[IndexZeroWeight*2-k]))
        { std::cout << " This is a programming error: an error check has failed. "
          << " While trying to decompose with respect to  h-characteristic <br> "
          << this->hCharacteristic.ToString()
          << ". The positive root system of the containing root subalgebra is <br>"
          << positiveRootsContainingRegularSA.ToString() << ". "
          << "<br>The preferred simple basis is <br>"
          << this->preferredAmbientSimpleBasis.ToString()
          << ". The coordinates relative to the preferred simple basis are<br>"
          << coordsInPreferredSimpleBasis.ToString()
          << " The starting weights list is <br>"
          << outputWeightSpaceDimensions << ". "
          << " I got that the root space of index  "
          <<  k+1 << " has negative dimension. Something is wrong. "
          << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
          assert(false);
        }
      }
    }
  }
}

void SltwoSubalgebras::ElementToStringModuleDecompositionMinimalContainingRegularSAs(std::string& output, bool useLatex, bool useHtml)
{ std::string tempS;
  std::stringstream out;
  for (int i=0; i<this->size; i++)
  { (*this)[i].ElementToStringModuleDecompositionMinimalContainingRegularSAs(useLatex, useHtml, *this, tempS);
    out << tempS;
    if (useHtml)
      out << "\n<br>";
    out << "\n";
  }
  output=out.str();
}

std::string SltwoSubalgebras::ElementToStringNoGenerators(FormatExpressions* theFormat)
{ MacroRegisterFunctionWithName("SltwoSubalgebras::ElementToStringNoGenerators");
  std::string tempS; std::stringstream out;
  std::string tooltipHchar="Let h be in the Cartan s.a. Let \\alpha_1, ..., \\alpha_n be simple Vectors<Rational> w.r.t. h. Then the h-characteristic is the n-tuple (\\alpha_1(h), ..., \\alpha_n(h))";
  std::string tooltipVDecomposition= "The sl(2) submodules of g are parametrized by their highest weight w.r.t. h. V_l is l+1 dimensional";
  std::string tooltipContainingRegular="A regular semisimple subalgebra might contain an sl(2) such that the sl(2) has no centralizer in the regular semisimple subalgebra, but the regular semisimple subalgebra might fail to be minimal containing. This happens when another minimal containing regular semisimple subalgebra of equal rank nests as a Vector<Rational> subalgebra in the containing SA. See Dynkin, Semisimple Lie subalgebras of semisimple Lie algebras, remark before Theorem 10.4.";
  std::string tooltipHvalue="The actual realization of h. The coordinates of h are given with respect to the fixed original simple basis. Note that the characteristic of h is given *with respect to another basis* (namely, with respect to an h-positive simple basis). I will fix this in the future (email me if you want that done sooner).";
  //if (this->owner==0)

  bool usePNG=theFormat==0? false : theFormat->flagUsePNG;
  bool useHtml=theFormat==0 ? true : theFormat->flagUseHTML;
  bool useLatex=theFormat==0 ? true : theFormat->flagUseLatex;
  std::string physicalPath, displayPath;
  physicalPath=theFormat==0 ? "../" : theFormat->PathPhysicalOutputFolder;
  displayPath=theFormat==0 ? "../" : theFormat->PathDisplayOutputFolder;
  out << "Number of sl(2) subalgebras " << this->size << ".\n";
  if (this->IndicesSl2decompositionFlas.size < this->size)
    usePNG = false;
  std::stringstream out2;
  out2
  << "<br> Given a root subsystem P, and a root subsubsystem P_0, in (10.2) "
  << " of Semisimple subalgebras of semisimple Lie algebras, E. Dynkin defines "
  << " a numerical constant e(P, P_0) (which we call Dynkin epsilon). "
  << " <br>In Theorem 10.3, Dynkin proves that if an sl(2) is an S-subalgebra in "
  << " the root subalgebra generated by P, such that it has characteristic 2 "
  << " for all simple roots of P lying in P_0, then e(P, P_0)=0. ";
  if (this->BadHCharacteristics.size>0)
  { bool allbadAreGoodInTheirBadness=true;
    for (int i=0; i<this->BadHCharacteristics.size; i++)
    { bool isGoodInItsbaddness=false;
      for (int j=0; j<this->size; j++)
        if (this->BadHCharacteristics[i]==(*this)[j].theH.GetCartanPart())
        { isGoodInItsbaddness=true;
          break;
        }
      if (!isGoodInItsbaddness)
      { allbadAreGoodInTheirBadness=false;
        out << "<b>Bad characteristic: " << this->BadHCharacteristics[i]
        << "</b>";
      }
      else
      { out2
        << "<br><b>It turns out that in the current case of Cartan element h = "
        << this->BadHCharacteristics[i] << " we have that, for a certain P, "
        << " e(P, P_0) equals 0, but I failed to realize the corresponding sl(2) "
        << " as a subalgebra of that P. However, it turns out that h "
        << " is indeed an S-subalgebra of a smaller root subalgebra P'.</b>"
        ;
      }
    }
    if (!allbadAreGoodInTheirBadness)
      out << "<b>found bad characteristics!</b>";
  } else
  { out2 << "It turns out by direct computation that, in the current case of "
    << this->GetOwner().GetLieAlgebraName()
    << ",  e(P,P_0)=0 implies that an S-sl(2) subalgebra of the root subalgebra generated by "
    << " P with characteristic with 2's in the simple roots of P_0 "
    << " always exists. Note that Theorem 10.3 is stated in one "
    << " direction only.";
  }
  if(useHtml)
    out << "<br><br><table><tr><td style=\"padding-right:20px\">"
    << CGI::ElementToStringTooltip("Characteristic", tooltipHchar)
    << "</td><td align=\"center\" title=\"" << tooltipHvalue
    << "\"> h</td><td style=\"padding-left:20px\" title=\""
    << tooltipVDecomposition
    << "\"> Decomposition of ambient Lie algebra</td>"
    << "<td>The square of the length of the weight dual to h.</td>"
    << "<td>Dynkin index = (previous column) *(long root length ambient algebra)^2/4</td>"
    << "<td>Minimal containing regular semisimple SAs</td><td title=\""
    << tooltipContainingRegular << "\">Containing regular semisimple SAs in "
    << "which the sl(2) has no centralizer</td> </tr>";

  for (int i=0; i<this->size; i++)
  { const slTwoSubalgebra& theSl2= (*this)[i];
    if (useHtml)
      out << "<tr><td style=\"padding-right:20px\"><a href=\"./sl2s.html#sl2index"
      << i << "\"title=\"" << tooltipHchar << "\" >";
    out << theSl2.hCharacteristic.ToString();
    if (useHtml)
      out << "</a></td><td title=\"" << tooltipHvalue << "\">";
    out << theSl2.theH.GetCartanPart().ToString();
    if(!this->GetOwnerWeyl().IsDominantWeight(theSl2.theH.GetCartanPart()))
      out << "<b>This is wrong!!!!!!!!!!!!! The h is not dual to a dominant weight... </b>";
    if (useHtml)
      out << "</td><td style=\"padding-left:20px\" title=\"" << tooltipVDecomposition << "\">";
    if (useHtml && usePNG)
      out << "<img src=\"./fla"
      << this->IndicesSl2decompositionFlas[i]+1 << ".png\"></td><td>";
    else
    { theSl2.ElementToStringModuleDecomposition(useLatex, useHtml, tempS);
      out << tempS;
      if (useHtml)
        out << "</td><td>";
    }
    out << theSl2.LengthHsquared;
    if (useHtml)
      out << "</td><td>";
    out << theSl2.LengthHsquared * this->GetOwnerWeyl().GetLongestRootLengthSquared()/4;
    if (useHtml)
      out << "</td><td>";
    for (int j=0; j<theSl2.IndicesMinimalContainingRootSA.size; j++)
    { rootSubalgebra& currentSA= this->theRootSAs[theSl2.IndicesMinimalContainingRootSA[j]];
      out << "<a href=\""
      << displayPath
      << "rootHtml_rootSA" << theSl2.IndicesMinimalContainingRootSA[j]
      << ".html\">" << currentSA.theDynkinDiagram.ToString() << "</a>" << ";  ";
    }
    if (useHtml)
      out << "</td><td title=\"" << tooltipContainingRegular << "\">";
    for (int j=0; j<theSl2.IndicesContainingRootSAs.size; j++)
    { rootSubalgebra& currentSA= this->theRootSAs[theSl2.IndicesContainingRootSAs[j]];
      out << "<a href=\""
      <<  displayPath
      << "rootHtml_rootSA" << theSl2.IndicesContainingRootSAs[j] << ".html\">"
      << currentSA.theDynkinDiagram.ToString() << "</a>" << ";  ";
    }
    if (useHtml)
      out << "</td></tr>\n";
  }
  if (useHtml)
    out << "</table><HR width=\"100%\">";
  out << out2.str() << "<hr>";
  return out.str();
}

std::string SltwoSubalgebras::ToString(FormatExpressions* theFormat)
{ std::string tempS; std::stringstream out; std::stringstream body;
  bool useHtml= theFormat==0 ? true : theFormat->flagUseHTML;
  for (int i=0; i<this->size; i++)
  { tempS=(*this)[i].ToString(theFormat);
  //  body<< "Index "<< i<<": ";
    if(useHtml)
      body << "<br>";
    body << tempS;
    if(useHtml)
      body << "<HR width=\"100%\">";
  }
  if(useHtml)
    out << "<br>";
  out << this->ElementToStringNoGenerators(theFormat);
  out << body.str();
  return out.str();
}

void SltwoSubalgebras::ElementToHtml
(FormatExpressions* theFormat, GlobalVariables* theGlobalVariables)
{ std::string physicalPathSAs= theFormat==0 ? "": theFormat->PathPhysicalOutputFolder;
  std::string htmlPathServerSAs= theFormat==0 ? "": theFormat->PathDisplayOutputFolder;
  std::string physicalPathSl2s= theFormat==0 ? "": theFormat->PathPhysicalOutputFolder+"sl2s/";
  std::string htmlPathServerSl2s= theFormat==0 ? "": theFormat->PathDisplayOutputFolder+"sl2s/";

  ProgressReport theReport(theGlobalVariables);
  theReport.Report("Preparing html pages for sl(2) subalgebras. This might take a while.");
  std::string tempS;
  std::string DisplayNameCalculator= theFormat==0 ? "" : theFormat->PathDisplayNameCalculator;
  this->theRootSAs.ElementToHtml
  (tempS, physicalPathSAs, htmlPathServerSAs, this, DisplayNameCalculator, theGlobalVariables);
  bool usePNG=true;
  if (physicalPathSAs=="")
    usePNG=false;
  if(usePNG)
  { int numExpectedFiles= this->size*8;
    this->texFileNamesForPNG.ReservE(numExpectedFiles);
    this->texStringsEachFile.ReservE(numExpectedFiles);
    this->listSystemCommandsLatex.ReservE(numExpectedFiles);
    this->listSystemCommandsDVIPNG.ReservE(numExpectedFiles);
  }
  this->texFileNamesForPNG.size=0;
  this->texStringsEachFile.size=0;
  this->listSystemCommandsLatex.size=0;
  this->listSystemCommandsDVIPNG.size=0;
  std::stringstream out, outNotation, outNotationCommand;
  std::string fileName;
  std::fstream theFile, fileFlas;
  outNotationCommand << "printSemisimpleLieAlgebra{}("
  << this->GetOwnerWeyl().theDynkinType.ToString() << ")"
 ;
  outNotation
  << "Notation, structure constants and Weyl group info: "
  << CGI::GetCalculatorLink(DisplayNameCalculator, outNotationCommand.str())
  << "<br> <a href=\""<< DisplayNameCalculator
  << "\"> Calculator main page</a><br><a href=\"../rootHtml.html\">Root subsystem table</a><br>";
  std::string notation= outNotation.str();
  out << this->ToString(theFormat);
  if(usePNG)
  { fileName= physicalPathSl2s;
    fileName.append("sl2s.html");
    CGI::OpenFileCreateIfNotPresent(theFile, fileName, false, true, false);
    tempS= out.str();
    theFile << "<HMTL>"
    << "<title>sl(2)-subalgebras of "
    << this->theRootSAs[0].theDynkinDiagram.ToString() << "</title>";
    theFile << "<meta name=\"keywords\" content=\""
    <<  this->theRootSAs[0].theDynkinDiagram.ToString()
    << " sl(2)-triples, sl(2)-subalgebras, nilpotent orbits simple Lie algebras,"
    << " nilpotent orbits of "
    <<  this->theRootSAs[0].theDynkinDiagram.ToString()
    << ", sl(2)-triples of "
    << this->theRootSAs[0].theDynkinDiagram.ToString()
    << " \">";
    theFile << "<BODY>" << notation << "<a href=\"" << htmlPathServerSl2s
    << "sl2s_nopng.html\"> plain html for your copy+paste convenience</a><br>\n"
    << tempS << "</HTML></BODY>";
    theFile.close();
  }
  fileName= physicalPathSl2s;
  fileName.append("sl2s_nopng.html");
  bool tempB=theFormat->flagUsePNG;
  theFormat->flagUsePNG=false;
  tempS = this->ToString(theFormat);
  theFormat->flagUsePNG=tempB;
  CGI::OpenFileCreateIfNotPresent(theFile, fileName, false, true, false);
  theFile << "<HMTL><BODY>" << notation << "<a href=\""
  << htmlPathServerSl2s << "sl2s.html\"> "
  << ".png rich html for your viewing pleasure</a><br>\n" << tempS << "</HTML></BODY>";
  theFile.close();
  if (usePNG)
  { this->listSystemCommandsLatex.SetSize(this->texFileNamesForPNG.size);
    this->listSystemCommandsDVIPNG.SetSize(this->texFileNamesForPNG.size);
    for (int i=0; i<this->texFileNamesForPNG.size; i++)
    { CGI::OpenFileCreateIfNotPresent(fileFlas, this->texFileNamesForPNG[i], false, true, false);
      fileFlas << "\\documentclass{article}\\begin{document}\\pagestyle{empty}\n"
      << this->texStringsEachFile[i] << "\n\\end{document}";
      std::stringstream tempStreamLatex, tempStreamPNG;
      tempStreamLatex << "latex " << " -output-directory="
      << physicalPathSl2s << " " << this->texFileNamesForPNG[i];
      tempS= this->texFileNamesForPNG[i];
      tempS.resize(tempS.size()-4);
      tempStreamPNG << "dvipng " << tempS << ".dvi -o " << tempS << ".png -T tight";
      this->listSystemCommandsLatex[i]= tempStreamLatex.str();
      this->listSystemCommandsDVIPNG[i]=tempStreamPNG.str();
      fileFlas.close();
    }
  }
}

void rootSubalgebra::ToString
(std::string& output, SltwoSubalgebras* sl2s, int indexInOwner, bool useLatex,
 bool useHtml, bool includeKEpsCoords, GlobalVariables* theGlobalVariables)
{ std::stringstream out;
  std::string tempS;
  std::string latexFooter, latexHeader;
  if (this->SimpleBasisgEpsCoords.size!=this->SimpleBasisK.size || this->SimpleBasisKEpsCoords.size!= this->SimpleBasisK.size || this->kModulesgEpsCoords.size!= this->kModules.size || this->kModulesKepsCoords.size!= this->kModules.size)
    includeKEpsCoords=false;
  int LatexLineCounter=0;
  this->ElementToStringHeaderFooter(latexHeader, latexFooter, useLatex, useHtml, includeKEpsCoords);
  tempS=this->theDynkinDiagram.ToString();
  if (useLatex)
    out << "\\noindent$\\mathfrak{k}_{ss}:$ ";
  else
    out << "k_{ss}: ";
  out << tempS;
  if (sl2s!=0)
  { out << " &nbsp&nbsp&nbsp Contained in: ";
    for (int i=0; i<this->indicesSubalgebrasContainingK.size; i++)
    { if (useHtml)
        out << "<a href=\"./rootHtml_rootSA"
        << this->indicesSubalgebrasContainingK[i] << ".html\">";
      out << tempS;
      if (useHtml)
        out << "</a>, ";
    }
    if (useHtml)
      out << "<br> <a href=\"./rootHtml.html\">Back to root subsystem table </a> ";
  }
  tempS=this->SimpleBasisK.ToString();
  if (useHtml)
    out << "\n<br>\n";
  if (useLatex)
    out << "\n\\noindent";
  out << " Simple basis: " << tempS;
  tempS=this->SimpleBasisgEpsCoords.ElementToStringEpsilonForm(useLatex, useHtml, false);
  if (useHtml)
    out << "\n<br>\nSimple basis epsilon form: " << tempS;
  tempS=this->SimpleBasisKEpsCoords.ElementToStringEpsilonForm(useLatex, useHtml, false);
  if (useHtml)
    out << "\n<br>\nSimple basis epsilon form with respect to k: " << tempS;
  tempS= this->theCentralizerDiagram.ToString();
  if(!useLatex)
    CGI::clearDollarSigns(tempS, tempS);
  if (useLatex)
    out << "\n\n\\noindent ";
  if (useHtml)
    out << "<br>\n";
  if (useLatex)
    out << "$C(\\mathfrak{k_{ss}})_{ss}$: ";
  else
    out << "C(k_{ss})_{ss}: ";
  out << tempS;
  //int CartanPieceSize=
    //this->AmbientWeyl.CartanSymmetric.NumRows- this->SimpleBasisCentralizerRoots.size-
    //  this->SimpleBasisK.size;
  //if (CartanPieceSize!=0)
  //{  if (useLatex)
  //    out << "$\\oplus\\mathfrak{h}_" << CartanPieceSize<<"$";
  //  if (useHtml)
  //    out <<"+h_"<<CartanPieceSize;
  //}
  if (useHtml)
    out << "<br>\n simple basis centralizer: ";
  if (useLatex)
    out << "; simple basis centralizer: ";
  tempS=this->SimpleBasisCentralizerRoots.ToString();
  out << tempS;
  if (sl2s!=0)
  { if (useHtml)
      out << "\n<hr>\n";
    List<int> hCharacteristics_S_subalgebras;
    //this->ComputeIndicesSl2s(indexInOwner, *sl2s, hCharacteristics_S_subalgebras);
    hCharacteristics_S_subalgebras.size=0;
    out << "\nCharacteristics of sl(2) subalgebras that have no centralizer in k (total "
    << sl2s->IndicesSl2sContainedInRootSA[indexInOwner].size << "): ";
    for (int i=0; i<sl2s->IndicesSl2sContainedInRootSA[indexInOwner].size; i++)
    { int theSl2index=sl2s->IndicesSl2sContainedInRootSA[indexInOwner][i];
      const slTwoSubalgebra& theSl2 = (*sl2s)[theSl2index];
      if (useHtml)
        out << "<a href=\"./sl2s/sl2s.html#sl2index" << theSl2index << "\">";
      out << theSl2.hCharacteristic.ToString() << ", ";
      if (useHtml)
        out << "</a>";
      bool isS_subalgebra=true;
//      theSl2.hCharacteristic.ComputeDebugString();
      for (int j=0; j<theSl2.IndicesContainingRootSAs.size; j++)
      { int indexComparison= theSl2.IndicesContainingRootSAs[j];
        if (indexComparison!=indexInOwner && sl2s->theRootSAs[indexComparison].indicesSubalgebrasContainingK.Contains(indexInOwner))
        { isS_subalgebra=false;
          break;
        }
      }
      if (isS_subalgebra)
        hCharacteristics_S_subalgebras.AddOnTop(sl2s->IndicesSl2sContainedInRootSA[indexInOwner][i]);
    }
    if (useHtml)
      out << "\n<br>\n";
    out << "\nS-sl(2) subalgebras in k (total " << hCharacteristics_S_subalgebras.size << "): ";
    for (int i=0; i<hCharacteristics_S_subalgebras.size; i++)
      out << sl2s->TheObjects[hCharacteristics_S_subalgebras[i]].hCharacteristic.ToString() << ", ";
  }
  if (useHtml)
  { out << "<hr>\n Number of k-submodules of g/k: " << this->HighestWeightsGmodK.size;
    out << "<br>Module decomposition over k follows. "
    << "The decomposition is given in 1) epsilon coordinates w.r.t. g 2)"
    << " simple coordinates w.r.t. g <br> ";
    std::stringstream //tempStream1,
    tempStream2, tempStream3;
    for(int i=0; i<this->HighestWeightsGmodK.size; i++)
    { //tempStream1 << "\\underbrace{V_{";
      tempStream2 << "\\underbrace{V_{";
      tempStream3 << "\\underbrace{V_{";
      //tempStream1
      //<< this->AmbientWeyl.GetFundamentalCoordinatesFromSimple
      //(this->HighestWeightsGmodK[i]).ElementToStringLetterFormat("\\omega", true, false);
      tempStream2
      << this->kModulesgEpsCoords[i][0].ToStringLetterFormat("\\epsilon");
      tempStream3
      << this->HighestWeightsGmodK[i].ToStringLetterFormat("\\alpha");
      //tempStream1 << "}}_{dim= " << this->kModules[i].size << "} ";
      tempStream2 << "}}_{dim= " << this->kModules[i].size << "} ";
      tempStream3 << "}}_{dim= " << this->kModules[i].size << "} ";
      if (i!=this->HighestWeightsGmodK.size-1)
      { //tempStream1 << "\\oplus";
        tempStream2 << "\\oplus";
        tempStream3 << "\\oplus";
      }
    }
//    out << "\n<hr>\n" << CGI::GetHtmlMathSpanFromLatexFormula(tempStream1.str()) << "\n";
    out << "\n<hr>\n" << CGI::GetHtmlMathSpanFromLatexFormula(tempStream2.str()) << "\n";
    out << "\n<hr>\n" << CGI::GetHtmlMathSpanFromLatexFormula(tempStream3.str()) << "\n<hr>\n";
  }
  if (useLatex)
    out << "\n\n\\noindent Number $\\mathfrak{g}/\\mathfrak{k}$ $\\mathfrak{k}$-submodules: ";
  if (!useHtml)
    out << this->LowestWeightsGmodK.size ;
  if (useHtml)
    out << "<br>\n";
  if (useLatex)
    out << "\n\n";
  out << latexHeader;
  this->kModulesgEpsCoords.SetSize(this->kModules.size);
  for (int i=0; i<this->kModules.size; i++)
  { tempS=this->LowestWeightsGmodK[i].ToString();
    if (useHtml)
      out << "\n<tr><td>";
    if (useLatex)
      out << "\\hline ";
    out << i;
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & ";
    out << this->kModules[i].size;
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & ";
    out << tempS;
    tempS=this->HighestWeightsGmodK[i].ToString();
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & ";
    out  << tempS;
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & \n";
    out << this->kModules[i].ToString();
    if (useHtml)
      out << "</td><td>";
    if (i>=this->kModulesgEpsCoords.size)
      this->GetAmbientWeyl().GetEpsilonCoords(this->kModules[i], this->kModulesgEpsCoords[i]);
    out << this->kModulesgEpsCoords[i].ElementToStringEpsilonForm(useLatex, useHtml, true);
    if (useLatex)
      out << " & \n";
    if (useHtml)
      out << "</td>";
    if (includeKEpsCoords)
    { if (useHtml)
        out << "<td>";
      if (useLatex)
        out << " & ";
      out << tempS << this->kModulesKepsCoords[i].ElementToStringEpsilonForm(useLatex, useHtml, true);
      if (useHtml)
        out << "</td>";
      if (useLatex)
        out << "\\\\\n";
    }
    if (useHtml)
      out << "</tr>";
    if (LatexLineCounter>this->NumGmodKtableRowsAllowedLatex)
    { LatexLineCounter=0;
      out << latexFooter << latexHeader;
    }
    if (i!=this->kModules.size-1)
    { LatexLineCounter+=this->kModules[i].size;
      if (useLatex)
       if ((LatexLineCounter>this->NumGmodKtableRowsAllowedLatex) && (LatexLineCounter!=this->kModules.TheObjects[i].size))
        { out << latexFooter << latexHeader;
          LatexLineCounter = this->kModules[i].size;
        }
    }
  }
  if (useHtml)
    out << "</table>";
  if (useLatex)
    out << latexFooter;
  if ((useLatex|| useHtml)&& this->theMultTable.size==0 && this->kModules.size!=0)
    this->GenerateKmodMultTable(this->theMultTable, this->theOppositeKmods, theGlobalVariables);
  if (this->theMultTable.size!=0)
  { if (useHtml)
      out << "\n\n Pairing table:\n\n";
    if (useLatex)
      out << "\n\n\\noindent Pairing table:\n\n\\noindent";
    this->theMultTable.ToString(tempS, useLatex, useHtml, *this);
    out << tempS << "\n";
  }
  output=out.str();
}

std::string CandidateSSSubalgebra::ToStringModuleDecompo(FormatExpressions* theFormat)const
{ if (this->modulesGrouppedByWeight.size<=0)
    return "";
  std::stringstream out;
  out << "Isotypic module decomposition over primal subalgebra (total "
  << this->modulesGrouppedByWeight.size << " isotypic components). ";
  out << "<table border=\"1\"><tr><td>Module highest weight in fund. coords. and label</td>";
  for (int i=0; i<this->theCharOverCartanPlusCartanCentralizer.size; i++)
    out << "<td>" << "V_{" << i+1 << "}="
    << this->theCharOverCartanPlusCartanCentralizer[i].ToString() << "</td>";
  out << "</tr><tr><td>Module elements; all elements are weight vectors w.r.t. Cartan of subalgebra</td>";
  for (int i=0; i<this->modulesGrouppedByWeight.size; i++)
  { out << "<td><table border=\"1\"><tr>";
    for (int j=0; j<this->modulesGrouppedByWeight[i].size; j++)
    { List<ElementSemisimpleLieAlgebra<Rational> >& currentModule=this->modulesGrouppedByWeight[i][j];
      out << "<td>";
      for (int k=0; k<currentModule.size; k++)
      { out << currentModule[k].ToString();
        if (k!=currentModule.size-1)
          out << "<br>";
      }
      out << "</td>";
    }
//  out << "<td>|||</td><td>Union of the isotypic components:<br>";
//  for (int j=0; j<this->modulesGrouppedByPrimalType[i].size; j++)
//  { out << this->modulesGrouppedByPrimalType[i][j].ToString() << "<br>";
//  }
//  out << "</td>";
    out << "</tr></table></td>";
  }
  out << "</tr>";
  out << "<tr>";
  out << "<td>weights of elements in fundamental coords w.r.t. Cartan of subalgebra in same order as above</td>";
  for (int i=0; i<this->weightsOfModules.size; i++)
  { out << "<td>";
    for (int j=0; j<this->weightsOfModules[i].size; j++)
    { out << this->weightsOfModules[i][j].ToStringLetterFormat("\\omega");
      if (j!=this->weightsOfModules[i].size-1)
        out << "<br>";
    }
    out << "</td>";
  }
  out << "</tr>";
  out << "<tr>";
  out << "<td>weights of elements in (fundamental coords w.r.t. Cartan of subalgebra)+ Cartan centralizer</td>";
  for (int i=0; i<this->weightsOfPrimallySplitModules.size; i++)
  { out << "<td>";
    for (int j=0; j<this->weightsOfPrimallySplitModules[i].size; j++)
    { out << this->weightsOfPrimallySplitModules[i][j].ToStringLetterFormat("\\omega");
      if (j!=this->weightsOfPrimallySplitModules[i].size-1)
        out << "<br>";
    }
    out << "</td>";
  }
  out << "</tr>";
  out << "</table>";
  return out.str();
}

std::string CandidateSSSubalgebra::ToStringPairingTable(FormatExpressions* theFormat)const
{ if (!this->NilradicalPairingTable.size>0)
    return "";
  std::stringstream out;
  out << "<br>Below is the pairing table of the modules, defined as follows."
  << " Let g' be the subalgebra obtained by extending the current semisimple subalgebra "
  << " with the Cartan of the centralizer (following Penkov, Serganova, Zuckermann,  we call "
  << " a subalgebra g' arising in this fashion ``primal''). "
  << " Let A, B, C be "
  << " isotypic components in the decomposition of the ambient Lie algebra over g'."
  << " We say that A and B pair to C if there exist elements a in A and b in B such that  0\\neq [a,b]\\in C. "
  << " Note that, in general, A and B may pair to multiple modules C', C'', etc. We "
  << " note that if A and B pair to C then clearly C is isomorphic to some component in "
  << " the decomposition of A\\otimes B over g'. <br> We recall that V_{1}, V_{2}, ... are abbreviated notation "
  << " for the primal subalgebra modules indicated in the table above. ";
  out << "<br>Modules that have a zero weight (" << this->modulesWithZeroWeights.size << " total): ";
  for (int i=0; i<this->modulesWithZeroWeights.size; i++)
  { out << "V_{" << this->modulesWithZeroWeights[i]+1 << "}";
    if (i!=this->modulesWithZeroWeights.size-1)
      out << ", ";
  }
  out << "<br>Non-compatible (a.k.a. ``opposite'') modules:<br>"
  << " <table border=\"1\"><tr>";
  for (int i=0; i< this->OppositeModules.size; i++)
    out << "<td>V_{" << i+1 << "}" << "</td>";
  out << "</tr><tr>";
  for (int i=0; i< this->OppositeModules.size; i++)
  { out << "<td>";
    for (int j=0; j<this->OppositeModules[i].size; j++)
    { out << "V_{" << this->OppositeModules[i][j]+1 << "}";
      if (j!=this->OppositeModules[i].size-1)
      out << ", ";
    }
    out << "</td>";
  }
  out << "</tr></table>";
  out << "<br>Modules corresponding to the semisimple subalgebra: ";
  Vector<Rational> theSAvector, tempV;
  theSAvector.MakeZero(this->NilradicalPairingTable.size);
  for (int i=0; i<this->candidateSubalgebraModules.size; i++)
  { tempV.MakeEi(this->NilradicalPairingTable.size, this->candidateSubalgebraModules[i]);
    theSAvector+=tempV;
  }
  out << theSAvector.ToStringLetterFormat("V");
  out << "<br><table><tr><td>Modules</td>";
  for (int i=0; i<this->NilradicalPairingTable.size; i++)
  out << "<td><b>" << "V_{" << i+1 << "}="
  << this->theCharOverCartanPlusCartanCentralizer[i].ToString() << "</b></td>";
  out << "</tr>";
  for (int i=0; i<this->NilradicalPairingTable.size; i++)
  { out << "<tr><td> <b>" << "V_{" << i+1 << "}</b></td>";
    for (int j=0; j<this->NilradicalPairingTable.size; j++)
    { out << "<td>";
      for (int k=0; k<this->NilradicalPairingTable[i][j].size; k++)
      { out << "V_{" << this->NilradicalPairingTable[j][i][k]+1 << "}";
        if (k!=this->NilradicalPairingTable[i][j].size-1)
          out << ", ";
      }
      out << "</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  out << "<br>Number of nilradical candidates: " << this->FKNilradicalCandidates.size;
  if (this->FKNilradicalCandidates.size>0)
  { for (int i=0; i<this->FKNilradicalCandidates.size; i++)
    { out << "<br>Subalgebra " << i+1 << ": ";
      Vector<Rational> currentNilrad;
      currentNilrad=this->FKNilradicalCandidates[i];
      out << currentNilrad.ToStringLetterFormat("V");
      if (this->NilradicalConesIntersect[i])
        out << ". Cones intersect. ";
      else
        out << ". Cones don't intersect. ";
      out << "Nilradical cone: " << this->theNilradicalWeights[i].ToString()
      << "; highest weight cone: " << this->theNonFKhws[i].ToString();
      if (this->NilradicalConesIntersect[i])
        out << this->ConeIntersections[i].ToStringLetterFormat("w");
      else
        out << this->ConeSeparatingNormals[i].ToStringLetterFormat("u");
    }
    if (this->nilradicalGenerationLog!="")
      out << "<br>Nilradical generation log:" << this->nilradicalGenerationLog;
  }
  return out.str();
}

std::string SemisimpleSubalgebras::GetAlgebraLink(int ActualIndexSubalgebra, FormatExpressions* theFormat)const
{ if (ActualIndexSubalgebra<0)
    return "(non-initialized)";
  std::stringstream out;
  bool makeLink= theFormat==0? false : theFormat->flagUseHtmlAndStoreToHD;
  if (this->Hcandidates[ActualIndexSubalgebra].flagSystemProvedToHaveNoSolution)
    makeLink=false;
  if (makeLink)
    out << "<a href=\"" << this->GetDisplayFileName(ActualIndexSubalgebra, theFormat) << "\">"
    << CGI::GetHtmlMathSpanPure
    (this->Hcandidates[ActualIndexSubalgebra].theWeylNonEmbeddeD.theDynkinType.ToString()) << "</a>";
  else
    out << this->Hcandidates[ActualIndexSubalgebra].theWeylNonEmbeddeD.theDynkinType.ToString();
  return out.str();
}

std::string CandidateSSSubalgebra::ToString(FormatExpressions* theFormat)const
{ MacroRegisterFunctionWithName("CandidateSSSubalgebra::ToString");
  std::stringstream out;
  bool useLaTeX=theFormat==0? true : theFormat->flagUseLatex;
  bool useHtml=theFormat==0? true : theFormat->flagUseHTML;
  //bool writingToHD=theFormat==0? false : theFormat->flagUseHtmlAndStoreToHD;
  out << "Subalgebra type: " << this->owner->GetAlgebraLink(this->indexInOwner, theFormat)
  << " (click on type for detailed printout).";
  bool shortReportOnly=theFormat==0 ? true : theFormat->flagCandidateSubalgebraShortReportOnly;
  if (this->indicesDirectSummandSuperAlgebra.size>0)
  { /*out << "<br>Contained as an immediate (no in-betweens) direct summand of: ";
    DynkinType tempType;
    bool foundNonDirectContainers=false;
    for (int i=0; i<this->indicesDirectSummandSuperAlgebra.size; i++)
    { tempType=this->owner->Hcandidates[this->indicesDirectSummandSuperAlgebra[i]].theWeylNonEmbeddeD.theDynkinType;
      tempType-=this->theWeylNonEmbeddeD.theDynkinType;
      if (tempType.IsSimple())
      { out << this->owner->GetAlgebraLink(this->indicesDirectSummandSuperAlgebra[i], theFormat);
        if (i!=this->indicesDirectSummandSuperAlgebra.size-1)
          out << ", ";
      } else
        foundNonDirectContainers=true;
    }
    out << ". ";*/
    //if (foundNonDirectContainers)
    out << "<br>Contained as a direct summand of: ";
    for (int i=0; i<this->indicesDirectSummandSuperAlgebra.size; i++)
    { out << this->owner->GetAlgebraLink(this->indicesDirectSummandSuperAlgebra[i], theFormat);
      if (i!=this->indicesDirectSummandSuperAlgebra.size-1)
        out << ", ";
    }
    out << ". ";
  }
  if (!shortReportOnly)
  { List<DynkinSimpleType> theTypes;
    this->theWeylNonEmbeddeD.theDynkinType.GetTypesWithMults(theTypes);
    out << "<br>Elements Cartan by components: ";
    for (int i=0; i<this->CartanSAsByComponent.size; i++)
    { if (useLaTeX && useHtml)
        out << CGI::GetHtmlMathSpanPure(theTypes[i].ToString(), 1000) << ": ";
      else
        out << theTypes[i] << ":";
      for (int j=0; j<this->CartanSAsByComponent[i].size; j++)
      { out << this->CartanSAsByComponent[i][j].ToString() << ": "
        << this->GetAmbientWeyl().RootScalarCartanRoot
        (this->CartanSAsByComponent[i][j],this->CartanSAsByComponent[i][j]);
        if (j!=this->CartanSAsByComponent[i].size-1 || i!=this->CartanSAsByComponent.size-1)
          out << ", ";
      }
    }
  }
  MonomialChar<Rational> theZeroWeight;
  theZeroWeight.weightFundamentalCoords.MakeZero(this->theHs.size);
  Rational centralizerCartanSize=this->theCharFundCoords.GetMonomialCoefficient(theZeroWeight);
  bool CentralizerIsWellChosen= (centralizerCartanSize==0 )? true: false;
  if (centralizerCartanSize>0 && !this->flagSystemProvedToHaveNoSolution)
  { Rational centralizerRank=centralizerCartanSize;
    if (this->indexMaxSSContainer!=-1)
    { DynkinType centralizerType =
      this->owner->Hcandidates[this->indexMaxSSContainer].theWeylNonEmbeddeD.theDynkinType;
      centralizerType-=this->theWeylNonEmbeddeD.theDynkinType;
      out << "<br>Centralizer type: ";
      if (useLaTeX && useHtml)
        out << CGI::GetHtmlMathSpanPure(centralizerType.ToString());
      else
        out << CGI::GetHtmlMathSpanPure(centralizerType.ToString());
      out << ". ";
      centralizerRank-=centralizerType.GetRootSystemSize();
    }
    CentralizerIsWellChosen=(centralizerRank==this->CartanOfCentralizer.size );
    if (!CentralizerIsWellChosen)
    { out << "<br><b>My weight spaces were not chosen well so I did not get a good basis for the "
      << "Cartan of the centralizer, instead I got: </b> ";
    } else
      out << "<br>Basis of Cartan of centralizer: ";
    out << this->CartanOfCentralizer.ToString();
  }
//  out << "<br>Predefined or computed simple generators follow. ";
/*  out << "<br>Negative generators: ";
  for (int i=0; i<this->theNegGens.size; i++)
  { out << this->theNegGens[i].ToString(theFormat);
    if (i!=this->theNegGens.size-1)
      out << ", ";
  }
  out << "<br>Positive generators: ";
  for (int i=0; i<this->thePosGens.size; i++)
  { out << this->thePosGens[i].ToString(theFormat);
    if (i!=this->thePosGens.size-1)
      out << ", ";
  }*/
  if (this->flagSystemProvedToHaveNoSolution)
  { out << " <b> Subalgebra candidate proved to be impossible! </b> ";
    return out.str();
  }
  if (this->theBasis.size!=0)
  { out << "<br>Dimension of subalgebra generated by predefined or computed generators: "
    << this->theBasis.size << ". ";
    if (!shortReportOnly)
    { out << "<br>Negative simple generators: ";
      for (int i=0; i<this->theNegGens.size; i++)
      { if (useHtml && useLaTeX)
          out << CGI::GetHtmlMathSpanPure(this->theNegGens[i].ToString(theFormat), 2000);
        else
          out << this->theNegGens[i].ToString(theFormat);
        if (i!=this->theNegGens.size-1)
          out << ", ";
      }
      out << "<br>Positive simple generators: ";
      for (int i=0; i<this->thePosGens.size; i++)
      { if (useHtml && useLaTeX)
          out <<  CGI::GetHtmlMathSpanPure(this->thePosGens[i].ToString(theFormat), 2000);
        else
          out << this->thePosGens[i].ToString(theFormat);
        if (i!=this->thePosGens.size-1)
          out << ", ";
      }
    }
  }
  out << "<br>Symmetric Cartan matrix scaled: ";
  FormatExpressions tempFormat;
  tempFormat.flagUseLatex=true;
  tempFormat.flagUseHTML=false;
  if (useLaTeX && useHtml)
    out << CGI::GetHtmlMathSpanPure(this->theWeylNonEmbeddeD.CartanSymmetric.ToString(&tempFormat));
  else
    out << this->theWeylNonEmbeddeD.CartanSymmetric.ToString(theFormat);
  out << "<br>Decomposition of ambient Lie algebra: ";
  if (useLaTeX)
    out << CGI::GetHtmlMathSpanPure(this->theCharFundCoords.ToString());
  else
    out << this->theCharFundCoords.ToString();
  if (this->CartanOfCentralizer.size>0)
    out << "<br>Primal decomposition of the ambient Lie algebra "
    << "(refining above decomposition; order from above decomposition not preserved): "
    << (useLaTeX ? CGI::GetHtmlMathSpanPure(this->theCharOverCartanPlusCartanCentralizer.ToString(), 2000)
    :this->theCharOverCartanPlusCartanCentralizer.ToString());
  if (this->theBasis.size!=this->theWeylNonEmbeddeD.theDynkinType.GetRootSystemPlusRank())
  { if (!this->flagSystemSolved)
      out << " <b> Subalgebra not realized, but it might have a solution. </b> ";
    out << "<br>" << this->theUnknownNegGens.size << "*2 (unknown) gens:<br>(";
    for (int i=0; i<this->theUnknownNegGens.size; i++)
    { out << "<br>" << this->theUnknownNegGens[i].ToString(theFormat) << ", " ;
      out << this->theUnknownPosGens[i].ToString(theFormat);
      if (i!=this->theUnknownNegGens.size-1)
        out << ", ";
    }
    out << ")<br>";
    for (int i=0; i<this->theHs.size; i++)
    { out << "h: " << this->theHs[i] << ", "
      << " e = combination of " << this->theInvolvedPosGenerators[i].ToString()
      << ", f= combination of " << this->theInvolvedNegGenerators[i].ToString();
    }
    out << "Positive weight subsystem: " << this->theWeylNonEmbeddeD.RootsOfBorel.ToString();
    if (this->PosRootsPerpendicularPrecedingWeights.size>0)
      out << " Positive roots that commute with the weight subsystem: "
      << this->PosRootsPerpendicularPrecedingWeights.ToString();
    out << "<br>Symmetric Cartan default scale: "
    << this->theWeylNonEmbeddeDdefaultScale.CartanSymmetric.ToString(theFormat);
    out << "Character ambient Lie algebra: "
    << this->theCharFundamentalCoordsRelativeToCartan.ToString();
    out << "<br>A necessary system to realize the candidate subalgebra.  ";
    FormatExpressions tempFormat=this->theCoeffLetters;
    for (int i=0; i<this->theSystemToSolve.size; i++)
      out << "<br>" << this->theSystemToSolve[i].ToString(&tempFormat) << "= 0";
    out << "<br>The above system after transformation.  ";
    for (int i=0; i<this->transformedSystem.size; i++)
      out << "<br>" << this->transformedSystem[i].ToString(&tempFormat) << "= 0";
    if (!this->flagSystemGroebnerBasisFound)
      out << "<br><b>Failed to find Groebner basis of the above system (the computation is too large).</b>";
    if (this->owner!=0 && this->theHorbitIndices.size>0)
      if (this->theHorbitIndices[0].size>0)
        if (this->theUnknownPosGens.size>0)
        { const slTwoSubalgebra& theFirstSl2=this->owner->theSl2s[this->theHorbitIndices[0][0]];
          out << "<br>First positive generator seed realization.<br> "
          << this->theUnknownPosGens[0].ToString(theFormat) << " -> " << theFirstSl2.theE.ToString(theFormat);
          out << "<br>First negative generator seed realization.<br> "
          << this->theUnknownNegGens[0].ToString(theFormat) << " -> " << theFirstSl2.theF.ToString(theFormat);
        }
    out << "<br><b>For the calculator part 1: </b><br>FindOneSolutionSerreLikePolynomialSystem{}( ";
    for (int i=0; i<this->theSystemToSolve.size; i++)
    { out << this->theSystemToSolve[i].ToString(&tempFormat);
      if (i!=this->theSystemToSolve.size-1)
        out << ", ";
    }
    out << " )";
    out << "<br><b>For the calculator part 2: </b><br>GroebnerLexUpperLimit{}(10000, ";
    for (int i=0; i<this->theSystemToSolve.size; i++)
    { out << this->theSystemToSolve[i].ToString(&tempFormat);
      if (i!=this->theSystemToSolve.size-1)
        out << ", ";
    }
    out << " )";
//    if (this->flagSystemSolved)
//      out << "<br>Solution of above system: " << this->aSolution.ToString();
  }
  if (CentralizerIsWellChosen)
  { int numZeroWeights=0;
    out << "<br>The number of zero weights w.r.t. the Cartan subalgebra minus "
    << " the dimension of the centralizer of the subalgebra "
    << "equals " ;
    for (int i=0; i<this->weightsOfModules.size; i++)
      for (int j=0; j<this->weightsOfModules[i].size; j++)
        if(this->weightsOfModules[i][j].IsEqualToZero())
          numZeroWeights+=this->modulesGrouppedByWeight[i].size;
    out << numZeroWeights << " - " << centralizerCartanSize << "="
    << ((centralizerCartanSize-numZeroWeights)*(-1)).ToString() << ".";
  }
  if (this->flagSystemSolved && !shortReportOnly)
  { out << "<br>In the table below we indicate the highest weight "
    << "vectors of the decomposition of "
    << " the ambient Lie algebra as a module over the semisimple part. The second row indicates "
    << " weights of the highest weight vectors relative to the Cartan of the semisimple subalgebra. ";
    if (CentralizerIsWellChosen && this->CartanOfCentralizer.size>0)
    { out << "As the centralizer is well-chosen and the centralizer of our subalgebra is non-trivial, "
      << " we may in addition split highest weight vectors with the same weight over the semisimple part "
      << " over the centralizer (recall that the centralizer preserves the weights over the subalgebra "
      << " and in particular acts on the highest weight vectors). Therefore we have chosen our "
      << " highest weight vectors to be, in addition, weight vectors over the Cartan"
      << " of the centralizer of the "
      << " starting subalgebra. Their weight over the sum of the Cartans of the semisimple subalgebra and "
      << " its centralizer is indicated in the third row. The weights corresponding to the "
      << " Cartan of the centralizer are again indicated with the letter \\omega. "
      << " As there is no preferred way of chosing a basis of the Cartan of the centralizer "
      << " (unlike the starting semisimple Lie algebra: there we have a preferred basis induced by the"
      << " fundamental weights), our centralizer weights are simply given by the constant by which the "
      << " k^th basis element of the Cartan of the centralizer "
      << " acts on the highest weight vector. "
      << " Here, we use the choice for basis of the Cartan of the centralizer given at the start of the page. ";
    }
    out << "<table border=\"1px solid black\"><tr><td>Highest vectors of representations (total "
    << this->highestVectorsModules.size << ") ";
    if (CentralizerIsWellChosen)
      out << "; the vectors are over the primal subalgebra.";
    out << "</td>";
    for (int i=0; i<this->highestVectorsModules.size; i++)
      out << "<td>" << this->highestVectorsModules[i].ToString() << "</td>";
    out << "</tr><tr><td>weight</td>";
    for (int i=0; i<this->highestWeightsModules.size; i++)
      out << "<td>" << this->highestWeightsModules[i].ToStringLetterFormat("\\omega") << "</td>";
    out << "</tr>";
    if (CentralizerIsWellChosen && this->CartanOfCentralizer.size>0)
    { out << "<tr><td>weights rel. to Cartan of (centralizer+semisimple s.a.). "
      << "</td>";
      for (int i=0; i<this->highestWeightsCartanCentralizerSplitModules.size; i++)
        out << "<td>"
        << this->highestWeightsCartanCentralizerSplitModules[i].ToStringLetterFormat("\\omega")
        << "</td>";
      out << "</tr>";
    }
    out << "</table>";
  }
  if (!shortReportOnly)
  { out << this->ToStringModuleDecompo(theFormat);
    out << this->ToStringPairingTable(theFormat);
  }
  return out.str();
}

void CandidateSSSubalgebra::GetHsByType
(List<List<Vectors<Rational> > >& outputHsByType, List<DynkinSimpleType>& outputTypeList)
{ MacroRegisterFunctionWithName("CandidateSSSubalgebra::GetHsByType");
  List<DynkinSimpleType> allTypes;
  this->theWeylNonEmbeddeD.theDynkinType.GetTypesWithMults(allTypes);
  outputHsByType.SetSize(0);
  outputTypeList.SetSize(0);
  if (allTypes.size!=this->CartanSAsByComponent.size)
  { std::cout
    << "This is a programming error: allTypes.size must equal this->CartanSAsByComponent.size. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  for (int i=0; i<allTypes.size; i++)
  { bool shouldOpenNewType=true;
    if (i!=0)
      shouldOpenNewType=!(allTypes[i]==allTypes[i-1]);
    if (shouldOpenNewType)
    { outputHsByType.SetSize(outputHsByType.size+1);
      outputHsByType.LastObject()->SetSize(0);
      outputTypeList.AddOnTop(allTypes[i]);
    }
    outputHsByType.LastObject()->AddOnTop(this->CartanSAsByComponent[i]);
  }
}

template <class CoefficientType>
void WeylGroup::RaiseToMaximallyDominant
  (List<Vector<CoefficientType> >& theWeights)const
{ for (int i=0; i<theWeights.size; i++)
  { bool found=true;
    while (found)
    { found=false;
      for (int j=0; j<this->RootsOfBorel.size; j++)
        if (this->RootScalarCartanRoot(this->RootsOfBorel[j], theWeights[i])<0)
        { bool isGood=true;
          for (int k=0; k<i; k++)
            if (this->RootScalarCartanRoot(this->RootsOfBorel[j], theWeights[k])<0)
            { isGood=false;
              break;
            }
          if (isGood)
          { this->ReflectBetaWRTAlpha(this->RootsOfBorel[j], theWeights[i], false, theWeights[i]);
            found=true;
          }
        }
    }
  }
}

bool CandidateSSSubalgebra::HasConjugateHsTo(List<Vector<Rational> >& input)
{ //std::cout << "<br>Checking whether " << this->theHs.ToString()
  //<< " are conjugated to " << input.ToString();
  if (input.size!=this->theHs.size)
    return false;
  List<Vector<Rational> > raisedInput=input;
  List<Vector<Rational> > myVectors=this->theHs;
  WeylGroup& ambientWeyl=this->GetAmbientWeyl();
  ambientWeyl.RaiseToMaximallyDominant(raisedInput);
  ambientWeyl.RaiseToMaximallyDominant(myVectors);
  //std::cout << "<br>raised input is: " << raisedInput.ToString() << ", my raised h's are: "
  //<< myVectors.ToString();
  return myVectors==raisedInput;
}

bool CandidateSSSubalgebra::IsDirectSummandOf(CandidateSSSubalgebra& other, bool computeImmediateDirectSummandOnly)
{ if (other.flagSystemProvedToHaveNoSolution)
    return false;
 // std::cout << " <br>Testing whether "
  //<< this->theWeylNonEmbeddeD.theDynkinType.ToString() << " is a direct summand of "
  //<< other.theWeylNonEmbeddeD.theDynkinType.ToString() << "...";
  DynkinType theDifference;
  theDifference= other.theWeylNonEmbeddeD.theDynkinType;
  theDifference-=this->theWeylNonEmbeddeD.theDynkinType;
  for (int i=0; i<theDifference.size; i++)
    if (theDifference.theCoeffs[i]<0)
    { //std::cout << " it's not because types don't match.";
      return false;
    }
  Incrementable<SelectionFixedRank> selectedTypes;
  Incrementable<SelectionWithMaxMultiplicity> selectedOuterAutos;
  List<DynkinSimpleType> isoTypes;
  SelectionFixedRank currentTypeSelection;
  SelectionWithMaxMultiplicity outerIsoSelector;
  List<List<Vectors<Rational> > > theHsByType;
  this->GetHsByType(theHsByType, isoTypes);
  for (int i=0; i<isoTypes.size; i++)
  { Rational ratMult=this->theWeylNonEmbeddeD.theDynkinType.GetMonomialCoefficient(isoTypes[i]);
    int intMult;
    if (!ratMult.IsSmallInteger(&intMult))
      return false;
    currentTypeSelection.SetNumItemsAndDesiredSubsetSize(intMult, theHsByType[i].size);
    if (isoTypes[i].theLetter=='A' && isoTypes[i].theRank>1)
      outerIsoSelector.initMaxMultiplicity(intMult, 1);
    else if (isoTypes[i].theLetter=='D' && isoTypes[i].theRank==4)
      outerIsoSelector.initMaxMultiplicity(intMult, 5);
    else if (isoTypes[i].theLetter=='D' && isoTypes[i].theRank>4)
      outerIsoSelector.initMaxMultiplicity(intMult, 1);
    else if (isoTypes[i].theLetter=='D' && isoTypes[i].theRank==6)
      outerIsoSelector.initMaxMultiplicity(intMult, 1);
    else
      outerIsoSelector.initMaxMultiplicity(intMult, 0);
    selectedTypes.theElements.AddOnTop(currentTypeSelection);
    selectedOuterAutos.theElements.AddOnTop(outerIsoSelector);
  }
  Rational numCyclesFromTypes=selectedTypes.GetNumTotalCombinations();
  Rational numCyclesFromOuterIsos=selectedOuterAutos.GetNumTotalCombinations();
  int intNumCyclesFromTypes, intNumCyclesFromOuterIsos;
  if (!numCyclesFromTypes.IsSmallInteger(&intNumCyclesFromTypes) ||
      !numCyclesFromOuterIsos.IsSmallInteger(&intNumCyclesFromOuterIsos))
  { std::cout
    << "Computation is too large: I am crashing to let you know that the program cannot handle "
    << " such a large number of outer automorphisms"
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  List<Vector<Rational> > conjugationCandidates;
  Vectors<Rational> currentComponent;
  Matrix<Rational> currentOuterAuto;
  //std::cout << "Num combinations: " << selectedTypes.GetNumTotalCombinations().ToString()
  //<< " type selections  times " << selectedOuterAutos.GetNumTotalCombinations().ToString()
  //<< " outer autos.";
  int counter=0;
  do
    do
    { counter++;
      //std::cout << "Checking combination " << counter;
      conjugationCandidates.SetSize(0);
      for (int i=0; i<selectedTypes.theElements.size; i++)
      { Selection& currentSel=selectedTypes.theElements[i].theSelection;
        SelectionWithMaxMultiplicity& currentOuterSelector= selectedOuterAutos.theElements[i];
        for (int j=0; j<currentSel.CardinalitySelection; j++)
        { currentComponent= theHsByType[i][currentSel.elements[j]];
          isoTypes[i].GetAutomorphismActingOnVectorROWSwhichStandOnTheRight
          (currentOuterAuto, currentOuterSelector.Multiplicities[j]);
          currentOuterAuto.ActOnVectorROWSOnTheLeft(currentComponent);
          conjugationCandidates.AddListOnTop(currentComponent);
        }
      }
      if (this->HasConjugateHsTo(conjugationCandidates))
        return true;
    }
    while(selectedOuterAutos.IncrementReturnFalseIfBackToBeginning());
  while (selectedTypes.IncrementReturnFalseIfBackToBeginning());
  return false;
}

void SemisimpleSubalgebras::HookUpCentralizers()
{ this->Hcandidates.QuickSortAscending();
  for (int i=0; i<this->Hcandidates.size; i++)
  { CandidateSSSubalgebra& currentSA=this->Hcandidates[i];
    currentSA.indexInOwner=i;
    currentSA.indicesDirectSummandSuperAlgebra.SetSize(0);
    currentSA.indexMaxSSContainer=-1;
    for (int j=0; j<this->Hcandidates.size; j++)
    { if (i==j)
        continue;
      CandidateSSSubalgebra& otherSA=this->Hcandidates[j];
      if (currentSA.IsDirectSummandOf(otherSA, true))
      { currentSA.indicesDirectSummandSuperAlgebra.AddOnTop(j);
        if (currentSA.indexMaxSSContainer==-1)
          currentSA.indexMaxSSContainer=j;
        if (this->Hcandidates[currentSA.indexMaxSSContainer].theWeylNonEmbeddeD.theDynkinType.GetRootSystemPlusRank()
            <otherSA.theWeylNonEmbeddeD.theDynkinType.GetRootSystemPlusRank())
          currentSA.indexMaxSSContainer=j;
        //std::cout << currentSA.theWeylNonEmbeddeD.theDynkinType.ToString()
        //<< " is a direct summand of " << otherSA.ToString();
      }
    }
  }
}

bool DynkinType::operator>(const DynkinType& other)const
{ if (this->GetRank()>other.GetRank())
    return true;
  if (other.GetRank()>this->GetRank())
    return false;
  DynkinType difference;
  difference=*this;
  difference-=other;
  if (difference.IsEqualToZero())
    return false;
  DynkinSimpleType highestSimpleTypeDifference=difference[0];
  Rational maxComponentDifferenceMult=difference.theCoeffs[0];
  for (int i=1; i<difference.size; i++)
    if (difference[i]>highestSimpleTypeDifference)
    { maxComponentDifferenceMult=difference.theCoeffs[i];
      highestSimpleTypeDifference=difference[i];
    }
  return maxComponentDifferenceMult>0;
}

bool CandidateSSSubalgebra::operator>(const CandidateSSSubalgebra& other)const
{ //if (this->owner!=other.owner)
  //{ std::cout << "This is a programming error: comparing CandidateSSSubalgebra with different owners. "
  //  << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  //  assert(false);
  //}
  return this->theWeylNonEmbeddeD.theDynkinType>other.theWeylNonEmbeddeD.theDynkinType;
}

bool DynkinSimpleType::IsPossibleCoRootLength(const Rational& input)const
{ if (this->theLetter=='C')
  { if ((this->lengthFirstCoRootSquared/input)==2 ||
        (this->lengthFirstCoRootSquared/input)==1)
      return true;
    return false;
  }
  if (this->theLetter=='F' || this->theLetter=='B' )
  { if ((input/this->lengthFirstCoRootSquared)==2 ||
        (input/this->lengthFirstCoRootSquared)==1)
      return true;
    return false;
  }
  if (this->theLetter=='G')
  { //std::cout << "<br>Testing input co-root length " << input << " vs current co-root length: "
    //<< this->lengthFirstCoRootSquared;
    if ((this->lengthFirstCoRootSquared/input)==3 ||
        (this->lengthFirstCoRootSquared/input)==1)
      return true;
    return false;
  }
  if (this->theLetter=='A' || this->theLetter=='D' || this->theLetter=='E')
    return input==this->lengthFirstCoRootSquared;
  std::cout << "This is a programmig error: non-initialized or otherwise faulty Dynkin simple type. "
  << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  assert(false);
  return false;
}

void CandidateSSSubalgebra::ComputeCartanOfCentralizer(GlobalVariables* theGlobalVariables)
{ Vectors<Rational> theHWs;
  Vectors<Rational> theCartan;
  theHWs.SetSize(this->highestVectorsModules.size);
  for (int i=0; i<this->highestVectorsModules.size; i++)
  { ElementSemisimpleLieAlgebra<Rational>& currentElt=this->highestVectorsModules[i];
    currentElt.ElementToVectorNegativeRootSpacesFirst(theHWs[i]);
  }
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  Vector<Rational> tempV;
  theCartan.SetSize(this->GetAmbientSS().GetRank());
  for (int i=0; i<this->GetAmbientSS().GetRank(); i++)
  { tempV.MakeEi(this->GetAmbientSS().GetRank(), i);
    tempElt.MakeHgenerator(tempV, this->GetAmbientSS());
    tempElt.ElementToVectorNegativeRootSpacesFirst(theCartan[i]);
  }
  Vectors<Rational> outputCartanCentralizer;
  theHWs.IntersectTwoLinSpaces(theHWs, theCartan, outputCartanCentralizer, theGlobalVariables);
  this->CartanOfCentralizer.SetSize(outputCartanCentralizer.size);
  for (int i=0; i<this->CartanOfCentralizer.size; i++)
  { tempElt.AssignVectorNegRootSpacesCartanPosRootSpaces(outputCartanCentralizer[i], *this->owner->owneR);
    this->CartanOfCentralizer[i]=tempElt.GetCartanPart();
    this->CartanOfCentralizer[i].ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
  }
}


