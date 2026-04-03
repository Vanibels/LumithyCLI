namespace logs {
    typedef enum{error,succes,info} t_logLevel;
    const char* logLevelNames[] = { "ERROR", "SUCCESS", "INFO" };
}

namespace color{
    const std::string aqua = "\033[1;36m";
    const std::string reset = "\033[0m";
    const std::string red = "\033[31m";
}