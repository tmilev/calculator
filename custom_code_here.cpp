#include "polyhedra.h"
// This file is meant to be for people to modify if they do not want to modify the main files polyhedra.cpp or polyhedra.h
// The reason I would recommend that is because the file polyhedra.cpp compiles very slowly (around 30 seconds), so small modifications
// take a long time to check. Using a separate file and linking it to the GUI is the solution I chose (may not be the best, see below for discussion).
// If the code you write here reaches the mature phase where you have realized all functions you think it should have
// and you are generally satisfied with it, simply cut & paste it in the main files (the class declarations in polyhedra.h and the implementation in polyhedra.cpp).

//Discussion: other options for speeding up the compilation that I have considered.
//1. Cut up the main files polyhedra.h and polyhedra.cpp into small quick-to-compile pieces. This might be the best solution in the long run. However, I do not want to
//   do it just yet, because 1) I am not sure what should those parts be - it is not yet clear in my head how to partition the code in conceptually distinct pieces
//   2) this would certainly create additional maintainance work 3) this will increase the learning curve for a person wanting to just use the program and wanting to eventually
//   modify some tiny bit and 4) I got an advice on the c++ forum www.cplusplus.com that partitioning the .h file will eventually lead to slower compile times,
//   especially with the massive use of templates that I do. Therefore, such a partitioning should not be done before the code reaches
//   greater maturity (see also point (1) ).
//2. Use precompiled headers or some other voodoo. I am tocally against that. Those are compiler specific, will require me to learn extra unnecessary info which might
//    be out of date in a few years, and will make even higher entry learning curve for another to join the project. This is bad.
//    I should mention in favor of Microsoft that their IDE does recompile very quickly small modifications of the file polyhedra.cpp. I believe it does so by
//    keeping a database of your recently changed code, and recompiling only the recently changed pieces. Hats off to Microsoft for doing that completely programmatically,
//    and not bothering the programmer with stupid things such as how to set up precompiled headers.
//
// To whomever might be reading this (if anyone): happy hacking and I hope you find my code useful, that it didn't cause you many headaches, and that you
// did something useful with it! Cheers!

void slTwo::ElementToStringModuleDecomposition(bool useLatex, bool useHtml, std::string& output)
{ std::stringstream out;
  if (useLatex)
    out << "$";
  for (int i=0; i<this->highestWeights.size; i++)
  { if (this->multiplicitiesHighestWeights.TheObjects[i]>1)
      out << this->multiplicitiesHighestWeights.TheObjects[i];
    out << "V_{" << this->highestWeights.TheObjects[i] << "}";
    if (i!=this->highestWeights.size-1)
      out << "+";
  }
  if (useLatex)
    out << "$";
  output=out.str();
}

void slTwo::ElementToStringModuleDecompositionMinimalContainingRegularSAs(bool useLatex, bool useHtml, SltwoSubalgebras& owner, std::string& output)
{ std::stringstream out;
  std::string tempS;
  if (useLatex)
    out<<"$";
  if (useHtml)
  { out <<"<table><tr><td align=\"center\">Char.</td>";
    for (int i=0; i<this->IndicesMinimalContainingRootSA.size; i++)
    { rootSubalgebra& theSA= owner.theRootSAs.TheObjects[this->IndicesMinimalContainingRootSA.TheObjects[i]];
      CGIspecificRoutines::clearDollarSigns(theSA.theDynkinDiagram.DebugString, tempS);
      out << "<td align=\"center\">Decomp. " << tempS <<"</td>";
    }
    out << "</tr>\n";
  }
  out << "<tr><td align=\"center\"> " << this->hCharacteristic.ElementToString() << "</td>";
  for (int k=0; k<this->IndicesMinimalContainingRootSA.size; k++)
  { rootSubalgebra& theSA= owner.theRootSAs.TheObjects[this->IndicesMinimalContainingRootSA.TheObjects[k]];
    CGIspecificRoutines::clearDollarSigns(theSA.theDynkinDiagram.DebugString, tempS);
    if (useHtml)
      out <<"<td align=\"center\">";
    for (int i=0; i<this->HighestWeightsDecompositionMinimalContainingRootSA.TheObjects[k].size; i++)
    { if (this->MultiplicitiesDecompositionMinimalContainingRootSA.TheObjects[k].TheObjects[i]>1)
        out << this->MultiplicitiesDecompositionMinimalContainingRootSA.TheObjects[k].TheObjects[i];
      out << "V_{"<<this->HighestWeightsDecompositionMinimalContainingRootSA.TheObjects[k].TheObjects[i]<<"}";
      if (i!=this->HighestWeightsDecompositionMinimalContainingRootSA.TheObjects[k].size-1)
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

void slTwo::ElementToHtmlCreateFormulaOutputReference(const std::string& formulaTex, std::stringstream& output, bool usePNG, bool useHtml, SltwoSubalgebras& container, std::string* physicalPath, std::string* htmlPathServer)
{ if (!usePNG)
  { output << formulaTex;
    //if (useHtml)
      //output<<"\n<br>\n";
    return;
  }
  std::stringstream tempStream;
  container.texFileNamesForPNG.SetSizeExpandOnTopNoObjectInit(container.texFileNamesForPNG.size+1);
  container.texStringsEachFile.SetSizeExpandOnTopNoObjectInit(container.texFileNamesForPNG.size);
  (*container.texStringsEachFile.LastObject())=formulaTex;
  tempStream << (*physicalPath) << "fla";
  tempStream << container.texFileNamesForPNG.size << ".tex";
  container.texFileNamesForPNG.TheObjects[container.texFileNamesForPNG.size-1]=tempStream.str();
  output << "<img src=\"" << (*htmlPathServer) << "fla" << container.texFileNamesForPNG.size << ".png\">";
  if (useHtml)
    output << "\n<br>\n";
}

void slTwo::ElementToString(std::string& output, GlobalVariables& theGlobalVariables, SltwoSubalgebras& container, int indexInContainer, bool useLatex, bool useHtml, bool usePNG, std::string* physicalPath, std::string* htmlPathServer)
{ std::stringstream out;  std::string tempS;
  this->hCharacteristic.ElementToString(tempS);
  out << "<a name=\"sl2index" << indexInContainer << "\">h-characteristic: " <<  tempS << "</a>";
  this->preferredAmbientSimpleBasis.ElementToString(tempS, false, false, false);
  if (physicalPath==0 || htmlPathServer==0)
  { usePNG=false;
    useHtml=false;
  }
  if (useHtml)
    out << "<br>";
  out << "\nSimple basis ambient algebra w.r.t defining h: " << tempS;
  roots tempRoots;
  MatrixLargeRational tempMat;
  if (useHtml)
    out << "<br>";
  if (this->IndicesContainingRootSAs.size>1)
  { out << "Number of containing regular semisimple subalgebras: " << this->IndicesContainingRootSAs.size;
    if (useHtml)
    out << "<br>";
  }
  for (int i=0; i<this->IndicesContainingRootSAs.size; i++)
  { out << "\nContaining regular semisimple subalgebra number " << i+1 << ": ";
    rootSubalgebra& currentSA= container.theRootSAs.TheObjects[this->IndicesContainingRootSAs.TheObjects[i]];
    if (useHtml)
    { out << "<a href=\"" << (*htmlPathServer) << "../rootHtml_rootSA" << this->IndicesContainingRootSAs.TheObjects[i] << ".html\">";
      currentSA.theDynkinDiagram.ElementToString(tempS, true);
      CGIspecificRoutines::clearDollarSigns(tempS, tempS);
    }
    currentSA.theDynkinDiagram.GetSimpleBasisInBourbakiOrder(tempRoots);
    out << tempS;
    if(useHtml)
      out << "</a>";
    tempRoots.ElementToString(tempS, useLatex, useHtml, false);
    if (useHtml)
      out << "<br>";
    out << "\nSimple basis subalgebra: " << tempS;
    currentSA.theDynkinDiagram.GetKillingFormMatrixUseBourbakiOrder(tempMat, this->owner->theWeyl);
    if (!usePNG)
      tempMat.ElementToString(tempS, useHtml, useLatex);
    else
      tempMat.ElementToString(tempS, false, true);
    if (useHtml)
      out << "<br>";
    out << "\nSymmetric Cartan matrix in Bourbaki order:\n";
    if (useHtml)
    { out << "<br>";
      this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
    } else
      out << tempS;
  }
  out << "\nsl(2)-module decomposition of the ambient Lie algebra: ";
  this->ElementToStringModuleDecomposition(useLatex || usePNG, useHtml, tempS);
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  container.IndicesSl2decompositionFlas.SetSizeExpandOnTopNoObjectInit(container.size);
  container.IndicesSl2decompositionFlas.TheObjects[indexInContainer]=container.texFileNamesForPNG.size-1;
  out << "\nThe below list one possible realization of the sl(2) subalgebra.";
  if (useHtml)
    out <<"\n<br>\n";
  std::stringstream tempStreamH, tempStreamE, tempStreamF, tempStreamHE, tempStreamHF, tempStreamEF;
  this->theH.ElementToString(tempS, *this->owner, useHtml, useLatex);
  tempStreamH<<"\n$h=$ $" <<tempS<<"$";
  tempS= tempStreamH.str();
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  this->theE.ElementToString(tempS, *this->owner, useHtml, useLatex);
  tempStreamE<<"\n$e=$ $" <<tempS<<"$";
  tempS= tempStreamE.str();
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  this->theF.ElementToString(tempS, *this->owner, useHtml, useLatex);
  tempStreamF <<"\n$f=$ $" <<tempS<<"$";
  tempS= tempStreamF.str();
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  out <<"\n\nThe below are the Lie brackets of the above elements. Printed for debugging.";
  if (useHtml)
    out <<"\n<br>\n";
  this->bufferEbracketF.ElementToString(tempS, *this->owner, useHtml, useLatex);
  tempStreamEF<< "\n$[e, f]=$ $" <<tempS <<"$";
  tempS= tempStreamEF.str();
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  this->bufferHbracketE.ElementToString(tempS, *this->owner, useHtml, useLatex);
  tempStreamHE<< "\n$[h, e]=$ $" <<tempS <<"$";
  tempS= tempStreamHE.str();
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  this->bufferHbracketF.ElementToString(tempS, *this->owner, useHtml, useLatex);
  tempStreamHF<< "\n$[h, f]=$ $" <<tempS <<"$";
  tempS= tempStreamHF.str();
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  //this->theSystemMatrixForm.ElementToString(tempS);
  //out <<"\nSystem matrix form we try to solve:\n"<< tempS;
  //this->theSystemColumnVector.ElementToString(tempS);
  //out <<"\nColumn vector of the system:\n"<<tempS;
  std::stringstream tempStreamActual;
  for (int i=0; i<this->theSystemToBeSolved.size; i++)
  { this->theSystemToBeSolved.TheObjects[i].ElementToString(tempS);
    if (tempS=="")
    { if (useLatex || usePNG)
        tempStreamActual<<"~\\\\";
      else
        tempStreamActual<<"\n\n";
    }
    else
      tempStreamActual<<"\\noindent \\begin{eqnarray*}&&"<< tempS<<"=0\\end{eqnarray*}\n\n";
  }
  out << "\nThe system we need to solve:\n";
  if (useHtml)
    out <<"\n<br>\n";
  tempS= tempStreamActual.str();
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  output = out.str();
}

void slTwo::ElementToHtml(std::string& filePath)
{ std::fstream theFile;
  std::string theFileName=filePath;
  theFileName.append("theSlTwo.txt");
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(theFile, filePath, false, true, false);
}

void SemisimpleLieAlgebra::FindSl2Subalgebras(SltwoSubalgebras& output, GlobalVariables& theGlobalVariables)
{ output.theRootSAs.GenerateAllReductiveRootSubalgebrasUpToIsomorphism(theGlobalVariables, true, true);
  //output.theRootSAs.ComputeDebugString(false, false, false, 0, 0, theGlobalVariables);
  output.IndicesSl2sContainedInRootSA.SetSizeExpandOnTopNoObjectInit(output.theRootSAs.size);
  output.IndicesSl2sContainedInRootSA.MakeActualSizeAtLeastExpandOnTop(output.theRootSAs.size*2);
  for (int i=0; i<output.IndicesSl2sContainedInRootSA.size; i++)
    output.IndicesSl2sContainedInRootSA.TheObjects[i].size=0;
  theGlobalVariables.theIndicatorVariables.StatusString1=output.theRootSAs.DebugString;
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.FeedIndicatorWindow(theGlobalVariables.theIndicatorVariables);
  for (int i=0; i<output.theRootSAs.size-1; i++)
    output.theRootSAs.TheObjects[i].GetSsl2SubalgebrasAppendListNoRepetition(output, i, theGlobalVariables, *this);
  for (int i=0; i<output.size; i++)
  { slTwo& theSl2= output.TheObjects[i];
    theSl2.IndicesMinimalContainingRootSA.MakeActualSizeAtLeastExpandOnTop(theSl2.IndicesContainingRootSAs.size);
    theSl2.IndicesMinimalContainingRootSA.size=0;
    for (int j=0; j<theSl2.IndicesContainingRootSAs.size; j++)
    { bool isMinimalContaining=true;
//      rootSubalgebra& currentRootSA = output.theRootSAs.TheObjects[];
      for (int k=0; k<theSl2.IndicesContainingRootSAs.size; k++)
      { rootSubalgebra& theOtherRootSA = output.theRootSAs.TheObjects[theSl2.IndicesContainingRootSAs.TheObjects[k]];
        if (theOtherRootSA.indicesSubalgebrasContainingK.ContainsObject(theSl2.IndicesContainingRootSAs.TheObjects[j]))
        { isMinimalContaining=false;
          break;
        }
      }
      if (isMinimalContaining)
        theSl2.IndicesMinimalContainingRootSA.AddOnTopNoRepetition(theSl2.IndicesContainingRootSAs.TheObjects[j]);
    }
    output.ComputeModuleDecompositionsOfAmbientLieAlgebra(theGlobalVariables);
  }
  //tempRootSA.GetSsl2Subalgebras(tempSl2s, theGlobalVariables, *this);
}

void SemisimpleLieAlgebra::FindSl2Subalgebras(SltwoSubalgebras& output, char WeylLetter, int WeylRank, GlobalVariables& theGlobalVariables)
{ output.theRootSAs.AmbientWeyl.MakeArbitrary(WeylLetter, WeylRank);
  this->FindSl2Subalgebras(output, theGlobalVariables);
}

void rootSubalgebra::GetSsl2SubalgebrasAppendListNoRepetition(SltwoSubalgebras& output, int indexInContainer, GlobalVariables& theGlobalVariables, SemisimpleLieAlgebra& theLieAlgebra)
{ //reference: Dynkin, semisimple Lie algebras of simple lie algebras, theorems 10.1-10.4
  Selection theRootsWithZeroCharacteristic;
  roots RootsWithCharacteristic2;
  RootsWithCharacteristic2.MakeActualSizeAtLeastExpandOnTop(this->PositiveRootsK.size);
  DynkinDiagramRootSubalgebra tempDiagram;
  int theRelativeDimension = this->SimpleBasisK.size;
  theRootsWithZeroCharacteristic.init(theRelativeDimension);
  MatrixLargeRational InvertedRelativeKillingForm;
  InvertedRelativeKillingForm.init(theRelativeDimension, theRelativeDimension);
  for (int k=0; k<theRelativeDimension; k++)
    for (int j=0; j<theRelativeDimension; j++)
      InvertedRelativeKillingForm.elements[k][j]=this->AmbientWeyl.RootScalarCartanRoot(this->SimpleBasisK.TheObjects[k], this->SimpleBasisK.TheObjects[j]);
  InvertedRelativeKillingForm.Invert(theGlobalVariables);
  theLieAlgebra.ComputeChevalleyConstants(this->AmbientWeyl, theGlobalVariables);
  //int relativeDimension =
  int numCycles= MathRoutines::TwoToTheNth(theRootsWithZeroCharacteristic.MaxSize);
  roots tempRoots;
  tempRoots.MakeActualSizeAtLeastExpandOnTop(theRootsWithZeroCharacteristic.MaxSize);
  roots relativeRootSystem;
  this->PositiveRootsK.GetCoordsInBasis(this->SimpleBasisK, relativeRootSystem, theGlobalVariables);
  slTwo theSl2;
  theSl2.owner = &theLieAlgebra;
  for (int i=0; i<numCycles; i++, theRootsWithZeroCharacteristic.incrementSelection())
  { tempRoots.size=0;
    for (int j=0; j<theRootsWithZeroCharacteristic.CardinalitySelection; j++)
      tempRoots.AddObjectOnTop(this->SimpleBasisK.TheObjects[theRootsWithZeroCharacteristic.elements[j]]);
    tempDiagram.ComputeDiagramTypeModifyInput(tempRoots, this->AmbientWeyl);
    int theSlack=0;
    RootsWithCharacteristic2.size=0;
    for (int j=0; j<relativeRootSystem.size; j++)
    { Rational DimTwoSpace=0;
      for (int k=0; k<theRelativeDimension; k++)
        if (!theRootsWithZeroCharacteristic.selected[k])
        { DimTwoSpace+=relativeRootSystem.TheObjects[j].TheObjects[k];
          if (DimTwoSpace>1)
            break;
        }
      if (DimTwoSpace==1)
      { theSlack++;
        RootsWithCharacteristic2.AddObjectOnTop(this->PositiveRootsK.TheObjects[j]);
      }
    }
    int theDynkinEpsilon = tempDiagram.NumRootsGeneratedByDiagram() + theRelativeDimension - theSlack;
    //if Dynkin's epsilon is not zero the subalgebra cannot be an S sl(2) subalgebra.
    //otherwise, as far as I understand, it always is (but generators still have to be found)
    //this is done in the below code.
    theRootsWithZeroCharacteristic.ComputeDebugString();
    if (theDynkinEpsilon==0)
    { root tempRoot, tempRoot2;
      tempRoot.MakeZero(theRelativeDimension);
      for (int k=0; k<theRelativeDimension; k++)
        if(!theRootsWithZeroCharacteristic.selected[k])
          tempRoot.TheObjects[k]=2;
      InvertedRelativeKillingForm.ActOnAroot(tempRoot, tempRoot2);
      theSl2.theH.Nullify(theLieAlgebra);
      theSl2.theH.Hcomponent.MakeZero(this->AmbientWeyl.CartanSymmetric.NumRows);
      for(int j=0; j<theRelativeDimension; j++)
        theSl2.theH.Hcomponent+= this->SimpleBasisK.TheObjects[j]*tempRoot2.TheObjects[j];
      theSl2.theE.Nullify(theLieAlgebra);
      theSl2.theF.Nullify(theLieAlgebra);
      //theSl2.ComputeDebugString(false, false, theGlobalVariables);
      if(theLieAlgebra.AttemptExtendingHEtoHEFWRTSubalgebra(RootsWithCharacteristic2, relativeRootSystem, theRootsWithZeroCharacteristic, this->SimpleBasisK, theSl2.theH.Hcomponent, theSl2.theE, theSl2.theF, theSl2.theSystemMatrixForm, theSl2.theSystemToBeSolved, theSl2.theSystemColumnVector, theGlobalVariables))
      { int indexIsoSl2;
        theSl2.MakeReportPrecomputations(theGlobalVariables, output, output.size, indexInContainer, *this);
        if(output.ContainsSl2WithGivenHCharacteristic(theSl2.hCharacteristic, &indexIsoSl2))
        { output.TheObjects[indexIsoSl2].IndicesContainingRootSAs.AddObjectOnTop(indexInContainer);
          output.IndicesSl2sContainedInRootSA.TheObjects[indexInContainer].AddObjectOnTop(indexIsoSl2);
        }
        else
        { output.IndicesSl2sContainedInRootSA.TheObjects[indexInContainer].AddObjectOnTop(output.size);
          output.AddObjectOnTopHash(theSl2);
        }
      }
      else
        output.BadHCharacteristics.AddObjectOnTop(theSl2.theH.Hcomponent);
    }
  }
}

bool SemisimpleLieAlgebra:: AttemptExtendingHEtoHEFWRTSubalgebra(roots& RootsWithCharacteristic2, roots& relativeRootSystem, Selection& theZeroCharacteristics, roots& simpleBasisSA, root& h, ElementSimpleLieAlgebra& outputE, ElementSimpleLieAlgebra& outputF, MatrixLargeRational& outputMatrixSystemToBeSolved, PolynomialsRationalCoeff& outputSystemToBeSolved, MatrixLargeRational& outputSystemColumnVector, GlobalVariables& theGlobalVariables)
{ if (theZeroCharacteristics.CardinalitySelection== theZeroCharacteristics.MaxSize)
    return false;
  roots SelectedExtraPositiveRoots;
  roots rootsInPlay;
  rootsInPlay.size=0;
  SelectedExtraPositiveRoots.size=0;
  int theRelativeDimension = simpleBasisSA.size;
//  int theDimension= this->theWeyl.CartanSymmetric.NumRows;
  assert(theRelativeDimension==theZeroCharacteristics.MaxSize);
  root tempRoot, tempRoot2;
  //format. We are looking for an sl(2) for which e= a_0 g^\alpha_0+\dots a_kg^\alpha_k, and f=b_0 g^{-\alpha_0}+... +b_kg^{-\alpha_k}
  //where the first \alpha's are ordered as in rootsInPlay.
  //Those are ordered as such: first come  the simple roots of characteristic 2, and the last \alpha's are the members of SelectedExtraPositiveRoots
  //(i.e. roots equal to the sum of one simple root of characteristic 2 a simple root of characteristic 0).
  // Then the first k variables of the polynomials below will be a_0, ..., a_k., and the last k variables will be the b_i's
  // the l^th polynomial will correspond to the coefficient of g^\alpha_{l/2}, where l/2 is the index of the root of SelectedExtraPositiveRoots, if l is even, and to the
  // coefficient of  g^{-\alpha_{(l+1)/2}} otherwise
  for (int i=0; i<theRelativeDimension; i++)
    if (!theZeroCharacteristics.selected[i])
      rootsInPlay.AddObjectOnTop(simpleBasisSA.TheObjects[i]);
    else
      for (int j=0; j<theRelativeDimension; j++)
        if (!theZeroCharacteristics.selected[j])
        { tempRoot= simpleBasisSA.TheObjects[i]+simpleBasisSA.TheObjects[j];
          if (this->theWeyl.IsARoot(tempRoot))
            SelectedExtraPositiveRoots.AddObjectOnTop(tempRoot);
        }
  SelectedExtraPositiveRoots.size=0;
  for (int i=0; i<RootsWithCharacteristic2.size; i++)
    if (!simpleBasisSA.ContainsObject(RootsWithCharacteristic2.TheObjects[i]))
      SelectedExtraPositiveRoots.AddObjectOnTop(RootsWithCharacteristic2.TheObjects[i]);
  int numRootsChar2 = rootsInPlay.size;
  rootsInPlay.AddListOnTop(SelectedExtraPositiveRoots);
  int halfNumberVariables = rootsInPlay.size;
  int numberVariables = halfNumberVariables*2;
  Monomial<Rational> tempM;
  tempM.init((short)numberVariables);
  MatrixLargeRational coeffsF;
  coeffsF.init(1, halfNumberVariables);
  for (int i=0; i<numRootsChar2; i++)
    coeffsF.elements[0][i]=i+1; //(i%2==0)? 1: 2;
  for (int i=numRootsChar2; i<coeffsF.NumCols; i++)
    coeffsF.elements[0][i]=i+1;
  this->initHEFSystemFromECoeffs(theRelativeDimension, theZeroCharacteristics, rootsInPlay, simpleBasisSA, SelectedExtraPositiveRoots, numberVariables, numRootsChar2, halfNumberVariables, h, coeffsF, outputMatrixSystemToBeSolved, outputSystemColumnVector, outputSystemToBeSolved);
  MatrixLargeRational tempMat, tempMatColumn, tempMatResult;
  tempMat.Assign(outputMatrixSystemToBeSolved);
  tempMatColumn.Assign(outputSystemColumnVector);
  outputF.Nullify(*this);
  outputE.Nullify(*this);
//  if(MatrixLargeRational::Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(outputMatrixSystemToBeSolved, outputSystemColumnVector, tempMatResult))
  if(MatrixLargeRational::Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(tempMat, tempMatColumn, tempMatResult))
  { for (int i=0; i<rootsInPlay.size; i++)
    { outputF.SetCoefficient(-rootsInPlay.TheObjects[i], coeffsF.elements[0][i], *this);
      outputE.SetCoefficient(rootsInPlay.TheObjects[i], tempMatResult.elements[i][0], *this);
    }
    return true;
  }
  return true;
}

void SemisimpleLieAlgebra::initHEFSystemFromECoeffs(int theRelativeDimension, Selection& theZeroCharacteristics, roots& rootsInPlay, roots& simpleBasisSA, roots& SelectedExtraPositiveRoots, int numberVariables, int numRootsChar2, int halfNumberVariables, root& targetH, MatrixLargeRational& inputFCoeffs, MatrixLargeRational& outputMatrixSystemToBeSolved, MatrixLargeRational& outputSystemColumnVector, PolynomialsRationalCoeff& outputSystemToBeSolved)
{ root tempRoot;
  static int ProblemCounter=0;
  ProblemCounter++;
  Monomial<Rational> tempM;
  Rational tempRat;
  hashedRoots RootSpacesThatNeedToBeKilled;
  RootSpacesThatNeedToBeKilled.ClearTheObjects();
  //List<int> IndicesEquationsByRootSpace;
  RootSpacesThatNeedToBeKilled.MakeActualSizeAtLeastExpandOnTop(this->theWeyl.RootSystem.size);
//  IndicesEquationsByRootSpace.MakeActualSizeAtLeastExpandOnTop(this->theWeyl.RootSystem.size);
  outputSystemToBeSolved.size=0;
  outputMatrixSystemToBeSolved.init(0, numberVariables);
  //outputSystemToBeSolved.ComputeDebugString();
  for (int i=0; i<rootsInPlay.size; i++)
    for (int j=0; j<rootsInPlay.size; j++)
    { tempRoot= rootsInPlay.TheObjects[i]-rootsInPlay.TheObjects[j];
      if (this->theWeyl.IsARoot(tempRoot))
      { int indexEquation= RootSpacesThatNeedToBeKilled.IndexOfObjectHash(tempRoot);
        if (indexEquation==-1)
        { RootSpacesThatNeedToBeKilled.AddObjectOnTopHash(tempRoot);
          indexEquation=outputSystemToBeSolved.size;
//          IndicesEquationsByRootSpace.AddObjectOnTop(indexEquation);
          outputSystemToBeSolved.SetSizeExpandOnTopNoObjectInit(outputSystemToBeSolved.size+1);
          outputSystemToBeSolved.LastObject()->Nullify((short)numberVariables);
        }
        tempM.init((short)numberVariables);
        tempM.degrees[i]=1;
        tempM.degrees[j+halfNumberVariables]=1;
        tempM.Coefficient= this->GetConstant(rootsInPlay.TheObjects[i], -rootsInPlay.TheObjects[j]);
        outputSystemToBeSolved.TheObjects[indexEquation].AddMonomial(tempM);
        //outputSystemToBeSolved.ComputeDebugString();
      }
    }
  int oldSize=outputSystemToBeSolved.size;
  outputSystemToBeSolved.SetSizeExpandOnTopNoObjectInit(oldSize+this->theWeyl.CartanSymmetric.NumRows);
  for(int i=oldSize; i<outputSystemToBeSolved.size; i++)
    outputSystemToBeSolved.TheObjects[i].Nullify((short)numberVariables);
  //outputSystemToBeSolved.ComputeDebugString();
  for (int i=0; i<rootsInPlay.size; i++)
  { assert(rootsInPlay.size==halfNumberVariables);
    this->GetConstantOrHElement(rootsInPlay.TheObjects[i], -rootsInPlay.TheObjects[i], tempRat, tempRoot);
    for (int j=0; j<this->theWeyl.CartanSymmetric.NumRows; j++)
    { tempM.init((short)numberVariables);
      tempM.degrees[i]=1;
      tempM.degrees[i+halfNumberVariables]=1;
      tempM.Coefficient= tempRoot.TheObjects[j];
      outputSystemToBeSolved.TheObjects[j+oldSize].AddMonomial(tempM);
    }
  }
  for (int i=0; i<this->theWeyl.CartanSymmetric.NumRows; i++)
    outputSystemToBeSolved.TheObjects[i+oldSize].AddConstant(targetH.TheObjects[i]*(-1));
  outputMatrixSystemToBeSolved.init(outputSystemToBeSolved.size, halfNumberVariables);
  outputSystemColumnVector.init(outputSystemToBeSolved.size, 1);
  outputMatrixSystemToBeSolved.NullifyAll();
  outputSystemColumnVector.NullifyAll();
  for (int i=0; i<outputSystemToBeSolved.size; i++)
    for (int j=0; j<outputSystemToBeSolved.TheObjects[i].size; j++)
    { int lowerIndex=-1; int higherIndex=-1;
      Monomial<Rational>& theMonomial= outputSystemToBeSolved.TheObjects[i].TheObjects[j];
      for (int k=0; k<numberVariables; k++)
        if (theMonomial.degrees[k]==1)
        { if (k<halfNumberVariables)
            lowerIndex=k;
          else
          { higherIndex=k;
            break;
          }
        }
      if (lowerIndex==-1)
        outputSystemColumnVector.elements[i][0]= theMonomial.Coefficient*(-1);
      else
        outputMatrixSystemToBeSolved.elements[i][lowerIndex]=theMonomial.Coefficient* inputFCoeffs.elements[0][higherIndex-halfNumberVariables];
    }
//  outputSystemToBeSolved.ComputeDebugString();
}

bool SltwoSubalgebras::ContainsSl2WithGivenH(root& theH, int* outputIndex)
{ if (outputIndex!=0)
    *outputIndex=-1;
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].theH.Hcomponent==theH)
    { if (outputIndex!=0)
        *outputIndex=i;
      return true;
    }
  return false;
}

bool SltwoSubalgebras::ContainsSl2WithGivenHCharacteristic(root& theHCharacteristic, int* outputIndex)
{ if (outputIndex!=0)
    *outputIndex=-1;
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].hCharacteristic==theHCharacteristic)
    { if (outputIndex!=0)
        *outputIndex=i;
      return true;
    }
  return false;
}

void slTwo::MakeReportPrecomputations(GlobalVariables& theGlobalVariables, SltwoSubalgebras& container, int indexInContainer, int indexMinimalContainingRegularSA, rootSubalgebra& MinimalContainingRegularSubalgebra)
{ int theDimension=this->owner->theWeyl.CartanSymmetric.NumRows;
  this->IndicesContainingRootSAs.size=0;
  roots tempRoots;
  tempRoots.CopyFromBase(MinimalContainingRegularSubalgebra.SimpleBasisK);
  this->owner->theWeyl.TransformToSimpleBasisGeneratorsWRTh(tempRoots, this->theH.Hcomponent);
  DynkinDiagramRootSubalgebra theDiagram;
  theDiagram.ComputeDiagramTypeKeepInput(tempRoots, this->owner->theWeyl);
  theDiagram.GetSimpleBasisInBourbakiOrder(tempRoots);
  this->IndicesContainingRootSAs.AddObjectOnTop(indexMinimalContainingRegularSA);
  tempRoots.MakeEiBasis(theDimension);
  this->owner->theWeyl.TransformToSimpleBasisGeneratorsWRTh(tempRoots, this->theH.Hcomponent);
  DynkinDiagramRootSubalgebra tempDiagram;
  tempDiagram.ComputeDiagramTypeKeepInput(tempRoots, this->owner->theWeyl);
  tempDiagram.GetSimpleBasisInBourbakiOrder(this->preferredAmbientSimpleBasis);
  this->hCharacteristic.SetSizeExpandOnTopLight(theDimension);
  for (int i=0; i<theDimension; i++)
    this->hCharacteristic.TheObjects[i]=this->owner->theWeyl.RootScalarCartanRoot(this->theH.Hcomponent, this->preferredAmbientSimpleBasis.TheObjects[i]);
  //this->hCharacteristic.ComputeDebugString();
  if (this->theE.NonZeroElements.MaxSize==this->owner->theWeyl.RootSystem.size && this->theF.NonZeroElements.MaxSize==this->owner->theWeyl.RootSystem.size && this->theH.NonZeroElements.MaxSize==this->owner->theWeyl.RootSystem.size)
  { this->owner->LieBracket(this->theE, this->theF, this->bufferEbracketF);
    this->owner->LieBracket(this->theH, this->theE, this->bufferHbracketE);
    this->owner->LieBracket(this->theH, this->theF, this->bufferHbracketF);
  }
  //theSl2.hCharacteristic.ComputeDebugString();
//  this->ComputeModuleDecomposition();
}

void WeylGroup::PerturbWeightToRegularWRTrootSystem(const root& inputH, root& output)
{ output.Assign(inputH);
  int indexFirstNonRegular;
  while(!this->IsRegular(output, &indexFirstNonRegular))
  { root& theBadRoot= this->RootSystem.TheObjects[indexFirstNonRegular];
    Rational maxMovement=0; Rational tempRat1, tempRat2, tempMaxMovement;
    for (int i=0; i<this->RootsOfBorel.size; i++)
    { this->RootScalarCartanRoot(theBadRoot, this->RootsOfBorel.TheObjects[i], tempRat1);
      this->RootScalarCartanRoot(output, this->RootsOfBorel.TheObjects[i], tempRat2);
      if ((!tempRat1.IsEqualToZero()) && (!tempRat2.IsEqualToZero()))
      { tempMaxMovement = tempRat2/tempRat1;
        tempMaxMovement.AssignAbsoluteValue();
        if ((tempMaxMovement< maxMovement) || maxMovement.IsEqualToZero())
          maxMovement = tempMaxMovement;
      }
    }
    int tempInt=2;
    if (this->RootScalarCartanRoot(theBadRoot, inputH).IsNegative())
      tempInt=-2;
    output+=theBadRoot*maxMovement/tempInt;
  }
}

bool WeylGroup::IsRegular(root& input, int* indexFirstPerpendicularRoot)
{ if (indexFirstPerpendicularRoot!=0)
    *indexFirstPerpendicularRoot=-1;
  for (int i=0; i<this->RootSystem.size; i++)
    if (this->RootScalarCartanRoot(input, this->RootSystem.TheObjects[i]).IsEqualToZero())
    { if (indexFirstPerpendicularRoot!=0)
        *indexFirstPerpendicularRoot=i;
      return false;
    }
  return true;
}

Rational DynkinDiagramRootSubalgebra::GetSizeCorrespondingWeylGroupByFormula()
{ Rational output=1;
  for (int i=0; i<this->SimpleBasesConnectedComponents.size; i++)
    output*=WeylGroup::GetSizeWeylByFormula(this->DynkinTypeStrings.TheObjects[i].at(1), this->SimpleBasesConnectedComponents.TheObjects[i].size);
  return output;
}

Rational WeylGroup::GetSizeWeylByFormula(char weylLetter, int theDim)
{ //Humphreys, Introduction to Lie algebras and representation theory(1980), page 66, Table 1
  Rational theOutput=1;
  if (weylLetter=='A')
    theOutput= Rational::Factorial(theDim+1);
  if (weylLetter=='B' || weylLetter=='C')
    theOutput= Rational::Factorial(theDim)*Rational::TwoToTheNth(theDim);
  if (weylLetter=='D')
    theOutput= Rational::Factorial(theDim)*Rational::TwoToTheNth(theDim-1);
  if (weylLetter=='E')
  { if (theDim==6)
      theOutput= 51840;
    if (theDim==7)
    { theOutput=1024;
      theOutput*=81*35;
    }
    if (theDim==8)
    { theOutput=1024*16;
      theOutput*=81*3;
      theOutput*=25*7;
    }
  }
  if (weylLetter=='F')
    theOutput=128*9;
  if (weylLetter=='G')
    theOutput=12;
  return theOutput;
}

void DynkinDiagramRootSubalgebra::GetSimpleBasisInBourbakiOrder(roots& output)
{ output.size=0;
  output.MakeActualSizeAtLeastExpandOnTop(this->RankTotal());
  for (int i=0; i<this->SimpleBasesConnectedComponents.size; i++)
    this->GetSimpleBasisInBourbakiOrderOneComponentAppend(output, i);
}

void DynkinDiagramRootSubalgebra::GetSimpleBasisInBourbakiOrderOneComponentAppend(roots& outputAppend, int index)
{ std::string& theString= this->DynkinTypeStrings.TheObjects[index];
  // the order implemented here I took from the atlas of lie groups (http://www.liegroups.org/dissemination/spherical/explorer/rootSystem.cgi)
  // which should be the Bourbaki order. The order is as follows:
  // type A all roots are from left to right (or the other way round, whichever is your orientation)
  // in type B first comes the long roots in the order they appear in the diagram;
  // in types C,
  // Types F and G there were some recent changes due to different order choice in different parts of the code.
  // The info for F and G needs to be updated!
  // in type D first comes the long string, with the end node with lowest index;
  // then come the two end one-root strings in any order.
  // in type E the order is as below
  //  2
  //13456(78)
  //(2 is connected to 4)
  //The format of this function is in accordance with WeylGroup::GetEpsilonMatrix
  assert(theString.size()>0);
  if (theString.at(1)=='A'|| theString.at(1)=='B' || theString.at(1)=='G')
    outputAppend.AddListOnTop(this->SimpleBasesConnectedComponents.TheObjects[index]);
  if (theString.at(1)=='C' || theString.at(1)=='F')
    for (int i=this->SimpleBasesConnectedComponents.TheObjects[index].size-1; i>=0; i--)
      outputAppend.AddObjectOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[i]);
  if (theString.at(1)=='D')
  { int componentRank=this->SimpleBasesConnectedComponents.TheObjects[index].size;
    for (int i=componentRank-3; i>=0; i--)
      outputAppend.AddObjectOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[i]);
    outputAppend.AddObjectOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[componentRank-2]);
    outputAppend.AddObjectOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[componentRank-1]);
  }
  if (theString.at(1)=='E')
  { int componentRank=this->SimpleBasesConnectedComponents.TheObjects[index].size;
    outputAppend.AddObjectOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[componentRank-2]);
    outputAppend.AddObjectOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[componentRank-1]);
    outputAppend.AddObjectOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[componentRank-3]);
    for (int i=0; i<componentRank-3; i++)
      outputAppend.AddObjectOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[i]);
  }
}

void DynkinDiagramRootSubalgebra::GetKillingFormMatrixUseBourbakiOrder(MatrixLargeRational& output, WeylGroup& theWeyl)
{ roots tempRoots;
  int theDimension= this->RankTotal();
  output.init(theDimension, theDimension);
  this->GetSimpleBasisInBourbakiOrder(tempRoots);
  for (int i=0; i<theDimension; i++)
    for (int j=0; j<theDimension; j++)
      output.elements[i][j]=theWeyl.RootScalarCartanRoot(tempRoots.TheObjects[i], tempRoots.TheObjects[j]);
}

void slTwo::ComputeModuleDecompositionAmbientLieAlgebra(GlobalVariables& theGlobalVariables)
{ this->ComputeModuleDecomposition(this->owner->theWeyl.RootsOfBorel, this->owner->theWeyl.CartanSymmetric.NumRows, this->highestWeights, this->multiplicitiesHighestWeights, this->weightSpaceDimensions, theGlobalVariables);
}

void slTwo::ComputeModuleDecompositionOfMinimalContainingRegularSAs(SltwoSubalgebras& owner, int IndexInOwner, GlobalVariables& theGlobalVariables)
{ this->MultiplicitiesDecompositionMinimalContainingRootSA.SetSizeExpandOnTopNoObjectInit(this->IndicesMinimalContainingRootSA.size);
  this->HighestWeightsDecompositionMinimalContainingRootSA.SetSizeExpandOnTopNoObjectInit(this->IndicesMinimalContainingRootSA.size);
  List<int> buffer;
  for (int i=0; i<this->IndicesMinimalContainingRootSA.size; i++)
  { rootSubalgebra& theSA= owner.theRootSAs.TheObjects[this->IndicesMinimalContainingRootSA.TheObjects[i]];
    this->ComputeModuleDecomposition(theSA.PositiveRootsK, theSA.SimpleBasisK.size, this->HighestWeightsDecompositionMinimalContainingRootSA.TheObjects[i], this->MultiplicitiesDecompositionMinimalContainingRootSA.TheObjects[i], buffer, theGlobalVariables);
  }
}

//The below code is related to sl(2) subalgebras of simple Lie algebras
void slTwo::ComputeModuleDecomposition(roots& positiveRootsContainingRegularSA, int dimensionContainingRegularSA, List<int>& outputHighestWeights, List<int>& outputMultiplicitiesHighestWeights, List<int>& outputWeightSpaceDimensions, GlobalVariables& theGlobalVariables)
{ int IndexZeroWeight=positiveRootsContainingRegularSA.size*2;
  outputWeightSpaceDimensions.initFillInObject(4*positiveRootsContainingRegularSA.size+1, 0);
  outputWeightSpaceDimensions.TheObjects[IndexZeroWeight]=dimensionContainingRegularSA;
  List<int> BufferHighestWeights;
  bool possible=true;
  Rational tempRat;
  roots tempRoots;
  positiveRootsContainingRegularSA.GetCoordsInBasis(this->preferredAmbientSimpleBasis, tempRoots, theGlobalVariables);
  for (int k=0; k<positiveRootsContainingRegularSA.size; k++)
  { root::RootScalarEuclideanRoot(this->hCharacteristic, tempRoots.TheObjects[k], tempRat);
    assert(tempRat.DenShort==1);
    if (tempRat.NumShort>positiveRootsContainingRegularSA.size*2)
    { possible=false;
      break;
    }
    outputWeightSpaceDimensions.TheObjects[IndexZeroWeight+tempRat.NumShort]++;
    outputWeightSpaceDimensions.TheObjects[IndexZeroWeight-tempRat.NumShort]++;
  }
  BufferHighestWeights.CopyFromBase(outputWeightSpaceDimensions);
  outputHighestWeights.MakeActualSizeAtLeastExpandOnTop(positiveRootsContainingRegularSA.size*2);
  outputMultiplicitiesHighestWeights.MakeActualSizeAtLeastExpandOnTop(positiveRootsContainingRegularSA.size*2);
  outputHighestWeights.size=0;
  outputMultiplicitiesHighestWeights.size=0;
//  this->hCharacteristic.ComputeDebugString();
  for (int j=BufferHighestWeights.size-1; j>=IndexZeroWeight; j--)
  { int topMult = BufferHighestWeights.TheObjects[j];
    if (topMult<0)
    { possible=false;
      break;
    }
    if (topMult>0)
    { outputHighestWeights.AddObjectOnTop(j-IndexZeroWeight);
      outputMultiplicitiesHighestWeights.AddObjectOnTop(topMult);
      if (j!=IndexZeroWeight)
        BufferHighestWeights.TheObjects[IndexZeroWeight*2-j]-=topMult;
      for (int k=j-2; k>=IndexZeroWeight; k-=2)
      { BufferHighestWeights.TheObjects[k]-=topMult;
        if (k!=IndexZeroWeight)
           BufferHighestWeights.TheObjects[IndexZeroWeight*2-k]-=topMult;
        assert(BufferHighestWeights.TheObjects[k]==BufferHighestWeights.TheObjects[IndexZeroWeight*2-k]);
        if(BufferHighestWeights.TheObjects[k]<0)
        { possible=false;
          break;
        }
      }
    }
  }
  assert(possible);
}

void SltwoSubalgebras::ElementToStringModuleDecompositionMinimalContainingRegularSAs(std::string& output, bool useLatex, bool useHtml)
{ std::string tempS; std::stringstream out;
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].ElementToStringModuleDecompositionMinimalContainingRegularSAs(useLatex, useHtml, *this, tempS);
    out << tempS;
    if (useHtml)
      out <<"\n<br>";
    out <<"\n";
  }
  output=out.str();
}

void SltwoSubalgebras::ElementToStringNoGenerators(std::string& output, GlobalVariables& theGlobalVariables, WeylGroup& theWeyl, bool useLatex, bool useHtml, bool usePNG, std::string* physicalPath, std::string* htmlPathServer)
{ std::string tempS; std::stringstream out;
  std::string tooltipHchar="Let h be in the Cartan s.a. Let \\alpha_1, ..., \\alpha_n be simple roots w.r.t. h. Then the h-characteristic is the n-tuple (\\alpha_1(h), ..., \\alpha_n(h))";
  std::string tooltipVDecomposition= "The sl(2) submodules of g are parametrized by their highest weight w.r.t. h. V_l is l+1 dimensional";
  std::string tooltipContainingRegular="A regular semisimple subalgebra might contain an sl(2) such that the sl(2) has no centralizer in the regular semisimple subalgebra, but the regular semisimple subalgebra might fail to be minimal containing. This happens when another minimal containing regular semisimple subalgebra of equal rank nests as a root subalgebra in the containing SA. See Dynkin, Semisimple Lie subalgebras of semisimple Lie algebras, remark before Theorem 10.4.";
  std::string tooltipHvalue="The actual realization of h. The coordinates of h are given with respect to the fixed original simple basis. Note that the characteristic of h is given *with respect to another basis* (namely, with respect to an h-positive simple basis). I will fix this in the future (email me if you want that done sooner).";
  out <<"Number of sl(2) subalgebras "<< this->size<<"\n";
  if (this->IndicesSl2decompositionFlas.size < this->size)
    usePNG = false;
  if(useHtml)
    out << "<br><br><table><tr><td style=\"padding-right:20px\">" << CGIspecificRoutines::ElementToStringTooltip("Characteristic", tooltipHchar)  << "</td><td align=\"center\" title=\""<<tooltipHvalue<<"\"> h</td><td style=\"padding-left:20px\" title=\""<<tooltipVDecomposition<<"\"> Decomposition of ambient Lie algebra</td> <td>Minimal containing regular semisimple SAs</td><td title=\""<<tooltipContainingRegular<<"\">Containing regular semisimple SAs in which the sl(2) has no centralizer</td> </tr>";
  if (this->BadHCharacteristics.size>0)
  { if (useHtml)
      out << "<tr><td>Bad values of h</td><td>";
    this->BadHCharacteristics.ElementToString(tempS);
    out << tempS;
    if (useHtml)
      out << "</td></tr>";
  }
  for (int i=0; i<this->size; i++)
  { slTwo& theSl2= this->TheObjects[i];
    if (useHtml)
      out << "<tr><td style=\"padding-right:20px\"><a href=\"./sl2s.html#sl2index" << i << "\"title=\"" << tooltipHchar <<"\" >";
    out << theSl2.hCharacteristic.ElementToString();
    if (useHtml)
      out << "</a></td><td title=\"" << tooltipHvalue << "\">";
    out << theSl2.theH.Hcomponent.ElementToString();
    if (useHtml)
      out << "</td><td style=\"padding-left:20px\" title=\"" << tooltipVDecomposition << "\">";
    if (useHtml && usePNG)
      out << "<img src=\"./fla"<< this->IndicesSl2decompositionFlas.TheObjects[i]+1 <<  ".png\"></td><td>";
    else
    { theSl2.ElementToStringModuleDecomposition(useLatex, useHtml, tempS);
      out << tempS;
      if (useHtml)
        out << "</td><td>";
    }
    for (int j=0; j<theSl2.IndicesMinimalContainingRootSA.size; j++)
    { rootSubalgebra& currentSA= this->theRootSAs.TheObjects[theSl2.IndicesMinimalContainingRootSA.TheObjects[j]];
      CGIspecificRoutines::clearDollarSigns(currentSA.theDynkinDiagram.DebugString, tempS);
      out << "<a href=\"../rootHtml_rootSA" << theSl2.IndicesMinimalContainingRootSA.TheObjects[j] << ".html\">" << tempS << "</a>" << ";  ";
    }
    if (useHtml)
      out << "</td><td title=\"" << tooltipContainingRegular << "\">";
    for (int j=0; j<theSl2.IndicesContainingRootSAs.size; j++)
    { rootSubalgebra& currentSA= this->theRootSAs.TheObjects[theSl2.IndicesContainingRootSAs.TheObjects[j]];
      CGIspecificRoutines::clearDollarSigns(currentSA.theDynkinDiagram.DebugString, tempS);
      out << "<a href=\"../rootHtml_rootSA" << theSl2.IndicesContainingRootSAs.TheObjects[j] << ".html\">" <<tempS<< "</a>"<<";  ";
    }
    if (useHtml)
      out <<"</td></tr>\n";
  }
  if (useHtml)
    out << "</table><HR width=\"100%\">";
  output= out.str();
}

void SltwoSubalgebras::ElementToString(std::string& output, GlobalVariables& theGlobalVariables, WeylGroup& theWeyl, bool useLatex, bool useHtml, bool usePNG, std::string* physicalPath, std::string* htmlPathServer)
{ std::string tempS; std::stringstream out; std::stringstream body;
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].ElementToString(tempS, theGlobalVariables, *this, i, useLatex, useHtml, usePNG, physicalPath, htmlPathServer);
  //  body<< "Index "<< i<<": ";
    if(useHtml)
      body<<"<br>";
    body<<tempS;
    if(useHtml)
      body<<"<HR width=\"100%\">";
  }
  if(useHtml)
    out<<"<br>";
  this->ElementToStringNoGenerators(tempS, theGlobalVariables, theWeyl, useLatex, useHtml, usePNG, physicalPath, htmlPathServer);
  out << tempS;
  tempS = body.str();
  out << tempS;
  output = out.str();
  return;
}

void SltwoSubalgebras::ElementToHtml(GlobalVariables& theGlobalVariables, WeylGroup& theWeyl, bool usePNG, std::string& physicalPath, std::string& htmlPathServer)
{ std::string physicalPathSAs;
  std::string htmlPathServerSAs;
  physicalPathSAs= physicalPath;
  physicalPathSAs.append("../");
  htmlPathServerSAs= htmlPathServer;
  htmlPathServerSAs.append("../");
  std::string tempS;
  this->theRootSAs.ElementToHtml(tempS, physicalPathSAs, htmlPathServerSAs, this, theGlobalVariables);
  if(usePNG)
  { int numExpectedFiles= this->size*8;
    this->texFileNamesForPNG.MakeActualSizeAtLeastExpandOnTop(numExpectedFiles);
    this->texStringsEachFile.MakeActualSizeAtLeastExpandOnTop(numExpectedFiles);
    this->listSystemCommandsLatex.MakeActualSizeAtLeastExpandOnTop(numExpectedFiles);
    this->listSystemCommandsDVIPNG.MakeActualSizeAtLeastExpandOnTop(numExpectedFiles);
  }
  this->texFileNamesForPNG.size=0;
  this->texStringsEachFile.size=0;
  this->listSystemCommandsLatex.size=0;
  this->listSystemCommandsDVIPNG.size=0;
  std::stringstream out, outNotation;
  std::string fileName;
  std::fstream theFile, fileFlas;
  outNotation << "<a href=\"" << htmlPathServer << "StructureConstants.html\">Notation, structure constants and Weyl group info</a><br> <a href=\"../rootHtml.html\">Root subsystem table</a><br>";
  std::string notation= outNotation.str();
  this->ElementToString(tempS, theGlobalVariables, theWeyl, false, true, usePNG, &physicalPath, &htmlPathServer);
  out <<tempS;
  if(usePNG)
  { fileName= physicalPath;
    fileName.append("sl2s.html");
    CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(theFile, fileName, false, true, false);
    tempS= out.str();
    theFile<< "<HMTL><BODY>"<<notation<<"<a href=\""<< htmlPathServer<< "sl2s_nopng.html\"> plain html for your copy+paste convenience</a><br>\n" <<tempS<<"</HTML></BODY>";
    theFile.close();
  }
  fileName= physicalPath;
  fileName.append("sl2s_nopng.html");
  this->ElementToString(tempS, theGlobalVariables, theWeyl, false, true, false, &physicalPath, &htmlPathServer);
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(theFile, fileName, false, true, false);
  theFile<< "<HMTL><BODY>"<<notation<<"<a href=\""<< htmlPathServer<< "sl2s.html\"> "<<".png rich html for your viewing pleasure</a><br>\n" <<tempS<<"</HTML></BODY>";
  theFile.close();
  fileName= physicalPath;
  fileName.append("StructureConstants.html");
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(theFile, fileName, false, true, false);
  this->owner.ElementToString(tempS, true, false, usePNG, theGlobalVariables, &physicalPath, &htmlPathServer, &this->texFileNamesForPNG, &this->texStringsEachFile);
  theFile << tempS;
  theFile.close();
  if (usePNG)
  { this->listSystemCommandsLatex.SetSizeExpandOnTopNoObjectInit(this->texFileNamesForPNG.size);
    this->listSystemCommandsDVIPNG.SetSizeExpandOnTopNoObjectInit(this->texFileNamesForPNG.size);
    for (int i=0; i<this->texFileNamesForPNG.size; i++)
    { CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(fileFlas, this->texFileNamesForPNG.TheObjects[i], false, true, false);
      fileFlas << "\\documentclass{article}\\begin{document}\\pagestyle{empty}\n" << this->texStringsEachFile.TheObjects[i] << "\n\\end{document}";
      std::stringstream tempStreamLatex, tempStreamPNG;
      tempStreamLatex << "latex "<< " -output-directory=" << physicalPath << " " << this->texFileNamesForPNG.TheObjects[i];
      tempS= this->texFileNamesForPNG.TheObjects[i];
      tempS.resize(tempS.size()-4);
      tempStreamPNG << "dvipng " << tempS << ".dvi -o " << tempS << ".png -T tight";
      this->listSystemCommandsLatex.TheObjects[i]= tempStreamLatex.str();
      this->listSystemCommandsDVIPNG.TheObjects[i]=tempStreamPNG.str();
      fileFlas.close();
    }
  }
}

void ComputationSetup::CountNilradicals(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ rootSubalgebra tempSA;
  inputData.theRootSubalgebras.flagUseDynkinClassificationForIsomorphismComputation=true;
  inputData.theRootSubalgebras.flagUsingActionsNormalizerCentralizerNilradical=true;
  inputData.theRootSubalgebras.flagCountingNilradicalsOnlyNoComputation=true;
  inputData.theRootSubalgebras.UpperLimitNumElementsWeyl=0;
  inputData.theRootSubalgebras.GenerateAllReductiveRootSubalgebrasUpToIsomorphism(theGlobalVariables, inputData.WeylGroupLetter, inputData.WeylGroupIndex, true, true);
//  inputData.theRootSubalgebras.ComputeDebugString(true, false, true, 0, 0, theGlobalVariables);
  inputData.theRootSubalgebras.numNilradicalsBySA.initFillInObject(inputData.theRootSubalgebras.size, 0);
  inputData.theRootSubalgebras.ComputeAllRootSubalgebrasUpToIso(theGlobalVariables, 0, inputData.theRootSubalgebras.size-6);
  std::stringstream out;
  int total=0;
  for (int i=0; i<inputData.theRootSubalgebras.size-6; i++)
  { rootSubalgebra& currentSA= inputData.theRootSubalgebras.TheObjects[i];
    out << currentSA.theDynkinDiagram.DebugString << " & " << currentSA.theCentralizerDiagram.DebugString << " & " << inputData.theRootSubalgebras.numNilradicalsBySA.TheObjects[i] << " \\\\\n";
    total+=inputData.theRootSubalgebras.numNilradicalsBySA.TheObjects[i];
  }
  out << "Total: " << total << " nilradicals up to iso";
  std::string tempS;
  inputData.theRootSubalgebras.ElementToStringCentralizerIsomorphisms(tempS, true, false, 0, inputData.theRootSubalgebras.size-6, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.StatusString1=tempS;
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.FeedIndicatorWindow(theGlobalVariables.theIndicatorVariables);
  theGlobalVariables.FeedIndicatorWindow(theGlobalVariables.theIndicatorVariables);
  theGlobalVariables.FeedIndicatorWindow(theGlobalVariables.theIndicatorVariables);
//    (*this->theGlobalVariablesContainer->Default(), 0, this->theRootSubalgebras.size-1);
}

void rootSubalgebras::ElementToStringCentralizerIsomorphisms(std::string& output, bool useLatex, bool useHtml, int fromIndex, int NumToProcess, GlobalVariables& theGlobalVariables)
{ std::stringstream out; std::string tempS;
  //W'' stands for the graph isomorphisms of C(k_ss) extending to root system isomorphisms of the entire algebra.
  for (int i=fromIndex; i<NumToProcess; i++)
    this->GenerateKintersectBOuterIsos(this->TheObjects[i], theGlobalVariables);
  if (useLatex)
    out << "\\begin{tabular}{ccccc}$\\mathfrak{k}_{ss}$& $C(k_{ss})_{ss}$ & $\\#W''$ &$\\#W'''$&$\\#(W'''\\rtimes W'')$\\\\\\hline";
  if (useHtml)
    out << "<br><table><tr><td>k_{ss}</td><td></td><td>Weyl group of C(k_{ss})_{ss}</td><td>Outer automorphisms of C(k_{ss})_{ss}<td></tr>";
  roots emptyRoots;
  emptyRoots.size=0;
  for (int i=fromIndex; i<NumToProcess; i++)
  { rootSubalgebra& current= this->TheObjects[i];
    ReflectionSubgroupWeylGroup& theOuterIsos= this->CentralizerOuterIsomorphisms.TheObjects[i];
    theOuterIsos.ComputeSubGroupFromGeneratingReflections(emptyRoots, theOuterIsos.ExternalAutomorphisms, theGlobalVariables, 0, true);
    Rational numInnerIsos = current.theCentralizerDiagram.GetSizeCorrespondingWeylGroupByFormula();
    if (useHtml)
      out << "<td>";
    current.theDynkinDiagram.ElementToString(tempS, true);
    out << tempS;
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & ";
    current.theCentralizerDiagram.ElementToString(tempS, true);
    out << tempS;
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & ";
    out << theOuterIsos.size;
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & ";
    out << numInnerIsos.ElementToString();
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & ";
    out << (numInnerIsos*theOuterIsos.size).ElementToString();
    if (useHtml)
      out << "</td></tr>";
    if (useLatex)
      out << " \\\\\n";
  }
  if (useLatex)
    out << "\\end{tabular}";
  if(useHtml)
    out << "</table><br>";
  output= out.str();
}

bool IsRegularWRT(roots& input, root& h1, root& h2, WeylGroup& theWeyl)
{ for (int i=0; i<input.size; i++)
    if (theWeyl.RootScalarCartanRoot(h1, input.TheObjects[i]).IsEqualToZero() && theWeyl.RootScalarCartanRoot(h2, input.TheObjects[i]).IsEqualToZero())
      return false;
  return true;
}

void ComputationSetup::ExperimentWithH(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ SemisimpleLieAlgebra theG;
  SltwoSubalgebras tempSl2s;
  theG.FindSl2Subalgebras(tempSl2s, inputData.WeylGroupLetter, inputData.WeylGroupIndex, theGlobalVariables);
  roots h1s, h2s;
  for (int i=0; i<tempSl2s.size; i++)
    for (int j=i+1; j<tempSl2s.size; j++)
    { root& h1= tempSl2s.TheObjects[i].theH.Hcomponent;
      root& h2= tempSl2s.TheObjects[j].theH.Hcomponent;
      if (tempSl2s.theRootSAs.AmbientWeyl.RootScalarCartanRoot(h1, h2).IsEqualToZero())
        if (IsRegularWRT(tempSl2s.theRootSAs.AmbientWeyl.RootsOfBorel, h1, h2, tempSl2s.theRootSAs.AmbientWeyl))
        { h1s.AddObjectOnTop(h1);
          h2s.AddObjectOnTop(h2);
        }
    }
  std::stringstream out;
  tempSl2s.theRootSAs.AmbientWeyl.ProjectOnTwoPlane(h1s.TheObjects[0], h2s.TheObjects[0], theGlobalVariables);
  for (int i=0; i<h1s.size; i++)
    out << "h1: " << h1s.TheObjects[i].ElementToString() << " h2: " << h2s.TheObjects[i].ElementToString() <<"\n\n";
  theGlobalVariables.theIndicatorVariables.StatusString1 = out.str();
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.FeedIndicatorWindow(theGlobalVariables.theIndicatorVariables);
}

void WeylGroup::ProjectOnTwoPlane(root& orthonormalBasisVector1, root& orthonormalBasisVector2, GlobalVariables& theGlobalVariables)
{ for (int i=0; i<this->RootSystem.size; i++)
  { double x= this->RootScalarCartanRoot(orthonormalBasisVector1, this->RootSystem.TheObjects[i]).DoubleValue()*10;
    double y= this->RootScalarCartanRoot(orthonormalBasisVector2, this->RootSystem.TheObjects[i]).DoubleValue()*10;
    theGlobalVariables.theDrawingVariables.drawLineDirectly(0, 0, x, y, 0, 0);
  }
}

void DrawingVariables::drawLineDirectly(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex)
{ if (this->theDrawLineFunction!=0)
    this->theDrawLineFunction(X1+ this->centerX, Y1+ this->centerY, X2+ this->centerX, Y2+ this->centerY, thePenStyle, ColorIndex);
}

void ComputationSetup::DyckPathPolytopeComputation(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ inputData.flagDyckPathComputationLoaded=inputData.thePartialFraction.theChambers.ReadFromFile("./DyckPathPolytope.txt", theGlobalVariables);
  inputData.thePartialFraction.theChambers.ComputeDebugString();
  assert(inputData.thePartialFraction.theChambers.ConsistencyCheck(false, theGlobalVariables));
  IrreducibleFiniteDimensionalModule theModule;
  QuasiPolynomial tempP;
  if (!inputData.flagDyckPathComputationLoaded)
    theModule.InitAndPrepareTheChambersForComputation(3, inputData.thePartialFraction.theChambers, theGlobalVariables);
  inputData.thePartialFraction.theChambers.flagMustStop=false;
  inputData.thePartialFraction.theChambers.flagIsRunning=true;
  inputData.thePartialFraction.theChambers.flagReachSafePointASAP=false;
  inputData.flagDyckPathComputationLoaded=true;
  inputData.thePartialFraction.LimitSplittingSteps=100000;
  inputData.thePartialFraction.flagUsingCheckSum=true;
  inputData.thePartialFraction.flagAnErrorHasOccurredTimeToPanic=true;
  inputData.thePartialFraction.theChambers.thePauseController.InitComputation();
  inputData.thePartialFraction.theChambers.theDirections.ComputeDebugString();
  theGlobalVariables.theIndicatorVariables.StatusString1=inputData.thePartialFraction.theChambers.theDirections.DebugString;
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.MakeReport();
  inputData.thePartialFraction.theChambers.flagUsingStartingConesSeparation=true;
  inputData.thePartialFraction.theChambers.SliceTheEuclideanSpace(theGlobalVariables, true);
  inputData.thePartialFraction.DoTheFullComputation(theGlobalVariables);
  inputData.thePartialFraction.ComputeDebugString(theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1= inputData.thePartialFraction.DebugString;
  theGlobalVariables.MakeReport();
}

void ComputationSetup::TestGraphicalOutputPolys(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ PolynomialRationalCoeff tempP;
  Rational tempRat1=-1;
  Rational tempRat2= 2;
  tempP.MakeNVarDegOnePoly(5, 0, 4, tempRat1, tempRat2);
  tempP.RaiseToPower(8);
  DrawElementInputOutput theDrawData;
  theDrawData.TopLeftCornerX=50;
  theDrawData.TopLeftCornerY=50;
  tempP.DrawElement(theGlobalVariables, theDrawData);
}

void ComputationSetup::ComputeReductiveSAs(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ reductiveSubalgebras theRedSAs;
  theRedSAs.FindTheReductiveSubalgebras(inputData.WeylGroupLetter, inputData.WeylGroupIndex, theGlobalVariables);
}

void reductiveSubalgebras::FindTheReductiveSubalgebras(char WeylLetter, int WeylIndex, GlobalVariables& theGlobalVariables)
{ //this->theSl2s.owner.FindSl2Subalgebras(this->theSl2s, WeylLetter, WeylIndex, theGlobalVariables);
  this->theSl2s.owner.FindSl2Subalgebras(this->theSl2s, WeylLetter, WeylIndex, theGlobalVariables);
  this->theSl2s.ComputeModuleDecompositionsOfMinimalContainingRegularSAs(theGlobalVariables);
  this->GenerateModuleDecompositionsPrincipalSl2s(WeylIndex, theGlobalVariables);
  this->MatchActualSl2sFixedRootSAToPartitionSl2s(theGlobalVariables);
  this->MakeSelectionBasedOnPrincipalSl2s(theGlobalVariables);
  std::string tempS; std::stringstream out;
  //this->theSl2s.ComputeDebugString(theGlobalVariables, this->theSl2s.theRootSAs.AmbientWeyl, false, false);
//  theGlobalVariables.theIndicatorVariables.StatusString1= this->theSl2s.DebugString;
  this->ElementToStringCandidatePrincipalSl2s(false, true, tempS, theGlobalVariables);
  out << tempS;
//  this->theSl2s.ElementToStringModuleDecompositionMinimalContainingRegularSAs(tempS, false, true);
//  out << tempS;
  this->theSl2s.ElementToString(tempS, theGlobalVariables, this->theSl2s.theRootSAs.AmbientWeyl, false, true, false, 0, 0);
  out << tempS;
  theGlobalVariables.theIndicatorVariables.StatusString1.append(tempS);
  theGlobalVariables.theIndicatorVariables.StatusString1= out.str();
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.FeedIndicatorWindow(theGlobalVariables.theIndicatorVariables);
}

void reductiveSubalgebras::MakeSelectionBasedOnPrincipalSl2s(GlobalVariables& theGlobalVariables)
{ this->RemainingCandidates.init(this->theLetters.size);
  for (int i=0; i<this->theLetters.size; i++)
    if (this->IndicesMatchingActualSl2s.TheObjects[i].size>0)
    { SltwoSubalgebras& theCurrentAlgebrasSl2s= this->theCandidateSubAlgebras.TheObjects[i];
      SemisimpleLieAlgebra& theSSLieAlgebra= theCurrentAlgebrasSl2s.owner;
      theSSLieAlgebra.FindSl2Subalgebras(theCurrentAlgebrasSl2s, theGlobalVariables);
      bool DoesFit=true;
      for (int j=0; j<theCurrentAlgebrasSl2s.size; j++)
      { slTwo& currentSl2= theCurrentAlgebrasSl2s.TheObjects[j];
        bool currentSl2Fits=false;
        for (int k=0; k<this->theSl2s.size; k++)
        { slTwo& other= this->theSl2s.TheObjects[k];
          currentSl2Fits=currentSl2.ModuleDecompositionFitsInto(currentSl2.highestWeights, currentSl2.multiplicitiesHighestWeights, other.highestWeights, other.multiplicitiesHighestWeights);
          if (currentSl2Fits)
            break;
        }
        if (!currentSl2Fits)
        { DoesFit=false;
          break;
        }
      }
      if (DoesFit)
        this->RemainingCandidates.AddSelectionAppendNewIndex(i);
    }
}

void reductiveSubalgebras::GenerateModuleDecompositionsPrincipalSl2s(int theRank, GlobalVariables& theGlobalVariables)
{ this->EnumerateAllPossibleDynkinDiagramsOfRankUpTo(theRank);
  slTwo tempSl2;
  this->CandidatesPrincipalSl2ofSubalgebra.MakeActualSizeAtLeastExpandOnTop(this->theLetters.size);
  this->theCandidateSubAlgebras.SetSizeExpandOnTopNoObjectInit(this->theLetters.size);
  for (int i=0; i<this->theLetters.size; i++)
  { this->theCandidateSubAlgebras.TheObjects[i].theRootSAs.AmbientWeyl.MakeFromDynkinType(this->theLetters.TheObjects[i], this->theRanks.TheObjects[i], &this->theMultiplicities.TheObjects[i]);
    this->theCandidateSubAlgebras.TheObjects[i].theRootSAs.AmbientWeyl.GenerateRootSystemFromKillingFormMatrix();
    this->theCandidateSubAlgebras.TheObjects[i].owner.theWeyl.Assign(this->theCandidateSubAlgebras.TheObjects[i].theRootSAs.AmbientWeyl);
    int theDimension = this->theCandidateSubAlgebras.TheObjects[i].theRootSAs.AmbientWeyl.CartanSymmetric.NumRows;
    tempSl2.hCharacteristic.initFillInObject(theDimension, 2);
    tempSl2.preferredAmbientSimpleBasis.MakeEiBasis(theDimension);
    tempSl2.owner = &this->theCandidateSubAlgebras.TheObjects[i].owner;
    tempSl2.ComputeModuleDecompositionAmbientLieAlgebra(theGlobalVariables);
    this->CandidatesPrincipalSl2ofSubalgebra.AddObjectOnTopHash(tempSl2);
  }
}

void reductiveSubalgebras::EnumerateAllPossibleDynkinDiagramsOfRankUpTo(int theRank)
{ this->GenerateAllPartitionsUpTo(theRank);
  this->theLetters.size=0;
  this->theMultiplicities.size=0;
  this->theRanks.size=0;
  List<int> ranksBuffer, multiplicitiesBuffer;
  List<char> lettersBuffer;
  for (int i=0; i<this->thePartitionMultiplicities.size; i++)
    this->GenerateAllDiagramsForPartitionRecursive(i, 0, ranksBuffer, multiplicitiesBuffer, lettersBuffer);
}

void reductiveSubalgebras::GenerateAllDiagramsForPartitionRecursive(int indexPartition, int indexInPartition, List<int>& ranksBuffer, List<int>& multiplicitiesBuffer, List<char>& lettersBuffer)
{ List<int>& partitionValues= this->thePartitionValues.TheObjects[indexPartition];
  List<int>& partitionMults= this->thePartitionMultiplicities.TheObjects[indexPartition];
  if (indexInPartition>= partitionValues.size)
  { this->theLetters.AddObjectOnTop(lettersBuffer);
    this->theMultiplicities.AddObjectOnTop(multiplicitiesBuffer);
    this->theRanks.AddObjectOnTop(ranksBuffer);
    return;
  }
  Selection DistributionBetweenTheFourLetters;
  int theMult = partitionMults.TheObjects[indexInPartition];
  int theRank = partitionValues.TheObjects[indexInPartition];
  int numLetters;
  List<char> lettersAvailable; lettersAvailable.SetSizeExpandOnTopNoObjectInit(5);
  lettersAvailable.TheObjects[0]='A';
  lettersAvailable.TheObjects[1]='B';
  lettersAvailable.TheObjects[2]='C';
  lettersAvailable.TheObjects[3]='D';
  switch(theRank)
  { case 1: numLetters=1; break;
    case 2: numLetters=3; lettersAvailable.TheObjects[2]='G'; break;
    case 3: numLetters=3; break;
    case 4: numLetters=5; lettersAvailable.TheObjects[4]='F'; break;
    case 6: numLetters=5; lettersAvailable.TheObjects[4]='E'; break;
    case 7: numLetters=5; lettersAvailable.TheObjects[4]='E'; break;
    case 8: numLetters=5; lettersAvailable.TheObjects[4]='E'; break;
    default: numLetters=4; break;
  }
  int numBars=numLetters-1;
  DistributionBetweenTheFourLetters.init(numBars+theMult); //there are numLetters letters, therefore we need numLetters-1 barriers in numLetters-1+theMult cells to record a partition of mult into 7 letters.
  int numCycles= MathRoutines::NChooseK(DistributionBetweenTheFourLetters.MaxSize, numBars);
  int oldsize= ranksBuffer.size;
  for (int i=0; i<numCycles; i++)
  { DistributionBetweenTheFourLetters.incrementSelectionFixedCardinality(numBars);
    int startIndex=-1;
    int endIndex;
    for (int k=0; k< numLetters; k++)
    { if (k!=0)
        startIndex= DistributionBetweenTheFourLetters.elements[k-1];
      if (k!=numBars)
        endIndex= DistributionBetweenTheFourLetters.elements[k];
      else
        endIndex= DistributionBetweenTheFourLetters.MaxSize;
      int numIsotypic= endIndex-startIndex-1;
      if (numIsotypic!=0)
      { ranksBuffer.AddObjectOnTop(theRank);
        lettersBuffer.AddObjectOnTop(lettersAvailable.TheObjects[k]);
        multiplicitiesBuffer.AddObjectOnTop(numIsotypic);
      }
    }
    this->GenerateAllDiagramsForPartitionRecursive(indexPartition, indexInPartition+1, ranksBuffer, multiplicitiesBuffer, lettersBuffer);
    ranksBuffer.size=oldsize;
    multiplicitiesBuffer.size=oldsize;
    lettersBuffer.size=oldsize;
  }
}

void reductiveSubalgebras::MatchRealSl2sToPartitionSl2s()
{ this->IndicesMatchingSl2s.SetSizeExpandOnTopNoObjectInit(this->theLetters.size);
  for (int i=0; i<this->CandidatesPrincipalSl2ofSubalgebra.size; i++)
  { this->IndicesMatchingSl2s.TheObjects[i].size=0;
    for (int j=0; j<this->theSl2s.size; j++)
      if (this->CandidatesPrincipalSl2ofSubalgebra.TheObjects[i].ModuleDecompositionFitsInto(this->theSl2s.TheObjects[j]))
        this->IndicesMatchingSl2s.TheObjects[i].AddObjectOnTop(j);
  }
}

void reductiveSubalgebras::MatchActualSl2sFixedRootSAToPartitionSl2s(GlobalVariables& theGlobalVariables)
{ this->theSl2s.ComputeModuleDecompositionsOfMinimalContainingRegularSAs(theGlobalVariables);
  List<int> tempL;
  this->IndicesMatchingActualSl2s.initFillInObject(this->theLetters.size, tempL);
  this->IndicesMatchingPartitionSl2s.initFillInObject(this->theSl2s.size, tempL);
  for (int i=0; i<this->theSl2s.size; i++)
  { slTwo& theSl2= this->theSl2s.TheObjects[i];
    for (int j=0; j<theSl2.IndicesMinimalContainingRootSA.size; j++)
      for (int k=0; k<this->CandidatesPrincipalSl2ofSubalgebra.size; k++)
      { slTwo& candidateSl2= this->CandidatesPrincipalSl2ofSubalgebra.TheObjects[k];
        if (theSl2.ModuleDecompositionFitsInto(candidateSl2.highestWeights, candidateSl2.multiplicitiesHighestWeights, theSl2.HighestWeightsDecompositionMinimalContainingRootSA.TheObjects[j], theSl2.MultiplicitiesDecompositionMinimalContainingRootSA.TheObjects[j]))
        { this->IndicesMatchingActualSl2s.TheObjects[k].AddObjectOnTop(i);
          this->IndicesMatchingPartitionSl2s.TheObjects[i].AddObjectOnTop(k);
        }
      }
  }
}

bool slTwo::ModuleDecompositionFitsInto(const slTwo& other)
{ return this->ModuleDecompositionFitsInto(this->highestWeights, this->multiplicitiesHighestWeights, other.highestWeights, other.multiplicitiesHighestWeights);
}

bool slTwo::ModuleDecompositionFitsInto(const List<int>& highestWeightsLeft, const List<int>& multiplicitiesHighestWeightsLeft, const List<int>& highestWeightsRight, const List<int>& multiplicitiesHighestWeightsRight)
{ for (int i=0; i<highestWeightsLeft.size; i++)
  { int theIndex= highestWeightsRight.IndexOfObject(highestWeightsLeft.TheObjects[i]);
    if (theIndex==-1)
      return false;
    else
      if (multiplicitiesHighestWeightsLeft.TheObjects[i]>multiplicitiesHighestWeightsRight.TheObjects[theIndex])
        return false;
  }
  return true;
}

void reductiveSubalgebras::GenerateAllPartitionsRecursive(int remainingToBePartitioned, int CurrentValue, List<int>& Multiplicities, List<int>& Values)
{ if (remainingToBePartitioned==0)
  { this->thePartitionMultiplicities.AddObjectOnTop(Multiplicities);
    this->thePartitionValues.AddObjectOnTop(Values);
    return;
  }
  int possibleMults=(remainingToBePartitioned/CurrentValue)+1;
  for(int i=0; i<possibleMults; i++)
  { if (i>0)
    { Multiplicities.AddObjectOnTop(i);
      Values.AddObjectOnTop(CurrentValue);
    }
    int newRemainder = remainingToBePartitioned - i*CurrentValue;
    int newCurrentValue = MathRoutines::Minimum(CurrentValue-1, newRemainder);
    if (newCurrentValue!=0 || newRemainder==0)
      this->GenerateAllPartitionsRecursive(newRemainder, newCurrentValue, Multiplicities, Values);
    if (i>0)
    { Multiplicities.size--;
      Values.size--;
    }
  }
}

void reductiveSubalgebras::GenerateAllPartitionsUpTo(int theRank)
{ this->thePartitionMultiplicities.size=0;
  this->thePartitionValues.size=0;
  for (int i=1; i<=theRank; i++)
    this->GenerateAllPartitionsDontInit(i);
}

void reductiveSubalgebras::GenerateAllPartitionsDontInit(int theRank)
{ int upperLimit= MathRoutines::TwoToTheNth(theRank);
  this->thePartitionMultiplicities.MakeActualSizeAtLeastExpandOnTop(upperLimit);
  this->thePartitionValues.MakeActualSizeAtLeastExpandOnTop(upperLimit);
  List<int> buffer1, buffer2;
  this->GenerateAllPartitionsRecursive(theRank, theRank, buffer1, buffer2);
}

void reductiveSubalgebras::ElementToStringDynkinType(int theIndex, bool useLatex, bool useHtml, std::string& output)
{ std::stringstream out;
  for (int j=0; j<this->theLetters.TheObjects[theIndex].size; j++)
  { int theMult= this->theMultiplicities.TheObjects[theIndex].TheObjects[j];
    int theRank= this->theRanks.TheObjects[theIndex].TheObjects[j];
    char theLetter= this->theLetters.TheObjects[theIndex].TheObjects[j];
    if (theMult!=1)
      out << theMult;
    out << theLetter << "_" <<theRank << " ";
    if (j!=this->theLetters.TheObjects[theIndex].size-1)
      out << "+ ";
  }
  output=out.str();
}

void reductiveSubalgebras::ElementToStringCandidatePrincipalSl2s(bool useLatex, bool useHtml, std::string& output, GlobalVariables& theGlobalVariables)
{ std::stringstream out; std::string tempS;
  for (int i =0; i<this->thePartitionValues.size; i++)
  { for (int j=0; j< this->thePartitionValues.TheObjects[i].size; j++)
    { int mult= this->thePartitionMultiplicities.TheObjects[i].TheObjects[j];
      int val= this->thePartitionValues.TheObjects[i].TheObjects[j];
      if (mult!=1)
        out << mult << " x ";
      out << val << " ";
      if (j!=this->thePartitionValues.TheObjects[i].size-1)
        out << "+ ";
    }
    if (useHtml)
      out << "\n<br>";
    out << "\n";
  }
  for (int i=0; i<this->theLetters.size; i++)
  { this->ElementToStringDynkinType(i, useLatex, useHtml, tempS);
    out << tempS << "     Module decomposition: ";
    slTwo& theSl2= this->CandidatesPrincipalSl2ofSubalgebra.TheObjects[i];
    theSl2.ElementToStringModuleDecomposition(useLatex, useHtml, tempS);
    out << tempS;
    if (this->IndicesMatchingActualSl2s.size>0)
    { out << " Matching actual sl(2)'s: ";
      for (int j=0; j<this->IndicesMatchingActualSl2s.TheObjects[i].size; j++)
      { int tempI= this->IndicesMatchingActualSl2s.TheObjects[i].TheObjects[j];
        out << this->theSl2s.TheObjects[tempI].hCharacteristic.ElementToString() << ", ";
      }
    }
    if (useHtml)
      out << "\n<br>";
    out << "\n";
  }
  out << "Remaining candidates:\n";
  if (useHtml)
    out << "<br>\n";
  for (int i=0; i<this->theCandidateSubAlgebras.size; i++)
  { SltwoSubalgebras& currentSl2s= this->theCandidateSubAlgebras.TheObjects[i];
    if (currentSl2s.size>0)
    { this->ElementToStringDynkinType(i, useLatex, useHtml, tempS);
      out << tempS;
      if (this->RemainingCandidates.selected[i])
        out << " orbits fit\n";
      else
        out << " orbits do not fit, embedding impossible\n";
      if (useHtml)
        out<< "<br>\n";
      currentSl2s.ElementToStringNoGenerators(tempS, theGlobalVariables, currentSl2s.theRootSAs.AmbientWeyl, useLatex, useHtml, false, 0, 0);
      out << tempS;
    }
  }
  output=out.str();
}

void WeylGroup::MakeFromDynkinType(List<char>& theLetters, List<int>& theRanks, List<int>* theMultiplicities)
{ WeylGroup tempW;
  this->CartanSymmetric.init(0, 0);
  for (int i=0; i<theLetters.size; i++)
  { tempW.MakeArbitrary(theLetters.TheObjects[i], theRanks.TheObjects[i]);
    int numSummands=1;
    if (theMultiplicities!=0)
      numSummands =theMultiplicities->TheObjects[i];
    for (int j=0; j<numSummands; j++)
      this->CartanSymmetric.DirectSumWith(tempW.CartanSymmetric);
  }
}

void CombinatorialChamberContainer::WriteToFile(std::fstream& output, GlobalVariables& theGlobalVariables)
{ this->LabelChamberIndicesProperly();
  output << "Num_pointers: " << this->size << "\n";
///////////////////////////////////////////////////
  output << "Dimension: " << this->AmbientDimension << "\n";
  output << "Using_separating_cones: " << this->flagUsingStartingConesSeparation << "\n";
  output << "CurrentIndex: " << this->theCurrentIndex << "\n";
  output << "Directions:\n";
  this->theDirections.WriteToFile(output, theGlobalVariables);
  output << "\nNext_index_to_slice: " << this->indexNextChamberToSlice << "\n";
  output << "\nLowestNonCheckedForGlueing: " << this->indexLowestNonCheckedForGlueing << "\n";
  output << "\nTotalGlued: " << this->NumTotalGlued << "\n";
  output << "FirstNonExploredIndex: " << this->FirstNonExploredIndex << "\n";
  this->TheGlobalConeNormals.WriteToFile(output, theGlobalVariables);
  output << "\n";
  this->startingCones.WriteToFile(output, theGlobalVariables);
  output << "\nTheFacets: ";
  this->theHyperplanes.WriteToFile(output);
////////////////////////////////////////////////////
  output << "\nPreferredNextChambers: " << this->PreferredNextChambers.size << " ";
  for (int i=0; i<this->PreferredNextChambers.size; i++)
    output << this->PreferredNextChambers.TheObjects[i] << " ";
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i]!=0)
    { output << "\nChamber:\n";
      this->TheObjects[i]->WriteToFile(output, theGlobalVariables);
    } else
      output << "Empty\n";
}

void CombinatorialChamberContainer::ReadFromFile(std::fstream& input, GlobalVariables& theGlobalVariables)
{ std::string tempS;
  input.seekg(0);
  this->KillAllElements();
  int tempI;
  input >> tempS >> tempI;
  this->initAndCreateNewObjects(tempI);
///////////////////////////////////////////////////
  input >> tempS >> this->AmbientDimension;
  input >> tempS >> this->flagUsingStartingConesSeparation;
  input >> tempS >> this->theCurrentIndex;
  input >> tempS;
  this->theDirections.ReadFromFile(input, theGlobalVariables);
  input >> tempS >> this->indexNextChamberToSlice;
  input >> tempS >> this->indexLowestNonCheckedForGlueing;
  input >> tempS >> this->NumTotalGlued;
  input >> tempS >> this->FirstNonExploredIndex;
  this->TheGlobalConeNormals.ReadFromFile(input, theGlobalVariables);
  this->startingCones.ReadFromFile(input, theGlobalVariables);
  input >> tempS;
  this->theHyperplanes.ReadFromFile(input);
////////////////////////////////////////////////////
  input >> tempS >> tempI;
  this->PreferredNextChambers.SetSizeExpandOnTopNoObjectInit(tempI);
  for (int i=0; i<this->PreferredNextChambers.size; i++)
    input >> this->PreferredNextChambers.TheObjects[i];
  for (int i=0; i<this->size; i++)
  { input >> tempS;
    if (tempS=="Chamber:")
      this->TheObjects[i]->ReadFromFile(input, theGlobalVariables, *this);
    else
    { assert (tempS=="Empty");
      delete this->TheObjects[i];
      this->TheObjects[i]=0;
    }
    std::stringstream out;
    out << "reading chamber " << i+1 <<" out of " << this->size;
    theGlobalVariables.theIndicatorVariables.ProgressReportString1=out.str();
    theGlobalVariables.MakeReport();
  }
  this->flagIsRunning=false;
  this->flagMustStop=false;
  this->flagReachSafePointASAP=false;
}

void DrawOperations::drawLineBetweenTwoVectorsBuffer(root& vector1, root& vector2, unsigned long thePenStyle, int ColorIndex)
{ this->TypeNthDrawOperation.AddObjectOnTop(this->typeDrawLineBetweenTwoVectors);
  this->IndexNthDrawOperation.AddObjectOnTop(this->theDrawLineBetweenTwoRootsOperations.size);
  this->theDrawLineBetweenTwoRootsOperations.AddObjectOnTopCreateNew();
  this->theDrawLineBetweenTwoRootsOperations.LastObject()->init(vector1, vector2, thePenStyle, ColorIndex);
}

void DrawOperations::drawTextAtVectorBuffer(root& input, const std::string& inputText, int ColorIndex, int theFontSize, int theTextStyle)
{ this->TypeNthDrawOperation.AddObjectOnTop(this->typeDrawTextAtVector);
  this->IndexNthDrawOperation.AddObjectOnTop(this->theDrawTextAtVectorOperations.size);
  this->theDrawTextAtVectorOperations.AddObjectOnTopCreateNew();
  this->theDrawTextAtVectorOperations.LastObject()->init(input, inputText, ColorIndex, theFontSize, theTextStyle);
}

void DrawOperations::drawLineBuffer(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex)
{ this->TypeNthDrawOperation.AddObjectOnTop(this->typeDrawLine);
  this->IndexNthDrawOperation.AddObjectOnTop(this->theDrawLineOperations.size);
  this->theDrawLineOperations.AddObjectOnTopCreateNew();
  this->theDrawLineOperations.LastObject()->init(X1, Y1, X2, Y2, thePenStyle, ColorIndex);
}

void DrawOperations::drawTextBuffer(double X1, double Y1, const std::string& inputText, int ColorIndex, int theFontSize, int theTextStyle)
{ this->TypeNthDrawOperation.AddObjectOnTop(this->typeDrawText);
  this->IndexNthDrawOperation.AddObjectOnTop(this->theDrawTextOperations.size);
  this->theDrawTextOperations.AddObjectOnTopCreateNew();
  this->theDrawTextOperations.LastObject()->init(X1, Y1, inputText, ColorIndex, theFontSize, theTextStyle);
}

void DrawingVariables::drawBuffer()
{ double x1, x2, y1, y2; int currentPenStyle, currentTextStyle;
  for (int i=0; i<this->theBuffer.IndexNthDrawOperation.size; i++)
    switch (this->theBuffer.TypeNthDrawOperation.TheObjects[i])
    { case DrawOperations::typeDrawText:
        if (this->theDrawTextFunction!=0)
        { DrawTextOperation& theDrawTextOp= this->theBuffer.theDrawTextOperations.TheObjects[this->theBuffer.IndexNthDrawOperation.TheObjects[i]];
          currentTextStyle=this->GetActualTextStyleFromFlagsAnd(theDrawTextOp.TextStyle);
          if (currentTextStyle==this->TextStyleInvisible)
            break;
          this->theDrawTextFunction(theDrawTextOp.X1, theDrawTextOp.Y1, theDrawTextOp.theText.c_str(), theDrawTextOp.theText.size(), theDrawTextOp.ColorIndex, theDrawTextOp.fontSize);
        }
        break;
      case DrawOperations::typeDrawLine:
        if (this->theDrawLineFunction!=0)
        { DrawLineOperation& theDrawLineOp= this->theBuffer.theDrawLineOperations.TheObjects[this->theBuffer.IndexNthDrawOperation.TheObjects[i]];
          currentPenStyle= this->GetActualPenStyleFromFlagsAnd(theDrawLineOp.thePenStyle);
          if (currentPenStyle==this->PenStyleInvisible)
            break;
          this->theDrawLineFunction(theDrawLineOp.X1, theDrawLineOp.Y1, theDrawLineOp.X2, theDrawLineOp.Y2, currentPenStyle, theDrawLineOp.ColorIndex);
        }
        break;
      case DrawOperations::typeDrawLineBetweenTwoVectors:
        if (this->theDrawLineFunction!=0)
        { DrawLineBetweenTwoRootsOperation& theDrawLineBnTwoOp= this->theBuffer.theDrawLineBetweenTwoRootsOperations.TheObjects[this->theBuffer.IndexNthDrawOperation.TheObjects[i]];
          currentPenStyle= this->GetActualPenStyleFromFlagsAnd(theDrawLineBnTwoOp.thePenStyle);
          if (currentPenStyle==this->PenStyleInvisible)
            break;
          this->GetCoordsForDrawing(*this, theDrawLineBnTwoOp.v1, x1, y1);
          this->GetCoordsForDrawing(*this, theDrawLineBnTwoOp.v2, x2, y2);
          this->theDrawLineFunction(x1, y1, x2, y2, currentPenStyle, theDrawLineBnTwoOp.ColorIndex);
        }
        break;
      case DrawOperations::typeDrawTextAtVector:
        if (this->theDrawTextFunction!=0)
        { DrawTextAtVectorOperation& theDrawTextOp= this->theBuffer.theDrawTextAtVectorOperations.TheObjects[this->theBuffer.IndexNthDrawOperation.TheObjects[i]];
          currentTextStyle= this->GetActualTextStyleFromFlagsAnd(theDrawTextOp.TextStyle);
          if (currentTextStyle==this->TextStyleInvisible)
            break;
          this->GetCoordsForDrawing(*this, theDrawTextOp.theVector, x1, y1);
          this->theDrawTextFunction(x1, y1, theDrawTextOp.theText.c_str(), theDrawTextOp.theText.size(), theDrawTextOp.ColorIndex, theDrawTextOp.fontSize);
        }
        break;
      default: break;
    }
}

int DrawingVariables::GetActualPenStyleFromFlagsAnd(int inputPenStyle)
{ if (inputPenStyle==this->PenStyleInvisible)
    return this->PenStyleInvisible;
  if (inputPenStyle== this->PenStyleDashed)
    return this->PenStyleDashed;
  if (inputPenStyle==this->PenStyleDotted)
    return this->PenStyleDotted;
  if (inputPenStyle==this->PenStyleNormal)
    return this->PenStyleNormal;
  if (!this->flagDrawingInvisibles)
    if (inputPenStyle == this->PenStyleLinkToOriginPermanentlyZeroChamber || inputPenStyle == this->PenStyleLinkToOriginZeroChamber || inputPenStyle == this->PenStyleZeroChamber || inputPenStyle == this->PenStylePermanentlyZeroChamber)
      return this->PenStyleInvisible;
  if (inputPenStyle==this->PenStyleLinkToOrigin || inputPenStyle==this->PenStyleLinkToOriginPermanentlyZeroChamber || inputPenStyle==this->PenStyleLinkToOriginZeroChamber)
  { if (this->flagDrawingLinkToOrigin)
      return this->PenStyleDashed;
    else
      return this->PenStyleInvisible;
  }
  if (inputPenStyle==this->PenStylePermanentlyZeroChamber || inputPenStyle==this->PenStyleZeroChamber)
    return this->PenStyleDotted;
  return this->PenStyleNormal;
}

int DrawingVariables::GetActualTextStyleFromFlagsAnd(int inputTextStyle)
{ if (inputTextStyle==this->TextStyleInvisible)
    return this->TextStyleInvisible;
  if (inputTextStyle==this->TextStyleNormal)
    return this->TextStyleNormal;
  if (!this->flagDrawChamberIndices && (inputTextStyle==this->TextStyleChamber || inputTextStyle==this->TextStylePermanentlyZeroChamber || inputTextStyle==this->TextStyleZeroChamber))
    return this->TextStyleInvisible;
  if (!this->flagDrawingInvisibles && (inputTextStyle==this->TextStylePermanentlyZeroChamber || inputTextStyle==this->TextStyleZeroChamber))
    return this->TextStyleInvisible;
  return this->TextStyleNormal;
}

void DrawingVariables::drawLineBuffer(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex)
{ this->theBuffer.drawLineBuffer(X1, Y1, X2, Y2, thePenStyle, ColorIndex);
}

void DrawingVariables::drawTextBuffer(double X1, double Y1, const std::string& inputText, int color, std::fstream* LatexOutFile)
{ this->theBuffer.drawTextBuffer(X1, Y1, inputText, color, this->fontSizeNormal, this->TextStyleNormal);
}

void DrawingVariables::drawString(DrawElementInputOutput& theDrawData, const std::string& input, int theFontSize, int theTextStyle)
{ theDrawData.outputHeight=0; theDrawData.outputWidth=0;
  if (input=="")
    return;
  for (unsigned int i=0; i<input.size(); i++)
  { std::string tempS;
    tempS=input.at(i);
    this->theBuffer.drawTextBuffer(theDrawData.outputWidth+theDrawData.TopLeftCornerX, theDrawData.outputHeight+theDrawData.TopLeftCornerY, tempS, 0, theFontSize, theTextStyle);
    theDrawData.outputWidth+=(int)(((double) theFontSize)/1.15);
  }
}

void Rational::DrawElement(GlobalVariables& theGlobalVariables, DrawElementInputOutput& theDrawData)
{ std::string tempS;
  this->ElementToString(tempS);
  theGlobalVariables.theDrawingVariables.theBuffer.drawTextBuffer(theDrawData.TopLeftCornerX, theDrawData.TopLeftCornerY, tempS, 0, theGlobalVariables.theDrawingVariables.fontSizeNormal, theGlobalVariables.theDrawingVariables.TextStyleNormal);
  theDrawData.outputHeight=10;
  theDrawData.outputWidth=10*tempS.size();
}

void ComputationSetup::LProhibitingWeightsComputation(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ if (inputData.theRootSubalgebras.controllerLProhibitingRelations.IsRunning())
    return;
  rootSubalgebras& theRootSAs= inputData.theRootSubalgebras;
  inputData.theRootSubalgebras.controllerLProhibitingRelations.InitComputation();
  rootSubalgebra tempSA;
  //most important flags
  inputData.theRootSubalgebras.flagUseDynkinClassificationForIsomorphismComputation=false;
  inputData.theRootSubalgebras.flagUsingActionsNormalizerCentralizerNilradical=true;
  inputData.theRootSubalgebras.flagComputeConeCondition=true;
  inputData.theRootSubalgebras.flagComputingLprohibitingWeights=true;
  inputData.theRootSubalgebras.flagLookingForMinimalRels=true;
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  inputData.theRootSubalgebras.theGoodRelations.flagIncludeCoordinateRepresentation=false;
  inputData.theRootSubalgebras.theBadRelations.flagIncludeCoordinateRepresentation=false;
  inputData.theRootSubalgebras.theMinRels.flagIncludeCoordinateRepresentation=false;
  inputData.theRootSubalgebras.theGoodRelations.flagIncludeSubalgebraDataInDebugString=false;
  inputData.theRootSubalgebras.theBadRelations.flagIncludeSubalgebraDataInDebugString=false;
  if (!inputData.theRootSubalgebras.flagNilradicalComputationInitialized)
    if (!inputData.theRootSubalgebras.ReadFromDefaultFileNilradicalGeneration(theGlobalVariables))
    { inputData.theRootSubalgebras.GenerateAllReductiveRootSubalgebrasUpToIsomorphism(theGlobalVariables, inputData.WeylGroupLetter, inputData.WeylGroupIndex, true, true);
      inputData.theRootSubalgebras.initForNilradicalGeneration();
    }
  inputData.theRootSubalgebras.ComputeLProhibitingRelations(theGlobalVariables);
  std::string stringConeConditionSatisfying;
  if (theRootSAs.flagStoringNilradicals)
    theRootSAs.ElementToStringConeConditionNotSatisfying(stringConeConditionSatisfying, false, theGlobalVariables);
  std::string tempS;
  inputData.theRootSubalgebras.ComputeDebugString(true, false, true, 0, 0, theGlobalVariables);
  inputData.theRootSubalgebras.ElementToStringCentralizerIsomorphisms(tempS, true, false, 0, inputData.theRootSubalgebras.size-1, theGlobalVariables);
  inputData.theRootSubalgebras.DebugString.append(tempS);
  inputData.theOutput.DebugString.append("\\documentclass{article}\n\\usepackage{amssymb}\n\\usepackage{longtable}\n");
  inputData.theOutput.DebugString.append("\\addtolength{\\hoffset}{-3.5cm}\\addtolength{\\textwidth}{7cm}");
  inputData.theOutput.DebugString.append("\\addtolength{\\voffset}{-3.5cm}\\addtolength{\\textheight}{7cm}");
  inputData.theOutput.DebugString.append("\\begin{document}~");
  std::stringstream out;
  if (theRootSAs.NumConeConditionHoldsBySSpart.size==theRootSAs.size)
    for (int i=0; i< theRootSAs.size; i++)
      out << theRootSAs.TheObjects[i].theDynkinDiagram.DebugString << " " << theRootSAs.NumConeConditionHoldsBySSpart.TheObjects[i] << ";   ";
  out << "\n\n\\noindent Number of different subalgebras up to $\\mathfrak{g}$-automorphism such that $\\mathfrak{n}\\cap C(\\mathfrak{k}_{ss})$ is a nilradical of a parabolic subalgebra of $\\mathfrak{k}$: " << inputData.theRootSubalgebras.NumSubalgebrasProcessed << "\n";
  out << "\n\n\\noindent Among them " << inputData.theRootSubalgebras.NumSubalgebrasProcessed - inputData.theRootSubalgebras.NumConeConditionFailures << " satisfy the cone condition and " << inputData.theRootSubalgebras.NumConeConditionFailures << " do not." ;
  out << "\n\n";
  out << stringConeConditionSatisfying << "\n\n";
  inputData.theRootSubalgebras.theBadRelations.ComputeDebugString(inputData.theRootSubalgebras, theGlobalVariables);
  inputData.theRootSubalgebras.theGoodRelations.ComputeDebugString(inputData.theRootSubalgebras, theGlobalVariables);
  //this->theRootSubalgebras.theMinRels.ComputeDebugString
  //  (this->theRootSubalgebras, *this->theGlobalVariablesContainer->Default());
  inputData.theOutput.DebugString.append(inputData.theRootSubalgebras.DebugString);
  inputData.theOutput.DebugString.append("\n\n\n");
  tempS = out.str();
  inputData.theOutput.DebugString.append(tempS);

  if (inputData.theRootSubalgebras.theGoodRelations.size!=0)
    inputData.theOutput.DebugString.append(inputData.theRootSubalgebras.theGoodRelations.DebugString);
  inputData.theOutput.DebugString.append("\n\n\n");
  if (inputData.theRootSubalgebras.theBadRelations.size>0)
  { inputData.theOutput.DebugString.append("The bad relations: \n\n");
    inputData.theOutput.DebugString.append(inputData.theRootSubalgebras.theBadRelations.DebugString);
  }
  if (inputData.theRootSubalgebras.flagLookingForMinimalRels)
  { inputData.theOutput.DebugString.append("\n\nMinimal relations: \n\n");
    inputData.theOutput.DebugString.append(inputData.theRootSubalgebras.theMinRels.DebugString);
  }
  inputData.theOutput.DebugString.append("\\end{document}");
  theGlobalVariables.theIndicatorVariables.StatusString1= inputData.theOutput.DebugString;
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.MakeReport();
  inputData.theRootSubalgebras.controllerLProhibitingRelations.ExitComputation();
}

bool rootSubalgebras::ReadFromDefaultFileNilradicalGeneration(GlobalVariables& theGlobalVariables)
{ std::fstream theFile;
  if (CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(theFile, "./theNilradicalsGenerator.txt", false, false, false))
  { theFile.seekg(0);
    this->ReadFromFileNilradicalGeneration(theFile, theGlobalVariables);
    return true;
  }
  return false;
}

void rootSubalgebras::WriteToDefaultFileNilradicalGeneration(GlobalVariables& theGlobalVariables)
{ std::fstream theFile;
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(theFile, "./theNilradicalsGenerator.txt", false, true, false);
  this->WriteToFileNilradicalGeneration(theFile, theGlobalVariables);
}

void rootSubalgebras::WriteToFileNilradicalGeneration(std::fstream& output, GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.WriteToFile(output);
  output << "Number_subalgebras: " << this->size << "\n";
  //////////////////////////////////////////////////////////////////////////////////////
  output << "Index_current_SA_nilradicals_generation: " << this->IndexCurrentSANilradicalsGeneration << "\n";
  output << "Num_SAs_to_be_processed: " << this->NumReductiveRootSAsToBeProcessedNilradicalsGeneration << "\n";
  output << "Parabolics_counter_nilradical_generation: " << this->parabolicsCounterNilradicalGeneration << "\n";
  output << "Num_SAs_processed: " << this->NumSubalgebrasProcessed << "\n";
  output << "Num_cone_condition_failures: " << this->NumConeConditionFailures << "\n";
  output << "Implied_selections: ";
  this->ImpiedSelectionsNilradical.WriteToFile(output);
  output << "Counters_nilradicals_generation: ";
  output << this->CountersNilradicalsGeneration;
  output << "\nRecursion_depth: " << this->RecursionDepthNilradicalsGeneration << "\n";
  ////////////////////////////////////////////////////////////////////////////////////////
  for (int  i=0; i<this->size; i++)
    this->TheObjects[i].WriteToFileNilradicalGeneration(output, theGlobalVariables, *this);
  this->theGoodRelations.WriteToFile(output, theGlobalVariables);
  this->theBadRelations.WriteToFile(output, theGlobalVariables);
}

void rootSubalgebras::ReadFromFileNilradicalGeneration(std::fstream& input, GlobalVariables& theGlobalVariables)
{ std::string tempS; int tempI;
  this->AmbientWeyl.ReadFromFile(input);
  this->AmbientWeyl.ComputeRho(true);
  input >> tempS >> tempI;
  assert(tempS=="Number_subalgebras:");
  this->SetSizeExpandOnTopNoObjectInit(tempI);
  //////////////////////////////////////////////////////////////////////////////////////
  input >> tempS >> this->IndexCurrentSANilradicalsGeneration;
  input >> tempS >> this->NumReductiveRootSAsToBeProcessedNilradicalsGeneration;
  input >> tempS >> this->parabolicsCounterNilradicalGeneration;
  input >> tempS >> this->NumSubalgebrasProcessed;
  input >> tempS >> this->NumConeConditionFailures;
  input >> tempS;
  this->ImpiedSelectionsNilradical.ReadFromFile(input);
  input >> tempS;
  input >> this->CountersNilradicalsGeneration;
  input >> tempS >> this->RecursionDepthNilradicalsGeneration;
  /////////////////////////////////////////////////////////////////////////////////////
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].ReadFromFileNilradicalGeneration(input, theGlobalVariables, *this);
  this->theGoodRelations.ReadFromFile(input, theGlobalVariables, *this);
  this->theBadRelations.ReadFromFile(input, theGlobalVariables, *this);
  this->flagNilradicalComputationInitialized=true;
}

void rootSubalgebra::WriteToFileNilradicalGeneration(std::fstream& output, GlobalVariables& theGlobalVariables, rootSubalgebras& owner)
{ output << "Simple_basis_k: ";
  this->SimpleBasisK.WriteToFile(output, theGlobalVariables);
}

void rootSubalgebra::ReadFromFileNilradicalGeneration(std::fstream& input, GlobalVariables& theGlobalVariables, rootSubalgebras& owner)
{ std::string tempS;
  input >> tempS;
  assert(tempS=="Simple_basis_k:");
  this->SimpleBasisK.ReadFromFile(input, theGlobalVariables);
  this->genK.CopyFromBase(this->SimpleBasisK);
  this->AmbientWeyl.Assign(owner.AmbientWeyl);
  this->ComputeAll();
}

void rootSubalgebra::GeneratePossibleNilradicalsInit(List<Selection>& impliedSelections, int& parabolicsCounter)
{ impliedSelections.SetSizeExpandOnTopNoObjectInit(this->kModules.size+1);
  parabolicsCounter=0;
}

void rootSubalgebra::GeneratePossibleNilradicals(Controller& PauseMutex, List<Selection>& impliedSelections, int& parabolicsCounter, GlobalVariables& theGlobalVariables, bool useParabolicsInNilradical, rootSubalgebras& owner, int indexInOwner)
{  //this->ComputeAll();
  this->GenerateKmodMultTable(this->theMultTable, this->theOppositeKmods, theGlobalVariables);
  if (this->flagAnErrorHasOccuredTimeToPanic)
    this->theMultTable.ComputeDebugString(*this);
  if (this->flagAnErrorHasOccuredTimeToPanic)
    this->ComputeDebugString(theGlobalVariables);
  this->NumTotalSubalgebras=0;
  Selection emptySel;
  emptySel.init(this->SimpleBasisCentralizerRoots.size);
  owner.ComputeActionNormalizerOfCentralizerIntersectNilradical(emptySel, *this, theGlobalVariables);
  int numCycles= MathRoutines::TwoToTheNth(this->SimpleBasisCentralizerRoots.size);
  theGlobalVariables.selApproveSelAgainstOneGenerator.init(this->kModules.size);
  List<Selection> StartingNilradicalsNoRepetition;
  StartingNilradicalsNoRepetition.MakeActualSizeAtLeastExpandOnTop(numCycles);
  Selection tempSel, ParabolicsGenerator;
  if (!owner.flagNilradicalComputationInitialized)
    owner.CountersNilradicalsGeneration.SetSizeExpandOnTopNoObjectInit(this->kModules.size+1);
  if (owner.flagStoringNilradicals)
    owner.storedNilradicals.TheObjects[indexInOwner].size=0;
  roots tempRootsTest;
  if (useParabolicsInNilradical)
  { this->flagFirstRoundCounting=false;
    ParabolicsGenerator.init(this->SimpleBasisCentralizerRoots.size);
    for (int i=0; i<numCycles; i++, ParabolicsGenerator.incrementSelection())
    { tempSel.init(this->kModules.size);
      for (int j=0; j<this->CentralizerRoots.size; j++)
        if (this->rootIsInNilradicalParabolicCentralizer(ParabolicsGenerator, this->CentralizerRoots.TheObjects[j]))
          tempSel.AddSelectionAppendNewIndex(j);
      if (owner.flagUsingActionsNormalizerCentralizerNilradical)
        owner.RaiseSelectionUntilApproval(tempSel, theGlobalVariables);
      StartingNilradicalsNoRepetition.AddOnTopNoRepetition(tempSel);
      //StartingNilradicalsNoRepetition.AddObjectOnTop(tempSel);
    }

/*      tempRootsTest.size=0;
      std::string tempS; std::stringstream out;
      for (int s=0; s<impliedSelections.TheObjects[0].CardinalitySelection; s++)
        tempRootsTest.AddObjectOnTop(this->kModules.TheObjects[impliedSelections.TheObjects[0].elements[s]].TheObjects[0]);
      tempS=out.str();
      assert(this->RootsDefineASubalgebra(tempRootsTest));*/
    for (; parabolicsCounter<StartingNilradicalsNoRepetition.size; parabolicsCounter++, owner.flagNilradicalComputationInitialized=false)
    { if (!owner.flagNilradicalComputationInitialized)
      { impliedSelections.TheObjects[0].Assign(StartingNilradicalsNoRepetition.TheObjects[parabolicsCounter]);
        owner.RecursionDepthNilradicalsGeneration=0;
        owner.CountersNilradicalsGeneration.TheObjects[0]=this->CentralizerRoots.size;
      }
      this->GeneratePossibleNilradicalsRecursive(PauseMutex, theGlobalVariables, this->theMultTable, impliedSelections, this->theOppositeKmods, owner, indexInOwner);
    }
  }
  else
  { this->flagFirstRoundCounting=false;
    impliedSelections.TheObjects[0].init(this->kModules.size);
    owner.RecursionDepthNilradicalsGeneration=0;
    owner.CountersNilradicalsGeneration.TheObjects[0]=0;
    this->GeneratePossibleNilradicalsRecursive(PauseMutex, theGlobalVariables, this->theMultTable, impliedSelections, this->theOppositeKmods, owner, indexInOwner);
  }
}

void coneRelations::WriteToFile(std::fstream& output, GlobalVariables& theGlobalVariables)
{ output << "num_rels: " << this->size << "\n";
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].WriteToFile(output, theGlobalVariables);
}

void coneRelations::ReadFromFile(std::fstream& input, GlobalVariables& theGlobalVariables, rootSubalgebras& owner)
{ std::string tempS; int tempI;
  this->ClearTheObjects();
  input >> tempS >> tempI;
  coneRelation tempRel;
  for (int i=0; i<tempI; i++)
  { tempRel.ReadFromFile(input, theGlobalVariables, owner);
    this->AddRelationNoRepetition(tempRel, owner, tempRel.IndexOwnerRootSubalgebra);
  }
}

void coneRelation::WriteToFile(std::fstream& output, GlobalVariables& theGlobalVariables)
{ this->AlphaCoeffs.WriteToFile(output);
  this->Alphas.WriteToFile(output, theGlobalVariables);
  output << this->AlphaKComponents;
  this->BetaCoeffs.WriteToFile(output);
  this->Betas.WriteToFile(output, theGlobalVariables);
  output << this->BetaKComponents;
  output << "Index_owner_root_SA: " << this->IndexOwnerRootSubalgebra << " ";
}

void coneRelation::ReadFromFile(std::fstream& input, GlobalVariables& theGlobalVariables, rootSubalgebras& owner)
{ std::string tempS;
  this->AlphaCoeffs.ReadFromFile(input);
  this->Alphas.ReadFromFile(input, theGlobalVariables);
  input >> this->AlphaKComponents;
  this->BetaCoeffs.ReadFromFile(input);
  this->Betas.ReadFromFile(input, theGlobalVariables);
  input >> this->BetaKComponents;
  input >> tempS >> this->IndexOwnerRootSubalgebra;
  assert(tempS=="Index_owner_root_SA:");
  this->ComputeTheDiagramAndDiagramRelAndK(owner.TheObjects[this->IndexOwnerRootSubalgebra]);
  this->ComputeDebugString(owner, true, true);
}

void WeylGroup::WriteToFile(std::fstream& output)
{ output << "Weyl_group: ";
  output << this->WeylLetter << " " << this->CartanSymmetric.NumRows << "\n";
  output << "Long_root_length: ";
  this->LongRootLength.WriteToFile(output);
  output << "\n";
  this->CartanSymmetric.WriteToFile(output);
}

void WeylGroup::ReadFromFile(std::fstream& input)
{ std::string tempS; int tempI;
  input >> tempS;
  input >> this->WeylLetter >> tempI >> tempS;
  assert(tempS=="Long_root_length:");
  this->LongRootLength.ReadFromFile(input);
  this->CartanSymmetric.ReadFromFile(input);
}

bool CombinatorialChamber::ElementToString(std::string& output, CombinatorialChamberContainer& owner, bool useLatex, bool useHtml)
{ std::stringstream out;
  std::string tempS;
  //assert(this->ExternalWalls->size== this->ExternalWallsNormals.size);
  this->SortNormals();
  std::string endOfLine;
  endOfLine="\n";
  if (useLatex)
    endOfLine.assign("\\\\\n");
  if(useHtml)
    endOfLine.assign("\n<br>\n");
  this->ChamberNumberToString(tempS, owner);
  if(!useHtml)
    out << tempS << "\n";
  else
    out << "<a name=\"" << tempS << "\">" << tempS << "</a>";
  if (useLatex)
    out << endOfLine << "Projective representation\n\n";
  out << " index in owner: " << this->IndexInOwnerComplex << "\n";
  out << "Hash_id: " << this->GetHashFromSortedNormals() << "\n";
  this->ElementToInequalitiesString(tempS, owner, useLatex, useHtml);
  out << tempS;
  out << "Neighbors: ";
  for (int k=0; k<this->Externalwalls.size; k++)
  { int theWallIndex= this->IndicesCorrespondingNonSortedWalls.TheObjects[k];
    out << "wall " << k+1 << ": ";
    for (int j=0; j<this->Externalwalls.TheObjects[theWallIndex].NeighborsAlongWall.size; j++)
    { CombinatorialChamber* currentChamber = this->Externalwalls.TheObjects[theWallIndex].NeighborsAlongWall.TheObjects[j];
      if (currentChamber!=0)
      { currentChamber->ChamberNumberToString(tempS, owner);
        if (useHtml)
          out << "<a href=\"#" << tempS << "\">";
        out << tempS << ", ";
        if (useHtml)
          out << "</a>";
      }
    }
  }
  PolyFormatLocal.cutOffString=false;
  if (this->flagIncludeVerticesInDebugString)
  { out << endOfLine << "Vertices: ";
    for (int i=0; i<this->AllVertices.size; i++)
    { this->AllVertices.TheObjects[i].ElementToString(tempS);
      out << tempS << ", ";
    }
  }
  out << endOfLine;
  if (!owner.flagDrawingProjective)
  { if (useLatex)
    out << "Affine data" << endOfLine;
    out << "Affine walls: " << endOfLine;
    for (int i=0; i<this->affineExternalWalls.size; i++)
    { out << "wall" << i+1 << ": ";
      this->affineExternalWalls.TheObjects[i].ElementToString(tempS);
      out << tempS << endOfLine;
    }
    out << "Affine vertices (true): " << endOfLine;
    for (int i=0; i<affineVertices.size; i++)
    { if (i==this->NumTrueAffineVertices)
        out << "Affine vertices representing pt(s) at infty: " << endOfLine;
      this->affineVertices.TheObjects[i].ElementToString(tempS);
      out << tempS << endOfLine;
    }
  }
  output=out.str();
  return true;
}

void ComputationSetup::ChamberSlice(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ if (inputData.thePartialFraction.theChambers.thePauseController.IsRunning())
    return;
  inputData.thePartialFraction.theChambers.thePauseController.InitComputation();
  inputData.thePartialFraction.theChambers.ReadFromDefaultFile(theGlobalVariables);
  inputData.thePartialFraction.theChambers.theDirections.ReverseOrderElements();
  root tempRoot;
  tempRoot.MakeZero(inputData.thePartialFraction.theChambers.AmbientDimension);
  theGlobalVariables.theDrawingVariables.theBuffer.init();
  if (inputData.flagChopFully)
    inputData.thePartialFraction.theChambers.SliceAndComputeDebugString(theGlobalVariables, inputData.thePartialFraction.theChambers.flagSpanTheEntireSpace);
  else if (inputData.flagChopOneDirection)
  { inputData.thePartialFraction.theChambers.SliceOneDirection(0, theGlobalVariables);
    inputData.thePartialFraction.theChambers.ComputeDebugString(false, false);
    inputData.thePartialFraction.theChambers.drawOutput(theGlobalVariables.theDrawingVariables, tempRoot, 0);
  } else
  { inputData.thePartialFraction.theChambers.OneSlice(0, theGlobalVariables);
    inputData.thePartialFraction.theChambers.ComputeDebugString(false, false);
    inputData.thePartialFraction.theChambers.drawOutput(theGlobalVariables.theDrawingVariables, tempRoot, 0);
  }
  if (inputData.thePartialFraction.theChambers.theCurrentIndex>= inputData.thePartialFraction.theChambers.theDirections.size)
  { inputData.thePartialFraction.theChambers.PurgeZeroPolyChambers(theGlobalVariables);
    inputData.thePartialFraction.theChambers.ComputeDebugString(false);
    theGlobalVariables.theDrawingVariables.theBuffer.init();
    inputData.thePartialFraction.theChambers.drawOutput(theGlobalVariables.theDrawingVariables, tempRoot, 0);
  }
  inputData.thePartialFraction.theChambers.thePauseController.ExitComputation();
}

void CombinatorialChamberContainer::SliceAndComputeDebugString(GlobalVariables& theGlobalVariables, bool SpanTheEntireSpace)
{ this->SliceTheEuclideanSpace(theGlobalVariables, SpanTheEntireSpace);
  this->QuickSortAscending();
  this->LabelChamberIndicesProperly();
  this->PurgeInternalWalls();
  this->ComputeDebugString(false);
  root tempRoot;
  theGlobalVariables.theDrawingVariables.theBuffer.init();
  tempRoot.MakeZero(this->AmbientDimension);
  this->drawOutput(theGlobalVariables.theDrawingVariables, tempRoot, 0);
}

void ComputationSetup::TestUnitCombinatorialChamberHelperFunction(std::stringstream& logstream, char WeylLetter, int Dimension, ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ std::string tempS;
  inputData.thePartialFraction.theChambers.flagUsingStartingConesSeparation=false;
  inputData.thePartialFraction.theChambers.SetupBorelAndSlice(WeylLetter, Dimension, false, theGlobalVariables, false);
  tempS= inputData.thePartialFraction.theChambers.DebugString;
  inputData.thePartialFraction.theChambers.SetupBorelAndSlice(WeylLetter, Dimension, true, theGlobalVariables, false);
  if (tempS!=inputData.thePartialFraction.theChambers.DebugString)
    logstream << WeylLetter << Dimension << " reverse order no span IS DIFFERENT FROM regular order no span !!! Total chambers: " << inputData.thePartialFraction.theChambers.size << "\n";
  else
    logstream << WeylLetter << Dimension << " reverse order no span same as regular order no span. Total chambers: " << inputData.thePartialFraction.theChambers.size <<"\n";
  inputData.thePartialFraction.theChambers.SetupBorelAndSlice(WeylLetter, Dimension, false, theGlobalVariables, true);
  inputData.thePartialFraction.theChambers.PurgeZeroPolyChambers(theGlobalVariables);
  inputData.thePartialFraction.theChambers.ComputeDebugString(false);
  if (tempS!=inputData.thePartialFraction.theChambers.DebugString)
    logstream << WeylLetter << Dimension << " span entire space regular order full span IS DIFFERENT FROM regular order no span!!! Total chambers: " << inputData.thePartialFraction.theChambers.size << "\n";
  else
    logstream << WeylLetter << Dimension << " span entire space regular order full span same as regular order no span. Total chambers: " << inputData.thePartialFraction.theChambers.size <<"\n";
  inputData.thePartialFraction.theChambers.SetupBorelAndSlice(WeylLetter, Dimension, true, theGlobalVariables, true);
  inputData.thePartialFraction.theChambers.PurgeZeroPolyChambers(theGlobalVariables);
  inputData.thePartialFraction.theChambers.ComputeDebugString(false);
  if (tempS!=inputData.thePartialFraction.theChambers.DebugString)
    logstream << WeylLetter << Dimension << " span entire space reverse order full span IS DIFFERENT FROM regular order no span !!! Total chambers: " << inputData.thePartialFraction.theChambers.size << "\n";
  else
    logstream << WeylLetter << Dimension << " span entire space reverse order full span same as regular order no span. Total chambers: " << inputData.thePartialFraction.theChambers.size <<"\n";

  inputData.thePartialFraction.theChambers.flagUsingStartingConesSeparation=true;

  inputData.thePartialFraction.theChambers.SetupBorelAndSlice(WeylLetter, Dimension, true, theGlobalVariables, false);
  if (tempS!=inputData.thePartialFraction.theChambers.DebugString)
    logstream << WeylLetter << Dimension << " reverse order no span with cone separation optimization IS DIFFERENT FROM regular order no span !!! Total chambers: " << inputData.thePartialFraction.theChambers.size << "\n";
  else
    logstream << WeylLetter << Dimension << " reverse order no span with cone separation optimization same as regular order no span. Total chambers: " << inputData.thePartialFraction.theChambers.size <<"\n";
  inputData.thePartialFraction.theChambers.SetupBorelAndSlice(WeylLetter, Dimension, true, theGlobalVariables, true);
  inputData.thePartialFraction.theChambers.PurgeZeroPolyChambers(theGlobalVariables);
  inputData.thePartialFraction.theChambers.ComputeDebugString(false);
  if (tempS!=inputData.thePartialFraction.theChambers.DebugString)
    logstream << WeylLetter << Dimension << " span entire space reverse order full span with cone separation optimization IS DIFFERENT FROM regular order no span !!! Total chambers: " << inputData.thePartialFraction.theChambers.size << "\n";
  else
    logstream << WeylLetter << Dimension << " span entire space reverse order full span with cone separation optimization same as regular order no span. Total chambers: " << inputData.thePartialFraction.theChambers.size <<"\n";

  theGlobalVariables.theIndicatorVariables.StatusString1= logstream.str();
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.MakeReport();
}

void ComputationSetup::TestUnitCombinatorialChambersChambers(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  inputData.TestUnitCombinatorialChamberHelperFunction(out, 'A', 2, inputData, theGlobalVariables);
  inputData.TestUnitCombinatorialChamberHelperFunction(out, 'A', 3, inputData, theGlobalVariables);
  inputData.TestUnitCombinatorialChamberHelperFunction(out, 'B', 2, inputData, theGlobalVariables);
  inputData.TestUnitCombinatorialChamberHelperFunction(out, 'B', 3, inputData, theGlobalVariables);
  inputData.TestUnitCombinatorialChamberHelperFunction(out, 'C', 3, inputData, theGlobalVariables);
  inputData.TestUnitCombinatorialChamberHelperFunction(out, 'A', 4, inputData, theGlobalVariables);
  inputData.TestUnitCombinatorialChamberHelperFunction(out, 'D', 4, inputData, theGlobalVariables);
  inputData.TestUnitCombinatorialChamberHelperFunction(out, 'B', 4, inputData, theGlobalVariables);
  inputData.TestUnitCombinatorialChamberHelperFunction(out, 'A', 5, inputData, theGlobalVariables);
}

int CombinatorialChamber::GetHashFromSortedNormals()
{ this->SortNormals();
  int result=0;
  int topBound= MathRoutines::Minimum(SomeRandomPrimesSize, this->ExternalwallsNormalsSorted.size);
  for (int i=0; i<topBound; i++)
    result+=SomeRandomPrimes[i]*this->ExternalwallsNormalsSorted.TheObjects[i].HashFunction();
  return result;
}

void CombinatorialChamber::SortNormals()
{ if (this->flagNormalsAreSorted)
    return;
  roots originalRoots;
  this->ExternalwallsNormalsSorted.SetSizeExpandOnTopNoObjectInit(this->Externalwalls.size);
  originalRoots.SetSizeExpandOnTopNoObjectInit(this->Externalwalls.size);
  this->IndicesCorrespondingNonSortedWalls.SetSizeExpandOnTopNoObjectInit(this->Externalwalls.size);
  for (int i=0; i<this->Externalwalls.size; i++)
  { this->ExternalwallsNormalsSorted.TheObjects[i] = this->Externalwalls.TheObjects[i].normal;
    originalRoots.TheObjects[i] = this->Externalwalls.TheObjects[i].normal;
  }
  this->ExternalwallsNormalsSorted.QuickSortAscending();
  for (int i=0; i<this->ExternalwallsNormalsSorted.size; i++)
    this->IndicesCorrespondingNonSortedWalls.TheObjects[i]=originalRoots.IndexOfObject(this->ExternalwallsNormalsSorted.TheObjects[i]);
}

bool CombinatorialChamber::operator>(CombinatorialChamber& right)
{ //used to allow to sort lists of pointers including eventually zero pointers. This code might be changed in the future (although it seems safe and fine to me now).
  if (this==0)
    return false;
  if (!this->flagHasZeroPolynomiaL && right.flagHasZeroPolynomiaL)
    return true;
  if (this->flagHasZeroPolynomiaL && !right.flagHasZeroPolynomiaL)
    return false;
  if (this->Externalwalls.size> right.Externalwalls.size)
    return true;
  if (this->Externalwalls.size<right.Externalwalls.size)
    return false;
  this->SortNormals();
  right.SortNormals();
  for (int i=0; i<this->Externalwalls.size; i++)
  { if (this->ExternalwallsNormalsSorted.TheObjects[i] > right.ExternalwallsNormalsSorted.TheObjects[i])
      return true;
    if (right.ExternalwallsNormalsSorted.TheObjects[i] > this->ExternalwallsNormalsSorted.TheObjects[i])
      return false;
  }
  return false;
}

void CombinatorialChamber::ReplaceMeByAddExtraWallsToNewChamber(CombinatorialChamberContainer& owner, CombinatorialChamber* newChamber, List<int>& IndicesGluedChambers, roots& redundantNormals)
{ //owner.LabelChambersForDisplayAndGetNumVisibleChambers();
  //this->ComputeDebugString(owner);
  //newChamber->ComputeDebugString(owner);
  for (int i=0; i<this->Externalwalls.size; i++)
  { WallData& currentWall =  this->Externalwalls.TheObjects[i];
    if (!redundantNormals.ContainsObject(currentWall.normal))
    { int indexWallInNewChamber=newChamber->GetIndexWallWithNormal(currentWall.normal);
      if (indexWallInNewChamber==-1)
      { indexWallInNewChamber=newChamber->Externalwalls.size;
        newChamber->Externalwalls.AddObjectOnTopCreateNew();
        newChamber->Externalwalls.LastObject()->normal.Assign(currentWall.normal);
        newChamber->Externalwalls.LastObject()->NeighborsAlongWall.size=0;
        newChamber->Externalwalls.LastObject()->IndicesMirrorWalls.size=0;
      }
      WallData& WalllnNewChamber= newChamber->Externalwalls.TheObjects[indexWallInNewChamber];
      for (int j=0; j<currentWall.NeighborsAlongWall.size; j++)
      { CombinatorialChamber* other= currentWall.NeighborsAlongWall.TheObjects[j];
        WallData& otherWall = other->Externalwalls.TheObjects[currentWall.IndicesMirrorWalls.TheObjects[j]];
        otherWall.SubstituteNeighborOneAllowNeighborAppearingNotOnce(this, newChamber, indexWallInNewChamber);
        WalllnNewChamber.AddNeighbor(other, currentWall.IndicesMirrorWalls.TheObjects[j]);
        otherWall.RemoveNeighborExtraOcurrences(newChamber);
        WalllnNewChamber.RemoveNeighborExtraOcurrences(other);
        //other->ComputeDebugString(owner);
      }
    }
//    this->ComputeDebugString(owner);
//    newChamber->ComputeDebugString(owner);
  }
}

void partFractions::DoTheFullComputation(GlobalVariables& theGlobalVariables, roots& toBePartitioned)
{ if (toBePartitioned.size<1)
    return;
  this->AmbientDimension= toBePartitioned.TheObjects[0].size;
  this->theChambers.AmbientDimension= this->AmbientDimension;
  this->theChambers.theDirections.CopyFromBase(toBePartitioned);
  this->DoTheFullComputation(theGlobalVariables);
}

void partFractions::DoTheFullComputation(GlobalVariables& theGlobalVariables)
{ this->theChambers.thePauseController.InitComputation();
  this->theChambers.ReadFromDefaultFile(theGlobalVariables);
  this->theChambers.SliceTheEuclideanSpace(theGlobalVariables, false);
  this->theChambers.QuickSortAscending();
  this->theChambers.LabelChamberIndicesProperly();
  root tempRoot; tempRoot.MakeZero(this->AmbientDimension);
  tempRoot.MakeZero(this->AmbientDimension);
  this->theChambers.drawOutput(theGlobalVariables.theDrawingVariables, tempRoot, 0);
  this->theChambers.thePauseController.ExitComputation();
  this->initAndSplit(this->theChambers.theDirections, theGlobalVariables);
  QuasiPolynomial tempQP;
  for (int i=0; i<this->theChambers.size; i++)
    if (this->theChambers.TheObjects[i]!=0)
      this->partFractionsToPartitionFunctionAdaptedToRoot(tempQP, this->theChambers.TheObjects[i]->InternalPoint, false, false, theGlobalVariables, true);
}

bool CombinatorialChamber::GetSplittingFacet(root& output, CombinatorialChamberContainer& owner, GlobalVariables& theGlobalVariables)
{ root& currentDirection= owner.theDirections.TheObjects[owner.theCurrentIndex];
  bool tempBool;
  for (int i=0; i<this->Externalwalls.size; i++)
    if (this->Externalwalls.TheObjects[i].IsExternalWithRespectToDirection(currentDirection) && this->Externalwalls.TheObjects[i].EveryNeigborIsExplored(tempBool))
      for (int j=i+1; j<this->Externalwalls.size; j++)
        if (this->Externalwalls.TheObjects[j].IsExternalWithRespectToDirection(currentDirection) && this->Externalwalls.TheObjects[j].EveryNeigborIsExplored(tempBool))
          if (this->MakeFacetFromEdgeAndDirection(this->Externalwalls.TheObjects[i], this->Externalwalls.TheObjects[j], owner, currentDirection, owner.theDirections, owner.theCurrentIndex, output, theGlobalVariables))
            if (this->HasHPositiveAndHNegativeVertex(output))
              return true;
  return false;
}

bool CombinatorialChamber::HasHPositiveAndHNegativeVertex(root& h)
{ bool foundPositive=false; bool foundNegative=false;
  for (int j=0; j<this->AllVertices.size; j++)
  { if (!foundPositive)
      if (h.OurScalarProductIsPositive(this->AllVertices.TheObjects[j]))
        foundPositive=true;
      if (!foundNegative)
        if (h.OurScalarProductIsNegative(this->AllVertices.TheObjects[j]))
          foundNegative=true;
    if (foundNegative&& foundPositive)
      return true;
  }
  return false;
}

bool CombinatorialChamber::SliceInDirection(root& direction, roots& directions, int CurrentIndex, CombinatorialChamberContainer& output, hashedRoots& FacetOutput, GlobalVariables& theGlobalVariables)
{ if (!(this->Externalwalls.size>=output.AmbientDimension))
  { this->ComputeDebugString(output);
//    CombinatorialChamberContainer::TheBigDump<<this->DebugString;
  }
  assert(this->Externalwalls.size>=output.AmbientDimension);
  this->flagExplored=false;
  root KillerFacet;
  if (this->InternalWalls.size!=0)
  { if (this->SplitChamber(this->InternalWalls.TheObjects[0], output, direction, theGlobalVariables))
      return true;
    else
      this->InternalWalls.PopIndexSwapWithLast(0);
    return false;
  } else
    if (this->GetSplittingFacet(KillerFacet, output, theGlobalVariables))
      if (this->SplitChamber(KillerFacet, output, direction, theGlobalVariables))
        return true;
  this->ComputeDebugString(output);
  direction.ComputeDebugString();
  assert(this->TestPossibilityToSlice(direction, output));
  this->flagExplored=true;
  output.NumExplored++;
  if (this->flagHasZeroPolynomiaL && !this->flagPermanentlyZero)
    this->flagHasZeroPolynomiaL= !this->BordersViaExternalWRTDirectionNonZeroNeighbor(direction);
  return false;
}

void CombinatorialChamberContainer::initNextIndex()
{ this->PurgeZeroPointers();
  this->theCurrentIndex++;
  this->indexLowestNonCheckedForGlueing=0;
  this->NumTotalGlued=0;
  this->LabelAllUnexplored();
  if (this->theCurrentIndex<this->theDirections.size)
    this->ComputeNextIndexToSlice(this->theDirections.TheObjects[this->theCurrentIndex]);
}

void CombinatorialChamberContainer::OneSlice(root* theIndicatorRoot, GlobalVariables& theGlobalVariables)
{ //static int ProblemCounter=0;
  //ProblemCounter++;
  if (this->flagMustStop)
  { assert(this->ConsistencyCheck(true, theGlobalVariables));
    return;
  }
  if (this->theCurrentIndex==-1)
  { this->MakeStartingChambers(this->theDirections, theIndicatorRoot, theGlobalVariables);
  }
  else
  { if (this->theCurrentIndex<this->theDirections.size)
    { if(this->indexNextChamberToSlice!=-1)
      { if (this->TheObjects[this->indexNextChamberToSlice]!=0)
          if (this->TheObjects[this->indexNextChamberToSlice]->SliceInDirection(this->theDirections.TheObjects[this->theCurrentIndex], this->theDirections, this->theCurrentIndex, *this, this->theHyperplanes, theGlobalVariables))
          { assert(this->TheObjects[this->indexNextChamberToSlice]->HasNoNeighborsThatPointToThis());
            delete this->TheObjects[this->indexNextChamberToSlice];
  #ifdef CGIversionLimitRAMuse
    ParallelComputing::GlobalPointerCounter--;
    if (ParallelComputing::GlobalPointerCounter>::ParallelComputing::cgiLimitRAMuseNumPointersInList){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::ParallelComputing::cgiLimitRAMuseNumPointersInList; std::exit(0); }
  #endif
            this->TheObjects[this->indexNextChamberToSlice]=0;
          }
        if (this->theCurrentIndex<this->theDirections.size)
          this->ComputeNextIndexToSlice(this->theDirections.TheObjects[this->theCurrentIndex]);
      }
      else
      { this->PreferredNextChambers.size=0;
        if (this->theCurrentIndex<this->theDirections.size-1)
          this->indexLowestNonCheckedForGlueing=this->size;
        if (this->indexLowestNonCheckedForGlueing==0)
        { this->LabelChamberIndicesProperly();
          //if (this->flagMakeGrandMasterConsistencyCheck && this->theCurrentIndex==this->theDirections.size-1)
          //  this->GrandMasterConsistencyCheck(theGlobalVariables);
          assert(this->ConsistencyCheck(true, theGlobalVariables));
        }
        if (this->indexLowestNonCheckedForGlueing<this->size)
          this->GlueOverSubdividedChambersCheckLowestIndex(theGlobalVariables);
        else
        { this->initNextIndex();
          this->ConsistencyCheck(false, theGlobalVariables);
        }
      }
    }
    if (this->theCurrentIndex<this->theDirections.size)
      this->MakeReportOneSlice(theGlobalVariables, this->theCurrentIndex, this->theDirections.size, this->theDirections.TheObjects[this->theCurrentIndex]);
    //if (ProblemCounter>1024)
    //this->ComputeDebugString();
//    assert(this->ConsistencyCheck(false));
    //below follows the code to pause the computation
    this->thePauseController.SafePoint();
  }
}

bool CombinatorialChamber::GetNonSeparableChamberIndicesAppendList(CombinatorialChamberContainer& owner, List<int>& outputIndicesChambersToGlue, GlobalVariables& theGlobalVariables)
{ if (this->GetHashFromSortedNormals()==58238520)
    MathRoutines::KToTheNth(1,1);
  for (int i=0; i<this->Externalwalls.size; i++)
  { WallData& theWall= this->Externalwalls.TheObjects[i];
    for(int j=0; j<theWall.NeighborsAlongWall.size; j++)
      if (theWall.NeighborsAlongWall.TheObjects[j]!=0)
        if (!theWall.NeighborsAlongWall.TheObjects[j]->flagHasZeroPolynomiaL)
          if (theWall.NeighborsAlongWall.TheObjects[j]->IndexInOwnerComplex>=owner.indexLowestNonCheckedForGlueing)
            if (!outputIndicesChambersToGlue.ContainsObject(theWall.NeighborsAlongWall.TheObjects[j]->IndexInOwnerComplex))
              if (!this->IsSeparatedByStartingConesFrom(owner, *theWall.NeighborsAlongWall.TheObjects[j], theGlobalVariables))
              { outputIndicesChambersToGlue.AddObjectOnTop(theWall.NeighborsAlongWall.TheObjects[j]->IndexInOwnerComplex);
                theWall.NeighborsAlongWall.TheObjects[j]->GetNonSeparableChamberIndicesAppendList(owner, outputIndicesChambersToGlue, theGlobalVariables);
              }
  }
  return outputIndicesChambersToGlue.size>1;
}

bool CombinatorialChamberContainer::GrandMasterConsistencyCheck(GlobalVariables& theGlobalVariables)
{ roots OldVertices;
  roots tempRoots;
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i]!=0)
    { CombinatorialChamber& theChamber = *this->TheObjects[i];
      OldVertices.CopyFromBase(this->TheObjects[i]->AllVertices);
      this->TheObjects[i]->ComputeVerticesFromNormals(*this, theGlobalVariables);
      OldVertices.intersectWith(this->TheObjects[i]->AllVertices, tempRoots);
      if (tempRoots.size!=OldVertices.size || OldVertices.size!=this->TheObjects[i]->AllVertices.size)
      { this->TheObjects[i]->ComputeDebugString(*this);
        assert(false);
        return false;
      }
      /////////////////////////check that for each chamber and each of the starting cone, either the chamber and the cone have zero measure intersection, or the chamber lies entirely in the cone
      for (int j=0; j<this->startingCones.size; j++)
      { if (!this->startingCones.TheObjects[j].IsInCone(theChamber.AllVertices))
          if (theChamber.ChambersHaveNonZeroMeasureIntersection(this->startingCones.TheObjects[j], theGlobalVariables))
          { theChamber.ComputeDebugString(*this);
            this->startingCones.TheObjects[j].ComputeDebugString();
            assert(false);
            return false;
          }
        std::stringstream out;
        out << "Checking consistency chamber " << i+1 << " out of " << this->size << " cone " << j+1 << " out of " << this->startingCones.size;
        theGlobalVariables.theIndicatorVariables.ProgressReportString1= out.str();
        theGlobalVariables.MakeReport();
      }
    }
  return true;
}

bool CombinatorialChamber::ChambersHaveNonZeroMeasureIntersection(roots& WallsOther, GlobalVariables& theGlobalVariables)
{ Selection VertexSelection;
  if (this->Externalwalls.size<1)
    return false;
  int theDimension = this->Externalwalls.TheObjects[0].normal.size;
  roots theWalls;
  this->GetWallNormals(theWalls);
  theWalls.AddRootSnoRepetition(WallsOther);
  int numCandidates = MathRoutines::NChooseK(theWalls.size, theDimension-1);
  root tempRoot; root newInternalPoint;
  newInternalPoint.MakeZero(theDimension);
  VertexSelection.init(theWalls.size);
  for (int i=0; i<numCandidates; VertexSelection.incrementSelectionFixedCardinality(theDimension-1), i++)
    if (theWalls.ComputeNormalFromSelection(tempRoot, VertexSelection, theGlobalVariables, theDimension))
      if (this->PlusMinusPointIsInChamberModifyInput(tempRoot))
        if (WallsOther.ElementsHaveNonNegativeScalarProduct(tempRoot))
        { newInternalPoint.Add(tempRoot);
          if (this->PointIsStrictlyInsideChamber(newInternalPoint) && WallsOther.ElementsHavePositiveScalarProduct(newInternalPoint))
            return true;
        }
  return false;
}

bool roots::ElementsHaveNonNegativeScalarProduct(const root& theRoot) const
{ for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].OurScalarProductIsNegative(theRoot))
      return false;
  return true;
}

bool roots::ElementsHaveNonPositiveScalarProduct(const root& theRoot) const
{ for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].OurScalarProductIsPositive(theRoot))
      return false;
  return true;
}

bool roots::ElementsHavePositiveScalarProduct(const root& theRoot) const
{ for (int i=0; i<this->size; i++)
    if (!this->TheObjects[i].OurScalarProductIsPositive(theRoot))
      return false;
  return true;
}

void ComputationSetup::ProverOpenAndGo(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ inputData.theProverFixedK.ReadFromFile(inputData.theProverFixedK.ProverFileName, theGlobalVariables);
  inputData.theProver.ReadFromFile(theGlobalVariables);
  inputData.theProver.TheFullRecursion(inputData.theProver.theWeylGroup, theGlobalVariables);
}

void CombinatorialChamberContainer::MakeStartingChambersDontSpanEntireSpace(roots& directions, root* theIndicatorRoot, GlobalVariables& theGlobalVariables)
{ this->initAndCreateNewObjects(1);
  CombinatorialChamber& theStartingChamber= *this->TheObjects[0];
  theStartingChamber.Externalwalls.SetSizeExpandOnTopNoObjectInit(this->TheGlobalConeNormals.size);
  theStartingChamber.flagHasZeroPolynomiaL=false;
  theStartingChamber.flagPermanentlyZero=false;
  for (int i=0; i<this->TheGlobalConeNormals.size; i++)
    theStartingChamber.Externalwalls.TheObjects[i].normal.Assign(this->TheGlobalConeNormals.TheObjects[i]);
  this->StartingCrossSections.SetSizeExpandOnTopNoObjectInit(1);
  theStartingChamber.IndexStartingCrossSectionNormal=0;
  this->TheGlobalConeNormals.Average(this->StartingCrossSections.TheObjects[0].normal, this->AmbientDimension);
  this->StartingCrossSections.TheObjects[0].affinePoint.Assign(directions.TheObjects[0]);
  Selection tempSel;
  tempSel.init(this->TheGlobalConeNormals.size);
  roots tempRoots;
  root tempRoot;
  for (int i=0; i<directions.size; i++)
    if (this->TheGlobalConeNormals.PointIsAVertex(directions.TheObjects[i], theGlobalVariables))
      theStartingChamber.AllVertices.AddObjectOnTop(directions.TheObjects[i]);
  tempSel.init(this->AmbientDimension);
  tempRoots.CopyFromBase(this->theDirections);
  tempRoots.size=this->AmbientDimension;
  for (int i=0; i<this->AmbientDimension; i++)
  { tempRoots.ComputeNormalExcludingIndex(tempRoot, i, theGlobalVariables);
    tempRoot.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
    theStartingChamber.InternalWalls.AddObjectOnTop(tempRoot);
  }
  this->initNextIndex();
}

void CombinatorialChamberContainer::MakeStartingChambersSpanEntireSpace(roots& directions, root* theIndicatorRoot, GlobalVariables& theGlobalVariables)
{ Selection theSelection;
  theSelection.init(this->AmbientDimension);
  this->theHyperplanes.ClearTheObjects();
  this->theHyperplanes.MakeActualSizeAtLeastExpandOnTop(MathRoutines::NChooseK(directions.size, this->AmbientDimension-1));
  for(int i=0; i<this->AmbientDimension; i++)
  { roots TempRoots;
    root TempRoot;
    Rational tempRat;
    theSelection.incrementSelectionFixedCardinality(this->AmbientDimension-1);
    directions.SubSelection(theSelection, TempRoots);
    TempRoots.ComputeNormal(TempRoot);
    TempRoot.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
    this->theHyperplanes.AddObjectOnTopHash(TempRoot);
  }
  theSelection.initNoMemoryAllocation();
  int NumStartingChambers=MathRoutines::TwoToTheNth(this->AmbientDimension);
  this->initAndCreateNewObjects(NumStartingChambers);
  this->StartingCrossSections.SetSizeExpandOnTopNoObjectInit(NumStartingChambers);
  for(int i=0; i<NumStartingChambers; i++)
  { int tempI= theSelection.SelectionToIndex();
    this->TheObjects[tempI]->Externalwalls.MakeActualSizeAtLeastExpandOnTop(this->AmbientDimension);
    for (int j=0; j<this->AmbientDimension; j++)
    { this->TheObjects[tempI]->AllVertices.AddRoot(directions.TheObjects[j]);
      if (theSelection.selected[j])
      { int tempI2;
        theSelection.selected[j]=false;
        tempI2=theSelection.SelectionToIndex();
        theSelection.selected[j]=true;
        CombinatorialChamber* plusOwner; CombinatorialChamber* minusOwner;
        if (this->theHyperplanes.TheObjects[this->AmbientDimension-j-1].OurScalarProductIsPositive(directions.TheObjects[j]))
        { plusOwner= this->TheObjects[tempI];
          minusOwner= this->TheObjects[tempI2];
        } else
        { minusOwner= this->TheObjects[tempI];
          plusOwner= this->TheObjects[tempI2];
        }
        this->TheObjects[tempI]->MakeNewMutualNeighbors(plusOwner, minusOwner, this->theHyperplanes.TheObjects[this->AmbientDimension-j-1]);
      } else
        this->TheObjects[tempI]->AllVertices.LastObject()->MinusRoot();
//      if (this->flagAnErrorHasOcurredTimeToPanic)
  //      this->ComputeDebugString();
    }
    root tempRoot;
    tempRoot.Assign(directions.TheObjects[0]);
    if (!theSelection.selected[0])
      tempRoot.MinusRoot();
    this->StartingCrossSections.TheObjects[tempI].affinePoint.Assign(tempRoot);
    theSelection.incrementSelection();
//    if (this->flagAnErrorHasOcurredTimeToPanic)
//      this->ComputeDebugString();
  }
  for(int i=0; i<NumStartingChambers; i++)
  { this->TheObjects[i]->IndexStartingCrossSectionNormal=i;
    root Accum;  Accum.MakeZero(this->AmbientDimension);
    for (int j=0; j<this->TheObjects[i]->Externalwalls.size; j++)
      Accum.Add(this->TheObjects[i]->Externalwalls.TheObjects[j].normal);
    this->StartingCrossSections.TheObjects[i].normal.Assign(Accum);
//    if (this->flagAnErrorHasOcurredTimeToPanic)
//      Accum.ComputeDebugString();
    assert(this->TheObjects[i]->ConsistencyCheck(this->AmbientDimension, false, *this, theGlobalVariables));
    if (this->TheGlobalConeNormals.IsSurelyOutsideCone(this->TheObjects[i]->AllVertices))
      this->TheObjects[i]->flagPermanentlyZero=true;
    this->TheObjects[i]->AllVertices.Average(this->TheObjects[i]->InternalPoint, this->AmbientDimension);
  }
  this->TheObjects[NumStartingChambers-1]->flagHasZeroPolynomiaL=false;
  this->initNextIndex();
//  if (this->flagAnErrorHasOcurredTimeToPanic)
//    this->ComputeDebugString();
}

void CombinatorialChamberContainer::PurgeZeroPolyChambers(GlobalVariables& theGlobalVariables)
{ //this->ConsistencyCheck(false, theGlobalVariables);
  //this->WriteReportToFile("./Debug_A3_before_purge.html", false);
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i]!=0)
    { CombinatorialChamber& currentChamber= *this->TheObjects[i];
      if (currentChamber.flagHasZeroPolynomiaL || currentChamber.flagPermanentlyZero)
      { for (int j=0; j<currentChamber.Externalwalls.size; j++)
        { WallData& currentWall= currentChamber.Externalwalls.TheObjects[j];
          for (int k=0; k<currentWall.NeighborsAlongWall.size; k++)
            k-=currentWall.RemoveNeighborhoodBothSidesAllowRepetitionsReturnNeighborsDecrease(this->TheObjects[i], currentWall.NeighborsAlongWall.TheObjects[k]);
        }
      }
    }
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i]!=0)
    { CombinatorialChamber& currentChamber= *this->TheObjects[i];
      if (currentChamber.flagHasZeroPolynomiaL || currentChamber.flagPermanentlyZero)
      {
#ifdef CGIversionLimitRAMuse
  ParallelComputing::GlobalPointerCounter--;
  if (ParallelComputing::GlobalPointerCounter>::ParallelComputing::cgiLimitRAMuseNumPointersInList){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::ParallelComputing::cgiLimitRAMuseNumPointersInList; std::exit(0); }
#endif
        delete this->TheObjects[i];
        this->TheObjects[i]=0;
      }
    }
//  this->WriteReportToFile("./Debug_A3.html", false);
  //this->ConsistencyCheck(false, theGlobalVariables);
  this->PurgeZeroPointers();
  this->ConsistencyCheck(false, theGlobalVariables);
}

void rootSubalgebras::ElementToStringConeConditionNotSatisfying(std::string& output, bool includeMatrixForm, GlobalVariables& theGlobalVariables)
{ roots tempRoots, tempRoots2;
  std::stringstream out;
  std::stringstream out2;
  std::string tempS;
  int numNonSolvableNonReductive=0;
  if (this->AmbientWeyl.WeylLetter=='B')
    out << "$\\mathrm{so}(2n+1)$ is realized as a matrix Lie algebra as $\\left\\{\\left(\\begin{array}{c|c|c}A&\\begin{array}{c}v_1\\\\ \\vdots \\\\ v_n\\end{array} &C=-C^T \\\\\\hline \\begin{array}{ccc}w_1 &\\dots&  w_n\\end{array} &0& \\begin{array}{ccc}-v_n &\\dots&  -v_1\\end{array} \\\\\\hline D=-D^T&\\begin{array}{c}-w_n\\\\ \\vdots \\\\ -w_1\\end{array} & -A^T\\end{array}\\right)\\right\\}$.\n\n";
  if (this->AmbientWeyl.WeylLetter=='C')
    out << "$\\mathrm{sp}(2n)$ is realized as a matrix Lie algebra as $\\left\\{\\left(\\begin{array}{c|c}A& C \\\\\\hline D& -A^T\\end{array}\\right)| C=C^T, D=D^T\\right\\}.$";
  out << " In this realization, the Cartan subalgebra $\\mathfrak{h}$ can be chosen to consist of the diagonal matrices of the above form.\n\n";
  if (!includeMatrixForm)
  { out << "\n\\begin{longtable}{r|l}\n\\multicolumn{2}{c}{";
    if (this->AmbientWeyl.WeylLetter=='B')
      out << " $ \\mathfrak{g}\\simeq \\mathrm{so("<<this->AmbientWeyl.CartanSymmetric.NumRows*2+1 << ")}$";
    if (this->AmbientWeyl.WeylLetter=='C')
      out << " $\\mathfrak{g}\\simeq \\mathrm{sp("<<this->AmbientWeyl.CartanSymmetric.NumRows*2 << ")}$";
    out << "} \\\\\\hline";
  }
  for (int i=0; i<this->size-1; i++)
    if (this->storedNilradicals.TheObjects[i].size>0)
    { rootSubalgebra& currentRootSA=this->TheObjects[i];
      tempRoots.size=0;
      for (int j=0; j<currentRootSA.PositiveRootsK.size; j++)
      { tempRoots.AddObjectOnTop(currentRootSA.PositiveRootsK.TheObjects[j]);
        tempRoots.AddObjectOnTop(-currentRootSA.PositiveRootsK.TheObjects[j]);
      }
      if (includeMatrixForm)
        out << "\n\n\\noindent\\rule{\\textwidth}{1.5pt}\n\n";
      else
        out << "\\hline\\begin{tabular}{r}";
      out << "$\\Delta(\\mathfrak{k})$ is of type " << currentRootSA.theDynkinDiagram.DebugString << "; ";
      if (!includeMatrixForm)
        out <<"\\\\";
      currentRootSA.AmbientWeyl.GetEpsilonCoords(currentRootSA.PositiveRootsK, tempRoots2, theGlobalVariables);
      tempRoots2.ElementToStringEpsilonForm(tempS, true, false, false);
      out << " $\\Delta^+(\\mathfrak{k})=$ " << tempS;
      if (includeMatrixForm)
        out << "\n\n\\noindent\\rule{\\textwidth}{0.3pt}\n\n";
      else
        out << "\\end{tabular} &\n\\begin{tabular}{l}";
      int numNonReductiveCurrent=0;
      for (int j=0; j<this->storedNilradicals.TheObjects[i].size; j++)
      { List<int>& currentNilrad= this->storedNilradicals.TheObjects[i].TheObjects[j];
        if (currentNilrad.size>0)
        { numNonSolvableNonReductive++;
          numNonReductiveCurrent++;
          tempRoots.size= currentRootSA.PositiveRootsK.size*2;
          for(int k=0; k<currentNilrad.size; k++)
            tempRoots.AddListOnTop(currentRootSA.kModules.TheObjects[currentNilrad.TheObjects[k]]);
          this->ElementToStringRootSpaces(tempS, includeMatrixForm, tempRoots, theGlobalVariables);
          out << tempS << "\n";
          if (numNonReductiveCurrent%2==0)
          { out << "\n\n";
            if (!includeMatrixForm)
              out <<"\\\\";
          }
        }
      }
      if (!includeMatrixForm)
        out << "\\end{tabular}\n\n\\\\";
    }
  if (!includeMatrixForm)
    out << "\n\\end{longtable}";
  out2 << "\n\nThe number of non-conjugate non-solvable non-reductive root subalgebras not satisfying the cone condition is: " << numNonSolvableNonReductive << "\n\n";
  tempS = out.str();
  out2 << tempS;
  output=out2.str();
}

void rootSubalgebras::ElementToStringRootSpaces(std::string& output, bool includeMatrixForm, roots& input, GlobalVariables& theGlobalVariables)
{ std::string tempS; std::stringstream out;
  roots epsCoords;
  Matrix<int> tempMat;
  int theDimension=this->AmbientWeyl.CartanSymmetric.NumRows;
  if (this->AmbientWeyl.WeylLetter=='B')
  { this->AmbientWeyl.GetEpsilonCoords(input, epsCoords, theGlobalVariables);
    tempMat.MakeIdMatrix(theDimension*2+1);
    tempMat.elements[theDimension][theDimension]=0;
    for (int i=0; i<epsCoords.size; i++)
    { bool isShort=false;
      int firstIndex=-1;
      int secondIndex=-1;
      bool firstSignIsPositive=true;
      bool secondSignIsPositive=true;
      root& currentRoot=epsCoords.TheObjects[i];
      epsCoords.ComputeDebugString();
      for (int j=0; j<theDimension; j++)
      { if (currentRoot.TheObjects[j]!=0)
        { isShort=!isShort;
          if(isShort)
          { if (currentRoot.TheObjects[j].IsPositive())
              firstSignIsPositive=true;
            else
              firstSignIsPositive=false;
            firstIndex=j;
          } else
          { if (currentRoot.TheObjects[j].IsPositive())
              secondSignIsPositive=true;
            else
              secondSignIsPositive=false;
            secondIndex=j;
          }
        }
      }
      if (!isShort)
      { bool signsAreDifferent=(firstSignIsPositive!=secondSignIsPositive);
        if (signsAreDifferent)
        { int positiveIndex, negativeIndex;
          if (firstSignIsPositive)
          { positiveIndex= firstIndex;
            negativeIndex=secondIndex;
          } else
          { positiveIndex= secondIndex;
            negativeIndex=firstIndex;
          }
          tempMat.elements[positiveIndex][negativeIndex]=1;
          tempMat.elements[theDimension+1+negativeIndex][theDimension+1+positiveIndex]=-1;
        } else
        { if (firstSignIsPositive)
          { tempMat.elements[firstIndex][secondIndex+theDimension+1]=1;
            tempMat.elements[secondIndex][firstIndex+theDimension+1]=-1;
          } else
          { tempMat.elements[theDimension+1+firstIndex][secondIndex]=1;
            tempMat.elements[theDimension+1+secondIndex][firstIndex]=-1;
          }
        }
      } else
      { if (firstSignIsPositive)
        { tempMat.elements[firstIndex][theDimension]=1;
          tempMat.elements[theDimension][theDimension+1+firstIndex]=-1;
        } else
        { tempMat.elements[theDimension][firstIndex]=1;
          tempMat.elements[firstIndex+1+theDimension][theDimension]=-1;
        }
      }
    }
  }
  if (this->AmbientWeyl.WeylLetter=='C')
  { this->AmbientWeyl.GetEpsilonCoords(input, epsCoords, theGlobalVariables);
    tempMat.MakeIdMatrix(theDimension*2);
    for (int i=0; i<epsCoords.size; i++)
    { bool isLong=false;
      int firstIndex=-1;
      int secondIndex=-1;
      bool firstSignIsPositive=true;
      bool secondSignIsPositive=true;
      root& currentRoot=epsCoords.TheObjects[i];
      epsCoords.ComputeDebugString();
      for (int j=0; j<theDimension; j++)
      { if (currentRoot.TheObjects[j]!=0)
        { isLong=!isLong;
          if(isLong)
          { if (currentRoot.TheObjects[j].IsPositive())
              firstSignIsPositive=true;
            else
              firstSignIsPositive=false;
            firstIndex=j;
          } else
          { if (currentRoot.TheObjects[j].IsPositive())
              secondSignIsPositive=true;
            else
              secondSignIsPositive=false;
            secondIndex=j;
          }
        }
      }
      if (!isLong)
      { bool signsAreDifferent=(firstSignIsPositive!=secondSignIsPositive);
        if (signsAreDifferent)
        { int positiveIndex=-1, negativeIndex=-1;
          if (firstSignIsPositive)
          { positiveIndex= firstIndex;
            negativeIndex=secondIndex;
          } else
          { positiveIndex= secondIndex;
            negativeIndex=firstIndex;
          }
          tempMat.elements[positiveIndex][negativeIndex]=1;
          tempMat.elements[theDimension+negativeIndex][theDimension+positiveIndex]=-1;
        } else
        { if (firstSignIsPositive)
          { tempMat.elements[firstIndex][secondIndex+theDimension]=1;
            tempMat.elements[secondIndex][firstIndex+theDimension]=1;
          } else
          { tempMat.elements[theDimension+firstIndex][secondIndex]=1;
            tempMat.elements[theDimension+secondIndex][firstIndex]=1;
          }
        }
      } else
      { if (firstSignIsPositive)
          tempMat.elements[firstIndex][theDimension+firstIndex]=1;
        else
          tempMat.elements[theDimension+firstIndex][firstIndex]=1;
      }
    }
  }
  if (includeMatrixForm)
    out << "\\begin{tabular}{cc} \\begin{tabular}{l}";
  out << "$\\Delta(\\mathfrak{n})=$";
  if (includeMatrixForm)
    out <<"\\\\";
  int numNilradicalRootSpaces=0;
  for (int i=0; i<epsCoords.size; i++)
  { root& currentRoot=epsCoords.TheObjects[i];
    currentRoot.ElementToStringEpsilonForm(tempS, true, false);
    if (!epsCoords.ContainsObject(-currentRoot))
    { out << tempS << ", ";
      numNilradicalRootSpaces++;
    }
    if (includeMatrixForm)
      if (numNilradicalRootSpaces%2==0 && numNilradicalRootSpaces!=0)
        out << "\\\\";
  }
  if (includeMatrixForm)
  { out << "\\end{tabular} & $\\mathfrak{l}=\\left(\\begin{array}{";
    for (int i=0; i<tempMat.NumCols; i++)
    { out << "c";
      if (this->AmbientWeyl.WeylLetter=='B' && (i==theDimension-1 || i==theDimension))
        out  << "|";
    }
    out << "}";
    for (int i=0; i< tempMat.NumRows; i++)
    { if (this->AmbientWeyl.WeylLetter=='B' && (i==theDimension || i==theDimension+1))
        out  << "\\hline";
      for (int j=0; j<tempMat.NumCols; j++)
      { if (tempMat.elements[i][j]!=0 && tempMat.elements[j][i]==0)
          out << "\\blacktriangle";
        if (tempMat.elements[i][j]!=0 && tempMat.elements[j][i]!=0)
        out << "\\bullet";
        if (j!=tempMat.NumCols-1)
          out << "&";
        else
          out << "\\\\";
      }
    }
    out << "\\end{array}\\right)$ \\end{tabular}  ";
  }
  output=out.str();
}

void ComputationSetup::G2InD4Experiment(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  out << "\\documentclass{article}\\begin{document}";
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  SemisimpleLieAlgebra theLie;
  theLie.ComputeChevalleyConstants('G',  2, theGlobalVariables);
  theLie.ComputeDebugString(false, true, theGlobalVariables);
  out << theLie.DebugString;
  theLie.theWeyl.ComputeWeylGroup(-1);
  root tempHW="(0,1)";
  tempHW.ComputeDebugString();
  roots tempRoots;
  theLie.CreateEmbeddingFromFDModuleHaving1dimWeightSpaces(tempHW, theGlobalVariables);
  std::string tempS;
  if (theLie.CheckClosedness(tempS, theGlobalVariables))
    out << "Lie bracket is good! ";
  else
    out << tempS;
  theLie.ElementToStringEmbedding(tempS);
  out << "\n\n\n" << tempS << "\n\n\n";
 // theLie.
  MatrixLargeRational theAuto;
  theLie.ComputeChevalleyConstants('D', 4, theGlobalVariables);
  theLie.ComputeOneAutomorphism(theGlobalVariables, theAuto);
  theAuto.ElementToString(tempS, false, true);
  out << tempS;
  MatrixLargeRational tempMat;
  tempMat.MakeIdMatrix(theAuto.NumRows);
  theAuto= theAuto-tempMat;
  theAuto.FindZeroEigenSpace(tempRoots, theGlobalVariables);
  tempMat.AssignRootsToRowsOfMatrix(tempRoots);
  tempMat.Transpose(theGlobalVariables);
  tempMat= theAuto*tempMat;
  tempMat.ElementToString(tempS, false, true);
  std::string tempS2;
  tempRoots.ElementToString(tempS2, true, false, true);
  out << "\n\n" << tempS2 << "\n\n" << tempS;

  out << "\\end{document}";

  theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
  theGlobalVariables.MakeReport();
}

void MatrixLargeRational::FindZeroEigenSpace(roots& output, GlobalVariables& theGlobalVariables)
{ MatrixLargeRational tempMat;
  tempMat.Assign(*this);
  MatrixLargeRational emptyMat;
  Selection nonPivotPts;
  tempMat.GaussianEliminationByRows(tempMat, emptyMat, nonPivotPts);
  output.SetSizeExpandOnTopNoObjectInit(nonPivotPts.CardinalitySelection);
  for (int i=0; i<nonPivotPts.CardinalitySelection; i++)
  { root& current= output.TheObjects[i];
    current.MakeZero(this->NumCols);
    int currentPivotIndex = nonPivotPts.elements[i];
    current.TheObjects[currentPivotIndex]=1;
    int rowCounter=0;
    for (int j=0; j<this->NumCols; j++)
      if (!nonPivotPts.selected[j])
      { current.TheObjects[j]=-tempMat.elements[rowCounter][currentPivotIndex];
        rowCounter++;
      }
  }
}

void ElementSimpleLieAlgebra::AssignGeneratorCoeffOne(int generatorIndex, const SemisimpleLieAlgebra& owner)
{ int numPosRoots=owner.theWeyl.RootsOfBorel.size;
  int theDimension=owner.theWeyl.CartanSymmetric.NumRows;
  this->Nullify(owner);
  if(generatorIndex< numPosRoots)
  { this->SetCoefficient(owner.theWeyl.RootSystem.TheObjects[generatorIndex], 1, owner);
    return;
  }
  if (generatorIndex>=numPosRoots+theDimension)
  { this->SetCoefficient(owner.theWeyl.RootSystem.TheObjects[generatorIndex-theDimension], 1, owner);
    return;
  }
  this->Hcomponent.MakeEi(theDimension, generatorIndex-numPosRoots);
}

void ElementSimpleLieAlgebra::AssingVectorRootSpacesFirstThenCartan(const root& input, SemisimpleLieAlgebra& owner)
{ this->Nullify(owner);
  for (int i=0; i<this->coeffsRootSpaces.size; i++)
    this->coeffsRootSpaces.TheObjects[i]=input.TheObjects[i];
  for (int i=0; i<this->Hcomponent.size; i++)
   this->Hcomponent.TheObjects[i]=input.TheObjects[i+this->coeffsRootSpaces.size];
  this->ComputeNonZeroElements();
}

void ElementSimpleLieAlgebra::ElementToVectorRootSpacesFirstThenCartan(root& output)
{ output.MakeZero(this->coeffsRootSpaces.size+ this->Hcomponent.size);
  for (int i=0; i<this->NonZeroElements.CardinalitySelection; i++)
  { int theIndex=this->NonZeroElements.elements[i];
    output.TheObjects[theIndex]= this->coeffsRootSpaces.TheObjects[theIndex];
  }
  for (int i=0; i<this->Hcomponent.size; i++)
    output.TheObjects[i+this->coeffsRootSpaces.size]= this->Hcomponent.TheObjects[i];
}

void SemisimpleLieAlgebra::ComputeOneAutomorphism(GlobalVariables& theGlobalVariables, MatrixLargeRational& outputAuto)
{ rootSubalgebra theRootSA;
  theRootSA.AmbientWeyl.Assign(this->theWeyl);
  int theDimension= this->theWeyl.CartanSymmetric.NumRows;
  theRootSA.genK.MakeEiBasis(theDimension);
  ReflectionSubgroupWeylGroup theAutos;
  theRootSA.GenerateAutomorphismsPreservingBorel(theGlobalVariables, theAutos);
  MatrixLargeRational mapOnRootSpaces;
  int theAutoIndex= theAutos.ExternalAutomorphisms.size>1? 1 : 0;
  mapOnRootSpaces.AssignRootsToRowsOfMatrix(theAutos.ExternalAutomorphisms.TheObjects[theAutoIndex]);
  mapOnRootSpaces.Transpose(theGlobalVariables);
  mapOnRootSpaces.ComputeDebugString();
  Selection NonExplored;
  int numRoots= this->theWeyl.RootSystem.size;
  NonExplored.init(numRoots);
  NonExplored.MakeFullSelection();
  root domainRoot, rangeRoot;
  this->ComputeChevalleyConstants(this->theWeyl, theGlobalVariables);
  List<ElementSimpleLieAlgebra> Domain, Range;
  Range.SetSizeExpandOnTopNoObjectInit(numRoots+theDimension);
  Domain.SetSizeExpandOnTopNoObjectInit(numRoots+theDimension);
  ElementSimpleLieAlgebra tempElt;
  for (int i=0; i<theDimension; i++)
  { domainRoot.MakeEi(theDimension, i);
    mapOnRootSpaces.ActOnAroot(domainRoot, rangeRoot);
    tempElt.Nullify(*this);
    tempElt.Hcomponent= domainRoot;
    Domain.TheObjects[numRoots+i]=tempElt;
    tempElt.Hcomponent= rangeRoot;
    Range.TheObjects[numRoots+i]=tempElt;
    for (int i=0; i<2; i++, domainRoot.MinusRoot(), rangeRoot.MinusRoot())
    { int theIndex= this->theWeyl.RootSystem.IndexOfObjectHash(rangeRoot);
      tempElt.Nullify(*this);
      tempElt.SetCoefficient(rangeRoot, Rational::TheRingUnit, *this);
      Range.TheObjects[theIndex]=tempElt;
      tempElt.Nullify(*this);
      tempElt.SetCoefficient(domainRoot, Rational::TheRingUnit, *this);
      Domain.TheObjects[theIndex]=tempElt;
      NonExplored.RemoveSelection(theIndex);
    }
  }
  root left, right;
  while(NonExplored.CardinalitySelection>0)
  { for (int i=0; i<NonExplored.CardinalitySelection; i++)
    { int theIndex = NonExplored.elements[i];
      root& current = this->theWeyl.RootSystem.TheObjects[theIndex];
      for (int j=0; j<theDimension; j++)
      { left.MakeEi(theDimension, j);
        for (int k=0; k<2; k++, left.MinusRoot())
        { right= current-left;
          if (this->theWeyl.IsARoot(right))
          { int leftIndex= this->theWeyl.RootSystem.IndexOfObjectHash(left);
            int rightIndex= this->theWeyl.RootSystem.IndexOfObjectHash(right);
            if (!NonExplored.selected[rightIndex])
            { ElementSimpleLieAlgebra& leftDomainElt=Domain.TheObjects[leftIndex];
              ElementSimpleLieAlgebra& rightDomainElt= Domain.TheObjects[rightIndex];
              this->LieBracket(leftDomainElt, rightDomainElt, Domain.TheObjects[theIndex]);
              ElementSimpleLieAlgebra& leftRangeElt=Range.TheObjects[leftIndex];
              ElementSimpleLieAlgebra& rightRangeElt= Range.TheObjects[rightIndex];
              this->LieBracket(leftRangeElt, rightRangeElt, Range.TheObjects[theIndex]);
              NonExplored.RemoveSelection(theIndex);
            }
          }
        }
      }
    }
  }
  roots vectorsLeft, vectorsRight;
  vectorsLeft.SetSizeExpandOnTopNoObjectInit(Range.size);
  vectorsRight.SetSizeExpandOnTopNoObjectInit(Range.size);
  for (int i=0; i<Range.size; i++)
  { Range.TheObjects[i].ElementToVectorRootSpacesFirstThenCartan(vectorsRight.TheObjects[i]);
    Domain.TheObjects[i].ElementToVectorRootSpacesFirstThenCartan(vectorsLeft.TheObjects[i]);
  }
  outputAuto.MakeLinearOperatorFromDomainAndRange(vectorsLeft, vectorsRight, theGlobalVariables);
}

void MatrixLargeRational::MakeLinearOperatorFromDomainAndRange(roots& domain, roots& range, GlobalVariables& theGlobalVariables)
{ MatrixLargeRational A;
  MatrixLargeRational B;
  A.AssignRootsToRowsOfMatrix(domain);
  B.AssignRootsToRowsOfMatrix(range);
  A.Invert(theGlobalVariables);
  (*this)=A*B;
  this->Transpose(theGlobalVariables);
}

//void SemisimpleLieAlgebra::ComputeD

bool SemisimpleLieAlgebra::IsInTheWeightSupport(root& theWeight, root& highestWeight, GlobalVariables& theGlobalVariables)
{ root correspondingDominant= theWeight;
  this->theWeyl.RaiseToHighestWeight(correspondingDominant);

  root theDiff= highestWeight - correspondingDominant;
  correspondingDominant.ComputeDebugString();
  highestWeight.ComputeDebugString();
  theDiff.ComputeDebugString();
  if (!theDiff.IsPositiveOrZero())
    return false;
  return true;
}

bool WeylGroup::IsDominantWeight(root& theWeight)
{ int theDimension= this->CartanSymmetric.NumRows;
  root tempRoot;
  for (int i=0; i<theDimension; i++)
  { tempRoot.MakeEi(theDimension, i);
    if (this->RootScalarCartanRoot(tempRoot, theWeight)<0)
      return false;
  }
  return true;
}

void SemisimpleLieAlgebra::GenerateWeightSupport(root& theHighestWeight, roots& output, GlobalVariables& theGlobalVariables)
{ int indexFirstNonExplored=0;
  this->theWeyl.RaiseToHighestWeight(theHighestWeight);
  output.size=0;
  output.AddObjectOnTop(theHighestWeight);
  roots simpleBasis;
  int theDimension= this->theWeyl.CartanSymmetric.NumRows;
  simpleBasis.MakeEiBasis(theDimension);
  root current;
  while (indexFirstNonExplored<output.size)
  { for (int i=0; i<theDimension; i++)
    { current= output.TheObjects[indexFirstNonExplored]-simpleBasis.TheObjects[i];
      current.ComputeDebugString();
      if (this->IsInTheWeightSupport(current, theHighestWeight, theGlobalVariables))
        output.AddOnTopNoRepetition(current);
    }
    indexFirstNonExplored++;
  }
}

void WeylGroup::RaiseToHighestWeight(root& theWeight)
{ root correspondingDominant;
  for (int i=0; i<this->size; i++)
  { correspondingDominant= theWeight;
    this->ActOnRootByGroupElement(i, correspondingDominant, false, false);
    if (this->IsDominantWeight(correspondingDominant))
    { theWeight=correspondingDominant;
      break;
    }
  }
}

void SemisimpleLieAlgebra::GenerateOneMonomialPerWeightInTheWeightSupport(root& theHighestWeight, GlobalVariables& theGlobalVariables)
{ roots weightSupport;
  this->GenerateWeightSupport(theHighestWeight, weightSupport, theGlobalVariables);
  List<bool> Explored;
  Explored.initFillInObject(weightSupport.size, false);
  this->VermaMonomials.SetSizeExpandOnTopNoObjectInit(1);
  this->VermaMonomials.TheObjects[0].MakeConst(theHighestWeight, Rational::TheRingUnit, this);
  roots simpleBasis;
  int theDimension= this->theWeyl.CartanSymmetric.NumRows;
  simpleBasis.MakeEiBasis(theDimension);
  root current, tempRoot;
  VermaModuleMonomial tempMon;
  for (int indexLowestNonExplored=0; indexLowestNonExplored<this->VermaMonomials.size; indexLowestNonExplored++)
  { Explored.TheObjects[indexLowestNonExplored]=true;
    for (int i=0; i<theDimension; i++)
    { this->VermaMonomials.TheObjects[indexLowestNonExplored].GetWeight(current);
      current.ComputeDebugString();
      current-=simpleBasis.TheObjects[i];
      current.ComputeDebugString();
      int index=weightSupport.IndexOfObject(current);
      if (index!=-1)
        if (!Explored.TheObjects[index])
        { Explored.TheObjects[index]=true;
          this->VermaMonomials.TheObjects[indexLowestNonExplored].MultiplyBySimpleGenerator(i, tempMon);
          this->VermaMonomials.AddObjectOnTop(tempMon);
        }
    }
  }
}

void VermaModuleMonomial::GetWeight(root& output)
{ int theDimension= this->owner->theWeyl.CartanSymmetric.NumRows;
  roots simpleBasis;
  simpleBasis.MakeEiBasis(theDimension);
  output=this->theHighestWeight;
  for (int i=0; i<this->theSimpleGenerators.size; i++)
    output-=simpleBasis.TheObjects[this->theSimpleGenerators.TheObjects[i]]*this->thePowers.TheObjects[i];
}

void VermaModuleMonomial::MultiplyBySimpleGenerator(int index, VermaModuleMonomial& output)
{ output=*this;
  bool tempBool=true;
  if (this->theSimpleGenerators.size==0)
    tempBool=false;
  else
    tempBool=(*this->theSimpleGenerators.LastObject()==index);
  if (tempBool)
    (*output.thePowers.LastObject())++;
  else
  { output.theSimpleGenerators.AddObjectOnTop(index);
    output.thePowers.AddObjectOnTop(1);
  }
}

void VermaModuleMonomial::MakeConst(root& highestWeight, Rational& theCoeff, SemisimpleLieAlgebra* theOwner)
{ this->theHighestWeight=highestWeight;
  this->coeff= theCoeff;
  this->owner=theOwner;
  this->thePowers.size=0;
  this->theSimpleGenerators.size=0;
}

void VermaModuleMonomial::operator=(const VermaModuleMonomial& right)
{ this->owner= right.owner;
  this->theHighestWeight= right.theHighestWeight;
  this->thePowers= right.thePowers;
  this->theSimpleGenerators= right.theSimpleGenerators;
  this->coeff= right.coeff;
}

void SemisimpleLieAlgebra::ElementToStringVermaMonomials(std::string& output)
{ std::stringstream out;
  for (int i=0; i<this->VermaMonomials.size; i++)
  { VermaModuleMonomial& tempMon= this->VermaMonomials.TheObjects[i];
    out << "$" << tempMon.ElementToString() << "$\n\n";
  }
  output=out.str();
}

void VermaModuleMonomial::ElementToString(std::string& output)
{ std::stringstream out;
  for (int i=0; i<this->theSimpleGenerators.size; i++)
  { int thePower= this->thePowers.TheObjects[i];
    int theGen= this->theSimpleGenerators.TheObjects[i];
    if (thePower>1)
      out << "(";
    out << "g^{-\\alpha_{" << theGen+1 << "}}";
    if (thePower>1)
      out << ")^{" << thePower << "}";
  }
  if (this->theSimpleGenerators.size>0)
    out << "\\cdot";
  out << " v";
  output=out.str();
}

void SemisimpleLieAlgebra::CreateEmbeddingFromFDModuleHaving1dimWeightSpaces(root& theHighestWeight, GlobalVariables& theGlobalVariables)
{ roots weightSupport;
  this->GenerateWeightSupport(theHighestWeight, weightSupport, theGlobalVariables);
  int highestWeight, distanceToHW;
  this->EmbeddingsRootSpaces.SetSizeExpandOnTopNoObjectInit(this->theWeyl.RootSystem.size);
  int theDimension= this->theWeyl.CartanSymmetric.NumRows;
  List<bool> Explored;
  Explored.initFillInObject(this->theWeyl.RootSystem.size, false);
  int numExplored=0;
  for (int i=0; i<this->theWeyl.RootSystem.size; i++)
  { root& current= this->theWeyl.RootSystem.TheObjects[i];
    if (current.SumCoordinates()==1 || current.SumCoordinates()==-1)
    { numExplored++;
      Explored.TheObjects[i]=true;
      MatrixLargeRational& currentMat= this->EmbeddingsRootSpaces.TheObjects[i];
      currentMat.init(weightSupport.size, weightSupport.size);
      currentMat.NullifyAll();
      for (int j=0; j<weightSupport.size; j++)
      { int indexTarget= weightSupport.IndexOfObject(current+weightSupport.TheObjects[j]);
        if (indexTarget!=-1)
        { highestWeight = -1+ this->GetLengthStringAlongAlphaThroughBeta(current, weightSupport.TheObjects[j], distanceToHW, weightSupport);
          if (current.IsNegativeOrZero())
            currentMat.elements[indexTarget][j]=1;
          else
            currentMat.elements[indexTarget][j]=(highestWeight-distanceToHW+1)*distanceToHW;
        }
      }
    }
  }
  roots simpleBasis;
  simpleBasis.MakeEiBasis(theDimension);
  while (numExplored<this->theWeyl.RootSystem.size)
  { for (int i=0; i<this->theWeyl.RootSystem.size; i++)
      if (Explored.TheObjects[i])
        for (int j=0; j<this->theWeyl.RootSystem.size; j++)
          if (Explored.TheObjects[j])
          { root tempRoot= this->theWeyl.RootSystem.TheObjects[i]+this->theWeyl.RootSystem.TheObjects[j];
            if (this->theWeyl.IsARoot(tempRoot))
            { int index= this->theWeyl.RootSystem.IndexOfObjectHash(tempRoot);
              if (!Explored.TheObjects[index])
              { Explored.TheObjects[index]=true;
                numExplored++;
                this->EmbeddingsRootSpaces.TheObjects[index]= this->EmbeddingsRootSpaces.TheObjects[i];
                this->EmbeddingsRootSpaces.TheObjects[index].LieBracketWith(this->EmbeddingsRootSpaces.TheObjects[j]);
              }
            }
          }
  }
  this->EmbeddingsCartan.SetSizeExpandOnTopNoObjectInit(theDimension);
  for (int i=0; i<theDimension; i++)
  { MatrixLargeRational& current= this->EmbeddingsCartan.TheObjects[i];
    current.init(weightSupport.size, weightSupport.size);
    current.NullifyAll();
    root tempRoot;
    tempRoot.MakeEi(theDimension, i);
    for (int j=0; j<weightSupport.size; j++)
      current.elements[j][j]=this->theWeyl.RootScalarCartanRoot(tempRoot, weightSupport.TheObjects[j]);
  }
}

int SemisimpleLieAlgebra::GetLengthStringAlongAlphaThroughBeta(root& alpha, root& beta, int& distanceToHighestWeight, roots& weightSupport)
{ root tempRoot=beta;
  for (int i=0; ; i++)
  { tempRoot+= alpha;
    if (!weightSupport.ContainsObject(tempRoot))
    { distanceToHighestWeight=i;
      break;
    }
  }
  for (int i=0; ; i++)
  { tempRoot-= alpha;
    if (!weightSupport.ContainsObject(tempRoot))
      return i;
  }
//  assert(false);
//  return -1;
}

void SemisimpleLieAlgebra::ElementToStringEmbedding(std::string& output)
{ std::stringstream out;
  std::string tempS;
  for (int i=0; i<this->EmbeddingsRootSpaces.size; i++)
  { MatrixLargeRational& theMat = this->EmbeddingsRootSpaces.TheObjects[i];
    theMat.ElementToString(tempS, false, true);
    out << " $g^{\\alpha_{" << i+1 << "}}\\mapsto$ " << tempS << " \n\n";
  }
  for (int i=0; i<this->EmbeddingsCartan.size; i++)
  { MatrixLargeRational& theMat = this->EmbeddingsCartan.TheObjects[i];
    theMat.ElementToString(tempS, false, true);
    out << " $h_{" << i+1 << "}\\mapsto$ " << tempS << " \n\n";
  }
  output = out.str();
}

void MatrixLargeRational::LieBracketWith(MatrixLargeRational& right)
{ MatrixLargeRational tempMat, tempMat2, tempMat3;
  tempMat2=((*this)*right );
  tempMat2.ComputeDebugString();
  tempMat3= (right*(*this));
  tempMat3.ComputeDebugString();
  tempMat=tempMat2-tempMat3;
  tempMat.ComputeDebugString();
  this->Assign(tempMat);
}

bool roots::LinSpanContainsRoot(root& input, GlobalVariables& theGlobalVariables)
{ roots tempRoots;
  tempRoots.CopyFromBase(*this);
  tempRoots.AddObjectOnTop(input);
  this->ComputeDebugString();
  tempRoots.ComputeDebugString();
  input.ComputeDebugString();
  return this->GetRankOfSpanOfElements(theGlobalVariables)==tempRoots.GetRankOfSpanOfElements(theGlobalVariables);
}

bool SemisimpleLieAlgebra::CheckClosedness(std::string& output, GlobalVariables& theGlobalVariables)
{ List<MatrixLargeRational> tempElts;
  tempElts.size=0;
  tempElts.AddListOnTop(this->EmbeddingsCartan);
  tempElts.AddListOnTop(this->EmbeddingsRootSpaces);
  roots tempRoots;
  tempRoots.SetSizeExpandOnTopNoObjectInit(tempElts.size);
  for (int i=0; i<tempElts.size; i++)
    tempElts.TheObjects[i].MatrixToRoot(tempRoots.TheObjects[i]);
  MatrixLargeRational tempMat;
  for (int i=0; i<tempElts.size; i++)
    for (int j=0; j<tempElts.size; j++)
    { tempMat= tempElts.TheObjects[i];
      tempMat.ComputeDebugString();
      tempElts.TheObjects[j].ComputeDebugString();
      tempMat.LieBracketWith(tempElts.TheObjects[j]);
      tempMat.ComputeDebugString();
      root tempRoot;
      tempMat.MatrixToRoot(tempRoot);
      bool isGood=tempRoots.LinSpanContainsRoot(tempRoot, theGlobalVariables);
      if (!isGood)
      { std::stringstream out;
        std::string tempS1, tempS2, tempS3;
        tempMat.ElementToString(tempS1, false, true);
        tempElts.TheObjects[i].ElementToString(tempS2, false, true);
        tempElts.TheObjects[j].ElementToString(tempS3, false, true);
        out << "First bad: " << tempS1 << "=[" << tempS2 << "," << tempS3 << "]";
        output=out.str();
        return false;
      }
    }
  return true;
}

bool MatrixLargeRational::IsProportionalTo(MatrixLargeRational& right)
{ Rational coeff=0;
  bool found=false;
  for (int i=0; i<this->NumRows; i++)
    for(int j=0; j<this->NumCols; j++)
      if (this->elements[i][j]==0)
      { if (right.elements[i][j]!=0)
          return false;
      } else
      { if (!found)
        { found=true;
          coeff = right.elements[i][j]/this->elements[i][j];
        } else
        { if (coeff!=(right.elements[i][j]/this->elements[i][j]))
            return false;
        }
      }
  return true;
}

void MatrixLargeRational::MatrixToRoot(root& output)
{ output.SetSizeExpandOnTopLight(this->NumRows*this->NumCols);
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      output.TheObjects[i*this->NumRows+j]=this->elements[i][j];
}

void rootSubalgebra::GenerateAutomorphismsPreservingBorel(GlobalVariables& theGlobalVariables, ReflectionSubgroupWeylGroup& outputAutomorphisms)
{ this->ComputeAll();
  this->GenerateIsomorphismsPreservingBorel(*this, theGlobalVariables, &outputAutomorphisms, false);
}

void Lattice::DuflosComputation(List<char>& WeylLetters, List<int>& ranks, std::string& output, GlobalVariables& theGlobalVariables)
{ std::stringstream body, tables;
  std::string tempBody, tempTable;
  tables << "\\documentclass{article}\n\\usepackage{amssymb}\n\\begin{document}\n";
  for (int i=0; i<WeylLetters.size; i++)
  { this->DuflosComputationOneSA(WeylLetters.TheObjects[i], ranks.TheObjects[i], tempTable, tempBody, theGlobalVariables);
    tables << tempTable << "\n\n";
    body << tempBody;
  }
  tables << body.str();
  tables << "\n\\end{document}";
  output = tables.str();
}

void Lattice::DuflosComputationOneSA(char WeylLetter, int rank, std::string& outputTable, std::string& outputBody, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  std::stringstream niceTable;
  rootSubalgebras theRootSubalgebras;
  theRootSubalgebras.GenerateAllReductiveRootSubalgebrasUpToIsomorphism(theGlobalVariables, WeylLetter, rank, true, true);
  WeylGroup& theWeyl= theRootSubalgebras.AmbientWeyl;
  int theDimension= theWeyl.CartanSymmetric.NumRows;
  niceTable << "\\begin{tabular}{cc}\n\\multicolumn{2}{c}{Root system $\\Delta$ of type" << theRootSubalgebras.TheObjects[0].theDynkinDiagram.DebugString << "} \\\\\\hline\n Dynkin type subsystem $\\Delta'$ & Structure of $\\Lambda(\\Delta)/\\Lambda(\\Delta')$\\\\\\hline\n";
  out << "\n\nRoot system of "<< theRootSubalgebras.TheObjects[0].theDynkinDiagram.DebugString << ":\n\n";
  roots tempRoots;
  tempRoots.CopyFromBase(theWeyl.RootSystem);
  out << tempRoots.ElementToString() << "\n\n";
  for (int i=1; i<theRootSubalgebras.size; i++)
  { rootSubalgebra& currentSA=theRootSubalgebras.TheObjects[i];
    if (currentSA.SimpleBasisK.size==theDimension)
    { this->LatticeBasis= currentSA.SimpleBasisK;
      this->GetZnModLatticeRepresentativesRootCase(theWeyl, this->RepresentativesQuotient, theGlobalVariables);
      //std::string tempS;
      out << "\n\nType subsystem: " << currentSA.theDynkinDiagram.DebugString << "\n\n Cardinality quotient: " << this->RepresentativesQuotient.size;
      out << "\n\nSimple basis root subsystem: " << currentSA.SimpleBasisK.ElementToString();
      out << "\n\nRepresentatives elements in quotient: " << this->RepresentativesQuotient.ElementToString();
      std::string tempS;
      List<int> list1, list2;
      this->GetStructureQuotientRootCase(currentSA.AmbientWeyl, tempS, list1, list2, theGlobalVariables);
      out << "\n\nStructure: " << tempS;
      niceTable << currentSA.theDynkinDiagram.DebugString << " & " << tempS <<" \\\\\n";
    }
  }
  niceTable << "\\end{tabular}";
  outputTable = niceTable.str();
  outputBody = out.str();
}

void ComputationSetup::DuflosComputation(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ std::string tempS;
  Lattice tempLattice;
  List<char> WeylLetters;
  List<int> ranks;
 /* WeylLetters.AddObjectOnTop('E');
  ranks.AddObjectOnTop(8);
*/
  WeylLetters.AddObjectOnTop('G');
  ranks.AddObjectOnTop(2);
  WeylLetters.AddObjectOnTop('D');
  ranks.AddObjectOnTop(4);
  WeylLetters.AddObjectOnTop('F');
  ranks.AddObjectOnTop(4);
  WeylLetters.AddObjectOnTop('E');
  ranks.AddObjectOnTop(6);
  WeylLetters.AddObjectOnTop('E');
  ranks.AddObjectOnTop(7);
  WeylLetters.AddObjectOnTop('E');
  ranks.AddObjectOnTop(8);
  tempLattice.DuflosComputation(WeylLetters, ranks, tempS, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1=tempS;
  theGlobalVariables.MakeReport();
}

bool Lattice::IsInLattice(const root& input)
{ roots tempRoots;
  tempRoots = this->LatticeBasis;
  int theDimension = this->LatticeBasis.size;
  tempRoots.AddObjectOnTop(input);
  MatrixLargeRational tempMat;
  tempRoots.GetLinearDependence(tempMat);
  Rational coeffInFrontOfInput= tempMat.elements[theDimension][0];
  tempMat.ComputeDebugString();
  tempRoots.ComputeDebugString();
  if (coeffInFrontOfInput==0)
    return false;
  for (int i=0; i<theDimension; i++)
    if (!(tempMat.elements[i][0]/coeffInFrontOfInput).IsInteger())
      return false;
  return true;
}

void Lattice::GetZnModLatticeRepresentatives(WeylGroup* theWeyl, roots& representativesOutput, GlobalVariables& theGlobalVariables)
{ int theDimension= this->LatticeBasis.size;
  if (theDimension<1)
    return;
  if (theDimension!=this->LatticeBasis.TheObjects[0].size)
    return;
  if (theDimension!=this->LatticeBasis.GetRankOfSpanOfElements(theGlobalVariables))
    return;
  representativesOutput.size=0;
  root tempRoot;
  for (int i=0; i<theDimension; i++)
  { tempRoot.MakeEi(theDimension, i);
    if (!this->ContainsConjugacyClassRepresentedBy(representativesOutput, tempRoot))
      representativesOutput.AddObjectOnTop(tempRoot);
  }
  for (int lowestNonExploredIndex=0; lowestNonExploredIndex<representativesOutput.size; lowestNonExploredIndex++)
  { //root& current= representativesOutput.TheObjects[lowestNonExploredIndex];
    for (int k=0; k<=lowestNonExploredIndex; k++)
    { tempRoot= representativesOutput.TheObjects[lowestNonExploredIndex]+representativesOutput.TheObjects[k];
      if (!this->ContainsConjugacyClassRepresentedBy(representativesOutput, tempRoot))
      { if (theWeyl!=0)
          for (int l=0; l<theWeyl->RootSystem.size; l++)
          { if (this->IsInLattice(tempRoot- theWeyl->RootSystem.TheObjects[l]))
            { representativesOutput.AddObjectOnTop(theWeyl->RootSystem.TheObjects[l]);
              break;
            }
          }
        else
          representativesOutput.AddObjectOnTop(tempRoot);
      }
    }
  }
}

bool Lattice::ContainsConjugacyClassRepresentedBy(roots& representatives, root& input)
{ for (int i=0; i<representatives.size; i++)
    if (this->IsInLattice(representatives.TheObjects[i]-input))
      return true;
  return false;
}

void Lattice::GetStructureQuotientRootCase(WeylGroup& theWeyl, std::string& output, List<int>& outputIndices, List<int>& outputMults, GlobalVariables& theGlobalVariables)
{ Lattice tempLattice;
  std::stringstream out;
  tempLattice.LatticeBasis=this->LatticeBasis;
  tempLattice.GetZnModLatticeRepresentativesRootCase(theWeyl, theGlobalVariables);
  int maxRank=0;
  rootSubalgebra tempSA;
  tempSA.AmbientWeyl.Assign(theWeyl);
  tempSA.genK=tempLattice.LatticeBasis;
  tempSA.ComputeAll();
  outputIndices.size=0;
  outputMults.size=0;
  out << "$";
  for (int indexMaxRank=tempLattice.GetIndexFirstElementOfMaxRank(maxRank); maxRank>1; indexMaxRank=tempLattice.GetIndexFirstElementOfMaxRank(maxRank))
  { if (outputIndices.size>0)
      out << "+";
    if (outputIndices.ContainsObject(maxRank))
      outputMults.TheObjects[outputIndices.IndexOfObject(maxRank)]++;
    else
    { outputIndices.AddObjectOnTop(maxRank);
      outputMults.AddObjectOnTop(1);
    }
    out << "\\mathbb{Z}_{" << maxRank << "}";
    tempSA.genK.AddObjectOnTop(tempLattice.RepresentativesQuotient.TheObjects[indexMaxRank]);
    tempSA.ComputeAllButAmbientWeyl();
    tempLattice.LatticeBasis= tempSA.SimpleBasisK;
    tempLattice.GetZnModLatticeRepresentativesRootCase(theWeyl, theGlobalVariables);
  }
  out << "$";
  output=out.str();
}

int Lattice::GetIndexFirstElementOfMaxRank(int& outputRank)
{ int result=-1;
  outputRank=0;
  for (int i=0; i<this->RepresentativesQuotient.size; i++)
    if (outputRank< this->GetRankElementRepresentedBy(this->RepresentativesQuotient.TheObjects[i]))
    { outputRank=this->GetRankElementRepresentedBy(this->RepresentativesQuotient.TheObjects[i]);
      result=i;
    }
  return result;
}

int Lattice::GetRankElementRepresentedBy(root& elementRepresentative)
{ root tempRoot;
  tempRoot.MakeZero(elementRepresentative.size);
  for (int result=1; ; result++)
  { tempRoot+=elementRepresentative;
    if (this->IsInLattice(tempRoot))
      return result;
  }
//  assert(false);
//  return -1;
}

ElementUniversalEnveloping Parser::ParseAndCompute(const std::string& input, GlobalVariables& theGlobalVariables)
{ this->Parse(input);
  this->Evaluate(theGlobalVariables);
  this->theValue.UEElement.Simplify();
  return this->theValue.UEElement;
}

std::string Parser::ParseEvaluateAndSimplify(const std::string& input, GlobalVariables& theGlobalVariables)
{ this->theHmm.theRange.ComputeChevalleyConstants(this->DefaultWeylLetter, this->DefaultWeylRank, theGlobalVariables);
  this->Parse(input);
  this->ComputeDebugString(theGlobalVariables);
  this->Evaluate(theGlobalVariables);
  this->theValue.UEElement.Simplify();
  return this->theValue.ElementToStringValueOnly(true);
}

int DebugCounter=-1;

void Parser::ParserInit(const std::string& input)
{ this->TokenStack.MakeActualSizeAtLeastExpandOnTop(input.size());
  this->ValueStack.MakeActualSizeAtLeastExpandOnTop(input.size());
  this->MakeActualSizeAtLeastExpandOnTop(input.size());
  this->TokenStack.size=0;
  this->ValueStack.size=0;
  this->TokenBuffer.size=0;
  this->ValueBuffer.size=0;
  this->size=0;
  std::string buffer;
  int theLength=(signed) input.size();
  char LookAheadChar;
  for (int i=0; i<theLength; i++)
  { buffer.push_back(input[i]);
    if (i<theLength-1)
      LookAheadChar=input[i+1];
    else
      LookAheadChar=' ';
    if (this->IsAWordSeparatingCharacter(buffer[0]) || this->IsAWordSeparatingCharacter(LookAheadChar))
    { this->LookUpInDictionaryAndAdd(buffer);
      buffer="";
    }
  }
  this->ValueStack.size=0;
  this->TokenStack.size=0;
  this->ValueStack.MakeActualSizeAtLeastExpandOnTop(this->ValueBuffer.size);
  this->TokenStack.MakeActualSizeAtLeastExpandOnTop(this->TokenBuffer.size);
  this->StringBeingParsed=input;
  for (int i=0; i<this->numEmptyTokensAtBeginning; i++)
  { this->TokenStack.AddObjectOnTop(this->tokenEmpty);
    this->ValueStack.AddObjectOnTop(0);
  }
}

bool Parser::IsAWordSeparatingCharacter(char c)
{ switch (c)
  { case '0': return true;
    case '1': return true;
    case '2': return true;
    case '3': return true;
    case '4': return true;
    case '5': return true;
    case '6': return true;
    case '7': return true;
    case '8': return true;
    case '9': return true;
    case '*': return true;
    case '}': return true;
    case '{': return true;
    case '[': return true;
    case ']': return true;
    case '(': return true;
    case ',': return true;
    case ')': return true;
    case '^': return true;
    case '+': return true;
    case '-': return true;
    case '_': return true;
    case '/': return true;
    case ' ': return true;
    default: return false;
  }
//  return false;
}

bool Parser::LookUpInDictionaryAndAdd(std::string& input)
{ switch (input.at(0))
  { case '0': this->TokenBuffer.AddObjectOnTop(Parser::tokenDigit); this->ValueBuffer.AddObjectOnTop(0); return true;
    case '1': this->TokenBuffer.AddObjectOnTop(Parser::tokenDigit); this->ValueBuffer.AddObjectOnTop(1); return true;
    case '2': this->TokenBuffer.AddObjectOnTop(Parser::tokenDigit); this->ValueBuffer.AddObjectOnTop(2); return true;
    case '3': this->TokenBuffer.AddObjectOnTop(Parser::tokenDigit); this->ValueBuffer.AddObjectOnTop(3); return true;
    case '4': this->TokenBuffer.AddObjectOnTop(Parser::tokenDigit); this->ValueBuffer.AddObjectOnTop(4); return true;
    case '5': this->TokenBuffer.AddObjectOnTop(Parser::tokenDigit); this->ValueBuffer.AddObjectOnTop(5); return true;
    case '6': this->TokenBuffer.AddObjectOnTop(Parser::tokenDigit); this->ValueBuffer.AddObjectOnTop(6); return true;
    case '7': this->TokenBuffer.AddObjectOnTop(Parser::tokenDigit); this->ValueBuffer.AddObjectOnTop(7); return true;
    case '8': this->TokenBuffer.AddObjectOnTop(Parser::tokenDigit); this->ValueBuffer.AddObjectOnTop(8); return true;
    case '9': this->TokenBuffer.AddObjectOnTop(Parser::tokenDigit); this->ValueBuffer.AddObjectOnTop(9); return true;
    case '*': this->TokenBuffer.AddObjectOnTop(Parser::tokenTimes); this->ValueBuffer.AddObjectOnTop(9); return true;
    case '}': this->TokenBuffer.AddObjectOnTop(Parser::tokenCloseCurlyBracket); this->ValueBuffer.AddObjectOnTop(0); return true;
    case '{': this->TokenBuffer.AddObjectOnTop(Parser::tokenOpenCurlyBracket); this->ValueBuffer.AddObjectOnTop(0); return true;
    case '[': this->TokenBuffer.AddObjectOnTop(Parser::tokenOpenLieBracket); this->ValueBuffer.AddObjectOnTop(0); return true;
    case ']': this->TokenBuffer.AddObjectOnTop(Parser::tokenCloseLieBracket); this->ValueBuffer.AddObjectOnTop(0); return true;
    case '(': this->TokenBuffer.AddObjectOnTop(Parser::tokenOpenBracket); this->ValueBuffer.AddObjectOnTop(0); return true;
    case ',': this->TokenBuffer.AddObjectOnTop(Parser::tokenComma); this->ValueBuffer.AddObjectOnTop(0); return true;
    case ')': this->TokenBuffer.AddObjectOnTop(Parser::tokenCloseBracket); this->ValueBuffer.AddObjectOnTop(0); return true;
    case '^': this->TokenBuffer.AddObjectOnTop(Parser::tokenPower); this->ValueBuffer.AddObjectOnTop(0); return true;
    case '+': this->TokenBuffer.AddObjectOnTop(Parser::tokenPlus); this->ValueBuffer.AddObjectOnTop(0); return true;
    case '-': this->TokenBuffer.AddObjectOnTop(Parser::tokenMinus); this->ValueBuffer.AddObjectOnTop(0); return true;
    case '_': this->TokenBuffer.AddObjectOnTop(Parser::tokenUnderscore); this->ValueBuffer.AddObjectOnTop(0); return true;
    case '/': this->TokenBuffer.AddObjectOnTop(Parser::tokenDivide); this->ValueBuffer.AddObjectOnTop(0); return true;
    default: break;
  }
  if (input=="x")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenX);
    this->ValueBuffer.AddObjectOnTop(0);
    return true;
  }
  if (input=="d")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenPartialDerivative);
    this->ValueBuffer.AddObjectOnTop(0);
    return true;
  }
  if (input=="g")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenG);
    this->ValueBuffer.AddObjectOnTop(0);
    return true;
  }
  if (input =="h")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenH);
    this->ValueBuffer.AddObjectOnTop(0);
    return true;
  }
  if (input=="c")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenC);
    this->ValueBuffer.AddObjectOnTop(0);
    return true;
  }
  if (input =="i")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenMap);
    this->ValueBuffer.AddObjectOnTop(0);
    return true;
  }
  if (input=="n")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenVariable);
    this->ValueBuffer.AddObjectOnTop(0);
    return true;
  }
  if (input=="gcd")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenFunction);
    this->ValueBuffer.AddObjectOnTop(this->functionGCD);
    return true;
  }
  if (input=="lcm")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenFunction);
    this->ValueBuffer.AddObjectOnTop(this->functionLCM);
    return true;
  }
  if (input=="eigen")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenFunction);
    this->ValueBuffer.AddObjectOnTop(this->functionEigen);
    return true;
  }
  return false;
}

void Parser::Parse(const std::string& input)
{ this->ParserInit(input);
  this->ParseNoInit(0, this->TokenBuffer.size-1);
  this->ComputeNumberOfVariablesAndAdjustNodes();
}

void Parser::ParseNoInit(int indexFrom, int indexTo)
{ if (indexFrom<0 || indexTo>= this->TokenBuffer.size)
    return;
  for (int i=indexFrom; i<=indexTo; i++)
  { this->ValueStack.AddObjectOnTop(this->ValueBuffer.TheObjects[i]);
    this->TokenStack.AddObjectOnTop(this->TokenBuffer.TheObjects[i]);
    int lookAheadToken=this->tokenEnd;
    if (i<this->TokenBuffer.size-1)
      lookAheadToken=this->TokenBuffer.TheObjects[i+1];
    while(this->ApplyRules(lookAheadToken))
    {}
  }
}

bool Parser::TokenProhibitsUnaryMinus(int theToken)
{ if (theToken==this->tokenExpression)
    return true;
  return false;
}

bool Parser::lookAheadTokenProhibitsPlus(int theToken)
{ if (theToken==this->tokenEnd)
    return false;
  if (theToken==this->tokenPlus)
    return false;
  if (theToken==this->tokenMinus)
    return false;
  if (theToken==this->tokenCloseBracket)
    return false;
  if (theToken==this->tokenCloseLieBracket)
    return false;
  if (theToken==this->tokenCloseCurlyBracket)
    return false;
  if (theToken==this->tokenComma)
    return false;
//  if (theToken==this->tokenExpression)
//    return true;
//  if (theToken==this->tokenUnderscore)
//    return true;
  return true;
}

bool Parser::lookAheadTokenProhibitsTimes(int theToken)
{ if (theToken==this->tokenPower)
    return true;
  if (theToken==this->tokenUnderscore)
    return true;
  return false;
}

bool Parser::ApplyRules(int lookAheadToken)
{ if (this->TokenStack.size<=this->numEmptyTokensAtBeginning)
    return false;
  int tokenLast=*this->TokenStack.LastObject();
  int tokenSecondToLast= this->TokenStack.TheObjects[this->TokenStack.size-2];
  int tokenThirdToLast=this->TokenStack.TheObjects[this->TokenStack.size-3];
  int tokenFourthToLast=this->TokenStack.TheObjects[this->TokenStack.size-4];
  int tokenFifthToLast=this->TokenStack.TheObjects[this->TokenStack.size-5];
//  int tokenSixthToLast=this->TokenStack.TheObjects[this->TokenStack.size-6];
  if (tokenLast==this->tokenEmpty)
  { this->PopTokenAndValueStacksLast();
    return true;
  }
  if (tokenLast==this->tokenCloseCurlyBracket && tokenThirdToLast==this->tokenOpenCurlyBracket && tokenSecondToLast==this->tokenExpression)
  { this->PopTokenAndValueStacksLast();
    this->PopTokenAndValueStacksShiftDown(this->TokenStack.size-2);
    return true;
  }
  if (tokenLast==this->tokenExpression && tokenSecondToLast==this->tokenMap && lookAheadToken!=this->tokenUnderscore)
  { this->AddMapOnTop();
    return true;
  }
  if (tokenSecondToLast==this->tokenFunction && tokenLast==this->tokenExpression)
  { this->AddFunctionOnTop();
    return true;
  }
  if (tokenLast==this->tokenExpression && tokenSecondToLast==this->tokenMinus && !this->TokenProhibitsUnaryMinus(tokenThirdToLast) && !this->lookAheadTokenProhibitsPlus(lookAheadToken))
  { this->AddUnaryMinusOnTop();
    return true;
  }
  if (tokenLast==this->tokenCloseBracket && tokenSecondToLast==this->tokenExpression && tokenThirdToLast==this->tokenOpenBracket)
  { this->PopTokenAndValueStacksLast();
    this->PopTokenAndValueStacksShiftDown(this->TokenStack.size-2);
    return true;
  }
  if (tokenLast== this->tokenCloseLieBracket && tokenSecondToLast==this->tokenExpression && tokenThirdToLast==this->tokenComma && tokenFourthToLast==this->tokenExpression && tokenFifthToLast==this->tokenOpenLieBracket)
  { this->AddLieBracketOnTop();
    return true;
  }
  if (tokenLast==this->tokenDigit)
  { *this->TokenStack.LastObject()=this->tokenInteger;
    if (tokenSecondToLast!=this->tokenInteger)
      this->LargeIntegerReader=*this->ValueStack.LastObject();
    else
      this->MergeLastTwoIntegers();
    return true;
  }
  if (tokenLast==this->tokenInteger && lookAheadToken!=this->tokenDigit)
  { this->AddIntegerOnTopConvertToExpression();
    return true;
  }
  if (tokenLast==this->tokenPartialDerivative || tokenLast==this->tokenG || tokenLast==this->tokenX || tokenLast==this->tokenH || tokenLast== this->tokenC || tokenLast==this->tokenVariable)
  { this->AddLetterExpressionOnTop();
    return true;
  }
  if (tokenSecondToLast==this->tokenUnderscore && tokenLast==this->tokenExpression && tokenThirdToLast==this->tokenExpression && tokenFourthToLast!=this->tokenUnderscore)
  { this->AddIndexingExpressionOnTop();
    return true;
  }
  if (tokenLast==this->tokenExpression && tokenSecondToLast== this->tokenPower && tokenThirdToLast==this->tokenExpression)
  { this->AddPowerOnTop();
    return true;
  }
  if (tokenLast==this->tokenExpression && tokenSecondToLast== this->tokenDivide && tokenThirdToLast==this->tokenExpression && !this->lookAheadTokenProhibitsTimes(lookAheadToken))
  { this->AddDivideOnTop();
    return true;
  }
  if (tokenLast==this->tokenExpression && tokenThirdToLast==this->tokenExpression && tokenSecondToLast==tokenTimes && !this->lookAheadTokenProhibitsTimes(lookAheadToken))
  { this->PopTokenAndValueStacksShiftDown(this->TokenStack.size-2);
    this->AddTimesOnTop();
    return true;
  }
  if (tokenLast==this->tokenExpression && tokenSecondToLast==this->tokenExpression && !this->lookAheadTokenProhibitsTimes(lookAheadToken))
  { this->AddTimesOnTop();
    return true;
  }
  if (tokenSecondToLast==this->tokenPlus && tokenLast==this->tokenExpression && tokenThirdToLast==this->tokenExpression && !this->lookAheadTokenProhibitsPlus(lookAheadToken))
  { this->AddPlusOnTop();
    return true;
  }
  if (tokenSecondToLast==this->tokenMinus && tokenLast==this->tokenExpression && tokenThirdToLast==this->tokenExpression && !this->lookAheadTokenProhibitsPlus(lookAheadToken))
  { this->AddMinusOnTop();
    return true;
  }
  int rootDim;
  if (this->StackTopIsARoot(rootDim))
  { this->AddRootOnTop(rootDim);
    return true;
  }
  return false;
}

bool Parser::StackTopIsARoot(int& outputDimension)
{ if (*this->TokenStack.LastObject()!=this->tokenCloseBracket)
    return false;
  outputDimension=0;
  for (int i=this->TokenStack.size-2; i>=1; i--)
  { if (this->TokenStack.TheObjects[i]!=this->tokenExpression)
      return false;
    i--;
    outputDimension++;
    if (this->TokenStack.TheObjects[i]==this->tokenOpenBracket)
      return true;
    if (this->TokenStack.TheObjects[i]!=this->tokenComma)
      return false;
  }
  return false;
}

void Parser::DecreaseStackSetExpressionLastNode(int Decrease)
{ this->TokenStack.size-=Decrease;
  this->ValueStack.size-=Decrease;
  *this->TokenStack.LastObject()=this->tokenExpression;
  *this->ValueStack.LastObject()=this->size-1;
}

void Parser::MergeLastTwoIntegers()
{ this->LargeIntegerReader=this->LargeIntegerReader*10+(*this->ValueStack.LastObject());
  this->PopTokenAndValueStacksLast();
}

void Parser::AddPowerOnTop()
{ this->ExtendOnTop(1);
  this->LastObject()->Clear();
  this->LastObject()->Operation=this->tokenPower;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-3], this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(2);
}

void Parser::AddLetterExpressionOnTop()
{ this->ExtendOnTop(1);
  this->LastObject()->Clear();
  this->LastObject()->Operation=*this->TokenStack.LastObject();
  this->DecreaseStackSetExpressionLastNode(0);
}

void Parser::AddIntegerOnTopConvertToExpression()
{ this->ExtendOnTop(1);
  this->LastObject()->rationalValue=this->LargeIntegerReader;
  this->LastObject()->Operation=this->tokenInteger;
  this->DecreaseStackSetExpressionLastNode(0);
}

void Parser::AddFunctionOnTop()
{ this->ExtendOnTop(1);
  this->LastObject()->Operation=this->TokenStack.TheObjects[this->TokenStack.size-2];
  this->LastObject()->intValue=this->ValueStack.TheObjects[this->ValueStack.size-2];
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(1);
}

void Parser::AddIndexingExpressionOnTop()
{ this->ExtendOnTop(1);
  this->LastObject()->Operation = this->tokenUnderscore;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-3], this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(2);
}

void Parser::AddPlusOnTop()
{ this->ExtendOnTop(1);
  ParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenPlus;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-3], this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(2);
}

void Parser::AddDivideOnTop()
{ this->ExtendOnTop(1);
  ParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenDivide;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-3], this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(2);
}

void Parser::AddMinusOnTop()
{ this->ExtendOnTop(1);
  ParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenMinus;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-3], this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(2);
}

void Parser::AddUnaryMinusOnTop()
{ this->ExtendOnTop(1);
  ParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenMinusUnary;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(1);
}

void Parser::AddMapOnTop()
{ this->ExtendOnTop(1);
  ParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenMap;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(1);
}

void Parser::AddTimesOnTop()
{ this->ExtendOnTop(1);
  ParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenTimes;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-2], this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(1);
}

void Parser::AddLieBracketOnTop()
{ this->ExtendOnTop(1);
  ParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenLieBracket;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-4], this->ValueStack.TheObjects[this->ValueStack.size-2]);
  this->DecreaseStackSetExpressionLastNode(4);
}

void Parser::ParseAndCompute(const std::string& input, std::string& output, GlobalVariables& theGlobalVariables)
{ std::stringstream out; std::string tempS;
  this->Parse(input);
  out << "\\begin{eqnarray*}&&" << input << " = \\\\\n";
  this->Evaluate(theGlobalVariables);
//  this->WeylAlgebraValue.ElementToString(tempS, false, true, false);
  out << tempS;
  out << "\\end{eqnarray*}";
  output=out.str();
}

void Parser::AddRootOnTop(int theDimension)
{ this->ExtendOnTop(1);
  ParserNode& lastNode=*this->LastObject();
  lastNode.Operation=this->tokenRoot;
  lastNode.children.SetSizeExpandOnTopNoObjectInit(theDimension);
  for (int i=0; i<theDimension; i++)
  { int indexChild=this->ValueStack.TheObjects[this->ValueStack.size-2-2*i];
    lastNode.children.TheObjects[theDimension-1-i]=indexChild;
    this->TheObjects[indexChild].indexParent=this->size-1;
  }
  this->DecreaseStackSetExpressionLastNode(theDimension*2);
}

void Parser::Own(int indexParent, int indexChild1)
{ ParserNode* theNode= &this->TheObjects[indexParent];
  theNode->children.SetSizeExpandOnTopNoObjectInit(1);
  theNode->children.TheObjects[0]=indexChild1;
  this->TheObjects[indexChild1].indexParent= indexParent;
}

void Parser::Own(int indexParent, int indexChild1, int indexChild2)
{ ParserNode* theNode= & this->TheObjects[indexParent];
  theNode->children.SetSizeExpandOnTopNoObjectInit(2);
  theNode->children.TheObjects[0]=indexChild1;
  theNode->children.TheObjects[1]=indexChild2;
  this->TheObjects[indexChild1].indexParent= indexParent;
  this->TheObjects[indexChild2].indexParent= indexParent;
}

void Parser::Evaluate(GlobalVariables& theGlobalVariables)
{ if (this->TokenStack.size== this->numEmptyTokensAtBeginning+1)
    if (*this->TokenStack.LastObject()==this->tokenExpression)
    { this->TheObjects[this->ValueStack.TheObjects[this->numEmptyTokensAtBeginning]].Evaluate(theGlobalVariables);
//      this->WeylAlgebraValue.Assign(this->TheObjects[this->ValueStack.TheObjects[this->numEmptyTokensAtBeginning]].WeylAlgebraElement);
//      this->LieAlgebraValue= this->TheObjects[this->ValueStack.TheObjects[this->numEmptyTokensAtBeginning]].LieAlgebraElement;
      this->theValue=this->TheObjects[this->ValueStack.TheObjects[this->numEmptyTokensAtBeginning]];
    }
  if (this->TokenStack.size>this->numEmptyTokensAtBeginning+1)
    this->theValue.SetError(ParserNode::errorBadSyntax);
//  this->WeylAlgebraValue.ComputeDebugString(false, false);
}

void Parser::ExtendOnTop(int numNew)
{ this->SetSizeExpandOnTopNoObjectInit(this->size+numNew);
  for (int i=0; i<numNew; i++)
  { this->TheObjects[this->size-1-i].owner=this;
    this->TheObjects[this->size-1-i].Clear();
  }
}

void ParserNode::Evaluate(GlobalVariables& theGlobalVariables)
{ //this->UEElement.ComputeDebugString();
  this->Evaluated=true;
  for (int i=0; i<this->children.size; i++)
  { if (this->Operation==Parser::tokenMap)
      this->ContextLieAlgebra=&this->owner->theHmm.theDomain;
    this->owner->TheObjects[this->children.TheObjects[i]].ContextLieAlgebra=this->ContextLieAlgebra;
    this->owner->TheObjects[this->children.TheObjects[i]].Evaluate(theGlobalVariables);
    if (this->owner->TheObjects[this->children.TheObjects[i]].ExpressionType==this->typeError)
    { this->CopyError(this->owner->TheObjects[this->children.TheObjects[i]]);
      return;
    }
  }
  switch (this->Operation)
  { case Parser::tokenPlus: this->EvaluatePlus(theGlobalVariables); break;
    case Parser::tokenMinus: this->EvaluateMinus(theGlobalVariables); break;
    case Parser::tokenMinusUnary: this->EvaluateMinusUnary(theGlobalVariables); break;
    case Parser::tokenTimes: this->EvaluateTimes(theGlobalVariables); break;
    case Parser::tokenG: break;
    case Parser::tokenH: break;
    case Parser::tokenVariable: break;
    case Parser::tokenC: this->ExpressionType=this->typeUEelement; this->UEElement.MakeCasimir(*this->ContextLieAlgebra, this->owner->NumVariables, theGlobalVariables); break;
    case Parser::tokenDivide: this->EvaluateDivide(theGlobalVariables); break;
    case Parser::tokenUnderscore: this->EvaluateUnderscore(theGlobalVariables); break;
    case Parser::tokenPartialDerivative: this->ExpressionType=this->typeWeylAlgebraElement; break;
    case Parser::tokenX: this->ExpressionType=this->typeWeylAlgebraElement; break;
    case Parser::tokenInteger: this->EvaluateInteger(theGlobalVariables); break;
    case Parser::tokenLieBracket: this->EvaluateLieBracket(theGlobalVariables); break;
    case Parser::tokenPower: this->EvaluateThePower(theGlobalVariables); break;
    case Parser::tokenMap: this->EvaluateEmbedding(theGlobalVariables); break;
    case Parser::tokenFunction: this->EvaluateFunction(theGlobalVariables); break;
    case Parser::tokenRoot: this->ExpressionType=this->typeRoot; break;
    default: this->SetError(this->errorUnknownOperation); return;
  }
}

void ParserNode::EvaluateInteger(GlobalVariables& theGlobalVariables)
{ if (this->rationalValue.Extended!=0)
    this->ExpressionType=this->typeRational;
  else
  { this->ExpressionType= this->typeIntegerOrIndex;
    this->intValue= this->rationalValue.NumShort;
  }
}

void ParserNode::EvaluateThePower(GlobalVariables& theGlobalVariables)
{ this->ExpressionType=this->typeError;
  if (this->children.size!=2 || !this->AllChildrenAreOfDefinedNonErrorType())
  { this->SetError(this->errorOperationByUndefinedOrErrorType);
    return;
  }
  ParserNode& leftNode=this->owner->TheObjects[this->children.TheObjects[0]];
  ParserNode& rightNode=this->owner->TheObjects[this->children.TheObjects[1]];
  if (rightNode.ExpressionType!=this->typeIntegerOrIndex)
  { if ((rightNode.ExpressionType==this->typeRational || rightNode.ExpressionType==this->typePoly) && leftNode.ExpressionType==this->typeUEelement)
      if (leftNode.UEElement.IsAPowerOfASingleGenerator())
        { rightNode.ConvertToType(this->typePoly);
          leftNode.UEElement.SetNumVariables(this->owner->NumVariables);
          MonomialUniversalEnveloping tempMon;
          tempMon.operator=(leftNode.UEElement.TheObjects[0]);
          tempMon.Powers.TheObjects[0].MultiplyBy(rightNode.polyValue);
          this->UEElement.Nullify(this->ContextLieAlgebra);
          this->UEElement.AddMonomial(tempMon);
          this->ExpressionType=this->typeUEelement;
          return;
        }
    this->SetError(this->errorDunnoHowToDoOperation);
    return;
  }
  int thePower= rightNode.intValue;
  switch(leftNode.ExpressionType)
  { case ParserNode::typeIntegerOrIndex:
      this->rationalValue=leftNode.intValue;
      this->rationalValue.RaiseToPower(thePower);
      this->ExpressionType=this->typeRational;
      break;
    case ParserNode::typeRational:
      this->rationalValue=leftNode.rationalValue;
      this->rationalValue.RaiseToPower(thePower);
      this->ExpressionType=this->typeRational;
      break;
    case ParserNode::typePoly:
      this->polyValue=leftNode.polyValue;
      this->polyValue.RaiseToPower(thePower);
      this->ExpressionType=this->typePoly;
      break;
    case ParserNode::typeUEelement:
      this->UEElement=leftNode.UEElement;
      this->UEElement.RaiseToPower(thePower);
      this->ExpressionType=this->typeUEelement;
    break;
    default: this->ExpressionType=this->typeError; return;
  }
}

void ParserNode::EvaluateUnderscore(GlobalVariables& theGlobalVariables)
{ this->ExpressionType=this->typeError;
  if (this->children.size!=2)
  { this->SetError(this->errorProgramming);
    return;
  }
  ParserNode& leftNode=this->owner->TheObjects[this->children.TheObjects[0]];
  ParserNode& rightNode=this->owner->TheObjects[this->children.TheObjects[1]];
  if (rightNode.ExpressionType!=this->typeIntegerOrIndex)
  { this->SetError(this->errorBadIndex);
    return;
  }
  int theIndex= rightNode.intValue;
  int theDimension= this->ContextLieAlgebra->theWeyl.CartanSymmetric.NumRows;
  if (leftNode.Operation==Parser::tokenH)
  { theIndex--;
    if (theIndex>=theDimension || theIndex<0)
    { this->SetError(this->errorBadIndex);
      return;
    }
    root tempRoot;
    tempRoot.MakeEi(theDimension, theIndex);
    this->UEElement.AssignElementCartan(tempRoot, this->owner->NumVariables, *this->ContextLieAlgebra);
    this->ExpressionType=this->typeUEelement;
    return;
  }
  if (leftNode.Operation==Parser::tokenG)
  { if (theIndex<0)
      theIndex=this->ContextLieAlgebra->theWeyl.RootsOfBorel.size -theIndex-1;
    else
      theIndex--;
    theIndex=this->ContextLieAlgebra->RootIndexToGeneratorIndex(theIndex);
    if (theIndex==-1)
    { this->SetError(this->errorBadIndex);
      return;
    }
    this->UEElement.MakeOneGeneratorCoeffOne(theIndex, this->owner->NumVariables, *this->ContextLieAlgebra);
    this->ExpressionType=this->typeUEelement;
    return;
  }
  if (leftNode.Operation==Parser::tokenX)
    leftNode.Operation=Parser::tokenVariable;
  if (leftNode.Operation==Parser::tokenVariable)
  { if (theIndex<1 || theIndex>1000)
    { this->SetError(this->errorBadIndex);
      return;
    }
    this->owner->NumVariables=MathRoutines::Maximum(theIndex, this->owner->NumVariables);
    this->polyValue.MakeMonomialOneLetter((short)this->owner->NumVariables, theIndex-1, 1, (Rational) 1);
    this->ExpressionType=this->typePoly;
    return;
  }
}

bool ParserNode::ConvertToType(int theType)
{ if (this->ExpressionType==this->typeError)
    return false;
  if (theType==this->typeUndefined)
    return false;
  if (this->ExpressionType==this->typePoly)
      if (this->polyValue.NumVars< this->owner->NumVariables)
        this->polyValue.SetNumVariablesSubDeletedVarsByOne((short)this->owner->NumVariables);
  if (this->ExpressionType==this->typeUEelement)
    this->UEElement.SetNumVariables(this->owner->NumVariables);
  if (this->ExpressionType==theType)
    return true;
  if (this->ExpressionType> theType)
    return false;
  //we have eliminated the corner cases. Time to do some real conversion :)
  if (this->ExpressionType==this->typeIntegerOrIndex)
  { if (theType==this->typeRational)
      this->rationalValue= this->intValue;
    if (theType==this->typePoly)
      this->polyValue.MakeNVarConst((short) this->owner->NumVariables, (Rational) this->intValue);
    if (theType==this->typeUEelement)
      this->UEElement.AssignInt(this->intValue, (short)this->owner->NumVariables, *this->ContextLieAlgebra);
  }
  if (this->ExpressionType==this->typeRational)
  { if (theType==this->typePoly)
      this->polyValue.MakeNVarConst((short)this->owner->NumVariables, this->rationalValue);
    if (theType==this->typeUEelement)
      this->UEElement.MakeConst(this->rationalValue, this->owner->NumVariables, *this->ContextLieAlgebra);
  }
  if (this->ExpressionType==this->typePoly)
    if (theType==this->typeUEelement)
      this->UEElement.MakeConst(this->polyValue, *this->ContextLieAlgebra);
  this->ExpressionType=theType;
  return true;
}

void ParserNode::InitForAddition()
{ this->intValue=0;
  this->rationalValue.MakeZero();
  this->polyValue.Nullify((short)this->owner->NumVariables);
  this->UEElement.Nullify(this->ContextLieAlgebra);
//  this->WeylAlgebraElement.Nullify(this->owner->NumVariables);
}

void ParserNode::InitForMultiplication()
{ this->intValue=1;
  this->polyValue.MakeNVarConst((short)this->owner->NumVariables, (Rational)1);
  this->rationalValue=1;
  this->UEElement.AssignInt(1, this->owner->NumVariables, *this->ContextLieAlgebra);
}

int ParserNode::GetStrongestExpressionChildrenConvertChildrenIfNeeded()
{ int result=this->typeUndefined;
  for (int i=0; i<this->children.size; i++)
  { int childExpressionType=this->owner->TheObjects[this->children.TheObjects[i]].ExpressionType;
    if (childExpressionType>result)
      result=childExpressionType;
  }
  for (int i=0; i<this->children.size; i++)
    if(!this->owner->TheObjects[this->children.TheObjects[i]].ConvertToType(result))
    { this->SetError(this->owner->TheObjects[this->children.TheObjects[i]].ErrorType);
      return this->typeError;
    }
  return result;
}

void ParserNode::EvaluateFunction(GlobalVariables& theGlobalVariables)
{ switch(this->intValue)
  { case Parser::functionGCD: this->EvaluateGCDorLCM(theGlobalVariables); break;
    case Parser::functionLCM: this->EvaluateGCDorLCM(theGlobalVariables); break;
    case Parser::functionEigen: this->EvaluateEigen(theGlobalVariables); break;
    default: this->SetError(this->errorUnknownOperation); break;
  }
}

void ParserNode::EvaluateEigen(GlobalVariables& theGlobalVariables)
{ if (this->children.size!=1)
  { this->SetError(this->errorProgramming);
    return;
  }
  ParserNode& theArgument=this->owner->TheObjects[this->children.TheObjects[0]];
  int theDimension= theArgument.children.size;
  HomomorphismSemisimpleLieAlgebra& theHmm= this->owner->theHmm;
  if (theArgument.GetStrongestExpressionChildrenConvertChildrenIfNeeded()!=this->typeIntegerOrIndex || theDimension!=theHmm.theDomain.theWeyl.CartanSymmetric.NumRows)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  List<ElementUniversalEnveloping> theList;
  root theWeight;
  theWeight.SetSizeExpandOnTopLight(theDimension);
  for (int i=0; i<theDimension; i++)
  { ParserNode& current= this->owner->TheObjects[theArgument.children.TheObjects[i]];
    theWeight.TheObjects[i]=current.intValue;
  }
  this->owner->theHmm.WriteAllUEMonomialsWithWeightWRTDomain(theList, theWeight, theGlobalVariables);
  this->ExpressionType=this->typeUndefined;
}

void ParserNode::ConvertChildrenAndMyselfToStrongestExpressionChildren()
{ this->ExpressionType=this->GetStrongestExpressionChildrenConvertChildrenIfNeeded();
}

void ParserNode::EvaluatePlus(GlobalVariables& theGlobalVariables)
{ if (!this->AllChildrenAreOfDefinedNonErrorType())
  { this->ExpressionType=this->typeError;
    return;
  }
  this->ConvertChildrenAndMyselfToStrongestExpressionChildren();
  this->InitForAddition();
  LargeInt theInt;
  for (int i=0; i<this->children.size; i++)
  { ParserNode& currentChild=this->owner->TheObjects[this->children.TheObjects[i]];
    switch (this->ExpressionType)
    { case ParserNode::typeIntegerOrIndex:
        theInt=this->intValue;
        theInt+=currentChild.intValue;
        if (theInt.value.size>1)
        { this->ExpressionType= this->typeRational;
          this->rationalValue=theInt;
        } else
          this->intValue=theInt.value.TheObjects[0]*theInt.sign;
      break;
      case ParserNode::typeRational: this->rationalValue+=currentChild.rationalValue; break;
      case ParserNode::typePoly: this->polyValue.AddPolynomial(currentChild.polyValue); break;
      case ParserNode::typeUEelement: this->UEElement+=currentChild.UEElement; break;
      default: this->ExpressionType=this->typeError; return;
    }
  }
}

void ParserNode::EvaluateMinus(GlobalVariables& theGlobalVariables)
{ if (!this->AllChildrenAreOfDefinedNonErrorType())
  { this->ExpressionType=this->typeError;
    return;
  }
  this->InitForAddition();
  this->ConvertChildrenAndMyselfToStrongestExpressionChildren();
  for (int i=0; i<this->children.size; i++)
  { ParserNode& currentChild=this->owner->TheObjects[this->children.TheObjects[i]];
    switch (this->ExpressionType)
    { case ParserNode::typeIntegerOrIndex: if (i==0) this->intValue+=currentChild.intValue; else this->intValue-=currentChild.intValue; break;
      case ParserNode::typeRational: if (i==0) this->rationalValue+=currentChild.rationalValue; else this->rationalValue-=currentChild.rationalValue; break;
      case ParserNode::typePoly: if(i==0) this->polyValue.AddPolynomial(currentChild.polyValue); else this->polyValue.Subtract(currentChild.polyValue); break;
      case ParserNode::typeUEelement: if (i==0) this->UEElement+=currentChild.UEElement; else this->UEElement-=currentChild.UEElement; break;
      default: this->ExpressionType=this->typeError; return;
    }
  }
}

void ParserNode::EvaluateMinusUnary(GlobalVariables& theGlobalVariables)
{ if (!this->AllChildrenAreOfDefinedNonErrorType())
  { this->ExpressionType=this->typeError;
    return;
  }
  this->InitForAddition();
  this->ConvertChildrenAndMyselfToStrongestExpressionChildren();
  ParserNode& currentChild=this->owner->TheObjects[this->children.TheObjects[0]];
  switch (this->ExpressionType)
  { case ParserNode::typeIntegerOrIndex: this->intValue-=currentChild.intValue; break;
    case ParserNode::typeRational: this->rationalValue-=currentChild.rationalValue; break;
    case ParserNode::typePoly: this->polyValue.Subtract(currentChild.polyValue); break;
    case ParserNode::typeUEelement: this->UEElement-=currentChild.UEElement; break;
    default: this->ExpressionType=this->typeError; return;
  }
}

bool ParserNode::AllChildrenAreOfDefinedNonErrorType()
{ for (int i=0; i<this->children.size; i++)
    if (this->owner->TheObjects[this->children.TheObjects[i]].ExpressionType==this->typeError || this->owner->TheObjects[this->children.TheObjects[i]].ExpressionType==this->typeUndefined)
      return false;
  return true;
}

void ParserNode::EvaluateLieBracket(GlobalVariables& theGlobalVariables)
{ if (this->children.size!=2 || !this->AllChildrenAreOfDefinedNonErrorType())
  { this->ExpressionType=this->typeError;
    return;
  }
  this->ExpressionType=this->typeUEelement;
  for (int i=0; i<this->children.size; i++)
  { ParserNode& current= this->owner->TheObjects[this->children.TheObjects[i]];
    current.ConvertToType(this->typeUEelement);
  }
  ElementUniversalEnveloping& left= this->owner->TheObjects[this->children.TheObjects[0]].UEElement;
  ElementUniversalEnveloping& right= this->owner->TheObjects[this->children.TheObjects[1]].UEElement;
  left.LieBracketOnTheRight(right, this->UEElement);
}

void ParserNode::EvaluateEmbedding(GlobalVariables& theGlobalVariables)
{ if (!this->children.size==1)
  { this->SetError(this->errorProgramming);
    return;
  }
  ParserNode& child= this->owner->TheObjects[this->children.TheObjects[0]];
  if (child.ExpressionType!=this->typeUEelement)
  { this->SetError(this->errorOperationByUndefinedOrErrorType);
    return;
  }
  if (! this->owner->theHmm.ApplyHomomorphism(child.UEElement, this->UEElement, theGlobalVariables))
  { this->SetError(this->errorBadIndex);
    return;
  }
  this->ExpressionType=this->typeUEelement;
}

void ParserNode::EvaluateGCDorLCM(GlobalVariables& theGlobalVariables)
{ if (!this->AllChildrenAreOfDefinedNonErrorType())
  { this->SetError(this->errorOperationByUndefinedOrErrorType);
    return;
  }
  if (this->children.size!=1)
  { this->SetError(this->errorProgramming);
    return;
  }
  ParserNode& theRootNode= this->owner->TheObjects[this->children.TheObjects[0]];
  List<int>& theTrueChildren= theRootNode.children;
  if (theRootNode.ExpressionType!=this->typeRoot)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  if (theTrueChildren.size!=2)
  { this->SetError(this->errorWrongNumberOfArguments);
    return;
  }
  this->ExpressionType=theRootNode.GetStrongestExpressionChildrenConvertChildrenIfNeeded();
  if (this->ExpressionType==this->typeError)
    return;
  ParserNode& leftNode=this->owner->TheObjects[theTrueChildren.TheObjects[0]];
  ParserNode& rightNode=this->owner->TheObjects[theTrueChildren.TheObjects[1]];
  LargeIntUnsigned tempUI1, tempUI2, tempUI3;
  LargeInt tempInt=0;
  Rational tempRat;
  int tempI2;
  int theFunction=this->intValue;
  switch(leftNode.ExpressionType)
  { case ParserNode::typeIntegerOrIndex:
      if (leftNode.intValue==0 || rightNode.intValue==0)
      { this->SetError(this->errorDivisionByZero);
        return;
      }
      if (theFunction==Parser::functionGCD)
      { this->intValue= Rational::gcd(leftNode.intValue, rightNode.intValue);
        this->ExpressionType=this->typeIntegerOrIndex;
      } else
      { tempRat=leftNode.intValue;
        tempRat.MultiplyByInt(rightNode.intValue);
        tempI2=Rational::gcd(leftNode.intValue, rightNode.intValue);
        tempRat.DivideByInteger(tempI2);
        if (tempRat.IsSmallInteger())
        { this->intValue=tempRat.NumShort;
          this->ExpressionType=this->typeIntegerOrIndex;
        } else
        { this->rationalValue=tempRat;
          this->ExpressionType=this->typeRational;
        }
      }
      break;
    case ParserNode::typeRational:
      if (!leftNode.rationalValue.IsInteger() && !rightNode.rationalValue.IsInteger())
        this->SetError(this->errorDunnoHowToDoOperation);
      else
      { leftNode.rationalValue.GetNumUnsigned(tempUI1);
        rightNode.rationalValue.GetNumUnsigned(tempUI2);
        if (theFunction==Parser::functionGCD)
          LargeIntUnsigned::gcd(tempUI1, tempUI2, tempUI3);
        else
          LargeIntUnsigned::lcm(tempUI1, tempUI2, tempUI3);
        tempInt.AddLargeIntUnsigned(tempUI3);
        this->rationalValue.AssignLargeInteger(tempInt);
        this->ExpressionType=this->typeRational;
      }
      break;
    case ParserNode::typePoly:
      if (leftNode.polyValue.IsEqualToZero() || rightNode.polyValue.IsEqualToZero())
      { this->SetError(this->errorDivisionByZero);
        return;
      }
      if (theFunction==Parser::functionGCD)
        RationalFunction::gcd(leftNode.polyValue, rightNode.polyValue, this->polyValue);
      else
        RationalFunction::lcm(leftNode.polyValue, rightNode.polyValue, this->polyValue);
      this->ExpressionType=this->typePoly;
      break;
    case ParserNode::typeUEelement:
      this->SetError(errorDunnoHowToDoOperation);
    break;
    default: this->SetError(this->errorDunnoHowToDoOperation); return;
  }
}

void ParserNode::EvaluateDivide(GlobalVariables& theGlobalVariables)
{ if (!this->AllChildrenAreOfDefinedNonErrorType())
  { this->SetError(this->errorOperationByUndefinedOrErrorType);
    return;
  }
  if (this->children.size!=2)
  { this->SetError(this->errorProgramming);
    return;
  }
  this->InitForMultiplication();
  ParserNode& leftNode=this->owner->TheObjects[this->children.TheObjects[0]];
  ParserNode& rightNode=this->owner->TheObjects[this->children.TheObjects[1]];
  if (rightNode.ExpressionType==this->typeIntegerOrIndex)
  { rightNode.ExpressionType=this->typeRational;
    rightNode.rationalValue=rightNode.intValue;
  }
  if (rightNode.ExpressionType!=this->typeRational)
  { this->SetError(this->errorDivisionByNonAllowedType);
    return;
  }
  if (rightNode.rationalValue.IsEqualToZero())
  { this->SetError(this->errorDivisionByZero);
    return;
  }
  Rational& theDenominator= rightNode.rationalValue;
  switch(leftNode.ExpressionType)
  { case ParserNode::typeIntegerOrIndex:
      this->rationalValue=leftNode.intValue;
      this->rationalValue/=theDenominator;
      this->ExpressionType=this->typeRational;
      break;
    case ParserNode::typeRational:
      this->rationalValue=leftNode.rationalValue;
      this->rationalValue/=theDenominator;
      this->ExpressionType=this->typeRational;
      break;
    case ParserNode::typePoly:
      this->polyValue=leftNode.polyValue;
      this->polyValue.DivByRational(theDenominator);
      this->ExpressionType=this->typePoly;
      break;
    case ParserNode::typeUEelement:
      this->UEElement=leftNode.UEElement;
      this->UEElement/=theDenominator;
      this->ExpressionType=this->typeUEelement;
    break;
    default: this->SetError(this->errorDivisionByNonAllowedType); return;
  }
}

void ParserNode::EvaluateTimes(GlobalVariables& theGlobalVariables)
{ if (!this->AllChildrenAreOfDefinedNonErrorType())
  { this->SetError(this->errorOperationByUndefinedOrErrorType);
    return;
  }
  this->InitForMultiplication();
  this->ConvertChildrenAndMyselfToStrongestExpressionChildren();
  LargeInt theInt;
  for (int i=0; i<this->children.size; i++)
  { ParserNode& currentChild=this->owner->TheObjects[this->children.TheObjects[i]];
    switch (this->ExpressionType)
    { case ParserNode::typeIntegerOrIndex:
        theInt=this->intValue;
        theInt*=currentChild.intValue;
        if (theInt.value.size>1)
        { this->ExpressionType= this->typeRational;
          this->rationalValue=theInt;
        } else
          this->intValue=theInt.value.TheObjects[0]*theInt.sign;
      break;
      case ParserNode::typeRational: this->rationalValue*=currentChild.rationalValue; break;
      case ParserNode::typePoly: this->polyValue.MultiplyBy(currentChild.polyValue); break;
      case ParserNode::typeUEelement: this->UEElement*=currentChild.UEElement; break;
      default: this->SetError(this->errorMultiplicationByNonAllowedTypes); return;
    }
  }
}

ParserNode::ParserNode()
{ this->owner=0;
  this->Clear();
}

void Parser::ElementToString(std::string& output, bool useHtml, GlobalVariables& theGlobalVariables)
{ std::stringstream out; std::string tempS;
  out << "String: " << this->StringBeingParsed << "\n";
  if (useHtml)
    out << "<br>";
  out << "Tokens: ";
  for (int i=0; i<this->TokenBuffer.size; i++)
    this->TokenToStringStream(out, this->TokenBuffer.TheObjects[i]);
  out << "\nToken stack: ";
  for (int i=this->numEmptyTokensAtBeginning; i<this->TokenStack.size; i++)
    this->TokenToStringStream(out, this->TokenStack.TheObjects[i]);
  if (useHtml)
    out << "<br>";
  out << "\nValue stack: ";
  for (int i=this->numEmptyTokensAtBeginning; i<this->ValueStack.size; i++)
    out << this->ValueStack.TheObjects[i] << ", ";
  if (useHtml)
    out << "<br>";
  out << "\nElements:\n";
  if (useHtml)
    out << "<br>";
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].ElementToString(tempS);
    out << " Index: " << i << " " << tempS << ";\n";
    if (useHtml)
      out << "<br>";
  }
  if (useHtml)
    out << "<br><br>";
  out << "\n\nValue: " << this->theValue.ElementToStringValueOnly(true);

//  this->WeylAlgebraValue.ComputeDebugString(false, false);
//  this->LieAlgebraValue.ComputeDebugString(this->theLieAlgebra, false, false);
//  out << "\n\nWeyl algebra value: " << this->WeylAlgebraValue.DebugString;
//  out << "\nLie algebra value: " << this->LieAlgebraValue.DebugString;
  if (!useHtml)
  { out << "\nAmbient Lie algebra details:\n";
    out << this->theHmm.theRange.ElementToStringLieBracketPairing();
  }
  output=out.str();
}

void Parser::TokenToStringStream(std::stringstream& out, int theToken)
{ //out << theToken << " ";
  switch(theToken)
  { case Parser::tokenX: out << "x"; break;
    case Parser::tokenDigit: out << "D"; break;
    case Parser::tokenPlus: out << "+"; break;
    case Parser::tokenUnderscore: out << "_"; break;
    case Parser::tokenEmpty: out << " "; break;
    case Parser::tokenExpression: out << "E"; break;
    case Parser::tokenOpenLieBracket: out << "["; break;
    case Parser::tokenCloseLieBracket: out << "]"; break;
    case Parser::tokenLieBracket: out << "[, ]"; break;
    case Parser::tokenComma: out << ", "; break;
    case Parser::tokenOpenCurlyBracket: out << "{"; break;
    case Parser::tokenCloseCurlyBracket: out << "}"; break;
    case Parser::tokenOpenBracket: out << "("; break;
    case Parser::tokenCloseBracket: out << ")"; break;
    case Parser::tokenPartialDerivative: out << "d"; break;
    case Parser::tokenTimes: out << "*"; break;
    case Parser::tokenMinus: out << "-"; break;
    case Parser::tokenG : out << "g"; break;
    case Parser::tokenInteger: out << "Integer"; break;
    case Parser::tokenDivide: out << "/"; break;
    case Parser::tokenH: out << "h"; break;
    case Parser::tokenPower: out << "^"; break;
    case Parser::tokenC: out << "c"; break;
    case Parser::tokenMap: out << "i"; break;
    case Parser::tokenMinusUnary: out << "-"; break;
    case Parser::tokenVariable: out << "n"; break;
    case Parser::tokenRoot: out << "root"; break;
    case Parser::tokenFunction: out << "function"; break;
    default: out << "?"; break;
  }
}

void Parser::Clear()
{ this->theValue.Clear();
  this->theValue.UEElement.Nullify(&this->theHmm.theRange);
  this->TokenStack.size=0;
  this->ValueStack.size=0;
}

void Parser::ComputeNumberOfVariablesAndAdjustNodes()
{ this->NumVariables=0;
  for(int i=0; i<this->size; i++)
    if (this->NumVariables< this->TheObjects[i].WeylAlgebraElement.NumVariables)
      this->NumVariables=this->TheObjects[i].WeylAlgebraElement.NumVariables;
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].WeylAlgebraElement.SetNumVariablesPreserveExistingOnes(this->NumVariables);
}

std::string ParserNode::ElementToStringValueOnly(bool useHtml)
{ std::stringstream out;
  if (this->ExpressionType==this->typeIntegerOrIndex)
  { if (!useHtml)
      out << " an integer of value: " << this->intValue;
    else
      out << " an integer of value: <div class=\"math\">" << this->intValue << "</div>";
  }
  if (this->ExpressionType==this->typeRational)
  { if (!useHtml)
      out << " a rational number of value: " << this->rationalValue.ElementToString();
    else
      out << " a rational number of value: <div class=\"math\">" << this->rationalValue.ElementToString() << "</div>";
  }
  if (this->ExpressionType==this->typeRoot)
  { out << " is a root ";
  }
  if (this->ExpressionType==this->typePoly)
  { if (!useHtml)
      out << " a polynomial of value: " << this->polyValue.ElementToString();
    else
      out << " a polynomial of value: <div class=\"math\">\\begin{eqnarray*}&&" << this->polyValue.ElementToString() << "\\end{eqnarray*}</div>";
  }
  if (this->ExpressionType==this->typeUEelement)
  { if (!useHtml)
      out << " an element of U(g) of value: " << this->UEElement.ElementToString();
    else
      out << " an element of U(g) of value: <div class=\"math\">\\begin{eqnarray*}&&" << this->UEElement.ElementToString(true) << "\\end{eqnarray*}</div>";
  }
  if (this->ExpressionType==this->typeError)
    out << this->ElementToStringErrorCode(useHtml);
  return out.str();
}

std::string ParserNode::ElementToStringErrorCode(bool useHtml)
{ std::stringstream out;
  switch (this->ErrorType)
  { case ParserNode::errorBadIndex: out << "error: bad index"; break;
    case ParserNode::errorBadOrNoArgument: out << "error: bad or no argument"; break;
    case ParserNode::errorBadSyntax: out << "error: bad syntax."; break;
    case ParserNode::errorDunnoHowToDoOperation: out << "error: my master hasn't taught me how to do this operation (maybe he doesn't know how either)"; break;
    case ParserNode::errorDivisionByZero: out << "error: division by zero"; break;
    case ParserNode::errorDivisionByNonAllowedType: out << "error: division of/by non-allowed type"; break;
    case ParserNode::errorMultiplicationByNonAllowedTypes: out << "error: multiplication by non-allowed types"; break;
    case ParserNode::errorNoError: out << "error: error type claims no error, but expression type claims error. Slap the programmer."; break;
    case ParserNode::errorOperationByUndefinedOrErrorType: out << "error: operation with an undefined type"; break;
    case ParserNode::errorProgramming: out << "error: there has been some programming mistake (it's not your expression's fault). Slap the programmer!"; break;
    case ParserNode::errorUnknownOperation: out << "error: unknown operation. The lazy programmer has added the operation to the dictionary, but hasn't implemented it yet. Lazy programmers deserve no salary. "; break;
    default: out << "Non-documented error. Lazy programmers deserve no salaries.";
  }
  return out.str();
}

void ParserNode::ElementToString(std::string& output)
{ std::stringstream out; std::string tempS;
  owner->TokenToStringStream(out, this->Operation);
  PolyFormatLocal.MakeAlphabetArbitraryWithIndex("x");
  if (this->ExpressionType==this->typeRational)
    out << " is the rational number " << this->rationalValue.ElementToString();
  if (this->ExpressionType==this->typeIntegerOrIndex)
    out << " is the integer " << this->intValue;
  if (this->ExpressionType==this->typePoly)
  { this->polyValue.ElementToString(tempS);
    out << " is the polynomial " << tempS;
  }
  if (this->ExpressionType==this->typeRoot)
    out << " is a root ";
  if (this->ExpressionType==this->typeUndefined)
    out << " is of type undefined ";
  if (this->ExpressionType==this->typeWeylAlgebraElement)
  { this->WeylAlgebraElement.ElementToString(tempS, false, false, false);
    out << " is the algebra element " << tempS << " ";
  }
  if (this->ExpressionType==this->typeUEelement)
  { this->UEElement.ElementToString(tempS);
    out << " is the universal enveloping algebra element: " << tempS;
  }
  if (this->children.size>0)
  { out << " Its children are: ";
    for (int i=0; i<this->children.size; i++)
      out << this->children.TheObjects[i] << ", ";
  }
  output=out.str();
}

bool HomomorphismSemisimpleLieAlgebra::ComputeHomomorphismFromImagesSimpleChevalleyGenerators(GlobalVariables& theGlobalVariables)
{ int theDomainDimension= this->theDomain.theWeyl.CartanSymmetric.NumRows;
  Selection NonExplored;
  int numRoots= this->theDomain.theWeyl.RootSystem.size;
  NonExplored.init(numRoots);
  NonExplored.MakeFullSelection();
  root domainRoot, rangeRoot;
  this->theDomain.ComputeChevalleyConstants(this->theDomain.theWeyl, theGlobalVariables);
  this->theRange.ComputeChevalleyConstants(this->theRange.theWeyl, theGlobalVariables);
  List<ElementSimpleLieAlgebra> tempDomain, tempRange;
  tempDomain.SetSizeExpandOnTopNoObjectInit(numRoots+theDomainDimension);
  tempRange.SetSizeExpandOnTopNoObjectInit(numRoots+theDomainDimension);
  root tempRoot;
  for (int i=0; i<theDomainDimension; i++)
  { tempRoot.MakeEi(theDomainDimension, i);
    for (int j=0; j<2; j++, tempRoot.MinusRoot())
    { int index= this->theDomain.theWeyl.RootSystem.IndexOfObjectHash(tempRoot);
      tempDomain.TheObjects[index].Nullify(this->theDomain);
      tempDomain.TheObjects[index].SetCoefficient(tempRoot, 1, this->theDomain);
      tempRange.TheObjects[index] = this->ImagesSimpleChevalleyGenerators.TheObjects[i+j*theDomainDimension];
      NonExplored.RemoveSelection(index);
    }
  }
  ElementSimpleLieAlgebra tempElt;
  root right;
  while(NonExplored.CardinalitySelection>0)
  { for (int i=0; i<NonExplored.CardinalitySelection; i++)
    { int theIndex = NonExplored.elements[i];
      root& current = this->theDomain.theWeyl.RootSystem.TheObjects[theIndex];
      for (int j=0; j<NonExplored.MaxSize; j++)
        if (!NonExplored.selected[j])
        { root& left= this->theDomain.theWeyl.RootSystem.TheObjects[j];
          right= current-left;
          left.ComputeDebugString(); right.ComputeDebugString(); current.ComputeDebugString();
          if (this->theDomain.theWeyl.IsARoot(right))
          { int leftIndex= this->theDomain.theWeyl.RootSystem.IndexOfObjectHash(left);
            int rightIndex= this->theDomain.theWeyl.RootSystem.IndexOfObjectHash(right);
            if (!NonExplored.selected[rightIndex])
            { ElementSimpleLieAlgebra& leftDomainElt=tempDomain.TheObjects[leftIndex];
              ElementSimpleLieAlgebra& rightDomainElt= tempDomain.TheObjects[rightIndex];
              this->theDomain.LieBracket(leftDomainElt, rightDomainElt, tempDomain.TheObjects[theIndex]);
              ElementSimpleLieAlgebra& leftRangeElt=tempRange.TheObjects[leftIndex];
              ElementSimpleLieAlgebra& rightRangeElt= tempRange.TheObjects[rightIndex];
              this->theRange.LieBracket(leftRangeElt, rightRangeElt, tempRange.TheObjects[theIndex]);
              NonExplored.RemoveSelection(theIndex);
              break;
            }
          }
        }
    }
  }
  for (int i=0; i<theDomainDimension; i++)
  { tempRoot.MakeEi(theDomainDimension, i);
    int leftIndex= this->theDomain.theWeyl.RootSystem.IndexOfObjectHash(tempRoot);
    int rightIndex= this->theDomain.theWeyl.RootSystem.IndexOfObjectHash(-tempRoot);
    this->theDomain.LieBracket(tempDomain.TheObjects[leftIndex], tempDomain.TheObjects[rightIndex], tempDomain.TheObjects[numRoots+i]);
    this->theRange.LieBracket(tempRange.TheObjects[leftIndex], tempRange.TheObjects[rightIndex], tempRange.TheObjects[numRoots+i]);
  }

  roots vectorsLeft, vectorsRight;
  vectorsLeft.SetSizeExpandOnTopNoObjectInit(tempDomain.size);
  vectorsRight.SetSizeExpandOnTopNoObjectInit(tempDomain.size);
  for (int i=0; i<tempRange.size; i++)
  { tempDomain.TheObjects[i].ElementToVectorRootSpacesFirstThenCartan(vectorsLeft.TheObjects[i]);
    tempRange.TheObjects[i].ElementToVectorRootSpacesFirstThenCartan(vectorsRight.TheObjects[i]);
  }
  MatrixLargeRational tempMat;
  tempMat.MakeLinearOperatorFromDomainAndRange(vectorsLeft, vectorsRight, theGlobalVariables);
  root imageRoot;
  this->theChevalleyGenerators.SetSizeExpandOnTopNoObjectInit(tempDomain.size);
  this->ImagesAllChevalleyGenerators.SetSizeExpandOnTopNoObjectInit(tempDomain.size);
  for (int i=0; i<this->theDomain.theWeyl.RootSystem.size; i++)
  { tempElt.Nullify(this->theDomain);
    tempElt.SetCoefficient(this->theDomain.theWeyl.RootSystem.TheObjects[i], 1, this->theDomain);
    this->theChevalleyGenerators.TheObjects[this->theDomain.RootIndexToGeneratorIndex(i)]=tempElt;
  }
  for (int i=0; i<theDomainDimension; i++)
  { tempElt.Nullify(this->theDomain);
    tempElt.Hcomponent.MakeEi(theDomainDimension, i);
    this->theChevalleyGenerators.TheObjects[this->theDomain.theWeyl.RootsOfBorel.size+i]=tempElt;
  }
  for (int i=0; i<this->ImagesAllChevalleyGenerators.size; i++)
  { this->theChevalleyGenerators.TheObjects[i].ElementToVectorRootSpacesFirstThenCartan(tempRoot);
    tempMat.ActOnAroot(tempRoot, imageRoot);
    this->ImagesAllChevalleyGenerators.TheObjects[i].AssingVectorRootSpacesFirstThenCartan(imageRoot, this->theRange);
  }
  return true;
}

void HomomorphismSemisimpleLieAlgebra::WriteAllUEMonomialsWithWeightWRTDomain(List<ElementUniversalEnveloping>& output, root& theWeight, GlobalVariables& theGlobalVariables)
{ output.size=0;
//  this->theDomain.theWeyl.
  roots PosRootsEmbeddings, PosRootsProjections;
  root imageOfTheWeight;
  int theDimension=this->theRange.theWeyl.CartanSymmetric.NumRows;
  int theDomainDimension= this->theDomain.theWeyl.CartanSymmetric.NumRows;
  imageOfTheWeight.MakeZero(theDimension);
  int numPosRootsDomain=this->theDomain.theWeyl.RootsOfBorel.size;
  int numPosRootsRange=this->theRange.theWeyl.RootsOfBorel.size;
  PosRootsEmbeddings.SetSizeExpandOnTopNoObjectInit(numPosRootsDomain);
  for (int i=0; i<numPosRootsDomain; i++)
  { PosRootsEmbeddings.TheObjects[i].MakeZero(theDimension);
    for (int j=0; j<theDomainDimension; j++)
      PosRootsEmbeddings.TheObjects[i]+=this->ImagesAllChevalleyGenerators.TheObjects[numPosRootsDomain+j].Hcomponent*this->theDomain.theWeyl.RootsOfBorel.TheObjects[i].TheObjects[j];
  }
  std::cout << "Embeddings of roots:" << PosRootsEmbeddings.ElementToString(false, true, true);
  PosRootsProjections.SetSizeExpandOnTopNoObjectInit(numPosRootsRange);
  for (int i=0; i<numPosRootsRange; i++)
    this->ProjectOntoSmallCartan(this->theRange.theWeyl.RootsOfBorel.TheObjects[i], PosRootsProjections.TheObjects[i], theGlobalVariables);
  std::cout << "<br>Projections of roots: " << PosRootsProjections.ElementToString(false, true, true);
  VectorPartition theVP;
  theVP.PartitioningRoots=PosRootsProjections;
  theVP.theRoot=theWeight;
  theVP.ComputeAllPartitions();
  std::cout << "the partitions: <br>" << theVP.ElementToString(true);
  output.SetSizeExpandOnTopNoObjectInit(theVP.thePartitions.size);
  MonomialUniversalEnveloping currentMon;
  ElementUniversalEnveloping tempElt;
  for (int i=0; i<output.size; i++)
  { currentMon.Nullify(theDomainDimension, this->theRange);
    Rational tempRat=1;
    currentMon.Coefficient.ComputeDebugString();
    currentMon.Coefficient.MakeNVarConst((short)theDimension, tempRat);
    for (int j=0; j<theVP.thePartitions.TheObjects[i].size; j++)
      currentMon.MultiplyByGeneratorPowerOnTheRight(numPosRootsRange+theDimension+j, theVP.thePartitions.TheObjects[i].TheObjects[j]);
    std::cout << currentMon.ElementToString(false) << "<br>" ;
    tempElt.Nullify(&this->theRange);
    tempElt.AddObjectOnTopHash(currentMon);
    output.TheObjects[i]=tempElt;
  }
  List<List<ElementUniversalEnveloping> > targets;
  List<List<ElementUniversalEnveloping> > targetsNoMod;
  targets.SetSizeExpandOnTopNoObjectInit(theDomainDimension);
  targetsNoMod.SetSizeExpandOnTopNoObjectInit(theDomainDimension);
  ElementUniversalEnveloping theSimpleGenerator;
  std::string beginMath="<DIV class=\"math\" scale=\"50\">";
  std::string endMath="</DIV>";
  List<List<PolynomialRationalCoeff> > theSystem;
  theSystem.size=0;
  theSystem.SetSizeExpandOnTopNoObjectInit(output.size);
  ElementUniversalEnveloping basisMonomialBuffer;
  for (int i=0; i<targets.size; i++)
  { List<ElementUniversalEnveloping>& currentTargets= targets.TheObjects[i];
    List<ElementUniversalEnveloping>& currentTargetsNoMod= targetsNoMod.TheObjects[i];
    theSimpleGenerator.AssignElementLieAlgebra(this->ImagesSimpleChevalleyGenerators.TheObjects[i], theDimension, this->theRange);
    theSimpleGenerator.ComputeDebugString();
    std::cout << "Generator number " << i+1 << ": " << beginMath;
    for (int j=0; j<output.size; j++)
    { theSimpleGenerator.LieBracketOnTheRight(output.TheObjects[j], tempElt);
      tempElt.Simplify();
      currentTargetsNoMod.AddObjectOnTop(tempElt);
      tempElt.ModOutVermaRelations();
      currentTargets.AddObjectOnTop(tempElt);
      std::cout << tempElt.ElementToString() << ", \\quad ";
    }
    std::cout << endMath << "\n<br>";
    std::cout << "Elements before modding out: " << beginMath;
    for (int j=0; j<output.size; j++)
      std::cout << currentTargetsNoMod.TheObjects[j].ElementToString() << ", \\quad ";
    std::cout << endMath << "\n<br>";
    List<rootPoly> tempList;
    //Let the monomials corresponding to the given partition be m_1, \dots, m_l
    //Let the Chevalley generators of the smaller Lie algebra be k_1,\dots, k_s
    //Then the elements [k_i, m_1], \dots, [k_i, m_l] are recorded in this order in currentTargets
    ElementUniversalEnveloping::GetCoordinateFormOfSpanOfElements(currentTargets, tempList, basisMonomialBuffer, theGlobalVariables);
    std::cout << "Coordinate form of the above elements: ";
    for (int j=0; j<tempList.size; j++)
    { std::cout << tempList.TheObjects[j].ElementToString() << ",";
      //theSystem holds in the j^th row the action on the monomial m_j
      theSystem.TheObjects[j].AddListOnTop(tempList.TheObjects[j]);
    }
    std::cout << "<br>";
  }
  Matrix<RationalFunction> matSystem;
  int numEquations=theSystem.TheObjects[0].size;
  matSystem.init(numEquations, output.size);
  for (int i=0; i<matSystem.NumRows; i++)
    for (int j=0; j<matSystem.NumCols; j++)
      matSystem.elements[i][j]=theSystem.TheObjects[j].TheObjects[i];
  matSystem.ComputeDebugString(false, true);
  std::cout << "<br>The system we need to solve:<br>" << beginMath << matSystem.DebugString << endMath << "<br>";
  RationalFunction ZeroPoly, UnitPoly, MinusUnitPoly;
  ZeroPoly.MakeNVarConst(theDimension, (Rational) 0);
  UnitPoly.MakeNVarConst(theDimension, (Rational) 1);
  MinusUnitPoly.MakeNVarConst(theDimension, (Rational) -1);
  List<List<RationalFunction> > theAnswer;
  matSystem.FindZeroEigenSpacE(theAnswer, UnitPoly, MinusUnitPoly, ZeroPoly, theGlobalVariables);
  std::cout << "The found solutions: <br>";
  rootRationalFunction tempRatRoot;
  std::string tempS;
  for (int i=0; i<theAnswer.size; i++)
  { tempRatRoot.CopyFromBase(theAnswer.TheObjects[i]);
    std::cout << beginMath << tempRatRoot.ElementToString() << endMath << "<br>";
    std::cout << "Corresponding expression in monomial form: " << beginMath;
    for (int j=0; j<output.size; j++)
    { RationalFunction& currentCoeff= theAnswer.TheObjects[i].TheObjects[j];
      if (!currentCoeff.IsEqualToZero())
      { tempS= currentCoeff.ElementToString(true, false);
        if (tempS=="-1")
          std::cout << "-";
        else
        { if (j!=0)
            std::cout << "+";
          if (tempS!="1")
            std::cout << "(" << tempS << ")";
        }
        std::cout << output.TheObjects[j].ElementToString();
      }
    }
    std::cout << endMath << "<br>";
  }
}

void HomomorphismSemisimpleLieAlgebra::ProjectOntoSmallCartan(root& input, root& output, GlobalVariables& theGlobalVariables)
{ MatrixLargeRational invertedSmallCartan;
  invertedSmallCartan=this->theDomain.theWeyl.CartanSymmetric;
  invertedSmallCartan.Invert(theGlobalVariables);
  int theSmallDimension=this->theDomain.theWeyl.CartanSymmetric.NumRows;
  output.MakeZero(theSmallDimension);
  for (int i=0; i<theSmallDimension; i++)
    output.TheObjects[i]= this->theRange.theWeyl.RootScalarCartanRoot(this->ImagesAllChevalleyGenerators.TheObjects[this->theDomain.theWeyl.RootsOfBorel.size+i].Hcomponent, input);
  invertedSmallCartan.ActOnAroot(output, output);
}

bool HomomorphismSemisimpleLieAlgebra::ApplyHomomorphism(MonomialUniversalEnveloping& input, ElementUniversalEnveloping& output, GlobalVariables& theGlobalVariables)
{ ElementUniversalEnveloping tempElt;
  output.Nullify(&this->theRange);
  output.MakeConst(input.Coefficient, this->theRange);
  for (int i=0; i<input.generatorsIndices.size; i++)
  { if (input.generatorsIndices.TheObjects[i]>=this->ImagesAllChevalleyGenerators.size)
      return false;
    tempElt.AssignElementLieAlgebra(this->ImagesAllChevalleyGenerators.TheObjects[input.generatorsIndices.TheObjects[i]], input.Coefficient.NumVars, this->theRange);
    PolynomialRationalCoeff& thePower=input.Powers.TheObjects[i];
    if (thePower.TotalDegree()>0)
      return false;
    int theIntegralPower=thePower.TheObjects[0].Coefficient.NumShort;
    for (int j=0; j<theIntegralPower; j++)
      output*=tempElt;
  }
  return true;
}

bool HomomorphismSemisimpleLieAlgebra::ApplyHomomorphism(ElementUniversalEnveloping& input, ElementUniversalEnveloping& output, GlobalVariables& theGlobalVariables)
{ assert(&output!=&input);
  output.Nullify(&this->theRange);
  ElementUniversalEnveloping tempElt;
  for (int i=0; i<input.size; i++)
  { if(!this->ApplyHomomorphism(input.TheObjects[i], tempElt, theGlobalVariables))
      return false;
    output+=tempElt;
  }
  return true;
}

void HomomorphismSemisimpleLieAlgebra::MakeG2InB3(Parser& owner, GlobalVariables& theGlobalVariables)
{ owner.DefaultWeylLetter='B';
  owner.DefaultWeylRank=3;
  owner.theHmm.theRange.ComputeChevalleyConstants(owner.DefaultWeylLetter, owner.DefaultWeylRank, theGlobalVariables);
  this->theDomain.ComputeChevalleyConstants('G', 2, theGlobalVariables);
  this->theRange.ComputeChevalleyConstants('B', 3, theGlobalVariables);
  this->ImagesSimpleChevalleyGenerators.SetSizeExpandOnTopNoObjectInit(4);
  (owner.ParseAndCompute("g_2", theGlobalVariables)).ConvertToLieAlgebraElementIfPossible(this->ImagesSimpleChevalleyGenerators.TheObjects[0]);
  (owner.ParseAndCompute("g_1+g_3", theGlobalVariables)).ConvertToLieAlgebraElementIfPossible(this->ImagesSimpleChevalleyGenerators.TheObjects[1]);
  (owner.ParseAndCompute("g_{-2}", theGlobalVariables)).ConvertToLieAlgebraElementIfPossible(this->ImagesSimpleChevalleyGenerators.TheObjects[2]);
  (owner.ParseAndCompute("g_{-1}+g_{-3}", theGlobalVariables)).ConvertToLieAlgebraElementIfPossible(this->ImagesSimpleChevalleyGenerators.TheObjects[3]);
  this->ComputeHomomorphismFromImagesSimpleChevalleyGenerators(theGlobalVariables);
  owner.Clear();
  this->GetRestrictionAmbientRootSystemToTheSmallerCartanSA(this->RestrictedRootSystem, theGlobalVariables);
  //this->ComputeDebugString(true, theGlobalVariables);
  //std::cout << this->DebugString;
  //if (this->CheckClosednessLieBracket(theGlobalVariables))
  //{ std::cout <<"good good good good!!!!";
  //}
}

void HomomorphismSemisimpleLieAlgebra::ElementToString(std::string& output, bool useHtml, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  std::string tempS, tempS2;
  if (this->CheckClosednessLieBracket(theGlobalVariables))
    out << "Lie bracket closes, everything is good!";
  else
    out << "The Lie bracket is BAD BAD BAD!";
  if (useHtml)
    out << "<br>";
  out << "Images simple Chevalley generators:\n\n";
  if (useHtml)
    out << "<br>";
  for (int i=0; i<this->ImagesSimpleChevalleyGenerators.size; i++)
  { this->ImagesSimpleChevalleyGenerators.TheObjects[i].ElementToString(tempS, this->theRange, false, false);
    out << tempS << "\n\n";
    if (useHtml)
      out << "<br>";
  }
  out << "Maps of Chevalley generators:\n\n";
  for (int i=0; i<this->theChevalleyGenerators.size; i++)
  { this->ImagesAllChevalleyGenerators.TheObjects[i].ElementToString(tempS, this->theRange, false, false);
    this->theChevalleyGenerators.TheObjects[i].ElementToString(tempS2, this->theDomain, false, false);
    out << tempS2 << " \\mapsto " << tempS << "\n\n";
    if  (useHtml)
      out << "<br>";
  }

  output=out.str();
}

void HomomorphismSemisimpleLieAlgebra::GetRestrictionAmbientRootSystemToTheSmallerCartanSA(roots& output, GlobalVariables& theGlobalVariables)
{ List<root>& theRootSystem= this->theRange.theWeyl.RootSystem;
  int rankSA=this->theDomain.theWeyl.CartanSymmetric.NumRows;
  MatrixLargeRational tempMat;
  tempMat.Assign(this->theDomain.theWeyl.CartanSymmetric);
  tempMat.Invert(theGlobalVariables);
  int numPosRootsDomain=this->theDomain.theWeyl.RootsOfBorel.size;
  output.SetSizeExpandOnTopNoObjectInit(theRootSystem.size);
  root theScalarProducts;
  theScalarProducts.SetSizeExpandOnTopLight(rankSA);
//  tempMat.ComputeDebugString(true, false);
//  std::cout << tempMat.DebugString << "<br>";
  for (int i=0; i<theRootSystem.size; i++)
  { for (int j=0; j<rankSA; j++)
    { ElementSimpleLieAlgebra& currentH=this->ImagesAllChevalleyGenerators.TheObjects[j+numPosRootsDomain];
      theScalarProducts.TheObjects[j]=this->theRange.theWeyl.RootScalarCartanRoot(currentH.Hcomponent, theRootSystem.TheObjects[i]);
    }
    tempMat.ActOnAroot(theScalarProducts, output.TheObjects[i]);
  }
}

bool HomomorphismSemisimpleLieAlgebra::CheckClosednessLieBracket(GlobalVariables& theGlobalVariables)
{ ElementSimpleLieAlgebra tempElt;
  roots tempRoots; root tempRoot;
  tempRoots.SetSizeExpandOnTopNoObjectInit(this->ImagesAllChevalleyGenerators.size);
  for (int i=0; i<tempRoots.size; i++)
    this->ImagesAllChevalleyGenerators.TheObjects[i].ElementToVectorRootSpacesFirstThenCartan(tempRoots.TheObjects[i]);
  for (int i=0; i<this->ImagesAllChevalleyGenerators.size; i++)
    for (int j=0; j<this->ImagesAllChevalleyGenerators.size; j++)
    { this->theRange.LieBracket(this->ImagesAllChevalleyGenerators.TheObjects[i], this->ImagesAllChevalleyGenerators.TheObjects[j], tempElt);
      tempElt.ElementToVectorRootSpacesFirstThenCartan(tempRoot);
      if(!tempRoots.LinSpanContainsRoot(tempRoot, theGlobalVariables))
        return false;
    }
  return true;
}

void MonomialUniversalEnveloping::SimplifyAccumulateInOutputNoOutputInit(ElementUniversalEnveloping& output)
{ int IndexlowestNonSimplified=0;
  ElementUniversalEnveloping buffer2;
  MonomialUniversalEnveloping tempMon;
  //simplified order is descending order
  while (IndexlowestNonSimplified<output.size)
  { bool reductionOccurred=false;
    if (output.TheObjects[IndexlowestNonSimplified].Coefficient.IsEqualToZero())
      reductionOccurred=true;
    else
      for (int i=0; i<output.TheObjects[IndexlowestNonSimplified].generatorsIndices.size-1; i++)
        if (!this->owner->AreOrderedProperly(output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i], output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i+1]))
        { if (output.TheObjects[IndexlowestNonSimplified].SwitchConsecutiveIndicesIfTheyCommute(i, tempMon))
          { output.AddMonomialNoCleanUpZeroCoeff(tempMon);
            tempMon.ComputeDebugString();
            reductionOccurred=true;
            break;
          }
          if (this->CommutingRightIndexAroundLeftIndexAllowed(output.TheObjects[IndexlowestNonSimplified].Powers.TheObjects[i], output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i], output.TheObjects[IndexlowestNonSimplified].Powers.TheObjects[i+1], output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i+1]))
          { output.TheObjects[IndexlowestNonSimplified].CommuteConsecutiveIndicesRightIndexAroundLeft(i, buffer2);
            for (int j=0; j<buffer2.size; j++)
              output.AddMonomialNoCleanUpZeroCoeff(buffer2.TheObjects[j]);
            output.ComputeDebugString();
            reductionOccurred=true;
            break;
          }
          if (this->CommutingLeftIndexAroundRightIndexAllowed(output.TheObjects[IndexlowestNonSimplified].Powers.TheObjects[i], output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i], output.TheObjects[IndexlowestNonSimplified].Powers.TheObjects[i+1], output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i+1]))
          { output.TheObjects[IndexlowestNonSimplified].CommuteConsecutiveIndicesLeftIndexAroundRight(i, buffer2);
            for (int j=0; j<buffer2.size; j++)
              output.AddMonomialNoCleanUpZeroCoeff(buffer2.TheObjects[j]);
            output.ComputeDebugString();
            reductionOccurred=true;
            break;
          }
        }
    if (reductionOccurred)
      output.PopIndexSwapWithLastHash(IndexlowestNonSimplified);
    else
      IndexlowestNonSimplified++;
    output.ComputeDebugString();
  }
  output.CleanUpZeroCoeff();
}

bool MonomialUniversalEnveloping::CommutingLeftIndexAroundRightIndexAllowed(PolynomialRationalCoeff& theLeftPower, int leftGeneratorIndex, PolynomialRationalCoeff& theRightPower, int rightGeneratorIndex)
{ if (theLeftPower.TotalDegree()==0)
  { if(theRightPower.TotalDegree()==0)
      return true;
    int numPosRoots=this->owner->theWeyl.RootsOfBorel.size;
    int theDimension= this->owner->theWeyl.CartanSymmetric.NumRows;
    if(rightGeneratorIndex>= numPosRoots && rightGeneratorIndex<numPosRoots+theDimension)
    { if (this->owner->theLiebracketPairingCoefficients.elements[leftGeneratorIndex][rightGeneratorIndex].IsEqualToZero())
        return true;
      else
        return false;
    } else
      return true;
  }
  return false;
}

bool MonomialUniversalEnveloping::SwitchConsecutiveIndicesIfTheyCommute(int theLeftIndex, MonomialUniversalEnveloping& output)
{ if (theLeftIndex>= this->generatorsIndices.size-1)
    return false;
  int leftGenerator=this->generatorsIndices.TheObjects[theLeftIndex];
  int rightGenerator=this->generatorsIndices.TheObjects[theLeftIndex+1];
  if (this->owner->OppositeRootSpaces.TheObjects[leftGenerator]==rightGenerator)
    return false;
  if (this->owner->theLiebracketPairingCoefficients.elements[leftGenerator][rightGenerator].IsEqualToZero())
  { this->ComputeDebugString();
    output.generatorsIndices.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size);
    output.Powers.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size);
    output.Nullify(this->Coefficient.NumVars, *this->owner);
    output.Coefficient=this->Coefficient;
    output.ComputeDebugString();
    for (int i=0; i<theLeftIndex; i++)
      output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
    output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[theLeftIndex+1], this->Powers.TheObjects[theLeftIndex+1]);
    output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[theLeftIndex], this->Powers.TheObjects[theLeftIndex]);
    for (int i=theLeftIndex+2; i<this->generatorsIndices.size; i++)
      output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
    return true;
  }
  return false;
}

bool MonomialUniversalEnveloping::CommutingRightIndexAroundLeftIndexAllowed(PolynomialRationalCoeff& theLeftPower, int leftGeneratorIndex, PolynomialRationalCoeff& theRightPower, int rightGeneratorIndex)
{ return this->CommutingLeftIndexAroundRightIndexAllowed(theRightPower, rightGeneratorIndex, theLeftPower, leftGeneratorIndex);
}

void MonomialUniversalEnveloping::Simplify(ElementUniversalEnveloping& output)
{ output.Nullify(this->owner);
  output.AddObjectOnTopHash(*this);
  this->SimplifyAccumulateInOutputNoOutputInit(output);
}

void MonomialUniversalEnveloping::MultiplyByNoSimplify(const MonomialUniversalEnveloping& other)
{ this->generatorsIndices.MakeActualSizeAtLeastExpandOnTop(other.generatorsIndices.size+this->generatorsIndices.size);
  this->Powers.MakeActualSizeAtLeastExpandOnTop(other.generatorsIndices.size+this->generatorsIndices.size);
  this->Coefficient.MultiplyBy(other.Coefficient);
  if (other.generatorsIndices.size==0)
    return;
  int firstIndex=other.generatorsIndices.TheObjects[0];
  int i=0;
  if (this->generatorsIndices.size>0)
    if (firstIndex==(*this->generatorsIndices.LastObject()))
    { *this->Powers.LastObject()+=other.Powers.TheObjects[0];
      i=1;
    }
  for (; i<other.generatorsIndices.size; i++)
  { this->Powers.AddObjectOnTop(other.Powers.TheObjects[i]);
    this->generatorsIndices.AddObjectOnTop(other.generatorsIndices.TheObjects[i]);
  }
}

void MonomialUniversalEnveloping::CommuteConsecutiveIndicesRightIndexAroundLeft(int theIndeX, ElementUniversalEnveloping& output)
{ if (theIndeX==this->generatorsIndices.size-1)
    return;
  output.Nullify(this->owner);
  MonomialUniversalEnveloping tempMon;
  tempMon.Nullify(this->Coefficient.NumVars, *this->owner);
  tempMon.Powers.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size+2);
  tempMon.generatorsIndices.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size+2);
  tempMon.Powers.size=0;
  tempMon.generatorsIndices.size=0;
  int rightGeneratorIndeX= this->generatorsIndices.TheObjects[theIndeX+1];
  int leftGeneratorIndeX=this->generatorsIndices.TheObjects[theIndeX];
  PolynomialRationalCoeff theRightPoweR, theLeftPoweR;
  theRightPoweR= this->Powers.TheObjects[theIndeX+1];
  theLeftPoweR= this->Powers.TheObjects[theIndeX];
  theRightPoweR-=1;
  int powerDroP=0;
  PolynomialRationalCoeff acquiredCoefficienT, polyOne;
  acquiredCoefficienT.Assign(this->Coefficient);
  for (int i=0; i<theIndeX; i++)
    tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
  MonomialUniversalEnveloping startMon;
  startMon=tempMon;
  ElementSimpleLieAlgebra adResulT, tempElT, tempLefttElt;
  adResulT.AssignGeneratorCoeffOne(rightGeneratorIndeX, *this->owner);
  tempLefttElt.AssignGeneratorCoeffOne(leftGeneratorIndeX, *this->owner);
  //tempLefttElt.ComputeDebugString(*this->owner, false, false);
  polyOne.MakeNVarConst(this->Coefficient.NumVars, (Rational) 1);
  do
  { //acquiredCoefficienT.ComputeDebugString();
    //theRightPoweR.ComputeDebugString();
    //theLeftPoweR.ComputeDebugString();
    //adResulT.ComputeDebugString(*this->owner, false, false);
    //tempMon.ComputeDebugString();
    if (adResulT.NonZeroElements.CardinalitySelection>0)
    { int theNewGeneratorIndex= this->owner->RootIndexToGeneratorIndex(adResulT.NonZeroElements.elements[0]);
      tempMon.Coefficient=acquiredCoefficienT;
      tempMon.Coefficient.TimesConstant(adResulT.coeffsRootSpaces.TheObjects[adResulT.NonZeroElements.elements[0]]);
      tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, polyOne);
      tempMon.MultiplyByGeneratorPowerOnTheRight(leftGeneratorIndeX, theLeftPoweR);
      tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndeX, theRightPoweR);
      for (int i=theIndeX+2; i<this->generatorsIndices.size; i++)
        tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
      //tempMon.ComputeDebugString();
      output.AddObjectOnTopHash(tempMon);
    } else
    { int theDimension=this->owner->theWeyl.CartanSymmetric.NumRows;
      int numPosRoots= this->owner->theWeyl.RootsOfBorel.size;
      for (int i=0; i<theDimension; i++)
        if (!adResulT.Hcomponent.TheObjects[i].IsEqualToZero())
        { tempMon=startMon;
          tempMon.Coefficient=acquiredCoefficienT;
          tempMon.Coefficient.TimesConstant(adResulT.Hcomponent.TheObjects[i]);
          tempMon.MultiplyByGeneratorPowerOnTheRight(i+numPosRoots, polyOne);
          tempMon.MultiplyByGeneratorPowerOnTheRight(leftGeneratorIndeX, theLeftPoweR);
          tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndeX, theRightPoweR);
          for (int i=theIndeX+2; i<this->generatorsIndices.size; i++)
            tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
          //tempMon.ComputeDebugString();
          output.AddObjectOnTopHash(tempMon);
        }
    }
    acquiredCoefficienT.MultiplyBy(theLeftPoweR);
    theLeftPoweR-=1;
    tempMon=startMon;
    this->owner->LieBracket(tempLefttElt, adResulT, tempElT);
    adResulT=tempElT;
    powerDroP++;
    acquiredCoefficienT.DivByInteger(powerDroP);
    //adResulT.ComputeDebugString(*this->owner, false, false);
  }while(!adResulT.IsEqualToZero() && !acquiredCoefficienT.IsEqualToZero());
}

void MonomialUniversalEnveloping::CommuteConsecutiveIndicesLeftIndexAroundRight(int theIndeX, ElementUniversalEnveloping& output)
{ if (theIndeX==this->generatorsIndices.size-1)
    return;
  output.Nullify(this->owner);
  MonomialUniversalEnveloping tempMon;
  tempMon.Nullify(this->Coefficient.NumVars, *this->owner);
  tempMon.Powers.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size+2);
  tempMon.generatorsIndices.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size+2);
  tempMon.Powers.size=0;
  tempMon.generatorsIndices.size=0;
  int rightGeneratorIndex= this->generatorsIndices.TheObjects[theIndeX+1];
  int leftGeneratorIndex=this->generatorsIndices.TheObjects[theIndeX];
  PolynomialRationalCoeff theRightPower, theLeftPower;
  theRightPower= this->Powers.TheObjects[theIndeX+1];
  theLeftPower= this->Powers.TheObjects[theIndeX];
  theLeftPower-=1;
  int powerDrop=0;
  PolynomialRationalCoeff acquiredCoefficient, polyOne;
  acquiredCoefficient.Assign(this->Coefficient);
  for (int i=0; i<theIndeX; i++)
    tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
  tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[theIndeX], theLeftPower);
  MonomialUniversalEnveloping startMon, tempMon2;
  startMon=tempMon;
  ElementSimpleLieAlgebra adResult, tempElt, tempRightElt;
  adResult.AssignGeneratorCoeffOne(leftGeneratorIndex, *this->owner);
  tempRightElt.AssignGeneratorCoeffOne(rightGeneratorIndex, *this->owner);
//  tempRightElt.ComputeDebugString(*this->owner, false, false);
  polyOne.MakeNVarConst(this->Coefficient.NumVars, (Rational) 1);
  do
  { //acquiredCoefficient.ComputeDebugString();
    //theRightPower.ComputeDebugString();
    //adResult.ComputeDebugString(*this->owner, false, false);
    //tempMon.ComputeDebugString();
    tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndex, theRightPower);
    //tempMon.ComputeDebugString();
    if (adResult.NonZeroElements.CardinalitySelection>0)
    { int theNewGeneratorIndex= this->owner->RootIndexToGeneratorIndex(adResult.NonZeroElements.elements[0]);
      tempMon.Coefficient=acquiredCoefficient;
      tempMon.Coefficient.TimesConstant(adResult.coeffsRootSpaces.TheObjects[adResult.NonZeroElements.elements[0]]);
      tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, polyOne);
      for (int i=theIndeX+2; i<this->generatorsIndices.size; i++)
        tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
      //tempMon.ComputeDebugString();
      output.AddObjectOnTopHash(tempMon);
    } else
    { int theDimension=this->owner->theWeyl.CartanSymmetric.NumRows;
      int numPosRoots= this->owner->theWeyl.RootsOfBorel.size;
      tempMon.Coefficient=acquiredCoefficient;
      tempMon2=tempMon;
      for (int i=0; i<theDimension; i++)
        if (!adResult.Hcomponent.TheObjects[i].IsEqualToZero())
        { tempMon=tempMon2;
          tempMon.Coefficient.TimesConstant(adResult.Hcomponent.TheObjects[i]);
          tempMon.MultiplyByGeneratorPowerOnTheRight(i+numPosRoots, polyOne);
          for (int i=theIndeX+2; i<this->generatorsIndices.size; i++)
            tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
          //tempMon.ComputeDebugString();
          output.AddObjectOnTopHash(tempMon);
        }
    }
    acquiredCoefficient.MultiplyBy(theRightPower);
    theRightPower-=1;
    tempMon=startMon;
    this->owner->LieBracket(adResult, tempRightElt, tempElt);
    adResult=tempElt;
    powerDrop++;
    acquiredCoefficient.DivByInteger(powerDrop);
    //adResult.ComputeDebugString(*this->owner, false, false);
  }while(!adResult.IsEqualToZero() && !acquiredCoefficient.IsEqualToZero());
}

void MonomialUniversalEnveloping::MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, int thePower)
{ if (thePower==0)
    return;
  PolynomialRationalCoeff tempP;
  tempP.MakeNVarConst(this->Coefficient.NumVars, thePower);
  this->MultiplyByGeneratorPowerOnTheRight(theGeneratorIndex, tempP);
}

void MonomialUniversalEnveloping::MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, const PolynomialRationalCoeff& thePower)
{ if (thePower.IsEqualToZero())
    return;
  if (this->generatorsIndices.size>0)
    if (*this->generatorsIndices.LastObject()==theGeneratorIndex)
    { this->Powers.LastObject()->AddPolynomial(thePower);
      return;
    }
  this->Powers.AddObjectOnTop(thePower);
  this->generatorsIndices.AddObjectOnTop(theGeneratorIndex);
}

void ElementUniversalEnveloping::AddMonomialNoCleanUpZeroCoeff(const MonomialUniversalEnveloping& input)
{ int theIndex= this->IndexOfObjectHash(input);
  if (theIndex==-1)
    this->AddObjectOnTopHash(input);
  else
    this->TheObjects[theIndex].Coefficient+=input.Coefficient;
}

void ElementUniversalEnveloping::Simplify()
{ ElementUniversalEnveloping buffer;
  ElementUniversalEnveloping output;
  //this->ComputeDebugString();
  output.Nullify(this->owner);
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].Simplify(buffer);
    output+=buffer;
    //output.ComputeDebugString();
  }
  *this=output;
}

void ElementUniversalEnveloping::LieBracketOnTheRight(const ElementUniversalEnveloping& right, ElementUniversalEnveloping& output)
{ ElementUniversalEnveloping tempElt, tempElt2;
  tempElt=*this;
  tempElt*=right;
  tempElt2=right;
  tempElt2*=*this;
  output=tempElt;
  output-=tempElt2;
}

void ElementUniversalEnveloping::AddMonomial(const MonomialUniversalEnveloping& input)
{ int theIndex= this->IndexOfObjectHash(input);
  if (theIndex==-1)
    this->AddObjectOnTopHash(input);
  else
  { this->TheObjects[theIndex].Coefficient+=input.Coefficient;
    if (this->TheObjects[theIndex].Coefficient.IsEqualToZero())
      this->PopIndexSwapWithLastHash(theIndex);
  }
}

void ElementUniversalEnveloping::CleanUpZeroCoeff()
{ for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].Coefficient.IsEqualToZero())
    { this->PopIndexSwapWithLastHash(i);
      i--;
    }
}

void ElementUniversalEnveloping::MakeOneGeneratorCoeffOne(int theIndex, int numVars, SemisimpleLieAlgebra& theOwner)
{ this->Nullify(&theOwner);
  MonomialUniversalEnveloping tempMon;
  tempMon.Nullify(numVars, theOwner);
  tempMon.Coefficient.MakeNVarConst((short)numVars, (Rational) 1);
  tempMon.MultiplyByGeneratorPowerOnTheRight(theIndex, tempMon.Coefficient);
  this->AddObjectOnTopHash(tempMon);
}

bool ElementUniversalEnveloping::ConvertToLieAlgebraElementIfPossible(ElementSimpleLieAlgebra& output)const
{ output.Nullify(*this->owner);
  int numPosRoots=this->owner->theWeyl.RootsOfBorel.size;
  int theDimension=this->owner->theWeyl.CartanSymmetric.NumRows;
  Rational tempRat=0;
  PolynomialRationalCoeff tempP;
  for (int i=0; i<this->size; i++)
  { MonomialUniversalEnveloping& tempMon= this->TheObjects[i];
    tempMon.GetDegree(tempP);
    if (!tempP.IsEqualToOne())
      return false;
    if (tempMon.Coefficient.TotalDegree()>0)
      return false;
    if (tempMon.Coefficient.size>0)
      tempRat=tempMon.Coefficient.TheObjects[0].Coefficient;
    else
      tempRat=0;
    int theGeneratorIndex=tempMon.generatorsIndices.TheObjects[0];
    if(theGeneratorIndex>=numPosRoots && theGeneratorIndex< theDimension+numPosRoots)
    { theGeneratorIndex-=numPosRoots;
      output.Hcomponent.TheObjects[theGeneratorIndex]=tempRat;
    } else
    { if (theGeneratorIndex>numPosRoots)
        theGeneratorIndex-=theDimension;
      output.SetCoefficient(this->owner->theWeyl.RootSystem.TheObjects[theGeneratorIndex], tempRat, *this->owner);
    }
  }
  return true;
}

void ElementUniversalEnveloping::AssignElementLieAlgebra(const ElementSimpleLieAlgebra& input, int numVars, SemisimpleLieAlgebra& theOwner)
{ this->Nullify(&theOwner);
  this->AssignElementCartan(input.Hcomponent, numVars, theOwner);
  MonomialUniversalEnveloping tempMon;
  tempMon.Nullify(numVars, theOwner);
  tempMon.generatorsIndices.SetSizeExpandOnTopNoObjectInit(1);
  tempMon.Powers.SetSizeExpandOnTopNoObjectInit(1);
  tempMon.Powers.TheObjects[0].MakeNVarConst((short)numVars, 1);
  for (int i=0; i<input.NonZeroElements.CardinalitySelection; i++)
  { int theIndex=input.NonZeroElements.elements[i];
    int theGeneratorIndex=theOwner.RootIndexToGeneratorIndex(theIndex);
    tempMon.Coefficient.MakeNVarConst((short)numVars, input.coeffsRootSpaces.TheObjects[theIndex]);
    tempMon.generatorsIndices.TheObjects[0]=theGeneratorIndex;
    this->AddObjectOnTopHash(tempMon);
  }
}

void ElementUniversalEnveloping::ModOutVermaRelations()
{ MonomialUniversalEnveloping tempMon;
  ElementUniversalEnveloping output;
  output.Nullify(this->owner);
  for (int i=0; i<this->size; i++)
  { tempMon= this->TheObjects[i];
    tempMon.ModOutVermaRelations();
    output.AddMonomial(tempMon);
  }
  this->operator=(output);
}

void ElementUniversalEnveloping::GetCoordinateFormOfSpanOfElements
(List<ElementUniversalEnveloping>& theElements, List<rootPoly>& outputCoordinates, ElementUniversalEnveloping& outputCorrespondingMonomials, GlobalVariables& theGlobalVariables)
{ if (theElements.size==0)
    return;
  outputCorrespondingMonomials.Nullify(theElements.TheObjects[0].owner);
  MonomialUniversalEnveloping tempMon;
  int numVars= outputCorrespondingMonomials.owner->theWeyl.CartanSymmetric.NumRows;
  for (int i=0; i<theElements.size; i++)
    for (int j=0; j<theElements.TheObjects[i].size; j++)
      outputCorrespondingMonomials.AddObjectOnTopNoRepetitionOfObjectHash(theElements.TheObjects[i].TheObjects[j]);
  outputCoordinates.SetSizeExpandOnTopNoObjectInit(theElements.size);
  PolynomialRationalCoeff ZeroPoly;
  ZeroPoly.Nullify((short)numVars);
  for (int i=0; i<theElements.size; i++)
  { rootPoly& current=outputCoordinates.TheObjects[i];
    current.initFillInObject(outputCorrespondingMonomials.size, ZeroPoly);
    ElementUniversalEnveloping& currentElt=theElements.TheObjects[i];
    for (int j=0; j<currentElt.size; j++)
    { MonomialUniversalEnveloping& currentMon=currentElt.TheObjects[j];
      current.TheObjects[outputCorrespondingMonomials.IndexOfObjectHash(currentMon)]=currentMon.Coefficient;
    }
  }
}

void ElementUniversalEnveloping::AssignElementCartan(const root& input, int numVars, SemisimpleLieAlgebra& theOwner)
{ MonomialUniversalEnveloping tempMon;
  this->Nullify(&theOwner);
  tempMon.Nullify(numVars, theOwner);
  int theDimension= theOwner.theWeyl.CartanSymmetric.NumRows;
  int numPosRoots=theOwner.theWeyl.RootsOfBorel.size;
  tempMon.generatorsIndices.SetSizeExpandOnTopNoObjectInit(1);
  tempMon.Powers.SetSizeExpandOnTopNoObjectInit(1);
  for (int i=0; i<theDimension; i++)
    if (!input.TheObjects[i].IsEqualToZero())
    { (*tempMon.generatorsIndices.LastObject())=i+numPosRoots;
      tempMon.Powers.LastObject()->MakeNVarConst((short)numVars, 1);
      tempMon.Coefficient.MakeNVarConst((short)numVars, input.TheObjects[i]);
      this->AddObjectOnTopHash(tempMon);
    }
}

void ElementUniversalEnveloping::ElementToString(std::string& output, bool useLatex)
{ std::stringstream out;
  std::string tempS;
  if (this->size==0)
    out << "0";
  int IndexCharAtLastLineBreak=0;
  for (int i=0; i<this->size; i++)
  { MonomialUniversalEnveloping& current=this->TheObjects[i];
    tempS=current.ElementToString(false);
    if (i!=0)
      if (tempS.size()>0)
        if (tempS[0]!='-')
          out << '+';
    out << tempS;
    if (((int)out.tellp())- IndexCharAtLastLineBreak> 150)
    { IndexCharAtLastLineBreak=out.tellp();
      out << "\\\\&&";
    }
  }
  output=out.str();
}

void MonomialUniversalEnveloping::SetNumVariables(int newNumVars)
{ if (this->Coefficient.NumVars==newNumVars)
    return;
  this->Coefficient.SetNumVariablesSubDeletedVarsByOne((short)newNumVars);
  for(int i=0; i<this->generatorsIndices.size; i++)
    this->Powers.TheObjects[i].SetNumVariablesSubDeletedVarsByOne((short)newNumVars);
}

std::string MonomialUniversalEnveloping::ElementToString(bool useLatex)
{ std::stringstream out;
  std::string tempS;
  if (this->owner==0)
    return "faulty monomial non-initialized owner. Slap the programmer.";
  if (this->Coefficient.IsEqualToZero())
    tempS="0";
  else
    tempS = this->Coefficient.ElementToString();
  if (this->generatorsIndices.size>0)
  { if (tempS=="1")
      tempS="";
    if (tempS=="-1")
      tempS="-";
  }
  if (this->Coefficient.size>1)
    out << "(" << tempS << ")";
  else
    out << tempS;
  for (int i=0; i<this->generatorsIndices.size; i++)
  { PolynomialRationalCoeff& thePower=this->Powers.TheObjects[i];
    int theIndex=this->generatorsIndices.TheObjects[i];
    tempS=this->owner->getLetterFromGeneratorIndex(theIndex, useLatex);
    //if (thePower>1)
    //  out << "(";
    out << tempS;
    if (!thePower.IsEqualToOne())
    { out << "^";
     // if (useLatex)
      out << "{";
      out << thePower.ElementToString();
      //if (useLatex)
      out << "}";
    }
    //if (thePower>1)
    //  out << ")";
  }
  return out.str();
}

void MonomialUniversalEnveloping::ModOutVermaRelations()
{ int numPosRoots=this->owner->theWeyl.RootsOfBorel.size;
  int theDimension=this->owner->theWeyl.CartanSymmetric.NumRows;
  for (int i=this->generatorsIndices.size-1; i>=0; i--)
  { if (this->generatorsIndices.TheObjects[i]<numPosRoots)
    { this->Nullify(theDimension, *this->owner);
      return;
    }
    if (this->generatorsIndices.TheObjects[i]>=numPosRoots+theDimension)
      return;
    if (this->generatorsIndices.TheObjects[i]>=numPosRoots)
    { if (this->Powers.TheObjects[i].TotalDegree()!=0)
        return;
      int theDegree = this->Powers.TheObjects[i].TheObjects[0].Coefficient.NumShort;
      PolynomialRationalCoeff tempP;
      root tempRoot;
      tempRoot.MakeEi(theDimension, this->generatorsIndices.TheObjects[i]-numPosRoots);
      tempP.MakeLinPolyFromRoot(tempRoot);
      if (theDegree!=1)
        tempP.RaiseToPower(theDegree);
      this->Coefficient.MultiplyBy(tempP);
      this->generatorsIndices.size--;
      this->Powers.size--;
    }
  }
}

void MonomialUniversalEnveloping::Nullify(int numVars, SemisimpleLieAlgebra& theOwner)
{ this->Coefficient.Nullify((short)numVars);
  this->owner=&theOwner;
  this->generatorsIndices.size=0;
  this->Powers.size=0;
}

int MonomialUniversalEnveloping::HashFunction() const
{ int top=MathRoutines::Minimum(SomeRandomPrimesSize, this->generatorsIndices.size);
  int result=0;
  for (int i=0; i<top; i++)
    result+=SomeRandomPrimes[i]*this->generatorsIndices.TheObjects[i];
  return result;
}

void ParserNode::Clear()
{ this->indexParent=-1;
  this->intValue=0;
  this->ErrorType=this->errorNoError;
  this->Operation= Parser::tokenEmpty;
  this->Evaluated=false;
  this->ExpressionType=this->typeUndefined;
  this->children.size=0;
  this->rationalValue.MakeZero();
  if (this->owner!=0)
    this->ContextLieAlgebra=&this->owner->theHmm.theRange;
  this->polyValue.Nullify(0);
}

std::string SemisimpleLieAlgebra::getLetterFromGeneratorIndex(int theIndex, bool useLatex)
{ int numPosRoots= this->theWeyl.RootsOfBorel.size;
  int rank= this->theWeyl.CartanSymmetric.NumRows;
  std::stringstream out;
  if (theIndex<numPosRoots || theIndex>= numPosRoots+rank)
  { theIndex=this->IndexToRootIndex(theIndex);
    out << "g";
    if (useLatex)
      out << "^{\\alpha_{" << theIndex+1 << "}}";
    else
      if (theIndex<numPosRoots)
        out << "_{" << theIndex+1 << "}";
      else
        out << "_{" << -theIndex+numPosRoots-1 << "}";
  } else
  { out << "h";
    if (useLatex)
      out << "_{\\alpha_{" << theIndex-numPosRoots+1 << "}}";
    else
      out << "_{" << theIndex-numPosRoots+1 << "}";
  }
  return out.str();
}

bool SemisimpleLieAlgebra::AreOrderedProperly(int leftIndex, int rightIndex)
{ if (rightIndex>=this->theWeyl.RootsOfBorel.size)
    rightIndex=-rightIndex-1+ this->theWeyl.RootsOfBorel.size;
  if (leftIndex>=this->theWeyl.RootsOfBorel.size)
    leftIndex=-leftIndex-1+ this->theWeyl.RootsOfBorel.size;
  return leftIndex<=rightIndex;
}

int SemisimpleLieAlgebra::RootIndexToGeneratorIndex(int theIndex)
{ if (theIndex<0  || theIndex>=this->theWeyl.RootSystem.size)
    return -1;
  if (theIndex>=this->theWeyl.RootsOfBorel.size)
    theIndex+=this->theWeyl.CartanSymmetric.NumRows;
  return theIndex;
}

int SemisimpleLieAlgebra::IndexToRootIndex(int theRootIndex)
{ if (theRootIndex<this->theWeyl.RootsOfBorel.size)
    return theRootIndex;
  return theRootIndex-this->theWeyl.CartanSymmetric.NumRows;
}

std::string SemisimpleLieAlgebra::ElementToStringLieBracketPairing()
{ std::stringstream out;
  std::string tempS;
  out << "<br>\nThe generator indices:<br>\n ";
  for (int i=0; i<this->theLiebracketPairingIndices.NumRows; i++)
  { for (int j=0; j<this->theLiebracketPairingIndices.NumCols; j++)
      out << this->theLiebracketPairingIndices.elements[i][j] << ", ";
    out << "<br>\n";
  }
  this->theLiebracketPairingCoefficients.ElementToString(tempS);
  out << "<br>\nThe coefficients:\n" << tempS;
  return out.str();
}

void ElementUniversalEnveloping::MakeConst(const Rational& coeff, int numVars, SemisimpleLieAlgebra& theOwner)
{ MonomialUniversalEnveloping tempMon;
  this->Nullify(&theOwner);
  PolynomialRationalCoeff tempP;
  tempP.MakeNVarConst((short)numVars, coeff);
  tempMon.MakeConst(tempP, theOwner);
  this->AddMonomialNoCleanUpZeroCoeff(tempMon);
  this->CleanUpZeroCoeff();
}

void ElementUniversalEnveloping::Nullify(SemisimpleLieAlgebra* theOwner)
{ this->ClearTheObjects();
  this->owner=theOwner;
}

void ElementUniversalEnveloping::operator+=(const ElementUniversalEnveloping& other)
{ this->MakeActualSizeAtLeastExpandOnTop(other.size);
  for (int i=0; i<other.size; i++)
    this->AddMonomialNoCleanUpZeroCoeff(other.TheObjects[i]);
  this->CleanUpZeroCoeff();
}

void ElementUniversalEnveloping::operator-=(const ElementUniversalEnveloping& other)
{ this->MakeActualSizeAtLeastExpandOnTop(other.size);
  MonomialUniversalEnveloping tempMon;
  for (int i=0; i<other.size; i++)
  { tempMon=other.TheObjects[i];
    tempMon.Coefficient.TimesConstant(-1);
    this->AddMonomialNoCleanUpZeroCoeff(tempMon);
  }
  this->CleanUpZeroCoeff();
}

void ElementUniversalEnveloping::operator/=(const Rational& other)
{ for (int i=0; i<this->size; i++)
    this->TheObjects[i].Coefficient.DivByRational(other);
}

void ElementUniversalEnveloping::operator*=(const Rational& other)
{ if (other.IsEqualToZero())
  { this->Nullify(this->owner);
    return;
  }
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].Coefficient.TimesConstant(other);
}

void ElementUniversalEnveloping::operator*=(const ElementUniversalEnveloping& other)
{ this->MakeActualSizeAtLeastExpandOnTop(other.size*this->size);
  ElementUniversalEnveloping output;
  output.Nullify(this->owner);
  MonomialUniversalEnveloping tempMon;
  int sizeOriginal=0;
  PolynomialRationalCoeff powerOriginal, CoeffOriginal;
  for (int i=0; i<this->size; i++)
  { tempMon=this->TheObjects[i];
    sizeOriginal=tempMon.generatorsIndices.size;
    if (sizeOriginal>0)
      powerOriginal=*tempMon.Powers.LastObject();
    CoeffOriginal.Assign(tempMon.Coefficient);
    for(int j=0; j<other.size; j++)
    { tempMon.generatorsIndices.size=sizeOriginal;
      tempMon.Powers.size=sizeOriginal;
      if (sizeOriginal>0)
        *tempMon.Powers.LastObject()=powerOriginal;
      tempMon.Coefficient.Assign(CoeffOriginal);
      tempMon.MultiplyByNoSimplify(other.TheObjects[j]);
      //tempMon.ComputeDebugString();
      output.AddMonomialNoCleanUpZeroCoeff(tempMon);
    }
  }
  *this=output;
}

void ElementUniversalEnveloping::SetNumVariables(int newNumVars)
{ this->ComputeDebugString();
  if (this->size==0)
    return;
  int currentNumVars=this->TheObjects[0].Coefficient.NumVars;
  if (currentNumVars==newNumVars)
    return;
  ElementUniversalEnveloping Accum;
  Accum.Nullify(this->owner);
  MonomialUniversalEnveloping tempMon;
  for (int i=0; i<this->size; i++)
  { tempMon=this->TheObjects[i];
    tempMon.SetNumVariables(newNumVars);
    Accum.AddMonomial(tempMon);
  }
  Accum.ComputeDebugString();
  this->operator=(Accum);
  this->ComputeDebugString();
}

void ElementUniversalEnveloping::RaiseToPower(int thePower)
{ ElementUniversalEnveloping buffer;
  buffer=*this;
  if (this->size==0)
    return;
  this->MakeConst(1, this->TheObjects[0].Coefficient.NumVars, *this->owner);
  for (int i=0; i<thePower; i++)
    this->operator*=(buffer);
}

void ElementUniversalEnveloping::MakeCasimir(SemisimpleLieAlgebra& theOwner, int numVars, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  this->Nullify(&theOwner);
  WeylGroup& theWeyl= this->owner->theWeyl;
  int theDimension=theWeyl.CartanSymmetric.NumRows;
  root tempRoot1, tempRoot2;
  MatrixLargeRational killingForm;
  killingForm.init(theDimension, theDimension);
  for (int i=0; i<theDimension; i++)
  { tempRoot1.MakeEi(theDimension, i);
    for (int j=0; j<theDimension; j++)
    { killingForm.elements[i][j]=0;
      tempRoot2.MakeEi(theDimension, j);
      for (int k=0; k<theWeyl.RootSystem.size; k++)
        killingForm.elements[i][j]+= theWeyl.RootScalarCartanRoot(tempRoot1, theWeyl.RootSystem.TheObjects[k])* theWeyl.RootScalarCartanRoot(tempRoot2, theWeyl.RootSystem.TheObjects[k]);
    }
  }
  killingForm.Invert(theGlobalVariables);
  killingForm.ComputeDebugString();
  out << killingForm.DebugString;
  ElementUniversalEnveloping tempElt1, tempElt2;
  for (int i=0; i<theDimension; i++)
  { tempRoot1.MakeEi(theDimension, i);
    killingForm.ActOnAroot(tempRoot1, tempRoot2);
    tempElt1.AssignElementCartan(tempRoot1, numVars, theOwner);
    tempElt2.AssignElementCartan(tempRoot2, numVars, theOwner);
    tempElt1*=tempElt2;
    *this+=tempElt1;
  }
  Rational tempRat;
  root theSum;
  for (int i=0; i<theWeyl.RootSystem.size; i++)
  { tempRat=0;
    root& theRoot=theWeyl.RootSystem.TheObjects[i];
    int indexOfOpposite=theWeyl.RootSystem.IndexOfObjectHash(-theRoot);
    root& theOpposite= theWeyl.RootSystem.TheObjects[indexOfOpposite];
    for (int j=0; j<theWeyl.RootSystem.size; j++)
    { root& current=theWeyl.RootSystem.TheObjects[j];
      if (current==theOpposite)
        tempRat+=2;
      else
      { int indexOfSum=theWeyl.RootSystem.IndexOfObjectHash(current+theRoot);
        if (indexOfSum!=-1)
          tempRat+=(theOwner.ChevalleyConstants.elements[i][j]*theOwner.ChevalleyConstants.elements[indexOfOpposite][indexOfSum]);
      }
    }
    tempRat+=2;
    tempRat= 1/tempRat;
    tempElt2.MakeOneGeneratorCoeffOne(theOpposite, numVars, theOwner);
    tempElt1.MakeOneGeneratorCoeffOne(theRoot, numVars, theOwner);
    tempElt2*=tempElt1;
    tempElt2*=tempRat;
    *this+=tempElt2;
  }
  this->ComputeDebugString();
  out << "\n" << this->DebugString;
  this->DebugString=out.str();
/*  for (int i=0; i<theDimension; i++)
  { tempRoot.MakeEi(theDimension, i);
    if (!length1Explored)
    { length1= theWeyl.RootScalarCartanRoot(tempRoot, tempRoot);
      length1Explored=true;
      coefficient1=0;
      for (int j=0; j<theWeyl.RootsOfBorel.size; j++)
      { coefficient1+=theWeyl.RootScalarCartanRoot(tempRoot, theWeyl.RootsOfBorel.TheObjects[j])*theWeyl.RootScalarCartanRoot(tempRoot, theWeyl.RootsOfBorel.TheObjects[j]);
        coef
      }
    }
  }*/
}

void ComputationSetup::TestParser(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ std::string inputString;
  std::stringstream out;
  SemisimpleLieAlgebra theLieAlgebra;
  theLieAlgebra.ComputeChevalleyConstants('A', 1, theGlobalVariables);
  theLieAlgebra.ComputeDebugString(false, false, theGlobalVariables);
  ElementUniversalEnveloping theElt;
  theElt.MakeCasimir(theLieAlgebra, 1, theGlobalVariables);
  out << theElt.DebugString << "\n\n";
  out << theLieAlgebra.DebugString;
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1 = out.str();
  theGlobalVariables.MakeReport();
}

void CGIspecificRoutines::MakeSureWeylGroupIsSane(char& theWeylLetter, int& theRank)
{ if (theWeylLetter=='a') theWeylLetter='A';
  if (theWeylLetter=='b') theWeylLetter='B';
  if (theWeylLetter=='c') theWeylLetter='C';
  if (theWeylLetter=='d') theWeylLetter='D';
  if (theWeylLetter=='e') theWeylLetter='E';
  if (theWeylLetter=='f') theWeylLetter='F';
  if (theWeylLetter=='g') theWeylLetter='G';
  if (!(theWeylLetter=='A' || theWeylLetter=='B' || theWeylLetter=='C' || theWeylLetter=='D' || theWeylLetter=='E' || theWeylLetter=='F' || theWeylLetter=='G'))
      theWeylLetter='A';
  if (theRank>8 || theRank<1)
    theRank=1;
}

void SemisimpleLieAlgebra::ElementToStringLieBracket(std::string& output, bool useHtml, bool useLatex, bool usePNG, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  std::string tempS;
  root tempRoot;
  std::string beginMath;
  std::string endMath;
  if (!usePNG)
  { beginMath="<DIV class=\"math\" scale=\"50\">\\begin{array}{";
    endMath="\\end{array}</DIV>";
  } else
  { beginMath="\\begin{tabular}{";
    endMath="\\end{tabular}";
  }
  int numRoots=this->theWeyl.RootSystem.size;
  int theDimension = this->theWeyl.CartanSymmetric.NumRows;
//  out << beginMath << "\\begin{array}{ccc}a& a&a\\\\a&a&a\\end{array}";
  if (usePNG)
  { out << "\\begin{tabular}{cc}\\hline generator & corresponding root space\\\\\\hline";
    for (int i=0; i<this->theWeyl.RootSystem.size; i++)
    { int tempI=i+1;
      if (tempI>this->theWeyl.RootsOfBorel.size)
        tempI=-tempI+theWeyl.RootsOfBorel.size;
      out << "$g_{" << tempI << "}$&" << this->theWeyl.RootSystem.TheObjects[i].ElementToString() << "\\\\";
    }
    out << "\\end{tabular}";
  }
  out << beginMath;
  ElementUniversalEnveloping tempElt1, tempElt2, tempElt3;
  for (int i=0; i<numRoots+theDimension+1; i++)
    out << "c";
  out << "}";
  std::string tempHeader=out.str();
  if(usePNG)
    out << "$";
  out << "[\\bullet, \\bullet]\n";
  if(usePNG)
    out << "$";
  for (int i=0; i<this->theLiebracketPairingCoefficients.NumRows; i++)
  { tempElt1.MakeOneGeneratorCoeffOne(i, 0, *this);
    tempElt1.ElementToString(tempS);
    out << " & ";
    if(usePNG)
      out << "$";
    out << tempS;
    if(usePNG)
      out << "$";
  }
  out << "\\\\\n";
  Rational tempRat;
  for (int i=0; i<this->theLiebracketPairingCoefficients.NumRows; i++)
  { tempElt1.MakeOneGeneratorCoeffOne(i, 0, *this);
    tempElt1.ElementToString(tempS);
    if(usePNG)
      out << "$";
    out << tempS;
    if(usePNG)
      out << "$";
    for (int j=0; j<this->theLiebracketPairingCoefficients.NumRows; j++)
    { tempElt2.MakeOneGeneratorCoeffOne(j, 0, *this);
      tempElt1.LieBracketOnTheRight(tempElt2, tempElt3);
      tempElt3.Simplify();
      tempElt3.ElementToString(tempS);
      out << "& ";
      if(usePNG)
        out << "$";
      out << tempS;
      if(usePNG)
        out << "$";
    }
    out << "\\\\\n";
    //the below is a hack to avoid a latex crash due to memory overuse
    //if (usePNG && i%130==129)
    //  out << "\\end{tabular}\n\n\n" << tempHeader;
  }
  out << endMath;
  output=out.str();
}

void CGIspecificRoutines::ReplaceEqualitiesAndAmpersantsBySpaces(std::string& inputOutput)
{ for (int i=0; i<(signed)inputOutput.size(); i++)
    if (inputOutput[i]=='=' || inputOutput[i]=='&')
      inputOutput[i]=' ';
}

void VectorPartition::ComputeAllPartitions()
{ List<int> currentPartition;
  currentPartition.initFillInObject(PartitioningRoots.size, 0);
  this->thePartitions.size=0;
  this->ComputeAllPartitionsRecursive(0, currentPartition, -1, theRoot);
}

void VectorPartition::ComputeAllPartitionsRecursive(int currentIndex, List<int>& CurrentPartition, int UpperBoundEachIndex, root& toBePartitioned)
{ if (currentIndex>=this->PartitioningRoots.size)
    return;
  root currentRoot=toBePartitioned;
  while (currentRoot.IsPositiveOrZero() && (CurrentPartition.TheObjects[currentIndex]<=UpperBoundEachIndex || UpperBoundEachIndex==-1))
  { if (currentRoot.IsEqualToZero())
      this->thePartitions.AddObjectOnTop(CurrentPartition);
    else
    { this->ComputeAllPartitionsRecursive(currentIndex+1, CurrentPartition , UpperBoundEachIndex, currentRoot);
      for (int i=currentIndex+1; i<CurrentPartition.size; i++)
        CurrentPartition.TheObjects[i]=0;
    }
    currentRoot.Subtract(this->PartitioningRoots.TheObjects[currentIndex]);
    CurrentPartition.TheObjects[currentIndex]++;
  }
}

std::string VectorPartition::ElementToString(bool useHtml)
{ std::stringstream out;
  out << this->theRoot.ElementToString() << "\n\n";
  if (useHtml)
    out << "<br>";
  for (int i=0; i<this->thePartitions.size; i++)
  { bool found=false;
    out << "=";
    for (int j=0; j<this->thePartitions.TheObjects[i].size; j++)
    { int theCoefficient=thePartitions.TheObjects[i].TheObjects[j];
      if (theCoefficient!=0)
      { if(found)
          out << "+";
        found=true;
        if (theCoefficient>1)
          out << theCoefficient;
        out << this->PartitioningRoots.TheObjects[j].ElementToString();
      }
    }
    out << "\n\n";
    if (useHtml)
      out << "<br>\n";
  }
  return out.str();
}


std::string rootPoly::ElementToString()
{ std::stringstream out;
  out << "(";
  for (int i=0; i<this->size; i++)
  { out << this->TheObjects[i].ElementToString();
    if (i<this->size-1)
      out << ",";
  }
  out << ")";
  return out.str();
}

std::string RationalFunction::ElementToString(bool useLatex, bool breakLinesLatex)
{ std::stringstream out;
  bool hasDenominator=!this->Denominator.IsEqualToOne();
  if (hasDenominator && useLatex)
    out << "\\frac{";
  out << this->Numerator.ElementToString(breakLinesLatex);
  if (hasDenominator)
  { if (useLatex)
      out << "}{";
    out << this->Denominator.ElementToString(breakLinesLatex);
    if (useLatex)
      out << "}";
  }
  return out.str();
}

void RationalFunction::RemainderDivisionWithRespectToBasis(PolynomialRationalCoeff& input, List<PolynomialRationalCoeff>& theBasis, PolynomialRationalCoeff& outputRemainder, PolynomialRationalCoeff& buffer1, PolynomialRationalCoeff& buffer2, Monomial<Rational>& bufferMon1)
{ assert(&outputRemainder!=&input);
  PolynomialRationalCoeff* currentRemainder=&input;
  PolynomialRationalCoeff* nextRemainder=&buffer1;
  for (int i=0; i<theBasis.size; i++)
  { RationalFunction::RemainderDivision(*currentRemainder, theBasis.TheObjects[i], *nextRemainder, buffer2, bufferMon1);
    MathRoutines::swap(currentRemainder, nextRemainder);
    if (currentRemainder->IsEqualToZero())
      break;
  }
  if (currentRemainder!=&outputRemainder)
    outputRemainder.Assign(*currentRemainder);
}

void RationalFunction::RemainderDivision(PolynomialRationalCoeff& input, PolynomialRationalCoeff& divisor, PolynomialRationalCoeff& outputRemainder, PolynomialRationalCoeff& buffer, Monomial<Rational>& bufferMon1)
{ assert(&input!=&outputRemainder);
  outputRemainder.Assign(input);
  int divisorHighest=divisor.GetIndexMaxMonomial();
  Monomial<Rational>& highestMonDivisor=divisor.TheObjects[divisorHighest];
  int remainderHighest=-1;
  int theNumVars=input.NumVars;
  bufferMon1.init((short)theNumVars);
  assert(input.NumVars==theNumVars);
  assert(divisor.NumVars==theNumVars);
  for(;;)
  { if (outputRemainder.size==0)
      return;
    remainderHighest=  outputRemainder.GetIndexMaxMonomial();
    Monomial<Rational>& highestMonRemainder=outputRemainder.TheObjects[remainderHighest];
    /*outputRemainder.ComputeDebugString();
    highestMonRemainder.ComputeDebugString();
    highestMonDivisor.ComputeDebugString();*/
    for (int i=0; i<theNumVars; i++)
    { if (highestMonRemainder.degrees[i]<highestMonDivisor.degrees[i])
        return;
      bufferMon1.degrees[i]=highestMonRemainder.degrees[i]-highestMonDivisor.degrees[i];
    }
    bufferMon1.Coefficient=highestMonRemainder.Coefficient/highestMonDivisor.Coefficient;
    divisor.MultiplyByMonomial(bufferMon1, buffer);
/*    outputRemainder.ComputeDebugString();
    tempP.ComputeDebugString();
    highestMonRemainder.ComputeDebugString();
    tempMon.ComputeDebugString();
    highestMonDivisor.ComputeDebugString();*/
    outputRemainder.Subtract(buffer);/*
    outputRemainder.ComputeDebugString();*/
  }
}

void RationalFunction::TransformToGroebnerBasis(List<PolynomialRationalCoeff>& theBasis, PolynomialRationalCoeff& buffer1, PolynomialRationalCoeff& buffer2, PolynomialRationalCoeff& buffer3, PolynomialRationalCoeff& buffer4, Monomial<Rational>& bufferMon1, Monomial<Rational>& bufferMon2)
{ PolynomialRationalCoeff& tempP=buffer1;
  PolynomialRationalCoeff& Spoly=buffer2;
  Monomial<Rational>& leftShift=bufferMon1;
  Monomial<Rational>& rightShift=bufferMon2;
  int theNumVars=theBasis.TheObjects[0].NumVars;
  leftShift.init((short)theNumVars);
  rightShift.init((short)theNumVars);
 // std::string tempS;
  for (int lowestNonExplored=0; lowestNonExplored< theBasis.size; lowestNonExplored++)
  { //warning! currentPoly may expire if theBasis.TheObjects changes size
//    PolynomialRationalCoeff& currentPoly=;
    for (int j=0; j<theBasis.size; j++)
    { PolynomialRationalCoeff& currentLeft= theBasis.TheObjects[lowestNonExplored];
      PolynomialRationalCoeff& currentRight= theBasis.TheObjects[j];
      Monomial<Rational>& leftHighestMon=currentLeft.TheObjects[currentLeft.GetIndexMaxMonomial()];
      Monomial<Rational>& rightHighestMon=currentRight.TheObjects[currentRight.GetIndexMaxMonomial()];
      //leftHighestMon.ComputeDebugString();
      //rightHighestMon.ComputeDebugString();
      //currentRight.ComputeDebugString();
      //currentLeft.ComputeDebugString();
      leftShift.Coefficient=-rightHighestMon.Coefficient;
      rightShift.Coefficient=leftHighestMon.Coefficient;
      for (int k=0; k<leftHighestMon.NumVariables; k++)
        if (leftHighestMon.degrees[k]>rightHighestMon.degrees[k])
        { rightShift.degrees[k]=leftHighestMon.degrees[k]-rightHighestMon.degrees[k];
          leftShift.degrees[k]=0;
        }
        else
        { leftShift.degrees[k]=rightHighestMon.degrees[k]-leftHighestMon.degrees[k];
          rightShift.degrees[k]=0;
        }
      currentLeft.MultiplyByMonomial(leftShift, tempP);
      currentRight.MultiplyByMonomial(rightShift, Spoly);
      Spoly.AddPolynomial(tempP);
      //Spoly.ComputeDebugString();
//      theBasis.ElementToStringGeneric(tempS);
      RationalFunction::RemainderDivisionWithRespectToBasis(Spoly, theBasis, tempP, buffer3, buffer4, bufferMon1);

      //tempP.ComputeDebugString();
      if (!tempP.IsEqualToZero())
      { tempP.ScaleToIntegralNoGCDCoeffs();
        theBasis.AddObjectOnTop(tempP);
        std::cout << "<br> new element found: " << tempP.ElementToString();
      }
    }
  }
  std::cout << "<br> ... and the basis before reduction is: <br>";
  for (int i=0; i<theBasis.size; i++)
    std::cout << theBasis.TheObjects[i].ElementToString() << ", ";
  RationalFunction::ReduceGroebnerBasis(theBasis, buffer1);

}

void RationalFunction::ReduceGroebnerBasis(List<PolynomialRationalCoeff>& theBasis, PolynomialRationalCoeff& buffer1)
{ PolynomialRationalCoeff& LeadingCoeffs=buffer1;
  LeadingCoeffs.MakeActualSizeAtLeastExpandOnTop(theBasis.size);
  LeadingCoeffs.ClearTheObjects();
  List<Monomial< Rational> > tempList;
  std::cout << "<br> ... and the leading coefficients are: <br>";
  for (int i=0; i<theBasis.size; i++)
  { PolynomialRationalCoeff& current=theBasis.TheObjects[i];
    LeadingCoeffs.AddObjectOnTopHash(current.TheObjects[current.GetIndexMaxMonomial()]);
    LeadingCoeffs.LastObject()->Coefficient=1;
    LeadingCoeffs.LastObject()->ComputeDebugString();
    std::cout << LeadingCoeffs.LastObject()->DebugString << ", ";
  }
  tempList.CopyFromBase(LeadingCoeffs);
  tempList.QuickSortAscending();
  std::cout << "<br><br> and the sorted leading monomials are: ";
  for (int i=0; i<theBasis.size; i++)
  { tempList.TheObjects[i].ComputeDebugString();
    std::cout << tempList.TheObjects[i].DebugString << ", ";
  }
  for (int i=0; i<LeadingCoeffs.size; i++)
  { Monomial<Rational>& currentMon=LeadingCoeffs.TheObjects[i];
    for (int j=0; j<LeadingCoeffs.size; j++)
      if (i!=j)
        if (currentMon.IsDivisibleBy(LeadingCoeffs.TheObjects[j]))
        { LeadingCoeffs.PopIndexSwapWithLastHash(i);
          theBasis.PopIndexSwapWithLast(i);
          i--;
          break;
        }
  }
}

void RationalFunction::gcd(PolynomialRationalCoeff& left, PolynomialRationalCoeff& right, PolynomialRationalCoeff& output, PolynomialRationalCoeff& buffer1, PolynomialRationalCoeff& buffer2, PolynomialRationalCoeff& buffer3, PolynomialRationalCoeff& buffer4, PolynomialRationalCoeff& buffer5, Monomial<Rational>& bufferMon1, Monomial<Rational>& bufferMon2, List<PolynomialRationalCoeff>& bufferList)
{ RationalFunction::lcm(left, right, buffer4, buffer1, buffer2, buffer3, buffer5, bufferMon1, bufferMon2, bufferList);
  left.MultiplyBy(right, buffer2, buffer1, bufferMon1);
  std::cout << "<br>the product: " << buffer2.DebugString << " and the gcd: " << buffer4.DebugString << "<br>";
  buffer2.DivideBy(buffer4, output, buffer3);
}

void RationalFunction::lcm(PolynomialRationalCoeff& left, PolynomialRationalCoeff& right, PolynomialRationalCoeff& output, PolynomialRationalCoeff& buffer1, PolynomialRationalCoeff& buffer2, PolynomialRationalCoeff& buffer3, PolynomialRationalCoeff& buffer4, Monomial<Rational>& bufferMon1, Monomial<Rational>& bufferMon2, List<PolynomialRationalCoeff>& bufferList)
{ PolynomialRationalCoeff& leftTemp=buffer1;
  PolynomialRationalCoeff& rightTemp=buffer2;
  PolynomialRationalCoeff& tempP=buffer3;
  List<PolynomialRationalCoeff>& tempList=bufferList;
  leftTemp.Assign(left);
  rightTemp.Assign(right);
  assert(left.NumVars==right.NumVars);
  int theNumVars=left.NumVars;
  leftTemp.SetNumVariablesSubDeletedVarsByOne((short)theNumVars+1);
  rightTemp.SetNumVariablesSubDeletedVarsByOne((short)theNumVars+1);
  leftTemp.ScaleToIntegralNoGCDCoeffs();
  rightTemp.ScaleToIntegralNoGCDCoeffs();
  tempP.MakeMonomialOneLetter((short)theNumVars+1,(short) theNumVars, 1, (Rational) 1);
  leftTemp.MultiplyBy(tempP);
  tempP.TimesConstant((Rational)-1);
  tempP.AddConstant((Rational) 1);
  rightTemp.MultiplyBy(tempP);
  tempList.size=0;
  tempList.AddObjectOnTop(leftTemp);
  tempList.AddObjectOnTop(rightTemp);
  std::cout << "<br>In the beginning: <br>";
  for (int i=0; i<tempList.size; i++)
  { std::cout << "the groebner basis element with index " << i << " is " << tempList.TheObjects[i].ElementToString() << "<br>\n";
  }
  RationalFunction::TransformToGroebnerBasis(tempList, buffer1, buffer2, buffer3, buffer4, bufferMon1, bufferMon2);
  std::cout << "<br><br> ... and the basis is: <br>";
  for (int i=0; i<tempList.size; i++)
  { std::cout << tempList.TheObjects[i].ElementToString() << "<br>\n";
  }
  for(int i=0; i<tempList.size; i++)
  { PolynomialRationalCoeff& current=tempList.TheObjects[i];
    Monomial<Rational>& currentMon= current.TheObjects[current.GetIndexMaxMonomial()];
    currentMon.ComputeDebugString();
    if (currentMon.degrees[theNumVars]==0)
    { output=current;
      std::cout << "<br> the highest mon is: " << currentMon.DebugString << "<br>";
      output.SetNumVariablesSubDeletedVarsByOne((short)theNumVars);
      return;
    }
  }
  output.Nullify((short)theNumVars);
}

std::string rootRationalFunction::ElementToString()
{ std::stringstream out;
  out << "(";
  for (int i=0; i<this->size; i++)
  { out << this->TheObjects[i].ElementToString(true, false);
    if (i!=this->size-1)
      out << ",";
  }
  out << ")";
  return out.str();
}

void RationalFunction::Simplify()
{ if (this->Numerator.IsEqualToZero())
  { this->Denominator.MakeNVarConst((short)this->NumVariables, (Rational) 1);
    return;
  }
  if (this->Denominator.IsEqualToOne())
    return;
  PolynomialRationalCoeff theGCD, tempP, tempP2;
  this->gcd(this->Numerator, this->Denominator, theGCD);
  this->Numerator.DivideBy(theGCD, tempP, tempP2);
  this->Numerator.Assign(tempP);
  this->Denominator.DivideBy(theGCD, tempP, tempP2);
  this->Denominator.Assign(tempP);
  Rational tempRat, tempRat2;
  tempRat=this->Denominator.FindGCDCoefficientDenominators();
  tempRat2=this->Denominator.FindGCDCoefficientNumerators();
  tempRat.DivideBy(tempRat2);
  if (!tempRat.IsEqualToOne())
  { this->Denominator.TimesConstant(tempRat);
    this->Numerator.TimesConstant(tempRat);
  }
}

Rational PolynomialRationalCoeff::FindGCDCoefficientNumerators()
{ if (this->size==0)
    return (Rational) 1;
  Rational result; LargeIntUnsigned Accum, tempUI;
  this->TheObjects[0].Coefficient.GetNumUnsigned(Accum);
  for (int i=1; i<this->size; i++)
  { this->TheObjects[i].Coefficient.GetNumUnsigned(tempUI);
    LargeIntUnsigned::gcd(Accum, tempUI, Accum);
  }
  LargeInt tempInt;
  tempInt.AssignLargeIntUnsigned(Accum);
  result.AssignLargeInteger(tempInt);
  if (this->size>0)
  { if (this->TheObjects[0].Coefficient.IsNegative())
      result.Minus();
  }
  return result;
}

Rational PolynomialRationalCoeff::FindGCDCoefficientDenominators()
{ if (this->size==0)
    return (Rational) 1;
  Rational result; LargeIntUnsigned Accum, tempUI;
  this->TheObjects[0].Coefficient.GetDen(Accum);
  for (int i=1; i<this->size; i++)
  { this->TheObjects[i].Coefficient.GetDen(tempUI);
    LargeIntUnsigned::gcd(Accum, tempUI, Accum);
  }
  LargeInt tempInt;
  tempInt.AssignLargeIntUnsigned(Accum);
  result.AssignLargeInteger(tempInt);
  return result;
}

void PolynomialRationalCoeff::ScaleToIntegralNoGCDCoeffs()
{ if(this->size==0)
    return;
  int indexHighestMon=0;
  LargeIntUnsigned tempInt1, tempInt2, accumNum, accumDen;
  accumDen.MakeOne();
  this->TheObjects[0].Coefficient.GetNumUnsigned(accumNum);
  for (int i=0; i<this->size; i++)
  { if (this->TheObjects[i].IsGEQ(this->TheObjects[indexHighestMon]))
      indexHighestMon=i;
    Rational& tempRat=this->TheObjects[i].Coefficient;
    tempRat.GetDen(tempInt1);
    tempRat.GetNumUnsigned(tempInt2);
    LargeIntUnsigned::lcm(tempInt1, accumDen, accumDen);
    LargeIntUnsigned::gcd(tempInt2, accumNum, accumNum);
  }
  Rational theMultiple;
  theMultiple.MakeOne();
  if (this->TheObjects[indexHighestMon].Coefficient.IsNegative())
    theMultiple.MakeMOne();
  theMultiple.MultiplyByLargeIntUnsigned(accumDen);
  Rational tempRat2;
  LargeInt tempInt3;
  tempInt3.AssignLargeIntUnsigned(accumNum);
  tempRat2.AssignLargeInteger(tempInt3);
  theMultiple.DivideBy(tempRat2);
  this->TimesConstant(theMultiple);
}


std::string EigenVectorComputation::ComputeAndReturnString(GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  Parser theParser;
  theParser.theHmm.MakeG2InB3(theParser, theGlobalVariables);
  ElementUniversalEnveloping theElt, tempElt1, tempElt2;
  this->MakeGenericVermaElement(theElt, theParser.theHmm.theRange);
  int theDomainRank=theParser.theHmm.theDomain.theWeyl.CartanSymmetric.NumRows;
  int theRangeRank=theParser.theHmm.theRange.theWeyl.CartanSymmetric.NumRows;
  int numDomainPosRoots= theParser.theHmm.theDomain.theWeyl.RootsOfBorel.size;
  int numRangePosRoots= theParser.theHmm.theRange.theWeyl.RootsOfBorel.size;
  for (int i=0; i<theDomainRank; i++)
  { tempElt1.AssignElementLieAlgebra(theParser.theHmm.ImagesSimpleChevalleyGenerators.TheObjects[i], theRangeRank*2, theParser.theHmm.theRange);
    tempElt1.LieBracketOnTheRight(theElt, tempElt2);
    this->DetermineEquationsFromResultLieBracket(theParser, tempElt2, out, theGlobalVariables);
  }
  return out.str();
}

void RootIndexToPoly(int theIndex, SemisimpleLieAlgebra& theAlgebra, PolynomialRationalCoeff& output)
{ int theRank=theAlgebra.theWeyl.CartanSymmetric.NumRows;
  int numPosRoots=theAlgebra.theWeyl.RootsOfBorel.size;
  output.MakeNVarDegOnePoly((short)(theRank+numPosRoots), theIndex+theRank, (Rational) 1);
}

void EigenVectorComputation::DetermineEquationsFromResultLieBracket(Parser& theParser, ElementUniversalEnveloping& theElt, std::stringstream& out, GlobalVariables& theGlobalVariables)
{ int theDomainRank=theParser.theHmm.theDomain.theWeyl.CartanSymmetric.NumRows;
  int theRangeRank=theParser.theHmm.theRange.theWeyl.CartanSymmetric.NumRows;
  int numDomainPosRoots= theParser.theHmm.theDomain.theWeyl.RootsOfBorel.size;
  int numRangePosRoots= theParser.theHmm.theRange.theWeyl.RootsOfBorel.size;
  for (int i=0; i<theElt.size; i++)
  { for (int j=0; j<numRangePosRoots; j++)
    {
    }
  }
}

void EigenVectorComputation::MakeGenericVermaElement(ElementUniversalEnveloping& theElt, SemisimpleLieAlgebra& owner)
{ theElt.owner=&owner;
  int numPosRoots= owner.theWeyl.RootsOfBorel.size;
  int theRank=owner.theWeyl.CartanSymmetric.NumRows;
  theElt.MakeActualSizeAtLeastExpandOnTop(numPosRoots);
  theElt.MakeConst(Rational (1), theRank+numPosRoots, owner);
  MonomialUniversalEnveloping tempMon;
  PolynomialRationalCoeff tempP;
  tempP.MakeNVarConst((short) theRank+numPosRoots, (Rational) 1);
  tempMon.MakeConst(tempP, owner);
  for (int i=0; i<numPosRoots; i++)
  { tempP.MakeNVarDegOnePoly(theRank+numPosRoots, numPosRoots+i, (Rational) 1);
    tempMon.MultiplyByGeneratorPowerOnTheRight(owner.RootIndexToGeneratorIndex(2*numPosRoots-1-i), tempP);
  }
}
