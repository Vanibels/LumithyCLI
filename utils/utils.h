namespace logs {
    typedef enum{critical, error, info, debug, warn, trace} t_logLevel;
    const char* logLevelNames[] = {"FATAL", "ERROR", "INFO", "DEBUG", "WARN", "TRACE"};
}

namespace color{
    const std::string aqua = "\033[1;36m";
    const std::string reset = "\033[0m";
    const std::string red = "\033[31m";
}