#ifndef ALIAS_H
#define ALIAS_H
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

typedef enum alias {open, launch} t_alias;
const char* aliass[] = {"open","launch"};

class Alias {
    private:
        t_alias type;
        std::string name;
        fs::path path;
        fs::path ConfigFile;
    public:
        Alias(t_alias t,
             std::string n, 
             fs::path p, 
             fs::path cf ) : 
             type(t), 
             name(n), 
             path(p), 
             ConfigFile(cf) {}
        void setPath(fs::path p);
        void getByName(std::string n);
        void save();
        void unSave();
        fs::path getPath() const;
        std::string get() const;
};
#endif