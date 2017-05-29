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
  std::string theFileToCrawlPhysical;
  std::string theFileToCrawlRelative;
  std::string baseFolderStartFilePhysical;
  std::string theFileToCrawlNoPathPhysical;
  std::string theFileWorkingCopy;
  List<std::string> theLectureNumbers;
  List<std::string> theLectureDesiredNames;
  List<std::string> theSlides;
  List<std::string> theSlideNames;
  void* ownerCalculator;
  bool flagBuildSingleSlides;
  bool ExtractFileNamesFromRelativeFileName();
  void Crawl();
  void BuildFreecalc();
  void CrawlRecursive(const std::string& currentFileName);
  std::stringstream crawlingResult;
  std::stringstream displayResult;
  std::stringstream errorStream;
  std::string ToString();
  LaTeXcrawler()
  { this->flagBuildSingleSlides=false;
  }
};

#endif
