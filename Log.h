#ifndef SUBTLE_LIBRARY_LOG_H
#define SUBTLE_LIBRARY_LOG_H

#define LOG(X,Y...) Log::message(X, __FILE__, __LINE__, Y)
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <string>
#include <ostream>
#include <iostream>
#include <sstream>

class Log {
public:
    // log level equivalent to syslog per RFC 5424
    enum LOG_LEVEL {
        EMERGENCY = 0,
        ALERT = 1,
        CRITICAL = 2,
        ERROR = 3,
        WARNING = 4,
        NOTICE = 5,
        INFORMATIONAL = 6,
        DEBUG = 7,
        LOG_LEVEL_SIZE = 8
    };
    
    /**
     * 
     */
    static void setLogLevel(LOG_LEVEL level, bool enable = true) {
        LOG(DEBUG, "%s %s logging", enable ? "enabling" : "disabling", Log::get_instance().logLevelToString(level).c_str());
        Log::get_instance().bools[level] = enable;
    }

    /**
     * 
     */
    static void message(LOG_LEVEL type, const char *source, int line, const char *msg, ...) {
        Log log = Log::get_instance();

        if (!log.bools[type]) {
            return;
        }

        va_list args;
        va_start(args, message);

        while (strstr(source, "/") != NULL) {
            source = strstr(source, "/") + 1;
        }

        std::ostringstream oss;
        oss << "DATETIME";

        oss << "[" << log.logLevelToString(type) << " " << source << ":" << line;

    }
private:
    // boolean array storing state of log levels
    bool bools[LOG_LEVEL_SIZE];

    /**
     * 
     */
    static std::string logLevelToString(LOG_LEVEL level) {
        std::string response = "";
        switch(level) {
            case EMERGENCY:
                response = "EMERG";
                break;
            case ALERT:
                response = "ALERT";
                break;
            case CRITICAL:
                response = "CRIT ";
                break;
            case ERROR:
                response = "ERROR";
                break;
            case WARNING:
                response = "WARN ";
                break;
            case NOTICE:
                response = "NOTE ";
                break;
            case INFORMATIONAL:
                response = "INFO ";
                break;
            case DEBUG:
                response = "DEBUG";
                break;
            default:
                response = "UNKWN";
                break;
        }
        return response;
    }

    /**
     * 
     */
    Log() {

    }

    /**
     * 
     */
    ~Log() {

    }

    /**
     * 
     */
    static Log& get_instance() {
        static Log instance;
        return instance;
    }
};
#endif // ifndef SUBTLE_LIBRARY_LOG_H
