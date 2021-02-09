#include "room_mgr.h"
#include "utils.h"
#include <string>
#include <sstream>

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
    if (!room->settings.open() && room->participants.find(client) == room->participants.end()) {
        err = err_join_room_close;
        return err;
    }
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
    return err;
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
    auto *room = client->GetRoom();
    if (room == nullptr) {
        err = err_room_id_not_exist;
        return err;
    }
    recv_info_ntf ntf;
    std::string senderName = room->participants[client].join_name() + "(" + client->GetAuth() + ")";
    if (room->roomHolder == client) {
        senderName = "[房主]" + senderName;
    }
    ntf.set_sender_name(senderName);
    ntf.set_room_id(room->roomId);
    ntf.set_msg(msg);
    std::stringstream ss;
    ss << "[NotifyMessage] From " << client->GetAuth() << ", to ";
    for (auto &pair : room->participants) {
        if (pair.first == client) {
            continue;
        }
        if (pair.first->SendPack<recv_info_ntf>(1, ntf)) {
            ss << "[" << pair.first->GetAuth() << "] OK ";
        }
        else {
            ss << "[" << pair.first->GetAuth() << "] FAIL ";
        }
    }
    ss << "\n";
    printf(ss.str().c_str());
    return err;
}

int32_t RoomMgr::CreateNewRoom(Client* client, main::room_settings settings) {
    int32_t uuid = GenRoomUuid();
    auto p = new ChatRoom();
    p->roomId = uuid;
    p->settings = settings;
    p->settings.set_room_id(uuid); 
    p->roomHolder = client;
    main::join_settings joinSettings;
    joinSettings.set_join_name(std::to_string(uuid));
    p->participants[client] = std::move(joinSettings);
    m_mapRooms[uuid] = p;
    return uuid;
}

bool RoomMgr::GetAllRoomList(main::get_all_room_list_resp &ack) {
    for (auto &pair : m_mapRooms) {
        auto *p =ack.add_rooms();
        p->CopyFrom(pair.second->settings);
    }
    return true;
}

bool RoomMgr::GetRoomAllMembers(ChatRoom *room, main::get_room_all_member_resp &ack) {
    if (room == nullptr) {
        return true;
    }
    auto &members = room->participants;
    for (auto &pair : members) {
        std::string name = pair.second.join_name() + "(" + pair.first->GetAuth() + ")";
        if (room->roomHolder == pair.first) {
            name = "[房主]" + name;
        }
        ack.add_join_names(name);
    }
    printf("[GetRoomAllMembers] %d member(s) in total\n", ack.join_names_size());
    return true;
}

main::error_id RoomMgr::ChangeRoomSetting(Client* client, main::room_settings settings) {
    main::error_id err = err_none;
    ChatRoom *room = client->GetRoom();
    if (room->roomHolder != client) {
        return err_opt_disallowed_not_room_holder;
    }
    if (settings.has_open()) {
        room->settings.set_open(settings.open());
        printf("[ChangeRoomSetting] %d", settings.open());
    }
    return err;
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

