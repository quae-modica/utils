#include "Interface.h"
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace QuaeModicaUtil;

class SocketInterface : public Interface<std::string, std::string> {
public:
  SocketInterface(std::string _ip, std::string _port, bool _isServer = false) :
      ip(_ip), port(_port), isServer(_isServer), _socket(-1), client(-1) {}
  ~SocketInterface() { disconnect(); }
private:
  std::string ip;
  std::string port;
  bool isServer;
  int _socket;
  int client;

  // get sockaddr, IPv4 or IPv6:
  void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
      return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
  }

  bool childConnect() {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    int yes = 1;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (isServer) {
      hints.ai_flags = AI_PASSIVE; // use my IP
      if ((rv = getaddrinfo(NULL, port.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
      }
    } else {
      if ((rv = getaddrinfo(ip.c_str(), port.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
      }
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
      if ((_socket = socket(p->ai_family, p->ai_socktype,
          p->ai_protocol)) == -1) {
        perror("socket");
        continue;
      }

      if (isServer) {
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
            sizeof(int)) == -1) {
          perror("setsockopt");
          exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
          close(sockfd);
          perror("server: bind");
          continue;
        }
      } else {
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
          close(sockfd);
          perror("client: connect");
          continue;
        }
      }

      break;
    }

    if (!isServer) {
      if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
      }

      inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
          s, sizeof s);
      printf("client: connecting to %s\n", s);
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (isServer) {
      if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
      }

      if (listen(sockfd, 10) == -1) {
        perror("listen");
        exit(1);
      }
      struct sockaddr_storage their_addr;
      socklen_t sin_size = sizeof their_addr;
      client = accept(_socket, (struct sockaddr*)&their_addr, &sin_size);
      if (client == -1) {
        perror("accept");
        return false;
      }

      char s[INET6_ADDRSTRLEN];
      inet_ntop(their_addr.ss_family,
          get_in_addr((struct sockaddr *)&their_addr),
          s, sizeof s);
      printf("server: got connection from %s\n", s);
    }
    return true;
  }
  bool childDisconnect() {
    if (isServer) {
      return close(client) == 0 && close(_socket) == 0;
    } else {
      return close(_socket) == 0;
    }
  }
  bool childPing() {
    char data;
    if (isServer && client == -1) {
      return false;
    }
    return recv(_socket, &data, 1, MSG_PEEK) == 1;
  }
  std::string childReceive() {
    char data[1024 * 1024] = { 0 };
    if (recv(isServer ? client : _socket, data, sizeof(data), 0) > 0) {
      return std::string(data);
    }
    return "";
  }
  bool childSend(std::string value) {
    return send(isServer ? client : _socket, value.c_str(), value.length(), 0) == (unsigned int) value.length();
  }
};
