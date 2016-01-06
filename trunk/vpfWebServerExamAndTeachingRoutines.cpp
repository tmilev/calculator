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

class Problem
{
public:
  std::string fileName;
  std::string RelativePhysicalFileNameWithFolder;
  std::string inputHtml;
  std::string outputHtml;
  List<SyntacticElementHTML> theContent;
  std::string finalCommandsPoseProblem;
  std::string finalCommandsPoseAndAnswerProblem;
  List<std::string> calculatorClasses;
  List<std::string> calculatorOpenTags;
  List<std::string> calculatorCloseTags;
  List<char> splittingChars;
  bool IsSplittingChar(const std::string& input);
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
  bool ExecuteCommandsPrepare(Calculator& theInterpretter, std::stringstream& comments);
  bool ExecuteCommandsTestStudent(Calculator& theInterpretter, std::stringstream& comments);
  bool InterpretHtml(std::stringstream& comments);
  bool ExtractExpressionsFromHtml(std::stringstream& comments);
  std::string ToStringParsingStack(List<SyntacticElementHTML>& theStack);
  std::string CleanUpCommandString(const std::string& inputCommand);
  std::string ToStringExam();
  std::string GetSubmitAnswersJavascript();
  std::string ToStringContent();
  std::string ToStringExtractedCommands();
};

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
  out << "Latex javascript off!!!";
//  out << CGI::GetLaTeXProcessingJavascript();
  out << "\n<form class=\"problemForm\" method=\"GET\" id=\"formProblemCollection\" name=\"formProblemCollection\" action=\""
  << theGlobalVariables.DisplayNameCalculatorWithPath << "\">\n" ;
  out << theWebServer.GetActiveWorker().GetHtmlHiddenInputs();
  out << theWebServer.GetActiveWorker().GetHtmlHiddenInputExercise();
  out << "\n</form>\n";
  out << this->outputHtml;
  return out.str();
}

std::string ProblemCollection::ToStringProblemLinks()
{ MacroRegisterFunctionWithName("ProblemCollection::ToStringProblemLinks");
  std::stringstream out;
  out << this->theProblems.size << " problem(s).";
  out << "\n<form method=\"GET\" id=\"formProblemCollection\" name=\"formProblemCollection\" action=\""
  << theGlobalVariables.DisplayNameCalculatorWithPath << "\">\n" ;
  out << theWebServer.GetActiveWorker().GetHtmlHiddenInputs();
  out << theWebServer.GetActiveWorker().GetHtmlHiddenInputExercise();
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
  out << "\n<FORM method=\"GET\" id=\"formExercises\" name=\"formExercises\" action=\""
  << theGlobalVariables.DisplayNameCalculatorWithPath << "\">\n" ;
  out << theWebServer.GetActiveWorker().GetHtmlHiddenInputs();
  out << theWebServer.GetActiveWorker().GetHtmlHiddenInputExercise();
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

  //  out << "params+=" <<  << ";";
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
  out << "<table><tr><td>Command syntactic role</td><td>tag</td><td>content</td></tr>";
  for (int i=0; i<this->theContent.size; i++)
    if (this->theContent[i].syntacticRole!="")
      out << "<tr>" << "<td>" << this->theContent[i].syntacticRole << "</td>"
      << "<td>" << this->theContent[i].ToStringOpenTag() << "</td>"
      << "<td>" << this->theContent[i].content << "</td>"
      << "</tr>";
    else
      out << "<tr><td></td><td>n/a</td><td>n/a</td><td>n/a</td>";
  out << "</table>";
  return out.str();
}

std::string Problem::ToStringContent()
{ MacroRegisterFunctionWithName("Problem::ToStringContent");
  std::stringstream out;
  out << "<b>The html read follows.</b><br><hr> " << this->inputHtml;
//  out << "<hr><b>The split strings follow. </b><hr>" << splitStrings.ToStringCommaDelimited();
  out << "<hr><b>The extracted commands follow.</b><hr>";
  for (int i=0; i<this->theContent.size; i++)
    out << this->theContent[i].ToStringTagAndContent();
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
    out << " " << this->tagKeys << "=\"" << this->tagValues << "\"";
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
    return this->ToStringTagAndContent();
  std::stringstream out;
  out << "<span style=\"color:green\">";
  out << CGI::StringToHtmlStrinG(this->syntacticRole);
  out << "</span>";
  out << "[" << this->ToStringTagAndContent() << "]";
  return out.str();
}

std::string SyntacticElementHTML::GetKeyValue(const std::string& theKey)
{ int theIndex=this->tagKeys.GetIndex(theKey);
  if (theIndex==-1)
    return "";
  return this->tagKeys[theIndex];
}

void SyntacticElementHTML::SetKeyValue(const std::string& theKey, const std::string& theValue)
{ int theIndex=this->tagKeys.GetIndex(theKey);
  if (theIndex==-1)
  { theIndex=this->tagKeys.size-1;
    this->tagKeys.AddOnTop(theKey);
    this->tagValues.SetSize(this->tagKeys.size);
  }
  this->tagKeys[theIndex]=theValue;
}

std::string SyntacticElementHTML::ToStringInterpretted()
{ if (this->syntacticRole=="")
    return this->content;
  if (this->GetKeyValue("class")=="calculatorAnswerVerification")
    return "";
  std::stringstream out;
  out << this->ToStringOpenTag();
  out << this->interpretedCommand;
  out << this->ToStringCloseTag();
  return out.str();
}

bool SyntacticElementHTML::IsInterpretedDuringPreparation()
{ return this->syntacticRole!="" &&
         this->GetKeyValue("class")!="calculatorAnswerVerification" &&
         this->GetKeyValue("class")!="calculatorStudentAnswer";
}

bool Problem::ExecuteCommandsPrepare(Calculator& theInterpretter, std::stringstream& comments)
{ MacroRegisterFunctionWithName("Problem::ExecuteCommandsPrepare");
  theInterpretter.init();
  std::stringstream calculatorCommands;
  for (int i=0; i<this->theContent.size; i++)
  { if (!this->theContent[i].IsInterpretedDuringPreparation())
    { calculatorCommands << "SeparatorBetweenSpans;";
      continue;
    }
    calculatorCommands << this->theContent[i].content;
  }
//  stOutput << "nput cmds: " << inputCommands.str();
  theInterpretter.Evaluate(calculatorCommands.str());
  return !theInterpretter.flagAbortComputationASAP;
}

bool Problem::InterpretHtml(std::stringstream& comments)
{ MacroRegisterFunctionWithName("Problem::InterpretHtml");
  if (!this->ExtractExpressionsFromHtml(comments))
  { this->outputHtml="<b>Failed to interpret html input.</b><br>" +this->ToStringContent();
    return false;
  }
  int numAttempts=0;
  const int MaxNumAttempts=10;
  while (numAttempts<MaxNumAttempts)
  { numAttempts++;
    Calculator theInterpretter;
    std::stringstream out;
    if (!this->ExecuteCommandsPrepare(theInterpretter, comments))
    { if (numAttempts>=MaxNumAttempts)
      { out << "Failed to evaluate the commands: " << MaxNumAttempts
        << " attempts made. Calculator evaluation details follow.<hr> "
        << theInterpretter.outputString << "<hr><b>Comments</b><br>"
        << theInterpretter.Comments.str();
        this->outputHtml=out.str();
      }
      continue;
    }
    bool moreThanOneCommand=false;
    std::string lastCommands;
    this->finalCommandsPoseProblem="";
    int commandCounter=1;
    for (int spanCounter=0;spanCounter <this->theContent.size; spanCounter++)
    { if (!this->theContent[spanCounter].IsInterpretedDuringPreparation())
      { if (theInterpretter.theProgramExpression[commandCounter].ToString()!="SeparatorBetweenSpans")
        { out << "<b>Error: calculator commands don't match the tags.</g>";
          this->outputHtml=out.str();
          return false;
        }
        commandCounter++;
        continue;
      }
      moreThanOneCommand=false;
      for (; commandCounter<theInterpretter.theProgramExpression.children.size; commandCounter++ )
      { if (theInterpretter.theProgramExpression[commandCounter].ToString()=="SeparatorBetweenSpans")
          break;
        this->theContent[spanCounter].interpretedCommand+=theInterpretter.theProgramExpression[commandCounter].ToString();
        if (moreThanOneCommand)
          this->theContent[spanCounter].interpretedCommand+=" ";
        moreThanOneCommand=true;
      }
    }
    for (int i=0; i<this->theContent.size; i++)
      out << this->theContent[i].ToStringInterpretted();

    out << "<hr><hr><hr><hr><hr><hr><hr><hr><hr>Final calculator commands:<br>" << this->finalCommandsPoseProblem << "<hr>";
    out << "<hr>" << this->ToStringExtractedCommands() << "<hr>";
  //  out << "<hr> Between the commands:" << this->betweenTheCommands.ToStringCommaDelimited();
    this->outputHtml=out.str();
    break;
  }

  return true;
}

bool Problem::IsSplittingChar(const std::string& input)
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

bool Problem::ExtractExpressionsFromHtml(std::stringstream& comments)
{ MacroRegisterFunctionWithName("Problem::ExtractExpressionsFromHtml");
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
  for (int i=0; i<this->calculatorClasses.size; i++)
  { this->calculatorOpenTags.AddOnTop("<"+this->calculatorClasses[i]+">");
    this->calculatorCloseTags.AddOnTop("</"+this->calculatorClasses[i]+">");
  }
  List<SyntacticElementHTML> eltsStack;
  eltsStack.SetExpectedSize(theElements.size+SyntacticElementHTML::ParsingNumDummyElements);
  for (int i=0; i<SyntacticElementHTML::ParsingNumDummyElements; i++)
    eltsStack.AddOnTop( (std::string) "<>");
  int indexInElts=-1;
  bool reduced=false;
  do
  { if (!reduced)
    { indexInElts++;
      if (indexInElts<theElements.size)
        eltsStack.AddOnTop(theElements[indexInElts]);
    }
    stOutput << "<br>" << this->ToStringParsingStack(eltsStack);
    reduced=true;
    SyntacticElementHTML& last = eltsStack[eltsStack.size-1];
    SyntacticElementHTML& secondToLast = eltsStack[eltsStack.size-2];
    SyntacticElementHTML& thirdToLast = eltsStack[eltsStack.size-3];
    SyntacticElementHTML& fourthToLast = eltsStack[eltsStack.size-4];
    SyntacticElementHTML& fifthToLast = eltsStack[eltsStack.size-5];
    SyntacticElementHTML& sixthToLast = eltsStack[eltsStack.size-6];
//    SyntacticElementHTML& seventhToLast = eltsStack[eltsStack.size-7];
    if (secondToLast.syntacticRole=="<openTagCalc>" && last.syntacticRole=="<closeTag>" && secondToLast.tag==last.tag)
    { secondToLast.syntacticRole="command";
      eltsStack.RemoveLastObject();
      continue;
    }
    if (last.syntacticRole=="<closeTag>")
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
    if (sixthToLast.syntacticRole=="<openTag" && fourthToLast=="=" && thirdToLast=="\""
        && last=="\"" )
    { sixthToLast.SetKeyValue(fifthToLast.content, secondToLast.content);
      eltsStack.SetSize(eltsStack.size-5);
      continue;
    }
    if (secondToLast.syntacticRole=="<openTag" && last.syntacticRole==">")
    { if (this->calculatorClasses.Contains(secondToLast.GetKeyValue("class")))
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
      this->theContent.AddOnTop(eltsStack[i]);
  }
  return result;
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
