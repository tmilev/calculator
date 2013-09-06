//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
#include "vpf6_2innerFunctions.h"

ProjectInformationInstance ProjectInfoVpf6_3cpp
(__FILE__, "Implementation file: calculator front-end math routines. ");

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
      inputOutputElts.AddOnTop(theOpResult);
      inputOutputElts[0].GaussianEliminationByRowsDeleteZeroRows(inputOutputElts);
      if (upperDimensionBound>0 && inputOutputElts.size>upperDimensionBound)
        return false;
      if (theGlobalVariables!=0)
      { std::stringstream reportStream;
        reportStream << "Accunted Lie bracket out operation between elements " << i+1 << " and " << j+1 << " out of "
        << inputOutputElts.size;
        theReport2.Report(reportStream.str());
      }
    }
  return true;
}

bool CommandListFunctions::innerGenerateVectorSpaceClosedWRTLieBracket(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListFunctions::innerGenerateVectorSpaceClosedWRTLieBracket");
  Vector<ElementWeylAlgebra> theOps;
  Expression theContext;
  if (!theCommands.GetVectorFromFunctionArguments(input, theOps, &theContext))
    return false;
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
  std::stringstream out;
  out << "Starting elements: " << theOps.ToString(&theFormat);
  bool success=MathRoutines::GenerateVectorSpaceClosedWRTLieBracket(theOps, 100, theCommands.theGlobalVariableS);
  if (!success)
    out << "<br>Did not succeed with generating vector space.";
  else
    out << "<br>Lie bracket generates vector space with basis: " << theOps.ToString();

  return output.AssignValue(out.str(), theCommands);
}
