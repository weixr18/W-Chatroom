#pragma once

#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <sys/types.h>
#include <string.h>
#include <assert.h>

class ServerTask
{
private:
public:
    ServerTask() = default;

    SOCKET socket;

    inline SOCKET GetSocket()
    {
        return this->socket;
    }

    inline void SetSocket(SOCKET &so)
    {
        this->socket = so;
    }

    int WriteThread(void *arg);

    int ReadThread(void *arg);

    void *WriteThreadCallback(int res);
};
