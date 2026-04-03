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

#define LOG_VERSION "0.2.0"
#define VERSION "2.0"
#define CONFIG_VERSION "0.1.0"
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

void saveLogs(std::string logMessage, logs::t_logLevel logLevel) {
    std::time_t now = std::time(0);
    std::tm* localTime = std::localtime(&now);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", localTime);
    fs::path path = file / "history.log";
    std::ofstream stream(path.c_str(),std::ios::app);
    stream << buffer << "["<< logs::logLevelNames[logLevel] << "] " << logMessage << std::endl;
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
            saveLogs(cmd, logs::info);
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
        saveLogs(command, logs::info);
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
            saveLogs(cmd, logs::info);
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
    std::cout << "-l : launch" << std::endl << "    => This command launch an application that the path are put in the config file " << std::endl;
    std::cout << "-a : add" << std::endl << "    => This command add new key in configuration" << std::endl;
    std::cout << "-d : delete" << std::endl << "    => This command remove a specified key in configuration" << std::endl;
    std::string cmd;
    cmd += "-h ";
    saveLogs(cmd, logs::info);
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

std::string read(std::string section,std::string key, std::string file){
    std::ifstream checkFile(file);
    if (!checkFile.good()) {
        std::cout << "CRITICAL error" << "the file " << file << " doesn't exist" << std::endl;
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
        std::cout << "Write error : " << GetLastError() << std::endl;
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
    } else {
        std::cout << color::red << "Removal error: " << GetLastError() << color::reset << std::endl;
        saveLogs("Error: " + std::to_string(GetLastError()), logs::critical);
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
    fs::path configPath = file / "config.ini";
    std::string value;
    for (int i = 4; i < argc; i++) {
        value += argv[i];
        if (i < argc - 1) {
            value += " ";
        }
    }
    if (subArg == "open" || subArg == "-o") {
        write("open",argv[3],value,configPath.string());
    } else{
        write("launch",argv[3],value,configPath.string());
    }
    saveLogs(command, logs::info);
    return;
}

void handelRemove(int argc, char** argv) {
    std::string command;
    for (int i = 1; i < argc; i++) {
        command += " ";
        command += argv[i];
    }
    if (argc < 4) {
        std::cout << color::red << "[ERROR] This command requires 4 arguments." << std::endl;
        std::cout << "Usage: lumithy -r {-o/-l} {key}" << color::reset << std::endl;
        saveLogs(command, logs::error);
        return;
    }
    std::string subArg = argv[2];
    auto subcommand = {"open", "-o", "launch", "-l"};
    auto _sub = std::find(subcommand.begin(), subcommand.end(), subArg);
    if (_sub == subcommand.end()) {
        std::cout << color::red << "Wrong section argument: " << subArg << color::reset << std::endl;
        saveLogs(command, logs::error);
        return;
    }
    fs::path configPath = file / "config.ini";
    std::string section = (subArg == "open" || subArg == "-o") ? "open" : "launch";
    remove(section, argv[3], configPath.string());

    saveLogs(command, logs::info);
}

bool init (int argc, char** argv){
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
    if (!fs::is_regular_file(configPath)){
        std::cout << color::red << "Config file missing. Creating default..." << color::reset << std::endl;
        write("config", "version", VERSION, configPath.string());
        write("config", "log_version", LOG_VERSION, configPath.string());
        write("config", "config_version", CONFIG_VERSION, configPath.string());
        saveLogs("Config file was missing or corrupted. Created a new one.", logs::warn);
    }
    std::string app_version = read("config", "version", configPath.string());
    std::string log_version = read("config", "log_version", configPath.string());
    std::string config_version = read("config", "config_version", configPath.string());
    if (app_version == "NOT_FOUND" || log_version == "NOT_FOUND" || config_version == "NOT_FOUND" || config_version != CONFIG_VERSION){
        std::cout << color::red << "[ERROR] The config file is missing a required section or is incompatible." << color::reset << std::endl;
        std::cout << color::red << "[ERROR] Execution stopped to prevent errors. Exiting with critical status." << color::reset << std::endl;
        std::cout << color::yellow << "[WARN] Download the latest config and application here: https://github.com/Vanibels/LumithyCLI" << color::reset << std::endl;
        saveLogs("Config version mismatch or corruption - Execution halted.", logs::critical);
        return false;
    } else if (app_version != VERSION) {
        std::cout << color::yellow << "[WARN] Version mismatch! You are running " << app_version << ". Latest is " << VERSION << "." << color::reset << std::endl;
        std::cout << color::yellow << "[WARN] Please download the latest version here: https://github.com/Vanibels/LumithyCLI" << color::reset << std::endl;
        saveLogs("App version mismatch detected.", logs::warn);
    } else if (log_version != LOG_VERSION) {
        std::cout << color::yellow << "[WARN] Log format is outdated. Some logs might display incorrectly." << color::reset << std::endl;
        saveLogs("Log version mismatch detected.", logs::warn);
    }
    return true;
}

// Idea : pour la methode write faire en sorte de prendre une chaine de tout les argument derrier et non juste l'argurement argv[3]

int main(int argc, char** argv) {
    bool initialisation = init(argc, argv);
    if (!initialisation){
        std::cout << "A crital error has detected. The program turn off";
        return 1;
    }
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    fs::path exePath(buffer);
    fs::current_path(exePath.parent_path());
    fs::path configPath = exePath.parent_path() / "config.ini";
    std::map<std::string, std::string> ptr = loadConfig("open", configPath.string());
    std::map<std::string, std::string> launch = loadConfig("launch", configPath.string());
    if (argc < 2) {
        std::cout << color::red << "No argument error" << std::endl << color::reset;
        showHelp();
        return 1;
    }
    std::vector<std::string> subCommands = {"-h", "help",
                                            "-o","open", 
                                            "-r", "reload", 
                                            "-c","config", 
                                            "-l", "launch", 
                                            "-a", "add",
                                            "-d", "delete"
                                        };
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
        saveLogs(cmd,logs::info);
    } else if (command == "-c" || command == "config") {
        fs::path config = configPath;
        std::string cmd;
        cmd += "notepad " + config.string();
        system(cmd.c_str());
        saveLogs(cmd,logs::info);
    } else if (command == "-a" || command == "add"){
        handelAdd(argc, argv);
    } else if(command == "-d" || command == "delete") {
        handelRemove(argc, argv);
    } else {
        std::cout << color::red << "Error : " << command << " isn't an available command " << color::reset;
        showHelp();
    }

    return 0;
}

#endif