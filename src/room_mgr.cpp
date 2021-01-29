#include "room_mgr.h"
#include "utils.h"
#include <string>

namespace chat{

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
    return true;
}

RoomMgr::RoomMgr() {
    //
}

bool RoomMgr::ClientJoinRoom(Client* client, int32_t roomId, main::join_settings settings, main::join_room_resp &ack) {ack.set_error(main::err_none);
    ack.set_error(main::err_none);
    if (m_mapRooms.find(roomId) == m_mapRooms.end()) {
        ack.set_error(main::err_room_id_not_exist);
        return false;
    }
    auto room = m_mapRooms[roomId];
    room->participants[client] = settings;
    return true;
}

bool RoomMgr::ClientExitRoom(Client* client, int32_t roomId, main::exit_room_resp &ack) {
    ack.set_error(main::err_none);
    if (m_mapRooms.find(roomId) == m_mapRooms.end()) {
        ack.set_error(main::err_room_id_not_exist);
        return false;
    }
    auto room = m_mapRooms[roomId];
    room->participants.erase(client);
    if (room->roomHolder == client) {
        if (room->participants.empty()) {
            delete room;
            m_mapRooms.erase(roomId);
            return true;
        }
        room->roomHolder = room->participants.begin()->first;
    }
    return true;
}

bool RoomMgr::OnClientMsg(Client* client, std::string msg) {
    return true;
}

int32_t RoomMgr::CreateNewRoom(Client* client, main::room_settings settings) {
    int32_t uuid = Gen32Uuid();
    auto p = new ChatRoom();
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

