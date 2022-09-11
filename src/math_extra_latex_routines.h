
#ifndef header_math_extra_latex_routines_ALREADY_INCLUDED
#define header_math_extra_latex_routines_ALREADY_INCLUDED

#include "general_list_references.h"
#include "json.h"

class LaTeXCrawler {
public:
  struct FileWithOption {
  public:
    friend std::ostream& operator<<(
      std::ostream& output, const FileWithOption& file
    ) {
      output << file.fileName;
      if (file.isSolution) {
        output << "[solution]";
      }
      return output;
    }
    std::string fileName;
    bool isSolution;
    bool fromJSON(JSData& input, std::stringstream* commentsOnFailure);
    JSData toJSON();
    FileWithOption();
    FileWithOption(const std::string& input);
  };

  struct Slides {
  public:
    std::string title;
    List<LaTeXCrawler::FileWithOption> filesToCrawl;
    void addSlidesOnTop(const List<std::string>& input);
    JSData toJSON();
    bool fromJSON(JSData& input, std::stringstream* commentsOnFailure);
    bool fromString(
      const std::string& input, std::stringstream* commentsOnFailure
    );
  };

  int recursionDepth;
  std::string fileNameToCrawlPhysicalNoPathName;
  std::string fileNameToCrawlPhysicalWithPath;
  std::string fileNameToCrawlRelative;
  std::string baseFolderStartFilePhysical;
  List<std::string> baseFoldersCrawlableFilesPhysical;
  std::string fileNameWorkingCopy;
  std::string targetLaTeX;
  std::string targetPDFbinaryContent;
  std::string targetPDFFileNameWithPathVirtual;
  std::string targetPDFFileNameWithLatexPath;
  std::string targetPDFLatexPath;
  std::string targetPDFVirtualPath;
  std::string targetPDFNoPath;
  std::string targetVideoLatexPath;
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
  List<std::string> lectureNumbers;
  List<std::string> lectureDesiredNames;
  List<std::string> slideTexInputCommands;
  List<LaTeXCrawler::FileWithOption> slideFileNamesVirtualWithPath;
  List<std::string> slideFileNamesWithLatexPathNoExtension;
  List<std::string> slideFileNamesVirtualNoPathNoExtension;
  List<std::string> latexSnippets;
  void* ownerCalculator;
  bool flagAddSlideToSVN;
  bool flagBuildSingleSlides;
  bool flagProjectorMode;
  bool flagAnswerKey;
  bool flagHomeworkRatherThanSlides;
  bool flagForceSlideRebuild;
  bool flagSourceOnly;
  bool flagDoChangeDirs;
  bool flagCrawlTexSourcesRecursively;
  bool flagPDFExists;
  void addSlidesOnTop(List<std::string>& inputSlides);
  static std::string adjustDisplayTitle(
    const std::string& input, bool isHomework
  );
  bool isInCrawlableFolder(
    const std::string& folderName, std::stringstream* commentsOnFailure
  );
  void computeAllowedFolders();
  bool extractFileNamesFromRelativeFileName(
    std::stringstream* commentsOnFailure
  );
  bool extractFileNames(
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
  void crawl();
  bool extractFileNamesPdfExists(
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
  bool buildOrFetchFromCachePDF(
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
  bool buildTopicList(
    int topicNumber,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
  void buildFreecalc();
  void crawlRecursive(
    std::stringstream& crawlingResult,
    const std::string& currentFileName
  );
  bool initializeFromGlobalVariables(std::stringstream* commentsOnFailure);
  std::stringstream displayResult;
  std::stringstream errorStream;
  std::string toString();
  LaTeXCrawler();
};

#endif // header_math_extra_latex_routines_ALREADY_INCLUDED
