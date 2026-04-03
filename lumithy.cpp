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
#include <utils/utils.h>

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
            saveLogs(cmd, logs::succes);
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
    // {"-h","-o", "-r", "-c", "-l"
    std::cout << color::red << "Welcome to Lumithy CLI" << std::endl << color::reset << std::endl;
    std::cout << "-h : help" << std::endl << "      => This command show the command help" << std::endl;
    std::cout << "-o : open" << std::endl << "      => This command in the file explorer a directory that the path are put in the config file" << std::endl << "     => tips : set -c for edit config file and add -e to open the explorer";
    std::cout << "-r : reset" << std::endl << "     => This command reload the config file (don't use that not, you can but don't use)" << std::endl;
    std::cout << "-c : config" << std::endl << "    => This command open in notepad the config file" << std::endl;
    std::cout << "-l : launch" << std::endl << "    => This command launch an application that the path are put in the config file " << std::endl << "     => tips : set -c for edit config file" << std::endl;
    std::string cmd;
    cmd += "-h ";
    saveLogs(cmd, logs::succes);
}

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

    std::vector<std::string> subCommands = {"-h", "help","-o","open", "-r", "reload", "-c","config" "-l", "launch"};
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
        saveLogs(cmd,logs::succes);
    } else if (command == "-c" || command == "config") {
        fs::path config = configPath;
        std::string cmd;
        cmd += "notepad " + config.string();
        system(cmd.c_str());
        saveLogs(cmd,logs::succes);
    } else {
        std::cout << color::red << "Error : " << command << " isn't an available command " << color::reset;
        showHelp();
    }

    return 0;
}

#endif