//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader6WebServer.h"
#include "vpfHeader3Calculator0_Interface.h"
#include "vpfHeader7DatabaseInterface_MySQL.h"

ProjectInformationInstance projectInfoInstanceWebServerExamAndTeachingRoutines
(__FILE__, "Routines for calculus teaching: calculator exam mode.");

class Problem
{
public:
  std::string fileName;
  std::string RelativePhysicalFileNameWithFolder;
  std::string content;
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
  std::string ToStringStartProblem();
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

std::string Problem::ToStringStartProblem()
{ MacroRegisterFunctionWithName("ProblemCollection::ToStringStartProblem");
  std::stringstream out;
  out << CGI::GetLaTeXProcessingJavascript();
  out << "\n<FORM method=\"POST\" id=\"formProblemCollection\" name=\"formProblemCollection\" action=\""
  << theGlobalVariables.DisplayNameCalculatorWithPath << "\">\n" ;
  out << theWebServer.GetActiveWorker().GetHtmlHiddenInputs();
  out << this->content;
  out << "\n</FORM>\n";
  return out.str();
}

std::string ProblemCollection::ToStringProblemLinks()
{ MacroRegisterFunctionWithName("ProblemCollection::ToStringProblemLinks");
  std::stringstream out;
  out << this->theProblems.size << " problem(s).";
  out << "\n<FORM method=\"POST\" id=\"formProblemCollection\" name=\"formProblemCollection\" action=\""
  << theGlobalVariables.DisplayNameCalculatorWithPath << "\">\n" ;
  out << theWebServer.GetActiveWorker().GetHtmlHiddenInputs();
  for (int i=0; i<this->theProblems.size; i++)
  { out << this->theProblems[i].fileName
    << "<button type=\"submit\" name=\"currentProblem\" title=\"Start\" value=\""
    << this->theProblems[i].fileName
    << "\"> Start</button>";
    out << "<br>";
  }
  out << "\n</FORM>\n";
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
    this->content=contentStream.str();
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
  if (theGlobalVariables.userCurrentProblemCollection=="")
    return this->ToStringExerciseSelection();
  if (!this->HasExercise(theGlobalVariables.userCurrentProblemCollection))
  { out << "<hr><b>Could not find problem collection: " << theGlobalVariables.userCurrentProblemCollection << "</b><hr>";
    out << this->ToStringExerciseSelection();
    return out.str();
  }
  ProblemCollection& currentCollection=
  this->theExercises.GetElement(this->GetExerciseIndex(theGlobalVariables.userCurrentProblemCollection));
  if (!currentCollection.LoadMe(this->comments))
  { out << "<hr><b>Failed to load problem collection: " << currentCollection.fileName << ".</b> "
    << this->comments.str() << "<hr>"
    << this->ToStringExerciseSelection();
    return out.str();
  }
  if (theGlobalVariables.userCurrentProblem=="")
    return currentCollection.ToStringProblemLinks();
  if (!currentCollection.HasProblem(theGlobalVariables.userCurrentProblem))
  { out << "<hr><b>Could not find problem: " << theGlobalVariables.userCurrentProblem << ".</b></hr>"
    << currentCollection.ToStringProblemLinks();
    return out.str();
  }
  Problem& currentProblem=
  currentCollection.theProblems.GetElement
  (currentCollection.GetProblemIndex(theGlobalVariables.userCurrentProblem));
  if (!currentProblem.LoadMe(this->comments))
  { out << "<hr><b>Failed to load problem: " << currentProblem.fileName << ".</b>"
    << this->comments.str() << "<hr>"
    << currentCollection.ToStringProblemLinks();
    return out.str();
  }
  return currentProblem.ToStringStartProblem();
}

std::string WebWorker::GetTestingScreen()
{ MacroRegisterFunctionWithName("WebWorker::GetTestingScreen");
  TeachingRoutines theRoutines;
  std::stringstream out;
  out << "<html>"
  << "<header>"
  << WebWorker::GetJavascriptStandardCookies()
  << "<link rel=\"stylesheet\" type=\"text/css\" href=\"/ProblemCollections/calculator.css\">"
  << "</header>"
  << "<body onload=\"loadSettings();\">\n";
  //out << "<hr>Message follows.<br>" << this->theMessage << "<hr>";
  out << this->ToStringCalculatorArguments();
  out << theRoutines.GetCurrentProblemItem();
  out << "</body></html>";
  return out.str();
}
