

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
    int listenfd;
};

}
