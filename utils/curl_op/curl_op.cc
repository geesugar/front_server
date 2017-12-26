/*
 * Copyright [2013-2017] <geesugar>
 * file: curl_op.cc
 * author: longtao@geesugar.com
 * date: 2017-12-26
 */
#include "utils/curl_op/curl_op.h"
namespace utils {
struct Buffer {
  char* data = NULL;
  int32_t len = 0;
};

CurlOp::CurlOp() {
}

CurlOp::~CurlOp() {
}

bool CurlOp::GetHttpReturn(const std::string& url,
  const std::string& post_data, const int32_t& conn_tm,
  const int32_t& exec_tm, std::string* http_return) {
  CURL *curl_client = curl_easy_init();
  if (!curl_client) {
    LOG(ERROR) << __func__ << " curl_easy_init() failed";
    return false;
  }

  Buffer buffer;
  buffer.data = NULL;
  buffer.len = 0;

  curl_easy_setopt(curl_client, CURLOPT_URL, url.data());
  curl_easy_setopt(curl_client, CURLOPT_WRITEFUNCTION,
    &CurlOp::OnHttpData);
  curl_easy_setopt(curl_client, CURLOPT_WRITEDATA, &buffer);
  curl_easy_setopt(curl_client, CURLOPT_NOSIGNAL, 1L);

  if (conn_tm > 0)
    curl_easy_setopt(curl_client, CURLOPT_CONNECTTIMEOUT_MS, conn_tm);

  if (exec_tm > 0)
    curl_easy_setopt(curl_client, CURLOPT_TIMEOUT_MS, exec_tm);

  if (!post_data.empty()) {
    curl_easy_setopt(curl_client, CURLOPT_POST, 1);
    curl_easy_setopt(curl_client, CURLOPT_POSTFIELDS, post_data.data());
  }

  CURLcode ret_code = curl_easy_perform(curl_client);

  if (buffer.data) {
    if (http_return) *http_return = buffer.data;
    delete[] buffer.data;
  }
  curl_easy_cleanup(curl_client);

  if (CURLE_OK == ret_code) {
    VLOG(200) << __func__ << " curl url[" << url << "] succ. return["
      << *http_return << "]";
    return true;
  }
  LOG(ERROR) << __func__ << " code[" << ret_code << "] url[" << url
    << "] post[" << post_data << "]";
  return false;
}

size_t CurlOp::OnHttpData(void *ptr, size_t size, size_t nmemb,
  void *user_data) {
  if (!ptr || !user_data) {
    LOG(ERROR) << __func__ << " error. ptr[" << ptr
      << "] user_data[" << user_data << "]";
    return 0;
  }
  Buffer* buffer = static_cast<Buffer*>(user_data);
  char* recv_data = static_cast<char*>(ptr);
  if (!buffer->data) {
    buffer->data = new char[size * nmemb + 1];
    buffer->data[size * nmemb] = '\0';
    buffer->len = size * nmemb;
    strncpy(buffer->data, recv_data, size * nmemb);
  } else {
    char* old_data = buffer->data;
    buffer->data = new char[buffer->len + size * nmemb + 1];
    buffer->data[size * nmemb] = '\0';
    strncpy(buffer->data, old_data, buffer->len);
    strncpy(buffer->data + buffer->len, recv_data, size * nmemb);
    buffer->len += size * nmemb;
    delete[] old_data;
  }
  return size * nmemb;
}
}  // namespace utils
