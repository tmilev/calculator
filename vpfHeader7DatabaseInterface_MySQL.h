//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader7_databaseMySQL_already_included
#define vpfHeader7_databaseMySQL_already_included
#ifdef MACRO_use_MySQL
#include "vpfHeader3Calculator0_Interface.h"
#include <mysql/mysql.h>
static ProjectInformationInstance ProjectInfoVpf8_1HeaderDatabaseInterface_MySQLx(__FILE__, "MySQL interface header. ");

class DatabaseRoutines
{
  template<typename anyType>
  friend DatabaseRoutines& operator << (DatabaseRoutines& output, const anyType& any)
  { output.comments << any;
    return output;
  }
public:
  std::string username;
  std::string password;
  std::string theDatabaseName;
  std::string hostname;
  std::stringstream comments;
  MYSQL *connection; // Create a pointer to the MySQL instance
  operator bool()const
  { return false;
  }
  bool Query(const std::string& inputQuery);
  bool startMySQLDatabase();
  std::string ToString();
  static bool innerTestDatabase(Calculator& theCommands, const Expression& input, Expression& output);
  DatabaseRoutines();
};

#endif // MACRO_use_MySQL
#endif // vpfHeader7_databaseMySQL_already_included
