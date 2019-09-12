//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#ifndef vpfHeader1General8DatabaseSystemIndependent_already_included
#define vpfHeader1General8DatabaseSystemIndependent_already_included

#include "general_lists.h"
static ProjectInformationInstance ProjectInfoVpfHeader1General8DatabaseSystemIndependentinstance(
  __FILE__, "Header, system independent database data structures. "
);
#include "json.h"

class UserCalculatorData {
  public:
  struct Roles {
    static std::string admin;
    static std::string student;
    static std::string instructor;
    static std::string teacher;
  };

  double approximateHoursSinceLastTokenWasIssued;
  std::string usernameHashedPlusPassWordHashed;
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

#endif
