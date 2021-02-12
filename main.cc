#include "SocketInterface.h"
#include "Log.h"
using namespace QuaeModicaUtil;

void serverTest() {
  SocketInterface server("localhost", "24601", true);
  std::string response = server.receive();
  LOG(DEBUG, "server received: %s", response.c_str());
}

void clientTest() {
  SocketInterface client("localhost", "24601");
  std::string message = "I am the very model of a modern major general";
  LOG(DEBUG, "client sending:  %s", message.c_str());
  client.send(message);
}

int main(int argc, char **argv) {
  (void) argc;
  (void) argv;
  SET_LOG(DEBUG, true);
  if (fork() == 0) {
    LOG(DEBUG, "starting server test...");
    serverTest();
  }
  if (fork() == 0) {
    LOG(DEBUG, "starting client test...");
    clientTest();
  }
  return 0;
}
