/*
 * Copyright [2013-2017] <geesugar>
 * file: database_connector.h
 * author: longtao@geesugar.com
 * date: 2017-12-28
 */
#ifndef UTILS_DATABASE_DATABASE_CONNECTOR_H
#define UTILS_DATABASE_DATABASE_CONNECTOR_H

#include <memory>

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "utils/database/database_connection_pool.h"

namespace utils {
DECLARE_string(db_name);
DECLARE_string(db_user_name);
DECLARE_string(db_user_pass);
DECLARE_string(db_host);
DECLARE_int32(db_port);

// @ DatabaseConnector wrapp the DatabaseConnectionPool,
// @ all db operation should be done via DatabaseConnector's
// @ interfaces
// @
// @ DatabaseConnector provides two types of query interface
// @ one query via defualt charset of db connection, and the
// @ other via your specified charset of db connection.

// @ CAUTIONS:
// @ 1.
// @ In order to make DatabaseConnector meets more common situations,
// @ a lot of error or exception are handled in all it's interfaces,
// @ but you keep in mind that you should have to check the execute
// @ return value to figure out whether the execution is succeed, then
// @ you can dicide to do other operations.
// @
// @ 2.
// @   Commonly, you don't need to retry your executions, cause retries
// @ are already done in each execution interfaces, even the database
// @ connection reinitialization.
// @
// @ 3. MORE ATTENTIONS:
// @   If your have the need to stop/core your service while the database
// @ is down/cann't be connected, check the is_db_unreachable flag. If
// @ is_db_unreachable is true, then the database currently is unreachable,
// @
class DatabaseConnector {
public:
 // @ static const variables
 // @
 // @ kUTF8CharSetName and kGBKCharSetName are used as char set
 // @ name option for the database connection
 static const std::string kUTF8CharSetName;
 static const std::string kGBKCharSetName;
 static const std::string kLATIN1CharSetName;

 DatabaseConnector(
   const std::string& db_name = "",
   const std::string& db_user_name = "",
   const std::string& db_user_pass = "",
   const std::string& db_host = "",
   const int32_t& db_port = 0,
   const std::string& db_default_charset = "",
   const int32_t& connect_timeout = 10);

 virtual ~DatabaseConnector();

 // @ this interface do the query with specified charset of
 // @ db connection of your database
 bool ExecuteQuery(const std::string& query_str,
   const std::string& char_set_name,
   mysqlpp::StoreQueryResult* query_ret,
   bool* is_db_unreachable = NULL);

 // @ this interface do the query with default chareset of
 // @ db connection
 bool ExecuteQuery(const std::string& query_str,
   mysqlpp::StoreQueryResult* query_ret,
   bool* is_db_unreachable = NULL);

 // @ this interface do execution of sql statement
 // @ the simple_ret is an out parameter which contains the
 // @ detail info of sql execution
 bool Execute(const std::string& sql_str,
   bool* is_db_unreachable = NULL,
   mysqlpp::SimpleResult* simple_ret = NULL);

 // @ this interface do escape of sql statement
 bool Escape(std::string* sql_str,
   bool* is_db_unreachable = NULL);

private:
 bool Init();

private:
 std::shared_ptr<DatabaseConnectionPool> m_db_cons_pool;

 std::string m_db_name;      // database name
 std::string m_db_user_name; // database user name
 std::string m_db_user_pass; // database user password
 std::string m_db_host;      // database server ip
 int32_t     m_db_port;      // database port
 std::string m_db_default_charset; // default char set of client connection
 int32_t     m_db_connect_timeout; // database connect timeout in seconds
};  // class DatabaseConnector
}  // namespace utils
#endif  // UTILS_DATABASE_DATABASE_CONNECTOR_H
