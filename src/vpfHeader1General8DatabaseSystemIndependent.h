//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1General8DatabaseSystemIndependent_already_included
#define vpfHeader1General8DatabaseSystemIndependent_already_included

#include "vpfHeader1General0_General.h"
static ProjectInformationInstance ProjectInfoVpfHeader1General8DatabaseSystemIndependentinstance(__FILE__, "Header, system independent database data structures. ");

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
  std::string userId;
  std::string username;
  std::string email;
  std::string currentTable;
  std::string actualActivationToken;
  std::string enteredActivationToken;
  std::string enteredAuthenticationToken;
  std::string actualAuthenticationToken;
  std::string problemDataString;
  std::string courseInfoString;
  CourseAndUserInfo courseInfo;

  std::string enteredPassword;
  std::string actualShaonedSaltedPassword;
  std::string enteredShaonedSaltedPassword;
  std::string enteredGoogleToken;
  std::string userRole;
  std::string timeOfAuthenticationTokenCreation;
  std::string timeOfActivationTokenCreation;
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
  bool LoadFromJSON(JSData& input);
  JSData ToJSON();
  UserCalculatorData();
  bool AssignCourseInfoString(std::stringstream* errorStream);
  std::string ToStringIdSectionCourse();
  void reset();
  void clearPasswordFromMemory();
  void clearAuthenticationTokenAndPassword();
  std::string ToStringUnsecure();
};

struct DatabaseStrings
{
  static List<std::string> modifyableColumns;
public:
///credentials of calculator to use database
  static std::string theDatabaseUser;
  static std::string theDatabaseName;
  static std::string theDatabaseNameMongo;
///user column names
  static std::string labelUserId;
  static std::string labelUsername;
  static std::string labelEmail;
  static std::string labelPassword;
  static std::string labelCourseInfo;
  static std::string labelAuthenticationToken;

  static std::string labelActivationToken;
  static std::string labelUserRole;
  static std::string labelProblemData;
  static std::string labelTimeOfActivationTokenCreation;
  static std::string labelTimeOfAuthenticationTokenCreation;

  static std::string tableUsers;
///email info
  static std::string tableEmailInfo;
  static std::string labelLastActivationEmailTime;
  static std::string labelNumActivationEmails;
  static std::string labelUsernameAssociatedWithToken;

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
