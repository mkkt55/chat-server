#pragma once
#include "client.h"
#include <unordered_map>
#include "cs.pb.h"

namespace chat {

class ChatRoom {
  friend class RoomMgr;
  private:
    ChatRoom() {}
    ChatRoom(const ChatRoom& room) = delete;
    ChatRoom &operator= (const ChatRoom&) = delete;
    Client* roomHolder;
    main::room_settings settings;
    std::unordered_map<Client*, main::join_settings> participants;
};

class RoomMgr {
  private:
    RoomMgr();
    static RoomMgr* instance;
  public:
    static RoomMgr* Instance();
    static bool OnClientLogin(Client*);
    static bool OnClientLogout(Client*);

  public:
    bool ClientJoinRoom(Client* client, int32_t roomId, main::join_settings settings, main::join_room_resp &ack);
    bool ClientExitRoom(Client* client, int32_t roomId, main::exit_room_resp &ack);
    bool OnClientMsg(Client* client, std::string msg);
    bool GetAllRoomList(main::get_all_room_list_resp &ack);
    int32_t CreateNewRoom(Client*, main::room_settings settings);
    bool DismissRoom(Client* client, int32_t roomId, main::dismiss_room_resp &ack);

  private:
    std::unordered_map<int32_t, ChatRoom*> m_mapRooms;
};
}