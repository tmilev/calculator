//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1General7FileOperations_Encodings_already_included
#define vpfHeader1General7FileOperations_Encodings_already_included

#include "vpfHeader1General0_General.h"
#include "vpfHeader1General1_ListReferences.h"
#include "vpfHeader1General6Maps.h"
static ProjectInformationInstance ProjectInfoVpfHeader1General7FileOperations_EncodingsHeader1instance(__FILE__, "Header, file routines, encodings and more. ");

struct FileOperations
{
public:
  static HashedList<std::string, MathRoutines::hashString>&
  FilesStartsToWhichWeAppendHostName();
  static HashedList<std::string, MathRoutines::hashString>&
  FolderStartsToWhichWeAppendInstructorUsernameSlash();
  static HashedList<std::string, MathRoutines::hashString>&
  FolderVirtualLinksToWhichWeAppendTimeAndBuildHash();

  static MapLisT<std::string, std::string, MathRoutines::hashString>&
  FolderVirtualLinksNonSensitive();
  static MapLisT<std::string, std::string, MathRoutines::hashString>&
  FolderVirtualLinksSensitive(); //<- admin access only
  static MapLisT<std::string, std::string, MathRoutines::hashString>&
  FolderVirtualLinksULTRASensitive(); //<- no access allowed through web server

  static List<bool> safeFileCharacters;
  static List<bool>& GetSafeFileChars();

  static std::string ConvertStringToEscapedStringFileNameSafe(const std::string& input);
  static std::string ConvertStringToLatexFileName(const std::string& input);
  static bool LoadFileToStringUnsecure
  (const std::string& fileNameUnsecure, std::string& output, std::stringstream* commentsOnFailure);
  static bool LoadFileToStringVirtualCustomizedReadOnly(
    const std::string& fileName, std::string& output,
    std::stringstream* commentsOnFailure
  );
  static bool LoadFileToStringVirtual(
    const std::string& fileName, std::string& output,
    bool accessSensitiveFolders, bool accessULTRASensitiveFolders,
    std::stringstream* commentsOnFailure
  );
  static bool IsOKfileNameVirtual(
    const std::string& fileName,
    bool accessSensitiveFolders = false,
    std::stringstream* commentsOnFailure = 0
  );
  static bool IsFileNameWithoutDotsAndSlashes(const std::string& fileName);
  static bool IsFileNameSafeForSystemCommands(const std::string& fileName, std::stringstream* commentsOnFailure);
  static std::string GetFileNameFromFileNameWithPath(const std::string& fileName);
  static std::string GetPathFromFileNameWithPath(const std::string& fileName);
  static std::string GetFileExtensionWithDot(const std::string& theFileName, std::string* outputFileNameNoExtension = 0);
  static bool FileExistsUnsecure(const std::string& theFileName);
  static bool FileExistsVirtual(
    const std::string& theFileName,
    bool accessSensitiveFolders = false,
    bool accessULTRASensitiveFolders = false,
    std::stringstream* commentsOnFailure = 0
  );
  static bool FileExistsVirtualCustomizedReadOnly(const std::string& theFileName, std::stringstream* commentsOnFailure);
  static bool IsFolderUnsecure(const std::string& theFolderName);
  static bool GetFolderFileNamesUnsecure(
    const std::string& theFolderName,
    List<std::string>& outputFileNamesNoPath,
    List<std::string>* outputFileTypes = 0
  );
  static std::string GetCurrentFolder();
  static std::string GetWouldBeFolderAfterHypotheticalChdirNonThreadSafe(const std::string& wouldBePath);
  static bool GetFolderFileNamesVirtual(
    const std::string& theFolderName,
    List<std::string>& outputFileNamesNoPath,
    List<std::string>* outputFileTypes = 0,
    bool accessSensitiveFolders = false,
    bool accessULTRASensitiveFolders = false,
    std::stringstream *commentsOnFailure = 0
  );
  static bool GetPhysicalFileNameFromVirtualCustomizedReadOnly(
    const std::string& inputFileName,
    std::string& output,
    std::stringstream* commentsOnFailure
  );
  static bool GetPhysicalFileNameFromVirtualCustomizedWriteOnly(
    const std::string& inputFileName,
    std::string& output,
    std::stringstream* commentsOnFailure
  );
  static std::string GetVirtualNameWithHash(const std::string& inputFileName);

  static bool GetPhysicalFileNameFromVirtual(
    const std::string& inputFileName,
    std::string& output,
    bool accessSensitiveFolders,
    bool accessULTRASensitiveFolders,
    std::stringstream* commentsOnFailure
  );
  static bool OpenFileCreateIfNotPresentUnsecure(std::fstream& theFile, const std::string& theFileName, bool OpenInAppendMode, bool truncate, bool openAsBinary);
  static bool OpenFileCreateIfNotPresentVirtual(std::fstream& theFile, const std::string& theFileName, bool OpenInAppendMode, bool truncate, bool openAsBinary, bool accessSensitiveFolders = false);
  static bool OpenFileCreateIfNotPresentVirtualCreateFoldersIfNeeded(std::fstream& theFile, const std::string& theFileName, bool OpenInAppendMode, bool truncate, bool openAsBinary, bool accessSensitiveFolders = false);

  static bool OpenFileUnsecure(std::fstream& theFile, const std::string& theFileName, bool OpenInAppendMode, bool truncate, bool openAsBinary);
  static bool OpenFileUnsecureReadOnly(std::ifstream& theFile, const std::string& theFileName, bool openAsBinary);
  static bool OpenFileVirtual(std::fstream& theFile, const std::string& theFileName, bool OpenInAppendMode, bool truncate, bool openAsBinary, bool accessSensitiveFolders = false);
  static bool OpenFileVirtualCustomizedReadOnly(
    std::fstream& theFile,
    const std::string& theFileName,
    bool OpenInAppendMode,
    bool truncate,
    bool openAsBinary,
    std::stringstream* commentsOnFailure
  );
  static bool OpenFileVirtualCustomizedWriteOnly(
    std::fstream& theFile,
    const std::string& theFileName,
    bool OpenInAppendMode,
    bool truncate,
    bool openAsBinary,
    std::stringstream* commentsOnFailure
  );
  static bool OpenFileVirtualCustomizedWriteOnlyCreateIfNeeded(
    std::fstream& theFile,
    const std::string& theFileName,
    bool OpenInAppendMode,
    bool truncate,
    bool openAsBinary,
    std::stringstream* commentsOnFailure
  );
  static bool OpenFileVirtualReadOnly(std::ifstream& theFile, const std::string& theFileName, bool openAsBinary, bool accessSensitiveFolders = false);
};

struct StateMaintainerCurrentFolder
{
public:
  std::string currentFolderPhysicalAbsolute;
  StateMaintainerCurrentFolder();
  ~StateMaintainerCurrentFolder();
};
#endif
