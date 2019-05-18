#include "Log.h"

using namespace std;

bool Log::initialized = false;
std::ofstream Log::outfile;
std::ofstream Log::debugOutfile;
std::mutex Log::logMutex;

void Log::initialize() {
    outfile.open("/var/log/punjicc/punjiLog.log", ios_base::app);
    debugOutfile.open("/var/log/punjicc/debug.log", ios_base::app);
    initialized = true;
}

void Log::debug(const std::string& server, const std::string& message) {
    lock_guard<mutex> guard(logMutex);

    if (!initialized) {
        initialize();
    }

    debugOutfile << "DEBUG " << server << " " << message << endl;
}

void Log::info(const std::string& server, const std::string& message) {
    lock_guard<mutex> guard(logMutex);

    if (!initialized) {
        initialize();
    }

    outfile << "INFO  " << server << " " << message << endl;
}

void Log::warn(const std::string& server, const std::string& message) {
    lock_guard<mutex> guard(logMutex);

    if (!initialized) {
        initialize();
    }

    outfile << "WARN  " << server << " " << message << endl;
}

void Log::fatal(const std::string& server, const std::string& message) {
    lock_guard<mutex> guard(logMutex);

    if (!initialized) {
        initialize();
    }

    outfile << "FATAL " << server << " " << message << endl;
}