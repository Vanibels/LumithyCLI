#include <iostream>
#include <ctime>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

namespace fs = std::filesystem;

void handleOpen(int argc, char** argv){
    if (argc < 3){
        std::cout << "This command require an argument";
        return;
    }
}

int main(int argc, char** argv) {
    std::vector<std::string> subCommands;
    subCommands.push_back("open");
    subCommands.push_back("reload");
    subCommands.push_back("config");
    // std::cout << "Argc : " << argc << ", Argv[0] : " << argv[0] << std::endl;
    // fs::path path(argv[0]);
    if (argv[1] == NULL){std::cout << "No argument error" << std::endl ;return 1;}
    auto _subCommand = std::find(subCommands.begin(), subCommands.end(), argv[1]);
    if (_subCommand == subCommands.end()) {
        std::cout << "Error to resolve comand name : " << std::endl << " Comand available : " << std::endl;
        for (int i = 0; i <= subCommands.size() -1 ; i++){
            std::cout <<"   [" << i<< "] " << subCommands[i] << std::endl;
        }
        return 1;
    }
    std::cout << "Command found" << std::endl;
    std::string command = argv[1];

    if (command == "open") {
        handleOpen(argc, argv);
    } 
    else if (command == "reload") {
        std::cout << "Relancement de la config..." << std::endl;
    }
    else if (command == "config") {
        std::cout << "Ouverture de la config..." << std::endl;
    return 0;
    }
}

