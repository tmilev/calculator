//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader6WebServer.h"
#include "vpfHeader3Calculator0_Interface.h"
#include "vpfHeader7DatabaseInterface_MySQL.h"

ProjectInformationInstance projectInfoInstanceWebServerExamAndTeachingRoutines
(__FILE__, "Routines for calculus teaching: calculator exam mode.");

class ExerciseCalculus{

public:
  std::string name;
  std::string content;
};

class TeachingRoutines
{
public:
  std::string RelativePhysicalNameExercisesBaseFoler;
  List<ExerciseCalculus> theExercises;
  std::stringstream comments;
  bool LoadExercises();
  std::string ToStringExerciseSelection();
  TeachingRoutines();
};

TeachingRoutines::TeachingRoutines()
{ this->RelativePhysicalNameExercisesBaseFoler="exercises/";
}

bool TeachingRoutines::LoadExercises()
{ MacroRegisterFunctionWithName("TeachingRoutines::LoadExercises");
  List<std::string> theExerciseFileNames;
  if (!FileOperations::GetFolderFileNamesOnTopOfProjectBase
      (this->RelativePhysicalNameExercisesBaseFoler, theExerciseFileNames))
  { this->comments << "Failed to open folder with relative file name: "
    << this->RelativePhysicalNameExercisesBaseFoler;
    return false;
  }
  return false;
}

std::string TeachingRoutines::ToStringExerciseSelection()
{ MacroRegisterFunctionWithName("TeachingRoutines::ToStringExerciseSelection");
  std::stringstream out;
  out << this->theExercises.size << " homeworks.";
  for (int i=0; i<this->theExercises.size; i++)
    out << "<br>" << this->theExercises[i].name;
  return out.str();
}

std::string WebWorker::GetTestingScreen()
{ MacroRegisterFunctionWithName("WebWorker::GetTestingScreen");
  std::stringstream out;
  out << "<html>";
  TeachingRoutines theRoutines;
  out << WebWorker::GetJavascriptStandardCookies()
  << "<body onload=\"loadSettings();\">\n";
  if (!theRoutines.LoadExercises())
  { out << "Loading exercises failed, comments (which hopefully explain what happened) follow. "
    << "<hr>" << theRoutines.comments.str()
    << "</body></html>";
    return out.str();
  }
  out << theRoutines.ToStringExerciseSelection();
  out << "</body></html>";
  return out.str();
}
