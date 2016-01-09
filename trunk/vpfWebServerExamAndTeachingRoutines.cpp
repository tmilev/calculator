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
  bool IsInterpretedDuringPreparation();
  bool IsAnswerVerification();
  bool IsStudentAnswer();
  std::string GetKeyValue(const std::string& theKey);
  void SetKeyValue(const std::string& theKey, const std::string& theValue);
  void resetAllExceptContent();
  std::string ToStringInterpretted();
  std::string ToStringTagAndContent();
  std::string ToStringOpenTag();
  std::string ToStringCloseTag();
  std::string ToStringDebug();
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
  int randomSeed;
  bool flagRandomSeedGiven;
  std::string problemCommandsWithInbetweens;
  std::string problemCommandsNoVerification;
  std::string problemCommandsVerification;


  std::string fileName;
  std::string RelativePhysicalFileNameWithFolder;
  std::string inputHtml;
  std::string outputHtml;
  List<std::string> calculatorClasses;
  List<char> splittingChars;
  List<SyntacticElementHTML> theContent;
  bool LoadMe(std::stringstream& comments);
  bool ParseHTML(std::stringstream& comments);
  bool IsSplittingChar(const std::string& input);
  bool IsStudentAnswer(SyntacticElementHTML& inputElt);
  bool InterpretHtml(std::stringstream& comments);
  bool PrepareAndExecuteCommands(Calculator& theInterpreter, std::stringstream& comments);
  bool PrepareCommands(Calculator& theInterpreter, std::stringstream& comments);
  std::string CleanUpCommandString(const std::string& inputCommand);

  static unsigned int HashFunction(const CalculatorHTML& input)
  { return input.HashFunction();
  }
  unsigned int HashFunction()const
  { return MathRoutines::hashString(this->fileName);
  }
  bool operator==(const CalculatorHTML& other)const
  { return this->fileName==other.fileName;
  }
  std::string GetSubmitAnswersJavascript();
  std::string ToStringCalculatorArgumentsForProblem(const std::string& requestType)const;
  std::string ToStringGetProblemLink()const;
  std::string ToStringExam();
  std::string ToStringExtractedCommands();
  std::string ToStringContent();
  CalculatorHTML();
};

class Problem
{
public:
  bool ExecuteCommands(Calculator& theInterpreter, std::stringstream& comments);
  bool ExecuteCommandsTestStudent(Calculator& theInterpreter, std::stringstream& comments);
  std::string ToStringParsingStack(List<SyntacticElementHTML>& theStack);

  //Problem();
};

CalculatorHTML::CalculatorHTML()
{ this->randomSeed=0;
  this->flagRandomSeedGiven=false;
}

class TeachingRoutines
{
public:
  CalculatorHTML currentExamFile;
  bool flagLoadedSuccessfully;
  static const std::string RelativePhysicalFolderProblemCollections;
  std::stringstream comments;
  std::string GetSubmitAnswersJavascript();
  void LoadCurrentProblemItem();
  std::string LoadAndInterpretCurrentProblemItem();
  bool FindExamItem();

  TeachingRoutines();
};

const std::string TeachingRoutines::RelativePhysicalFolderProblemCollections="ProblemCollections/";

TeachingRoutines::TeachingRoutines()
{ this->flagLoadedSuccessfully=false;
}

std::string CalculatorHTML::ToStringExam()
{ MacroRegisterFunctionWithName("ProblemCollection::CalculatorHTML");
  std::stringstream out;
  std::stringstream failure;
/*  this->InterpretHtml(failure);

  out << this->GetSubmitAnswersJavascript();
//  out << "Latex javascript off!!!";
  out << CGI::GetLaTeXProcessingJavascript();
  out << "\n<form class=\"problemForm\" method=\"GET\" id=\"formProblemCollection\" name=\"formProblemCollection\" action=\""
  << theGlobalVariables.DisplayNameCalculatorWithPath << "\">\n" ;
  out << theWebServer.GetActiveWorker().GetHtmlHiddenInputs();
  out << theWebServer.GetActiveWorker().GetHtmlHiddenInputExercise();
  out << "\n</form>\n";*/
crash << "not implemented" <<crash;
//  out << this->outputHtml;
  return out.str();
}

bool CalculatorHTML::LoadMe(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::LoadMe");
  this->RelativePhysicalFileNameWithFolder=
  TeachingRoutines::RelativePhysicalFolderProblemCollections+
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

bool TeachingRoutines::FindExamItem()
{ MacroRegisterFunctionWithName("TeachingRoutines::FindExamItem");
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
    this->currentExamFile.fileName=theExerciseFileNames[i];
    return true;
  }
  return false;
}

std::string TeachingRoutines::LoadAndInterpretCurrentProblemItem()
{ MacroRegisterFunctionWithName("TeachingRoutines::GetCurrentExamItem");
  this->LoadCurrentProblemItem();
  if (!this->flagLoadedSuccessfully)
    return this->comments.str();
  std::stringstream out;
  if (!this->currentExamFile.InterpretHtml(this->comments))
  { out << "<b>Failed to interpret file: " << this->currentExamFile.fileName << "</b>. Comments: " << this->comments.str();
    return out.str();
  }
  return this->currentExamFile.outputHtml;
}

void TeachingRoutines::LoadCurrentProblemItem()
{ MacroRegisterFunctionWithName("TeachingRoutines::LoadCurrentProblemItem");
  this->currentExamFile.fileName= theGlobalVariables.GetWebInput("currentExamFile");
  this->flagLoadedSuccessfully=false;
  if (this->currentExamFile.fileName=="")
    if (!this->FindExamItem())
    { this->comments << "<b>No problems/exams to serve: found no html content in folder: "
      << this->RelativePhysicalFolderProblemCollections << ".</b>";
      return;
    }
  if (!this->currentExamFile.LoadMe(this->comments))
  { this->comments << "<b>Failed to load exam file: " << this->currentExamFile.fileName << ".</b>";
    return;
  }
  this->flagLoadedSuccessfully=true;
}

bool CalculatorHTML::IsStudentAnswer(SyntacticElementHTML& inputElt)
{ if (inputElt.syntacticRole!="command")
    return false;
  std::string tagClass=inputElt.GetKeyValue("class");
  return tagClass=="calculatorStudentAnswer";
}

std::string CalculatorHTML::GetSubmitAnswersJavascript()
{ std::stringstream out;
  out
  << "<script type=\"text/javascript\"> \n"
  << "function submitAnswers(){\n"
  << "  var https = new XMLHttpRequest();\n"
  << "  https.open(\"POST\", \"" << theGlobalVariables.DisplayNameCalculatorWithPath << "\", true);\n"
  << "  https.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\");\n";
  out
  << "  var params=\"" << this->ToStringCalculatorArgumentsForProblem("submitProblem") << "\";\n";
  for (int i=0; i<this->theContent.size; i++)
    if (this->IsStudentAnswer(this->theContent[i]))
      out << "  params+=\"&calculatorStudentAnswer" << this->theContent[i].GetKeyValue("id") << "=\"+encodeURIComponent("
      << "document.getElementById('" << this->theContent[i].GetKeyValue("id") << "').value);\n";
  out
  << "  https.send(params);\n"
  << "  https.onload = function() {\n"
  << "  span = document.getElementById('calculatorEvaluation');\n"
  << "  if (span==null){\n"
  << "    span = document.createElement('span');\n"
  << "    document.body.appendChild(span);\n"
  << "  }\n"
  << "  span.innerHTML=https.responseText;\n"
  << "}\n"
  << "}\n"
  << "</script>";
  return out.str();
}

int WebWorker::ProcessSubmitProblem()
{ MacroRegisterFunctionWithName("WebWorker::ProcessSubmitProblem");
  TeachingRoutines theRoutines;
  theRoutines.LoadCurrentProblemItem();
  std::stringstream comments;
  if (!theRoutines.flagLoadedSuccessfully)
  { stOutput << "Failed to load problem. " << theRoutines.comments.str();
    stOutput.Flush();
    return 0;
  }
  if (!theRoutines.currentExamFile.ParseHTML(comments))
  { stOutput << "<b>Failed to interpret html input.</b><br>" << comments.str();
    return 0;
  }
  Calculator theInterpreter;
  if (!theRoutines.currentExamFile.PrepareCommands(theInterpreter, comments))
  { stOutput << "<b>Failed to prepare commands.</b>" << comments.str();
    return 0;
  }
  std::string problemStatement=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("problemStatement"));
  std::stringstream studentAnswerStream, completedProblemStream;
  completedProblemStream << theRoutines.currentExamFile.problemCommandsNoVerification;
  std::string studentAnswerNameReader;
  List<std::string> studentAnswersUnadulterated;
  for (int i=0; i<theGlobalVariables.webFormArgumentNames.size; i++)
    if (MathRoutines::StringBeginsWith(theGlobalVariables.webFormArgumentNames[i], "calculatorStudentAnswer", &studentAnswerNameReader))
    { studentAnswerStream << studentAnswerNameReader << "= ("
      << CGI::URLStringToNormal( theGlobalVariables.webFormArguments[i]) << ");";
      studentAnswersUnadulterated.AddOnTop(CGI::URLStringToNormal( theGlobalVariables.webFormArguments[i]));
      if (*studentAnswersUnadulterated.LastObject()=="")
      { stOutput << "<b>You appear to have submitted at least one empty answer. Please resubmit. </b>";
        return 0;
      }
    }
  completedProblemStream << studentAnswerStream.str();
  completedProblemStream << "SeparatorBetweenSpans; ";
  completedProblemStream << theRoutines.currentExamFile.problemCommandsVerification;
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
  if (!isCorrect)
    stOutput << "<span style=\"color:red\"><b>Your answer appears to be incorrect.</b></span>";
  else
    stOutput << "<span style=\"color:green\"><b>Correct!</b></span>";
  if (!theGlobalVariables.flagLoggedIn)
  { stOutput << "<br><b>Submitting problem solutions allowed only for logged-in users. </b>";
    return 0;
  }
//  stOutput << "<hr>" << theInterpreter.outputString << "<hr><hr><hr><hr><hr><hr>";
//  stOutput << this->ToStringCalculatorArgumentsHumanReadable();
  //Calculator answerInterpretter;
  //answerInterpretter.theProgramExpression=theGlobalVariables.GetWebInput("mainInput");
  //answerInterpretter.init();
  return 0;
}

std::string WebWorker::GetExamPage()
{ MacroRegisterFunctionWithName("WebWorker::GetExamPage");
  TeachingRoutines theRoutines;
  std::stringstream out;
  out << "<html>"
  << "<header>"
  << WebWorker::GetJavascriptStandardCookies()
  << "<link rel=\"stylesheet\" type=\"text/css\" href=\"/ProblemCollections/calculator.css\">"
  << "</header>"
  << "<body onload=\"loadSettings();\">\n";
  out << theRoutines.LoadAndInterpretCurrentProblemItem();
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
  << "<hr>";
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
  if (this->GetKeyValue("class")=="calculatorAnswerVerification")
    return "";
  std::stringstream out;
  out << this->ToStringOpenTag();
  if (this->interpretedCommand!="")
    out << "\\( " << this->interpretedCommand << " \\)";
  out << this->ToStringCloseTag();
  return out.str();
}

bool SyntacticElementHTML::IsInterpretedDuringPreparation()
{ return this->syntacticRole=="command" &&
         this->GetKeyValue("class")!="calculatorAnswerVerification" &&
         this->GetKeyValue("class")!="calculatorStudentAnswer";
}

bool SyntacticElementHTML::IsAnswerVerification()
{ return this->syntacticRole=="command" &&
         this->GetKeyValue("class")=="calculatorAnswerVerification";
}

bool SyntacticElementHTML::IsStudentAnswer()
{ return this->syntacticRole=="command" &&
         this->GetKeyValue("class")=="calculatorStudentAnswer";
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
  for (int i=1; i<this->theContent.size; i++)
  { if (!this->theContent[i].IsInterpretedDuringPreparation())
    { streamWithInbetween << "SeparatorBetweenSpans; ";
      continue;
    }
    streamWithInbetween << this->CleanUpCommandString( this->theContent[i].content);
  }
  for (int i=1; i<this->theContent.size; i++)
  { if (this->theContent[i].syntacticRole!="command")
      continue;
    if (this->theContent[i].IsAnswerVerification())
    { streamVerification << this->CleanUpCommandString(this->theContent[i].content);
      continue;
    }
    streamNoVerification << this->CleanUpCommandString( this->theContent[i].content);
  }
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
  return !theInterpreter.flagAbortComputationASAP;
}

std::string CalculatorHTML::ToStringGetProblemLink()const
{ std::stringstream out;
  out << "<a href=\""
  << theGlobalVariables.DisplayNameCalculatorWithPath << "?" << this->ToStringCalculatorArgumentsForProblem("exercises")
  << "\">Link to this problem.</a>";
  return out.str();
}

std::string CalculatorHTML::ToStringCalculatorArgumentsForProblem(const std::string& requestType)const
{ MacroRegisterFunctionWithName("WebWorker::ToStringGetLink");
  if (!theGlobalVariables.flagLoggedIn)
    return "";
  std::stringstream out;
   out << "request=" << requestType << "&" << WebWorker::ToStringCalculatorArgumentsCGIinputExcludeRequestTypeAndPassword();
  if (theGlobalVariables.GetWebInput("randomSeed")=="")
    out << "randomSeed=" << this->randomSeed << "&";
  return out.str();
}

bool CalculatorHTML::InterpretHtml(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretHtml");

  if (!this->ParseHTML(comments))
  { this->outputHtml="<b>Failed to interpret html input.</b><br>" +this->ToStringContent();
    return false;
  }
  int numAttempts=0;
  int MaxNumAttempts=10;
  if (this->flagRandomSeedGiven)
    MaxNumAttempts=1;
  srand (time(NULL));
  List<int> theRandomSeeds;
  theRandomSeeds.SetSize(MaxNumAttempts);
  for (int i=0; i<theRandomSeeds.size; i++)
    theRandomSeeds[i]=rand()%100000000;
  while (numAttempts<MaxNumAttempts)
  { numAttempts++;
    Calculator theInterpreter;
    std::stringstream out;
    if (!this->flagRandomSeedGiven)
      this->randomSeed=theRandomSeeds[numAttempts-1];
    out << "Link to the problem: " << this->ToStringGetProblemLink() << "<br>";
    if (!this->PrepareAndExecuteCommands(theInterpreter, comments))
    { if (numAttempts>=MaxNumAttempts)
      { out << "Failed to evaluate the commands: " << numAttempts
        << " attempts made. Calculator evaluation details follow.<hr> "
        << theInterpreter.outputString << "<hr><b>Comments</b><br>"
        << theInterpreter.Comments.str();
        this->outputHtml=out.str();
        return false;
      }
      continue;
    }
    bool moreThanOneCommand=false;
    std::string lastCommands;
    int commandCounter=2;
    //first command and first syntactic element are the random seed and are ignored.
    for (int spanCounter=1; spanCounter <this->theContent.size; spanCounter++)
    { if (!this->theContent[spanCounter].IsInterpretedDuringPreparation())
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
      out << this->theContent[i].ToStringInterpretted();

//    out << "<hr><hr><hr><hr><hr><hr><hr><hr><hr>The calculator activity:<br>" << theInterpreter.outputString << "<hr>";
//    out << "<hr>" << this->ToStringExtractedCommands() << "<hr>";
  //  out << "<hr> Between the commands:" << this->betweenTheCommands.ToStringCommaDelimited();
    this->outputHtml=out.str();
    break;
  }
  return true;
}

bool CalculatorHTML::IsSplittingChar(const std::string& input)
{ if (input.size()!=1)
    return false;
  return this->splittingChars.Contains(input[0]);
}

int SyntacticElementHTML::ParsingNumDummyElements=8;
std::string Problem::ToStringParsingStack(List<SyntacticElementHTML>& theStack)
{ MacroRegisterFunctionWithName("Problem::ToStringParsingStack");
  std::stringstream out;
  for (int i=SyntacticElementHTML::ParsingNumDummyElements; i<theStack.size; i++)
    out << theStack[i].ToStringDebug();
  return out.str();
}

bool CalculatorHTML::ParseHTML(std::stringstream& comments)
{ MacroRegisterFunctionWithName("Problem::ParseHTML");
  std::stringstream theReader(this->inputHtml);
  theReader.seekg(0);
  std::string word, readChars;
  List<SyntacticElementHTML> theElements;
  theElements.SetSize(0);
  theElements.SetExpectedSize(theReader.str().size()/4);
  this->splittingChars.AddOnTop('<');
  this->splittingChars.AddOnTop('\"');
  this->splittingChars.AddOnTop('>');
  this->splittingChars.AddOnTop('=');
  this->splittingChars.AddOnTop('/');
  while (theReader >> readChars)
  { word="";
    for (unsigned i=0; i< readChars.size(); i++)
      if (splittingChars.Contains(readChars[i]))
      { if (word!="")
          theElements.AddOnTop(word);
        std::string charToString;
        charToString.push_back(readChars[i]);
        theElements.AddOnTop(charToString);
        word="";
      } else
        word.push_back(readChars[i]);
    if (word!="")
      theElements.AddOnTop(word);
  }
  this->calculatorClasses.AddOnTop("calculator");
  this->calculatorClasses.AddOnTop("calculatorHidden");
  this->calculatorClasses.AddOnTop("calculatorAnswerVerification");
  this->calculatorClasses.AddOnTop("calculatorStudentAnswer");
  List<SyntacticElementHTML> eltsStack;
  SyntacticElementHTML dummyElt;
  dummyElt.content="<>";
  dummyElt.syntacticRole="filler";
  eltsStack.SetExpectedSize(theElements.size+SyntacticElementHTML::ParsingNumDummyElements);
  for (int i=0; i<SyntacticElementHTML::ParsingNumDummyElements; i++)
    eltsStack.AddOnTop( dummyElt);
  int indexInElts=-1;
  bool reduced=false;
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
      if (!this->IsSplittingChar(secondToLast.content))
        thirdToLast.content+=" ";
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
    if (! this->IsSplittingChar(last.content) && last.syntacticRole=="" && !this->IsSplittingChar(secondToLast.content) &&
        secondToLast.syntacticRole=="")
    { secondToLast.content+=" "+last.content;
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
      result=false;
    if (!needNewTag)
    { this->theContent.LastObject()->content+=eltsStack[i].content;
      if (!this->IsSplittingChar(eltsStack[i].content))
        this->theContent.LastObject()->content+=" ";
    } else
    { if (this->theContent.size>0)
        if (this->theContent.LastObject()->IsInterpretedDuringPreparation() && eltsStack[i].IsInterpretedDuringPreparation())
        { SyntacticElementHTML emptyElt;
          this->theContent.AddOnTop(emptyElt);
        }
      this->theContent.AddOnTop(eltsStack[i]);
    }
  }
  return result;
}

std::string CalculatorHTML::CleanUpCommandString(const std::string& inputCommand)
{ MacroRegisterFunctionWithName("CalculatorHTML::CleanUpCommandString");
  if (inputCommand=="")
    return "";
  int realStart=0;
  int realEnd=inputCommand.size()-1;
  for (; realStart< (signed) inputCommand.size(); realStart++)
  { if (inputCommand[realStart]==' ')
      continue;
    if (inputCommand[realStart]=='\\')
      if (realStart+1<(signed) inputCommand.size())
        if (inputCommand[realStart+1]=='(')
          realStart+=2;
    break;
  }
  for (;realEnd >=0; realEnd--)
  { if (inputCommand[realEnd]==' ')
      continue;
    if (inputCommand[realEnd]==')')
      if (realEnd>0)
        if (inputCommand[realEnd-1]=='\\')
          realEnd-=2;
    break;
  }
  if (realEnd<realStart)
    realEnd=realStart-1;
  std::string result=inputCommand.substr(realStart, realEnd-realStart+1);
  for (int i=(signed)result.size()-1; i>=0; i--)
  { if (result[i]==' ')
      continue;
    if (result[i]==';')
      return result;
    break;
  }
  result.push_back(';');
  return result;
}

bool CalculatorFunctionsGeneral::innerExtractCalculatorExpressionFromHtml
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerExtractCalculatorExpressionFromHtml");
  TeachingRoutines theRoutines;
  if (!input.IsOfType<std::string>(&theRoutines.currentExamFile.inputHtml))
    return theCommands << "Extracting calculator expressions from html takes as input strings. ";
  if (!theRoutines.currentExamFile.ParseHTML(theCommands.Comments))
    return false;

  return output.AssignValue(theRoutines.currentExamFile.ToStringExtractedCommands(), theCommands);
}
