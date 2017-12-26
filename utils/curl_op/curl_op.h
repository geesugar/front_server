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

#include "thirdparty/glog/logging.h"

namespace utils {
class CurlOp {
public:
  /*
  * url: eg(https://www.baidu.com)
  * conn_tm: connection timeout, unit(ms)
  * exec_tm: execution timeout, unit(ms)
  * post_data.empty() ? : "GET method" : "POST method"
  */
  static bool GetHttpReturn(const std::string& url,
    const std::string& post_data, const int32_t& conn_tm,
    const int32_t& exec_tm, std::string* http_return);

 private:
  CurlOp();
  virtual ~CurlOp();

  static size_t OnHttpData(void *ptr, size_t size, size_t nmemb, 
    void *user_data);

 private:
};  // class CurlOp
}  // namespace utils
#endif  // UTILS_CURL_OP_CURL_OP_publicH
