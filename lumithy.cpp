#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <filesystem>
#include <cstring>
#include <cstdlib>
#include <map>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winbase.h>
#include <lmcons.h>   

namespace fs = std::filesystem;
namespace logs {
    typedef enum{error,succes} t_status;
}
/*
    -e : opne explorer
 */
std::map<std::string, std::string> loadConfig(const std::string& section, const std::string& fileName){
    std::map<std::string, std::string> parametres;
    char buffer[2048];
    DWORD size = GetPrivateProfileStringA(section.c_str(), NULL, "",buffer, 2048, fileName.c_str());
    if (size == 0) {
        DWORD err = GetLastError();
        if (err == 0) {
            std::cout << "File donf but section [" << section << "] is empty." << std::endl;
        } else {
            std::cout << "Read error (Code: " << err << "). verify ANSI encode !" << std::endl;
        }
        return parametres;
    }
    char* pkey = buffer;
    while (*pkey) {
        std::string key = pkey;
        char valBuffer[512];
        GetPrivateProfileStringA(section.c_str(), key.c_str(), "", valBuffer, 512, fileName.c_str());
        parametres[key] = std::string(valBuffer);
        pkey += key.length() + 1;
    }
    return parametres;
}
void saveLogs(std::string commands, logs::t_status status) {
    std::time_t now = std::time(0);
    std::tm* localTime = std::localtime(&now);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", localTime);
    std::ofstream stream("history.log",std::ios::app);
    if (status == logs::error){
        stream << buffer << " " << commands << " s: " << "failed" << std::endl;
    } else if(status == logs::succes) {
        stream << buffer << " " << commands << " s: " << "success" << std::endl;
    }
    stream.close();
    return;
}
void handleOpen(int argc, char** argv, std::map<std::string, std::string> ptr){
    if (argc < 3){
        std::cout << "This command require an argument" << std::endl;
        std::string command = "open";
        saveLogs(command, logs::error);
        return;
    }
    std::string path;
    std::string cmd;
    std::string args = argv[2];
    for (auto const& [key,_path] : ptr) {
        if (key == args) {
            path = _path; //cmd.c_str()
            cmd += "explorer " + _path;
            system(cmd.c_str());
            saveLogs(cmd, logs::succes);
        }
    }
    if (args == "-e") {
        char username[UNLEN +1];
        std::string USER;
        DWORD size = UNLEN +1;
        std::string command = "open -e";
        if (GetUserNameA(username, &size)) {
            USER = username;
        } else {
            saveLogs(command, logs::error);
        }
        path = "C:\\Users\\" + USER;
        cmd = "explorer " +  path;
        system(cmd.c_str());
        saveLogs(command, logs::succes);
        return;
    }
}
int main(int argc, char** argv) {
    fs::path runPath = fs::current_path() / "config.ini";
    std::map<std::string, std::string> ptr = loadConfig("paths", runPath.string());
    if (argc < 2) {
        std::cout << "No argument error" << std::endl;
        return 1;
    }

    std::vector<std::string> subCommands = {"open", "reload", "config"};
    std::string command = argv[1];
    auto _subCommand = std::find(subCommands.begin(), subCommands.end(), command);
    if (_subCommand == subCommands.end()) {
        std::cout << "Error to resolve comand name : " << std::endl << " Comand available : " << std::endl;
        for (int i = 0; i <= subCommands.size() -1 ; i++){
            std::cout <<"   [" << i<< "] " << subCommands[i] << std::endl;
        }
        return 1;
    }

    if (command == "open") {
        handleOpen(argc, argv, ptr);
    } else if (command == "reload") {
        std::cout << "Relancement de la config..." << std::endl;
    } else if (command == "config") {
        std::cout << "Ouverture de la config..." << std::endl;
    }

    return 0;
}

#endif