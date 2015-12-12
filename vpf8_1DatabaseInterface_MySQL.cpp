//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifdef MACRO_use_MySQL
#include "vpfHeader7DatabaseInterface_MySQL.h"
ProjectInformationInstance ProjectInfoVpf8_1MySQLcpp(__FILE__, "MySQL interface. ");

std::string DatabaseRoutines::ToString()
{ MacroRegisterFunctionWithName("DatabaseRoutines::ToString");
  std::stringstream out;
  out << "Mysql, user: " << this->username << ", password: " << this->password << ", database: "
  << this->theDatabaseName;
  return out.str();
}

DatabaseRoutines::DatabaseRoutines()
{ this->connection=0;
}

bool DatabaseRoutines::Query(const std::string& inputQuery)
{ MacroRegisterFunctionWithName("DatabaseRoutines::Query");
  if (mysql_query(this->connection, inputQuery.c_str())!=0)
    return *this << "Query " << inputQuery << " failed. ";
  return true;
}

bool DatabaseRoutines::startMySQLDatabase()
{ MacroRegisterFunctionWithName("DatabaseRoutines::startMySQLDatabase");
  this->password="";
  this->username="calculator";
  this->hostname="localhost";
  this->theDatabaseName="calculatorUsers";
  this->connection=mysql_init(0); // Initialise the instance
  if(this->connection==0)
    return *this << "MySQL initialization failed.";
  //real connection to the database follows.
  this->connection=mysql_real_connect
  (this->connection, this->hostname.c_str(), this->username.c_str(), this->password.c_str(),
   this->theDatabaseName.c_str(), 0, 0, 0);
  if(this->connection==0)
    return *this << "Connection failed on: " << this->ToString();
  stOutput << "Connection succeeded.\n";
  *this << "Connection succeeded\n";
  if (mysql_select_db(this->connection, this->theDatabaseName.c_str())!=0)//Note: here zero return value = success.
    return *this << "Failed to select database: " << this->theDatabaseName << ". ";
  if (!this->Query("CREATE TABLE users(email VARCHAR(50) NOT NULL PRIMARY KEY)"))
    return false;
  mysql_close(this->connection);   // Close and shutdown
  return 0;
}

bool DatabaseRoutines::innerTestDatabase(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("DatabaseRoutines::innerTestDatabase");
  DatabaseRoutines theRoutines;
  std::stringstream out;
  out << "Testing database ... Comments:<br>";
  theRoutines.startMySQLDatabase();
  out << theRoutines.comments.str();
  return output.AssignValue(out.str(), theCommands);
}
#endif // MACRO_use_MySQL

