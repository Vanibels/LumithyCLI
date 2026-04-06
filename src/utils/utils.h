#include <map>
#include <fstream>
#include <filesystem>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winbase.h>
#include <lmcons.h>


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

std::map<std::string, std::string> read(std::string section, std::string file){
    std::ifstream checkFile(file);
    if (!checkFile.good()) {
        std::ofstream createFile(file);
        createFile << "[open]" << std::endl;
        createFile << "[launch]" << std::endl;
        createFile.close();
    }
    const char* iniPath = file.c_str();
    std::map<std::string, std::string> openAliases;
    char buffer[2048]; 
    GetPrivateProfileStringA(section.c_str(), NULL, "", buffer, 2048, iniPath);
    char* key = buffer;
    while (*key != '\0') {
        char value[MAX_PATH];
        GetPrivateProfileStringA(section.c_str(), key, "NOT_FOUND", value, MAX_PATH, iniPath);
        openAliases[key] = value;
        key += strlen(key) + 1;
    }
    return openAliases;
}

#endif