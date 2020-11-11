#include "InfoQueue.h"

std::hash<std::string> InfoQueue::string_hash;
std::hash<size_t> InfoQueue::sizet_hash;
std::hash<time_t> InfoQueue::time_hash;
//std::queue<InfoQueue::Info> InfoQueue::_queue;
InfoQueue::Info latest_info;

InfoQueue::InfoQueue()
{
}

InfoQueue::Info InfoQueue::read(size_t last_hash, SOCKET sock)
{
    Info info;
    while (true)
    {
        queueLock.Lock();
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

void InfoQueue::add(const std::string message, const size_t port_num)
{
    time_t now_time;
    time(&now_time);
    Info info(message, port_num, now_time);

    queueLock.Lock();
    //_queue.push(info);
    latest_info = info;
    read_num = 1;
    queueLock.UnLock();
}