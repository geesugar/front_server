/*
 * Copyright [2013-2017] <geesugar>
 * file: server.h
 * author: longtao@geesugar.com
 * date: 2017-11-17
 */
#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H


#include <thirdparty/event2/buffer.h>
#include <thirdparty/event2/bufferevent.h>
#include <thirdparty/event2/bufferevent_struct.h>
#include <thirdparty/event2/event.h>
#include <thirdparty/event2/listener.h>
#include <thirdparty/event2/util.h>

namespace Front { namespace Server {
class Server {
public:
  Server();
  virtual ~Server();
  // add your public interface here:

private:
  bool InitServer();
  bool InitEventServer();

  static void ListenerCallBack(struct evconnlistener *listener,
    evutil_socket_t fd, struct sockaddr *sa,
    int socklen, void *user_data);
  static void SignalCallBack(evutil_socket_t sig, int16_t events,
    void* user_data);
  static void ConnReadCallBack(struct bufferevent *bev, void *user_data);
  static void ConnWriteCallBack(struct bufferevent *bev, void *user_data);
  static void ConnEventCallBack(struct bufferevent *bev, int16_t events,
    void *user_data);

  bool CreateSocket(int32_t* fd);
  bool SetReuseAddr(const int32_t& fd);
  bool Listen(const int32_t& sock);
  bool SetNonblocking(const int32_t& fd);

  event_base* GetBase();

private:
  struct event_base*              m_base;
  struct evconnlistener*          m_listener;
  struct event*                   m_signal_event;
};  // class Server
}  // namespace Server
}  // namespace Front
#endif  // SERVER_SERVER_H
