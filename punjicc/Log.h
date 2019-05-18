#pragma once

#include <string>
#include <mutex>
#include <fstream>

class Log {
public:
    static void debug(const std::string& server, const std::string& message);
    static void info(const std::string& server, const std::string& message);
    static void warn(const std::string& server, const std::string& message);
    static void fatal(const std::string& server, const std::string& message);

private:
    static std::ofstream outfile;
    static std::ofstream debugOutfile;
    static std::mutex logMutex;
    static bool initialized;

    static void initialize();

};

