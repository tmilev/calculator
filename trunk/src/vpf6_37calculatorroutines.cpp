
//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
#include "vpfHeader3Calculator2_InnerFunctions.h"
#include "vpfHeader3Calculator1_InnerTypedFunctions.h"
#include "vpfHeader3Calculator4HtmlFunctions.h"
#include "vpfHeader8HtmlSnippets.h"
#include "vpfHeader8HtmlInterpretation.h"
#include "vpfHeader8HtmlInterpretationInterface.h"
ProjectInformationInstance ProjectInfoVpf6_37cpp(__FILE__, "More calculator built-in functions. ");

bool CalculatorFunctionsGeneral::innerAutomatedTestProblemInterpretation
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerAutomatedTestProblemInterpretation");
  if (input.size()!=3)
   return theCommands << "I expected two arguments: number of tests to run and number of tests to report";
  std::stringstream out;
  ProgressReport theReport;
  List<std::string> theFileNames, theFileTypes;
  int numDesiredTests=0;
  int numSamples=1;
  input[1].IsSmallInteger(&numDesiredTests);
  input[2].IsSmallInteger(&numSamples);
  FileOperations::GetFolderFileNamesVirtual
  ("DefaultProblemLocation/", theFileNames, &theFileTypes, false);
  std::stringstream randSeedStreaM;
  randSeedStreaM << theCommands.theObjectContainer.CurrentRandomSeed;
  std::string randomSeedCurrent=randSeedStreaM.str();
  out << "Random seed at start: " << randomSeedCurrent << "<br>";
  out << "<table>";
  out << "<tr>"
  << "<th>File name </th>"
  << "<th>Loading</th>"
  << "<th>Interpretation</th>"
  << "<th>AnswerGeneration</th>"
  << "<th>Accepting built in answer?</th>"
  << "</tr>";
  if (numDesiredTests<=0)
    numDesiredTests=theFileNames.size;
  int numInterpretations=0;
  int totalToInterpret=0;
  for (int i=0; i< theFileNames.size; i++)
    if (theFileTypes[i]==".html")
      totalToInterpret++;
  totalToInterpret= MathRoutines::Minimum(numDesiredTests, totalToInterpret);
  MapLisT<std::string, std::string, MathRoutines::hashString>&
  globalKeys= theGlobalVariables.webArguments;
  for (int i=0; i< theFileNames.size; i++)
  { if (numInterpretations>=numDesiredTests)
      break;
    if (theFileTypes[i]!=".html")
      continue;
    numInterpretations++;
    std::stringstream reportStream;
    reportStream << "Interpreting file "
    << theFileNames[i] << " ("
    << numInterpretations
    << " out of "
    << totalToInterpret
    << "). Random seed: "
    << randomSeedCurrent << "."
    ;
    theReport.Report(reportStream.str());
    CalculatorHTML theProblem;
    std::stringstream problemComments;
    theProblem.fileName="DefaultProblemLocation/"+theFileNames[i];
    bool isGood=theProblem.LoadMe(false, problemComments, randomSeedCurrent);
    std::stringstream randSeedCurrentStream;
    randSeedCurrentStream << theProblem.theProblemData.randomSeed;
    randomSeedCurrent=randSeedCurrentStream.str();
    out << "<tr>";
    out << "<td>"
    << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=exerciseNoLogin"
    << "&" << theGlobalVariables.ToStringCalcArgsNoNavigation(true)
    << "fileName=" << theProblem.fileName << "&randomSeed="
    << randomSeedCurrent << "\">"
    << theFileNames[i]
    << "</a>"
    << "</td>";  
    if (!isGood)
    { out << "<td><b>Couldn't load. </b>"
      << problemComments.str() << "</td>";
      out << "</tr>";
      break;
    } else
      out << "<td><span style=\"color:green\">Success</span></td>";
    if (!theProblem.InterpretHtml(problemComments))
    { out << "<td><span style=\"color:red\"><b>Failure.</b></span> "
      << "Comments: " << problemComments.str();
      out << "</td></tr>";
      break;
    } else
      out << "<td><span style=\"color:green\">Success</span></td>";
    bool answerGenerated=false;
    bool answersWork=false;
    std::string answerGeneration;
    std::string solutionReport;
    for (int j=0; j<theProblem.theProblemData.theAnswers.size; j++)
    { std::string currentAnswer;
      std::string currentKey="calculatorAnswer"+
      theProblem.theProblemData.theAnswers[j].answerId;
      theGlobalVariables.SetWebInpuT(currentKey, "1");
      theGlobalVariables.SetWebInpuT("fileName", theProblem.fileName);
      answerGeneration+=HtmlInterpretation::GetAnswerOnGiveUp
      (randomSeedCurrent, &currentAnswer, &answerGenerated)+"<hr>";
      if (!answerGenerated)
        break;
      theGlobalVariables.SetWebInpuT(currentKey, CGI::StringToURLString(currentAnswer, false));
      solutionReport+=
      HtmlInterpretation::SubmitProblem(randomSeedCurrent, &answersWork, false)+"<hr>";
      if (!answersWork)
        break;
      globalKeys.RemoveKey(currentKey);
    }
    if (!answerGenerated)
    { out << "<td><span style=\"color:red\"><b>Failure.</b></span>";
      out << "</td>"
      << "<td>"
      << answerGeneration
      << "</td>"
      << "</tr>";
      break;
    } else
      out << "<td><span style=\"color:green\">Success</span></td>";
    if (!answersWork)
    { out << "<td><span style=\"color:red\"><b>Failure.</b></span>";
      out << "</td></tr>";
      break;
    } else
      out << "<td><span style=\"color:green\">Success</span></td>";
    if (numInterpretations<=numSamples)
    { out << "<td><b>Problem</b><hr>" << theProblem.outputHtmlBodyNoTag
      << "</td>"
      ;
      out << "<td><b>Answer(s)</b><hr>" << answerGeneration
      << "</td>"
      ;
      out << "<td><b>Answer(s) confirmation(s)</b><hr>" << solutionReport
      << "</td>"
      ;
    }
    out << "</tr>";
  }
  out << "</table>";
  return output.AssignValue(out.str(), theCommands);
}
