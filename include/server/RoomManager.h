#ifndef ROOM_MANAGER_H
#define ROOM_MANAGER_H

#include "auth/UserService.h"
#include "bus/MoveLogSubscriber.h"
#include "bus/SoundSubscriber.h"
#include "server/ConnectionRegistry.h"
#include "server/MatchRoom.h"

#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

struct OutboundMessage {
    ConnectionHdl hdl;
    std::string payload;
};

struct RoomCommandResult {
    std::vector<OutboundMessage> messages;
    std::string errorReason;
};

// Owns live MatchRooms + shared move/sound subscribers. Never sends on the wire.
class RoomManager {
    ConnectionRegistry& registry_;
    UserService& users_;
    MoveLogSubscriber moveLog_;
    SoundSubscriber sound_;
    std::map<int, std::unique_ptr<MatchRoom>> rooms_;
    int nextMatchId_ = 1;

    void appendBroadcast(const MatchRoom& room, const std::string& payload,
                         std::vector<OutboundMessage>& out) const;

public:
    RoomManager(ConnectionRegistry& registry, UserService& users,
                std::ostream& moveLogOut, std::string soundsDir,
                std::ostream& soundOut);

    std::vector<OutboundMessage> createMatch(ConnectionHdl white,
                                             ConnectionHdl black);
    std::vector<OutboundMessage> tearDownMatch(int matchId,
                                               ConnectionHdl exceptHdl);
    RoomCommandResult handleSeatedCommand(ConnectionHdl hdl,
                                          const std::string& line);
    std::vector<OutboundMessage> tickAll(int ms);
    void clear();
};

#endif
