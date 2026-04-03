#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#ifdef _WIN32
#include <windows.h>


std::map<std::string, std::string> read(std::string section, std::string file){
    std::ifstream checkFile(file);
    if (!checkFile.good()) {
        std::ofstream createFile(file);
        createFile << "[open]" << std::endl;
        createFile << "[launch]" << std::endl;
        createFile.close();
        std::cout << "Fichier unit.ini cree manuellement." << std::endl;
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
    std::cout << "--- Contenu de la Map [open] ---" << std::endl;
    for (auto const& [alias, path] : openAliases) {
        std::cout << "Alias : " << alias << " -> Path : " << path << std::endl;
    }
    std::string userChoice = "root";
    if (openAliases.count(userChoice)) {
        std::cout << "\nOuverture de : " << openAliases[userChoice] << std::endl;
    }
    return openAliases;
}

void write(std::string section, std::string key, std::string value,  std::string file) {
    std::string fullPath = ".\\" + std::string(file); 
    const char* iniPath = fullPath.c_str();
    std::map<std::string, std::string> keys = read(section, file);
    if (keys.count(key)) {
        if (keys[key] == value) {
            std::cout << "La cle : " << key << " a deja la bonne valeur." << std::endl;
            return;
        }
        std::cout << "La cle : " << key << " existe (actuel: " << keys[key] << "). Ecraser ? Y/N : ";
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
        std::cout << "Configuration mise a jour !" << std::endl;
    } else {
        std::cout << "Erreur d'ecriture : " << GetLastError() << std::endl;
    }
}
int main() { 
    write("test","root","C:\\Windows\\System32", "unit.ini");
    return 0;
}
#endif