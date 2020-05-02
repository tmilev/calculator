// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader1General7FileOperations_Encodings_already_included
#define vpfHeader1General7FileOperations_Encodings_already_included

#include "general_lists.h"
#include "general_list_references.h"
#include "general_maps.h"

class FileOperations {
public:
  static HashedList<std::string, MathRoutines::hashString>&
  folderStartsToWhichWeAppendInstructorUsernameSlash();
  static HashedList<std::string, MathRoutines::hashString>&
  folderVirtualLinksToWhichWeAppendTimeAndBuildHash();

  static MapList<std::string, std::string, MathRoutines::hashString>&
  folderVirtualLinksNonSensitive();
  static MapList<std::string, std::string, MathRoutines::hashString>&
  folderVirtualLinksSensitive(); //<- admin access only
  static MapList<std::string, std::string, MathRoutines::hashString>&
  folderVirtualLinksULTRASensitive(); //<- no access allowed through web server

  static List<List<std::string> >& folderVirtualLinksDefault();

  static bool checkFolderLinks();
  static void initializeFoldersNonSensitive();
  static void initializeFoldersSensitive();
  static void initializeFoldersULTRASensitive();
  static List<List<std::string> >& initializeFolderVirtualLinksDefaults();

  static List<bool> safeFileCharacters;
  static List<bool>& getSafeFileChars();

  static std::string convertStringToEscapedStringFileNameSafe(const std::string& input);
  static std::string convertStringToLatexFileName(const std::string& input);
  static bool loadFileToStringUnsecure(
    const std::string& fileNameUnsecure,
    std::string& output,
    std::stringstream* commentsOnFailure
  );
  static bool loadFileToStringVirtualCustomizedReadOnly(
    const std::string& fileName,
    std::string& output,
    std::stringstream* commentsOnFailure
  );
  static bool writeFileVirual(
    const std::string& fileNameVirtual,
    const std::string& fileContent,
    std::stringstream *commentsOnError
  );
  static bool writeFileVirualWithPermissions(
    const std::string& fileNameVirtual,
    const std::string& fileContent,
    bool accessSensitiveFolders,
    std::stringstream *commentsOnError
  );
  static bool writeFileVirualWithPermissions_AccessUltraSensitiveFoldersIfNeeded(
    const std::string& fileNameVirtual,
    const std::string& fileContent,
    bool accessSensitiveFolders,
    bool accessUltraSensitiveFolders,
    std::stringstream *commentsOnError
  );
  static std::string writeFileReturnHTMLLink(
    const std::string& fileContent, const std::string& fileNameVirtual, const std::string& linkText
  );
  static bool loadFileToStringVirtual(
    const std::string& fileName,
    std::string& output,
    bool accessSensitiveFolders,
    std::stringstream* commentsOnFailure
  );
  // This function can access ultra sensitive folders.
  // Please do not use outside of critical functions.
  // At the time of writing, there are only
  // a few use cases that qualify.
  // 1. Load the server's private key and server certificate.
  //    The certificate is not a secret but we keep it next to
  //    the private key for ease of management.
  // 2. Load other server secrets:
  //    - recaptcha credentials
  //    - email credentials.
  // 3. Load private user computation reports from the results/ folder.
  //    These are not expected to contain secret information
  //    or pose security risks. However since they contain personal user
  //    data we treat them as ultra sensitive.
  // Ultra sensitive data is only available through administrator ssh
  // connection, or, in the case of the results/ folder, only with a
  // 256 bit secure one-time use token (= filename of the actual report).
  // The available tokens are only relayed to the party that initiated the computation.
  // They available tokens can only be browsed with an ssh admin connection.
  static bool loadFileToStringVirtual_AccessUltraSensitiveFoldersIfNeeded(
    const std::string& fileName,
    std::string& output,
    bool accessSensitiveFolders,
    bool accessULTRASensitiveFolders,
    std::stringstream* commentsOnFailure
  );
  static bool isOKFileNameVirtual(
    const std::string& fileName,
    bool accessSensitiveFolders = false,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool isFileNameWithoutDotsAndSlashes(const std::string& fileName);
  static bool isFileNameSafeForSystemCommands(const std::string& fileName, std::stringstream* commentsOnFailure);
  static std::string getFileNameFromFileNameWithPath(const std::string& fileName);
  static std::string getPathFromFileNameWithPath(const std::string& fileName);
  static std::string getFileExtensionWithDot(const std::string& theFileName, std::string* outputFileNameNoExtension = nullptr);
  static bool fileExistsUnsecure(const std::string& theFileName);
  static bool fileExistsVirtual(
    const std::string& theFileName,
    bool accessSensitiveFolders = false,
    bool accessULTRASensitiveFolders = false,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool fileExistsVirtualCustomizedReadOnly(const std::string& theFileName, std::stringstream* commentsOnFailure);
  static bool isFolderUnsecure(const std::string& theFolderName);
  static bool getFolderFileNamesUnsecure(
    const std::string& theFolderName,
    List<std::string>& outputFileNamesNoPath,
    List<std::string>* outputFileTypesWithDot = nullptr
  );
  static std::string cleanUpForFileNameUse(const std::string& inputString);
  static std::string getCurrentFolder();
  static std::string getWouldBeFolderAfterHypotheticalChdirNonThreadSafe(const std::string& wouldBePath);
  static bool getFolderFileNamesVirtual(
    const std::string& theFolderName,
    List<std::string>& outputFileNamesNoPath,
    List<std::string>* outputFileTypesWithDot = nullptr,
    bool accessSensitiveFolders = false,
    bool accessULTRASensitiveFolders = false,
    std::stringstream *commentsOnFailure = nullptr
  );
  static bool getPhysicalFileNameFromVirtualCustomizedReadOnly(
    const std::string& inputFileName,
    std::string& output,
    std::stringstream* commentsOnFailure
  );
  static bool getPhysicalFileNameFromVirtualCustomizedWriteOnly(
    const std::string& inputFileName,
    std::string& output,
    std::stringstream* commentsOnFailure
  );
  static std::string GetVirtualNameWithHash(const std::string& inputFileName);
  static bool getPhysicalFileNameFromVirtual(
    const std::string& inputFileName,
    std::string& output,
    bool accessSensitiveFolders,
    bool accessULTRASensitiveFolders,
    std::stringstream* commentsOnFailure
  );
  static bool openFileCreateIfNotPresentUnsecure(
    std::fstream& theFile,
    const std::string& theFileName,
    bool OpenInAppendMode,
    bool truncate,
    bool openAsBinary
  );
  static bool openFileCreateIfNotPresentVirtual(std::fstream& theFile,
    const std::string& theFileName,
    bool OpenInAppendMode,
    bool truncate,
    bool openAsBinary,
    bool accessSensitiveFolders = false,
    bool accessUltraSensitiveFolders = false
  );
  static bool openFileCreateIfNotPresentVirtualCreateFoldersIfNeeded_UltraSensitiveOptions(
    std::fstream& theFile,
    const std::string& theFileName,
    bool OpenInAppendMode,
    bool truncate,
    bool openAsBinary,
    bool accessSensitiveFolders = false,
    bool accessUltraSensitiveFolders = false
  );
  static bool openFileCreateIfNotPresentVirtualCreateFoldersIfNeeded(
    std::fstream& theFile,
    const std::string& theFileName,
    bool OpenInAppendMode,
    bool truncate,
    bool openAsBinary,
    bool accessSensitiveFolders = false
  );
  static bool openFileUnsecure(
    std::fstream& theFile,
    const std::string& theFileName,
    bool OpenInAppendMode,
    bool truncate,
    bool openAsBinary
  );
  static bool openFileUnsecureReadOnly(std::ifstream& theFile, const std::string& theFileName, bool openAsBinary);
  static bool openFileVirtual(
    std::fstream& theFile,
    const std::string& theFileName,
    bool OpenInAppendMode,
    bool truncate,
    bool openAsBinary,
    bool accessSensitiveFolders = false
  );
  static bool openFileVirtualCustomizedReadOnly(
    std::fstream& theFile,
    const std::string& theFileName,
    bool OpenInAppendMode,
    bool truncate,
    bool openAsBinary,
    std::stringstream* commentsOnFailure
  );
  static bool openFileVirtualCustomizedWriteOnly(
    std::fstream& theFile,
    const std::string& theFileName,
    bool OpenInAppendMode,
    bool truncate,
    bool openAsBinary,
    std::stringstream* commentsOnFailure
  );
  static bool openFileVirtualCustomizedWriteOnlyCreateIfNeeded(
    std::fstream& theFile,
    const std::string& theFileName,
    bool OpenInAppendMode,
    bool truncate,
    bool openAsBinary,
    std::stringstream* commentsOnFailure
  );
  static bool openFileVirtualReadOnly(
    std::ifstream& theFile,
    const std::string& theFileName,
    bool openAsBinary,
    bool accessSensitiveFolders = false
  );
};

class StateMaintainerCurrentFolder {
public:
  std::string currentFolderPhysicalAbsolute;
  StateMaintainerCurrentFolder();
  ~StateMaintainerCurrentFolder();
};
#endif
