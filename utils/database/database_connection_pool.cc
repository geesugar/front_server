/*
 * Copyright [2013-2017] <geesugar>
 * file: database_connection_pool.cc
 * author: longtao@geesugar.com
 * date: 2017-12-28
 */
#include "utils/database/database_connection_pool.h"
namespace utils {
DEFINE_int32(pool_max_db_cons, 20, "the max connections in a database pool");
DEFINE_int32(connection_max_idle_time, 120,
  "the max connections idle time before timeout");
DECLARE_int32(db_retry_time);

DatabaseConnectionPool::DatabaseConnectionPool(const std::string& db_name,
  const std::string& db_server, int32_t db_port,
  const std::string& db_user, const std::string& db_passwd,
  const std::string& db_default_charset, int32_t connect_timeout)
  : m_conns_in_use(0), m_db_name(db_name),
  m_db_server(db_server), m_db_user(db_user),
  m_db_passwd(db_passwd), m_db_port(db_port),
  m_db_default_charset(db_default_charset),
  m_db_connect_timeout(connect_timeout) {
  }

// @ The destructor.  We _must_ call ConnectionPool::clear() here,
// @ because our superclass can't do it for us.
DatabaseConnectionPool::~DatabaseConnectionPool() {
  clear();
}

mysqlpp::Connection* DatabaseConnectionPool::grab() {
  mysqlpp::Connection *pconnection = NULL;
  {
    std::unique_lock<std::mutex> lock(m_conns_in_use_mutex);
    while (m_conns_in_use > FLAGS_pool_max_db_cons) {
      LOG(WARNING) << "the number of db connection in pool reach to it"
        << "max allowed:<" << m_conns_in_use << ">, wait to get conns"
        << " released by others.";

      m_conns_in_use_mutex_cond.wait(lock);
    }

    pconnection = mysqlpp::ConnectionPool::grab();
    if (NULL != pconnection) {
      ++m_conns_in_use;
      VLOG(150) << "the number of db connections in pool is:<"
        << m_conns_in_use  << ">.";
    }
  }

  // @ set the default charset of client connection
  SetDefaultClientCharset(pconnection);
  return pconnection;
}

void DatabaseConnectionPool::SetDefaultClientCharset(mysqlpp::Connection *pc) {
  if (!pc) return;

  if (!m_db_default_charset.empty()) {
    std::ostringstream oss_sql;
    oss_sql << "set NAMES '" << m_db_default_charset << "'";
    mysqlpp::Query query = pc->query(oss_sql.str());
    mysqlpp::SimpleResult ret = query.execute();
  }
}

void DatabaseConnectionPool::release(const mysqlpp::Connection* pc,
  bool is_bad_con) {
  std::unique_lock<std::mutex> lock(m_conns_in_use_mutex);
  mysqlpp::ConnectionPool::release(pc);

  VLOG(100) << "[DatabaseConnectionPool::release] mysql[" << m_db_server << ":"
    << m_db_user << "] db[" << m_db_name  << "] is_bad["
    << (is_bad_con ? "TRUE" : "FALSE") << "] conn_in_use["
    << m_conns_in_use << "]";
  --m_conns_in_use;

  // @ remove the bad connection from the pool
  if (is_bad_con) {
    mysqlpp::ConnectionPool::remove(pc);
  }

  // @ then notify waited connection in pool is avaiable
  m_conns_in_use_mutex_cond.notify_one();
}

void DatabaseConnectionPool::release(const mysqlpp::Connection* pc) {
  std::unique_lock<std::mutex> lock(m_conns_in_use_mutex);
  mysqlpp::ConnectionPool::release(pc);

  VLOG(100) << "[DatabaseConnectionPool::release] mysql[" << m_db_server << ":"
    << m_db_user << "] db[" << m_db_name  << "] is_bad[FALSE] conn_in_use["
    << m_conns_in_use << "]";
  --m_conns_in_use;
  m_conns_in_use_mutex_cond.notify_one();
}

// @ Superclass overrides
mysqlpp::Connection* DatabaseConnectionPool::create() {
  int32_t db_retry_time = FLAGS_db_retry_time;
  while (db_retry_time--) {
    mysqlpp::Connection *pconnection = NULL;
    try {
      pconnection = new mysqlpp::Connection;
      CHECK(NULL != pconnection);
      pconnection->set_option(
        new mysqlpp::ConnectTimeoutOption(m_db_connect_timeout));
      pconnection->connect(
        m_db_name.c_str(), m_db_server.c_str(),
        m_db_user.c_str(), m_db_passwd.c_str(), m_db_port);

      return pconnection;
    } catch (const std::exception& e) {
      delete pconnection;
      pconnection = NULL;
      LOG(ERROR) << "An exception occurred while connect to database:<"
        << m_db_name << "server:<" << m_db_server << ":" << m_db_port
        << "> with user:<" << m_db_user << ":" << m_db_passwd
        << ">. Exception info: " << e.what() << ". Retry ...";
      continue;
    }
  }

  LOG(ERROR) << "Unable to connect to database:<"
    << m_db_name << "server:<" << m_db_server << ":" << m_db_port
    << "> with user:<" << m_db_user << ":" << m_db_passwd
    << "> after retry:" << FLAGS_db_retry_time << " times.";

  return NULL;
}

void DatabaseConnectionPool::destroy(mysqlpp::Connection* pc) {
  delete pc;
}

unsigned int DatabaseConnectionPool::max_idle_time() {
  // @ Set our idle time at an 120 seconds.  A real
  // @ pool would return some fraction of the server's connection
  // @ idle timeout instead.
  return FLAGS_connection_max_idle_time;
}

}  // namespace utils
