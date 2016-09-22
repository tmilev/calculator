//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1General8DatabaseSystemIndependent_already_included
#define vpfHeader1General8DatabaseSystemIndependent_already_included

#include "vpfHeader1General0_General.h"
static ProjectInformationInstance ProjectInfoVpfHeader1General8DatabaseSystemIndependentinstance(__FILE__, "Header, system independent database data structures. ");

class MySQLdata{
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
  MySQLdata(const std::string& other)
  { this->value=other;
  }
  MySQLdata(){}
  bool operator==(const std::string& other)
  { return this->value==other;
  }
  bool operator!=(const std::string& other)
  { return !(*this==other);
  }
  void operator=(const std::string& other)
  { this->value=other;
  }
  std::string GetDatA()const;
  std::string GetDataNoQuotes()const;
  std::string GetIdentifierNoQuotes()const;
  std::string GetIdentifieR()const;
};

class UserCalculatorData{
  public:
  double approximateHoursSinceLastTokenWasIssued;
  std::string usernamePlusPassWord;
  MySQLdata userId;
  MySQLdata username;
  MySQLdata email;
  MySQLdata currentTable;
  MySQLdata actualActivationToken;
  MySQLdata enteredActivationToken;
  MySQLdata enteredAuthenticationToken;
  MySQLdata actualAuthenticationToken;
  MySQLdata problemDataString;
  MySQLdata problemInfoRowId;
  MySQLdata problemInfoString;
  MySQLdata sectionInfoString;
  MySQLdata deadlineInfoRowId;
  MySQLdata deadlineInfoString;
  MySQLdata userGroup;

  std::string enteredPassword;
  std::string actualShaonedSaltedPassword;
  std::string enteredShaonedSaltedPassword;
  std::string userRole;
  std::string authenticationTokenCreationTime;
  //List<std::string> sectionsViewableByUser;
  List<std::string> selectedColumnsUnsafe;
  List<std::string> selectedColumnValuesUnsafe;
  List<std::string> selectedColumnsRetrievalFailureRemarks;

  List<std::string> selectedRowFieldsUnsafe;
  List<std::string> selectedRowFieldNamesUnsafe;
  void reset();
  void resetPassword();
  void resetAuthenticationTokenAndPassword();
  std::string ToStringUnsecure();
};

struct DatabaseStrings{
public:
  static std::string userId;

  static std::string databaseUser;
  static std::string theDatabaseName;
  static std::string userColumnLabel;
  static std::string usersTableName;
  static std::string userGroupLabel;

  static std::string deadlinesTableName;
  static std::string deadlinesIdColumnName;
  static std::string infoColumnInDeadlinesTable;

  static std::string problemWeightsTableName;
  static std::string problemWeightsIdColumnName;
  static std::string infoColumnInProblemWeightsTable;

  static std::string sectionsList;

};
#endif
