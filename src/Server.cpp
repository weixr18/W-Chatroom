#include "ThreadPool.h"
#include "InfoQueue.h"
#include "ServerTask.h"

//#define __USE_TCP
#ifdef __USE_TCP
#define USE_TCP true
#else //__USE_TCP
#define USE_TCP false
#endif //__USE_TCP

#define SERVER_PORT 6500
#define BUF_SIZE 1024

// UDP
SOCKET UDPSock;
in_addr UDP_addr;
u_short UDP_port;

//create thread pool
ThreadPool Pool(10);

unsigned __stdcall UDPReceiveThread(void *arg)
{
    char recvBuf[BUF_SIZE];
    while (true)
    {
        memset(recvBuf, 0x00, sizeof(recvBuf));
        struct sockaddr_in addr;
        int len = sizeof(sockaddr);
        int res = recvfrom(UDPSock, recvBuf, sizeof(recvBuf), 0, (struct sockaddr *)&addr, &len);
        if (res == SOCKET_ERROR)
        {
            return -1;
        }
        printf("[%s]Says:%s\n", inet_ntoa(addr.sin_addr), recvBuf);
        UDP_addr = addr.sin_addr;
        UDP_port = addr.sin_port;
    }
    return 0;
}

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
    SOCKET sockfd;
    if (USE_TCP)
    {
        sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    else
    {
        sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        UDPSock = sockfd;
    }

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
    ser.sin_port = htons(SERVER_PORT);
    ser.sin_addr.S_un.S_addr = INADDR_ANY; //inet_addr("127.0.0.1");

    // bind socket
    if (bind(sockfd, (LPSOCKADDR)&ser, sizeof(ser)) == SOCKET_ERROR)
    {
        printf("Bind error!\n");
    }
    else
    {
        printf("Bind success.\n");
    }

    if (USE_TCP)
    {
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
    }

    srand(time(NULL));

    if (USE_TCP)
    {
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
    }
    else
    {
        HANDLE hThread;
        unsigned threadID;
        hThread = (HANDLE)_beginthreadex(NULL, 0, UDPReceiveThread, nullptr, 0, &threadID);
        if (hThread == NULL)
        {
            closesocket(UDPSock);
            WSACleanup();
            printf("Fail to create receive socket.\n");
            return -1;
        }

        char sendBuff[BUF_SIZE];
        sockaddr_in addrObj;
        addrObj.sin_family = AF_INET;

        while (1)
        {
            memset(sendBuff, 0x00, sizeof(sendBuff));
            gets(sendBuff);
            addrObj.sin_addr = UDP_addr;
            addrObj.sin_port = UDP_port;
            int res = sendto(UDPSock, sendBuff, strlen(sendBuff), 0, (struct sockaddr *)&addrObj, sizeof(addrObj));
            if (res == SOCKET_ERROR)
            {
                printf("Send error!!\n");
            }
        }
    }

    close(sockfd);
    WSACleanup();
    return 0;
}
