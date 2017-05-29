//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader2Math10_LaTeXRoutines.h"
#include "vpfHeader1General7FileOperations_Encodings.h"
#include "vpfHeader1General4General_Logging_GlobalVariables.h"

ProjectInformationInstance ProjectInfoVpf6_38LaTeXRoutines(__FILE__, "LaTeX routines. ");


bool LaTeXcrawler::ExtractFileNamesFromRelativeFileName()
{ MacroRegisterFunctionWithName("LaTeXcrawler::ExtractFileNamesFromRelativeFileName");
  if (!FileOperations::IsOKfileNameVirtual(this->theFileToCrawlRelative))
  { this->displayResult << "The folders below the file name contain dots. This is not allowed. ";
    return false;
  }
  FileOperations::GetPhysicalFileNameFromVirtual
  (this->theFileToCrawlRelative,this->theFileToCrawlPhysical,true,false);
//  =theGlobalVariables.PhysicalPathProjectBase+ "../freecalc/" +
//  this->theFileToCrawlRelative;
  this->baseFolderStartFilePhysical=
  FileOperations::GetPathFromFileNameWithPath(this->theFileToCrawlPhysical);
  MathRoutines::StringBeginsWith
  (this->theFileToCrawlPhysical, this->baseFolderStartFilePhysical,
   &this->theFileToCrawlNoPathPhysical);
  return true;
}

void LaTeXcrawler::BuildFreecalc()
{ MacroRegisterFunctionWithName("LaTeXcrawler::BuildFreecalc");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
  { this->displayResult << "Build freecalc command allowed only for logged-in admins.";
    return;
  }
  theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=50000;
  if (!this->ExtractFileNamesFromRelativeFileName())
    return;
  std::fstream inputFile;
  if (!FileOperations::OpenFileUnsecure(inputFile, this->theFileToCrawlPhysical, false, false, false))
  { this->displayResult << "Failed to open input file: " << this->theFileToCrawlPhysical << ", aborting. ";
    return;
  }
  inputFile.seekg(0);
  std::string buffer;
  ProgressReport theReport;
  std::stringstream reportStream;
  reportStream << "Processing input file: extracting lecture numbers...";
  theReport.Report(reportStream.str());
//  stOutput << "Processing input file: extracting lecture numbers.";
  bool isLecturE =false;
  bool isHW=false;
  int currentLineIndex=-1;
  while (!inputFile.eof())
  { std::getline(inputFile, buffer);
    currentLineIndex++;
    bool isInput=false;
    if (MathRoutines::StringBeginsWith(buffer, "\\lect") &&
        !MathRoutines::StringBeginsWith(buffer, "\\lecture"))
    { isInput=true;
      isLecturE=true;
    }
    if (MathRoutines::StringBeginsWith(buffer, "\\homeworkOnATopic"))
    { isInput=true;
      isHW=true;
    }
    if (this->flagBuildSingleSlides && isLecturE)
    { //stOutput << "<br>DEBUG: Checking " << buffer << " trimmed to: " << MathRoutines::StringTrimWhiteSpace(buffer);
      if (MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "\\input", 0) )
        this->theSlides.AddOnTop(MathRoutines::StringTrimWhiteSpace(buffer));
    }
    if (!isInput)
      continue;
    int leftBracketsMinusRight=0;
    int numBallancedBracketGroups=0;
    std::string lectureNumber;
    bool recordingLectureNumber=false;
    for (unsigned i=0; i<buffer.size(); i++)
    { if (buffer[i]=='}')
      { leftBracketsMinusRight--;
        if (leftBracketsMinusRight==0)
        { numBallancedBracketGroups++;
          if (recordingLectureNumber)
            break;
        }
      }
      if (buffer[i]=='{')
      { leftBracketsMinusRight++;
        if (leftBracketsMinusRight==1 && numBallancedBracketGroups==2)
          recordingLectureNumber=true;
      }
      if (recordingLectureNumber)
        lectureNumber.push_back(buffer[i]);
    }
    if (lectureNumber.size()>0)
    { this->theLectureNumbers.AddOnTop(lectureNumber.substr(1, lectureNumber.size()-1));
    } else
    { this->displayResult << "Failed to extract lecture/homework number from line: " << buffer;
      return;
    }
    std::getline(inputFile, buffer);
    std::string desiredName;
    if (!MathRoutines::StringBeginsWith(buffer, "%DesiredLectureName: ", &desiredName))
      if(!MathRoutines::StringBeginsWith(buffer, "%DesiredHomeworkName: ", &desiredName))
        desiredName="";
    for (unsigned i=0; i<desiredName.size(); i++)
      if (desiredName[i]== '.' || desiredName[i]=='/' || desiredName[i]=='\\')
        desiredName="";
    if (desiredName=="")
      this->displayResult << "Failed to extract desired homework/lecture name from: " << buffer
      << "<br>This is the line immediately after the \\lect command. It should begin with the string \"%DesiredLectureName: \""
      << "(<-has space bar in the end). The name itself should not contain the characters . / or \\. "
      << "I am assigning an automatic file name. <br>";
    theLectureDesiredNames.AddOnTop(desiredName);
  }
  reportStream << " done. Extracted: " << theLectureNumbers.size << " homework/lecture numbers. Preparing Homework/Lecture content ... ";
  theReport.Report(reportStream.str());
//  stOutput << reportStream.str();
  if (isLecturE && isHW)
  { this->displayResult << "I was not able to determine whether the file is a homework or a lecture file. Aborting.";
    return;
  }
  if (!isLecturE && !isHW)
  { this->displayResult << "Could not find any lecture or homework entries. Aborting.";
    return;
  }
  std::stringstream LectureContentNoDocumentClassNoCurrentLecture;
  std::stringstream LectureHeaderNoDocumentClass;
  inputFile.clear();
  inputFile.seekg(0);
  bool foundFirstLecture=false;
  if (isLecturE)
    while (!inputFile.eof())
    { std::getline(inputFile, buffer);
      if (!MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "\\documentclass") &&
          !MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "[handout]") &&
          !MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "{beamer}") &&
          !MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "\\newcommand{\\currentLecture}")
         )
        LectureContentNoDocumentClassNoCurrentLecture << buffer << "\n";
      if (this->flagBuildSingleSlides)
      { if (MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "\\lect") &&
            !MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "\\lecture"))
          foundFirstLecture=true;
        if (!foundFirstLecture)
          if (!MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "\\documentclass") &&
              !MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "[handout]") &&
              !MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "{beamer}"))
            LectureHeaderNoDocumentClass << buffer << "\n";
      }
    }
  else
    while (!inputFile.eof())
    { std::getline(inputFile, buffer);
      if (!MathRoutines::StringBeginsWith(buffer, "\\newcommand{\\currentHW}"))
        LectureContentNoDocumentClassNoCurrentLecture << buffer << "\n";
    }
  reportStream << " done. Proceding to compile homeworks/lectures. ";
  theReport.Report(reportStream.str());
//  stOutput << reportStream2.str();
  if(isLecturE)
    this->displayResult << "<table><tr><td>Lecture number</td><td>Lecture name</td><td>Lecture pdf</td>"
    << "<td>Lecture handout pdf</td><td>Comments</td></tr>";
  else
    this->displayResult << "<table><tr><td>Homework number</td><td>Homework name</td><td>Homework pdf</td>"
    << "<td>Homework handout pdf</td><td>Comments</td></tr>";
  std::string lectureFileNameEnd;
  if (!MathRoutines::StringBeginsWith(this->theFileToCrawlNoPathPhysical, "Lecture_", &lectureFileNameEnd))
    if (!MathRoutines::StringBeginsWith(this->theFileToCrawlNoPathPhysical, "Homework_", &lectureFileNameEnd))
      lectureFileNameEnd="";
  std::string folderEnd=this->theFileToCrawlNoPathPhysical.substr(0, this->theFileToCrawlNoPathPhysical.size()-4);
  if (lectureFileNameEnd.size()>4)
    lectureFileNameEnd=lectureFileNameEnd.substr(0, lectureFileNameEnd.size()-4);
  std::stringstream executedCommands, resultTable;
  std::string currentSysCommand; //"ch " + this->baseFolderStartFilePhysical+"\n\n\n\n";
  //executedCommands << "<br>" << currentSysCommand;
  theGlobalVariables.ChDir(this->baseFolderStartFilePhysical);
  executedCommands << "Commands executed: "
  << "<br>Directory changed: " << this->baseFolderStartFilePhysical;
  reportStream << "<br>Directory changed: " << this->baseFolderStartFilePhysical;
  std::string lectureProjectorFolder="lectures_projector_" + folderEnd + "/";
  std::string printableFolder;
  std::string slideProjectorFolder, slideHandoutFolder;
  if (!isLecturE)
    printableFolder="homework_" + folderEnd + "/";
  else
    printableFolder="lectures_printable_" + folderEnd + "/";
  theGlobalVariables.CallSystemNoOutput("mkdir "+printableFolder);
  if (isLecturE)
  { theGlobalVariables.CallSystemNoOutput("mkdir "+lectureProjectorFolder);
    if (this->flagBuildSingleSlides)
    { slideProjectorFolder = "slides_projector_" + folderEnd + "/";
      slideHandoutFolder = "slides_handout_" + folderEnd + "/";
      theGlobalVariables.CallSystemNoOutput("mkdir "+slideProjectorFolder);
      theGlobalVariables.CallSystemNoOutput("mkdir "+slideHandoutFolder);
    }
  }
  //this->theFileWorkingCopy=this->baseFolderStartFilePhysical+ "working_file_"+ this->theFileToCrawlNoPathPhysical;
  //std::string theFileWorkingCopyPDF=this->baseFolderStartFilePhysical+ "working_file_"
  //+this->theFileToCrawlNoPathPhysical.substr(0, this->theFileToCrawlNoPathPhysical.size()-3)+"pdf";
  this->theFileWorkingCopy= "working_file_"+ this->theFileToCrawlNoPathPhysical;
  std::string theFileWorkingCopyPDF= "working_file_"
  +this->theFileToCrawlNoPathPhysical.substr(0, this->theFileToCrawlNoPathPhysical.size()-3)+"pdf";
  int numLecturesToProcess=theLectureNumbers.size;
  std::fstream workingFile;
  for (int i=0; i<numLecturesToProcess; i++)
  { reportStream << "<br>Processing lecture " << i+1 << " out of " << theLectureNumbers.size << ". ";
    resultTable << "<tr>";
    resultTable << "<td>" << theLectureNumbers[i] << "</td>";
    resultTable << "<td>" << theLectureDesiredNames[i] << "</td>";
    if (!FileOperations::OpenFileCreateIfNotPresentUnsecure(workingFile, this->theFileWorkingCopy, false, true, false))
    { resultTable << "<td>-</td><td>-</td><td>Failed to open working file: "
      << this->theFileWorkingCopy << ", aborting. </td> </tr>";
      break;
    }
    if (isLecturE)
      workingFile << "\\documentclass[handout]{beamer}\n\\newcommand{\\currentLecture}{"
      << theLectureNumbers[i] << "}\n"
      << LectureContentNoDocumentClassNoCurrentLecture.str();
    else
      workingFile << "\\newcommand{\\currentHW}{" << theLectureNumbers[i] << "}\n"
      << LectureContentNoDocumentClassNoCurrentLecture.str();
    workingFile.close();
    currentSysCommand="pdflatex -shell-escape "+this->theFileWorkingCopy;
    executedCommands << "<br>" << currentSysCommand;
    reportStream << currentSysCommand;
    theReport.Report(reportStream.str());
    theGlobalVariables.CallSystemNoOutput(currentSysCommand);
    reportStream << "<b>[x2]</b>";
    theReport.Report(reportStream.str());
    theGlobalVariables.CallSystemNoOutput(currentSysCommand);
    std::stringstream thePdfFileNameHandout;
    if (isLecturE)
    { thePdfFileNameHandout << "./" << printableFolder << "Lecture" << theLectureNumbers[i] << "Handout_"
      << theLectureDesiredNames[i];
      if (lectureFileNameEnd!="")
        thePdfFileNameHandout << "_" << lectureFileNameEnd;
      thePdfFileNameHandout << ".pdf";
    } else
    { thePdfFileNameHandout << "./" << printableFolder << "Homework" << theLectureNumbers[i] << "_" << theLectureDesiredNames[i];
      if (lectureFileNameEnd!="")
        thePdfFileNameHandout << "_" << lectureFileNameEnd;
      thePdfFileNameHandout << ".pdf";
    }
    currentSysCommand="mv " +theFileWorkingCopyPDF+" " + thePdfFileNameHandout.str();
    executedCommands << "<br>" << currentSysCommand;
    reportStream << "<br>Lecture/Homework " << i+1 << " handout compiled, renaming file ... ";
    theReport.Report(reportStream.str());
    theGlobalVariables.CallSystemNoOutput(currentSysCommand);
    reportStream << " done.";
    theReport.Report(reportStream.str());
    if (!isLecturE)
    { theReport.Report(reportStream.str());
      resultTable << "<td>" << thePdfFileNameHandout.str() << "</td>";
      resultTable << "</tr>";
      continue;
    }
    if (!FileOperations::OpenFileUnsecure(workingFile, this->theFileWorkingCopy, false, true, false))
    { resultTable << "<td>-</td><td>-</td><td>Failed to open working file: "
      << this->theFileWorkingCopy << ", aborting. </td> </tr>";
      break;
    }
    workingFile << "\\documentclass{beamer}\n\\newcommand{\\currentLecture}{"
    << theLectureNumbers[i] << "}\n";
    workingFile << LectureContentNoDocumentClassNoCurrentLecture.str();
    workingFile.close();
    currentSysCommand="pdflatex -shell-escape "+this->theFileWorkingCopy;
    executedCommands << "<br>" << currentSysCommand;
    reportStream << currentSysCommand;
    theReport.Report(reportStream.str());
    theGlobalVariables.CallSystemNoOutput(currentSysCommand);
    std::stringstream thePdfFileNameNormal;
    thePdfFileNameNormal << "./" << lectureProjectorFolder << "Lecture"
    << theLectureNumbers[i] << "_" << theLectureDesiredNames[i] << "_"
    << lectureFileNameEnd << ".pdf";
    currentSysCommand= "mv " +theFileWorkingCopyPDF + " " + thePdfFileNameNormal.str();
    executedCommands << "<br>" << currentSysCommand;
    reportStream << "<br>Lecture " << i+1 << " regular slides compiled, renaming file ... ";
    theReport.Report(reportStream.str());
    theGlobalVariables.CallSystemNoOutput(currentSysCommand);
    reportStream << " done.";
    theReport.Report(reportStream.str());
    resultTable << "<td>" << thePdfFileNameNormal.str() << "</td>" << "<td>" << thePdfFileNameHandout.str() << "</td>";
    resultTable << "</tr>";
  }
  int numSlidesToBuild=this->theSlides.size;
  this->theSlideNames.SetSize(this->theSlides.size);
  for (int i=0; i<this->theSlides.size; i++)
  { this->theSlideNames[i]="";
    if (this->theSlides[i]=="")
      continue;
    std::string currentName=this->theSlides[i].substr(0, this->theSlides[i].size()-1);
    this->theSlideNames[i]= FileOperations::GetFileNameFromFileNameWithPath(currentName);
  }
  //executedCommands << "<br>Slides extracted: " << this->theSlides.ToStringCommaDelimited();
  //executedCommands << "<br>Slides names: " << this->theSlideNames.ToStringCommaDelimited();
  for (int i=0; i<numSlidesToBuild; i++)
  { for (int k=0; k<2; k++)
    { if (!FileOperations::OpenFileUnsecure(workingFile, this->theFileWorkingCopy, false, true, false))
      { resultTable << "<td>-</td><td>-</td><td>Failed to open working file: "
        << this->theFileWorkingCopy << ", aborting. </td> </tr>";
        break;
      }
      workingFile << "\\documentclass";
      if (k==1)
        workingFile << "[handout]";
      workingFile << "{beamer}";
      workingFile << LectureHeaderNoDocumentClass.str()
      << "\n"
      << this->theSlides[i]
      << "\n"
      << "\\end{document}";
      workingFile.close();
      currentSysCommand="pdflatex -shell-escape "+this->theFileWorkingCopy;
      executedCommands << "<br>" << currentSysCommand;
      reportStream << currentSysCommand;
      theReport.Report(reportStream.str());
      theGlobalVariables.CallSystemNoOutput(currentSysCommand);
//      executedCommands << "<br>result: "
//      << theGlobalVariables.CallSystemWithOutput(currentSysCommand);
      std::stringstream thePdfFileNameNormal;
      thePdfFileNameNormal << "./";
      if (k==0)
        thePdfFileNameNormal << slideProjectorFolder;
      else
        thePdfFileNameNormal << slideHandoutFolder;
      thePdfFileNameNormal << "Slide_"
      << this->theSlideNames[i] << ".pdf";
      currentSysCommand= "mv " +theFileWorkingCopyPDF + " " + thePdfFileNameNormal.str();
      executedCommands << "<br>" << currentSysCommand;
      reportStream << "<br>Slide " << i+1 << ", run " << k << " compiled, renaming file ... ";
      theReport.Report(reportStream.str());
      theGlobalVariables.CallSystemWithOutput(currentSysCommand);
      reportStream << " done.";
      theReport.Report(reportStream.str());
      resultTable << "<td>" << thePdfFileNameNormal.str() << "</td>";
      resultTable << "</tr>";
    }
  }

  resultTable << "</table>";
  this->displayResult << executedCommands.str() << "<br>" << resultTable.str();
  theGlobalVariables.ChDir("../");
}

void LaTeXcrawler::Crawl()
{ MacroRegisterFunctionWithName("LaTeXcrawler::Crawl");
  if (!this->ExtractFileNamesFromRelativeFileName())
    return;
  this->recursionDepth=0;
  this->CrawlRecursive(this->theFileToCrawlPhysical);
  std::fstream outputFile;
  std::string outputFileName = "latexOutput.tex";
  if (!FileOperations::OpenFileCreateIfNotPresentVirtual(outputFile, "output/"+ outputFileName, false, true, false, true))
  { this->displayResult << "Failed to open output file: " << outputFileName << ", check write permissions. ";
    //this->displayResult << "The crawling appears to have been successful, below is the crawled file.<hr>"
    //<< this->crawlingResult.str();
    return;
  }
  outputFile << this->crawlingResult.str();
  if (this->errorStream.str()!="")
    this->displayResult << "Errors encountered. " << this->errorStream.str();
  this->displayResult << "Output file: <a href=\"" << theGlobalVariables.DisplayPathOutputFolder
  << "latexOutput.tex\">" << "latexOutput.tex" << "</a>";
}

void LaTeXcrawler::CrawlRecursive(const std::string& currentFileName)
{ MacroRegisterFunctionWithName("LaTeXcrawler::CrawlRecursive");
  RecursionDepthCounter theCounter(&this->recursionDepth);
  if (this->recursionDepth>1000)
  { this->errorStream << "While crawling theFileToCrawl, reached max recursion depth of 1000";
    return;
  }
  if (this->errorStream.str()!="")
    return;
  std::fstream theFile;
  if (!FileOperations::OpenFileUnsecure(theFile, currentFileName, false, false, false))
  { this->errorStream << "Failed to open file "
    << currentFileName << ", aborting. ";
    return;
  }
  std::string buffer;
  while (!theFile.eof())
  { std::getline(theFile, buffer);
    std::stringstream newFileName;
    bool isGood=true;
    if (buffer.find("IfFileExists")!=std::string::npos)
      isGood=false;
    if (buffer.size()>0)
      if (buffer[0]=='%')
        isGood=false;
    int foundInput= buffer.find("\\input");
    if (isGood)
      if (((unsigned) foundInput)<buffer.size() )
      { this->crawlingResult << buffer.substr(0, foundInput);
        buffer=buffer.substr(foundInput);
        newFileName << this->baseFolderStartFilePhysical;
        unsigned i=0;
        for (i=7; buffer[i]!='}' && i< buffer.size();i++)
          newFileName << buffer[i];
        newFileName << ".tex";
        this->crawlingResult << "%input from file: " << newFileName.str() << "\n";
        this->CrawlRecursive(newFileName.str());
        this->crawlingResult << "\n";
        if (i+1<buffer.size())
          buffer = buffer.substr(i+1);
        else
          continue;
      }
    this->crawlingResult << buffer;
    this->crawlingResult << "\n";
  }
}
