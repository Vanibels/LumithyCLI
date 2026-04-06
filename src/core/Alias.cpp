#include "Alias.h"
#include "utils/utils.h"
#include <string>
#include <filesystem>
#include <map>

void Alias::getByName(std::string n){
    name = " ";
    path = " ";
    std::map<std::string, std::string> keys = read(aliass[type], ConfigFile.string());

    if (keys.count(n)){
        name = n;
        path = keys[n];
        return;
    } else {
        path = NULL;
        return;
    }
}

void Alias::save() {
    write(aliass[type], name, path.string(), ConfigFile.string());
    return;
}

void Alias::unSave() {
    remove(aliass[type], name, ConfigFile.string());
    return;
}

void Alias::setPath(fs::path p){
    this->path = p;
    return;
}

fs::path Alias::getPath() const {
    return path;
}

std::string Alias::get() const {
    return name;
}