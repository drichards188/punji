#include <iostream>
#include <folly/Memory.h>
#include <folly/io/async/EventBaseManager.h>
#include <proxygen/httpserver/HTTPServer.h>

#include "muaHandler.h"
#include "Log.h"
#include "honeypotHandler.h"

using namespace ContentService;
using namespace proxygen;
using folly::SocketAddress;

const std::string Version = "0.0.1";

class MessageHandlerFactory : public RequestHandlerFactory {
public:
    void onServerStart(folly::EventBase* evb) noexcept override {
        sequenceNumber = 0;
        std::cout << "Server is running" << std::endl;
    }

    void onServerStop() noexcept override {
        sequenceNumber = 0;
    }

    RequestHandler* onRequest(RequestHandler*, HTTPMessage* httpMessage) noexcept override {
        httpMessage->dumpMessage(-1);
        return new ContentHandler(sequenceNumber, messsageHandlerMutex);
    }

private:
    int sequenceNumber = 0;
    std::mutex messsageHandlerMutex;
};

void honeypotThread() {
//    Log::info("port2525", "Starting Punji2525 " + VERSION);

    constexpr int MTA_PORT = 33000;

    int listenFD = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFD == -1) {
        Log::fatal("ccport", "ccport thread exiting with socket error: " + string(strerror(errno)));
        return;
    }

    int optval = 1;
    int optResult = setsockopt(listenFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (optResult == -1) {
        Log::fatal("ccport", "ccport thread exiting with setsockopt error: " + string(strerror(errno)));
        return;
    }

    struct sockaddr_in servaddr = {0};
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(MTA_PORT);

    int bindResult = bind(listenFD, (sockaddr *) &servaddr, sizeof(servaddr));
    if (bindResult == -1) {
        Log::fatal("ccport", "ccport thread on port " + to_string(MTA_PORT) + " exiting with bind error: " + strerror(errno));
        return;
    }

    Log::info("ccport", " listening for connections on port 33000");

    int listenResult = listen(listenFD, 5);
    if (listenResult == -1) {
        Log::fatal("ccport", "ccport thread exiting with listen error: " + string(strerror(errno)));
        return;
    }

    while (true) {

        // todo: make sure we log connections & active defense bad actors
        // todo: punji to the rescue!!!
        struct sockaddr_in clientaddr = {0};
        socklen_t addrlen = sizeof(struct sockaddr_in);
        int acceptedFD = accept(listenFD, (sockaddr *) &clientaddr, &addrlen);
        if (acceptedFD == -1) {
            Log::fatal("ccport", "ccport thread exiting with accept error: " + std::string(strerror(errno)));
            continue;
        }

        struct timeval tv;
        tv.tv_sec = 15;  // 15 Secs Timeout
        tv.tv_usec = 0;
        optResult = setsockopt(acceptedFD, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
        if (optResult == -1) {
            Log::fatal("ccport", "ccport thread exiting with setsockopt error: " + std::string(strerror(errno)));
            return;
        }

        std::string clientAddress = inet_ntoa(clientaddr.sin_addr);
        Log::info("ccport", "ccport client connected from " + clientAddress);
        time_t dateInSeconds = std::time(nullptr);
        std::stringstream message;
        message << std::put_time(std::localtime(&dateInSeconds), "%a %d %b %Y %H:%M:%S %z");

        std::thread t(handleConnection, acceptedFD, clientAddress);
        t.detach();
    }
}

int main(int argc, char* argv[]){

    if (argc < 2) {

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

    google::InitGoogleLogging(argv[0]);

    std::vector<HTTPServer::IPConfig> IPs = {
            {SocketAddress("127.0.0.1", 7777, true), HTTPServer::Protocol::HTTP}
    };

    HTTPServerOptions options;
    options.idleTimeout = std::chrono::milliseconds(60000);
    options.shutdownOn = {SIGINT, SIGTERM};
    options.enableContentCompression = false;
    options.handlerFactories = RequestHandlerChain()
            .addThen<MessageHandlerFactory>()
            .build();

    HTTPServer server(std::move(options));
    server.bind(IPs);

//     Start HTTPServer mainloop in a separate thread
    std::thread t([&] () {
        server.start();
    });

    std::thread u(honeypotThread);

    t.join();
    u.join();

    return 0;
}