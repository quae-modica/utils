/*
improvements:
-enable UDP
-verify all inputs are valid
-look into appropriate socket options
-look into enabling RAW sockets
*/
#ifndef __QUAE_MODICA_SOCKET_H__
#define __QUAE_MODICA_SOCKET_H__

#define SERVER_LISTEN_COUNT 100

#include <string>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>

#include "Log.h"

namespace QuaeModicaUtil {
class Socket {
public:
  enum PROTOCOL { DEFAULT, TCP, UDP, RAW };
  enum ROLE { DEFAULT_ROLE, CLIENT, SERVER, CONNECTION };

  Socket() : socket_descriptor(-1), protocol(DEFAULT), role(DEFAULT_ROLE), address(""), port(""), server(NULL) {}

  int init(PROTOCOL _protocol, ROLE _role, std::string _address, std::string _port) {
    if (socket_descriptor != -1) {
      LOG(ERROR, "Using Initialized Socket");
      return -1;
    }

    // store the passed in parameters
    protocol = _protocol;
    role = _role;
    address = _address;
    port = _port;

    // setup for getaddrinfo() call
    struct addrinfo hints, *servinfo, *p;
    int status;

    // TODO switch statement protocol and role strings
    LOG(DEBUG, "Creating %s With Address = '%s', Port = '%s'",
        role == SERVER ? "Server" : "Client", address.c_str(), port.c_str());

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
    hints.ai_socktype = protocol == TCP ? SOCK_STREAM : SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // fill in IP address for me

    // getaddrinfo
    if ((status = getaddrinfo(address.c_str(), port.c_str(), &hints, &servinfo)) != 0) {
      LOG(ERROR, "Socket getaddrinfo() Error: %s", gai_strerror(status));
      return -1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
      // socket
      if ((socket_descriptor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
        LOG(ERROR, "Socket socket() Error: %s", strerror(errno));
        continue;
      }

      // TODO setsockopt
      int yes = 1;
      if (setsockopt(socket_descriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("Socket setsockopt( Error");
        continue;
      }

      // if server, bind
      if (role == SERVER && bind(socket_descriptor, p->ai_addr, p->ai_addrlen) == -1) {
        close(socket_descriptor);
        LOG(ERROR, "Socket bind() Error: %s", strerror(errno));
        continue;
      }

      // if server, listen
      if (role == SERVER && listen(socket_descriptor, SERVER_LISTEN_COUNT) == -1) {
        close(socket_descriptor);
        LOG(ERROR, "Socket listen() Error: %s", strerror(errno));
        continue;
      }

      // if TCP client, connect
      if ((protocol == TCP && role == CLIENT) && connect(socket_descriptor, p->ai_addr, p->ai_addrlen) == -1) {
        close(socket_descriptor);
        LOG(ERROR, "Socket connect() Error: %s", strerror(errno));
        continue;
      }

      break;
    }

    // freeaddrinfo
    freeaddrinfo(servinfo);

    // if we couldn't use any of the addresses
    if (p == NULL) {
      return -1;
    }

    LOG(DEBUG, "Created Socket With Address: '%s', Port: '%s', Descriptor: %d",
        _address.c_str(), _port.c_str(), socket_descriptor);
    return 0;
  }

  int init(PROTOCOL _protocol, ROLE _role, Socket* _server) {
    // if this socket is already initialized and not closed
    if (socket_descriptor != -1) {
      LOG(ERROR, "Using initialized Socket");
      return -1;
    }

    // store passed in parameters
    protocol = _protocol;
    role = _role;
    server = _server;

    // if this is a TCP connection
    if (protocol == TCP && role == CONNECTION) {
      // if the server has a valid socket and is a TCP SERVER
      if (_server->socket_descriptor != -1 &&
          _server->protocol == TCP && _server->role == SERVER) {
        socket_descriptor = accept(_server->socket_descriptor, NULL, NULL);
      } else {
        LOG(ERROR, "Error Creating Server Connection: Invalid Server");
        return -1;
      }

      if (socket_descriptor == -1) {
        LOG(ERROR, "Error Creating Connection: Accept Call Failed: %s",
            strerror(errno));
        return -1;
      }
    } else {
      LOG(ERROR, "Error Creating Connection: not a TCP CONNECTION");
      return -1;
    }
    return 0;
  }

  virtual ~Socket() {
    LOG(DEBUG, "Calling ~Socket()");
    stop_socket();
  }

  /**
   * send or receive some information
   */
  int send_byte(char data) {
    return send_data((void*) &data, sizeof(data)) == sizeof(data);
  }
  int send_short(short data) {
    return send_data((void*) &data, sizeof(data)) == sizeof(data);
  }
  int send_int(int data) {
    return send_data((void*) &data, sizeof(data)) == sizeof(data);
  }
  char recv_byte() {
    char data;
    recv_data((void*) &data, sizeof(data));
    return data;
  }
  char recv_short() {
    short data;
    recv_data((void*) &data, sizeof(data));
    return data;
  }
  char recv_int() {
    int data;
    recv_data((void*) &data, sizeof(data));
    return data;
  }

  int send_data(const void* data, int size) {
    // if this socket is not setup correctly or stopped, return error
    if (socket_descriptor == -1) {
      LOG(ERROR, "Cannot send on uninitialized Socket");
      return -1;
    }

    int cur = 0, send_cnt = 0;
    do {
      cur = send(socket_descriptor, (void*) (((long long int) data) + send_cnt), size - send_cnt, 0);
    } while (cur > 0 && ((send_cnt += cur) < size));
    return cur < 0 ? cur : send_cnt;
  }

  int recv_data(const void* data, int size) {
    // if this socket is not setup correctly or stopped, return error
    if (socket_descriptor == -1) {
      LOG(ERROR, "Cannot recv on uninitialized Socket");
      return -1;
    }

    int cur = 0, recv_cnt = 0;
    do {
      cur = recv(socket_descriptor, (void*) (((long long int) data) + recv_cnt), size - recv_cnt, 0);
    } while (cur > 0 && ((recv_cnt += cur) < size));
    return cur < 0 ? cur : recv_cnt;
  }

  int stop_socket() {
    if (socket_descriptor == -1) {
      return -1;
    }
    std::string role_string, protocol_string;
    switch(role) {
      case CLIENT:
        role_string = "CLIENT";
        break;
      case SERVER:
        role_string = "SERVER";
        break;
      case CONNECTION:
        role_string = "CCONNECTIONLIENT";
        break;
      default:
        role_string = "UNKNOWN";
        break;
    }
    switch (protocol) {
      case TCP:
        protocol_string = "TCP";
        break;
      case UDP:
        protocol_string = "UDP";
        break;
      case RAW:
        protocol_string = "RAW";
        break;
      default:
        protocol_string = "UNKNOWN";
        break;
    }
    LOG(DEBUG, "Soppting %s %s @ %s:%s", protocol_string.c_str(), role_string.c_str(), address.c_str(), port.c_str());

    close(socket_descriptor);
    return 0;
  }

  int restart_socket() {
    stop_socket();
    if (role == CONNECTION) {
      return init(protocol, role, server);
    } else {
      return init(protocol, role, address, port);
    }
  }

  int is_valid() {
    return socket_descriptor != -1;
  }

private:
  int socket_descriptor;
  PROTOCOL protocol;
  ROLE role;
  std::string address;
  std::string port;
  Socket *server;
}; // class Socket
} // namespace QuaeModicaUtil
#endif // #ifndef __QUAE_MODICA_SOCKET_H__
