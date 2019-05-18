//
// Created by root on 11/20/17.
//
#include <unistd.h>
#include <regex>

#include "Log.h"
#include "SocketIO.h"

using namespace std;

void writeLine(int connfd, string message) {
    message = message + "\r\n";
    write(connfd, message.c_str(), message.size());
    Log::info("writeLine", "W: " + message);
}

void readLine(int fd, string &line) {
    line.clear();
    if (fd == -1) {
        return;
    }
    constexpr static size_t bufSize = 1024000;
    char buf[bufSize] = {0};
    char *buffer = buf;

    size_t totRead = 0; // Total bytes read so far
    ssize_t numRead = 0;                    // # of bytes fetched by last read()
    char ch = 0;

    for (;;) {
        numRead = read(fd, &ch, 1);

        if (numRead == -1) {
            if (errno == EINTR) {         // Interrupted --> restart read()
                continue;
            } else {
                return;              // Some other error
            }

        } else if (numRead == 0) {      // EOF
            if (totRead == 0) {          // No bytes read; return 0
                return;
            } else {                        // Some bytes read; add '\0'
                break;
            }

        } else {                        // 'numRead' must be 1 if we get here
            if (totRead < bufSize - 1) {      // Discard > (n - 1) bytes
                totRead++;
                *buffer++ = ch;
            }

            if (ch == '\n') {
                break;
            }
        }
    }
    *buffer = '\0';
    line = buf;
    Log::info("readLine", "R: " + line);
}

bool checkVerb(const string& buffer, const string& verb) {
    string temp = buffer;
    transform(temp.begin(), temp.end(), temp.begin(), ::toupper);

    return (temp.find(verb) == 0);

}