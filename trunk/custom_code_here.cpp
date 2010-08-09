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
      currentSA.theDynkinDiagram.ElementToString(tempS);
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
      InvertedRelativeKillingForm.elements[k][j]=this->AmbientWeyl.RootScalarKillingFormMatrixRoot(this->SimpleBasisK.TheObjects[k], this->SimpleBasisK.TheObjects[j]);
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
      theSl2.theH.Hcomponent.MakeZero(this->AmbientWeyl.KillingFormMatrix.NumRows);
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
//  int theDimension= this->theWeyl.KillingFormMatrix.NumRows;
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
  outputSystemToBeSolved.SetSizeExpandOnTopNoObjectInit(oldSize+this->theWeyl.KillingFormMatrix.NumRows);
  for(int i=oldSize; i<outputSystemToBeSolved.size; i++)
    outputSystemToBeSolved.TheObjects[i].Nullify((short)numberVariables);
  //outputSystemToBeSolved.ComputeDebugString();
  for (int i=0; i<rootsInPlay.size; i++)
  { assert(rootsInPlay.size==halfNumberVariables);
    this->GetConstantOrHElement(rootsInPlay.TheObjects[i], -rootsInPlay.TheObjects[i], tempRat, tempRoot);
    for (int j=0; j<this->theWeyl.KillingFormMatrix.NumRows; j++)
    { tempM.init((short)numberVariables);
      tempM.degrees[i]=1;
      tempM.degrees[i+halfNumberVariables]=1;
      tempM.Coefficient= tempRoot.TheObjects[j];
      outputSystemToBeSolved.TheObjects[j+oldSize].AddMonomial(tempM);
    }
  }
  for (int i=0; i<this->theWeyl.KillingFormMatrix.NumRows; i++)
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
{ int theDimension=this->owner->theWeyl.KillingFormMatrix.NumRows;
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
    this->hCharacteristic.TheObjects[i]=this->owner->theWeyl.RootScalarKillingFormMatrixRoot(this->theH.Hcomponent, this->preferredAmbientSimpleBasis.TheObjects[i]);
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
    { this->RootScalarKillingFormMatrixRoot(theBadRoot, this->RootsOfBorel.TheObjects[i], tempRat1);
      this->RootScalarKillingFormMatrixRoot(output, this->RootsOfBorel.TheObjects[i], tempRat2);
      if ((!tempRat1.IsEqualToZero()) && (!tempRat2.IsEqualToZero()))
      { tempMaxMovement = tempRat2/tempRat1;
        tempMaxMovement.AssignAbsoluteValue();
        if ((tempMaxMovement< maxMovement) || maxMovement.IsEqualToZero())
          maxMovement = tempMaxMovement;
      }
    }
    int tempInt=2;
    if (this->RootScalarKillingFormMatrixRoot(theBadRoot, inputH).IsNegative())
      tempInt=-2;
    output+=theBadRoot*maxMovement/tempInt;
  }
}

bool WeylGroup::IsRegular(root& input, int* indexFirstPerpendicularRoot)
{ if (indexFirstPerpendicularRoot!=0)
    *indexFirstPerpendicularRoot=-1;
  for (int i=0; i<this->RootSystem.size; i++)
    if (this->RootScalarKillingFormMatrixRoot(input, this->RootSystem.TheObjects[i]).IsEqualToZero())
    { if (indexFirstPerpendicularRoot!=0)
        *indexFirstPerpendicularRoot=i;
      return false;
    }
  return true;
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
      output.elements[i][j]=theWeyl.RootScalarKillingFormMatrixRoot(tempRoots.TheObjects[i], tempRoots.TheObjects[j]);
}

void slTwo::ComputeModuleDecompositionAmbientLieAlgebra(GlobalVariables& theGlobalVariables)
{ this->ComputeModuleDecomposition(this->owner->theWeyl.RootsOfBorel, this->owner->theWeyl.KillingFormMatrix.NumRows, this->highestWeights, this->multiplicitiesHighestWeights, this->weightSpaceDimensions, theGlobalVariables);
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
    body<< "Index "<< i<<": ";
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
  theFile<< tempS;
  theFile.close();
  if (usePNG)
  { this->listSystemCommandsLatex.SetSizeExpandOnTopNoObjectInit(this->texFileNamesForPNG.size);
    this->listSystemCommandsDVIPNG.SetSizeExpandOnTopNoObjectInit(this->texFileNamesForPNG.size);
    for (int i=0; i<this->texFileNamesForPNG.size; i++)
    { CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(fileFlas, this->texFileNamesForPNG.TheObjects[i], false, true, false);
      fileFlas<< "\\documentclass{article}\\begin{document}\\pagestyle{empty}\n"<< this->texStringsEachFile.TheObjects[i]<<"\n\\end{document}";
      std::stringstream tempStreamLatex, tempStreamPNG;
      tempStreamLatex << "latex "<< " -output-directory="<< physicalPath<<" "<< this->texFileNamesForPNG.TheObjects[i];
      tempS= this->texFileNamesForPNG.TheObjects[i];
      tempS.resize(tempS.size()-4);
      tempStreamPNG << "dvipng "<< tempS <<".dvi -o " << tempS<<".png -T tight";
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
    out << currentSA.theDynkinDiagram.DebugString<< " & " <<currentSA.theCentralizerDiagram.DebugString<<" & " << inputData.theRootSubalgebras.numNilradicalsBySA.TheObjects[i]<<" \\\\\n";
    total+=inputData.theRootSubalgebras.numNilradicalsBySA.TheObjects[i];
  }
  out <<"Total: " <<total<<" nilradicals up to iso";
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
    this->GenerateActionKintersectBIsos(this->TheObjects[i], theGlobalVariables);
  if (useLatex)
    out <<"\\begin{tabular}{ccccc}$\\mathfrak{k}_{ss}$& $C(k_{ss})_{ss}$ & $\\#W''$ &$\\#W'''$&$\\#(W'''\\rtimes W'')$\\\\\\hline";
  if (useHtml)
    out << "<br><table><tr><td>k_{ss}</td><td></td><td>Weyl group of C(k_{ss})_{ss}</td><td>Outer automorphisms of C(k_{ss})_{ss}<td></tr>";
  for (int i=fromIndex; i<NumToProcess; i++)
  { rootSubalgebra& current= this->TheObjects[i];
    ReflectionSubgroupWeylGroup& theOuterIsos= this->CentralizerOuterIsomorphisms.TheObjects[i];
    ReflectionSubgroupWeylGroup& theIsos= this->CentralizerIsomorphisms.TheObjects[i];
    theOuterIsos.ComputeSubGroupFromGeneratingReflections(theOuterIsos.simpleGenerators, theOuterIsos.ExternalAutomorphisms, theGlobalVariables, 0, true);
    theIsos.ComputeSubGroupFromGeneratingReflections(theIsos.simpleGenerators, theIsos.ExternalAutomorphisms, theGlobalVariables, 0, true);
    if (useHtml)
      out <<"<td>";
    out << current.theDynkinDiagram.DebugString;
    if (useHtml)
      out <<"</td><td>";
    if (useLatex)
      out <<" & ";
    out << current.theCentralizerDiagram.DebugString;
    if (useHtml)
      out <<"</td><td>";
    if (useLatex)
      out <<" & ";
    out << theOuterIsos.size;
    if (useHtml)
      out <<"</td><td>";
    if (useLatex)
      out <<" & ";
    out << theIsos.size/theOuterIsos.size;
    if (useHtml)
      out <<"</td><td>";
    if (useLatex)
      out <<" & ";
    out << theIsos.size;
    if (useHtml)
      out <<"</td></tr>";
    if (useLatex)
      out <<" \\\\\n";
  }
  if (useLatex)
    out << "\\end{tabular}";
  if(useHtml)
    out << "</table><br>";
  output= out.str();
}

bool IsRegularWRT(roots& input, root& h1, root& h2, WeylGroup& theWeyl)
{ for (int i=0; i<input.size; i++)
    if (theWeyl.RootScalarKillingFormMatrixRoot(h1, input.TheObjects[i]).IsEqualToZero() && theWeyl.RootScalarKillingFormMatrixRoot(h2, input.TheObjects[i]).IsEqualToZero())
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
      if (tempSl2s.theRootSAs.AmbientWeyl.RootScalarKillingFormMatrixRoot(h1, h2).IsEqualToZero())
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
  { double x= this->RootScalarKillingFormMatrixRoot(orthonormalBasisVector1, this->RootSystem.TheObjects[i]).DoubleValue()*10;
    double y= this->RootScalarKillingFormMatrixRoot(orthonormalBasisVector2, this->RootSystem.TheObjects[i]).DoubleValue()*10;
    theGlobalVariables.theDrawingVariables.drawLineDirectly(0, 0, x, y, 0, 0);
  }
}

void DrawingVariables::drawLineDirectly(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex)
{ if (this->theDrawLineFunction!=0)
    this->theDrawLineFunction(X1+ this->centerX, Y1+ this->centerY, X2+ this->centerX, Y2+ this->centerY, thePenStyle, ColorIndex);
}

void ComputationSetup::DyckPathPolytopeComputation(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ if (false)
  { inputData.flagDyckPathComputationLoaded=inputData.thePartialFraction.theChambers.ReadFromDefaultFile(theGlobalVariables);
    inputData.thePartialFraction.theChambers.ComputeDebugString();
    inputData.thePartialFraction.theChambers.flagAnErrorHasOcurredTimeToPanic=true;
    assert(inputData.thePartialFraction.theChambers.ConsistencyCheck(true, theGlobalVariables));
  }
  inputData.flagDyckPathComputationLoaded=false;
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
  inputData.thePartialFraction.Run(inputData.thePartialFraction.theChambers.theDirections, theGlobalVariables);
  inputData.thePartialFraction.ComputeDebugString(theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1= inputData.thePartialFraction.DebugString;
  theGlobalVariables.MakeReport();
  inputData.thePartialFraction.theChambers.SliceTheEuclideanSpace(theGlobalVariables);
  inputData.thePartialFraction.theChambers.WriteToDefaultFile(theGlobalVariables);
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
    int theDimension = this->theCandidateSubAlgebras.TheObjects[i].theRootSAs.AmbientWeyl.KillingFormMatrix.NumRows;
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
      out <<"+ ";
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
        out << mult <<" x ";
      out << val <<" ";
      if (j!=this->thePartitionValues.TheObjects[i].size-1)
        out <<"+ ";
    }
    if (useHtml)
      out <<"\n<br>";
    out <<"\n";
  }
  for (int i=0; i<this->theLetters.size; i++)
  { this->ElementToStringDynkinType(i, useLatex, useHtml, tempS);
    out << tempS <<"     Module decomposition: ";
    slTwo& theSl2= this->CandidatesPrincipalSl2ofSubalgebra.TheObjects[i];
    theSl2.ElementToStringModuleDecomposition(useLatex, useHtml, tempS);
    out << tempS;
    if (this->IndicesMatchingActualSl2s.size>0)
    { out <<" Matching actual sl(2)'s: ";
      for (int j=0; j<this->IndicesMatchingActualSl2s.TheObjects[i].size; j++)
      { int tempI= this->IndicesMatchingActualSl2s.TheObjects[i].TheObjects[j];
        out << this->theSl2s.TheObjects[tempI].hCharacteristic.ElementToString() <<", ";
      }
    }
    if (useHtml)
      out <<"\n<br>";
    out<<"\n";
  }
  out <<"Remaining candidates:\n";
  if (useHtml)
    out <<"<br>\n";
  for (int i=0; i<this->theCandidateSubAlgebras.size; i++)
  { SltwoSubalgebras& currentSl2s= this->theCandidateSubAlgebras.TheObjects[i];
    if (currentSl2s.size>0)
    { this->ElementToStringDynkinType(i, useLatex, useHtml, tempS);
      out << tempS;
      if (this->RemainingCandidates.selected[i])
        out << " orbits fit\n";
      else
        out <<" orbits do not fit, embedding impossible\n";
      if (useHtml)
        out<<"<br>\n";
      currentSl2s.ElementToStringNoGenerators(tempS, theGlobalVariables, currentSl2s.theRootSAs.AmbientWeyl, useLatex, useHtml, false, 0, 0);
      out << tempS;
    }
  }
  output=out.str();
}

void WeylGroup::MakeFromDynkinType(List<char>& theLetters, List<int>& theRanks, List<int>* theMultiplicities)
{ WeylGroup tempW;
  this->KillingFormMatrix.init(0, 0);
  for (int i=0; i<theLetters.size; i++)
  { tempW.MakeArbitrary(theLetters.TheObjects[i], theRanks.TheObjects[i]);
    int numSummands=1;
    if (theMultiplicities!=0)
      numSummands =theMultiplicities->TheObjects[i];
    for (int j=0; j<numSummands; j++)
      this->KillingFormMatrix.DirectSumWith(tempW.KillingFormMatrix);
  }
}

void CombinatorialChamberContainer::WriteToFile(std::fstream& output, GlobalVariables& theGlobalVariables)
{ this->LabelChamberIndicesProperly();
  output << "Num_pointers: " << this->size << "\n";
///////////////////////////////////////////////////
  output << "Dimension: " << this->AmbientDimension << "\n";
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
  std::string tempS;
  inputData.theRootSubalgebras.ComputeDebugString(true, false, true, 0, 0, theGlobalVariables);
  inputData.theRootSubalgebras.ElementToStringCentralizerIsomorphisms(tempS, true, false, 0, inputData.theRootSubalgebras.size-1, theGlobalVariables);
  inputData.theRootSubalgebras.DebugString.append(tempS);
  inputData.theOutput.DebugString.append("\\documentclass{article}\n\\usepackage{amssymb}\n");
  inputData.theOutput.DebugString.append("\\addtolength{\\hoffset}{-3.5cm}\\addtolength{\\textwidth}{7cm}");
  inputData.theOutput.DebugString.append("\\addtolength{\\voffset}{-3.5cm}\\addtolength{\\textheight}{7cm}");
  inputData.theOutput.DebugString.append("\\begin{document}~");
  inputData.theRootSubalgebras.theBadRelations.ComputeDebugString(inputData.theRootSubalgebras, theGlobalVariables);
  inputData.theRootSubalgebras.theGoodRelations.ComputeDebugString(inputData.theRootSubalgebras, theGlobalVariables);
  //this->theRootSubalgebras.theMinRels.ComputeDebugString
  //  (this->theRootSubalgebras, *this->theGlobalVariablesContainer->Default());
  inputData.theOutput.DebugString.append(inputData.theRootSubalgebras.DebugString);
  inputData.theOutput.DebugString.append("\n\n\n");
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
  output << "Parabolics_selection: ";
  this->ParabolicsSelectionNilradicalGeneration.WriteToFile(output);
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
  this->ParabolicsSelectionNilradicalGeneration.ReadFromFile(input);
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

void rootSubalgebra::GeneratePossibleNilradicalsInit(List<Selection>& impliedSelections, Selection& ParabolicsSelection, int& parabolicsCounter)
{ impliedSelections.SetSizeExpandOnTopNoObjectInit(this->kModules.size+1);
  ParabolicsSelection.init(this->SimpleBasisCentralizerRoots.size);
  parabolicsCounter=0;
}

void rootSubalgebra::GeneratePossibleNilradicals(Controller& PauseMutex, List<Selection>& impliedSelections, Selection& ParabolicsSelection, int& parabolicsCounter, GlobalVariables& theGlobalVariables, bool useParabolicsInNilradical, bool ComputeGroupsOnly, rootSubalgebras& owner, int indexInOwner)
{  //this->ComputeAll();
  this->GenerateKmodMultTable(this->theMultTable, this->theOppositeKmods, theGlobalVariables);
  if (this->flagAnErrorHasOccuredTimeToPanic)
    this->theMultTable.ComputeDebugString(*this);
  if (this->flagAnErrorHasOccuredTimeToPanic)
    this->ComputeDebugString(theGlobalVariables);
  this->NumTotalSubalgebras=0;
  owner.GenerateActionKintersectBIsos(*this, theGlobalVariables);
  if (ComputeGroupsOnly)
    return;
  int numCycles= MathRoutines::TwoToTheNth(this->SimpleBasisCentralizerRoots.size);
  theGlobalVariables.selApproveSelAgainstOneGenerator.init(this->kModules.size);
  if (!owner.flagNilradicalComputationInitialized)
    owner.CountersNilradicalsGeneration.SetSizeExpandOnTopNoObjectInit(this->kModules.size+1);
  roots tempRootsTest;
  if (useParabolicsInNilradical)
  { this->flagFirstRoundCounting=false;
    for (; parabolicsCounter<numCycles; parabolicsCounter++, ParabolicsSelection.incrementSelection(), owner.flagNilradicalComputationInitialized=false)
    { if (!owner.flagNilradicalComputationInitialized)
      { impliedSelections.TheObjects[0].init(this->kModules.size);
        for (int j=0; j<this->CentralizerRoots.size; j++)
          if (this->rootIsInNilradicalParabolicCentralizer(ParabolicsSelection, this->CentralizerRoots.TheObjects[j]))
            impliedSelections.TheObjects[0].AddSelectionAppendNewIndex(j);
        owner.RecursionDepthNilradicalsGeneration=0;
        owner.CountersNilradicalsGeneration.TheObjects[0]=this->CentralizerRoots.size;
      }
      if (owner.flagUsingActionsNormalizerCentralizerNilradical)
        owner.RaiseSelectionUntilApproval(impliedSelections.TheObjects[0], theGlobalVariables);
/*      tempRootsTest.size=0;
      std::string tempS; std::stringstream out;
      for (int s=0; s<impliedSelections.TheObjects[0].CardinalitySelection; s++)
        tempRootsTest.AddObjectOnTop(this->kModules.TheObjects[impliedSelections.TheObjects[0].elements[s]].TheObjects[0]);
      tempS=out.str();
      assert(this->RootsDefineASubalgebra(tempRootsTest));*/
      this->GeneratePossibleNilradicalsRecursive(PauseMutex, theGlobalVariables, this->theMultTable, impliedSelections, this->theOppositeKmods, owner, indexInOwner);
    }
  }
  else
  { impliedSelections.TheObjects[0].init(this->kModules.size);
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
  output << this->WeylLetter << " " << this->KillingFormMatrix.NumRows << "\n";
  output << "Long_root_length: ";
  this->LongRootLength.WriteToFile(output);
  output << "\n";
  this->KillingFormMatrix.WriteToFile(output);
}

void WeylGroup::ReadFromFile(std::fstream& input)
{ std::string tempS; int tempI;
  input >> tempS;
  input >> this->WeylLetter >> tempI >> tempS;
  assert(tempS=="Long_root_length:");
  this->LongRootLength.ReadFromFile(input);
  this->KillingFormMatrix.ReadFromFile(input);
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
  inputData.thePartialFraction.theChambers.SliceAndComputeDebugString(theGlobalVariables);
  inputData.thePartialFraction.theChambers.thePauseController.ExitComputation();
}

void CombinatorialChamberContainer::SliceAndComputeDebugString(GlobalVariables& theGlobalVariables)
{ this->SliceTheEuclideanSpace(theGlobalVariables);
  this->QuickSortAscending();
  this->LabelChamberIndicesProperly();
  this->PurgeInternalWalls();
  this->ComputeDebugString(false);
  root tempRoot;
  theGlobalVariables.theDrawingVariables.theBuffer.init();
  tempRoot.MakeZero(this->AmbientDimension);
  this->drawOutput(theGlobalVariables.theDrawingVariables, tempRoot, 0);
}

void ComputationSetup::TestUnitCombinatorialChamberHelperFunction(std::stringstream& logstream, char WeylLetter, int Dimension, ComputationSetup& inputData, GlobalVariables& theGlobalVariables )
{ std::string tempS;
  inputData.thePartialFraction.theChambers.SetupBorelAndSlice(WeylLetter, Dimension, false, theGlobalVariables);
  tempS= inputData.thePartialFraction.theChambers.DebugString;
  inputData.thePartialFraction.theChambers.SetupBorelAndSlice(WeylLetter, Dimension, true, theGlobalVariables);
  if (tempS!=inputData.thePartialFraction.theChambers.DebugString)
    logstream << WeylLetter << Dimension << " test NOT ok !!! Total chambers: "<< inputData.thePartialFraction.theChambers.size << "\n";
  else
    logstream <<  WeylLetter << Dimension << " test OK. Total chambers: "<< inputData.thePartialFraction.theChambers.size <<"\n";
  theGlobalVariables.theIndicatorVariables.StatusString1= logstream.str();
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.MakeReport();
}

void ComputationSetup::TestUnitCombinatorialChambersChambers(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  inputData.TestUnitCombinatorialChamberHelperFunction(out, 'A', 5, inputData, theGlobalVariables);
  inputData.TestUnitCombinatorialChamberHelperFunction(out, 'A', 2, inputData, theGlobalVariables);
  inputData.TestUnitCombinatorialChamberHelperFunction(out, 'A', 3, inputData, theGlobalVariables);
  inputData.TestUnitCombinatorialChamberHelperFunction(out, 'B', 2, inputData, theGlobalVariables);
  inputData.TestUnitCombinatorialChamberHelperFunction(out, 'B', 3, inputData, theGlobalVariables);
  inputData.TestUnitCombinatorialChamberHelperFunction(out, 'C', 3, inputData, theGlobalVariables);
  inputData.TestUnitCombinatorialChamberHelperFunction(out, 'A', 4, inputData, theGlobalVariables);
  inputData.TestUnitCombinatorialChamberHelperFunction(out, 'D', 4, inputData, theGlobalVariables);
  inputData.TestUnitCombinatorialChamberHelperFunction(out, 'B', 4, inputData, theGlobalVariables);
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

void ComputationSetup::TestQuickSort(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  List<int> tempList;
  tempList.SetSizeExpandOnTopNoObjectInit(20);
  tempList.TheObjects[0]=4;
  tempList.TheObjects[1]=2;
  tempList.TheObjects[2]=3;
  tempList.TheObjects[3]=1;
  tempList.TheObjects[4]=8;
  tempList.TheObjects[5]=6;
  tempList.TheObjects[6]=7;
  tempList.TheObjects[7]=4;
  tempList.TheObjects[8]=8;
  tempList.TheObjects[9]=3;
  tempList.TheObjects[10]=5;
  tempList.TheObjects[11]=-1;
  tempList.TheObjects[12]=10;
  tempList.TheObjects[13]=12;
  tempList.TheObjects[14]=3;
  tempList.TheObjects[15]=6;
  tempList.TheObjects[16]=18;
  tempList.TheObjects[17]=-1;
  tempList.TheObjects[18]=13;
  tempList.TheObjects[19]=12;
  out << "non sorted:\n";
  for (int i=0; i < tempList.size; i++)
    out << tempList.TheObjects[i] << " ";
  out << "\nsorted:\n";
  tempList.QuickSortAscending();
  for (int i=0; i<tempList.size; i++)
    out << tempList.TheObjects[i] << " ";
  theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.MakeReport();
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
  this->theChambers.SliceTheEuclideanSpace(theGlobalVariables);
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
{/* if (!owner.flagUsingEdgeToMakeSlice)
  { for (int i=0; i<owner.HyperplanesComingFromCurrentDirectionAndSmallerIndices.size; i++)
    { root& candidateFacet = owner.HyperplanesComingFromCurrentDirectionAndSmallerIndices.TheObjects[i];
      if (this->HasHPositiveAndHNegativeVertex(candidateFacet))
      { output.Assign(candidateFacet);
        return true;
      }
    }
  } else
  { */
  root& currentDirection= owner.theDirections.TheObjects[owner.theCurrentIndex];
  bool tempBool;
  for (int i=0; i<this->Externalwalls.size; i++)
    if (this->Externalwalls.TheObjects[i].IsExternalWithRespectToDirection(currentDirection) && this->Externalwalls.TheObjects[i].EveryNeigborIsExplored(tempBool))
      for (int j=i+1; j<this->Externalwalls.size; j++)
        if (this->Externalwalls.TheObjects[j].IsExternalWithRespectToDirection(currentDirection) && this->Externalwalls.TheObjects[j].EveryNeigborIsExplored(tempBool))
          if (this->MakeFacetFromEdgeAndDirection(this->Externalwalls.TheObjects[i], this->Externalwalls.TheObjects[j], owner, currentDirection, owner.theDirections, owner.theCurrentIndex, output, theGlobalVariables))
            if (this->HasHPositiveAndHNegativeVertex(output))
              return true;
  //}
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
      if(this->SplitChamber(KillerFacet, output, direction, theGlobalVariables))
        return true;
  assert(this->TestPossibilityToSlice(direction, output));
  this->flagExplored=true;
  return false;
}

void CombinatorialChamberContainer::initNextIndex()
{ this->PurgeZeroPointers();
  this->theCurrentIndex++;
  this->indexLowestNonCheckedForGlueing=0;
  this->NumTotalGlued=0;
  this->LabelAllUnexplored();
  if (this->theCurrentIndex<this->theDirections.size)
  { this->ComputeNextIndexToSlice(this->theDirections.TheObjects[this->theCurrentIndex]);
/*    this->HyperplanesComingFromCurrentDirectionAndSmallerIndices.MakeActualSizeAtLeastExpandOnTop(this->theHyperplanes.size);
    this->HyperplanesComingFromCurrentDirectionAndSmallerIndices.size=0;
    for (int i=0; i<this->theHyperplanes.size; i++)
      if (this->theHyperplanes.TheObjects[i].OurScalarProductIsZero(this->theDirections.TheObjects[this->theCurrentIndex]))
        this->HyperplanesComingFromCurrentDirectionAndSmallerIndices.AddObjectOnTop(this->theHyperplanes.TheObjects[i]);*/
  }
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

bool roots::ElementsHavePositiveScalarProduct(const root& theRoot) const
{ for (int i=0; i<this->size; i++)
    if (!this->TheObjects[i].OurScalarProductIsPositive(theRoot))
      return false;
  return true;
}

void ComputationSetup::ProverOpenAndGo(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ inputData.theProverFixedK.ReadFromFile(inputData.theProverFixedK.ProverFileName, theGlobalVariables);
  inputData.theProver.ReadFromFile(theGlobalVariables);
  inputData.theProver.RecursionStep(inputData.theProver.theWeylGroup, theGlobalVariables);
}
