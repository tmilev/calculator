
#ifndef header_general_database_system_independent_ALREADY_INCLUDED
#define header_general_database_system_independent_ALREADY_INCLUDED

#include "general_lists.h"
#include "json.h"

class QueryOneOfExactly;

class UserCalculatorData {
public:
  struct Roles {
    static std::string administator;
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
  std::string problemDataString;
  JSData problemDataJSON;
  // Plain text password. Keep in memory as little as possible.
  // zero bytes as soon as enteredHashedSaltedPassword is computed.
  std::string enteredPassword;
  // At the moment, this is obtained via:
  // Base64url(Sha3(concatenate(user,password))).
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
  // List<std::string> sectionsViewableByUser;
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
  bool loadFromJSON(JSData& input);
  JSData toJSON();
  UserCalculatorData();
  bool computeCourseInformation();
  void reset();
  void clearPasswordFromMemory();
  void clearAuthenticationTokenAndPassword();
  // Creates a string that describes the user.
  // Will reveal password hashes, so this should never be shown in
  // production.
  std::string toStringUnsecure();
  // Reveals the username and password, but does not show any
  // password hashes.
  std::string toStringSecure();
  std::string toStringCourseInfo();
  void getFindMeQuery(QueryOneOfExactly& output) const;
  std::string toStringFindQueries() const;
};

#endif // header_general_database_system_independent_ALREADY_INCLUDED
