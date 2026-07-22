#ifndef IMATCHMAKER_H
#define IMATCHMAKER_H

#include <string>

namespace client {

// Play-button collaborator. Injected once; NoOp offline, Ws online.
class IMatchmaker {
public:
    virtual ~IMatchmaker() = default;

    virtual void requestPlay() = 0;
    virtual bool isPlayVisible() const = 0;
    virtual std::string statusLine() const = 0;
    virtual std::string bannerText() const = 0;
};

class NoOpMatchmaker : public IMatchmaker {
public:
    void requestPlay() override;
    bool isPlayVisible() const override;
    std::string statusLine() const override;
    std::string bannerText() const override;
};

class WebSocketClient;
class RemoteStateSource;

class WsMatchmaker : public IMatchmaker {
public:
    WsMatchmaker(WebSocketClient& client, RemoteStateSource& remote);

    void requestPlay() override;
    bool isPlayVisible() const override;
    std::string statusLine() const override;
    std::string bannerText() const override;

private:
    WebSocketClient& client_;
    RemoteStateSource& remote_;
};

}  // namespace client

#endif
