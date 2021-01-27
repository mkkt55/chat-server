#include "logic_handler.h"
#include <cstdio>
#include "sock_wrapper.h"

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
    case login_req_id:
        return handle_login_req_id(pPack->buffer, pPack->len, pPack->pConn);
        break;
    case create_room_req_id:
        return handle_create_room_req_id(pPack->buffer, pPack->len, pPack->pConn);
        break;
    case dismiss_room_req_id:
        return handle_dismiss_room_req_id(pPack->buffer, pPack->len, pPack->pConn);
        break;
    case change_room_settings_req_id:
        return handle_change_room_settings_req_id(pPack->buffer, pPack->len, pPack->pConn);
        break;
    case join_room_req_id:
        return handle_join_room_req_id(pPack->buffer, pPack->len, pPack->pConn);
        break;
    case change_join_settings_req_id:
        return handle_change_join_settings_req_id(pPack->buffer, pPack->len, pPack->pConn);
        break;
    case send_info_req_id:
        return handle_send_info_req_id(pPack->buffer, pPack->len, pPack->pConn);
        break;
    case exit_room_req_id:
        return handle_exit_room_req_id(pPack->buffer, pPack->len, pPack->pConn);
        break;
    default:
        printf("Receive unregistered proto id: %d\n", pPack->protoId);
        return false;
        break;
    }
    return true;
}

bool LogicHandler::handle_login_req_id(char* pData, int len, SockWrapper* conn) {
    login_req req;
    login_resp ack;
    if (!req.ParseFromArray(pData, len)) {
        ack.set_error(err_parsing_proto);
        ack.set_auth("");
        conn->SendPack<login_resp>(12, ack);
        printf("Handle pack FAIL, pack: %s\n", req.DebugString().c_str());
        return false;
    }
    ack.set_error(err_none);
    ack.set_auth("Hi");
    conn->SendPack<login_resp>(12, ack);
    printf("Handle pack OK, pack: %s\n", req.DebugString().c_str());
    return true;
}

bool LogicHandler::handle_create_room_req_id(char* pData, int len, SockWrapper* conn) {
    create_room_req req;
    create_room_resp ack;
    if (!req.ParseFromArray(pData, len)){
        ack.set_error(err_parsing_proto);
        conn->SendPack<create_room_resp>(12, ack);
        printf("Handle pack FAIL, pack: %s\n", req.DebugString().c_str());
        return false;
    }
    ack.set_error(err_none);
    conn->SendPack<create_room_resp>(12, ack);
    printf("Handle pack OK, pack: %s\n", req.DebugString().c_str());
    return true;
}

bool LogicHandler::handle_dismiss_room_req_id(char* pData, int len, SockWrapper* conn) {
    dismiss_room_req req;
    dismiss_room_resp ack;
    if (!req.ParseFromArray(pData, len)){
        ack.set_error(err_parsing_proto);
        conn->SendPack<dismiss_room_resp>(12, ack);
        return false;
    }
    ack.set_error(err_none);
    conn->SendPack<dismiss_room_resp>(12, ack);
    printf("Handle pack OK, pack: %s\n", req.DebugString().c_str());
    return true;
}

bool LogicHandler::handle_change_room_settings_req_id(char* pData, int len, SockWrapper* conn) {
    change_room_settings_req req;
    change_room_settings_resp ack;
    if (!req.ParseFromArray(pData, len)){
        ack.set_error(err_parsing_proto);
        conn->SendPack<change_room_settings_resp>(12, ack);
        return false;
    }
    ack.set_error(err_none);
    conn->SendPack<change_room_settings_resp>(12, ack);
    printf("Handle pack OK, pack: %s\n", req.DebugString().c_str());
    return true;
}

bool LogicHandler::handle_join_room_req_id(char* pData, int len, SockWrapper* conn) {
    join_room_req req;
    join_room_resp ack;
    if (!req.ParseFromArray(pData, len)){
        ack.set_error(err_parsing_proto);
        conn->SendPack<join_room_resp>(12, ack);
        return false;
    }
    ack.set_error(err_none);
    conn->SendPack<join_room_resp>(12, ack);
    printf("Handle pack OK, pack: %s\n", req.DebugString().c_str());
    return true;
}

bool LogicHandler::handle_change_join_settings_req_id(char* pData, int len, SockWrapper* conn) {
    change_join_settings_req req;
    change_join_settings_resp ack;
    if (!req.ParseFromArray(pData, len)){
        ack.set_error(err_parsing_proto);
        conn->SendPack<change_join_settings_resp>(12, ack);
        return false;
    }
    ack.set_error(err_none);
    conn->SendPack<change_join_settings_resp>(12, ack);
    printf("Handle pack OK, pack: %s\n", req.DebugString().c_str());
    return true;
}

bool LogicHandler::handle_send_info_req_id(char* pData, int len, SockWrapper* conn) {
    send_info_req req;
    send_info_resp ack;
    if (!req.ParseFromArray(pData, len)){
        ack.set_error(err_parsing_proto);
        conn->SendPack<send_info_resp>(12, ack);
        return false;
    }
    ack.set_error(err_none);
    conn->SendPack<send_info_resp>(12, ack);
    printf("Handle pack OK, pack: %s\n", req.DebugString().c_str());
    return true;
}

bool LogicHandler::handle_exit_room_req_id(char* pData, int len, SockWrapper* conn) {
    exit_room_req req;
    exit_room_resp ack;
    if (!req.ParseFromArray(pData, len)){
        ack.set_error(err_parsing_proto);
        conn->SendPack<exit_room_resp>(12, ack);
        return false;
    }
    ack.set_error(err_none);
    conn->SendPack<exit_room_resp>(12, ack);
    printf("Handle pack OK, pack: %s\n", req.DebugString().c_str());
    return true;
}

} // namespace chat
