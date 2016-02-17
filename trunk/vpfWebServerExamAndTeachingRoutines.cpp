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
  std::string ToStringInterpreted();
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
  bool flagParentIsIntermediateExamFile;
#ifdef MACRO_use_MySQL
  UserCalculator currentUser;
#endif
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
  List<std::string> hdProblemList;
  List<List<std::string> > hdHomeworkGroups;
  HashedList<std::string, MathRoutines::hashString> databaseProblemList;
  List<std::string> databaseHomeworkGroups;
  List<std::string> databaseProblemWeights;
  List<List<std::string> > databaseStudentSections;
  List<List<std::string> > databaseDeadlinesBySection;
  List<List<std::string> > userTablE;
  List<std::string> labelsUserTablE;
  List<std::string> problemListOfParent;
  List<std::string> classSections;
  std::string currentUserDatabaseString;
  std::string currentProblemCollectionDatabaseString;
  bool flagLoadedSuccessfully;
  bool flagLoadedClassDataSuccessfully;
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
  std::string InterpretGenerateDeadlineLink
(SyntacticElementHTML& inputOutput, std::stringstream& refStreamForReal, std::stringstream& refStreamExercise,
 const std::string& cleaneduplink, const std::string& urledProblem)
  ;
  void InterpretGenerateLink(SyntacticElementHTML& inputOutput);
  std::string InterpretGenerateProblemManagementLink
(SyntacticElementHTML& inputOutput, std::stringstream& refStreamForReal, std::stringstream& refStreamExercise,
 const std::string& cleaneduplink, const std::string& urledProblem)
  ;
  void InterpretGenerateStudentAnswerButton(SyntacticElementHTML& inputOutput);
  bool PrepareClassData(std::stringstream& commentsOnFailure);
  void InterpretManageClass(SyntacticElementHTML& inputOutput);
  std::string ToStringClassDetails
( bool adminsOnly,
  const SyntacticElementHTML& inputElement
)
  ;
  std::string GetSubmitStringAreaAsMainInput();
  std::string GetSubmitEmailsJavascript();
  std::string GetDatePickerJavascriptInit();
  std::string GetDatePickerStart(const std::string& theId);
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
  this->flagParentIsIntermediateExamFile=false;
  this->flagLoadedClassDataSuccessfully=false;
}

const std::string CalculatorHTML::RelativePhysicalFolderProblemCollections="ProblemCollections/";

#ifdef MACRO_use_MySQL
bool DatabaseRoutines::ReadProblemInfo
  (const std::string& stringToReadFrom, HashedList<std::string, MathRoutines::hashString>& outputProblemNames,
   List<std::string>& outputHomeworkGroups,
   List<std::string>& outputWeights, List<List<std::string> >& outputDeadlinesPerSection,
   List<List<std::string> >& outputSections, std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutines::ReadProblemInfo");
  HashedList<std::string, MathRoutines::hashString> problemNamesCGIed;
  List<std::string> theProblemData;
  if (!CGI::ChopCGIInputStringToMultipleStrings
      (stringToReadFrom, theProblemData, problemNamesCGIed, commentsOnFailure) )
    return false;
  outputProblemNames.Clear();
  outputProblemNames.SetExpectedSize(problemNamesCGIed.size);
  for (int i=0; i<problemNamesCGIed.size; i++)
    outputProblemNames.AddOnTop(CGI::URLStringToNormal(problemNamesCGIed[i]));
  HashedList<std::string, MathRoutines::hashString> problemKeys, sectionKeys;
  List<std::string> problemValues, sectionValues;
  outputHomeworkGroups.initFillInObject(outputProblemNames.size, "");
  outputWeights.initFillInObject(outputProblemNames.size, "");
  outputSections.initFillInObject(outputProblemNames.size, problemValues);
  outputDeadlinesPerSection.initFillInObject(outputProblemNames.size, problemValues);
  for (int i=0; i<outputProblemNames.size; i++)
  { if (!CGI::ChopCGIInputStringToMultipleStrings
        (CGI::URLStringToNormal(theProblemData[i]), problemValues, problemKeys, commentsOnFailure))
      return false;
//    stOutput << "<br>current problem: " << outputProblemNames[i]
//    << "<br>being read from data: " << theProblemData[i]
//    << "<br>which is normalized to: " << CGI::URLStringToNormal(theProblemData[i]);
    if (problemKeys.Contains("weight"))
      outputWeights[i]=
      CGI::URLStringToNormal(problemValues[problemKeys.GetIndex("weight")]);
    if (!problemKeys.Contains("deadlines"))
      continue;
    std::string deadlineString=CGI::URLStringToNormal(problemValues[problemKeys.GetIndex("deadlines")]);
    if (!CGI::ChopCGIInputStringToMultipleStrings
        (deadlineString, sectionValues, sectionKeys, commentsOnFailure))
      return false;
    for (int j=0; j<sectionKeys.size; j++)
    { outputSections[i].AddOnTop(CGI::URLStringToNormal(sectionKeys[j]));
      outputDeadlinesPerSection[i].AddOnTop(CGI::URLStringToNormal(sectionValues[j]));
    }
  }
  return true;
}

void DatabaseRoutines::StoreProblemInfo
  (std::string& outputString, const HashedList<std::string, MathRoutines::hashString>& inputProblemNames,
   const List<std::string>& inputHomeworkGroups,
   const List<std::string>& inputWeights, const List<List<std::string> >& inputSections,
   const List<List<std::string> >& inputDeadlines)
{ MacroRegisterFunctionWithName("DatabaseRoutines::StoreProblemInfo");
  if (inputProblemNames.size!=inputWeights.size ||
      inputProblemNames.size!=inputDeadlines.size ||
      inputProblemNames.size!=inputSections.size
      )
    crash << "This shouldn't happen: non-matching data sizes while storing problem info. "
    << "The present function should only be called with sanitized input. " << crash;
  std::stringstream out;
  for (int i=0; i<inputProblemNames.size; i++)
  { std::stringstream currentProblemStream, currentDeadlineStream;
    currentProblemStream << "weight=" << CGI::StringToURLString(inputWeights[i]) << "&";
    for (int j=0; j<inputSections[i].size; j++)
    { if (inputSections[j].size!=inputDeadlines[j].size)
        crash << "Input sections and input deadlines have mismatching sizes. " << crash;
      currentDeadlineStream << CGI::StringToURLString(inputSections[i][j])
      << "=" << CGI::StringToURLString(inputDeadlines[i][j]) << "&";
    }
    currentProblemStream << "deadlines=" << CGI::StringToURLString(currentDeadlineStream.str()) << "&";
    out << CGI::StringToURLString(inputProblemNames[i]) << "="
    << CGI::StringToURLString(currentProblemStream.str()) << "&";
  }
  outputString=out.str();
}

bool DatabaseRoutines::ReadProblemDatabaseInfo
(const std::string& problemHomeName, std::string& outputString,
  std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutines::ReadProblemDatabaseInfo");
  if (!this->startMySQLDatabaseIfNotAlreadyStarted(&commentsOnFailure))
    return false;
  if (!this->TableExists("problemData", &commentsOnFailure))
    if (!this->CreateTable("problemData", "problemCollection VARCHAR(255) NOT NULL PRIMARY KEY, \
        problemInformation LONGTEXT", &commentsOnFailure))
      return false;
  if (!this->RowExists
      ((std::string) "problemCollection", problemHomeName,
       (std::string) "problemData", &commentsOnFailure))
    if (!this->SetEntry
        ((std::string) "problemCollection", problemHomeName,
         (std::string) "problemData", (std::string) "problemInformation",
         (std::string) "", &commentsOnFailure))
      return false;
//  stOutput << " ... got to here ...";
  return this->FetchEntry
  ((std::string) "problemCollection", problemHomeName, (std::string) "problemData",
   (std::string) "problemInformation", outputString, &commentsOnFailure);
}

bool DatabaseRoutines::StoreProblemDatabaseInfo
  (const std::string& problemHomeName, const std::string& inputString,
   std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutines::StoreProblemDatabaseInfo");
  if (!this->startMySQLDatabaseIfNotAlreadyStarted(&commentsOnFailure))
    return false;
  if (!this->TableExists("problemData", &commentsOnFailure))
    if (!this->CreateTable("problemData", "problemCollection VARCHAR(255) NOT NULL PRIMARY KEY, \
        problemInformation LONGTEXT", &commentsOnFailure))
      return false;
  if (!this->RowExists
      ((std::string) "problemCollection", problemHomeName,
       (std::string) "problemData", &commentsOnFailure))
    if (!this->SetEntry
        ((std::string) "problemCollection", problemHomeName,
         (std::string) "problemData", (std::string) "problemInformation",
         (std::string) "", &commentsOnFailure))
      return false;
  return this->SetEntry
  ((std::string) "problemCollection", problemHomeName, (std::string) "problemData",
   (std::string) "problemInformation", inputString, &commentsOnFailure);
}

bool DatabaseRoutines::MergeProblemInfoInDatabase
(const std::string& problemHomeName, std::string& inputString,
  std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutines::MergeProblemInfoInDatabase");
  std::string storedDatabaseInfo;
  if (!this->ReadProblemDatabaseInfo(problemHomeName, storedDatabaseInfo, commentsOnFailure))
  { commentsOnFailure << "Failed to read database for problem collection: "
    << problemHomeName;
    return false;
  }
//  stOutput << "stored db info: <br>" << storedDatabaseInfo;
  HashedList<std::string, MathRoutines::hashString> theProblems;
  List<std::string> theProblemWeights;
  List<std::string> theProblemGroups;
  List<std::string> theHomeworkGroups;
  List<List<std::string> > theSections;
  List<List<std::string> > theDeadlines;
  if (!this->ReadProblemInfo
      (storedDatabaseInfo, theProblems, theHomeworkGroups, theProblemWeights, theSections, theDeadlines, commentsOnFailure))
  { commentsOnFailure << "Failed to parse stored database info. ";
    return false;
  }
//  stOutput << "data extracted from stored db nfo:<br>"
//  << "Problems: " << theProblems.ToStringCommaDelimited() << "<br>"
//  << "theProblemWeights: " << theProblemWeights.ToStringCommaDelimited() << "<br>";

  HashedList<std::string, MathRoutines::hashString> incomingProblems;
  List<std::string> incomingWeights;
  List<std::string> incomingHomeworkGroups;
  List<List<std::string> > incomingSections;
  List<List<std::string> > incomingDeadlines;
  if (!this->ReadProblemInfo
      (inputString, incomingProblems, incomingHomeworkGroups, incomingWeights, incomingSections, incomingDeadlines, commentsOnFailure))
  { commentsOnFailure << "Failed to parse your request";
    return false;
  }
  std::string currentFileName;
  bool result=true;
  for (int i=0; i<incomingProblems.size; i++)
  { currentFileName=theGlobalVariables.PhysicalPathProjectBase+
    CalculatorHTML::RelativePhysicalFolderProblemCollections+ incomingProblems[i];
    if (!FileOperations::FileExistsUnsecure(currentFileName))
    { result=false;
      commentsOnFailure << "Could not find problem: " << incomingProblems[i] << ". ";
      continue;
    }
    int theIndex=theProblems.GetIndex(incomingProblems[i]);
    if (theIndex==-1)
    { theProblems.AddOnTop(incomingProblems[i]);
      theProblemWeights.AddOnTop(incomingWeights[i]);
      theSections.AddOnTop(incomingSections[i]);
      theDeadlines.AddOnTop(incomingDeadlines[i]);
      theHomeworkGroups.AddOnTop(incomingHomeworkGroups[i]);
      continue;
    }
    if (incomingWeights[i]!="")
      theProblemWeights[theIndex]=incomingWeights[i];
    if (incomingHomeworkGroups[i]!="")
      theHomeworkGroups[theIndex]=incomingHomeworkGroups[i];
    if (incomingSections[i].size!=0)
      for (int j=0; j<incomingSections.size; j++)
      { int sectionIndex=theSections[theIndex].GetIndex(incomingSections[i][j]);
        if (sectionIndex==-1)
        { theSections[theIndex].AddOnTop(incomingSections[i][j]);
          theDeadlines[theIndex].AddOnTop(incomingDeadlines[i][j]);
          continue;
        }
        theSections[theIndex][sectionIndex]=incomingSections[i][j];
        theDeadlines[theIndex][sectionIndex]=incomingDeadlines[i][j];
      }
  }
  std::string stringToStore;

  this->StoreProblemInfo(stringToStore, theProblems, theHomeworkGroups, theProblemWeights, theSections, theDeadlines);
//  stOutput << "<br>about to store back : <br>" << stringToStore;
  if (!this->StoreProblemDatabaseInfo(problemHomeName, stringToStore, commentsOnFailure))
    return false;
//  stOutput << "<br>probs incoming: <br>" << incomingProblems.ToStringCommaDelimited()
//  << " with weights: " << incomingWeights.ToStringCommaDelimited()
//  << "Problems final: " << theProblems.ToStringCommaDelimited()
//  << " weights: " << theProblemWeights.ToStringCommaDelimited() << "<br>";
  return result;
}
#endif // MACRO_use_MySQL

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
//stOutput << "loading: ";
  DatabaseRoutines theRoutines;
  if (doLoadDatabase)
  { //stOutput << " accessing db... ";
    this->currentUser.username=theGlobalVariables.userDefault;
    if (!this->currentUser.LoadProblemStringFromDatabase(theRoutines, this->currentUserDatabaseString, comments))
    { comments << "Failed to load current users' problem save-file.";
      return false;
    }
    if (!this->currentUser.InterpretDatabaseProblemData(this->currentUserDatabaseString, comments))
    { comments << "Failed to load current users' problem save-file.";
      return false;
    }
    this->theProblemData=this->currentUser.GetProblemDataAddIfNotPresent(this->fileName);
  }
  this->currentExamHomE=  CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamHome"));
  if (doLoadDatabase && this->currentExamHomE!="")
  { //stOutput << "loading db, problem collection: " << this->currentExamHomE;
    if (!theRoutines.ReadProblemDatabaseInfo
        (this->currentExamHomE, this->currentProblemCollectionDatabaseString, comments))
    { comments << "Failed to load current problem collection's database string. ";
      return false;
    }
    //stOutput << ", the db string is: " << this->currentProblemCollectionDatabaseString;
    if (!theRoutines.ReadProblemInfo
        (this->currentProblemCollectionDatabaseString, this->databaseProblemList,
         this->databaseHomeworkGroups,
         this->databaseProblemWeights,
         this->databaseStudentSections, this->databaseDeadlinesBySection, comments))
    { comments << "Failed to interpret the database problem string. ";
      return false;
    }
    this->currentUser.ComputePointsEarned
    (this->databaseProblemList, this->databaseProblemWeights);
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
    theGlobalVariables.SetWebInput("currentExamHome", CGI::StringToURLString(this->fileName));
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

std::string CalculatorHTML::GetSubmitStringAreaAsMainInput()
{ std::stringstream out;
  out
  << "<script type=\"text/javascript\"> \n"
  << "function submitStringAsMainInput(theString, idOutput, requestType){\n"
  << "  spanOutput = document.getElementById(idOutput);\n"
  << "  if (spanOutput==null){\n"
  << "    spanOutput = document.createElement('span');\n"
  << "    document.body.appendChild(spanOutput);\n"
  << "    spanOutput.innerHTML= \"<span style='color:red'> ERROR: span with id \" + idOutput + \"MISSING! </span>\";\n"
  << "  }\n"
  << "  inputParams='request='+requestType;\n"
  << "  inputParams+='&" << theGlobalVariables.ToStringCalcArgsNoNavigation() << "';\n"
  << "  inputParams+='&currentExamHome=" << CGI::StringToURLString(this->fileName) << "';\n"
  << "  inputParams+='&mainInput=' + encodeURIComponent(theString);\n"
//  << "  inputParams+='&currentExamHome=' + problemCollectionName;\n"
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

std::string CalculatorHTML::GetDatePickerStart(const std::string& theId)
{ std::stringstream out;
  out << "\n<script type=\"text/javascript\">\n"
  << "$(function() {\n"
  << "$('#" << theId << "').datepicker();\n"
  << "});\n"
  << "</script>\n";
  return out.str();
}

std::string CalculatorHTML::GetDatePickerJavascriptInit()
{ std::stringstream out;
  out
  << "<link rel=\"stylesheet\" href=\"//code.jquery.com/ui/1.11.4/themes/smoothness/jquery-ui.css\">\n"
  << "<script src=\"//code.jquery.com/jquery-1.10.2.js\"></script>\n"
  << "<script src=\"//code.jquery.com/ui/1.11.4/jquery-ui.js\"></script>\n"
  ;
  return out.str();
}

std::string CalculatorHTML::GetSubmitEmailsJavascript()
{ std::stringstream out;
  out
  << "<script type=\"text/javascript\" id=\"scriptSubmitEmails\"> \n"
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
//  << "    code=document.getElementById('progressReportJavascript').innerHTML;"
//  << "    eval.call(code);\n"
  //<< "    p();\n"
//  << "    eval(spanOutput.innerHTML);\n"
//  << "    if (typeof progressReport == 'function')\n"
//  << "      progressReport();\n"
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

int WebWorker::ProcessSetProblemDatabaseInfo()
{ MacroRegisterFunctionWithName("WebWorker::ProcessSetProblemDatabaseInfo");
  stOutput << this->GetSetProblemDatabaseInfoHtml();
  return 0;
}

int WebWorker::ProcessAddUserEmails()
{ MacroRegisterFunctionWithName("WebWorker::ProcessAddUserEmails");
  stOutput << this->GetAddUserEmails();
  stOutput.Flush();
  return 0;
}

std::string WebWorker::GetSetProblemDatabaseInfoHtml()
{ MacroRegisterFunctionWithName("WebWorker::GetSetProblemDatabaseInfoHtml");
#ifdef MACRO_use_MySQL
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return "<b>Only admins may set problem weights.</b>";
  std::string inputProblemInfo=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("mainInput"));
  std::string inputProblemHome= CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamHome"));
  DatabaseRoutines theRoutines;
  std::stringstream commentsOnFailure;
  bool result=theRoutines.MergeProblemInfoInDatabase(inputProblemHome, inputProblemInfo, commentsOnFailure);
  std::stringstream out;
  if (result)
    out << "<span style=\"color:green\"><b>Successfully changed problem data.</b></span>";
  else
    out << "<span style=\"color:red\"><b>" << commentsOnFailure.str() << "</b></span>";
  out << "<br>Debug message:<br>inputProblemInfo: " << inputProblemInfo << "<br>inputProblemHome: " << inputProblemHome;
  return out.str();
#else
  return "Cannot modify problem weights (no database available)";
#endif // MACRO_use_MySQL
}

std::string WebWorker::GetAddUserEmails()
{ MacroRegisterFunctionWithName("WebWorker::GetAddUserEmails");
  std::stringstream out;
  if (!theGlobalVariables.UserDefaultHasAdminRights())
  { out << "<b>Only admins may add users.</b>";
    return out.str();
  }
  std::string inputEmails, extraInfo;
  inputEmails=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("mainInput"));
  extraInfo= CGI::URLStringToNormal(theGlobalVariables.GetWebInput("extraInfo"));
  if (extraInfo=="")
    extraInfo="default";
  if (inputEmails=="")
  { out << "<b>No emails to add</b>";
    return out.str();
  }
#ifdef MACRO_use_MySQL
  DatabaseRoutines theRoutines;
  std::stringstream comments;
  bool sentEmails=false;
  bool createdUsers=theRoutines.AddUsersFromEmails(inputEmails, extraInfo, sentEmails, comments);
  if (createdUsers)
    out << "<span style=\"color:green\">Users successfully added. </span>";
  else
    out << "<span style=\"color:red\">Failed to add all users. </span>";
  if (sentEmails)
    out << "<span style=\"color:green\">Activation emails successfully sent. </span>";
  else
    out << "<span style=\"color:red\">Failed to sent all activation emails. </span>";
  if (!createdUsers || !sentEmails)
    out << "<br>Comments:<br>" << comments.str();
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
#ifdef MACRO_use_MySQL
  int correctSubmissionsRelevant=0;
  int totalSubmissionsRelevant=0;
  DatabaseRoutines theRoutines;
  theProblem.currentUser.username=theGlobalVariables.userDefault;
  if (theProblem.flagIsForReal)
  { //stOutput << "<hr>problem string: " << theProblem.currentUserDatabaseString << "<hr>";
    if (!theProblem.currentUser.InterpretDatabaseProblemData(theProblem.currentUserDatabaseString, comments))
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
#endif // MACRO_use_MySQL
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
    theProblem.currentUser.SetProblemData(theProblem.fileName, theProblem.theProblemData);
    if (!theProblem.currentUser.StoreProblemDataToDatabase(theRoutines, comments))
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

std::string SyntacticElementHTML::ToStringInterpreted()
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
    << "currentExamHome=" << CGI::StringToURLString(this->currentExamHomE) << "&" << "currentExamIntermediate=&"
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
( bool adminsOnly,
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
  for (int i=0; i<this->labelsUserTablE.size; i++)
  { if (this->labelsUserTablE[i]=="username")
      indexUser=i;
    if (this->labelsUserTablE[i]=="extraInfo")
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
  int numUsers=0;
  std::stringstream tableStream;
  tableStream << "<table><tr><th>User</th><th>Email</th><th>Activated?</th><th>Activation link</th><th>Points</th><th>Section/Group</th></tr>";
  UserCalculator currentUser;
  currentUser.currentTable="users";
  DatabaseRoutines theRoutines;

  for (int i=0; i<this->userTablE.size; i++)
  { std::stringstream failureStream;
    currentUser.username=this->userTablE[i][indexUser];
    if (!currentUser.FetchOneUserRow(theRoutines, failureStream ))
    { currentUser.email=failureStream.str();
      currentUser.activationToken="error";
      currentUser.userRole="error";
    }
    if (adminsOnly xor (currentUser.userRole=="admin"))
      continue;
    numUsers++;
    tableStream << "<tr>"
    << "<td>" << this->userTablE[i][indexUser] << "</td>"
    << "<td>" << currentUser.email.value << "</td>"
    ;
    if (currentUser.activationToken!="activated" && currentUser.activationToken!="error")
    { tableStream << "<td><span style=\"color:red\">not activated</span></td>";
      if (currentUser.activationToken!="")
        tableStream << "<td>"
        << "<a href=\""
        << UserCalculator::GetActivationAddressFromActivationToken
        (currentUser.activationToken.value, this->userTablE[i][indexUser])
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
      << UserCalculator::GetActivationAddressFromActivationToken(currentUser.activationToken.value, this->userTablE[i][indexUser])
      << "\"> (Re)activate account and change password</a>"
      << "</span></td>";*/
    }
    int indexProblemData=currentUser.selectedRowFieldNamesUnsafe.GetIndex("problemData");
    std::stringstream commentsProblemData;
    if (indexProblemData==-1)
      tableStream << "<td>No solutions history</td>";
    else if (currentUser.InterpretDatabaseProblemData
             (currentUser.selectedRowFieldsUnsafe[indexProblemData], commentsProblemData))
    { currentUser.ComputePointsEarned(this->databaseProblemList, this->databaseProblemWeights);
      tableStream << "<td>" << currentUser.pointsEarned << "</td>";
    } else
      tableStream << "<td>Failed to load problem data. Comments: " << commentsProblemData.str() << "</td>";
    tableStream << "<td>" << this->userTablE[i][indexExtraInfo] << "</td>";
    tableStream << "</tr>";
  }
  tableStream << "</table>";
  out << "\n" << numUsers << " user(s). " << tableStream.str();
#else
  out << "<b>Adding emails not available (database not present).</b> ";
#endif // MACRO_use_MySQL

  return out.str();
}

bool CalculatorHTML::PrepareClassData(std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareClassData");
  DatabaseRoutines theRoutines;
  if (!theRoutines.startMySQLDatabaseIfNotAlreadyStarted(&commentsOnFailure))
  { commentsOnFailure << "<b>Failed to start database</b>";
    return false;
  }
  std::string classTableName=DatabaseRoutines::GetTableUnsafeNameUsersOfFile(this->fileName);
  if (!theRoutines.TableExists(classTableName, &commentsOnFailure))
    if (!theRoutines.CreateTable
        (classTableName, "username VARCHAR(255) NOT NULL PRIMARY KEY, \
        extraInfo LONGTEXT ", &commentsOnFailure))
      return false;
  bool tableTruncated=false;
  int numRows=-1;
  std::stringstream failureComments;
  if (!DatabaseRoutinesGlobalFunctions::FetchTablE
      (this->userTablE, this->labelsUserTablE, tableTruncated, numRows,
       classTableName, failureComments))
  { comments << "<span style=\"color:red\"><b>Failed to fetch table: "
    << classTableName
    << failureComments.str() << "</b></span>";
    return false;
  }
  if (tableTruncated)
  { comments << "<span style=\"color:red\"><b>This shouldn't happen: email list truncated. "
    << "This is likely a software bug.</b></span>";
    return false;
  }
  int indexExtraInfo=-1;
  for (int i=0; i<this->labelsUserTablE.size; i++)
    if (this->labelsUserTablE[i]=="extraInfo")
      indexExtraInfo=i;
  if (indexExtraInfo==-1)
  { comments << "Failed to load section data. ";
    return false;
  }
  HashedList<std::string, MathRoutines::hashString> theSections;
  for (int i=0; i<this->userTablE.size; i++)
    theSections.AddOnTopNoRepetition(this->userTablE[i][indexExtraInfo]);
  this->classSections=theSections;
  this->classSections.QuickSortAscending();
  return true;
}

void CalculatorHTML::InterpretManageClass(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretManageClass");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return;
#ifdef MACRO_use_MySQL
  std::stringstream out;
  if (!this->flagLoadedClassDataSuccessfully)
  { out << "<b>Failed to load class data successfully.</b>";
    inputOutput.interpretedCommand=out.str();
    return;
  }
  out << "<hr><hr>";
  out << this->ToStringClassDetails(false, inputOutput);
  out << "<hr><hr>";
  out << this->ToStringClassDetails(true, inputOutput);
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

#include "vpfHeader5Crypto.h"
std::string CalculatorHTML::InterpretGenerateProblemManagementLink
(SyntacticElementHTML& inputOutput, std::stringstream& refStreamForReal, std::stringstream& refStreamExercise,
 const std::string& cleaneduplink, const std::string& urledProblem)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretGenerateProblemManagementLink");
  std::stringstream out;
  out << " <a href=\"" << refStreamForReal.str() << "\">Start (for credit)</a> ";
  out << " <a href=\"" << refStreamExercise.str() << "\">Exercise (no credit, unlimited tries)</a> ";
  //stOutput << "<hr>CurrentUser.problemNames=" << this->currentUser.problemNames.ToStringCommaDelimited();
  std::string thePoints="";
  if (this->databaseProblemList.Contains(cleaneduplink))
    thePoints= this->databaseProblemWeights[this->databaseProblemList.GetIndex(cleaneduplink)];
  #ifdef MACRO_use_MySQL
  bool noSubmissionsYet=false;
  if (this->currentUser.problemNames.Contains(cleaneduplink))
  { ProblemData& theProbData=this->currentUser.problemData[this->currentUser.problemNames.GetIndex(cleaneduplink)];
    if (!theProbData.flagProblemWeightIsOK)
    { out << "<span style=\"color:orange\">No point weight assigned yet. </span>";
      if (theProbData.ProblemWeightUserInput!="")
        out << "<span style=\"color:red\"><b>Failed to interpret weight string: "
        << theProbData.ProblemWeightUserInput << ". </b></span>";
    } else if (theProbData.totalNumSubmissions==0)
      noSubmissionsYet=true;
    else if (theProbData.numCorrectlyAnswered<theProbData.answerIds.size)
    { out << "<span style=\"color:red\"><b> "
      << theProbData.Points << " out of " << theProbData.ProblemWeight << " point(s). </b></span>";
    } else if (theProbData.numCorrectlyAnswered==theProbData.answerIds.size)
      out << "<span style=\"color:green\"><b> "
      << theProbData.Points << " out of " << theProbData.ProblemWeight << " point(s). </b></span>";
  } else
    noSubmissionsYet=true;
  if (thePoints!="" && noSubmissionsYet)
    out << "<span style=\"color:brown\"><b>No submissions: 0 out of " << thePoints
    << " point(s). </b> </span>" ;
  if (theGlobalVariables.UserDefaultHasAdminRights())
  { //stOutput << "<hr>this->databaseProblemList is: " << this->databaseProblemList.ToStringCommaDelimited();
    //stOutput << "<br>this->databaseProblemWeights is: " << this->databaseProblemWeights.ToStringCommaDelimited();
    //stOutput << "<br> cleanedupLink: " << cleaneduplink;
    std::string idPoints = "points" + urledProblem;
    std::string idButtonModifyPoints = "modifyPoints" + urledProblem;
    std::string idPointsModOutput = "modifyPointsOutputSpan" + urledProblem;
    out << "Points: <textarea rows=\"1\" cols=\"3\" id=\"" << idPoints << "\">";
    out << thePoints;
    out << "</textarea>";
    out << "<button id=\"" << idButtonModifyPoints << "\" "
    << "onclick=\"" << "submitStringAsMainInput('" << urledProblem
    << "='+encodeURIComponent('weight='+getElementById('" << idPoints << "').value), '"
    << idPointsModOutput << "', 'setProblemData');"
    << "\""
    << ">";
    out << "Modify";
    out << "</button>";
    out << "<span id=\"" << idPointsModOutput << "\">" << "</span>";
  }
  #endif // MACRO_use_MySQL
  return out.str();
}

std::string CalculatorHTML::InterpretGenerateDeadlineLink
(SyntacticElementHTML& inputOutput, std::stringstream& refStreamForReal, std::stringstream& refStreamExercise,
 const std::string& cleaneduplink, const std::string& urledProblem)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretGenerateDeadlineLink");
//  return "Submission deadline: to be announced. ";
  std::stringstream out;
  out << "deadline: To be announced.";
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return out.str();
  out << "<table><tr><td> Deadline: </td>";
  out << "<td><table><tr><th>Grp.</th><th>Deadline</th></tr>";
  List<std::string> deadlineIds;
  deadlineIds.SetSize(this->classSections.size);
  for (int i=0; i<this->classSections.size; i++)
  { std::string& currentDeadlineId=deadlineIds[i];
    currentDeadlineId = "deadline" + Crypto::CharsToBase64String(this->classSections[i]+cleaneduplink);
    if (currentDeadlineId[currentDeadlineId.size()-1]=='=')
      currentDeadlineId.resize(currentDeadlineId.size()-1);
    if (currentDeadlineId[currentDeadlineId.size()-1]=='=')
      currentDeadlineId.resize(currentDeadlineId.size()-1);
    out << "<tr>";
    out << "<td>" << this->classSections[i] << "</td>";
    out << "<td> <input type=\"text\" id=\"" << currentDeadlineId << "\"> " ;
    out << this->GetDatePickerStart(currentDeadlineId);
    out << "</td>";
    out << "</tr>";
  }
  out << "</table></td>";
  out << "<td>\n";
  out << "<button onclick=\"";
  out << "submitStringAsMainInput('" << urledProblem
  << "='+encodeURIComponent('deadlines='";
  std::string deadlineIdReport="deadlineReport"+Crypto::CharsToBase64String(cleaneduplink);
  if (deadlineIdReport[deadlineIdReport.size()-1]=='=')
    deadlineIdReport.resize(deadlineIdReport.size()-1);
  if (deadlineIdReport[deadlineIdReport.size()-1]=='=')
    deadlineIdReport.resize(deadlineIdReport.size()-1);
  for (int i=0; i<this->classSections.size; i++)
  { out << "+";
    out << "'" << CGI::StringToURLString(this->classSections[i]) << "='";
    out << "+ encodeURIComponent(document.getElementById('"
    << deadlineIds[i] << "').value)+'&'";
  }
  out << "), '" << deadlineIdReport << "', 'setProblemData' );"
  << "\""
  << ">\n";
  out << "  Set deadline(s)</button>";
  out << "<span id=\"" << deadlineIdReport << "\"></span>";
  out << "</td>";
  out << "</tr></table>";
  return out.str();
}

void CalculatorHTML::InterpretGenerateLink(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretGenerateLink");
  this->NumProblemsFound++;
  std::string cleaneduplink = this->CleanUpLink(inputOutput.content);
  std::string urledProblem=CGI::StringToURLString(cleaneduplink);
  std::stringstream out, refStreamNoRequest, refStreamExercise, refStreamForReal;
//  out << "cleaned up link: " << cleaneduplink;
//  out << "<br>urled link: " <<  urledProblem;
  refStreamNoRequest << theGlobalVariables.ToStringCalcArgsNoNavigation()
  << "currentExamFile=" << urledProblem << "&";
  if (this->flagIsExamHome || this->flagIsExamIntermediate)
    refStreamNoRequest << "currentExamHome=" << theGlobalVariables.GetWebInput("currentExamHome") << "&";
  if (this->flagIsExamIntermediate)
    refStreamNoRequest << "currentExamIntermediate="
    << theGlobalVariables.GetWebInput("currentExamIntermediate") << "&";
  refStreamExercise << theGlobalVariables.DisplayNameCalculatorWithPath << "?request=exercises&" << refStreamNoRequest.str();
  refStreamForReal << theGlobalVariables.DisplayNameCalculatorWithPath << "?request=examForReal&" << refStreamNoRequest.str();
  if (inputOutput.GetTagClass()=="calculatorExamProblem")
    out << this->InterpretGenerateProblemManagementLink
    (inputOutput, refStreamForReal, refStreamExercise, cleaneduplink, urledProblem);
  //else
  //  out << " <a href=\"" << refStreamExercise.str() << "\">Start</a> ";
  if (inputOutput.GetTagClass()=="calculatorExamIntermediate")
    out << this->InterpretGenerateDeadlineLink
    (inputOutput, refStreamForReal, refStreamExercise, cleaneduplink, urledProblem);
  std::string stringToDisplay = FileOperations::GetFileNameFromFileNameWithPath(inputOutput.content);
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
  out << this->GetSubmitStringAreaAsMainInput();
  if (this->flagIsExamProblem)
    out << this->GetSubmitAnswersJavascript();
  else if (this->flagIsExamHome)
    out << this->GetSubmitEmailsJavascript();
  if ((this->flagIsExamIntermediate || this->flagIsExamHome)&& theGlobalVariables.UserDefaultHasAdminRights())
    out << this->GetDatePickerJavascriptInit();
//  else
//    out << " no date picker";
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
    if (this->theContent[i].GetTagClass()=="calculatorManageClass")
    { this->flagLoadedClassDataSuccessfully= this->PrepareClassData(comments);
      break;
    }
  this->hdHomeworkGroups.SetExpectedSize(50);
  this->hdHomeworkGroups.SetSize(0);
  for (int i=0; i<this->theContent.size; i++)
    if (this->theContent[i].GetTagClass()=="calculatorExamIntermediate")
    { this->hdHomeworkGroups.SetSize(this->hdHomeworkGroups.size+1);
      this->hdHomeworkGroups.LastObject()->SetSize(0);
    } else if (this->theContent[i].GetTagClass()=="calculatorExamProblem")
    { if (this->hdHomeworkGroups.size==0)
      { out << "<b>Error: found a tag of type calculatorExamProblem before finding the first tag of type calculatorExamIntermediate."
        << " Please make a tag of type calculatorExamIntermediate before making tags of type calculatorExamProblem.</b>";
        continue;
      }
      this->hdHomeworkGroups.LastObject()->AddOnTop(this->theContent[i].content);
    }
//  out << "Debug data: homework groups found: " << this->hdHomeworkGroups.ToStringCommaDelimited();
  for (int i=0; i<this->theContent.size; i++)
    if (this->theContent[i].IsInterpretedNotByCalculator())
      this->InterpretNotByCalculator(this->theContent[i]);
  for (int i=0; i<this->theContent.size; i++)
    out << this->theContent[i].ToStringInterpreted();
//   out << "<hr><hr><hr><hr><hr><hr><hr><hr><hr>The calculator activity:<br>" << theInterpreter.outputString << "<hr>";
//   out << "<hr>" << this->ToStringExtractedCommands() << "<hr>";
  //  out << "<hr> Between the commands:" << this->betweenTheCommands.ToStringCommaDelimited();
#ifdef MACRO_use_MySQL
  if (this->flagIsForReal)
  { this->theProblemData.flagRandomSeedComputed=true;
    DatabaseRoutines theRoutines;
    //this->currentUser.username=theGlobalVariables.userDefault;
//    stOutput << "About to store problem data: " << this->theProblemData.ToString();
    //if (!this->currentUser.InterpretDatabaseProblemData(this->currentUserDatabaseString, comments))
      //out << "<b>Error: corrupt database string. </b>";
    //else
    this->currentUser.SetProblemData(this->fileName, this->theProblemData);
    if (!this->currentUser.StoreProblemDataToDatabase(theRoutines, comments))
        out << "<b>Error: failed to store problem in database. </b>" << comments.str();
  }
//  out << "Current collection problems: " << this->databaseProblemList.ToStringCommaDelimited()
//  << " with weights: " << this->databaseProblemWeights.ToStringCommaDelimited();
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
  if (parserOfParent.fileName!="")
    this->flagParentIsIntermediateExamFile=true;
  else
  { this->flagParentIsIntermediateExamFile=false;
    parserOfParent.fileName=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamHome"));
  }
  std::stringstream commentsOfparent;
  if (!parserOfParent.LoadMe(false, commentsOfparent))
  { comments << "Failed to load parent problem collection. Comments: " << commentsOfparent.str();
    return;
  }
  if (!parserOfParent.ParseHTML(commentsOfparent))
  { comments << "Failed to parse parent problem collection. Comments: " << commentsOfparent.str();
    return;
  }
  this->problemListOfParent=parserOfParent.hdProblemList;
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
  this->hdProblemList.SetSize(0);
  for (int i=0; i<this->theContent.size; i++)
    if (this->theContent[i].GetTagClass()=="calculatorExamProblem")
      this->hdProblemList.AddOnTop(this->CleanUpLink(this->theContent[i].content));
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
