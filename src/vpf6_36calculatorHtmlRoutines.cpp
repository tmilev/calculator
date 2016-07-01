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
  MapList<Expression, std::string, MathRoutines::hashString> theArguments;
  if (!CalculatorConversions::innerLoadKeysFromStatementList(theCommands, input, theArguments, &theCommands.Comments))
    return false;
  std::stringstream out;
  if (!theArguments.Contains("name"))
    return theCommands << "User input name not specified in: " << input.ToString();
  std::string boxName= CalculatorHtmlFunctions::GetUserInputBoxName(input);
  if (theCommands.theObjectContainer.theUserInputTextBoxesWithValues.Contains(boxName))
  { output=theCommands.theObjectContainer.theUserInputTextBoxesWithValues.GetValueCreateIfNotPresent(boxName);
    return true;
  }
  output.reset(theCommands);
  output.AddChildAtomOnTop(theCommands.opUserInputTextBox());
  Expression nextParamE(theCommands);
  Expression theKeyE;
  for (int i=0; i<theArguments.theKeys.size; i++ )
  { theKeyE.AssignValue(theArguments.theKeys[i], theCommands);
    nextParamE.MakeXOX(theCommands, theCommands.opDefine(), theKeyE, theArguments.theValues[i]);
    output.AddChildOnTop(nextParamE);
  }
  theCommands.theObjectContainer.theUserInputTextBoxes.AddOnTopNoRepetition(boxName);
  return true;
}

bool CalculatorHtmlFunctions::innerSetInputBox
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorHtmlFunctions::innerUserInputBox");
  MapList<Expression, std::string, MathRoutines::hashString> theArguments;
  if (!CalculatorConversions::innerLoadKeysFromStatementList(theCommands, input, theArguments, &theCommands.Comments))
    return false;
  if (!theArguments.Contains("name"))
    return theCommands << "User input name not specified in: " << input.ToString();
  if (!theArguments.Contains("value") )
    return theCommands << "Input box value not specified in: " << input.ToString();
  std::string boxName= CalculatorHtmlFunctions::GetUserInputBoxName(input);
  if (theCommands.theObjectContainer.theUserInputTextBoxesWithValues.Contains(boxName))
    return theCommands << "Input box with name: " << boxName << " already has value.";
  theCommands.theObjectContainer.theUserInputTextBoxesWithValues.SetValue
  (theArguments.GetValueCreateIfNotPresent("value"), boxName);
  std::stringstream out;
  out << "Set value to input box name: " << boxName;
  return output.AssignValue(out.str(), theCommands);
}


std::string CalculatorHtmlFunctions::GetUserInputBox(const Expression& theBox)
{ MacroRegisterFunctionWithName("CalculatorHtmlFunctions::GetUserInputBox");
  std::stringstream out;
  out << "\\FormInput{" << CalculatorHtmlFunctions::GetUserInputBoxName(theBox) << "}" ;
  return out.str();
}

std::string CalculatorHtmlFunctions::GetUserInputBoxName(const Expression& theBox)
{ MacroRegisterFunctionWithName("CalculatorHtmlFunctions::GetUserInputBoxName");
  if (theBox.owner==0)
    return "non-initialized-expression";
  Calculator& theCommands=*theBox.owner;
  MapList<Expression, std::string, MathRoutines::hashString> theArguments;
  if (!CalculatorConversions::innerLoadKeysFromStatementList
      (theCommands, theBox, theArguments))
    return "corrupt-box";
  if (!theArguments.Contains("name"))
    return "box-without-name";
  std::string theBoxName="faultyBoxName";
  if (!theArguments.GetValueCreateIfNotPresent("name").IsOfType<std::string>(&theBoxName))
    theBoxName=theArguments.GetValueCreateIfNotPresent("name").ToString();
  return theBoxName;
}

std::string CalculatorHtmlFunctions::GetMathQuillBox
 (const std::string& mathquillSpan, const std::string& answerIdMathQuillSpan,
 const std::string& answerIdSpan, const std::string& answerId,
 const std::string& mathquillObject, const std::string& previewAnswerStream,
 const std::string& updateMQfunction)
{ MacroRegisterFunctionWithName("CalculatorHtmlFunctions::GetMathQuillBox");
  std::stringstream out;
  out << "<script>\n"
  << "var " << mathquillSpan << " = document.getElementById('" << answerIdMathQuillSpan << "');\n"
  << "var " << answerIdSpan << " = document.getElementById('" << answerId << "');\n"
  << "var ignoreNextMathQuillUpdateEvent=false;\n"
  << "globalMQ.config({\n"
  << "  autoFunctionize: 'sin cos tan sec csc cot log ln'\n"
  << "  });\n"
  << mathquillObject << " = globalMQ.MathField(" << mathquillSpan << ", {\n"
  << "spaceBehavesLikeTab: true, // configurable\n"
  << "handlers: {\n"
  << "edit: function() { // useful event handlers\n"
  << "if (ignoreNextMathQuillUpdateEvent){\n"
//  << "  ignoreNextMathQuillUpdateEvent=false;\n"
  << "  return;\n"
  << "}\n"
  << answerIdSpan << ".value = " << mathquillObject << ".latex(); // simple API\n"
  << previewAnswerStream
  << "}\n"
  << "}\n"
  << "});\n"
  << "function " << updateMQfunction << "(){\n"
  << "ignoreNextMathQuillUpdateEvent=true;\n"
  << mathquillObject << ".latex(" << answerIdSpan << ".value);\n"
  << "ignoreNextMathQuillUpdateEvent=false;\n"
  << "}\n"
  << "</script>";
  return out.str();
}
