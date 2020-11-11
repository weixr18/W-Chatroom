#include "InfoQueue.h"

std::hash<std::string> InfoQueue::string_hash;
std::hash<size_t> InfoQueue::sizet_hash;
std::hash<time_t> InfoQueue::time_hash;
InfoQueue::Info latest_info;
volatile size_t connect_num = 0;
volatile size_t read_num = 0;
const std::string InfoQueue::EXIT_STR = "$THR_EXIT$";

InfoQueue::InfoQueue()
{
}

InfoQueue::Info InfoQueue::read(size_t last_hash, SOCKET sock)
{
    Info info;
    while (true)
    {
        queueLock.Lock();

        // thread exit
        if (strncmp(latest_info.message.c_str(), EXIT_STR.c_str(), EXIT_STR.length()) == 0)
        {
            if (sock == latest_info.sock_number)
            {
                info = latest_info;
                queueLock.UnLock();
                return info;
            }
            else
            {
                queueLock.UnLock();
                continue;
            }
        }

        if (read_num >= connect_num)
        {
            queueLock.UnLock();
            Sleep(rand() % 200);
            continue;
        }
        else
        {
            info = latest_info;

            // self-produce
            if (sock == info.sock_number)
            {
                queueLock.UnLock();
                continue;
            }

            // from others but old
            else if (info_hash(info) == last_hash)
            {
                queueLock.UnLock();
                Sleep(rand() % 200);
                continue;
            }

            // new message from others
            else
            {
                read_num++;
                queueLock.UnLock();
                return info;
            }
        }
    }
}

void InfoQueue::add(const std::string message, const size_t sock_num)
{
    time_t now_time;
    time(&now_time);
    Info info(message, sock_num, now_time);

    queueLock.Lock();
    latest_info = info;
    read_num = 1;
    queueLock.UnLock();
}