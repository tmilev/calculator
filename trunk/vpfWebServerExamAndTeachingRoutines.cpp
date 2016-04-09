//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader6WebServer.h"
#include "vpfHeader3Calculator2_InnerFunctions.h"
#include "vpfHeader7DatabaseInterface_MySQL.h"
#include <iomanip>

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
  List<std::string> tagKeysWithoutValue;
  bool flagUseDisplaystyleInMathMode;
  bool flagUseMathMode;
  std::string interpretedCommand;
  static int ParsingNumDummyElements;
  bool IsInterpretedByCalculatorDuringPreparatioN();
  bool IsInterpretedByCalculatorDuringSubmission();
  bool IsInterpretedNotByCalculator();
  bool IsHidden();
  bool IsAnswer();
  bool IsCommentBeforeSubmission();
  bool IsAnswerOnGiveUp();
  bool IsCommentAfterSubmission();
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
    this->flagUseMathMode=true;
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
  double timeToParseHtml;
  List<double> timePerAttempt;
  List<List<double> > timeIntermediatePerAttempt;
  List<List<std::string> > timeIntermediateComments;
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
  HashedList<std::string, MathRoutines::hashString> tagKeysNoValue;
  List<std::string> calculatorClasses;
  List<char> splittingChars;
  List<SyntacticElementHTML> eltsStack;
  List<SyntacticElementHTML> theContent;
  List<std::string> answerVerificationCommand;
//  List<std::string> answerFirstCorrectSubmission;
  Selection studentTagsAnswered;
  ProblemData theProblemData;
  List<std::string> studentAnswersUnadulterated;
  HashedList<std::string, MathRoutines::hashString> hdProblemList;
  List<std::string> hdHomeworkGroupCorrespondingToEachProblem;
  List<List<std::string> > hdHomeworkGroups;
  HashedList<std::string, MathRoutines::hashString> hdHomeworkGroupNames;
  HashedList<std::string, MathRoutines::hashString> databaseProblemAndHomeworkGroupList;
//  List<std::string> databaseHomeworkGroupCorrespondingToEachProblem;
  List<List<std::string> > databaseHomeworkGroupDeadlinesPerSection;
  List<List<std::string> > databaseHomeworkGroupMaxNumTriesPerSection;
  List<std::string> databaseProblemWeights;
  List<List<std::string> > databaseStudentSectionsPerProblem;
  List<List<std::string> > databaseDeadlinesBySection;
  List<std::string> databaseStudentSectionS;
  List<List<std::string> > userTablE;
  List<std::string> labelsUserTablE;
  List<std::string> problemListOfParent;
  std::string currentUserDatabaseString;
  std::string currentProblemCollectionDatabaseString;
  bool flagLoadedSuccessfully;
  bool flagLoadedClassDataSuccessfully;
  static const std::string RelativePhysicalFolderProblemCollections;
  std::stringstream comments;
  bool CheckContent(std::stringstream& comments);
  bool CanBeMerged(const SyntacticElementHTML& left, const SyntacticElementHTML& right);
  bool LoadMe(bool doLoadDatabase, std::stringstream& comments);
  std::string CleanUpFileName(const std::string& inputLink);
  bool ParseHTMLComputeChildFiles(std::stringstream& comments);
  bool ParseHTML(std::stringstream& comments);
  bool IsSplittingChar(const std::string& input);
  void LoadFileNames();
  bool IsStateModifierApplyIfYes(SyntacticElementHTML& inputElt);
  bool ExtractAnswerIds(std::stringstream& comments);
  bool InterpretHtml(std::stringstream& comments);
  bool InterpretHtmlOneAttempt(Calculator& theInterpreter, std::stringstream& comments);
  bool PrepareAndExecuteCommands(Calculator& theInterpreter, std::stringstream& comments);
  bool PrepareCommands(Calculator& theInterpreter, std::stringstream& comments);
  std::string CleanUpCommandString(const std::string& inputCommand);
  void InterpretNotByCalculator(SyntacticElementHTML& inputOutput);
  std::string GetDeadline
  (
   const std::string& problemName
   //int indexInDatabase
   ,
   const std::string& sectionNumber
   , bool inheritFromGroup, bool& outputIsInherited);
  std::string InterpretGenerateDeadlineLink
(SyntacticElementHTML& inputOutput, std::stringstream& refStreamForReal, std::stringstream& refStreamExercise,
 const std::string& cleaneduplink, const std::string& urledProblem, bool problemAlreadySolved)
  ;
  std::string ToStringLinkFromFileName(const std::string& theFileName);
  std::string ToStringCalculatorProblemSourceFromFileName(const std::string& theFileName);
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
  std::string GetEditPageButton();
  std::string GetJavascriptSubmitMainInputIncludeCurrentFile();
  std::string GetSubmitEmailsJavascript();
  std::string GetDatePickerJavascriptInit();
  std::string GetDatePickerStart(const std::string& theId);
  std::string GetSubmitAnswersJavascript();
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
  std::string ToStringDeadlinesFormatted
  (const std::string& cleanedUpLink, const List<std::string>& sectionNumbers, bool isActualProblem, bool problemAlreadySolved)
  ;
  std::string ToStringOnEDeadlineFormatted
  (const std::string& cleanedUpLink, const std::string& sectionNumber, bool isActualProblem, bool problemAlreadySolved)
  ;
  std::string ToStringCalculatorArgumentsForProblem
  (const std::string& requestType, const std::string& studentView,
   const std::string& studentSection="", bool includeRandomSeedIfAppropriate=false)const;
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
  this->timeToParseHtml=0;
}

const std::string CalculatorHTML::RelativePhysicalFolderProblemCollections="ProblemCollections/";

#ifdef MACRO_use_MySQL
bool DatabaseRoutines::ReadProblemInfo
  (const std::string& stringToReadFrom, HashedList<std::string, MathRoutines::hashString>& outputProblemNames,
//   List<std::string>& outputHomeworkGroups,
   List<std::string>& outputWeights,
   List<List<std::string> >& outputSections, List<List<std::string> >& outputDeadlinesPerSection,
   std::stringstream& commentsOnFailure)
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
//  outputHomeworkGroups.initFillInObject(outputProblemNames.size, "");
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
//    if (problemKeys.Contains("homeworkGroup"))
//      outputHomeworkGroups[i]=
//      CGI::URLStringToNormal(problemValues[problemKeys.GetIndex("homeworkGroup")]);
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
//  stOutput << "reading from: " << CGI::URLKeyValuePairsToNormalRecursiveHtml(stringToReadFrom);
  return true;
}

void DatabaseRoutines::StoreProblemInfo
  (std::string& outputString, const HashedList<std::string, MathRoutines::hashString>& inputProblemNames,
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
    if (inputWeights[i]!="")
      currentProblemStream << "weight=" << CGI::StringToURLString(inputWeights[i]) << "&";
    if (inputSections[i].size!=inputDeadlines[i].size)
      crash << "Input sections and input deadlines have mismatching sizes. " << crash;
    for (int j=0; j<inputSections[i].size; j++)
      currentDeadlineStream << CGI::StringToURLString(inputSections[i][j])
      << "=" << CGI::StringToURLString(inputDeadlines[i][j]) << "&";
    currentProblemStream << "deadlines=" << CGI::StringToURLString(currentDeadlineStream.str()) << "&";
    out << CGI::StringToURLString(inputProblemNames[i]) << "="
    << CGI::StringToURLString(currentProblemStream.str()) << "&";
  }

//  stOutput << "Storing prob string: " << CGI::URLKeyValuePairsToNormalRecursiveHtml(out.str())
//  << "<br>nput sections: " << inputSections << "<br>";
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
        problemInformation LONGTEXT", &commentsOnFailure, 0))
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
        problemInformation LONGTEXT", &commentsOnFailure, 0))
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
  CalculatorHTML problemHome;
  problemHome.fileName=problemHomeName;
  if (!problemHome.LoadMe(true, commentsOnFailure))
    return false;
  if (!problemHome.ParseHTML(commentsOnFailure))
    return false;
  HashedList<std::string, MathRoutines::hashString> incomingProblems;
  List<std::string> incomingWeights;
  List<List<std::string> > incomingSections;
  List<List<std::string> > incomingDeadlines;
  if (!this->ReadProblemInfo
      (inputString, incomingProblems, incomingWeights,
       incomingSections, incomingDeadlines, commentsOnFailure))
  { commentsOnFailure << "Failed to parse your request";
    return false;
  }
//  stOutput << "<hr>incoming sections: " << incomingSections.ToStringCommaDelimited() << "<hr>";
//  stOutput << "incoming deadlines: " << incomingDeadlines.ToStringCommaDelimited() << "<hr>";

  std::string currentFileName;
  bool result=true;
  for (int i=0; i<incomingProblems.size; i++)
  { if (!problemHome.hdProblemList.Contains(incomingProblems[i]) &&
        !problemHome.hdHomeworkGroupNames.Contains(incomingProblems[i]))
    { commentsOnFailure << "Did not find " << incomingProblems[i]
      << " among the list of problems and homework groups. ";
//      << " The problem list consists of: "
//      << problemHome.hdProblemList.ToStringCommaDelimited() << ". The homework group names are: "
//      << problemHome.hdHomeworkGroupNames.ToStringCommaDelimited();
      result=false;
      continue;
    }
    int theIndex=problemHome.databaseProblemAndHomeworkGroupList.GetIndex(incomingProblems[i]);
    if (theIndex==-1)
    { problemHome.databaseProblemAndHomeworkGroupList.AddOnTop(incomingProblems[i]);
      problemHome.databaseProblemWeights.AddOnTop(incomingWeights[i]);
      problemHome.databaseStudentSectionsPerProblem.AddOnTop(incomingSections[i]);
      problemHome.databaseDeadlinesBySection.AddOnTop(incomingDeadlines[i]);
      continue;
    }
//    stOutput << "<br>DEBUG: modifying problem info for problem: " << incomingProblems[i];
    if (incomingWeights[i]!="")
      problemHome.databaseProblemWeights[theIndex]=incomingWeights[i];
    if (incomingSections[i].size!=0)
      for (int j=0; j<incomingSections[i].size; j++)
      { int sectionIndex=problemHome.databaseStudentSectionsPerProblem[theIndex].GetIndex(incomingSections[i][j]);
        if (sectionIndex==-1)
        { problemHome.databaseStudentSectionsPerProblem[theIndex].AddOnTop(incomingSections[i][j]);
          problemHome.databaseDeadlinesBySection[theIndex].AddOnTop(incomingDeadlines[i][j]);
          continue;
        }
//        stOutput << "<br>DEBUG: modifying problem info for section: " << incomingSections[i][j];
        problemHome.databaseStudentSectionsPerProblem[theIndex][sectionIndex]=incomingSections[i][j];
        problemHome.databaseDeadlinesBySection[theIndex][sectionIndex]=incomingDeadlines[i][j];
      }
  }
  std::string stringToStore;

  this->StoreProblemInfo
  (stringToStore, problemHome.databaseProblemAndHomeworkGroupList, problemHome.databaseProblemWeights,
   problemHome.databaseStudentSectionsPerProblem, problemHome.databaseDeadlinesBySection);
  //stOutput << "<br>about to store back : <br>" << stringToStore << " interpreted as: <br>"
  //<< CGI::URLKeyValuePairsToNormalRecursiveHtml(stringToStore) ;
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
  this->flagIsForReal=theGlobalVariables.UserRequestRequiresLoadingRealExamData();
#ifdef MACRO_use_MySQL
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
  this->currentExamHomE=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamHome"));
  if (doLoadDatabase && this->currentExamHomE!="")
  { //stOutput << "loading db, problem collection: " << this->currentExamHomE;
    this->currentUser.currentTable=theRoutines.GetTableUnsafeNameUsersOfFile(this->currentExamHomE);
    //stOutput << "loading extra info ... " << this->currentExamHomE;
    if(!this->currentUser.FetchOneColumn("extraInfo", this->currentUser.extraInfoUnsafe, theRoutines, &comments))
    { comments << "Failed to load the section/group of the current user. ";
//      return false;
    }
    if (!theRoutines.ReadProblemDatabaseInfo
        (this->currentExamHomE, this->currentProblemCollectionDatabaseString, comments))
    { comments << "Failed to load current problem collection's database string. ";
      return false;
    }
    //stOutput << ", the db string is: " << this->currentProblemCollectionDatabaseString;
    if (!theRoutines.ReadProblemInfo
        (this->currentProblemCollectionDatabaseString, this->databaseProblemAndHomeworkGroupList,
         this->databaseProblemWeights,
         this->databaseStudentSectionsPerProblem, this->databaseDeadlinesBySection, comments))
    { comments << "Failed to interpret the database problem string. ";
      return false;
    }
    this->currentUser.ComputePointsEarned
    (this->databaseProblemAndHomeworkGroupList, this->databaseProblemWeights);
  }
#endif // MACRO_use_MySQL
  if (!this->flagIsForReal)
  { std::string randString= theGlobalVariables.GetWebInput("randomSeed");
    if (randString!="")
    { std::stringstream randSeedStream(randString);
      //stOutput << "radSeedStream: " << randString;
      randSeedStream >> this->theProblemData.randomSeed;
      //stOutput << ", interpreted as: " << this->theProblemData.randomSeed;
      this->flagRandomSeedGiven=true;
    }
  }
//  stOutput << "debug info: rand seed after all's been said and done: " << this->theProblemData.randomSeed;
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
  { if (theGlobalVariables.UserDefaultHasAdminRights())
      out << this->GetEditPageButton();
    out << "<b>Failed to interpret file: " << this->fileName << "</b>. Comments: " << this->comments.str();
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

void CalculatorHTML::LoadFileNames()
{ this->fileName = CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamFile"));
  this->currentExamHomE = CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamHome"));
}

void CalculatorHTML::LoadCurrentProblemItem()
{ MacroRegisterFunctionWithName("CalculatorHTML::LoadCurrentProblemItem");
  this->LoadFileNames();
  this->flagLoadedSuccessfully=false;
  bool needToFindDefault=(this->fileName=="");
  bool needToLoadDatabase=true;
  if (theGlobalVariables.UserGuestMode())
    needToLoadDatabase=false;
  if (!needToFindDefault)
    needToFindDefault=!this->LoadMe(needToLoadDatabase, this->comments);
  else
    this->comments << "<b>Selecting default course homework file.</b><br>";
  if (needToFindDefault)
  { if (!this->FindExamItem())
    { this->comments << "<b>No problems/exams to serve: found no html content in folder: "
      << this->RelativePhysicalFolderProblemCollections << ".</b>";
      return;
    }
    theGlobalVariables.SetWebInput("currentExamHome", CGI::StringToURLString(this->fileName));
    if (!this->LoadMe(needToLoadDatabase, this->comments))
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

std::string CalculatorHTML::GetEditPageButton()
{ MacroRegisterFunctionWithName("CalculatorHTML::GetEditPageButton");
  std::stringstream out;
  out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath
  << "?request=editPage&";
  std::string urledProblem=CGI::StringToURLString(this->fileName);
  std::stringstream refStreamNoRequest;
//  out << "cleaned up link: " << cleaneduplink;
//  out << "<br>urled link: " <<  urledProblem;
  refStreamNoRequest << theGlobalVariables.ToStringCalcArgsNoNavigation()
  << "currentExamFile=" << urledProblem << "&"
  << "currentExamHome=" << theGlobalVariables.GetWebInput("currentExamHome") << "&";
  out << refStreamNoRequest.str() << "\">" << "Edit problem/page" << "</a>";
  out << "<textarea id=\"clonePageAreaID\" rows=\"1\" cols=\"100\">" << this->fileName << "</textarea>\n"
  << "<button class=\"submitButton\" onclick=\""
  << "submitStringAsMainInput(document.getElementById('clonePageAreaID').value, 'spanCloningAttemptResultID', 'clonePage');"
  << "\" >Clone page</button> <span id=\"spanCloningAttemptResultID\"></span><br>";
  return out.str();
}

std::string CalculatorHTML::GetJavascriptSubmitMainInputIncludeCurrentFile()
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
  << "  inputParams+='&currentExamFile=" << CGI::StringToURLString(this->fileName) << "';\n"
  << "  inputParams+='&currentExamHome=" << CGI::StringToURLString(this->currentExamHomE) << "';\n"
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
  << "function addEmailsOrUsers(idEmailList, problemCollectionName, idOutput, userRole, idExtraInfo, requestType){\n"
  << "  spanOutput = document.getElementById(idOutput);\n"
  << "  if (spanOutput==null){\n"
  << "    spanOutput = document.createElement('span');\n"
  << "    document.body.appendChild(spanOutput);\n"
  << "    spanOutput.innerHTML= \"<span style='color:red'> ERROR: span with id \" + idEmailList + \"MISSING! </span>\";\n"
  << "  }\n"
  << "  spanEmailList = document.getElementById(idEmailList);\n"
  << "  spanExtraInfo = document.getElementById(idExtraInfo);\n"
  << "  inputParams='request='+requestType;\n"
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
  std::string requestTypeSubmit, requestTypePreview, requestGiveUp;
  bool submitRandomSeed=false;
  if (theGlobalVariables.UserRequestRequiresLoadingRealExamData())
  { requestTypeSubmit  = "submitProblem";
    requestTypePreview = "submitProblemPreview";
  } else if (theGlobalVariables.UserGuestMode())
  { requestTypeSubmit  = "submitExerciseNoLogin";
    requestTypePreview = "submitExercisePreviewNoLogin";
    submitRandomSeed=true;
  } else
  { requestTypeSubmit  = "submitExercise";
    requestTypePreview = "submitExercisePreview";
    submitRandomSeed=true;
  }
  if (!theGlobalVariables.UserGuestMode())
    requestGiveUp="problemGiveUp";
  else
    requestGiveUp="problemGiveUpNoLogin";
  out
  << "<script type=\"text/javascript\"> \n"
  << "var JavascriptInsertionAlreadyCalled;\n"
  << "JavascriptInsertionAlreadyCalled=false;\n"
  << "var numInsertedJavascriptChildren;\n"
  << "var numInsertedJavascriptChildren=0;\n"
  << "var timerForPreviewAnswers;\n"
  << "function previewAnswers(idAnswer, idVerification){\n"
  << "  clearTimeout(timerForPreviewAnswers);\n"
  << "  timerForPreviewAnswers=setTimeout(function(){\n"
  << "    params=\"" << this->ToStringCalculatorArgumentsForProblem(requestTypePreview, "true", "", submitRandomSeed) << "\";\n"
  << "    submitOrPreviewAnswers(idAnswer, idVerification, params);\n"
  << "  }, 1700);"
  << "}\n"
  << "function submitAnswers(idAnswer, idVerification){\n"
  << "  clearTimeout(timerForPreviewAnswers);\n"
  << "  params=\"" << this->ToStringCalculatorArgumentsForProblem(requestTypeSubmit, "true", "", submitRandomSeed) << "\";\n"
  << "  submitOrPreviewAnswers(idAnswer, idVerification, params);\n"
  << "}\n"
  << "function giveUp(idAnswer, idVerification){\n"
  << "  clearTimeout(timerForPreviewAnswers);\n"
  << "  params=\"" << this->ToStringCalculatorArgumentsForProblem(requestGiveUp, "true", "", submitRandomSeed) << "\";\n"
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
  << "    var scripts = spanVerification.getElementsByTagName('script');\n"
  << "    for (i=0; i<numInsertedJavascriptChildren; i++)"
  << "    { document.getElementsByTagName( 'head' )[0].removeChild(document.getElementsByTagName( 'head' )[0].lastChild);\n"
  << "      document.getElementsByTagName( 'head' )[0].appendChild(scriptChild);\n"
  << "    }\n"
  << "    numInsertedJavascriptChildren=0;"
  << "    for (i=0; i<scripts.length; i++){\n"
  << "      scriptChild= document.createElement('script');\n"
  << "      scriptChild.innerHTML=scripts[i].innerHTML;\n"
  << "      scriptChild.type='text/javascript';\n"
  << "      document.getElementsByTagName( 'head' )[0].appendChild(scriptChild);\n"
  << "      numInsertedJavascriptChildren++;\n "
  << "    }\n"
  << "    JavascriptInsertionAlreadyCalled=true;\n"
  << "    MathJax.Hub.Queue(['Typeset', MathJax.Hub, spanVerification]);\n"
  << "  }\n"
  << "  https.send(inputParams);\n"
  << "}\n"
  << "</script>";
  return out.str();
}

const std::string CalculatorHTML::BugsGenericMessage=
"Please take a screenshot, copy the link address and send those along \
with a short explanation to the following UMass instructor: Todor Milev (todor.milev@gmail.com)";

int WebWorker::ProcessProblemGiveUp()
{ MacroRegisterFunctionWithName("WebWorker::ProcessProblemGiveUp");
  double startTime=theGlobalVariables.GetElapsedSeconds();
  CalculatorHTML theProblem;
  theProblem.LoadCurrentProblemItem();
  if (!theProblem.flagLoadedSuccessfully)
  { stOutput << "Problem name is: " << theProblem.fileName;
    stOutput << " <b>Could not load problem, this may be a bug. "
    << CalculatorHTML::BugsGenericMessage << "</b>";
    if(theProblem.comments.str()!="")
      stOutput << " Comments: " << theProblem.comments.str();
    return 0;
  }
  if (theProblem.flagIsForReal)
  { stOutput << " <b>Not allowed to show answer of a problem being tested for real. </b>";
    return 0;
  }
  if(theGlobalVariables.GetWebInput("randomSeed")=="")
  { stOutput << " <b>I could not figure out the exercise problem (missing random seed). </b>";
    return 0;
  }
  std::stringstream comments;
  if (!theProblem.ParseHTML(comments))
  { stOutput << "<br><b>Failed to parse problem.</b> Comments: " << comments.str();
    return 0;
  }
  std::string lastStudentAnswerID;
  for (int i=0; i<theGlobalVariables.webFormArgumentNames.size; i++)
    MathRoutines::StringBeginsWith(theGlobalVariables.webFormArgumentNames[i], "calculatorAnswer", &lastStudentAnswerID);
  int indexLastAnswerId=theProblem.theProblemData.answerIds.GetIndex(lastStudentAnswerID);
  if (indexLastAnswerId==-1)
  { stOutput << "<b>Student submitted answerID: " << lastStudentAnswerID
    << " but that is not an ID of an answer tag. "
    << "</b><br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return 0;
  }
  if (theProblem.theProblemData.commandsForGiveUpAnswer[indexLastAnswerId]=="")
  { stOutput << "<b> Unfortunately there is no answer given for this question (answerID: " << lastStudentAnswerID << ").";
    return 0;
  }
  Calculator theInterpreteR;
  theInterpreteR.init();
  if(!theProblem.PrepareCommands(theInterpreteR, comments))
  { stOutput << "<b>Failed to prepare calculator commands. </b> <br>Comments:<br>" << comments.str();
    return 0;
  }
  std::stringstream answerCommands;
  answerCommands << theProblem.problemCommandsNoVerification;
  answerCommands << "SeparatorBetweenSpans; "
  << theProblem.theProblemData.commandsForGiveUpAnswer[indexLastAnswerId];
  theInterpreteR.Evaluate(answerCommands.str());
  if (theInterpreteR.syntaxErrors!="")
  { stOutput << "<span style=\"color:red\"><b>Failed to evaluate the default answer. "
    << "Likely there is a bug with the problem. </b></span>"
    << "<br>" << CalculatorHTML::BugsGenericMessage << "<br>Details: <br>"
    << theInterpreteR.ToStringSyntacticStackHumanReadable(false);
    stOutput << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return 0;
  } else if (theInterpreteR.flagAbortComputationASAP)
  { stOutput << "<span style=\"color:red\"><b>Failed to evaluate the default answer. "
    << "Likely there is a bug with the problem. </b></span>"
    << "<br>" << CalculatorHTML::BugsGenericMessage << "<br>Details: <br>"
    << theInterpreteR.outputString;
    stOutput << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return 0;
  }
  FormatExpressions theFormat;
  List<std::string> answersReverseOrder;
  for (int j=theInterpreteR.theProgramExpression.children.size-1; j>=0; j--)
  { if (theInterpreteR.theProgramExpression[j].ToString()=="SeparatorBetweenSpans")
      break;
//        stOutput << "<hr>DEBUG Before final computation: <hr>";
    theFormat.flagExpressionIsFinal=true;
    theFormat.flagIncludeExtraHtmlDescriptionsInPlots=false;
    theFormat.flagUseQuotes=false;
    if (!theInterpreteR.theProgramExpression[j].StartsWith(theInterpreteR.opRulesChanged()))
      answersReverseOrder.AddOnTop(theInterpreteR.theProgramExpression[j].ToString(&theFormat));
  }
  for (int i=answersReverseOrder.size-1; i>=0; i--)
  { stOutput << "\\(" << answersReverseOrder[i] << "\\)";
    if (i!=0)
      stOutput << "<br>";
  }
  stOutput << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
  return 0;
}

int WebWorker::ProcessSubmitProblemPreview()
{ MacroRegisterFunctionWithName("WebWorker::ProcessSubmitProblemPreview");
  double startTime=theGlobalVariables.GetElapsedSeconds();
  std::stringstream studentInterpretation;
  std::string lastStudentAnswerID;
  std::string lastAnswer;
  for (int i=0; i<theGlobalVariables.webFormArgumentNames.size; i++)
    if (MathRoutines::StringBeginsWith(theGlobalVariables.webFormArgumentNames[i], "calculatorAnswer", &lastStudentAnswerID))
    { lastAnswer= "("+ CGI::URLStringToNormal( theGlobalVariables.webFormArguments[i]) + "); ";
      studentInterpretation << lastAnswer;
    }
  stOutput << "Your answer(s): \\(" << studentInterpretation.str() << "\\)" << "\n<br>\n";
  CalculatorHTML theProblem;
  theProblem.LoadCurrentProblemItem();
  if (!theProblem.flagLoadedSuccessfully)
    stOutput << "<br><b>Failed to load problem.</b> Comments: " << theProblem.comments.str();
  std::stringstream comments;
  if (!theProblem.ParseHTML(comments))
    stOutput << "<br><b>Failed to parse problem.</b> Comments: " << comments.str();
  int indexLastAnswerId=theProblem.theProblemData.answerIds.GetIndex(lastStudentAnswerID);
  if (indexLastAnswerId==-1)
  { stOutput << "<br>Student submitted answerID: " << lastStudentAnswerID
    << " but that is not an ID of an answer tag. "
    << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return 0;
  }
  Calculator theInterpreteR;
  theInterpreteR.init();
  theInterpreteR.Evaluate(studentInterpretation.str());
  if (theInterpreteR.syntaxErrors!="")
  { stOutput << "<span style=\"color:red\"><b>Failed to parse your answer, got:</b></span><br>"
    << theInterpreteR.ToStringSyntacticStackHumanReadable(false);
    stOutput << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return 0;
  } else if (theInterpreteR.flagAbortComputationASAP)
  { stOutput << "<span style=\"color:red\"><b>Failed to evaluate your answer, got:</b></span><br>"
    << theInterpreteR.outputString;
    stOutput << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return 0;
  }
  Calculator interpreterWithContext;
  interpreterWithContext.init();
  std::stringstream interpreterInputStream;
  if (theProblem.theProblemData.commandsForPreview[indexLastAnswerId].size>0)
  { for (int i=0; i<theProblem.theProblemData.commandsForPreview[indexLastAnswerId].size; i++)
      interpreterInputStream << theProblem.CleanUpCommandString
      (theProblem.theProblemData.commandsForPreview[indexLastAnswerId][i]);
    interpreterInputStream << "SeparatorBetweenSpans; ";
  }
  interpreterInputStream << studentInterpretation.str();
  interpreterWithContext.Evaluate(interpreterInputStream.str());
  if (interpreterWithContext.syntaxErrors!="")
  { stOutput << "<span style=\"color:red\">"
    << "<b>Failed to parse your answer in the context of the problem, got:</b>"
    << "</span><br>"
    << interpreterWithContext.ToStringSyntacticStackHumanReadable(false);
    stOutput << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return 0;
  } else if (interpreterWithContext.flagAbortComputationASAP)
  { stOutput << "<span style=\"color:red\">"
    << "<b>Failed to evaluate your answer in the context of the problem, got:</b>"
    << "</span><br>"
    << interpreterWithContext.outputString;
    stOutput << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return 0;
  }
  stOutput << "<span style=\"color:magenta\"><b>Interpreting your answer as:</b></span><br>";
  int indexLowest=-1;
  for (indexLowest=interpreterWithContext.theProgramExpression.children.size-1; indexLowest>=1; indexLowest--)
    if (interpreterWithContext.theProgramExpression[indexLowest].ToString()=="SeparatorBetweenSpans")
    { indexLowest++;
      break;
    }
  if (indexLowest<1)
    indexLowest=1;
  stOutput << "\\(";
  for (int i=indexLowest; i<interpreterWithContext.theProgramExpression.children.size; i++)
  { stOutput << interpreterWithContext.theProgramExpression[i].ToString() ;
    if (i!=interpreterWithContext.theProgramExpression.children.size-1)
      stOutput << "; ";
  }
  stOutput << "\\)";
  if (theProblem.theProblemData.commentsBeforeSubmission[indexLastAnswerId].size==0)
  { stOutput << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return 0;
  }
  std::stringstream calculatorInputStream;
  Calculator theInterpreterWithAdvice;
  theInterpreterWithAdvice.init();
  theInterpreterWithAdvice.flagWriteLatexPlots=false;
  if (!theProblem.PrepareCommands(theInterpreterWithAdvice, comments))
  { stOutput << "Something went wrong while interpreting the problem file. ";
    if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights())
      stOutput << comments.str();
    stOutput << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
    return 0;
  }
  calculatorInputStream << theProblem.problemCommandsNoVerification;
  calculatorInputStream << theProblem.theProblemData.answerIds[indexLastAnswerId] << " = "
  << lastAnswer;
  calculatorInputStream << "SeparatorBetweenSpans; ";
  for (int i=0; i<theProblem.theProblemData.commentsBeforeSubmission[indexLastAnswerId].size; i++)
    calculatorInputStream << theProblem.CleanUpCommandString
    (theProblem.theProblemData.commentsBeforeSubmission[indexLastAnswerId][i]);
  theInterpreterWithAdvice.Evaluate(calculatorInputStream.str());
  //stOutput << "DEBUG: Interpreter with advice: " << theInterpreterWithAdvice.theProgramExpression.ToString();
  if (theInterpreterWithAdvice.syntaxErrors!="")
  { stOutput << "<br><span style=\"color:red\"><b>"
    << "Something went wrong when parsing your answer in the context of the current problem. "
    << "</b></span>";
    if (theGlobalVariables.UserDefaultHasAdminRights() && theGlobalVariables.UserDebugFlagOn())
      stOutput << theInterpreterWithAdvice.outputString << "<br>" << theInterpreterWithAdvice.outputCommentsString;
  } else if (theInterpreterWithAdvice.flagAbortComputationASAP )
  { stOutput << "<br><span style=\"color:red\"><b>"
    << "Something went wrong when interpreting your answer in the context of the current problem. "
    << "</b></span>";
    if (theGlobalVariables.UserDefaultHasAdminRights() && theGlobalVariables.UserDebugFlagOn() )
      stOutput << "<br>Logged-in as admin with debug flag on=> printing error details. "
      << theInterpreterWithAdvice.outputString << "<br>" << theInterpreterWithAdvice.outputCommentsString;
  } else
  { List<std::string> answersReverseOrder;
    FormatExpressions theFormat;
//       stOutput << "DBGDBG<br>";
    for (int j=theInterpreterWithAdvice.theProgramExpression.children.size-1; j>=0; j--)
    { if (theInterpreterWithAdvice.theProgramExpression[j].ToString()=="SeparatorBetweenSpans")
        break;
//         stOutput << "<hr>DEBUG Before final computation: <hr>";
      theFormat.flagExpressionIsFinal=true;
      theFormat.flagIncludeExtraHtmlDescriptionsInPlots=false;
      theFormat.flagUseQuotes=false;
      answersReverseOrder.AddOnTop(theInterpreterWithAdvice.theProgramExpression[j].ToString(&theFormat));
    }
    for (int j=answersReverseOrder.size-1; j>=0; j--)
      stOutput << answersReverseOrder[j];
  }
  stOutput << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";
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

int WebWorker::ProcessModifyPage()
{ MacroRegisterFunctionWithName("WebWorker::ProcessModifyPage");
  stOutput << this->GetModifyProblemReport();
  stOutput.Flush();
  return 0;
}

std::string WebWorker::GetSetProblemDatabaseInfoHtml()
{ MacroRegisterFunctionWithName("WebWorker::GetSetProblemDatabaseInfoHtml");
#ifdef MACRO_use_MySQL
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return "<b>Only admins may set problem weights.</b>";
  std::string inputProblemInfo=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("mainInput"));
  std::string inputProblemHome=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamHome"));
  DatabaseRoutines theRoutines;
  std::stringstream commentsOnFailure;
  bool result=theRoutines.MergeProblemInfoInDatabase(inputProblemHome, inputProblemInfo, commentsOnFailure);
  std::stringstream out;
  if (result)
  { out << "<span style=\"color:green\"><b>Successfully modified problem data. </b></span>";
    //out << "<meta http-equiv=\"refresh\" content=\"0;\">";
  } else
    out << "<span style=\"color:red\"><b>" << commentsOnFailure.str() << "</b></span>";
//out << "<br>Debug message:<br>inputProblemInfo raw: " << inputProblemInfo << "<br>Processed: "
//  << CGI::URLKeyValuePairsToNormalRecursiveHtml(inputProblemInfo)
//  << "<br>inputProblemHome: " << inputProblemHome;
  return out.str();
#else
  return "Cannot modify problem weights (no database available)";
#endif // MACRO_use_MySQL
}

std::string WebWorker::GetModifyProblemReport()
{ MacroRegisterFunctionWithName("WebWorker::GetModifyProblemReport");
  if (!theGlobalVariables.flagLoggedIn || !theGlobalVariables.UserDefaultHasAdminRights() ||
      !theGlobalVariables.flagUsingSSLinCurrentConnection)
    return "<b>Modifying problems allowed only for logged-in admins under ssl connection. </b>";
  std::string mainInput=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("mainInput"));
  std::string fileName= CalculatorHTML::RelativePhysicalFolderProblemCollections+
  CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamFile"));
  std::fstream theFile;
  std::stringstream out;
  if (!FileOperations::OpenFileOnTopOfProjectBase(theFile, fileName, false, true, false))
  { out << "<b><span style=\"color:red\">Failed to open file: " << fileName << ". </span></b>";
    return out.str();
  }
  theFile << mainInput;
  theFile.close();
  out << "<b><span style=\"color:green\">Written content to file: "
  << fileName << ". </span></b>";
  return out.str();
}

std::string CalculatorHTML::ToStringCalculatorProblemSourceFromFileName(const std::string& theFileName)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringCalculatorProblemSourceFromFileName");
  std::stringstream out;
  out << "<span class=\"calculatorExamProblem\">\n"
  << theFileName
  << "\n</span>";
  return out.str();
}

std::string CalculatorHTML::ToStringLinkFromFileName(const std::string& theFileName)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringLinkFromFileName");
  std::stringstream notUsed;
//  stOutput << "Figuring out current prob list ...";
  this->FigureOutCurrentProblemList(notUsed);
//  stOutput << "current home: " << this->currentExamHomE;
  SyntacticElementHTML theElt;
  theElt.content=theFileName;
  theElt.tagKeys.AddOnTop("class");
  theElt.tagValues.AddOnTop("calculatorExamProblem");
  this->InterpretGenerateLink(theElt);
  return  theElt.interpretedCommand;
}

std::string WebWorker::GetClonePageResult()
{ MacroRegisterFunctionWithName("WebWorker::GetClonePageResult");
  if (!theGlobalVariables.flagLoggedIn || !theGlobalVariables.UserDefaultHasAdminRights() ||
      !theGlobalVariables.flagUsingSSLinCurrentConnection)
    return "<b>Cloning problems allowed only for logged-in admins under ssl connection. </b>";
  std::string fileNameResulT = CGI::URLStringToNormal(theGlobalVariables.GetWebInput("mainInput"));
  std::string fileNameResultRelative = CalculatorHTML::RelativePhysicalFolderProblemCollections+fileNameResulT;
  std::string fileNameToBeCloned = CalculatorHTML::RelativePhysicalFolderProblemCollections+
  CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamFile"));
  std::stringstream out;
  std::string startingFileString;
  if (!FileOperations::LoadFileToStringOnTopOfProjectBase(fileNameToBeCloned, startingFileString, out))
  { out << "Could not find file: " << fileNameToBeCloned;
    return out.str();
  }
  std::fstream theFile;
  if (FileOperations::FileExistsOnTopOfProjectBase(fileNameResultRelative))
  { out << "<b>File: " << fileNameToBeCloned << " already exists. </b>";
    return out.str();
  }
  if (!FileOperations::OpenFileCreateIfNotPresentOnTopOfProjectBase(theFile, fileNameResultRelative, false, false, false))
  { out << "<b><span style=\"color:red\">Failed to open output file: " << fileNameResultRelative << ". </span></b>";
    return out.str();
  }
  theFile << startingFileString;
  theFile.close();
  out << "<b><span style=\"color:green\">Written content to file: "
  << fileNameResulT << ". </span></b>";
  CalculatorHTML linkInterpreter;
  out << linkInterpreter.ToStringLinkFromFileName(fileNameResulT);
  return out.str();
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
//  if (extraInfo=="")
//    extraInfo="default";
  if (inputEmails=="")
  { out << "<b>No emails to add</b>";
    return out.str();
  }
#ifdef MACRO_use_MySQL
  DatabaseRoutines theRoutines;
  std::stringstream comments;
  bool sentEmails=true;
  bool doSendEmails= theGlobalVariables.userCalculatorRequestType=="sendEmails" ?  true : false;
  bool createdUsers=theRoutines.AddUsersFromEmails(doSendEmails, inputEmails, extraInfo, sentEmails, comments);
  if (createdUsers)
    out << "<span style=\"color:green\">Users successfully added. </span>";
  else
    out << "<span style=\"color:red\">Failed to add all users. </span>";
  if (doSendEmails)
  { if (sentEmails)
      out << "<span style=\"color:green\">Activation emails successfully sent. </span>";
    else
      out << "<span style=\"color:red\">Failed to send all activation emails. </span>";
  }
  CalculatorHTML theProblemCollection;
  std::string userRole=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("userRole"));
  bool usersAreAdmins= (userRole=="admin");
  std::string currentExamHome=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamHome"));
  CalculatorHTML theCollection;
  theRoutines.PrepareClassData
  (currentExamHome, theCollection.userTablE, theCollection.labelsUserTablE, comments);
  out << theRoutines.ToStringClassDetails
  (usersAreAdmins, theCollection.userTablE, theCollection.labelsUserTablE,
   currentExamHome, theCollection.databaseProblemAndHomeworkGroupList, theCollection.databaseProblemWeights);
  if (!createdUsers || !sentEmails)
    out << "<br>Comments:<br>" << comments.str();
  return out.str();
#else
  return "<b>no database present.</b>";
#endif // MACRO_use_MySQL
}

int WebWorker::ProcessSubmitProblem()
{ MacroRegisterFunctionWithName("WebWorker::ProcessSubmitProblem");
  double startTime=theGlobalVariables.GetElapsedSeconds();
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
  theProblem.currentExamHomE         = CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamHome"));
  theProblem.currentExamIntermediatE = CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamIntermediate"));
  if (theProblem.currentExamHomE == "")
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
      { stOutput << "<b> You submitted an answer to tag with id " << studentAnswerNameReader
        << " which is not on my list of "
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
    { CalculatorHTML theProblemHome;
      theProblemHome.fileName=theProblem.currentExamHomE;
      bool isGood=true;
//      stOutput << "Time before loading collection: " << theGlobalVariables.GetElapsedSeconds()-startTime;
      if (!theProblemHome.LoadMe(true, comments))
        isGood=false;
      if (isGood)
        if (!theProblemHome.ParseHTML(comments))
          isGood=false;
 //     stOutput << "Time after loading collection: " << theGlobalVariables.GetElapsedSeconds()-startTime;
      if (!isGood)
      { stOutput << "<b>Failed to load problem collection home: " << theProblem.currentExamHomE
        << ". Comments: " << comments.str()  << " Answer not recorded. "
        << "This should not happen. " << CalculatorHTML::BugsGenericMessage << "</b>";
        return 0;
      }
 //     stOutput << "User: " << theProblem.currentUser.username.value << "; section: "
 //     << theProblem.currentUser.extraInfoUnsafe;
 //     bool tmp=false;
 //     stOutput << "problem db info: " << theProblemHome.databaseProblemAndHomeworkGroupList.ToStringCommaDelimited();
 //     stOutput
      bool unused=false;
      bool deadLinePassed=false;
      std::string theDeadlineString=theProblemHome.GetDeadline
      (theProblem.fileName, theProblem.currentUser.extraInfoUnsafe, true, unused);
      if (theDeadlineString!="" && theDeadlineString!=" ")
      { TimeWrapper now, deadline; //<-needs a fix for different time formats.
      //<-For the time being, we hard-code it to month/day/year format (no time to program it better).
        std::stringstream badDateStream;
        if (!deadline.AssignMonthDayYear(theDeadlineString, badDateStream))
        { stOutput << "<b>Problem reading deadline. </b> The deadline string was: "
          << theDeadlineString << ". Comments: "
          << "<span style=\"color:red\">" << badDateStream.str() << "</span>"
          << " This should not happen. " << CalculatorHTML::BugsGenericMessage;
          return 0;
        }
        //  out << "deadline.date: " << deadline.theTime.tm_mday;
        now.AssignLocalTime();
        //  out << "Now: " << asctime (&now.theTime) << " mktime: " << mktime(&now.theTime)
        //  << " deadline: " << asctime(&deadline.theTime) << " mktime: " << mktime(&deadline.theTime);

        double secondsTillDeadline= deadline.SubtractAnotherTimeFromMeInSeconds(now)+7*3600;
        deadLinePassed=(secondsTillDeadline<-18000);
      }
      //bool deadLinePassed=false;
      if (deadLinePassed)
        stOutput << "<span style=\"color:red\"> <b>Deadline has passed, no answer recorded.</b></span>";
      else
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
  }
#endif // MACRO_use_MySQL
  stOutput << "<table width=\"300\"><tr><td>";
  if (!isCorrect)
  { stOutput << "<span style=\"color:red\"><b>Your answer appears to be incorrect. </b></span>";
    if (theGlobalVariables.UserDefaultHasAdminRights() && theGlobalVariables.UserDebugFlagOn())
      stOutput << "For debugging: the calculator output is: " << theInterpreter.outputString
      << "Comments: " << theInterpreter.Comments.str();
  } else
    stOutput << "<span style=\"color:green\"><b>Correct! </b></span>";
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
  std::string errorMessage;
  for (int i=0; i< theProblem.studentAnswersUnadulterated.size; i++ )
  { stOutput << "\\(" << theProblem.studentAnswersUnadulterated[i] << "\\)";
    errorMessage=theInterpreter.ToStringIsCorrectAsciiCalculatorString(theProblem.studentAnswersUnadulterated[i]);
    if (errorMessage!="")
      stOutput << "<br>" << errorMessage
      << "<hr><b>If you entered this expression through the keyboard (without copying + pasting) this is a bug: "
      << "please report it to Todor Milev. Don't forget to mention your keyboard/character setup. "
      << "Are you using the standard English keyboard? Cyrillic, Chinese, etc. characters are not accepted. </b> "
      << "<hr><span style=\"color:red\"><b>Copying and pasting an answer not computed by yourself "
      << " is considered cheating (example: answer from an online program for doing homework).</b> </span>";
    if (i<theProblem.studentAnswersUnadulterated.size-1)
      stOutput << "<br>";
  }
  stOutput << "</td></tr>";

  stOutput << "</table>";
  stOutput << "Response time: " << theGlobalVariables.GetElapsedSeconds()-startTime << " second(s).";

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
  << "<head>"
  << CGI::GetCalculatorStyleSheetWithTags()
  << WebWorker::GetJavascriptStandardCookies()
  << "</head>"
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

std::string WebWorker::GetEditPageHTML()
{ MacroRegisterFunctionWithName("WebWorker::GetEditPageHTML");
  std::stringstream out;
  if (!theGlobalVariables.flagLoggedIn || !theGlobalVariables.UserDefaultHasAdminRights())
    return "<b>Only logged-in admins are allowed to edit pages. </b>";
  CalculatorHTML theFile;
  theFile.LoadFileNames();
  out << "<html>"
  << "<head>"
  << WebWorker::GetJavascriptStandardCookies()
//  << CGI::GetLaTeXProcessingJavascript()
//  << CGI::GetCalculatorStyleSheetWithTags()
  << theFile.GetJavascriptSubmitMainInputIncludeCurrentFile()
<< "<style type=\"text/css\" media=\"screen\">\n"
<< "    #editor { \n"
<< "      height: 400px;\n"
<< "      font-size: 100%;\n"
<< "   }\n"
<< "</style>\n"
<< "<script src=\"https://cdn.jsdelivr.net/ace/1.2.3/min/ace.js\" type=\"text/javascript\" charset=\"utf-8\"></script>\n"
//  << "<link rel=\"stylesheet\" href=\"//cdnjs.cloudflare.com/ajax/libs/highlight.js/9.2.0/styles/default.min.css\">"
//  << "<script src=\"//cdnjs.cloudflare.com/ajax/libs/highlight.js/9.2.0/highlight.min.js\"></script>"

  << "</head>"
  << "<body onload=\"loadSettings();\">\n";
  std::stringstream failureStream;
  if (!theFile.LoadMe(false, failureStream))
  { out << "<b>Failed to load file: " << theFile.fileName << ", perhaps the file does not exist. </b>";
    out << "</body></html>";
    return out.str();
  }
  if (!theFile.ParseHTML(failureStream))
  { out << "<b>Failed to parse file: " << theFile.fileName << ". Details:<br>" << failureStream.str();
    out << "</body></html>";
//    return out.str();
  }
  std::stringstream buttonStream, submitModPageJS;
  submitModPageJS
//  << "submitStringAsMainInput(document.getElementById('mainInput').value, 'spanSubmitReport', 'modifyPage');"
<< "submitStringAsMainInput(editor.getValue(), 'spanSubmitReport', 'modifyPage');"
  ;
  buttonStream
  << "<button "
  << "onclick=\"" << submitModPageJS.str() << "\" >Save changes</button>";
//  out << "<form>";
//  out << "<input type=\"submit\" value=\"Save changes\"> ";
  out << buttonStream.str();
  out << "To include the problem in your problem home page, add the following source code. <br>"
  << "<textarea cols=\"70\", rows=\"3\">"
  << theFile.ToStringCalculatorProblemSourceFromFileName(theFile.fileName) << "</textarea>";
  out << "<br>\n";
  out << "Ctrl+S saves your changes. Edit bravely, all files are backed-up/stored in a svn history tree."
  << " You only risk losing your own changes.";
  out << "<br>\n";
  out
<< "Many thanks to the <a href=\"https://ace.c9.io\">ace editor</a> project. <br>"
<< "<div id=\"editor\" onkeydown=\"ctrlSPress(event);\" name=\"editor\">"
  //<< "<textarea cols=\"150\", rows=\"30\" id=\"mainInput\" name=\"mainInput\" onkeydown=\"ctrlSPress(event);\">"
;
  out
  //<< "</textarea>"
<< "</div>"
  << "\n<br>\n";
  out << "<script type=\"text/javascript\"> \n"
  << "function ctrlSPress(event){\n"
  << "   if (event.ctrlKey!=true)\n"
  << "     return;\n"
  << "   if (event.keyCode!=83)\n"
  << "     return;\n"
  << "   event.preventDefault();"
  << submitModPageJS.str() << "\n"
  << "}\n"
  << "</script>\n";
  out << "<script type=\"text/javascript\"> \n"
  //<< " document.getElementById('mainInput').value=decodeURIComponent(\""
<< " document.getElementById('editor').textContent=decodeURIComponent(\""
  << CGI::StringToURLString(theFile.inputHtml, false)
  << "\");\n"
<< "    var editor = ace.edit(\"editor\");\n"
<< "    editor.setTheme(\"ace/theme/chrome\");\n"
<< "    editor.getSession().setMode(\"ace/mode/xml\");\n"

  << "</script>\n";
  out << buttonStream.str();
  out << "<span id=\"spanSubmitReport\"></span><br>";
  out << theFile.ToStringLinkFromFileName(theFile.fileName);
//  out << "<input type=\"submit\" value=\"Save changes\">";
//  out << "</form>";
  out << "</body></html>";
  return out.str();
}

std::string WebWorker::GetExamPage()
{ MacroRegisterFunctionWithName("WebWorker::GetExamPage");
  CalculatorHTML theFile;
  std::stringstream out;
  out << "<html>"
  << "<head>"
  << WebWorker::GetJavascriptStandardCookies()
  << CGI::GetLaTeXProcessingJavascript()
  << CGI::GetCalculatorStyleSheetWithTags()
  << "</head>"
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
  std::stringstream out;
  out << theProblem.outputHtmlMain;
  out << "<hr>Time to parse html: " << std::fixed
  << theProblem.timeToParseHtml << " second(s). ";
  out << "<br>Intermediate interpretation times (per attempt): ";
  for (int i=0; i<theProblem.timeIntermediatePerAttempt.size; i++)
    for (int j=0; j<theProblem.timeIntermediateComments[i].size; j++ )
      out << "<br>" << theProblem.timeIntermediateComments[i][j]
      << ": " << theProblem.timeIntermediatePerAttempt[i][j] << " second(s)";
  out << "<br>Interpretation times (per attempt): "
  << theProblem.timePerAttempt.ToStringCommaDelimited();
  out << "<hr>Interpretation times (per attempt): "
  << theProblem.problemCommandsWithInbetweens;

  return output.AssignValue(out.str(), theCommands);
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
  if (this->tagKeysWithoutValue.size>0)
    out << " " << this->tagKeysWithoutValue[0];
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
  { if (this->flagUseMathMode)
    { out << "\\( ";
      if (this->flagUseDisplaystyleInMathMode)
        out << "\\displaystyle ";
    }
    out << this->interpretedCommand;
    if (this->flagUseMathMode)
      out << " \\)";
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

bool SyntacticElementHTML::IsInterpretedByCalculatorDuringPreparatioN()
{ if (this->syntacticRole!="command")
    return false;
  std::string tagClass=this->GetKeyValue("class");
  return tagClass=="calculator" || tagClass=="calculatorHidden" ||
  tagClass=="calculatorShowToUserOnly" ;
}

bool SyntacticElementHTML::IsInterpretedByCalculatorDuringSubmission()
{ if (this->syntacticRole!="command")
    return false;
  std::string tagClass=this->GetKeyValue("class");
  return tagClass=="calculator" || tagClass=="calculatorHidden"
;
}

bool SyntacticElementHTML::IsAnswer()
{ if (this->syntacticRole!="command")
    return false;
  return this->GetKeyValue("class")=="calculatorAnswer";
}

bool SyntacticElementHTML::IsHidden()
{ if (this->syntacticRole!="command")
    return false;
  std::string tagClass=this->GetKeyValue("class");
  return tagClass=="calculatorHidden" || tagClass== "calculatorHiddenIncludeInCommentsBeforeSubmission"
;
}

bool SyntacticElementHTML::IsAnswerOnGiveUp()
{ if (this->syntacticRole!="command")
    return false;
  std::string tagClass=this->GetKeyValue("class");
  return tagClass=="calculatorAnswerOnGiveUp"
;
}

bool SyntacticElementHTML::IsCommentBeforeSubmission()
{ if (this->syntacticRole!="command")
    return false;
  std::string tagClass=this->GetKeyValue("class");
  return tagClass=="calculatorCommentBeforeSubmission"
//  ||
//  tagClass=="calculatorHiddenIncludeInCommentsBeforeSubmission"
;
}

bool SyntacticElementHTML::IsCommentAfterSubmission()
{ if (this->syntacticRole!="command")
    return false;
  std::string tagClass=this->GetKeyValue("class");
  return tagClass=="calculatorCommentAfterSubmission"
;
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
  { if (!this->theContent[i].IsInterpretedByCalculatorDuringPreparatioN())
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
    if (this->theContent[i].IsInterpretedByCalculatorDuringSubmission())
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
  double startTime=theGlobalVariables.GetElapsedSeconds();
  this->PrepareCommands(theInterpreter, comments);

  theInterpreter.init();
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("calculator init time");

  //stOutput << "nput cmds: " << calculatorCommands.str();
  theInterpreter.Evaluate(this->problemCommandsWithInbetweens);
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("calculator evaluation time");
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
  std::string exerciseRequest="exercises";
  std::string studentView= theGlobalVariables.UserStudentViewOn() ? "true" : "false";
  if (theGlobalVariables.UserGuestMode())
    exerciseRequest="exercisesNoLogin";
  if (theGlobalVariables.UserGuestMode())
  { out << "<b>Guest mode</b><br>No points scored/deadlines. <hr>"
    << "<a href=\"" << theGlobalVariables.hopefullyPermanent_HTTPS_WebAdressOfServerExecutable
    << "?request=login\" "
//    << "onclick=\"javascript:event.target.port="
//    << theWebServer.httpSSLPort
//    << "\""
    << ">Log in</a><br>";
  }
  out << theGlobalVariables.ToStringNavigation();
  List<std::string> randomSeedContainer;
  randomSeedContainer.AddOnTop("randomSeed");
  std::string calcArgsNoPassExamDetails=
  theGlobalVariables.ToStringCalcArgsNoNavigation(&randomSeedContainer);
  if (theGlobalVariables.UserDefaultHasAdminRights())
  { if (theGlobalVariables.UserStudentViewOn())
      out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?"
      << this->ToStringCalculatorArgumentsForProblem(theGlobalVariables.userCalculatorRequestType, "false")
      << "\">Admin view</a><br>";
    else
    { if (this->databaseStudentSectionS.size==0)
        out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?"
        << this->ToStringCalculatorArgumentsForProblem
        (theGlobalVariables.userCalculatorRequestType, "true", "")
        << "\">Student view</a><br>";
      for (int i=0; i<this->databaseStudentSectionS.size; i++)
        if (this->databaseStudentSectionS[i]!="")
          out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?"
          << this->ToStringCalculatorArgumentsForProblem
          (theGlobalVariables.userCalculatorRequestType, "true", this->databaseStudentSectionS[i])
          << "\">Student view section " << this->databaseStudentSectionS[i] << " </a><br>";
    }
  }
  if (!this->flagIsExamHome)
  { out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?request="
    << exerciseRequest << "&"
    << calcArgsNoPassExamDetails
    << "studentView=" << studentView << "&"
    << "currentExamHome=" << CGI::StringToURLString(this->currentExamHomE) << "&" << "currentExamIntermediate=&"
    << "currentExamFile=" << CGI::StringToURLString(this->currentExamHomE) << "&\"> Course homework home </a><br>";
  } else
    out << "<b>Course homework home</b>";
  if (this->flagIsExamProblem && this->currentExamIntermediatE!="")
    out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?request=exercises&"
    << calcArgsNoPassExamDetails
    << "studentView=" << studentView << "&"
    << "currentExamHome=" << CGI::StringToURLString(this->currentExamHomE) << "&" << "currentExamIntermediate=&"
    << "currentExamFile=" << CGI::StringToURLString(this->currentExamIntermediatE) << "&\">&nbspCurrent homework. </a><br>";
  else if (this->flagIsExamIntermediate)
    out << "<b>&nbspCurrent homework</b><br>";
  if (this->flagIsExamProblem)
  { if (theGlobalVariables.userCalculatorRequestType=="exercises")
      out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?"
      << this->ToStringCalculatorArgumentsForProblem("examForReal", studentView)
      << "\">&nbsp&nbspStart exam for real</a><br>";
    else if (theGlobalVariables.userCalculatorRequestType=="examForReal")
      out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?"
      << this->ToStringCalculatorArgumentsForProblem("exercises", studentView)
      << "\">&nbsp&nbspExercise this problem type</a><br>";
  }
  if (this->flagIsExamProblem && this->flagParentInvestigated)
  { int indexInParent=this->problemListOfParent.GetIndex(this->fileName);
    if (indexInParent==-1)
    { out << "<b>Could not find the problem collection that contains this problem.</b><br>";
//      out << "<b>This is either a faulty link or a programming error. </b> "
      //out << "this->filename is: " << this->fileName << " and the problem list of the parent is: "
      //<< this->problemListOfParent.ToStringCommaDelimited();
    } else
    { if (indexInParent>0)
        out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?request="
        << theGlobalVariables.userCalculatorRequestType
        << "&" << calcArgsNoPassExamDetails
        << "studentView=" << studentView << "&"
        << "&currentExamHome=" << CGI::StringToURLString(this->currentExamHomE)
        << "&currentExamIntermediate=" << CGI::StringToURLString(this->currentExamIntermediatE)
        << "&currentExamFile=" << CGI::StringToURLString(this->problemListOfParent[indexInParent-1] )
        << "&\"> <-Previous </a><br>";
      if (indexInParent<this->problemListOfParent.size-1)
        out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?request="
        << theGlobalVariables.userCalculatorRequestType
        << "&" << calcArgsNoPassExamDetails
        << "studentView=" << studentView << "&"
        << "&currentExamHome=" << CGI::StringToURLString(this->currentExamHomE)
        << "&currentExamIntermediate=" << CGI::StringToURLString(this->currentExamIntermediatE)
        << "&currentExamFile=" << CGI::StringToURLString(this->problemListOfParent[indexInParent+1] )
        << "\"> Next-> </a><br>";
      else
        out << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?request="
        << theGlobalVariables.userCalculatorRequestType
        << "&" << calcArgsNoPassExamDetails
        << "studentView=" << studentView << "&"
        << "&currentExamHome=" << CGI::StringToURLString(this->currentExamHomE)
        << "&currentExamIntermediate="
        << "&currentExamFile=" << CGI::StringToURLString(this->currentExamHomE)
        << "&\">Last problem, return to course page.</a><br>";
    }
  }
  if (this->flagIsExamProblem &&
      (theGlobalVariables.userCalculatorRequestType=="exercises" ||
       theGlobalVariables.userCalculatorRequestType=="exercisesNoLogin"))
    out << "<hr><a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?"
    << this->ToStringCalculatorArgumentsForProblem(exerciseRequest, studentView, "", true)
    << "\">Link to this problem</a><br>";

  return out.str();
}

std::string CalculatorHTML::ToStringCalculatorArgumentsForProblem
(const std::string& requestType, const std::string& studentView,
 const std::string& studentSection, bool includeRandomSeedIfAppropriate)const
{ MacroRegisterFunctionWithName("WebWorker::ToStringCalculatorArgumentsForProblem");
  if (!theGlobalVariables.flagLoggedIn && !theGlobalVariables.UserGuestMode())
    return "";
  std::stringstream out;
  out << "request=" << requestType << "&" << theGlobalVariables.ToStringCalcArgsNoNavigation()
  << "currentExamHome=" << theGlobalVariables.GetWebInput("currentExamHome") << "&"
  << "currentExamIntermediate=" << theGlobalVariables.GetWebInput("currentExamIntermediate") << "&"
  << "currentExamFile=" << CGI::StringToURLString(this->fileName) << "&";
  if (!theGlobalVariables.UserGuestMode())
  { out << "studentView=" << studentView << "&";
    if (studentSection!="")
      out << "studentSection=" << CGI::StringToURLString(studentSection) << "&";
  }
  if (//theGlobalVariables.GetWebInput("randomSeed")=="" &&
      //theGlobalVariables.userCalculatorRequestType!="examForReal" &&
      includeRandomSeedIfAppropriate)
    out << "randomSeed=" << this->theProblemData.randomSeed << "&";
//  out << "currentExamFile=" << CGI::StringToURLString(this->fileName) << "&";
  return out.str();
}

std::string SyntacticElementHTML::GetTagClass()
{ return this->GetKeyValue("class");
}

#ifdef MACRO_use_MySQL

std::string DatabaseRoutines::ToStringClassDetails
(bool adminsOnly, List<List<std::string> >& userTable, List<std::string>& userLabels,
 const std::string& classFileName,
 HashedList<std::string, MathRoutines::hashString>& databaseSpanList,
 List<std::string>& databaseProblemWeights
 )
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringClassDetails");
  std::stringstream out;
  std::string userRole = adminsOnly ? "admin" : "student";
  int numUsers=0;
  std::stringstream tableStream;
  tableStream << "<table><tr><th>User</th><th>Email</th><th>Activated?</th><th>Activation link</th>"
  << "<th>Activation manual email</th>"
  << "<th>Points</th><th>Section/Group</th></tr>";
  UserCalculator currentUser;
  currentUser.currentTable="users";

  int indexUser=-1, indexExtraInfo=-1;
  for (int i=0; i<userLabels.size; i++)
  { if (userLabels[i]=="username")
      indexUser=i;
    if (userLabels[i]=="extraInfo")
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
  }
  HashedList<std::string, MathRoutines::hashString> sectionNames;
  List<List<std::string> > activatedAccountBucketsBySection;
  List<List<std::string> > nonActivatedAccountBucketsBySection;
  for (int i=0; i<userTable.size; i++)
    sectionNames.AddOnTopNoRepetition(userTable[i][indexExtraInfo]);
  sectionNames.QuickSortAscending();
  activatedAccountBucketsBySection.SetSize(sectionNames.size);
  nonActivatedAccountBucketsBySection.SetSize(sectionNames.size);
  int numActivatedUsers=0;
  for (int i=0; i<userTable.size; i++)
  { std::stringstream failureStream;
    currentUser.username=userTable[i][indexUser];
    if (!currentUser.FetchOneUserRow(*this, failureStream ))
    { currentUser.email=failureStream.str();
      currentUser.activationToken="error";
      currentUser.userRole="error";
    }
    if (adminsOnly xor (currentUser.userRole=="admin"))
      continue;
    numUsers++;
    std::stringstream oneTableLineStream;
    oneTableLineStream << "<tr>"
    << "<td>" << userTable[i][indexUser] << "</td>"
    << "<td>" << currentUser.email.value << "</td>"
    ;
    bool isActivated=true;
    if (currentUser.activationToken!="activated" && currentUser.activationToken!="error")
    { isActivated=false;
      numActivatedUsers++;
      oneTableLineStream << "<td><span style=\"color:red\">not activated</span></td>";
      if (currentUser.activationToken!="")
        oneTableLineStream << "<td>"
        << "<a href=\""
        << UserCalculator::GetActivationAddressFromActivationToken
        (currentUser.activationToken.value, theGlobalVariables.DisplayNameCalculatorWithPath,
         userTable[i][indexUser])
        << "\"> (Re)activate account and change password</a>"
        << "</td>";
      oneTableLineStream << "<td>";
      oneTableLineStream
      << "<a href=\"mailto:" << currentUser.email.value
      << "?subject=Math 140 Homework account activation&";

      oneTableLineStream << "body=";
      std::stringstream emailBody;
      emailBody << "Dear student,\n you have not activated your homework server account yet. \n"
      << "To activate your account and set your password please use the link: "
      << CGI::StringToURLString("\n\n")
      << CGI::StringToURLString( UserCalculator::GetActivationAddressFromActivationToken
        (currentUser.activationToken.value, theGlobalVariables.hopefullyPermanent_HTTPS_WebAdressOfServerExecutable,
         userTable[i][indexUser]) )
      << CGI::StringToURLString("\n\n")
      << "The link does not work with apple safari; if you use safari, please contact us by email"
      << " and we will activate your account manually. "
      << " Once you activate your account, you can log in safely here: \n"
      << CGI::StringToURLString("\n\n")
      << theGlobalVariables.hopefullyPermanent_HTTPS_WebAdressOfServerExecutable
      << CGI::StringToURLString("\n\n")
      << "\n\nIf using the Safari browser/apple, you can use this (a little less safe link): "
      << CGI::StringToURLString("\n\n")
      << CGI::StringToURLString(theGlobalVariables.hopefullyPermanentWebAdressOfServerExecutable + "?request=login&ignoreSecurity=true")
      << CGI::StringToURLString("\n\n")
      << "Best regards, \n your Math 140 instructors."
      ;
      oneTableLineStream << emailBody.str() << "\">Send email manually.</a> "
      ;
      oneTableLineStream << "</td>";
      //      else
//        oneTableLineStream << "<td>Activation token: " << currentUser.activationToken.value << "</td>";
    } else if (currentUser.activationToken=="error")
      oneTableLineStream << "<td>error</td><td></td>";
    else
    { oneTableLineStream << "<td><span style=\"color:green\">activated</span></td><td></td><td></td>";
/*      oneTableLineStream << "<td><span style=\"color:red\">"
      << "<a href=\""
      << UserCalculator::GetActivationAbsoluteAddressFromActivationToken(currentUser.activationToken.value, userTable[i][indexUser])
      << "\"> (Re)activate account and change password</a>"
      << "</span></td>";*/
    }
    int indexProblemData=currentUser.selectedRowFieldNamesUnsafe.GetIndex("problemData");
    std::stringstream commentsProblemData;
    if (indexProblemData==-1)
      oneTableLineStream << "<td>No solutions history</td>";
    else if (currentUser.InterpretDatabaseProblemData
             (currentUser.selectedRowFieldsUnsafe[indexProblemData], commentsProblemData))
    { currentUser.ComputePointsEarned(databaseSpanList, databaseProblemWeights);
      oneTableLineStream << "<td>" << std::fixed << std::setw(1)
      << std::setprecision(1) << currentUser.pointsEarned.GetDoubleValue() << "</td>";
    } else
      oneTableLineStream << "<td>Failed to load problem data. Comments: " << commentsProblemData.str() << "</td>";
    oneTableLineStream << "<td>" << userTable[i][indexExtraInfo] << "</td>";
    oneTableLineStream << "</tr>";
    if (isActivated)
      activatedAccountBucketsBySection[sectionNames.GetIndex(userTable[i][indexExtraInfo])].AddOnTop(oneTableLineStream.str());
    else
      nonActivatedAccountBucketsBySection[sectionNames.GetIndex(userTable[i][indexExtraInfo])].AddOnTop(oneTableLineStream.str());
  }
  for (int i=0; i<nonActivatedAccountBucketsBySection.size; i++)
    nonActivatedAccountBucketsBySection[i].QuickSortAscending();
  for (int i=0; i<activatedAccountBucketsBySection.size; i++)
    activatedAccountBucketsBySection[i].QuickSortAscending();
  for (int i=0; i<nonActivatedAccountBucketsBySection.size; i++)
    for (int j=0; j<nonActivatedAccountBucketsBySection[i].size; j++)
      tableStream << nonActivatedAccountBucketsBySection[i][j];
  for (int i=0; i<activatedAccountBucketsBySection.size; i++)
    for (int j=0; j<activatedAccountBucketsBySection[i].size; j++)
      tableStream << activatedAccountBucketsBySection[i][j];
  tableStream << "</table>";
  out << "\n" << numUsers << " user(s)";
  if (numActivatedUsers>0)
    out << ", <span style=\"color:red\">" << numActivatedUsers << " have not activated their accounts";
  out << ".";
  out << tableStream.str();
  return out.str();
}
#endif // MACRO_use_MySQL

std::string CalculatorHTML::ToStringClassDetails
( bool adminsOnly,
  const SyntacticElementHTML& inputElement
)
{ MacroRegisterFunctionWithName("CalculatorHTML::ToStringClassDetails");
  std::stringstream out;
#ifdef MACRO_use_MySQL
  std::string idAddressTextarea= adminsOnly ? "inputAddAdmins" : "inputAddStudents";
  std::string idExtraTextarea= adminsOnly ? "inputAddAdminsExtraInfo" : "inputAddStudentsExtraInfo";
  if (inputElement.GetKeyValue("id")!="")
  { idAddressTextarea+= inputElement.GetKeyValue("id");
    idExtraTextarea+=inputElement.GetKeyValue("id");
  } else
  { idAddressTextarea+=this->fileName;
    idExtraTextarea+=this->fileName;
  }
  std::string userRole = adminsOnly ? "admin" : "student";
  std::string idOutput="outputAdd";
  if (adminsOnly)
    idOutput+="Admins";
  else
    idOutput+="Students";
  idOutput+= CGI::StringToURLString(this->fileName);
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
  out << "<br>";
  out
  << "<button class=\"submitButton\" "
//  << "onclick=\"addEmailsOrUsers("
//  << "'"    << idAddressTextarea
//  << "', '" << CGI::StringToURLString(this->fileName)
//  << "', '" << idOutput
//  << "', '" << userRole
//  << "', '" << idExtraTextarea
//  << "', 'addEmails'"
//  << " )\" "
  << "disabled> [Disabled] Add emails</button>";
  out
  << "<button class=\"submitButton\" onclick=\"addEmailsOrUsers("
  << "'"    << idAddressTextarea
  << "', '" << CGI::StringToURLString(this->fileName)
  << "', '" << idOutput
  << "', '" << userRole
  << "', '" << idExtraTextarea
  << "', 'addUsers'"
  << " )\"> Add users (no email sending)</button> ";
  out << "<br><span id=\"" << idOutput << "\">\n";
  DatabaseRoutines theRoutines;
  out << theRoutines.ToStringClassDetails
  (adminsOnly, this->userTablE, this->labelsUserTablE, this->fileName,
   this->databaseProblemAndHomeworkGroupList, this->databaseProblemWeights);
  out << "</span>";
#else
  out << "<b>Adding emails not available (database not present).</b> ";
#endif // MACRO_use_MySQL

  return out.str();
}

#ifdef MACRO_use_MySQL
bool DatabaseRoutines::PrepareClassData
(const std::string& classFileName, List<List<std::string> >& outputUserTable,
 List<std::string>& outputLabelsUserTable,
  std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareClassData");
  DatabaseRoutines theRoutines;
  if (!theRoutines.startMySQLDatabaseIfNotAlreadyStarted(&commentsOnFailure))
  { commentsOnFailure << "<b>Failed to start database</b>";
    return false;
  }
  std::string classTableName=DatabaseRoutines::GetTableUnsafeNameUsersOfFile(classFileName);
  if (!theRoutines.TableExists(classTableName, &commentsOnFailure))
    if (!theRoutines.CreateTable
        (classTableName, "username VARCHAR(255) NOT NULL PRIMARY KEY, \
        extraInfo LONGTEXT ", &commentsOnFailure, 0))
      return false;
  bool tableTruncated=false;
  int numRows=-1;
  if (!DatabaseRoutinesGlobalFunctions::FetchTablE
      (outputUserTable, outputLabelsUserTable, tableTruncated, numRows,
       classTableName, commentsOnFailure))
  { commentsOnFailure << "<span style=\"color:red\"><b>Failed to fetch table: "
    << classTableName << "</b></span>";
    return false;
  }
  if (tableTruncated)
  { commentsOnFailure << "<span style=\"color:red\"><b>This shouldn't happen: email list truncated. "
    << "This is likely a software bug.</b></span>";
    return false;
  }
  return true;
}
#endif // MACRO_use_MySQL

bool CalculatorHTML::PrepareClassData(std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("CalculatorHTML::PrepareClassData");
#ifdef MACRO_use_MySQL
  DatabaseRoutines theRoutines;
  if (!theRoutines.PrepareClassData(this->fileName, this->userTablE, this->labelsUserTablE, commentsOnFailure))
    return false;
  int indexExtraInfo=-1;
  for (int i=0; i<this->labelsUserTablE.size; i++)
    if (this->labelsUserTablE[i]=="extraInfo")
      indexExtraInfo=i;
  if (indexExtraInfo==-1)
  { commentsOnFailure << "Failed to load section data. ";
    return false;
  }
  HashedList<std::string, MathRoutines::hashString> theSections;
  for (int i=0; i<this->userTablE.size; i++)
    theSections.AddOnTopNoRepetition(this->userTablE[i][indexExtraInfo]);
  this->databaseStudentSectionS=theSections;
  this->databaseStudentSectionS.QuickSortAscending();
  return true;
#else
  commentsOnFailure << "Error: database not running. ";
  return false;
#endif // MACRO_use_MySQL

}

void CalculatorHTML::InterpretManageClass(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretManageClass");
  if (!theGlobalVariables.UserDefaultHasAdminRights() || theGlobalVariables.UserStudentViewOn())
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
    out << "<td>" << inputOutput.ToStringOpenTag() << inputOutput.ToStringCloseTag() ;// << "</td>";
    //out << "<td>";
    out << "<button class=\"submitButton\" onclick=\"submitAnswers('"
    << answerId << "', '" << answerEvaluationId << "')\"> Submit </button>";
    if (!this->flagIsForReal)
    { int theIndex=this->theProblemData.answerIds.GetIndex(answerId);
      bool hasShowAnswerButton=false;
      if (theIndex!=-1)
        if (this->theProblemData.commandsForGiveUpAnswer[theIndex]!="")
        { out << "<button class=\"submitButton\" onclick=\"giveUp('"
          << answerId << "', '" << answerEvaluationId << "')\"> Show answer </button>";
          hasShowAnswerButton=true;
        }
      if (!hasShowAnswerButton)
        out << "No ``give-up'' answer available. ";
    }
    out << "</td>";
    out << "</tr><tr>";
    out << "<td>";
    out << "<span id=\"" << answerEvaluationId << "\">";

    int theIndex=this->theProblemData.answerIds.GetIndex(answerId);
    if (theIndex==-1)
      crash << "Index of answer id not found: this shouldn't happen. " << crash;
    int numCorrectSubmissions=0;
    int numSubmissions= 0;
    if (theIndex >=this->theProblemData.numCorrectSubmissions.size ||
        theIndex >=this->theProblemData.numSubmissions.size ||
        theIndex >=this->theProblemData.firstCorrectAnswer.size)
      out << "<b>Error: the problemData index is " << theIndex << " but "
      << "  numCorrectSubmissions.size is: " << this->theProblemData.numCorrectSubmissions.size
      << ", numSubmissions.size is: " << this->theProblemData.numSubmissions.size
      << ", firstCorrectAnswer.size is: " << this->theProblemData.firstCorrectAnswer.size
      << ". </b>";
    if (theIndex!=-1)
    { numCorrectSubmissions= this->theProblemData.numCorrectSubmissions[theIndex];
      numSubmissions= this->theProblemData.numSubmissions[theIndex];
    }
//    stOutput << "got to here 2";
    if (theGlobalVariables.userCalculatorRequestType=="examForReal")
    { if (numCorrectSubmissions >0)
      { out << "<b><span style=\"color:green\">Correctly answered: \\("
        << this->theProblemData.firstCorrectAnswer[theIndex] << "\\) </span></b> ";
        if (numSubmissions>0)
          out << "<br>Used: " << numSubmissions << " attempt(s) (" << numCorrectSubmissions << " correct).";
      } else
      { out << " <b><span style=\"color:brown\">Need to submit answer. </span></b>";
        if (numSubmissions>0)
          out << numSubmissions << " attempt(s) so far. ";
      }
    } else
      out << " <b><span style=\"color:brown\">Submit (no credit, unlimited tries)</span></b>";
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

std::string CalculatorHTML::CleanUpFileName(const std::string& inputLink)
{ unsigned firstMeaningfulChar=0;
  for (; firstMeaningfulChar<inputLink.size(); firstMeaningfulChar++)
    if (inputLink[firstMeaningfulChar]!='\n' &&
        inputLink[firstMeaningfulChar]!='\r' &&
        inputLink[firstMeaningfulChar]!='\t' &&
        inputLink[firstMeaningfulChar]!=' ')
      break;
  unsigned lastMeaningfulChar=inputLink.size()-1;
  for (;lastMeaningfulChar>firstMeaningfulChar; lastMeaningfulChar--)
    if (inputLink[lastMeaningfulChar]!='\n' &&
        inputLink[lastMeaningfulChar]!='\r' &&
        inputLink[lastMeaningfulChar]!='\t' &&
        inputLink[lastMeaningfulChar]!=' ')
      break;
  if (firstMeaningfulChar>=inputLink.size())
    return "";
  return inputLink.substr(firstMeaningfulChar, lastMeaningfulChar-firstMeaningfulChar+1);
}

#include "vpfHeader5Crypto.h"
std::string CalculatorHTML::InterpretGenerateProblemManagementLink
(SyntacticElementHTML& inputOutput, std::stringstream& refStreamForReal, std::stringstream& refStreamExercise,
 const std::string& cleaneduplink, const std::string& urledProblem)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretGenerateProblemManagementLink");
  std::stringstream out;
  if (!theGlobalVariables.UserGuestMode())
    out << " <a href=\"" << refStreamForReal.str() << "\">Start (for credit)</a> ";
  out << " <a href=\"" << refStreamExercise.str() << "\">Exercise (no credit, unlimited tries)</a> ";
  if (theGlobalVariables.UserGuestMode())
  { out << "Exercise points/deadlines require login. ";
    return out.str();
  }
  //stOutput << "<hr>CurrentUser.problemNames=" << this->currentUser.problemNames.ToStringCommaDelimited();
  std::string thePoints="";
  if (this->databaseProblemAndHomeworkGroupList.Contains(cleaneduplink))
    thePoints= this->databaseProblemWeights[this->databaseProblemAndHomeworkGroupList.GetIndex(cleaneduplink)];
  #ifdef MACRO_use_MySQL
  bool noSubmissionsYet=false;
  bool weightPrinted=false;
  if (this->currentUser.problemNames.Contains(cleaneduplink))
  { ProblemData& theProbData=this->currentUser.problemData[this->currentUser.problemNames.GetIndex(cleaneduplink)];
    if (!theProbData.flagProblemWeightIsOK)
    { out << "<span style=\"color:orange\">No point weight assigned yet. </span>";
      if (theProbData.ProblemWeightUserInput!="")
        out << "<span style=\"color:red\"><b>Failed to interpret weight string: "
        << theProbData.ProblemWeightUserInput << ". </b></span>";

      if (theProbData.numSubmissions!=0)
      { if (theProbData.answerIds.size==1)
        { if (theProbData.numCorrectlyAnswered==1)
            out << theProbData.totalNumSubmissions << " submission(s), problem correctly answered. ";
          else
            out << theProbData.totalNumSubmissions << " submission(s), problem not correctly answered yet. ";
        } else if (theProbData.answerIds.size>1)
          out << theProbData.totalNumSubmissions << " submission(s), " << theProbData.numCorrectlyAnswered
          << " out of "<< theProbData.answerIds.size << " subproblems correctly answered. ";
      }
      weightPrinted=true;
    } else if (theProbData.totalNumSubmissions==0)
      noSubmissionsYet=true;
    else if (theProbData.numCorrectlyAnswered<theProbData.answerIds.size)
    { out << "<span style=\"color:red\"><b> "
      << theProbData.Points << " out of " << theProbData.ProblemWeight << " point(s). </b></span>";
      weightPrinted=true;
    } else if (theProbData.numCorrectlyAnswered==theProbData.answerIds.size)
    { out << "<span style=\"color:green\"><b> "
      << theProbData.Points << " out of " << theProbData.ProblemWeight << " point(s). </b></span>";
      weightPrinted=true;
    }
  } else
    noSubmissionsYet=true;
  if (thePoints!="" && noSubmissionsYet)
  { out << "<span style=\"color:brown\"><b>No submissions: 0 out of " << thePoints
    << " point(s). </b> </span>" ;
    weightPrinted=true;
  }
  if (!weightPrinted)
    out << "<span style=\"color:orange\">No point weight assigned yet. </span>";
  if (theGlobalVariables.UserDefaultHasAdminRights() && !theGlobalVariables.UserStudentViewOn())
  { //stOutput << "<hr>this->databaseProblemList is: " << this->databaseProblemList.ToStringCommaDelimited();
    //stOutput << "<br>this->databaseProblemWeights is: " << this->databaseProblemWeights.ToStringCommaDelimited();
    //stOutput << "<br> cleanedupLink: " << cleaneduplink;
    std::string idPoints = "points" + urledProblem;
    std::string idNumTries= "numTries"+urledProblem;
    std::string idButtonModifyPoints = "modifyPoints" + urledProblem;
    std::string idPointsModOutput = "modifyPointsOutputSpan" + urledProblem;
    out << "Points: <textarea rows=\"1\" cols=\"3\" id=\"" << idPoints << "\">";
    out << thePoints;
    out << "</textarea>";
    out << "<button id=\"" << idButtonModifyPoints << "\" "
    << "onclick=\"" << "submitStringAsMainInput('" << urledProblem
    << "='+encodeURIComponent('weight='+  getElementById('" << idPoints << "').value)"
//    << "  +encodeURIComponent('numTries='+getElementById('"
    << ", '"
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

std::string CalculatorHTML::GetDeadline
(const std::string& problemName, const std::string& sectionNumber, bool inheritFromGroup, bool& outputIsInherited)
{ MacroRegisterFunctionWithName("CalculatorHTML::GetDeadline");
  outputIsInherited=false;
  std::string result;
//  stOutput << "Fetchign deadline for: " << problemName;
  int indexInDatabase=this->databaseProblemAndHomeworkGroupList.GetIndex(problemName);
  if (indexInDatabase!=-1)
  { int indexSection=  this->databaseStudentSectionsPerProblem[indexInDatabase].GetIndex(sectionNumber);
    if (indexSection!=-1)
      result=this->databaseDeadlinesBySection[indexInDatabase][indexSection];
  }
//  stOutput << "Index in db is: " << indexInDatabase << " result:" << result << "Inherit frm grp:"
//  << inheritFromGroup;
  if (result!="" || !inheritFromGroup)
    return result;
//  stOutput << "hd prob list: " << this->hdProblemList.ToStringCommaDelimited();
  int indexInHDproblemList=this->hdProblemList.GetIndex(problemName);
  if (indexInHDproblemList==-1)
    return "";
//  stOutput << "got to hw group phase for prob: " << problemName;
  const std::string& hwGroup=this->hdHomeworkGroupCorrespondingToEachProblem[indexInHDproblemList];
  indexInDatabase=this->databaseProblemAndHomeworkGroupList.GetIndex(hwGroup);
  if (indexInDatabase!=-1)
  { int indexSection=  this->databaseStudentSectionsPerProblem[indexInDatabase].GetIndex(sectionNumber);
    if (indexSection!=-1)
    { result=this->databaseDeadlinesBySection[indexInDatabase][indexSection];
      outputIsInherited=true;
    }
  }
  return result;
}

std::string CalculatorHTML::ToStringOnEDeadlineFormatted
  (const std::string& cleanedUpLink,  const std::string& sectionNumber, bool isActualProblem,
   bool problemAlreadySolved)
{ bool deadlineInherited=false;
  std::stringstream out;
  std::string currentDeadline =
  this->GetDeadline(cleanedUpLink, sectionNumber, true, deadlineInherited);
  if (currentDeadline=="")
  { if (isActualProblem)
      out << "<span style=\"color:orange\">No deadline yet. </span>";
    return out.str();
  }
#ifdef MACRO_use_MySQL
  TimeWrapper now, deadline; //<-needs a fix for different time formats.
  //<-For the time being, we hard-code it to month/day/year format (no time to program it better).
  std::stringstream badDateStream;
  if (!deadline.AssignMonthDayYear(currentDeadline, badDateStream))
    out << "<span style=\"color:red\">" << badDateStream.str() << "</span>";
  //  out << "deadline.date: " << deadline.theTime.tm_mday;
  now.AssignLocalTime();
  //  out << "Now: " << asctime (&now.theTime) << " mktime: " << mktime(&now.theTime)
  //  << " deadline: " << asctime(&deadline.theTime) << " mktime: " << mktime(&deadline.theTime);
  double secondsTillDeadline= deadline.SubtractAnotherTimeFromMeInSeconds(now)+7*3600;

  std::stringstream hoursTillDeadlineStream;
  if (secondsTillDeadline>0)
  { if (secondsTillDeadline<24*3600 && !problemAlreadySolved)
      hoursTillDeadlineStream << "<span style=\"color:red\">"
      << TimeWrapper::ToStringSecondsToDaysHoursSecondsString(secondsTillDeadline, false) << "</span>";
    else
      hoursTillDeadlineStream << TimeWrapper::ToStringSecondsToDaysHoursSecondsString(secondsTillDeadline, false)
      << " left. ";
  } else
    hoursTillDeadlineStream << "Deadline has passed. ";
  if (isActualProblem)
  { out << "Deadline: ";
    if (deadlineInherited)
      out << "<span style=\"color:blue\">" << currentDeadline << " (default)</span>. ";
    else
      out << "<span style=\"color:brown\">" << currentDeadline << " (individual deadline)</span>. ";
    out << hoursTillDeadlineStream.str();
    return out.str();
  }
  if (!theGlobalVariables.UserDefaultHasAdminRights() || theGlobalVariables.UserStudentViewOn())
    return out.str();
  out << "<span style=\"color:blue\">" << currentDeadline << "</span>. ";
  out << hoursTillDeadlineStream.str();
#else
  out  << "Database not running: no deadlines";
#endif // MACRO_use_MySQL
  return out.str();
}

std::string CalculatorHTML::ToStringDeadlinesFormatted
  (const std::string& cleanedUpLink,  const List<std::string>& sectionNumbers, bool isActualProblem,
   bool problemAlreadySolved)
{ if (sectionNumbers.size==0)
    return "No section number. ";
  if (sectionNumbers.size==1)
    return this->ToStringOnEDeadlineFormatted
    (cleanedUpLink, sectionNumbers[0], isActualProblem, problemAlreadySolved);
  std::stringstream out;
  out << "<table>";
  for (int i=0; i<sectionNumbers.size; i++)
  { if (sectionNumbers[i]=="")
      continue;
    out << "<tr><td>Section " << sectionNumbers[i] << ":</td>";
    out << "<td>" << this->ToStringOnEDeadlineFormatted
    (cleanedUpLink, sectionNumbers[i], isActualProblem, problemAlreadySolved) << "</td>";
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

std::string CalculatorHTML::InterpretGenerateDeadlineLink
(SyntacticElementHTML& inputOutput, std::stringstream& refStreamForReal, std::stringstream& refStreamExercise,
 const std::string& cleaneduplink, const std::string& urledProblem, bool problemAlreadySolved)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretGenerateDeadlineLink");
  //  return "Submission deadline: to be announced. ";
  if (theGlobalVariables.UserGuestMode())
    return "";
  std::stringstream out;
  bool deadlineInherited=false;
  bool isActualProblem=(inputOutput.GetTagClass()=="calculatorExamProblem");
#ifdef MACRO_use_MySQL
  if (isActualProblem)
  { if (!theGlobalVariables.UserDefaultHasAdminRights())
      out << this->ToStringOnEDeadlineFormatted
      (cleaneduplink, this->currentUser.extraInfoUnsafe, isActualProblem, problemAlreadySolved);
    else if (!theGlobalVariables.UserStudentViewOn())
      out << this->ToStringDeadlinesFormatted
      (cleaneduplink, this->databaseStudentSectionS, isActualProblem, problemAlreadySolved);
    else
      out << this->ToStringDeadlinesFormatted
      (cleaneduplink,
       CGI::URLStringToNormal(theGlobalVariables.GetWebInput("studentSection")),
       isActualProblem, problemAlreadySolved);
  }
#endif // MACRO_use_MySQL
  if (!theGlobalVariables.UserDefaultHasAdminRights() || theGlobalVariables.UserStudentViewOn())
    return out.str();
  std::stringstream deadlineStream;
  deadlineStream << "<table><tr><td> Deadline: </td>";
  deadlineStream << "<td><table><tr><th>Grp.</th><th>Deadline</th></tr>";
  List<std::string> deadlineIds;
  deadlineIds.SetSize(this->databaseStudentSectionS.size);
  for (int i=0; i<this->databaseStudentSectionS.size; i++)
  { std::string& currentDeadlineId=deadlineIds[i];
    if (this->databaseStudentSectionS[i]=="")
      continue;
    currentDeadlineId = "deadline" + Crypto::CharsToBase64String(this->databaseStudentSectionS[i]+cleaneduplink);
    if (currentDeadlineId[currentDeadlineId.size()-1]=='=')
      currentDeadlineId.resize(currentDeadlineId.size()-1);
    if (currentDeadlineId[currentDeadlineId.size()-1]=='=')
      currentDeadlineId.resize(currentDeadlineId.size()-1);
    deadlineStream << "<tr>";
    deadlineStream << "<td>" << this->databaseStudentSectionS[i] << "</td>";
    deadlineStream << "<td> <input type=\"text\" id=\"" << currentDeadlineId << "\" value=\""
    << this->GetDeadline(cleaneduplink, this->databaseStudentSectionS[i], false, deadlineInherited)
    << "\"> " ;
    deadlineStream << this->GetDatePickerStart(currentDeadlineId);
    deadlineStream << "</td>";
    deadlineStream << "</tr>";
  }
  deadlineStream << "</table></td>";
  deadlineStream << "<td>\n";
  std::string deadlineIdReport="deadlineReport"+Crypto::CharsToBase64String(cleaneduplink);
  if (deadlineIdReport[deadlineIdReport.size()-1]=='=')
    deadlineIdReport.resize(deadlineIdReport.size()-1);
  if (deadlineIdReport[deadlineIdReport.size()-1]=='=')
    deadlineIdReport.resize(deadlineIdReport.size()-1);
  deadlineStream << "<button onclick=\"";
  deadlineStream << "submitStringAsMainInput('" << urledProblem
  << "='+encodeURIComponent('deadlines='+encodeURIComponent(";
  bool isFirst=true;
  for (int i=0; i<this->databaseStudentSectionS.size; i++)
  { if (this->databaseStudentSectionS[i]=="")
      continue;
    if (!isFirst)
      deadlineStream << "+";
    isFirst=false;
    deadlineStream << "'" << CGI::StringToURLString(this->databaseStudentSectionS[i]) << "='";
    deadlineStream << "+ encodeURIComponent(document.getElementById('"
    << deadlineIds[i] << "').value)+'&'";
  }
  deadlineStream << ")), '" << deadlineIdReport << "', 'setProblemData' );"
  << "\""
  << ">\n";
  deadlineStream << "  Set deadline(s)</button>";
  deadlineStream << "<span id=\"" << deadlineIdReport << "\"></span>";
  deadlineStream << "</td>";
  deadlineStream << "</tr></table>";
//  out << deadlineStream.str();
  out << CGI::GetHtmlSpanHidableStartsHiddeN(deadlineStream.str(), "Set deadline (show/hide). ");
  if (isActualProblem)
    out << "(overrides default). ";
  else
    out << "(overridden by individual deadlines). ";
  return out.str();
}

void CalculatorHTML::InterpretGenerateLink(SyntacticElementHTML& inputOutput)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretGenerateLink");
  this->NumProblemsFound++;
  std::string cleaneduplink = this->CleanUpFileName(inputOutput.content);
  std::string urledProblem=CGI::StringToURLString(cleaneduplink);
  std::stringstream out, refStreamNoRequest, refStreamExercise, refStreamForReal;
//  out << "<span style=\"white-space: nowrap; display= inline-block; width=1200px; overflow-x: scroll;\">";
//  out << "cleaned up link: " << cleaneduplink;
//  out << "<br>urled link: " <<  urledProblem;
  refStreamNoRequest << theGlobalVariables.ToStringCalcArgsNoNavigation()
  << "currentExamFile=" << urledProblem << "&";
  if (this->currentExamHomE!="")
    refStreamNoRequest << "currentExamHome=" << this->currentExamHomE << "&";
  if (this->flagIsExamIntermediate)
    refStreamNoRequest << "currentExamIntermediate="
    << theGlobalVariables.GetWebInput("currentExamIntermediate") << "&";
  if (!theGlobalVariables.UserGuestMode())
  { refStreamExercise << theGlobalVariables.DisplayNameCalculatorWithPath << "?request=exercises&" << refStreamNoRequest.str();
    refStreamForReal << theGlobalVariables.DisplayNameCalculatorWithPath << "?request=examForReal&" << refStreamNoRequest.str();
  } else
  { refStreamExercise << theGlobalVariables.DisplayNameCalculatorWithPath
    << "?request=exercisesNoLogin&" << refStreamNoRequest.str();
  }
  if (inputOutput.GetTagClass()=="calculatorExamProblem")
    out << this->InterpretGenerateProblemManagementLink
    (inputOutput, refStreamForReal, refStreamExercise, cleaneduplink, urledProblem);
  //else
  //  out << " <a href=\"" << refStreamExercise.str() << "\">Start</a> ";
  //if (inputOutput.GetTagClass()=="calculatorExamIntermediate")
#ifdef MACRO_use_MySQL
  bool problemAlreadySolved=false;
  if (this->currentUser.problemNames.Contains(cleaneduplink))
  { ProblemData& theProbData=this->currentUser.problemData[this->currentUser.problemNames.GetIndex(cleaneduplink)];
    if (theProbData.numCorrectlyAnswered>=theProbData.answerIds.size)
      problemAlreadySolved=true;
//    out << " probdata crct: " << theProbData.numCorrectlyAnswered << " # answ id: " << theProbData.answerIds.size;
  }
  out << this->InterpretGenerateDeadlineLink
  (inputOutput, refStreamForReal, refStreamExercise, cleaneduplink, urledProblem, problemAlreadySolved);
#endif // MACRO_use_MySQL
  std::string stringToDisplay = FileOperations::GetFileNameFromFileNameWithPath(inputOutput.content);
  //out << " " << this->NumProblemsFound << ". "
  out << stringToDisplay;
//  out << "</span>";
  inputOutput.interpretedCommand=out.str();
}

bool CalculatorHTML::InterpretHtmlOneAttempt(Calculator& theInterpreter, std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretHtmlOneAttempt");
  double startTime=theGlobalVariables.GetElapsedSeconds();
  std::stringstream out;
  if (!this->flagIsForReal || !this->theProblemData.flagRandomSeedComputed)
    if (!this->flagRandomSeedGiven || this->NumAttemptsToInterpret>1)
      this->theProblemData.randomSeed=this->randomSeedsIfInterpretationFails[this->NumAttemptsToInterpret-1];
  this->FigureOutCurrentProblemList(comments);
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time before after loading problem list");
  out << this->GetJavascriptSubmitMainInputIncludeCurrentFile();
  if (this->flagIsExamProblem)
    out << this->GetSubmitAnswersJavascript();
  else if (this->flagIsExamHome)
    out << this->GetSubmitEmailsJavascript();
  if ((this->flagIsExamIntermediate || this->flagIsExamHome)
      &&  theGlobalVariables.UserDefaultHasAdminRights() && !theGlobalVariables.UserStudentViewOn())
  { out << WebWorker::GetJavascriptHideHtml();
    out << this->GetDatePickerJavascriptInit();
  }
  if (theGlobalVariables.UserDefaultHasAdminRights() && !theGlobalVariables.UserStudentViewOn())
    out << this->GetEditPageButton();
//  else
//    out << " no date picker";
  theInterpreter.flagWriteLatexPlots=false;
  FormatExpressions theFormat;
  theFormat.flagExpressionIsFinal=true;
  theFormat.flagIncludeExtraHtmlDescriptionsInPlots=false;
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time before execution");
  if (!this->PrepareAndExecuteCommands(theInterpreter, comments))
    return false;
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time after execution");
  bool moreThanOneCommand=false;
  std::string lastCommands;
  int commandCounter=2;
//  out << "DEBUG nfo, remove when done. <br>randseed: " << this->theProblemData.randomSeed
//  << "<br> forReal: " << this->flagIsForReal << " seed computed: " << this->theProblemData.flagRandomSeedComputed
//  << " flagRandomSeedGiven: " << this->flagRandomSeedGiven
//  << this->ToStringCalculatorArgumentsForProblem("exercise");
  //first command and first syntactic element are the random seed and are ignored.
  for (int spanCounter=1; spanCounter <this->theContent.size; spanCounter++)
  { SyntacticElementHTML& currentElt=this->theContent[spanCounter];
    if (!currentElt.IsInterpretedByCalculatorDuringPreparatioN())
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
    this->theContent[spanCounter].flagUseMathMode=true;
    for (; commandCounter<theInterpreter.theProgramExpression.children.size; commandCounter++ )
    { if (theInterpreter.theProgramExpression[commandCounter].ToString()=="SeparatorBetweenSpans")
        break;
      if (moreThanOneCommand)
        this->theContent[spanCounter].interpretedCommand+="; ";
      else if
      (theInterpreter.theProgramExpression[commandCounter].IsOfType<Plot>() ||
       theInterpreter.theProgramExpression[commandCounter].IsOfType<std::string>())
        this->theContent[spanCounter].flagUseMathMode=false;
      this->theContent[spanCounter].interpretedCommand+=
      theInterpreter.theProgramExpression[commandCounter].ToString(&theFormat);
      moreThanOneCommand=true;
    }
  }
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time before class management routines");
//  stOutput << "got to here, this->theProblemData: " << this->theProblemData.ToString();
  for (int i=0; i<this->theContent.size; i++)
    if (this->theContent[i].GetTagClass()=="calculatorManageClass")
    { this->flagLoadedClassDataSuccessfully= this->PrepareClassData(comments);
      break;
    }

//  out << "Debug data: homework groups found: " << this->hdHomeworkGroups.ToStringCommaDelimited();
  for (int i=0; i<this->theContent.size; i++)
    if (this->theContent[i].IsInterpretedNotByCalculator())
      this->InterpretNotByCalculator(this->theContent[i]);
  for (int i=0; i<this->theContent.size; i++)
    if (!this->theContent[i].IsHidden())
      out << this->theContent[i].ToStringInterpreted();
//   out << "<hr><hr><hr><hr><hr><hr><hr><hr><hr>The calculator activity:<br>" << theInterpreter.outputString << "<hr>";
//   out << "<hr>" << this->ToStringExtractedCommands() << "<hr>";
  //  out << "<hr> Between the commands:" << this->betweenTheCommands.ToStringCommaDelimited();
  this->timeIntermediatePerAttempt.LastObject()->AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
  this->timeIntermediateComments.LastObject()->AddOnTop("Time before database storage");
#ifdef MACRO_use_MySQL
  if (this->flagIsForReal)
  { //stOutput << "This is for real!<br>";
    this->theProblemData.flagRandomSeedComputed=true;
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
  this->outputHtmlNavigation=this->ToStringProblemNavigation();
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
  //stOutput << "Is this an exam problem? ";
  if (!this->flagIsExamProblem)
  { //stOutput << "NONONO! -Emily";
    return;
  }
  //stOutput << "yesyesyes! -Emily";

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

bool CalculatorHTML::InterpretHtml(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::InterpretHtml");
  double startTime=theGlobalVariables.GetElapsedSeconds();
  if (!this->ParseHTML(comments))
  { this->outputHtmlMain="<b>Failed to interpret html input. </b><br>" +this->ToStringContent();
    this->timeToParseHtml=theGlobalVariables.GetElapsedSeconds()-startTime;
    return false;
  }
  this->timeToParseHtml=theGlobalVariables.GetElapsedSeconds()-startTime;
  this->NumAttemptsToInterpret=0;
  if (this->flagRandomSeedGiven && this->flagIsForReal)
    this->MaxInterpretationAttempts=1;
  srand (time(NULL));
  this->randomSeedsIfInterpretationFails.SetSize(this->MaxInterpretationAttempts);
  for (int i=0; i<this->randomSeedsIfInterpretationFails.size; i++)
    this->randomSeedsIfInterpretationFails[i]=rand()%100000000;
  this->timePerAttempt.SetSize(0);
  this->timeIntermediatePerAttempt.SetSize(0);
  this->timeIntermediateComments.SetSize(0);
  while (this->NumAttemptsToInterpret<this->MaxInterpretationAttempts)
  { startTime=theGlobalVariables.GetElapsedSeconds();
    this->timeIntermediatePerAttempt.SetSize(this->timeIntermediatePerAttempt.size+1);
    this->timeIntermediatePerAttempt.LastObject()->SetSize(0);
    this->timeIntermediateComments.SetSize(this->timeIntermediateComments.size+1);
    this->timeIntermediateComments.LastObject()->SetSize(0);
    Calculator theInterpreter;
    this->NumAttemptsToInterpret++;
    if (this->InterpretHtmlOneAttempt(theInterpreter, comments))
    { this->timePerAttempt.AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
      return true;
    }
    this->timePerAttempt.AddOnTop(theGlobalVariables.GetElapsedSeconds()-startTime);
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

bool CalculatorHTML::ParseHTMLComputeChildFiles(std::stringstream& comments)
{ MacroRegisterFunctionWithName("CalculatorHTML::ParseHTMLComputeChildFiles");
  bool result=true;
  this->hdProblemList.Clear();
  for (int i=0; i<this->theContent.size; i++)
    if (this->theContent[i].GetTagClass()=="calculatorExamProblem")
      this->hdProblemList.AddOnTop(this->CleanUpFileName(this->theContent[i].content));
  this->hdHomeworkGroups.SetSize(0);
  this->hdHomeworkGroups.SetExpectedSize(50);
  this->hdHomeworkGroupCorrespondingToEachProblem.SetSize(0);
  this->hdHomeworkGroupCorrespondingToEachProblem.SetExpectedSize(50);
  std::string currentHomeworkGroup;
  for (int i=0; i<this->theContent.size; i++)
    if (this->theContent[i].GetTagClass()=="calculatorExamIntermediate")
    { this->hdHomeworkGroups.SetSize(this->hdHomeworkGroups.size+1);
      this->hdHomeworkGroups.LastObject()->SetSize(0);
      currentHomeworkGroup=this->CleanUpFileName(this->theContent[i].content);
      this->hdHomeworkGroupNames.AddOnTop(currentHomeworkGroup);
    } else if (this->theContent[i].GetTagClass()=="calculatorExamProblem")
    { if (this->hdHomeworkGroups.size==0)
      { comments << "<b>Error: found a tag of type calculatorExamProblem before finding the first tag of type calculatorExamIntermediate."
        << " Please make a tag of type calculatorExamIntermediate before making tags of type calculatorExamProblem.</b>";
        result=false;
        continue;
      }
      this->hdHomeworkGroups.LastObject()->AddOnTop(this->CleanUpFileName(this->theContent[i].content));
      this->hdHomeworkGroupCorrespondingToEachProblem.AddOnTop(currentHomeworkGroup);
    }
  return result;
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
  if (this->calculatorClasses.size==0)
  { this->calculatorClasses.AddOnTop("calculator");
    this->calculatorClasses.AddOnTop("calculatorShowToUserOnly");
    this->calculatorClasses.AddOnTop("calculatorHidden");
    this->calculatorClasses.AddOnTop("calculatorHiddenIncludeInCommentsBeforeSubmission");
    this->calculatorClasses.AddOnTop("calculatorAnswer");
    this->calculatorClasses.AddOnTop("calculatorAnswerOnGiveUp");
    this->calculatorClasses.AddOnTop("calculatorCommentBeforeSubmission");
    this->calculatorClasses.AddOnTop("calculatorExamIntermediate");
    this->calculatorClasses.AddOnTop("calculatorExamProblem");
    this->calculatorClasses.AddOnTop("calculatorManageClass");
    this->calculatorClasses.AddOnTop("setCalculatorExamProblem");
    this->calculatorClasses.AddOnTop("setCalculatorExamIntermediate");
    this->calculatorClasses.AddOnTop("setCalculatorExamHome");
  }
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
        && last!="\"" )
    { if (last.syntacticRole!="" && last.content=="")
        secondToLast.content+=last.syntacticRole;
      else
        secondToLast.content+=last.content;
      eltsStack.RemoveLastObject();
      continue;
    }
    if (sixthToLast.syntacticRole=="<openTag" && fourthToLast=="=" && thirdToLast=="\""
        && last=="\"" )
    { sixthToLast.SetKeyValue(fifthToLast.content, secondToLast.content);
      eltsStack.SetSize(eltsStack.size-5);
      continue;
    }
    if (thirdToLast.syntacticRole=="<openTag" && secondToLast.syntacticRole=="" && last.syntacticRole==">")
    { thirdToLast.tagKeysWithoutValue.AddOnTop(secondToLast.content);
      eltsStack[eltsStack.size-2]=*eltsStack.LastObject();
      eltsStack.RemoveLastObject();
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
        if (this->theContent.LastObject()->IsInterpretedByCalculatorDuringPreparatioN() &&
            eltsStack[i].IsInterpretedByCalculatorDuringPreparatioN())
        { SyntacticElementHTML emptyElt;
          this->theContent.AddOnTop(emptyElt);
        }
      this->theContent.AddOnTop(eltsStack[i]);
    }
  }
  if (!this->ParseHTMLComputeChildFiles(comments))
    result=false;
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
  List<std::string> absoluteCommandsFoundSoFar;
  for (int i=0; i<this->theContent.size; i++)
  { if (this->theContent[i].IsCommentBeforeSubmission() || this->theContent[i].IsCommentAfterSubmission()||
        this->theContent[i].IsAnswerOnGiveUp())
    { if (answerIdsInTheFile.size<=0)
      { comments << "<b>Found comment to an answer before finding any answer tag. Each comment applies "
        << "to the first answer tag above it. </b>";
        return false;
      }
      int theIndex=this->theProblemData.answerIds.GetIndex(*answerIdsInTheFile.LastObject());
      if (theIndex==-1)
        crash << "This shouldn't happen: answerId from file not found in problem data. " << crash;
      if (this->theContent[i].IsAnswerOnGiveUp())
        this->theProblemData.commandsForGiveUpAnswer[theIndex]=this->theContent[i].content;
      if (this->theContent[i].IsCommentAfterSubmission())
        this->theProblemData.commentsAfterSubmission[theIndex].AddOnTop(this->theContent[i].content);
      if (this->theContent[i].IsCommentBeforeSubmission())
        this->theProblemData.commentsBeforeSubmission[theIndex].AddOnTop(this->theContent[i].content);
      continue;
    }
    if (this->theContent[i].GetTagClass()=="calculatorHiddenIncludeInCommentsBeforeSubmission")
    { absoluteCommandsFoundSoFar.AddOnTop(this->theContent[i].content);
      continue;
    }
    if (!this->theContent[i].IsAnswer())
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
    { int indexID=this->theProblemData.answerIds.GetIndex(newId);
      this->answerVerificationCommand[indexID]=this->theContent[i].content;
      this->theProblemData.commandsForPreview[indexID]=(absoluteCommandsFoundSoFar);
    } else
    { this->theProblemData.AddEmptyAnswerIdOnTop(newId);
      *this->theProblemData.commandsForPreview.LastObject()=absoluteCommandsFoundSoFar;
      this->answerVerificationCommand.AddOnTop(this->theContent[i].content);
    }
  }
//  stOutput << "cmnts before sub: " << this->theProblemData.commentsBeforeSubmission.ToStringCommaDelimited()
//  << "abs cmds: " << absoluteCommandsFoundSoFar.ToStringCommaDelimited();
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
