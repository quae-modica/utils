#ifndef __QUAE_MODICA_MANAGER_H__
#define __QUAE_MODICA_MANAGER_H__

#include <pthread.h>
#include <string>

#include "Log.h"
#include "Properties.h"
#include "Messenger.h"

namespace QuaeModicaUtil {
class Manager {
public:
  /**
   * Create a Manager, reading in a properties file
   * Expected Properties:
   *   # used for pub-sub messaging
   *   name=[app name]
   * messaging_host=[publisher hostname]
   * messaging_port=[publisher port]
   */
  Manager(std::string properties_file, void (*_callback)(std::string)) {
    LOG(DEBUG, "Starting Manager Using File '%s'", properties_file.c_str());

    // save the callbac function
    callback = _callback;

    // load properties from a file
    properties = new Properties();
    properties->from_file(properties_file);

    // subscribe to the publisher
    messenger = new Messenger(properties->get_string("name"),
                              properties->get_string("messaging_host"),
                              properties->get_string("messaging_port"));
    
    // start the receiver thread
    pthread_create(&receiver_thread, NULL, receive, this);
  }

private:
  Properties *properties;
  Messenger *messenger;
  pthread_t receiver_thread;
  void (*callback)(std::string);

  static void* receive(void *v) {
    Manager *manager = (Manager*) v;
    std::string message;

    // forever, try to receive messages
    for (;;) {
      message = manager->messenger->recv();
      // if there was a message, use the callback
      if (message != "") {
        manager->callback(message);
      } else {
        // if no message received, sleep for 1 second
        sleep(1);
      }
    }
    return NULL;
  }
}; // class Manager
} // namespace QuaeModicaUtil

#endif // #ifndef __QUAE_MODICA_MANAGER_H__
