#include "ServerTask.h"
#include "InfoQueue.h"
#include "ThreadPool.h"

InfoQueue IQ;
InfoQueue::Info_hash info_hash;

void *ServerTask::WriteThreadCallback(int res)
{

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
            printf("%d: No buf. Write thread exit.\n", connfd);
            close(connfd);
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
        Pool.QueueTaskItem(writefun, nullptr, nullptr);

        connfd = GetSocket();
        printf("%d Connected.\n", connfd);
        size_t last_hash = 0;

        while (1)
        {
            char sendbuf[1024];
            memset(sendbuf, 0x00, sizeof(sendbuf));
            InfoQueue::Info info;
            info = IQ.read(last_hash, connfd);
            size_t res = info_hash(info);
            if (res != last_hash)
            {
                last_hash = res;
                info.message = std::to_string(info.sock_number) + ":" + info.message;
                memcpy(sendbuf, info.message.c_str(), 1024);
                int sock_res = send(connfd, sendbuf, sizeof(sendbuf), 0);
                if (sock_res == SOCKET_ERROR)
                {
                    printf("%d: Read thread exit.\n", connfd);
                    close(connfd);
                    break;
                }
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