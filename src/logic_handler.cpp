#include "logic_handler.h"
#include <cstdio>
#include "sock_wrapper.h"

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

    using namespace main;
    
    switch (pPack->protoId)
    {
    
    case login_req_id:
        return handle_login_req_id(pPack->buffer, pPack->pConn);
        break;
    case create_room_req_id:
        return handle_create_room_req_id(pPack->buffer, pPack->pConn);
        break;
    case dismiss_room_req_id:
        return handle_dismiss_room_req_id(pPack->buffer, pPack->pConn);
        break;
    case change_room_settings_req_id:
        return handle_change_room_settings_req_id(pPack->buffer, pPack->pConn);
        break;
    case join_room_req_id:
        return handle_join_room_req_id(pPack->buffer, pPack->pConn);
        break;
    case change_join_name_req_id:
        return handle_change_join_name_req_id(pPack->buffer, pPack->pConn);
        break;
    case send_info_req_id:
        return handle_send_info_req_id(pPack->buffer, pPack->pConn);
        break;
    case exit_room_req_id:
        return handle_exit_room_req_id(pPack->buffer, pPack->pConn);
        break;
    default:
        printf("Receive unregistered proto id: %d", pPack->protoId);
        return false;
        break;
    }
    return true;
}

bool LogicHandler::handle_login_req_id(char* pData, SockWrapper* conn) {
    main::login_req req;
    req.ParseFromString(pData);
    main::login_resp ack;

    ack.set_auth("");

    conn->WritePack();
}

bool LogicHandler::handle_create_room_req_id(char* pData, SockWrapper* conn) {}
bool LogicHandler::handle_dismiss_room_req_id(char* pData, SockWrapper* conn) {}
bool LogicHandler::handle_change_room_settings_req_id(char* pData, SockWrapper* conn) {}

bool LogicHandler::handle_join_room_req_id(char* pData, SockWrapper* conn) {}
bool LogicHandler::handle_change_join_name_req_id(char* pData, SockWrapper* conn) {}
bool LogicHandler::handle_send_info_req_id(char* pData, SockWrapper* conn) {}
bool LogicHandler::handle_exit_room_req_id(char* pData, SockWrapper* conn) {}


} // namespace chat
