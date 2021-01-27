#include "client.h"
#include "sock_wrapper.h"

namespace chat {

Client* Client::BindOneAndRet(std::string auth, SockWrapper* sw) {
    auto pClient = s_mapAuth2Client[auth];
    if (pClient == nullptr) {
        pClient = new Client(auth);
    }
    pClient->BindConn(sw);
    return pClient;
}

Client::Client(std::string auth) {
    this->auth = auth;
}

bool Client::BindConn(SockWrapper* conn) {
    this->conn = conn;
    return this;
}

}