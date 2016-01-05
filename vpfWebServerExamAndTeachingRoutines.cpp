//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader6WebServer.h"
#include "vpfHeader3Calculator2_InnerFunctions.h"
#include "vpfHeader7DatabaseInterface_MySQL.h"

ProjectInformationInstance projectInfoInstanceWebServerExamAndTeachingRoutines
(__FILE__, "Routines for calculus teaching: calculator exam mode.");

class Problem
{
public:
  std::string fileName;
  std::string RelativePhysicalFileNameWithFolder;
  std::string inputHtml;
  std::string outputHtml;
  List<std::string> betweenTheCommands;
  List<std::string> commands;
  List<std::string> commandTypes;
  List<std::string> tagTypes;
  List<std::string> calculatorClasses;
  std::string finalCommandsPoseProblem;
  std::string finalCommandsPoseAndAnswerProblem;
  List<char> splittingChars;
  static unsigned int HashFunction(const Problem& input)
  { return input.HashFunction();
  }
  unsigned int HashFunction()const
  { return MathRoutines::hashString(this->fileName);
  }
  bool operator==(const Problem& other)const
  { return this->fileName==other.fileName;
  }
  bool LoadMe(std::stringstream& comments);
  bool ExecuteCommands(Calculator& theInterpretter, std::stringstream& comments);
  bool InterpretHtml(std::stringstream& comments);
  bool ExtractExpressionsFromHtml(std::stringstream& comments);
  std::string CleanUpCommandString(const std::string& inputCommand);
  std::string ToStringExam();
  std::string GetSubmitAnswersJavascript();
  std::string ToStringExtractedCommands();
  Problem();
};

Problem::Problem()
{ this->calculatorClasses.AddOnTop("calculator");
  this->calculatorClasses.AddOnTop("calculatorHidden");
  this->calculatorClasses.AddOnTop("calculatorAnswerVerification");
  this->calculatorClasses.AddOnTop("calculatorStudentAnswer");
  this->splittingChars.AddOnTop('<');
  this->splittingChars.AddOnTop('\"');
  this->splittingChars.AddOnTop('>');
  this->splittingChars.AddOnTop('=');
}

class ProblemCollection
{
public:
  std::string fileName;
  std::string RelativePhysicalFileNameWithFolder;
  HashedList<Problem> theProblems;

  static unsigned int HashFunction(const ProblemCollection& input)
  { return input.HashFunction();
  }
  unsigned int HashFunction()const
  { return MathRoutines::hashString(this->fileName);
  }
  bool operator==(const ProblemCollection& other)const
  { return this->fileName==other.fileName;
  }
  bool HasProblem(const std::string& desiredProblem);
  int GetProblemIndex(const std::string& desiredProblem);
  bool LoadMe(std::stringstream& comments);
  std::string ToStringProblemLinks();
  std::string ToStringSelectMeForm()const;
};

class TeachingRoutines
{
public:
  static const std::string RelativePhysicalFolderProblemCollections;
  static const std::string RelativePhysicalFolderProblems;
  HashedList<ProblemCollection> theExercises;
  bool HasExercise(const std::string& desiredExercise);
  int GetExerciseIndex(const std::string& desiredExercise);
  std::stringstream comments;
  bool LoadExercises();
  std::string GetSubmitAnswersJavascript();
  std::string GetCurrentProblemItem();
  std::string ToStringExerciseSelection();
  TeachingRoutines();
};

const std::string TeachingRoutines::RelativePhysicalFolderProblemCollections="ProblemCollections/";
const std::string TeachingRoutines::RelativePhysicalFolderProblems="Problems/";

TeachingRoutines::TeachingRoutines()
{
}

bool TeachingRoutines::LoadExercises()
{ MacroRegisterFunctionWithName("TeachingRoutines::LoadExercises");
  List<std::string> theExerciseFileNames, theExerciseFileNameExtensions;
  if (!FileOperations::GetFolderFileNamesOnTopOfProjectBase
      (this->RelativePhysicalFolderProblemCollections, theExerciseFileNames, &theExerciseFileNameExtensions))
  { this->comments << "Failed to open folder with relative file name: "
    << this->RelativePhysicalFolderProblemCollections;
    return false;
  }
  this->theExercises.SetExpectedSize(theExerciseFileNames.size);
  this->theExercises.Clear();
  ProblemCollection newExercise;
  for (int i=0; i<theExerciseFileNames.size; i++)
  { if (theExerciseFileNameExtensions[i]!=".txt")
      continue;
    newExercise.fileName=theExerciseFileNames[i];
    newExercise.RelativePhysicalFileNameWithFolder=
    this->RelativePhysicalFolderProblemCollections+newExercise.fileName;
    this->theExercises.AddOnTop(newExercise);
  }
//  stOutput << "ExerciseFileNames: " << theExerciseFileNames.ToStringCommaDelimited() << " with extensions: "
//  << theExerciseFileNameExtensions.ToStringCommaDelimited();
  return true;
}

std::string ProblemCollection::ToStringSelectMeForm()const
{ std::stringstream out;
  out << this->fileName
  << "<button type=\"submit\" name=\"currentProblemCollection\" value=\""
  << this->fileName
  << "\"> Start</button>";
  return out.str();
}

std::string Problem::ToStringExam()
{ MacroRegisterFunctionWithName("ProblemCollection::ToStringExam");
  std::stringstream out;
  std::stringstream failure;
  this->InterpretHtml(failure);

  out << this->GetSubmitAnswersJavascript();
  out << CGI::GetLaTeXProcessingJavascript();
  out << "\n<form class=\"problemForm\" method=\"POST\" id=\"formProblemCollection\" name=\"formProblemCollection\" action=\""
  << theGlobalVariables.DisplayNameCalculatorWithPath << "\">\n" ;
  out << theWebServer.GetActiveWorker().GetHtmlHiddenInputs();
  out << "\n</form>\n";
  out << this->outputHtml;
  return out.str();
}

std::string ProblemCollection::ToStringProblemLinks()
{ MacroRegisterFunctionWithName("ProblemCollection::ToStringProblemLinks");
  std::stringstream out;
  out << this->theProblems.size << " problem(s).";
  out << "\n<form method=\"POST\" id=\"formProblemCollection\" name=\"formProblemCollection\" action=\""
  << theGlobalVariables.DisplayNameCalculatorWithPath << "\">\n" ;
  out << theWebServer.GetActiveWorker().GetHtmlHiddenInputs();
  for (int i=0; i<this->theProblems.size; i++)
  { out << this->theProblems[i].fileName
    << "<button type=\"submit\" name=\"currentProblem\" title=\"Start\" value=\""
    << this->theProblems[i].fileName
    << "\"> Start</button>";
    out << "<br>";
  }
  out << "\n</form>\n";
  return out.str();
}

std::string TeachingRoutines::ToStringExerciseSelection()
{ MacroRegisterFunctionWithName("TeachingRoutines::ToStringExerciseSelection");
  std::stringstream out;
  out << this->theExercises.size << " homework(s).";
  out << "\n<FORM method=\"POST\" id=\"formExercises\" name=\"formExercises\" action=\""
  << theGlobalVariables.DisplayNameCalculatorWithPath << "\">\n" ;
  out << theWebServer.GetActiveWorker().GetHtmlHiddenInputs();
  for (int i=0; i<this->theExercises.size; i++)
    out << this->theExercises[i].ToStringSelectMeForm() << "<br>";
  out << "\n</FORM>\n";
  return out.str();
}

bool TeachingRoutines::HasExercise(const std::string& desiredExercise)
{ return this->GetExerciseIndex(desiredExercise)!=-1;
}

int TeachingRoutines::GetExerciseIndex(const std::string& desiredExercise)
{ ProblemCollection tempExercise;
  tempExercise.fileName=desiredExercise;
  return this->theExercises.GetIndex(tempExercise);
}

bool ProblemCollection::HasProblem(const std::string& desiredExercise)
{ return this->GetProblemIndex(desiredExercise)!=-1;
}

int ProblemCollection::GetProblemIndex(const std::string& desiredExercise)
{ Problem tempExercise;
  tempExercise.fileName=desiredExercise;
  return this->theProblems.GetIndex(tempExercise);
}

bool Problem::LoadMe(std::stringstream& comments)
{ MacroRegisterFunctionWithName("Problem::LoadMe");
  this->RelativePhysicalFileNameWithFolder=
  TeachingRoutines::RelativePhysicalFolderProblemCollections+
  TeachingRoutines::RelativePhysicalFolderProblems+
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
  }
  return true;
}

bool ProblemCollection::LoadMe(std::stringstream& comments)
{ MacroRegisterFunctionWithName("ProblemCollection::LoadMe");
  std::fstream theFile;
  if (!FileOperations::OpenFileOnTopOfProjectBase(theFile, this->RelativePhysicalFileNameWithFolder, false, false, false))
  { comments << "<b>Failed to open file " << this->RelativePhysicalFileNameWithFolder << "</b>";
    return false;
  } else
  { this->theProblems.SetSize(0);
    Problem currentProblem;
    theFile >> currentProblem.fileName;
    //stOutput << "Exercise file name: " << this->RelativePhysicalFileNameWithFolder << " Current file name: " << currentProblemFileName;
    theFile.seekg(0);
    while (std::getline(theFile, currentProblem.fileName))
      this->theProblems.AddOnTop(currentProblem);
  }
  return true;
}

std::string TeachingRoutines::GetCurrentProblemItem()
{ MacroRegisterFunctionWithName("TeachingRoutines::GetCurrentProblemItem");
  std::stringstream out;
  if (!this->LoadExercises())
  { out << "<hr><b>Loading exercises failed.</b><br> "
    << this->comments.str() << "<hr>";
    return out.str();
  }
  std::string currentCollectionName=theGlobalVariables.GetWebInput("currentProblemCollection");
  std::string currentProblemName=theGlobalVariables.GetWebInput("currentProblem");
  if (currentCollectionName=="")
    return this->ToStringExerciseSelection();
  if (!this->HasExercise(currentCollectionName))
  { out << "<hr><b>Could not find problem collection: " << currentCollectionName << "</b><hr>";
    out << this->ToStringExerciseSelection();
    return out.str();
  }
  ProblemCollection& currentCollection= this->theExercises.GetElement(this->GetExerciseIndex(currentCollectionName));
  if (!currentCollection.LoadMe(this->comments))
  { out << "<hr><b>Failed to load problem collection: " << currentCollection.fileName << ".</b> "
    << this->comments.str() << "<hr>" << this->ToStringExerciseSelection();
    return out.str();
  }
  if (currentProblemName=="")
    return currentCollection.ToStringProblemLinks();
  if (!currentCollection.HasProblem(currentProblemName))
  { out << "<hr><b>Could not find problem: " << currentProblemName << ".</b></hr>"
    << currentCollection.ToStringProblemLinks();
    return out.str();
  }
  Problem& currentProblem=currentCollection.theProblems.GetElement(currentCollection.GetProblemIndex(currentProblemName));
  if (!currentProblem.LoadMe(this->comments))
  { out << "<hr><b>Failed to load problem: " << currentProblem.fileName << ".</b>"
    << this->comments.str() << "<hr>" << currentCollection.ToStringProblemLinks();
    return out.str();
  }
  return currentProblem.ToStringExam();
}

std::string Problem::GetSubmitAnswersJavascript()
{ std::stringstream out;
  out
  << "<script type=\"text/javascript\"> \n"
  << "function submitAnswers(){\n"
  << "  var https = new XMLHttpRequest();\n"
  << "  https.open(\"POST\", \"" << theGlobalVariables.DisplayNameCalculatorWithPath << "\", true);\n"
  << "  https.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\");\n";
  std::stringstream requestStream;
  requestStream << "request=submitProblem&" << WebWorker::ToStringCalculatorArgumentsCGIinputExcludeRequestType()
  << "problemStatement=" << CGI::StringToURLString(this->finalCommandsPoseProblem);
  out << "var params=\"" << requestStream.str() << "\";\n";
  out
  << "  https.send(params);\n"
  << "  https.onload = function() {\n"
  << "  span = document.createElement('span');\n"
  << "  span.innerHTML=https.responseText;\n"
  << "  document.body.appendChild(span);\n"
  << "}\n"
  << "}\n"
  << "</script>";
  return out.str();
}

int WebWorker::ProcessSubmitProblem()
{ MacroRegisterFunctionWithName("WebWorker::ProcessSubmitProblem");
  stOutput << "<html><body>" << this->ToStringCalculatorArgumentsHumanReadable() << "<hr><hr><hr><hr><hr><hr>"
  << "</body></html>";
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
  //out << "<hr>Message follows.<br>" << this->theMessage << "<hr>";
  out << this->ToStringCalculatorArgumentsHumanReadable() << "<hr><hr><hr><hr><hr><hr><hr><hr>";
  out << theRoutines.GetCurrentProblemItem();
  out << "</body></html>";
  return out.str();
}

bool CalculatorFunctionsGeneral::innerInterpretHtml
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerInterpretHtml");
  Problem theProblem;
  if (!input.IsOfType<std::string>(&theProblem.inputHtml))
    return theCommands << "Extracting calculator expressions from html takes as input strings. ";
  theProblem.InterpretHtml(theCommands.Comments);
  return output.AssignValue(theProblem.outputHtml, theCommands);
}

std::string Problem::ToStringExtractedCommands()
{ MacroRegisterFunctionWithName("Problem::ToStringExtractedCommands");
  std::stringstream out;
  out << "<b>The html read follows.</b><br><hr> " << this->inputHtml;
//  out << "<hr><b>The split strings follow. </b><hr>" << splitStrings.ToStringCommaDelimited();
  out << "<hr><b>The extracted commands follow.</b><hr>";
  out << "<table><tr><td>Command(s)</td><td>Command type</td></tr>";
  for (int i=0; i<this->commands.size; i++)
    out << "<tr>" << "<td>" << this->commands[i] << "</td><td>"
    << this->commandTypes[i] << "</td></tr>";
  out << "</table>";
  return out.str();
}

bool Problem::ExecuteCommands(Calculator& theInterpretter, std::stringstream& comments)
{ MacroRegisterFunctionWithName("Problem::ExecuteCommands");
  theInterpretter.init();
  std::stringstream calculatorCommands;
  for (int i=0; i<this->commands.size; i++)
    calculatorCommands << "SeparatorBetweenSpans;" << this->commands[i];
  calculatorCommands << "SeparatorBetweenSpans;";
//  stOutput << "nput cmds: " << inputCommands.str();
  theInterpretter.Evaluate(calculatorCommands.str());
  return !theInterpretter.flagAbortComputationASAP;
}

bool Problem::InterpretHtml(std::stringstream& comments)
{ MacroRegisterFunctionWithName("Problem::InterpretHtml");
  if (!this->ExtractExpressionsFromHtml(comments))
    return false;
  int numAttempts=0;
  const int MaxNumAttempts=10;
  while (numAttempts<MaxNumAttempts)
  { numAttempts++;
    Calculator theInterpretter;
    std::stringstream out;
    if (!this->ExecuteCommands(theInterpretter, comments))
    { if (numAttempts>=MaxNumAttempts)
      { out << "Failed to evaluate the commands: " << MaxNumAttempts
        << " attempts made. Calculator evaluation details follow.<hr> "
        << theInterpretter.outputString << "<hr><b>Comments</b><br>"
        << theInterpretter.Comments.str();
        this->outputHtml=out.str();
      }
      continue;
    }
    int spanCounter=0;
    bool moreThanOneCommand=false;
    std::string lastCommands;
    this->finalCommandsPoseProblem="";
    for (int i=1; i<theInterpretter.theProgramExpression.children.size; i++)
    { if (theInterpretter.theProgramExpression[i].ToString()!="SeparatorBetweenSpans")
      { if (this->commandTypes[spanCounter]=="calculatorAnswerVerification")
          continue;
        if (moreThanOneCommand)
        { out << "; ";
          lastCommands+="; ";
        }
        out << theInterpretter.theProgramExpression[i].ToString();
        lastCommands+=theInterpretter.theProgramExpression[i].ToString();
        moreThanOneCommand=true;
        continue;
      }
      if (i!=1)
      { out << " \\) </" << this->tagTypes[spanCounter] << ">";
        this->finalCommandsPoseProblem+=lastCommands;
        lastCommands="";
        spanCounter++;
      }
      out << this->betweenTheCommands[spanCounter];
      if (i!=theInterpretter.theProgramExpression.children.size-1)
      { out << "<" << this->tagTypes[spanCounter] << " class=\"" << this->commandTypes[spanCounter] << "\"> \\( ";
        moreThanOneCommand=false;
      }
    }
    out << "<hr><hr><hr><hr><hr><hr><hr><hr><hr>Final calculator commands:<br>" << this->finalCommandsPoseProblem << "<hr>";
    out << "<hr>" << this->ToStringExtractedCommands() << "<hr>";
  //  out << "<hr> Between the commands:" << this->betweenTheCommands.ToStringCommaDelimited();
    this->outputHtml=out.str();
    break;
  }

  return true;
}

bool Problem::ExtractExpressionsFromHtml(std::stringstream& comments)
{ MacroRegisterFunctionWithName("Problem::ExtractExpressionsFromHtml");
  std::stringstream theReader(this->inputHtml);
  theReader.seekg(0);
  List<std::string> splitStrings;
  std::string word, readChars;
  splitStrings.SetExpectedSize(theReader.str().size()/4);
  while (theReader >> readChars)
  { word="";
    for (unsigned i=0; i< readChars.size(); i++)
      if (splittingChars.Contains(readChars[i]))
      { if (word!="")
          splitStrings.AddOnTop(word);
        std::string charToString;
        charToString.push_back(readChars[i]);
        splitStrings.AddOnTop(charToString);
        word="";
      } else
        word.push_back(readChars[i]);
    if (word!="")
      splitStrings.AddOnTop(word);
  }
  std::string nextString, outsideOfCalculatorSpan, tagString;
  bool buildingSpanStartTag=false;
  bool buildingSpanFinishTag=false;
  bool buildingTextAreaStartTag=false;
  bool buildingTextAreaFinishTag=false;
  int currentSpanType=-1;
  for (int i=0; i<splitStrings.size; i++)
  { const std::string currentString=splitStrings[i];
    if (i<splitStrings.size-1)
      nextString=splitStrings[i+1];
    else
      nextString="";
    if (currentString=="<")
    { tagString="<";
      if (nextString=="span")
        buildingSpanStartTag=true;
      if (nextString=="/span")
        buildingSpanFinishTag=true;
      if (nextString=="textarea")
        buildingTextAreaStartTag=true;
      if (nextString=="/textarea")
        buildingTextAreaFinishTag=true;
    }
    if (buildingSpanStartTag)
      if (this->calculatorClasses.Contains(currentString))
      { currentSpanType=this->calculatorClasses.GetIndex(currentString);
        this->commands.AddOnTop("");
        this->commandTypes.AddOnTop(currentString);
        this->tagTypes.AddOnTop("span");
        this->betweenTheCommands.AddOnTop(outsideOfCalculatorSpan);
        outsideOfCalculatorSpan="";
      }
    if (buildingTextAreaStartTag)
      if (this->calculatorClasses.Contains(currentString))
      { currentSpanType=this->calculatorClasses.GetIndex(currentString);
        this->commands.AddOnTop("");
        this->commandTypes.AddOnTop(currentString);
        this->tagTypes.AddOnTop("textarea");
        this->betweenTheCommands.AddOnTop(outsideOfCalculatorSpan);
        outsideOfCalculatorSpan="";
      }
    if (!buildingSpanFinishTag    && !buildingSpanStartTag &&
        !buildingTextAreaStartTag && !buildingTextAreaFinishTag &&
        currentSpanType!=-1)
      *this->commands.LastObject()+= currentString +" ";
    if (tagString=="" )
      outsideOfCalculatorSpan+=currentString+" ";
    if (tagString!="" && currentString!="<")
    { tagString+=currentString;
      bool needsSpace=true;
      if (currentString.size()==1)
        if (this->splittingChars.Contains(currentString[0]))
          needsSpace=false;
      if (nextString.size()==1)
        if (this->splittingChars.Contains(nextString[0]))
          needsSpace=false;
      if (needsSpace)
        tagString+=" ";
    }
    if (currentString==">")
    { buildingSpanStartTag=false;
      buildingTextAreaStartTag=false;
      if (currentSpanType==-1)
      { outsideOfCalculatorSpan+=tagString;
        tagString="";
      }
      if (buildingSpanFinishTag || buildingTextAreaFinishTag)
        currentSpanType=-1;
      buildingSpanFinishTag=false;
      buildingTextAreaFinishTag=false;
    }
  }
  this->betweenTheCommands.AddOnTop(outsideOfCalculatorSpan);
  if (this->betweenTheCommands.size!=this->commands.size+1)
  { crash << "I have that this->betweenTheCommands.size!=this->commands.size+1: "
    << "this shouldn't happen - or at least it appears so to me as I program this right now. "
    << "Crashing to let you know. "
    << crash;
  }
  for (int i=0; i<this->commands.size; i++)
    this->commands[i]=this->CleanUpCommandString(this->commands[i]);
  return true;
}

std::string Problem::CleanUpCommandString(const std::string& inputCommand)
{ MacroRegisterFunctionWithName("Problem::CleanUpCommandString");
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
  Problem theProblem;
  if (!input.IsOfType<std::string>(&theProblem.inputHtml))
    return theCommands << "Extracting calculator expressions from html takes as input strings. ";
  if (!theProblem.ExtractExpressionsFromHtml(theCommands.Comments))
    return false;

  return output.AssignValue(theProblem.ToStringExtractedCommands(), theCommands);
}
