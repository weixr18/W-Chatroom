#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <hash_map>
#include <vector>

#include "Server.h"

#define SERVER_PORT 6501
#define BUF_SIZE 1024
#define MESG_SIZE 990 // "[192.168.127.131:4560]:"

SOCKET UDPSock = INVALID_SOCKET;
sockaddr_in currnt_addr = {0};
volatile char recvBuf[BUF_SIZE] = {0};

typedef __gnu_cxx::hash_map<size_t, sockaddr_in> SocketMap;

struct hash_sockaddr_in
{
    size_t operator()(const class sockaddr_in &s) const
    {
        return (s.sin_addr.s_addr << 8) + s.sin_port;
    }
} sock_hash;

SocketMap socketMap;


/*
 * Receive thread
*/
unsigned __stdcall UDPReceiveThread(void *arg)
{
    char tempBuf[BUF_SIZE];
    while (true)
    {
        memset((void*)tempBuf, 0x00, sizeof(tempBuf));
        sockaddr_in addr;
        int len = sizeof(sockaddr);
        int res =
            recvfrom(UDPSock, (char *)tempBuf, sizeof(tempBuf), 0, (sockaddr *)&addr, &len);
        if (res == SOCKET_ERROR)
        {
            printf("recvfrom error.\n");
            return -1;
        }
        if (strncmp("quit", (const char *)recvBuf, 4) == 0)
        {
            printf("%s Bye.", inet_ntoa(addr.sin_addr));
            socketMap.erase(socketMap.find(sock_hash(addr)));
        }
        else
        {
            memcpy((void*)recvBuf, tempBuf, sizeof(tempBuf));
            printf("[%s:%d]:%s\n", inet_ntoa(addr.sin_addr), addr.sin_port, recvBuf);
        }
        
        if (socketMap.find(sock_hash(addr)) == socketMap.end())
        {
            socketMap.insert(std::pair<size_t, sockaddr_in>(sock_hash(addr), addr));
        }
        currnt_addr = addr;
    }
    return 0;
}

/*
 * Main thread (send)
*/
int runUDPServer(int argc, char *argv[])
{
    // random seed initialize
    srand(time(NULL));

    // WSA startup
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    printf("Server start.\n");
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        printf("WSA startup error!\n");
        WSACleanup();
        return -1;
    }

    // create UDP socket
    UDPSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (UDPSock == INVALID_SOCKET)
    {
        printf("Socket error!\n");
        closesocket(UDPSock);
        WSACleanup();
        return -1;
    }

    // set server socket address information
    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.S_un.S_addr = INADDR_ANY; // inet_addr("127.0.0.1");

    // bind socket
    if (bind(UDPSock, (LPSOCKADDR)&serv_addr, sizeof(serv_addr)) ==
        SOCKET_ERROR)
    {
        printf("Bind error!\n");
        closesocket(UDPSock);
        WSACleanup();
        return -1;
    }

    // create receive thread
    HANDLE hThread;
    unsigned threadID;
    hThread =
        (HANDLE)_beginthreadex(NULL, 0, UDPReceiveThread, nullptr, 0, &threadID);
    if (hThread == NULL)
    {
        closesocket(UDPSock);
        WSACleanup();
        printf("Fail to create receive socket.\n");
        return -1;
    }
    else
    {
        printf("Server initialize success.\n");
    }

    // main loop
    size_t lastHash;
    char lastRecv[BUF_SIZE] = {0};
    char sendBuff[BUF_SIZE] = {0};

    while (1)
    {
        // sleep for a random time
        Sleep(rand() % 200);
        
        // check if has read the massage
        if (sock_hash(currnt_addr) == lastHash &&
            strncmp((const char*)recvBuf, lastRecv, MESG_SIZE) == 0)
        {
            continue;
        }
        lastHash = sock_hash(currnt_addr);
        memset(lastRecv, 0x00, sizeof(lastRecv));
        memcpy(lastRecv, (void*)recvBuf, MESG_SIZE);
        
        // prepare the send string
        memset(sendBuff, 0x00, sizeof(sendBuff));
        std::string prefix = "[" + std::string(inet_ntoa(currnt_addr.sin_addr)) +
                             ":" + std::to_string(currnt_addr.sin_port) + "]:";
        memcpy(sendBuff, prefix.c_str(), prefix.length());
        memcpy(sendBuff + prefix.length(), (void*)recvBuf, MESG_SIZE);
        
        // send to all other clients
        for (auto t = socketMap.begin(); t != socketMap.end(); t++)
        {
            if ((*t).first != sock_hash(currnt_addr))
            {
                sockaddr_in send_addr = (*t).second;
                int res = sendto(UDPSock, sendBuff, strlen(sendBuff), 0,
                                 (struct sockaddr *)&send_addr, sizeof(send_addr));
                if (res == SOCKET_ERROR)
                {
                    printf("Send error!!\n");
                }
            }
        }
    }

    //clean up
    close(UDPSock);
    fflush(stdout);
    WSACleanup();
    return 0;
}
