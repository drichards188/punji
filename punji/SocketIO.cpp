#include "SocketIO.h"

#include <unistd.h>

using namespace std;

SocketIO::SocketIO(
) : fd(-1) {

}

SocketIO::SocketIO(int newFD
) : fd(newFD) {

}

SocketIO::~SocketIO() {
    if (fd >= 0) {
        ::close(fd);
    }
}

void SocketIO::setFD(int fd) {
    this->fd = fd;
}

int SocketIO::getFD() const {
    return fd;
}

void SocketIO::readLine(string& line) {
    line.clear();
    if (fd == -1) {
        return;
    }
    char* buffer = buf;

    size_t totRead = 0; // Total bytes read so far
    ssize_t numRead = 0;                    // # of bytes fetched by last read()
    char ch = 0;

    for (; ;) {
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
}

int SocketIO::writeLine(const string& line) const {
    int returnCode = 0;

    string actualString = line + "\r\n";
    string::size_type expectedSize = actualString.length();
    size_t actualSize = writen(actualString.c_str(), expectedSize);
    if (actualSize != expectedSize) {
        return -1;
    }

    return returnCode;
}

/* Write 'n' bytes to 'fd' from 'buf', restarting after partial
   write or interruptions by a signal handlers */

size_t SocketIO::writen(const char* buffer, size_t n) const {
    ssize_t numWritten;                 /* # of bytes written by last write() */
    size_t totWritten;                  /* Total # of bytes written so far */

    for (totWritten = 0; totWritten < n;) {
        numWritten = write(fd, buffer, n - totWritten);

        /* The "write() returns 0" case should never happen, but the
           following ensures that we don't loop forever if it does */

        if (numWritten <= 0) {
            if (numWritten == -1 && errno == EINTR)
                continue;               /* Interrupted --> restart write() */
            else
                return 0;              /* Some other error */
        }
        totWritten += numWritten;
        buffer += numWritten;
    }
    return totWritten;                  /* Must be 'n' bytes if we get here */
}
