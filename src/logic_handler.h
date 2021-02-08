#pragma once
#include<unordered_map>
#include "sock_wrapper.h"
#include "client.h"

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

    bool handle_get_all_room_list_req_id(char* pData, int len, Client* pClient);
    bool handle_create_room_req_id(char* pData, int len, Client* conn);
    bool handle_dismiss_room_req_id(char* pData, int len, Client* conn);
    bool handle_change_room_settings_req_id(char* pData, int len, Client* conn);

    bool handle_join_room_req_id(char* pData, int len, Client* conn);
    bool handle_get_room_all_member_req_id(char* pData, int len, Client* conn);
    bool handle_change_join_settings_req_id(char* pData, int len, Client* conn);
    bool handle_send_info_req_id(char* pData, int len, Client* conn);
    bool handle_exit_room_req_id(char* pData, int len, Client* conn);

    // std::unordered_map<main::proto_id, std::function<bool(char*)>> m_mapDispatcher;
};

} // namespace chat
