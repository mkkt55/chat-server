

namespace chat {

enum RecvStatus {
    Empty,
    WaitHeader,
    WaitBody,
    Closed
};

class Header {
    public:
        int flag;
        int bodyLen;
};

class PackWrapper {

    public:
        PackWrapper(int fd);
        inline int GetFd() {
            return fd;
        }
        bool OnRecv();
        bool TryReadAndDeal();
        bool WritePack();

        void DebugInfo();
    private:
        int fd = -1;
        RecvStatus status;
        Header header;
        char recvBuf[1024] = {0};
        int recvLen = 0;
        char sendBuf[1024] = {0};
        int sendLen = 0;
};

}