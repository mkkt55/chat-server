#pragma once
#include <string>
#include <unordered_map>
#include "sock_wrapper.h"

namespace chat {
class SockWrapper;

class Client {
  public:
    static Client* BindOneAndRet(std::string, SockWrapper*);
  private:
    static unordered_map<std::string, Client*> s_mapAuth2Client;
  private:
    Client(std::string auth);
  public:
    bool BindConn(SockWrapper* conn);
    
    template <typename T>
    bool SendPack(char flag, T proto) {
        if (conn == nullptr) {
            return false;
        }
        auto str = proto.SerializeAsString();
        conn->SendPack(flag, proto.id(), str.size(), str.c_str());
    }
  private:
    std::string auth = 0;
    SockWrapper *conn = nullptr;
};
}