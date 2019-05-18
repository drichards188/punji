#pragma once

#include <folly/Memory.h>
#include <proxygen/httpserver/RequestHandler.h>

namespace ContentService {
    class ContentHandler : public proxygen::RequestHandler {
    public:
        explicit ContentHandler(int sequenceNumber, std::mutex& contentHandlerMutex);

        void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers)
        noexcept override;

        void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;

        void onEOM() noexcept override;

        void onUpgrade(proxygen::UpgradeProtocol proto) noexcept override;

        void requestComplete() noexcept override;

        void onError(proxygen::ProxygenError err) noexcept override;

    private:
        std::unique_ptr<proxygen::HTTPMessage> httpMessage_;
        std::unique_ptr<folly::IOBuf> body_;
    };
}