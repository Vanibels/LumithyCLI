#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>

 std::map<std::string, std::string> read(std::string section){
    const char* iniPath = "unit.ini";
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

void write(std::string section, std::string key, std::string value){
    std::map<std::string, std::string> keys = read(section);
    if (keys.count(key)){
        if (keys[key] == value){
            std::cout << "La clé : " << key << " de la section : " << section << " possede deja la valeur : " << value << std::endl;
            return;
        }
        std::cout << "La clé : " << key << " de la section : " << section << " possede deja une valeur : " << keys[key] << std::endl << " Voulez vous l'écrasser ?" << std::endl;
        std::cout << "Y/N : ";
        char input;
        std::cin >> input;
        std::vector<char> inputs = {'Y','N','y','n'};
        auto _inputs = std::find(inputs.begin(), inputs.end(), input);
        while (_inputs == inputs.end()){
            std::cout <<"Wrong input retry. correct input are :";
            for (int i = 0; i <= inputs.size() -1 ; i++){
                std::cout << inputs[i];
            }
            std::cout << std::endl;
            std::cin >> input;
            _inputs = std::find(inputs.begin(), inputs.end(), input);
        }
        if (input == 'N' || input == 'n'){
            return;
        }
    }
    const char* iniPath = "unit.ini";
    // Syntaxe : Section, Clé, Valeur, Chemin
    BOOL success = WritePrivateProfileStringA(
        section.c_str(),           // [Section]
        key.c_str(),           // Clé
        value.c_str(),    // Nouvelle valeur (le chemin)
        iniPath           // Chemin du fichier .ini
    );

    if (success) {
        std::cout << "Configuration mise a jour avec succes !" << std::endl;
    } else {
        std::cout << "Erreur d'ecriture. Code : " << GetLastError() << std::endl;
    }

    return;
}
int main() {
    return 0;
}
#endif