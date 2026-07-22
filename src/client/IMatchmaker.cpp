#include "client/IMatchmaker.h"

#include "client/RemoteStateSource.h"
#include "client/WebSocketClient.h"

namespace client {

void NoOpMatchmaker::requestPlay() {}

bool NoOpMatchmaker::isPlayVisible() const {
    return false;
}

std::string NoOpMatchmaker::statusLine() const {
    return "";
}

std::string NoOpMatchmaker::bannerText() const {
    return "";
}

WsMatchmaker::WsMatchmaker(WebSocketClient& client, RemoteStateSource& remote)
    : client_(client), remote_(remote) {}

void WsMatchmaker::requestPlay() {
    if (!remote_.isPlayVisible()) {
        return;
    }
    remote_.notePlayRequested();
    client_.send("PLAY");
}

bool WsMatchmaker::isPlayVisible() const {
    return remote_.isPlayVisible();
}

std::string WsMatchmaker::statusLine() const {
    return remote_.statusLine();
}

std::string WsMatchmaker::bannerText() const {
    return remote_.bannerText();
}

}  // namespace client
