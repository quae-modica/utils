#include <pthread.h>

#include "Log.h"
#include "Time.h"
#include "Properties.h"
#include "Messenger.h"
#include "Manager.h"

using namespace QuaeModicaUtil;

Properties properties;
Socket server;

void test_callback(std::string s) {
  LOG(INFO, "Test Callback: %s", s.c_str());
}

void test_time() {
  LOG(INFO, "Test Time");
}

void test_log() {
  LOG(INFO, "Test Log");
  LOG(INFO, "Test Log INFO");
  LOG(WARN, "Test Log WARN");
  LOG(DEBUG, "Test Log DEBUG");
  LOG(ERROR, "Test Log ERROR");
  LOG(STATS, "Test Log STATS");
}

void test_properties() {
  LOG(INFO, "Test Properties");
  properties.from_file("props.txt");
  LOG(INFO, "Properties Name: %s", properties.get_string("name").c_str());
  LOG(INFO, "Messaging Host: %s", properties.get_string("messaging_host").c_str());
  LOG(INFO, "Messaging Port: %s", properties.get_string("messaging_port").c_str());
  LOG(INFO, "Messaging Port: %d", properties.get_int("messaging_port"));
  LOG(INFO, "Test Bool: %s", properties.get_string("properties_bool").c_str());
  LOG(INFO, "Test Bool: %s", properties.get_bool("properties_bool") ? "true" : "false");
  LOG(INFO, "Test Float: %s", properties.get_string("properties_float").c_str());
  LOG(INFO, "Test Float: %g", properties.get_float("properties_float"));
}

void* echo_loop(void *v) {
  Socket *s = (Socket*) v;
  Socket connection;
  connection.init(Socket::TCP, Socket::CONNECTION, s);
  int size;
  char message[65536];
  while (true) {
    if (!connection.is_valid()) {
      connection.init(Socket::TCP, Socket::CONNECTION, s);
      continue;
    }
    size = connection.recv_int();
    connection.recv_data(message, size);
    LOG(INFO, "Server Received Message: '%s'", message);
    connection.send_int(size);
    connection.send_data(message, size);
  }
  return NULL;
}

void test_socket() {
  LOG(INFO, "Test Socket");
  server.init(Socket::TCP, Socket::SERVER, properties.get_string("messaging_host"), properties.get_string("messaging_port"));
  pthread_t echo_thread;
  pthread_create(&echo_thread, NULL, echo_loop, &server);
  sleep(1);

  Socket client;
  client.init(Socket::TCP, Socket::CLIENT, properties.get_string("messaging_host"), properties.get_string("messaging_port"));

  std::string s = "Test Message, Please Ignore";
  client.send_int(s.size());
  client.send_data(s.c_str(), s.size());
  sleep(1);

  int size = client.recv_int();
  char message[65536];
  client.recv_data(message, size);
  LOG(INFO, "Client Received Message: '%s'", message);
}

void test_messenger() {
  LOG(INFO, "Test Messenger");

  // subscribe to the publisher
  Messenger m("Messenger Test",
      properties.get_string("messaging_host"),
      properties.get_string("messaging_port"));
  
  // send the publisher a message
  m.send("Test Message, Please Ignore");

  Socket conn;
  conn.init(Socket::TCP, Socket::CONNECTION, &server);

  // try to receive the message
  char message[1024];
  int status = conn.recv_data(&message, sizeof(message));
  LOG(INFO, "Received Message (%d): '%s'", status, message);
}

void test_manager() {
  LOG(INFO, "Test Manager");
  Manager m("props.txt", test_callback);
}

int main() {
  SET_LOG_TYPE(DEBUG, true);
  test_time();
  LOG(DEBUG, "**********");
  test_log();
  LOG(DEBUG, "**********");
  test_properties();
  LOG(DEBUG, "**********");
  test_socket();
  LOG(DEBUG, "**********");
  test_manager();
  LOG(DEBUG, "**********");

  return 0;
}
