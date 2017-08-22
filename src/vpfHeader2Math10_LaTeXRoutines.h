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
  std::string theFileNameToCrawlPhysicalNoPathName;
  std::string theFileNameToCrawlPhysicalWithPath;
  std::string theFileNameToCrawlRelative;
  std::string baseFolderStartFilePhysical;
  List<std::string> baseFoldersCrawlableFilesPhysical;

  std::string theFileToCrawlContent;
  std::string theFileNameWorkingCopy;
  std::string theFileNameWorkinGCopyPDF;

  std::string targetLaTeX;
  std::string targetPDFbinaryContent;
  std::string targetPDFFileNameWithPathVirtual;
  std::string targetPDFFileNameWithLatexPath;
  std::string targetPDFLatexPath;
  std::string targetPDFVirtualPath;
  std::string targetPDFNoPath;

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
  List<std::string> latexSnippets;
  void* ownerCalculator;
  bool flagAddSlideToSVN;
  bool flagBuildSingleSlides;
  bool flagProjectorMode;
  bool flagForceSlideRebuild;
  bool flagDoChangeDirs;
  bool flagCrawlTexSourcesRecursively;
  static std::string AdjustDisplayTitle(const std::string& input);
  bool IsInCrawlableFolder(const std::string& folderName, std::stringstream* commentsOnFailure);
  void ComputeAllowedFolders();
  bool ExtractFileNamesFromRelativeFileName(std::stringstream* commentsOnFailure);
  bool ExtractPresentationFileNames(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  void Crawl();
  bool BuildOrFetchFromCachePresentationFromSlides(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  bool BuildTopicList(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  void BuildFreecalC();
  void CrawlRecursive(std::stringstream& crawlingResult, const std::string& currentFileName);
  std::stringstream displayResult;
  std::stringstream errorStream;
  std::string ToString();
  LaTeXcrawler();
};

#endif
