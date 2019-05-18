#include <iostream>
#include <fstream>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <folly/dynamic.h>
#include <folly/json.h>
#include <regex>

#include "Log.h"
#include "SocketIO.h"
#include "muaHandler.h"

using folly::dynamic;
using namespace proxygen;

namespace ContentService {

    ContentHandler::ContentHandler(int sequenceNumber, std::mutex &contentHandlerMutex) {}

    void ContentHandler::onRequest(std::unique_ptr<proxygen::HTTPMessage> httpMessage) noexcept {
        HTTPHeaders headers = httpMessage->getHeaders();
        std::string path = httpMessage->getPath();
        httpMessage_ = std::move(httpMessage);
    }

    void ContentHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept {
        if (body_) {
            body_->prependChain(std::move(body));
        } else {
            body_ = std::move(body);
        }
    }

    void ContentHandler::onEOM() noexcept {
        std::string path = httpMessage_->getPath();
        Log::debug("TEST", path);
        if (path.find("/punjicc/functions/subscribe") == 0) {
            folly::fbstring contents = body_->moveToFbString();
            dynamic jsonObj = folly::parseJson(contents);
            std::string content = jsonObj["content"].getString();

            if (content.size() > 0) {
                std::ofstream fileStream;
                fileStream.open("/home/drich/Desktop/testData.txt", std::ios_base::app);
                fileStream << content << std::endl;
                fileStream.close();

                std::string jwt = "Watson!!!";

                ResponseBuilder(downstream_)
                        .status(200, "OK")
                        .header("Content-Type", "application/json")
                        .body(R"({"response": ")" + jwt + R"("})")
                        .sendWithEOM();
                return;
            }
        }
    }

    void ContentHandler::onUpgrade(proxygen::UpgradeProtocol protocol) noexcept {}

    void ContentHandler::requestComplete() noexcept {
        delete this;
    }

    void ContentHandler::onError(proxygen::ProxygenError err) noexcept {
        delete this;
    }
}