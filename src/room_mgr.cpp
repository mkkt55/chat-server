#include "room_mgr.h"

namespace chat{

RoomMgr* RoomMgr::instance = nullptr;
RoomMgr* RoomMgr::Instance() {
    if (instance == nullptr) {
        instance = new RoomMgr();
    }
    return instance; 
}

RoomMgr::RoomMgr() {
    //
}

bool RoomMgr::OnClientEnter(Client* client){
    return true;
}

bool RoomMgr::OnClientLeave(Client* client) {
    return true;
}

bool RoomMgr::OnClientMsg(Client* client, std::string msg) {
    return true;
}

}

