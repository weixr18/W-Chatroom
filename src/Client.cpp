#include <iostream>
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <winsock.h>
#include <sys/types.h>
#include <string.h>

#define SERVER_PORT 6500

SOCKET Client_Sock;

unsigned __stdcall PrintThread(void *arg)
{
    while (1)
    {
        char buff[1024] = {0};
        fflush(stdout);
        memset(buff, 0, 1024);
        int res = recv(Client_Sock, buff, 1023, 0);
        if (res <= 0)
        {
            printf("Server end connection.\n");
            break;
        }

        printf("%s\n>", buff);
    }
    return 0;
}

int main()
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
    hThread = (HANDLE)_beginthreadex(NULL, 0, PrintThread, nullptr, 0, &threadID);
    if (hThread == NULL)
    {
        printf("Thread create error!\n");
        return -1;
    }

    while (1)
    {
        char buff[1024] = {0};
        fgets(buff, 1023, stdin);
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