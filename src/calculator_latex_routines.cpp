// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_extra_latex_routines.h"
#include "general_file_operations_encodings.h"
#include "general_logging_global_variables.h"
#include "web_api.h"
#include "general_strings.h"
#include "calculator_html_interpretation.h"

bool LaTeXCrawler::isInCrawlableFolder(
  const std::string& folderName, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("LaTeXcrawler::isInCrawlableFolder");
  for (int i = 0; i < this->baseFoldersCrawlableFilesPhysical.size; i ++) {
    if (
      StringRoutines::stringBeginsWith(
        folderName, this->baseFoldersCrawlableFilesPhysical[i]
      )
    ) {
      return true;
    }
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure
    << "File folder: "
    << folderName
    << " does not appear to be in one of the allowed folders: "
    << this->baseFoldersCrawlableFilesPhysical.toStringCommaDelimited()
    << ". ";
  }
  return false;
}

void LaTeXCrawler::computeAllowedFolders() {
  STACK_TRACE("LaTeXcrawler::computeAllowedFolders");
  if (this->baseFoldersCrawlableFilesPhysical.size > 0) {
    return;
  }
  List<std::string> allowedFoldersVirtual;
  allowedFoldersVirtual.addOnTop("freecalc/");
  allowedFoldersVirtual.addOnTop("LaTeX-materials/");
  allowedFoldersVirtual.addOnTop("/freecalc/");
  allowedFoldersVirtual.addOnTop("/LaTeX-materials/");
  this->baseFoldersCrawlableFilesPhysical.setSize(allowedFoldersVirtual.size);
  for (int i = 0; i < this->baseFoldersCrawlableFilesPhysical.size; i ++) {
    FileOperations::getPhysicalFileNameFromVirtual(
      allowedFoldersVirtual[i],
      this->baseFoldersCrawlableFilesPhysical[i],
      false,
      false,
      nullptr
    );
    this->baseFoldersCrawlableFilesPhysical[i] =
    FileOperations::getWouldBeFolderAfterHypotheticalChdirNonThreadSafe(
      this->baseFoldersCrawlableFilesPhysical[i]
    ) +
    "/";
  }
}

LaTeXCrawler::FileWithOption::FileWithOption() {
  this->isSolution = false;
}

LaTeXCrawler::FileWithOption::FileWithOption(const std::string& input) {
  this->fileName = input;
  this->isSolution = false;
}

void LaTeXCrawler::addSlidesOnTop(List<std::string>& inputSlides) {
  for (int i = 0; i < inputSlides.size; i ++) {
    LaTeXCrawler::FileWithOption file;
    file.fileName = inputSlides[i];
    this->slideFileNamesVirtualWithPath.addOnTop(file);
  }
}

bool LaTeXCrawler::extractFileNamesFromRelativeFileName(
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("LaTeXcrawler::extractFileNamesFromRelativeFileName");
  if (
    !FileOperations::isOKFileNameVirtual(this->fileNameToCrawlRelative)
  ) {
    this->displayResult
    << "The folders below the file name contain dots. This is not allowed. ";
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "The folders below the file name contain dots. This is not allowed. ";
    }
    return false;
  }
  FileOperations::getPhysicalFileNameFromVirtual(
    this->fileNameToCrawlRelative,
    this->fileNameToCrawlPhysicalWithPath,
    true,
    false,
    commentsOnFailure
  );
  std::string basePath =
  FileOperations::getPathFromFileNameWithPath(
    this->fileNameToCrawlPhysicalWithPath
  );
  this->baseFolderStartFilePhysical =
  FileOperations::getWouldBeFolderAfterHypotheticalChdirNonThreadSafe(basePath)
  +
  "/";
  this->fileNameToCrawlPhysicalNoPathName =
  FileOperations::getFileNameFromFileNameWithPath(
    this->fileNameToCrawlPhysicalWithPath
  );
  this->computeAllowedFolders();
  if (
    !this->isInCrawlableFolder(
      this->baseFolderStartFilePhysical, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Bad crawlable folder, extracted from user input: "
      << this->fileNameToCrawlRelative
      << " corresponsing to file: "
      << fileNameToCrawlPhysicalWithPath
      << " in path: "
      << basePath;
    }
    return false;
  }
  return true;
}

void LaTeXCrawler::buildFreecalc() {
  STACK_TRACE("LaTeXcrawler::BuildFreecalc");
  StateMaintainerCurrentFolder preserveCurrentFolder;
  if (!global.userDefaultHasAdminRights()) {
    this->displayResult
    << "Build freecalc command allowed only for logged-in administrators.";
    return;
  }
  global.millisecondsMaxComputation = 50000000;
  // 50k seconds: ok as we are administrator.
  if (!this->extractFileNamesFromRelativeFileName(&this->errorStream)) {
    return;
  }
  std::fstream inputFile;
  if (
    !FileOperations::openFileUnsecure(
      inputFile,
      this->fileNameToCrawlPhysicalWithPath,
      false,
      false,
      false
    )
  ) {
    this->displayResult
    << "Failed to open input file: "
    << this->fileNameToCrawlPhysicalWithPath
    << ", aborting. ";
    return;
  }
  inputFile.seekg(0);
  std::string buffer;
  ProgressReport report;
  std::stringstream reportStream;
  reportStream << "Processing input file: extracting lecture numbers...";
  report.report(reportStream.str());
  bool isLecture = false;
  bool isHomework = false;
  int currentLineIndex = - 1;
  while (!inputFile.eof()) {
    std::getline(inputFile, buffer);
    currentLineIndex ++;
    bool isInput = false;
    if (
      StringRoutines::stringBeginsWith(buffer, "\\lect") &&
      !StringRoutines::stringBeginsWith(buffer, "\\lecture")
    ) {
      isInput = true;
      isLecture = true;
    }
    if (
      StringRoutines::stringContains(buffer, "\\homeworkOnATopic") ||
      StringRoutines::stringContains(buffer, "\\input{../homework-problems}")
    ) {
      isHomework = true;
    }
    if (this->flagBuildSingleSlides && isLecture) {
      if (
        StringRoutines::stringBeginsWith(
          StringRoutines::stringTrimWhiteSpace(buffer),
          "\\input",
          nullptr
        )
      ) {
        this->slideTexInputCommands.addOnTop(
          StringRoutines::stringTrimWhiteSpace(buffer)
        );
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
        leftBracketsMinusRight --;
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
      this->lectureNumbers.addOnTop(
        lectureNumber.substr(1, lectureNumber.size() - 1)
      );
    } else {
      this->displayResult
      << "Failed to extract lecture/homework number from line: "
      << buffer;
      return;
    }
    std::getline(inputFile, buffer);
    std::string desiredName;
    if (
      !StringRoutines::stringBeginsWith(
        buffer, "%DesiredLectureName: ", &desiredName
      )
    ) {
      if (
        !StringRoutines::stringBeginsWith(
          buffer, "%DesiredHomeworkName: ", &desiredName
        )
      ) {
        desiredName = "";
      }
    }
    for (unsigned i = 0; i < desiredName.size(); i ++) {
      if (
        desiredName[i] == '.' ||
        desiredName[i] == '/' ||
        desiredName[i] == '\\'
      ) {
        desiredName = "";
      }
    }
    if (desiredName == "") {
      this->displayResult
      << "Failed to extract desired homework/lecture name from: "
      << buffer
      << "<br>This is the line immediately after the \\lect command. "
      << "It should begin with the string \"%DesiredLectureName: \""
      << "(<-has space bar in the end). "
      << "The name itself should not contain the characters . / or \\. "
      << "I am assigning an automatic file name. <br>";
    }
    lectureDesiredNames.addOnTop(desiredName);
  }
  reportStream
  << " done. Extracted: "
  << lectureNumbers.size
  << " homework/lecture numbers. Preparing Homework/Lecture content ... ";
  report.report(reportStream.str());
  if (isLecture && isHomework) {
    this->displayResult
    << "I was not able to determine whether the "
    << "file is a homework or a lecture file. Aborting.";
    return;
  }
  if (!isLecture && !isHomework) {
    this->displayResult
    << "Could not find any lecture or homework entries. Aborting.";
    return;
  }
  std::stringstream LectureContentNoDocumentClassNoCurrentLecture;
  std::stringstream LectureHeaderNoDocumentClass;
  inputFile.clear();
  inputFile.seekg(0);
  bool foundFirstLecture = false;
  if (isLecture) {
    while (!inputFile.eof()) {
      std::getline(inputFile, buffer);
      if (
        !StringRoutines::stringBeginsWith(
          StringRoutines::stringTrimWhiteSpace(buffer), "\\documentclass"
        ) &&
        !StringRoutines::stringBeginsWith(
          StringRoutines::stringTrimWhiteSpace(buffer), "[handout]"
        ) &&
        !StringRoutines::stringBeginsWith(
          StringRoutines::stringTrimWhiteSpace(buffer), "{beamer}"
        ) &&
        !StringRoutines::stringBeginsWith(
          StringRoutines::stringTrimWhiteSpace(buffer),
          "\\newcommand{\\currentLecture}"
        )
      ) {
        LectureContentNoDocumentClassNoCurrentLecture << buffer << "\n";
      }
      if (this->flagBuildSingleSlides) {
        if (
          StringRoutines::stringBeginsWith(
            StringRoutines::stringTrimWhiteSpace(buffer), "\\lect"
          ) &&
          !StringRoutines::stringBeginsWith(
            StringRoutines::stringTrimWhiteSpace(buffer), "\\lecture"
          )
        ) {
          foundFirstLecture = true;
        }
        if (!foundFirstLecture) {
          if (
            !StringRoutines::stringBeginsWith(
              StringRoutines::stringTrimWhiteSpace(buffer),
              "\\documentclass"
            ) &&
            !StringRoutines::stringBeginsWith(
              StringRoutines::stringTrimWhiteSpace(buffer), "[handout]"
            ) &&
            !StringRoutines::stringBeginsWith(
              StringRoutines::stringTrimWhiteSpace(buffer), "{beamer}"
            )
          ) {
            LectureHeaderNoDocumentClass << buffer << "\n";
          }
        }
      }
    }
  } else {
    while (!inputFile.eof()) {
      std::getline(inputFile, buffer);
      if (
        !StringRoutines::stringBeginsWith(buffer, "\\newcommand{\\currentHW}")
      ) {
        LectureContentNoDocumentClassNoCurrentLecture << buffer << "\n";
      }
    }
  }
  reportStream << " done. Proceding to compile homeworks/lectures. ";
  report.report(reportStream.str());
  if (isLecture) {
    this->displayResult
    << "<table><tr><td>Lecture number</td>"
    << "<td>Lecture name</td><td>Lecture pdf</td>"
    << "<td>Lecture handout pdf</td><td>Comments</td></tr>";
  } else {
    this->displayResult
    << "<table><tr><td>Homework number</td>"
    << "<td>Homework name</td><td>Homework pdf</td>"
    << "<td>Homework handout pdf</td><td>Comments</td></tr>";
  }
  std::string lectureFileNameEnd;
  if (
    !StringRoutines::stringBeginsWith(
      this->fileNameToCrawlPhysicalNoPathName,
      "Lecture_",
      &lectureFileNameEnd
    )
  ) {
    if (
      !StringRoutines::stringBeginsWith(
        this->fileNameToCrawlPhysicalNoPathName,
        "Homework_",
        &lectureFileNameEnd
      )
    ) {
      lectureFileNameEnd = "";
    }
  }
  std::string folderEnd =
  this->fileNameToCrawlPhysicalNoPathName.substr(
    0, this->fileNameToCrawlPhysicalNoPathName.size() - 4
  );
  if (lectureFileNameEnd.size() > 4) {
    lectureFileNameEnd =
    lectureFileNameEnd.substr(0, lectureFileNameEnd.size() - 4);
  }
  std::stringstream executedCommands, resultTable;
  std::string currentSysCommand;
  // "ch " + this->baseFolderStartFilePhysical+"\n\n\n\n";
  global.changeDirectory(this->baseFolderStartFilePhysical);
  executedCommands
  << "<br>Directory changed to: "
  << this->baseFolderStartFilePhysical
  << ", making the current directory: "
  << FileOperations::getCurrentFolder();
  reportStream
  << "<br>Directory changed to: "
  << this->baseFolderStartFilePhysical
  << ", making the current directory: "
  << FileOperations::getCurrentFolder();
  std::string lectureProjectorFolder = "lectures_projector_" + folderEnd + "/";
  std::string printableFolder;
  std::string slideProjectorFolder, slideHandoutFolder;
  if (!isLecture) {
    printableFolder = "homework_" + folderEnd + "/";
  } else {
    printableFolder = "lectures_printable_" + folderEnd + "/";
  }
  global.externalCommandNoOutput("mkdir " + printableFolder, true);
  if (isLecture) {
    global.externalCommandNoOutput("mkdir " + lectureProjectorFolder, true);
    if (this->flagBuildSingleSlides) {
      slideProjectorFolder = "slides_projector_" + folderEnd + "/";
      slideHandoutFolder = "slides_handout_" + folderEnd + "/";
      global.externalCommandNoOutput("mkdir " + slideProjectorFolder, true);
      global.externalCommandNoOutput("mkdir " + slideHandoutFolder, true);
    }
  }
  this->fileNameWorkingCopy =
  "working_file_" + this->fileNameToCrawlPhysicalNoPathName;
  std::string fileNameWorkingCopyPDF =
  "working_file_" +
  this->fileNameToCrawlPhysicalNoPathName.substr(
    0, this->fileNameToCrawlPhysicalNoPathName.size() - 3
  ) +
  "pdf";
  int numLecturesToProcess = this->lectureNumbers.size;
  std::fstream workingFile;
  for (int i = 0; i < numLecturesToProcess; i ++) {
    reportStream
    << "<br>Processing lecture "
    << i + 1
    << " out of "
    << this->lectureNumbers.size
    << ". ";
    resultTable << "<tr>";
    resultTable << "<td>" << this->lectureNumbers[i] << "</td>";
    resultTable << "<td>" << this->lectureDesiredNames[i] << "</td>";
    if (
      !FileOperations::openFileCreateIfNotPresentUnsecure(
        workingFile, this->fileNameWorkingCopy, false, true, false
      )
    ) {
      resultTable
      << "<td>-</td><td>-</td><td>Failed to open working file: "
      << this->fileNameWorkingCopy
      << ", aborting. </td> </tr>";
      break;
    }
    if (isLecture) {
      workingFile
      << "\\documentclass[handout]{beamer}\n\\newcommand{\\currentLecture}{"
      << this->lectureNumbers[i]
      << "}\n"
      << LectureContentNoDocumentClassNoCurrentLecture.str();
    } else {
      workingFile
      << "\\newcommand{\\currentHW}{"
      << this->lectureNumbers[i]
      << "}\n"
      << LectureContentNoDocumentClassNoCurrentLecture.str();
    }
    workingFile.close();
    currentSysCommand = "pdflatex --shell-escape " + this->fileNameWorkingCopy;
    executedCommands << "<br>" << currentSysCommand;
    reportStream << currentSysCommand;
    report.report(reportStream.str());
    global.externalCommandNoOutput(currentSysCommand, true);
    reportStream << "<b>[x2]</b>";
    report.report(reportStream.str());
    global.externalCommandNoOutput(currentSysCommand, true);
    std::stringstream pdfFileNameHandout;
    if (isLecture) {
      pdfFileNameHandout
      << "./"
      << printableFolder
      << "Lecture"
      << this->lectureNumbers[i]
      << "Handout_"
      << this->lectureDesiredNames[i];
      if (lectureFileNameEnd != "") {
        pdfFileNameHandout << "_" << lectureFileNameEnd;
      }
      pdfFileNameHandout << ".pdf";
    } else {
      pdfFileNameHandout
      << "./"
      << printableFolder
      << "Homework"
      << lectureNumbers[i]
      << "_"
      << lectureDesiredNames[i];
      if (lectureFileNameEnd != "") {
        pdfFileNameHandout << "_" << lectureFileNameEnd;
      }
      pdfFileNameHandout << ".pdf";
    }
    currentSysCommand =
    "mv " + fileNameWorkingCopyPDF + " " + pdfFileNameHandout.str();
    executedCommands << "<br>" << currentSysCommand;
    reportStream
    << "<br>Lecture/Homework "
    << i + 1
    << " handout compiled, renaming file ... ";
    report.report(reportStream.str());
    global.externalCommandNoOutput(currentSysCommand, true);
    reportStream << " done.";
    report.report(reportStream.str());
    if (!isLecture) {
      report.report(reportStream.str());
      resultTable << "<td>" << pdfFileNameHandout.str() << "</td>";
      resultTable << "</tr>";
      continue;
    }
    if (
      !FileOperations::openFileUnsecure(
        workingFile, this->fileNameWorkingCopy, false, true, false
      )
    ) {
      resultTable
      << "<td>-</td><td>-</td><td>Failed to open working file: "
      << this->fileNameWorkingCopy
      << ", aborting. </td> </tr>";
      break;
    }
    workingFile << "\\documentclass";
    workingFile
    << "{beamer}\n\\newcommand{\\currentLecture}{"
    << lectureNumbers[i]
    << "}\n";
    workingFile << LectureContentNoDocumentClassNoCurrentLecture.str();
    workingFile.close();
    currentSysCommand = "pdflatex --shell-escape " + this->fileNameWorkingCopy;
    executedCommands << "<br>" << currentSysCommand;
    reportStream << currentSysCommand;
    report.report(reportStream.str());
    global.externalCommandNoOutput(currentSysCommand, true);
    std::stringstream pdfFileNameNormal;
    pdfFileNameNormal
    << "./"
    << lectureProjectorFolder
    << "Lecture"
    << lectureNumbers[i]
    << "_"
    << lectureDesiredNames[i]
    << "_"
    << lectureFileNameEnd
    << ".pdf";
    currentSysCommand =
    "mv " + fileNameWorkingCopyPDF + " " + pdfFileNameNormal.str();
    executedCommands << "<br>" << currentSysCommand;
    reportStream
    << "<br>Lecture "
    << i + 1
    << " regular slides compiled, renaming file ... ";
    report.report(reportStream.str());
    global.externalCommandNoOutput(currentSysCommand, true);
    reportStream << " done.";
    report.report(reportStream.str());
    resultTable
    << "<td>"
    << pdfFileNameNormal.str()
    << "</td>"
    << "<td>"
    << pdfFileNameHandout.str()
    << "</td>";
    resultTable << "</tr>";
  }
  int numSlidesToBuild = this->slideTexInputCommands.size;
  this->slideFileNamesVirtualNoPathNoExtension.setSize(
    this->slideTexInputCommands.size
  );
  for (int i = 0; i < this->slideTexInputCommands.size; i ++) {
    this->slideFileNamesVirtualNoPathNoExtension[i] = "";
    if (this->slideTexInputCommands[i] == "") {
      continue;
    }
    std::string currentName =
    this->slideTexInputCommands[i].substr(
      0, this->slideTexInputCommands[i].size() - 1
    );
    this->slideFileNamesVirtualNoPathNoExtension[i] =
    FileOperations::getFileNameFromFileNameWithPath(currentName);
  }
  // executedCommands << "<br>Slides extracted: " <<
  // this->slides.toStringCommaDelimited();
  // executedCommands << "<br>Slides names: " <<
  // this->slideNames.toStringCommaDelimited();
  for (int i = 0; i < numSlidesToBuild; i ++) {
    for (int k = 0; k < 2; k ++) {
      if (
        !FileOperations::openFileUnsecure(
          workingFile, this->fileNameWorkingCopy, false, true, false
        )
      ) {
        resultTable
        << "<td>-</td><td>-</td><td>Failed to open working file: "
        << this->fileNameWorkingCopy
        << ", aborting. </td> </tr>";
        break;
      }
      workingFile << "\\documentclass";
      if (k == 1) {
        workingFile << "[handout]";
      }
      workingFile << "{beamer}";
      workingFile
      << LectureHeaderNoDocumentClass.str()
      << "\n"
      << this->slideTexInputCommands[i]
      << "\n"
      << "\\end{document}";
      workingFile.close();
      currentSysCommand =
      "pdflatex --shell-escape " + this->fileNameWorkingCopy;
      executedCommands << "<br>" << currentSysCommand;
      reportStream << currentSysCommand;
      report.report(reportStream.str());
      global.externalCommandNoOutput(currentSysCommand, true);
      std::stringstream pdfFileNameNormal;
      pdfFileNameNormal << "./";
      if (k == 0) {
        pdfFileNameNormal << slideProjectorFolder;
      } else {
        pdfFileNameNormal << slideHandoutFolder;
      }
      pdfFileNameNormal
      << "Slide_"
      << this->slideFileNamesVirtualNoPathNoExtension[i]
      << ".pdf";
      currentSysCommand =
      "mv " + fileNameWorkingCopyPDF + " " + pdfFileNameNormal.str();
      executedCommands << "<br>" << currentSysCommand;
      reportStream
      << "<br>Slide "
      << i + 1
      << ", run "
      << k
      << " compiled, renaming file ... ";
      report.report(reportStream.str());
      global.externalCommandReturnOutput(currentSysCommand);
      reportStream << " done.";
      report.report(reportStream.str());
      resultTable << "<td>" << pdfFileNameNormal.str() << "</td>";
      resultTable << "</tr>";
    }
  }
  resultTable << "</table>";
  this->displayResult << executedCommands.str() << "<br>" << resultTable.str();
}

void LaTeXCrawler::crawl() {
  STACK_TRACE("LaTeXcrawler::crawl");
  if (
    !this->extractFileNamesFromRelativeFileName(&this->displayResult)
  ) {
    return;
  }
  this->recursionDepth = 0;
  std::stringstream crawlingResult;
  std::string startingDirectory = FileOperations::getCurrentFolder();
  global.changeDirectory(this->baseFolderStartFilePhysical);
  this->crawlRecursive(
    crawlingResult,
    this->baseFolderStartFilePhysical + this->fileNameToCrawlPhysicalNoPathName
  );
  global.changeDirectory(startingDirectory);
  std::fstream outputFile;
  std::string outputFileName = "latexOutput.tex";
  if (
    !FileOperations::openFileCreateIfNotPresentVirtual(
      outputFile, "output/" + outputFileName, false, true, false, true
    )
  ) {
    this->displayResult
    << "Failed to open output file: "
    << outputFileName
    << ", check write permissions. ";
    // this->displayResult << "The crawling appears to have been successful,
    // below is the crawled file.<hr>"
    // << this->crawlingResult.str();
    return;
  }
  outputFile << crawlingResult.str();
  if (this->errorStream.str() != "") {
    this->displayResult << "Errors encountered. " << this->errorStream.str();
  }
  this->displayResult
  << "Output file: <a href=\""
  << global.displayPathOutputFolder
  << "latexOutput.tex\">"
  << "latexOutput.tex"
  << "</a>";
}

LaTeXCrawler::LaTeXCrawler() {
  this->flagBuildSingleSlides = false;
  this->flagProjectorMode = true;
  this->flagForceSlideRebuild = false;
  this->flagAddSlideToSVN = false;
  this->flagDoChangeDirs = true;
  this->flagCrawlTexSourcesRecursively = false;
  this->flagAnswerKey = false;
  this->flagHomeworkRatherThanSlides = false;
  this->flagSourceOnly = false;
  this->ownerCalculator = nullptr;
  this->recursionDepth = 0;
  this->flagPDFExists = false;
}

void LaTeXCrawler::crawlRecursive(
  std::stringstream& crawlingResult, const std::string& currentFileNamE
) {
  STACK_TRACE("LaTeXcrawler::crawlRecursive");
  RecursionDepthCounter counter(&this->recursionDepth);
  if (this->recursionDepth > 1000) {
    this->errorStream
    << "While crawling fileToCrawl, reached max recursion depth of 1000";
    return;
  }
  if (this->errorStream.str() != "") {
    return;
  }
  // /////////
  // / ALERT
  // / potential security risk.
  // / Not sure if the code below is secure enough.
  // / If the folders read are not sanitized properly,
  // / an attacker could get a .tex file with our site's private keys!
  // / Touch very carefully.
  if (this->baseFoldersCrawlableFilesPhysical.size == 0) {
    global.fatal
    << "Error: this->baseFoldersCrawlableFilesPhysical "
    << "is empty which is not allowed here. "
    << global.fatal;
  }
  std::string trimmedFileName =
  StringRoutines::stringTrimWhiteSpace(currentFileNamE);
  std::string trimmedFolder =
  FileOperations::getPathFromFileNameWithPath(trimmedFileName);
  std::string resultingFolder =
  FileOperations::getWouldBeFolderAfterHypotheticalChdirNonThreadSafe(
    trimmedFolder
  );
  if (!this->isInCrawlableFolder(resultingFolder, &this->errorStream)) {
    this->errorStream
    << "Error: file "
    << trimmedFileName
    << " appears to be located in "
    << resultingFolder
    << ", which in turn does not appear to be a sub-folder "
    << "of the designated crawlable folders: "
    << this->baseFoldersCrawlableFilesPhysical.toStringCommaDelimited();
    crawlingResult
    << "%Error: file "
    << trimmedFileName
    << " appears to be located in "
    << resultingFolder
    << ", which in turn does not appear to be a sub-folder "
    << "of the designated crawlable folders: "
    << this->baseFoldersCrawlableFilesPhysical.toStringCommaDelimited()
    << "\n";
    return;
  }
  std::fstream fileStream;
  if (
    !FileOperations::openFileUnsecure(
      fileStream, trimmedFileName, false, false, false
    )
  ) {
    this->errorStream
    << "Failed to open file "
    << trimmedFileName
    << "; current folder: "
    << FileOperations::getCurrentFolder()
    << ". Aborting.<br>";
    crawlingResult << "%Failed to open file: " << trimmedFileName << "\n";
    return;
  }
  std::string buffer;
  while (!fileStream.eof()) {
    std::getline(fileStream, buffer);
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
    unsigned foundInput = static_cast<unsigned>(buffer.find("\\input"));
    if (isGood) {
      if (foundInput < buffer.size()) {
        crawlingResult << buffer.substr(0, foundInput);
        buffer = buffer.substr(foundInput);
        unsigned i = 0;
        newFileName = "";
        for (i = 7; buffer[i] != '}' && i < buffer.size(); i ++) {
          newFileName += buffer[i];
        }
        newFileName = StringRoutines::stringTrimWhiteSpace(newFileName);
        std::string newFileNameEnd;
        if (
          StringRoutines::stringBeginsWith(
            newFileName, "\\freecalcBaseFolder", &newFileNameEnd
          )
        ) {
          newFileName = "../.." + newFileNameEnd;
        }
        newFileName += ".tex";
        crawlingResult << "%input from file: " << newFileName << "\n";
        this->crawlRecursive(crawlingResult, newFileName);
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

bool LaTeXCrawler::extractFileNames(
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  STACK_TRACE("LaTeXcrawler::ExtractPresentationFileNames");
  (void) commentsGeneral;
  if (this->slideFileNamesVirtualWithPath.size < 1) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Could not find slide/homework file names. ";
    }
    return false;
  }
  this->slideFileNamesWithLatexPathNoExtension.initializeFillInObject(
    this->slideFileNamesVirtualWithPath.size, ""
  );
  this->latexSnippets.initializeFillInObject(
    this->slideFileNamesVirtualWithPath.size, ""
  );
  for (int i = 0; i < this->slideFileNamesVirtualWithPath.size; i ++) {
    if (
      StringRoutines::stringBeginsWith(
        this->slideFileNamesVirtualWithPath[i].fileName,
        "LaTeX: ",
        &this->latexSnippets[i]
      )
    ) {
      if (i == 0) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << "Found LaTeX snippet without a header file. "
          << this->slideFileNamesVirtualWithPath[i].fileName
          << "<br>";
        }
        return false;
      }
      continue;
    }
    if (
      !StringRoutines::stringEndsWith(
        this->slideFileNamesVirtualWithPath[i].fileName, ".tex"
      )
    ) {
      this->slideFileNamesVirtualWithPath[i].fileName += ".tex";
    }
    if (
      !FileOperations::isFileNameSafeForSystemCommands(
        this->slideFileNamesVirtualWithPath[i].fileName,
        commentsOnFailure
      )
    ) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Found unsafe slide name: "
        << this->slideFileNamesVirtualWithPath[i].fileName
        << "<br>";
      }
      return false;
    }
    if (
      !FileOperations::isOKFileNameVirtual(
        this->slideFileNamesVirtualWithPath[i].fileName,
        false,
        commentsOnFailure
      )
    ) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Found invalid slide name: "
        << this->slideFileNamesVirtualWithPath[i].fileName
        << "<br>";
      }
      return false;
    }
    if (
      global.userDefaultHasAdminRights() &&
      !global.flagDisableDatabaseLogEveryoneAsAdmin
    ) {
      if (
        !FileOperations::fileExistsVirtual(
          this->slideFileNamesVirtualWithPath[i].fileName,
          false,
          false
        )
      ) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << "Failed to find file: "
          << this->slideFileNamesVirtualWithPath[i].fileName
          << ". ";
        }
        return false;
      }
    }
    FileOperations::getFileExtensionWithDot(
      this->slideFileNamesVirtualWithPath[i].fileName,
      &this->slideFileNamesWithLatexPathNoExtension[i]
    );
    if (
      StringRoutines::stringBeginsWith(
        this->slideFileNamesWithLatexPathNoExtension[i],
        "freecalc",
        nullptr
      )
    ) {
      this->slideFileNamesWithLatexPathNoExtension[i] =
      "../../" + this->slideFileNamesWithLatexPathNoExtension[i];
    }
    if (
      StringRoutines::stringBeginsWith(
        this->slideFileNamesWithLatexPathNoExtension[i],
        "LaTeX-materials",
        nullptr
      )
    ) {
      this->slideFileNamesWithLatexPathNoExtension[i] =
      "../../" + this->slideFileNamesWithLatexPathNoExtension[i];
    }
  }
  this->headerFileNameWithPathVirtual =
  this->slideFileNamesVirtualWithPath[0].fileName;
  this->headerFilePathVirtual =
  FileOperations::getPathFromFileNameWithPath(
    this->headerFileNameWithPathVirtual
  );
  this->headerFileNameNoPath =
  FileOperations::getFileNameFromFileNameWithPath(
    this->headerFileNameWithPathVirtual
  );
  this->headerPathBelowFileNameVirtual = this->headerFilePathVirtual;
  if (this->headerPathBelowFileNameVirtual.size() > 0) {
    this->headerPathBelowFileNameVirtual =
    this->headerPathBelowFileNameVirtual.substr(
      0, this->headerPathBelowFileNameVirtual.size() - 1
    );
    this->headerPathBelowFileNameVirtual =
    FileOperations::getFileNameFromFileNameWithPath(
      this->headerPathBelowFileNameVirtual
    );
  }
  if (
    !FileOperations::getPhysicalFileNameFromVirtual(
      this->headerFilePathVirtual,
      this->workingFilePathPhysical,
      false,
      false,
      commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to extract physical path from: "
      << this->headerFilePathVirtual;
    }
    return false;
  }
  if (commentsGeneral != nullptr) {
    *commentsGeneral
    << "Working file path physical: "
    << this->workingFilePathPhysical
    << " extracted from virtual path: "
    << this->headerFilePathVirtual
    << "\n";
  }
  FileOperations::getFileExtensionWithDot(
    this->headerFileNameNoPath, &this->headerFileNameNoPathNoExtension
  );
  this->workingFileNameNoPathTex =
  "workingfile" + this->headerFileNameNoPathNoExtension + ".tex";
  this->workingFileNameNoPathPDF =
  "workingfile" + this->headerFileNameNoPathNoExtension + ".pdf";
  std::string firstSignificantSlideName = "";
  if (this->slideFileNamesVirtualWithPath.size >= 1) {
    firstSignificantSlideName = this->slideFileNamesVirtualWithPath[0].fileName
    ;
  }
  for (int i = 1; i < this->slideFileNamesVirtualWithPath.size; i ++) {
    if (this->latexSnippets[i] == "") {
      firstSignificantSlideName =
      this->slideFileNamesVirtualWithPath[i].fileName;
      break;
    }
  }
  this->targetPDFVirtualPath =
  FileOperations::getPathFromFileNameWithPath(firstSignificantSlideName);
  std::string tempString;
  if (
    StringRoutines::stringBeginsWith(
      this->targetPDFVirtualPath, "freecalc", &tempString
    )
  ) {
    this->targetPDFVirtualPath = "slides-video" + tempString;
  }
  if (
    StringRoutines::stringBeginsWith(
      this->targetPDFVirtualPath, "LaTeX-materials", &tempString
    )
  ) {
    this->targetPDFVirtualPath = "slides-video" + tempString;
  }
  this->targetPDFNoPath =
  FileOperations::getFileNameFromFileNameWithPath(firstSignificantSlideName);
  FileOperations::getFileExtensionWithDot(
    this->targetPDFNoPath, &this->targetPDFNoPath
  );
  if (!this->flagHomeworkRatherThanSlides) {
    if (this->flagProjectorMode) {
      this->targetPDFNoPath +=
      "-projector-" + this->headerPathBelowFileNameVirtual;
    } else {
      this->targetPDFNoPath +=
      "-printable-" + this->headerPathBelowFileNameVirtual;
    }
  } else {
    if (this->flagAnswerKey) {
      this->targetPDFNoPath +=
      "-answer-key-" + this->headerPathBelowFileNameVirtual;
    } else {
      this->targetPDFNoPath +=
      "-no-answers-" + this->headerPathBelowFileNameVirtual;
    }
  }
  this->targetPDFNoPath += this->desiredPresentationTitle;
  this->targetPDFNoPath =
  HtmlRoutines::convertStringToURLString(this->targetPDFNoPath, false);
  StringRoutines::stringTrimToLengthWithHash(this->targetPDFNoPath, 230);
  this->targetPDFNoPath += ".pdf";
  this->targetPDFFileNameWithPathVirtual =
  this->targetPDFVirtualPath + this->targetPDFNoPath;
  this->targetPDFLatexPath = "../../" + this->targetPDFVirtualPath;
  if (
    !StringRoutines::stringBeginsWith(
      this->targetPDFVirtualPath, "slides-video/modules/", &tempString
    )
  ) {
    this->targetVideoLatexPath = "";
  } else {
    this->targetVideoLatexPath =
    "../../slides-video/modules-video/" + tempString;
  }
  this->targetPDFFileNameWithLatexPath =
  this->targetPDFLatexPath + this->targetPDFNoPath;
  return true;
}

std::string LaTeXCrawler::adjustDisplayTitle(
  const std::string& input, bool isHomework
) {
  STACK_TRACE("LaTeXcrawler::adjustDisplayTitle");
  std::string result = input;
  List<std::string> ignoredTags;
  ignoredTags.addOnTop("lectureTag");
  ignoredTags.addOnTop("advancedTopic");
  ignoredTags.addOnTop("reviewProblem");
  for (int i = 0; i < ignoredTags.size; i ++) {
    std::string closeTag = "</" + ignoredTags[i] + ">";
    std::string openTag = "<" + ignoredTags[i] + ">";
    if (
      input.find(openTag) != std::string::npos &&
      input.find(closeTag) != std::string::npos
    ) {
      unsigned int start = static_cast<unsigned>(input.find(openTag));
      unsigned int finish = static_cast<unsigned>(
        input.find(closeTag) + closeTag.size()
      );
      result = result.substr(0, start) + result.substr(finish);
    }
  }
  if (result.find("\\(\\LaTeX\\)") != std::string::npos) {
    unsigned pos = static_cast<unsigned>(result.find("\\(\\LaTeX\\)"));
    result =
    result.substr(0, pos) +
    "\\LaTeX" +
    result.substr(pos + (std::string("\\(\\LaTeX\\)")).size());
  }
  result = StringRoutines::stringTrimWhiteSpace(result);
  if (
    isHomework &&
    result.size() > 4 &&
    !StringRoutines::stringBeginsWith(result, "\\\\")
  ) {
    result = "\\\\" + result;
  }
  return result;
}

bool LaTeXCrawler::extractFileNamesPdfExists(
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  this->desiredPresentationTitle =
  this->adjustDisplayTitle(
    this->desiredPresentationTitle, this->flagHomeworkRatherThanSlides
  );
  if (!this->extractFileNames(commentsOnFailure, commentsGeneral)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "While building pdf, failed to extract file names. ";
    }
    return false;
  }
  this->flagPDFExists =
  FileOperations::fileExistsVirtual(
    this->targetPDFFileNameWithPathVirtual,
    false,
    false,
    commentsOnFailure
  );
  return true;
}

bool LaTeXCrawler::buildOrFetchFromCachePDF(
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  STACK_TRACE("LaTeXcrawler::buildOrFetchFromCachePDF");
  if (
    !this->extractFileNamesPdfExists(commentsOnFailure, commentsGeneral)
  ) {
    return false;
  }
  if (
    !this->flagForceSlideRebuild &&
    this->flagPDFExists &&
    !this->flagSourceOnly
  ) {
    bool result =
    FileOperations::loadFileToStringVirtual(
      this->targetPDFFileNameWithPathVirtual,
      this->targetPDFbinaryContent,
      false,
      commentsOnFailure
    );
    return result;
  }
  if (
    !global.userDefaultHasAdminRights() ||
    global.flagDisableDatabaseLogEveryoneAsAdmin
  ) {
    if (!this->flagPDFExists || !this->flagSourceOnly) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Pdf of slides not created. "
        << "Only logged-in admins can compile pdfs. "
        << "Computed file name: "
        << HtmlRoutines::convertStringToHtmlString(
          this->targetPDFFileNameWithPathVirtual, false
        );
      }
      return false;
    }
  }
  std::stringstream tempStream;
  if (commentsOnFailure == nullptr) {
    commentsOnFailure = &tempStream;
  }
  std::stringstream crawlingResult;
  std::fstream fileStream;
  if (!this->flagCrawlTexSourcesRecursively) {
    if (
      !FileOperations::openFileVirtual(
        fileStream,
        this->headerFileNameWithPathVirtual,
        false,
        false,
        false
      )
    ) {
      return false;
    }
    std::string buffer;
    do {
      std::getline(fileStream, buffer);
      if (
        !StringRoutines::stringBeginsWith(
          StringRoutines::stringTrimWhiteSpace(buffer), "[handout]"
        )
      ) {
        crawlingResult << buffer << "\n";
      }
      if (!this->flagProjectorMode && !this->flagHomeworkRatherThanSlides) {
        if (
          StringRoutines::stringBeginsWith(
            StringRoutines::stringTrimWhiteSpace(buffer),
            "\\documentclass"
          )
        ) {
          crawlingResult << "[handout]" << "\n";
        }
      }
    } while (!fileStream.eof());
    fileStream.close();
  }
  bool addExtraTex = (this->slideFileNamesVirtualWithPath.size > 1);
  if (this->flagDoChangeDirs) {
    global.changeDirectory(this->workingFilePathPhysical);
    if (commentsGeneral != nullptr) {
      *commentsGeneral
      << "Changed directory. Current: "
      << "<b style = 'color:blue'>"
      << FileOperations::getCurrentFolder()
      << "</b><br>";
    }
  }
  if (this->flagCrawlTexSourcesRecursively) {
    crawlingResult
    << "%file automatically generated from file: "
    << this->headerFileNameNoPath
    << "\n%This file compiles with pdflatex --shell-escape\n";
    if (!this->flagHomeworkRatherThanSlides) {
      crawlingResult
      << "\n%Comment out/in the [handout] "
      << "line to get the slide in projector/handout mode.\n";
    } else {
      crawlingResult
      << "\n%Use \\togglefalse{answers} and "
      << "\\togglefalse{solutions} to turn off the answer key/solutions.\n";
    }
    this->computeAllowedFolders();
    this->crawlRecursive(crawlingResult, this->headerFileNameNoPath);
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
      crawlingResult
      << "\\lect{\\semester}{"
      << this->desiredPresentationTitle
      << "}{1}{\n";
    } else {
      crawlingResult
      << "\\homeworkOnATopic{}{"
      << this->desiredPresentationTitle
      << "}{1}{\n";
    }
    for (int i = 1; i < this->slideFileNamesVirtualWithPath.size; i ++) {
      if (this->latexSnippets[i] == "") {
        if (this->flagHomeworkRatherThanSlides) {
          if (!this->slideFileNamesVirtualWithPath[i].isSolution) {
            crawlingResult << "\\item\n";
          }
        }
        if (this->flagCrawlTexSourcesRecursively) {
          crawlingResult
          << "%input from file: "
          << this->slideFileNamesWithLatexPathNoExtension[i]
          << ".tex\n";
          this->crawlRecursive(
            crawlingResult,
            this->slideFileNamesWithLatexPathNoExtension[i] + ".tex"
          );
        } else {
          crawlingResult
          << "\\input{"
          << this->slideFileNamesWithLatexPathNoExtension[i]
          << "}\n";
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
  if (
    FileOperations::openFileCreateIfNotPresentUnsecure(
      fileStream, this->workingFileNameNoPathTex, false, true, false
    )
  ) {
    fileStream << crawlingResult.str();
    if (commentsGeneral != nullptr) {
      *commentsGeneral
      << "Stored working file: "
      << this->workingFileNameNoPathTex
      << "<br>";
    }
  } else if (commentsGeneral != nullptr) {
    *commentsGeneral
    << "FAILED to store file: "
    << this->workingFileNameNoPathTex
    << "<br>";
  }
  fileStream.close();
  std::string currentSysCommand =
  "pdflatex --shell-escape " + this->workingFileNameNoPathTex;
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "Executing command: " << currentSysCommand << " ... ";
  }
  global.externalCommandNoOutput(currentSysCommand, true);
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "done!<br>";
  }
  currentSysCommand = "mkdir -p " + this->targetPDFLatexPath;
  std::string commandResult =
  global.externalCommandReturnOutput(currentSysCommand);
  if (commentsGeneral != nullptr) {
    *commentsGeneral
    << "Executed command: "
    << currentSysCommand
    << " ... to get result: "
    << commandResult
    << "<br>";
  }
  if (this->targetVideoLatexPath != "") {
    currentSysCommand = "mkdir -p " + this->targetVideoLatexPath;
    std::string commandResult =
    global.externalCommandReturnOutput(currentSysCommand);
    if (commentsGeneral != nullptr) {
      *commentsGeneral
      << "Executed command: "
      << currentSysCommand
      << " ... to get result: "
      << commandResult
      << "<br>";
    }
  }
  currentSysCommand =
  "mv " +
  this->workingFileNameNoPathPDF +
  " " +
  this->targetPDFFileNameWithLatexPath;
  commandResult = global.externalCommandReturnOutput(currentSysCommand);
  if (commentsGeneral != nullptr) {
    *commentsGeneral
    << "Executed command: "
    << currentSysCommand
    << " ... to get result: "
    << commandResult
    << "<br>";
  }
  if (
    !FileOperations::loadFileToStringUnsecure(
      this->targetPDFFileNameWithLatexPath,
      this->targetPDFbinaryContent,
      commentsOnFailure
    )
  ) {
    return false;
  }
  if (
    !FileOperations::isFileNameSafeForSystemCommands(
      this->targetPDFFileNameWithLatexPath, commentsOnFailure
    )
  ) {
    return true;
  }
  return true;
}

bool LaTeXCrawler::buildTopicList(
  int topicNumber,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  STACK_TRACE("LaTeXCrawler::buildTopicList");
  StateMaintainerCurrentFolder preserveCurrentFolder;
  ProgressReport report;
  CalculatorHTML topicParser;
  std::stringstream temp;
  if (commentsGeneral == nullptr) {
    commentsGeneral = &temp;
  }
  topicParser.loadFileNames();
  if (
    !topicParser.loadAndParseTopicIndex(topicNumber, *commentsOnFailure)
  ) {
    return false;
  }
  int numberOfSlidePairsToBuild = 0;
  for (int i = 0; i < topicParser.topics.topics.size(); i ++) {
    if (topicParser.topics.topics.values[i].sourceSlides.size > 0) {
      numberOfSlidePairsToBuild ++;
    }
  }
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "Loaded topic list: " << topicParser.topicListFileName;
    *commentsGeneral
    << "<br> "
    << numberOfSlidePairsToBuild
    << " slide pairs to build ";
    *commentsGeneral
    << "("
    << topicParser.topics.topics.size()
    << " total topic elements)";
  }
  bool result = true;
  int processedCount = 0;
  this->slideFileNamesVirtualWithPath.setSize(
    topicParser.sourcesHomeworkHeaders.size
  );
  for (int i = 0; i < topicParser.sourcesHomeworkHeaders.size; i ++) {
    this->slideFileNamesVirtualWithPath[i].fileName =
    topicParser.sourcesHomeworkHeaders[i];
    this->slideFileNamesVirtualWithPath[i].isSolution = false;
  }
  for (int i = 0; i < topicParser.topics.topics.size(); i ++) {
    TopicElement& currentElement = topicParser.topics.topics.values[i];
    if (currentElement.sourceHomework.size == 0) {
      continue;
    }
    std::stringstream reportStream;
    processedCount ++;
    reportStream
    << "Processing homework pdfs: "
    << processedCount
    << " out of "
    << numberOfSlidePairsToBuild
    << ". ";
    report.report(reportStream.str());
    for (int i = 0; i < currentElement.sourceHomework.size; i ++) {
      LaTeXCrawler::FileWithOption file;
      file.fileName = currentElement.sourceHomework[i];
      file.isSolution = false;
      if (i < currentElement.sourceHomeworkIsSolution.size) {
        if (currentElement.sourceHomeworkIsSolution[i]) {
          file.isSolution = true;
        }
      }
      this->slideFileNamesVirtualWithPath.addOnTop(file);
    }
    this->desiredPresentationTitle = currentElement.title;
    this->flagForceSlideRebuild = true;
    this->flagProjectorMode = false;
    this->flagAddSlideToSVN = false;
    this->flagHomeworkRatherThanSlides = true;
    this->flagAnswerKey = false;
    if (commentsGeneral != nullptr) {
      *commentsGeneral
      << "<br>Build homework pair from: "
      << this->slideFileNamesVirtualWithPath.toStringCommaDelimited()
      << ". ";
    }
    if (
      !this->buildOrFetchFromCachePDF(commentsOnFailure, commentsGeneral)
    ) {
      result = false;
    }
    this->flagDoChangeDirs = false;
    this->flagAddSlideToSVN = true;
    if (
      !this->buildOrFetchFromCachePDF(commentsOnFailure, commentsGeneral)
    ) {
      result = false;
    }
    this->flagAnswerKey = true;
    if (
      !this->buildOrFetchFromCachePDF(commentsOnFailure, commentsGeneral)
    ) {
      result = false;
    }
  }
  this->slideFileNamesVirtualWithPath.setSize(0);
  processedCount = 0;
  this->addSlidesOnTop(topicParser.slidesSourcesHeaders);
  for (int i = 0; i < topicParser.topics.topics.size(); i ++) {
    TopicElement& currentElt = topicParser.topics.topics.values[i];
    if (currentElt.sourceSlides.size == 0) {
      continue;
    }
    std::stringstream reportStream;
    processedCount ++;
    reportStream
    << "Processing lecture slides: "
    << processedCount
    << " out of "
    << numberOfSlidePairsToBuild
    << ". ";
    reportStream
    << "<br>Slide file names: "
    << this->slideFileNamesVirtualWithPath.toStringCommaDelimited();
    report.report(reportStream.str());
    this->slideFileNamesVirtualWithPath.setSize(
      topicParser.slidesSourcesHeaders.size
    );
    this->addSlidesOnTop(currentElt.sourceSlides);
    this->desiredPresentationTitle = currentElt.title;
    this->flagForceSlideRebuild = true;
    this->flagProjectorMode = false;
    this->flagAddSlideToSVN = false;
    this->flagHomeworkRatherThanSlides = false;
    if (commentsGeneral != nullptr) {
      *commentsGeneral
      << "<br>Build slide pair from: "
      << this->slideFileNamesVirtualWithPath.toStringCommaDelimited();
    }
    if (
      !this->buildOrFetchFromCachePDF(commentsOnFailure, commentsGeneral)
    ) {
      result = false;
    }
    this->flagDoChangeDirs = false;
    this->flagAddSlideToSVN = true;
    this->flagProjectorMode = false;
    if (
      !this->buildOrFetchFromCachePDF(commentsOnFailure, commentsGeneral)
    ) {
      result = false;
    }
    this->flagProjectorMode = true;
    if (
      !this->buildOrFetchFromCachePDF(commentsOnFailure, commentsGeneral)
    ) {
      result = false;
      break;
    }
  }
  return result;
}
