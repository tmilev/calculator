
//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
#include "vpfHeader3Calculator2_InnerFunctions.h"
#include "vpfHeader3Calculator1_InnerTypedFunctions.h"
#include "vpfHeader3Calculator4HtmlFunctions.h"
#include "vpfHeader8HtmlSnippets.h"
#include "vpfHeader8HtmlInterpretation.h"
ProjectInformationInstance ProjectInfoVpf6_37cpp(__FILE__, "More calculator built-in functions. ");

bool CalculatorFunctionsGeneral::innerAutomatedTestProblemInterpretation
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerAutomatedTestProblemInterpretation");
  (void) input;
  std::stringstream out;
  ProgressReport theReport;
  List<std::string> theFileNames, theFileTypes;
  int numDesiredTests=0;
  input.IsSmallInteger(&numDesiredTests);
  FileOperations::GetFolderFileNamesVirtual
  ("DefaultProblemLocation/", theFileNames, &theFileTypes, false);
  out << "<table><tr><th>File name </th><th>Loading</th><th>Interpretation</th> <th>AnswerGeneration</th></tr>";
  if (numDesiredTests<=0)
    numDesiredTests=theFileNames.size;
  int numInterpretations=0;
  int numSamples=1;
  int totalToInterpret=0;
  for (int i=0; i< theFileNames.size; i++)
    if (theFileTypes[i]==".html")
      totalToInterpret++;
  totalToInterpret= MathRoutines::Minimum(numDesiredTests, totalToInterpret);
  for (int i=0; i< theFileNames.size; i++)
  { if (numInterpretations>=numDesiredTests)
      break;
    if (theFileTypes[i]!=".html")
      continue;
    numInterpretations++;
    std::stringstream reportStream;
    reportStream << "Interpreting file "
    << numInterpretations
    << " out of "
    << totalToInterpret
    << theFileNames[i];
    theReport.Report(reportStream.str());
    CalculatorHTML theProblem;
    std::stringstream problemComments;
    theProblem.fileName="DefaultProblemLocation/"+theFileNames[i];
    out << "<tr>";
    out << "<td>"
    << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=exerciseNoLogin"
    << "&" << theGlobalVariables.ToStringCalcArgsNoNavigation(true)
    << "fileName=" << theProblem.fileName << "&\">"
    << theFileNames[i]
    << "</a>"
    << "</td>";
    if (!theProblem.LoadMe(false, problemComments))
    { out << "<td><b>Couldn't load. </b>"
      << problemComments.str() << "</td>";
      out << "</tr>";
      break;
    } else
      out << "<td><span style=\"color:green\">Success</span></td>";
    if (!theProblem.InterpretHtml(problemComments))
    {
      out << "<td><b>Failed to interpret file: " << theProblem.fileName
      << "</b>. Comments: " << problemComments.str();

      out << "</td></tr>";
      break;
    } else
      out << "<td><span style=\"color:green\">Success</span></td>";
    if (numInterpretations<=numSamples)
    { out << "<td>" << theProblem.outputHtmlBodyNoTag
      << "</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  return output.AssignValue(out.str(), theCommands);
}
