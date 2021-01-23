#pragma once
#include<set>
#include "sock_wrapper.h"

namespace chat {

class CChatServer {
  public:
    CChatServer() {}
    ~CChatServer() {}
    void PrintInfo();
    bool Init();
    bool Run();
    bool Stop();
  private:
    int epollfd;
    SockWrapper* m_oListenWrapper;
    std::set<SockWrapper*> m_setInsockWrapper;
};

}
