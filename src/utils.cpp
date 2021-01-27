#include<ctime>

namespace chat {

int GenUuid() {
    return time(0);
}

}