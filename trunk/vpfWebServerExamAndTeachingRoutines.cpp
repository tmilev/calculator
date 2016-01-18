//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader6WebServer.h"
#include "vpfHeader3Calculator2_InnerFunctions.h"
#include "vpfHeader7DatabaseInterface_MySQL.h"

ProjectInformationInstance projectInfoInstanceWebServerExamAndTeachingRoutines
(__FILE__, "Routines for calculus teaching: calculator exam mode.");

class SyntacticElementHTML{
public:
  std::string syntacticRole;
  std::string content;
  std::string tag;
  List<std::string> tagKeys;
  List<std::string> tagValues;
  std::string interpretedCommand;
  static int ParsingNumDummyElements;
  bool IsInterpretedByCalculatorDuringPreparation();
  bool IsInterpretedNotByCalculator();
  bool IsAnswerVerification();
  bool IsStudentAnswer();
  std::string GetKeyValue(const std::string& theKey);
  void SetKeyValue(const std::string& theKey, const std::string& theValue);
  void resetAllExceptContent();
  std::string ToStringInterpretted();
  std::string ToStringTagAndContent();
  std::string ToStringOpenTag();
  std::string ToStringCloseTag();
  std::string GetTagClass();
  std::string ToStringDebug();
  std::string GetMyVerificationTag()
  { return this->GetVerificationTag(this->GetKeyValue("id"));
  }
  static std::string GetVerificationTag(const std::string& idTagToBeVerified);
  SyntacticElementHTML(){}
  SyntacticElementHTML(const std::string& inputContent)
  { this->content=inputContent;
  }
  bool operator==(const std::string& other)
  { return this->content==other;
  }
  bool operator!=(const std::string& other)
  { return this->content!=other;
  }
};

class CalculatorHTML
{
public:
  int NumAttemptsToInterpret;
  int MaxInterpretationAttempts;
  int randomSeed;
  List<int> randomSeedsIfInterpretationFails;
  bool flagRandomSeedGiven;
  bool flagIsExamHome;
  bool flagIsExamIntermediate;
  bool flagIsExamProblem;
  std::string databaseInfo;
  std::string problemCommandsWithInbetweens;
  std::string problemCommandsNoVerification;
  std::string problemCommandsVerification;
  std::string fileName;
  std::string RelativePhysicalFileNameWithFolder;
  std::string inputHtml;
  std::string outputHtml;
  List<std::string> calculatorClasses;
  List<char> splittingChars;
  List<SyntacticElementHTML> eltsStack;
  List<SyntacticElementHTML> theContent;
  bool flagLoadedSuccessfully;
  static const std::string RelativePhysicalFolderProblemCollections;
  std::stringstream comments;
  bool CheckContent(std::stringstream& comments);
  bool CanBeMerged(const SyntacticElementHTML& left, const SyntacticElementHTML& right);
  bool LoadMe(std::stringstream& comments);
  bool ParseAndInterpretDatabaseInfo(std::stringstream& comments);
  void GenerateDatabaseInfo();
  bool ParseHTML(std::stringstream& comments);
  bool IsSplittingChar(const std::string& input);
  bool IsStudentAnswer(SyntacticElementHTML& inputElt);
  bool IsStateModifierApplyIfYes(SyntacticElementHTML& inputElt);
  bool InterpretHtml(std::stringstream& comments);
  bool InterpretHtmlOneAttempt(Calculator& theInterpreter, std::stringstream& comments);
  bool PrepareAndExecuteCommands(Calculator& theInterpreter, std::stringstream& comments);
  bool PrepareCommands(Calculator& theInterpreter, std::stringstream& comments);
  std::string CleanUpCommandString(const std::string& inputCommand);
  void InterpretNotByCalculator(SyntacticElementHTML& inputOutput);
  void InterpretGenerateLink(SyntacticElementHTML& inputOutput);
  void InterpretGenerateStudentAnswerButton(SyntacticElementHTML& inputOutput);
  std::string GetSubmitAnswersJavascript();
  std::string GetDatabaseTableName();
  void LoadCurrentProblemItem();
  std::string LoadAndInterpretCurrentProblemItem();
  bool FindExamItem();
  static unsigned int HashFunction(const CalculatorHTML& input)
  { return input.HashFunction();
  }
  unsigned int HashFunction()const
  { return MathRoutines::hashString(this->fileName);
  }
  bool operator==(const CalculatorHTML& other)const
  { return this->fileName==other.fileName;
  }
  std::string ToStringCalculatorArgumentsForProblem(const std::string& requestType)const;
  std::string ToStringProblemNavigation()const;
  std::string ToStringExtractedCommands();
  std::string ToStringContent();
  std::string ToStringParsingStack(List<SyntacticElementHTML>& theStack);
  CalculatorHTML();
};

CalculatorHTML::CalculatorHTML()
{ this->randomSeed=0;
  this->flagRandomSeedGiven=false;
  this->NumAttemptsToInterpret=0;
  this->MaxInterpretationAttempts=10;
  this->flagLoadedSuccessfully=false;
  this->flagIsExamHome=false;
  this->flagIsExamIntermediate=false;
  this->flagIsExamProblem=false;
}

const std::string CalculatorHTML::RelativePhysicalFolderProblemCollections="ProblemCollections/";

bool CalculatorHTML::LoadMe(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::LoadMe");
  this->RelativePhysicalFileNameWithFolder=
  this->RelativePhysicalFolderProblemCollections+
  this->fileName
  ;
  std::fstream theFile;
  if (!FileOperations::OpenFileOnTopOfProjectBase(theFile, this->RelativePhysicalFileNameWithFolder, false, false, false))
  { comments << "<b>Failed to open file " << this->RelativePhysicalFileNameWithFolder << "</b>";
    return false;
  } else
  { std::stringstream contentStream;
    contentStream << theFile.rdbuf();
    this->inputHtml=contentStream.str();
    std::string randString= theGlobalVariables.GetWebInput("randomSeed");
    if (randString!="")
    { std::stringstream randSeedStream(randString);
      randSeedStream >> this->randomSeed;
      this->flagRandomSeedGiven=true;
    }
  }
  return true;
}

bool CalculatorHTML::FindExamItem()
{ MacroRegisterFunctionWithName("CalculatorHTML::FindExamItem");
  List<std::string> theExerciseFileNames, theExerciseFileNameExtensions;
  if (!FileOperations::GetFolderFileNamesOnTopOfProjectBase
      (this->RelativePhysicalFolderProblemCollections, theExerciseFileNames, &theExerciseFileNameExtensions))
  { this->comments << "Failed to open folder with relative file name: "
    << this->RelativePhysicalFolderProblemCollections;
    return false;
  }
  for (int i=0; i<theExerciseFileNames.size; i++)
  { if (theExerciseFileNameExtensions[i]!=".html")
      continue;
    this->fileName=theExerciseFileNames[i];
    return true;
  }
  return false;
}

std::string CalculatorHTML::LoadAndInterpretCurrentProblemItem()
{ MacroRegisterFunctionWithName("CalculatorHTML::GetCurrentExamItem");
  double startTime=theGlobalVariables.GetElapsedSeconds();
  this->LoadCurrentProblemItem();
  if (!this->flagLoadedSuccessfully)
    return this->comments.str();
  std::stringstream out;
  if (!this->InterpretHtml(this->comments))
  { out << "<b>Failed to interpret file: " << this->fileName << "</b>. Comments: " << this->comments.str();
    return out.str();
  }
  out << "Generated in "
  << MathRoutines::ReducePrecision(theGlobalVariables.GetElapsedSeconds()-startTime)
  << " second(s). " << this->outputHtml;
  return out.str();
}

void CalculatorHTML::LoadCurrentProblemItem()
{ MacroRegisterFunctionWithName("CalculatorHTML::LoadCurrentProblemItem");
  this->fileName= CGI::URLStringToNormal( theGlobalVariables.GetWebInput("currentExamFile"));
  this->flagLoadedSuccessfully=false;
  bool needToFindDefault=(this->fileName=="");
  if (!needToFindDefault)
    needToFindDefault=!this->LoadMe(this->comments);
  else
    this->comments << "<b>Selecting default course homework file.</b><br>";
  if (needToFindDefault)
  { if (!this->FindExamItem())
    { this->comments << "<b>No problems/exams to serve: found no html content in folder: "
      << this->RelativePhysicalFolderProblemCollections << ".</b>";
      return;
    }
    if (!this->LoadMe(this->comments))
      return;
    this->inputHtml=this->comments.str()+this->inputHtml;
  }
  this->flagLoadedSuccessfully=true;
}

bool CalculatorHTML::IsStateModifierApplyIfYes(SyntacticElementHTML& inputElt)
{ MacroRegisterFunctionWithName("CalculatorHTML::IsStateModifierApplyIfYes");
  if (inputElt.syntacticRole!="command")
    return false;
  std::string tagClass=inputElt.GetKeyValue("class");
  if (tagClass=="setCalculatorExamHome")
  { this->flagIsExamHome=true;
    this->flagIsExamProblem=false;
    this->flagIsExamIntermediate=false;
    theGlobalVariables.SetWebInput("currentExamHome", CGI::StringToURLString(this->fileName));
    theGlobalVariables.SetWebInput("currentExamIntermediate", "");
  }
  if (tagClass=="setCalculatorExamIntermediate")
  { this->flagIsExamHome=false;
    this->flagIsExamIntermediate=true;
    this->flagIsExamProblem=false;
    theGlobalVariables.SetWebInput("currentExamIntermediate", CGI::StringToURLString(this->fileName));
    return true;
  }
  if (tagClass=="setCalculatorExamProblem")
  { this->flagIsExamHome=false;
    this->flagIsExamIntermediate=false;
    this->flagIsExamProblem=true;
    return true;
  }
  return false;
}

bool CalculatorHTML::IsStudentAnswer(SyntacticElementHTML& inputElt)
{ if (inputElt.syntacticRole!="command")
    return false;
  std::string tagClass=inputElt.GetKeyValue("class");
  return tagClass=="calculatorStudentAnswerWithButton";
}

std::string CalculatorHTML::GetSubmitAnswersJavascript()
{ std::stringstream out;
  out
  << "<script type=\"text/javascript\"> \n"
  << "function submitAnswers(idAnswer, idVerification){\n"
  << "  spanVerification = document.getElementById(idVerification);\n"
  << "  if (spanVerification==null){\n"
  << "    spanVerification = document.createElement('span');\n"
  << "    document.body.appendChild(spanVerification);\n"
  << "    spanVerification.innerHTML= \"<span style='color:red'> ERROR: span with id \" + idVerification + \"MISSING! </span>\";\n"
  << "  }\n"
  << "  spanStudentAnswer = document.getElementById(idAnswer);\n"
  << "  var params=\"" << this->ToStringCalculatorArgumentsForProblem("submitProblem") << "\";\n"
  << "  params+=\"&calculatorStudentAnswerWithButton\" + idAnswer\n"
  << "          + \"=\"+encodeURIComponent(spanStudentAnswer.value);\n"
  << "  var https = new XMLHttpRequest();\n"
  << "  https.open(\"POST\", \"" << theGlobalVariables.DisplayNameCalculatorWithPath << "\", true);\n"
  << "  https.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\");\n"
;
//Old code, submits all answers. May need to be used as an alternative
//submission option.
//  for (int i=0; i<this->theContent.size; i++)
//    if (this->IsStudentAnswer(this->theContent[i]))
//      out << "  params+=\"&calculatorStudentAnswerWithButton" << this->theContent[i].GetKeyValue("id") << "=\"+encodeURIComponent("
//      << "document.getElementById('" << this->theContent[i].GetKeyValue("id") << "').value);\n";
  out
  << "  https.onload = function() {\n"
  << "    spanVerification.innerHTML=https.responseText;\n"
  << "  }\n"
  << "  https.send(params);\n"
  << "}\n"
  << "</script>";
  return out.str();
}

int WebWorker::ProcessSubmitProblem()
{ MacroRegisterFunctionWithName("WebWorker::ProcessSubmitProblem");
  CalculatorHTML theProblem;
  theProblem.LoadCurrentProblemItem();
  std::stringstream comments;
  if (!theProblem.flagLoadedSuccessfully)
  { stOutput << "Failed to load problem. " << theProblem.comments.str();
    stOutput.Flush();
    return 0;
  }
  if (!theProblem.ParseHTML(comments))
  { stOutput << "<b>Failed to interpret html input.</b><br>" << comments.str();
    return 0;
  }
  Calculator theInterpreter;
  if (!theProblem.PrepareCommands(theInterpreter, comments))
  { stOutput << "<b>Failed to prepare commands.</b>" << comments.str();
    return 0;
  }
  std::string problemStatement=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("problemStatement"));
  std::stringstream studentAnswerStream, completedProblemStream;
  completedProblemStream << theProblem.problemCommandsNoVerification;
  std::string studentAnswerNameReader;
  List<std::string> studentTagsAnswered;
  List<std::string> studentTagVerifications;
  List<std::string> studentAnswersUnadulterated;
  bool found=false;
  for (int i=0; i<theGlobalVariables.webFormArgumentNames.size; i++)
    if (MathRoutines::StringBeginsWith(theGlobalVariables.webFormArgumentNames[i], "calculatorStudentAnswerWithButton", &studentAnswerNameReader))
    { found=true;
      studentAnswerStream << studentAnswerNameReader << "= ("
      << CGI::URLStringToNormal( theGlobalVariables.webFormArguments[i]) << ");";
      studentAnswersUnadulterated.AddOnTop(CGI::URLStringToNormal( theGlobalVariables.webFormArguments[i]));
      studentTagsAnswered.AddOnTop(studentAnswerNameReader);
      studentTagVerifications.AddOnTop(SyntacticElementHTML::GetVerificationTag(studentAnswerNameReader));
      if (*studentAnswersUnadulterated.LastObject()=="")
      { stOutput << "<b>You appear to have submitted at least one empty answer. Please resubmit. </b>";
        return 0;
      }
    }
  if (!found)
  { stOutput << "<b>Something is wrong: I found no submitted answers.</b>";
    return 0;
  }
  completedProblemStream << studentAnswerStream.str();
  completedProblemStream << "SeparatorBetweenSpans; ";
  found=false;
  for (int i=0; i<theProblem.theContent.size; i++)
  { if (!theProblem.theContent[i].IsAnswerVerification())
      continue;
    if (!studentTagVerifications.Contains(theProblem.theContent[i].GetKeyValue("id")))
      continue;
    completedProblemStream << theProblem.CleanUpCommandString(theProblem.theContent[i].content);
    found=true;
  }
  if (!found)
  { stOutput << "<b>Something is wrong:</b> I found the answers:<br>"
    << studentAnswerStream.str()
    << " to the tags:<br>"
    << studentTagsAnswered.ToStringCommaDelimited()
    << " but I was not able to find the tags in the problem template.";
    return 0;
  }
//  stOutput << "input to the calculator: " << completedProblemStream.str() << "<hr>";
  theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=theGlobalVariables.GetElapsedSeconds()+20;
  theInterpreter.init();
  theInterpreter.Evaluate(completedProblemStream.str());
  if (theInterpreter.flagAbortComputationASAP || theInterpreter.syntaxErrors!="")
  { stOutput << "<b>Error while processing your answer(s).</b> Here's what I understood. ";
    for (int i=0; i<studentAnswersUnadulterated.size; i++)
    { Calculator isolatedInterpreter;
      isolatedInterpreter.init();
      theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=theGlobalVariables.GetElapsedSeconds()+20;
      isolatedInterpreter.Evaluate("("+studentAnswersUnadulterated[i]+")");
      if (isolatedInterpreter.syntaxErrors!="")
        stOutput << isolatedInterpreter.ToStringSyntacticStackHumanReadable(false);
      else
        stOutput << isolatedInterpreter.outputString;
    }
//    stOutput << "yer input: " << completedProblemStream.str();
//    stOutput << theInterpreter.outputString;
    return 0;
  }
  bool isCorrect=false;
  for (int i=theInterpreter.theProgramExpression.children.size-1; i>=0; i--)
  { if (theInterpreter.theProgramExpression[i].ToString()== "SeparatorBetweenSpans")
      break;
    int mustbeOne=-1;
    if (!theInterpreter.theProgramExpression[i].IsSmallInteger(&mustbeOne))
      isCorrect=false;
    else
      isCorrect=(mustbeOne==1);
    if (!isCorrect)
      break;
  }
  stOutput << "<table width=\"300\"><tr><td>";
  if (!isCorrect)
    stOutput << "<span style=\"color:red\"><b>Your answer appears to be incorrect.</b></span>";
  else
    stOutput << "<span style=\"color:green\"><b>Correct!</b></span>";
  stOutput << "</td></tr>";
  if (!isCorrect)
  {
  }

  stOutput << "<tr><td>Your answer was: " << studentAnswerStream.str() << "</td></tr>";
  if (!theGlobalVariables.flagLoggedIn)
  { stOutput << "<br><b>Submitting problem solutions allowed only for logged-in users. </b>";
    return 0;
  }

  stOutput << "</table>";
//  stOutput << "<hr>" << theInterpreter.outputString << "<hr><hr><hr><hr><hr><hr>";
//  stOutput << this->ToStringCalculatorArgumentsHumanReadable();
  //Calculator answerInterpretter;
  //answerInterpretter.theProgramExpression=theGlobalVariables.GetWebInput("mainInput");
  //answerInterpretter.init();
  return 0;
}

std::string WebWorker::GetExamPage()
{ MacroRegisterFunctionWithName("WebWorker::GetExamPage");
  CalculatorHTML theFile;
  std::stringstream out;
  out << "<html>"
  << "<header>"
  << WebWorker::GetJavascriptStandardCookies()
  << CGI::GetLaTeXProcessingJavascript()
  << "<link rel=\"stylesheet\" type=\"text/css\" href=\"/ProblemCollections/calculator.css\">"
  << "</header>"
  << "<body onload=\"loadSettings();\">\n";
  out << theFile.LoadAndInterpretCurrentProblemItem();
  out <<"<hr><hr><hr><hr><hr><hr><hr><hr>" << this->ToStringCalculatorArgumentsHumanReadable();
  out << "</body></html>";
  return out.str();
}

bool CalculatorFunctionsGeneral::innerInterpretHtml
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerInterpretHtml");
  CalculatorHTML theProblem;
  if (!input.IsOfType<std::string>(&theProblem.inputHtml))
    return theCommands << "Extracting calculator expressions from html takes as input strings. ";
  theProblem.InterpretHtml(theCommands.Comments);
  return output.AssignValue(theProblem.outputHtml, theCommands);
}

std::string CalculatorHTML::ToStringExtractedCommands()
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringExtractedCommands");
  std::stringstream out;
  out << "<hr><b>The commands extracted from the HTML follow.</b><br>";
  out << "<table>";
  for (int i=0; i<this->theContent.size; i++)
    if (this->theContent[i].syntacticRole!="")
      out << "<tr>" << "<td>" << this->theContent[i].ToStringDebug() << "</td>"
      << "</tr>";
    else
      out << "<tr><td></td></tr>";
  out << "</table>";
  out << "<hr><b>The HTML from which the commands were extracted follows.</b><br>" << this->inputHtml
  << "<hr><b>The parsing stack follows.</b>" << this->ToStringParsingStack(this->eltsStack) << "<hr>";
  return out.str();
}

std::string CalculatorHTML::ToStringContent()
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringContent");
  std::stringstream out;
//  out << "<hr><b>The split strings follow. </b><hr>" << splitStrings.ToStringCommaDelimited();
  out << "<hr><b>The extracted commands follow.</b><br>";
  for (int i=0; i<this->theContent.size; i++)
    out << this->theContent[i].ToStringTagAndContent();
  out << "<hr><b>The html read follows.</b><br>" << this->inputHtml << "<hr>";
  return out.str();
}

void SyntacticElementHTML::resetAllExceptContent()
{ this->tag="";
  this->tagKeys.SetSize(0);
  this->tagValues.SetSize(0);
  this->syntacticRole="";
}

std::string SyntacticElementHTML::ToStringOpenTag()
{ if (this->tag=="")
    return "";
  std::stringstream out;
  out << "<" << this->tag;
  for (int i=0; i<this->tagKeys.size; i++)
    out << " " << this->tagKeys[i] << "=\"" << this->tagValues[i] << "\"";
  out << ">";
  return out.str();
}

std::string SyntacticElementHTML::ToStringCloseTag()
{ if (this->tag=="")
    return "";
  return  "</" + this->tag + ">";
}

std::string SyntacticElementHTML::ToStringTagAndContent()
{ MacroRegisterFunctionWithName("SyntacticElementHTML::ToStringTagAndContent");
  if (this->syntacticRole=="")
    return this->content;
  return this->ToStringOpenTag() + this->content + this->ToStringCloseTag();
}

std::string SyntacticElementHTML::ToStringDebug()
{ MacroRegisterFunctionWithName("SyntacticElementHTML::ToString");
  if (this->syntacticRole=="")
    return CGI::StringToHtmlStrinG( this->ToStringTagAndContent());
  std::stringstream out;
  out << "<span style=\"color:green\">";
  out << CGI::StringToHtmlStrinG(this->syntacticRole);
  out << "</span>";
  out << "[" << CGI::StringToHtmlStrinG(this->ToStringTagAndContent() ) << "]";
  return out.str();
}

std::string SyntacticElementHTML::GetVerificationTag(const std::string& idTagToBeVerified)
{ return "verification_"+idTagToBeVerified;
}

std::string SyntacticElementHTML::GetKeyValue(const std::string& theKey)
{ MacroRegisterFunctionWithName("SyntacticElementHTML::GetKeyValue");
  int theIndex=this->tagKeys.GetIndex(theKey);
  if (theIndex==-1)
    return "";
  return this->tagValues[theIndex];
}

void SyntacticElementHTML::SetKeyValue(const std::string& theKey, const std::string& theValue)
{ MacroRegisterFunctionWithName("SyntacticElementHTML::SetKeyValue");
  if (this->tagKeys.size!=this->tagValues.size)
    crash << "Programming error: number of keys different from number of values" << crash;
  int theIndex=this->tagKeys.GetIndex(theKey);
  if (theIndex==-1)
  { theIndex=this->tagKeys.size;
    this->tagKeys.AddOnTop(theKey);
    this->tagValues.SetSize(this->tagKeys.size);
  }
  this->tagValues[theIndex]=theValue;
}

std::string SyntacticElementHTML::ToStringInterpretted()
{ if (this->syntacticRole=="")
    return this->content;
  std::string tagClass=this->GetKeyValue("class");
  if (tagClass=="calculatorAnswerVerification")
    return "";
  if (this->IsInterpretedNotByCalculator())
    return this->interpretedCommand;
  std::stringstream out;
  out << this->ToStringOpenTag();
  if (this->interpretedCommand!="")
    out << "\\( " << this->interpretedCommand << " \\)";
  out << this->ToStringCloseTag();
  return out.str();
}

bool SyntacticElementHTML::IsInterpretedNotByCalculator()
{ MacroRegisterFunctionWithName("SyntacticElementHTML::IsInterpretedNotByCalculator");
  if (this->syntacticRole!="command")
    return false;
  std::string tagClass=this->GetKeyValue("class");
  return tagClass=="calculatorExamProblem" || tagClass== "calculatorExamIntermediate"
  || tagClass=="calculatorStudentAnswerWithButton";
}

bool SyntacticElementHTML::IsInterpretedByCalculatorDuringPreparation()
{ if (this->syntacticRole!="command")
    return false;
  std::string tagClass=this->GetKeyValue("class");
  return tagClass=="calculator" || tagClass=="calculatorHidden";
}

bool SyntacticElementHTML::IsAnswerVerification()
{ if (this->syntacticRole!="command")
    return false;
  return this->GetKeyValue("class")=="calculatorAnswerVerification";
}

bool SyntacticElementHTML::IsStudentAnswer()
{ if (this->syntacticRole!="command")
    return false;
  return this->GetKeyValue("class")=="calculatorStudentAnswerWithButton";
}

bool CalculatorHTML::PrepareCommands(Calculator& theInterpreter, std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareCommands");
  if (this->theContent.size==0)
    crash << "Empty content not allowed. " << crash;
  if (this->theContent[0].syntacticRole!="")
    crash << "First command must be empty to allow for command for setting of random seed. " << crash;
  std::stringstream streamWithInbetween, streamNoVerification, streamVerification;
  //stOutput << " The big bad random seed: " << this->randomSeed ;
  streamWithInbetween << "setRandomSeed{}(" << this->randomSeed << ");";
  streamNoVerification << streamWithInbetween.str();
  for (int i=1; i<this->theContent.size; i++) // the first element of the content is fake (used for the random seed)
  { if (!this->theContent[i].IsInterpretedByCalculatorDuringPreparation())
    { streamWithInbetween << "SeparatorBetweenSpans; ";
      continue;
    }
    std::string cleanedupCommand=this->CleanUpCommandString( this->theContent[i].content);
    if (cleanedupCommand!="")
      streamWithInbetween << cleanedupCommand;
    else
      streamWithInbetween << "SeparatorBetweenSpans; ";
  }
  for (int i=1; i<this->theContent.size; i++)
  { if (this->theContent[i].syntacticRole!="command")
      continue;
    if (this->theContent[i].IsAnswerVerification())
    { streamVerification << this->CleanUpCommandString(this->theContent[i].content);
      continue;
    }
    if (this->theContent[i].IsInterpretedByCalculatorDuringPreparation())
    { streamNoVerification << this->CleanUpCommandString( this->theContent[i].content);
      continue;
    }
  }
  streamWithInbetween << "SeparatorBetweenSpans; " << streamVerification.str();
  this->problemCommandsWithInbetweens=streamWithInbetween.str();
  this->problemCommandsNoVerification=streamNoVerification.str();
  this->problemCommandsVerification=streamVerification.str();
  return true;
}

bool CalculatorHTML::PrepareAndExecuteCommands(Calculator& theInterpreter, std::stringstream& comments)
{ MacroRegisterFunctionWithName("Problem::ExecuteCommandsPrepare");
  this->PrepareCommands(theInterpreter, comments);
  theInterpreter.init();
  //stOutput << "nput cmds: " << calculatorCommands.str();
  theInterpreter.Evaluate(this->problemCommandsWithInbetweens);
//  stOutput << "<hr>Fter eval: " << theInterpreter.outputString;
  bool result=!theInterpreter.flagAbortComputationASAP && theInterpreter.syntaxErrors=="";
  if (!result)
    comments << "Failed to interpret your file. The interpretation input was:<br> "
    << this->problemCommandsWithInbetweens << "<br>"
    << "The result of the interpretation attempt is:<br>"
    << theInterpreter.outputString;
  return result;
}

std::string CalculatorHTML::ToStringProblemNavigation()const
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringProblemNavigation");
  if (this->flagIsExamHome)
    return "";
  std::stringstream out;
  std::string currentExamFile= theGlobalVariables.GetWebInput("currentExamFile");
  std::string currentExamHome= theGlobalVariables.GetWebInput("currentExamHome");
  std::string currentExamIntermediate=theGlobalVariables.GetWebInput("currentExamIntermediate");
  out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?request=exercises&"
  << WebWorker::ToStringCalcArgsExcludeRequestPasswordExamDetails()
  << "currentExamHome=&" << "currentExamIntermediate=&"
  << "currentExamFile=" << currentExamHome << "&\"> Course homework home. </a><br>";
  if (this->flagIsExamProblem && currentExamIntermediate!="")
    out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?request=exercises&"
    << WebWorker::ToStringCalcArgsExcludeRequestPasswordExamDetails()
    << "currentExamHome=" << currentExamHome << "&"
    << "currentExamIntermediate=&"
    << "currentExamFile=" << currentExamIntermediate << "&\"> Current homework. </a><br>";
  if (this->flagIsExamProblem && theGlobalVariables.userCalculatorRequestType!="examForReal")
    out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?"
    << this->ToStringCalculatorArgumentsForProblem("exercises")
    << "\">Link to this problem.</a>";
  return out.str();
}

std::string CalculatorHTML::ToStringCalculatorArgumentsForProblem(const std::string& requestType)const
{ MacroRegisterFunctionWithName("WebWorker::ToStringGetLink");
  if (!theGlobalVariables.flagLoggedIn)
    return "";
  std::stringstream out;
  out << "request=" << requestType << "&" << WebWorker::ToStringCalcArgsExcludeRequestPasswordExamDetails()
  << "currentExamHome=" << theGlobalVariables.GetWebInput("currentExamHome") << "&"
  << "currentExamIntermediate=" << theGlobalVariables.GetWebInput("currentExamIntermediate") << "&"
  << "currentExamFile=" << CGI::StringToURLString(this->fileName) << "&";
  if (theGlobalVariables.GetWebInput("randomSeed")=="")
    out << "randomSeed=" << this->randomSeed << "&";
//  out << "currentExamFile=" << CGI::StringToURLString(this->fileName) << "&";
  return out.str();
}

std::string SyntacticElementHTML::GetTagClass()
{ return this->GetKeyValue("class");
}

void CalculatorHTML::InterpretGenerateStudentAnswerButton(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretGenerateStudentAnswerButton");
  std::stringstream out;
  out << "<table><tr>";
  out << "<td>" << inputOutput.ToStringOpenTag() << inputOutput.ToStringCloseTag() << "</td>";
  std::string answerId = inputOutput.GetKeyValue("id");
  if (answerId=="")
    out << "<td><b>Error: could not generate submit button: the answer area does not have a valid id</b></td>";
  else
  { std::string answerEvaluationId=inputOutput.GetMyVerificationTag();
    out << "<td><button class=\"submitButton\" onclick=\"submitAnswers('"
    << answerId << "', '" << answerEvaluationId << "')\"> Submit </button></td>"
    << "<td>";
    out << "<span id=\"" << answerEvaluationId << "\"> <b><span style=\"color:brown\">No answer submitted.</span></b></span></td>";
  }
  out << "</tr></table>";

  inputOutput.interpretedCommand=out.str();
}

void CalculatorHTML::InterpretNotByCalculator(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretNotByCalculator");
  std::string tagClass=inputOutput.GetTagClass();
  if (tagClass=="calculatorExamProblem" || tagClass=="calculatorExamIntermediate")
    this->InterpretGenerateLink(inputOutput);
  else if (tagClass=="calculatorStudentAnswerWithButton")
    this->InterpretGenerateStudentAnswerButton(inputOutput);
}

void CalculatorHTML::InterpretGenerateLink(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretGenerateLink");
  std::string cleaneduplink;
  cleaneduplink.reserve(inputOutput.content.size());
  for (unsigned i=0; i<inputOutput.content.size(); i++)
    if (inputOutput.content[i]!='\n' && inputOutput.content[i]!='\r')
      cleaneduplink.push_back(inputOutput.content[i]);
  std::stringstream out, refStreamNoRequest, refStreamExercise, refStreamForReal;
//  out << "cleaned up link: " << cleaneduplink;
//  out << "<br>urled link: " <<  CGI::StringToURLString(cleaneduplink);
  refStreamNoRequest << WebWorker::ToStringCalcArgsExcludeRequestPasswordExamDetails()
  << "currentExamFile=" << CGI::StringToURLString(cleaneduplink) << "&";
  if (this->flagIsExamHome || this->flagIsExamIntermediate)
    refStreamNoRequest << "currentExamHome=" << theGlobalVariables.GetWebInput("currentExamHome")<< "&";
  if (this->flagIsExamIntermediate)
    refStreamNoRequest << "currentExamIntermediate=" << theGlobalVariables.GetWebInput("currentExamIntermediate") << "&";
  refStreamExercise << theGlobalVariables.DisplayNameCalculatorWithPath << "?request=exercises&" << refStreamNoRequest.str();
  refStreamForReal << theGlobalVariables.DisplayNameCalculatorWithPath << "?request=examForReal&" << refStreamNoRequest.str();
  out << inputOutput.content;
  if (inputOutput.GetTagClass()=="calculatorExamProblem")
  { out << " <a href=\"" << refStreamForReal.str() << "\">Start (for credit).</a>";
    out << " <a href=\"" << refStreamExercise.str() << "\">Exercise (no credit, unlimited tries).</a>";
  } else
    out << " <a href=\"" << refStreamExercise.str() << "\">Start.</a>";
  inputOutput.interpretedCommand=out.str();
}

bool CalculatorHTML::InterpretHtmlOneAttempt(Calculator& theInterpreter, std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretHtmlOneAttempt");
  std::stringstream out;
  if (!this->flagRandomSeedGiven)
    this->randomSeed=this->randomSeedsIfInterpretationFails[this->NumAttemptsToInterpret-1];
  std::string navigationLinks=this->ToStringProblemNavigation();
  out << this->GetSubmitAnswersJavascript();
  if (navigationLinks!="")
    out << "Links.<br>" << navigationLinks << "<br>";
  if (!this->PrepareAndExecuteCommands(theInterpreter, comments))
    return false;
  bool moreThanOneCommand=false;
  std::string lastCommands;
  int commandCounter=2;
  //first command and first syntactic element are the random seed and are ignored.
  for (int spanCounter=1; spanCounter <this->theContent.size; spanCounter++)
  { if (!this->theContent[spanCounter].IsInterpretedByCalculatorDuringPreparation())
    { if (theInterpreter.theProgramExpression[commandCounter].ToString()!="SeparatorBetweenSpans")
      { out << "<b>Error: calculator commands don't match the tags.</g>";
        this->outputHtml=out.str();
        return false;
      }
      commandCounter++;
      continue;
    }
    moreThanOneCommand=false;
    this->theContent[spanCounter].interpretedCommand="";
    for (; commandCounter<theInterpreter.theProgramExpression.children.size; commandCounter++ )
    { if (theInterpreter.theProgramExpression[commandCounter].ToString()=="SeparatorBetweenSpans")
        break;
      if (moreThanOneCommand)
        this->theContent[spanCounter].interpretedCommand+="; ";
      this->theContent[spanCounter].interpretedCommand+=theInterpreter.theProgramExpression[commandCounter].ToString();
      moreThanOneCommand=true;
    }
  }
  for (int i=0; i<this->theContent.size; i++)
    if (this->theContent[i].IsInterpretedNotByCalculator())
      this->InterpretNotByCalculator(this->theContent[i]);
  for (int i=0; i<this->theContent.size; i++)
    out << this->theContent[i].ToStringInterpretted();
//   out << "<hr><hr><hr><hr><hr><hr><hr><hr><hr>The calculator activity:<br>" << theInterpreter.outputString << "<hr>";
//   out << "<hr>" << this->ToStringExtractedCommands() << "<hr>";
  //  out << "<hr> Between the commands:" << this->betweenTheCommands.ToStringCommaDelimited();
  if (theGlobalVariables.flagLoggedIn)
    if (theGlobalVariables.userCalculatorRequestType=="examForReal")
    { //store the random seed.
//      stOutput << "Storing random seed...";
      this->GenerateDatabaseInfo();
      DatabaseRoutinesGlobalFunctions::CreateColumn(this->fileName, this->GetDatabaseTableName(), comments);
      DatabaseRoutinesGlobalFunctions::SetEntry
      (theGlobalVariables.userDefault, this->GetDatabaseTableName(), this->fileName, this->databaseInfo, comments);
      out << "<hr>Database comments: " << comments.str();
    }
  this->outputHtml=out.str();
  return true;
}

std::string CalculatorHTML::GetDatabaseTableName()
{ std::string result="grades"+ CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamHome"));
//  stOutput << "GetDatabaseTableName result: " << result << "<br>";
  return result;
}

bool CalculatorHTML::ParseAndInterpretDatabaseInfo(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::ParseAndInterpretDatabaseInfo");
  HashedList<std::string, MathRoutines::hashString> theKeys;
  List<std::string> theValues;
  CGI::ChopCGIInputStringToMultipleStrings(this->databaseInfo, theValues, theKeys, this->comments);
  if (theKeys.Contains("randomSeed"))
  { std::stringstream reader(theValues[theKeys.GetIndex("randomSeed")]);
    reader >> this->randomSeed;
    this->flagRandomSeedGiven=true;
  }
  return true;
}

void CalculatorHTML::GenerateDatabaseInfo()
{ MacroRegisterFunctionWithName("CalculatorHTML::GenerateDatabaseInfo");
  std::stringstream out;
  out << "randomSeed=" << this->randomSeed << "&";
  this->databaseInfo=out.str();
}

bool CalculatorHTML::InterpretHtml(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretHtml");
  if (!this->ParseHTML(comments))
  { this->outputHtml="<b>Failed to interpret html input.</b><br>" +this->ToStringContent();
    return false;
  }
  this->NumAttemptsToInterpret=0;
  if (theGlobalVariables.flagLoggedIn)
    if (theGlobalVariables.userCalculatorRequestType=="examForReal")
    { //load the random seed.
      if (!DatabaseRoutinesGlobalFunctions::TableExists(this->GetDatabaseTableName(),comments))
        if (!DatabaseRoutinesGlobalFunctions::CreateTable(this->GetDatabaseTableName(), comments))
        { this->outputHtml ="<b>Failed to create table for storing exam results. </b>" +comments.str();
          return false;
        }
      if (DatabaseRoutinesGlobalFunctions::FetchEntry
          (theGlobalVariables.userDefault, this->GetDatabaseTableName(), this->fileName, this->databaseInfo, comments))
        if (!this->ParseAndInterpretDatabaseInfo(comments))
        { this->outputHtml = "<b>Something wrong has happened: failed to parse the information stored in the database. Please let your instructor know.</b>";
          return false;
        }
    }
  if (this->flagRandomSeedGiven)
    this->MaxInterpretationAttempts=1;
  srand (time(NULL));
  this->randomSeedsIfInterpretationFails.SetSize(this->MaxInterpretationAttempts);
  for (int i=0; i<this->randomSeedsIfInterpretationFails.size; i++)
    this->randomSeedsIfInterpretationFails[i]=rand()%100000000;
  while (this->NumAttemptsToInterpret<this->MaxInterpretationAttempts)
  { Calculator theInterpreter;
    this->NumAttemptsToInterpret++;
    if (this->InterpretHtmlOneAttempt(theInterpreter, comments))
      return true;
    if (this->NumAttemptsToInterpret>=this->MaxInterpretationAttempts)
    { std::stringstream out;
      out << "Failed to evaluate the commands: " << this->NumAttemptsToInterpret
      << " attempts made. Calculator evaluation details follow.<hr> "
      << theInterpreter.outputString << "<hr><b>Comments</b><br>"
      << theInterpreter.Comments.str();
      this->outputHtml=out.str();
      return false;
    }
  }
  return true;
}

bool CalculatorHTML::IsSplittingChar(const std::string& input)
{ if (input.size()!=1)
    return false;
  return this->splittingChars.Contains(input[0]);
}

int SyntacticElementHTML::ParsingNumDummyElements=8;
std::string CalculatorHTML::ToStringParsingStack(List<SyntacticElementHTML>& theStack)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringParsingStack");
  std::stringstream out;
  out << "#Non-dummy elts: " << theStack.size-SyntacticElementHTML::ParsingNumDummyElements << ". ";
  for (int i=SyntacticElementHTML::ParsingNumDummyElements; i<theStack.size; i++)
  { out << "<span style=\"color:" << ((i%2==0) ? "orange":"blue") << "\">";
    std::string theContent=theStack[i].ToStringDebug();
    if (theContent.size()==0)
      theContent= "<b>empty</b>";
    else if (theContent==" ")
      theContent="_";
    out << theContent << "</span>";
  }
  return out.str();
}

bool CalculatorHTML::CanBeMerged(const SyntacticElementHTML& left, const SyntacticElementHTML& right)
{ MacroRegisterFunctionWithName("SyntacticElementHTML::CanBeMerged");
  if (left.syntacticRole!="" || right.syntacticRole!="")
    return false;
  if (this->IsSplittingChar(left.content) && left.content!=" ")
    return false;
  if (this->IsSplittingChar(right.content) && right.content!=" ")
    return false;
  return true;
}

bool CalculatorHTML::ParseHTML(std::stringstream& comments)
{ MacroRegisterFunctionWithName("Problem::ParseHTML");
  std::stringstream theReader(this->inputHtml);
  theReader.seekg(0);
  std::string word;
  char currentChar;
  List<SyntacticElementHTML> theElements;
  theElements.SetSize(0);
  theElements.SetExpectedSize(theReader.str().size()/4);
  this->splittingChars.AddOnTop('<');
  this->splittingChars.AddOnTop('\"');
  this->splittingChars.AddOnTop('>');
  this->splittingChars.AddOnTop('=');
  this->splittingChars.AddOnTop('/');
  this->splittingChars.AddOnTop(' ');
  while (theReader.get(currentChar))
  { if (splittingChars.Contains(currentChar))
    { if (word!="")
        theElements.AddOnTop(word);
      std::string charToString;
      charToString.push_back(currentChar);
      theElements.AddOnTop(charToString);
      word="";
    } else
      word.push_back(currentChar);
  }
  if (word!="")
    theElements.AddOnTop(word);
  this->calculatorClasses.AddOnTop("calculator");
  this->calculatorClasses.AddOnTop("calculatorHidden");
  this->calculatorClasses.AddOnTop("calculatorAnswerVerification");
  this->calculatorClasses.AddOnTop("calculatorStudentAnswerWithButton");
  this->calculatorClasses.AddOnTop("calculatorExamIntermediate");
  this->calculatorClasses.AddOnTop("calculatorExamProblem");
  this->calculatorClasses.AddOnTop("setCalculatorExamProblem");
  this->calculatorClasses.AddOnTop("setCalculatorExamIntermediate");
  this->calculatorClasses.AddOnTop("setCalculatorExamHome");
  this->eltsStack.SetSize(0);
  SyntacticElementHTML dummyElt;
  dummyElt.content="<>";
  dummyElt.syntacticRole="filler";
  eltsStack.SetExpectedSize(theElements.size+SyntacticElementHTML::ParsingNumDummyElements);
  for (int i=0; i<SyntacticElementHTML::ParsingNumDummyElements; i++)
    eltsStack.AddOnTop( dummyElt);
  int indexInElts=-1;
  bool reduced=false;
  this->flagIsExamProblem=true;
  this->flagIsExamHome=false;
  this->flagIsExamIntermediate=false;
  do
  { if (!reduced)
    { indexInElts++;
      if (indexInElts<theElements.size)
        eltsStack.AddOnTop(theElements[indexInElts]);
    }
//    stOutput << "<br>" << this->ToStringParsingStack(eltsStack);
    reduced=true;
    SyntacticElementHTML& last = eltsStack[eltsStack.size-1];
    SyntacticElementHTML& secondToLast = eltsStack[eltsStack.size-2];
    SyntacticElementHTML& thirdToLast = eltsStack[eltsStack.size-3];
    SyntacticElementHTML& fourthToLast = eltsStack[eltsStack.size-4];
    SyntacticElementHTML& fifthToLast = eltsStack[eltsStack.size-5];
    SyntacticElementHTML& sixthToLast = eltsStack[eltsStack.size-6];
//    SyntacticElementHTML& seventhToLast = eltsStack[eltsStack.size-7];
    if (secondToLast.syntacticRole=="<openTagCalc>" && last.syntacticRole=="</closeTag>" && secondToLast.tag==last.tag)
    { secondToLast.syntacticRole="command";
      eltsStack.RemoveLastObject();
      if (this->IsStateModifierApplyIfYes(secondToLast))
        eltsStack.RemoveLastObject();
      continue;
    }
    if (last.syntacticRole=="</closeTag>")
    { last.content=last.ToStringCloseTag();
      last.resetAllExceptContent();
      continue;
    }
    if (thirdToLast.syntacticRole=="<openTagCalc>" && secondToLast=="<" && last=="/")
    { secondToLast.syntacticRole="</";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (thirdToLast.syntacticRole=="<openTagCalc>" && secondToLast.syntacticRole=="")
    { thirdToLast.content+=secondToLast.content;
      secondToLast=last;
      eltsStack.SetSize(eltsStack.size-1);
      continue;
    }
    if (secondToLast.syntacticRole!="<openTagCalc>" && last=="<")
    { last.content="";
      last.syntacticRole="<";
      continue;
    }
    if (secondToLast.syntacticRole!="<openTagCalc>" && last==">")
    { last.content="";
      last.syntacticRole=">";
      continue;
    }
    if (secondToLast.syntacticRole=="<" && last!="/")
    { secondToLast.syntacticRole="<openTag";
      secondToLast.tag=last.content;
      secondToLast.content="";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole=="" && last=="/")
    { secondToLast.content+=last.content;
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole=="<" && last=="/")
    { secondToLast.syntacticRole="</";
      secondToLast.tag="";
      secondToLast.content="";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole=="</" )
    { secondToLast.syntacticRole="</closeTag";
      secondToLast.tag=last.content;
      secondToLast.content="";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.syntacticRole=="</closeTag" && last.syntacticRole==">")
    { secondToLast.syntacticRole="</closeTag>";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (last==" " &&
        (secondToLast.syntacticRole=="<openTag" || thirdToLast.syntacticRole=="<openTag"
         || fourthToLast.syntacticRole=="<openTag")
        )
    { eltsStack.RemoveLastObject();
      continue;
    }
    if (this->CanBeMerged(secondToLast, last))
    { secondToLast.content+=last.content;
      eltsStack.RemoveLastObject();
      continue;
    }
    if (sixthToLast.syntacticRole=="<openTag" && fourthToLast=="=" && thirdToLast=="\""
        && last=="\"" )
    { sixthToLast.SetKeyValue(fifthToLast.content, secondToLast.content);
      eltsStack.SetSize(eltsStack.size-5);
      continue;
    }
    if (secondToLast.syntacticRole=="<openTag" && last.syntacticRole==">")
    { //stOutput << secondToLast.ToStringDebug() << " class key value: " << secondToLast.GetKeyValue("class");
      if (this->calculatorClasses.Contains(secondToLast.GetKeyValue("class")))
        secondToLast.syntacticRole="<openTagCalc>";
      else
      { secondToLast.content=secondToLast.ToStringOpenTag();
        secondToLast.resetAllExceptContent();
      }
      eltsStack.RemoveLastObject();
      continue;
    }
    reduced=false;
  } while (reduced || indexInElts<theElements.size);
  this->theContent.SetSize(0);
  bool result=true;
  SyntacticElementHTML emptyElt;
  this->theContent.AddOnTop(emptyElt);//<-adding one fake element on top.
  for (int i=SyntacticElementHTML::ParsingNumDummyElements; i<eltsStack.size; i++)
  { bool needNewTag=false;
    if (i==SyntacticElementHTML::ParsingNumDummyElements)
      needNewTag=true;
    else if (this->theContent.LastObject()->syntacticRole!="")
      needNewTag=true;
    if (eltsStack[i].syntacticRole!="")
      needNewTag=true;
    if (eltsStack[i].syntacticRole!="command" && eltsStack[i].syntacticRole!="" )
    { result=false;
      comments << "<br>Syntactic element: " << eltsStack[i].ToStringDebug()
      << " is not a command but has non-empty syntactic role.";
    }
    if (!needNewTag)
      this->theContent.LastObject()->content+=eltsStack[i].content;
    else
    { if (this->theContent.size>0)
        if (this->theContent.LastObject()->IsInterpretedByCalculatorDuringPreparation() && eltsStack[i].IsInterpretedByCalculatorDuringPreparation())
        { SyntacticElementHTML emptyElt;
          this->theContent.AddOnTop(emptyElt);
        }
      this->theContent.AddOnTop(eltsStack[i]);
    }
  }
  if (result)
    result=this->CheckContent(comments);
  return result;
}

bool CalculatorHTML::CheckContent(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::CheckContent");
  bool result=true;
  for (int i=0; i<this->theContent.size; i++)
  { SyntacticElementHTML& currentElt=this->theContent[i];
    if (currentElt.syntacticRole=="command" && currentElt.IsStudentAnswer() &&
        currentElt.GetKeyValue("id").find('=')!=std::string::npos)
    { result=false;
      comments << "Error: the id of tag " << currentElt.ToStringDebug() << " contains the equality sign which is not allowed. ";
    }
  }
  return result;
}

std::string CalculatorHTML::CleanUpCommandString(const std::string& inputCommand)
{ MacroRegisterFunctionWithName("CalculatorHTML::CleanUpCommandString");
  if (inputCommand=="")
    return "";
//  stOutput << "Cleaning up: " << inputCommand;
  int realStart=0;
  int realEnd=inputCommand.size()-1;
  for (; realStart< (signed) inputCommand.size(); realStart++)
  { if (inputCommand[realStart]==' ' || inputCommand[realStart]=='\n')
      continue;
    if (inputCommand[realStart]=='\\')
      if (realStart+1<(signed) inputCommand.size())
        if (inputCommand[realStart+1]=='(')
        { realStart++;
          continue;
        }
    break;
  }
  for (;realEnd >=0; realEnd--)
  { if (inputCommand[realEnd]==' ' || inputCommand[realEnd]=='\n')
      continue;
    if (inputCommand[realEnd]==')')
      if (realEnd>0)
        if (inputCommand[realEnd-1]=='\\')
        { realEnd--;
          continue;
        }
    break;
  }
  if (realEnd<realStart)
    realEnd=realStart-1;
  std::string result=inputCommand.substr(realStart, realEnd-realStart+1);
  for (int i=(signed)result.size()-1; i>=0; i--)
  { if (result[i]==' ' || result[i]=='\n')
      continue;
    if (result[i]==';')
    { //stOutput << "to get: " << result;
      return result;
    }
    break;
  }
  if (result=="")
    return "";
  result.push_back(';');
  //stOutput << " to get: " << result << "<br>";
  return result;
}

bool CalculatorFunctionsGeneral::innerExtractCalculatorExpressionFromHtml
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerExtractCalculatorExpressionFromHtml");
  CalculatorHTML theFile;
  if (!input.IsOfType<std::string>(&theFile.inputHtml))
    return theCommands << "Extracting calculator expressions from html takes as input strings. ";
  if (!theFile.ParseHTML(theCommands.Comments))
    return false;
  return output.AssignValue(theFile.ToStringExtractedCommands(), theCommands);
}
