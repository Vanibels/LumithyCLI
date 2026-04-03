namespace logs {
    typedef enum{critical, error, info, debug, warn, trace} t_logLevel;
    const char* logLevelNames[] = {"FATAL", "ERROR", "INFO", "DEBUG", "WARN", "TRACE"};
}

namespace color{
    inline const std::string aqua = "\033[1;36m";
    inline const std::string reset = "\033[0m";
    inline const std::string red = "\033[31m";
    inline const std::string yellow = "\033[33m";
}