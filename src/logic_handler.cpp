#include "logic_handler.h"
#include <cstdio>
#include "cs.pb.h"
#include "room_mgr.h"

using namespace main;

namespace chat
{
LogicHandler *LogicHandler::instance = nullptr;
LogicHandler *LogicHandler::Instance() {
    if (instance == nullptr) {
        instance = new LogicHandler();
        if (instance == nullptr || !instance->init()) {
            std::printf("Fail when init LogicHandler");
            exit(1);
        }
    }
    return instance;
}

LogicHandler::LogicHandler() {
    //
}

bool LogicHandler::init() {
    //
    return true;
}

bool LogicHandler::HandlePack(NetPack *pPack) {
    if (pPack == nullptr) {
        return false;
    }
    printf("Handled one pack, pPack->protoId: %d\n", pPack->protoId);
    
    switch (pPack->protoId)
    {
    case create_room_req_id:
        return handle_create_room_req_id(pPack->buffer, pPack->len, pPack->pClient);
        break;
    case get_all_room_list_req_id:
        return handle_get_all_room_list_req_id(pPack->buffer, pPack->len, pPack->pClient);
        break;
    case dismiss_room_req_id:
        return handle_dismiss_room_req_id(pPack->buffer, pPack->len, pPack->pClient);
        break;
    case change_room_settings_req_id:
        return handle_change_room_settings_req_id(pPack->buffer, pPack->len, pPack->pClient);
        break;
    case join_room_req_id:
        return handle_join_room_req_id(pPack->buffer, pPack->len, pPack->pClient);
        break;
    case change_join_settings_req_id:
        return handle_change_join_settings_req_id(pPack->buffer, pPack->len, pPack->pClient);
        break;
    case send_info_req_id:
        return handle_send_info_req_id(pPack->buffer, pPack->len, pPack->pClient);
        break;
    case exit_room_req_id:
        return handle_exit_room_req_id(pPack->buffer, pPack->len, pPack->pClient);
        break;
    default:
        printf("Receive unregistered proto id: %d\n", pPack->protoId);
        return false;
        break;
    }
    return true;
}

bool LogicHandler::handle_create_room_req_id(char* pData, int len, Client* pClient) {
    create_room_req req;
    create_room_resp ack;
    if (!req.ParseFromArray(pData, len)){
        ack.set_error(err_parsing_proto);
        pClient->SendPack<create_room_resp>(12, ack);
        printf("Handle pack FAIL, pack: %s\n", req.DebugString().c_str());
        return false;
    }
    int32_t roomId = RoomMgr::Instance()->CreateNewRoom(pClient, req.settings());
    ack.set_new_room_id(roomId);
    ack.set_error(err_none);
    pClient->SendPack<create_room_resp>(12, ack);
    printf("Handle pack OK, pack: %s\n", req.DebugString().c_str());
    return true;
}

bool LogicHandler::handle_get_all_room_list_req_id(char* pData, int len, Client* pClient) {
    get_all_room_list_req req;
    get_all_room_list_resp ack;
    if (!req.ParseFromArray(pData, len)){
        printf("Handle pack FAIL, pack: %s\n", req.DebugString().c_str());
        return false;
    }
    if (!RoomMgr::Instance()->GetAllRoomList(ack)) {
        printf("GetAllRoomList FAIL, pack: %s\n", req.DebugString().c_str());
        return false;
    }
    pClient->SendPack<get_all_room_list_resp>(12, ack);
    printf("Handle pack OK, pack: %s\n", req.DebugString().c_str());
    return true;
}

bool LogicHandler::handle_dismiss_room_req_id(char* pData, int len, Client* pClient) {
    dismiss_room_req req;
    dismiss_room_resp ack;
    if (!req.ParseFromArray(pData, len)){
        ack.set_error(err_parsing_proto);
        pClient->SendPack<dismiss_room_resp>(12, ack);
        return false;
    }
    RoomMgr::Instance()->DismissRoom(pClient, req.room_id(), ack);
    pClient->SendPack<dismiss_room_resp>(12, ack);
    printf("Handle pack OK, pack: %s\n", req.DebugString().c_str());
    return true;
}

bool LogicHandler::handle_change_room_settings_req_id(char* pData, int len, Client* pClient) {
    change_room_settings_req req;
    change_room_settings_resp ack;
    if (!req.ParseFromArray(pData, len)){
        ack.set_error(err_parsing_proto);
        pClient->SendPack<change_room_settings_resp>(12, ack);
        return false;
    }
    ack.set_error(err_none);
    pClient->SendPack<change_room_settings_resp>(12, ack);
    printf("Handle pack OK, pack: %s\n", req.DebugString().c_str());
    return true;
}

bool LogicHandler::handle_join_room_req_id(char* pData, int len, Client* pClient) {
    join_room_req req;
    join_room_resp ack;
    if (!req.ParseFromArray(pData, len)){
        ack.set_error(err_parsing_proto);
        pClient->SendPack<join_room_resp>(12, ack);
        return false;
    }
    error_id err = RoomMgr::Instance()->ClientJoinRoom(pClient, req.room_id(), req.settings());
    ack.set_error(err);
    pClient->SendPack<join_room_resp>(12, ack);
    printf("Handle pack OK, pack: %s\n", req.DebugString().c_str());
    return true;
}

bool LogicHandler::handle_change_join_settings_req_id(char* pData, int len, Client* pClient) {
    change_join_settings_req req;
    change_join_settings_resp ack;
    if (!req.ParseFromArray(pData, len)){
        ack.set_error(err_parsing_proto);
        pClient->SendPack<change_join_settings_resp>(12, ack);
        return false;
    }
    ack.set_error(err_none);
    pClient->SendPack<change_join_settings_resp>(12, ack);
    printf("Handle pack OK, pack: %s\n", req.DebugString().c_str());
    return true;
}

bool LogicHandler::handle_send_info_req_id(char* pData, int len, Client* pClient) {
    send_info_req req;
    send_info_resp ack;
    if (!req.ParseFromArray(pData, len)){
        ack.set_error(err_parsing_proto);
        pClient->SendPack<send_info_resp>(12, ack);
        return false;
    }
    // printf("[Trace] %s, 1\n", __FUNCTION__);
    error_id err = RoomMgr::Instance()->OnClientMsg(pClient, req.info());
    ack.set_error(err);
    pClient->SendPack<send_info_resp>(12, ack);
    printf("Handle pack OK, pack: %s\n", req.DebugString().c_str());
    return true;
}

bool LogicHandler::handle_exit_room_req_id(char* pData, int len, Client* pClient) {
    exit_room_req req;
    exit_room_resp ack;
    if (!req.ParseFromArray(pData, len)){
        ack.set_error(err_parsing_proto);
        pClient->SendPack<exit_room_resp>(12, ack);
        return false;
    }
    auto err = RoomMgr::Instance()->ClientExitRoom(pClient, req.room_id());
    ack.set_error(err);
    pClient->SendPack<exit_room_resp>(12, ack);
    printf("Handle pack OK, pack: %s\n", req.DebugString().c_str());
    return true;
}

} // namespace chat
