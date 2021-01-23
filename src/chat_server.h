#include<set>
namespace chat {

class PackWrapper;
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
    PackWrapper* m_oListenWrapper;
    set<PackWrapper*> m_setInsockWrapper;
};

}
