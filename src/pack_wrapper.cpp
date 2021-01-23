#include "pack_wrapper.h"
#include <unistd.h>
#include <cstdio>
#include <sys/errno.h>

using namespace std;

namespace chat {

PackWrapper::PackWrapper(int _fd) {
    fd = _fd;
}

bool PackWrapper::OnRecv() {
    int n = read(fd, recvBuf, 1024);
    if (n == 0) {
        if (close(fd) == -1) {
            printf("Close socket %d error... errno: %d", fd, errno);
        }
        return false;
    }
    if (n == -1) {
        printf("Read socket %d error... errno: %d", fd, errno);
        if (close(fd) == -1) {
            printf("Close socket %d error... errno: %d", fd, errno);
        }
        return false;
    }

    recvLen += n;

    TryReadAndDeal();
    return true;
}

bool PackWrapper::TryReadAndDeal() {
    while (1) {
        bool loop = false;
        switch (status)
        {
        case Empty:
        case WaitHeader:
            if (recvLen > sizeof(Header)) {
                header = ParseHeader();
                status = WaitBody;
                loop = true;
            }
            break;
        case WaitBody:
            if (recvLen > sizeof(Header) + header.bodyLen) {
                DealOnePack();
                status = WaitHeader;
                loop = true;
            }
        default:
            break;
        }
        if (!loop) {
            break;
        }
    }

    
    // main::login_req loginReq;
    // loginReq.set_id(main::login_resp_id);
    // if (!loginReq.ParseFromArray(recvBuf, strlen(recvBuf))) {
    //     std::cout << "Parse proto fail...";
    // }
    // // snprintf(sendBuf, sizeof(sendBuf), "Hello, your file descriptor: ", connfd, "\n");
    // // write(connfd, sendBuf, strlen(sendBuf));
    // // std::cout << sendBuf << std::endl;
    // std::cout << "Get pack id: " << loginReq.id() << "\n";
    return true;
}

bool PackWrapper::WritePack() {
    return true;
}
}