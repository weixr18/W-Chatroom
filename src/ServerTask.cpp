#include "ServerTask.h"
#include "InfoQueue.h"
#include "ThreadPool.h"

InfoQueue IQ;
InfoQueue::Info_hash info_hash;
const std::string ServerTask::EXIT_STR = "$THR_EXIT$";

void *ServerTask::WriteThreadCallback(int res)
{
    SOCKET connfd = GetSocket();
    printf("%d: Write thread exit.\n", connfd);
    close(connfd);
    std::string message = EXIT_STR;
    IQ.add(message, connfd);
    return nullptr;
}

int ServerTask::WriteThread(void *arg)
{
    SOCKET connfd = GetSocket();
    while (1)
    {
        char recvbuf[1024];
        fflush(stdout);
        memset(recvbuf, 0x00, sizeof(recvbuf));
        int len = recv(connfd, recvbuf, sizeof(recvbuf), 0);
        if (len <= 0)
        {
            return -1;
        }
        printf("%d: %s\n", connfd, recvbuf);

        std::string message(recvbuf);
        IQ.add(message, connfd);
    }
    return 0;
}

int ServerTask::ReadThread(void *arg)
{
    SOCKET connfd;
    try
    {
        TaskFun writefun = std::bind(&ServerTask::WriteThread, this, std::placeholders::_1);
        TaskCallbackFun writeCallback = std::bind(&ServerTask::WriteThreadCallback, this, std::placeholders::_1);
        Pool.QueueTaskItem(writefun, nullptr, writeCallback);

        connfd = GetSocket();
        printf("%d Connected.\n", connfd);
        size_t last_hash = 0;

        while (1)
        {
            char sendbuf[1024];
            memset(sendbuf, 0x00, sizeof(sendbuf));

            InfoQueue::Info info = IQ.read(last_hash, connfd);
            if (strncmp(info.message.c_str(), EXIT_STR.c_str(), EXIT_STR.length()) == 0)
            {
                printf("%d: Read thread exit.\n", connfd);
                connect_num--;
                close(connfd);
                return 0;
            }

            size_t hash_res = info_hash(info);
            if (hash_res != last_hash)
            {
                last_hash = hash_res;
                info.message = std::to_string(info.sock_number) + ":" + info.message;
                memcpy(sendbuf, info.message.c_str(), 1024);
                int sock_res = send(connfd, sendbuf, strlen(sendbuf), 0);
            }
        }
    }
    catch (std::exception e)
    {
        std::cerr << e.what() << std::endl;
        close(connfd);
        return -1;
    }

    return 0;
}