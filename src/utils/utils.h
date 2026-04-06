#include <map>
#include <string>

#ifdef _WIN32


#define LOG_VERSION "0.2.0"
#define VERSION "2.0"
#define CONFIG_VERSION "0.1.0"
#define WIN32_LEAN_AND_MEAN


namespace logs {
    typedef enum{critical, error, info, debug, warn, trace} t_logLevel;
    const char* logLevelNames[] = {"FATAL", "ERROR", "INFO", "DEBUG", "WARN", "TRACE"};
}

namespace color{
    inline const std::string aqua = "\033[1;36m";
    inline const std::string reset = "\033[0m";
    inline const std::string red = "\033[31m";
    inline const std::string yellow = "\033[33m";
}

std::map<std::string, std::string> read(std::string section, std::string file);
void saveLogs(std::string logMessage, logs::t_logLevel logLevel);
std::string read(std::string section, std::string key, std::string file);
void write(std::string section, std::string key, std::string value, std::string file);
void remove(std::string section, std::string key, std::string file);
fs::path getInitFiles();

#endif