//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader6WebServer.h"
#include "vpfHeader3Calculator0_Interface.h"
#include "vpfHeader7DatabaseInterface_MySQL.h"

ProjectInformationInstance projectInfoInstanceWebServerExamAndTeachingRoutines
(__FILE__, "Routines for calculus teaching: calculator exam mode.");

class ExerciseCollection{

public:
  std::string fileName;
  List<std::string> problemFileNames;
  static unsigned int HashFunction(const ExerciseCollection& input)
  { return input.HashFunction();
  }
  unsigned int HashFunction()const
  { return MathRoutines::hashString(this->fileName);
  }
  bool operator==(const ExerciseCollection& other)const
  { return this->fileName==other.fileName;
  }
  bool LoadMe(std::stringstream& comments);
  std::string ToStringProblemLinks();
};

class TeachingRoutines
{
public:
  std::string RelativePhysicalNameExercisesBaseFoler;
  HashedList<ExerciseCollection> theExercises;
  bool HasExercise(const std::string& desiredExercise);
  int GetExerciseIndex(const std::string& desiredExercise);
  std::stringstream comments;
  bool LoadExercises();
  std::string GetOneExerciseScreen();
  std::string GetAllExercisesScreen();
  std::string ToStringExerciseSelection();
  TeachingRoutines();
};

TeachingRoutines::TeachingRoutines()
{ this->RelativePhysicalNameExercisesBaseFoler="exercises/";
}

bool TeachingRoutines::LoadExercises()
{ MacroRegisterFunctionWithName("TeachingRoutines::LoadExercises");
  List<std::string> theExerciseFileNames, theExerciseFileNameExtensions;
  if (!FileOperations::GetFolderFileNamesOnTopOfProjectBase
      (this->RelativePhysicalNameExercisesBaseFoler, theExerciseFileNames, &theExerciseFileNameExtensions))
  { this->comments << "Failed to open folder with relative file name: "
    << this->RelativePhysicalNameExercisesBaseFoler;
    return false;
  }
  this->theExercises.SetExpectedSize(theExerciseFileNames.size);
  this->theExercises.Clear();
  ExerciseCollection newExercise;
  for (int i=0; i<theExerciseFileNames.size; i++)
  { if (theExerciseFileNameExtensions[i]!=".txt")
      continue;
    newExercise.fileName=theExerciseFileNames[i];
    this->theExercises.AddOnTop(newExercise);
  }
//  stOutput << "ExerciseFileNames: " << theExerciseFileNames.ToStringCommaDelimited() << " with extensions: "
//  << theExerciseFileNameExtensions.ToStringCommaDelimited();
  return true;
}

std::string TeachingRoutines::ToStringExerciseSelection()
{ MacroRegisterFunctionWithName("TeachingRoutines::ToStringExerciseSelection");
  std::stringstream out;
  out << this->theExercises.size << " homework(s).";
  out << "\n<FORM method=\"POST\" id=\"formExercises\" name=\"formExercises\" action=\""
  << theGlobalVariables.DisplayNameCalculatorWithPath << "\">\n" ;
  out << theWebServer.GetActiveWorker().GetHtmlHiddenInputs();
  for (int i=0; i<this->theExercises.size; i++)
  { out << this->theExercises[i].fileName
    << "<button type=\"submit\" name=\"currentProblemCollection\" value=\""
    << this->theExercises[i].fileName
    << "\"> Start</button>";
    out << "<br>";
  }
  out << "\n</FORM>\n";
  return out.str();
}

bool TeachingRoutines::HasExercise(const std::string& desiredExercise)
{ return this->GetExerciseIndex(desiredExercise)!=-1;
}

int TeachingRoutines::GetExerciseIndex(const std::string& desiredExercise)
{ ExerciseCollection tempExercise;
  tempExercise.fileName=desiredExercise;
  return this->theExercises.GetIndex(tempExercise);
}

std::string ExerciseCollection::ToStringProblemLinks()
{ MacroRegisterFunctionWithName("ExerciseCollection::ToStringProblemLinks");
  std::stringstream out;
  out << this->problemFileNames.size << " problem(s).";
  out << "\n<FORM method=\"POST\" id=\"formProblemCollection\" name=\"formProblemCollection\" action=\""
  << theGlobalVariables.DisplayNameCalculatorWithPath << "\">\n" ;
  out << theWebServer.GetActiveWorker().GetHtmlHiddenInputs();
  for (int i=0; i<this->problemFileNames.size; i++)
  { out << this->problemFileNames[i]
    << "<button type=\"submit\" name=\"currentProblemCollection\" title=\"Start\" value=\""
    << this->problemFileNames[i]
    << "\"> Start</button>";
    out << "<br>";
  }
  out << "\n</FORM>\n";
  return out.str();
}

bool ExerciseCollection::LoadMe(std::stringstream& comments)
{ MacroRegisterFunctionWithName("ExerciseCollection::LoadMe");
  std::fstream theFile;
  if (!FileOperations::OpenFileCreateIfNotPresentOnTopOfProjectBase(theFile, this->fileName, false, false, false))
  { comments << "<b>Failed to open file " << this->fileName << "</b>";
    return false;
  } else
  { this->problemFileNames.SetSize(0);
    std::string currentProblemFileName;
    while (std::getline(theFile, currentProblemFileName))
      this->problemFileNames.AddOnTop(currentProblemFileName);
  }
  return true;
}

std::string TeachingRoutines::GetOneExerciseScreen()
{ MacroRegisterFunctionWithName("TeachingRoutines::GetOneExerciseScreen");
  if (theGlobalVariables.userCurrentProblemCollection=="")
    return this->GetAllExercisesScreen();
  std::stringstream out;
  if (!this->HasExercise(theGlobalVariables.userCurrentProblemCollection))
  { out << "<b>Could not find exercise: " << theGlobalVariables.userCurrentProblemCollection << "</b>";
    out << this->GetAllExercisesScreen();
    return out.str();
  }
  ExerciseCollection& currentExercise=
  this->theExercises.GetElement(this->GetExerciseIndex(theGlobalVariables.userCurrentProblemCollection));
  out << "Exercise " << currentExercise.fileName << ". <hr>";
  std::stringstream failureComments;
  if (!currentExercise.LoadMe(failureComments))
    out << "Failed to load: " << failureComments.str();
  else
    out << currentExercise.ToStringProblemLinks();
  return out.str();
}

std::string TeachingRoutines::GetAllExercisesScreen()
{ MacroRegisterFunctionWithName("WebWorker::GetAllExercisesScreen");
  std::stringstream out;
  out << this->ToStringExerciseSelection();
  return out.str();
}

std::string WebWorker::GetTestingScreen()
{ MacroRegisterFunctionWithName("WebWorker::GetTestingScreen");
  TeachingRoutines theRoutines;
  std::stringstream out;
  out << "<html>" << WebWorker::GetJavascriptStandardCookies()
  << "<body onload=\"loadSettings();\">\n";
  out << this->ToStringCalculatorArguments();
  if (!theRoutines.LoadExercises())
  { out << "Loading exercises failed, comments (which hopefully explain what happened) follow. "
    << "<hr>" << theRoutines.comments.str()
    << "</body></html>";
    return out.str();
  }
  out << theRoutines.GetOneExerciseScreen();
  out << "</body></html>";
  return out.str();
}
