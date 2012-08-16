//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1.h"
#include "vpfHeader1_2.h"
ProjectInformationInstance ProjectInfoVpf9_2cpp(__FILE__, "Main implementation file, part 3. ");

void ReflectionSubgroupWeylGroup::WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)
{  output << "generator_reflections: ";
  this->simpleGenerators.WriteToFile(output, theGlobalVariables);
  output << "\nouter_generators: ";
  this->ExternalAutomorphisms.WriteToFile(output, theGlobalVariables);
}

void ReflectionSubgroupWeylGroup::ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables)
{ std::string tempS;
  input >> tempS;
  this->simpleGenerators.ReadFromFile(input, theGlobalVariables);
  input >> tempS;
  this->ExternalAutomorphisms.ReadFromFile(input, theGlobalVariables);
}

void ReflectionSubgroupWeylGroup::ComputeSubGroupFromGeneratingReflections
(Vectors<Rational>& inputGenerators, List<Vectors<Rational> > & inputExternalAutos, GlobalVariables& theGlobalVariables,
 int UpperLimitNumElements, bool recomputeAmbientRho)
{ HashedList<Vector<Rational> >& orbitRho = theGlobalVariables.hashedRootsComputeSubGroupFromGeneratingReflections.GetElement();
  this->truncated=false;
  this->Clear();
  orbitRho.Clear();
  if (this->AmbientWeyl.CartanSymmetric.NumRows<1)
    return;
  if (recomputeAmbientRho)
    this->AmbientWeyl.ComputeRho(false);
  this->simpleGenerators.CopyFromBase(inputGenerators);
  this->AmbientWeyl.TransformToSimpleBasisGenerators(this->simpleGenerators);
  this->ComputeRootSubsystem();
  ElementWeylGroup tempEW;
  tempEW.size=0;
  Vector<Rational> tempRoot;
  tempRoot=(this->AmbientWeyl.rho);
  // rho is invariant under external graph automorphisms (!)
  //Hence the below lines. Needs to be fixed (mathematically).
  //for the time being the below lines remain, until I think of how to do it properly.
  tempRoot*=(50);
  tempRoot.TheObjects[0].AddInteger(1);
  orbitRho.AddOnTop(tempRoot);
  this->AddOnTop(tempEW);
  Vector<Rational> currentRoot;
  for (int i=0; i<this->size; i++)
  { tempEW=this->TheObjects[i];
    for (int j=0; j<this->simpleGenerators.size; j++)
    { this->AmbientWeyl.ReflectBetaWRTAlpha(this->simpleGenerators.TheObjects[j], orbitRho.TheObjects[i], false, currentRoot);
      if (!orbitRho.Contains(currentRoot))
      { orbitRho.AddOnTop(currentRoot);
        tempEW.AddOnTop(j);
        this->AddOnTop(tempEW);
        tempEW.PopLastObject();
      }
    }
    for (int j=1; j<inputExternalAutos.size; j++)
    { orbitRho[i].GetCoordsInBasiS(inputExternalAutos[j], currentRoot);
      if (!orbitRho.Contains(currentRoot))
      { orbitRho.AddOnTop(currentRoot);
        tempEW.AddOnTop(j+this->simpleGenerators.size);
        this->AddOnTop(tempEW);
        tempEW.PopLastObject();
      }
    }
    if (UpperLimitNumElements>0)
      if (this->size>=UpperLimitNumElements)
      { this->truncated=true;
        return;
      }
/*    if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()!=0)
    { std::stringstream out;
      out << "Generated: " << i+1<<" elements";
      theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
      theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
      theGlobalVariables.FeedIndicatorWindow(theGlobalVariables.theIndicatorVariables);
    }*/
  }
}

void ElementWeylAlgebra::MultiplyTwoMonomials(MonomialP& left, MonomialP& right, Polynomial<Rational>& OrderedOutput)
{ MonomialP buffer;
  SelectionWithDifferentMaxMultiplicities tempSel;
  assert(left.monBody.size%2==0);
  int theDimension=left.monBody.size/2;
  tempSel.Multiplicities.initFillInObject(theDimension, 0);
  tempSel.MaxMultiplicities.SetSize(theDimension);
  for (int i=0; i<theDimension; i++)
  { if (!left[theDimension+i].IsSmallInteger())
    { std::cout << "This is a programming error. You have requested operations with elements of weyl algebra"
      << "that have monomials of exponent " << left[theDimension+i] << " which I cannot handle. "
      << "Error catching should be done at an earlier level. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    tempSel.MaxMultiplicities[i]=left[theDimension+i].NumShort;
  }
  tempSel.elements.initFillInObject(theDimension, 0);
  buffer.MakeConst(left.monBody.size);
  assert(left.monBody.size==right.monBody.size);
  OrderedOutput.MakeZero(left.monBody.size);
  int numCycles= tempSel.getTotalNumSubsets();
  Rational coeffBuff;
  for (int i=0; i<numCycles; i++)
  { coeffBuff=1;
    for (int k=0; k< theDimension; k++)
    { int multDrop=tempSel.Multiplicities[k];
      coeffBuff*=Rational::NChooseK(left[theDimension+k].NumShort, multDrop)
      *Rational::NChooseK(right[k].NumShort, multDrop)* Rational::Factorial(multDrop);
      buffer[k]=left[k]+right[k]-multDrop;
      buffer[k+theDimension]= left[k+theDimension]+right[k+theDimension]-multDrop;
    }
    OrderedOutput.AddMonomial(buffer, coeffBuff);
    tempSel.IncrementSubset();
  }
}

void ElementWeylAlgebra::LieBracketOnTheLeftMakeReport(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables, std::string& report)
{ this->LieBracketOnTheLeft(standsOnTheLeft, theGlobalVariables);
  report=this->ToString();
}

void ElementWeylAlgebra::LieBracketOnTheRightMakeReport(ElementWeylAlgebra& standsOnTheRight, GlobalVariables& theGlobalVariables, std::string& report)
{ this->LieBracketOnTheRight(standsOnTheRight, theGlobalVariables);
  report=this->ToString();
}

void ElementWeylAlgebra::LieBracketOnTheLeft(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables)
{ ElementWeylAlgebra tempEl1, tempEl2;
  tempEl1.Assign(*this);
  tempEl1.MultiplyOnTheLeft(standsOnTheLeft, theGlobalVariables);
  //tempEl1.ComputeDebugString(false);
  tempEl2.Assign(standsOnTheLeft);
  tempEl2.MultiplyOnTheLeft(*this, theGlobalVariables);
  //tempEl2.ComputeDebugString(false);
  this->Assign(tempEl1);
  this->Subtract(tempEl2);
  //this->ComputeDebugString(false);
}

void ElementWeylAlgebra::MakeConst(int NumVars, const Rational& theConst)
{ this->MakeZero(NumVars);
  MonomialP tempM;
  tempM.MakeConst(this->NumVariables*2);
  this->StandardOrder.AddMonomial(tempM, theConst);
}

void ElementWeylAlgebra::LieBracketOnTheRight(ElementWeylAlgebra& standsOnTheRight, GlobalVariables& theGlobalVariables)
{ ElementWeylAlgebra tempEl1, tempEl2;
  tempEl1.Assign(standsOnTheRight);
  tempEl1.MultiplyOnTheLeft(*this, theGlobalVariables);
  //tempEl1.ComputeDebugString(false);
  tempEl2.Assign(*this);
  tempEl2.MultiplyOnTheLeft(standsOnTheRight, theGlobalVariables);
  //tempEl2.ComputeDebugString(false);
  this->Assign(tempEl1);
  this->Subtract(tempEl2);
  //this->ComputeDebugString(false);
}

void ElementWeylAlgebra::MultiplyOnTheLeft(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables)
{ Polynomial<Rational> buffer;
  Polynomial<Rational> Accum;
  Accum.MakeZero(this->NumVariables*2);
  for (int j=0; j<standsOnTheLeft.StandardOrder.size; j++)
    for (int i=0; i<this->StandardOrder.size; i++)
    { this->MultiplyTwoMonomials(standsOnTheLeft.StandardOrder[j], this->StandardOrder[i], buffer);
      buffer*=standsOnTheLeft.StandardOrder.theCoeffs[j]*this->StandardOrder.theCoeffs[i];
      Accum+=(buffer);
    }
  this->StandardOrder=Accum;
}

void ElementWeylAlgebra::MultiplyOnTheRight(const ElementWeylAlgebra& standsOnTheRight)
{ Polynomial<Rational>  buffer;
  Polynomial<Rational>  Accum;
  Accum.MakeZero(this->NumVariables*2);
  for (int j=0; j<standsOnTheRight.StandardOrder.size; j++)
    for (int i=0; i<this->StandardOrder.size; i++)
    { this->MultiplyTwoMonomials(this->StandardOrder[i], standsOnTheRight.StandardOrder[j], buffer);
      buffer*=this->StandardOrder.theCoeffs[i]*standsOnTheRight.StandardOrder.theCoeffs[j];
      Accum+=(buffer);
    }
  this->StandardOrder=Accum;
}

std::string ElementWeylAlgebra::ToString(FormatExpressions* theFormat)
{ if (this->NumVariables>100)
    return this->StandardOrder.ToString();
  FormatExpressions tempFormat;
  tempFormat.polyAlphabeT.SetSize(this->NumVariables*2);
  for (int i=0; i<this->NumVariables; i++)
  { std::stringstream tempStream;
    if (theFormat!=0)
      tempFormat.polyAlphabeT[i]=theFormat->GetPolyLetter(i);
    else
    { std::stringstream tempStream2;
      tempStream2 << "x_{" << i+1 << "}";
      tempFormat.polyAlphabeT[i]=tempStream2.str();
    }
    tempStream << "\\partial";
    if (tempFormat.polyAlphabeT[i][0]!='x')
      tempStream << "_{" << tempFormat.polyAlphabeT[i] << "}";
    else
      tempStream << "_{" << i+1 << "}";
    tempFormat.polyAlphabeT[i+NumVariables]=tempStream.str();
  }
  return this->StandardOrder.ToString(&tempFormat);
}

void ElementWeylAlgebra::SetNumVariables(int newNumVars)
{ if (newNumVars<this->NumVariables)
    this->NumVariables=newNumVars;
  Polynomial<Rational> Accum;
  Accum.MakeZero(newNumVars*2);
  Accum.ReservE(this->StandardOrder.size);
  MonomialP tempM;
  for (int i=0; i<this->StandardOrder.size; i++)
  { tempM.MakeConst(newNumVars*2);
    for (int j=0; j< this->NumVariables; j++)
    { tempM[j]=this->StandardOrder[i][j];
      tempM[j+newNumVars]=this->StandardOrder[i][j+this->NumVariables];
    }
    Accum.AddMonomial(tempM, this->StandardOrder.theCoeffs[i]);
  }
  this->NumVariables= newNumVars;
  this->StandardOrder=(Accum);
}

void ElementWeylAlgebra::MakeGEpsPlusEpsInTypeD(int i, int j, int NumVars)
{ this->MakeZero(NumVars*2);
  MonomialP tempMon;
  tempMon.MakeConst(this->NumVariables*2);
  tempMon[i]=1;
  tempMon[j+this->NumVariables+NumVars]=1;
  this->StandardOrder.AddMonomial(tempMon, 1);
  tempMon.MakeConst(this->NumVariables*2);
  tempMon[j]=1;
  tempMon[i+this->NumVariables+NumVars]=1;
  this->StandardOrder.AddMonomial(tempMon,1);
}

void ElementWeylAlgebra::MakeGEpsMinusEpsInTypeD(int i, int j, int NumVars)
{ this->MakeZero(NumVars*2);
  MonomialP tempMon;
  tempMon.MakeConst(this->NumVariables*2);
  tempMon[i]=1;
  tempMon[j+this->NumVariables]=1;
  this->StandardOrder.AddMonomial(tempMon,1);
  tempMon.MakeConst(this->NumVariables*2);
  tempMon[j+NumVars]=1;
  tempMon[i+this->NumVariables+NumVars]=1;
  this->StandardOrder.AddMonomial(tempMon,1);
}

void ElementWeylAlgebra::MakeGMinusEpsMinusEpsInTypeD(int i, int j, int NumVars)
{ this->MakeZero(NumVars*2);
  MonomialP tempMon;
  tempMon.MakeConst(this->NumVariables*2);
  tempMon[i+NumVars]=1;
  tempMon[j+this->NumVariables]=1;
  this->StandardOrder.AddMonomial(tempMon,1);
  tempMon.MakeConst(this->NumVariables*2);
  tempMon[j+NumVars]=1;
  tempMon[i+this->NumVariables]=1;
  this->StandardOrder.AddMonomial(tempMon,1);
}

void ElementWeylAlgebra::Makedidj(int i, int j, int NumVars)
{ this->MakeZero(NumVars);
  MonomialP tempMon;
  tempMon.MakeConst(this->NumVariables*2);
  tempMon[i+NumVars]+=1;
  tempMon[j+NumVars]+=1;
  this->StandardOrder.AddMonomial(tempMon, 1);
}

void ElementWeylAlgebra::Makexixj(int i, int j, int NumVars)
{ this->MakeZero(NumVars);
  MonomialP tempMon;
  tempMon.MakeConst(this->NumVariables*2);
  tempMon[i]+=1;
  tempMon[j]+=1;
  this->StandardOrder.AddMonomial(tempMon, 1);
}

void ElementWeylAlgebra::Makexi(int i, int NumVars)
{ this->MakeZero(NumVars);
  MonomialP tempMon;
  tempMon.MakeConst(this->NumVariables*2);
  tempMon[i]+=1;
  this->StandardOrder.AddMonomial(tempMon, 1);
}

void ElementWeylAlgebra::Makedi(int i, int NumVars)
{ this->MakeZero(NumVars);
  MonomialP tempMon;
  tempMon.MakeConst(this->NumVariables*2);
  tempMon[i+NumVars]+=1;
  this->StandardOrder.AddMonomial(tempMon, 1);
}

void ElementWeylAlgebra::Makexidj(int i, int j, int NumVars)
{ this->MakeZero(NumVars);
  MonomialP tempMon;
  tempMon.MakeConst(this->NumVariables*2);
  tempMon[i]=1;
  tempMon[NumVars+j]=1;
  this->StandardOrder.AddMonomial(tempMon, 1);
}

void ElementWeylAlgebra::MakeZero(int NumVars)
{ this->NumVariables=NumVars;
  this->StandardOrder.MakeZero(this->NumVariables*2);
}

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
    out << "$";
  if (useHtml)
  { out << "<table><tr><td align=\"center\">Char.</td>";
    for (int i=0; i<this->IndicesMinimalContainingRootSA.size; i++)
    { rootSubalgebra& theSA= owner.theRootSAs.TheObjects[this->IndicesMinimalContainingRootSA.TheObjects[i]];
      CGI::clearDollarSigns(theSA.theDynkinDiagram.DynkinStrinG, tempS);
      out << "<td align=\"center\">Decomp. " << tempS << "</td>";
    }
    out << "</tr>\n";
  }
  out << "<tr><td align=\"center\"> " << this->hCharacteristic.ToString() << "</td>";
  for (int k=0; k<this->IndicesMinimalContainingRootSA.size; k++)
  { rootSubalgebra& theSA= owner.theRootSAs.TheObjects[this->IndicesMinimalContainingRootSA.TheObjects[k]];
    CGI::clearDollarSigns(theSA.theDynkinDiagram.DynkinStrinG, tempS);
    if (useHtml)
      out << "<td align=\"center\">";
    for (int i=0; i<this->HighestWeightsDecompositionMinimalContainingRootSA.TheObjects[k].size; i++)
    { if (this->MultiplicitiesDecompositionMinimalContainingRootSA.TheObjects[k].TheObjects[i]>1)
        out << this->MultiplicitiesDecompositionMinimalContainingRootSA.TheObjects[k].TheObjects[i];
      out << "V_{"<<this->HighestWeightsDecompositionMinimalContainingRootSA.TheObjects[k].TheObjects[i] << "}";
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
  container.texFileNamesForPNG.SetSize(container.texFileNamesForPNG.size+1);
  container.texStringsEachFile.SetSize(container.texFileNamesForPNG.size);
  (*container.texStringsEachFile.LastObject())=formulaTex;
  tempStream << (*physicalPath) << "fla";
  tempStream << container.texFileNamesForPNG.size << ".tex";
  container.texFileNamesForPNG.TheObjects[container.texFileNamesForPNG.size-1]=tempStream.str();
  output << "<img src=\"" << (*htmlPathServer) << "fla" << container.texFileNamesForPNG.size << ".png\">";
  if (useHtml)
    output << "\n<br>\n";
}

void slTwo::ToString(std::string& output, GlobalVariables& theGlobalVariables, SltwoSubalgebras& container, int indexInContainer, bool useLatex, bool useHtml, bool usePNG, std::string* physicalPath, std::string* htmlPathServer, FormatExpressions& PolyFormatLocal)
{ std::stringstream out;  std::string tempS;
  out << "<a name=\"sl2index" << indexInContainer << "\">h-characteristic: " <<  this->hCharacteristic.ToString() << "</a>";
  tempS=this->preferredAmbientSimpleBasis.ToString();
  if (physicalPath==0 || htmlPathServer==0)
  { usePNG=false;
    useHtml=false;
  }
  if (useHtml)
    out << "<br>";
  out << "\nSimple basis ambient algebra w.r.t defining h: " << tempS;
  Vectors<Rational> tempRoots;
  Matrix<Rational> tempMat;
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
      currentSA.theDynkinDiagram.ElementToStrinG(tempS, useLatex, true);
      CGI::clearDollarSigns(tempS, tempS);
    }
    currentSA.theDynkinDiagram.GetSimpleBasisInBourbakiOrder(tempRoots);
    out << tempS;
    if(useHtml)
      out << "</a>";
    tempS = tempRoots.ToString();
    if (useHtml)
      out << "<br>";
    out << "\nSimple basis subalgebra: " << tempS;
    currentSA.theDynkinDiagram.GetKillingFormMatrixUseBourbakiOrder(tempMat, this->owner->theWeyl);
    if (!usePNG)
      tempS=tempMat.ToString(useHtml, useLatex);
    else
    { std::stringstream tempStreamX;
      tempS=tempMat.ToString(false, true);
      tempStreamX << "\\[" << tempS << "\\]";
      tempS=tempStreamX.str();
    }
    if (useHtml)
      out << "<br>";
    out << "\nSymmetric Cartan matrix in Bourbaki order:\n";
    if (useHtml)
    { out << "<br>";
      this->ElementToHtmlCreateFormulaOutputReference
      (tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
    } else
      out << tempS;
  }
  out << "\nsl(2)-module decomposition of the ambient Lie algebra: ";
  this->ElementToStringModuleDecomposition(useLatex || usePNG, useHtml, tempS);
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  container.IndicesSl2decompositionFlas.SetSize(container.size);
  container.IndicesSl2decompositionFlas.TheObjects[indexInContainer]=container.texFileNamesForPNG.size-1;
  out << "\nThe below list one possible realization of the sl(2) subalgebra.";
  if (useHtml)
    out << "\n<br>\n";
  std::stringstream tempStreamH, tempStreamE, tempStreamF, tempStreamHE, tempStreamHF, tempStreamEF;
  tempS=this->theH.ToString(&PolyFormatLocal);
  tempStreamH << "\n$h=$ $" << tempS << "$";
  tempS= tempStreamH.str();
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  tempStreamE << "\n$e=$ $" << this->theE.ToString(&PolyFormatLocal) << "$";
  tempS= tempStreamE.str();
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  tempStreamF << "\n$f=$ $" << this->theF.ToString(&PolyFormatLocal) << "$";
  tempS= tempStreamF.str();
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  out << "\n\nThe below are the Lie brackets of the above elements. Printed for debugging.";
  if (useHtml)
    out << "\n<br>\n";
  tempStreamEF << "\n$[e, f]=$ $" <<  this->bufferEbracketF.ToString(&PolyFormatLocal) << "$";
  tempS= tempStreamEF.str();
  this->ElementToHtmlCreateFormulaOutputReference
  (tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  tempStreamHE << "\n$[h, e]=$ $" << this->bufferHbracketE.ToString(&PolyFormatLocal) << "$";
  tempS= tempStreamHE.str();
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  tempStreamHF << "\n$[h, f]=$ $" << this->bufferHbracketF.ToString(&PolyFormatLocal) << "$";
  tempS= tempStreamHF.str();
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  //this->theSystemMatrixForm.ToString(tempS);
  //out <<"\nSystem matrix form we try to solve:\n"<< tempS;
  //this->theSystemColumnVector.ToString(tempS);
  //out <<"\nColumn vector of the system:\n"<<tempS;
  std::stringstream tempStreamActual;
  for (int i=0; i<this->theSystemToBeSolved.size; i++)
  { tempS=this->theSystemToBeSolved.TheObjects[i].ToString(&PolyFormatLocal);
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
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  output = out.str();
}

void slTwo::ElementToHtml(std::string& filePath)
{ std::fstream theFile;
  std::string theFileName=filePath;
  theFileName.append("theSlTwo.txt");
  CGI::OpenFileCreateIfNotPresent(theFile, filePath, false, true, false);
}

void SemisimpleLieAlgebra::FindSl2Subalgebras(SltwoSubalgebras& output, GlobalVariables& theGlobalVariables)
{ output.theRootSAs.GenerateAllReductiveRootSubalgebrasUpToIsomorphism(theGlobalVariables, true, true);
  //output.theRootSAs.ComputeDebugString(false, false, false, 0, 0, theGlobalVariables);
  output.IndicesSl2sContainedInRootSA.SetSize(output.theRootSAs.size);
  output.IndicesSl2sContainedInRootSA.ReservE(output.theRootSAs.size*2);
  for (int i=0; i<output.IndicesSl2sContainedInRootSA.size; i++)
    output.IndicesSl2sContainedInRootSA.TheObjects[i].size=0;
  theGlobalVariables.theIndicatorVariables.StatusString1=output.theRootSAs.DebugString;
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.ProgressReportStringsNeedRefresh=true;
  theGlobalVariables.MakeReport();
  for (int i=0; i<output.theRootSAs.size-1; i++)
  { output.theRootSAs[i].GetSsl2SubalgebrasAppendListNoRepetition(output, i, theGlobalVariables, *this);
    theGlobalVariables.ClearIndicatorVars();
    std::stringstream tempStream;
    tempStream << "Exploring root subalgebra number " << (i+1)
    << " out of " << output.theRootSAs.size-1 << " non-trivial";
    theGlobalVariables.theIndicatorVariables.ProgressReportStrings[0]=tempStream.str();
    theGlobalVariables.MakeReport();
  }
  for (int i=0; i<output.size; i++)
  { slTwo& theSl2= output[i];
    theSl2.IndicesMinimalContainingRootSA.ReservE(theSl2.IndicesContainingRootSAs.size);
    theSl2.IndicesMinimalContainingRootSA.size=0;
    for (int j=0; j<theSl2.IndicesContainingRootSAs.size; j++)
    { bool isMinimalContaining=true;
//      rootSubalgebra& currentRootSA = output.theRootSAs.TheObjects[];
      for (int k=0; k<theSl2.IndicesContainingRootSAs.size; k++)
      { rootSubalgebra& theOtherRootSA =
        output.theRootSAs[theSl2.IndicesContainingRootSAs[k]];
        if (theOtherRootSA.indicesSubalgebrasContainingK.ContainsObject(theSl2.IndicesContainingRootSAs[j]))
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

void SemisimpleLieAlgebra::FindSl2Subalgebras(SltwoSubalgebras& output, char WeylLetter, int WeylRank, GlobalVariables& theGlobalVariables)
{ output.theRootSAs.AmbientWeyl.MakeArbitrary(WeylLetter, WeylRank);
  this->theWeyl.MakeArbitrary(WeylLetter, WeylRank);
  this->FindSl2Subalgebras(output, theGlobalVariables);
}

void rootSubalgebra::GetSsl2SubalgebrasAppendListNoRepetition
(SltwoSubalgebras& output, int indexInContainer, GlobalVariables& theGlobalVariables,
 SemisimpleLieAlgebra& theLieAlgebra)
{ //reference: Dynkin, semisimple Lie algebras of simple lie algebras, theorems 10.1-10.4
  Selection theRootsWithZeroCharacteristic;
  Vectors<Rational> RootsWithCharacteristic2;
  RootsWithCharacteristic2.ReservE(this->PositiveRootsK.size);
  DynkinDiagramRootSubalgebra tempDiagram;
  int theRelativeDimension = this->SimpleBasisK.size;
  theRootsWithZeroCharacteristic.init(theRelativeDimension);
  Matrix<Rational> InvertedRelativeKillingForm;
  InvertedRelativeKillingForm.init(theRelativeDimension, theRelativeDimension);
  for (int k=0; k<theRelativeDimension; k++)
    for (int j=0; j<theRelativeDimension; j++)
      InvertedRelativeKillingForm.elements[k][j]=this->AmbientWeyl.RootScalarCartanRoot
      (this->SimpleBasisK[k], this->SimpleBasisK[j]);
  InvertedRelativeKillingForm.Invert();
  theLieAlgebra.theWeyl=this->AmbientWeyl;
  theLieAlgebra.ComputeChevalleyConstantS(theGlobalVariables);
  //int relativeDimension =
  int numCycles= MathRoutines::TwoToTheNth(theRootsWithZeroCharacteristic.MaxSize);
  Vectors<Rational> tempRoots;
  tempRoots.ReservE(theRootsWithZeroCharacteristic.MaxSize);
  Vectors<Rational> relativeRootSystem, bufferVectors;
  Matrix<Rational> tempMat;
//  Selection tempSel;
  this->PositiveRootsK.GetCoordsInBasis(this->SimpleBasisK, relativeRootSystem, bufferVectors, tempMat);

  slTwo theSl2;
  theSl2.owner = &theLieAlgebra;
  for (int i=0; i<numCycles; i++, theRootsWithZeroCharacteristic.incrementSelection())
  { tempRoots.size=0;
    for (int j=0; j<theRootsWithZeroCharacteristic.CardinalitySelection; j++)
      tempRoots.AddOnTop(this->SimpleBasisK[theRootsWithZeroCharacteristic.elements[j]]);
    tempDiagram.ComputeDiagramTypeModifyInput(tempRoots, this->AmbientWeyl);
    int theSlack=0;
    RootsWithCharacteristic2.size=0;
    for (int j=0; j<relativeRootSystem.size; j++)
    { Rational DimTwoSpace=0;
      for (int k=0; k<theRelativeDimension; k++)
        if (!theRootsWithZeroCharacteristic.selected[k])
        { DimTwoSpace+=relativeRootSystem[j][k];
          if (DimTwoSpace>1)
            break;
        }
      if (DimTwoSpace==1)
      { theSlack++;
        RootsWithCharacteristic2.AddOnTop(this->PositiveRootsK[j]);
      }
    }
    int theDynkinEpsilon = tempDiagram.NumRootsGeneratedByDiagram() + theRelativeDimension - theSlack;
    //if Dynkin's epsilon is not zero the subalgebra cannot be an S sl(2) subalgebra.
    //otherwise, as far as I understand, it always is (but generators still have to be found)
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
      theSl2.theH.MakeHgenerator(characteristicH, *theLieAlgebra.owner, theLieAlgebra.indexInOwner);
      theSl2.theE.MakeZero(*theLieAlgebra.owner, theLieAlgebra.indexInOwner);
      theSl2.theF.MakeZero(*theLieAlgebra.owner, theLieAlgebra.indexInOwner);
      //theSl2.ComputeDebugString(false, false, theGlobalVariables);
//      std::cout << "<br>accounting " << characteristicH.ToString();
      if(theLieAlgebra.AttemptExtendingHEtoHEFWRTSubalgebra
         (RootsWithCharacteristic2, relativeRootSystem, theRootsWithZeroCharacteristic, this->SimpleBasisK,
          characteristicH, theSl2.theE, theSl2.theF, theSl2.theSystemMatrixForm, theSl2.theSystemToBeSolved,
          theSl2.theSystemColumnVector, theGlobalVariables))
      { int indexIsoSl2;
        theSl2.MakeReportPrecomputations(theGlobalVariables, output, output.size, indexInContainer, *this);
        if(output.ContainsSl2WithGivenHCharacteristic(theSl2.hCharacteristic, &indexIsoSl2))
        { output.TheObjects[indexIsoSl2].IndicesContainingRootSAs.AddOnTop(indexInContainer);
          output.IndicesSl2sContainedInRootSA.TheObjects[indexInContainer].AddOnTop(indexIsoSl2);
        }
        else
        { output.IndicesSl2sContainedInRootSA.TheObjects[indexInContainer].AddOnTop(output.size);
          output.AddOnTop(theSl2);
        }
      }
      else
        output.BadHCharacteristics.AddOnTop(characteristicH);
    }
    std::stringstream out;
    out << "Exploring Dynkin characteristics case " << i+1 << " out of " << numCycles;
//    std::cout << "<br>" << out.str();
    theGlobalVariables.theIndicatorVariables.ProgressReportStrings[1]=out.str();
    theGlobalVariables.MakeReport();
  }
//  std::cout << "Bad chracteristics: " << output.BadHCharacteristics.ToString();
}

bool SemisimpleLieAlgebra:: AttemptExtendingHEtoHEFWRTSubalgebra(Vectors<Rational>& RootsWithCharacteristic2, Vectors<Rational>& relativeRootSystem, Selection& theZeroCharacteristics, Vectors<Rational>& simpleBasisSA, Vector<Rational>& h, ElementSemisimpleLieAlgebra& outputE, ElementSemisimpleLieAlgebra& outputF, Matrix<Rational> & outputMatrixSystemToBeSolved, PolynomialSubstitution<Rational>& outputSystemToBeSolved, Matrix<Rational> & outputSystemColumnVector, GlobalVariables& theGlobalVariables)
{ if (theZeroCharacteristics.CardinalitySelection== theZeroCharacteristics.MaxSize)
    return false;
  Vectors<Rational> SelectedExtraPositiveRoots;
  Vectors<Rational> rootsInPlay;
  rootsInPlay.size=0;
  SelectedExtraPositiveRoots.size=0;
  int theRelativeDimension = simpleBasisSA.size;
//  int theDimension= this->theWeyl.CartanSymmetric.NumRows;
  assert(theRelativeDimension==theZeroCharacteristics.MaxSize);
  Vector<Rational> tempRoot, tempRoot2;
  //format. We are looking for an sl(2) for which e= a_0 g^\alpha_0+\dots a_kg^\alpha_k, and f=b_0 g^{-\alpha_0}+... +b_kg^{-\alpha_k}
  //where the first \alpha's are ordered as in rootsInPlay.
  //Those are ordered as follows. First come the simple Vectors<Rational> of characteristic 2,
  //and the last \alpha's are the members of SelectedExtraPositiveRoots
  //(i.e. Vectors<Rational> equal to the sum of one simple Vector<Rational> of characteristic 2 a simple Vector<Rational> of characteristic 0).
  // Then the first k variables of the polynomials below will be a_0, ..., a_k., and the last k variables will be the b_i's
  // the l^th polynomial will correspond to the coefficient of g^\alpha_{l/2}, where l/2 is the index of the Vector<Rational>
  // of SelectedExtraPositiveRoots, if l is even, and to the
  // coefficient of  g^{-\alpha_{(l+1)/2}} otherwise
  for (int i=0; i<theRelativeDimension; i++)
    if (!theZeroCharacteristics.selected[i])
      rootsInPlay.AddOnTop(simpleBasisSA.TheObjects[i]);
    else
      for (int j=0; j<theRelativeDimension; j++)
        if (!theZeroCharacteristics.selected[j])
        { tempRoot= simpleBasisSA.TheObjects[i]+simpleBasisSA.TheObjects[j];
          if (this->theWeyl.IsARoot(tempRoot))
            SelectedExtraPositiveRoots.AddOnTop(tempRoot);
        }
  SelectedExtraPositiveRoots.size=0;
  for (int i=0; i<RootsWithCharacteristic2.size; i++)
    if (!simpleBasisSA.ContainsObject(RootsWithCharacteristic2.TheObjects[i]))
      SelectedExtraPositiveRoots.AddOnTop(RootsWithCharacteristic2.TheObjects[i]);
  int numRootsChar2 = rootsInPlay.size;
  rootsInPlay.AddListOnTop(SelectedExtraPositiveRoots);
  int halfNumberVariables = rootsInPlay.size;
  int numberVariables = halfNumberVariables*2;
  MonomialP tempM;
  tempM.MakeConst((int)numberVariables);
  Matrix<Rational> coeffsF;
  coeffsF.init(1, halfNumberVariables);
  for (int i=0; i<numRootsChar2; i++)
    coeffsF.elements[0][i]=i+1; //(i%2==0)? 1: 2;
  for (int i=numRootsChar2; i<coeffsF.NumCols; i++)
    coeffsF.elements[0][i]=i+1;
  this->initHEFSystemFromECoeffs(theRelativeDimension, theZeroCharacteristics, rootsInPlay, simpleBasisSA, SelectedExtraPositiveRoots, numberVariables, numRootsChar2, halfNumberVariables, h, coeffsF, outputMatrixSystemToBeSolved, outputSystemColumnVector, outputSystemToBeSolved);
  Matrix<Rational>  tempMat, tempMatColumn, tempMatResult;
  tempMat=(outputMatrixSystemToBeSolved);
  tempMatColumn=(outputSystemColumnVector);
  outputF.MakeZero(*this->owner, this->indexInOwner);
  outputE.MakeZero(*this->owner, this->indexInOwner);
//  if(Matrix<Rational> ::Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(outputMatrixSystemToBeSolved, outputSystemColumnVector, tempMatResult))
  ChevalleyGenerator tempGen;
  if(Matrix<Rational> ::Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(tempMat, tempMatColumn, tempMatResult))
  { for (int i=0; i<rootsInPlay.size; i++)
    { tempGen.MakeGenerator
      (*this->owner, this->indexInOwner,
        this->GetGeneratorFromRoot( -rootsInPlay[i]));
      outputF.AddMonomial(tempGen, coeffsF.elements[0][i]);
      tempGen.MakeGenerator
      (*this->owner, this->indexInOwner,
        this->GetGeneratorFromRoot( rootsInPlay[i]));
      outputE.AddMonomial(tempGen, tempMatResult.elements[i][0]);
    }
    return true;
  }
  return true;
}

void SemisimpleLieAlgebra::initHEFSystemFromECoeffs
(int theRelativeDimension, Selection& theZeroCharacteristics, Vectors<Rational>& rootsInPlay, Vectors<Rational>& simpleBasisSA,
 Vectors<Rational>& SelectedExtraPositiveRoots, int numberVariables, int numRootsChar2, int halfNumberVariables,
 Vector<Rational>& targetH, Matrix<Rational>& inputFCoeffs, Matrix<Rational>& outputMatrixSystemToBeSolved,
 Matrix<Rational>& outputSystemColumnVector, PolynomialSubstitution<Rational>& outputSystemToBeSolved)
{ Vector<Rational> tempRoot;
  static int ProblemCounter=0;
  ProblemCounter++;
  MonomialP tempM;
  Rational tempRat;
  HashedList<Vector<Rational> > RootSpacesThatNeedToBeKilled;
  RootSpacesThatNeedToBeKilled.SetExpectedSize(this->theWeyl.RootSystem.size);
//  IndicesEquationsByRootSpace.Reserve(this->theWeyl.RootSystem.size);
  outputSystemToBeSolved.size=0;
  outputMatrixSystemToBeSolved.init(0, numberVariables);
  //outputSystemToBeSolved.ComputeDebugString();
  for (int i=0; i<rootsInPlay.size; i++)
    for (int j=0; j<rootsInPlay.size; j++)
    { tempRoot= rootsInPlay[i]-rootsInPlay[j];
      if (this->theWeyl.IsARoot(tempRoot))
      { int indexEquation= RootSpacesThatNeedToBeKilled.GetIndex(tempRoot);
        if (indexEquation==-1)
        { RootSpacesThatNeedToBeKilled.AddOnTop(tempRoot);
          indexEquation=outputSystemToBeSolved.size;
//          IndicesEquationsByRootSpace.AddOnTop(indexEquation);
          outputSystemToBeSolved.SetSize(outputSystemToBeSolved.size+1);
          outputSystemToBeSolved.LastObject()->MakeZero(numberVariables);
        }
        tempM.MakeConst((int)numberVariables);
        tempM[i]=1;
        tempM[j+halfNumberVariables]=1;
        Rational tempCoeff= this->GetConstant(rootsInPlay[i], -rootsInPlay[j]);
        outputSystemToBeSolved[indexEquation].AddMonomial(tempM, tempCoeff);
        //outputSystemToBeSolved.ComputeDebugString();
      }
    }
  int oldSize=outputSystemToBeSolved.size;
  outputSystemToBeSolved.SetSize(oldSize+this->theWeyl.CartanSymmetric.NumRows);
  for(int i=oldSize; i<outputSystemToBeSolved.size; i++)
    outputSystemToBeSolved[i].MakeZero(numberVariables);
  //outputSystemToBeSolved.ComputeDebugString();
//  ElementSemisimpleLieAlgebra g1, g2;
  for (int i=0; i<rootsInPlay.size; i++)
  { assert(rootsInPlay.size==halfNumberVariables);
    this->GetConstantOrHElement(rootsInPlay[i], -rootsInPlay[i], tempRat, tempRoot);
    for (int j=0; j<this->theWeyl.CartanSymmetric.NumRows; j++)
    { tempM.MakeConst((int)numberVariables);
      tempM[i]=1;
      tempM[i+halfNumberVariables]=1;
      outputSystemToBeSolved[j+oldSize].AddMonomial(tempM, tempRoot[j]);
    }
  }
  for (int i=0; i<this->theWeyl.CartanSymmetric.NumRows; i++)
    outputSystemToBeSolved.TheObjects[i+oldSize].AddConstant(targetH.TheObjects[i]*(-1));
  outputMatrixSystemToBeSolved.init(outputSystemToBeSolved.size, halfNumberVariables);
  outputSystemColumnVector.init(outputSystemToBeSolved.size, 1);
  outputMatrixSystemToBeSolved.NullifyAll();
  outputSystemColumnVector.NullifyAll();
  for (int i=0; i<outputSystemToBeSolved.size; i++)
    for (int j=0; j<outputSystemToBeSolved[i].size; j++)
    { int lowerIndex=-1; int higherIndex=-1;
      Polynomial<Rational>& currentPoly= outputSystemToBeSolved[i];
      Rational& currentCoeff=currentPoly.theCoeffs[j];
      for (int k=0; k<numberVariables; k++)
        if (currentPoly[j][k]==1)
        { if (k<halfNumberVariables)
            lowerIndex=k;
          else
          { higherIndex=k;
            break;
          }
        }
      if (lowerIndex==-1)
        outputSystemColumnVector.elements[i][0]= currentCoeff*(-1);
      else
        outputMatrixSystemToBeSolved.elements[i][lowerIndex]=currentCoeff* inputFCoeffs.elements[0][higherIndex-halfNumberVariables];
    }
//  outputSystemToBeSolved.ComputeDebugString();
}

bool SltwoSubalgebras::ContainsSl2WithGivenH(Vector<Rational>& theH, int* outputIndex)
{ if (outputIndex!=0)
    *outputIndex=-1;
  ElementSemisimpleLieAlgebra tempH;
  tempH.MakeHgenerator(theH, this->owner, 0);
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].theH==tempH)
    { if (outputIndex!=0)
        *outputIndex=i;
      return true;
    }
  return false;
}

bool SltwoSubalgebras::ContainsSl2WithGivenHCharacteristic(Vector<Rational>& theHCharacteristic, int* outputIndex)
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

Vector<Rational> ElementSemisimpleLieAlgebra::GetCartanPart
()const
{ Vector<Rational> result;
  ChevalleyGenerator tempGen;
  int theRank=this->GetOwner().GetRank();
  int numPosRoots=this->GetOwner().GetNumPosRoots();
  result.MakeZero(theRank);
  for (int i=0; i<theRank; i++)
  { tempGen.MakeGenerator(*this->ownerArray, this->indexOfOwnerAlgebra, i+numPosRoots);
    int currentIndex=this->GetIndex(tempGen);
    if (currentIndex!=-1)
      result[i]+=this->theCoeffs[currentIndex];
  }
  return result;
}

void slTwo::MakeReportPrecomputations(GlobalVariables& theGlobalVariables, SltwoSubalgebras& container, int indexInContainer, int indexMinimalContainingRegularSA, rootSubalgebra& MinimalContainingRegularSubalgebra)
{ int theDimension=this->owner->theWeyl.CartanSymmetric.NumRows;
  this->IndicesContainingRootSAs.size=0;
  Vectors<Rational> tempRoots;
  tempRoots.CopyFromBase(MinimalContainingRegularSubalgebra.SimpleBasisK);
  this->owner->theWeyl.TransformToSimpleBasisGeneratorsWRTh(tempRoots, this->theH.GetCartanPart());
  DynkinDiagramRootSubalgebra theDiagram;
  theDiagram.ComputeDiagramTypeKeepInput(tempRoots, this->owner->theWeyl);
  theDiagram.GetSimpleBasisInBourbakiOrder(tempRoots);
  this->IndicesContainingRootSAs.AddOnTop(indexMinimalContainingRegularSA);
  tempRoots.MakeEiBasis(theDimension);
  this->owner->theWeyl.TransformToSimpleBasisGeneratorsWRTh(tempRoots, this->theH.GetCartanPart());
  DynkinDiagramRootSubalgebra tempDiagram;
  tempDiagram.ComputeDiagramTypeKeepInput(tempRoots, this->owner->theWeyl);
  tempDiagram.GetSimpleBasisInBourbakiOrder(this->preferredAmbientSimpleBasis);
  this->hCharacteristic.SetSize(theDimension);
  for (int i=0; i<theDimension; i++)
    this->hCharacteristic.TheObjects[i]=
    this->owner->theWeyl.RootScalarCartanRoot(this->theH.GetCartanPart(), this->preferredAmbientSimpleBasis[i]);
  //this->hCharacteristic.ComputeDebugString();
//  if (this->theE.NonZeroElements.MaxSize==this->owner->theWeyl.RootSystem.size
//      && this->theF.NonZeroElements.MaxSize==this->owner->theWeyl.RootSystem.size
//      && this->theH.NonZeroElements.MaxSize==this->owner->theWeyl.RootSystem.size)
  this->owner->LieBracket(this->theE, this->theF, this->bufferEbracketF);
//  std:: cout << "[" << this->theE.ToString() << ", " << this->theF.ToString() << "]="
//  << this->bufferEbracketF.ToString();
  this->owner->LieBracket(this->theH, this->theE, this->bufferHbracketE);
  this->owner->LieBracket(this->theH, this->theF, this->bufferHbracketF);

  //theSl2.hCharacteristic.ComputeDebugString();
//  this->ComputeModuleDecomposition();
}

void WeylGroup::PerturbWeightToRegularWRTrootSystem(const Vector<Rational>& inputH, Vector<Rational>& output)
{ output=(inputH);
  int indexFirstNonRegular;
  while(!this->IsRegular(output, &indexFirstNonRegular))
  { Vector<Rational>& theBadRoot= this->RootSystem.TheObjects[indexFirstNonRegular];
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

bool WeylGroup::IsRegular(Vector<Rational>& input, int* indexFirstPerpendicularRoot)
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
    output*=WeylGroup::GetSizeWeylByFormula(this->DynkinTypeStrings.TheObjects[i].at(0), this->SimpleBasesConnectedComponents.TheObjects[i].size);
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

void DynkinDiagramRootSubalgebra::GetSimpleBasisInBourbakiOrder(Vectors<Rational>& output)
{ output.size=0;
  output.ReservE(this->RankTotal());
  for (int i=0; i<this->SimpleBasesConnectedComponents.size; i++)
    this->GetSimpleBasisInBourbakiOrderOneComponentAppend(output, i);
}

void DynkinDiagramRootSubalgebra::GetSimpleBasisInBourbakiOrderOneComponentAppend(Vectors<Rational>& outputAppend, int index)
{ std::string& theString= this->DynkinTypeStrings.TheObjects[index];
  // the order implemented here I took from the atlas of lie groups (http://www.liegroups.org/dissemination/spherical/explorer/rootSystem.cgi)
  // which should be the Bourbaki order. The order is as follows:
  // type A all Vectors<Rational> are from left to right (or the other way round, whichever is your orientation)
  // in type B first comes the long Vectors<Rational> in the order they appear in the diagram;
  // in types C,
  // Types F and G there were some recent changes due to different order choice in different parts of the code.
  // The info for F and G needs to be updated!
  // in type D first comes the long string, with the end node with lowest index;
  // then come the two end one-Vector<Rational> strings in any order.
  // in type E the order is as below
  //  2
  //13456(78)
  //(2 is connected to 4)
  //The format of this function is in accordance with WeylGroup::GetEpsilonMatrix
  assert(theString.size()>0);
  if (theString.at(0)=='A'|| theString.at(0)=='B' || theString.at(0)=='G')
    outputAppend.AddListOnTop(this->SimpleBasesConnectedComponents.TheObjects[index]);
  if (theString.at(0)=='C' || theString.at(0)=='F')
    for (int i=this->SimpleBasesConnectedComponents.TheObjects[index].size-1; i>=0; i--)
      outputAppend.AddOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[i]);
  if (theString.at(0)=='D')
  { int componentRank=this->SimpleBasesConnectedComponents.TheObjects[index].size;
    for (int i=componentRank-3; i>=0; i--)
      outputAppend.AddOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[i]);
    outputAppend.AddOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[componentRank-2]);
    outputAppend.AddOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[componentRank-1]);
  }
  if (theString.at(0)=='E')
  { int componentRank=this->SimpleBasesConnectedComponents.TheObjects[index].size;
    outputAppend.AddOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[componentRank-2]);
    outputAppend.AddOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[componentRank-1]);
    outputAppend.AddOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[componentRank-3]);
    for (int i=0; i<componentRank-3; i++)
      outputAppend.AddOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[i]);
  }
}

void DynkinDiagramRootSubalgebra::GetKillingFormMatrixUseBourbakiOrder(Matrix<Rational> & output, WeylGroup& theWeyl)
{ Vectors<Rational> tempRoots;
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
{ this->MultiplicitiesDecompositionMinimalContainingRootSA.SetSize(this->IndicesMinimalContainingRootSA.size);
  this->HighestWeightsDecompositionMinimalContainingRootSA.SetSize(this->IndicesMinimalContainingRootSA.size);
  List<int> buffer;
  for (int i=0; i<this->IndicesMinimalContainingRootSA.size; i++)
  { rootSubalgebra& theSA= owner.theRootSAs.TheObjects[this->IndicesMinimalContainingRootSA.TheObjects[i]];
    this->ComputeModuleDecomposition(theSA.PositiveRootsK, theSA.SimpleBasisK.size, this->HighestWeightsDecompositionMinimalContainingRootSA.TheObjects[i], this->MultiplicitiesDecompositionMinimalContainingRootSA.TheObjects[i], buffer, theGlobalVariables);
  }
}

//The below code is related to sl(2) subalgebras of simple Lie algebras
void slTwo::ComputeModuleDecomposition
(Vectors<Rational>& positiveRootsContainingRegularSA, int dimensionContainingRegularSA,
 List<int>& outputHighestWeights, List<int>& outputMultiplicitiesHighestWeights,
 List<int>& outputWeightSpaceDimensions, GlobalVariables& theGlobalVariables)
{ int IndexZeroWeight=positiveRootsContainingRegularSA.size*2;
  outputWeightSpaceDimensions.initFillInObject(4*positiveRootsContainingRegularSA.size+1, 0);
  outputWeightSpaceDimensions.TheObjects[IndexZeroWeight]=dimensionContainingRegularSA;
  List<int> BufferHighestWeights;
  bool possible=true;
  Rational tempRat;
  Vectors<Rational> tempRoots, tempRoots2;
  Matrix<Rational> tempMat;
  positiveRootsContainingRegularSA.GetCoordsInBasis(this->preferredAmbientSimpleBasis, tempRoots, tempRoots2, tempMat);
  for (int k=0; k<positiveRootsContainingRegularSA.size; k++)
  { tempRat=Vector<Rational>::ScalarEuclidean(this->hCharacteristic, tempRoots[k]);
    assert(tempRat.DenShort==1);
    if (tempRat.NumShort>positiveRootsContainingRegularSA.size*2)
    { possible=false;
      break;
    }
    outputWeightSpaceDimensions.TheObjects[IndexZeroWeight+tempRat.NumShort]++;
    outputWeightSpaceDimensions.TheObjects[IndexZeroWeight-tempRat.NumShort]++;
  }
  BufferHighestWeights.CopyFromBase(outputWeightSpaceDimensions);
  outputHighestWeights.ReservE(positiveRootsContainingRegularSA.size*2);
  outputMultiplicitiesHighestWeights.ReservE(positiveRootsContainingRegularSA.size*2);
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
    { outputHighestWeights.AddOnTop(j-IndexZeroWeight);
      outputMultiplicitiesHighestWeights.AddOnTop(topMult);
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
      out << "\n<br>";
    out << "\n";
  }
  output=out.str();
}

void SltwoSubalgebras::ElementToStringNoGenerators(std::string& output, GlobalVariables& theGlobalVariables, WeylGroup& theWeyl, bool useLatex, bool useHtml, bool usePNG, std::string* physicalPath, std::string* htmlPathServer)
{ std::string tempS; std::stringstream out;
  std::string tooltipHchar="Let h be in the Cartan s.a. Let \\alpha_1, ..., \\alpha_n be simple Vectors<Rational> w.r.t. h. Then the h-characteristic is the n-tuple (\\alpha_1(h), ..., \\alpha_n(h))";
  std::string tooltipVDecomposition= "The sl(2) submodules of g are parametrized by their highest weight w.r.t. h. V_l is l+1 dimensional";
  std::string tooltipContainingRegular="A regular semisimple subalgebra might contain an sl(2) such that the sl(2) has no centralizer in the regular semisimple subalgebra, but the regular semisimple subalgebra might fail to be minimal containing. This happens when another minimal containing regular semisimple subalgebra of equal rank nests as a Vector<Rational> subalgebra in the containing SA. See Dynkin, Semisimple Lie subalgebras of semisimple Lie algebras, remark before Theorem 10.4.";
  std::string tooltipHvalue="The actual realization of h. The coordinates of h are given with respect to the fixed original simple basis. Note that the characteristic of h is given *with respect to another basis* (namely, with respect to an h-positive simple basis). I will fix this in the future (email me if you want that done sooner).";
  out << "Number of sl(2) subalgebras "<< this->size<<"\n";
  if (this->IndicesSl2decompositionFlas.size < this->size)
    usePNG = false;
  if(useHtml)
    out << "<br><br><table><tr><td style=\"padding-right:20px\">" << CGI::ElementToStringTooltip("Characteristic", tooltipHchar)  << "</td><td align=\"center\" title=\"" << tooltipHvalue << "\"> h</td><td style=\"padding-left:20px\" title=\"" << tooltipVDecomposition << "\"> Decomposition of ambient Lie algebra</td> <td>Minimal containing regular semisimple SAs</td><td title=\"" << tooltipContainingRegular << "\">Containing regular semisimple SAs in which the sl(2) has no centralizer</td> </tr>";
  if (this->BadHCharacteristics.size>0)
  { if (useHtml)
      out << "<tr><td>Bad values of h</td><td>";
    tempS= this->BadHCharacteristics.ToString();
    out << tempS;
    if (useHtml)
      out << "</td></tr>";
  }
  for (int i=0; i<this->size; i++)
  { slTwo& theSl2= this->TheObjects[i];
    if (useHtml)
      out << "<tr><td style=\"padding-right:20px\"><a href=\"./sl2s.html#sl2index" << i << "\"title=\"" << tooltipHchar << "\" >";
    out << theSl2.hCharacteristic.ToString();
    if (useHtml)
      out << "</a></td><td title=\"" << tooltipHvalue << "\">";
    out << theSl2.theH.GetCartanPart().ToString();
    if (useHtml)
      out << "</td><td style=\"padding-left:20px\" title=\"" << tooltipVDecomposition << "\">";
    if (useHtml && usePNG)
      out << "<img src=\"./fla" << this->IndicesSl2decompositionFlas.TheObjects[i]+1 << ".png\"></td><td>";
    else
    { theSl2.ElementToStringModuleDecomposition(useLatex, useHtml, tempS);
      out << tempS;
      if (useHtml)
        out << "</td><td>";
    }
    for (int j=0; j<theSl2.IndicesMinimalContainingRootSA.size; j++)
    { rootSubalgebra& currentSA= this->theRootSAs.TheObjects[theSl2.IndicesMinimalContainingRootSA.TheObjects[j]];
      CGI::clearDollarSigns(currentSA.theDynkinDiagram.DynkinStrinG, tempS);
      out << "<a href=\"../rootHtml_rootSA" << theSl2.IndicesMinimalContainingRootSA.TheObjects[j] << ".html\">" << tempS << "</a>" << ";  ";
    }
    if (useHtml)
      out << "</td><td title=\"" << tooltipContainingRegular << "\">";
    for (int j=0; j<theSl2.IndicesContainingRootSAs.size; j++)
    { rootSubalgebra& currentSA= this->theRootSAs.TheObjects[theSl2.IndicesContainingRootSAs.TheObjects[j]];
      CGI::clearDollarSigns(currentSA.theDynkinDiagram.DynkinStrinG, tempS);
      out << "<a href=\"../rootHtml_rootSA" << theSl2.IndicesContainingRootSAs.TheObjects[j] << ".html\">" << tempS << "</a>" << ";  ";
    }
    if (useHtml)
      out <<"</td></tr>\n";
  }
  if (useHtml)
    out << "</table><HR width=\"100%\">";
  output= out.str();
}

void SltwoSubalgebras::ToString(std::string& output, GlobalVariables& theGlobalVariables, WeylGroup& theWeyl, bool useLatex, bool useHtml, bool usePNG, std::string* physicalPath, std::string* htmlPathServer)
{ std::string tempS; std::stringstream out; std::stringstream body;
  FormatExpressions PolyFormatLocal;
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].ToString(tempS, theGlobalVariables, *this, i, useLatex, useHtml, usePNG, physicalPath, htmlPathServer, PolyFormatLocal);
  //  body<< "Index "<< i<<": ";
    if(useHtml)
      body << "<br>";
    body << tempS;
    if(useHtml)
      body << "<HR width=\"100%\">";
  }
  if(useHtml)
    out << "<br>";
  this->ElementToStringNoGenerators(tempS, theGlobalVariables, theWeyl, useLatex, useHtml, usePNG, physicalPath, htmlPathServer);
  out << tempS;
  tempS = body.str();
  out << tempS;
  output = out.str();
  return;
}

void SltwoSubalgebras::ElementToHtml
(GlobalVariables& theGlobalVariables, WeylGroup& theWeyl, bool usePNG, std::string& physicalPath, std::string& htmlPathServer,
 std::string& DisplayNameCalculator)
{ std::string physicalPathSAs;
  std::string htmlPathServerSAs;
  physicalPathSAs= physicalPath;
  physicalPathSAs.append("../");
  htmlPathServerSAs= htmlPathServer;
  htmlPathServerSAs.append("../");
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1= "Preparing html pages for sl(2) subalgebras. This might take a while.";
  theGlobalVariables.MakeReport();
  std::string tempS;
  this->theRootSAs.ElementToHtml(tempS, physicalPathSAs, htmlPathServerSAs, this, theGlobalVariables);
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
  std::stringstream out, outNotation;
  std::string fileName;
  std::fstream theFile, fileFlas;
  outNotation << "<a href=\"" << htmlPathServer << "StructureConstants.html\">Notation, structure constants and Weyl group info</a>"
  << "<br> <a href=\""<< DisplayNameCalculator << "\"> Calculator main page</a><br><a href=\"../rootHtml.html\">Root subsystem table</a><br>";
  std::string notation= outNotation.str();
  this->ToString(tempS, theGlobalVariables, theWeyl, false, true, usePNG, &physicalPath, &htmlPathServer);
  out << tempS;
  if(usePNG)
  { fileName= physicalPath;
    fileName.append("sl2s.html");
    CGI::OpenFileCreateIfNotPresent(theFile, fileName, false, true, false);
    tempS= out.str();
    theFile << "<HMTL>"
    << "<title>sl(2)-subalgebras of "
    << this->theRootSAs[0].theDynkinDiagram.ElementToStrinG(false, true) <<  "</title>";
    theFile << "<meta name=\"keywords\" content=\""
    <<  this->theRootSAs[0].theDynkinDiagram.ElementToStrinG(false, true)
    << " sl(2)-triples, sl(2)-subalgebras, nilpotent orbits simple Lie algebras,"
    << " nilpotent orbits of "
    <<  this->theRootSAs[0].theDynkinDiagram.ElementToStrinG(false, true)
    << ", sl(2)-triples of "
    << this->theRootSAs[0].theDynkinDiagram.ElementToStrinG(false, true)
    << " \">";
    theFile << "<BODY>" << notation << "<a href=\"" << htmlPathServer << "sl2s_nopng.html\"> plain html for your copy+paste convenience</a><br>\n" <<tempS<<"</HTML></BODY>";
    theFile.close();
  }
  fileName= physicalPath;
  fileName.append("sl2s_nopng.html");
  this->ToString(tempS, theGlobalVariables, theWeyl, false, true, false, &physicalPath, &htmlPathServer);
  CGI::OpenFileCreateIfNotPresent(theFile, fileName, false, true, false);
  theFile << "<HMTL><BODY>" << notation << "<a href=\"" << htmlPathServer << "sl2s.html\"> " << ".png rich html for your viewing pleasure</a><br>\n" << tempS << "</HTML></BODY>";
  theFile.close();
  fileName= physicalPath;
  fileName.append("StructureConstants.html");
  CGI::OpenFileCreateIfNotPresent(theFile, fileName, false, true, false);
  this->owner[0].ToString(&theGlobalVariables.theDefaultFormat);
  theFile << tempS;
  theFile.close();
  if (usePNG)
  { this->listSystemCommandsLatex.SetSize(this->texFileNamesForPNG.size);
    this->listSystemCommandsDVIPNG.SetSize(this->texFileNamesForPNG.size);
    for (int i=0; i<this->texFileNamesForPNG.size; i++)
    { CGI::OpenFileCreateIfNotPresent(fileFlas, this->texFileNamesForPNG.TheObjects[i], false, true, false);
      fileFlas << "\\documentclass{article}\\begin{document}\\pagestyle{empty}\n" << this->texStringsEachFile.TheObjects[i] << "\n\\end{document}";
      std::stringstream tempStreamLatex, tempStreamPNG;
      tempStreamLatex << "latex " << " -output-directory=" << physicalPath << " " << this->texFileNamesForPNG.TheObjects[i];
      tempS= this->texFileNamesForPNG.TheObjects[i];
      tempS.resize(tempS.size()-4);
      tempStreamPNG << "dvipng " << tempS << ".dvi -o " << tempS << ".png -T tight";
      this->listSystemCommandsLatex.TheObjects[i]= tempStreamLatex.str();
      this->listSystemCommandsDVIPNG.TheObjects[i]=tempStreamPNG.str();
      fileFlas.close();
    }
  }
}

void rootSubalgebras::ElementToStringCentralizerIsomorphisms(std::string& output, bool useLatex, bool useHtml, int fromIndex, int NumToProcess, GlobalVariables& theGlobalVariables)
{ std::stringstream out; std::string tempS;
  //W'' stands for the graph isomorphisms of C(k_ss) extending to Vector<Rational> system isomorphisms of the entire algebra.
  for (int i=fromIndex; i<NumToProcess; i++)
    this->GenerateKintersectBOuterIsos(this->TheObjects[i], theGlobalVariables);
  if (useLatex)
    out << "\\begin{tabular}{ccccc}$\\mathfrak{k}_{ss}$& $C(k_{ss})_{ss}$ & $\\#W''$ &$\\#W'''$&$\\#(W'''\\rtimes W'')$\\\\\\hline";
  if (useHtml)
    out << "<br><table><tr><td>k_{ss}</td><td></td><td>Weyl group of C(k_{ss})_{ss}</td><td>Outer automorphisms of C(k_{ss})_{ss}<td></tr>";
  Vectors<Rational> emptyRoots;
  emptyRoots.size=0;
  for (int i=fromIndex; i<NumToProcess; i++)
  { rootSubalgebra& current= this->TheObjects[i];
    ReflectionSubgroupWeylGroup& theOuterIsos= this->CentralizerOuterIsomorphisms.TheObjects[i];
    theOuterIsos.ComputeSubGroupFromGeneratingReflections(emptyRoots, theOuterIsos.ExternalAutomorphisms, theGlobalVariables, 0, true);
    Rational numInnerIsos = current.theCentralizerDiagram.GetSizeCorrespondingWeylGroupByFormula();
    if (useHtml)
      out << "<td>";
    current.theDynkinDiagram.ElementToStrinG(tempS, useLatex, true);
    out << tempS;
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & ";
    current.theCentralizerDiagram.ElementToStrinG(tempS, useLatex, true);
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
    out << numInnerIsos.ToString();
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & ";
    out << (numInnerIsos*theOuterIsos.size).ToString();
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

bool IsRegularWRT(Vectors<Rational>& input, Vector<Rational>& h1, Vector<Rational>& h2, WeylGroup& theWeyl)
{ for (int i=0; i<input.size; i++)
    if (theWeyl.RootScalarCartanRoot(h1, input.TheObjects[i]).IsEqualToZero() && theWeyl.RootScalarCartanRoot(h2, input.TheObjects[i]).IsEqualToZero())
      return false;
  return true;
}

void reductiveSubalgebras::FindTheReductiveSubalgebras(char WeylLetter, int WeylIndex, GlobalVariables& theGlobalVariables)
{ //this->theSl2s.owner.FindSl2Subalgebras(this->theSl2s, WeylLetter, WeylIndex, theGlobalVariables);
  this->theSl2s.owner[0].FindSl2Subalgebras(this->theSl2s, WeylLetter, WeylIndex, theGlobalVariables);
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
  this->theSl2s.ToString(tempS, theGlobalVariables, this->theSl2s.theRootSAs.AmbientWeyl, false, true, false, 0, 0);
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
      SemisimpleLieAlgebra& theSSLieAlgebra= theCurrentAlgebrasSl2s.owner[0];
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
  this->CandidatesPrincipalSl2ofSubalgebra.ReservE(this->theLetters.size);
  this->theCandidateSubAlgebras.SetSize(this->theLetters.size);
  for (int i=0; i<this->theLetters.size; i++)
  { this->theCandidateSubAlgebras.TheObjects[i].theRootSAs.AmbientWeyl.MakeFromDynkinType(this->theLetters.TheObjects[i], this->theRanks.TheObjects[i], &this->theMultiplicities.TheObjects[i]);
    this->theCandidateSubAlgebras.TheObjects[i].theRootSAs.AmbientWeyl.GenerateRootSystemFromKillingFormMatrix();
    this->theCandidateSubAlgebras.TheObjects[i].owner[0].theWeyl=(this->theCandidateSubAlgebras.TheObjects[i].theRootSAs.AmbientWeyl);
    int theDimension = this->theCandidateSubAlgebras.TheObjects[i].theRootSAs.AmbientWeyl.CartanSymmetric.NumRows;
    tempSl2.hCharacteristic.initFillInObject(theDimension, 2);
    tempSl2.preferredAmbientSimpleBasis.MakeEiBasis(theDimension);
    tempSl2.owner = &this->theCandidateSubAlgebras.TheObjects[i].owner[0];
    tempSl2.ComputeModuleDecompositionAmbientLieAlgebra(theGlobalVariables);
    this->CandidatesPrincipalSl2ofSubalgebra.AddOnTop(tempSl2);
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
  { this->theLetters.AddOnTop(lettersBuffer);
    this->theMultiplicities.AddOnTop(multiplicitiesBuffer);
    this->theRanks.AddOnTop(ranksBuffer);
    return;
  }
  Selection DistributionBetweenTheFourLetters;
  int theMult = partitionMults.TheObjects[indexInPartition];
  int theRank = partitionValues.TheObjects[indexInPartition];
  int numLetters;
  List<char> lettersAvailable; lettersAvailable.SetSize(5);
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
      { ranksBuffer.AddOnTop(theRank);
        lettersBuffer.AddOnTop(lettersAvailable.TheObjects[k]);
        multiplicitiesBuffer.AddOnTop(numIsotypic);
      }
    }
    this->GenerateAllDiagramsForPartitionRecursive(indexPartition, indexInPartition+1, ranksBuffer, multiplicitiesBuffer, lettersBuffer);
    ranksBuffer.size=oldsize;
    multiplicitiesBuffer.size=oldsize;
    lettersBuffer.size=oldsize;
  }
}

void reductiveSubalgebras::MatchRealSl2sToPartitionSl2s()
{ this->IndicesMatchingSl2s.SetSize(this->theLetters.size);
  for (int i=0; i<this->CandidatesPrincipalSl2ofSubalgebra.size; i++)
  { this->IndicesMatchingSl2s.TheObjects[i].size=0;
    for (int j=0; j<this->theSl2s.size; j++)
      if (this->CandidatesPrincipalSl2ofSubalgebra.TheObjects[i].ModuleDecompositionFitsInto(this->theSl2s.TheObjects[j]))
        this->IndicesMatchingSl2s.TheObjects[i].AddOnTop(j);
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
        { this->IndicesMatchingActualSl2s.TheObjects[k].AddOnTop(i);
          this->IndicesMatchingPartitionSl2s.TheObjects[i].AddOnTop(k);
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
  { this->thePartitionMultiplicities.AddOnTop(Multiplicities);
    this->thePartitionValues.AddOnTop(Values);
    return;
  }
  int possibleMults=(remainingToBePartitioned/CurrentValue)+1;
  for(int i=0; i<possibleMults; i++)
  { if (i>0)
    { Multiplicities.AddOnTop(i);
      Values.AddOnTop(CurrentValue);
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
  this->thePartitionMultiplicities.ReservE(upperLimit);
  this->thePartitionValues.ReservE(upperLimit);
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
    out << theLetter << "_" << theRank << " ";
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
        out << this->theSl2s.TheObjects[tempI].hCharacteristic.ToString() << ", ";
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
        out << "<br>\n";
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
      this->CartanSymmetric.DirectSumWith(tempW.CartanSymmetric, (Rational) 0);
  }
}

void DrawOperations::drawLineBetweenTwoVectorsBuffer(const Vector<Rational>& vector1, const Vector<Rational>& vector2, unsigned long thePenStyle, int ColorIndex)
{ this->TypeNthDrawOperation.AddOnTop(this->typeDrawLineBetweenTwoVectors);
  this->IndexNthDrawOperation.AddOnTop(this->theDrawLineBetweenTwoRootsOperations.size);
  this->theDrawLineBetweenTwoRootsOperations.AddObjectOnTopCreateNew();
  this->theDrawLineBetweenTwoRootsOperations.LastObject()->init(vector1, vector2, thePenStyle, ColorIndex);
}

void DrawOperations::drawCircleAtVectorBuffer
(const Vector<Rational>& input, double radius, unsigned long thePenStyle, int theColor)
{ this->TypeNthDrawOperation.AddOnTop(this->typeDrawCircleAtVector);
  this->IndexNthDrawOperation.AddOnTop(this->theDrawCircleAtVectorOperations.size);
  this->theDrawCircleAtVectorOperations.AddObjectOnTopCreateNew();
  this->theDrawCircleAtVectorOperations.LastObject()->init(input, radius, thePenStyle, theColor);
}

void DrawOperations::drawTextAtVectorBuffer(const Vector<Rational>& input, const std::string& inputText, int ColorIndex, int theFontSize, int theTextStyle)
{ this->TypeNthDrawOperation.AddOnTop(this->typeDrawTextAtVector);
  this->IndexNthDrawOperation.AddOnTop(this->theDrawTextAtVectorOperations.size);
  this->theDrawTextAtVectorOperations.AddObjectOnTopCreateNew();
  this->theDrawTextAtVectorOperations.LastObject()->init(input, inputText, ColorIndex, theFontSize, theTextStyle);
}

void DrawOperations::drawLineBuffer(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex)
{ this->TypeNthDrawOperation.AddOnTop(this->typeDrawLine);
  this->IndexNthDrawOperation.AddOnTop(this->theDrawLineOperations.size);
  this->theDrawLineOperations.AddObjectOnTopCreateNew();
  this->theDrawLineOperations.LastObject()->init(X1, Y1, X2, Y2, thePenStyle, ColorIndex);
}

void DrawOperations::drawTextBuffer(double X1, double Y1, const std::string& inputText, int ColorIndex, int theFontSize, int theTextStyle)
{ this->TypeNthDrawOperation.AddOnTop(this->typeDrawText);
  this->IndexNthDrawOperation.AddOnTop(this->theDrawTextOperations.size);
  this->theDrawTextOperations.AddObjectOnTopCreateNew();
  this->theDrawTextOperations.LastObject()->init(X1, Y1, inputText, ColorIndex, theFontSize, theTextStyle);
}

void DrawingVariables::drawBufferNoIniT
(DrawOperations& theOps)
{ this->LockedWhileDrawing.LockMe();
  theOps.EnsureProperInitialization();
  theOps.ComputeProjectionsEiVectors();
  theOps.ComputeXYsFromProjectionsEisAndGraphicsUnit();
  int currentPenStyle, currentTextStyle;
  if (this->theDrawClearScreenFunction!=0)
    this->theDrawClearScreenFunction();
  int numOps=theOps.IndexNthDrawOperation.size;
  //the bad debugline is here:
  //numOps=(numOps>10)? 10: numOps;
  for (int i=0; i<numOps; i++)
    switch (theOps.TypeNthDrawOperation.TheObjects[i])
    { case DrawOperations::typeDrawText:
        if (this->theDrawTextFunction!=0)
        { DrawTextOperation& theDrawTextOp= theOps.theDrawTextOperations.TheObjects[theOps.IndexNthDrawOperation.TheObjects[i]];
          currentTextStyle=this->GetActualTextStyleFromFlagsAnd(theDrawTextOp.TextStyle);
          if (currentTextStyle==this->TextStyleInvisible)
            break;
          this->theDrawTextFunction(theDrawTextOp.X1, theDrawTextOp.Y1, theDrawTextOp.theText.c_str(), theDrawTextOp.theText.size(), theDrawTextOp.ColorIndex, theDrawTextOp.fontSize);
        }
        break;
      case DrawOperations::typeDrawLine:
        if (this->theDrawLineFunction!=0)
        { DrawLineOperation& theDrawLineOp= theOps.theDrawLineOperations.TheObjects[theOps.IndexNthDrawOperation.TheObjects[i]];
          currentPenStyle= this->GetActualPenStyleFromFlagsAnd(theDrawLineOp.thePenStyle);
          if (currentPenStyle==this->PenStyleInvisible)
            break;
          this->theDrawLineFunction(theDrawLineOp.X1, theDrawLineOp.Y1, theDrawLineOp.X2, theDrawLineOp.Y2, currentPenStyle, theDrawLineOp.ColorIndex);
        }
        break;
      case DrawOperations::typeDrawLineBetweenTwoVectors:
        if (this->theDrawLineFunction!=0)
        { DrawLineBetweenTwoRootsOperation& theDrawLineBnTwoOp= theOps.theDrawLineBetweenTwoRootsOperations.TheObjects[theOps.IndexNthDrawOperation.TheObjects[i]];
          currentPenStyle= this->GetActualPenStyleFromFlagsAnd(theDrawLineBnTwoOp.thePenStyle);
          if (currentPenStyle==this->PenStyleInvisible)
            break;
          this->theDrawLineFunction(theDrawLineBnTwoOp.precomputedX1, theDrawLineBnTwoOp.precomputedY1, theDrawLineBnTwoOp.precomputedX2, theDrawLineBnTwoOp.precomputedY2, currentPenStyle, theDrawLineBnTwoOp.ColorIndex);
        }
        break;
      case DrawOperations::typeDrawTextAtVector:
        if (this->theDrawTextFunction!=0)
        { DrawTextAtVectorOperation& theDrawTextOp= theOps.theDrawTextAtVectorOperations.TheObjects[theOps.IndexNthDrawOperation.TheObjects[i]];
          currentTextStyle= this->GetActualTextStyleFromFlagsAnd(theDrawTextOp.TextStyle);
          if (currentTextStyle==this->TextStyleInvisible)
            break;
          this->theDrawTextFunction(theDrawTextOp.precomputedX, theDrawTextOp.precomputedY, theDrawTextOp.theText.c_str(), theDrawTextOp.theText.size(), theDrawTextOp.ColorIndex, theDrawTextOp.fontSize);
        }
        break;
      case DrawOperations::typeDrawCircleAtVector:
        if (this->theDrawCircleFunction!=0)
        { DrawCircleAtVectorOperation& theDrawCircleOp= theOps.theDrawCircleAtVectorOperations.TheObjects[theOps.IndexNthDrawOperation.TheObjects[i]];
          currentPenStyle= this->GetActualPenStyleFromFlagsAnd(theDrawCircleOp.thePenStyle);
          if (currentPenStyle==this->PenStyleInvisible)
            break;
          this->theDrawCircleFunction(theDrawCircleOp.precomputedX, theDrawCircleOp.precomputedY, theDrawCircleOp.radius, theDrawCircleOp.thePenStyle, theDrawCircleOp.ColorIndex);
        }
        break;
      default: break;
    }
  this->LockedWhileDrawing.UnlockMe();
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
  tempS=this->ToString();
  theGlobalVariables.theDrawingVariables.theBuffer.drawTextBuffer(theDrawData.TopLeftCornerX, theDrawData.TopLeftCornerY, tempS, 0, theGlobalVariables.theDrawingVariables.fontSizeNormal, theGlobalVariables.theDrawingVariables.TextStyleNormal);
  theDrawData.outputHeight=10;
  theDrawData.outputWidth=10*tempS.size();
}

bool rootSubalgebras::ReadFromDefaultFileNilradicalGeneration(GlobalVariables* theGlobalVariables)
{ std::fstream theFile;
  if (CGI::OpenFileCreateIfNotPresent(theFile, "./theNilradicalsGenerator.txt", false, false, false))
  { theFile.seekg(0);
    this->ReadFromFileNilradicalGeneration(theFile, theGlobalVariables);
    return true;
  }
  return false;
}

void rootSubalgebras::WriteToDefaultFileNilradicalGeneration(GlobalVariables* theGlobalVariables)
{ std::fstream theFile;
  CGI::OpenFileCreateIfNotPresent(theFile, "./theNilradicalsGenerator.txt", false, true, false);
  this->WriteToFileNilradicalGeneration(theFile, theGlobalVariables);
}

void rootSubalgebras::WriteToFileNilradicalGeneration(std::fstream& output, GlobalVariables* theGlobalVariables)
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

void rootSubalgebras::ReadFromFileNilradicalGeneration(std::fstream& input, GlobalVariables* theGlobalVariables)
{ std::string tempS; int tempI;
  this->AmbientWeyl.ReadFromFile(input);
  this->AmbientWeyl.ComputeRho(true);
  input >> tempS >> tempI;
  assert(tempS=="Number_subalgebras:");
  this->SetSize(tempI);
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

void rootSubalgebra::WriteToFileNilradicalGeneration(std::fstream& output, GlobalVariables* theGlobalVariables, rootSubalgebras& owner)
{ output << "Simple_basis_k: ";
  this->SimpleBasisK.WriteToFile(output, theGlobalVariables);
}

void rootSubalgebra::ReadFromFileNilradicalGeneration(std::fstream& input, GlobalVariables* theGlobalVariables, rootSubalgebras& owner)
{ std::string tempS;
  input >> tempS;
  assert(tempS=="Simple_basis_k:");
  this->SimpleBasisK.ReadFromFile(input, theGlobalVariables);
  this->genK.CopyFromBase(this->SimpleBasisK);
  this->AmbientWeyl=(owner.AmbientWeyl);
  this->ComputeAll();
}

void rootSubalgebra::GeneratePossibleNilradicalsInit(List<Selection>& impliedSelections, int& parabolicsCounter)
{ impliedSelections.SetSize(this->kModules.size+1);
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
  List<Selection> StartingNilradicalsNoRepetition;
  StartingNilradicalsNoRepetition.ReservE(numCycles);
  Selection tempSel, ParabolicsGenerator;
  if (!owner.flagNilradicalComputationInitialized)
    owner.CountersNilradicalsGeneration.SetSize(this->kModules.size+1);
  if (owner.flagStoringNilradicals)
    owner.storedNilradicals.TheObjects[indexInOwner].size=0;
  Vectors<Rational> tempRootsTest;
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
      //StartingNilradicalsNoRepetition.AddOnTop(tempSel);
    }

/*      tempRootsTest.size=0;
      std::string tempS; std::stringstream out;
      for (int s=0; s<impliedSelections.TheObjects[0].CardinalitySelection; s++)
        tempRootsTest.AddOnTop(this->kModules.TheObjects[impliedSelections.TheObjects[0].elements[s]].TheObjects[0]);
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

void coneRelations::WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)
{ output << "num_rels: " << this->size << "\n";
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].WriteToFile(output, theGlobalVariables);
}

void coneRelations::ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables, rootSubalgebras& owner)
{ std::string tempS; int tempI;
  this->Clear();
  input >> tempS >> tempI;
  coneRelation tempRel;
  for (int i=0; i<tempI; i++)
  { tempRel.ReadFromFile(input, theGlobalVariables, owner);
    this->AddRelationNoRepetition(tempRel, owner, tempRel.IndexOwnerRootSubalgebra);
  }
}

void coneRelation::WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)
{ this->AlphaCoeffs.WriteToFile(output);
  this->Alphas.WriteToFile(output, theGlobalVariables);
  output << this->AlphaKComponents;
  this->BetaCoeffs.WriteToFile(output);
  this->Betas.WriteToFile(output, theGlobalVariables);
  output << this->BetaKComponents;
  output << "Index_owner_root_SA: " << this->IndexOwnerRootSubalgebra << " ";
}

void coneRelation::ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables, rootSubalgebras& owner)
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

void rootSubalgebras::ElementToStringConeConditionNotSatisfying(std::string& output, bool includeMatrixForm, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> tempRoots, tempRoots2;
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
      { tempRoots.AddOnTop(currentRootSA.PositiveRootsK.TheObjects[j]);
        tempRoots.AddOnTop(-currentRootSA.PositiveRootsK.TheObjects[j]);
      }
      if (includeMatrixForm)
        out << "\n\n\\noindent\\rule{\\textwidth}{1.5pt}\n\n";
      else
        out << "\\hline\\begin{tabular}{r}";
      out << "$\\Delta(\\mathfrak{k})$ is of type " << currentRootSA.theDynkinDiagram.DynkinStrinG << "; ";
      if (!includeMatrixForm)
        out <<"\\\\";
      currentRootSA.AmbientWeyl.GetEpsilonCoords(currentRootSA.PositiveRootsK, tempRoots2);
      tempS=tempRoots2.ElementToStringEpsilonForm(true, false, false);
      out << " $\\Delta^+(\\mathfrak{k})=$ " << tempS;
      if (includeMatrixForm)
        out << "\n\n\\noindent\\rule{\\textwidth}{0.3pt}\n\n";
      else
        out << "\\end{tabular} &\n\\begin{tabular}{l}";
      int numNonReductiveCurrent=0;
      for (int j=0; j<this->storedNilradicals.TheObjects[i].size; j++)
      { List<int>& currentNilrad= this->storedNilradicals[i].TheObjects[j];
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
              out << "\\\\";
          }
        }
      }
      if (!includeMatrixForm)
        out << "\\end{tabular}\n\n\\\\";
    }
  if (!includeMatrixForm)
    out << "\n\\end{longtable}";
  out2 << "\n\nThe number of non-conjugate non-solvable non-reductive Vector<Rational> subalgebras not satisfying the cone condition is: " << numNonSolvableNonReductive << "\n\n";
  tempS = out.str();
  out2 << tempS;
  output=out2.str();
}

void rootSubalgebras::ElementToStringRootSpaces(std::string& output, bool includeMatrixForm, Vectors<Rational>& input, GlobalVariables& theGlobalVariables)
{ std::string tempS; std::stringstream out;
  Vectors<Rational> epsCoords;
  Matrix<int> tempMat;
  int theDimension=this->AmbientWeyl.CartanSymmetric.NumRows;
  if (this->AmbientWeyl.WeylLetter=='B')
  { this->AmbientWeyl.GetEpsilonCoords(input, epsCoords);
    tempMat.MakeIdMatrix(theDimension*2+1, 1, 0);
    tempMat.elements[theDimension][theDimension]=0;
    for (int i=0; i<epsCoords.size; i++)
    { bool isShort=false;
      int firstIndex=-1;
      int secondIndex=-1;
      bool firstSignIsPositive=true;
      bool secondSignIsPositive=true;
      Vector<Rational>& currentRoot=epsCoords.TheObjects[i];
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
  { this->AmbientWeyl.GetEpsilonCoords(input, epsCoords);
    tempMat.MakeIdMatrix(theDimension*2, 1, 0);
    for (int i=0; i<epsCoords.size; i++)
    { bool isLong=false;
      int firstIndex=-1;
      int secondIndex=-1;
      bool firstSignIsPositive=true;
      bool secondSignIsPositive=true;
      Vector<Rational>& currentRoot=epsCoords[i];
      for (int j=0; j<theDimension; j++)
      { if (currentRoot[j]!=0)
        { isLong=!isLong;
          if(isLong)
          { if (currentRoot[j].IsPositive())
              firstSignIsPositive=true;
            else
              firstSignIsPositive=false;
            firstIndex=j;
          } else
          { if (currentRoot[j].IsPositive())
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
  { Vector<Rational>& currentRoot=epsCoords.TheObjects[i];
    tempS=currentRoot.ToStringEpsilonFormat();
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
        out << "|";
    }
    out << "}";
    for (int i=0; i< tempMat.NumRows; i++)
    { if (this->AmbientWeyl.WeylLetter=='B' && (i==theDimension || i==theDimension+1))
        out << "\\hline";
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

void ElementSemisimpleLieAlgebra::MakeGenerator
  (int generatorIndex, List<SemisimpleLieAlgebra>& inputOwners, int inputIndexInOwners)
{ //Changing RootSystem order invalidates this function!
  if (&inputOwners==0 || inputIndexInOwners==-1)
  { std::cout << " This is a programming error: usage of non-initialized Semisimple Lie algebra. Please debug file "
    << __FILE__ << " line " << __LINE__;
    assert(false);
  }
  this->MakeZero(inputOwners, inputIndexInOwners);
  ChevalleyGenerator tempGenerator;
  tempGenerator.MakeGenerator(inputOwners, inputIndexInOwners, generatorIndex);
  this->AddMonomial(tempGenerator,1);
}

void ElementSemisimpleLieAlgebra::MakeGGenerator
(const Vector<Rational>& theRoot, List<SemisimpleLieAlgebra>& inputOwners, int inputIndexInOwners)
{ this->MakeGenerator
  (inputOwners[inputIndexInOwners].GetGeneratorFromRoot(theRoot), inputOwners, inputIndexInOwners);
}

void ElementSemisimpleLieAlgebra::AssignVectorNegRootSpacesCartanPosRootSpaces
(const Vector<Rational>& input, SemisimpleLieAlgebra& owner)
{ //Changing RootSystem order invalidates this function!
  this->MakeZero(*owner.owner, owner.indexInOwner);
  ChevalleyGenerator tempGenerator;
  for (int i=0; i<input.size; i++)
    if (input[i]!=0)
    { tempGenerator.MakeGenerator(*this->ownerArray, this->indexOfOwnerAlgebra, i);
      this->AddMonomial(tempGenerator, input[i]);
    }
}

void ElementSemisimpleLieAlgebra::ElementToVectorNegativeRootSpacesFirst(Vector<Rational>& output)const
{ output.MakeZero(this->GetOwner().GetNumGenerators());
  for (int i=0; i<this->size; i++)
    output[this->TheObjects[i].theGeneratorIndex]=this->theCoeffs[i];
}

void SemisimpleLieAlgebra::ComputeOneAutomorphism
(GlobalVariables& theGlobalVariables, Matrix<Rational> & outputAuto, bool useNegativeRootsFirst)
{ rootSubalgebra theRootSA;
  theRootSA.AmbientWeyl=(this->theWeyl);
  int theDimension= this->theWeyl.CartanSymmetric.NumRows;
  theRootSA.genK.MakeEiBasis(theDimension);
  ReflectionSubgroupWeylGroup theAutos;
  theRootSA.GenerateAutomorphismsPreservingBorel(theGlobalVariables, theAutos);
  Matrix<Rational>  mapOnRootSpaces;
  int theAutoIndex= theAutos.ExternalAutomorphisms.size>1? 1 : 0;
  /*if (this->theWeyl.WeylLetter=='D' && theDimension==4)
    theAutoIndex=2;
*/
  mapOnRootSpaces.AssignRootsToRowsOfMatrix(theAutos.ExternalAutomorphisms.TheObjects[theAutoIndex]);
  mapOnRootSpaces.Transpose(theGlobalVariables);
//  mapOnRootSpaces.ComputeDebugString();
//  Matrix<Rational>  theDet=mapOnRootSpaces;
//  Rational tempRat;
//  theDet.ComputeDeterminantOverwriteMatrix(tempRat);
//  std::cout << " ... and the det is: " << tempRat.ToString();
  Selection NonExplored;
  int numRoots= this->theWeyl.RootSystem.size;
  NonExplored.init(numRoots);
  NonExplored.MakeFullSelection();
  Vector<Rational> domainRoot, rangeRoot;

  this->ComputeChevalleyConstantS(theGlobalVariables);
  List<ElementSemisimpleLieAlgebra> Domain, Range;
  Range.SetSize(numRoots+theDimension);
  Domain.SetSize(numRoots+theDimension);
  ElementSemisimpleLieAlgebra tempElt;
  for (int i=0; i<theDimension; i++)
  { domainRoot.MakeEi(theDimension, i);
    mapOnRootSpaces.ActOnVectorColumn(domainRoot, rangeRoot);
    tempElt.MakeHgenerator(domainRoot, *this->owner, this->indexInOwner);
    Domain[numRoots+i]=tempElt;
    tempElt.MakeHgenerator(rangeRoot, *this->owner, this->indexInOwner);
    Range[numRoots+i]=tempElt;
    for (int i=0; i<2; i++, domainRoot.Minus(), rangeRoot.Minus())
    { int theIndex= this->theWeyl.RootSystem.GetIndex(rangeRoot);
      tempElt.MakeGGenerator(rangeRoot, *this->owner, this->indexInOwner);
      Range[theIndex]=tempElt;
      tempElt.MakeGGenerator(domainRoot, *this->owner, this->indexInOwner);
      Domain.TheObjects[theIndex]=tempElt;
      NonExplored.RemoveSelection(theIndex);
    }
  }
  Vector<Rational> left, right;
  while(NonExplored.CardinalitySelection>0)
  { for (int i=0; i<NonExplored.CardinalitySelection; i++)
    { int theIndex = NonExplored.elements[i];
      Vector<Rational>& current = this->theWeyl.RootSystem.TheObjects[theIndex];
      for (int j=0; j<theDimension; j++)
      { left.MakeEi(theDimension, j);
        for (int k=0; k<2; k++, left.Minus())
        { right= current-left;
          if (this->theWeyl.IsARoot(right))
          { int leftIndex= this->theWeyl.RootSystem.GetIndex(left);
            int rightIndex= this->theWeyl.RootSystem.GetIndex(right);
            if (!NonExplored.selected[rightIndex])
            { ElementSemisimpleLieAlgebra& leftDomainElt=Domain.TheObjects[leftIndex];
              ElementSemisimpleLieAlgebra& rightDomainElt= Domain.TheObjects[rightIndex];
              this->LieBracket(leftDomainElt, rightDomainElt, Domain.TheObjects[theIndex]);
              ElementSemisimpleLieAlgebra& leftRangeElt=Range.TheObjects[leftIndex];
              ElementSemisimpleLieAlgebra& rightRangeElt= Range.TheObjects[rightIndex];
              this->LieBracket(leftRangeElt, rightRangeElt, Range.TheObjects[theIndex]);
              NonExplored.RemoveSelection(theIndex);
            }
          }
        }
      }
    }
  }
  Vectors<Rational> vectorsLeft, vectorsRight;
  vectorsLeft.SetSize(Range.size);
  vectorsRight.SetSize(Range.size);
  if (!useNegativeRootsFirst)
    for (int i=0; i<Range.size; i++)
    { Range[i].ElementToVectorNegativeRootSpacesFirst(vectorsRight[i]);
      Domain[i].ElementToVectorNegativeRootSpacesFirst(vectorsLeft[i]);
    }
  else
    for (int i=0; i<Range.size; i++)
    { Range[i].ElementToVectorNegativeRootSpacesFirst(vectorsRight[i]);
      Domain[i].ElementToVectorNegativeRootSpacesFirst(vectorsLeft[i]);
    }
  outputAuto.MakeLinearOperatorFromDomainAndRange(vectorsLeft, vectorsRight);
}

bool SemisimpleLieAlgebra::IsInTheWeightSupport(Vector<Rational>& theWeight, Vector<Rational>& highestWeight, GlobalVariables& theGlobalVariables)
{ Vector<Rational> correspondingDominant= theWeight;
  this->theWeyl.RaiseToDominantWeight(correspondingDominant);
  Vector<Rational> theDiff= highestWeight - correspondingDominant;
  if (!theDiff.IsPositiveOrZero())
    return false;
  return true;
}

void SemisimpleLieAlgebra::CreateEmbeddingFromFDModuleHaving1dimWeightSpaces(Vector<Rational>& theHighestWeight, GlobalVariables& theGlobalVariables)
{ /*Vectors<Rational> weightSupport;
  this->GenerateWeightSupport(theHighestWeight, weightSupport, theGlobalVariables);
  int highestWeight, distanceToHW;
  this->EmbeddingsRootSpaces.SetSize(this->theWeyl.RootSystem.size);
  int theDimension= this->theWeyl.CartanSymmetric.NumRows;
  List<bool> Explored;
  Explored.initFillInObject(this->theWeyl.RootSystem.size, false);
  int numExplored=0;
  for (int i=0; i<this->theWeyl.RootSystem.size; i++)
  { Vector<Rational>& current= this->theWeyl.RootSystem.TheObjects[i];
    if (current.SumCoordinates()==1 || current.SumCoordinates()==-1)
    { numExplored++;
      Explored.TheObjects[i]=true;
      Matrix<Rational> & currentMat= this->EmbeddingsRootSpaces.TheObjects[i];
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
  Vectors<Rational> simpleBasis;
  simpleBasis.MakeEiBasis(theDimension);
  while (numExplored<this->theWeyl.RootSystem.size)
  { for (int i=0; i<this->theWeyl.RootSystem.size; i++)
      if (Explored.TheObjects[i])
        for (int j=0; j<this->theWeyl.RootSystem.size; j++)
          if (Explored.TheObjects[j])
          { Vector<Rational> tempRoot= this->theWeyl.RootSystem.TheObjects[i]+this->theWeyl.RootSystem.TheObjects[j];
            if (this->theWeyl.IsARoot(tempRoot))
            { int index= this->theWeyl.RootSystem.GetIndex(tempRoot);
              if (!Explored.TheObjects[index])
              { Explored.TheObjects[index]=true;
                numExplored++;
                this->EmbeddingsRootSpaces.TheObjects[index]= this->EmbeddingsRootSpaces.TheObjects[i];
                this->EmbeddingsRootSpaces.TheObjects[index].LieBracketWith(this->EmbeddingsRootSpaces.TheObjects[j]);
              }
            }
          }
  }
  this->EmbeddingsCartan.SetSize(theDimension);
  for (int i=0; i<theDimension; i++)
  { Matrix<Rational> & current= this->EmbeddingsCartan.TheObjects[i];
    current.init(weightSupport.size, weightSupport.size);
    current.NullifyAll();
    Vector<Rational> tempRoot;
    tempRoot.MakeEi(theDimension, i);
    for (int j=0; j<weightSupport.size; j++)
      current.elements[j][j]=this->theWeyl.RootScalarCartanRoot(tempRoot, weightSupport.TheObjects[j]);
  }*/
}

int SemisimpleLieAlgebra::GetLengthStringAlongAlphaThroughBeta(Vector<Rational>& alpha, Vector<Rational>& beta, int& distanceToHighestWeight, Vectors<Rational>& weightSupport)
{ Vector<Rational> tempRoot=beta;
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

void rootSubalgebra::GenerateAutomorphismsPreservingBorel(GlobalVariables& theGlobalVariables, ReflectionSubgroupWeylGroup& outputAutomorphisms)
{ this->ComputeAll();
  this->GenerateIsomorphismsPreservingBorel(*this, theGlobalVariables, &outputAutomorphisms, false);
}

ElementUniversalEnveloping<Polynomial<Rational> >
Parser::ParseAndCompute(const std::string& input, GlobalVariables& theGlobalVariables)
{ this->Parse(input);
  this->Evaluate(theGlobalVariables);
  int numVars=this->theValue.impliedNumVars;
  Polynomial<Rational>  polyOne, polyZero;
  polyOne.MakeOne(numVars);
  polyZero.MakeZero(numVars);
  this->theValue.UEElement.GetElement().Simplify(theGlobalVariables, polyOne, polyZero);
  return this->theValue.UEElement.GetElement();
}

void Parser::ParseEvaluateAndSimplifyPart1(const std::string& input, GlobalVariables& theGlobalVariables)
{ this->theHmm.theRange().ComputeChevalleyConstantS
  (theGlobalVariables)
  ;
  ANNOYINGSTATISTICS;
  //std::cout << "<hr>Chevalley constants computed on the " << theGlobalVariables.GetElapsedSeconds() << " second.";
  this->Parse(input);
  //this->ComputeDebugString(false, theGlobalVariables);
}

std::string Parser::ParseEvaluateAndSimplifyPart2
(const std::string& input, bool useHtml, GlobalVariables& theGlobalVariables, FormatExpressions& theFormat)
{ this->Evaluate(theGlobalVariables);
  int numVars=this->theValue.impliedNumVars;
  Polynomial<Rational>  polyOne, polyZero;
  polyOne.MakeOne(numVars);
  polyZero.MakeZero(numVars);
  if (!this->theValue.UEElement.IsZeroPointer())
    this->theValue.UEElement.GetElement().Simplify(theGlobalVariables, polyOne, polyZero);
  if(!this->theValue.UEElement.IsZeroPointer())
    if (!this->theValue.UEElementOrdered.GetElement().IsEqualToZero())
    { Polynomial<Rational>  polyOne, polyZero;
      int numVars=this->theValue.UEElementOrdered.GetElement().TheObjects[0].Coefficient.NumVars;
      polyOne.MakeConst(numVars, 1);
      polyZero.MakeZero(numVars);
      this->theValue.UEElementOrdered.GetElement().Simplify(&theGlobalVariables, polyOne, polyZero);
    }
  std::stringstream out;
  if (useHtml)
    out << "<DIV class=\"math\" scale=\"50\">\\begin{eqnarray*}&& " << this->StringBeingParsed << "\\end{eqnarray*} = </div>";
  out << this->theValue.ElementToStringValueAndType(true, theGlobalVariables, true, theFormat);
  return out.str();
}

std::string Parser::ParseEvaluateAndSimplify
(const std::string& input, bool useHtml, GlobalVariables& theGlobalVariables, FormatExpressions& theFormat)
{ this->ParseEvaluateAndSimplifyPart1(input, theGlobalVariables);
  return this->ParseEvaluateAndSimplifyPart2(input, useHtml, theGlobalVariables, theFormat);
}

int DebugCounter=-1;

void Parser::InitAndTokenize(const std::string& input)
{ this->TokenStack.ReservE(input.size());
  this->ValueStack.ReservE(input.size());
  this->ReservE(input.size());
  this->TokenStack.size=0;
  this->ValueStack.size=0;
  this->TokenBuffer.size=0;
  this->ValueBuffer.size=0;
  this->NodeIndexStack.size=0;
  this->size=0;
  this->MaxFoundVars=0;
  this->numEmptyTokensAtBeginning=6;
  this->SystemCommands.size=0;
  this->initFunctionList(this->DefaultWeylLetter, this->DefaultWeylRank);
  this->theValue.theAnimation.GetElement().flagAnimating=false;
  this->flagDisplayIndicator=true;
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
  this->ValueStack.ReservE(this->ValueBuffer.size);
  this->TokenStack.ReservE(this->TokenBuffer.size);
  this->NodeIndexStack.ReservE(this->TokenBuffer.size);
  this->StringBeingParsed=input;
  for (int i=0; i<this->numEmptyTokensAtBeginning; i++)
  { this->TokenStack.AddOnTop(this->tokenEmpty);
    this->ValueStack.AddOnTop(0);
    this->NodeIndexStack.AddOnTop(-1);
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
    case ';': return true;
    case ':': return true;
    default: return false;
  }
//  return false;
}

void Parser::Parse(const std::string& input)
{ this->InitAndTokenize(input);
  //std::cout << "Num vars before actual parsing: " << this->NumVariables;
  this->ParseNoInit(0, this->TokenBuffer.size-1);
}

void Parser::ParseNoInit(int indexFrom, int indexTo)
{ if (indexFrom<0 || indexTo>= this->TokenBuffer.size)
    return;
  for (int i=indexFrom; i<=indexTo; i++)
  { this->ValueStack.AddOnTop(this->ValueBuffer.TheObjects[i]);
    this->TokenStack.AddOnTop(this->TokenBuffer.TheObjects[i]);
    this->NodeIndexStack.AddOnTop(-1);
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
{ switch(theToken)
  { case Parser::tokenEnd:
    case Parser::tokenPlus:
    case Parser::tokenMinus:
    case Parser::tokenCloseBracket:
    case Parser::tokenCloseLieBracket:
    case Parser::tokenCloseCurlyBracket:
    case Parser::tokenComma:
    case Parser::tokenColon:
    case Parser::tokenMapsTo:
      return false;
    default: return true;
  }
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
    return this->TransformXtoNothing();
  if (tokenLast==this->tokenCloseCurlyBracket && tokenThirdToLast==this->tokenOpenCurlyBracket && tokenSecondToLast==this->tokenExpression)
    return this->TransformXYXtoY();
  if (tokenLast==this->tokenExpression && tokenSecondToLast==this->tokenEmbedding && lookAheadToken!=this->tokenUnderscore)
    return this->ReplaceOYbyE();
  if (tokenLast== this->tokenFunctionNoArgument)
    return this->ReplaceObyE();
  if (tokenSecondToLast==this->tokenFunction && tokenLast== this->tokenArraY)
    return this->ReplaceOYbyE();
  if (tokenFourthToLast==this->tokenFunction && tokenThirdToLast==this->tokenOpenBracket && tokenSecondToLast== this->tokenExpression && tokenLast==this->tokenCloseBracket)
    return this->ReplaceXYXbyA();
  if (tokenLast==this->tokenArraY)
    return this->TransformXtoE();
  if (tokenLast==this->tokenExpression && tokenSecondToLast==this->tokenMinus && !this->TokenProhibitsUnaryMinus(tokenThirdToLast) && !this->lookAheadTokenProhibitsPlus(lookAheadToken))
    return this->ReplaceXYbyE(this->tokenMinusUnary);
  if (tokenLast==this->tokenCloseBracket && tokenSecondToLast==this->tokenExpression && tokenThirdToLast==this->tokenOpenBracket)
    return this->TransformXYXtoY();
  if (tokenSecondToLast==this->tokenMapsTo && tokenLast==this->tokenExpression && tokenThirdToLast==this->tokenExpression && this->lookAheadTokenAllowsMapsTo(lookAheadToken))
    return this->ReplaceYOZbyE();
  if (tokenLast== this->tokenCloseLieBracket && tokenSecondToLast==this->tokenExpression && tokenThirdToLast==this->tokenComma && tokenFourthToLast==this->tokenExpression && tokenFifthToLast==this->tokenOpenLieBracket)
    return this->ReplaceXYCZXbyE(this->tokenLieBracket);
  if (tokenLast==this->tokenCloseLieBracket && tokenSecondToLast==this->tokenExpression && tokenThirdToLast==this->tokenOpenLieBracket && tokenFourthToLast==this->tokenExpression)
    return this->ReplaceEXEXbyE(this->tokenDereferenceArray);
  if (tokenLast==this->tokenDigit && lookAheadToken!=this->tokenDigit && lookAheadToken!=this->tokenDot)
    return this->ReplaceDdotsDbyEdoTheArithmetic();
  if (tokenLast==this->tokenPartialDerivative || tokenLast==this->tokenG || tokenLast==this->tokenF || tokenLast==this->tokenX || tokenLast==this->tokenH || tokenLast== this->tokenC || tokenLast==this->tokenVariable)
    return this->ReplaceObyE();
  if (tokenSecondToLast==this->tokenUnderscore && tokenLast==this->tokenExpression && tokenThirdToLast==this->tokenExpression && tokenFourthToLast!=this->tokenUnderscore)
    return this->ReplaceYOZbyE();
  if (tokenLast==this->tokenExpression && tokenSecondToLast== this->tokenPower && tokenThirdToLast==this->tokenExpression)
    return this->ReplaceYOZbyE();
  if (tokenLast==this->tokenExpression && tokenSecondToLast== this->tokenDivide && tokenThirdToLast==this->tokenExpression && !this->lookAheadTokenProhibitsTimes(lookAheadToken))
    return this->ReplaceYOZbyE();
  if (tokenLast==this->tokenExpression && tokenThirdToLast==this->tokenExpression && tokenSecondToLast==tokenTimes && !this->lookAheadTokenProhibitsTimes(lookAheadToken))
    return this->ReplaceYOZbyE();
  if (tokenLast==this->tokenExpression && tokenSecondToLast==this->tokenExpression && !this->lookAheadTokenProhibitsTimes(lookAheadToken))
    return this->ReplaceZYbyE(this->tokenTimes);
  if (tokenSecondToLast==this->tokenPlus && tokenLast==this->tokenExpression && tokenThirdToLast==this->tokenExpression && !this->lookAheadTokenProhibitsPlus(lookAheadToken))
    return this->ReplaceYOZbyE();
  if (tokenSecondToLast==this->tokenMinus && tokenLast==this->tokenExpression && tokenThirdToLast==this->tokenExpression && !this->lookAheadTokenProhibitsPlus(lookAheadToken))
    return this->ReplaceYOZbyE();
  int rootDim;
  if (this->StackTopIsDelimiter1ECdotsCEDelimiter2(rootDim, this->tokenOpenBracket, this->tokenCloseBracket))
    return this->ReplaceXECdotsCEXbyE(rootDim, this->tokenArraY, this->tokenArraY);
  if (this->StackTopIsDelimiter1ECdotsCEDelimiter2EDelimiter3(rootDim, this->tokenOpenBracket, this->tokenColon, this->tokenCloseBracket))
    return this->ReplaceXECdotsCEXEXbyE(rootDim, this->tokenExpression, this->tokenColon);
  return false;
}

void Parser::ParseAndCompute(const std::string& input, std::string& output, GlobalVariables& theGlobalVariables)
{ std::stringstream out; std::string tempS;
  this->Parse(input);
  out << "\\begin{eqnarray*}&& " << input << " = \\\\\n";
  this->Evaluate(theGlobalVariables);
//  this->WeylAlgebraValue.ToString(tempS, false, true, false);
  out << tempS;
  out << "\\end{eqnarray*}";
  output=out.str();
}

void Parser::Evaluate(GlobalVariables& theGlobalVariables)
{ if (this->TokenStack.size== this->numEmptyTokensAtBeginning+1)
  { if (*this->TokenStack.LastObject()==this->tokenExpression)
    { this->TheObjects[this->NodeIndexStack.TheObjects[this->numEmptyTokensAtBeginning]].Evaluate(theGlobalVariables);
//      this->WeylAlgebraValue.Assign(this->TheObjects[this->ValueStack.TheObjects[this->numEmptyTokensAtBeginning]].WeylAlgebraElement);
//      this->LieAlgebraValue= this->TheObjects[this->ValueStack.TheObjects[this->numEmptyTokensAtBeginning]].LieAlgebraElement;
      this->theValue=this->TheObjects[this->NodeIndexStack.TheObjects[this->numEmptyTokensAtBeginning]];
    } else
      this->theValue.SetError(ParserNode::errorBadSyntax);
  }
  if (this->TokenStack.size>this->numEmptyTokensAtBeginning+1)
    this->theValue.SetError(ParserNode::errorBadSyntax);
//  this->WeylAlgebraValue.ComputeDebugString(false, false);
}

void ParserNode::Evaluate(GlobalVariables& theGlobalVariables)
{ //this->UEElement.ComputeDebugString();
  this->Evaluated=true;
  for (int i=0; i<this->children.size; i++)
  { if (this->Operation==Parser::tokenEmbedding)
      this->IndexContextLieAlgebra=this->owner->theHmm.theDomain().indexInOwner;
    this->owner->TheObjects[this->children.TheObjects[i]].IndexContextLieAlgebra=this->IndexContextLieAlgebra;
    this->owner->TheObjects[this->children.TheObjects[i]].Evaluate(theGlobalVariables);
    if (this->owner->TheObjects[this->children.TheObjects[i]].ExpressionType==this->typeError)
    { this->CopyError(this->owner->TheObjects[this->children.TheObjects[i]]);
      return;
    }
  }
  this->impliedNumVars=this->GetMaxImpliedNumVarsChildren();
  switch (this->Operation)
  { case Parser::tokenPlus: this->EvaluatePlus(theGlobalVariables); break;
    case Parser::tokenMinus: this->EvaluateMinus(theGlobalVariables); break;
    case Parser::tokenMinusUnary: this->EvaluateMinusUnary(theGlobalVariables); break;
    case Parser::tokenTimes: this->EvaluateTimes(theGlobalVariables); break;
    case Parser::tokenG: break;
    case Parser::tokenH: break;
    case Parser::tokenF: break;
    case Parser::tokenVariable: break;
    case Parser::tokenPartialDerivative: break;
    case Parser::tokenX: break;
    case Parser::tokenDivide: this->EvaluateDivide(theGlobalVariables); break;
    case Parser::tokenUnderscore: this->EvaluateUnderscore(theGlobalVariables); break;
    case Parser::tokenInteger: this->EvaluateInteger(theGlobalVariables); break;
    case Parser::tokenLieBracket: this->EvaluateLieBracket(theGlobalVariables); break;
    case Parser::tokenPower: this->EvaluateThePower(theGlobalVariables); break;
//    case Parser::tokenEmbedding: this->EvaluateEmbedding(theGlobalVariables); break;
    case Parser::tokenFunction: this->EvaluateFunction(theGlobalVariables); break;
    case Parser::tokenFunctionNoArgument: this->EvaluateFunction(theGlobalVariables); break;
    case Parser::tokenArraY: this->ExpressionType=this->typeArray; break;
    case Parser::tokenMapsTo: this->EvaluateSubstitution(theGlobalVariables); break;
    case Parser::tokenColon: this->EvaluateApplySubstitution(theGlobalVariables); break;
    case Parser::tokenDereferenceArray: this->EvaluateDereferenceArray(theGlobalVariables); break;
    default: this->SetError(this->errorUnknownOperation); return;
  }
}

void ParserNode::EvaluateInteger(GlobalVariables& theGlobalVariables)
{ if (!this->rationalValue.IsSmallInteger())
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
  { this->impliedNumVars=this->GetMaxImpliedNumVarsChildren();
    if ((rightNode.ExpressionType==this->typeRational || rightNode.ExpressionType==this->typePoly) && leftNode.ExpressionType==this->typeUEelement)
      if (leftNode.UEElement.GetElement().IsAPowerOfASingleGenerator())
        { rightNode.ConvertToType(this->typePoly, this->impliedNumVars, theGlobalVariables);
          leftNode.UEElement.GetElement().SetNumVariables(this->impliedNumVars);
          MonomialUniversalEnveloping<Polynomial<Rational> > tempMon;
          tempMon.operator=(leftNode.UEElement.GetElement().TheObjects[0]);
          tempMon.Powers[0]*=rightNode.polyValue.GetElement();
          this->UEElement.GetElement().MakeZero(this->owner->theAlgebras, this->IndexContextLieAlgebra);
          this->UEElement.GetElement().AddMonomial(tempMon, tempMon.Powers[0].GetOne());
          this->ExpressionType=this->typeUEelement;
          return;
        }
    if ((rightNode.ExpressionType==this->typeRational || rightNode.ExpressionType==this->typePoly) && leftNode.ExpressionType==this->typeUEElementOrdered)
      if (leftNode.UEElementOrdered.GetElement().IsAPowerOfASingleGenerator())
      { rightNode.ConvertToType(this->typePoly, this->impliedNumVars, theGlobalVariables);
        leftNode.UEElementOrdered.GetElement().SetNumVariables(this->impliedNumVars);
        MonomialUniversalEnvelopingOrdered<Polynomial<Rational> > tempMon;
        tempMon.operator=(leftNode.UEElementOrdered.GetElement().TheObjects[0]);
        tempMon.Powers.TheObjects[0]*=(rightNode.polyValue.GetElement());
        this->UEElementOrdered.GetElement().MakeZero(this->owner->testAlgebra);
        this->UEElementOrdered.GetElement().AddMonomial(tempMon);
        this->ExpressionType=this->typeUEElementOrdered;
        return;
      }
    this->SetError(this->errorDunnoHowToDoOperation);
    return;
  }
  int thePower= rightNode.intValue;
  this->impliedNumVars=leftNode.impliedNumVars;
  Polynomial<Rational>  PolyOne;
  PolyOne.MakeConst(this->impliedNumVars, 1);
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
      this->polyValue.GetElement()=leftNode.polyValue.GetElement();
      if (thePower>=0)
      { this->polyValue.GetElement().RaiseToPower(thePower, (Rational) 1);
        this->ExpressionType=this->typePoly;
      } else
      { this->ratFunction.GetElement().context=&theGlobalVariables;
        this->ratFunction.GetElement()=this->polyValue.GetElement();
        this->ratFunction.GetElement().RaiseToPower(thePower);
        this->ExpressionType=this->typeRationalFunction;
      }
      break;
    case ParserNode::typeRationalFunction:
      this->ratFunction.GetElement()=leftNode.ratFunction.GetElementConst();
      this->ratFunction.GetElement().RaiseToPower(thePower);
      this->ExpressionType=this->typeRationalFunction;
      break;
    case ParserNode::typeUEelement:
      this->UEElement.GetElement()=leftNode.UEElement.GetElement();
      this->UEElement.GetElement().RaiseToPower(thePower);
      this->ExpressionType=this->typeUEelement;
    break;
    case ParserNode::typeUEElementOrdered:
      this->UEElementOrdered.GetElement()=leftNode.UEElementOrdered.GetElement();
      this->UEElementOrdered.GetElement().RaiseToPower(thePower, PolyOne);
      this->ExpressionType=this->typeUEElementOrdered;
    break;
    case ParserNode::typeWeylAlgebraElement:
      if (thePower<0)
      { this->SetError(errorDunnoHowToDoOperation);
        return;
      }
      this->WeylAlgebraElement.GetElement()=leftNode.WeylAlgebraElement.GetElement();
      this->WeylAlgebraElement.GetElement().RaiseToPower(thePower);
      this->ExpressionType=this->typeWeylAlgebraElement;
    break;
    default: this->SetError(this->errorDunnoHowToDoOperation); return;
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
  { if (rightNode.ExpressionType==this->typeArray)
      this->EvaluateUnderscoreLeftArgumentIsArray(theGlobalVariables);
    else
      this->SetError(this->errorBadIndex);
    return;
  }
  int theIndex= rightNode.intValue;
  int theDimension= this->GetContextLieAlgebra().theWeyl.CartanSymmetric.NumRows;
  if (leftNode.Operation==Parser::tokenH)
  { theIndex--;
    if (theIndex>=theDimension || theIndex<0)
    { this->SetError(this->errorBadIndex);
      return;
    }
    Vector<Rational> tempRoot;
    tempRoot.MakeEi(theDimension, theIndex);
    Polynomial<Rational>  polyZero;
    polyZero.MakeZero(0);
    this->UEElement.GetElement().MakeHgenerator
    (tempRoot, this->owner->theAlgebras, this->IndexContextLieAlgebra, polyZero.GetOne(), polyZero);
    this->ExpressionType=this->typeUEelement;
    return;
  }
  if (leftNode.Operation==Parser::tokenG)
  { theIndex=this->GetContextLieAlgebra().GetRootIndexFromDisplayIndex(theIndex);
    theIndex=this->GetContextLieAlgebra().GetGeneratorFromRootIndex(theIndex);
    if (theIndex<0 || theIndex>this->GetContextLieAlgebra().theWeyl.RootSystem.size+theDimension)
    { this->SetError(this->errorBadIndex);
      return;
    }
    Polynomial<Rational>  polyZero;
    polyZero.MakeZero(0);
    this->UEElement.GetElement().MakeOneGeneratorCoeffOne
    (theIndex, this->owner->theAlgebras, this->IndexContextLieAlgebra, polyZero.GetOne(), polyZero);
    this->ExpressionType=this->typeUEelement;
    return;
  }
  if (leftNode.Operation==Parser::tokenPartialDerivative)
  { theIndex--;
    if (theIndex<0 || theIndex>1000)
    { this->SetError(this->errorBadIndex);
      return;
    }
    this->owner->MaxFoundVars=MathRoutines::Maximum(theIndex+1, this->owner->MaxFoundVars);
    this->impliedNumVars=theIndex+1;
    this->WeylAlgebraElement.GetElement().Makedi(theIndex, this->impliedNumVars);
    this->ExpressionType=this->typeWeylAlgebraElement;
  }
  if (leftNode.Operation==Parser::tokenF)
  { if (theIndex>0 && theIndex<=this->GetContextLieAlgebra().GetNumPosRoots())
      theIndex+= this->GetContextLieAlgebra().GetNumPosRoots()+this->GetContextLieAlgebra().GetRank()-1;
    else if (theIndex==0)
      theIndex=this->GetContextLieAlgebra().GetNumPosRoots()+this->GetContextLieAlgebra().GetRank()-1;
    else if (theIndex<0)
      theIndex+= this->GetContextLieAlgebra().GetNumPosRoots();
    else
      theIndex--;
    if (theIndex<0 || theIndex>this->GetContextLieAlgebra().theWeyl.RootSystem.size+this->GetContextLieAlgebra().GetRank())
    { this->SetError(this->errorBadIndex);
      return;
    }
    Polynomial<Rational>  polyOne;
    polyOne.MakeOne(0);
    this->UEElementOrdered.GetElement().MakeOneGenerator(theIndex, polyOne, this->owner->testAlgebra, &theGlobalVariables);
    this->ExpressionType=this->typeUEElementOrdered;
    return;
  }
  if (leftNode.Operation==Parser::tokenX)
    leftNode.Operation=Parser::tokenVariable;
  if (leftNode.Operation==Parser::tokenVariable)
  { theIndex--;
    if (theIndex<0 || theIndex>1000)
    { this->SetError(this->errorBadIndex);
      return;
    }
    this->impliedNumVars=theIndex+1;
    this->owner->MaxFoundVars=MathRoutines::Maximum(this->impliedNumVars, this->owner->MaxFoundVars);
    this->polyValue.GetElement().MakeMonomial(this->impliedNumVars, theIndex, 1, (Rational) 1);
    this->ExpressionType=this->typePoly;
    return;
  }
}

bool ParserNode::ConvertChildrenToType(int theType, int GoalNumVars, GlobalVariables& theGlobalVariables)
{ for (int i=0; i<this->children.size; i++)
    if (!this->owner->TheObjects[this->children.TheObjects[i]].ConvertToType(theType, GoalNumVars, theGlobalVariables))
      return false;
  return true;
}

void ParserNode::InitForAddition(GlobalVariables* theContext)
{ this->intValue=0;
  this->rationalValue.MakeZero();
  switch(this->ExpressionType)
  { case ParserNode::typePoly:
      this->polyValue.GetElement().MakeZero(this->impliedNumVars);
      break;
    case ParserNode::typeUEelement:
      this->UEElement.GetElement().MakeZero(this->owner->theAlgebras, this->IndexContextLieAlgebra);
      break;
    case ParserNode::typeUEElementOrdered:
      this->UEElementOrdered.GetElement().MakeZero(this->owner->testAlgebra);
      break;
    case ParserNode::typeWeylAlgebraElement:
      this->WeylAlgebraElement.GetElement().MakeZero(this->impliedNumVars);
      break;
    case ParserNode::typeRationalFunction:
      this->ratFunction.GetElement().MakeZero(this->impliedNumVars, theContext);
      break;
    case ParserNode::typeQuasiPolynomial:
      this->theQP.GetElement().MakeZeroLatTiceZn(this->impliedNumVars);
      break;
    case ParserNode::typeAnimation:
      this->theAnimation.GetElement().MakeZero();
      break;
    case ParserNode::typeGenVermaElt:
      this->theGenVermaElt.GetElement().MakeZero();//this->owner->theModulePolys);
      this->impliedNumVars=this->owner->MaxFoundVars;
      this->owner->SetNumVarsModulePolys(this->impliedNumVars);
    break;
    case ParserNode::typeCharSSFDMod:
      this->theChar.GetElement().MakeZero(this->owner->theAlgebras, this->IndexContextLieAlgebra);
      break;
    default:
      break;
  }
}

void ParserNode::InitForMultiplication(GlobalVariables* theContext)
{ this->intValue=1;
  this->rationalValue=1;
  if(this->ExpressionType==this->typePoly)
    this->polyValue.GetElement().MakeConst(this->impliedNumVars, (Rational)1);
  if(this->ExpressionType==this->typeUEelement)
    this->UEElement.GetElement().AssignInt(1, this->impliedNumVars, this->GetContextLieAlgebra());
  if(this->ExpressionType==this->typeUEElementOrdered)
  { Polynomial<Rational>  PolyOne;
    PolyOne.MakeConst(this->impliedNumVars, 1);
    this->UEElementOrdered.GetElement().MakeConst(PolyOne, this->owner->testAlgebra);
  }
  if(this->ExpressionType==this->typeWeylAlgebraElement)
    this->WeylAlgebraElement.GetElement().MakeConst(this->impliedNumVars, (Rational) 1);
  if (this->ExpressionType==this->typeRationalFunction)
    this->ratFunction.GetElement().MakeConst(this->impliedNumVars, (Rational) 1, theContext);
  if (this->ExpressionType==this->typeCharSSFDMod)
    this->theChar.GetElement().MakeTrivial(this->owner->theAlgebras, this->IndexContextLieAlgebra);
}

int ParserNode::GetStrongestExpressionChildrenConvertChildrenIfNeeded(GlobalVariables& theGlobalVariables)
{ int result=this->typeUndefined;
  for (int i=0; i<this->children.size; i++)
  { int childExpressionType=this->owner->TheObjects[this->children.TheObjects[i]].ExpressionType;
    if (childExpressionType>result)
      result=childExpressionType;
  }
  this->impliedNumVars=this->GetMaxImpliedNumVarsChildren();
  for (int i=0; i<this->children.size; i++)
    if(!this->owner->TheObjects[this->children.TheObjects[i]].ConvertToType(result, this->impliedNumVars, theGlobalVariables))
    { this->SetError(this->owner->TheObjects[this->children.TheObjects[i]].ErrorType);
      return this->typeError;
    }
  return result;
}

void ParserNode::ConvertChildrenAndMyselfToStrongestExpressionChildren(GlobalVariables& theGlobalVariables)
{ this->ExpressionType=this->GetStrongestExpressionChildrenConvertChildrenIfNeeded(theGlobalVariables);
}

void ParserNode::EvaluateMinus(GlobalVariables& theGlobalVariables)
{ if (!this->AllChildrenAreOfDefinedNonErrorType())
  { this->ExpressionType=this->typeError;
    return;
  }
  this->ConvertChildrenAndMyselfToStrongestExpressionChildren(theGlobalVariables);
  this->InitForAddition(&theGlobalVariables);
  for (int i=0; i<this->children.size; i++)
  { ParserNode& currentChild=this->owner->TheObjects[this->children.TheObjects[i]];
    switch (this->ExpressionType)
    { case ParserNode::typeIntegerOrIndex: if (i==0) this->intValue+=currentChild.intValue; else this->intValue-=currentChild.intValue; break;
      case ParserNode::typeRational:
        if (i==0)
          this->rationalValue+=currentChild.rationalValue;
        else
          this->rationalValue-=currentChild.rationalValue;
        break;
      case ParserNode::typePoly: if(i==0) this->polyValue.GetElement()+=(currentChild.polyValue.GetElement()); else this->polyValue.GetElement()-=(currentChild.polyValue.GetElement()); break;
      case ParserNode::typeRationalFunction: if(i==0) this->ratFunction.GetElement()+=(currentChild.ratFunction.GetElement()); else this->ratFunction.GetElement()-=(currentChild.ratFunction.GetElement()); break;
      case ParserNode::typeUEelement: if (i==0) this->UEElement.GetElement()+=currentChild.UEElement.GetElement(); else this->UEElement.GetElement()-=currentChild.UEElement.GetElement(); break;
      case ParserNode::typeUEElementOrdered: if (i==0) this->UEElementOrdered.GetElement()+=currentChild.UEElementOrdered.GetElement(); else this->UEElementOrdered.GetElement()-=currentChild.UEElementOrdered.GetElement(); break;
      case ParserNode::typeWeylAlgebraElement: if (i==0) this->WeylAlgebraElement.GetElement()+=currentChild.WeylAlgebraElement.GetElement(); else this->WeylAlgebraElement.GetElement()-=currentChild.WeylAlgebraElement.GetElement(); break;
      case ParserNode::typeCharSSFDMod:
        if (i==0)
          this->theChar.GetElement()+=currentChild.theChar.GetElement();
        else
          this->theChar.GetElement()-=currentChild.theChar.GetElement();
        break;
      case ParserNode::typeGenVermaElt:
        if (i==0)
          this->theGenVermaElt.GetElement()+=currentChild.theGenVermaElt.GetElement();
        else
          this->theGenVermaElt.GetElement()-=currentChild.theGenVermaElt.GetElement();
        break;
      default: this->ExpressionType=this->typeError; return;
    }
  }
}

void ParserNode::EvaluateMinusUnary(GlobalVariables& theGlobalVariables)
{ if (!this->AllChildrenAreOfDefinedNonErrorType())
  { this->ExpressionType=this->typeError;
    return;
  }
  this->ConvertChildrenAndMyselfToStrongestExpressionChildren(theGlobalVariables);
  this->InitForAddition(&theGlobalVariables);
  ParserNode& currentChild=this->owner->TheObjects[this->children.TheObjects[0]];
  switch (this->ExpressionType)
  { case ParserNode::typeIntegerOrIndex: this->intValue-=currentChild.intValue; break;
    case ParserNode::typeRational: this->rationalValue-=currentChild.rationalValue; break;
    case ParserNode::typePoly: this->polyValue.GetElement()-=(currentChild.polyValue.GetElement()); break;
    case ParserNode::typeUEelement: this->UEElement.GetElement()-=currentChild.UEElement.GetElement(); break;
    case ParserNode::typeCharSSFDMod: this->theChar.GetElement()-=currentChild.theChar.GetElement(); break;
    case ParserNode::typeUEElementOrdered: this->UEElementOrdered.GetElement()-=currentChild.UEElementOrdered.GetElement(); break;
    case ParserNode::typeWeylAlgebraElement: this->WeylAlgebraElement.GetElement()-=currentChild.WeylAlgebraElement.GetElement(); break;
    case ParserNode::typeGenVermaElt: this->theGenVermaElt.GetElement()-=currentChild.theGenVermaElt.GetElement(); break;
    default: this->ExpressionType=this->typeError; return;
  }
}

void ParserNode::ReduceRatFunction()
{ if (this->ExpressionType!=this->typeRationalFunction)
    return;
  this->ratFunction.GetElement().ReduceMemory();
  switch (this->ratFunction.GetElement().expressionType)
  { case RationalFunction::typeRational:
      this->rationalValue= this->ratFunction.GetElement().ratValue;
      this->ExpressionType=this->typeRational;
      break;
    case RationalFunction::typePoly:
      this->polyValue.GetElement().operator=(this->ratFunction.GetElement().Numerator.GetElement());
      this->ExpressionType=this->typePoly;
      break;
  }
}

bool ParserNode::OneChildrenOrMoreAreOfType(int theType)
{ for (int i=0; i<this->children.size; i++)
    if (this->owner->TheObjects[this->children.TheObjects[i]].ExpressionType==theType)
      return true;
  return false;
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
  if (this->OneChildrenOrMoreAreOfType(this->typeWeylAlgebraElement))
  { this->ExpressionType=this->typeWeylAlgebraElement;
    if (this->OneChildrenOrMoreAreOfType(this->typeUEelement) || this->OneChildrenOrMoreAreOfType(this->typeUEElementOrdered))
    { this->SetError(this->errorMultiplicationByNonAllowedTypes);
      return;
    }
  }
  if (this->OneChildrenOrMoreAreOfType(this->typeUEElementOrdered) && !this->OneChildrenOrMoreAreOfType(this->typeUEelement))
    this->ExpressionType= this->typeUEElementOrdered;
  this->impliedNumVars=this->GetMaxImpliedNumVarsChildren();
  for (int i=0; i<this->children.size; i++)
    if(!this->owner->TheObjects[this->children.TheObjects[i]].ConvertToType(this->ExpressionType, this->impliedNumVars, theGlobalVariables))
    { this->SetError(this->errorDunnoHowToDoOperation);
      return;
    }
  if (this->ExpressionType==this->typeUEelement)
  { ElementUniversalEnveloping<Polynomial<Rational> >& left= this->owner->TheObjects[this->children[0]].UEElement.GetElement();
    ElementUniversalEnveloping<Polynomial<Rational> >& right= this->owner->TheObjects[this->children[1]].UEElement.GetElement();
    left.LieBracketOnTheRight(right, this->UEElement.GetElement());
  } else if (this->ExpressionType==this->typeUEElementOrdered)
  { ElementUniversalEnvelopingOrdered<Polynomial<Rational> >& left=this->owner->TheObjects[this->children.TheObjects[0]].UEElementOrdered.GetElement();
    ElementUniversalEnvelopingOrdered<Polynomial<Rational> >& right= this->owner->TheObjects[this->children.TheObjects[1]].UEElementOrdered.GetElement();
    left.LieBracketOnTheRight(right, this->UEElementOrdered.GetElement());
    Polynomial<Rational>  polyOne, polyZero;
    polyOne.MakeConst(this->impliedNumVars, (Rational)1);
    polyZero.MakeZero(this->impliedNumVars);
    this->UEElementOrdered.GetElement().Simplify(&theGlobalVariables, polyOne, polyZero);
  } else if (this->ExpressionType==this->typeWeylAlgebraElement)
  { ElementWeylAlgebra& left=this->owner->TheObjects[this->children.TheObjects[0]].WeylAlgebraElement.GetElement();
    ElementWeylAlgebra& right= this->owner->TheObjects[this->children.TheObjects[1]].WeylAlgebraElement.GetElement();
    this->WeylAlgebraElement.GetElement().Assign(left);
    this->WeylAlgebraElement.GetElement().LieBracketOnTheRight(right, theGlobalVariables);
  } else
    this->SetError(this->errorProgramming);
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
  if (theRootNode.ExpressionType!=this->typeArray)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  if (theTrueChildren.size!=2)
  { this->SetError(this->errorWrongNumberOfArguments);
    return;
  }
  this->ExpressionType=theRootNode.GetStrongestExpressionChildrenConvertChildrenIfNeeded(theGlobalVariables);
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
      { leftNode.rationalValue.GetNumerator(tempUI1);
        rightNode.rationalValue.GetNumerator(tempUI2);
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
      if (leftNode.polyValue.GetElement().IsEqualToZero() || rightNode.polyValue.GetElement().IsEqualToZero())
      { this->SetError(this->errorDivisionByZero);
        return;
      }
      if (theFunction==Parser::functionGCD)
        RationalFunction::gcd(leftNode.polyValue.GetElement(), rightNode.polyValue.GetElement(), this->polyValue.GetElement(), &theGlobalVariables);
      else
        RationalFunction::lcm(leftNode.polyValue.GetElement(), rightNode.polyValue.GetElement(), this->polyValue.GetElement());
      this->ExpressionType=this->typePoly;
      break;
    case ParserNode::typeUEelement:
      this->SetError(errorDunnoHowToDoOperation);
    break;
    default: this->SetError(this->errorDunnoHowToDoOperation); return;
  }
}

ParserNode::ParserNode()
{ this->owner=0;
  this->Clear();
}

void Parser::ToString(bool includeLastNode, std::string& output, bool useHtml, GlobalVariables& theGlobalVariables, FormatExpressions& theFormat)
{ std::stringstream out; std::string tempS;
  std::string htmlSafish;
  if (CGI::GetHtmlStringSafeishReturnFalseIfIdentical(this->StringBeingParsed, htmlSafish))
    out << "&lt; -modified string: ";
  else
    out << "String: ";
  out << htmlSafish << "\n";
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
  int NumDisplayedNodes=this->size;
  if(!includeLastNode)
    NumDisplayedNodes--;
  for (int i=0; i<NumDisplayedNodes; i++)
  { this->TheObjects[i].ToString(tempS, theGlobalVariables, theFormat);
    out << " Index: " << i << " " << tempS << ";\n";
    if (useHtml)
      out << "<br>";
  }
  if (useHtml)
    out << "<br><br>";
  if (includeLastNode)
    out << "\n\nValue: " << this->theValue.ElementToStringValueAndType(false, theGlobalVariables, false, theFormat);

//  this->WeylAlgebraValue.ComputeDebugString(false, false);
//  this->LieAlgebraValue.ComputeDebugString(this->theLieAlgebra, false, false);
//  out << "\n\nWeyl algebra value: " << this->WeylAlgebraValue.DebugString;
//  out << "\nLie algebra value: " << this->LieAlgebraValue.DebugString;
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
    case Parser::tokenEmbedding: out << "i"; break;
    case Parser::tokenMinusUnary: out << "-"; break;
    case Parser::tokenVariable: out << "variable"; break;
    case Parser::tokenArraY: out << "array"; break;
    case Parser::tokenDereferenceArray: out << "deref[]"; break;
    case Parser::tokenMapsTo: out << "->"; break;
    case Parser::tokenFunction: out << "function"; break;
    case Parser::tokenEndStatement: out << ";"; break;
    case Parser::tokenColon: out << ":"; break;
//    case Parser::tokenSubstitution: out << "Substitution: "; break;
    case Parser::tokenFunctionNoArgument: out << "functionNoArgument"; break;
    case Parser::tokenF: out << "f"; break;
    default: out << "?"; break;
  }
}

void Parser::Clear()
{ this->theValue.Clear();
  this->theValue.UEElement.GetElement().MakeZero(this->theAlgebras, 0);
  this->TokenStack.size=0;
  this->ValueStack.size=0;
}

std::string ParserNode::ElementToStringErrorCode(bool useHtml)
{ std::stringstream out;
  switch (this->ErrorType)
  { case ParserNode::errorBadIndex: out << "error: bad index"; break;
    case ParserNode::errorDimensionProblem: out << "error with dimension and/or number of arguments"; break;
    case ParserNode::errorBadOrNoArgument: out << "error: bad or no argument"; break;
    case ParserNode::errorBadSyntax: out << "error: bad syntax."; break;
    case ParserNode::errorDunnoHowToDoOperation: out << "error: my master hasn't taught me how to do this operation (maybe he doesn't know how either)"; break;
    case ParserNode::errorDivisionByZero: out << "error: division by zero"; break;
    case ParserNode::errorDivisionByNonAllowedType: out << "error: division of/by non-allowed type"; break;
    case ParserNode::errorMultiplicationByNonAllowedTypes: out << "error: multiplication by non-allowed types"; break;
    case ParserNode::errorNoError: out << "error: error type claims no error, but expression type claims error. Slap the programmer."; break;
    case ParserNode::errorOperationByUndefinedOrErrorType: out << "error: operation with an undefined type"; break;
    case ParserNode::errorWrongNumberOfArguments: out << "error: wrong number of arguments."; break;
    case ParserNode::errorProgramming: out << "error: there has been some programming mistake (it's not your expression's fault). Slap the programmer!"; break;
    case ParserNode::errorUnknownOperation: out << "error: unknown operation. The lazy programmer has added the operation to the dictionary, but hasn't implemented it yet. Lazy programmers deserve no salary. "; break;
    case ParserNode::errorImplicitRequirementNotSatisfied: out << "Error: an implicit requirement for the funciton input has not been satisfied."; break;
    case ParserNode::errorBadFileFormat: out << "Bad input file format. "; break;
    case ParserNode::errorConversionError: out << "Error with the conversion routines. In some cases this may be a programming error."; break;
    default: out << "Non-documented error number " << this->ErrorType << ". Lazy programmers deserve no salaries."; break;
  }
  return out.str();
}

void ParserNode::ToString(std::string& output, GlobalVariables& theGlobalVariables, FormatExpressions& theFormat)
{ std::stringstream out; std::string tempS;
  this->owner->TokenToStringStream(out, this->Operation);
  if (this->Operation==Parser::tokenFunction)
  { out << " " << this->owner->theFunctionList[this->intValue].functionName;

  }
  out << "; #implied vars: " << this->impliedNumVars;
  if (this->children.size>0)
  { out << ". Children: ";
    for (int i=0; i<this->children.size; i++)
      out << this->children.TheObjects[i] << ", ";
  }
  out << this->ElementToStringValueAndType(false, theGlobalVariables, false, theFormat);
  output=out.str();
}

bool HomomorphismSemisimpleLieAlgebra::ComputeHomomorphismFromImagesSimpleChevalleyGenerators
(GlobalVariables& theGlobalVariables)
{ this->theDomain().ComputeChevalleyConstantS(theGlobalVariables);
  this->theRange().ComputeChevalleyConstantS(theGlobalVariables);
  int theDomainDimension= this->theDomain().theWeyl.CartanSymmetric.NumRows;
  Selection NonExplored;
  int numRoots= this->theDomain().theWeyl.RootSystem.size;
  NonExplored.init(numRoots);
  NonExplored.MakeFullSelection();
  Vector<Rational> domainRoot, rangeRoot;
  List<ElementSemisimpleLieAlgebra> tempDomain, tempRange;
  tempDomain.SetSize(numRoots+theDomainDimension);
  tempRange.SetSize(numRoots+theDomainDimension);
  Vector<Rational> tempRoot;
  for (int i=0; i<theDomainDimension; i++)
  { tempRoot.MakeEi(theDomainDimension, i);
    for (int j=0; j<2; j++, tempRoot.Minus())
    { int index= this->theDomain().theWeyl.RootSystem.GetIndex(tempRoot);
      tempDomain.TheObjects[index].MakeZero(*this->owners, this->indexDomain);
      ChevalleyGenerator tempGen;
      tempGen.MakeGenerator
      (*this->owners, this->indexDomain, this->theDomain().GetGeneratorFromRoot(tempRoot));
      tempDomain.TheObjects[index].AddMonomial(tempGen, 1);
      tempRange.TheObjects[index] = this->imagesSimpleChevalleyGenerators.TheObjects[i+j*theDomainDimension];
      NonExplored.RemoveSelection(index);
//      std::cout <<"<br>" << tempDomain.TheObjects[index].ElementToStringNegativeRootSpacesFirst(false, true, this->theDomain);
//      std::cout <<"->" << tempRange.TheObjects[index].ElementToStringNegativeRootSpacesFirst(false, true, this->theRange);
    }
  }
//  std::cout << this->ToString(theGlobalVariables) << "<br>";
  Vector<Rational> right;
  while (NonExplored.CardinalitySelection>0)
  { for (int i=0; i<NonExplored.CardinalitySelection; i++)
    { int theIndex = NonExplored.elements[i];
      Vector<Rational>& current = this->theDomain().theWeyl.RootSystem.TheObjects[theIndex];
      for (int j=0; j<NonExplored.MaxSize; j++)
        if (!NonExplored.selected[j])
        { Vector<Rational>& left= this->theDomain().theWeyl.RootSystem.TheObjects[j];
          right= current-left;
//          left.ComputeDebugString(); right.ComputeDebugString(); current.ComputeDebugString();
          if (this->theDomain().theWeyl.IsARoot(right))
          { int leftIndex= this->theDomain().theWeyl.RootSystem.GetIndex(left);
            int rightIndex= this->theDomain().theWeyl.RootSystem.GetIndex(right);
            if (!NonExplored.selected[rightIndex])
            { ElementSemisimpleLieAlgebra& leftDomainElt=tempDomain[leftIndex];
              ElementSemisimpleLieAlgebra& rightDomainElt= tempDomain[rightIndex];
              this->theDomain().LieBracket(leftDomainElt, rightDomainElt, tempDomain[theIndex]);
              ElementSemisimpleLieAlgebra& leftRangeElt=tempRange[leftIndex];
              ElementSemisimpleLieAlgebra& rightRangeElt= tempRange[rightIndex];
              this->theRange().LieBracket(leftRangeElt, rightRangeElt, tempRange[theIndex]);
              NonExplored.RemoveSelection(theIndex);
              break;
            }
          }
        }
    }
  }
  for (int i=0; i<theDomainDimension; i++)
  { tempRoot.MakeEi(theDomainDimension, i);
    int leftIndex= this->theDomain().theWeyl.RootSystem.GetIndex(tempRoot);
    int rightIndex= this->theDomain().theWeyl.RootSystem.GetIndex(-tempRoot);
    this->theDomain().LieBracket(tempDomain.TheObjects[leftIndex], tempDomain.TheObjects[rightIndex], tempDomain.TheObjects[numRoots+i]);
    this->theRange().LieBracket(tempRange.TheObjects[leftIndex], tempRange.TheObjects[rightIndex], tempRange.TheObjects[numRoots+i]);
  }
  Vectors<Rational> vectorsLeft, vectorsRight;
  vectorsLeft.SetSize(tempDomain.size);
  vectorsRight.SetSize(tempDomain.size);
  for (int i=0; i<tempRange.size; i++)
  { tempDomain[i].ElementToVectorNegativeRootSpacesFirst(vectorsLeft.TheObjects[i]);
    tempRange[i].ElementToVectorNegativeRootSpacesFirst(vectorsRight.TheObjects[i]);
  }
  Matrix<Rational> tempMat;
  tempMat.MakeLinearOperatorFromDomainAndRange(vectorsLeft, vectorsRight);
  Vector<Rational> imageRoot;
  this->domainAllChevalleyGenerators.SetSize(tempDomain.size);
  this->imagesAllChevalleyGenerators.SetSize(tempDomain.size);
  ElementSemisimpleLieAlgebra tempElt;
  for (int i=0; i<this->owners->TheObjects[this->indexDomain].GetNumGenerators(); i++)
  { this->domainAllChevalleyGenerators[i].MakeGenerator(i, *this->owners, this->indexDomain);
  }
  for (int i=0; i<this->imagesAllChevalleyGenerators.size; i++)
  { this->domainAllChevalleyGenerators[i].ElementToVectorNegativeRootSpacesFirst(tempRoot);
    tempMat.ActOnVectorColumn(tempRoot, imageRoot);
    this->imagesAllChevalleyGenerators[i].AssignVectorNegRootSpacesCartanPosRootSpaces
    (imageRoot, this->theRange());
//    std::cout << this->domainAllChevalleyGenerators.TheObjects[i].ElementToStringNegativeRootSpacesFirst(false, true, this->theDomain);
//    std::cout << "->" << this->imagesAllChevalleyGenerators.TheObjects[i].ElementToStringNegativeRootSpacesFirst(false, true, this->theRange);
//    std::cout << "<br>";
  }
  return true;
}

void HomomorphismSemisimpleLieAlgebra::ProjectOntoSmallCartan(Vectors<Rational>& input, Vectors<Rational>& output, GlobalVariables& theGlobalVariables)
{ output.SetSize(input.size);
  for (int i=0; i<input.size; i++)
    this->ProjectOntoSmallCartan(input.TheObjects[i], output.TheObjects[i], theGlobalVariables);
}

void HomomorphismSemisimpleLieAlgebra::ProjectOntoSmallCartan
(Vector<Rational>& input, Vector<Rational>& output, GlobalVariables& theGlobalVariables)
{ Matrix<Rational>  invertedSmallCartan;
  invertedSmallCartan=this->theDomain().theWeyl.CartanSymmetric;
  invertedSmallCartan.Invert(theGlobalVariables);
  int theSmallDimension=this->theDomain().theWeyl.CartanSymmetric.NumRows;
  output.MakeZero(theSmallDimension);
  for (int i=0; i<theSmallDimension; i++)
    output.TheObjects[i]= this->theRange().theWeyl.RootScalarCartanRoot
    (this->imagesAllChevalleyGenerators.TheObjects[this->theDomain().theWeyl.RootsOfBorel.size+i].GetCartanPart(), input);
  invertedSmallCartan.ActOnVectorColumn(output, output);
}

bool HomomorphismSemisimpleLieAlgebra::ApplyHomomorphism
(MonomialUniversalEnveloping<RationalFunction>& input, const RationalFunction& theCoeff,
 ElementUniversalEnveloping<RationalFunction>& output, GlobalVariables& theGlobalVariables)
{ ElementUniversalEnveloping<RationalFunction> tempElt;
  output.MakeZero(*this->owners, this->indexRange);
  RationalFunction polyOne;
  polyOne=theCoeff.GetOne();
  output.MakeConst(theCoeff, *this->owners, this->indexRange);
  for (int i=0; i<input.generatorsIndices.size; i++)
  { if (input.generatorsIndices[i]>=this->imagesAllChevalleyGenerators.size)
      return false;
    tempElt.AssignElementLieAlgebra
    (this->imagesAllChevalleyGenerators[input.generatorsIndices[i]],
     *this->owners, this->indexRange, polyOne, polyOne.GetZero());
    RationalFunction& thePower=input.Powers[i];
    int theIntegralPower;
    if (!thePower.IsSmallInteger(&theIntegralPower))
      return false;
    for (int j=0; j<theIntegralPower; j++)
      output*=tempElt;
  }
  return true;
}

bool HomomorphismSemisimpleLieAlgebra::ApplyHomomorphism
(ElementUniversalEnveloping<RationalFunction>& input,
 ElementUniversalEnveloping<RationalFunction>& output, GlobalVariables& theGlobalVariables)
{ assert(&output!=&input);
  output.MakeZero(*this->owners, this->indexRange);
  ElementUniversalEnveloping<RationalFunction> tempElt;
  for (int i=0; i<input.size; i++)
  { if(!this->ApplyHomomorphism(input[i], input.theCoeffs[i], tempElt, theGlobalVariables))
      return false;
    output+=tempElt;
  }
  return true;
}

void HomomorphismSemisimpleLieAlgebra::MakeGinGWithId
(char theWeylLetter, int theWeylDim, List<SemisimpleLieAlgebra>& ownerOfAlgebras, GlobalVariables& theGlobalVariables)
{ this->owners=&ownerOfAlgebras;
  this->owners->SetSize(2);
  this->theDomain().indexInOwner=0;
  this->theDomain().owner=this->owners;
  this->theRange().indexInOwner=1;
  this->theRange().owner=this->owners;
  this->theDomain().theWeyl.MakeArbitrary(theWeylLetter, theWeylDim);
  this->theRange().theWeyl.MakeArbitrary(theWeylLetter, theWeylDim);
  this->theDomain().ComputeChevalleyConstantS(theGlobalVariables);
  this->theRange().ComputeChevalleyConstantS(theGlobalVariables);
  int numPosRoots=this->theDomain().theWeyl.RootsOfBorel.size;
  this->imagesAllChevalleyGenerators.SetSize(numPosRoots*2+theWeylDim);
  this->domainAllChevalleyGenerators.SetSize(numPosRoots*2+theWeylDim);
  this->imagesSimpleChevalleyGenerators.SetSize(theWeylDim*2);
  for (int i=0; i<2*numPosRoots+theWeylDim; i++)
  { ElementSemisimpleLieAlgebra& tempElt1=this->imagesAllChevalleyGenerators.TheObjects[i];
    ElementSemisimpleLieAlgebra& tempElt2=this->domainAllChevalleyGenerators.TheObjects[i];
    tempElt2.MakeGenerator
    (i, *this->owners, this->indexDomain);
    tempElt1.MakeGenerator
    (i, *this->owners, this->indexRange);
  }
  for (int i=0; i<theWeylDim; i++)
  { ElementSemisimpleLieAlgebra& tempElt1=this->imagesSimpleChevalleyGenerators.TheObjects[i];
    tempElt1.MakeGenerator
    (i, *this->owners, this->indexRange);
    ElementSemisimpleLieAlgebra& tempElt2=this->imagesSimpleChevalleyGenerators.TheObjects[theWeylDim+i];
    tempElt2.MakeGenerator
    (i+numPosRoots, *this->owners, this->indexRange);
  }
//  std::cout << this->ToString(theGlobalVariables);
}

void HomomorphismSemisimpleLieAlgebra::ToString
(std::string& output, bool useHtml, GlobalVariables& theGlobalVariables)
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
  for (int i=0; i<this->imagesSimpleChevalleyGenerators.size; i++)
  { out <<  this->imagesSimpleChevalleyGenerators[i].ToString(&theGlobalVariables.theDefaultFormat) << "\n\n";
    if (useHtml)
      out << "<br>";
  }
  out << "Maps of Chevalley generators:\n\n";
  for (int i=0; i<this->domainAllChevalleyGenerators.size; i++)
  { out << "<br>"
    << this->domainAllChevalleyGenerators[i].ToString(&theGlobalVariables.theDefaultFormat)
    << " \\mapsto "
    << this->imagesAllChevalleyGenerators[i].ToString(&theGlobalVariables.theDefaultFormat)
    ;
  }
  output=out.str();
}

void ParserNode::EvaluateFunction(GlobalVariables& theGlobalVariables)
{ if (this->intValue<this->owner->theFunctionList.size && this->intValue>=0)
    this->owner->theFunctionList[this->intValue].CallMe(*this, theGlobalVariables);
  else
    this->SetError(this->errorUnknownOperation);
}

void ParserNode::EvaluateOrder(GlobalVariables& theGlobalVariables)
{ if (this->children.size!=1)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  ParserNode& theArgument=this->owner->TheObjects[this->children[0]];
  this->impliedNumVars=theArgument.impliedNumVars;
  if (!theArgument.ConvertToType(this->typeUEElementOrdered, this->impliedNumVars, theGlobalVariables))
  { this->SetError(this->errorConversionError);
    return;
  }
  this->UEElementOrdered.GetElement()=theArgument.UEElementOrdered.GetElement();
  this->ExpressionType=this->typeUEElementOrdered;
}

void ParserNode::EvaluatePrintWeyl(GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  WeylGroup& theWeyl=this->owner->theHmm.theRange().theWeyl;
  theWeyl.ComputeWeylGroup(51840);
  out << "<br>Symmetric Cartan matrix in Bourbaki order:<br><div class=\"math\">"
  << this->owner->theHmm.theRange().theWeyl.CartanSymmetric.ToString(false, true) << "</div>Root system:";
  out << "<br>Size of Weyl group by formula: " << theWeyl.GetSizeWeylByFormula(theWeyl.WeylLetter, theWeyl.GetDim()).ToString();
  out << "<br>Explicitly computed elements: " << theWeyl.size;
  out << "<br>Each element is parametrized by a list of simple reflections. <br>For example an element parametrized by 1,2 corresponds to the element <span class=\"math\">s_{\\alpha_1}s_{\\alpha_2}</span>,";
  out << " where <span class=\"math\">s_{\\beta}</span> is the reflection with respect to <span class=\"math\">\\beta</span> and <span class=\"math\">\\alpha_i</span> is the i^th simple Vector<Rational>";
  for (int i=0; i<theWeyl.size; i++)
  { ElementWeylGroup& current=theWeyl.TheObjects[i];
    out << "<br>";
    if (current.size==0)
      out << "id";
    else
      for (int j=0; j<current.size; j++)
        out << current.TheObjects[j]+1 << ",";
  }
  this->outputString=out.str();
  this->ExpressionType=this->typeString;
}

class slTwoInSlN;

void ParserNode::EvaluateOuterAutos(GlobalVariables& theGlobalVariables)
{ Matrix<Rational>  tempMat;
  std::stringstream out;
  this->ExpressionType=this->typeString;
  this->owner->theHmm.theRange().ComputeOneAutomorphism(theGlobalVariables, tempMat, true);
  Matrix<Rational>  tempMat2=tempMat;
  Rational tempRat;
  tempMat2.ComputeDeterminantOverwriteMatrix(tempRat);
  out << "<br>one outer automorphism of the Lie algebra is realized as the following matrix. <br> The coordinates of the matrix are given in the ordered basis ";
  out << " <div class=\"math\" scale=\"50\"> g_{-n}, \\dots, g_{-1}, h_1,\\dots, h_k, g_1,\\dots, g_n</div> where the generators are as in the table on the right.<br> Its determinant is \n" << tempRat.ToString();
  out << "<div class=\"math\" scale=\"50\">" << tempMat.ToString(false, true) << "</div>";
  this->outputString=out.str();
}

bool Parser::LookUpInDictionaryAndAdd(std::string& input)
{ switch (input.at(0))
  { case '0': this->TokenBuffer.AddOnTop(Parser::tokenDigit); this->ValueBuffer.AddOnTop(0); return true;
    case '1': this->TokenBuffer.AddOnTop(Parser::tokenDigit); this->ValueBuffer.AddOnTop(1); return true;
    case '2': this->TokenBuffer.AddOnTop(Parser::tokenDigit); this->ValueBuffer.AddOnTop(2); return true;
    case '3': this->TokenBuffer.AddOnTop(Parser::tokenDigit); this->ValueBuffer.AddOnTop(3); return true;
    case '4': this->TokenBuffer.AddOnTop(Parser::tokenDigit); this->ValueBuffer.AddOnTop(4); return true;
    case '5': this->TokenBuffer.AddOnTop(Parser::tokenDigit); this->ValueBuffer.AddOnTop(5); return true;
    case '6': this->TokenBuffer.AddOnTop(Parser::tokenDigit); this->ValueBuffer.AddOnTop(6); return true;
    case '7': this->TokenBuffer.AddOnTop(Parser::tokenDigit); this->ValueBuffer.AddOnTop(7); return true;
    case '8': this->TokenBuffer.AddOnTop(Parser::tokenDigit); this->ValueBuffer.AddOnTop(8); return true;
    case '9': this->TokenBuffer.AddOnTop(Parser::tokenDigit); this->ValueBuffer.AddOnTop(9); return true;
    case '*': this->TokenBuffer.AddOnTop(Parser::tokenTimes); this->ValueBuffer.AddOnTop(9); return true;
    case '}': this->TokenBuffer.AddOnTop(Parser::tokenCloseCurlyBracket); this->ValueBuffer.AddOnTop(0); return true;
    case '{': this->TokenBuffer.AddOnTop(Parser::tokenOpenCurlyBracket); this->ValueBuffer.AddOnTop(0); return true;
    case '[': this->TokenBuffer.AddOnTop(Parser::tokenOpenLieBracket); this->ValueBuffer.AddOnTop(0); return true;
    case ']': this->TokenBuffer.AddOnTop(Parser::tokenCloseLieBracket); this->ValueBuffer.AddOnTop(0); return true;
    case '(': this->TokenBuffer.AddOnTop(Parser::tokenOpenBracket); this->ValueBuffer.AddOnTop(0); return true;
    case ',': this->TokenBuffer.AddOnTop(Parser::tokenComma); this->ValueBuffer.AddOnTop(0); return true;
    case '.': this->TokenBuffer.AddOnTop(Parser::tokenDot); this->ValueBuffer.AddOnTop(0); return true;
    case ')': this->TokenBuffer.AddOnTop(Parser::tokenCloseBracket); this->ValueBuffer.AddOnTop(0); return true;
    case '^': this->TokenBuffer.AddOnTop(Parser::tokenPower); this->ValueBuffer.AddOnTop(0); return true;
    case '+': this->TokenBuffer.AddOnTop(Parser::tokenPlus); this->ValueBuffer.AddOnTop(0); return true;
    case '-': this->TokenBuffer.AddOnTop(Parser::tokenMinus); this->ValueBuffer.AddOnTop(0); return true;
    case '_': this->TokenBuffer.AddOnTop(Parser::tokenUnderscore); this->ValueBuffer.AddOnTop(0); return true;
    case ';': this->TokenBuffer.AddOnTop(Parser::tokenEndStatement); this->ValueBuffer.AddOnTop(0); return true;
    case ':': this->TokenBuffer.AddOnTop(Parser::tokenColon); this->ValueBuffer.AddOnTop(0); return true;
    case '/': this->TokenBuffer.AddOnTop(Parser::tokenDivide); this->ValueBuffer.AddOnTop(0); return true;
    default: break;
  }
  ParserFunction tempPF;
  tempPF.functionName=input;
  int functionIndex= this->theFunctionList.GetIndex(tempPF);
  if (functionIndex!=-1)
  { if (this->theFunctionList.TheObjects[functionIndex].theArguments.functionArguments.size==0)
      this->TokenBuffer.AddOnTop(Parser::tokenFunctionNoArgument);
    else
      this->TokenBuffer.AddOnTop(Parser::tokenFunction);
    this->ValueBuffer.AddOnTop(functionIndex);
    return true;
  }
  if (input == "\\\\&&")
  { return true;
  }
  if (input=="NoIndicator")
  { this->flagDisplayIndicator=false;
    return true;
  }
  if (input=="x")
  { this->TokenBuffer.AddOnTop(Parser::tokenX);
    this->ValueBuffer.AddOnTop(0);
    return true;
  }
  if (input=="d")
  { this->TokenBuffer.AddOnTop(Parser::tokenPartialDerivative);
    this->ValueBuffer.AddOnTop(0);
    return true;
  }
  if (input=="\\partial")
  { this->TokenBuffer.AddOnTop(Parser::tokenPartialDerivative);
    this->ValueBuffer.AddOnTop(0);
    return true;
  }
  if (input=="g")
  { this->TokenBuffer.AddOnTop(Parser::tokenG);
    this->ValueBuffer.AddOnTop(0);
    return true;
  }
  if (input=="f")
  { this->TokenBuffer.AddOnTop(Parser::tokenF);
    this->ValueBuffer.AddOnTop(0);
    return true;
  }
  if (input =="h")
  { this->TokenBuffer.AddOnTop(Parser::tokenH);
    this->ValueBuffer.AddOnTop(0);
    return true;
  }
  if (input=="c")
  { this->TokenBuffer.AddOnTop(Parser::tokenC);
    this->ValueBuffer.AddOnTop(0);
    return true;
  }
  if (input =="i")
  { this->TokenBuffer.AddOnTop(Parser::tokenEmbedding);
    this->ValueBuffer.AddOnTop(0);
    return true;
  }
  if (input=="n")
  { this->TokenBuffer.AddOnTop(Parser::tokenVariable);
    this->ValueBuffer.AddOnTop(0);
    return true;
  }
  if (input=="\\mapsto")
  { this->TokenBuffer.AddOnTop(Parser::tokenMapsTo);
    this->ValueBuffer.AddOnTop(0);
    return true;
  }
  return false;
}

void HomomorphismSemisimpleLieAlgebra::GetRestrictionAmbientRootSystemToTheSmallerCartanSA
(Vectors<Rational>& output, GlobalVariables& theGlobalVariables)
{ List<Vector<Rational> >& theRootSystem= this->theRange().theWeyl.RootSystem;
  int rankSA=this->theDomain().theWeyl.GetDim();
  Matrix<Rational>  tempMat;
  tempMat=(this->theDomain().theWeyl.CartanSymmetric);
  tempMat.Invert(theGlobalVariables);
  int numPosRootsDomain=this->theDomain().theWeyl.RootsOfBorel.size;
  output.SetSize(theRootSystem.size);
  Vector<Rational> theScalarProducts;
  theScalarProducts.SetSize(rankSA);
//  tempMat.ComputeDebugString(true, false);
//  std::cout << tempMat.DebugString << "<br>";
  for (int i=0; i<theRootSystem.size; i++)
  { for (int j=0; j<rankSA; j++)
    { ElementSemisimpleLieAlgebra& currentH=this->imagesAllChevalleyGenerators[j+numPosRootsDomain];
      theScalarProducts[j]=this->theRange().theWeyl.RootScalarCartanRoot
      (currentH.GetCartanPart(), theRootSystem[i]);
    }
    tempMat.ActOnVectorColumn(theScalarProducts, output[i]);
  }
  this->ImagesCartanDomain.SetSize(rankSA);
  for (int i=0; i<rankSA; i++)
    this->ImagesCartanDomain[i]=this->imagesAllChevalleyGenerators[i+numPosRootsDomain].GetCartanPart();
}

bool HomomorphismSemisimpleLieAlgebra::CheckClosednessLieBracket(GlobalVariables& theGlobalVariables)
{ ElementSemisimpleLieAlgebra tempElt;
  Vectors<Rational> tempRoots; Vector<Rational> tempRoot;
  tempRoots.SetSize(this->imagesAllChevalleyGenerators.size);
  for (int i=0; i<tempRoots.size; i++)
    this->imagesAllChevalleyGenerators[i].ElementToVectorNegativeRootSpacesFirst(tempRoots[i]);
  for (int i=0; i<this->imagesAllChevalleyGenerators.size; i++)
    for (int j=0; j<this->imagesAllChevalleyGenerators.size; j++)
    { this->theRange().LieBracket
      (this->imagesAllChevalleyGenerators[i], this->imagesAllChevalleyGenerators[j], tempElt);
      tempElt.ElementToVectorNegativeRootSpacesFirst(tempRoot);
      if(!tempRoots.LinSpanContainsRoot(tempRoot))
        return false;
    }
  return true;
}

int ParserNode::GetMaxImpliedNumVarsChildren()
{ int result=0;
  for (int i=0; i<this->children.size; i++)
    result=MathRoutines::Maximum(result, this->owner->TheObjects[this->children[i]].impliedNumVars);
  return result;
}

void ParserNode::Clear()
{ this->indexParentNode=-1;
  this->indexInOwner=-1;
  this->intValue=0;
  this->impliedNumVars=0;
  this->ErrorType=this->errorNoError;
  this->Operation= Parser::tokenEmpty;
  this->Evaluated=false;
  this->ExpressionType=this->typeUndefined;
  this->children.size=0;
  this->rationalValue.MakeZero();
  if (this->owner!=0)
    this->IndexContextLieAlgebra=this->owner->theHmm.indexRange;
}

std::string ChevalleyGenerator::ToString(FormatExpressions* inputFormat)const
{ if (this-> indexOfOwnerAlgebra<0 || this->ownerArray==0)
    return "(ErrorProgramming:Non-Initialized-Chevalley-Weyl-Generator)";
  return this->ownerArray->TheObjects[this->indexOfOwnerAlgebra].GetStringFromChevalleyGenerator(this->theGeneratorIndex, inputFormat);
}

std::string SemisimpleLieAlgebra::GetStringFromChevalleyGenerator
(int theIndex, FormatExpressions* thePolynomialFormat)const
{ std::stringstream out;
  MemorySaving<FormatExpressions> tempFormat;
  if (thePolynomialFormat==0)
    thePolynomialFormat=&tempFormat.GetElement();
  if (this->IsGeneratorFromCartan(theIndex))
    out << thePolynomialFormat->chevalleyHgeneratorLetter << "_{" << theIndex-this->GetNumPosRoots()+1 << "}";
  else
  { out << thePolynomialFormat->chevalleyGgeneratorLetter << "_{";
    if (theIndex>=this->GetNumPosRoots())
      out << theIndex-this->GetNumPosRoots()-this->GetRank()+1;
    else
      out << theIndex-this->GetNumPosRoots();
    out << "}";
  }
  return out.str();
}

bool SemisimpleLieAlgebra::AreOrderedProperly(int leftIndex, int rightIndex)
{ return this->UEGeneratorOrderIncludingCartanElts[leftIndex]<=
  this->UEGeneratorOrderIncludingCartanElts[rightIndex];
}

int SemisimpleLieAlgebra::GetRootIndexFromDisplayIndex(int theIndex)
{ int numPosRoots=this->theWeyl.RootsOfBorel.size;
  if (theIndex<0)
    return theIndex+numPosRoots;
  if (theIndex>0)
    return theIndex+numPosRoots-1;
  return -10000000;
}

int SemisimpleLieAlgebra::GetDisplayIndexFromRootIndex(int theIndex)const
{ int numPosRoots=this->theWeyl.RootsOfBorel.size;
  if (theIndex>=numPosRoots)
    return theIndex-numPosRoots+1;
  if (theIndex<numPosRoots)
    return theIndex-numPosRoots;
  return -10000000;
}

int SemisimpleLieAlgebra::GetGeneratorFromRootIndex(int theIndex)const
{ if (theIndex<0  || theIndex>=this->theWeyl.RootSystem.size)
    return -1;
  int theDimension=this->theWeyl.CartanSymmetric.NumRows;
  int numPosRoots=this->theWeyl.RootsOfBorel.size;
  if (theIndex>=numPosRoots)
    return theIndex+theDimension;
  return theIndex;
}

int SemisimpleLieAlgebra::GetRootIndexFromGenerator(int theIndex)const
{ int numPosRoots=this->theWeyl.RootsOfBorel.size;
  int theDimension=this->theWeyl.CartanSymmetric.NumRows;
  if (theIndex<numPosRoots)
    return theIndex;
  if (theIndex>=numPosRoots+theDimension)
    return theIndex-theDimension;
  return -1;
}

void CGI::MakeSureWeylGroupIsSane(char& theWeylLetter, int& theRank)
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
  if (theWeylLetter!='A' && theRank==1)
    theRank=2;
  if (theWeylLetter=='E' && theRank<6)
    theRank=6;
  if (theWeylLetter=='F')
    theRank=4;
  if (theWeylLetter=='G')
    theRank=2;
  if (theWeylLetter=='D' && theRank <4)
    theRank=4;
}

void CGI::ReplaceEqualitiesAndAmpersantsBySpaces(std::string& inputOutput)
{ for (int i=0; i<(signed)inputOutput.size(); i++)
    if (inputOutput[i]=='=' || inputOutput[i]=='&')
      inputOutput[i]=' ';
}

void VectorPartition::ComputeAllPartitions()
{ List<int> currentPartition;
  currentPartition.initFillInObject(this->PartitioningRoots.size, 0);
  this->thePartitions.size=0;
  this->ComputeAllPartitionsRecursive(0, currentPartition, -1, theRoot);
}

void VectorPartition::ComputeAllPartitionsRecursive(int currentIndex, List<int>& CurrentPartition, int UpperBoundEachIndex, Vector<Rational>& toBePartitioned)
{ if (currentIndex>=this->PartitioningRoots.size)
    return;
  Vector<Rational> currentRoot=toBePartitioned;
  while (currentRoot.IsPositiveOrZero() && (CurrentPartition.TheObjects[currentIndex]<=UpperBoundEachIndex || UpperBoundEachIndex==-1))
  { if (currentRoot.IsEqualToZero())
      this->thePartitions.AddOnTop(CurrentPartition);
    else
    { this->ComputeAllPartitionsRecursive(currentIndex+1, CurrentPartition , UpperBoundEachIndex, currentRoot);
      for (int i=currentIndex+1; i<CurrentPartition.size; i++)
        CurrentPartition[i]=0;
    }
    currentRoot-=(this->PartitioningRoots[currentIndex]);
    CurrentPartition[currentIndex]++;
  }
}

std::string VectorPartition::ToString(bool useHtml)
{ std::stringstream out;
  out << this->theRoot.ToString() << "\n\n";
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
        out << this->PartitioningRoots.TheObjects[j].ToString();
      }
    }
    out << "\n\n";
    if (useHtml)
      out << "<br>\n";
  }
  return out.str();
}

std::string RationalFunction::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  //out << "( Number variables: " << this->NumVars << ", hash: " << this->HashFunction() << ")";
  if (this->expressionType==this->typeRational)
  { //out << "(type: rational)";
    out << this->ratValue.ToString();
    return out.str();
  }
  bool hasDenominator=(this->expressionType==this->typeRationalFunction);
  //if (hasDenominator)
    //out << "(type: honest RF)";
  //else
    //out << "(type: poly)";
//  if (hasDenominator && useLatex)
//    out << "\\frac{";
  out << this->Numerator.GetElementConst().ToString(theFormat);
  if (hasDenominator)
  { //if (useLatex)
    //  out << "}{";
    out << "/(" << this->Denominator.GetElementConst().ToString(theFormat) << ")";
    //if (useLatex)
    //  out << "}";
  }
  return out.str();
}

void RationalFunction::RemainderDivisionWithRespectToBasis
(Polynomial<Rational> & input, List<Polynomial<Rational> >& theBasis, Polynomial<Rational> & outputRemainder, Polynomial<Rational> & buffer1,
 Polynomial<Rational> & buffer2, MonomialP& bufferMon1,
 bool (*MonomialOrderLeftIsGreaterThanOrEqualToRight) (const MonomialP& left, const MonomialP& right)
 )
{ assert(&outputRemainder!=&input);
  Polynomial<Rational> * currentRemainder=&input;
  Polynomial<Rational> * nextRemainder=&buffer1;
  for (int i=0; i<theBasis.size; i++)
  { RationalFunction::RemainderDivision(*currentRemainder, theBasis.TheObjects[i], *nextRemainder, buffer2, bufferMon1, MonomialOrderLeftIsGreaterThanOrEqualToRight);
    MathRoutines::swap(currentRemainder, nextRemainder);
    if (currentRemainder->IsEqualToZero())
      break;
  }
  if (currentRemainder!=&outputRemainder)
    outputRemainder=(*currentRemainder);
}

void RationalFunction::RemainderDivision
(Polynomial<Rational> & input, Polynomial<Rational> & divisor, Polynomial<Rational> & outputRemainder,
 Polynomial<Rational> & buffer, MonomialP& bufferMon1,
   bool (*MonomialOrderLeftIsGreaterThanOrEqualToRight) (const MonomialP& left, const MonomialP& right)
 )
{ assert(&input!=&outputRemainder);
  outputRemainder=(input);
  int divisorHighest=divisor.GetIndexMaxMonomial(MonomialOrderLeftIsGreaterThanOrEqualToRight);
  MonomialP& highestMonDivisor=divisor[divisorHighest];
  int remainderHighest=-1;
  int theNumVars=input.NumVars;
  bufferMon1.MakeConst((int)theNumVars);
  assert(input.NumVars==theNumVars);
  assert(divisor.NumVars==theNumVars);
  for(;;)
  { if (outputRemainder.size==0)
      return;
    remainderHighest=  outputRemainder.GetIndexMaxMonomial(MonomialOrderLeftIsGreaterThanOrEqualToRight);
    MonomialP& highestMonRemainder=outputRemainder[remainderHighest];
    /*outputRemainder.ComputeDebugString();
    highestMonRemainder.ComputeDebugString();
    highestMonDivisor.ComputeDebugString();*/
    for (int i=0; i<theNumVars; i++)
    { if (highestMonRemainder[i]<highestMonDivisor[i])
        return;
      bufferMon1[i]=highestMonRemainder[i]-highestMonDivisor[i];
    }
    Rational tempCoeff=outputRemainder.theCoeffs[remainderHighest]/divisor.theCoeffs[divisorHighest];
    buffer=divisor;
    buffer.MultiplyBy(bufferMon1, tempCoeff);
    outputRemainder-=(buffer);/*
    outputRemainder.ComputeDebugString();*/
  }
}

void RationalFunction::TransformToReducedGroebnerBasis
  (List<Polynomial<Rational> >& theBasis, Polynomial<Rational> & buffer1, Polynomial<Rational> & buffer2, Polynomial<Rational> & buffer3, Polynomial<Rational> & buffer4, MonomialP& bufferMon1, MonomialP& bufferMon2,
  bool (*MonomialOrderLeftIsGreaterThanOrEqualToRight) (const MonomialP& left, const MonomialP& right),
   GlobalVariables* theGlobalVariables
 )
{ Polynomial<Rational>& tempP=buffer1;
  Polynomial<Rational>& Spoly=buffer2;
  MonomialP& leftShift=bufferMon1;
  MonomialP& rightShift=bufferMon2;
  int theNumVars=theBasis.TheObjects[0].NumVars;
  leftShift.MakeConst(theNumVars);
  rightShift.MakeConst(theNumVars);
 // std::string tempS;
  for (int lowestNonExplored=0; lowestNonExplored< theBasis.size; lowestNonExplored++)
  { //warning! currentPoly may expire if theBasis.TheObjects changes size
//    Polynomial<Rational> & currentPoly=;
    for (int j=0; j<theBasis.size; j++)
    { Polynomial<Rational> & currentLeft= theBasis.TheObjects[lowestNonExplored];
      Polynomial<Rational> & currentRight= theBasis.TheObjects[j];
      int leftIndex=currentLeft.GetIndexMaxMonomial(MonomialOrderLeftIsGreaterThanOrEqualToRight);
      int rightIndex=currentRight.GetIndexMaxMonomial(MonomialOrderLeftIsGreaterThanOrEqualToRight);
      MonomialP& leftHighestMon=currentLeft[leftIndex];
      MonomialP& rightHighestMon=currentRight[rightIndex];
      Rational leftCoeff=-currentLeft.theCoeffs[leftIndex];
      Rational rightCoeff=currentRight.theCoeffs[rightIndex];
      for (int k=0; k<leftHighestMon.monBody.size; k++)
        if (leftHighestMon[k]>rightHighestMon[k])
        { rightShift[k]=leftHighestMon[k]-rightHighestMon[k];
          leftShift[k]=0;
        }
        else
        { leftShift[k]=rightHighestMon[k]-leftHighestMon[k];
          rightShift[k]=0;
        }
      currentLeft.MultiplyBy(leftShift, leftCoeff, tempP);
      currentRight.MultiplyBy(rightShift, rightCoeff, Spoly);
      Spoly+=(tempP);
      //Spoly.ComputeDebugString();
      //std::cout << "<br> Spoly found: " << Spoly.DebugString;
//      theBasis.ElementToStringGeneric(tempS);
      RationalFunction::RemainderDivisionWithRespectToBasis
      (Spoly, theBasis, tempP, buffer3, buffer4, bufferMon1, MonomialOrderLeftIsGreaterThanOrEqualToRight);

      //tempP.ComputeDebugString();
      if (!tempP.IsEqualToZero())
      { tempP.ScaleToIntegralNoGCDCoeffs();
        theBasis.AddOnTop(tempP);
        //std::cout << "<br> new element found: " << tempP.ToString();
      }
      if (theGlobalVariables!=0)
        if (theGlobalVariables->GetFeedDataToIndicatorWindowDefault()!=0)
        { std::stringstream out;
          out << "<br> Exploring element " << lowestNonExplored+1 << " out of " << theBasis.size;
          theGlobalVariables->theIndicatorVariables.ProgressReportStringsNeedRefresh=true;
          theGlobalVariables->theIndicatorVariables.ProgressReportStrings[0]=out.str();
          theGlobalVariables->MakeReport();
        }
    }
  }
//  std::cout << "<br> ... and the basis before reduction is: <br>";
//  for (int i=0; i<theBasis.size; i++)
//    std::cout << theBasis.TheObjects[i].ToString() << ", ";
  RationalFunction::ReduceGroebnerBasis(theBasis, buffer1, MonomialOrderLeftIsGreaterThanOrEqualToRight);

}

void RationalFunction::ReduceGroebnerBasis
(List<Polynomial<Rational> >& theBasis, Polynomial<Rational> & buffer1,
 bool (*MonomialOrderLeftIsGreaterThanOrEqualToRight) (const MonomialP& left, const MonomialP& right)
 )
{ Polynomial<Rational>& LeadingCoeffs=buffer1;
  LeadingCoeffs.ReservE(theBasis.size);
  LeadingCoeffs.MakeZero(theBasis[0].NumVars);
  List<MonomialP> tempList;
//  std::cout << "<br> ... and the leading coefficients are: <br>";
  for (int i=0; i<theBasis.size; i++)
  { Polynomial<Rational> & current=theBasis.TheObjects[i];
    LeadingCoeffs.AddMonomial
    (current.TheObjects[current.GetIndexMaxMonomial(MonomialOrderLeftIsGreaterThanOrEqualToRight)], 1);
    *LeadingCoeffs.theCoeffs.LastObject()=1;
//    LeadingCoeffs.LastObject()->ComputeDebugString();
//    std::cout << LeadingCoeffs.LastObject()->DebugString << ", ";
  }
  tempList.CopyFromBase(LeadingCoeffs);
  tempList.QuickSortAscending();
//  std::cout << "<br><br> and the sorted leading monomials are: ";
//  for (int i=0; i<theBasis.size; i++)
//  { tempList.TheObjects[i].ComputeDebugString();
//    std::cout << tempList.TheObjects[i].DebugString << ", ";
//  }
  for (int i=0; i<LeadingCoeffs.size; i++)
  { MonomialP& currentMon=LeadingCoeffs.TheObjects[i];
    for (int j=0; j<LeadingCoeffs.size; j++)
      if (i!=j)
        if (currentMon.IsDivisibleBy(LeadingCoeffs.TheObjects[j]))
        { LeadingCoeffs.PopIndexSwapWithLast(i);
          theBasis.PopIndexSwapWithLast(i);
          i--;
          break;
        }
  }
}

void RationalFunction::gcd
(const Polynomial<Rational>& left, const Polynomial<Rational>& right, Polynomial<Rational>& output,
 Polynomial<Rational>& buffer1, Polynomial<Rational>& buffer2, Polynomial<Rational>& buffer3,
 Polynomial<Rational>& buffer4, Polynomial<Rational>& buffer5, MonomialP& bufferMon1,
 MonomialP& bufferMon2, List<Polynomial<Rational> >& bufferList)
{ RationalFunction::lcm(left, right, buffer1, buffer2, buffer3, buffer4, buffer5, bufferMon1, bufferMon2, bufferList);
  buffer2=left;
  buffer2*=right;
//  std::cout << "<hr>the product: " << buffer2.ToString() << " and the lcm: " << buffer1.ToString() << "<br>";
  buffer2.DivideBy(buffer1, output, buffer3);
//  std::cout << "<br>and the result of gcd (product/lcm)= " << output.ToString() << "<hr>";
}

void RationalFunction::lcm
(const Polynomial<Rational>& left, const Polynomial<Rational>& right, Polynomial<Rational>& output,
 Polynomial<Rational>& buffer1, Polynomial<Rational> & buffer2, Polynomial<Rational>& buffer3,
 Polynomial<Rational>& buffer4, MonomialP& bufferMon1, MonomialP& bufferMon2, List<Polynomial<Rational> >& bufferList)
{ Polynomial<Rational>& leftTemp=buffer1;
  Polynomial<Rational>& rightTemp=buffer2;
  Polynomial<Rational>& tempP=buffer3;
  List<Polynomial<Rational> >& tempList=bufferList;
  leftTemp=(left);
  rightTemp=(right);
  assert(left.NumVars==right.NumVars);
  int theNumVars=left.NumVars;
  leftTemp.SetNumVariablesSubDeletedVarsByOne((int)theNumVars+1);
  rightTemp.SetNumVariablesSubDeletedVarsByOne((int)theNumVars+1);
  leftTemp.ScaleToIntegralNoGCDCoeffs();
  rightTemp.ScaleToIntegralNoGCDCoeffs();
  tempP.MakeMonomial((int)theNumVars+1,(int) theNumVars, 1, (Rational) 1);
  leftTemp*=(tempP);
  tempP*=-1;
  tempP+=1;
  rightTemp*=(tempP);
  tempList.size=0;
  tempList.AddOnTop(leftTemp);
  tempList.AddOnTop(rightTemp);
//  std::cout << "<br>In the beginning: <br>";
//  for (int i=0; i<tempList.size; i++)
//  { std::cout << "the groebner basis element with index " << i << " is " << tempList.TheObjects[i].ToString() << "<br>\n";
//  }
  RationalFunction::TransformToReducedGroebnerBasis(tempList, buffer1, buffer2, buffer3, buffer4, bufferMon1, bufferMon2, 0);
//  std::cout << "<br><br> ... and the basis is: <br>";
//  for (int i=0; i<tempList.size; i++)
//  { std::cout << tempList.TheObjects[i].ToString() << "<br>\n";
//  }
  for(int i=0; i<tempList.size; i++)
  { Polynomial<Rational> & current=tempList.TheObjects[i];
    MonomialP& currentMon= current.TheObjects[current.GetIndexMaxMonomialLexicographicLastVariableStrongest()];
    //currentMon.ComputeDebugString();
    if (currentMon[theNumVars]==0)
    { output=current;
//      std::cout << "<br> the highest mon is: " << currentMon.DebugString << "<br>";
      output.SetNumVariablesSubDeletedVarsByOne((int)theNumVars);
      return;
    }
  }
  output.MakeZero(theNumVars);
}

void RationalFunction::operator*=(const MonomialP& other)
{ Polynomial<Rational> otherP;
  otherP.MakeConst(other.monBody.size);
  otherP.AddMonomial(other, 1);
  (*this)*=otherP;
}

void RationalFunction::operator*=(const Polynomial<Rational>& other)
{ if (this->NumVars!=other.NumVars)
  { int NewNumVars=MathRoutines::Maximum(other.NumVars, this->NumVars);
    Polynomial<Rational> otherNew=other;
    otherNew.SetNumVariables(NewNumVars);
    this->SetNumVariables(NewNumVars);
    *this*=otherNew;
    return;
  }
  if (other.IsEqualToZero())
  { this->MakeZero(this->NumVars, this->context);
    return;
  }
  if (this->expressionType== this->typeRational)
    this->ConvertToType(this->typePoly);
  if (this->expressionType==this->typePoly)
  { this->Numerator.GetElement()*=other;
    this->ReduceMemory();
    return;
  }
  Polynomial<Rational> theGCD, theResult, tempP;
  if (this->context!=0)
  { std::stringstream out;
    out << "Multiplying " << this->ToString(&this->context->theDefaultFormat) << " by "
    << other.ToString(&this->context->theDefaultFormat);
    this->context->theIndicatorVariables.StatusString1NeedsRefresh=true;
    this->context->theIndicatorVariables.StatusString1=out.str();
    this->context->MakeReport();
  }
  RationalFunction::gcd(this->Denominator.GetElement(), other, theGCD, this->context);
  this->Numerator.GetElement()*=other;
  this->Numerator.GetElement().DivideBy(theGCD, theResult, tempP);
  //theGCD.ComputeDebugString();
  //theResult.ComputeDebugString();
  //tempP.ComputeDebugString();
  assert(tempP.IsEqualToZero());
  this->Numerator.GetElement()=theResult;
  this->Denominator.GetElement().DivideBy(theGCD, theResult, tempP);
  //theGCD.ComputeDebugString();
  //theResult.ComputeDebugString();
//  tempP.ComputeDebugString();
  assert(tempP.IsEqualToZero());
  this->Denominator.GetElement()=theResult;
  this->ReduceMemory();
  this->SimplifyLeadingCoefficientOnly();
  if (this->context!=0)
  { std::stringstream out;
    out << "Multiplying " << this->ToString(&this->context->theDefaultFormat) << " by "
    << other.ToString(&this->context->theDefaultFormat);
    out << " and the result is:\n" << this->ToString();
    this->context->theIndicatorVariables.StatusString1NeedsRefresh=true;
    this->context->theIndicatorVariables.StatusString1=out.str();
    this->context->MakeReport();
  }
//  this->ComputeDebugString();
}

void RationalFunction::operator*=(const Rational& other)
{ //assert(this->checkConsistency());
  if (other.IsEqualToZero())
  { this->MakeZero(this->NumVars, this->context);
    return;
  }
  switch(this->expressionType)
  { case RationalFunction::typeRational: this->ratValue*=other; return;
    case RationalFunction::typePoly: this->Numerator.GetElement()*=(other); return;
    case RationalFunction::typeRationalFunction:
      this->Numerator.GetElement()*=(other);
      this->SimplifyLeadingCoefficientOnly();
      return;
  }
}

void RationalFunction::operator*=(const RationalFunction& other)
{// int commentChecksWhenDoneDebugging=-1;
  //this->checkConsistency();
  //other.checkConsistency();

  if (this->NumVars!=other.NumVars || this==&other)
  { RationalFunction tempRF;
    tempRF=other;
    int maxNumVars=MathRoutines::Maximum(this->NumVars, other.NumVars);
    this->SetNumVariables(maxNumVars);
    tempRF.SetNumVariables(maxNumVars);
    *this*=tempRF;
    return;
  }
  if (other.context!=0)
    this->context=other.context;
  if (other.IsEqualToZero() || this->IsEqualToZero())
  { this->MakeZero(this->NumVars, this->context);
    return;
  }
  if (other.expressionType==this->typeRational)
  { this->operator*=(other.ratValue);
    return;
  }
  if (other.expressionType==this->typePoly)
  { this->operator*=(other.Numerator.GetElementConst());
    return;
  }
  if (this->expressionType ==this->typeRational)
  { Rational tempRat;
    tempRat=this->ratValue;
    this->operator=(other);
    this->operator*=(tempRat);
    return;
  }
  if (this->expressionType==this->typePoly)
  { Polynomial<Rational>  tempP;
    tempP=this->Numerator.GetElement();
    this->operator=(other);
    this->operator*=(tempP);
    return;
  }
  Polynomial<Rational>  theGCD1, theGCD2, tempP1, tempP2;
  //this->ComputeDebugString();
//  RationalFunction tempde_Bugger;
//  tempde_Bugger=other;
//  tempde_Bugger.ComputeDebugString();
  if (this->context!=0)
  { std::stringstream out;
    out << "Multiplying " << this->ToString() << " by " << other.ToString();
    this->context->theIndicatorVariables.StatusString1NeedsRefresh=true;
    this->context->theIndicatorVariables.StatusString1=out.str();
    this->context->MakeReport();
  }
  RationalFunction::gcd(other.Denominator.GetElementConst(), this->Numerator.GetElement(), theGCD1, this->context);
  RationalFunction::gcd(this->Denominator.GetElement(), other.Numerator.GetElementConst(), theGCD2, this->context);
  this->Numerator.GetElement().DivideBy(theGCD1, tempP1, tempP2);
  this->Numerator.GetElement()=tempP1;
  assert(tempP2.IsEqualToZero());
  other.Denominator.GetElementConst().DivideBy(theGCD1, tempP1, tempP2);
  assert(tempP2.IsEqualToZero());
  this->Denominator.GetElement()*=tempP1;
  this->Denominator.GetElement().DivideBy(theGCD2, tempP1, tempP2);
  assert(tempP2.IsEqualToZero());
  this->Denominator.GetElement()=tempP1;
  other.Numerator.GetElementConst().DivideBy(theGCD2, tempP1, tempP2);
  assert(tempP2.IsEqualToZero());
  this->Numerator.GetElement()*=(tempP1);
  this->ReduceMemory();
  this->SimplifyLeadingCoefficientOnly();
  if (this->context!=0)
  { std::stringstream out;
    out << "Multiplying " << this->ToString() << " by " << other.ToString();
    out << " and the result is:\n" << this->ToString();
    this->context->theIndicatorVariables.StatusString1NeedsRefresh=true;
    this->context->theIndicatorVariables.StatusString1=out.str();
    this->context->MakeReport();
  }
}

void RationalFunction::Simplify()
{ MacroRegisterFunctionWithName("RationalFunction::Simplify");
  if (this->expressionType==this->typeRationalFunction)
    if(!this->Numerator.GetElement().IsEqualToZero())
    { Polynomial<Rational> theGCD, tempP, tempP2;
//      std::cout << "<br>fetching gcd of " << this->Numerator.GetElement().ToString() << " and "
//      << this->Denominator.GetElement().ToString() << "<br>";
      this->gcd(this->Numerator.GetElement(), this->Denominator.GetElement(), theGCD, this->context);
      if (theGCD.IsEqualToZero())
      { std::cout << "This is a programing error: "
        << " while fetching the gcd of " << this->Numerator.GetElement().ToString() << " and "
        << this->Denominator.GetElement().ToString() << " I got 0, which is impossible. "
        << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }
//      std::cout << "to get " << theGCD.ToString();
//      std::cout << "<br>dividing " << this->Numerator.GetElement().ToString() << " by " << theGCD.ToString() << "<br>";
      this->Numerator.GetElement().DivideBy(theGCD, tempP, tempP2);
      this->Numerator.GetElement()=(tempP);
      this->Denominator.GetElement().DivideBy(theGCD, tempP, tempP2);
      this->Denominator.GetElement()=(tempP);
    }
  this->ReduceMemory();
  this->SimplifyLeadingCoefficientOnly();
//  int commentMeWhendone;
//  assert(this->checkConsistency());
}

void RationalFunction::SimplifyLeadingCoefficientOnly()
{ if (this->expressionType!=this->typeRationalFunction)
    return;
  Rational tempRat= this->Denominator.GetElement().theCoeffs[this->Denominator.GetElement().GetIndexMaxMonomialTotalDegThenLexicographic()];
  tempRat.Invert();
  this->Denominator.GetElement()*=(tempRat);
  this->Numerator.GetElement()*=(tempRat);
}

void RootIndexToPoly(int theIndex, SemisimpleLieAlgebra& theAlgebra, Polynomial<Rational> & output)
{ int theRank=theAlgebra.theWeyl.CartanSymmetric.NumRows;
  int numPosRoots=theAlgebra.theWeyl.RootsOfBorel.size;
  output.MakeDegreeOne((int)(theRank+numPosRoots), theIndex+theRank, (Rational) 1);
}

std::string SemisimpleSubalgebras::ToString()
{ std::stringstream out;
  out << this->theHcandidates.ToString();
  return out.str();
}

void SemisimpleSubalgebras::FindHCandidates(char WeylLetter, int WeylDim, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> emptyStart;
//  List<List<Vector<Rational>> >::ListActualSizeIncrement=10000;
  this->theAlgebra.FindSl2Subalgebras(this->theSl2s, WeylLetter, WeylDim, theGlobalVariables);
  this->theHcandidates.size=0;
  this->theHcandidates.AddOnTop(emptyStart);
  for (this->indexLowestUnexplored=0; this->indexLowestUnexplored<this->theHcandidates.size; this->indexLowestUnexplored++)
    this->FindHCandidatesWithOneExtraHContaining(this->theHcandidates.TheObjects[this->indexLowestUnexplored], theGlobalVariables);
  this->ComputeDebugString();
  theGlobalVariables.theIndicatorVariables.StatusString1.append(this->DebugString);
  theGlobalVariables.MakeReport();
}

void  SemisimpleSubalgebras::FindHCandidatesWithOneExtraHContaining(Vectors<Rational>& inpuT, GlobalVariables& theGlobalVariables)
{ int theDimension= this->theAlgebra.theWeyl.CartanSymmetric.NumRows;
  SelectionWithMaxMultiplicity theSel;
  theSel.initMaxMultiplicity(theDimension, 2);
  int theCounter=MathRoutines::KToTheNth(3, theDimension);
  Vector<Rational> theRoot;
  WeylGroup& theWeyl=this->theAlgebra.theWeyl;
  Matrix<Rational>  invertedCartan=this->theAlgebra.theWeyl.CartanSymmetric;
  invertedCartan.Invert(theGlobalVariables);
  Vectors<Rational> tempRoots;
  tempRoots=inpuT;
  Vectors<Rational> inputCopy;
  inputCopy=inpuT;
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
      std::stringstream out;
      out << "index lowest non explored: " << this->indexLowestUnexplored+1 << " Total number found: " << this->theHcandidates.size;
      theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
      theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
      theGlobalVariables.MakeReport();
    }
  }
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::GetCoordinateFormOfSpanOfElements
(List<ElementUniversalEnveloping<CoefficientType> >& theElements, Vectors<Rational>& outputCoordinates,
 ElementUniversalEnveloping<CoefficientType>& outputCorrespondingMonomials, GlobalVariables& theGlobalVariables)
{ if (theElements.size==0)
    return;
  outputCorrespondingMonomials.MakeZero(*theElements.TheObjects[0].owner);
  for (int i=0; i<theElements.size; i++)
    for (int j=0; j<theElements.TheObjects[i].size; j++)
      outputCorrespondingMonomials.AddNoRepetition(theElements.TheObjects[i].TheObjects[j]);
  outputCoordinates.SetSize(theElements.size);
  Rational tempRat;
  for (int i=0; i<theElements.size; i++)
  { Vector<Rational>& current=outputCoordinates.TheObjects[i];
    current.MakeZero(outputCorrespondingMonomials.size);
    ElementUniversalEnveloping<CoefficientType>& currentElt=theElements.TheObjects[i];
    for (int j=0; j<currentElt.size; j++)
    { MonomialUniversalEnveloping<CoefficientType>& currentMon=currentElt.TheObjects[j];
      if (currentMon.Coefficient.size>0)
        tempRat=currentMon.Coefficient.TheObjects[0].Coefficient;
      else
        tempRat.MakeZero();
      current.TheObjects[outputCorrespondingMonomials.GetIndex(currentMon)]=tempRat;
    }
  }
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::AssignFromCoordinateFormWRTBasis
  (List<ElementUniversalEnveloping<CoefficientType> >& theBasis,
   Vector<CoefficientType>& input, SemisimpleLieAlgebra& owner)
{ /*int numVars=0;
  if (theBasis.size>0)
    numVars= theBasis.TheObjects[0].GetNumVars();*/
  this->MakeZero(*owner.owner, owner.indexInOwner);
  ElementUniversalEnveloping<CoefficientType> tempElt;
  for (int i=0; i<input.size; i++)
    if (!input.TheObjects[i].IsEqualToZero())
    { tempElt.operator=(theBasis.TheObjects[i]);
      tempElt.operator*=(input.TheObjects[i]);
      this->operator+=(tempElt);
    }
}

void SemisimpleLieAlgebra::ComputeCommonAdEigenVectors
(int theDegree, List<ElementUniversalEnveloping<Polynomial<Rational> > >& theGenerators,
 List<ElementUniversalEnveloping<Polynomial<Rational> > >& generatorsBeingActedOn,
 List<ElementUniversalEnveloping<Polynomial<Rational> > >& output, std::stringstream& out,
 GlobalVariables& theGlobalVariables)
{ SelectionWithMaxMultiplicity theSel;
 int numGenerators=generatorsBeingActedOn.size;
  theSel.initMaxMultiplicity(numGenerators+1, theDegree);
  int numCycles=theSel.NumCombinationsOfCardinality(theDegree);
  List<ElementUniversalEnveloping<Polynomial<Rational> > > candidateElements, theBracketsOfTheElements;
  ElementUniversalEnveloping<Polynomial<Rational> > tempElt, tempElt2, currentOutput, Accum, UEUnit;
  theBracketsOfTheElements.size=0;
  theSel.IncrementSubsetFixedCardinality(theDegree);
  int numVars=this->GetRank();
  Polynomial<Rational>  polyOne, polyZero;
  polyOne.MakeOne(numVars);
  polyZero.MakeZero(numVars);
  for (int i=0; i<numCycles; i++, theSel.IncrementSubsetFixedCardinality(theDegree))
  { Accum.MakeConst((Rational) 1, 0, *this->owner, this->indexInOwner);
    for (int j=0; j<theSel.elements.size; j++)
      if (theSel.elements.TheObjects[j]<numGenerators)
      { int generatorIndex=theSel.elements.TheObjects[j];
        int generatorPower=theSel.Multiplicities.TheObjects[generatorIndex];
        tempElt.operator=(generatorsBeingActedOn.TheObjects[generatorIndex]);
        tempElt.RaiseToPower(generatorPower);
        Accum*=(tempElt);
      }
    tempElt.operator=(Accum);
    tempElt.SetNumVariables(numVars);
    candidateElements.AddOnTop(tempElt);
//    out << "<br>" << Accum.ToString(true);
    for (int j=0; j<theGenerators.size; j++)
    { tempElt2.operator=(theGenerators.TheObjects[j]);
      tempElt2.SetNumVariables(numVars);
      tempElt2.LieBracketOnTheRight(tempElt, currentOutput);
//      currentOutput.ComputeDebugString();
//      std::cout<< currentOutput.DebugString;
      currentOutput.Simplify(theGlobalVariables, polyOne, polyZero);
      currentOutput.ModOutVermaRelations(&theGlobalVariables, 0, polyOne, polyZero);
      theBracketsOfTheElements.AddOnTop(currentOutput);
      out << "<br>";
      //out << "<div class=\"math\">";
      out << "[" << theGenerators[j].ToString(&theGlobalVariables.theDefaultFormat) << "," << Accum.ToString(&theGlobalVariables.theDefaultFormat) << "]=";
      //out << "\\begin{eqnarray*}&&";
      out << currentOutput.ToString(&theGlobalVariables.theDefaultFormat);
      //out << "  (mod Verma relations)";
      //out << "\\end{eqnarray*}</div>";
    }
  }
  out << "<br>number of monomials of given degree: " << candidateElements.size;
  Vectors<Polynomial<Rational> > theBasisRootForm;
  ElementUniversalEnveloping<Polynomial<Rational> > theMonBasis;
  ElementUniversalEnveloping<Polynomial<Rational> >::GetCoordinateFormOfSpanOfElements
  (numVars, theBracketsOfTheElements, theBasisRootForm, theMonBasis, theGlobalVariables);
  std::string tempS;
  out << "<br>...and the monomial basis is(" << theMonBasis.size << " elements total): ";
  for (int i=0; i<theMonBasis.size; i++)
    out << theMonBasis.TheObjects[i].ToString(&theGlobalVariables.theDefaultFormat) << ", ";
  Matrix<RationalFunction> theSystem;
  theSystem.init(theMonBasis.size*theGenerators.size, candidateElements.size);
  for (int k=0; k<theGenerators.size; k++)
    for (int i=0; i<candidateElements.size; i++)
    { Vector<Polynomial<Rational> >& currentRoot=theBasisRootForm.TheObjects[k+i*theGenerators.size];
      //out << "<br>Vector<Rational> index" << i << ": " << currentRoot.ToString();
      for (int j=0; j<currentRoot.size; j++)
        theSystem.elements[currentRoot.size*k+j][i]=currentRoot.TheObjects[j];
    }
  out << "<br>...and the system is: <div class=\"math\">" << theSystem.ToString(false, true) << "</div>";
  List<List<RationalFunction> > theEigenVectors;
  RationalFunction oneRF, minusOneRF, zeroRF;
  oneRF.MakeConst(numVars, (Rational) 1, &theGlobalVariables);
  minusOneRF.MakeConst(numVars, (Rational) -1, &theGlobalVariables);
  zeroRF.MakeConst(numVars, (Rational) 0, &theGlobalVariables);

  theSystem.FindZeroEigenSpacE(theEigenVectors, oneRF, minusOneRF, zeroRF, theGlobalVariables);
  out << "<br> the dim of the eigen space: " << theEigenVectors.size;
  Vector<Polynomial<Rational> > tempProot;

  for (int i=0; i<theEigenVectors.size; i++)
  { List<RationalFunction>& currentEigen=theEigenVectors.TheObjects[i];
    RationalFunction::ScaleClearDenominator(currentEigen, tempProot);
    tempElt.AssignFromCoordinateFormWRTBasis(candidateElements, tempProot, *this);
    out << "<br>" << tempElt.ToString(&theGlobalVariables.theDefaultFormat);
  }
}

void RationalFunction::ScaleClearDenominator
  (List<RationalFunction>& input, Vector<Polynomial<Rational> >& output)
{ Polynomial<Rational>  tempP;
  List<RationalFunction> buffer;
  buffer.CopyFromBase(input);
  for (int i=0; i<buffer.size; i++)
  { RationalFunction& current=buffer.TheObjects[i];
    if (current.expressionType==RationalFunction::typeRationalFunction)
    { tempP.operator=(current.Denominator.GetElement());
      for (int j=0; j<buffer.size; j++)
        buffer.TheObjects[j].operator*=(tempP);
    }
  }
  output.SetSize(input.size);
  for (int i=0; i<buffer.size; i++)
  { buffer.TheObjects[i].GetNumerator(tempP);
    output.TheObjects[i]=tempP;
  }
}

void SemisimpleLieAlgebra::ComputeCommonAdEigenVectorsFixedWeight
(Vector<Rational>& theWeight, Vectors<Rational>& theHs, List<ElementUniversalEnveloping<Polynomial<Rational> > >& theGenerators,
 List<ElementUniversalEnveloping<Polynomial<Rational> > >& output, std::stringstream& out,
 GlobalVariables& theGlobalVariables)
{ VectorPartition theVP;
  int numGenerators= this->GetNumPosRoots();
  int numVars=this->GetRank();
  Polynomial<Rational>  polyOne, polyZero;
  polyOne.MakeOne(numVars);
  polyZero.MakeZero(numVars);
  int theRank=this->GetRank();
  theVP.PartitioningRoots.SetSize(numGenerators);
  List<ElementUniversalEnveloping<Polynomial<Rational> > > generatorsBeingActedOn;
  generatorsBeingActedOn.SetSize(numGenerators);
  theVP.PartitioningRoots.operator=(this->theWeyl.RootsOfBorel);
  out << "<br>" << theWeight.ToString() << "<br> the H's: " << theHs.ToString();
  out << "<br> Partitioning Vectors<Rational>: " << theVP.PartitioningRoots.ToString();
  Polynomial<Rational>  polyZeroZeroVars;
  polyZeroZeroVars.MakeZero(0);
  for (int i=0; i<numGenerators; i++)
  { ElementUniversalEnveloping<Polynomial<Rational> >& currentUE=generatorsBeingActedOn.TheObjects[i];
    currentUE.MakeOneGeneratorCoeffOne(i, *this->owner, this->indexInOwner, polyZeroZeroVars.GetOne(), polyZeroZeroVars);
  }
  theVP.theRoot=theWeight;
  theVP.ComputeAllPartitions();
  out << "<br><br>" << theVP.ToString(true);
  Vector<Rational> accumulatedHweight, tempRoot;
  for (int i=0; i<theVP.thePartitions.size; i++)
  { List<int>& currentPartition=theVP.thePartitions.TheObjects[i];
    accumulatedHweight.MakeZero(theRank);
    for (int generatorIndex=0; generatorIndex<currentPartition.size; generatorIndex++)
    { int generatorPower=currentPartition.TheObjects[generatorIndex];
      accumulatedHweight+=theVP.PartitioningRoots[generatorIndex]* generatorPower;
    }
    bool isGood=true;
    for (int j=0; j<theHs.size; j++)
    { Vector<Rational>& currentH=theHs.TheObjects[j];
      Rational theTargetScalar;
      Rational currentScalar;
      bool found=false;
      for (int k=0; k<currentH.size; k++)
        if (currentH.TheObjects[k]!=0)
        { tempRoot.MakeEi(currentH.size, k);
          currentScalar=this->theWeyl.RootScalarCartanRoot(tempRoot, accumulatedHweight);
          if (!found)
          { theTargetScalar=currentScalar;
            found=true;
          }
          if (theTargetScalar!=currentScalar)
          { isGood=false;
            break;
          }
        }
      if (!isGood)
        break;
    }
    if (isGood)
    { out << "<br>" << accumulatedHweight.ToString();
      out << " is good";
    } else
    { theVP.thePartitions.PopIndexSwapWithLast(i);
      i--;
    }
  }
  List<ElementUniversalEnveloping<Polynomial<Rational> > > candidateElements, theBracketsOfTheElements;
  ElementUniversalEnveloping<Polynomial<Rational> > tempElt, tempElt2, currentOutput, Accum, UEUnit;
  theBracketsOfTheElements.size=0;
  for (int i=0; i<theVP.thePartitions.size; i++)
  { Accum.MakeConst((Rational) 1, 0, *this->owner, this->indexInOwner);
    List<int>& currentPartition=theVP.thePartitions.TheObjects[i];
    for (int generatorIndex=0; generatorIndex<currentPartition.size; generatorIndex++)
    { int generatorPower=currentPartition.TheObjects[generatorIndex];
      tempElt.operator=(generatorsBeingActedOn.TheObjects[generatorIndex]);
      tempElt.RaiseToPower(generatorPower);
      Accum*=(tempElt);
    }
    tempElt.operator=(Accum);
    tempElt.SetNumVariables(numVars);
    candidateElements.AddOnTop(tempElt);
//    out << "<br>" << Accum.ToString(true);
    for (int j=0; j<theGenerators.size; j++)
    { tempElt2.operator=(theGenerators.TheObjects[j]);
      tempElt2.SetNumVariables(numVars);
      tempElt2.LieBracketOnTheRight(tempElt, currentOutput);
//      currentOutput.ComputeDebugString();
//      std::cout<< currentOutput.DebugString;
      currentOutput.Simplify(theGlobalVariables, polyOne, polyZero);
      currentOutput.ModOutVermaRelations(&theGlobalVariables, 0, polyOne, polyZero);
      theBracketsOfTheElements.AddOnTop(currentOutput);
      out << "<br>";
      //out << "<div class=\"math\">";
      out << "[" << theGenerators[j].ToString(&theGlobalVariables.theDefaultFormat) << ","
      << Accum.ToString(&theGlobalVariables.theDefaultFormat) << "]=";
      //out << "\\begin{eqnarray*}&&";
      out << currentOutput.ToString(&theGlobalVariables.theDefaultFormat);
      //out << "  (mod Verma relations)";
      //out << "\\end{eqnarray*}</div>";
    }
  }
  out << "<br>number of monomials of given degree: " << candidateElements.size;
  Vectors<Polynomial<Rational> > theBasisRootForm;
  ElementUniversalEnveloping<Polynomial<Rational> > theMonBasis;
  ElementUniversalEnveloping<Polynomial<Rational> >::GetCoordinateFormOfSpanOfElements(numVars, theBracketsOfTheElements, theBasisRootForm, theMonBasis, theGlobalVariables);
  std::string tempS;
  out << "<br>...and the monomial basis is(" << theMonBasis.size << " elements total): ";
  for (int i=0; i<theMonBasis.size; i++)
    out << theMonBasis[i].ToString(&theGlobalVariables.theDefaultFormat) << ", ";
  Matrix<RationalFunction> theSystem;
  theSystem.init(theMonBasis.size*theGenerators.size, candidateElements.size);
  for (int k=0; k<theGenerators.size; k++)
    for (int i=0; i<candidateElements.size; i++)
    { Vector<Polynomial<Rational> >& currentRoot=theBasisRootForm[k+i*theGenerators.size];
      //out << "<br>Vector<Rational> index" << i << ": " << currentRoot.ToString();
      for (int j=0; j<currentRoot.size; j++)
        theSystem.elements[currentRoot.size*k+j][i]=currentRoot[j];
    }
  out << "<br>...and the system is: <div class=\"math\">" <<  theSystem.ToString(false, true) << "</div>";
  List<List<RationalFunction> > theEigenVectors;
  RationalFunction oneRF, minusOneRF, zeroRF;
  oneRF.MakeConst(numVars, (Rational) 1, &theGlobalVariables);
  minusOneRF.MakeConst(numVars, (Rational) -1, &theGlobalVariables);
  zeroRF.MakeConst(numVars, (Rational) 0, &theGlobalVariables);
  theSystem.FindZeroEigenSpacE(theEigenVectors, oneRF, minusOneRF, zeroRF, theGlobalVariables);
  out << "<br> the dim of the eigen space: " << theEigenVectors.size;
  Vector<Polynomial<Rational> > tempProot;
  for (int i=0; i<theEigenVectors.size; i++)
  { List<RationalFunction>& currentEigen=theEigenVectors[i];
    RationalFunction::ScaleClearDenominator(currentEigen, tempProot);
    tempElt.AssignFromCoordinateFormWRTBasis(candidateElements, tempProot, *this);
    out << "<br>" << tempElt.ToString(&theGlobalVariables.theDefaultFormat);
  }
}

void SemisimpleLieAlgebraOrdered::GetLinearCombinationFrom
  (ElementSemisimpleLieAlgebra& input, Vector<Rational>& theCoeffs)
{ theCoeffs.MakeZero(this->theOwner.GetNumGenerators());
  for (int i=0; i<input.size; i++)
  { int theIndex=input[i].theGeneratorIndex;
    theCoeffs.TheObjects[this->theOwner.GetGeneratorFromRootIndex(theIndex)]=input.theCoeffs[i];
  }
  int numPosRoots=this->theOwner.GetNumPosRoots();
  Vector<Rational> tempH=input.GetCartanPart();
  for (int i=0; i<this->theOwner.GetRank(); i++)
    theCoeffs.TheObjects[numPosRoots+i]= tempH[i];
  this->ChevalleyGeneratorsInCurrentCoords.ActOnVectorColumn(theCoeffs);
}

int SemisimpleLieAlgebraOrdered::GetDisplayIndexFromGeneratorIndex(int GeneratorIndex)
{ int numPosRoots=this->theOwner.GetNumPosRoots();
  int posRootsPlusRank=numPosRoots+this->theOwner.GetRank();
  if (GeneratorIndex>= posRootsPlusRank )
    return GeneratorIndex-posRootsPlusRank+1;
  if (GeneratorIndex>=numPosRoots)
    return GeneratorIndex+1;
  return -numPosRoots+GeneratorIndex;
}

void SemisimpleLieAlgebraOrdered::init
(List<ElementSemisimpleLieAlgebra>& inputOrder, SemisimpleLieAlgebra& owner, GlobalVariables& theGlobalVariables)
{ return;
  if (inputOrder.size!=owner.GetNumGenerators())
    return;
  this->theOwner=owner;
  this->theOrder=inputOrder;
  this->ChevalleyGeneratorsInCurrentCoords.init(owner.GetNumGenerators(), owner.GetNumGenerators());
  this->ChevalleyGeneratorsInCurrentCoords.NullifyAll();
  ElementSemisimpleLieAlgebra tempElt;
  Vector<Rational> coordsInCurrentBasis;
  ElementSemisimpleLieAlgebra currentElt;
  for (int i=0; i<owner.GetNumGenerators(); i++)
  { currentElt.MakeGenerator
    (i, *owner.owner, owner.indexInOwner);
    currentElt.GetCoordsInBasis(this->theOrder, coordsInCurrentBasis, theGlobalVariables);
    for (int j=0; j<coordsInCurrentBasis.size; j++)
      this->ChevalleyGeneratorsInCurrentCoords.elements[j][i]=coordsInCurrentBasis.TheObjects[j];
//    std::cout << "<br> " << currentElt.ToString() << " in new coords becomes: " << coordsInCurrentBasis.ToString();
  }
//  std::cout << this->ChevalleyGeneratorsInCurrentCoords.ToString(true, false) << "<br><br>";

 // this->ChevalleyGeneratorsInCurrentCoords.Invert(theGlobalVariables);
  //std::cout << this->ChevalleyGeneratorsInCurrentCoords.ToString(true, false);
}

void SemisimpleLieAlgebraOrdered::initDefaultOrder
  (SemisimpleLieAlgebra& owner, GlobalVariables& theGlobalVariables)
{ List<ElementSemisimpleLieAlgebra> defaultOrder;
  defaultOrder.SetSize(owner.GetNumGenerators());
  for (int i=0; i<defaultOrder.size; i++)
  { ElementSemisimpleLieAlgebra& currentElt=defaultOrder.TheObjects[i];
    currentElt.MakeGenerator
    (i, *owner.owner, owner.indexInOwner);
  }
  this->init(defaultOrder, owner, theGlobalVariables);
}

bool ElementSemisimpleLieAlgebra::MustUseBracketsWhenDisplayingMeRaisedToPower()
{ if (this->size==1)
    if (this->theCoeffs[0]==1)
      return false;
  return true;
}

bool ParserNode::ConvertToNextType
(int GoalType, int GoalNumVariables, bool& ErrorHasOccured, GlobalVariables& theGlobalVariables)
{ ErrorHasOccured=false;
  assert(this->impliedNumVars<=GoalNumVariables);
  this->impliedNumVars=GoalNumVariables;
  switch(this->ExpressionType)
  { case ParserNode::typePoly:
      assert(this->polyValue.GetElement().NumVars<=GoalNumVariables);
      this->polyValue.GetElement().SetNumVariablesSubDeletedVarsByOne(GoalNumVariables);
      break;
    case ParserNode::typeUEelement:
      assert(this->UEElement.GetElement().GetNumVars()<=GoalNumVariables);
      this->UEElement.GetElement().SetNumVariables(GoalNumVariables);
      break;
    case ParserNode::typeUEElementOrdered:
      assert(this->UEElementOrdered.GetElement().GetNumVars()<=GoalNumVariables);
      this->UEElementOrdered.GetElement().SetNumVariables(GoalNumVariables);
      break;
    case ParserNode::typeRationalFunction:
      assert(this->ratFunction.GetElement().NumVars<=GoalNumVariables);
      this->ratFunction.GetElement().SetNumVariablesSubDeletedVarsByOne(GoalNumVariables);
      break;
    case ParserNode::typeWeylAlgebraElement:
      assert(this->WeylAlgebraElement.GetElement().NumVariables<=GoalNumVariables);
      this->WeylAlgebraElement.GetElement().SetNumVariables(GoalNumVariables);
      break;
  }
  if (GoalType==this->ExpressionType)
    return false;
  if (this->ExpressionType==this->typeIntegerOrIndex)
  { this->rationalValue= this->intValue;
    this->ExpressionType=this->typeRational;
    return true;
  }
  if (this->ExpressionType==this->typeRational)
  { if (GoalType<this->typeRational)
    { if (this->rationalValue.IsSmallInteger())
      { this->ExpressionType=this->typeIntegerOrIndex;
        this->intValue=this->rationalValue.NumShort;
        return true;
      }
      return false;
    }
    if (GoalType==this->typeCharSSFDMod)
    { MonomialChar<Rational> tempMon;
      tempMon.weightFundamentalCoords.MakeZero(this->GetContextLieAlgebra().GetRank());
//      tempMon.Coefficient=this->rationalValue;
      this->theChar.GetElement().MakeZero(this->owner->theAlgebras, this->IndexContextLieAlgebra);
//      this->theChar.GetElement()+=tempMon;
      this->ExpressionType=this->typeCharSSFDMod;
      return true;
    }
    this->polyValue.GetElement().MakeConst(GoalNumVariables, this->rationalValue);
    this->ExpressionType=this->typePoly;
    return true;
  }
  if (this->ExpressionType==this->typePoly)
  { if (GoalType<this->typePoly)
    { if(this->polyValue.GetElement().IsAConstant(this->rationalValue))
      { this->ExpressionType=this->typeRational;
        return true;
      }
      return false;
    }
    int typeToConvertTo=this->typeRationalFunction;
    if (GoalType==this->typeUEElementOrdered || GoalType==this->typeUEelement)
      typeToConvertTo=this->typeUEElementOrdered;
    if (GoalType==this->typeWeylAlgebraElement)
      typeToConvertTo=this->typeWeylAlgebraElement;
    switch (typeToConvertTo)
    { case ParserNode::typeUEelement:
        this->UEElement.GetElement().MakeConst(this->polyValue.GetElement(), this->owner->theAlgebras, this->IndexContextLieAlgebra);
        this->ExpressionType=this->typeUEelement;
        return true;
      case ParserNode::typeWeylAlgebraElement:
        this->WeylAlgebraElement.GetElement().AssignPolynomial(this->polyValue.GetElement());
        this->ExpressionType=this->typeWeylAlgebraElement;
        return true;
      case ParserNode::typeRationalFunction:
        this->ratFunction.GetElement()=this->polyValue.GetElement();
        this->ExpressionType=this->typeRationalFunction;
        return true;
      case ParserNode::typeUEElementOrdered:
        this->UEElementOrdered.GetElement().MakeConst(this->polyValue.GetElement(), this->owner->testAlgebra);
        this->ExpressionType=this->typeUEElementOrdered;
        return true;
    }
  }
  if (this->ExpressionType==this->typeRationalFunction)
  { if (GoalType<this->typeRationalFunction)
    { if (this->ratFunction.GetElement().expressionType==RationalFunction::typePoly)
      { this->polyValue=this->ratFunction.GetElement().Numerator;
        this->ExpressionType=this->typePoly;
        return true;
      }
      if (this->ratFunction.GetElement().expressionType==RationalFunction::typeRational)
      { this->polyValue.GetElement().MakeConst(this->ratFunction.GetElement().NumVars, this->ratFunction.GetElement().ratValue);
        this->ExpressionType=this->typePoly;
        return true;
      }
      return false;
    }
    return false;
  }
  if (this->ExpressionType==this->typeUEelement && GoalType==this->typeUEElementOrdered)
  { Polynomial<Rational>  unitPoly, zeroPoly;
    unitPoly.MakeConst(this->UEElement.GetElement().GetNumVars(), (Rational) 1);
    zeroPoly.MakeZero(unitPoly.NumVars);
    if (this->UEElementOrdered.GetElement().AssignElementUniversalEnveloping(this->UEElement.GetElement(), this->owner->testAlgebra, unitPoly, zeroPoly, &theGlobalVariables))
    { this->ExpressionType=this->typeUEElementOrdered;
      return true;
    } else
    { ErrorHasOccured=true;
      return false;
    }
  }
  if (this->ExpressionType==this->typeUEElementOrdered && GoalType==this->typeUEelement)
  { if (!this->UEElementOrdered.GetElement().GetElementUniversalEnveloping(this->UEElement.GetElement(), this->owner->theHmm.theRange()))
    { ErrorHasOccured=true;
      return false;
    }
    this->ExpressionType=this->typeUEelement;
    return true;
  }
  return false;
}

bool ParserNode::ConvertToType
(int theType, int GoalNumVars, GlobalVariables& theGlobalVariables)
{ if (this->ExpressionType==this->typeError)
    return false;
  if (theType==this->typeUndefined)
    return false;
  if (GoalNumVars< this->impliedNumVars)
    return false;
  this->impliedNumVars=GoalNumVars;
  this->owner->MaxFoundVars=MathRoutines::Maximum(this->impliedNumVars, this->owner->MaxFoundVars);
  switch(this->ExpressionType)
  { case ParserNode::typeMapPolY:
      this->polyBeingMappedTo.GetElement().SetNumVariablesSubDeletedVarsByOne(GoalNumVars);
    case ParserNode::typePoly:
      this->polyValue.GetElement().SetNumVariablesSubDeletedVarsByOne(GoalNumVars);
      break;
    case ParserNode::typeRationalFunction:
      this->ratFunction.GetElement().SetNumVariablesSubDeletedVarsByOne(GoalNumVars);
      break;
    case ParserNode::typeUEelement:
      this->UEElement.GetElement().SetNumVariables(GoalNumVars);
      break;
    case ParserNode::typeUEElementOrdered:
      this->UEElementOrdered.GetElement().SetNumVariables(GoalNumVars);
      break;
    case ParserNode::typeMapWeylAlgebra:
      this->weylEltBeingMappedTo.GetElement().SetNumVariables(GoalNumVars);
    case ParserNode::typeWeylAlgebraElement:
      this->WeylAlgebraElement.GetElement().SetNumVariables(GoalNumVars); break;
    case ParserNode::typeAnimation:
      break;
    case ParserNode::typeQuasiPolynomial:
      if (GoalNumVars!=this->theQP.GetElement().GetNumVars())
      { this->SetError(this->errorDimensionProblem);
        return false;
      }
      break;
    case ParserNode::typeGenVermaElt:
      this->impliedNumVars=this->owner->MaxFoundVars;
      GoalNumVars=this->impliedNumVars;
      this->owner->SetNumVarsModulePolys(GoalNumVars);
      this->theGenVermaElt.GetElement().SetNumVariables(GoalNumVars); break;
    default: break;
  }
  bool ConversionError;
  while (this->ConvertToNextType(theType, GoalNumVars, ConversionError, theGlobalVariables))
    if (ConversionError)
    { this->SetError(this->errorConversionError);
      return false;
    }
  return this->ExpressionType==theType;
}

bool ElementSemisimpleLieAlgebra::IsACoeffOneChevalleyGenerator(int& outputGenerator, SemisimpleLieAlgebra& owner)
{ if (this->size==1)
    return this->theCoeffs[0]==1;
  return false;
}

std::string ParserNode::ElementToStringValueOnlY
(bool useHtml, int RecursionDepth, int maxRecursionDepth, GlobalVariables& theGlobalVariables, FormatExpressions& theFormat)
{ std::stringstream LatexOutput;
  int i; //can't declare variables within a switch clause!
  switch (this->ExpressionType)
  { case ParserNode::typeIntegerOrIndex: LatexOutput << this->intValue; break;
    case ParserNode::typeRational: LatexOutput << this->rationalValue.ToString(); break;
    case ParserNode::typePoly: LatexOutput << "\\begin{array}{rcl}&&\n" << this->polyValue.GetElement().ToString(&theFormat) << "\n\\end{array}"; break;
    case ParserNode::typeRationalFunction: LatexOutput << "\\begin{array}{rcl}&&\n" << this->ratFunction.GetElement().ToString(&theFormat) << "\n\\end{array}"; break;
    case ParserNode::typeUEElementOrdered:
      LatexOutput << "\\begin{array}{rcl}&&\n"
      << this->UEElementOrdered.GetElement().ToString
      (true, theFormat.flagUseCalculatorFormatForUEOrdered, theFormat, theGlobalVariables)
      << "\n\\end{array}";
      break;
    case ParserNode::typeLittelman:
      break;
    case ParserNode::typeUEelement:
      LatexOutput << "\\begin{array}{rcl}&&\n"
      << this->UEElement.GetElement().ToString(&theFormat)
      << "\n\\end{array}";
      break;
    case ParserNode::typeWeylAlgebraElement:
      LatexOutput << "\\begin{array}{rcl}&&\n"
      << this->WeylAlgebraElement.GetElement().ToString()
      << "\n\\end{array}";
      break;
    case ParserNode::typeCharSSFDMod:
    //  LatexOutput << "\\begin{array}{rcl}&&\n"
//      << this->theChar.GetElement().ToString("char", "", true)
  //    << "\\end{array}";
      break;
    case ParserNode::typeGenVermaElt:
      LatexOutput << this->theGenVermaElt.GetElement().ToString(&theFormat);
      break;
    case ParserNode::typePartialFractions: LatexOutput << this->thePFs.GetElement().ToString(theGlobalVariables, theFormat); break;
    case ParserNode::typeLattice: LatexOutput << this->theLattice.GetElement().ToString(true, false); break;
//    case ParserNode::typeCone: LatexOutput << this->theCone.GetElement().ToString(false, false, true, false, theFormat); break;
    case ParserNode::typeArray:
      LatexOutput << "(";
      RecursionDepth++;
      if (RecursionDepth<=maxRecursionDepth)
        for (i=0; i<this->children.size; i++)
        { LatexOutput << this->owner->TheObjects[this->children.TheObjects[i]].ElementToStringValueOnlY(useHtml, RecursionDepth, maxRecursionDepth, theGlobalVariables, theFormat);
          if (i!=this->children.size-1)
            LatexOutput << ",";
        }
      else
        LatexOutput << "...";
      LatexOutput << ")";
      break;
    default: break;
  }
  return LatexOutput.str();
}

std::string ParserNode::ElementToStringValueAndType
(bool useHtml, int RecursionDepth, int maxRecursionDepth, GlobalVariables& theGlobalVariables, bool displayOutputString, FormatExpressions& theFormat)
{ std::stringstream out;
  std::string stringValueOnly= this->ElementToStringValueOnlY(useHtml, RecursionDepth, maxRecursionDepth, theGlobalVariables, theFormat);
  switch (this->ExpressionType)
  { case ParserNode::typeIntegerOrIndex:  out << " an integer of value: "; break;
    case ParserNode::typeRational: out << " a rational number of value: "; break;
    case ParserNode::typePoly: out << " a polynomial of value: "; break;
    case ParserNode::typeRationalFunction: out << " a rational function of value: "; break;
    case ParserNode::typeUEElementOrdered: out << "an element of U(g) ordered:"; break;
    case ParserNode::typeUEelement: out << " an element of U(g) of value: "; break;
    case ParserNode::typeWeylAlgebraElement: out << " a Weyl algebra element: "; break;
    case ParserNode::typeLittelman: out << "a Littelmann path: " << this->theLittelmann.GetElement().ToString(); break;
    case ParserNode::typeArray: out << " an array of " << this->children.size << " elements. "; break;
    case ParserNode::typeString: out << "<br>A printout of value: "; break;
    case ParserNode::typeCharSSFDMod: out << "Character of a finite dimensional representation of the ambient"
    << " simple Lie algebra:"; break;
    case ParserNode::typeError: out << this->ElementToStringErrorCode(useHtml); break;
    case ParserNode::typeGenVermaElt: out << "Tensor product of elemetns of generalized Verma modules: "; break;
    case ParserNode::typeLattice: out << "A lattice."; useHtml=true; break;
    case ParserNode::typeCone:
//      theGlobalVariables.theDrawingVariables.theBuffer.init();
//      theGlobalVariables.theDrawingVariables.drawCoordSystemBuffer(theGlobalVariables.theDrawingVariables, this->theCone.GetElement().GetDim(), 0);
      out << "a cone: ";// << this->theCone.GetElement().DrawMeToHtmlProjective(theGlobalVariables.theDrawingVariables);
      break;
    case ParserNode::typePiecewiseQP: out << "Piecewise quasi-polynomial: "; break;
    case ParserNode::typeQuasiPolynomial: out << "Quasipolynomial of value: "; break;
    case ParserNode::typePartialFractions: out << "Partial fraction(s): "; break;
    case ParserNode::typeUndefined: out << "Undefined expression (type 0)."; break;
    case ParserNode::typeAnimation: out << "Animation."; break;
    default: out << "Expression type " << this->ExpressionType << "; the programmer(s) have forgotten to enter a type description. "; break;
  }
  if (stringValueOnly!="")
  { if (!useHtml)
      out << stringValueOnly;
    else
      out << CGI::GetHtmlMathDivFromLatexFormulA(stringValueOnly);
  }
  if (displayOutputString)
  { if (this->outputString!="" && this->ExpressionType!=this->typeString)
      out << "<br>In addition, the program generated the following printout. ";
    out << this->outputString;
  } else
    if (this->outputString!="")
      out << "Output string not shown to avoid javascript conflicts.";
  if (this->ExpressionType==this->typeArray)
  { out << "<br>Elements of the array follow.";
    RecursionDepth++;
    if (RecursionDepth<=maxRecursionDepth)
      for (int i=0; i<this->children.size; i++)
        out << "<br>Element of index " << i+1 << ":" << this->owner->TheObjects[this->children.TheObjects[i]].ElementToStringValueAndType
        (useHtml, RecursionDepth, maxRecursionDepth, theGlobalVariables, displayOutputString, theFormat);
  }
  return out.str();
}

int ParserNode::EvaluateSubstitution(GlobalVariables& theGlobalVariables)
{ if (this->children.size!=2)
    return this->SetError(this->errorBadOrNoArgument);
  ParserNode& leftNode=this->owner->TheObjects[this->children[0]];
  ParserNode& rightNode=this->owner->TheObjects[this->children[1]];
  this->impliedNumVars=this->GetMaxImpliedNumVarsChildren();
  if(!leftNode.ConvertToType(this->typeWeylAlgebraElement, this->impliedNumVars, theGlobalVariables))
    return this->SetError(this->errorBadOrNoArgument);
  if(!rightNode.ConvertToType(this->typeWeylAlgebraElement, this->impliedNumVars, theGlobalVariables))
    return this->SetError(this->errorBadOrNoArgument);
  this->WeylAlgebraElement.GetElement()=leftNode.WeylAlgebraElement.GetElementConst();
  this->weylEltBeingMappedTo.GetElement()=rightNode.WeylAlgebraElement.GetElementConst();
  this->ExpressionType=this->typeMapWeylAlgebra;
  return this->errorNoError;
}

void ParserNode::EvaluateDereferenceArray(GlobalVariables& theGlobalVariables)
{ if (this->children.size!=2)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  ParserNode& firstChild=this->owner->TheObjects[this->children.TheObjects[0]];
  ParserNode& secondChild=this->owner->TheObjects[this->children.TheObjects[1]];
  if (secondChild.ExpressionType!=this->typeIntegerOrIndex || firstChild.ExpressionType!=this->typeArray)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  int arraySize=firstChild.children.size;
  int arrayIndex=secondChild.intValue-1;
  if (arrayIndex>=arraySize || arrayIndex<0)
  { this->SetError(this->errorBadIndex);
    return;
  }
  ParserNode& relevantChild=this->owner->TheObjects[firstChild.children.TheObjects[arrayIndex]];
  this->CopyValue(relevantChild);
  return;
}

int ParserNode::EvaluateApplySubstitution(GlobalVariables& theGlobalVariables)
{ if (this->children.size<2)
    return this->SetError(this->errorProgramming);
  this->impliedNumVars=this->GetMaxImpliedNumVarsChildren();
  Polynomial<Rational>  currentLeft, currentRight;
  Vectors<Rational> leftHandSide;
  PolynomialSubstitution<Rational> theSubInitial;
  leftHandSide.ReservE(this->impliedNumVars*2);
  theSubInitial.ReservE(this->impliedNumVars*2);
  Vector<Rational> currentLeftRoot;
  for (int i=0; i<this->children.size-1; i++)
  { ParserNode& currentNode=this->owner->TheObjects[this->children.TheObjects[i]];
    if (!currentNode.ConvertToType(this->typeMapWeylAlgebra, this->impliedNumVars, theGlobalVariables))
      return this->SetError(this->errorBadOrNoArgument);
    currentNode.WeylAlgebraElement.GetElement().GetStandardOrder(currentLeft);
    currentNode.weylEltBeingMappedTo.GetElement().GetStandardOrder(currentRight);
    if (currentLeft.IsLinear())
    { currentLeft.GetRootFromLinPolyConstTermLastVariable(currentLeftRoot);
      theSubInitial.AddOnTop(currentRight);
      *theSubInitial.LastObject()-=*currentLeftRoot.LastObject();
      currentLeftRoot.size--;
      leftHandSide.AddOnTop(currentLeftRoot);
    } else
      return this->SetError(this->errorBadOrNoArgument);
  }
  int NumVarsDoubled=this->impliedNumVars*2;
  Matrix<Rational> tempMat;
  Selection tempSel;
  int currentRank=leftHandSide.GetRankOfSpanOfElements(tempMat, tempSel);
  Vector<Rational> ei;
  Polynomial<Rational>  tempP;
  std::stringstream report, out;
  bool found=false;
  int numImpliedDsubs=0;
  int numImpliedXsubs=0;
  for (int i=0; i<NumVarsDoubled && currentRank<NumVarsDoubled; i++)
  { ei.MakeEi(this->impliedNumVars*2, i);
    leftHandSide.AddOnTop(ei);
    int candidateRank=leftHandSide.GetRankOfSpanOfElements(tempMat, tempSel);
    if (candidateRank>currentRank)
    { currentRank=candidateRank;
      tempP.MakeLinPolyFromRootNoConstantTerm(ei);
      theSubInitial.AddOnTop(tempP);
      if (found)
        report << ",";
      found=true;
      report << tempP.ToString(&theGlobalVariables.theDefaultFormat) << " \\mapsto " << tempP.ToString(&theGlobalVariables.theDefaultFormat);
      if (i<NumVarsDoubled/2)
        numImpliedXsubs++;
      else
        numImpliedDsubs++;
    } else
      leftHandSide.size--;
  }
  Matrix<Rational>  matLeft;
  matLeft.AssignRootsToRowsOfMatrix(leftHandSide);
  matLeft.Invert();
  PolynomialSubstitution<Rational> theSub;
  theSub.SetSize(NumVarsDoubled);
  for (int i=0; i<NumVarsDoubled; i++)
  { (theSub[i]).MakeZero(NumVarsDoubled);
    for (int j=0; j<NumVarsDoubled; j++)
    { tempP=theSubInitial[j];
      tempP*=matLeft.elements[i][j];
      theSub[i]+=tempP;
    }
  }
  out << "<hr> The substitution carried out was: <br>\n ( " << theSub.ToString();
  ParserNode& lastNode=this->owner->TheObjects[*this->children.LastObject()];
  if (lastNode.ExpressionType==this->typePoly)
    out << lastNode.polyValue.GetElement().ToString(&theGlobalVariables.theDefaultFormat) << ")";
  else
    out << " ... )";
  if (found)
  { out << "<hr> I modified your input  by generating the following susbstitutions.\n<br>\n" << report.str();
    out << "<hr>";
    if (numImpliedDsubs==NumVarsDoubled/2)
      out << "<br>All substitutions are done in 2n variables, where n is the implied number of variables"
      << "(for reasons specific to the internal implementation of Weyl algebra elements); this explains the last "
      << " n automatically generated substitutions. \n<br>\n\n";
  }
  lastNode.ConvertToType(lastNode.ExpressionType, this->impliedNumVars, theGlobalVariables);
  this->CopyValue(lastNode);
  this->outputString=out.str();
  return this->CarryOutSubstitutionInMe(theSub, theGlobalVariables);
}

void ParserNode::TrimSubToMinNumVarsChangeImpliedNumVars(PolynomialSubstitution<Rational>& theSub, int theDimension)
{ theSub.SetSize(theDimension);
  for (int i=0; i<theSub.size; i++)
    theSub.TheObjects[i].SetNumVariablesSubDeletedVarsByOne(theDimension);
  int minNumberVarsAfterSub=theSub.GetHighestIndexSuchThatHigherIndexVarsDontParticipate()+1;
//      std::cout << "minNumberVarsAfterSub: " << minNumberVarsAfterSub << "  " << theSub.ToString() << "<br>";
  for (int i=0; i<theDimension; i++)
    theSub.TheObjects[i].SetNumVariablesSubDeletedVarsByOne(minNumberVarsAfterSub);
  this->impliedNumVars=minNumberVarsAfterSub;
}

int ParserNode::CarryOutSubstitutionInMe(PolynomialSubstitution<Rational>& theSub, GlobalVariables& theGlobalVariables)
{ int theDimension=this->impliedNumVars;
  Rational ratOne;
  ratOne.MakeOne();
  QuasiPolynomial tempQP;
  Polynomial<Rational>  polyOneAfterSub, polyZeroAfterSub;
  switch(this->ExpressionType)
  { case ParserNode::typeWeylAlgebraElement:
      this->WeylAlgebraElement.GetElement().SubstitutionTreatPartialsAndVarsAsIndependent(theSub);
      this->ExpressionType=this->typeWeylAlgebraElement;
      return this->errorNoError;
    case ParserNode::typeIntegerOrIndex:
    case ParserNode::typeRational:
    case ParserNode::typePoly:
 //     std::cout << "<br> ... and the sub is: " << theSub.ToString();
      theSub.SetSize(theDimension);
      for (int i=0; i<theDimension; i++)
        theSub.TheObjects[i].SetNumVariablesSubDeletedVarsByOne(theDimension);
   //   std::cout << "<br> ... and the sub is: " << theSub.ToString();
      this->polyValue.GetElement().Substitution(theSub, theDimension, ratOne);
      this->ExpressionType=this->typePoly;
      return this->errorNoError;
    case ParserNode::typeArray:
      for (int i=0; i<this->children.size; i++)
        this->owner->TheObjects[this->children.TheObjects[i]].CarryOutSubstitutionInMe(theSub, theGlobalVariables);
      return this->errorNoError;
    case ParserNode::typeUEElementOrdered:
      this->TrimSubToMinNumVarsChangeImpliedNumVars(theSub, this->impliedNumVars);
      this->UEElementOrdered.GetElement().SetNumVariables(this->impliedNumVars);
      this->UEElementOrdered.GetElement().SubstitutionCoefficients(theSub, & theGlobalVariables);
      return this->errorNoError;
    case ParserNode::typeUEelement:
      this->TrimSubToMinNumVarsChangeImpliedNumVars(theSub, this->impliedNumVars);
      polyOneAfterSub.MakeOne(this->impliedNumVars);
      polyZeroAfterSub.MakeZero(this->impliedNumVars);
      this->UEElement.GetElement().SetNumVariables(this->impliedNumVars);
      this->UEElement.GetElement().SubstitutionCoefficients
      (theSub, & theGlobalVariables, polyOneAfterSub, polyZeroAfterSub);
      return this->errorNoError;
    case ParserNode::typeLattice:
      if (theDimension!=this->theLattice.GetElement().GetDim())
        return this->SetError(this->errorDimensionProblem);
      this->TrimSubToMinNumVarsChangeImpliedNumVars(theSub, theDimension);
      if (!this->theLattice.GetElement().SubstitutionHomogeneous(theSub, theGlobalVariables))
        return this->SetError(this->errorImplicitRequirementNotSatisfied);
      this->outputString=this->theLattice.GetElement().ToString(true, false);
      return this->errorNoError;
    case ParserNode::typeQuasiPolynomial:
      if(theDimension!=this->theQP.GetElement().GetNumVars())
        return this->SetError(this->errorDimensionProblem);
      this->TrimSubToMinNumVarsChangeImpliedNumVars(theSub, theDimension);
      tempQP=this->theQP.GetElement();
      if (!tempQP.SubstitutionLessVariables(theSub, this->theQP.GetElement(), theGlobalVariables))
        return this->SetError(this->errorImplicitRequirementNotSatisfied);
      this->outputString=this->theQP.GetElement().ToString(true, false);
      return this->errorNoError;
    default:
      return this->SetError(this->errorDunnoHowToDoOperation);
  }
}

void ParserNode::CopyValue(const ParserNode& other)
{ this->intValue=other.intValue;
  this->ErrorType=other.ErrorType;
  this->rationalValue=other.rationalValue;
  this->IndexContextLieAlgebra=other.IndexContextLieAlgebra;
  this->children.CopyFromBase(other.children);
  this->Evaluated= other.Evaluated;
  this->outputString= other.outputString;
  this->ExpressionType=other.ExpressionType;
  this->impliedNumVars=other.impliedNumVars;
  switch (other.ExpressionType)
  { case ParserNode::typeError:
    case ParserNode::typeIntegerOrIndex:
    case ParserNode::typeRational:
    case ParserNode::typeUndefined:
    case ParserNode::typeString:
      break;
    case ParserNode::typeWeylAlgebraElement:
      this->WeylAlgebraElement.GetElement().Assign(other.WeylAlgebraElement.GetElementConst());
      break;
    case ParserNode::typeUEelement:
      this->UEElement.GetElement().operator=(other.UEElement.GetElementConst());
      break;
    case ParserNode::typeUEElementOrdered:
      this->UEElementOrdered.GetElement().operator=(other.UEElementOrdered.GetElementConst());
      break;
    case ParserNode::typePoly:
      this->polyValue.GetElement().operator=(other.polyValue.GetElementConst());
      break;
    case ParserNode::typeMapPolY:
      this->polyValue.GetElement().operator=(other.polyValue.GetElementConst());
      this->polyBeingMappedTo.GetElement().operator=(other.polyBeingMappedTo.GetElementConst());
      break;
    case ParserNode::typeMapWeylAlgebra:
      this->WeylAlgebraElement.GetElement().operator=(other.WeylAlgebraElement.GetElementConst());
      this->weylEltBeingMappedTo.GetElement().operator=(other.weylEltBeingMappedTo.GetElementConst());
      break;
    case ParserNode::typeArray:
      break;
    case ParserNode::typeRationalFunction:
      this->ratFunction.GetElement().operator=(other.ratFunction.GetElementConst());
      break;
    case ParserNode::typeCone:
      this->theCone=other.theCone;
      break;
    case ParserNode::typePartialFractions:
      this->thePFs=other.thePFs;
      break;
    case ParserNode::typePiecewiseQP:
      this->thePiecewiseQP=other.thePiecewiseQP;
    case ParserNode::typeQuasiPolynomial:
      this->theQP=other.theQP;
      break;
    case ParserNode::typeLattice:
      this->theLattice=other.theLattice;
      break;
    case ParserNode::typeFile:
      break;
    case ParserNode::typeAnimation:
      this->theAnimation=other.theAnimation;
      break;
    case ParserNode::typeCharSSFDMod:
      this->theChar=other.theChar;
      break;
    case ParserNode::typeLittelman:
      this->theLittelmann=other.theLittelmann;
      break;
    case ParserNode::typeGenVermaElt:
      this->theGenVermaElt=other.theGenVermaElt;
      break;
    default:
      this->SetError(errorProgramming);
      this->outputString="The lazy programmer(s) forgot to implement the correct \
      copying procedure for the parser node. Feel free to send them anry emails!";
      break;
  }
}

void ParserNode::operator=(const ParserNode& other)
{ this->owner=other.owner;
  this->indexParentNode=other.indexParentNode;
  this->indexInOwner=other.indexInOwner;
  this->Operation=other.Operation;
  this->CopyValue(other);
}

bool Parser::lookAheadTokenAllowsMapsTo(int theToken)
{ return theToken==this->tokenComma || theToken==this->tokenCloseBracket || theToken==this->tokenEnd || theToken==this->tokenEndStatement || theToken==this->tokenColon;
}

bool Parser::StackTopIsAVector(int& outputDimension)
{ return this->StackTopIsDelimiter1ECdotsCEDelimiter2(outputDimension, this->tokenOpenBracket, this->tokenCloseBracket);
}

bool Parser::ReplaceDdotsDbyEdoTheArithmetic()
{ int numTotalDigits=0;
  int indexFirstDoT=this->TokenStack.size;
  for (int i= this->TokenStack.size-1; i>=0; i--)
    if(this->TokenStack.TheObjects[i]!=this->tokenDigit)
    { if (this->TokenStack.TheObjects[i]==this->tokenDot)
      { if (indexFirstDoT!=this->TokenStack.size)
          return false;
        indexFirstDoT=i;
      } else
        break;
    }
    else
      numTotalDigits++;
  int numXs=numTotalDigits;
  if (indexFirstDoT!=this->TokenStack.size)
    numXs++;
  assert(numTotalDigits>0);
  LargeIntUnsigned LargeIntegerReader;
  LargeIntegerReader.MakeZero();

  for (int i=this->ValueStack.size-numXs; i<indexFirstDoT; i++)
  { LargeIntegerReader*=10;
    LargeIntegerReader+= this->ValueStack.TheObjects[i];
  }
  for (int i=indexFirstDoT+1; i<this->ValueStack.size; i++)
  { LargeIntegerReader*=10;
    LargeIntegerReader+= this->ValueStack.TheObjects[i];
  }
  Rational denominator=1;
  if (this->TokenStack.size>indexFirstDoT)
  { denominator=10;
    MathRoutines::RaiseToPower(denominator, this->TokenStack.size-indexFirstDoT-1, (Rational) 1);
    denominator.Invert();
  }
  this->ExpandOnTopIncreaseStacks(this->tokenInteger, this->tokenExpression, 0);
  this->LastObject()->rationalValue=LargeIntegerReader;
  if (!denominator.IsEqualToOne())
    this->LastObject()->rationalValue*=denominator;
  this->TransformRepeatXAtoA(numXs);
  return true;
}

bool Parser::ReplaceXECdotsCEXbyE(int theDimension, int theNewToken, int theOperation)
{ this->ExpandOnTopIncreaseStacks(theOperation, theNewToken, 0);
  int stackSize=this->ValueStack.size;
  for (int i=0; i<theDimension; i++)
    this->FatherByLastNodeChildrenWithIndexInNodeIndex(stackSize-1-2*theDimension+2*i);
  this->TransformRepeatXAtoA(theDimension*2+1);
  return true;
}

bool Parser::ReplaceXECdotsCEXEXbyE(int theDimension, int theNewToken, int theOperation)
{ return this->ReplaceXECdotsCEXbyE(theDimension+1, theNewToken, theOperation);
}

bool Parser::StackTopIsDelimiter1ECdotsCEDelimiter2(int& outputDimension, int LeftDelimiter, int RightDelimiter)
{ if (*this->TokenStack.LastObject()!=RightDelimiter)
    return false;
  outputDimension=0;
  for (int i=this->TokenStack.size-2; i>=1; i--)
  { if (this->TokenStack.TheObjects[i]!=this->tokenExpression)
      return false;
    i--;
    outputDimension++;
    if (this->TokenStack.TheObjects[i]==LeftDelimiter)
      return true;
    if (this->TokenStack.TheObjects[i]!=this->tokenComma)
      return false;
  }
  return false;
}

bool Parser::StackTopIsDelimiter1ECdotsCEDelimiter2EDelimiter3
  (int& outputDimension, int LeftDelimiter, int middleDelimiter, int rightDelimiter)
{ int LastToken=this->TokenStack.TheObjects[this->TokenStack.size-1];
  int SecondToLastToken=this->TokenStack.TheObjects[this->TokenStack.size-2];
  int ThirdToLastToken=this->TokenStack.TheObjects[this->TokenStack.size-3];
  if (LastToken!=rightDelimiter || SecondToLastToken!=this->tokenExpression || ThirdToLastToken!=middleDelimiter)
    return false;
  outputDimension=0;
  for (int i=this->TokenStack.size-4; i>=1; i--)
  { if (this->TokenStack.TheObjects[i]!=this->tokenExpression)
      return false;
    i--;
    outputDimension++;
    if (this->TokenStack.TheObjects[i]==LeftDelimiter)
      return true;
    if (this->TokenStack.TheObjects[i]!=this->tokenComma)
      return false;
  }
  return false;
}

void ElementWeylAlgebra::SubstitutionTreatPartialsAndVarsAsIndependent
(PolynomialSubstitution<Rational>& theSub)
{ assert(theSub.size==this->NumVariables*2);
//  std::cout << "<br>...and the sub is: " << theSub.ToString();
  this->StandardOrder.Substitution(theSub, this->NumVariables*2, (Rational) 1);
}

void ElementWeylAlgebra::RaiseToPower(int thePower)
{ ElementWeylAlgebra WeylOne;
  WeylOne.MakeConst(this->NumVariables, (Rational) 1);
  MathRoutines::RaiseToPower(*this, thePower, WeylOne);
}

void HomomorphismSemisimpleLieAlgebra::GetWeightsWrtKInSimpleCoordsK
  (Vectors<Rational>& outputWeights, List<ElementSemisimpleLieAlgebra>& inputElts, GlobalVariables& theGlobalVariables)
{ outputWeights.SetSize(inputElts.size);
  Rational tempRat;
  ElementSemisimpleLieAlgebra tempLieElement;
  for (int i=0; i<inputElts.size; i++)
  { Vector<Rational>& currentWeight= outputWeights.TheObjects[i];
    currentWeight.MakeZero(this->theDomain().GetRank());
    ElementSemisimpleLieAlgebra& currentLieElt=inputElts.TheObjects[i];
    for (int j=0; j<this->theDomain().GetRank(); j++)
    { this->theRange().LieBracket(this->imagesAllChevalleyGenerators.TheObjects[j+this->theDomain().GetNumPosRoots()], currentLieElt, tempLieElement);
      if(!currentLieElt.IsProportionalTo(tempLieElement, tempRat)){ assert(false);}
      currentWeight.TheObjects[j]=tempRat;
    }
  }
  Matrix<Rational>  tempMat=this->theDomain().theWeyl.CartanSymmetric;
  tempMat.Invert();
  tempMat.ActOnVectorsColumn(outputWeights);
}

void ElementSemisimpleLieAlgebra::ActOnMe
  (const ElementSemisimpleLieAlgebra& theElt, ElementSemisimpleLieAlgebra& output, SemisimpleLieAlgebra& owner)
{ owner.LieBracket(theElt, *this, output);
}

void ElementSemisimpleLieAlgebra::ActOnMe
  (const ElementSemisimpleLieAlgebra& theElt, ElementSemisimpleLieAlgebra& output, SemisimpleLieAlgebra& owner,
   const RationalFunction& theRingUnit, const RationalFunction& theRingZero, GlobalVariables* theGlobalVariables)
{ owner.LieBracket(theElt, *this, output);
}

void ElementSemisimpleLieAlgebra::GetBasisFromSpanOfElements
  (List<ElementSemisimpleLieAlgebra>& theElements, Vectors<RationalFunction>& outputCoords, List<ElementSemisimpleLieAlgebra>& outputTheBasis, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> theRootForm;
  theRootForm.SetSize(theElements.size);
  for(int i=0; i<theElements.size; i++)
  { ElementSemisimpleLieAlgebra& currentElt=theElements.TheObjects[i];
    currentElt.ElementToVectorNegativeRootSpacesFirst(theRootForm.TheObjects[i]);
  }
//  int theRank=0; int numRoots=0;
//  if (theElements.size>0)
//  { theRank=theElements.TheObjects[0].Hcomponent.size;
//    numRoots=theElements.TheObjects[0].coeffsRootSpaces.size;
//  }
  theRootForm.ChooseABasis(theGlobalVariables);
  outputTheBasis.SetSize(theRootForm.size);
  for(int i=0; i<theRootForm.size; i++)
  { ElementSemisimpleLieAlgebra& currentElt=outputTheBasis.TheObjects[i];
    currentElt.AssignVectorNegRootSpacesCartanPosRootSpaces
    (theRootForm.TheObjects[i], currentElt.ownerArray->TheObjects[currentElt.indexOfOwnerAlgebra]);
  }

}

bool ElementSemisimpleLieAlgebra::GetCoordsInBasis(const List<ElementSemisimpleLieAlgebra>& theBasis, Vector<Rational>& output, GlobalVariables& theGlobalVariables)const
{ Vectors<Rational> tempBasis;
  Vector<Rational> tempRoot;
  tempBasis.SetSize(theBasis.size);
  for (int i=0 ; i<theBasis.size; i++)
    theBasis.TheObjects[i].ElementToVectorNegativeRootSpacesFirst(tempBasis.TheObjects[i]);
  this->ElementToVectorNegativeRootSpacesFirst(tempRoot);
  return tempRoot.GetCoordsInBasiS(tempBasis, output, (Rational) 1, (Rational) 0);
}

bool RationalFunction::gcdQuicK
  (const Polynomial<Rational> & left, const Polynomial<Rational> & right, Polynomial<Rational> & output)
{ if (left.TotalDegree()>1 && right.TotalDegree()>1)
    return false;
  Polynomial<Rational>  quotient, remainder;
  if (left.TotalDegree()>right.TotalDegree())
  { left.DivideBy(right, quotient, remainder);
    if (remainder.IsEqualToZero())
      output=right;
    else
      output.MakeConst(left.NumVars, (Rational) 1);
  } else
  { //std::string tempS1, tempS2, tempS3, tempS4;
//    tempS1=left.ToString(theGlobalVariables.theDefaultPolyFormat);
//    tempS2=right.ToString(theGlobalVariables.theDefaultPolyFormat);
    right.DivideBy(left, quotient, remainder);
//    tempS3=quotient.ToString();
//    tempS4=remainder.ToString();
    if (remainder.IsEqualToZero())
      output=left;
    else
      output.MakeConst(left.NumVars, (Rational) 1);
  }
  return true;
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
  this->impliedNumVars=this->GetMaxImpliedNumVarsChildren();
  ParserNode& leftNode=this->owner->TheObjects[this->children.TheObjects[0]];
  ParserNode& rightNode=this->owner->TheObjects[this->children.TheObjects[1]];
  switch(leftNode.ExpressionType)
  { case ParserNode::typeIntegerOrIndex:
    case ParserNode::typeRational:
    case ParserNode::typePoly:
    case ParserNode::typeRationalFunction:
      this->ExpressionType=this->typeRationalFunction;
      this->InitForMultiplication(&theGlobalVariables);
      if (!rightNode.ConvertToType(this->typeRationalFunction, this->impliedNumVars, theGlobalVariables))
      { this->SetError(this->errorConversionError);
        return;
      }
      leftNode.ConvertToType(this->typeRationalFunction, this->impliedNumVars, theGlobalVariables);
      this->ratFunction.GetElement()=leftNode.ratFunction.GetElementConst();
      this->ratFunction.GetElement()/=rightNode.ratFunction.GetElement();
      this->ExpressionType=this->typeRationalFunction;
      this->ReduceRatFunction();
      break;
    case ParserNode::typeUEelement:
      if (!rightNode.ConvertToType(this->typeRational, this->impliedNumVars, theGlobalVariables))
      { this->SetError(this->errorDunnoHowToDoOperation);
        return;
      }
      this->UEElement.GetElement()=leftNode.UEElement.GetElement();
      this->UEElement.GetElement()/=rightNode.rationalValue;
      this->ExpressionType=this->typeUEelement;
      break;
    case ParserNode::typeUEElementOrdered:
      if (!rightNode.ConvertToType(this->typeRational, this->impliedNumVars, theGlobalVariables))
      { this->SetError(this->errorDunnoHowToDoOperation);
        return;
      }
      this->UEElementOrdered.GetElement()=leftNode.UEElementOrdered.GetElement();
      this->UEElementOrdered.GetElement()/=rightNode.rationalValue;
      this->ExpressionType=this->typeUEElementOrdered;
      break;
    break;
    default: this->SetError(this->errorDivisionByNonAllowedType); return;
  }
}

void RationalFunction::RaiseToPower(int thePower)
{ MacroRegisterFunctionWithName("RationalFunction::RaiseToPower");
  Polynomial<Rational>  theNum, theDen;
  this->checkConsistency();
  if (thePower<0)
  { this->Invert();
    thePower=-thePower;
  }
  if (thePower==0)
  { if (this->IsEqualToZero())
    { std::cout << "This is a programming error: attempting to raise 0 to the 0th power, which is undefined. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    this->MakeOne(this->NumVars, this->context);
    return;
  }
  switch (this->expressionType)
  { case RationalFunction::typeRational:
      this->ratValue.RaiseToPower(thePower);
      break;
    case RationalFunction::typePoly:
      this->Numerator.GetElement().RaiseToPower(thePower, (Rational) 1);
      break;
    case RationalFunction::typeRationalFunction:
      this->Numerator.GetElement().RaiseToPower(thePower, (Rational) 1);
      this->Denominator.GetElement().RaiseToPower(thePower, (Rational) 1);
      break;
  }
  this->checkConsistency();
}

void RationalFunction::gcd
(const Polynomial<Rational> & left, const Polynomial<Rational> & right, Polynomial<Rational> & output, GlobalVariables* theContext)
{ if (RationalFunction::gcdQuicK(left, right, output))
    return;
  if (theContext==0)
  { Polynomial<Rational> buffer1, buffer2, buffer3, buffer4, buffer5; List<Polynomial<Rational> > bufferList; MonomialP tempMon1, tempMon2;
    RationalFunction::gcd(left, right, output, buffer1, buffer2, buffer3, buffer4, buffer5, tempMon1, tempMon2, bufferList);
  } else
    RationalFunction::gcd
    (left, right, output, theContext->RFgcdBuffer1.GetElement(), theContext->RFgcdBuffer2.GetElement(), theContext->RFgcdBuffer3.GetElement(),
     theContext->RFgcdBuffer4.GetElement(), theContext->RFgcdBuffer5.GetElement(),  theContext->RFgcdBuferMon1.GetElement(),
     theContext->RFgcdBuferMon2.GetElement(), theContext->RFgcdBufferList1.GetElement());
}

void RationalFunction::ClearDenominators
(RationalFunction& outputWasMultipliedBy)
{ //outputWasMultipliedBy.MakeConst(this->NumVars, (Rational) 1, this->context);
  Rational tempRat;
  switch(this->expressionType)
  { case RationalFunction::typeRational:
      this->ratValue.GetDenominator(tempRat);
      outputWasMultipliedBy.MakeConst(this->NumVars, tempRat, this->context);
      this->ratValue*=tempRat;
    break;
    case RationalFunction::typePoly:
      this->Numerator.GetElement().ClearDenominators(tempRat);
      outputWasMultipliedBy.MakeConst(this->NumVars, tempRat, this->context);
    break;
    case RationalFunction::typeRationalFunction:
      RationalFunction tempRF;
      outputWasMultipliedBy.operator=(this->Denominator.GetElement());
      *this*=outputWasMultipliedBy;
      this->ClearDenominators(tempRF);
      outputWasMultipliedBy*=tempRF;
      break;
  }
}

bool ElementSemisimpleLieAlgebra::NeedsBrackets()const
{ return this->size>1;
}

void slTwoInSlN::ClimbDownFromHighestWeightAlongSl2String
  (Matrix<Rational> & input, Matrix<Rational> & output, Rational& outputCoeff, int generatorPower)
{ assert(&input!=&output);
  Rational currentWeight;
  Matrix<Rational> ::LieBracket(this->theH, input, output);
  bool tempBool=input.IsProportionalTo(output, currentWeight);
  if (!tempBool)
    std::cout << "<br>oh no! climbing down is fucked up!";
  Rational RaiseCoeff;
  RaiseCoeff.MakeZero();
  outputCoeff.MakeOne();
  output=input;
  for (int i=0; i<generatorPower; i++)
  { RaiseCoeff+=currentWeight;
    currentWeight-=2;
    outputCoeff*=RaiseCoeff;
    Matrix<Rational> ::LieBracket(this->theF, output, output);
  }
}

std::string slTwoInSlN::ElementModuleIndexToString(int input, bool useHtml)
{ std::string beginMath, endMath, newLine;
  if (useHtml)
  { beginMath= "<span class=\"math\">";
    endMath= "</span>";
    newLine="<br>";
  } else
  { beginMath="$";
    endMath="$";
    newLine="\n\n\n";
  }
  Matrix<Rational> & currentHW=this->theHighestWeightVectors.TheObjects[input];
  int currentEtaHw=this->theGmodKModules.TheObjects[input].size-1;
  //currentEtaHw-=currentEtaHw/2;
  int firstNonZeroRow=-1, firstNonZeroColumn=-1;
  bool found=false;
  for (int i=0; i<currentHW.NumRows; i++)
  { if (found)
      break;
    for (int j=0; j<currentHW.NumCols; j++)
      if (!currentHW.elements[i][j].IsEqualToZero())
      { firstNonZeroColumn=j;
        firstNonZeroRow=i;
        found =true;
        break;
      }
  }
  int sRow, kRow, sColumn, kColumn;
  this->GetIsPlusKIndexingFrom(firstNonZeroRow, sRow, kRow);
  this->GetIsPlusKIndexingFrom(firstNonZeroColumn, sColumn, kColumn);
  std::stringstream out;
  out << "V_{";
  if (currentEtaHw!=0)
  { if (currentEtaHw!=1)
      out << currentEtaHw;
    out << "\\frac\\eta 2";
  }
  if (sRow!=sColumn)
    out << "-\\zeta_" << sColumn << "+\\zeta_" << sRow;
  out << "}";
  return out.str();
}

void slTwoInSlN::GetIsPlusKIndexingFrom(int input, int& s, int& k)
{ s=0;
  k=input;
  if (input >=this->theDimension || input <0)
    return;
  for (int offset=0; offset<=input; offset+=this->thePartition.TheObjects[s-1])
  { k=input-offset;
    s++;
  }
}

std::string slTwoInSlN::ElementMatrixToTensorString(const Matrix<Rational> & input, bool useHtml)
{ std::string beginMath, endMath, newLine;
  if (useHtml)
  { beginMath= "<span class=\"math\">";
    endMath= "</span>";
    newLine="<br>";
  } else
  { beginMath="$";
    endMath="$";
    newLine="\n\n\n";
  }
  std::stringstream out;
  std::string tempS;
  bool found=false;
  for (int i=0; i<input.NumRows; i++)
    for(int j=0; j<input.NumCols; j++)
      if (!input.elements[i][j].IsEqualToZero())
      { tempS=input.elements[i][j].ToString();
        if (tempS=="-1")
          tempS="-";
        if (tempS=="1")
        { tempS="";
          if (found)
            out << "+";
        } else
          if (found)
            if (tempS[0]!='-')
              out << "+";
        found=true;
        out << tempS;
        int sI, kI, sJ, kJ;
        this->GetIsPlusKIndexingFrom(i, sI, kI);
        this->GetIsPlusKIndexingFrom(j, sJ, kJ);
        out << "v_{i_{" << sI << "}";
        if (kI!=0)
          out <<"+" << kI;
        out << "}\\otimes v^*_{i_{" << sJ << "}";
        if (kJ!=0)
          out  << "+" << kJ;
        out << "}";
      }
  return out.str();
}

void slTwoInSlN::ExtractHighestWeightVectorsFromVector
  (Matrix<Rational> & input, List<Matrix<Rational> >& outputDecompositionOfInput, List<Matrix<Rational> >& outputTheHWVectors)
{ outputDecompositionOfInput.size=0; outputTheHWVectors.size=0;
  Matrix<Rational>  remainder; remainder=input;
  Matrix<Rational>  component, highestWeightVector, tempMat;
  Rational theCoeff, tempRat;
  int largestPowerNotKillingInput;
  while(!remainder.IsEqualToZero() )
  { //std::cout << "<br>remainder:<div class=\"math\">" << remainder.ToString(false, true) << "</div>";
    this->ClimbUpFromVector(remainder, highestWeightVector, largestPowerNotKillingInput);
    //std::cout << "<br>highest weight vector:<div class=\"math\">" << highestWeightVector.ToString(false, true) << "</div>";
    this->ClimbDownFromHighestWeightAlongSl2String(highestWeightVector, component, theCoeff, largestPowerNotKillingInput);
    for (int i=0; i<this->theProjectors.size; i++)
    { Matrix<Rational> & currentProjector=this->theProjectors.TheObjects[i];
      tempMat=highestWeightVector;
      tempMat.MultiplyOnTheLeft(currentProjector);
      if (!tempMat.IsEqualToZero())
      { tempMat.FindFirstNonZeroElementSearchEntireRow(tempRat);
        tempMat/=tempRat;
        outputTheHWVectors.AddOnTop(tempMat);
      }
    }
    //assert(!theCoeff.IsEqualToZero());
    component/=(theCoeff);
    outputDecompositionOfInput.AddOnTop(component);
    //std::cout << "<br>component:<div class=\"math\">" << component.ToString(false, true) << "</div><br><br><br><br>";
    remainder-=(component);
  }
  //remainder.NullifyAll();
//  for (int i=0; i<outputVectors.size; i++)
//    remainder.Add(outputVectors.TheObjects[i]);
//  std::cout << "<br>sum of all components:<div class=\"math\">" << remainder.ToString(false, true) << "</div>";

}

void slTwoInSlN::ClimbUpFromVector
  (Matrix<Rational> & input, Matrix<Rational> & outputLastNonZero, int& largestPowerNotKillingInput)
{ Matrix<Rational>  tempMat;
  assert(&input!=&outputLastNonZero);
  outputLastNonZero=input;
  largestPowerNotKillingInput=0;
  for(Matrix<Rational> ::LieBracket(this->theE, outputLastNonZero, tempMat); !tempMat.IsEqualToZero(); Matrix<Rational> ::LieBracket(this->theE, outputLastNonZero, tempMat))
  { largestPowerNotKillingInput++;
    outputLastNonZero=tempMat;
  }
}

std::string slTwoInSlN::GetNotationString(bool useHtml)
{ std::stringstream out;
  std::string beginMath, endMath, newLine;
  if (useHtml)
  { beginMath= "<span class=\"math\">";
    endMath= "</span>";
    newLine="<br>";
  } else
  { beginMath="$";
    endMath="$";
    newLine="\n\n\n";
  }
  out << newLine << "Let the starting index of the j-th block be " << beginMath << "i_j" << endMath;
  out << "." << newLine << "In particular let: ";
  int offset=1;
  for (int i=0; i< this->thePartition.size; i++)
  { out << beginMath << "i_" << i+1 << "=" << offset << endMath << " ";
    out << " (size of block = " << this->thePartition.TheObjects[i] << "), ";
    offset+=this->thePartition.TheObjects[i];
  }
  out << newLine << "Let " << beginMath << "\\eta" << endMath << " be the weight corresponding to h.";
  out << newLine << "Let " << beginMath << "\\zeta_{j}" << endMath << " be the weight corresponding to the j-th block";
  return out.str();
}

std::string slTwoInSlN::initFromModuleDecomposition
(List<int>& decompositionDimensions, bool useHtml, bool computePairingTable)
{ std::stringstream out;
  this->thePartition.CopyFromBase(decompositionDimensions);
  this->thePartition.QuickSortDescending();
  this->theDimension=0;
  for (int i=0; i<this->thePartition.size; i++)
    this->theDimension+=this->thePartition.TheObjects[i];
  theH.init(this->theDimension, this->theDimension); theH.NullifyAll();
  theE.init(this->theDimension, this->theDimension); theE.NullifyAll();
  theF.init(this->theDimension, this->theDimension); theF.NullifyAll();
  this->theProjectors.SetSize(this->thePartition.size);
  int currentOffset=0;
  std::string beginMath, endMath, newLine;
  if (useHtml)
  { beginMath= "<span class=\"math\">";
    endMath= "</span>";
    newLine="<br>";
  } else
  { beginMath="$";
    endMath="$";
    newLine="\n\n\n";
  }
  for (int i=0; i<this->thePartition.size; i++)
  { this->theProjectors.TheObjects[i].init(this->theDimension, this->theDimension);
    this->theProjectors.TheObjects[i].NullifyAll();
    for (int j=0; j<this->thePartition.TheObjects[i]; j++)
    { theH.elements[currentOffset+j][currentOffset+j]=this->thePartition.TheObjects[i]-1-2*j;
      this->theProjectors.TheObjects[i].elements[currentOffset+j][currentOffset+j]=1;
      if (j!=this->thePartition.TheObjects[i]-1)
      { theF.elements[currentOffset +j+1][currentOffset +j]=1;
        theE.elements[currentOffset +j][currentOffset +j+1]=(j+1)*(this->thePartition.TheObjects[i]-j-1);
      }
    }
    currentOffset+=this->thePartition.TheObjects[i];
  }
  out << newLine << beginMath << "h=" << this->ElementMatrixToTensorString(this->theH, useHtml) << "="
  << this->theH.ElementToStringWithBlocks(this->thePartition) << endMath;
  out << newLine << beginMath << "e=" << this->ElementMatrixToTensorString(this->theE, useHtml) << "="
  << this->theE.ElementToStringWithBlocks(this->thePartition) << endMath;
  out << newLine << beginMath << "f=" << this->ElementMatrixToTensorString(this->theF, useHtml) << "="
  << this->theF.ElementToStringWithBlocks(this->thePartition)  << endMath;
  Matrix<Rational>  tempMat;
  tempMat.init(this->theDimension, this->theDimension);
  List<Matrix<Rational> > Decomposition, theHwCandidatesBeforeProjection, theHwCandidatesProjected;
  this->theHighestWeightVectors.size=0;
  this->theGmodKModules.size=0;
  for (int i=0; i<this->theDimension; i++)
    for (int j=0; j< this->theDimension; j++)
    { tempMat.NullifyAll();
      tempMat.elements[i][j]=1;
      this->ExtractHighestWeightVectorsFromVector(tempMat, Decomposition, theHwCandidatesBeforeProjection);
      theHwCandidatesProjected.size=0;
      for (int k=0; k<theHwCandidatesBeforeProjection.size; k++)
        for (int l=0; l<this->theProjectors.size; l++)
        { tempMat=theHwCandidatesBeforeProjection.TheObjects[k];
          tempMat.MultiplyOnTheLeft(this->theProjectors.TheObjects[l]);
          if (!tempMat.IsEqualToZero())
            theHwCandidatesProjected.AddOnTop(tempMat);
        }
      for (int k=0; k<theHwCandidatesProjected.size; k++)
        if (this->GetModuleIndexFromHighestWeightVector(theHwCandidatesProjected.TheObjects[k])==-1)
        { Matrix<Rational> & currentHighest=theHwCandidatesProjected.TheObjects[k];
          this->theHighestWeightVectors.AddOnTop(currentHighest);
          this->theGmodKModules.ExpandOnTop(1);
          List<Matrix<Rational> >& currentMod=*this->theGmodKModules.LastObject();
          currentMod.size=0;
          for (tempMat=currentHighest; !tempMat.IsEqualToZero(); Matrix<Rational> ::LieBracket(this->theF, tempMat, tempMat))
            currentMod.AddOnTop(tempMat);
        }
    }
  out << this->GetNotationString(useHtml);
  out << newLine << "...and the highest weights of the module decomposition are (" <<
  this->theHighestWeightVectors.size << " modules):";
  for (int i=0; i<this->theHighestWeightVectors.size; i++)
  { out << newLine << beginMath << this->ElementMatrixToTensorString(theHighestWeightVectors.TheObjects[i], useHtml)
    << endMath << ", highest weight of ";
    out << beginMath << this->ElementModuleIndexToString(i, useHtml) << endMath;
   //for (int j=1; j<this->theGmodKModules.TheObjects[i].size; j++)
   //   out << "<br><div class=\"math\">" << this->theGmodKModules.TheObjects[i].TheObjects[j].ToString(false, true) << "</div>";
   // out << "<br><br><br>";
  }
  if (computePairingTable)
    out << this->initPairingTable(useHtml);
  return out.str();
}

std::string slTwoInSlN::initPairingTable(bool useHtml)
{ std::stringstream out;
  this->PairingTable.SetSize(this->theHighestWeightVectors.size);
  for (int i=0; i<this->PairingTable.size; i++)
  { this->PairingTable.TheObjects[i].SetSize(this->theHighestWeightVectors.size);
    for(int j=0; j<this->PairingTable.TheObjects[i].size; j++)
    { List<int>& currentPairing=this->PairingTable.TheObjects[i].TheObjects[j];
      out << this->PairTwoIndices(currentPairing, i, j, useHtml);
    }
  }
  return out.str();
}

std::string slTwoInSlN::PairTwoIndices
  (List<int>& output, int leftIndex, int rightIndex, bool useHtml)
{ std::string beginMath, endMath, newLine;
  if (useHtml)
  { beginMath= "<span class=\"math\">";
    endMath= "</span>";
    newLine="<br>";
  } else
  { beginMath="$";
    endMath="$";
    newLine="\n\n\n";
  }
  std::stringstream out;
  output.size=0;
  List<Matrix<Rational> >& leftElements=this->theGmodKModules.TheObjects[leftIndex];
  List<Matrix<Rational> >& rightElements=this->theGmodKModules.TheObjects[rightIndex];
  Matrix<Rational>  tempMat;
  List<Matrix<Rational> > HighestWeightsContainingModules;
  List<Matrix<Rational> > tempDecomposition;
  for (int i=0; i<leftElements.size; i++)
    for (int j=0; j<rightElements.size; j++)
    { Matrix<Rational> & leftElt=leftElements.TheObjects[i];
      Matrix<Rational> & rightElt=rightElements.TheObjects[j];
      Matrix<Rational> ::LieBracket(leftElt, rightElt, tempMat);
      if (!tempMat.IsEqualToZero())
      { this->ExtractHighestWeightVectorsFromVector(tempMat, tempDecomposition, HighestWeightsContainingModules);
        for (int k=0; k<HighestWeightsContainingModules.size; k++)
        { output.AddOnTopNoRepetition(this->GetModuleIndexFromHighestWeightVector(HighestWeightsContainingModules.TheObjects[k]));
          if (this->GetModuleIndexFromHighestWeightVector(HighestWeightsContainingModules.TheObjects[k])==-1)
            std::cout << newLine << beginMath << "[" << leftElt.ToString(false, true) << ", "
            << rightElt.ToString(false, true) << "]=" << tempMat.ToString(false, true) << endMath;
        }
      }
    }
  out << newLine << beginMath << this->ElementModuleIndexToString(leftIndex, useHtml) << endMath << " and "
  << beginMath << this->ElementModuleIndexToString(rightIndex, useHtml) << endMath << " pair to: ";
  for (int i=0; i<output.size; i++)
  { out << beginMath << this->ElementModuleIndexToString(output.TheObjects[i], useHtml) << endMath;
    if (i!=output.size-1)
      out << beginMath << "\\oplus" << endMath;
  }
  if (output.size>0)
  { out << "  hw vectors: ";
    for (int i=0; i<output.size; i++)
      out << beginMath << this->ElementMatrixToTensorString(this->theHighestWeightVectors.TheObjects[output.TheObjects[i]], useHtml) << endMath << ",";
  }
  return out.str();
}

int ParserNode::EvaluateSlTwoInSlN
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ List<int> thePartition;
  thePartition.SetSize(theArgumentList.size);
  for (int i=0; i<theArgumentList.size; i++)
  { thePartition.TheObjects[i]=theNode.owner->TheObjects[theArgumentList.TheObjects[i]].intValue;
//    std::cout<< thePartition.TheObjects[i] << ",";
  }
  slTwoInSlN theSl2;
  std::stringstream tempStream;
  tempStream << "\\documentclass{article} \\begin{document}\n"
  << theSl2.initFromModuleDecomposition(thePartition, false, true) << "\n\\end{document}";
  theNode.CreateDefaultLatexAndPDFfromString(tempStream.str());
  return theNode.errorNoError;
}

void RationalFunction::AddHonestRF(const RationalFunction& other)
{ MacroRegisterFunctionWithName("RationalFunction::AddHonestRF");
  Rational tempRat;
  if (!this->Denominator.GetElement().IsProportionalTo(other.Denominator.GetElementConst(), tempRat, (Rational) 1))
  { Polynomial<Rational> buffer;
//    RationalFunction debugger;
//    debugger=other;
//    debugger.ComputeDebugString();
//    this->ComputeDebugString();
    buffer=this->Denominator.GetElement();
    this->Numerator.GetElement()*=(other.Denominator.GetElementConst());
    buffer*=(other.Numerator.GetElementConst());
    this->Numerator.GetElement()+=(buffer);
    this->Denominator.GetElement()*=(other.Denominator.GetElementConst());
    assert(!this->Denominator.GetElement().IsEqualToZero());
    this->Simplify();
//    this->ComputeDebugString();
  } else
  { this->Numerator.GetElement()*=(tempRat);
    this->Denominator.GetElement()*=(tempRat);
    this->Numerator.GetElement()+=(other.Numerator.GetElementConst());
    this->ReduceMemory();
    this->SimplifyLeadingCoefficientOnly();
  }
  assert(this->checkConsistency());
}

int ParserNode::EvaluateTimes(GlobalVariables& theGlobalVariables)
{ if (!this->AllChildrenAreOfDefinedNonErrorType())
    return this->SetError(this->errorOperationByUndefinedOrErrorType);
  assert(this->children.size==2);
  ParserNode& right=this->owner->TheObjects[this->children[1]];
  if (right.ExpressionType==this->typeGenVermaElt)
  { List<int> argumentList;
    argumentList.AddOnTop(this->children[0]);
    argumentList.AddOnTop(this->children[1]);
    return this->EvaluateMultiplyEltGenVermaOnTheRight(*this, argumentList, theGlobalVariables);
  }
  else
    this->ConvertChildrenAndMyselfToStrongestExpressionChildren(theGlobalVariables);
  this->InitForMultiplication(&theGlobalVariables);
  LargeInt theInt;
  std::string tempS;
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
      case ParserNode::typeCharSSFDMod:
        theGlobalVariables.MaxAllowedComputationTimeInSeconds=MathRoutines::Maximum
        (theGlobalVariables.MaxAllowedComputationTimeInSeconds, 30);
//        tempS=(this->theChar.GetElement().MultiplyBy(currentChild.theChar.GetElement(), theGlobalVariables));
        if (tempS!="")
        { this->ExpressionType=this->typeError;
          this->ErrorType=this->errorImplicitRequirementNotSatisfied;
          this->outputString=tempS;
        } else
        { this->ExpressionType=this->typeCharSSFDMod;
          this->outputString="The tensor product decomposition is computed using the Brauer-Klimyk formula, Humphreys,  \
          Introduction to Lie algebras and representation theory (Springer, Third printing, revised, 1980), page 142, exercise 9. \
          A faster implementation of the formula, together with detailed algorithm explanation, \
          is available from the online interface of LiE. ";
        }
        break;
      case ParserNode::typeRational: this->rationalValue*=currentChild.rationalValue; break;
      case ParserNode::typeRationalFunction: this->ratFunction.GetElement()*=currentChild.ratFunction.GetElement(); break;
      case ParserNode::typePoly: this->polyValue.GetElement()*=(currentChild.polyValue.GetElement()); break;
      case ParserNode::typeUEelement: this->UEElement.GetElement()*=currentChild.UEElement.GetElement(); break;
      case ParserNode::typeUEElementOrdered: this->UEElementOrdered.GetElement()*=currentChild.UEElementOrdered.GetElement(); break;
      case ParserNode::typeWeylAlgebraElement: this->WeylAlgebraElement.GetElement().MultiplyOnTheRight(currentChild.WeylAlgebraElement.GetElement()); break;
      case ParserNode::typeLattice:
        this->theLattice.GetElement()=this->owner->TheObjects[this->children.TheObjects[0]].theLattice.GetElement();
        this->theLattice.GetElement().IntersectWith(this->owner->TheObjects[this->children.TheObjects[1]].theLattice.GetElement());
        this->outputString=this->theLattice.GetElement().ToString();
        return this->errorNoError;
      default: return this->SetError(this->errorMultiplicationByNonAllowedTypes);
    }
  }
  return this->errorNoError;
}

int ParserNode::EvaluatePlus(GlobalVariables& theGlobalVariables)
{ if (!this->AllChildrenAreOfDefinedNonErrorType())
    return this->SetError(this->errorOperationByUndefinedOrErrorType);
  this->ConvertChildrenAndMyselfToStrongestExpressionChildren(theGlobalVariables);
  this->InitForAddition(&theGlobalVariables);
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
      case ParserNode::typeRationalFunction: this->ratFunction.GetElement()+=currentChild.ratFunction.GetElement(); break;
      case ParserNode::typePoly: this->polyValue.GetElement()+=(currentChild.polyValue.GetElement()); break;
      case ParserNode::typeUEElementOrdered: this->UEElementOrdered.GetElement().operator+=(currentChild.UEElementOrdered.GetElement()); break;
      case ParserNode::typeUEelement: this->UEElement.GetElement()+=currentChild.UEElement.GetElement(); break;
      case ParserNode::typeWeylAlgebraElement: this->WeylAlgebraElement.GetElement().Add(currentChild.WeylAlgebraElement.GetElement()); break;
      case ParserNode::typeCharSSFDMod:
        this->theChar.GetElement()+=currentChild.theChar.GetElement();
        break;
      case ParserNode::typeLattice:
        this->theLattice.GetElement()=currentChild.theLattice.GetElement();
        this->theLattice.GetElement().RefineByOtherLattice(this->owner->TheObjects[this->children.TheObjects[1]].theLattice.GetElement());
        this->outputString=this->theLattice.GetElement().ToString();
        return this->errorNoError;
      case ParserNode::typeQuasiPolynomial: this->theQP.GetElement()+=currentChild.theQP.GetElement();
        this->outputString=this->theQP.GetElement().ToString(true, false);
        break;
      case ParserNode::typeAnimation:
        this->theAnimation.GetElement()+=currentChild.theAnimation.GetElement();
        break;
      case ParserNode::typeGenVermaElt:
        this->theGenVermaElt.GetElement()+=currentChild.theGenVermaElt.GetElement();
        break;
      default: return this->SetError(this->errorDunnoHowToDoOperation);
    }
  }
  return this->errorNoError;
}

void CGI::CivilizedStringTranslationFromCGI(std::string& input, std::string& output)
{ std::string readAhead;
  std::stringstream out;
  int inputSize=(signed) input.size();
  for (int i=0; i<inputSize; i++)
  { readAhead="";
    for (int j=0; j<6; j++)
    { if (i+j<inputSize)
        readAhead.push_back(input[i+j]);
      if (CGI::AttemptToCivilize(readAhead, out))
      { i+=j;
        break;
      }
    }
  }
  output=out.str();
}

void CGI::ChopCGIInputStringToMultipleStrings
(const std::string& input, List<std::string>& outputData, List<std::string>& outputFieldNames)
{ int inputLength= (signed) input.size();
  bool readingData=false;
  outputData.ReservE(10);
  outputFieldNames.ReservE(10);
  outputData.SetSize(1);
  outputFieldNames.SetSize(1);
  outputData[0]="";
  outputFieldNames[0]="";
  for (int i=0; i<inputLength; i++)
  { if (input[i]=='=')
      readingData=!readingData;
    if (input[i]=='&')
    { outputData.SetSize(outputData.size+1);
      outputFieldNames.SetSize(outputData.size);
      outputData.LastObject()->reserve(1000);
      outputFieldNames.LastObject()->reserve(20);
      *outputFieldNames.LastObject()="";
      *outputData.LastObject()="";
      readingData=false;
    }
    if (input[i]!='=' && input[i]!='&')
    { if (readingData)
        outputData.LastObject()->push_back(input[i]);
      else
        outputFieldNames.LastObject()->push_back(input[i]);
    }
  }
}

bool CGI::AttemptToCivilize(std::string& readAhead, std::stringstream& out)
{ if (readAhead[0]!='%' && readAhead[0]!='&' && readAhead[0]!='+')
  { out << readAhead[0];
    return true;
  }
  if (readAhead=="&")
  { out << " ";
    return true;
  }
  if (readAhead=="+")
  { out << " ";
    return true;
  }
  if (readAhead=="%2B")
  { out << "+";
    return true;
  }
  if (readAhead=="%28")
  { out << "(";
    return true;
  }
  if (readAhead=="%29")
  { out << ")";
    return true;
  }
  if (readAhead=="%5B")
  { out << "[";
    return true;
  }
  if (readAhead=="%5D")
  { out << "]";
    return true;
  }
  if (readAhead=="%2C")
  { out << ",";
    return true;
  }
  if (readAhead=="%7B")
  { out << "{";
    return true;
  }
  if (readAhead=="%27")
  { out << "'";
    return true;
  }
  if (readAhead=="%3B")
  { out << ";";
    return true;
  }
  if (readAhead=="%3C")
  { out << "<";
    return true;
  }
  if (readAhead=="%3E")
  { out << ">";
    return true;
  }
  if (readAhead=="%2F")
  { out << "/";
    return true;
  }
  if (readAhead=="%3A")
  { out << ":";
    return true;
  }
  if (readAhead=="%5E")
  { out << "^";
    return true;
  }
  if (readAhead=="%5C")
  { out << "\\";
    return true;
  }
  if (readAhead=="%26")
  { out << "&";
    return true;
  }
  if (readAhead=="%3D")
  { out << "=";
    return true;
  }
  if (readAhead=="%7D")
  { out << "}";
    return true;
  }
  if (readAhead=="%0D%0A")
  { out << "\n";
    return true;
  }
  if (readAhead=="%25")
  { out << "%";
    return true;
  }
  return false;
}

template < >
Rational& ParserNode::GetElement<Rational>()
{ return this->rationalValue;
}

template < >
RationalFunction& ParserNode::GetElement<RationalFunction>()
{ return this->ratFunction.GetElement();
}

template < >
Polynomial<Rational> & ParserNode::GetElement<Polynomial<Rational> >()
{ return this->polyValue.GetElement();
}

bool MonomialP::IsGEQLexicographicLastVariableStrongest(const MonomialP& m)const
{ assert(this->monBody.size==m.monBody.size);
  for (int i=this->monBody.size-1; i>=0; i--)
  { if ((*this)[i]>m[i])
      return true;
    if ((*this)[i]<m[i])
      return false;
  }
  return true;
}


bool MonomialP::IsGEQpartialOrder(MonomialP& m)
{ assert(this->monBody.size == m.monBody.size);
  for (int i=0; i<m.monBody.size; i++)
    if ((*this)[i]<m[i])
      return false;
  return true;
}

bool MonomialP::IsGEQLexicographicLastVariableWeakest(const MonomialP& m)const
{ assert(this->monBody.size==m.monBody.size);
  for (int i=0; i<this->monBody.size; i++)
  { if ((*this)[i]>m[i])
      return true;
    if ((*this)[i]<m[i])
      return false;
  }
  return true;
}

void MonomialP::MultiplyBy(const MonomialP& other)
{ int numCycles=MathRoutines::Minimum(this->monBody.size, other.monBody.size);
  for (int i=0; i<numCycles; i++)
    (*this)[i]+=other[i];
  this->SetNumVariablesSubDeletedVarsByOne
  (MathRoutines::Maximum(this->monBody.size, other.monBody.size));
}

void MonomialP::DivideBy(const MonomialP& other)
{ int numCycles=MathRoutines::Minimum(this->monBody.size, other.monBody.size);
  for (int i=0; i<numCycles; i++)
    (*this)[i]-=other[i];
  this->SetNumVariablesSubDeletedVarsByOne
  (MathRoutines::Maximum(this->monBody.size, other.monBody.size));
}

void MonomialP::SetNumVariablesSubDeletedVarsByOne(int newNumVars)
{ if (newNumVars<0)
    newNumVars=0;
  int oldSize=this->monBody.size;
  this->monBody.SetSize(newNumVars);
  for(int i=oldSize; i<this->monBody.size; i++)
    (*this)[i]=0;
}

bool Cone::IsInCone(const Vector<Rational>& point) const
{ if (this->flagIsTheZeroCone)
    return point.IsEqualToZero();
  Rational tempRat;
  for (int i=0; i<this->Normals.size; i++)
  { tempRat=Vector<Rational>::ScalarEuclidean(this->Normals[i], point);
    if (tempRat.IsNegative())
      return false;
  }
  return true;
}

void MathRoutines::NChooseK(int n, int k, LargeInt& result)
{ result=1;
  for (int i=0; i<k; i++)
  { result*=(n-i);
    result/=(i+1);
  }
}

std::string MonomialP::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  MemorySaving<FormatExpressions> tempFormat;
  if (theFormat==0)
    theFormat=&tempFormat.GetElement();
  if (this->IsAConstant())
    return "1";
  for (int i=0; i<this->monBody.size; i++)
    if (!(*this)[i].IsEqualToZero())
    { out << theFormat->GetPolyLetter(i);
      if (!((*this)[i]==1))
        out << "^{" << (*this)[i] << "}";
    }
  return out.str();
}
