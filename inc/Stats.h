#ifndef __QUAE_MODICA_STATS_H__
#define __QUAE_MODICA_STATS_H__

#include <sstream>
#include <string>
#include <map>

namespace QuaeModicaUtil {
class Stats {
public:
  Stats() {}
  ~Stats() {}

  std::string get_stat(std::string name) {
    return pairs[name];
  }
  void set_stat(std::string name, std::string value) {
    pairs[name] = value;
  }
  void clear_stats() {
    pairs.clear();
  }
  std::string to_string() {
    std::string ans;
    for (std::map<std::string, std::string>::iterator ii = pairs.begin(); ii != pairs.end(); ii++) {
      ans += ii->first + "=" + ii->second + "\n";
    }
    return ans;
  }
  void from_string(std::string s) {
    std::istringstream ss(s);
    std::string line;
    while (std::getline(ss, line)) {
      std::string name = line.substr(0, line.find("="));
      std::string value = line.substr(line.find("=") + 1);
      set_stat(name, value);
    }
  }
private:
  std::map<std::string, std::string> pairs;
}; // class Stats
} // namespace QuaeModicaUtil
#endif // #ifndef __QUAE_MODICA_STATS_H__
