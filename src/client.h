#pragma once
#include <string>
#include <unistd.h>
#include <list>
#include <unordered_map>
#include "sock_wrapper.h"

namespace chat {
class ChatRoom;

class Client {
  // Static usage, instead of a stand alone ClientMgr class
  public:
    static Client* BindOneAndRet(std::string, SockWrapper*);
    static bool UnbindConn(Client*, SockWrapper*);
    static bool OnLogin(Client* client);
    static bool OnLogout(Client* client);
    static bool ClearUnbind();
  private:
    static std::unordered_map<std::string, Client*> s_mapAuth2Client;
    static std::list<Client*> s_listUnbindClient;
  // Static usage end

  private:
    Client(std::string auth);
    ~Client() {};
  public:
    bool BindConn(SockWrapper* conn);
    void UpdateActiveTime();
    void OnRoomDismiss(int32_t roomId);
    void OnReceiveMsg(int32_t roomId, std::string senderName,  std::string msg);
    ChatRoom* GetRoom();
    bool SetRoom(ChatRoom* room);

    template <typename T>
    bool SendPack(char flag, T proto) {
        if (m_oConn == nullptr) {
            return false;
        }
        auto str = proto.SerializeAsString();
        m_oConn->SendPack(flag, proto.id(), str.size(), str.c_str());
    }
  private:
    std::string m_strAuth;
    SockWrapper *m_oConn = nullptr;
    ChatRoom* m_oRoom = nullptr;
    int32_t m_nUnbindTime = 0;
    int32_t m_oChatRoom = 0;
};
}