#include "client.h"
#include "sock_wrapper.h"
#include "room_mgr.h"

namespace chat {

std::unordered_map<std::string, Client*> Client::s_mapAuth2Client;

Client* Client::BindOneAndRet(std::string auth, SockWrapper* sw) {
    auto pClient = s_mapAuth2Client[auth];
    if (pClient == nullptr) {
        pClient = new Client(auth);
    }
    pClient->BindConn(sw);
    return pClient;
}

void Client::UpdateActiveTime() {
    lastActiveTime = 0;
}

bool Client::OnClientLeave(Client* client) {
    RoomMgr::Instance()->OnClientLeave(client);
    s_mapAuth2Client.erase(client->auth);
    return true;
}

Client::Client(std::string auth) {
    this->auth = auth;
}

bool Client::BindConn(SockWrapper* conn) {
    this->conn = conn;
    return true;
}

}