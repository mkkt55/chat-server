#pragma once
#include<unordered_map>
#include "cs.pb.h"
#include "pack_queue.h"

namespace chat
{
class LogicHandler {
public:
    static LogicHandler* Instance();
private:
    static LogicHandler *instance;
    LogicHandler();
public:
    bool HandlePack(NetPack *pack);
private:
    bool init();

    bool handle_login_req_id(char* pData, SockWrapper* conn);

    bool handle_create_room_req_id(char* pData, SockWrapper* conn);
    bool handle_dismiss_room_req_id(char* pData, SockWrapper* conn);
    bool handle_change_room_settings_req_id(char* pData, SockWrapper* conn);

    bool handle_join_room_req_id(char* pData, SockWrapper* conn);
    bool handle_change_join_name_req_id(char* pData, SockWrapper* conn);
    bool handle_send_info_req_id(char* pData, SockWrapper* conn);
    bool handle_exit_room_req_id(char* pData, SockWrapper* conn);

    // std::unordered_map<main::proto_id, std::function<bool(char*)>> m_mapDispatcher;
};

} // namespace chat
