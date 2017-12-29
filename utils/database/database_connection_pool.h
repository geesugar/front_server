/*
 * Copyright [2013-2017] <geesugar>
 * file: database_connection_pool.h
 * author: longtao@geesugar.com
 * date: 2017-12-28
 */
#ifndef UTILS_DATABASE_DATABASE_CONNECTION_POOL_H
#define UTILS_DATABASE_DATABASE_CONNECTION_POOL_H

#include <mysql++/cpool.h>
#include <mysql++/mysql++.h>

#include <condition_variable>
#include <mutex>
#include <string>

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

namespace utils {
class DatabaseConnectionPool : public mysqlpp::ConnectionPool {
 public:
  DatabaseConnectionPool(const std::string& db_name,
    const std::string& db_server, int32_t db_port,
    const std::string& db_user, const std::string& db_passwd,
    const std::string& db_default_charset, int32_t connect_timeout);

  virtual ~DatabaseConnectionPool();

  // @ Do a simple form of in-use connection limiting: wait to return
  // @ a connection until there are a reasonably low number in use
  // @ already.  Can't do this in create() because we're interested in
  // @ connections actually in use, not those created.  Also note that
  // @ we keep our own count; ConnectionPool::size() isn't the same!
  mysqlpp::Connection* grab();

  // @ parameter: is_bad_con
  // @    If is_bad_con set true, then the connection specified by pc
  // @ will be removed from db connection pool, otherwise it return to
  // @ pool.
  void release(const mysqlpp::Connection* pc, bool is_bad_con);

 protected:
  void SetDefaultClientCharset(mysqlpp::Connection *pc);
  void release(const mysqlpp::Connection* pc);

  // @ Superclass overrides
  mysqlpp::Connection* create();
  void destroy(mysqlpp::Connection* pc);
  unsigned int max_idle_time();

 private:
  // @ Number of connections currently in use
  int32_t m_conns_in_use;

  // @ exclusive access m_conns_in_use
  std::mutex              m_conns_in_use_mutex;
  std::condition_variable m_conns_in_use_mutex_cond;

  // @ connection parameters
  std::string m_db_name;
  std::string m_db_server;
  std::string m_db_user;
  std::string m_db_passwd;
  int32_t     m_db_port;
  std::string m_db_default_charset;
  int32_t     m_db_connect_timeout;
  int32_t     m_db_read_timeout;
  int32_t     m_db_write_timeout;
};  // class DatabaseConnectionPool
}  // namespace utils
#endif  // UTILS_DATABASE_DATABASE_CONNECTION_POOL_H
