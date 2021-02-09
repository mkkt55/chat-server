# chat-server
这是一个聊天室Demo的服务器端


项目依赖
---
本项目依赖ProtoBuffer作为通信协议，因此若需要从源码编译和链接程序下载，需要到Github或ProtoBuffer官网安装依赖库。库中已经编译的二进制文件链接了静态的ProtoBuffer库，因此可以在Linux服务器上直接运行。

编译运行
---
安装好ProtoBuffer后，可以选择在项目根目录下使用bash build.sh或创建新文件夹bin后，使用cmake编译。若ProtoBuffer被编译为动态库，则运行时需要设置相应环境变量指向动态库的地址。
