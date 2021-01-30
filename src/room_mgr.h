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
    int32_t roomId = 0;
    main::room_settings settings;
    std::unordered_map<Client*, main::join_settings> participants;
};

class RoomMgr {
  private:
    RoomMgr();
    static RoomMgr* instance;
  public:
    static RoomMgr* Instance();

  public:
    bool OnClientLogin(Client*);
    bool OnClientLogout(Client*);
    main::error_id ClientJoinRoom(Client* client, int32_t roomId, main::join_settings settings);
    main::error_id ClientExitRoom(Client* client, ChatRoom* room);
    main::error_id ClientExitRoom(Client* client, int32_t roomId);
    main::error_id OnClientMsg(Client* client, std::string msg);
    bool GetAllRoomList(main::get_all_room_list_resp &ack);
    int32_t CreateNewRoom(Client*, main::room_settings settings);
    bool DismissRoom(Client* client, int32_t roomId, main::dismiss_room_resp &ack);

  private:
    std::unordered_map<int32_t, ChatRoom*> m_mapRooms;
};
}