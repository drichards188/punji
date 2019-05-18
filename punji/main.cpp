#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <sstream>
#include <iomanip>
#include <set>
#include <fcntl.h>
#include <sys/stat.h>
#include <vector>
#include "ContentHandler.h"

using namespace std;

int main(int argc, char* argv[]){
    printf("Server is running\n");
    if (argc == 1) {
        switch (fork()) {
            case -1:
                return -1;
            case 0:
                break;
            default:
                _exit(EXIT_SUCCESS);
        }

        if (setsid() == -1) {
            return -1;
        }

        switch (fork()) {
            case -1:
                return -1;
            case 0:
                break;
            default:
                _exit(EXIT_SUCCESS);
        }

        umask(0);
        chdir("/");

        long maxfd = sysconf(_SC_OPEN_MAX);
        if (maxfd == -1) {
            maxfd = 64;
        }

        for (int fd = 0; fd < maxfd; ++fd) {
            close(fd);
        }

        close(STDIN_FILENO);

        int fd = open("/dev/null", O_RDWR);

        if (fd != STDIN_FILENO) {
            return -1;
        }

        if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO) {
            return -1;
        }

        if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO) {
            return -1;
        }
    }
    int listenfd = 0;
    int connfd = 0;
    socklen_t len;
    struct sockaddr_in servaddr, cliaddr;
    pid_t childpid;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(25);

    int optval = 1;
    int optResult = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (optResult == -1) {
        printf("MTA thread exiting with setsockopt error: %s\n", strerror(errno));
        return 1;
    }

    int result = bind(listenfd, (sockaddr *) &servaddr, sizeof(servaddr));
    if (result == -1) {
        printf("bind failed: %s\n", strerror(errno));
        return 1;
    }
    result = listen(listenfd, 5);
    if (result == -1) {
        printf("listen failed: %s\n", strerror(errno));
        return 1;
    }
    socklen_t addrlen = sizeof(struct sockaddr_in);

    while(true) {
        connfd = accept(listenfd, (sockaddr *) & cliaddr, &addrlen);
        string clientAddress = inet_ntoa(cliaddr.sin_addr);
        result = handleConnection(connfd, clientAddress);
        if (result != 0) {
            break;
        }
//        if ( (childpid = fork()) == 0) {
//            close(listenfd);
//            handleConnection(connfd, clientAddress);
//            exit(0);
//        }
    }
    printf("Closing connection\n");
    close(connfd);
    close(listenfd);
    printf("Graceful Shutdown\n");
    return 0;
}