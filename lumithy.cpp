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
#include "utils/utils.h"

namespace fs = std::filesystem;

fs::path file;

std::map<std::string, std::string> loadConfig(const std::string& section, const std::string& fileName){
    std::map<std::string, std::string> parametres;
    char buffer[2048];
    memset(buffer, 0, 2048);
    DWORD size = GetPrivateProfileStringA(section.c_str(), NULL, "",buffer, 2048, fileName.c_str());
    if (size == 0) {
        DWORD err = GetLastError();
        if (err == 0) {
            std::cout << color::red << "File exist but section [" << section << "] is empty." << std::endl;
        } else {
            std::cout << color::red << "Read error (Code: " << err << "). verify ANSI encode !" << std::endl;
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

void saveLogs(std::string commands, logs::t_logLevel logLevel) {
    std::time_t now = std::time(0);
    std::tm* localTime = std::localtime(&now);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", localTime);
    fs::path path = file / "history.log";
    std::ofstream stream(path.c_str(),std::ios::app);
    stream << buffer << "["<< logs::logLevelNames[logLevel] << "] " << commands << std::endl;
    stream.close();
    return;
}

void handleOpen(int argc, char** argv, std::map<std::string, std::string> ptr){
    if (argc < 3){
        std::cout << color::red << "This command require an argument" << std::endl << color::reset;
        std::string command = "-o";
        saveLogs(command, logs::error);
        return;
    }
    std::string command;
    for (int i = 1; i < argc; i++) {
        command += " ";
        command += argv[i];
    }
    std::string path;
    std::string cmd;
    std::string args = argv[2];
    bool found = false;
    for (auto const& [key,_path] : ptr) {
        if (key == args) {
            path = _path;
            cmd += "explorer " + _path;
            system(cmd.c_str());
            saveLogs(cmd, logs::success);
            found = true;
            break;
        }
    } 
    if (!found && args != "-e") {
        std::cout << color::red << "Alias not found!" << std::endl;
        saveLogs(command, logs::error);
    } 
    if (args == "-e") {
        char username[UNLEN +1];
        std::string USER;
        DWORD size = UNLEN +1;
        std::string command = "-o -e";
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

void handleLaunch(int argc, char** argv, std::map<std::string, std::string> ptr){
    if (argc < 3){
        std::cout << color::red << "This command require an argument" << std::endl << color::reset;
        std::string command = "-l";
        saveLogs(command, logs::error);
        return;
    }
    std::string path;
    std::string cmd;
    std::string args = argv[2];
    for (auto const& [key,_path] : ptr) {
        if (key == args) {
            path = _path; //cmd.c_str()
            cmd += "start " + _path;
            system(cmd.c_str());
            saveLogs(cmd, logs::success);
        }
    }
}

void displayLogo() {
    std::string logo = R"(
    __    __  ____  _________ __  __________  __   _________    ____
   / /   / / / /  |/  /  _/ /_/ / / /_  __/\ \/ /  / ____/ /   /  _/
  / /   / / / / /|_/ // // __/ /_/ / / /    \  /  / /   / /    / /  
 / /___/ /_/ / /  / // // /_/ __  / / /     / /  / /___/ /____/ /   
/_____/\____/_/  /_/___/\__/_/ /_/ /_/     /_/   \____/_____/___/   
    )";
    
    std::cout << color::aqua << logo << color::reset << std::endl;
    std::cout << " Version 1.0 | Dev by Luka Vanibels" << std::endl;
    std::cout << "------------------------------------" << std::endl;
}

void showHelp(){
    std::cout << color::red << "Welcome to Lumithy CLI" << std::endl << color::reset << std::endl;
    std::cout << "-h : help" << std::endl << "      => This command show the command help" << std::endl;
    std::cout << "-o : open" << std::endl << "      => This command in the file explorer a directory that the path are put in the config file" << std::endl << "     => tips : set -c for edit config file and add -e to open the explorer";
    std::cout << "-r : reset" << std::endl << "     => This command reload the config file (don't use that not, you can but don't use)" << std::endl;
    std::cout << "-c : config" << std::endl << "    => This command open in notepad the config file" << std::endl;
    std::cout << "-l : launch" << std::endl << "    => This command launch an application that the path are put in the config file " << std::endl << "     => tips : set -c for edit config file" << std::endl;
    std::string cmd;
    cmd += "-h ";
    saveLogs(cmd, logs::success);
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

void write(std::string section, std::string key, std::string value,  std::string file) {
    std::string fullPath = ".\\" + std::string(file); 
    const char* iniPath = fullPath.c_str();
    std::map<std::string, std::string> keys = read(section, file);
    if (keys.count(key)) {
        if (keys[key] == value) {
            std::cout << "Key : " << key << " already contains this value" << std::endl;
            saveLogs("[Internal config write] : already contain this value for this key",logs::info);
            return;
        }
        std::cout << "Key : " << key << " exists and contains : " << keys[key] << " would you like to replace it ? Y/N : ";
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
        std::cout << "Erreur d'ecriture : " << GetLastError() << std::endl;
        saveLogs("[Internal config write]",logs::error);
    }
}

void handelAdd(int argc, char** argv){ // lumithy -a {-o - open/-l - launch} {key} {value}
    std::string command;
    for (int i = 1; i < argc; i++) {
        command += " ";
        command += argv[i];
    }
    if (argc < 5){
        std::cout << color::red << "This command require 5 arguments" << std::endl << color::reset;
        saveLogs(command, logs::error);
        return;
    }
    std::string subArg = argv[2];
    auto subcommand = {"open","-o","launch", "-l"};
    auto _sub = std::find(subcommand.begin(), subcommand.end(), subArg);
    if (_sub == subcommand.end()) {
        std::cout << color::red << "Wrong argument : " << subArg << color::reset << std::endl;
        saveLogs(command,logs::error);
        return;
    }
    fs::path configPath = file.parent_path() / "config.ini";
    if (subArg == "open" || subArg == "-o") {
        write("open",argv[3],argv[4],configPath.string());
    } else{
        write("launch",argv[3],argv[4],configPath.string());
    }
    saveLogs(command, logs::success);
    return;
}
// duplicate entrey black-1-natax26 for jey debt_entities_pk
int main(int argc, char** argv) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    displayLogo();
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    fs::path exePath(buffer);
    fs::current_path(exePath.parent_path());
    fs::path configPath = exePath.parent_path() / "config.ini";
    file = exePath.parent_path();
    std::map<std::string, std::string> ptr = loadConfig("open", configPath.string());
    std::map<std::string, std::string> launch = loadConfig("launch", configPath.string());
    if (argc < 2) {
        std::cout << color::red << "No argument error" << std::endl << color::reset;
        showHelp();
        return 1;
    }
    std::vector<std::string> subCommands = {"-h", "help","-o","open", "-r", "reload", "-c","config", "-l", "launch", "-a", "add"};
    std::string command = argv[1];
    auto _subCommand = std::find(subCommands.begin(), subCommands.end(), command);
    if (_subCommand == subCommands.end()) {
        std::cout << color::red << "Error to resolve command name : " << std::endl << " Comand available : " << std::endl << color::reset;
        for (int i = 0; i <= subCommands.size() -1 ; i++){
            std::cout <<"   [" << i<< "] " << subCommands[i] << std::endl;
        }
        return 1;
    }
    if (command == "-h" || command == "help") {
        showHelp();
    } else if (command == "-o" || command == "open") {
        handleOpen(argc, argv, ptr);
    } else if(command == "-l" || command == "launch") {
        handleLaunch(argc, argv, launch);
    } else if (command == "-r" || command == "reload") {
        std::string cmd = "reload ";
        std::cout << "Relancement de la config..." << std::endl;
        ptr = loadConfig("open", configPath.string());
        launch = loadConfig("launch", configPath.string());
        saveLogs(cmd,logs::success);
    } else if (command == "-c" || command == "config") {
        fs::path config = configPath;
        std::string cmd;
        cmd += "notepad " + config.string();
        system(cmd.c_str());
        saveLogs(cmd,logs::success);
    } else if (command == "-a" || command == "add"){
        handelAdd(argc, argv);
    } else {
        std::cout << color::red << "Error : " << command << " isn't an available command " << color::reset;
        showHelp();
    }

    return 0;
}

#endif