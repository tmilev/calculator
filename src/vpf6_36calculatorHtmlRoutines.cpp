//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
#include "vpfHeader3Calculator2_InnerFunctions.h"
#include "vpfHeader3Calculator1_InnerTypedFunctions.h"
#include "vpfHeader3Calculator4HtmlFunctions.h"

ProjectInformationInstance ProjectInfoVpf6_36pp(__FILE__, "Calculator html functions implementation. ");

bool CalculatorHtmlFunctions::innerUserInputBox
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorHtmlFunctions::innerUserInputBox");
  MapLisT<std::string, Expression, MathRoutines::hashString> theArguments;
  if (!CalculatorConversions::innerLoadKeysFromStatementList(theCommands, input, theArguments, &theCommands.Comments))
    return false;
  if (!theArguments.Contains("name"))
    return theCommands << "User input name not specified in: " << input.ToString();
  std::string boxName = CalculatorHtmlFunctions::GetUserInputBoxName(input);
  if (theCommands.theObjectContainer.theUserInputTextBoxesWithValues.Contains(boxName))
    return output.AssignValue(theCommands.theObjectContainer.theUserInputTextBoxesWithValues.GetValueCreate(boxName), theCommands);
  InputBox newBox;
  newBox.name = boxName;
  for (int i = 0; i<theArguments.theKeys.size; i ++)
  { if (theArguments.theKeys[i] == "value")
      newBox.value = theArguments.theValues[i];
    if (theArguments.theKeys[i] == "min")
      newBox.min = theArguments.theValues[i];
    if (theArguments.theKeys[i] == "max")
      newBox.max = theArguments.theValues[i];
    if (theArguments.theKeys[i] == "step")
      newBox.step = theArguments.theValues[i];
  }
  return output.AssignValue(newBox, theCommands);
}

bool CalculatorHtmlFunctions::innerEvaluateSymbols
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorHtmlFunctions::innerEvaluateSymbols");
  std::string theString;
  if (!input.IsOfType(&theString))
    return false;
  List<SyntacticElement> theElts;
  theCommands.ParseFillDictionary(theString, theElts);
  Expression evaluatedE;
  std::stringstream out;
  bool previousWasInteger=false;
  for (int i = 0; i < theElts.size; i ++)
  { SyntacticElement& currentElt = theElts[i];
    if (currentElt.controlIndex == theCommands.conVariable())
    { theCommands.EvaluateExpression(theCommands, currentElt.theData, evaluatedE);
      out << evaluatedE.ToString();
      continue;
    }
    if (currentElt.controlIndex == theCommands.conInteger())
    { if (!previousWasInteger)
        out << "{";
      out << currentElt.theData.ToString();
      previousWasInteger = true;
      continue;
    }
    if (previousWasInteger)
      out << "}";
    previousWasInteger = false;
    out << theCommands.controlSequences[currentElt.controlIndex];
  }
  if (previousWasInteger)
    out << "}";
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorHtmlFunctions::innerSetInputBox
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorHtmlFunctions::innerUserInputBox");
  MapLisT<std::string, Expression, MathRoutines::hashString> theArguments;
  if (!CalculatorConversions::innerLoadKeysFromStatementList(theCommands, input, theArguments, &theCommands.Comments))
    return false;
  if (!theArguments.Contains("name"))
    return theCommands << "User input name not specified in: " << input.ToString();
  if (!theArguments.Contains("value") )
    return theCommands << "Input box value not specified in: " << input.ToString();
  std::string boxName = CalculatorHtmlFunctions::GetUserInputBoxName(input);
  if (theCommands.theObjectContainer.theUserInputTextBoxesWithValues.Contains(boxName))
    return theCommands << "Input box with name: " << boxName << " already has value.";
  InputBox& theBox =
  theCommands.theObjectContainer.theUserInputTextBoxesWithValues.GetValueCreate(boxName);
  theBox.value = theArguments.GetValueCreate("value");
  std::stringstream out;
  out << "Set value to input box name: " << boxName;
  return output.AssignValue(out.str(), theCommands);
}

std::string CalculatorHtmlFunctions::GetUserInputBoxName(const Expression& theBox)
{ MacroRegisterFunctionWithName("CalculatorHtmlFunctions::GetUserInputBoxName");
  if (theBox.owner == 0)
    return "non-initialized-expression";
  Calculator& theCommands = *theBox.owner;
  MapLisT<std::string, Expression, MathRoutines::hashString> theArguments;
  if (!CalculatorConversions::innerLoadKeysFromStatementList(theCommands, theBox, theArguments))
    return "corrupt-box";
  if (!theArguments.Contains("name"))
    return "box-without-name";
  std::string theBoxName = "faultyBoxName";
  if (!theArguments.GetValueCreate("name").IsOfType<std::string>(&theBoxName))
    theBoxName = theArguments.GetValueCreate("name").ToString();
  return theBoxName;
}
