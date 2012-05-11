//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
ProjectInformationInstance ProjectInfoVpf5cpp(__FILE__, "Implementation file for the calculator parser part 2: meant for built-in functions. ");

class DoxygenInstance
{
  public:
  ElementTensorsGeneralizedVermas<RationalFunction> doXyCanYouParseME;
};


bool CommandList::fWeylDimFormula
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ IncrementRecursion recursionCounter(theCommands);
  SemisimpleLieAlgebra* theSSowner=0;
  if (theExpression.children.size!=2)
  { theExpression.SetError("This function takes 2 arguments");
    return true;
  }
  Expression LieAlgebraNameNode=theExpression.children[0];
  if (!theCommands.fSSAlgebra(theCommands, inputIndexBoundVars, LieAlgebraNameNode, comments))
  { theExpression.SetError("Failed to convert the first argument of "+theExpression.ElementToString()+ " to a semisimple Lie algebra. ");
    return true;
  }
  if (LieAlgebraNameNode.errorString!="")
  { theExpression.SetError("While trying to generate Lie algebra from the first argument of the Weyl dim formula, I got the error: " + LieAlgebraNameNode.errorString);
    return true;
  }
  theSSowner=&LieAlgebraNameNode.GetData().GetAmbientSSAlgebra();
  Vector<Rational> theWeight;
  Context tempContext(theCommands);
  if (!theExpression.children[1].GetVector<Rational>(theWeight, tempContext, theSSowner->GetRank(), 0, comments))
  { theExpression.SetError("Failed to convert the argument of the function to a highest weight vector");
    return true;
  }
  theExpression.MakeDatA
  (theSSowner->theWeyl.WeylDimFormula(theWeight, *theCommands.theGlobalVariableS),
    theCommands, inputIndexBoundVars);
  return true;
}
