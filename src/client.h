#pragma once
#include <string>
#include <unistd.h>
#include <unordered_map>
#include "sock_wrapper.h"

namespace chat {
class SockWrapper;

class Client {
  // Static usage, instead of a stand alone ClientMgr class
  public:
    static Client* BindOneAndRet(std::string, SockWrapper*);
    static bool OnLogin(Client* client);
    static bool OnLogout(Client* client);
  private:
    static std::unordered_map<std::string, Client*> s_mapAuth2Client;
  // Static usage end

  private:
    Client(std::string auth);
  public:
    bool BindConn(SockWrapper* conn);
    void UpdateActiveTime();
    void OnRoomDismiss(int32_t roomId);
    void OnReceiveMsg(int32_t roomId, std::string msg);
    
    template <typename T>
    bool SendPack(char flag, T proto) {
        if (conn == nullptr) {
            return false;
        }
        auto str = proto.SerializeAsString();
        conn->SendPack(flag, proto.id(), str.size(), str.c_str());
    }
  private:
    std::string auth;
    SockWrapper *conn = nullptr;
    int lastActiveTime = 0;
};
}