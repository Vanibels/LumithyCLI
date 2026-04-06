#include "utils.h"
#include <map>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <ctime>
#include <iostream>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winbase.h>
#include <lmcons.h>

namespace fs = std::filesystem;

fs::path getInitFiles() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    fs::path exePath(buffer);
    fs::current_path(exePath.parent_path());
    return exePath.parent_path();
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

void saveLogs(std::string logMessage, logs::t_logLevel logLevel) {
    std::time_t now = std::time(0);
    std::tm* localTime = std::localtime(&now);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", localTime);
    fs::path path = getInitFiles() / "history.log";
    std::ofstream stream(path.c_str(),std::ios::app);
    stream << buffer << "["<< logs::logLevelNames[logLevel] << "] " << logMessage << std::endl;
    stream.close();
    return;
}

std::string read(std::string section,std::string key, std::string file){
    std::ifstream checkFile(file);
    if (!checkFile.good()) {
        std::cout << color::red << "CRITICAL error" << "the file " << file << " doesn't exist" << color::reset <<  std::endl;
        saveLogs("[Internal Read Config]", logs::critical);
        return "NOT_FOUND";
    }
    const char* iniPath = file.c_str();
    char value[MAX_PATH];
    GetPrivateProfileStringA(section.c_str(), key.c_str(), "NOT_FOUND",value, MAX_PATH, iniPath);
    return std::string(value);
}

void write(std::string section, std::string key, std::string value,  std::string file) {
    std::string fullPath = std::string(file); 
    const char* iniPath = fullPath.c_str();
    std::map<std::string, std::string> keys = read(section, file);
    if (keys.count(key)) {
        if (keys[key] == value) {
            std::cout << color::yellow  << "Key : " << key << " already contains this value" << color::reset << std::endl;
            saveLogs("[Internal config write] : already contain this value for this key",logs::warn);
            return;
        }
        std::cout << color::yellow  << "Key : " << key << " exists and contains : " << keys[key] << " would you like to replace it ? Y/N : " << color::reset ;
        char input;
        std::cin >> input;
        if (input == 'n' || input == 'N') return;
    }
    BOOL success = WritePrivateProfileStringA(
        section.c_str(), 
        key.c_str(), 
        value.c_str(), 
        iniPath
    );

    if (success) {
        WritePrivateProfileStringA(NULL, NULL, NULL, iniPath);
        saveLogs("[Internal config write]",logs::info);
    } else {
        std::cout << color::red << "Write error : " << GetLastError() << color::reset << std::endl;
        saveLogs("[Internal config write] " + std::to_string(GetLastError()),logs::critical);
    }
}

void remove(std::string section, std::string key, std::string file) {
    std::map<std::string, std::string> keys = read(section, file);
    if (keys.find(key) == keys.end()) {
        std::cout << color::yellow << "[WARN] Key '" << key << "' not found in section [" << section << "]." << color::reset << std::endl;
        return;
    }
    std::cout << "Are you sure you want to delete '" << key << "' (Value: " << keys[key] << ")? (Y/N): ";
    char input;
    std::cin >> input;
    if (input == 'n' || input == 'N') {
        std::cout << "Deletion cancelled." << std::endl;
        return;
    }

    BOOL success = WritePrivateProfileStringA(
        section.c_str(),
        key.c_str(),
        NULL,
        file.c_str()
    );

    if (success) {
        WritePrivateProfileStringA(NULL, NULL, NULL, file.c_str());
        std::cout << color::aqua << "Key '" << key << "' successfully removed!" << color::reset << std::endl;
        saveLogs("Key: " + key + " from [" + section + "]", logs::info);
        return;
    } else {
        std::cout << color::red << "Removal error: " << GetLastError() << color::reset << std::endl;
        saveLogs("Error: " + std::to_string(GetLastError()), logs::critical);
        return;
    }
}

#endif