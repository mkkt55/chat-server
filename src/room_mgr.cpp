#include "room_mgr.h"
#include "utils.h"
#include <string>

namespace chat{

using namespace main;

RoomMgr* RoomMgr::instance = nullptr;
RoomMgr* RoomMgr::Instance() {
    if (instance == nullptr) {
        instance = new RoomMgr();
    }
    return instance; 
}

bool RoomMgr::OnClientLogin(Client* client) {
    return true;
}

bool RoomMgr::OnClientLogout(Client* client) {
    return err_none == ClientExitRoom(client, client->GetRoom());
}

RoomMgr::RoomMgr() {
    //
}

error_id RoomMgr::ClientJoinRoom(Client* client, int32_t roomId, main::join_settings settings) {
    error_id err = err_none;
    if (m_mapRooms.find(roomId) == m_mapRooms.end()) {
        err = err_room_id_not_exist;
        return err;
    }
    auto room = m_mapRooms[roomId];
    room->participants[client] = settings;
    client->SetRoom(room);
    return err;
}

error_id RoomMgr::ClientExitRoom(Client* client, ChatRoom* room) {
    error_id err = err_none;
    if (room == nullptr) {
        return err_room_id_not_exist;
    }
    room->participants.erase(client);
    if (room->roomHolder == client) {
        if (room->participants.empty()) {
            delete room;
            m_mapRooms.erase(room->roomId);
            return err;
        }
        room->roomHolder = room->participants.begin()->first;
    }
}

error_id RoomMgr::ClientExitRoom(Client* client, int32_t roomId) {
    error_id err = err_none;
    if (m_mapRooms.find(roomId) == m_mapRooms.end()) {
        err = err_room_id_not_exist;
        return err;
    }
    auto room = m_mapRooms[roomId];
    return ClientExitRoom(client, room);
}

error_id RoomMgr::OnClientMsg(Client* client, std::string msg) {
    error_id err = err_none;
    printf("[Trace] %s, 1\n", __FUNCTION__);
    auto *room = client->GetRoom();
    if (room == nullptr) {
        err = err_room_id_not_exist;
        return err;
    }
    printf("[Trace] %s, 2\n", __FUNCTION__);
    recv_info_ntf ntf;
    std::string senderName = room->participants[client].join_name();
    printf("[Trace] %s, 3\n", __FUNCTION__);
    ntf.set_sender_name(senderName);
    printf("[Trace] %s, 10\n", __FUNCTION__);
    ntf.set_room_id(room->roomId);
    ntf.set_msg(msg);
    printf("[Trace] %s, 20\n", __FUNCTION__);
    for (auto &pair : room->participants) {
        if (pair.first == client) {
            //
        }
        pair.first->SendPack<recv_info_ntf>(1, ntf);
    }
    printf("[Trace] %s, 30\n", __FUNCTION__);
    return err;
}

int32_t RoomMgr::CreateNewRoom(Client* client, main::room_settings settings) {
    int32_t uuid = Gen32Uuid();
    auto p = new ChatRoom();
    p->roomId = uuid;
    p->settings = settings;
    p->roomHolder = client;
    main::join_settings joinSettings;
    joinSettings.set_join_name("房主（" + std::to_string(uuid) + "）");
    p->participants[client] = std::move(joinSettings);
    m_mapRooms[uuid] = p;
    return uuid;
}

bool RoomMgr::GetAllRoomList(main::get_all_room_list_resp &ack) {
    for (auto &pair : m_mapRooms) {
        ack.add_room_ids(pair.first);
    }
    return true;
}

bool RoomMgr::DismissRoom(Client* client, int32_t roomId, main::dismiss_room_resp &ack) {
    ack.set_error(main::err_none);
    if (m_mapRooms.find(roomId) == m_mapRooms.end()) {
        ack.set_error(main::err_room_id_not_exist);
        return false;
    }
    auto room = m_mapRooms[roomId];
    if (room->roomHolder != client) {
        ack.set_error(main::err_opt_disallowed_not_room_holder);
        return false;
    }
    auto &participants = room->participants;
    for (auto &pair : participants) {
        pair.first->OnRoomDismiss(roomId);
    }
    delete room;
    m_mapRooms.erase(roomId);
    return true;
}

}

