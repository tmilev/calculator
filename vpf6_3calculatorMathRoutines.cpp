//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
#include "vpfHeader3Calculator2_InnerFunctions.h"
#include "vpfImplementationHeader2Math3_WeylAlgebra.h"

ProjectInformationInstance ProjectInfoVpf6_3cpp(__FILE__, "Implementation file: calculator front-end math routines. ");

template <class theType>
bool MathRoutines::GenerateVectorSpaceClosedWRTOperation
(List<theType>& inputOutputElts, int upperDimensionBound, void (*theBinaryOperation)(const theType& left, const theType& right, theType& output),
 GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("MathRoutines::GenerateVectorSpaceClosedWRTOperation");
  inputOutputElts[0].GaussianEliminationByRowsDeleteZeroRows(inputOutputElts);
  theType theOpResult;
  ProgressReport theReport1(theGlobalVariables), theReport2(theGlobalVariables);
  if (theGlobalVariables!=0)
    theReport1.Report("Extending vector space to closed with respect to binary operation. ");
  for (int i=0; i<inputOutputElts.size; i++)
    for (int j=i; j<inputOutputElts.size; j++)
    { theBinaryOperation(inputOutputElts[i], inputOutputElts[j], theOpResult);
      //int oldNumElts=inputOutputElts.size;
      inputOutputElts.AddOnTop(theOpResult);
      inputOutputElts[0].GaussianEliminationByRowsDeleteZeroRows(inputOutputElts);
      //if (oldNumElts<inputOutputElts.size)
        //std::cout << "<hr>Operation between <br>" << inputOutputElts[i].ToString() << " <br> " << inputOutputElts[j].ToString() << " <br>=<br> "
        //<< theOpResult.ToString();
      if (upperDimensionBound>0 && inputOutputElts.size>upperDimensionBound)
        return false;
      if (theGlobalVariables!=0)
      { std::stringstream reportStream;
        reportStream << "Accounted operation between elements " << i+1 << " and " << j+1 << " out of " << inputOutputElts.size;
        theReport2.Report(reportStream.str());
      }
    }
  return true;
}

bool CommandListFunctions::innerGenerateVectorSpaceClosedWRTLieBracket(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerGenerateVectorSpaceClosedWRTLieBracket");
  Vector<ElementWeylAlgebra<Rational> > theOps;
  Expression theContext;
  if (!input.children.size>1)
    return false;
  int upperBound=-1;
  if (!input[1].IsSmallInteger(&upperBound))
  { theCommands.Comments << "<hr>Failed to extract upper bound for the vector space dimension from the first argument. ";
    return false;
  }
  Expression inputModded=input;
  inputModded.children.RemoveIndexShiftDown(1);
  if (!theCommands.GetVectorFromFunctionArguments(inputModded, theOps, &theContext))
    return false;
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
  std::stringstream out;
  out << "Starting elements: <br>";
  for (int i=0; i<theOps.size; i++)
    out << CGI::GetHtmlMathSpanPure(theOps[i].ToString(&theFormat)) << "<br>";
  bool success=MathRoutines::GenerateVectorSpaceClosedWRTLieBracket(theOps, upperBound, theCommands.theGlobalVariableS);
  if (!success)
    out << "<br>Did not succeed with generating vector space, instead got this: " << theOps.ToString();
  else
    out << "<br>Lie bracket generates vector space of dimension " << theOps.size << " with basis: <br>" << theOps.ToString();

  return output.AssignValue(out.str(), theCommands);
}

bool CommandListFunctions::innerFourierTransformEWA(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerFourierTransformEWA");
  if (!input.IsOfType<ElementWeylAlgebra<Rational> >())
    return false;
  ElementWeylAlgebra<Rational> theElt;
  input.GetValue<ElementWeylAlgebra<Rational> >().FourierTransform(theElt, theCommands.theObjectContainer.theAlgebraicClosure);
  return output.AssignValueWithContext(theElt, input.GetContext(), theCommands);
}
