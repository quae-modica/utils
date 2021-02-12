#ifndef __QUAE_MODICA_PROPERTIES_H__
#define __QUAE_MODICA_PROPERTIES_H__

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>

#include "Log.h"

namespace QuaeModicaUtil {
class Properties {
public:
  Properties() {}
  ~Properties() {}

  void set_string(std::string name, std::string value) {
    pairs[name] = value;
  }

  void set_int(std::string name, long value) {
    std::stringstream ss;
    ss << value;
    set_string(name, ss.str());
  }

  void set_float(std::string name, double value) {
    std::stringstream ss;
    ss << value;
    set_string(name, ss.str());
  }

  void set_bool(std::string name, bool value) {
    std::stringstream ss;
    ss << value;
    set_string(name, ss.str());
  }

  std::string get_string(std::string name) {
    return pairs[name];
  }

  long get_int(std::string name) {
    return atol(get_string(name).c_str());
  }

  double get_float(std::string name) {
    return atof(get_string(name).c_str());
  }

  bool get_bool(std::string name) {
    return !(get_string(name) == "false" || get_string(name) == "0");
  }

  void clear() {
    LOG(DEBUG, "Clearing Properties");

    pairs.clear();
  }

  std::string to_string() {
    LOG(DEBUG, "Writing Properties To String");

    // put the properties into [name]=[value] format
    std::string ans;
    for (std::map<std::string, std::string>::iterator ii = pairs.begin(); ii != pairs.end(); ii++) {
      ans += ii->first + "=" + ii->second + "\n"; 
    }
    return ans;
  }

  void from_string(std::string s) {
    LOG(DEBUG, "Reading Properties From String");

    // clear the properties
    clear();

    // read in each line in the file
    std::istringstream ss(s);
    std::string line;
    while (std::getline(ss, line)) {
      parse_line(line);
    }
  }

  void to_file(std::string filename) {
    LOG(DEBUG, "Writing Properties To File '%s'", filename.c_str());

    std::ofstream file(filename.c_str());
    if (file.is_open()) {
      file << to_string();
      file.close();
    } else {
      LOG(ERROR, "Unable To Open File '%s' To Write Properties", filename.c_str());
    }
  }

  void from_file(std::string filename) {
    LOG(DEBUG, "Reading Properties From File '%s'", filename.c_str());

    std::ifstream file(filename.c_str());
    if (file.is_open()) {
      std::string line;
      while (std::getline(file, line)) {
        parse_line(line);
      }
      file.close();
    } else {
      LOG(ERROR, "Unable To Open File '%s' To Read Properties", filename.c_str());
    }
  }

private:
  std::map<std::string, std::string> pairs;

  void parse_line(std::string line) {
    // check for comment
    if (line[0] == '#') {
      return;
    }

    // parse the line
    std::string name = line.substr(0, line.find("="));
    std::string value = line.substr(line.find("=") + 1);
    set_string(name, value);
  }
}; // class Messenger
} // namespace QuaeModicaUtil
#endif // #ifndef __QUAE_MODICA_PROPERTIES_H__
