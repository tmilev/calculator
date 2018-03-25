//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1General8DatabaseSystemIndependent_already_included
#define vpfHeader1General8DatabaseSystemIndependent_already_included

#include "vpfHeader1General0_General.h"
static ProjectInformationInstance ProjectInfoVpfHeader1General8DatabaseSystemIndependentinstance(__FILE__, "Header, system independent database data structures. ");

class DatabaseData
{
//This class is needed to attempt to deal with mySQL's
//numerous design errors, to the extent possible.
//Documenting those errors for the interested reader.
//1. Mysql identifiers have max length of 64 characters.
//   Workaround this MySQL bug: when used as identifiers, strings are
//   trimmed. We use the first 30 characters
//   + we append SHA-1 of the entire string.
//   Motivation: we don't lose human-readability for small strings.
//2. Mysql identifiers cannot have ` characters in them.
//   Workaround this MySQL bug: we url-encode any data stored in
//   the database.
//   Motivation: we retain limited human-readability.
//-------------------------
//The value entry of the class stores an arbitrary sequence of characters,
//the sequence we care about and want stored/loaded from DB.
public:
  std::string value;
  DatabaseData(const std::string& other)
  { this->value = other;
  }
  DatabaseData(){}
  bool operator==(const std::string& other)
  { return this->value == other;
  }
  bool operator!=(const std::string& other)
  { return !(*this == other);
  }
  void operator=(const std::string& other)
  { this->value = other;
  }
  std::string GetDatA()const;
  std::string GetDataNoQuotes()const;
  std::string GetIdentifierNoQuotes()const;
  std::string GetIdentifieR()const;
};

class JSData;

struct CourseAndUserInfo
{
public:
  MemorySaving<JSData> courseInfoJSON;
  std::string rawStringStoredInDB;
  std::string problemWeightSchemaIDComputed;
  std::string deadlineSchemaIDComputed;
  std::string instructorComputed;
  std::string semesterComputed;
  std::string sectionComputed;
  std::string courseComputed;
  std::string deadlinesString;
  std::string problemWeightString;
  std::string getCurrentCourseInDB();
  void setCurrentCourseInDB(const std::string& input);
  std::string getSectionInDB();
  void setSectionInDB(const std::string& input);
  std::string getSectonsTaughtByUser();
  void setSectonsTaughtByUser(const std::string& input);
  std::string getInstructorInDB();
  void setInstructorInDB(const std::string& input);

  std::string ToStringForDBStorage();
  std::string ToStringHumanReadable();
  ~CourseAndUserInfo();
};

class UserCalculatorData
{
  public:
  double approximateHoursSinceLastTokenWasIssued;
  std::string usernamePlusPassWord;
  DatabaseData userId;
  DatabaseData username;
  DatabaseData email;
  DatabaseData currentTable;
  DatabaseData timeOfActivationTokenCreation;
  DatabaseData actualActivationToken;
  DatabaseData enteredActivationToken;
  DatabaseData enteredAuthenticationToken;
  DatabaseData actualAuthenticationToken;
  DatabaseData problemDataString;
  CourseAndUserInfo courseInfo;

  std::string enteredPassword;
  std::string actualShaonedSaltedPassword;
  std::string enteredShaonedSaltedPassword;
  std::string enteredGoogleToken;
  std::string userRole;
  std::string authenticationTokenCreationTime;
  std::string activationTokenCreationTime;
  std::string activationEmail;
  std::string activationEmailSubject;
  //List<std::string> sectionsViewableByUser;
  List<std::string> selectedColumnsUnsafe;
  List<std::string> selectedColumnValuesUnsafe;
  List<std::string> selectedColumnsRetrievalFailureRemarks;

  List<std::string> selectedRowFieldsUnsafe;
  List<std::string> selectedRowFieldNamesUnsafe;
  bool flagEnteredAuthenticationToken;
  bool flagEnteredPassword;
  bool flagEnteredActivationToken;
  bool flagMustLogin;
  bool flagStopIfNoLogin;
  bool flagUserHasActivationToken;
  bool flagUserHasNoPassword;
  UserCalculatorData();
  bool AssignCourseInfoString(std::stringstream* errorStream);
  std::string ToStringIdSectionCourse();
  void reset();
  void clearPasswordFromMemory();
  void clearAuthenticationTokenAndPassword();
  std::string ToStringUnsecure();
};

struct DatabaseStrings{
  static List<std::string> modifyableColumns;
public:
///credentials of calculator to use database
  static std::string theDatabaseUser;
  static std::string theDatabaseName;
///user column names
  static std::string columnUserId;
  static std::string columnUsername;
  static std::string columnEmail;
  static std::string tableUsers;
  static std::string columnCourseInfo;
///course info column names
  static std::string columnCurrentCourses;
  static std::string columnSection;
  static std::string columnDeadlinesSchema;
  static std::string columnProblemWeightsSchema;
  static std::string columnSectionsTaught;
  static std::string columnInstructor;
///deadlines info
  static std::string columnDeadlines;
  static std::string tableDeadlines;
///problem weights info
  static std::string tableProblemWeights;
  static std::string columnProblemWeights;

  static List<std::string>& GetModifyableColumnsNotThreadSafe();

};
#endif
