#include "client.h"
#include "sock_wrapper.h"
#include "room_mgr.h"
#include "utils.h"
#include "room_mgr.h"

namespace chat {

std::unordered_map<std::string, Client*> Client::s_mapAuth2Client;
std::list<Client*> Client::s_listUnbindClient;

Client* Client::BindOneAndRet(std::string auth, SockWrapper* sw) {
    auto pClient = s_mapAuth2Client[auth];
    if (pClient == nullptr) {
        printf("[NewClient] auth: %s\n", auth.c_str());
        pClient = new Client(auth);
    }
    else {
        printf("[RebindClient] auth: %s\n", auth.c_str());
    }
    pClient->BindConn(sw);
    return pClient;
}

bool Client::UnbindConn(Client* client, SockWrapper* conn) {
    if (client == nullptr || conn == nullptr) {
        printf("[WARNING] Attemp to unbind client %p with conn %p\n", client, conn);
        return false;
    }
    if (client->m_oConn != conn) {
        return false;
    } 
    client->m_oConn = nullptr;
    client->m_nUnbindTime = GetNowTime();
    s_listUnbindClient.push_back(client);
    return true;
}

bool Client::ClearUnbind() {
    int32_t nNow = GetNowTime();
    auto it = s_listUnbindClient.begin();
    int count = 0;
    while (it != s_listUnbindClient.end()) {
        if ((*it)->m_nUnbindTime == 0) {
            s_listUnbindClient.pop_front();
            printf("[ClearClient] %s已重连，不用delete\n", (*it)->m_strAuth.c_str());
        }
        else if (nNow - (*it)->m_nUnbindTime >= 0) {
            printf("[ClearClient] %s已下线\n", (*it)->m_strAuth.c_str());
            OnLogout(*it);
            if (s_mapAuth2Client.erase((*it)->m_strAuth.c_str())) {
                printf("[Error] %s, %s, %s\n", __FILE__, __LINE__, __FUNCTION__);
            }
            delete (*it);
            s_listUnbindClient.pop_front();
            count++;
        }
        else {
            break;
        }
        it = s_listUnbindClient.begin();
    }
    if (count > 0) {
        printf("[ClearClient] 完成清除下线客户端，剩余%d在线\n", s_mapAuth2Client.size());
    }
    printf("[AllClients] ");
    for (auto &pair : s_mapAuth2Client) {
        printf("auth[%s]<=>Client[%p] ", pair.first.c_str(), pair.second);
    }
    printf("\n");
    return true;
}

void Client::OnRoomDismiss(int32_t roomId) {
    // 不做处理，等用户试图发消息的时候返回一个错误
}

void Client::OnReceiveMsg(int32_t roomId, std::string senderName, std::string msg) {
    main::recv_info_ntf ntf;
    ntf.set_room_id(roomId);
    ntf.set_sender_name(senderName);
    ntf.set_msg(msg);
    SendPack<main::recv_info_ntf>(1, ntf);
}

bool Client::OnLogin(Client* client) {
    return true;
}

bool Client::OnLogout(Client* client) {
    RoomMgr::Instance()->OnClientLogout(client);
    s_mapAuth2Client.erase(client->m_strAuth);
    return true;
}

Client::Client(std::string auth) {
    this->m_strAuth = auth;
}

bool Client::BindConn(SockWrapper* conn) {
    this->m_nUnbindTime = 0;
    this->m_oConn = conn;
    return true;
}

ChatRoom* Client::GetRoom() {
    return m_oRoom;
}

bool Client::SetRoom(ChatRoom* room) {
    m_oRoom = room;
    return true;
}

}