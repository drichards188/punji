#pragma once

#include <string>

class SocketIO {
public:
    SocketIO();
    SocketIO(int newFD);
    ~SocketIO();

    void readLine(std::string& line);
    int writeLine(const std::string& line) const;
    void setFD(int fd);
    int getFD() const;
private:
    size_t writen(const char* buffer, size_t n) const;

    int fd = -1;
    constexpr static size_t bufSize = 1024000;
    char buf[bufSize] = {0};
};

