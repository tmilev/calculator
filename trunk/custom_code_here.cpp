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
    out<<"$";
  for (int i=0; i<this->highestWeights.size; i++)
  { if (this->multiplicitiesHighestWeights.TheObjects[i]>1)
      out << this->multiplicitiesHighestWeights.TheObjects[i];
    out << "V_{"<<this->highestWeights.TheObjects[i]<<"}";
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
      out <<"<td align=\"center\">Decomp. "<< tempS<<"</td>";
    }
    out <<"</tr>\n";
  }
  out <<"<tr><td align=\"center\"> "<< this->hCharacteristic.ElementToString() << "</td>";
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
      out <<"</td>";
    out <<"\n";
  }
  if (useHtml)
    out <<"</tr></table>";
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
  tempStream<<(*physicalPath) <<"fla";
  tempStream<<container.texFileNamesForPNG.size<<".tex";
  container.texFileNamesForPNG.TheObjects[container.texFileNamesForPNG.size-1]=tempStream.str();
  output <<"<img src=\""<< (*htmlPathServer) <<"fla"<<container.texFileNamesForPNG.size<<".png\">";
  if (useHtml)
    output<<"\n<br>\n";
}

void slTwo::ElementToString(std::string& output, GlobalVariables& theGlobalVariables, SltwoSubalgebras& container, int indexInContainer, bool useLatex, bool useHtml, bool usePNG, std::string* physicalPath, std::string* htmlPathServer)
{ std::stringstream out;  std::string tempS;
  this->hCharacteristic.ElementToString(tempS);
  out <<"<a name=\"sl2index" << indexInContainer<< "\">h-characteristic: "<<  tempS <<"</a>";
  this->preferredAmbientSimpleBasis.ElementToString(tempS, false, false, false);
  if (physicalPath==0 || htmlPathServer==0)
  { usePNG=false;
    useHtml=false;
  }
  if (useHtml)
    out << "<br>";
  out<<"\nSimple basis ambient algebra w.r.t defining h: "<< tempS;
  roots tempRoots;
  MatrixLargeRational tempMat;
  if (useHtml)
    out << "<br>";
  if (this->IndicesContainingRootSAs.size>1)
  { out <<"Number of containing regular semisimple subalgebras: " << this->IndicesContainingRootSAs.size;
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
      out <<tempS;
  }
  out <<"\nsl(2)-module decomposition of the ambient Lie algebra: ";
  this->ElementToStringModuleDecomposition(useLatex || usePNG, useHtml, tempS);
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  container.IndicesSl2decompositionFlas.SetSizeExpandOnTopNoObjectInit(container.size);
  container.IndicesSl2decompositionFlas.TheObjects[indexInContainer]=container.texFileNamesForPNG.size-1;
  out <<"\nThe below list one possible realization of the sl(2) subalgebra.";
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
        theSl2.IndicesMinimalContainingRootSA.AddObjectOnTopNoRepetitionOfObject(theSl2.IndicesContainingRootSAs.TheObjects[j]);
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
  //ListBasicObjects<int> IndicesEquationsByRootSpace;
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
  ListBasicObjects<int> buffer;
  for (int i=0; i<this->IndicesMinimalContainingRootSA.size; i++)
  { rootSubalgebra& theSA= owner.theRootSAs.TheObjects[this->IndicesMinimalContainingRootSA.TheObjects[i]];
    this->ComputeModuleDecomposition(theSA.PositiveRootsK, theSA.SimpleBasisK.size, this->HighestWeightsDecompositionMinimalContainingRootSA.TheObjects[i], this->MultiplicitiesDecompositionMinimalContainingRootSA.TheObjects[i], buffer, theGlobalVariables);
  }
}

//The below code is related to sl(2) subalgebras of simple Lie algebras
void slTwo::ComputeModuleDecomposition(roots& positiveRootsContainingRegularSA, int dimensionContainingRegularSA, ListBasicObjects<int>& outputHighestWeights, ListBasicObjects<int>& outputMultiplicitiesHighestWeights, ListBasicObjects<int>& outputWeightSpaceDimensions, GlobalVariables& theGlobalVariables)
{ int IndexZeroWeight=positiveRootsContainingRegularSA.size*2;
  outputWeightSpaceDimensions.initFillInObject(4*positiveRootsContainingRegularSA.size+1, 0);
  outputWeightSpaceDimensions.TheObjects[IndexZeroWeight]=dimensionContainingRegularSA;
  ListBasicObjects<int> BufferHighestWeights;
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
    theGlobalVariables.theDrawingVariables.drawLine(0, 0, x, y, 0, 0);
  }
}

void DrawingVariables::drawLine(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex)
{ if (this->theDrawFunction!=0)
    this->theDrawFunction(X1+ this->centerX, Y1+ this->centerY, X2+ this->centerX, Y2+ this->centerY, thePenStyle, ColorIndex);
}

void ComputationSetup::DyckPathPolytopeComputation(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ if (false)
  { inputData.flagDyckPathComputationLoaded=inputData.theChambers.ReadFromDefaultFile(theGlobalVariables);
    inputData.theChambers.ComputeDebugString();
    inputData.theChambers.flagAnErrorHasOcurredTimeToPanic=true;
    assert(inputData.theChambers.ConsistencyCheck());
  }
  inputData.flagDyckPathComputationLoaded=false;
  IrreducibleFiniteDimensionalModule theModule;
  QuasiPolynomial tempP;
  if (!inputData.flagDyckPathComputationLoaded)
    theModule.InitAndPrepareTheChambersForComputation(3, inputData.theChambers, theGlobalVariables);
  inputData.theChambers.flagMustStop=false;
  inputData.theChambers.flagIsRunning=true;
  inputData.theChambers.flagReachSafePointASAP=false;
  inputData.flagDyckPathComputationLoaded=true;
  inputData.thePartialFraction.LimitSplittingSteps=1;
  inputData.thePartialFraction.Run(inputData.theChambers.theDirections, theGlobalVariables);
  inputData.thePartialFraction.ComputeDebugString(theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1= inputData.thePartialFraction.DebugString;
  theGlobalVariables.MakeReport();
//  inputData.theChambers.SliceTheEuclideanSpace(theGlobalVariables);
//  inputData.theChambers.WriteToDefaultFile(theGlobalVariables);
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
  ListBasicObjects<int> ranksBuffer, multiplicitiesBuffer;
  ListBasicObjects<char> lettersBuffer;
  for (int i=0; i<this->thePartitionMultiplicities.size; i++)
    this->GenerateAllDiagramsForPartitionRecursive(i, 0, ranksBuffer, multiplicitiesBuffer, lettersBuffer);
}

void reductiveSubalgebras::GenerateAllDiagramsForPartitionRecursive(int indexPartition, int indexInPartition, ListBasicObjects<int>& ranksBuffer, ListBasicObjects<int>& multiplicitiesBuffer, ListBasicObjects<char>& lettersBuffer)
{ ListBasicObjects<int>& partitionValues= this->thePartitionValues.TheObjects[indexPartition];
  ListBasicObjects<int>& partitionMults= this->thePartitionMultiplicities.TheObjects[indexPartition];
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
  ListBasicObjects<char> lettersAvailable; lettersAvailable.SetSizeExpandOnTopNoObjectInit(5);
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
  ListBasicObjects<int> tempL;
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

bool slTwo::ModuleDecompositionFitsInto(const ListBasicObjects<int>& highestWeightsLeft, const ListBasicObjects<int>& multiplicitiesHighestWeightsLeft, const ListBasicObjects<int>& highestWeightsRight, const ListBasicObjects<int>& multiplicitiesHighestWeightsRight)
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

void reductiveSubalgebras::GenerateAllPartitionsRecursive(int remainingToBePartitioned, int CurrentValue, ListBasicObjects<int>& Multiplicities, ListBasicObjects<int>& Values)
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
  ListBasicObjects<int> buffer1, buffer2;
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

void WeylGroup::MakeFromDynkinType(ListBasicObjects<char>& theLetters, ListBasicObjects<int>& theRanks, ListBasicObjects<int>* theMultiplicities)
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
  output << "Num_pointers: " << this->size<<"\n";
///////////////////////////////////////////////////
  output << "Dimension: "<< this->AmbientDimension << "\n";
  output << "CurrentIndex: "<< this->theCurrentIndex << "\n";
  output << "Directions:\n";
  this->theDirections.WriteToFile(output, theGlobalVariables);
  output << "\nNext_index_to_slice: " << this->indexNextChamberToSlice << "\n";
  output <<"\nLowestNonCheckedForGlueing: " << this->indexLowestNonCheckedForGlueing<< "\n";
  output <<"\nTotalGlued: " << this->NumTotalGlued<< "\n";
  output << "FirstNonExploredIndex: " << this->FirstNonExploredIndex <<"\n";
  this->TheGlobalConeNormals.WriteToFile(output, theGlobalVariables);
  output << "\n";
  this->startingCones.WriteToFile(output, theGlobalVariables);
  output<<"\nTheFacets: ";
  this->theHyperplanes.WriteToFile(output);
////////////////////////////////////////////////////
  output << "\nPreferredNextChambers: " << this->PreferredNextChambers.size << " ";
  for (int i=0; i<this->PreferredNextChambers.size; i++)
    output << this->PreferredNextChambers.TheObjects[i] << " ";
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i]!=0)
    { output <<"\nChamber:\n";
      this->TheObjects[i]->WriteToFile(output, theGlobalVariables);
    } else
      output <<"Empty\n";
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
  input>>tempS;
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
