#include <iostream>
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <winsock.h>
#include <sys/types.h>
#include <string.h>

sockaddr UDPServerAddr;

#define SERVER_PORT 6500
SOCKET Client_Sock;
#define BUF_SIZE 1024

unsigned __stdcall TCPPrintThread(void *arg)
{
    while (1)
    {
        char buff[BUF_SIZE] = {0};
        fflush(stdout);
        memset(buff, 0, BUF_SIZE);
        int res = recv(Client_Sock, buff, BUF_SIZE - 1, 0);
        if (res <= 0)
        {
            printf("Server end connection.\n");
            break;
        }

        printf("%s\n>", buff);
    }
    return 0;
}

unsigned __stdcall UDPPrintThread(void *arg)
{

    sockaddr_in addrObj;
    addrObj.sin_family = AF_INET;
    addrObj.sin_port = htons(SERVER_PORT);
    int len = sizeof(sockaddr);

    while (1)
    {
        char buff[BUF_SIZE] = {0};
        fflush(stdout);
        memset(buff, 0, BUF_SIZE);
        int res = recvfrom(Client_Sock, buff, BUF_SIZE - 1, 0, &UDPServerAddr, &len);
        if (res == SOCKET_ERROR)
        {
            printf("Server end connection.\n");
            printf("errno = %d\n", GetLastError());
            break;
        }

        printf("%s\n>", buff);
    }
    return 0;
}

int TCPClient(int argc, char *argv[])
{
    // Input server IP
    char serverIPAddr[20] = {0};
    printf("Server IP address:");
    gets(serverIPAddr);
    unsigned long server_addr = inet_addr(serverIPAddr);

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
    Client_Sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Client_Sock == INVALID_SOCKET)
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
    memset(&ser, 0x00, sizeof(ser));
    ser.sin_family = AF_INET;
    ser.sin_addr.S_un.S_addr = server_addr;
    ser.sin_port = htons(SERVER_PORT);

    // connect socket
    if (connect(Client_Sock, (struct sockaddr *)&ser, sizeof(ser)) == SOCKET_ERROR)
    {
        printf("Connect error!\n");
        return -1;
    }
    else
    {
        printf("Connect success.\n");
    }

    // create thread
    HANDLE hThread;
    unsigned threadID;
    hThread = (HANDLE)_beginthreadex(NULL, 0, TCPPrintThread, nullptr, 0, &threadID);
    if (hThread == NULL)
    {
        printf("Thread create error!\n");
        return -1;
    }

    while (1)
    {
        char buff[BUF_SIZE] = {0};
        fgets(buff, BUF_SIZE - 1, stdin);
        printf(">");
        if (strncmp(buff, "quit", 4) == 0)
        {
            printf("Connection close. Bye.\n");
            close(Client_Sock);
            break;
        }
        send(Client_Sock, buff, strlen(buff) - 1, 0);
        memset(buff, 0, 1024);
    }

    close(Client_Sock);
    return 0;
}

int UDPClient(int argc, char *argv[])
{
    // Input server IP
    char serverIPAddr[20] = {0};
    printf("Server IP address:");
    gets(serverIPAddr);
    unsigned long server_addr = inet_addr(serverIPAddr);

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

    // create UDP socket
    Client_Sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (Client_Sock == INVALID_SOCKET)
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
    memset(&ser, 0x00, sizeof(ser));
    ser.sin_family = AF_INET;
    ser.sin_addr.S_un.S_addr = server_addr;
    ser.sin_port = htons(SERVER_PORT);

    memcpy(&UDPServerAddr, (struct sockaddr *)&ser, sizeof(struct sockaddr));

    // connect socket
    if (connect(Client_Sock, (struct sockaddr *)&ser, sizeof(ser)) == SOCKET_ERROR)
    {
        printf("Connect error!\n");
        return -1;
    }
    else
    {
        printf("Connect success.\n");
    }

    // create thread
    HANDLE hThread;
    unsigned threadID;
    hThread = (HANDLE)_beginthreadex(NULL, 0, UDPPrintThread, nullptr, 0, &threadID);
    if (hThread == NULL)
    {
        printf("Thread create error!\n");
        return -1;
    }

    while (1)
    {
        char buff[BUF_SIZE] = {0};
        fgets(buff, BUF_SIZE - 1, stdin);
        printf(">");
        if (strncmp(buff, "quit", 4) == 0)
        {
            printf("Connection close. Bye.\n");
            close(Client_Sock);
            break;
        }
        send(Client_Sock, buff, strlen(buff) - 1, 0);
        memset(buff, 0, 1024);
    }

    close(Client_Sock);
    return 0;
}

int main(int argc, char *argv[])
{
    printf("Choose protocal: TCP(T)/UDP(U) ?");
    char s[10] = {0};
    gets(s);
    if (s[0] == 'T')
    {
        TCPClient(argc, argv);
    }
    else if (s[0] == 'U')
    {
        UDPClient(argc, argv);
    }
}