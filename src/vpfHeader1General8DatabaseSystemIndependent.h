//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1General8DatabaseSystemIndependent_already_included
#define vpfHeader1General8DatabaseSystemIndependent_already_included

#include "vpfHeader1General0_General.h"
static ProjectInformationInstance ProjectInfoVpfHeader1General8DatabaseSystemIndependentinstance(__FILE__, "Header, system independent database data structures. ");
#include "vpfJson.h"

class UserCalculatorData
{
  public:
  double approximateHoursSinceLastTokenWasIssued;
  std::string usernamePlusPassWord;
  std::string userId;
  std::string username;
  std::string email;
  std::string actualActivationToken;
  std::string enteredActivationToken;
  std::string enteredAuthenticationToken;
  std::string actualAuthenticationToken;
  std::string problemDataStrinG;
  JSData problemDataJSON;

  // Plain text password. Keep in memory as little as possible.
  // Zero bytes as soon as enteredHashedSaltedPassword is computed.
  std::string enteredPassword;
  //At the moment, this is obtained via:
  //Base64url(Sha3(concatenate(user,password))).
  std::string actualHashedSaltedPassword;
  std::string enteredHashedSaltedPassword;
  std::string enteredGoogleToken;
  std::string userRole;
  std::string timeOfAuthenticationTokenCreation;
  std::string timeOfActivationTokenCreation;
  std::string activationEmail;
  std::string activationEmailSubject;

  std::string instructorInDB;
  std::string semesterInDB;
  std::string sectionInDB;
  std::string courseInDB;

  std::string instructorComputed;
  std::string semesterComputed;
  std::string sectionComputed;
  std::string courseComputed;

  std::string problemWeightSchema;
  JSData problemWeights;
  std::string deadlineSchema;
  JSData deadlines;

  List<std::string> sectionsTaught;


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
  bool ComputeCourseInfo();
  void reset();
  void clearPasswordFromMemory();
  void clearAuthenticationTokenAndPassword();
  std::string ToStringUnsecure();
  std::string ToStringCourseInfo();
  List<JSData> GetFindMeFromUserNameQuery();
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
  static std::string labelAuthenticationToken;

  static std::string labelActivationToken;
  static std::string labelUserRole;
  static std::string labelProblemDatA;
  static std::string labelProblemDataJSON;
  static std::string labelTimeOfActivationTokenCreation;
  static std::string labelTimeOfAuthenticationTokenCreation;

  static std::string tableUsers;
///email info
  static std::string tableEmailInfo;
  static std::string labelLastActivationEmailTime;
  static std::string labelNumActivationEmails;
  static std::string labelUsernameAssociatedWithToken;

///course info column names
  static std::string labelCurrentCourses;
  static std::string labelSection;
  static std::string labelDeadlinesSchema;
  static std::string labelProblemWeightsSchema;
  static std::string labelSectionsTaught;
  static std::string labelInstructor;
  static std::string labelSemester;
///deadlines info
  static std::string labelDeadlines;
  static std::string tableDeadlines;
///problem weights info
  static std::string tableProblemWeights;
  static std::string labelProblemWeights;
///problem information
  static std::string tableProblemInformation;
  static std::string labelProblemTotalQuestions;
  static std::string labelProblemName;
///deleted fields
  static std::string anyField;
  static std::string labelFields;
  static std::string labelTable;
  static std::string objectSelector;
  static std::string objectSelectorMongo;
  static std::string labelIdMongo;
///deleted info
  static std::string tableDeleted;
///app info
  static std::string labelCurrentPage;
  static std::string labelPageSignUp;
  static std::string labelPageAccount;
  static std::string labelPageActivateAccount;
  static std::string labelCalculatorRequest;
};
#endif
