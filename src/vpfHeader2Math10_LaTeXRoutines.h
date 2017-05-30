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
  std::string theFileToCrawlContent;
  std::string theFileNameToCrawlPhysical;
  std::string theFileNameToCrawlRelative;
  std::string baseFolderStartFilePhysical;
  std::string theFileNameToCrawlNoPathPhysical;
  std::string theFileNameWorkingCopy;
  std::string theFileNameWorkinGCopyPDF;
  std::string slideHeader;
  std::string slidePDFbinaryBlob;
  std::string targetPDFfileName;
  List<std::string> theLectureNumbers;
  List<std::string> theLectureDesiredNames;
  List<std::string> theSlides;
  List<std::string> theSlideNames;
  void* ownerCalculator;
  bool flagBuildSingleSlides;
  bool ExtractFileNamesFromRelativeFileName(std::stringstream* commentsOnFailure);
  void Crawl();
  bool BuildOrFetchFromCachePresentationFromSlides(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  void BuildFreecalc();
  void CrawlRecursive(const std::string& currentFileName);
  std::stringstream crawlingResult;
  std::stringstream displayResult;
  std::stringstream errorStream;
  std::string ToString();
  LaTeXcrawler()
  { this->flagBuildSingleSlides=false;
    this->ownerCalculator=0;
  }
};

#endif
