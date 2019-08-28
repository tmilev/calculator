//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader2Math10_LaTeXRoutines.h"
#include "vpfHeader1General7FileOperations_Encodings.h"
#include "vpfHeader1General4General_Logging_GlobalVariables.h"
#include "vpfHeader8HtmlSnippets.h"

ProjectInformationInstance ProjectInfoVpf6_38LaTeXRoutines(__FILE__, "LaTeX routines. ");

extern logger logWorker;

bool LaTeXcrawler::IsInCrawlableFolder(const std::string& folderName, std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("LaTeXcrawler::IsInCrawlableFolder");
  for (int i = 0; i < this->baseFoldersCrawlableFilesPhysical.size; i ++) {
    if (MathRoutines::StringBeginsWith(folderName, this->baseFoldersCrawlableFilesPhysical[i])) {
      return true;
    }
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "File folder: " << folderName
    << " does not appear to be in one of the allowed folders: " << this->baseFoldersCrawlableFilesPhysical.ToStringCommaDelimited()
    << ". ";
  }
  return false;
}

void LaTeXcrawler::ComputeAllowedFolders() {
  MacroRegisterFunctionWithName("LaTeXcrawler::ComputeAllowedFolders");
  if (this->baseFoldersCrawlableFilesPhysical.size > 0) {
    return;
  }
  List<std::string> allowedFoldersVirtual;
  allowedFoldersVirtual.AddOnTop("freecalc/");
  allowedFoldersVirtual.AddOnTop("LaTeX-materials/");
  allowedFoldersVirtual.AddOnTop("/freecalc/");
  allowedFoldersVirtual.AddOnTop("/LaTeX-materials/");
  this->baseFoldersCrawlableFilesPhysical.SetSize(allowedFoldersVirtual.size);
  for (int i = 0; i < this->baseFoldersCrawlableFilesPhysical.size; i ++) {
    FileOperations::GetPhysicalFileNameFromVirtual(
      allowedFoldersVirtual[i], this->baseFoldersCrawlableFilesPhysical[i], false, false, 0
    );
    this->baseFoldersCrawlableFilesPhysical[i] = FileOperations::GetWouldBeFolderAfterHypotheticalChdirNonThreadSafe(
      this->baseFoldersCrawlableFilesPhysical[i]
    ) + "/";
  }
}

bool LaTeXcrawler::ExtractFileNamesFromRelativeFileName(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("LaTeXcrawler::ExtractFileNamesFromRelativeFileName");
  if (!FileOperations::IsOKfileNameVirtual(this->theFileNameToCrawlRelative)) {
    this->displayResult << "The folders below the file name contain dots. This is not allowed. ";
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "The folders below the file name contain dots. This is not allowed. ";
    }
    return false;
  }
  FileOperations::GetPhysicalFileNameFromVirtual(
    this->theFileNameToCrawlRelative, this->theFileNameToCrawlPhysicalWithPath, true, false, commentsOnFailure
  );
  this->baseFolderStartFilePhysical = FileOperations::GetWouldBeFolderAfterHypotheticalChdirNonThreadSafe(
    FileOperations::GetPathFromFileNameWithPath(this->theFileNameToCrawlPhysicalWithPath)
  ) + "/";

  this->theFileNameToCrawlPhysicalNoPathName = FileOperations::GetFileNameFromFileNameWithPath(
    this->theFileNameToCrawlPhysicalWithPath
  );
  this->ComputeAllowedFolders();
  if (!this->IsInCrawlableFolder(this->baseFolderStartFilePhysical, commentsOnFailure)) {
    return false;
  }
  return true;
}

void LaTeXcrawler::BuildFreecalC() {
  MacroRegisterFunctionWithName("LaTeXcrawler::BuildFreecalc");
  if (!theGlobalVariables.UserDefaultHasAdminRights()) {
    this->displayResult << "Build freecalc command allowed only for logged-in admins.";
    return;
  }
  theGlobalVariables.millisecondsMaxComputation = 50000000; //50k seconds: ok as we are admin.
  if (!this->ExtractFileNamesFromRelativeFileName(&this->errorStream)) {
    return;
  }
  std::fstream inputFile;
  if (!FileOperations::OpenFileUnsecure(inputFile, this->theFileNameToCrawlPhysicalWithPath, false, false, false)) {
    this->displayResult << "Failed to open input file: "
    << this->theFileNameToCrawlPhysicalWithPath << ", aborting. ";
    return;
  }
  inputFile.seekg(0);
  std::string buffer;
  ProgressReport theReport;
  std::stringstream reportStream;
  reportStream << "Processing input file: extracting lecture numbers...";
  theReport.Report(reportStream.str());
  bool isLecturE = false;
  bool isHW = false;
  int currentLineIndex = - 1;
  while (!inputFile.eof()) {
    std::getline(inputFile, buffer);
    currentLineIndex ++;
    bool isInput = false;
    if (
      MathRoutines::StringBeginsWith(buffer, "\\lect") &&
      !MathRoutines::StringBeginsWith(buffer, "\\lecture")
    ) {
      isInput = true;
      isLecturE = true;
    }
    if (MathRoutines::StringBeginsWith(buffer, "\\homeworkOnATopic")) {
      isInput = true;
      isHW = true;
    }
    if (this->flagBuildSingleSlides && isLecturE) {
      if (MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "\\input", 0)) {
        this->slideTexInputCommands.AddOnTop(MathRoutines::StringTrimWhiteSpace(buffer));
      }
    }
    if (!isInput) {
      continue;
    }
    int leftBracketsMinusRight = 0;
    int numBallancedBracketGroups = 0;
    std::string lectureNumber;
    bool recordingLectureNumber = false;
    for (unsigned i = 0; i < buffer.size(); i ++) {
      if (buffer[i] == '}') {
        leftBracketsMinusRight--;
        if (leftBracketsMinusRight == 0) {
          numBallancedBracketGroups ++;
          if (recordingLectureNumber) {
            break;
          }
        }
      }
      if (buffer[i] == '{') {
        leftBracketsMinusRight ++;
        if (leftBracketsMinusRight == 1 && numBallancedBracketGroups == 2) {
          recordingLectureNumber = true;
        }
      }
      if (recordingLectureNumber) {
        lectureNumber.push_back(buffer[i]);
      }
    }
    if (lectureNumber.size() > 0) {
      this->theLectureNumbers.AddOnTop(lectureNumber.substr(1, lectureNumber.size() - 1));
    } else {
      this->displayResult << "Failed to extract lecture/homework number from line: " << buffer;
      return;
    }
    std::getline(inputFile, buffer);
    std::string desiredName;
    if (!MathRoutines::StringBeginsWith(buffer, "%DesiredLectureName: ", &desiredName)) {
      if (!MathRoutines::StringBeginsWith(buffer, "%DesiredHomeworkName: ", &desiredName)) {
        desiredName = "";
      }
    }
    for (unsigned i = 0; i < desiredName.size(); i ++) {
      if (desiredName[i] == '.' || desiredName[i] == '/' || desiredName[i] == '\\') {
        desiredName = "";
      }
    }
    if (desiredName == "") {
      this->displayResult << "Failed to extract desired homework/lecture name from: " << buffer
      << "<br>This is the line immediately after the \\lect command. It should begin with the string \"%DesiredLectureName: \""
      << "(<-has space bar in the end). The name itself should not contain the characters . / or \\. "
      << "I am assigning an automatic file name. <br>";
    }
    theLectureDesiredNames.AddOnTop(desiredName);
  }
  reportStream << " done. Extracted: " << theLectureNumbers.size << " homework/lecture numbers. Preparing Homework/Lecture content ... ";
  theReport.Report(reportStream.str());
  if (isLecturE && isHW) {
    this->displayResult << "I was not able to determine whether the file is a homework or a lecture file. Aborting.";
    return;
  }
  if (!isLecturE && !isHW) {
    this->displayResult << "Could not find any lecture or homework entries. Aborting.";
    return;
  }
  std::stringstream LectureContentNoDocumentClassNoCurrentLecture;
  std::stringstream LectureHeaderNoDocumentClass;
  inputFile.clear();
  inputFile.seekg(0);
  bool foundFirstLecture = false;
  if (isLecturE) {
    while (!inputFile.eof()) {
      std::getline(inputFile, buffer);
      if (
        !MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "\\documentclass") &&
        !MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "[handout]") &&
        !MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "{beamer}") &&
        !MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "\\newcommand{\\currentLecture}")
      ) {
        LectureContentNoDocumentClassNoCurrentLecture << buffer << "\n";
      }
      if (this->flagBuildSingleSlides) {
        if (
          MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "\\lect") &&
          !MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "\\lecture")
        ) {
          foundFirstLecture = true;
        }
        if (!foundFirstLecture) {
          if (
            !MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "\\documentclass") &&
            !MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "[handout]") &&
            !MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "{beamer}")
          ) {
            LectureHeaderNoDocumentClass << buffer << "\n";
          }
        }
      }
    }
  } else {
    while (!inputFile.eof()) {
      std::getline(inputFile, buffer);
      if (!MathRoutines::StringBeginsWith(buffer, "\\newcommand{\\currentHW}")) {
        LectureContentNoDocumentClassNoCurrentLecture << buffer << "\n";
      }
    }
  }
  reportStream << " done. Proceding to compile homeworks/lectures. ";
  theReport.Report(reportStream.str());
  if (isLecturE) {
    this->displayResult << "<table><tr><td>Lecture number</td><td>Lecture name</td><td>Lecture pdf</td>"
    << "<td>Lecture handout pdf</td><td>Comments</td></tr>";
  } else {
    this->displayResult << "<table><tr><td>Homework number</td><td>Homework name</td><td>Homework pdf</td>"
    << "<td>Homework handout pdf</td><td>Comments</td></tr>";
  }
  std::string lectureFileNameEnd;
  if (!MathRoutines::StringBeginsWith(this->theFileNameToCrawlPhysicalNoPathName, "Lecture_", &lectureFileNameEnd)) {
    if (!MathRoutines::StringBeginsWith(this->theFileNameToCrawlPhysicalNoPathName, "Homework_", &lectureFileNameEnd)) {
      lectureFileNameEnd = "";
    }
  }
  std::string folderEnd = this->theFileNameToCrawlPhysicalNoPathName.substr(0, this->theFileNameToCrawlPhysicalNoPathName.size() - 4);
  if (lectureFileNameEnd.size() > 4) {
    lectureFileNameEnd = lectureFileNameEnd.substr(0, lectureFileNameEnd.size() - 4);
  }
  std::stringstream executedCommands, resultTable;
  std::string currentSysCommand; //"ch " + this->baseFolderStartFilePhysical+"\n\n\n\n";
  theGlobalVariables.ChDir(this->baseFolderStartFilePhysical);
  executedCommands
  << "<br>Directory changed to: " << this->baseFolderStartFilePhysical
  << ", making the current directory: "
  << FileOperations::GetCurrentFolder();
  reportStream
  << "<br>Directory changed to: " << this->baseFolderStartFilePhysical
  << ", making the current directory: "
  << FileOperations::GetCurrentFolder();

  std::string lectureProjectorFolder = "lectures_projector_" + folderEnd + "/";
  std::string printableFolder;
  std::string slideProjectorFolder, slideHandoutFolder;
  if (!isLecturE) {
    printableFolder = "homework_" + folderEnd + "/";
  } else {
    printableFolder = "lectures_printable_" + folderEnd + "/";
  }
  theGlobalVariables.CallSystemNoOutput("mkdir " + printableFolder, &logWorker);
  if (isLecturE) {
    theGlobalVariables.CallSystemNoOutput("mkdir " + lectureProjectorFolder, &logWorker);
    if (this->flagBuildSingleSlides) {
      slideProjectorFolder = "slides_projector_" + folderEnd + "/";
      slideHandoutFolder = "slides_handout_" + folderEnd + "/";
      theGlobalVariables.CallSystemNoOutput("mkdir " + slideProjectorFolder, &logWorker);
      theGlobalVariables.CallSystemNoOutput("mkdir " + slideHandoutFolder, &logWorker);
    }
  }
  this->theFileNameWorkingCopy = "working_file_" + this->theFileNameToCrawlPhysicalNoPathName;
  std::string theFileNameWorkingCopyPDF = "working_file_" +
  this->theFileNameToCrawlPhysicalNoPathName.substr(0, this->theFileNameToCrawlPhysicalNoPathName.size() - 3) + "pdf";
  int numLecturesToProcess = theLectureNumbers.size;
  std::fstream workingFile;
  for (int i = 0; i < numLecturesToProcess; i ++) {
    reportStream << "<br>Processing lecture " << i + 1 << " out of " << theLectureNumbers.size << ". ";
    resultTable << "<tr>";
    resultTable << "<td>" << theLectureNumbers[i] << "</td>";
    resultTable << "<td>" << theLectureDesiredNames[i] << "</td>";
    if (!FileOperations::OpenFileCreateIfNotPresentUnsecure(workingFile, this->theFileNameWorkingCopy, false, true, false)) {
      resultTable << "<td>-</td><td>-</td><td>Failed to open working file: "
      << this->theFileNameWorkingCopy << ", aborting. </td> </tr>";
      break;
    }
    if (isLecturE) {
      workingFile << "\\documentclass[handout]{beamer}\n\\newcommand{\\currentLecture}{"
      << theLectureNumbers[i] << "}\n"
      << LectureContentNoDocumentClassNoCurrentLecture.str();
    } else {
      workingFile << "\\newcommand{\\currentHW}{" << theLectureNumbers[i] << "}\n"
      << LectureContentNoDocumentClassNoCurrentLecture.str();
    }
    workingFile.close();
    currentSysCommand = "pdflatex -shell-escape " + this->theFileNameWorkingCopy;
    executedCommands << "<br>" << currentSysCommand;
    reportStream << currentSysCommand;
    theReport.Report(reportStream.str());
    theGlobalVariables.CallSystemNoOutput(currentSysCommand, &logWorker);
    reportStream << "<b>[x2]</b>";
    theReport.Report(reportStream.str());
    theGlobalVariables.CallSystemNoOutput(currentSysCommand, &logWorker);
    std::stringstream thePdfFileNameHandout;
    if (isLecturE) {
      thePdfFileNameHandout << "./" << printableFolder << "Lecture" << theLectureNumbers[i] << "Handout_"
      << theLectureDesiredNames[i];
      if (lectureFileNameEnd != "") {
        thePdfFileNameHandout << "_" << lectureFileNameEnd;
      }
      thePdfFileNameHandout << ".pdf";
    } else {
      thePdfFileNameHandout << "./" << printableFolder << "Homework" << theLectureNumbers[i] << "_" << theLectureDesiredNames[i];
      if (lectureFileNameEnd != "") {
        thePdfFileNameHandout << "_" << lectureFileNameEnd;
      }
      thePdfFileNameHandout << ".pdf";
    }
    currentSysCommand = "mv " + theFileNameWorkingCopyPDF + " " + thePdfFileNameHandout.str();
    executedCommands << "<br>" << currentSysCommand;
    reportStream << "<br>Lecture/Homework " << i + 1 << " handout compiled, renaming file ... ";
    theReport.Report(reportStream.str());
    theGlobalVariables.CallSystemNoOutput(currentSysCommand, &logWorker);
    reportStream << " done.";
    theReport.Report(reportStream.str());
    if (!isLecturE) {
      theReport.Report(reportStream.str());
      resultTable << "<td>" << thePdfFileNameHandout.str() << "</td>";
      resultTable << "</tr>";
      continue;
    }
    if (!FileOperations::OpenFileUnsecure(workingFile, this->theFileNameWorkingCopy, false, true, false)) {
      resultTable << "<td>-</td><td>-</td><td>Failed to open working file: "
      << this->theFileNameWorkingCopy << ", aborting. </td> </tr>";
      break;
    }
    workingFile << "\\documentclass";
    workingFile << "{beamer}\n\\newcommand{\\currentLecture}{"
    << theLectureNumbers[i] << "}\n";
    workingFile << LectureContentNoDocumentClassNoCurrentLecture.str();
    workingFile.close();
    currentSysCommand = "pdflatex -shell-escape " + this->theFileNameWorkingCopy;
    executedCommands << "<br>" << currentSysCommand;
    reportStream << currentSysCommand;
    theReport.Report(reportStream.str());
    theGlobalVariables.CallSystemNoOutput(currentSysCommand, &logWorker);
    std::stringstream thePdfFileNameNormal;
    thePdfFileNameNormal << "./" << lectureProjectorFolder << "Lecture"
    << theLectureNumbers[i] << "_" << theLectureDesiredNames[i] << "_"
    << lectureFileNameEnd << ".pdf";
    currentSysCommand = "mv " + theFileNameWorkingCopyPDF + " " + thePdfFileNameNormal.str();
    executedCommands << "<br>" << currentSysCommand;
    reportStream << "<br>Lecture " << i + 1 << " regular slides compiled, renaming file ... ";
    theReport.Report(reportStream.str());
    theGlobalVariables.CallSystemNoOutput(currentSysCommand, &logWorker);
    reportStream << " done.";
    theReport.Report(reportStream.str());
    resultTable << "<td>" << thePdfFileNameNormal.str() << "</td>" << "<td>" << thePdfFileNameHandout.str() << "</td>";
    resultTable << "</tr>";
  }
  int numSlidesToBuild = this->slideTexInputCommands.size;
  this->slideFileNamesVirtualNoPathNoExtensioN.SetSize(this->slideTexInputCommands.size);
  for (int i = 0; i < this->slideTexInputCommands.size; i ++) {
    this->slideFileNamesVirtualNoPathNoExtensioN[i] = "";
    if (this->slideTexInputCommands[i] == "") {
      continue;
    }
    std::string currentName = this->slideTexInputCommands[i].substr(0, this->slideTexInputCommands[i].size() - 1);
    this->slideFileNamesVirtualNoPathNoExtensioN[i] = FileOperations::GetFileNameFromFileNameWithPath(currentName);
  }
  //executedCommands << "<br>Slides extracted: " << this->theSlides.ToStringCommaDelimited();
  //executedCommands << "<br>Slides names: " << this->theSlideNames.ToStringCommaDelimited();
  for (int i = 0; i < numSlidesToBuild; i ++) {
    for (int k = 0; k < 2; k ++) {
      if (!FileOperations::OpenFileUnsecure(workingFile, this->theFileNameWorkingCopy, false, true, false)) {
        resultTable << "<td>-</td><td>-</td><td>Failed to open working file: "
        << this->theFileNameWorkingCopy << ", aborting. </td> </tr>";
        break;
      }
      workingFile << "\\documentclass";
      if (k == 1) {
        workingFile << "[handout]";
      }
      workingFile << "{beamer}";
      workingFile << LectureHeaderNoDocumentClass.str()
      << "\n"
      << this->slideTexInputCommands[i]
      << "\n"
      << "\\end{document}";
      workingFile.close();
      currentSysCommand = "pdflatex -shell-escape " + this->theFileNameWorkingCopy;
      executedCommands << "<br>" << currentSysCommand;
      reportStream << currentSysCommand;
      theReport.Report(reportStream.str());
      theGlobalVariables.CallSystemNoOutput(currentSysCommand, &logWorker);
      std::stringstream thePdfFileNameNormal;
      thePdfFileNameNormal << "./";
      if (k == 0) {
        thePdfFileNameNormal << slideProjectorFolder;
      } else {
        thePdfFileNameNormal << slideHandoutFolder;
      }
      thePdfFileNameNormal << "Slide_"
      << this->slideFileNamesVirtualNoPathNoExtensioN[i] << ".pdf";
      currentSysCommand = "mv " + theFileNameWorkingCopyPDF + " " + thePdfFileNameNormal.str();
      executedCommands << "<br>" << currentSysCommand;
      reportStream << "<br>Slide " << i + 1 << ", run " << k << " compiled, renaming file ... ";
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

void LaTeXcrawler::Crawl() {
  MacroRegisterFunctionWithName("LaTeXcrawler::Crawl");
  if (!this->ExtractFileNamesFromRelativeFileName(&this->displayResult)) {
    return;
  }
  this->recursionDepth = 0;
  std::stringstream crawlingResult;

  std::string startingDirectory = FileOperations::GetCurrentFolder();
  //stOutput << "DEBUG: starting dir: " << startingDirectory << "<br>";
  theGlobalVariables.ChDir(this->baseFolderStartFilePhysical);
  //stOutput << "DEBUG: after ch dir: " << FileOperations::GetCurrentFolder() << "<br>";
  this->CrawlRecursive(crawlingResult, this->baseFolderStartFilePhysical + this->theFileNameToCrawlPhysicalNoPathName);
  theGlobalVariables.ChDir(startingDirectory);
  //stOutput << "DEBUG: After crawling, current dir: " << FileOperations::GetCurrentFolder() << "<br>";
  std::fstream outputFile;
  std::string outputFileName = "latexOutput.tex";
  if (!FileOperations::OpenFileCreateIfNotPresentVirtual(
    outputFile, "output/" + outputFileName, false, true, false, true)
  ) {
    this->displayResult << "Failed to open output file: " << outputFileName << ", check write permissions. ";
    //this->displayResult << "The crawling appears to have been successful, below is the crawled file.<hr>"
    //<< this->crawlingResult.str();
    return;
  }
  outputFile << crawlingResult.str();
  if (this->errorStream.str() != "") {
    this->displayResult << "Errors encountered. " << this->errorStream.str();
  }
  this->displayResult << "Output file: <a href=\"" << theGlobalVariables.DisplayPathOutputFolder
  << "latexOutput.tex\">" << "latexOutput.tex" << "</a>";
}

LaTeXcrawler::LaTeXcrawler() {
  this->flagBuildSingleSlides = false;
  this->flagProjectorMode = true;
  this->flagForceSlideRebuild = false;
  this->flagAddSlideToSVN = false;
  this->flagDoChangeDirs = true;
  this->flagCrawlTexSourcesRecursively = false;
  this->flagAnswerKey = false;
  this->flagHomeworkRatherThanSlides = false;
  this->flagSourceOnly = false;
  this->ownerCalculator = 0;
  this->recursionDepth = 0;
}

void LaTeXcrawler::CrawlRecursive(std::stringstream& crawlingResult, const std::string& currentFileNamE) {
  MacroRegisterFunctionWithName("LaTeXcrawler::CrawlRecursive");
  RecursionDepthCounter theCounter(&this->recursionDepth);
  if (this->recursionDepth > 1000) {
    this->errorStream << "While crawling theFileToCrawl, reached max recursion depth of 1000";
    return;
  }
  //this->crawlingResult << "%DEBUG: before error stream: opening: " << currentFileName << "\n";
  if (this->errorStream.str() != "")
    return;
  ///////////
  /// ALERT
  /// potential security risk.
  /// Not sure if the code below is secure enough.
  /// If the folders read are not sanitized properly,
  /// an attacker could get a .tex file with our site's private keys!
  /// Touch very carefully.
  if (this->baseFoldersCrawlableFilesPhysical.size == 0) {
    crash << "Error: this->baseFoldersCrawlableFilesPhysical is empty which is not allowed here. " << crash;
  }
  std::string trimmedFileName = MathRoutines::StringTrimWhiteSpace(currentFileNamE);
  std::string trimmedFolder = FileOperations::GetPathFromFileNameWithPath(trimmedFileName);
  std::string resultingFolder = FileOperations::GetWouldBeFolderAfterHypotheticalChdirNonThreadSafe(trimmedFolder);
  if (!this->IsInCrawlableFolder(resultingFolder, &this->errorStream)) {
    this->errorStream << "Error: file " << trimmedFileName << " appears to be located in "
    << resultingFolder << ", which in turn does not appear to be a sub-folder "
    << "of the designated crawlable folders: "
    << this->baseFoldersCrawlableFilesPhysical.ToStringCommaDelimited();
    crawlingResult << "%Error: file " << trimmedFileName << " appears to be located in "
    << resultingFolder << ", which in turn does not appear to be a sub-folder "
    << "of the designated crawlable folders: "
    << this->baseFoldersCrawlableFilesPhysical.ToStringCommaDelimited() << "\n";
    return;
  }
  std::fstream theFile;
  if (!FileOperations::OpenFileUnsecure(theFile, trimmedFileName, false, false, false)) {
    this->errorStream << "Failed to open file " << trimmedFileName << "; current folder: "
    << FileOperations::GetCurrentFolder() << ". Aborting.<br>";
    crawlingResult << "%Failed to open file: " << trimmedFileName << "\n";
    return;
  }
  std::string buffer;
  while (!theFile.eof()) {
    std::getline(theFile, buffer);
    std::string newFileName;
    bool isGood = true;
    if (buffer.find("IfFileExists") != std::string::npos) {
      isGood = false;
    }
    if (buffer.size() > 0) {
      if (buffer[0] == '%') {
        isGood = false;
      }
    }
    int foundInput = buffer.find("\\input");
    if (isGood) {
      if (((unsigned) foundInput) < buffer.size()) {
        crawlingResult << buffer.substr(0, foundInput);
        buffer = buffer.substr(foundInput);
        unsigned i = 0;
        newFileName = "";
        for (i = 7; buffer[i] != '}' && i < buffer.size(); i ++) {
          newFileName += buffer[i];
        }
        newFileName = MathRoutines::StringTrimWhiteSpace(newFileName);
        std::string newFileNameEnd;
        if (MathRoutines::StringBeginsWith(newFileName, "\\freecalcBaseFolder", &newFileNameEnd)) {
          newFileName = "../../freecalc" + newFileNameEnd;
        }
        newFileName += ".tex";
        crawlingResult << "%input from file: " << newFileName << "\n";
        this->CrawlRecursive(crawlingResult, newFileName);
        crawlingResult << "\n";
        if (i + 1 < buffer.size()) {
          buffer = buffer.substr(i + 1);
        } else {
          continue;
        }
      }
    }
    crawlingResult << buffer;
    crawlingResult << "\n";
  }
}

bool LaTeXcrawler::ExtractPresentationFileNames(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral) {
  MacroRegisterFunctionWithName("LaTeXcrawler::ExtractPresentationFileNames");
  (void) commentsGeneral;
  if (this->slideFileNamesVirtualWithPatH.size < 1) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Could not find slide/homework file names. ";
    }
    return false;
  }
  this->slideFileNamesWithLatexPathNoExtension.initializeFillInObject(this->slideFileNamesVirtualWithPatH.size, "");
  this->latexSnippets.initializeFillInObject(this->slideFileNamesVirtualWithPatH.size, "");
  for (int i = 0; i < this->slideFileNamesVirtualWithPatH.size; i ++) {
    if (MathRoutines::StringBeginsWith(this->slideFileNamesVirtualWithPatH[i], "LaTeX: ", &this->latexSnippets[i])) {
      if (i == 0) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "Found LaTeX snippet without a header file. "
          << this->slideFileNamesVirtualWithPatH[i] << "<br>";
        }
        return false;
      }
      continue;
    }
    if (!MathRoutines::StringEndsWith(this->slideFileNamesVirtualWithPatH[i], ".tex")) {
      this->slideFileNamesVirtualWithPatH[i] += ".tex";
    }
    if (!FileOperations::IsFileNameSafeForSystemCommands(this->slideFileNamesVirtualWithPatH[i], commentsOnFailure)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Found unsafe slide name: " << this->slideFileNamesVirtualWithPatH[i] << "<br>";
      }
      return false;
    }
    if (!FileOperations::IsOKfileNameVirtual(this->slideFileNamesVirtualWithPatH[i], false, commentsOnFailure)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Found invalid slide name: " << this->slideFileNamesVirtualWithPatH[i] << "<br>";
      }
      return false;
    }
    if (theGlobalVariables.UserDefaultHasAdminRights()) {
      if (!FileOperations::FileExistsVirtual(this->slideFileNamesVirtualWithPatH[i], false, false)) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "Failed to find file: " << this->slideFileNamesVirtualWithPatH[i] << "<br>";
        }
        return false;
      }
    }
    FileOperations::GetFileExtensionWithDot(
      this->slideFileNamesVirtualWithPatH[i], &this->slideFileNamesWithLatexPathNoExtension[i]
    );
    if (MathRoutines::StringBeginsWith(this->slideFileNamesWithLatexPathNoExtension[i], "freecalc", 0)) {
      this->slideFileNamesWithLatexPathNoExtension[i] = "../../" + this->slideFileNamesWithLatexPathNoExtension[i];
    }
    if (MathRoutines::StringBeginsWith(this->slideFileNamesWithLatexPathNoExtension[i], "LaTeX-materials", 0)) {
      this->slideFileNamesWithLatexPathNoExtension[i] = "../../" + this->slideFileNamesWithLatexPathNoExtension[i];
    }
  }
  this->headerFileNameWithPathVirtual = this->slideFileNamesVirtualWithPatH[0];
  this->headerFilePathVirtual = FileOperations::GetPathFromFileNameWithPath(this->headerFileNameWithPathVirtual);
  this->headerFileNameNoPath = FileOperations::GetFileNameFromFileNameWithPath(this->headerFileNameWithPathVirtual);
  this->headerPathBelowFileNameVirtual = this->headerFilePathVirtual;
  if (this->headerPathBelowFileNameVirtual.size() > 0) {
    this->headerPathBelowFileNameVirtual = this->headerPathBelowFileNameVirtual.substr(
      0, this->headerPathBelowFileNameVirtual.size() - 1
    );
    this->headerPathBelowFileNameVirtual = FileOperations::GetFileNameFromFileNameWithPath(
      this->headerPathBelowFileNameVirtual
    );
  }
  if (!FileOperations::GetPhysicalFileNameFromVirtual(
    this->headerFilePathVirtual, this->workingFilePathPhysical, false, false, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to extract physical path from: " << this->headerFilePathVirtual;
    }
    return false;
  }
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "Working file path physical: " << this->workingFilePathPhysical
    << " extracted from virtual path: " << this->headerFilePathVirtual << "\n";
  }
  FileOperations::GetFileExtensionWithDot(this->headerFileNameNoPath, &this->headerFileNameNoPathNoExtension);
  this->workingFileNameNoPathTex = "workingfile" + this->headerFileNameNoPathNoExtension + ".tex";
  this->workingFileNameNoPathPDF = "workingfile" + this->headerFileNameNoPathNoExtension + ".pdf";
  std::string firstSignificantSlideName = "";
  if (this->slideFileNamesVirtualWithPatH.size >= 1) {
    firstSignificantSlideName = this->slideFileNamesVirtualWithPatH[0];
  }
  for (int i = 1; i < this->slideFileNamesVirtualWithPatH.size; i ++) {
    if (this->latexSnippets[i] == "") {
      firstSignificantSlideName = this->slideFileNamesVirtualWithPatH[i];
      break;
    }
  }
  this->targetPDFVirtualPath = FileOperations::GetPathFromFileNameWithPath(firstSignificantSlideName);
  std::string tempString;
  if (MathRoutines::StringBeginsWith(this->targetPDFVirtualPath, "freecalc", &tempString)) {
    this->targetPDFVirtualPath = "slides-video" + tempString;
  }
  if (MathRoutines::StringBeginsWith(this->targetPDFVirtualPath, "LaTeX-materials", &tempString)) {
    this->targetPDFVirtualPath = "slides-video" + tempString;
  }
  this->targetPDFNoPath = FileOperations::GetFileNameFromFileNameWithPath(firstSignificantSlideName);
  FileOperations::GetFileExtensionWithDot(this->targetPDFNoPath, &this->targetPDFNoPath);
  if (!this->flagHomeworkRatherThanSlides) {
    if (this->flagProjectorMode) {
      this->targetPDFNoPath += "-projector-" + this->headerPathBelowFileNameVirtual;
    } else {
      this->targetPDFNoPath += "-printable-" + this->headerPathBelowFileNameVirtual;
    }
  } else {
    if (this->flagAnswerKey) {
      this->targetPDFNoPath += "-answer-key-" + this->headerPathBelowFileNameVirtual;
    } else {
      this->targetPDFNoPath += "-no-answers-" + this->headerPathBelowFileNameVirtual;
    }
  }
  this->targetPDFNoPath += this->desiredPresentationTitle;
  this->targetPDFNoPath = HtmlRoutines::ConvertStringToURLString(this->targetPDFNoPath, false);
  MathRoutines::StringTrimToLengthWithHash(this->targetPDFNoPath, 230);
  this->targetPDFNoPath += ".pdf";
  this->targetPDFFileNameWithPathVirtual = this->targetPDFVirtualPath + this->targetPDFNoPath;
  this->targetPDFLatexPath = "../../" + this->targetPDFVirtualPath;
  if (!MathRoutines::StringBeginsWith(this->targetPDFVirtualPath, "slides-video/modules/", &tempString)) {
    this->targetVideoLatexPath = "";
  } else {
    this->targetVideoLatexPath = "../../slides-video/modules-video/" + tempString;
  }
  this->targetPDFFileNameWithLatexPath = this->targetPDFLatexPath + this->targetPDFNoPath;
  return true;
}

std::string LaTeXcrawler::AdjustDisplayTitle(const std::string& input, bool isHomework) {
  MacroRegisterFunctionWithName("LaTeXcrawler::AdjustDisplayTitle");
  std::string result = input;
  List<std::string> ignoredTags;
  ignoredTags.AddOnTop("actualExamProblem");
  ignoredTags.AddOnTop("lectureTag");
  ignoredTags.AddOnTop("advancedTopic");
  ignoredTags.AddOnTop("reviewProblem");
  for (int i = 0; i < ignoredTags.size; i ++) {
    std::string closeTag = "</" + ignoredTags[i] + ">";
    std::string openTag  = "<"  + ignoredTags[i] + ">";
    if (input.find(openTag) != std::string::npos && input.find(closeTag) != std::string::npos) {
      int start = input.find(openTag);
      int finish = input.find(closeTag) + closeTag.size();
      result = result.substr(0, start) + result.substr(finish);
    }
  }
  if (result.find("\\(\\LaTeX\\)") != std::string::npos) {
    int pos = result.find("\\(\\LaTeX\\)");
    result = result.substr(0, pos) + "\\LaTeX" + result.substr(pos + ((std::string) "\\(\\LaTeX\\)").size());
  }
  result = MathRoutines::StringTrimWhiteSpace(result);
  if (isHomework && result.size() > 4 && !MathRoutines::StringBeginsWith(result, "\\\\")) {
    result = "\\\\" + result;
  }
  return result;
}

bool LaTeXcrawler::BuildOrFetchFromCachePDF(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral) {
  MacroRegisterFunctionWithName("LaTeXcrawler::BuildOrFetchFromCachePDF");
  StateMaintainerCurrentFolder preserveCurrentFolder;
  this->desiredPresentationTitle = this->AdjustDisplayTitle(this->desiredPresentationTitle, this->flagHomeworkRatherThanSlides);
  if (!this->ExtractPresentationFileNames(commentsOnFailure, commentsGeneral)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to extract file names. ";
    }
    return false;
  }
  bool pdfExists = FileOperations::FileExistsVirtual(this->targetPDFFileNameWithPathVirtual, false, false, commentsOnFailure);
  if (!this->flagForceSlideRebuild && pdfExists && !this->flagSourceOnly) {
    return FileOperations::LoadFileToStringVirtual(
      this->targetPDFFileNameWithPathVirtual, this->targetPDFbinaryContent, false, false, commentsOnFailure
    );
  }
  if (!theGlobalVariables.UserDefaultHasAdminRights()) {
    if (!pdfExists || !this->flagSourceOnly) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Pdf of slides not created. Only logged-in admins can compile pdfs. "
        << "Computed file name: <br>"
        << HtmlRoutines::ConvertStringToHtmlString(this->targetPDFFileNameWithPathVirtual, false);
      }
      return false;
    }
  }
  std::stringstream tempStream;
  if (commentsOnFailure == 0) {
    commentsOnFailure = &tempStream;
  }
  std::stringstream crawlingResult;
  std::fstream theFile;
  if (!this->flagCrawlTexSourcesRecursively) {
    if (!FileOperations::OpenFileVirtual(theFile, this->headerFileNameWithPathVirtual, false, false, false)) {
      return false;
    }
    std::string buffer;
    do {
      std::getline(theFile, buffer);
      if (!MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "[handout]")) {
        crawlingResult << buffer << "\n";
      }
      if (!this->flagProjectorMode && !this->flagHomeworkRatherThanSlides) {
        if (MathRoutines::StringBeginsWith(MathRoutines::StringTrimWhiteSpace(buffer), "\\documentclass")) {
          crawlingResult << "[handout]" << "\n";
        }
      }
    } while (!theFile.eof());
    theFile.close();
  }
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "About to change dir to: " << this->workingFilePathPhysical << "<br>";
  }
  bool addExtraTex = (this->slideFileNamesVirtualWithPatH.size > 1);
  if (this->flagDoChangeDirs) {
    theGlobalVariables.ChDir(this->workingFilePathPhysical);
  }
  if (this->flagCrawlTexSourcesRecursively) {
    crawlingResult << "%file automatically generated from file: " << this->headerFileNameNoPath
    << "\n%This file compiles with pdflatex -shell-escape\n";
    if (!this->flagHomeworkRatherThanSlides) {
      crawlingResult << "\n%Comment out/in the [handout] line to get the slide in projector/handout mode.\n";
    } else {
      crawlingResult << "\n%Use \\togglefalse{answers} and "
      << "\\togglefalse{solutions} to turn off the answer key/solutions.\n";
    }
    this->ComputeAllowedFolders();
    this->CrawlRecursive(crawlingResult, this->headerFileNameNoPath);
  }
  if (addExtraTex) {
    if (this->flagHomeworkRatherThanSlides) {
      if (this->flagAnswerKey) {
        crawlingResult << "\\toggletrue{solutions}\n\\toggletrue{answers}\n";
      } else {
        crawlingResult << "\\togglefalse{solutions}\n\\togglefalse{answers}\n";
      }
    }
    crawlingResult << "\\begin{document}\n";
    if (!this->flagHomeworkRatherThanSlides) {
      crawlingResult << "\\providecommand{\\currentLecture}{1}\n";
    } else {
      crawlingResult << "\\providecommand{\\currentHW}{1}\n";
    }
    if (!this->flagHomeworkRatherThanSlides) {
      crawlingResult << "\\lect{\\semester}{" << this->desiredPresentationTitle << "}{1}{\n";
    } else {
      crawlingResult << "\\homeworkOnATopic{}{" << this->desiredPresentationTitle << "}{1}{\n";
    }
    for (int i = 1; i < this->slideFileNamesVirtualWithPatH.size; i ++) {
      if (this->latexSnippets[i] == "") {
        if (this->flagHomeworkRatherThanSlides) {
          bool doIncludeItem = true;
          if (i < this->slideFilesExtraFlags.size) {
            if (this->slideFilesExtraFlags[i] == "true") {
              doIncludeItem = false;
            }
          }
          if (doIncludeItem) {
            crawlingResult << "\\item\n";
          }
        }
        if (this->flagCrawlTexSourcesRecursively) {
          crawlingResult << "%input from file: " << this->slideFileNamesWithLatexPathNoExtension[i] << ".tex\n";
          this->CrawlRecursive(crawlingResult, this->slideFileNamesWithLatexPathNoExtension[i] + ".tex");
        } else {
          crawlingResult << "\\input{" << this->slideFileNamesWithLatexPathNoExtension[i] << "}\n";
        }
      } else {
        crawlingResult << this->latexSnippets[i] << "\n";
      }
    }
    crawlingResult << "}\n";
    crawlingResult << "\\end{document}";
  }
  if (this->flagCrawlTexSourcesRecursively || this->flagSourceOnly) {
    this->targetLaTeX = crawlingResult.str();
    return true;
  }
  if (FileOperations::OpenFileCreateIfNotPresentUnsecure(theFile, this->workingFileNameNoPathTex, false, true, false)) {
    theFile << crawlingResult.str();
    if (commentsGeneral != nullptr) {
      *commentsGeneral << "Stored working file: " << this->workingFileNameNoPathTex << "<br>";
    }
  } else if (commentsGeneral != nullptr) {
    *commentsGeneral << "FAILED to store file: " << this->workingFileNameNoPathTex << "<br>";
  }
  theFile.close();
  std::string currentSysCommand = "pdflatex -shell-escape " + this->workingFileNameNoPathTex;
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "Executing command: " << currentSysCommand << " ... ";
  }
  theGlobalVariables.CallSystemNoOutput(currentSysCommand, &logWorker);
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "done!<br>";
  }
  currentSysCommand = "mkdir -p " + this->targetPDFLatexPath;
  std::string commandResult = theGlobalVariables.CallSystemWithOutput(currentSysCommand);
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "Executed command: " << currentSysCommand << " ... to get result: " << commandResult << "<br>";
  }
  if (this->targetVideoLatexPath!= "") {
    currentSysCommand = "mkdir -p " + this->targetVideoLatexPath;
    std::string commandResult = theGlobalVariables.CallSystemWithOutput(currentSysCommand);
    if (commentsGeneral != nullptr) {
      *commentsGeneral << "Executed command: " << currentSysCommand << " ... to get result: " << commandResult << "<br>";
    }
  }
  currentSysCommand = "mv " + this->workingFileNameNoPathPDF + " " + this->targetPDFFileNameWithLatexPath;
  commandResult = theGlobalVariables.CallSystemWithOutput(currentSysCommand);
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "Executed command: " << currentSysCommand
    << " ... to get result: " << commandResult << "<br>";
  }
  if (!FileOperations::LoadFileToStringUnsecure(
    this->targetPDFFileNameWithLatexPath, this->targetPDFbinaryContent, commentsOnFailure
  )) {
    return false;
  }
  if (!FileOperations::IsFileNameSafeForSystemCommands(this->targetPDFFileNameWithLatexPath, commentsOnFailure)) {
    return true;
  }
  //  std::stringstream svnAddFileCommand, svnAddDirCommand, svnResult;
  //Old code to add pdf files to svn repo:
  //svnAddDirCommand << "svn add " << this->targetPDFLatexPath << " --depth=empty";
  //svnResult << "<br>Command: " << svnAddDirCommand.str() << "<br>Result: ";
  //svnResult << theGlobalVariables.CallSystemWithOutput(svnAddDirCommand.str());
  //svnAddFileCommand << "svn add " << this->targetPDFFileNameWithLatexPath;
  //svnResult << "<br>Command: " << svnAddFileCommand.str() << "<br>Result: ";
  //svnResult << theGlobalVariables.CallSystemWithOutput(svnAddFileCommand.str());
  //if (commentsGeneral != nullptr)
  //  *commentsGeneral << svnResult.str();
  return true;
}

#include "vpfHeader8HtmlInterpretation.h"
bool LaTeXcrawler::BuildTopicList(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral) {
  MacroRegisterFunctionWithName("LaTeXcrawler::BuildTopicList");
  //stOutput << "DEBUG: Here we are. ";
  ProgressReport theReport;
  CalculatorHTML topicParser;
  std::stringstream temp;
  if (commentsGeneral == 0) {
    commentsGeneral = &temp;
  }
  topicParser.LoadFileNames();
  if (!topicParser.LoadAndParseTopicList(*commentsOnFailure)) {
    return false;
  }
  int numSlidePairsToBuild = 0;
  for (int i = 0; i < topicParser.theTopicS.size(); i ++) {
    if (topicParser.theTopicS.theValues[i].sourceSlides.size > 0) {
      numSlidePairsToBuild ++;
    }
  }
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "Loaded topic list: " << topicParser.topicListFileName;
    *commentsGeneral << "<br> " << numSlidePairsToBuild << " slide pairs to build ";
    *commentsGeneral << "(" << topicParser.theTopicS.size() << " total topic elements)";
  }
  bool result = true;
  int numProcessed = 0;
  this->slideFileNamesVirtualWithPatH.AddListOnTop(topicParser.sourcesHomeworkHeaders);
  this->slideFilesExtraFlags.initializeFillInObject(this->slideFileNamesVirtualWithPatH.size, "");
  for (int i = 0; i < topicParser.theTopicS.size()
//        && numProcessed < 2
        ; i ++) {
    TopicElement& currentElt = topicParser.theTopicS.theValues[i];
    if (currentElt.sourceHomework.size == 0) {
      continue;
    }
    std::stringstream reportStream;
    numProcessed ++;
    reportStream << "Processing homework pdfs: " << numProcessed << " out of " << numSlidePairsToBuild << ". ";
    theReport.Report(reportStream.str());

    this->slideFileNamesVirtualWithPatH.SetSize(topicParser.sourcesHomeworkHeaders.size);
    this->slideFilesExtraFlags.SetSize(topicParser.sourcesHomeworkHeaders.size);
    this->slideFileNamesVirtualWithPatH.AddListOnTop(currentElt.sourceHomework);
    for (int i = 0; i < currentElt.sourceHomeworkIsSolution.size; i ++) {
      std::string currentFlag = "false";
      if (currentElt.sourceHomeworkIsSolution[i]) {
        currentFlag = "true";
      }
      this->slideFilesExtraFlags.AddOnTop(currentFlag);
    }
    this->desiredPresentationTitle = currentElt.title;
    this->flagForceSlideRebuild = true;
    this->flagProjectorMode = false;
    this->flagAddSlideToSVN = false;
    this->flagHomeworkRatherThanSlides = true;
    this->flagAnswerKey = false;
    if (commentsGeneral != nullptr) {
      *commentsGeneral << "<br>Build homework pair from: "
      << this->slideFileNamesVirtualWithPatH.ToStringCommaDelimited();
    }
    if (!this->BuildOrFetchFromCachePDF(commentsOnFailure, commentsGeneral)) {
      result = false;
    }
    this->flagDoChangeDirs = false;
    this->flagAddSlideToSVN = true;
    if (!this->BuildOrFetchFromCachePDF(commentsOnFailure, commentsGeneral)) {
      result = false;
    }
    this->flagAnswerKey = true;
    if (!this->BuildOrFetchFromCachePDF(commentsOnFailure, commentsGeneral)) {
      result = false;
    }
  }

  this->slideFileNamesVirtualWithPatH.SetSize(0);

  numProcessed = 0;
  this->slideFileNamesVirtualWithPatH.AddListOnTop(topicParser.slidesSourcesHeaders);
  for (int i = 0; i < topicParser.theTopicS.size()
//        && numProcessed < 2
        ; i ++) {
    TopicElement& currentElt = topicParser.theTopicS.theValues[i];
    if (currentElt.sourceSlides.size == 0) {
      continue;
    }
    std::stringstream reportStream;
    numProcessed ++;
    reportStream << "Processing lecture slides: " << numProcessed << " out of " << numSlidePairsToBuild << ". ";
    reportStream << "<br>Slide file names: " << this->slideFileNamesVirtualWithPatH.ToStringCommaDelimited();
    theReport.Report(reportStream.str());

    this->slideFileNamesVirtualWithPatH.SetSize(topicParser.slidesSourcesHeaders.size);
    this->slideFileNamesVirtualWithPatH.AddListOnTop(currentElt.sourceSlides);
    this->desiredPresentationTitle = currentElt.title;
    this->flagForceSlideRebuild = true;
    this->flagProjectorMode = false;
    this->flagAddSlideToSVN = false;
    this->flagHomeworkRatherThanSlides = false;
    if (commentsGeneral != nullptr) {
      *commentsGeneral << "<br>Build slide pair from: "
      << this->slideFileNamesVirtualWithPatH.ToStringCommaDelimited();
    }
    if (!this->BuildOrFetchFromCachePDF(commentsOnFailure, commentsGeneral)) {
      result = false;
    }
    this->flagDoChangeDirs = false;
    this->flagAddSlideToSVN = true;
    this->flagProjectorMode = false;
    if (!this->BuildOrFetchFromCachePDF(commentsOnFailure, commentsGeneral)) {
      result = false;
    }
    this->flagProjectorMode = true;
    if (!this->BuildOrFetchFromCachePDF(commentsOnFailure, commentsGeneral)) {
      result = false;
    }
  }
  return result;
}
