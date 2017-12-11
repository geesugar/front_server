/*
 * Copyright [2013-2017] <geesugar>
 * file: server.cc
 * author: longtao@geesugar.com
 * date: 2017-11-17
 */
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "server/src/config.h"
#include "server/src/defines.h"
#include "server/src/helper.h"
#include "server/src/server.h"

namespace Front { namespace Server {
Server::Server() : m_base(NULL), m_listener(NULL), m_signal_event(NULL) {
  // CHECK(InitServer()) << "init server failed.";
  CHECK(InitEventServer()) << "init event server failed.";
}

Server::~Server() {
  if (m_listener) evconnlistener_free(m_listener);
  if (m_signal_event) event_free(m_signal_event);
  if (m_base) event_base_free(m_base);
}

event_base* Server::GetBase() {
  return m_base;
}

void Server::ConnReadCallBack(struct bufferevent *bev, void *user_data) {
  char msg[1024] = {0};
  bufferevent_read(bev, msg, sizeof(msg));
  LOG(INFO) << FUNC_NAME << GetAddressBySocket(bev->ev_read.ev_fd) << " ECHO: " << msg;
  bufferevent_write(bev, msg, sizeof(msg));
}

void Server::ConnWriteCallBack(struct bufferevent *bev, void *user_data) {
}

void Server::ConnEventCallBack(struct bufferevent *bev, int16_t events,
  void *user_data) {
  if (events & BEV_EVENT_EOF) {
    GetAddressBySocket(bev->ev_read.ev_fd);
    LOG(INFO) << FUNC_NAME << "Connection closed. peer host["
      << GetAddressBySocket(bev->ev_read.ev_fd) << "]";
  } else if (events & BEV_EVENT_ERROR) {
    LOG(INFO) << FUNC_NAME << "Got an error on the connection:" << strerror(errno);
  }
  bufferevent_free(bev);
}

bool Server::InitEventServer() {
  struct sockaddr_in sin;

  m_base = event_base_new();
  if (!m_base) {
    LOG(ERROR) << FUNC_NAME << "could not initialize libevent";
    return false;
  }

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(FLAGS_port);

  m_listener = evconnlistener_new_bind(m_base, Server::ListenerCallBack,
    static_cast<void*>(this), LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
    (struct sockaddr*)&sin, sizeof(sin));
  if (!m_listener) {
    LOG(ERROR) << FUNC_NAME << "could not create a listener";
    return false;
  }
  LOG(INFO) << FUNC_NAME << "server start listen port[" << FLAGS_port << "]";

  m_signal_event = evsignal_new(m_base, SIGINT, Server::SignalCallBack,
    static_cast<void*>(this));
  if (!m_signal_event || event_add(m_signal_event, NULL) < 0) {
    LOG(ERROR) << FUNC_NAME << "could not create/add a signal event";
    return false;
  }

  event_base_dispatch(m_base);
  return true;
}

void Server::SignalCallBack(evutil_socket_t sig,
  int16_t events, void* user_data) {
  Server* server = static_cast<Server*>(user_data);
  struct timeval delay = { 2, 0 };
  LOG(INFO) << FUNC_NAME
    << "Caught an interrupt signal, exiting cleanly in two seconds";
  event_base_loopexit(server->GetBase(), &delay);
}

void Server::ListenerCallBack(struct evconnlistener *listener,
  evutil_socket_t fd, struct sockaddr *sa, int socklen, void *user_data) {
  Server* server = static_cast<Server*>(user_data);
  struct bufferevent* bev;
  bev = bufferevent_socket_new(server->GetBase(), fd, BEV_OPT_CLOSE_ON_FREE);
  if (!bev) {
    LOG(INFO) << FUNC_NAME << "error constructing buferevent";
    return;
  }
  bufferevent_setcb(bev, Server::ConnReadCallBack,
    Server::ConnWriteCallBack,
    Server::ConnEventCallBack, NULL);
  bufferevent_enable(bev, EV_WRITE);
  bufferevent_enable(bev, EV_READ);
}

bool Server::InitServer() {
  int32_t listen_sock = -1;
  if (!CreateSocket(&listen_sock)) {
    LOG(ERROR) << FUNC_NAME << "create socket failed.";
    return false;
  }

  if (!SetNonblocking(listen_sock)) {
    LOG(ERROR) << FUNC_NAME << "set listen_sock[" << listen_sock
      << "] noblocking failed.";
    return false;
  }

  int32_t epollfd = epoll_create(1);
  if (-1 == epollfd) {
    LOG(ERROR) << FUNC_NAME << "epoll_create failed. error["
      << strerror(errno) << "]";
    return false;
  }

  struct epoll_event ev;
  ev.events = EPOLLIN | EPOLLET;
  ev.data.fd = listen_sock;
  if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev)) {
    LOG(ERROR) << FUNC_NAME << "epoll_ctl add listen_sock[" << listen_sock
      << "] failed. error[" << strerror(errno) << "]";
    return false;
  }

  if (!Listen(listen_sock)) {
    LOG(ERROR) << FUNC_NAME << "server listen failed.";
    return false;
  }

  pid_t pid = getpid();
  LOG(INFO) << FUNC_NAME << "start server. pid[" << pid << "] listen port["
    << FLAGS_port << "]";

  struct epoll_event events[MAX_EVENTS];
  while (true) {
    int32_t nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    for (int32_t i = 0; i < nfds; ++i) {
      if (events[i].data.fd == listen_sock) {
        struct sockaddr client_addr;
        socklen_t addrlen;
        int32_t conn_fd = accept(listen_sock, &client_addr, &addrlen);
        if (conn_fd < 0) {
          LOG(ERROR) << FUNC_NAME << "accept socket failed.";
          continue;
        }

        if (!SetNonblocking(conn_fd)) {
          LOG(ERROR) << FUNC_NAME << "set conn_fd[" << conn_fd
            << "] nonblocking failed.";
          continue;
        }

        ev.data.fd = conn_fd;
        ev.events = EPOLLIN | EPOLLET;
        if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_fd, &ev)) {
          LOG(ERROR) << FUNC_NAME << "add conn_fd[" << conn_fd
            << "] to epoll listen set failed.";
          continue;
        }
      } else if (events[i].events & EPOLLIN) {
        int32_t sock_fd = events[i].data.fd;
        if (sock_fd < 0) {
          LOG(ERROR) << FUNC_NAME << "read sock illegal.";
          continue;
        }

        int32_t size = 0;
        char buffer[BUFFER_SIZE] = {0};
        while ((size = read(sock_fd, &buffer, BUFFER_SIZE)) > 0) {
          LOG(INFO) << FUNC_NAME << "recv:" << buffer;
          memset(buffer, 0, BUFFER_SIZE);
        }
      } else if (events[i].events & EPOLLOUT) {
        int32_t sock_fd = events[i].data.fd;
        if (sock_fd < 0) {
          LOG(ERROR) << FUNC_NAME << "write sock illegal.";
          continue;
        }
      }
    }
  }

  LOG(INFO) << FUNC_NAME << "exit server";
  return false;
}

bool Server::CreateSocket(int32_t* fd) {
  int32_t sock = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == sock) {
    LOG(ERROR) << FUNC_NAME << "create socket failed. error["
      << strerror(errno) << "]";
    return false;
  }

  if (!SetReuseAddr(sock)) {
    LOG(ERROR) << FUNC_NAME << "set socket reuse address failed.";
    return false;
  }

  *fd = sock;
  return true;
}

/*
 * DESC: The SO_REUSEADDR socket option allows a socket to forcibly
 *       bind to a port in use by another socket.
 */
bool Server::SetReuseAddr(const int32_t& fd) {
  int32_t yes = 1;
  if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))) {
    LOG(ERROR) << FUNC_NAME << "setsockopt SO_REUSEADDR failed. error["
      << strerror(errno) << "]";
    return false;
  }
  return true;
}

bool Server::Listen(const int32_t& sock) {
  // bind
  struct sockaddr_in sock_addr;
  memset(&sock_addr, 0, sizeof(sock_addr));
  sock_addr.sin_family = AF_INET;
  sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  sock_addr.sin_port = htons(FLAGS_port);
  if (-1 == bind(sock, (struct sockaddr*)&sock_addr, sizeof(struct sockaddr))) {
    LOG(ERROR) << FUNC_NAME << "bind sock[" << sock << "] on[INADDR_ANY:"
      << FLAGS_port << "] failed. error[" << strerror(errno) << "]";
    return false;
  }

  // listen
  if (-1 == listen(sock, FLAGS_tcp_backlog)) {
    LOG(ERROR) << FUNC_NAME << "listen error. error[" << strerror(errno) << "]";
    return false;
  }
  return true;
}

bool Server::SetNonblocking(const int32_t& fd) {
  int32_t opts;
  opts = fcntl(fd, F_GETFL);
  if (opts < 0) {
    LOG(ERROR) << FUNC_NAME << "get fd[" << fd << "] options failed.";
    return false;
  }
  opts = opts | O_NONBLOCK;
  if (-1 == fcntl(fd, F_SETFL, opts)) {
    LOG(ERROR) << FUNC_NAME << "set fd[" << fd << "] nonblocking failed. opts["
      << opts << "]";
    return false;
  }
  return true;
}

}  // namespace Server
}  // namespace Front
