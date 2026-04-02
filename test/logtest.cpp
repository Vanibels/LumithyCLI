#include <fstream>
#include <iostream>
#include <ctime>
#include <string>

typedef enum{error,succes} t_status;
void saveLogs(std::string commands, t_status status) {
    std::time_t now = std::time(0);
    std::tm* localTime = std::localtime(&now);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", localTime);
    std::cout << "Save";

    std::ofstream stream("test.log",std::ios::app);
    stream << buffer << " " << commands << " " << status << std::endl;
    stream.close();
    return;
}
int main(int argc, char** argv) {
    saveLogs(argv[0], error);
    return 0;
}
