#ifndef __QUAE_MODICA_LOG_H__
#define __QUAE_MODICA_LOG_H__

#define LOG(X,Y...) Log::msg(Log::X,__FILE__,__LINE__,Y)
#define SET_LOG_TYPE(X,Y) Log::set_log_type(Log::X,Y)
#define SET_LOG_FILE(X) Log::set_log_file(X)
#define LOG_STRING_SIZE 1024
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "Time.h"

namespace QuaeModicaUtil {
/**
 * Represents a singleton logger.  All logging should be done via the LOG() macro
 * to eliminate the need to manually use __FILE__ and __LINE__.  Likewise,
 * toggling of specific message types should be set using the SET_LOG_TYPE()
 * macro to prevent having to use class namespace.
 * 
 * Example Usage:
 *      Log(ERROR, "Error Message %d", 24601);
 * Outputs Something Like:
 *      2013-11-06 21:31:29.123 (ERROR  log_test.cpp:16) Error Message 24601
 */
class Log {
public:
  // enum containing all types of messages to log.  TYPE_SIZE must be last.
  enum TYPE { ERROR, WARN, INFO, DEBUG, STATS, TYPE_SIZE };

  /**
   * Toggle a specific type of logging on or off.  Should be accessed via macro
   * at beginning of file.
   * 
   * @param type the type of Logging to turn on or off
   * @param value the value to set the type of logging to
   */
  static void set_log_type(TYPE type, bool value) {
    // create a string for the message type
    char type_string[8];
    switch(type) {
      case ERROR:
        strncpy(type_string, "ERROR", sizeof(type_string));
        break;
      case WARN:
        strncpy(type_string, "WARN", sizeof(type_string));
        break;
      case INFO:
        strncpy(type_string, "INFO", sizeof(type_string));
        break;
      case DEBUG:
        strncpy(type_string, "DEBUG", sizeof(type_string));
        break;
      case STATS:
        strncpy(type_string, "STATS", sizeof(type_string));
        break;
      default:
        strncpy(type_string, "UNKWN", sizeof(type_string));
        break;
    }

    LOG(DEBUG, "%s %s Log Messages", value ? "Enabling" : "Disabling", type_string);

    Log::get_instance().bools[type] = value;
  }

  /**
   * set the file to store log messages into.
   * 
   * @param filename the new file to log messages to
   */
  static void set_log_file(const char* filename) {
    // attempt to open the file supplied
    FILE *f = fopen(filename, "w");
    if (f != NULL) {
      LOG(DEBUG, "Switching Log File To '%s'", filename);
      Log::get_instance().file = f;
    }
  }

  /**
   * Log various message types.  Should be accessed via macro at beginning of file.
   * 
   * @param type the type of message to log
   * @param source the source file it occurred in
   * @param line the line number it occurred on
   * @param msg a printf style string
   * @param ... any trailing variables for vsprintf to use
   */
  static void msg(TYPE type, const char *source, int line, const char *msg, ...) {
    Log log = Log::get_instance();

    // if we are not logging this type of message, return
    if (!log.bools[type]) {
      return;
    }

    // create a va_list of the trailing variables and log the message
    va_list args;
    va_start(args, msg);

    // create a string for the message type
    char type_string[8];
    switch(type) {
      case ERROR:
        strncpy(type_string, "ERROR", sizeof(type_string));
        break;
      case WARN:
        strncpy(type_string, "WARN", sizeof(type_string));
        break;
      case INFO:
        strncpy(type_string, "INFO", sizeof(type_string));
        break;
      case DEBUG:
        strncpy(type_string, "DEBUG", sizeof(type_string));
        break;
      case STATS:
        strncpy(type_string, "STATS", sizeof(type_string));
        break;
      default:
        strncpy(type_string, "UNKWN", sizeof(type_string));
        break;
    }

    // strip the source of its path
    while (strstr(source, "/") != NULL) {
      source = strstr(source, "/") + 1;
    }

    // lock mutex on log string
    pthread_mutex_lock(&(log.mutex));

    // create a combined date and time string in ISO 8601 format
    sprintf(log.log_string, "%s", Time::now().to_string().c_str());

    // record the passed in values for type of message, source file, and line number
    sprintf(log.log_string + strlen(log.log_string), " [%s  %s:%d] ", type_string, source, line);

    // record the passed in variables and append a newline
    vsprintf(log.log_string + strlen(log.log_string), msg, args);
    strcpy(log.log_string + strlen(log.log_string), "\n");

    // log the generated message and free the va_list
    fprintf(log.file, "%s", log.log_string);
    va_end(args);

    // unlock the mutex on log_string
    pthread_mutex_unlock(&(log.mutex));
  }
private:
  FILE *file; // file to write log messages to
  char log_string[LOG_STRING_SIZE]; // string used to generate log message.
  bool bools[TYPE_SIZE]; // Boolean array storing state of Log types.
  pthread_mutex_t mutex; // mutex to protect file writing and log_string changing.

  /**
   * Constructor, set default file to stdout, turn all logging on except debug
   */
  Log() : file(stdout) {
    // initialize mutex
    pthread_mutex_init(&mutex, NULL);

    // enable all logging except DEBUG
    for (int ii = 0; ii < TYPE_SIZE; ii++) {
      bools[ii] = true;
    }
    bools[DEBUG] = false;
  }

  /**
   * Get the singleton instance of the Log object.
   * 
   * @return a pointer to the singleton instance of the Log object
   */
  static Log& get_instance() {
    static Log instance;
    return instance;
  }
}; // class Log
} // namespace QuaeModicaUtil
#endif // #ifndef __QUAE_MODICA_LOG_H__
