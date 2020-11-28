#include "SocketInterface.h"
using namespace QuaeModicaUtil;

int main(int argc, char **argv) {
  (void) argc;
  (void) argv;
  SocketInterface server("localhost", "24601", true);
  SocketInterface client("localhost", "24601");

  std::string message = "I am the very model of a modern major general";
  client.send(message);
  std::string response = server.receive();
  std::cout << message << std::endl;
  std::cout << response << std::endl;
  return 0;
}
