#include "vpf.h"
#include "vpfCharacters.h"
#include "vpfGraph.h"
#include "vpfCharacters_CalculatorInterface.h"

static ProjectInformationInstance ProjectInfoVpfCharactersCalculatorInterfaceCPP
(__FILE__, "Weyl group calculator interface");

bool WeylGroupCalculatorFunctions::innerWeylOrbit
(CommandList& theCommands, const Expression& input, Expression& output,
 bool useFundCoords, bool useRho)
{ if (!input.IsListNElements(3))
    return output.SetError("innerWeylOrbit takes two arguments", theCommands);
  const Expression& theSSalgebraNode=input[1];
  const Expression& vectorNode=input[2];
  SemisimpleLieAlgebra* theSSalgebra;
  std::string errorString;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerSSLieAlgebra, theSSalgebraNode, theSSalgebra, &errorString))
    return output.SetError(errorString, theCommands);
  Vector<Polynomial<Rational> > theHWfundCoords, theHWsimpleCoords, currentWeight;
  Expression theContext;
  if (!theCommands.GetVectoR(vectorNode, theHWfundCoords, &theContext, theSSalgebra->GetRank(), Serialization::innerPolynomial))
    return output.SetError("Failed to extract highest weight", theCommands);
  WeylGroup& theWeyl=theSSalgebra->theWeyl;
  if (!useFundCoords)
  { theHWsimpleCoords=theHWfundCoords;
    theHWfundCoords=theWeyl.GetFundamentalCoordinatesFromSimple(theHWsimpleCoords);
  } else
    theHWsimpleCoords=theWeyl.GetSimpleCoordinatesFromFundamental(theHWfundCoords);
  std::stringstream out, latexReport;
  Vectors<Polynomial<Rational> > theHWs;
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
//  theFormat.fundamentalWeightLetter="\\psi";
  theHWs.AddOnTop(theHWsimpleCoords);
  HashedList<Vector<Polynomial<Rational> > > outputOrbit;
  WeylGroup orbitGeneratingSet;
  Polynomial<Rational> theExp;
  if (!theSSalgebra->theWeyl.GenerateOrbit(theHWs, useRho, outputOrbit, false, 1921, &orbitGeneratingSet, 1921))
    out << "Failed to generate the entire orbit (maybe too large?), generated the first " << outputOrbit.size
    << " elements only.";
  else
    out << "The orbit has " << outputOrbit.size << " elements.";
  latexReport
  << "\\begin{longtable}{p{3cm}p{4cm}p{4cm}p{4cm}}Element & Eps. coord. & Image fund. coordinates& "
  << "Hw minus wt. \\\\\n<br>";
  out << "<table><tr> <td>Group element</td> <td>Image in simple coords</td> "
  << "<td>Epsilon coords</td><td>Fundamental coords</td>";
  if (useRho)
    out << "<td>Corresponding b-singular vector candidate</td>";
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
    << (useMathTag ? CGI::GetHtmlMathSpanPure(orbitGeneratingSet[i].ToString()) : orbitGeneratingSet[i].ToString())
    << "</td><td>"
    << (useMathTag ? CGI::GetHtmlMathSpanPure(orbitEltString) : orbitEltString) << "</td><td>"
    << (useMathTag ? CGI::GetHtmlMathSpanPure(orbitEltStringEpsilonCoords) : orbitEltStringEpsilonCoords)
    << "</td><td>"
    << (useMathTag ? CGI::GetHtmlMathSpanPure(weightEltString) : weightEltString)
    << "</td>";
    latexReport << "$" << orbitGeneratingSet[i].ToString(&theFormat) << "$ & $"
    << orbitEltStringEpsilonCoords
    << "$ & $"
    <<  weightEltString << "$ & $"
    << (outputOrbit[0]-outputOrbit[i]).ToStringLetterFormat(theFormat.simpleRootLetter, &theFormat)
    << "$\\\\\n<br>"
    ;
    if (useRho)
    { currentWeight=theHWsimpleCoords;
      standardElt.MakeConst(*theSSalgebra);
      bool isGood=true;
      for (int j=0; j<orbitGeneratingSet[i].size; j++)
      { int simpleIndex=orbitGeneratingSet[i][j];
        theExp=theWeyl.GetScalarProdSimpleRoot(currentWeight, simpleIndex);
        theWeyl.SimpleReflection(simpleIndex, currentWeight, useRho, false);
        theExp*=2;
        theExp/=theWeyl.CartanSymmetric.elements[simpleIndex][simpleIndex];
        if (useRho)
          theExp+=1;
        if (theExp.IsInteger(&tempInt))
          if (tempInt<0)
          { isGood=false;
            break;
          }
        standardElt.MultiplyByGeneratorPowerOnTheLeft
        (theSSalgebra->GetNumPosRoots() -simpleIndex-1, theExp);
      }
      out << "<td>";
      if (isGood)
        out << CGI::GetHtmlMathSpanPure(standardElt.ToString(&theFormat));
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

bool WeylGroupCalculatorFunctions::innerWeylGroupIrrepsAndCharTable
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (!WeylGroupCalculatorFunctions::innerWeylGroupConjugacyClasses(theCommands, input, output))
    return false;
  if (!output.IsOfType<CoxeterGroup>())
    return true;
  CoxeterGroup& theGroup=output.GetValuENonConstUseWithCaution<CoxeterGroup>();
  theGroup.ComputeInitialCharacters();
  std::stringstream out;
  for(int i=0; i<theGroup.characterTable.size; i++)
    out << theGroup.characterTable[i] << "<br>";
  theGroup.ComputeIrreducibleRepresentations();
  FormatExpressions tempFormat;
  tempFormat.flagUseLatex=true;
  tempFormat.flagUseHTML=false;
  out << theGroup.ToString(&tempFormat);
  return output.AssignValue(out.str(), theCommands);
}

bool WeylGroupCalculatorFunctions::innerWeylGroupOrbitFundRho
(CommandList& theCommands, const Expression& input, Expression& output)
{ return WeylGroupCalculatorFunctions::innerWeylOrbit(theCommands, input, output, true, true);
}

bool WeylGroupCalculatorFunctions::innerWeylGroupOrbitFund
(CommandList& theCommands, const Expression& input, Expression& output)
{ return WeylGroupCalculatorFunctions::innerWeylOrbit(theCommands, input, output, true, false);
}

bool WeylGroupCalculatorFunctions::innerWeylGroupOrbitSimple
(CommandList& theCommands, const Expression& input, Expression& output)
{ return WeylGroupCalculatorFunctions::innerWeylOrbit(theCommands, input, output, false, false);
}

bool WeylGroupCalculatorFunctions::innerWeylGroupConjugacyClasses
(CommandList& theCommands, const Expression& input, Expression& output)
{ SemisimpleLieAlgebra* thePointer;
  std::string errorString;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerSSLieAlgebra, input, thePointer, &errorString))
    return output.SetError(errorString, theCommands);
  CoxeterGroup tmpG;
  tmpG.MakeFrom(thePointer->theWeyl.theDynkinType);
  output.AssignValue(tmpG, theCommands);
  CoxeterGroup& theGroup=output.GetValuENonConstUseWithCaution<CoxeterGroup>();
  if (theGroup.CartanSymmetric.NumRows>4)
    return output.AssignValue<std::string>
    ("I have been instructed not to do this for Weyl groups of rank greater \
     than 4 because of the size of the computation.", theCommands);
  theGroup.ComputeConjugacyClasses();
  return true;
}

bool WeylGroupCalculatorFunctions::innerDecomposeProductWeylIrreps
(CommandList& theCommands, const Expression& input, Expression& output)
{ return false;
}

bool WeylGroupCalculatorFunctions::innerCoxeterElement
(CommandList& theCommands, const Expression& input, Expression& output)
{ //if (!input.IsSequenceNElementS(2))
  //return output.SetError("Function Coxeter element takes two arguments.", theCommands);
  if(input.children.size<2){
    return output.SetError
    ("Function CoxeterElement needs to know what group the element belongs to", theCommands);
  }
  //note that if input is list of 2 elements then input[0] is sequence atom, and your two elements are in fact
  //input[1] and input[2];
  SemisimpleLieAlgebra* thePointer;
  std::string errorString;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerSSLieAlgebra, input[1], thePointer, &errorString))
    return output.SetError(errorString, theCommands);
  List<int> theReflections;
  for(int i=2; i<input.children.size; i++){
    int tmp;
    if (!input[i].IsSmallInteger(& tmp))
      return false;
    theReflections.AddOnTop(tmp-1);
  }
  CoxeterGroup theGroup;
  theGroup.MakeFrom(thePointer->theWeyl.theDynkinType);
  CoxeterElement theElt;
  int indexOfOwnerGroupInObjectContainer=
  theCommands.theObjectContainer.theCoxeterGroups.AddNoRepetitionOrReturnIndexFirst(theGroup);
  //std::cout << "Group type: " << theGroup.ToString() << "<br>Index in container: "
  //<< indexOfOwnerGroupInObjectContainer;

  theElt.owner=&theCommands.theObjectContainer.theCoxeterGroups[indexOfOwnerGroupInObjectContainer];
  //std::cout << "<br>theElt.owner: " << theElt.owner;
//  std::cout << "<b>Not implemented!!!!!</b> You requested reflection indexed by " << theReflection;
  for(int i=0; i<theReflections.size; i++){
    if (theReflections[i] >= thePointer->GetRank() || theReflections[i] < 0)
      return output.SetError("Bad reflection index", theCommands);
  }
//  std::cout << "\n" << theGroup.rho << " " << theElt.owner->rho << std::endl;
  theElt.reflections=(theReflections);
  theElt.canonicalize();
  return output.AssignValue(theElt, theCommands);
}

bool CommandList::innerMinPolyMatrix
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (!theCommands.innerMatrixRational(theCommands, input, output))
    return false;
  Matrix<Rational> theMat;
  if (!output.IsOfType<Matrix<Rational> >(&theMat))
  { theCommands.Comments << "<hr> Successfully called innerMatrixRational onto input " << input.ToString()
    << " to get " << output.ToString()
    << " but the return type was not a matrix of rationals. ";
    return true;
  }
  if (theMat.NumRows!=theMat.NumCols || theMat.NumRows<=0)
    return output.SetError("Error: matrix is not square!", theCommands);
  FormatExpressions tempF;
  tempF.polyAlphabeT.SetSize(1);
  tempF.polyAlphabeT[0]="q";
  UDPolynomial<Rational> theMinPoly;
  theMinPoly.AssignMinPoly(theMat);
  return output.AssignValue(theMinPoly.ToString(&tempF), theCommands);
}
