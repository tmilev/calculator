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
  List<std::string> defaultKeysIfMissing;
  List<std::string> defaultValuesIfMissing;
  bool flagUseDisplaystyleInMathMode;
  std::string interpretedCommand;
  static int ParsingNumDummyElements;
  bool IsInterpretedByCalculatorDuringPreparation();
  bool IsInterpretedNotByCalculator();
  bool IsAnswer();
  std::string GetKeyValue(const std::string& theKey)const;
  void SetKeyValue(const std::string& theKey, const std::string& theValue);
  void resetAllExceptContent();
  std::string ToStringInterpretted();
  std::string ToStringTagAndContent();
  std::string ToStringOpenTag();
  std::string ToStringCloseTag();
  std::string GetTagClass();
  std::string ToStringDebug();
  SyntacticElementHTML(){this->flagUseDisplaystyleInMathMode=false;}
  SyntacticElementHTML(const std::string& inputContent)
  { this->flagUseDisplaystyleInMathMode=false;
    this->content=inputContent;
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
  int NumProblemsFound;
  List<int> randomSeedsIfInterpretationFails;
  bool flagRandomSeedGiven;
  bool flagIsExamHome;
  bool flagIsExamIntermediate;
  bool flagIsExamProblem;
  bool flagParentInvestigated;
  bool flagIsForReal;
  bool flagLoadedFromDB;

  std::string databaseInfo;
  std::string problemCommandsWithInbetweens;
  std::string problemCommandsNoVerification;
  std::string problemCommandsVerification;
  std::string fileName;
  std::string RelativePhysicalFileNameWithFolder;
  std::string inputHtml;
  std::string outputHtmlMain;
  std::string outputHtmlNavigation;
  std::string currentExamHomE;
  std::string currentExamIntermediatE;
  static const std::string BugsGenericMessage;
  List<std::string> calculatorClasses;
  List<char> splittingChars;
  List<SyntacticElementHTML> eltsStack;
  List<SyntacticElementHTML> theContent;
  List<std::string> answerVerificationCommand;
//  List<std::string> answerFirstCorrectSubmission;
  Selection studentTagsAnswered;
  ProblemData theProblemData;
  List<std::string> studentAnswersUnadulterated;
  List<std::string> problemList;
  List<std::string> problemListOfParent;
  std::string currentUserDatabaseString;
  bool flagLoadedSuccessfully;
  static const std::string RelativePhysicalFolderProblemCollections;
  std::stringstream comments;
  bool CheckContent(std::stringstream& comments);
  bool CanBeMerged(const SyntacticElementHTML& left, const SyntacticElementHTML& right);
  bool LoadMe(bool doLoadDatabase, std::stringstream& comments);
  std::string CleanUpLink(const std::string& inputLink);
  bool ParseHTML(std::stringstream& comments);
  bool IsSplittingChar(const std::string& input);
  bool IsStateModifierApplyIfYes(SyntacticElementHTML& inputElt);
  bool ExtractAnswerIds(std::stringstream& comments);
  bool InterpretHtml(std::stringstream& comments);
  bool InterpretHtmlOneAttempt(Calculator& theInterpreter, std::stringstream& comments);
  bool PrepareAndExecuteCommands(Calculator& theInterpreter, std::stringstream& comments);
  bool PrepareCommands(Calculator& theInterpreter, std::stringstream& comments);
  std::string CleanUpCommandString(const std::string& inputCommand);
  void InterpretNotByCalculator(SyntacticElementHTML& inputOutput);
  void InterpretGenerateLink(SyntacticElementHTML& inputOutput);
  void InterpretGenerateStudentAnswerButton(SyntacticElementHTML& inputOutput);
  void InterpretManageClass(SyntacticElementHTML& inputOutput);
  std::string ToStringClassDetails
( const List<List<std::string> >& userTable,
  const List<std::string>& labelsUserTable,
  bool adminsOnly,
  const SyntacticElementHTML& inputElement
)
  ;
  std::string GetSubmitEmailsJavascript();
  std::string GetSubmitAnswersJavascript();
  std::string GetDatabaseTableName();
  void LoadCurrentProblemItem();
  void FigureOutCurrentProblemList(std::stringstream& comments);
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
{ this->flagRandomSeedGiven=false;
  this->NumAttemptsToInterpret=0;
  this->MaxInterpretationAttempts=10;
  this->flagLoadedSuccessfully=false;
  this->flagIsExamHome=false;
  this->flagIsExamIntermediate=false;
  this->flagIsExamProblem=false;
  this->flagParentInvestigated=false;
  this->NumProblemsFound=0;
  this->flagIsForReal=false;
  this->flagLoadedFromDB=false;
}

const std::string CalculatorHTML::RelativePhysicalFolderProblemCollections="ProblemCollections/";

bool CalculatorHTML::LoadMe(bool doLoadDatabase, std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::LoadMe");
  this->RelativePhysicalFileNameWithFolder=
  this->RelativePhysicalFolderProblemCollections+
  this->fileName
  ;
  std::fstream theFile;
  if (!FileOperations::OpenFileOnTopOfProjectBase(theFile, this->RelativePhysicalFileNameWithFolder, false, false, false))
  { comments << "<b>Failed to open file " << this->RelativePhysicalFileNameWithFolder << "</b>";
    return false;
  }
  std::stringstream contentStream;
  contentStream << theFile.rdbuf();
  this->inputHtml=contentStream.str();
  std::string randString= theGlobalVariables.GetWebInput("randomSeed");
  if (randString!="")
  { std::stringstream randSeedStream(randString);
    randSeedStream >> this->theProblemData.randomSeed;
    this->flagRandomSeedGiven=true;
  }
  this->flagIsForReal=false;
  if (theGlobalVariables.flagLoggedIn)
    this->flagIsForReal=
    (theGlobalVariables.userCalculatorRequestType=="submitProblem" ||
     theGlobalVariables.userCalculatorRequestType=="examForReal"
    );
#ifdef MACRO_use_MySQL
  if (this->flagIsForReal && doLoadDatabase)
  { UserCalculator theUser;
    theUser.username=theGlobalVariables.userDefault;
    DatabaseRoutines theRoutines;
    if (!theUser.LoadProblemStringFromDatabase(theRoutines, this->currentUserDatabaseString, comments))
    { comments << "Failed to load current users' problem save-file.";
      return false;
    }
    if (!theUser.InterpretDatabaseProblemData(this->currentUserDatabaseString, comments))
    { comments << "Failed to load current users' problem save-file.";
      return false;
    }
    this->theProblemData=theUser.GetProblemDataAddIfNotPresent(this->fileName);
  }
#endif // MACRO_use_MySQL
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
  out << "<nav>"
  << this->outputHtmlNavigation
  << "<hr><small>Generated in "
  << MathRoutines::ReducePrecision(theGlobalVariables.GetElapsedSeconds()-startTime)
  << " second(s).</small>" << "</nav> "
  //<< "<section>"
  << this->outputHtmlMain
  //<< "</section>"
  ;
  return out.str();
}

void CalculatorHTML::LoadCurrentProblemItem()
{ MacroRegisterFunctionWithName("CalculatorHTML::LoadCurrentProblemItem");
  this->fileName= CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamFile"));
  this->flagLoadedSuccessfully=false;
  bool needToFindDefault=(this->fileName=="");
  if (!needToFindDefault)
    needToFindDefault=!this->LoadMe(true, this->comments);
  else
    this->comments << "<b>Selecting default course homework file.</b><br>";
  if (needToFindDefault)
  { if (!this->FindExamItem())
    { this->comments << "<b>No problems/exams to serve: found no html content in folder: "
      << this->RelativePhysicalFolderProblemCollections << ".</b>";
      return;
    }
    if (!this->LoadMe(true, this->comments))
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

std::string CalculatorHTML::GetSubmitEmailsJavascript()
{ std::stringstream out;
  out
  << "<script type=\"text/javascript\"> \n"
  << "function addEmails(idEmailList, problemCollectionName, idOutput, userRole, idExtraInfo){\n"
  << "  spanOutput = document.getElementById(idOutput);\n"
  << "  if (spanOutput==null){\n"
  << "    spanOutput = document.createElement('span');\n"
  << "    document.body.appendChild(spanOutput);\n"
  << "    spanOutput.innerHTML= \"<span style='color:red'> ERROR: span with id \" + idEmailList + \"MISSING! </span>\";\n"
  << "  }\n"
  << "  spanEmailList = document.getElementById(idEmailList);\n"
  << "  spanExtraInfo = document.getElementById(idExtraInfo);\n"
  << "  inputParams='request=addEmails';\n"
  << "  inputParams+='&userRole='+userRole;\n"
  << "  inputParams+='&" << theGlobalVariables.ToStringCalcArgsNoNavigation() << "';\n"
  << "  inputParams+='&mainInput=' + encodeURIComponent(spanEmailList.value);\n"
  << "  inputParams+='&extraInfo=' + encodeURIComponent(spanExtraInfo.value);\n"
//  << "  inputParams+='&currentExamHome=' + problemCollectionName;\n"
  << "  inputParams+='&currentExamHome=" << CGI::StringToURLString(this->fileName) << "';\n"
  << "  var https = new XMLHttpRequest();\n"
  << "  https.open(\"POST\", \"" << theGlobalVariables.DisplayNameCalculatorWithPath << "\", true);\n"
  << "  https.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\");\n"
  << "  https.onload = function() {\n"
  << "    spanOutput.innerHTML=https.responseText;\n"
  << "  }\n"
  << "  https.send(inputParams);\n"
  << "}\n"
  << "</script>";
  return out.str();
}

std::string CalculatorHTML::GetSubmitAnswersJavascript()
{ std::stringstream out;
  std::string requestType= theGlobalVariables.userCalculatorRequestType=="examForReal" ? "submitProblem" :
  "submitExercise";
  out
  << "<script type=\"text/javascript\"> \n"
  << "var timerForPreviewAnswers;\n"
  << "function previewAnswers(idAnswer, idVerification){\n"
  << "  clearTimeout(timerForPreviewAnswers);\n"
  << "  timerForPreviewAnswers=setTimeout(function(){\n"
  << "    params=\"" << this->ToStringCalculatorArgumentsForProblem("submitProblemPreview") << "\";\n"
  << "    submitOrPreviewAnswers(idAnswer, idVerification, params);\n"
  << "  }, 1700);"
  << "}\n"
  << "function submitAnswers(idAnswer, idVerification){\n"
  << "  clearTimeout(timerForPreviewAnswers);\n"
  << "  params=\"" << this->ToStringCalculatorArgumentsForProblem(requestType) << "\";\n"
  << "  submitOrPreviewAnswers(idAnswer, idVerification, params);\n"
  << "}\n"
  << "function submitOrPreviewAnswers(idAnswer, idVerification, inputParams){\n"
  << "  clearTimeout(timerForPreviewAnswers);\n"
  << "  spanVerification = document.getElementById(idVerification);\n"
  << "  if (spanVerification==null){\n"
  << "    spanVerification = document.createElement('span');\n"
  << "    document.body.appendChild(spanVerification);\n"
  << "    spanVerification.innerHTML= \"<span style='color:red'> ERROR: span with id \" + idVerification + \"MISSING! </span>\";\n"
  << "  }\n"
  << "  spanStudentAnswer = document.getElementById(idAnswer);\n"
  << "  inputParams+=\"&calculatorAnswer\" + idAnswer\n"
  << "          + \"=\"+encodeURIComponent(spanStudentAnswer.value);\n"
  << "  var https = new XMLHttpRequest();\n"
  << "  https.open(\"POST\", \"" << theGlobalVariables.DisplayNameCalculatorWithPath << "\", true);\n"
  << "  https.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\");\n"
;
//Old code, submits all answers. May need to be used as an alternative
//submission option.
//  for (int i=0; i<this->theContent.size; i++)
//    if (this->IsStudentAnswer(this->theContent[i]))
//      out << "  inputParams+=\"&calculatorAnswer" << this->theContent[i].GetKeyValue("id") << "=\"+encodeURIComponent("
//      << "document.getElementById('" << this->theContent[i].GetKeyValue("id") << "').value);\n";
  out
  << "  https.onload = function() {\n"
  << "    spanVerification.innerHTML=https.responseText;\n"
  << "    MathJax.Hub.Queue(['Typeset', MathJax.Hub, spanVerification]);"
  << "  }\n"
  << "  https.send(inputParams);\n"
  << "}\n"
  << "</script>";
  return out.str();
}

const std::string CalculatorHTML::BugsGenericMessage=
"Please take a screenshot, copy the link address and send those along \
with a short explanation to the following UMass instructor: Todor Milev (todor.milev@gmail.com)";

int WebWorker::ProcessSubmitProblemPreview()
{ MacroRegisterFunctionWithName("WebWorker::ProcessSubmitProblemPreview");
  std::stringstream studentInterpretation;
  for (int i=0; i<theGlobalVariables.webFormArgumentNames.size; i++)
    if (MathRoutines::StringBeginsWith(theGlobalVariables.webFormArgumentNames[i], "calculatorAnswer", 0))
    { std::string theAnswer= CGI::URLStringToNormal( theGlobalVariables.webFormArguments[i]);
      studentInterpretation << "(" << theAnswer << ");";
    }
  Calculator theInterpreter;
  theInterpreter.init();
  theInterpreter.Evaluate(studentInterpretation.str());
//  TimeWrapper now;
//  std::string yourInputSpanId = "idAnswer"+ Crypto::CharsToBase64String(now.ToStringHumanReadable()) ;
  if (!theInterpreter.flagAbortComputationASAP && theInterpreter.syntaxErrors=="")
    stOutput << "<span style=\"color:magenta\"><b>Interpreting your answer as:</b></span><br>"
    << "\\(" << theInterpreter.theProgramExpression.ToString() << "\\)";
  else if (theInterpreter.syntaxErrors!="")
    stOutput << "<span style=\"color:red\"><b>Failed to parse your answer, got:</b></span><br>"
    << theInterpreter.ToStringSyntacticStackHumanReadable(false);
  else if (theInterpreter.flagAbortComputationASAP)
    stOutput << "<span style=\"color:red\"><b>Failed to evaluate your answer, got:</b></span><br>"
    << theInterpreter.outputString;
  return 0;
}

int WebWorker::ProcessAddUserEmails()
{ MacroRegisterFunctionWithName("WebWorker::ProcessAddUserEmails");
  stOutput << this->GetAddUserEmails();
  stOutput.Flush();
  return 0;
}

std::string WebWorker::GetAddUserEmails()
{ MacroRegisterFunctionWithName("WebWorker::GetAddUserEmails");
  std::stringstream out;
  if (!theGlobalVariables.UserDefaultHasAdminRights())
  { out << "<b>Only admins may add users.</b>";
    return out.str();
  }
  std::string inputEmails;
  inputEmails=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("mainInput"));
  if (inputEmails=="")
  { out << "<b>No emails to add</b>";
    return out.str();
  }
#ifdef MACRO_use_MySQL
  DatabaseRoutines theRoutines;
  std::stringstream comments;
  bool result=theRoutines.AddUsersFromEmails(inputEmails, comments);
  if (result)
    out << "<span style=\"color:green\"> Emails successfully added. Comments: </span>" << comments.str();
  else
    out << "<span style=\"color:red\">Failed to add all users. Comments: </span>" << comments.str();
  return out.str();
#else
  return "<b>no database present.</b>";
#endif // MACRO_use_MySQL
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
  { stOutput << "<b>Failed to parse problem. </b>Comments: " << comments.str();
    return 0;
  }
  Calculator theInterpreter;
  if (!theProblem.PrepareCommands(theInterpreter, comments))
  { stOutput << "<b>Failed to prepare commands.</b>" << comments.str();
    return 0;
  }
  if (!theProblem.flagRandomSeedGiven && !theProblem.flagIsForReal)
    stOutput << "<b>Random seed not given.</b>";
//  stOutput << "<b>debug remove when done: Random seed: " << theProblem.theProblemData.randomSeed << "</b>";
  theProblem.currentExamHomE=        CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamHome"));
  theProblem.currentExamIntermediatE=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamIntermediate"));
  if (theProblem.currentExamHomE=="")
  { stOutput << "<b>Could not find the problem collection to which this problem belongs. "
    << "If you think this is a bug, do the following. " << theProblem.BugsGenericMessage << "</b>";
    return 0;
  }
  if (theProblem.fileName=="")
    crash << "This shouldn't happen: empty file name: theProblem.fileName." << crash;
  std::string problemStatement=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("problemStatement"));
  std::stringstream studentAnswerStream, completedProblemStream;
  completedProblemStream << theProblem.problemCommandsNoVerification;
  std::string studentAnswerNameReader;
  theProblem.studentAnswersUnadulterated.SetSize(theProblem.theProblemData.answerIds.size);
  theProblem.studentTagsAnswered.init(theProblem.theProblemData.answerIds.size);
  for (int i=0; i<theGlobalVariables.webFormArgumentNames.size; i++)
    if (MathRoutines::StringBeginsWith(theGlobalVariables.webFormArgumentNames[i], "calculatorAnswer", &studentAnswerNameReader))
    { int answerIdIndex=theProblem.theProblemData.answerIds.GetIndex(studentAnswerNameReader);
      if (answerIdIndex==-1)
      { stOutput << "<b> You submitted an answer to tag with id " << studentAnswerNameReader << " which is not on my list of "
        << "answerable tags. </b>";
        return 0;
      }
      std::string theAnswer= CGI::URLStringToNormal( theGlobalVariables.webFormArguments[i]);
      if (theAnswer=="")
      { stOutput << "<b> Your answer to tag with id " << studentAnswerNameReader
        << " appears to be empty, please resubmit. </b>";
        return 0;
      }
      theProblem.studentAnswersUnadulterated[answerIdIndex]=theAnswer;
      theProblem.studentTagsAnswered.AddSelectionAppendNewIndex(answerIdIndex);
      studentAnswerStream << studentAnswerNameReader << "= ("
      << CGI::URLStringToNormal(theGlobalVariables.webFormArguments[i]) << ");";
    }
  if (theProblem.studentTagsAnswered.CardinalitySelection==0)
  { stOutput << "<b>Something is wrong: I found no submitted answers.</b>";
    return 0;
  }
  completedProblemStream << studentAnswerStream.str();
  completedProblemStream << "SeparatorBetweenSpans; ";
  for (int i=0; i<theProblem.studentTagsAnswered.CardinalitySelection; i++)
    completedProblemStream << theProblem.CleanUpCommandString(
    theProblem.answerVerificationCommand[theProblem.studentTagsAnswered.elements[i]]);
//  stOutput << "input to the calculator: " << completedProblemStream.str() << "<hr>";
  theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=theGlobalVariables.GetElapsedSeconds()+20;
  theInterpreter.init();
  theInterpreter.Evaluate(completedProblemStream.str());
  if (theInterpreter.flagAbortComputationASAP || theInterpreter.syntaxErrors!="")
  { stOutput << "<b>Error while processing your answer(s).</b> Here's what I understood. ";
    for (int i=0; i<theProblem.studentAnswersUnadulterated.size; i++)
    { Calculator isolatedInterpreter;
      isolatedInterpreter.init();
      theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=theGlobalVariables.GetElapsedSeconds()+20;
      isolatedInterpreter.Evaluate("("+theProblem.studentAnswersUnadulterated[i]+")");
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
  int correctSubmissionsRelevant=0;
  int totalSubmissionsRelevant=0;
  UserCalculator theUser;
  DatabaseRoutines theRoutines;
  theUser.username=theGlobalVariables.userDefault;
  if (theProblem.flagIsForReal)
  { if (!theUser.InterpretDatabaseProblemData(theProblem.databaseInfo, comments))
    { stOutput << "<b>Failed to load user information from database. Answer not recorded. "
      << "This should not happen. " << CalculatorHTML::BugsGenericMessage << "</b>";
      theProblem.flagIsForReal=false;
    } else
      for (int i=0; i<theProblem.studentTagsAnswered.CardinalitySelection; i++)
      { int theIndex=theProblem.studentTagsAnswered.elements[i];
        theProblem.theProblemData.numSubmissions[theIndex]++;
        totalSubmissionsRelevant+=theProblem.theProblemData.numSubmissions[theIndex];
        correctSubmissionsRelevant+=theProblem.theProblemData.numCorrectSubmissions[theIndex];
        if (isCorrect)
        { theProblem.theProblemData.numCorrectSubmissions[theIndex]++;
          correctSubmissionsRelevant++;
          if (theProblem.theProblemData.firstCorrectAnswer[theIndex]=="")
            theProblem.theProblemData.firstCorrectAnswer[theIndex]=theProblem.studentAnswersUnadulterated[theIndex];
          else
            stOutput << "[correct answer already submitted: " << theProblem.theProblemData.firstCorrectAnswer[theIndex] << "]";
        }
      }
  }
  stOutput << "<table width=\"300\"><tr><td>";
  if (!isCorrect)
  { stOutput << "<span style=\"color:red\"><b>Your answer appears to be incorrect.</b></span>";
    //stOutput << "The calculator output: " << theInterpreter.outputString;
  } else
    stOutput << "<span style=\"color:green\"><b>Correct!</b></span>";
  stOutput << "</td></tr>";
#ifdef MACRO_use_MySQL
  if (theProblem.flagIsForReal)
  { std::stringstream comments;
    theUser.SetProblemData(theProblem.fileName, theProblem.theProblemData);
    if (!theUser.StoreProblemDataToDatabase(theRoutines, comments))
      stOutput << "<tr><td><b>This shouldn't happen and may be a bug: failed to store your answer in the database. "
      << CalculatorHTML::BugsGenericMessage << "</b><br>Comments: "
      << comments.str() << "</td></tr>";
    else
      stOutput << "<tr><td>So far " << correctSubmissionsRelevant << " correct and "
      << totalSubmissionsRelevant-correctSubmissionsRelevant << " incorrect submissions.</td></tr>";
  } //else
    //stOutput << "<tr><td><b>Submitting problem solutions allowed only for logged-in users. </b></td></tr>";
#endif
  stOutput << "<tr><td>Your answer was: ";
  for (int i=0; i< theProblem.studentAnswersUnadulterated.size; i++ )
  { stOutput << "\\(" << theProblem.studentAnswersUnadulterated[i] << "\\)";
    if (i<theProblem.studentAnswersUnadulterated.size-1)
      stOutput << "<br>";
  }
  stOutput << "</td></tr>";

  stOutput << "</table>";
//  stOutput << "<hr>" << theInterpreter.outputString << "<hr><hr><hr><hr><hr><hr>";
//  stOutput << this->ToStringCalculatorArgumentsHumanReadable();
  //Calculator answerInterpretter;
  //answerInterpretter.theProgramExpression=theGlobalVariables.GetWebInput("mainInput");
  //answerInterpretter.init();
  return 0;
}

std::string WebWorker::GetDatabasePage()
{ MacroRegisterFunctionWithName("WebWorker::GetDatabasePage");
  std::stringstream out;
  out << "<html>"
  << "<header>"
  << "<link rel=\"stylesheet\" type=\"text/css\" href=\"/styleCalculator.css\">"
  << WebWorker::GetJavascriptStandardCookies()
  << "</header>"
  << "<body onload=\"loadSettings();\">\n";
  out << "<nav>" << theGlobalVariables.ToStringNavigation() << "</nav>";
#ifdef MACRO_use_MySQL
  DatabaseRoutines theRoutines;
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    out << "Browsing database allowed only for logged-in admins.";
  else
    out << "<section>" << theRoutines.ToStringCurrentTableHTML() << "</section>";
#else
out << "<b>Database not available. </b>";
#endif // MACRO_use_MySQL
  out << this->ToStringCalculatorArgumentsHumanReadable();
  out << "</body></html>";
  return out.str();
}

std::string WebWorker::GetExamPage()
{ MacroRegisterFunctionWithName("WebWorker::GetExamPage");
  CalculatorHTML theFile;
  std::stringstream out;
  out << "<html>"
  << "<header>"
  << WebWorker::GetJavascriptStandardCookies()
  << CGI::GetLaTeXProcessingJavascript()
  << "<link rel=\"stylesheet\" type=\"text/css\" href=\"/styleCalculator.css\">"
  << "</header>"
  << "<body onload=\"loadSettings();\">\n";
  out << theFile.LoadAndInterpretCurrentProblemItem();
  out << this->ToStringCalculatorArgumentsHumanReadable();
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
  return output.AssignValue(theProblem.outputHtmlMain, theCommands);
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
  this->flagUseDisplaystyleInMathMode=false;
}

std::string SyntacticElementHTML::ToStringOpenTag()
{ if (this->tag=="")
    return "";
  std::stringstream out;
  out << "<" << this->tag;
  for (int i=0; i<this->tagKeys.size; i++)
    out << " " << this->tagKeys[i] << "=\"" << this->tagValues[i] << "\"";
  for (int i=0; i<this->defaultKeysIfMissing.size; i++)
    if (!this->tagKeys.Contains(this->defaultKeysIfMissing[i]))
      out << " " << this->defaultKeysIfMissing[i] << "=\"" << this->defaultValuesIfMissing[i] << "\"";
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

std::string SyntacticElementHTML::GetKeyValue(const std::string& theKey)const
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
  if (this->IsInterpretedNotByCalculator())
    return this->interpretedCommand;
  std::stringstream out;
  out << this->ToStringOpenTag();
  if (this->interpretedCommand!="")
  { out << "\\( ";
    if (this->flagUseDisplaystyleInMathMode)
      out << "\\displaystyle ";
    out << this->interpretedCommand << " \\)";
  }
  out << this->ToStringCloseTag();
  return out.str();
}

bool SyntacticElementHTML::IsInterpretedNotByCalculator()
{ MacroRegisterFunctionWithName("SyntacticElementHTML::IsInterpretedNotByCalculator");
  if (this->syntacticRole!="command")
    return false;
  std::string tagClass=this->GetKeyValue("class");
  return tagClass=="calculatorExamProblem" || tagClass== "calculatorExamIntermediate"
  || tagClass=="calculatorAnswer" || tagClass=="calculatorManageClass";
}

bool SyntacticElementHTML::IsInterpretedByCalculatorDuringPreparation()
{ if (this->syntacticRole!="command")
    return false;
  std::string tagClass=this->GetKeyValue("class");
  return tagClass=="calculator" || tagClass=="calculatorHidden";
}

bool SyntacticElementHTML::IsAnswer()
{ if (this->syntacticRole!="command")
    return false;
  return this->GetKeyValue("class")=="calculatorAnswer";
}

bool CalculatorHTML::PrepareCommands(Calculator& theInterpreter, std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareCommands");
  if (this->theContent.size==0)
  { comments << "Empty content not allowed. ";
    return false;
  }
  if (this->theContent[0].syntacticRole!="")
    crash << "First command must be empty to allow for command for setting of random seed. " << crash;
  std::stringstream streamWithInbetween, streamNoVerification, streamVerification;
  //stOutput << " The big bad random seed: " << this->randomSeed ;
  streamWithInbetween << "setRandomSeed{}(" << this->theProblemData.randomSeed << ");";
  streamNoVerification << streamWithInbetween.str();
  for (int i=1; i<this->theContent.size; i++) // the first element of the content is fake (used for the random seed)
  { if (!this->theContent[i].IsInterpretedByCalculatorDuringPreparation())
    { streamWithInbetween << "SeparatorBetweenSpans; ";
      continue;
    }
    std::string cleanedupCommand=this->CleanUpCommandString( this->theContent[i].content);
    if (cleanedupCommand.find("\\displaystyle")!=std::string::npos)
      this->theContent[i].flagUseDisplaystyleInMathMode=true;
    if (cleanedupCommand!="")
      streamWithInbetween << cleanedupCommand;
    else
      streamWithInbetween << "SeparatorBetweenSpans; ";
  }
  for (int i=1; i<this->theContent.size; i++)
  { if (this->theContent[i].syntacticRole!="command")
      continue;
    if (this->theContent[i].IsAnswer())
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
{ MacroRegisterFunctionWithName("Problem::PrepareAndExecuteCommands");
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
  std::stringstream out;
  out << theGlobalVariables.ToStringNavigation();
  std::string calcArgsNoPassExamDetails=theGlobalVariables.ToStringCalcArgsNoNavigation();
  if (!this->flagIsExamHome)
    out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?request=exercises&"
    << calcArgsNoPassExamDetails
    << "currentExamHome=&" << "currentExamIntermediate=&"
    << "currentExamFile=" << CGI::StringToURLString(this->currentExamHomE) << "&\"> Course homework home </a><br>";
  else
    out << "<b>Course homework home</b>";
  if (this->flagIsExamProblem && this->currentExamIntermediatE!="")
    out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?request=exercises&"
    << calcArgsNoPassExamDetails
    << "currentExamHome=" << CGI::StringToURLString(this->currentExamHomE) << "&" << "currentExamIntermediate=&"
    << "currentExamFile=" << CGI::StringToURLString(this->currentExamIntermediatE) << "&\">&nbspCurrent homework. </a><br>";
  else if (this->flagIsExamIntermediate)
    out << "<b>&nbspCurrent homework</b><br>";
  if (this->flagIsExamProblem)
  { if (theGlobalVariables.userCalculatorRequestType=="exercises")
    { out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?"
      << this->ToStringCalculatorArgumentsForProblem("examForReal")
      << "\">&nbsp&nbspStart exam for real</a><br>";
    } else
      out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?"
      << this->ToStringCalculatorArgumentsForProblem("exercises")
      << "\">&nbsp&nbspExercise this problem type</a><br>";
  }
  if (this->flagIsExamProblem && this->flagParentInvestigated)
  { int indexInParent=this->problemListOfParent.GetIndex(this->fileName);
    if (indexInParent==-1)
    { out << "<b>Could not find the problem collection that contains this problem.</b><br>";
      out << "<b>This is either a faulty link or a programming error. </b> "
      << "this->filename is: " << this->fileName << " and the problem list of the parent is: "
      << this->problemListOfParent.ToStringCommaDelimited();
    } else
    { if (indexInParent>0)
        out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?request="
        << theGlobalVariables.userCalculatorRequestType
        << "&" << calcArgsNoPassExamDetails
        << "&currentExamHome=" << CGI::StringToURLString(this->currentExamHomE)
        << "&currentExamIntermediate=" << CGI::StringToURLString(this->currentExamIntermediatE)
        << "&currentExamFile=" << CGI::StringToURLString(this->problemListOfParent[indexInParent-1] )
        << "&\"> <-Previous </a><br>";
      if (indexInParent<this->problemListOfParent.size-1)
        out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?request="
        << theGlobalVariables.userCalculatorRequestType
        << "&" << calcArgsNoPassExamDetails
        << "&currentExamHome=" << CGI::StringToURLString(this->currentExamHomE)
        << "&currentExamIntermediate=" << CGI::StringToURLString(this->currentExamIntermediatE)
        << "&currentExamFile=" << CGI::StringToURLString(this->problemListOfParent[indexInParent+1] )
        << "\"> Next-> </a><br>";
      else
        out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?request="
        << theGlobalVariables.userCalculatorRequestType
        << "&" << calcArgsNoPassExamDetails
        << "&currentExamHome=" << CGI::StringToURLString(this->currentExamHomE)
        << "&currentExamIntermediate="
        << "&currentExamFile=" << CGI::StringToURLString(this->currentExamHomE)
        << "&\">Last problem, return to course page.</a><br>";
    }
  }
  if (this->flagIsExamProblem && theGlobalVariables.userCalculatorRequestType=="exercises")
    out << "<hr><a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?"
    << this->ToStringCalculatorArgumentsForProblem("exercises")
    << "\">Link to this problem</a><br>";

  return out.str();
}

std::string CalculatorHTML::ToStringCalculatorArgumentsForProblem(const std::string& requestType)const
{ MacroRegisterFunctionWithName("WebWorker::ToStringCalculatorArgumentsForProblem");
  if (!theGlobalVariables.flagLoggedIn)
    return "";
  std::stringstream out;
  out << "request=" << requestType << "&" << theGlobalVariables.ToStringCalcArgsNoNavigation()
  << "currentExamHome=" << theGlobalVariables.GetWebInput("currentExamHome") << "&"
  << "currentExamIntermediate=" << theGlobalVariables.GetWebInput("currentExamIntermediate") << "&"
  << "currentExamFile=" << CGI::StringToURLString(this->fileName) << "&";
  if (theGlobalVariables.GetWebInput("randomSeed")=="" && theGlobalVariables.userCalculatorRequestType!="examForReal")
    out << "randomSeed=" << this->theProblemData.randomSeed << "&";
//  out << "currentExamFile=" << CGI::StringToURLString(this->fileName) << "&";
  return out.str();
}

std::string SyntacticElementHTML::GetTagClass()
{ return this->GetKeyValue("class");
}

std::string CalculatorHTML::ToStringClassDetails
( const List<List<std::string> >& userTable,
  const List<std::string>& labelsUserTable,
  bool adminsOnly,
  const SyntacticElementHTML& inputElement
)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringClassDetails");
  std::stringstream out;
#ifdef MACRO_use_MySQL
  std::string idOutput="outputAdd";
  if (adminsOnly)
    idOutput+="Admins";
  else
    idOutput+="Students";
  idOutput+= CGI::StringToURLString(this->fileName);
  std::string userRole = adminsOnly ? "admin" : "student";
  std::string idAddressTextarea= adminsOnly ? "inputAddAdmins" : "inputAddStudents";
  std::string idExtraTextarea= adminsOnly ? "inputAddAdminsExtraInfo" : "inputAddStudentsExtraInfo";
  if (inputElement.GetKeyValue("id")!="")
  { idAddressTextarea+= inputElement.GetKeyValue("id");
    idExtraTextarea+=inputElement.GetKeyValue("id");
  } else
  { idAddressTextarea+=this->fileName;
    idExtraTextarea+=this->fileName;
  }
  out << "Add <b>" << userRole << "(s)</b>.<br> ";
//  out << "<b>Warning: there's no remove button yet.</b><br>";
  out << "<textarea width=\"500px\" ";
  out << "id=\"" << idAddressTextarea << "\"";
  out << "placeholder=\"email or user list, comma, space or ; separated\">";
  out << "</textarea>";
  out << "<textarea width=\"500px\" ";
  out << "id=\"" << idExtraTextarea << "\"";
  out << " placeholder=\"optional, for sorting users in groups; for example section #\">";
  out << "</textarea>";
  out << "<br><button class=\"submitButton\" onclick=\"addEmails("
  << "'"    << idAddressTextarea
  << "', '" << CGI::StringToURLString(this->fileName)
  << "', '" << idOutput
  << "', '" << userRole
  << "', '" << idExtraTextarea
  << "')\"> Add emails</button>";
  out << "<br><span id=\"" << idOutput << "\"></span>\n<br>\n";
  int indexUser=-1, indexExtraInfo=-1;
  for (int i=0; i<labelsUserTable.size; i++)
  { if (labelsUserTable[i]=="username")
      indexUser=i;
    if (labelsUserTable[i]=="extraInfo")
      indexExtraInfo=i;
  }
  if (indexUser==-1 || indexExtraInfo==-1)
  { out << "<span style=\"color:red\"><b>This shouldn't happen: failed to find necessary "
    << "column entries in the database. "
    << "This is likely a software bug.</b></span>"
    << "indexUser, indexExtraInfo: "
    << indexUser << ", "
    << indexExtraInfo << ", "
    ;
    return out.str();
  }
  int numUsers=0;;
  std::stringstream tableStream;
  tableStream << "<table><tr><th>User</th><th>Email</th><th>Activated?</th><th>Activation link</th><th>Points</th></tr>";
  UserCalculator currentUser;
  currentUser.currentTable="users";
  DatabaseRoutines theRoutines;

  for (int i=0; i<userTable.size; i++)
  { std::stringstream failureStream;
    currentUser.username=userTable[i][indexUser];
    if (!currentUser.FetchOneUserRow(theRoutines, failureStream ))
    { currentUser.email=failureStream.str();
      currentUser.activationToken="error";
      currentUser.userRole="error";
    }
    if (adminsOnly xor (currentUser.userRole=="admin"))
      continue;
    numUsers++;
    tableStream << "<tr>"
    << "<td>" << userTable[i][indexUser] << "</td>"
    << "<td>" << currentUser.email.value << "</td>"
    ;
    if (currentUser.activationToken!="activated" && currentUser.activationToken!="error")
    { tableStream << "<td><span style=\"color:red\">not activated</span></td>";
      if (currentUser.activationToken!="")
        tableStream << "<td>"
        << "<a href=\""
        << UserCalculator::GetActivationAddressFromActivationToken(currentUser.activationToken.value, userTable[i][indexUser])
        << "\"> (Re)activate account and change password</a>"
        << "</td>";
      else
        tableStream << "<td>Activation token: " << currentUser.activationToken.value << "</td>";
    } else if (currentUser.activationToken=="error")
      tableStream << "<td>error</td><td></td>";
    else
    { tableStream << "<td><span style=\"color:green\">activated</span></td><td></td>";
/*      tableStream << "<td><span style=\"color:red\">"
      << "<a href=\""
      << UserCalculator::GetActivationAddressFromActivationToken(currentUser.activationToken.value, userTable[i][indexUser])
      << "\"> (Re)activate account and change password</a>"
      << "</span></td>";*/
    }
    int indexProblemData=currentUser.selectedRowFieldNamesUnsafe.GetIndex("problemData");
    std::stringstream commentsProblemData;
    if (indexProblemData==-1)
      tableStream << "<td>No solutions history</td>";
    else if ( currentUser.InterpretDatabaseProblemData
              (currentUser.selectedRowFieldsUnsafe[indexProblemData], commentsProblemData))
    { currentUser.ComputePointsEarned();
      tableStream << "<td>" << currentUser.pointsEarned << "</td>";
    } else
      tableStream << "<td>Failed to load problem data. Comments: " << commentsProblemData.str() << "</td>";
    tableStream << "</tr>";
  }
  tableStream << "</table>";
  out << "\n" << numUsers << " user(s). " << tableStream.str();
#else
  out << "<b>Adding emails not available (database not present).</b> ";
#endif // MACRO_use_MySQL

  return out.str();
}

void CalculatorHTML::InterpretManageClass(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretManageClass");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return;
#ifdef MACRO_use_MySQL
  DatabaseRoutines theRoutines;
  std::stringstream out;
  if (!theRoutines.startMySQLDatabaseIfNotAlreadyStarted(&out))
  { out << "<b>Failed to start database</b>";
    inputOutput.interpretedCommand=out.str();
    return;
  }
  std::string classTableName=DatabaseRoutines::GetTableUnsafeNameUsersOfFile(this->fileName);
  if (!theRoutines.TableExists(classTableName, &out))
    if (!theRoutines.CreateTable
        (classTableName, "username VARCHAR(255) NOT NULL PRIMARY KEY, \
        extraInfo LONGTEXT ", &out))
    { inputOutput.interpretedCommand=out.str();
      return;
    }
  List<List<std::string> > userTable;
  List<std::string> labelsUserTable;
  bool tableTruncated=false;
  int numRows=-1;
  std::stringstream failureComments;
  if (!DatabaseRoutinesGlobalFunctions::FetchTablE
      (userTable, labelsUserTable, tableTruncated, numRows,
       classTableName, failureComments))
  { out << "<span style=\"color:red\"><b>Failed to fetch table: "
    << classTableName
    << failureComments.str() << "</b></span>";
    inputOutput.interpretedCommand=out.str();
    return;
  }
  if (tableTruncated)
    out << "<span style=\"color:red\"><b>This shouldn't happen: email list truncated. "
    << "This is likely a software bug.</b></span>";
  out << "<hr><hr>";
  out << this->ToStringClassDetails(userTable, labelsUserTable, false, inputOutput);
  out << "<hr><hr>";
  out << this->ToStringClassDetails(userTable, labelsUserTable, true, inputOutput);
  inputOutput.interpretedCommand=out.str();
#else
  inputOutput.interpretedCommand="<b>Managing class not available (no database).</b>";
#endif // MACRO_use_MySQL
}

void CalculatorHTML::InterpretGenerateStudentAnswerButton(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretGenerateStudentAnswerButton");
  std::stringstream out;
  out << "<table><tr>";
  std::string answerId = inputOutput.GetKeyValue("id");
  if (answerId=="")
    out << "<td><b>Error: could not generate submit button: the answer area does not have a valid id</b></td>";
  else
  { std::string answerEvaluationId="verification"+inputOutput.GetKeyValue("id");
    std::stringstream previewAnswerStream;
    previewAnswerStream << "previewAnswers('" << answerId << "', '" << answerEvaluationId << "')";
    inputOutput.defaultKeysIfMissing.AddOnTop("onkeypress");
    inputOutput.defaultValuesIfMissing.AddOnTop(previewAnswerStream.str());
    inputOutput.defaultKeysIfMissing.AddOnTop("style");
    inputOutput.defaultValuesIfMissing.AddOnTop("height:70px");
    out << "<td>" << inputOutput.ToStringOpenTag() << inputOutput.ToStringCloseTag() << "</td>";
    out << "<td><button class=\"submitButton\" onclick=\"submitAnswers('"
    << answerId << "', '" << answerEvaluationId << "')\"> Submit </button></td>"
    << "<td>";
    out << "<span id=\"" << answerEvaluationId << "\">";
    int theIndex=this->theProblemData.answerIds.GetIndex(answerId);
    if (theIndex==-1)
      crash << "Index of answer id not found: this shouldn't happen. " << crash;
    int numCorrectSubmissions=0;
    int numSubmissions= 0;
    if (theIndex >=this->theProblemData.numCorrectSubmissions.size ||
        theIndex >=this->theProblemData.numSubmissions.size ||
        theIndex >=this->theProblemData.firstCorrectAnswer.size)
      out << "<b>Error: the problemData index  is " << theIndex << " but "
      << "  numCorrectSubmissions.size is: " << this->theProblemData.numCorrectSubmissions.size
      << ", numSubmissions.size is: " << this->theProblemData.numSubmissions.size
      << ", firstCorrectAnswer.size is: " << this->theProblemData.firstCorrectAnswer.size
      << ". </b>";
    if (theIndex!=-1)
    { numCorrectSubmissions= this->theProblemData.numCorrectSubmissions[theIndex];
      numSubmissions= this->theProblemData.numSubmissions[theIndex];
    }
//    stOutput << "got to here 2";
    if (numCorrectSubmissions >0)
    { out << "<b><span style=\"color:green\">Correctly answered: \\("
      << this->theProblemData.firstCorrectAnswer[theIndex] << "\\) </span></b> ";
      if (numSubmissions>0)
        out << "<br>Used: " << numSubmissions << " attempt(s) (" << numCorrectSubmissions << " correct).";
    } else
    { if (theGlobalVariables.userCalculatorRequestType=="examForReal")
      { out << " <b><span style=\"color:brown\">Need to submit answer. </span></b>";
        if (numSubmissions>0)
          out << numSubmissions << " attempt(s) so far. ";
      } else
        out << " <b><span style=\"color:brown\">Submit (no credit, unlimited tries)</span></b>";
    }
    out << "</span></td>";
  }
  out << "</tr></table>";
  inputOutput.interpretedCommand=out.str();
}

void CalculatorHTML::InterpretNotByCalculator(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretNotByCalculator");
  std::string tagClass=inputOutput.GetTagClass();
  if (tagClass=="calculatorExamProblem" || tagClass=="calculatorExamIntermediate")
    this->InterpretGenerateLink(inputOutput);
  else if (tagClass=="calculatorAnswer")
    this->InterpretGenerateStudentAnswerButton(inputOutput);
  else if (tagClass=="calculatorManageClass")
    this->InterpretManageClass(inputOutput);
}

std::string CalculatorHTML::CleanUpLink(const std::string& inputLink)
{ std::string result;
  result.reserve(inputLink.size());
  for (unsigned i=0; i<inputLink.size(); i++)
    if (inputLink[i]!='\n' && inputLink[i]!='\r' && inputLink[i]!='\t' && inputLink[i]!=' ')
      result.push_back(inputLink[i]);
  return result;
}

void CalculatorHTML::InterpretGenerateLink(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretGenerateLink");
  this->NumProblemsFound++;
  std::string cleaneduplink = this->CleanUpLink(inputOutput.content);
  std::stringstream out, refStreamNoRequest, refStreamExercise, refStreamForReal;
//  out << "cleaned up link: " << cleaneduplink;
//  out << "<br>urled link: " <<  CGI::StringToURLString(cleaneduplink);
  refStreamNoRequest << theGlobalVariables.ToStringCalcArgsNoNavigation()
  << "currentExamFile=" << CGI::StringToURLString(cleaneduplink) << "&";
  if (this->flagIsExamHome || this->flagIsExamIntermediate)
    refStreamNoRequest << "currentExamHome=" << theGlobalVariables.GetWebInput("currentExamHome") << "&";
  if (this->flagIsExamIntermediate)
    refStreamNoRequest << "currentExamIntermediate=" << theGlobalVariables.GetWebInput("currentExamIntermediate") << "&";
  refStreamExercise << theGlobalVariables.DisplayNameCalculatorWithPath << "?request=exercises&" << refStreamNoRequest.str();
  refStreamForReal << theGlobalVariables.DisplayNameCalculatorWithPath << "?request=examForReal&" << refStreamNoRequest.str();
  if (inputOutput.GetTagClass()=="calculatorExamProblem")
  { out << " <a href=\"" << refStreamForReal.str() << "\">Start (for credit)</a> ";
    out << " <a href=\"" << refStreamExercise.str() << "\">Exercise (no credit, unlimited tries)</a> ";
  } else
    out << " <a href=\"" << refStreamExercise.str() << "\">Start</a> ";
  std::string stringToDisplay=  FileOperations::GetFileNameFromFileNameWithPath(inputOutput.content);
  //out << " " << this->NumProblemsFound << ". "
  out << stringToDisplay;
  inputOutput.interpretedCommand=out.str();
}

bool CalculatorHTML::InterpretHtmlOneAttempt(Calculator& theInterpreter, std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretHtmlOneAttempt");
  std::stringstream out;
  if (!this->flagIsForReal || !this->theProblemData.flagRandomSeedComputed)
    if (!this->flagRandomSeedGiven)
      this->theProblemData.randomSeed=this->randomSeedsIfInterpretationFails[this->NumAttemptsToInterpret-1];
  this->FigureOutCurrentProblemList(comments);
  this->outputHtmlNavigation=this->ToStringProblemNavigation();
  if (this->flagIsExamProblem)
    out << this->GetSubmitAnswersJavascript();
  else if (this->flagIsExamHome)
    out << this->GetSubmitEmailsJavascript();
  if (!this->PrepareAndExecuteCommands(theInterpreter, comments))
    return false;
  bool moreThanOneCommand=false;
  std::string lastCommands;
  int commandCounter=2;
//  out << "DEBUG nfo, remove when done. randseed: " << this->randomSeed;
  //first command and first syntactic element are the random seed and are ignored.
  for (int spanCounter=1; spanCounter <this->theContent.size; spanCounter++)
  { SyntacticElementHTML& currentElt=this->theContent[spanCounter];
    if (!currentElt.IsInterpretedByCalculatorDuringPreparation())
    { if (theInterpreter.theProgramExpression[commandCounter].ToString()!="SeparatorBetweenSpans")
      { out << "<b>Error: calculator commands don't match the tags.</g>";
        this->outputHtmlMain=out.str();
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
//  stOutput << "got to here, this->theProblemData: " << this->theProblemData.ToString();
  for (int i=0; i<this->theContent.size; i++)
    if (this->theContent[i].IsInterpretedNotByCalculator())
      this->InterpretNotByCalculator(this->theContent[i]);
  for (int i=0; i<this->theContent.size; i++)
    out << this->theContent[i].ToStringInterpretted();
//   out << "<hr><hr><hr><hr><hr><hr><hr><hr><hr>The calculator activity:<br>" << theInterpreter.outputString << "<hr>";
//   out << "<hr>" << this->ToStringExtractedCommands() << "<hr>";
  //  out << "<hr> Between the commands:" << this->betweenTheCommands.ToStringCommaDelimited();
#ifdef MACRO_use_MySQL
  if (this->flagIsForReal)
  { this->theProblemData.flagRandomSeedComputed=true;
    UserCalculator theUser;
    DatabaseRoutines theRoutines;
    theUser.username=theGlobalVariables.userDefault;
//    stOutput << "About to store problem data: " << this->theProblemData.ToString();
    if (!theUser.InterpretDatabaseProblemData(this->currentUserDatabaseString, comments))
      out << "<b>Error: corrupt database string. </b>";
    else
      theUser.SetProblemData(this->fileName, this->theProblemData);
    if (!theUser.StoreProblemDataToDatabase(theRoutines, comments))
        out << "<b>Error: failed to store problem in database. </b>" << comments.str();
  }
#endif // MACRO_use_MySQL
  this->outputHtmlMain=out.str();
  return true;
}

void CalculatorHTML::FigureOutCurrentProblemList(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::FigureOutCurrentProblemList");
  if (this->flagParentInvestigated)
    return;
  this->flagParentInvestigated=true;
  this->currentExamHomE=  CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamHome"));
  this->currentExamIntermediatE=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamIntermediate"));
  if (!this->flagIsExamProblem)
    return;
  CalculatorHTML parserOfParent;
  parserOfParent.fileName=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamIntermediate"));
  std::stringstream commentsOfparent;
  if (!parserOfParent.LoadMe(false, commentsOfparent))
  { comments << "Failed to load parent problem collection. Comments: " << commentsOfparent.str();
    return;
  }
  if (!parserOfParent.ParseHTML(commentsOfparent))
  { comments << "Failed to parse parent problem collection. Comments: " << commentsOfparent.str();
    return;
  }
  this->problemListOfParent=parserOfParent.problemList;
}

std::string CalculatorHTML::GetDatabaseTableName()
{ std::string result="grades"+ CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamHome"));
//  stOutput << "GetDatabaseTableName result: " << result << "<br>";
  return result;
}

bool CalculatorHTML::InterpretHtml(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretHtml");
  if (!this->ParseHTML(comments))
  { this->outputHtmlMain="<b>Failed to interpret html input. </b><br>" +this->ToStringContent();
    return false;
  }
  this->NumAttemptsToInterpret=0;
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
      this->outputHtmlMain=out.str();
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
  this->calculatorClasses.AddOnTop("calculatorAnswer");
  this->calculatorClasses.AddOnTop("calculatorExamIntermediate");
  this->calculatorClasses.AddOnTop("calculatorExamProblem");
  this->calculatorClasses.AddOnTop("calculatorManageClass");
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
    SyntacticElementHTML& last         = eltsStack[eltsStack.size-1];
    SyntacticElementHTML& secondToLast = eltsStack[eltsStack.size-2];
    SyntacticElementHTML& thirdToLast  = eltsStack[eltsStack.size-3];
    SyntacticElementHTML& fourthToLast = eltsStack[eltsStack.size-4];
    SyntacticElementHTML& fifthToLast  = eltsStack[eltsStack.size-5];
    SyntacticElementHTML& sixthToLast  = eltsStack[eltsStack.size-6];
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
  this->problemList.SetSize(0);
  for (int i=0; i<this->theContent.size; i++)
    if (this->theContent[i].GetTagClass()=="calculatorExamProblem")
      this->problemList.AddOnTop(this->CleanUpLink(this->theContent[i].content));
  if (result)
    result=this->ExtractAnswerIds(comments);
  if (result)
    result=this->CheckContent(comments);
  return result;
}

bool CalculatorHTML::ExtractAnswerIds(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::ExtractAnswerIds");
  //we shouldn't touch this->theProblemData.answerIds.SetSize: it may contain
  //outdated information loaded from the database. We don't want to loose that info
  //(say we renamed an answerId but students have already stored answers using the old answerId...).
  this->answerVerificationCommand.SetSize(this->theProblemData.answerIds.size);
  List<std::string> answerIdsInTheFile;
  for (int i=0; i<this->theContent.size; i++)
  { if (!this->theContent[i].IsAnswer())
      continue;
    std::string newId=this->theContent[i].GetKeyValue("id");
    if (newId=="")
    { comments << "<b>Error: answer tag has no id</b>";
      return false;
    }
    if (answerIdsInTheFile.Contains(newId))
    { comments << "<b>Error: answer tag id: " << newId << " contained more than once</b>";
      return false;
    }
    answerIdsInTheFile.AddOnTop(newId);
    if (this->theProblemData.answerIds.Contains(newId))
      this->answerVerificationCommand[this->theProblemData.answerIds.GetIndex(newId)]=
      this->theContent[i].content;
    else
    { this->theProblemData.AddEmptyAnswerIdOnTop(newId);
      this->answerVerificationCommand.AddOnTop(this->theContent[i].content);
    }
  }
  return true;
}

bool CalculatorHTML::CheckContent(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::CheckContent");
  bool result=true;
  for (int i=0; i<this->theContent.size; i++)
  { SyntacticElementHTML& currentElt=this->theContent[i];
    if (currentElt.syntacticRole=="command" && currentElt.IsAnswer() &&
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
