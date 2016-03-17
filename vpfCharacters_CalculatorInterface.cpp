#include "vpf.h"
#include "vpfHeader2Math3_FiniteGroups.h"
#include "vpfImplementationHeader2Math3_FiniteGroups.h"
#include "vpfHeader2Math3_SymmetricGroupsAndGeneralizations.h"
#include "vpfHeader2Math4_Graph.h"
#include "vpfHeader3Calculator3_WeylGroupCharacters.h"

static ProjectInformationInstance ProjectInfoVpfCharactersCalculatorInterfaceCPP(__FILE__, "Weyl group calculator interface. Work in progress by Thomas & Todor. ");

template<>
List<ClassFunction<WeylGroupData::WeylGroupBase, Rational> >::OrderLeftGreaterThanRight
FormatExpressions::GetMonOrder<ClassFunction<WeylGroupData::WeylGroupBase, Rational> >()
{ return 0;
}

bool WeylGroupData::CheckConsistency()const
{ if (this==0)
    crash << "The this pointer of a Weyl group is zero. " << crash;
  if (this->flagDeallocated)
    crash << "This is a programming error: use after free of WeylGroup. " << crash;
  for (int i=0; i<this->theGroup.generators.size; i++)
    this->theGroup.generators[i].CheckConsistency();
  this->RootsOfBorel.CheckConsistency();
  return true;
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::CheckInitializationFDrepComputation()const
{ if (this->theElements.size==0)
  { crash << "This is a programming error: requesting to compute character hermitian product in a group whose "
    << "conjugacy classes and/or elements have not been computed. The group reports to have "
    << this->ConjugacyClassCount() << " conjugacy classes and " << this->theElements.size << " elements. "
    << crash;
    return false;
  }
  return this->CheckInitializationConjugacyClasses();
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::CheckInitializationConjugacyClasses()const
{ if (this->ConjugacyClassCount()==0)
  { crash << "This is a programming error: requesting to compute character hermitian product in a group whose "
    << "conjugacy classes and/or elements have not been computed. The group reports to have "
    << this->ConjugacyClassCount() << " conjugacy classes and " << this->theElements.size << " elements. "
    << crash;
    return false;
  }
  for (int i=0; i<this->irreps.size; i++)
    if (this->irreps[i].theCharacteR.data.IsEqualToZero())
    { crash << "This is a programming error: irrep number " << i+1 << " has zero character!" << crash;
      return false;
    }
/*  Rational sumSquares=0;
  Rational tempRat;
  for (int i=0; i<this->ConjugacyClassCount(); i++)
  { tempRat=this->ConjugacyClassCount()/this->GetGroupSizeByFormula();
    sumSquares+=tempRat*tempRat;
  }
  if (sumSquares!=1)
    crash << "This is a programming error: sumSquares equals " << sumSquares.ToString() << " when it should equal 1. " << crash;*/
  return true;
}

template <typename somegroup, typename coefficient>
bool GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::CheckAllSimpleGensAreOK()const
{ this->CheckInitialization();
  for (int i=0; i<this->ownerGroup->generators.size; i++)
    if (this->generatorS[i].NumRows==0)
    { crash << "This is a programming error: working with a representation in which the action of the simple generators is not computed. " << crash;
      return false;
    }
  return true;
}

template <typename somegroup, typename coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::CheckRepIsMultiplicativelyClosed()
{ HashedList<Matrix<Rational> > tempList;
  tempList.AddOnTop(this->theElementImages);
  Matrix<Rational> tempMat;
  ElementWeylGroup<WeylGroupData> tempElt;
  for (int i=0; i<tempList.size; i++)
    for (int j=0; j<tempList.size; j++)
    { tempMat=tempList[i];
      tempMat.MultiplyOnTheLeft(tempList[j]);
      tempElt=this->ownerGroup->theElements[j];
      tempElt*=this->ownerGroup->theElements[i];
      tempElt.MakeCanonical();
      int targetIndex=this->ownerGroup->theElements.GetIndex(tempElt);
      if (!(tempMat==this->theElementImages[targetIndex]))
        crash << "this is a programming error: element " << i+1 << " times element "<< j+1 << " is outside of the set, i.e.,  "
        << tempList[i].ToString() << " * " << tempList[j].ToString() << " is bad. " << crash;
    }
}

/*
template <typename somegroup, typename coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::ComputeAllGeneratorImagesFromSimple(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("GroupRepresentationCarriesAllMatrices::ComputeAllGeneratorImagesFromSimple");
  this->CheckInitialization();
  this->ownerGroup->CheckInitializationFDrepComputation();
  HashedList<ElementWeylGroup<WeylGroupData> > ElementsExplored;
  ElementsExplored.SetExpectedSize(this->ownerGroup->theElements.size);
  this->theElementImageS[0].MakeIdMatrix(this->GetDim());
  ElementWeylGroup<WeylGroupData> currentElt;
  int theRank=this->ownerGroup->GetDim();
  currentElt.MakeID(*this->ownerGroup);
  ElementsExplored.AddOnTop(currentElt);
  List<ElementWeylGroup<WeylGroupData> > theGens;
  theGens.SetSize(theRank);
  for (int i=0; i<theRank; i++)
    theGens[i].MakeSimpleReflection(i, *this->ownerGroup);
  for (int i=0; i<ElementsExplored.size; i++)
  { int indexParentElement=this->ownerGroup->theElements.GetIndex(ElementsExplored[i]);
    for (int j=0; j<theRank; j++)
    { currentElt=theGens[j]* ElementsExplored[i];
      if (!ElementsExplored.Contains(currentElt))
      { int indexCurrentElt=this->ownerGroup->theElements.GetIndex(currentElt);
        this->theElementIsComputed[indexCurrentElt]=true;
        this->theElementImageS[indexParentElement].MultiplyOnTheLeft(this->generatorS[j], this->theElementImageS[indexCurrentElt]);
        ElementsExplored.AddOnTop(currentElt);
      }
    }
  }
//  this->CheckRepIsMultiplicativelyClosed();
  //stOutput << "<hr>";
  //FormatExpressions tempFormat;
  //tempFormat.flagUseLatex=true;
  //for (int i=0; i<this->theElementImages.size; i++)
  //{ stOutput << "<br>Element  " << i+1 << ": " << this->theElementImages[i].ToString() << " = "
  //  << this->theElementImages[i].ToString(& tempFormat);
  //}
  //stOutput << "<hr>";
}
*/

// This method uses auto everywhere, and a lot of copying data in order to use auto
template <typename somegroup, typename coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::ComputeAllElementImages(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("GroupRepresentationCarriesAllMatrices::ComputeAllGeneratorImagesFromSimple");
  this->CheckInitialization();
  this->ownerGroup->CheckInitializationFDrepComputation();
  auto ElementsExplored = this->ownerGroup->theElements;
  ElementsExplored.Clear();
  ElementsExplored.SetExpectedSize(this->ownerGroup->theElements.size);
  this->theElementImageS[0].MakeIdMatrix(this->GetDim());
  auto currentElt = this->ownerGroup->generators[0];
  currentElt.MakeID(this->ownerGroup->generators[0]);
  int theRank=this->ownerGroup->generators.size;
  auto& theGens = this->ownerGroup->generators;

  ElementsExplored.AddOnTop(currentElt);
  for (int i=0; i<ElementsExplored.size; i++)
  { int indexParentElement=this->ownerGroup->theElements.GetIndex(ElementsExplored[i]);
    for (int j=0; j<theRank; j++)
    { currentElt=theGens[j]* ElementsExplored[i];
      if (!ElementsExplored.Contains(currentElt))
      { int indexCurrentElt=this->ownerGroup->theElements.GetIndex(currentElt);
        this->theElementIsComputed[indexCurrentElt]=true;
        this->theElementImageS[indexParentElement].MultiplyOnTheLeft(this->generatorS[j], this->theElementImageS[indexCurrentElt]);
        ElementsExplored.AddOnTop(currentElt);
      }
    }
  }
//  this->CheckRepIsMultiplicativelyClosed();
  //stOutput << "<hr>";
  //FormatExpressions tempFormat;
  //tempFormat.flagUseLatex=true;
  //for (int i=0; i<this->theElementImages.size; i++)
  //{ stOutput << "<br>Element  " << i+1 << ": " << this->theElementImages[i].ToString() << " = "
  //  << this->theElementImages[i].ToString(& tempFormat);
  //}
  //stOutput << "<hr>";
}

template <typename somegroup, typename coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::operator*=(const GroupRepresentationCarriesAllMatrices<somegroup, coefficient>& other)
{ MacroRegisterFunctionWithName("WeylGroupRepresentation::operator*=");
  //lazy programmers handling://////
  if (&other==this )
  { GroupRepresentationCarriesAllMatrices<somegroup, coefficient> otherCopy;
    otherCopy=other;
    *this*=otherCopy;
    return;
  }
  //////////////////////////////////
  if (this->ownerGroup!=other.ownerGroup)
    crash << "This is a programming error: attempting to multiply representations with different owner groups. " << crash;
  if (!this->flagCharacterIsComputed || !other.flagCharacterIsComputed)
    crash << "Attempting to multiply weyl group reps whose characters have not been computed. " << crash;
  GroupRepresentationCarriesAllMatrices<somegroup, coefficient> output;
  output.init(*this->ownerGroup);
  output.theCharacteR=this->theCharacteR;
  output.theCharacteR*=other.theCharacteR;
  for (int i=0; i<output.generatorS.size; i++)
    output.generatorS[i].AssignTensorProduct(this->generatorS[i], other.generatorS[i]);
  for(int i=0; i<output.theElementImageS.size; i++)
    if (this->theElementIsComputed[i] && other.theElementIsComputed[i])
    { output.theElementImageS[i].AssignTensorProduct(this->theElementImageS[i],other.theElementImageS[i]);
      output.theElementIsComputed[i]=true;
    }
  *this=output;
}

template <typename somegroup, typename coefficient>
void GroupRepresentation<somegroup, coefficient>::operator*=(const GroupRepresentation<somegroup, coefficient>& other)
{ MacroRegisterFunctionWithName("GroupRepresentation::operator*=");
  //lazy programmers handling://////
  if (&other==this )
  { GroupRepresentation<somegroup, coefficient> otherCopy;
    otherCopy=other;
    *this*=otherCopy;
    return;
  }
  //////////////////////////////////
  if (this->ownerGroup!=other.ownerGroup)
    crash << "This is a programming error: attempting to multiply representations with different owner groups. " << crash;
  if (!this->flagCharacterIsComputed || !other.flagCharacterIsComputed)
    crash << "Attempting to multiply weyl group reps whose characters have not been computed. " << crash;
  GroupRepresentation<somegroup, coefficient> output;
  output.ownerGroup = this->ownerGroup;
  output.theCharacteR=this->theCharacteR;
  output.theCharacteR*=other.theCharacteR;
  for (int i=0; i<output.generatorS.size; i++)
    output.generatorS[i].AssignTensorProduct(this->generatorS[i], other.generatorS[i]);
  *this=output;
}

template <typename somegroup, typename coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::Restrict
(const Vectors<coefficient>& VectorSpaceBasisSubrep, const ClassFunction<somegroup, Rational>& remainingCharacter,
 GroupRepresentationCarriesAllMatrices<somegroup, coefficient>& output, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("WeylGroupRepresentation::Restrict");
  this->CheckAllSimpleGensAreOK();
  if (VectorSpaceBasisSubrep.size==0)
    crash << "This is a programming error: restriction of representation to a zero subspace is not allowed. " << crash;
  output.init(*this->ownerGroup);
  output.basis = VectorSpaceBasisSubrep;
  output.basis.GetGramMatrix(output.gramMatrixInverted, 0);
  output.gramMatrixInverted.Invert();
  output.theCharacteR=remainingCharacter;
  ProgressReport theReport(theGlobalVariables);
  for(int i=0; i<this->generatorS.size; i++)
  { if (theGlobalVariables!=0)
    { std::stringstream reportStream;
      reportStream << "Restricting the action of generator of index " << i;
      theReport.Report(reportStream.str());
    }
    Matrix<coefficient>::MatrixInBasis(this->generatorS[i], output.generatorS[i], output.basis, output.gramMatrixInverted);
  }
  /*
  for (int i=0; i<this->classFunctionMatrices.size; i++)
    if (this->classFunctionMatricesComputed[i])
    { output.classFunctionMatricesComputed[i]=true;
      if (theGlobalVariables!=0)
      { std::stringstream reportStream;
        reportStream << "Restricting class function matrix " << i+1 << " out of "
        << this->classFunctionMatrices.size;
        theReport.Report(reportStream.str());
      }
      Matrix<coefficient>::MatrixInBasis
      (this->classFunctionMatrices[i], output.classFunctionMatrices[i], output.vectorSpaceBasis,
       output.gramMatrixInverted);
    }
  */
  output.CheckAllSimpleGensAreOK();
}

template <typename somegroup, typename coefficient>
bool GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::DecomposeTodorsVersion(VirtualRepresentation<somegroup, coefficient>& outputIrrepMults,
                                                                                           List<GroupRepresentationCarriesAllMatrices<somegroup, coefficient> >* appendOnlyGRCAMSList)
{ MacroRegisterFunctionWithName("WeylGroupRepresentation::DecomposeTodorsVersion");
  this->CheckInitialization();
  this->ownerGroup->CheckInitializationFDrepComputation();
  outputIrrepMults.MakeZero();
  List<GroupRepresentation<somegroup, coefficient> > appendOnlyIrrepsList;
  for(int i=0; i<this->ownerGroup->irreps.size; i++)
    appendOnlyIrrepsList.AddOnTop(this->ownerGroup->irreps[i]);
  return this->DecomposeTodorsVersionRecursive(outputIrrepMults, appendOnlyIrrepsList, appendOnlyGRCAMSList);
}

template <class Element>
bool Matrix<Element>::GetEigenspacesProvidedAllAreIntegralWithEigenValueSmallerThanDim(List<Vectors<Element> >& output)const
{ int upperLimitComputations = 100000;
  output.SetSize(0);
  int found = 0;
  Polynomial<Element> theMinPoly;
  theMinPoly.AssignMinPoly(*this);
  Vector<Element> theEigenValueCandidate;
  theEigenValueCandidate.SetSize(1);
  Matrix<Rational> tempMat;
  for(int ii=0; ii<upperLimitComputations; ii++)
  { int i = ((ii+1)/2) * (2*(ii%2)-1); // 0,1,-1,2,-2,3,-3,...
//    stOutput << "checking " << i << " found " << found << "\n";
    theEigenValueCandidate[0] = i;
    if(theMinPoly.Evaluate(theEigenValueCandidate) == 0)
    { tempMat = *this;
//      stOutput << "<hr>The min poly is: " << theMinPoly.ToString() << " and evaluates at "
//      << theEigenValueCandidate << " to " << theMinPoly.Evaluate(theEigenValueCandidate).ToString();
      output.SetSize(output.size+1);
      tempMat.GetEigenspaceModifyMe(theEigenValueCandidate[0], *output.LastObject());
      if (output.LastObject()->size==0)
        crash << "This is a programmig error: " << theEigenValueCandidate[0].ToString() << " is a zero of the minimal polynomial "
        << theMinPoly.ToString() << " of the operator " << this->ToString() << " but the corresponding eigenspace is empty. " << crash;
//      stOutput << " <br> And the resulting eigenspace is: " << *output.LastObject();
      found += output.LastObject()->size;
      if(found == this->NumCols)
        return true;
    }
  }
  return false;
}

void WeylGroupData::ComputeIrreducibleRepresentationsWithFormulasImplementation(FiniteGroup<ElementWeylGroup<WeylGroupData> >& G)
{ List<char> letters;
  List<int> ranks;
  WeylGroupData& WD = *(G.generators[0].owner);
  WD.theDynkinType.GetLettersTypesMults(&letters,&ranks,NULL);
  // When WeylGroupRepresentation is merged with GroupRepresentation,
  // and WeylGroupData split from FiniteGroup<ElementWeylGroup<WeylGroup> >
  // theRepresentations will be this->theGroup->irreps
  // currently we have the difficulty with GroupRepresentation<WeylGroup, coefficient> vs
  // GroupRepresentation<FiniteGroup<ElementWeylGroup<WeylGroup> >, coefficient>
  if((letters.size == 1)&&(letters[0] == 'A'))
  { int theRank = ranks[0];
    List<Partition> thePartitions;
    Partition::GetPartitions(thePartitions,theRank+1);
    G.irreps.SetSize(thePartitions.size);
    for(int i=0; i<thePartitions.size; i++)
    { thePartitions[i].SpechtModuleMatricesOfTranspositionsjjplusone(G.irreps[i].generatorS);
      G.irreps[i].ownerGroup = &G;
      G.irreps[i].identifyingString = thePartitions[i].ToString();
      G.irreps[i].ComputeCharacter();
    }
    G.irreps.QuickSortAscending();
  }
  else if((letters.size == 1)&&(letters[0] == 'B'))
  { int theRank = ranks[0];
    HyperoctahedralGroupData HOG;
    HOG.MakeHyperoctahedralGroup(theRank);
    HOG.AllSpechtModules();
    GroupHomomorphism<ElementWeylGroup<WeylGroupData>, ElementHyperoctahedralGroupR2> phi;
    phi.preimageGroup = &G;
    phi.generatorImages.SetSize(G.generators.size);
    for(int i=0; i<phi.generatorImages.size; i++)
    { phi.generatorImages[i].h.AddTransposition(i,i+1);
      phi.generatorImages[0].k.ToggleBit(i+1);
    }
    G.irreps.SetSize(HOG.theGroup->irreps.size);
    for(int i=0; i<HOG.theGroup->irreps.size; i++)
      G.irreps[i] = phi.PullbackRepresentation(HOG.theGroup->irreps[i]);
    stOutput << "Generator commutation relations of groups\n";
    stOutput << HOG.theGroup->PrettyPrintGeneratorCommutationRelations();
    FiniteGroup<ElementHyperoctahedralGroupR2> phiG;
    phiG.generators = phi.generatorImages;
    stOutput << phiG.PrettyPrintGeneratorCommutationRelations();
    stOutput << G.PrettyPrintGeneratorCommutationRelations();
  }
  else if((letters.size == 1) && (letters[0] == 'D'))
  { int theRank = ranks[0];
    HyperoctahedralGroupData HOG;
    HOG.MakeHyperoctahedralGroup(theRank);
    GroupHomomorphism<ElementWeylGroup<WeylGroupData>, ElementHyperoctahedralGroupR2> inclusionMap;
    stOutput << HOG.theGroup->PrettyPrintGeneratorCommutationRelations() << '\n';
    stOutput << G.PrettyPrintGeneratorCommutationRelations() << '\n';
  }
// silently fail instead of crashing to support calling into this and if it doesn't
// work then using brute force
//  else
//    crash << "ComputeIrreducibleRepresentationsUsingSpechtModules: Type " << this->theDynkinType << " is unsupported.  If you think it should work, edit " << __FILE__ << ":" << __LINE__ << crash;
  stOutput << G.PrettyPrintCharacterTable() << '\n';
}

bool CalculatorFunctionsWeylGroup::innerWeylRaiseToMaximallyDominant(Calculator& theCommands, const Expression& input, Expression& output, bool useOuter)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylRaiseToMaximallyDominant");
  if (input.children.size<2)
    return output.MakeError("Raising to maximally dominant takes at least 2 arguments, type and vector", theCommands);
  const Expression& theSSalgebraNode=input[1];
  SemisimpleLieAlgebra* theSSalgebra;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, theSSalgebraNode, theSSalgebra))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  Vectors<Rational> theHWs;
  theHWs.SetSize(input.children.size-2);
  bool isGood=true;
  for (int i=2; i<input.children.size; i++)
    if (!theCommands.GetVectoR<Rational>(input[i], theHWs[i-2], 0, theSSalgebra->GetRank()))
    { isGood=false;
      break;
    }
  if (!isGood && input.children.size==3)
  { //stOutput << "here be i";
    Matrix<Rational> theHWsMatForm;
    if (theCommands.GetMatrix(input[2], theHWsMatForm, 0, theSSalgebra->GetRank()))
    { theHWsMatForm.GetVectorsFromRows(theHWs);
      isGood=true;
    }
  }
  if (theHWs.size==0 || !isGood)
    return output.MakeError("Failed to extract rational vectors from expression " + input.ToString() + ". ", theCommands);
  std::stringstream out;
  out << "Input: " << theHWs.ToString()
  << ", simultaneously raising to maximally dominant in Weyl group of type "
  << theSSalgebra->theWeyl.theDynkinType.ToString();
//  stOutput << out.str();
  theSSalgebra->theWeyl.RaiseToMaximallyDominant(theHWs, useOuter);
  out << "<br>Maximally dominant output: " << theHWs.ToString();
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupOrbitOuterSimple(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupOrbitOuterSimple");
  if (!input.IsListNElements(3))
    return output.MakeError("innerWeylOrbit takes two arguments", theCommands);
  const Expression& theSSalgebraNode=input[1];
  const Expression& vectorNode=input[2];
  DynkinType theType;
  if (theSSalgebraNode.IsOfType<SemisimpleLieAlgebra>())
    theType=theSSalgebraNode.GetValue<SemisimpleLieAlgebra>().theWeyl.theDynkinType;
  else
    if (!CalculatorConversions::innerDynkinType(theCommands, theSSalgebraNode, theType))
      return false;
  Vector<Polynomial<Rational> > theHWfundCoords, theHWsimpleCoords, currentWeight;
  Expression theContext;
  if (!theCommands.GetVectoR(vectorNode, theHWfundCoords, &theContext, theType.GetRank(), CalculatorConversions::innerPolynomial<Rational>))
    return output.MakeError("Failed to extract highest weight", theCommands);
  WeylGroupData theWeyl;
  theWeyl.MakeFromDynkinType(theType);
  theHWsimpleCoords=theHWfundCoords;
  theHWfundCoords=theWeyl.GetFundamentalCoordinatesFromSimple(theHWsimpleCoords);
  std::stringstream out, latexReport;
  Vectors<Polynomial<Rational> > theHWs;
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
//  theFormat.fundamentalWeightLetter="\\psi";
  theHWs.AddOnTop(theHWsimpleCoords);
  HashedList<Vector<Polynomial<Rational> > > outputOrbit;
  WeylGroupData orbitGeneratingSet;
  Polynomial<Rational> theExp;
  if (!theWeyl.GenerateOuterOrbit(theHWs, outputOrbit, &orbitGeneratingSet.theGroup.theElements, 1921*2))
    out << "Failed to generate the entire orbit (maybe too large?), generated the first " << outputOrbit.size
    << " elements only.";
  else
    out << "The orbit has " << outputOrbit.size << " elements.";
  latexReport << "\\begin{longtable}{p{3cm}p{4cm}p{4cm}p{4cm}}Element & Eps. coord. & Image fund. coordinates& Hw minus wt. \\\\\n<br>";
  out << "<table><tr> <td>Group element</td> <td>Image in simple coords</td> <td>Epsilon coords</td><td>Fundamental coords</td>";
  out << "</tr>";
  MonomialUniversalEnveloping<Polynomial<Rational> > standardElt;
  LargeInt tempInt;
  bool useMathTag=outputOrbit.size<150;
  Matrix<Rational> epsCoordMat;
  theWeyl.theDynkinType.GetEpsilonMatrix(epsCoordMat);
  for (int i=0; i<outputOrbit.size; i++)
  { theFormat.simpleRootLetter="\\alpha";
    theFormat.fundamentalWeightLetter="\\psi";
    std::string orbitEltString=outputOrbit[i].ToString(&theFormat);
    Vector<Polynomial<Rational> > epsVect=outputOrbit[i];
    epsCoordMat.ActOnVectorColumn(epsVect);
    std::string orbitEltStringEpsilonCoords=epsVect.ToStringLetterFormat("\\varepsilon", &theFormat);
    std::string weightEltString=
    theWeyl.GetFundamentalCoordinatesFromSimple(outputOrbit[i]).ToStringLetterFormat
    (theFormat.fundamentalWeightLetter, &theFormat);
    out << "<tr>" << "<td>"
    << (useMathTag ? CGI::GetMathSpanPure(orbitGeneratingSet.theGroup.theElements[i].ToString()) : orbitGeneratingSet.theGroup.theElements[i].ToString())
    << "</td><td>" << (useMathTag ? CGI::GetMathSpanPure(orbitEltString) : orbitEltString) << "</td><td>"
    << (useMathTag ? CGI::GetMathSpanPure(orbitEltStringEpsilonCoords) : orbitEltStringEpsilonCoords)
    << "</td><td>" << (useMathTag ? CGI::GetMathSpanPure(weightEltString) : weightEltString) << "</td>";
    latexReport << "$" << orbitGeneratingSet.theGroup.theElements[i].ToString(&theFormat) << "$ & $" << orbitEltStringEpsilonCoords << "$ & $"
    << weightEltString << "$ & $" << (outputOrbit[0]-outputOrbit[i]).ToStringLetterFormat(theFormat.simpleRootLetter, &theFormat) << "$\\\\\n<br>";
    out << "</tr>";
  }
  latexReport << "\\end{longtable}";
  out << "</table>" << "<br> " << latexReport.str();
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupOrbitSize
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupOrbitSize");
  //double startTimeForDebug=theGlobalVariables.GetElapsedSeconds();
  SemisimpleLieAlgebra* theSSalgebra=0;
  Vector<Rational> theWeightRat;
  Expression theContextE;
  if (theCommands.GetTypeWeight<Rational>(theCommands, input, theWeightRat, theContextE, theSSalgebra, 0))
  { //stOutput << " DEBUG: needed " << theGlobalVariables.GetElapsedSeconds()-startTimeForDebug
    //<< " seconds to get type/weight info. ";
    //stOutput << "DEBUG: Computing orbit size for: " << theSSalgebra->theWeyl.theDynkinType.ToString();
    Rational result=theSSalgebra->theWeyl.GetOrbitSize(theWeightRat);
    return output.AssignValue(result, theCommands);
  }
  Vector<Polynomial<Rational> > theWeightPoly;
  if (theCommands.GetTypeWeight<Polynomial<Rational> >
      (theCommands, input, theWeightPoly, theContextE, theSSalgebra,CalculatorConversions::innerPolynomial<Rational>))
  { Rational result=theSSalgebra->theWeyl.GetOrbitSize(theWeightPoly);
    return output.AssignValue(result, theCommands);
  }
  return false;
}

bool CalculatorFunctionsWeylGroup::innerWeylOrbit(Calculator& theCommands, const Expression& input, Expression& output, bool useFundCoords, bool useRho)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylOrbit");
  if (!input.IsListNElements(3))
    return output.MakeError("innerWeylOrbit takes two arguments", theCommands);
  SemisimpleLieAlgebra* theSSalgebra=0;
  Vector<Polynomial<Rational> > theWeight;
  Expression theContextE;
  if (!theCommands.GetTypeWeight
      (theCommands, input, theWeight, theContextE, theSSalgebra, CalculatorConversions::innerPolynomial<Rational>))
    return false;
  Vector<Polynomial<Rational> > theHWfundCoords, theHWsimpleCoords, currentWeight;
  WeylGroupData& theWeyl=theSSalgebra->theWeyl;
  if (!useFundCoords)
  { theHWsimpleCoords=theWeight;
    theHWfundCoords=theWeyl.GetFundamentalCoordinatesFromSimple(theWeight);
  } else
  { theHWfundCoords=theWeight;
    theHWsimpleCoords=theWeyl.GetSimpleCoordinatesFromFundamental(theWeight);
  }
  std::stringstream out, latexReport;
  Vectors<Polynomial<Rational> > theHWs;
  FormatExpressions theFormat;
  theContextE.ContextGetFormatExpressions(theFormat);
//  theFormat.fundamentalWeightLetter="\\psi";
  theHWs.AddOnTop(theHWsimpleCoords);
  HashedList<Vector<Polynomial<Rational> > > outputOrbit;
  HashedList<ElementWeylGroup<WeylGroupData> > orbitGeneratingSet;
  Polynomial<Rational> theExp;
  if (!theSSalgebra->theWeyl.GenerateOrbit(theHWs, useRho, outputOrbit, false, 1921, &orbitGeneratingSet, 1921))
    out << "Failed to generate the entire orbit (maybe too large?), generated the first " << outputOrbit.size << " elements only.";
  else
    out << "The orbit has " << outputOrbit.size << " elements.";
  latexReport
  << "\\begin{longtable}{p{3cm}p{4cm}p{4cm}p{4cm}}Element & Eps. coord. & Image fund. coordinates& Hw minus wt. \\\\\n<br>";
  out << "<table><tr> <td>Group element</td> <td>Image in simple coords</td> <td>Epsilon coords</td><td>Fundamental coords</td>";
  if (useRho)
    out << "<td>Homomorphism generator candidate</td>";
  out << "</tr>";
  MonomialUniversalEnveloping<Polynomial<Rational> > standardElt;
  LargeInt tempInt;
  bool useMathTag=outputOrbit.size<150;
  Matrix<Rational> epsCoordMat;
  theWeyl.theDynkinType.GetEpsilonMatrix(epsCoordMat);
  Graph integralPositiveRootReflectionGraph;
  integralPositiveRootReflectionGraph.numNodes=outputOrbit.size;
  integralPositiveRootReflectionGraph.nodeLabels.SetSize(outputOrbit.size);
  theFormat.flagUseFrac=true;
  for (int i=0; i<outputOrbit.size; i++)
    integralPositiveRootReflectionGraph.nodeLabels[i]=
    "$" + theWeyl.GetEpsilonCoords(outputOrbit[i]).ToStringEpsilonFormat(&theFormat) + "$ = $"+
    theWeyl.GetFundamentalCoordinatesFromSimple(outputOrbit[i]).ToStringLetterFormat("\\psi")+"$";
  ElementWeylGroup<WeylGroupData> currentElt;
  Vector<Polynomial<Rational> > differenceVector;
  Rational currentCoordDifference;
  for (int i=0; i<outputOrbit.size; i++)
  { for (int j=0; j<theWeyl.RootsOfBorel.size; j++)
    { currentWeight=outputOrbit[i];
      currentElt.MakeRootReflection(theWeyl.RootsOfBorel[j], theWeyl);
      if (useRho)
        currentWeight+=theWeyl.rho;
      theWeyl.ActOn(currentElt, currentWeight);
      if (useRho)
        currentWeight-=theWeyl.rho;
      differenceVector=outputOrbit[i]-currentWeight;
      bool isGood=!differenceVector.IsEqualToZero();
      for (int k=0; k<differenceVector.size; k++)
        if (!differenceVector[k].IsConstant(&currentCoordDifference))
        { isGood=false;
          break;
        } else
          if (!currentCoordDifference.IsInteger() || currentCoordDifference<0)
          { isGood=false;
            break;
          }
      if (isGood)
      { std::stringstream reflectionStream;
        reflectionStream << "s_{" << i << "}";
        integralPositiveRootReflectionGraph.AddEdge(i, outputOrbit.GetIndex(currentWeight), reflectionStream.str());
      }
    }
  }
  integralPositiveRootReflectionGraph.CheckConsistency();
  out << integralPositiveRootReflectionGraph.ToStringPsTricks(0);
  for (int i=0; i<outputOrbit.size; i++)
  { theFormat.simpleRootLetter="\\alpha";
    theFormat.fundamentalWeightLetter="\\psi";
    std::string orbitEltString=outputOrbit[i].ToString(&theFormat);
    Vector<Polynomial<Rational> > epsVect=outputOrbit[i];
    epsCoordMat.ActOnVectorColumn(epsVect);
    std::string orbitEltStringEpsilonCoords=epsVect.ToStringLetterFormat("\\varepsilon", &theFormat);
    std::string weightEltString=
    theWeyl.GetFundamentalCoordinatesFromSimple(outputOrbit[i]).ToStringLetterFormat(theFormat.fundamentalWeightLetter, &theFormat);
    out << "<tr>" << "<td>"
    << (useMathTag ? CGI::GetMathMouseHover(orbitGeneratingSet[i].ToString()) : orbitGeneratingSet[i].ToString())
    << "</td><td>"
    << (useMathTag ? CGI::GetMathMouseHover(orbitEltString) : orbitEltString) << "</td><td>"
    << (useMathTag ? CGI::GetMathMouseHover(orbitEltStringEpsilonCoords) : orbitEltStringEpsilonCoords)
    << "</td><td>"
    << (useMathTag ? CGI::GetMathMouseHover(weightEltString) : weightEltString)
    << "</td>";
    latexReport << "$" << orbitGeneratingSet[i].ToString(&theFormat) << "$ & $" << orbitEltStringEpsilonCoords
    << "$ & $" <<  weightEltString << "$ & $" << (outputOrbit[0]-outputOrbit[i]).ToStringLetterFormat(theFormat.simpleRootLetter, &theFormat)
    << "$\\\\\n<br>";
    if (useRho)
    { currentWeight=theHWsimpleCoords;
      standardElt.MakeOne(*theSSalgebra);
      bool isGood=true;
      for (int j=orbitGeneratingSet[i].generatorsLastAppliedFirst.size-1; j>=0; j--)
      { int simpleIndex=orbitGeneratingSet[i].generatorsLastAppliedFirst[j].index;
        theExp=theWeyl.GetScalarProdSimpleRoot(currentWeight, simpleIndex);
        theWeyl.SimpleReflectionRhoModified(simpleIndex, currentWeight);
        theExp*=2;
        theExp/=theWeyl.CartanSymmetric.elements[simpleIndex][simpleIndex];
        theExp+=1;
        if (theExp.IsInteger(&tempInt))
          if (tempInt<0)
          { isGood=false;
            break;
          }
        standardElt.MultiplyByGeneratorPowerOnTheLeft(theSSalgebra->GetNumPosRoots() -simpleIndex-1, theExp);
      }
      out << "<td>";
      if (isGood)
        out << CGI::GetMathMouseHover(standardElt.ToString(&theFormat));
      else
        out << "-";
      out << "</td>";
    }
    out << "</tr>";
  }
  latexReport << "\\end{longtable}";
  out << "</table>" << "<br> " << latexReport.str();
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupLoadOrComputeCharTable(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupLoadOrComputeCharTable");
  if (!CalculatorConversions::innerLoadWeylGroup(theCommands, input, output))
    return false;
  WeylGroupData& theGroup=output.GetValueNonConst<WeylGroupData>();
  if (theGroup.GetDim()>8)
  { theCommands << "Computing character table disabled for rank>=8, modify file " << __FILE__
    << " line "  << __LINE__ << " to change that. ";
    return false;
  }
  std::stringstream reportStream;
  theGroup.ComputeOrLoadCharacterTable(&reportStream);
  theCommands << reportStream.str();
  return output.AssignValue(theGroup, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClasseS(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClasseS");
  if (!CalculatorConversions::innerLoadWeylGroup(theCommands, input, output))
    return false;
//  stOutput << "got ere3!";
  WeylGroupData& theGroup=output.GetValueNonConst<WeylGroupData>();
//  stOutput << "got ere4!";
  if (theGroup.GetDim()>8)
  { theCommands << "Conjugacy classes computation disabled for rank >8. Modify source code "
    << "file " << __FILE__ << " line " << __LINE__ << " and rebuild the calculator to change that.";
    return false;
  }
  std::stringstream out;
  theGroup.ComputeOrLoadConjugacyClasses(&out);
  theCommands << out.str();
  return output.AssignValue(theGroup, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClassesFromAllElements(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClassesFromAllElements");
  if (!CalculatorConversions::innerLoadWeylGroup(theCommands, input, output))
    return false;
  WeylGroupData& theGroupData=output.GetValueNonConst<WeylGroupData>();
  if (theGroupData.GetDim()>7)
  { theCommands << "<hr>Loaded Dynkin type " << theGroupData.theDynkinType.ToString() << " of rank " << theGroupData.GetDim() << " but I've been told "
    << "not to compute when the rank is larger than 7. ";
    return false;
  }
  double timeStart1=theGlobalVariables.GetElapsedSeconds();
  theGroupData.theGroup.ComputeCCfromAllElements();
  //std::stringstream out;
  theCommands << "<hr> Computed conjugacy classes of " << theGroupData.ToString() << " in " << theGlobalVariables.GetElapsedSeconds()-timeStart1
  << " second(s). ";
  return output.AssignValue(theGroupData, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClassesRepresentatives(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClassesRepresentatives");
  if (!CalculatorConversions::innerLoadWeylGroup(theCommands, input, output))
    return false;
  WeylGroupData& theGroupData=output.GetValueNonConst<WeylGroupData>();
  theGroupData.CheckConsistency();
  if (theGroupData.GetDim()>8)
    return theCommands << "<hr>Loaded Dynkin type " << theGroupData.theDynkinType.ToString() << " of rank " << theGroupData.GetDim() << " but I've been told "
    << "not to compute when the rank is larger than 8. ";
  theGroupData.CheckConsistency();
  double timeStart1=theGlobalVariables.GetElapsedSeconds();
  theGroupData.CheckConsistency();
//  theGroup.ComputeCCRepresentatives(&theGlobalVariables);
  theGroupData.theGroup.ComputeCCSizesAndRepresentatives();
  //std::stringstream out;
  theCommands << "<hr> Computed conjugacy classes representatives of "
  << theGroupData.theDynkinType.ToString() << " in " << theGlobalVariables.GetElapsedSeconds()-timeStart1
  << " second(s). ";
  return output.AssignValue(theGroupData, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupIrrepsAndCharTableComputeFromScratch(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupIrrepsAndCharTableComputeFromScratch");
  if (!CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClasseS(theCommands, input, output))
    return false;
  if (!output.IsOfType<WeylGroupData>())
    return true;
  WeylGroupData& theGroupData=output.GetValueNonConst<WeylGroupData>();
//  stOutput << "And the group is: " << theGroup.ToString();
  theGroupData.ComputeInitialIrreps();
  theGroupData.theGroup.ComputeIrreducibleRepresentationsTodorsVersion();
  FormatExpressions tempFormat;
  tempFormat.flagUseLatex=true;
  tempFormat.flagUseHTML=false;
  std::stringstream out;
  out << "Character table: ";
  out << theGroupData.theGroup.PrettyPrintCharacterTable();
  //Matrix<Rational> charMat;
  //charMat.init(theGroupData.theGroup.ConjugacyClassCount(), theGroupData.theGroup.ConjugacyClassCount());
  //for (int i=0; i<theGroupData.theGroup.irreps.size; i++)
  //{ //out << "<br>" << theGroup.irreps[i].theCharacteR.ToString();
  //  charMat.AssignVectorToRowKeepOtherRowsIntactNoInit(i, theGroupData.irreps[i].GetCharacter().data);
  //}
  //out << CGI::GetMathSpanPure(charMat.ToString(&tempFormat));
  out << "<br>Explicit realizations of each representation follow.";
  for (int i=0; i<theGroupData.theGroup.irreps.size; i++)
    out << "<hr>" << theGroupData.theGroup.irreps[i].ToString(&tempFormat);
  out << theGroupData.ToString(&tempFormat);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupOuterAutoGeneratorsPrint(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupOuterAutoGeneratorsPrint");
  DynkinType theType;
  if (!CalculatorConversions::innerDynkinType(theCommands, input, theType))
    return output.MakeError("Failed to extract Dynkin type from argument. ", theCommands);
  std::stringstream out, outCommand;
  FinitelyGeneratedMatrixMonoid<Rational> groupGeneratedByMatrices;
  theType.GetOuterAutosGeneratorsActOnVectorColumn(groupGeneratedByMatrices.theGenerators);
  FormatExpressions tempFormat;
  tempFormat.flagUseLatex=true;
  tempFormat.flagUseHTML=false;
  for (int i=0; i<groupGeneratedByMatrices.theGenerators.size; i++)
  { outCommand << "<br>s_{" << i+1 << "}=MatrixRationals" << groupGeneratedByMatrices.theGenerators[i].ToStringMatForm(&tempFormat) << ";";
    out << "<br>s_" << i+1 << " = " << CGI::GetMathSpanPure(groupGeneratedByMatrices.theGenerators[i].ToStringMatForm(&tempFormat) );
  }
  outCommand << "<br>GenerateFiniteMultiplicativelyClosedSet(1000, ";
  for (int i=0; i<groupGeneratedByMatrices.theGenerators.size; i++)
  { outCommand << "s_{" << i+1 << "}";
    if (i!=groupGeneratedByMatrices.theGenerators.size-1)
      outCommand << ", ";
  }
  outCommand << ");";
  out << outCommand.str();
  bool success= groupGeneratedByMatrices.GenerateElements(10000);
  if (!success)
    out << "<br>Did not succeed to generate all elements of the group - the group is of size larger than 10000";
  else
  { out << "<br>The group generated by the outer automorphisms is of size " << groupGeneratedByMatrices.theElements.size;
    if (groupGeneratedByMatrices.theElements.size>100)
      out << "<br>As the group has more than 100 elements, I shall abstain from printing them. ";
    else
    { out << "<table><tr><td>Element</td><td>Matrix</td></tr>";
      for (int i=0; i<groupGeneratedByMatrices.theElements.size; i++)
      { std::stringstream elementNameStream;
        elementNameStream << "t_" << i+1;
        out << "<tr><td>" << CGI::GetMathMouseHover(elementNameStream.str())<< "</td><td>"
        << CGI::GetMathMouseHover(groupGeneratedByMatrices.theElements[i].ToStringMatForm(&tempFormat)) << "</td></tr>";
      }
      out << "</table>";
    }
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupOrbitFundRho(Calculator& theCommands, const Expression& input, Expression& output)
{ return CalculatorFunctionsWeylGroup::innerWeylOrbit(theCommands, input, output, true, true);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupOrbitFund(Calculator& theCommands, const Expression& input, Expression& output)
{ return CalculatorFunctionsWeylGroup::innerWeylOrbit(theCommands, input, output, true, false);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupOrbitSimple
(Calculator& theCommands, const Expression& input, Expression& output)
{ return CalculatorFunctionsWeylGroup::innerWeylOrbit(theCommands, input, output, false, false);
}

bool CalculatorFunctionsWeylGroup::innerTensorWeylReps(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerTensorWeylReps");
  if (input.children.size!=3)
    return false;
  GroupRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> leftRep;
  GroupRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> rightRep;
  if (!input[1].IsOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> >(&leftRep))
    return false;
  if (!input[2].IsOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> > (&rightRep))
    return false;
  FormatExpressions theFormat;
  theFormat.flagUseLatex=true;
  theFormat.flagUseHTML=false;
  //stOutput << "<br>left rep is: " << leftRep.ToString(&theFormat);
  //stOutput << "<br>right rep is: " << rightRep.ToString(&theFormat);
  if (leftRep.ownerGroup!=rightRep.ownerGroup)
    return output.MakeError("Error: attempting to tensor irreps with different owner groups. ", theCommands);
  leftRep*=rightRep;
  return output.AssignValue(leftRep, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerTensorAndDecomposeWeylReps(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerTensorAndDecomposeWeylReps");
  Expression theTensor;
  if (!(input.children.size==3))
    return false;
  const Expression& leftE=input[1];
  const Expression& rightE=input[2];
  if (!leftE.IsOfType<VirtualRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> >())
    return false;
  if (!rightE.IsOfType<VirtualRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> >())
    return false;
  VirtualRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> outputRep=leftE.GetValue<VirtualRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> >();
  outputRep*=rightE.GetValue<VirtualRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> >();
  return output.AssignValue(outputRep, theCommands);
}

std::string WeylGroupData::ToStringIrrepLabel(int indexIrrep)
{ if (indexIrrep<this->irrepsCarterLabels.size)
    return this->irrepsCarterLabels[indexIrrep];
  std::stringstream out;
  out << "\\phi_{" << indexIrrep+1 << "}";
  return out.str();
}

std::string WeylGroupData::ToStringSignSignatureRootSubsystem(const List<SubgroupDataRootReflections>& inputSubgroups)
{ MacroRegisterFunctionWithName("WeylGroup::ToStringSignSignatureRootSubsystem");
  if (inputSubgroups.size==0)
    return "";
  std::stringstream out;

  FormatExpressions formatSupressUpperIndexOne;
  formatSupressUpperIndexOne.flagSupressDynkinIndexOne=true;
  DynkinSimpleType simpleType;

  std::stringstream mainTableStream;
  int numParabolicClasses=0, numNonParabolicPseudoParabolic=0, numNonPseudoParabolic=0;
  for (int i=0; i<inputSubgroups.size; i++)
  { SubgroupDataRootReflections& currentSG=inputSubgroups[i];
    if (!currentSG.flagIsParabolic && !currentSG.flagIsExtendedParabolic)
    { numNonPseudoParabolic++;
      continue;
    }
    if (currentSG.flagIsParabolic)
      numParabolicClasses++;
    if (!currentSG.flagIsParabolic && currentSG.flagIsExtendedParabolic)
      numNonParabolicPseudoParabolic++;
  }

  //check for repeating signatures
  List<List<Rational> > pseudoSignSig, fullSignSig, parabolicSignSig;
  fullSignSig.SetSize(this->theGroup.ConjugacyClassCount());
  pseudoSignSig.SetSize(this->theGroup.ConjugacyClassCount());
  parabolicSignSig.SetSize(this->theGroup.ConjugacyClassCount());
  for (int i=0; i<this->theGroup.ConjugacyClassCount(); i++)
  { fullSignSig[i].SetSize(inputSubgroups.size);
    pseudoSignSig[i].SetSize(numParabolicClasses+numNonParabolicPseudoParabolic);
    parabolicSignSig[i].SetSize(numParabolicClasses);
    for (int j=0; j<inputSubgroups.size; j++)
    { fullSignSig[i][j]= (inputSubgroups[j].tauSignature[i]==0 ? 0 : 1 );
      if (j<numParabolicClasses+numNonParabolicPseudoParabolic)
        pseudoSignSig[i][j]=fullSignSig[i][j];
      if (j<numParabolicClasses)
        parabolicSignSig[i][j]=fullSignSig[i][j];
    }
  }
  bool hasRepeatingFullSigs=false;
  bool hasRepeatingPseudoParabolicSigs=false;
  bool hasRepeatingParSigs=false;
  for (int i=0; i<fullSignSig.size && !hasRepeatingFullSigs; i++)
    for (int j=i+1; j<fullSignSig.size && !hasRepeatingFullSigs; j++)
      if (fullSignSig[i]==fullSignSig[j])
        hasRepeatingFullSigs=true;
  for (int i=0; i<pseudoSignSig.size && !hasRepeatingPseudoParabolicSigs; i++)
    for (int j=i+1; j<pseudoSignSig.size && !hasRepeatingPseudoParabolicSigs; j++)
      if (pseudoSignSig[i]==pseudoSignSig[j])
        hasRepeatingPseudoParabolicSigs=true;
  for (int i=0; i<parabolicSignSig.size && !hasRepeatingParSigs; i++)
    for (int j=i+1; j<parabolicSignSig.size && !hasRepeatingParSigs; j++)
      if (parabolicSignSig[i]==parabolicSignSig[j])
        hasRepeatingParSigs=true;
  if (hasRepeatingParSigs)
    out << "<hr><b>There are repeating parabolic sign signatures. </b><hr>";
  else
    out << "<hr>No repeating parabolic sign signatures. <hr>";
  if (hasRepeatingPseudoParabolicSigs)
    out << "<hr><b>There are repeating pseudo-parabolic sign signatures. </b><hr>";
  else
    out << "<hr>No repeating pseudo-parabolic sign signatures. <hr>";
  if (hasRepeatingFullSigs)
    out << "<hr><b>There are repeating extended sign signatures.</b> <hr>";
  else
    out << "<hr>No repeating extended sign signatures. <hr>";
  if (hasRepeatingParSigs)
  { HashedList<List<Rational> > parSignSigsNoRepetition;
    List<List<std::string> > irrepsPerSignature;
    parSignSigsNoRepetition.AddOnTopNoRepetition(parabolicSignSig);
    irrepsPerSignature.SetSize(parSignSigsNoRepetition.size);
    for (int i=0; i<parabolicSignSig.size; i++)
      irrepsPerSignature[parSignSigsNoRepetition.GetIndex(parabolicSignSig[i])].AddOnTop
      (this->irrepsCarterLabels[i]);
    mainTableStream << "\n<br>\n\n<br>\nThe following families of representations share the same sign signature. ";
    for (int i=0; i<irrepsPerSignature.size; i++)
      if (irrepsPerSignature[i].size>1)
      { mainTableStream << "$(";
        for (int j=0; j<irrepsPerSignature[i].size; j++)
        { mainTableStream << irrepsPerSignature[i][j];
          if (j!=irrepsPerSignature[i].size-1)
            mainTableStream << ", ";
        }
        mainTableStream << ")$ ";
      }
    mainTableStream << "\n<br>\n";
  } //end of check for repeating signatures
  if (hasRepeatingPseudoParabolicSigs)
  { HashedList<List<Rational> > pseudoSigsNoRepetition;
    List<List<std::string> > irrepsPerSignature;
    pseudoSigsNoRepetition.AddOnTopNoRepetition(pseudoSignSig);
    irrepsPerSignature.SetSize(pseudoSigsNoRepetition.size);
    for (int i=0; i<pseudoSignSig.size; i++)
      irrepsPerSignature[pseudoSigsNoRepetition.GetIndex(pseudoSignSig[i])].AddOnTop(this->irrepsCarterLabels[i]);
    mainTableStream << "\n<br>\n\n<br>\nThe following families of representations share the same pseudo-sign signature. ";
    for (int i=0; i<irrepsPerSignature.size; i++)
      if (irrepsPerSignature[i].size>1)
      { mainTableStream << "$(";
        for (int j=0; j<irrepsPerSignature[i].size; j++)
        { mainTableStream << irrepsPerSignature[i][j];
          if (j!=irrepsPerSignature[i].size-1)
            mainTableStream << ", ";
        }
        mainTableStream << ")$ ";
      }
    mainTableStream << "\n<br>\n";
  } //end of check for repeating signatures
  int startIndex=0;
  int numColsPerPage=25;
  int startIndexNextCol=0;
  for (;;)
  { startIndex=startIndexNextCol;
    if (startIndex>=inputSubgroups.size)
      break;
    startIndexNextCol=startIndex+numColsPerPage;
    if (startIndexNextCol-inputSubgroups.size>-3)
      startIndexNextCol=inputSubgroups.size;

    mainTableStream << "\n<br>\n\\begin{longtable}{c|";
    for (int i=startIndex; i<startIndexNextCol; i++)
    { if (i==numParabolicClasses)
        mainTableStream << "|";
      if (i==numParabolicClasses+numNonParabolicPseudoParabolic)
        mainTableStream << "|";
      mainTableStream << "p{0.275cm}";
    }
    mainTableStream << "}\n<br>\n" << "\\caption{\\label{table:SignSignature"
    << CGI::CleanUpForLaTeXLabelUse(this->theDynkinType.ToString())
    << "}Multiplicity of the sign representation over the classes of root subgroups. "
    << "There are " << numParabolicClasses << " parabolic subgroup classes, " << numNonParabolicPseudoParabolic
    << " pseudo-parabolic subgroup classes that are not parabolic, and "
    << numNonPseudoParabolic << " non-pseudo-parabolic subgroup classes. \n<br>\n"
    << "}\\\\ ";
    for (int i=startIndex; i<startIndexNextCol; i++)
    { SubgroupDataRootReflections& currentSG=inputSubgroups[i];
      if (!currentSG.flagIsParabolic && !currentSG.flagIsExtendedParabolic)
      { mainTableStream << "&-";
        continue;
      }
      if (currentSG.flagIsParabolic)
        mainTableStream << "&$\\mathfrak{p}_{" << i+1 << "}$";
      if (!currentSG.flagIsParabolic && currentSG.flagIsExtendedParabolic)
        mainTableStream << "&${\\widehat{\\mathfrak{ p}}}_{" << i-numParabolicClasses+1 << "}$";
    }
    mainTableStream << "\\\\\n<br>\n";
    mainTableStream << "Irrep label";
    if (this->theDynkinType.IsSimple(&simpleType.theLetter, &simpleType.theRank, &simpleType.CartanSymmetricInverseScale))
      for (int i=startIndex; i<startIndexNextCol; i++)
        mainTableStream << "&$" << inputSubgroups[i].theDynkinType.ToString(&formatSupressUpperIndexOne) << "$";
    else
      for (int i=startIndex; i<startIndexNextCol; i++)
        mainTableStream << "&$" << inputSubgroups[i].theDynkinType.ToString() << "$";
    for (int i=0; i<this->theGroup.ConjugacyClassCount(); i++)
    { mainTableStream << "\\\\";
      if (i==0)
        mainTableStream << "\\hline";
      mainTableStream << "\n<br>\n$" << this->ToStringIrrepLabel(i) << "$";
      for (int j=startIndex; j<startIndexNextCol; j++)
        mainTableStream << "&" << inputSubgroups[j].tauSignature[i].ToString();
      mainTableStream << "\\\\\n<br>\n";
    }
    mainTableStream << "\\end{longtable}\n<br>\n";
  }
  for (int s=0; s<2; s++)
  { out << "<table style=\"white-space: nowrap;\" border=\"1\">";
    Selection parSelrootsAreOuttaLevi;
    out << "<tr><td>Irrep Label</td><td>Irreducible representation characters</td>";
    if (inputSubgroups[0].flagIsParabolic || inputSubgroups[0].flagIsExtendedParabolic)
    { for (int i=0; i<inputSubgroups.size; i++)
      { parSelrootsAreOuttaLevi=inputSubgroups[i].simpleRootsInLeviParabolic;
        parSelrootsAreOuttaLevi.InvertSelection();
        out << "<td>" << parSelrootsAreOuttaLevi.ToString() << "</td>";
      }
      out << "</tr><tr><td></td><td></td>";
    }
    if (this->theDynkinType.IsSimple(&simpleType.theLetter, &simpleType.theRank, &simpleType.CartanSymmetricInverseScale))
      for (int i=0; i<inputSubgroups.size; i++)
        out << "<td>" << inputSubgroups[i].theDynkinType.ToString(&formatSupressUpperIndexOne)
        << "</td>";
    else
      for (int i=0; i<inputSubgroups.size; i++)
        out << "<td>" << inputSubgroups[i].theDynkinType.ToString() << "</td>";
    out << "</tr>";
    for (int i=0; i<this->theGroup.ConjugacyClassCount(); i++)
    { out << "<tr>";
      if (i<this->irrepsCarterLabels.size)
        out << "<td>" << this->irrepsCarterLabels[i] << "</td>";
      else
        out << "<td></td>";
      out << "<td>" << this->theGroup.characterTable[i].ToString() << "</td>";
      if (s==0)
        for (int j=0; j<inputSubgroups.size; j++)
          out << "<td>" << inputSubgroups[j].tauSignature[i].ToString() << "</td>";
      if (s==1)
        for (int j=0; j<pseudoSignSig[i].size; j++)
          out << "<td>" << pseudoSignSig[i][j].ToString() << "</td>";
      out << "</tr>";
    }
    out << "</table>";
    out << "<br>";
    if (s==0)
      out << "Clipped tau signature follows.<br>";
  }

  out << "<br>A version of the table in ready for LaTeX consumption form follows.<br>\n<br>\n\n<br>\n\n<br>\n";
  out << "\\documentclass{article}\\usepackage{amssymb}\\usepackage{longtable}\\usepackage{pdflscape}"
  << "\\addtolength{\\hoffset}{-3.5cm}\\addtolength{\\textwidth}{6.8cm}\\addtolength{\\voffset}{-3.3cm}\\addtolength{\\textheight}{6.3cm}"
  << " \\begin{document}\\begin{landscape}\n<br>\n\n<br>\n\n<br>\n\n<br>\n";
  out << "{\\tiny \n<br>\n \\renewcommand{\\arraystretch}{0}%\n<br>\n";
  out << "\\begin{longtable}{rl}\\caption{\\label{tableIrrepChars" << this->theDynkinType.ToString()
  << "}\\\\ Irreducible representations and their characters}\\\\ \n<br>\n Irrep label & Character\\\\\n<br>\n";
  for (int i=0; i<this->theGroup.characterTable.size; i++)
    out << "$" << this->ToStringIrrepLabel(i) << "$&$" << this->theGroup.characterTable[i].ToString() << "$\\\\\n<br>\n";
  out << "\\end{longtable}\n<br>\n";
  out << "\\begin{longtable}{rcl}"<< "\\caption{\\label{tableConjugacyClassTable"
  << CGI::CleanUpForLaTeXLabelUse(this->theDynkinType.ToString()) << "}}\\\\ ";
  out << "Representative & Class size & Root subsystem label\\\\\n<br>\n";
  for (int i=0; i<this->theGroup.ConjugacyClassCount(); i++)
  { out << "$" << this->theGroup.conjugacyClasseS[i].representative.ToString() << "$&$ " << this->theGroup.conjugacyClasseS[i].size.ToString() << "$";
    if (i<this->ccCarterLabels.size)
      out << "&$" << this->ccCarterLabels[i] << "$";
    out << "\\\\\n<br>\n";
  }
  out << "\\end{longtable}\n<br>\n";

  out << mainTableStream.str();
  out << "}%arraystretch renewcommand scope\n<br\n>\n<br>\n\n<br>\n\n<br>\n\n<br>\n";
  out << "\\end{landscape}\\end{document}";
  return out.str();
}

bool CalculatorFunctionsWeylGroup::innerSignSignatureRootSubsystems(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerSignSignatureRootSubsystems");
  if (!CalculatorConversions::innerLoadWeylGroup(theCommands, input, output))
    return false;
  if (!output.IsOfType<WeylGroupData>())
    return false;
  WeylGroupData& theWeyl=output.GetValueNonConst<WeylGroupData>();
  if (theWeyl.GetDim()>8)
  { theCommands << "<hr>Computing sign signatures restricted up to rank 8.";
    return false;
  }
  std::stringstream out;
  List<SubgroupDataRootReflections> parabolicSubgroupS, extendedParabolicSubgroups, allRootSubgroups, finalSubGroups;
  if (!theWeyl.LoadSignSignatures(finalSubGroups, &theGlobalVariables))
  { theWeyl.GetSignSignatureParabolics(parabolicSubgroupS);
    theWeyl.GetSignSignatureExtendedParabolics(extendedParabolicSubgroups);
    theWeyl.GetSignSignatureAllRootSubsystems(allRootSubgroups);
    List<Pair<std::string, List<Rational>, MathRoutines::hashString> > tauSigPairs;
    finalSubGroups.Reserve(allRootSubgroups.size);
    Pair<std::string, List<Rational>, MathRoutines::hashString> currentTauSig;
    for (int j=0; j<3; j++)
    { List<SubgroupDataRootReflections>* currentSGs=0;
      if (j==0)
        currentSGs=&parabolicSubgroupS;
      if (j==1)
        currentSGs=&extendedParabolicSubgroups;
      if (j==2)
        currentSGs=&allRootSubgroups;
      for (int i=0; i<currentSGs->size; i++)
      { currentTauSig.Object1=(*currentSGs)[i].theDynkinType.ToString();
        currentTauSig.Object2=(*currentSGs)[i].tauSignature;
        if (!tauSigPairs.Contains(currentTauSig))
        { tauSigPairs.AddOnTop(currentTauSig);
          finalSubGroups.AddOnTop((*currentSGs)[i]);
        }
      }
    }
  }
  out << theWeyl.ToStringSignSignatureRootSubsystem(finalSubGroups);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerDecomposeWeylRep(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerDecomposeWeylRep");
  if (!input.IsOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> > ())
    return false;
//  theRep.Decomposition(theCFs, outputReps);
  GroupRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational>& inputRep =
      input.GetValueNonConst<GroupRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> >();
  VirtualRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> outputRep;
  inputRep.MakeGRCAM().DecomposeTodorsVersion(outputRep);
  return output.AssignValue(outputRep, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerIsOuterAutoWeylGroup(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerIsOuterAutoWeylGroup");
  if (input.children.size!=3)
    return theCommands << "<hr>IsOuterAuto expects 2 arguments.";
  DynkinType theType;
  if (!CalculatorConversions::innerDynkinType(theCommands, input[1], theType))
    return theCommands << "<hr>Failed to get Dynkin type from argument. " << input[1].ToString();
  Matrix<Rational> theMat;
  if (!input[2].IsOfType<Matrix<Rational> >(&theMat))
    if (!theCommands.GetMatrix(input[2], theMat))
      return theCommands << "<hr>Failed to get matrix from argument. " << input[2].ToString();
  if (theMat.NumCols!=theMat.NumRows || theMat.NumCols!=theType.GetRank())
  { theCommands << "<hr>Extracted Dynkin type " << theType.ToString() << " is of rank " << theType.GetRank()
    << " but extracted linear operator has " << theMat.NumCols << " columns and " << theMat.NumRows << " rows.";
    return false;
  }
  WeylGroupData tempW;
  tempW.MakeFromDynkinType(theType);
  tempW.ComputeRho(true);
  MatrixTensor<Rational> theOp;
  theOp=theMat;
  if (tempW.IsElementWeylGroupOrOuterAuto(theOp))
    return output.AssignValue(1, theCommands);
  return output.AssignValue(0, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupNaturalRep(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupNaturalRep");
  if (!CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClasseS(theCommands, input, output))
    return false;
  if (!output.IsOfType<WeylGroupData>())
    return false;
//  stOutput << "not implemented!";
  WeylGroupData& theGroup=output.GetValueNonConst<WeylGroupData>();
  theGroup.ComputeInitialIrreps();
  theGroup.theGroup.ComputeIrreducibleRepresentationsTodorsVersion();
//  stOutput << theGroup.ToString();
  GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> tempRep;
  theGroup.GetStandardRepresentation(tempRep);
  return output.AssignValue(tempRep.MakeOtherGroupRepresentationClass(), theCommands);
}

class MonomialMacdonald
{
public:
  SemisimpleLieAlgebra* owner;
  Selection rootSel;
  bool flagDeallocated;
  MonomialMacdonald():owner(0), flagDeallocated(false)
  {
  }
  std::string ToString(FormatExpressions* theFormat=0)const;
  bool CheckConsistency()
  { if (this->flagDeallocated)
    { crash << "This is a programming error: use after free of MonomialMacdonald. " << crash;
      return false;
    }
    return true;
  }
  static unsigned int HashFunction(const MonomialMacdonald& input)
  { return input.rootSel.HashFunction();
  }
  bool operator==(const MonomialMacdonald& other)const;
  void MakeFromRootSubsystem(const Vectors<Rational>& inputRoots, SemisimpleLieAlgebra& inputOwner);
  void ActOnMeSimpleReflection(int indexSimpleReflection, Rational& outputMultiple);
  void GenerateMyOrbit(HashedList<MonomialMacdonald>& output);
  ~MonomialMacdonald()
  { this->flagDeallocated=true;
  }
};

std::string MonomialMacdonald::ToString(FormatExpressions* theFormat)const
{ MacroRegisterFunctionWithName("MonomialMacdonald::ToString");
  if (this->owner==0)
    return "(non-initialized)";
  if (this->rootSel.CardinalitySelection==0)
    return "1";
  std::stringstream out;
  for (int i=0; i<this->rootSel.CardinalitySelection; i++)
  { out << "a_" << this->rootSel.elements[i] << "";
  }
  return out.str();
}

bool MonomialMacdonald::operator==(const MonomialMacdonald& other)const
{ return this->owner==other.owner && this->rootSel==other.rootSel;
}

void MonomialMacdonald::GenerateMyOrbit(HashedList<MonomialMacdonald>& output)
{ MacroRegisterFunctionWithName("MonomialMacdonald::GenerateMyOrbit");
  output.Clear();
  output.AddOnTop(*this);
  MonomialMacdonald currentMon;
  Rational tempRat;
  for (int i=0; i<output.size; i++)
    for (int j=0; j<this->owner->GetRank(); j++)
    { currentMon=output[i];
      currentMon.ActOnMeSimpleReflection(j, tempRat);
      output.AddOnTopNoRepetition(currentMon);
    }
}

void MonomialMacdonald::MakeFromRootSubsystem(const Vectors<Rational>& inputRoots, SemisimpleLieAlgebra& inputOwner)
{ MacroRegisterFunctionWithName("MonomialMacdonald::MakeFromRootSubsystem");
  this->owner=&inputOwner;
  this->rootSel.init(inputOwner.theWeyl.RootSystem.size);
  Vector<Rational> currentV;
  for (int i=0; i<inputRoots.size; i++)
  { currentV=inputRoots[i];
    if (currentV.IsNegative())
      currentV*=-1;
    int indexInRoots=inputOwner.theWeyl.RootSystem.GetIndex(currentV);
    if (indexInRoots<0)
      crash << "This is a programming error: attempting to make a Macdonald polynomial from " << inputRoots.ToString() << ": the vector " << currentV.ToString()
      << " is not a root. " << crash;
    this->rootSel.AddSelectionAppendNewIndex(indexInRoots);
  }
  this->rootSel.ComputeIndicesFromSelection();
}

void MonomialMacdonald::ActOnMeSimpleReflection(int indexSimpleReflection, Rational& outputMultiple)
{ Selection originalSel;
 // stOutput << "this->rootSel:" << this->rootSel.ToString() << ", elements: " << this->rootSel.elements;

  originalSel=this->rootSel;
  this->rootSel.init(this->owner->theWeyl.RootSystem.size);
  Vector<Rational> currentV;
  outputMultiple=1;
//  stOutput << originalSel.ToString() << ", elements: " << originalSel.elements;
  for (int i=0; i<originalSel.CardinalitySelection; i++)
  { currentV=this->owner->theWeyl.RootSystem[originalSel.elements[i]];
    this->owner->theWeyl.SimpleReflection(indexSimpleReflection, currentV);
    if (currentV.IsNegative())
    { currentV*=-1;
      outputMultiple*=-1;
    }
    this->rootSel.AddSelectionAppendNewIndex(this->owner->theWeyl.RootSystem.GetIndex(currentV));
  }
}

bool CalculatorFunctionsWeylGroup::innerMacdonaldPolys(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerMacdonaldPolys");
  //note that if input is list of 2 elements then input[0] is sequence atom, and your two elements are in fact
  //input[1] and input[2];
  SemisimpleLieAlgebra* thePointer=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, input, thePointer))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  rootSubalgebras theRootSAs;
  theRootSAs.owner=thePointer;
  theRootSAs.ComputeAllReductiveRootSubalgebrasUpToIsomorphism();
  std::stringstream out;
  MonomialMacdonald theGenerator;
  HashedList<MonomialMacdonald> theOrbit;
  for (int i=0; i<theRootSAs.theSubalgebras.size; i++)
  { rootSubalgebra& currentRootSA=theRootSAs.theSubalgebras[i];
    theGenerator.MakeFromRootSubsystem(currentRootSA.PositiveRootsK, *thePointer);
    theGenerator.GenerateMyOrbit(theOrbit);
    out << "<hr>Root subsystem type " << currentRootSA.theDynkinDiagram.ToString();
    out << ". Orbit has " << theOrbit.size << " element(s), here they are: ";
    for (int j=0; j<theOrbit.size; j++)
      out << "<br>" << theOrbit[j].ToString();
  }
  out << "Type: " << theRootSAs.owner->theWeyl.theDynkinType.ToString() << ". Number of root subsystems: " << theRootSAs.theSubalgebras.size;
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerLieAlgebraWeight(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerLieAlgebraWeight");
  Weight<Polynomial<Rational> > resultWeight;
  if (input.children.size!=4)
    return false;
  Expression tempE;
  SemisimpleLieAlgebra* theSSowner=0;
  if (!CalculatorConversions::innerSSLieAlgebra(theCommands, input[1], tempE, theSSowner))
    return theCommands << "<hr>Failed to load semisimple Lie algebra";
  std::string theCoordsString;
  bool isGood=input[3].IsAtom(&theCoordsString);
  if (isGood)
    isGood= (theCoordsString=="epsilon") || (theCoordsString=="fundamental") || (theCoordsString=="simple");
  if (!isGood)
    return theCommands << "<hr>The third argument of MakeWeight is bad: must be one of the keywords: epsilon, fundamental, simple. ";
  int theWeightIndex=-1;
  if (!input[2].IsSmallInteger(&theWeightIndex))
    return false;
  if (theCoordsString!="epsilon")
  { if (theWeightIndex<1 || theWeightIndex> theSSowner->GetRank())
    { std::stringstream errorStream;
      errorStream << "The second argument of the MakeWeight function needs to be index of a weight between 1 and the Lie algebra rank. "
      << "However, the index is " << theWeightIndex << ".";
      return output.MakeError(errorStream.str(), theCommands);
    }
    Vector<Polynomial<Rational> > EiVector;
    EiVector.MakeEi(theSSowner->GetRank(), theWeightIndex-1);
    if (theCoordsString=="fundamental")
      resultWeight.weightFundamentalCoordS= EiVector;
    else if (theCoordsString=="simple")
      resultWeight.weightFundamentalCoordS= theSSowner->theWeyl.GetFundamentalCoordinatesFromSimple(EiVector);
  } else
  { Vector<Rational> EiVector;
    EiVector.MakeZero(theSSowner->GetRank());
    Vector<Rational> tempV=theSSowner->theWeyl.GetEpsilonCoords(EiVector);
    if (theWeightIndex>tempV.size || theWeightIndex<1)
    { std::stringstream errorStream;
      errorStream << "The second argument of the MakeWeight function needs to be index of a weight between 1 and " << tempV.size
      << ". However, the index is " << theWeightIndex << ".";
      return output.MakeError(errorStream.str(), theCommands);
    }
    EiVector.MakeEi(tempV.size, theWeightIndex-1);
//    stOutput << "Getting fundamental coords from eps coords: " << EiVector.ToString();
    resultWeight.weightFundamentalCoordS=theSSowner->theWeyl.GetFundamentalCoordinatesFromEpsilon(EiVector);
  }
  resultWeight.owner=theSSowner;
  Expression theContext;
  theContext.MakeContextSSLieAlg(theCommands, *theSSowner);
  return output.AssignValueWithContext(resultWeight, theContext, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerLieAlgebraRhoWeight(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerLieAlgebraRhoWeight");
  Weight<Polynomial<Rational> > resultWeight;
  SemisimpleLieAlgebra* theSSowner=0;
  Expression tempE;
  if (!CalculatorConversions::innerSSLieAlgebra(theCommands, input, tempE, theSSowner))
    return theCommands << "<hr>Failed to load semisimple Lie algebra. ";
  theSSowner->CheckConsistency();
  Expression theContext;
  theContext.MakeContextSSLieAlg(theCommands, *theSSowner);
  resultWeight.weightFundamentalCoordS= theSSowner->theWeyl.GetFundamentalCoordinatesFromSimple(theSSowner->theWeyl.rho);
  resultWeight.owner=theSSowner;
  return output.AssignValueWithContext(resultWeight, theContext, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerTestSpechtModules(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerTestSpechtModules");
  int theSymmetricGroupRank=0;
  if (!input.IsSmallInteger(&theSymmetricGroupRank))
    return theCommands << "innerTestSpechtModules called with input that is not a small integer, not performing any tests.";
  if (theSymmetricGroupRank <1)
    return theCommands << "innerTestSpechtModules takes as input a small positive integer, instead the input I got was "
    << theSymmetricGroupRank << ". ";
  if (theSymmetricGroupRank>6)
    return theCommands
    << "For speed/memory reasons, innerTestSpechtModules is currently restricted to take input no larger than 6. Your input was: "
    << theSymmetricGroupRank;
  std::stringstream out;
  out << "User has requested the test of Specht modules of S_"
  << theSymmetricGroupRank << ". ";
  Partition::TestAllSpechtModules(theSymmetricGroupRank);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerRepresentElementHyperOctahedral(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerRepresentElementHyperOctahedral");
  if (input.children.size!=3)
    return theCommands << "Representating takes 2 arguments: element and rep.";
  ElementHyperoctahedralGroupR2 theElt;
  GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> theRep;
  if (!input[1].IsOfType(&theElt))
    return theCommands << "Failed to extract element of hyperoctahedral group from " << input[1].ToString();
  if (!input[2].IsOfType(&theRep))
    return theCommands << "Failed to extract representation from " << input[2].ToString();
  Matrix<Rational> result;
  if (!theRep.GetMatrixOfElement(theElt, result))
    return theCommands << "Failed to get matrix of element " << theElt.ToString() << " from representation: " << theRep.ToString();
  return output.AssignMatrix(result, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerHyperOctahedralGetOneRepresentation(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerHyperOctahedralGetOneRepresentation");
  Vector<Rational> inputLeftRat, inputRightRat;
  if (input.children.size!=3)
    return theCommands << "CalculatorFunctionsWeylGroup::innerHyperOctahedralGetOneRepresentation needs two arguments";
//  stOutput << "<br>nput 1 and 2: " << input[1].ToString() << input[2].ToString();
  if (!theCommands.GetVectoR(input[1], inputLeftRat)|| !theCommands.GetVectoR(input[2], inputRightRat))
    return false;
//  stOutput << "<br>extracted vectors: " << inputLeftRat.ToString() << inputRightRat.ToString();
  if (inputLeftRat.size<1 || inputRightRat.size<1)
    return false;
  const int maxPartitionSize=1000;
  Vector<int> inputLeft, inputRight;
  Vector<int>* currentInput=& inputLeft;
  Vector<Rational>* currentInputRat=& inputLeftRat;
  for ( int inputCounter=0; inputCounter<2; inputCounter++, currentInput=&inputRight, currentInputRat= &inputRightRat)
  { currentInput->SetSize(currentInputRat->size);
    for (int i=0; i<currentInputRat->size; i++)
      if (!(*currentInputRat)[i].IsIntegerFittingInInt(&(*currentInput)[i]))
        return theCommands << "Failed to convert input: " << input.ToString() << " to a list of small integers.";
    for (int i=0; i<currentInput->size; i++)
      if ((*currentInput)[i]<1 || (*currentInput)[i]> maxPartitionSize )
        return theCommands <<  "Entry: " << (*currentInput)[i] << " of " << (*currentInput) << " is outside of the allowed input range.";
    if (currentInput->SumCoords()>maxPartitionSize)
      return theCommands << "The coordinates of vector " << (*currentInput) << " have sum that is too large. ";
  }
  Partition partitionLeft, partitionRight;
  partitionLeft.FromListInt(inputLeft);
  partitionRight.FromListInt(inputRight);
  //std::stringstream out;
  //out << "Left partition is: " << partitionLeft.ToString() << ", created from: " << inputLeft;
  //out << "Right partition is: " << partitionRight.ToString() << ", created from: " << inputRight;
  int index=0;
  for(; index<theCommands.theObjectContainer.theHyperOctahedralGroups.size; index++)
    if(theCommands.theObjectContainer.theHyperOctahedralGroups[index].flagIsEntireHyperoctahedralGroup && theCommands.theObjectContainer.theHyperOctahedralGroups[index].N == partitionLeft.n + partitionRight.n)
      break;
  if(index == theCommands.theObjectContainer.theHyperOctahedralGroups.size)
  { theCommands.theObjectContainer.theHyperOctahedralGroups.SetSize(theCommands.theObjectContainer.theHyperOctahedralGroups.size+1);
    theCommands.theObjectContainer.theHyperOctahedralGroups[index].MakeHyperoctahedralGroup(partitionLeft.n+partitionRight.n);
  }
  //<-may be broken if copying of groups doesn't work!!!!!!!!
  HyperoctahedralGroupData& HD=theCommands.theObjectContainer.theHyperOctahedralGroups[index];
  GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational> R;
  HD.SpechtModuleOfPartititons(partitionLeft,partitionRight, R);
  //out << R;
  R.ComputeCharacter();
  return output.AssignValue(R, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerSpechtModule(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerSpechtModule");
  Vector<Rational> inputRat;
  if (!theCommands.GetVectorFromFunctionArguments(input, inputRat))
    return false;
  if (inputRat.size<1)
    return false;
  const int maxPartitionSize=1000;
  Vector<int> inputInt;
  inputInt.SetSize(inputRat.size);
  for (int i=0; i<inputRat.size; i++)
    if (!(inputRat[i].IsIntegerFittingInInt(&inputInt[i])))
      return theCommands << "Failed to convert input: " << input.ToString() << " to a list of small integers.";
  for (int i=0; i<inputInt.size; i++)
    if ((inputInt[i]<1) || (inputInt[i]> maxPartitionSize))
      return theCommands <<  "Entry: " << inputInt[i] << " of " << inputInt << " is outside of the allowed input range.";
  if (inputInt.SumCoords()>maxPartitionSize)
    return theCommands << "The coordinates of vector " << inputInt << " have sum that is too large. ";
  Partition p;
  p.FromListInt(inputInt);
  std::stringstream out;
  out << "Partition is " << p.ToString();
  List<Matrix<Rational> > gens;
  p.SpechtModuleMatricesOfTranspositionsjjplusone(gens);
  for(int i=0; i<gens.size; i++)
    out << i << "\n" << gens[i].ToStringPlainText() << "\n";
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerHyperOctahedralAllModulesInducedFromSpechtModules(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerHyperOctahedralAllModulesInducedFromSpechtModules");
  int theRank=0;
  if (!input.IsSmallInteger(&theRank))
    return false;
  if (theRank<1|| theRank >7)
    return theCommands << "Input of hyperoctahedral print function has to be between 1 and 10";
  HyperoctahedralGroupData G;
  G.MakeHyperoctahedralGroup(theRank);
  G.AllSpechtModules();
  return output.AssignValue(G.theGroup->PrettyPrintCharacterTable(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerHyperOctahedralPrintGeneratorCommutationRelations(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerHyperOctahedralPrintGeneratorCommutationRelations");
  int theRank=0;
  if (!input.IsSmallInteger(&theRank))
    return false;
  if (theRank<1|| theRank >10)
    return theCommands << "Input of hyperoctahedral print function has to be between 1 and 10";
  HyperoctahedralGroupData G;
  G.MakeHyperoctahedralGroup(theRank);
  return output.AssignValue(G.theGroup->PrettyPrintGeneratorCommutationRelations(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupElement(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupElement");
  //if (!input.IsSequenceNElementS(2))
  //return output.MakeError("Function Coxeter element takes two arguments.", theCommands);
  if(input.children.size<2)
    return output.MakeError("Function WeylElement needs to know what group the element belongs to", theCommands);
  //note that if input is list of 2 elements then input[0] is sequence atom, and your two elements are in fact
  //input[1] and input[2];
  SemisimpleLieAlgebra* thePointer;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorConversions::innerSSLieAlgebra, input[1], thePointer))
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  ElementWeylGroup<WeylGroupData> theElt;
  theElt.generatorsLastAppliedFirst.Reserve(input.children.size-2);
  for(int i=2; i<input.children.size; i++)
  { int tmp;
    if (!input[i].IsSmallInteger(& tmp))
      return false;
    theElt.MultiplyOnTheRightBySimpleReflection(tmp-1);
  }
  theElt.owner=&thePointer->theWeyl;
  //stOutput << "<br>theElt.owner: " << theElt.owner;
//  stOutput << "<b>Not implemented!!!!!</b> You requested reflection indexed by " << theReflection;
  for(int i=0; i<theElt.generatorsLastAppliedFirst.size; i++)
    if (theElt.generatorsLastAppliedFirst[i].index >= thePointer->GetRank() ||
        theElt.generatorsLastAppliedFirst[i].index < 0)
      return output.MakeError("Bad reflection index", theCommands);
//  stOutput << "\n" << theGroup.rho << " " << theElt.owner->rho << "\n";
  theElt.MakeCanonical();
  return output.AssignValue(theElt, theCommands);
}

bool Calculator::innerGenerateMultiplicativelyClosedSet(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerGenerateMultiplicativelyClosedSet");
  if (input.children.size<=2)
    return output.MakeError("I need at least two arguments - upper bound and at least one element to multiply.", theCommands);
  int upperLimit;
  if (!input[1].IsSmallInteger(&upperLimit))
    return output.MakeError("First argument must be a small integer, serving as upper bound for the set.", theCommands);
  if (upperLimit <=0)
  { upperLimit=10000;
    theCommands << "The upper computation limit I got was 0 or less; I replaced it with the default value " << upperLimit << ".";
  }
  HashedList<Expression> theSet;
  theSet.SetExpectedSize(input.children.size-2);
  for (int i=2; i<input.children.size; i++)
    theSet.AddOnTop(input[i]);
  int numGenerators=theSet.size;
  Expression theProduct, evaluatedProduct;
  //stOutput << "<br>" << theSet[0].ToString() << "->" << theSet[0].ToStringFull() << " is with hash " << theSet[0].HashFunction();
  ProgressReport theReport(&theGlobalVariables);
  for (int i=0; i<theSet.size; i++)
    for (int j=0; j<numGenerators; j++)
    { theProduct.MakeProducT(theCommands, theSet[j], theSet[i]);
      std::stringstream reportStream;
      reportStream << "found " << theSet.size << "elements so far, exploring element " << i+1;
      reportStream << "<br>Evaluating: " << theProduct.ToString();
      theReport.Report(reportStream.str());
      theCommands.EvaluateExpression(theCommands, theProduct, evaluatedProduct);
      //stOutput << " to get " << evaluatedProduct.ToString() << "->" << evaluatedProduct.ToStringFull();
      //stOutput << " with hash " << evaluatedProduct.HashFunction();
      //if (evaluatedProduct==theSet[0])
      //{ //stOutput << " and equals the first element. ";
      //}
      theSet.AddOnTopNoRepetition(evaluatedProduct);
      if (theSet.size>upperLimit)
      { std::stringstream out;
        out << "<hr>While generating multiplicatively closed set, I went above the upper limit of " << upperLimit << " elements.";
        evaluatedProduct.MakeError(out.str(), theCommands);
        theSet.AddOnTop(evaluatedProduct);
        i=theSet.size; break;
      }
    }
  theCommands << "<hr>Generated a list of " << theSet.size << " elements";
  output.reset(theCommands, theSet.size+1);
  output.AddChildAtomOnTop(theCommands.opSequence());
  for (int i=0; i<theSet.size; i++)
    output.AddChildOnTop(theSet[i]);
  return true;
}

template <typename somegroup, typename coefficient>
void VirtualRepresentation<somegroup, coefficient>::operator*=(const VirtualRepresentation<somegroup, coefficient>& other)
{ MacroRegisterFunctionWithName("VirtualRepresentation::operator*=");
  crash << "Not implemented yet. " << crash;
/*  WeylGroupVirtualRepresentation<coefficient> output, currentContribution;
  output.ownerGroup=this->ownerGroup;
  output.coefficientsIrreps.MakeZero(this->coefficientsIrreps.size);
  WeylGroupRepresentation<Rational> tempRep;
  for (int i=0; i<this->coefficientsIrreps.size; i++)
    for (int j=0; j<other.coefficientsIrreps.size; j++)
    { Rational theCoeff= this->coefficientsIrreps[i]*other.coefficientsIrreps[j];
      if (theCoeff==0)
        continue;
      tempRep=this->ownerGroup->irreps[i];
      tempRep*=this->ownerGroup->irreps[j];
      tempRep.DecomposeTodorsVersion(currentContribution, 0);
      output.coefficientsIrreps+=currentContribution*theCoeff;
    }
  *this=output;*/
}

template <typename somegroup, typename coefficient>
void VirtualRepresentation<somegroup, coefficient>::AssignRep(const GroupRepresentationCarriesAllMatrices<somegroup, Rational>& other, GlobalVariables* theGlobalVariables)
{ crash << " not implemented " << crash;
  GroupRepresentationCarriesAllMatrices<somegroup, coefficient> otherCopy;
  otherCopy=other;
//  otherCopy.DecomposeTodorsVersion(this->coefficientsIrreps, theGlobalVariables);
}

template <typename somegroup, typename coefficient>
void VirtualRepresentation<somegroup, coefficient>::AssignRep(const GroupRepresentation<somegroup, Rational>& other, GlobalVariables* theGlobalVariables)
{ VirtualRepresentation<somegroup, coefficient> out;
  out.AddMonomial(other.theCharacteR, 1);
//  otherCopy.DecomposeTodorsVersion(this->coefficientsIrreps, theGlobalVariables);
}

bool CalculatorFunctionsWeylGroup::innerMakeVirtualWeylRep(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerMakeVirtualWeylRep");
  if (input.IsOfType<VirtualRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> >())
  { output=input;
    return true;
  }

  if (!input.IsOfType<GroupRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> >())
    return false;
  GroupRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational>& inputRep=
      input.GetValueNonConst<GroupRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> >();
  WeylGroupData* theWeylData = inputRep.ownerGroup->generators[0].owner;
  if (inputRep.ownerGroup->irreps.size<inputRep.ownerGroup->ConjugacyClassCount() && theWeylData->theGroup.ComputeIrreducibleRepresentationsWithFormulas)
    theWeylData->theGroup.ComputeIrreducibleRepresentationsWithFormulas(theWeylData->theGroup);
  if (inputRep.ownerGroup->irreps.size<inputRep.ownerGroup->ConjugacyClassCount())
  { theWeylData->ComputeInitialIrreps();
    theWeylData->theGroup.ComputeIrreducibleRepresentationsTodorsVersion();
  }
  VirtualRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> outputRep;
  outputRep.AssignRep(inputRep);
  return output.AssignValue(outputRep, theCommands);
}

