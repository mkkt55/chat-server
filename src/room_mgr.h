#pragma once
#include "client.h"
#include <vector>
#include <list>

namespace chat {

class ChatRoom {
  friend class RoomMgr;
  private:
    ChatRoom() {}
    ChatRoom(const ChatRoom& room) = delete;
    ChatRoom &operator= (const ChatRoom&) = delete;
    Client* roomHolder;
    std::list<Client*> participants;
};

class RoomMgr {
  private:
    RoomMgr();
    static RoomMgr* instance;
  public:
    static RoomMgr* Instance();

  public:
    bool OnClientEnter(Client* client);
    bool OnClientLeave(Client* client);
    bool OnClientMsg(Client* client, std::string msg);

  private:
    std::vector<ChatRoom*> rooms;
};
}