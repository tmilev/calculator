//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1_HeaderLaTeXRoutines
#define vpfHeader1_HeaderLaTeXRoutines
#include "vpfHeader1General1_ListReferences.h"

static ProjectInformationInstance ProjectInfovpfHeader_HeaderLaTeXRoutines(__FILE__, "Header, LaTeX routines. ");

class LaTeXcrawler
{
  public:
  int recursionDepth;
  std::string topicListToBuild;
  std::string theFileNameToCrawlPhysical;
  std::string theFileNameToCrawlRelative;
  std::string baseFolderStartFilePhysical;

  std::string theFileToCrawlContent;
  std::string theFileNameToCrawlNoPathPhysical;
  std::string theFileNameWorkingCopy;
  std::string theFileNameWorkinGCopyPDF;

  std::string targetPDFbinaryContent;
  std::string targetPDFFileNameWithPathVirtual;
  std::string targetPDFFileNameWithLatexPath;
  std::string targetPDFLatexPath;

  std::string headerFileNameWithPathVirtual;
  std::string headerFileNameNoPath;
  std::string headerFileNameNoPathNoExtension;
  std::string headerFilePathVirtual;
  std::string headerPathBelowFileNameVirtual;

  std::string workingFilePathPhysical;
  std::string workingFileNameNoPathTex;
  std::string workingFileNameNoPathPDF;
  std::string workingFileContent;

  std::string desiredPresentationTitle;

  List<std::string> theLectureNumbers;
  List<std::string> theLectureDesiredNames;
  List<std::string> slideTexInputCommands;
  List<std::string> slideFileNamesVirtualWithPatH;
  List<std::string> slideFileNamesWithLatexPathNoExtension;
  List<std::string> slideFileNamesVirtualNoPathNoExtensioN;
  void* ownerCalculator;
  bool flagAddSlideToSVN;
  bool flagBuildSingleSlides;
  bool flagProjectorMode;
  bool flagForceSlideRebuild;
  bool ExtractFileNamesFromRelativeFileName(std::stringstream* commentsOnFailure);
  bool ExtractPresentationFileNames(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  void Crawl();
  bool BuildOrFetchFromCachePresentationFromSlides(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  bool BuildTopicList(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  void BuildFreecalc();
  void CrawlRecursive(const std::string& currentFileName);
  std::stringstream crawlingResult;
  std::stringstream displayResult;
  std::stringstream errorStream;
  std::string ToString();
  LaTeXcrawler()
  { this->flagBuildSingleSlides=false;
    this->flagProjectorMode=true;
    this->flagForceSlideRebuild=false;
    this->flagAddSlideToSVN=false;
    this->ownerCalculator=0;
  }
};

#endif
