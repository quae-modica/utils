#ifndef __QUAE_MODICA_MESSENGER_H__
#define __QUAE_MODICA_MESSENGER_H__

#include <string>

#include "Socket.h"
#include "Log.h"

namespace QuaeModicaUtil {
class Messenger {
public:
  Messenger() {}
  Messenger(std::string _name, std::string ip, std::string port) {
    name = _name;
    connection.init(Socket::TCP, Socket::CLIENT, ip, port);
  }

  void send(std::string s) {
    if (s == "") {
      return;
    }
    // put the source on the front of the message TODO
//    s = "source=" + name + endl + s;
    connection.send_int(s.length());
    connection.send_data(s.c_str(), s.length());
  }

  std::string recv() {
    int length = connection.recv_int();
    if (length > 0) {
      char message[length];
      connection.recv_data(message, length);
      return message;
    }
    return "";
  }

private:
  Socket connection;
  std::string name;
}; // class Messenger
} // namespace QuaeModicaUtil
#endif // #ifndef __QUAE_MODICA_MESSENGER_H__
