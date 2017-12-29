/*
 * Copyright [2013-2017] <geesugar>
 * file: database_connector.cc
 * author: longtao@geesugar.com
 * date: 2017-12-28
 */
#include "utils/database/database_connector.h"
namespace utils {
// @ gflags database connection related
DEFINE_string(db_name, "", "database name");
DEFINE_string(db_user_name, "", "database user name");
DEFINE_string(db_user_pass, "", "database user password");
DEFINE_string(db_host, "", "database host");
DEFINE_int32(db_port, 0, "connection port");
DEFINE_int32(db_retry_time, 3, "database opreation retry times");

const std::string DatabaseConnector::kUTF8CharSetName = "utf8";
const std::string DatabaseConnector::kGBKCharSetName = "gbk";
const std::string DatabaseConnector::kLATIN1CharSetName = "latin1";

DatabaseConnector::DatabaseConnector(
  const std::string& db_name,
  const std::string& db_user_name,
  const std::string& db_user_pass,
  const std::string& db_host,
  const int32_t& db_port,
  const std::string& db_default_charset,
  const int32_t& connect_timeout) :
  m_db_user_name(db_user_name),
  m_db_user_pass(db_user_pass),
  m_db_host(db_host),
  m_db_port(db_port),
  m_db_default_charset(db_default_charset),
  m_db_connect_timeout(connect_timeout) {
    Init();
  }

DatabaseConnector::~DatabaseConnector() {
}

bool DatabaseConnector::Init() {
  if (m_db_name.empty()) {
    m_db_name      = FLAGS_db_name;
    m_db_user_name = FLAGS_db_user_name;
    m_db_user_pass = FLAGS_db_user_pass;
    m_db_host      = FLAGS_db_host;
    m_db_port      = FLAGS_db_port;
    m_db_default_charset = "";
  }

  LOG(INFO) << "Init database[" << m_db_name << "] connection pool["
    << m_db_user_name << "@" << m_db_host << ":" << m_db_port << "]";

  m_db_cons_pool = std::shared_ptr<DatabaseConnectionPool>(
    new DatabaseConnectionPool(m_db_name, m_db_host, m_db_port,
      m_db_user_name, m_db_user_pass, m_db_default_charset,
      m_db_connect_timeout));

  return true;
}


// @ ExecuteQuery() with the parameter of char_set_name, will retrive data from
// @ db with specified coding style.
// @@@@
// @ NOTE:
// @   ExecuteQuery() with parameter char_set_name will retrive data from database
// @   with specified client connection charset. It do it by following steps:
// @   1. step 1: get the original charset of connection
// @   2. step 2: set client/result charest of connection to specified type
// @   3. step 3: set client/result charset of connection to original charset
bool DatabaseConnector::ExecuteQuery(const std::string& query_str,
  const std::string& char_set_name,
  mysqlpp::StoreQueryResult* query_ret,
  bool* is_db_unreachable) {
  int32_t retry_time = FLAGS_db_retry_time;

  while (retry_time--) {
    mysqlpp::Connection *pcon = m_db_cons_pool->grab();
    if (NULL == pcon) {
      // @ can't create a db connection for several retries via connection
      // @ pool, then break to return false;
      LOG(ERROR) << "can't create a db connection for several retries "
        << "via connection pool.";

      if (NULL != is_db_unreachable) {
        *is_db_unreachable = true;
      }

      break;
    }

    try {
      if (!pcon->connected()) {
        LOG(ERROR) << "database connection is unestablished";

        // @ release to db connection pool and remove it from pool
        m_db_cons_pool->release(pcon, true);
        continue;
      }

      // @ set the conn charset name option to char_set_name
      mysqlpp::Query query = pcon->query();

      // @ step 1: get the original charset of connection
      std::string original_charset;
      query << "show variables like 'character_set_client'";
      mysqlpp::StoreQueryResult charset_res = query.store();
      for (mysqlpp::StoreQueryResult::const_iterator
        it = charset_res.begin();
        it != charset_res.end(); ++it) {
        const mysqlpp::Row row = *it;
        original_charset = row[1].c_str();
      }

      VLOG(100) << "get client charset of connection:<"
        << original_charset << ">.";

      // @ step 2: set client/result charest of connection to specified type
      query << "SET NAMES '" << char_set_name << "'";
      query.execute();
      VLOG(100) << "set client charset of connection to:<"
        << char_set_name << ">.";

      VLOG(100) << query_str;
      query = pcon->query(query_str);
      *query_ret = query.store();

      // @ step 3: set client/result charset of connection to original charset
      // @
      query << "SET NAMES '" << original_charset << "'";
      query.execute();
      VLOG(100) << "set client charset of connection to original:<"
        << char_set_name << ">.";

      // @ release to db connection pool
      m_db_cons_pool->release(pcon, false);

      return true;
    } catch (const std::exception &er) {
      LOG(ERROR) << "An exception occured during database query: ["
        << query_str << "], exception info: "
        << er.what();

      // @ release to db connection pool and remove it from pool
      m_db_cons_pool->release(pcon, true);

      continue;
    }
  }

  return false;
}

bool DatabaseConnector::ExecuteQuery(const std::string& query_str,
  mysqlpp::StoreQueryResult* query_ret,
  bool* is_db_unreachable) {
  int32_t retry_time = FLAGS_db_retry_time;

  while (retry_time--) {
    // @ grab connection
    mysqlpp::Connection *pcon = m_db_cons_pool->grab();
    if (NULL == pcon) {
      // @ can't create a db connection for several retries via connection
      // @ pool, then break to return false;
      LOG(ERROR) << "can't create a db connection for several retries "
        << "via connection pool.";

      if (NULL != is_db_unreachable) {
        *is_db_unreachable = true;
      }

      break;
    }

    try {
      if (!pcon->connected()) {
        LOG(ERROR) << "database connection is unestablished";

        // @ release to db connection pool and remove it from pool
        m_db_cons_pool->release(pcon, true);
        continue;
      }

      VLOG(100) << query_str;
      mysqlpp::Query query = pcon->query(query_str);
      *query_ret = query.store();

      // @ release to db connection pool
      m_db_cons_pool->release(pcon, false);

      return true;
    } catch (const std::exception &er) {
      LOG(ERROR) << "An exception occured during database query: ["
        << query_str << "], exception info: " << er.what();

      // @ release to db connection pool and remove it from pool
      m_db_cons_pool->release(pcon, true);

      continue;
    }
  }

  return false;
}

bool DatabaseConnector::Execute(const std::string& sql_str,
  bool* is_db_unreachable,
  mysqlpp::SimpleResult* simple_ret) {
  int32_t retry_time = FLAGS_db_retry_time;

  while (retry_time--) {
    // @ grab connection
    mysqlpp::Connection *pcon = m_db_cons_pool->grab();
    if (NULL == pcon) {
      // @ can't create a db connection for several retries via connection
      // @ pool, then break to return false;
      LOG(ERROR) << "can't create a db connection for several retries "
        << "via connection pool.";

      if (NULL != is_db_unreachable) {
        *is_db_unreachable = true;
      }

      break;
    }

    try {
      if (!pcon->connected()) {
        LOG(ERROR) << "database connection is unestablished.";

        // @ release to db connection pool and remove it from pool
        m_db_cons_pool->release(pcon, true);
        continue;
      }

      mysqlpp::Query query = pcon->query(sql_str);
      VLOG(100) << "Execute an sql statement:" << sql_str;
      mysqlpp::SimpleResult ret = query.execute();

      if (ret) {
        // @ release to db connection pool
        m_db_cons_pool->release(pcon, false);

        if (NULL != simple_ret) {
          *simple_ret = ret;
        }
        return true;
      } else {
        LOG(ERROR) << "the insert operation:[" << sql_str
          << "] is failed. retry ...";

        // @ release to db connection pool and remove it from pool
        m_db_cons_pool->release(pcon, true);
        continue;
      }
    } catch (const std::exception &er) {
      LOG(ERROR) << "An exception is occurred during databse insert operation:["
        << sql_str << "], exception info: " << er.what()
        << ". Retry ...";

      // @ release to db connection pool and remove it from pool
      m_db_cons_pool->release(pcon, true);
      continue;
    }
  }

  return false;
}

bool DatabaseConnector::Escape(std::string *sql_str,
  bool *is_db_unreachable) {
  int32_t retry_time = FLAGS_db_retry_time;

  while (retry_time--) {
    // @ grab connection
    mysqlpp::Connection *pcon = m_db_cons_pool->grab();
    if (NULL == pcon) {
      // @ can't create a db connection for several retries via connection
      // @ pool, then break to return false;
      LOG(ERROR) << "can't create a db connection for several retries "
        << "via connection pool.";

      if (NULL != is_db_unreachable) {
        *is_db_unreachable = true;
      }

      break;
    }

    try {
      if (!pcon->connected()) {
        LOG(ERROR) << "database connection is unestablished.";

        // @ release to db connection pool and remove it from pool
        m_db_cons_pool->release(pcon, true);
        continue;
      }

      mysqlpp::Query query = pcon->query();
      VLOG(100) << "Escape an sql statement:" << *sql_str;

      // @ release to db connection pool
      m_db_cons_pool->release(pcon, false);
      return true;
    } catch (const std::exception &er) {
      LOG(ERROR) << "An exception is occurred during databse escape operation:["
        << *sql_str << "], exception info: " << er.what()
        << ". Retry ...";

      // @ release to db connection pool and remove it from pool
      m_db_cons_pool->release(pcon, true);
      continue;
    }
  }

  return false;
}

}  // namespace utils
