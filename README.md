# TCP-Chatroom

Multithread CLI chatroom, pure C++, based on TCP.

## Acknowledgement

This project uses TTGuoying's thread pool implementation on Windows, with acknowledgements and thanks.

Link: [https://github.com/TTGuoying/ThreadPool](https://github.com/TTGuoying/ThreadPool)

Open source licence: GPLv3.

## Install

First clean the binary files by:

    make clean

Use this command to compile:

    make

You can also use multi-thread compiling:

    make -j4

## Run

First run Server.exe on one host, 

    Server.exe

then run Client.exe for each user participating in the chat.

    Client.exe

Each client needs to enter the IP address of the Server host (IPv4 support only).

