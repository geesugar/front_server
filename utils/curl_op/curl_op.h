/*
 * Copyright [2013-2017] <geesugar>
 * file: curl_op.h
 * author: longtao@geesugar.com
 * date: 2017-12-26
 */
#ifndef UTILS_CURL_OP_CURL_OP_H
#define UTILS_CURL_OP_CURL_OP_H
#include <curl/curl.h>
#include <curl/easy.h>
#include <string>

#include "thirdparty/glog/logging.h"

namespace utils {
enum CONTENT_TYPE {
  JSON_TYPE = 1,
  TEXT_TYPE = 2,
};

class CurlOp {
public:
  /*
  * url: eg(https://www.baidu.com)
  * conn_tm: connection timeout, unit(ms)
  * exec_tm: execution timeout, unit(ms)
  * post_data.empty() ? : "GET method" : "POST method"
  */
  static bool GetHttpReturn(const std::string& url,
    const std::string& post_data, const CONTENT_TYPE& content_type = TEXT_TYPE,
    const int32_t& conn_tm = 0, const int32_t& exec_tm = 0,
    std::string* http_return = NULL);

private:
  CurlOp();
  virtual ~CurlOp();

  static size_t OnHttpData(void *ptr, size_t size, size_t nmemb,
    void *user_data);

private:
};  // class CurlOp
}  // namespace utils
#endif  // UTILS_CURL_OP_CURL_OP_H
