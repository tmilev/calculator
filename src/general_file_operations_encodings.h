
#ifndef header_general_file_operations_encodings_ALREADY_INCLUDED
#define header_general_file_operations_encodings_ALREADY_INCLUDED

#include "general_lists.h"
#include "general_list_references.h"
#include "general_maps.h"

class FileOperations {
public:
  static HashedList<std::string>&
  folderStartsToWhichWeAppendInstructorUsernameSlash();
  static HashedList<std::string>&
  folderVirtualLinksToWhichWeAppendTimeAndBuildHash();
  static MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  >& folderVirtualLinksNonSensitive();
  static MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  >& folderVirtualLinksSensitive();
  // <- admin access only
  static MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  >& folderVirtualLinksULTRASensitive();
  // <- no access allowed through web server
  static List<List<std::string> >& folderVirtualLinksDefault();
  static bool checkFolderLinks();
  static void initializeFoldersNonSensitive();
  static void initializeFoldersSensitive();
  static void initializeFoldersULTRASensitive();
  static List<List<std::string> >& initializeFolderVirtualLinksDefaults();
  static List<bool> safeFileCharacters;
  static List<bool>& getSafeFileChars();
  static std::string convertStringToEscapedStringFileNameSafe(
    const std::string& input
  );
  static std::string convertStringToLatexFileName(const std::string& input);
  static bool loadFileToStringUnsecure(
    const std::string& fileNameUnsecure,
    std::string& output,
    std::stringstream* commentsOnFailure
  );
  static bool loadFiletoStringVirtualCustomizedReadOnly(
    const std::string& fileName,
    std::string& output,
    std::stringstream* commentsOnFailure
  );
  static bool writeFileVirtual(
    const std::string& fileNameVirtual,
    const std::string& fileContent,
    std::stringstream* commentsOnError
  );
  static bool deleteFileVirtual(
    const std::string& fileNameVirtual,
    std::stringstream* commentsOnError
  );
  static bool writeFileVirualWithPermissions(
    const std::string& fileNameVirtual,
    const std::string& fileContent,
    bool accessSensitiveFolders,
    std::stringstream* commentsOnError
  );
  static bool
  writeFileVirualWithPermissions_AccessUltraSensitiveFoldersIfNeeded(
    const std::string& fileNameVirtual,
    const std::string& fileContent,
    bool accessSensitiveFolders,
    bool accessUltraSensitiveFolders,
    std::stringstream* commentsOnError
  );
  static std::string writeFileReturnHTMLLink(
    const std::string& fileContent,
    const std::string& fileNameVirtual,
    const std::string& linkText
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
  // The available tokens are only relayed to the party that initiated the
  // computation.
  // They available tokens can only be browsed with an ssh admin connection.
  static bool loadFiletoStringVirtual_accessUltraSensitiveFoldersIfNeeded(
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
  static bool isFileNameSafeForSystemCommands(
    const std::string& fileName, std::stringstream* commentsOnFailure
  );
  static std::string getFileNameFromFileNameWithPath(
    const std::string& fileName
  );
  static std::string getPathFromFileNameWithPath(
    const std::string& fileName
  );
  static std::string getFileExtensionWithDot(
    const std::string& fileName,
    std::string* outputFileNameNoExtension = nullptr
  );
  static bool fileExistsUnsecure(const std::string& fileName);
  static bool fileExistsVirtual(
    const std::string& fileName,
    bool accessSensitiveFolders = false,
    bool accessULTRASensitiveFolders = false,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool fileExistsVirtualCustomizedReadOnly(
    const std::string& fileName, std::stringstream* commentsOnFailure
  );
  static bool isFolderUnsecure(const std::string& folderName);
  static bool getFolderFileNamesUnsecure(
    const std::string& folderName,
    List<std::string>& outputFileNamesNoPath,
    List<std::string>* outputFileTypesWithDot = nullptr
  );
  static std::string cleanUpForFileNameUse(const std::string& inputString);
  static std::string getCurrentFolder();
  static std::string getWouldBeFolderAfterHypotheticalChdirNonThreadSafe(
    const std::string& wouldBePath
  );
  static bool getFolderFileNamesVirtual(
    const std::string& folderName,
    List<std::string>& outputFileNamesNoPath,
    List<std::string>* outputFileTypesWithDot = nullptr,
    bool accessSensitiveFolders = false,
    bool accessULTRASensitiveFolders = false,
    std::stringstream* commentsOnFailure = nullptr
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
  static std::string getVirtualNameWithHash(
    const std::string& inputFileName
  );
  static bool getPhysicalFileNameFromVirtual(
    const std::string& inputFileName,
    std::string& output,
    bool accessSensitiveFolders,
    bool accessULTRASensitiveFolders,
    std::stringstream* commentsOnFailure
  );
  static bool openFileCreateIfNotPresentUnsecure(
    std::fstream& file,
    const std::string& fileName,
    bool openInAppendMode,
    bool truncate,
    bool openAsBinary
  );
  static bool openFileCreateIfNotPresentVirtual(
    std::fstream& file,
    const std::string& fileName,
    bool openInAppendMode,
    bool truncate,
    bool openAsBinary,
    bool accessSensitiveFolders = false,
    bool accessUltraSensitiveFolders = false
  );
  static bool
  openFileCreateIfNotPresentVirtualCreateFoldersIfNeeded_UltraSensitiveOptions(
    std::fstream& file,
    const std::string& fileName,
    bool openInAppendMode,
    bool truncate,
    bool openAsBinary,
    bool accessSensitiveFolders = false,
    bool accessUltraSensitiveFolders = false
  );
  static bool openFileCreateIfNotPresentVirtualCreateFoldersIfNeeded(
    std::fstream& file,
    const std::string& fileName,
    bool openInAppendMode,
    bool truncate,
    bool openAsBinary,
    bool accessSensitiveFolders = false
  );
  static bool openFileUnsecure(
    std::fstream& file,
    const std::string& fileName,
    bool openInAppendMode,
    bool truncate,
    bool openAsBinary
  );
  static bool openFileUnsecureReadOnly(
    std::ifstream& file, const std::string& fileName, bool openAsBinary
  );
  static bool openFileVirtual(
    std::fstream& file,
    const std::string& fileName,
    bool OpenInAppendMode,
    bool truncate,
    bool openAsBinary,
    bool accessSensitiveFolders = false
  );
  static bool openFileVirtualCustomizedReadOnly(
    std::fstream& file,
    const std::string& fileName,
    bool OpenInAppendMode,
    bool truncate,
    bool openAsBinary,
    std::stringstream* commentsOnFailure
  );
  static bool openFileVirtualCustomizedWriteOnly(
    std::fstream& file,
    const std::string& fileName,
    bool OpenInAppendMode,
    bool truncate,
    bool openAsBinary,
    std::stringstream* commentsOnFailure
  );
  static bool openFileVirtualCustomizedWriteOnlyCreateIfNeeded(
    std::fstream& file,
    const std::string& fileName,
    bool OpenInAppendMode,
    bool truncate,
    bool openAsBinary,
    std::stringstream* commentsOnFailure
  );
  static bool openFileVirtualReadOnly(
    std::ifstream& file,
    const std::string& fileName,
    bool openAsBinary,
    bool accessSensitiveFolders = false
  );
  // Combines paths making sure that we don't get a "//" in the middle.
  static std::string addPaths(
    const std::string& left, const std::string right
  );
};

class StateMaintainerCurrentFolder {
public:
  std::string currentFolderPhysicalAbsolute;
  StateMaintainerCurrentFolder();
  ~StateMaintainerCurrentFolder();
};

#endif // header_general_file_operations_encodings_ALREADY_INCLUDED
