#include "ThreadPool.h"
#include "InfoQueue.h"
#include "ServerTask.h"

//create thread pool
ThreadPool Pool(10);

int main(int argc, char *argv[])
{
    // startup
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    printf("Server start.\n");
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        printf("WSA startup error!\n");
        return -1;
    }
    else
    {
        printf("WSA started.\n");
    }

    // create socket
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET)
    {
        printf("Socket error!\n");
        return -1;
    }
    else
    {
        printf("Socket created.\n");
    }

    // set socket address information
    struct sockaddr_in ser, cli;
    memset(&ser, 0, sizeof(ser));
    ser.sin_family = AF_INET;
    ser.sin_port = htons(6500);
    ser.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    // bind socket
    if (bind(sockfd, (LPSOCKADDR)&ser, sizeof(ser)) == SOCKET_ERROR)
    {
        printf("Bind error!\n");
    }
    else
    {
        printf("Bind success.\n");
    }

    // listen
    if (listen(sockfd, 5) == SOCKET_ERROR)
    {
        printf("Listen error!\n");
        return 0;
    }
    else
    {
        printf("Start listen.\n");
    }

    srand(time(NULL));

    while (1)
    {
        socklen_t len = sizeof(cli);
        SOCKET connectfd = accept(sockfd, (struct sockaddr *)&cli, &len);
        if (connectfd < 0)
        {
            printf("Cli connect failed.\n");
            throw std::exception();
        }
        else
        {
            ServerTask *ta = new ServerTask;
            ta->SetSocket(connectfd);
            TaskFun fun = std::bind(&ServerTask::ReadThread, ta, std::placeholders::_1);
            Pool.QueueTaskItem(fun, nullptr, nullptr);
            connect_num++;
        }
    }

    close(sockfd);
    return 0;
}
