#pragma once
#include <Windows.h>
#include <queue>
#include <string>
#include <iostream>
#include <unordered_map>
#include <random>
#include <time.h>

class InfoQueue
{
public:
    class Info
    {
    public:
        std::string message;
        size_t sock_number;
        time_t time;

        Info() = default;
        Info(std::string mes, size_t sock_num, time_t tim)
        {
            this->message = mes;
            this->sock_number = sock_num;
            this->time = tim;
        }
        Info &operator=(const Info &cls)
        {
            // 避免自赋值
            if (this != &cls)
            {
                this->message = cls.message;
                this->sock_number = cls.sock_number;
                this->time = cls.time;
            }
            return *this;
        }
    };

    struct Info_hash
    {
        size_t operator()(const Info v) const
        {
            size_t h1 = string_hash(v.message);
            size_t h2 = sizet_hash(v.sock_number);
            size_t h3 = time_hash(v.time);
            return size_t(h1 + h2 + h3);
        }
    } info_hash;

private:
    static std::hash<std::string> string_hash;
    static std::hash<size_t> sizet_hash;
    static std::hash<time_t> time_hash;

    class CriticalSectionLock
    {
    private:
        CRITICAL_SECTION cs; //临界区
    public:
        CriticalSectionLock() { InitializeCriticalSection(&cs); }
        ~CriticalSectionLock() { DeleteCriticalSection(&cs); }
        void Lock() { EnterCriticalSection(&cs); }
        void UnLock() { LeaveCriticalSection(&cs); }
    };

    CriticalSectionLock queueLock;

public:
    InfoQueue();
    Info read(size_t last_hash, SOCKET sock);
    void add(const std::string message, const size_t port_num);
};

extern volatile size_t connect_num;
extern volatile size_t read_num;
extern InfoQueue::Info latest_info;