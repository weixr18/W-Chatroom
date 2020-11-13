# W-Chatroom

Multithread CLI chatroom, pure C++. Now supports both **TCP** and **UDP**.

## Acknowledgement

This project uses [@TTGuoying](https://github.com/TTGuoying)'s thread pool implementation on Windows, with acknowledgements and thanks.

Link: [https://github.com/TTGuoying/ThreadPool](https://github.com/TTGuoying/ThreadPool)

Open source licence: LGPLv3.

## Target Environment

Windows 7/Vista/8/10

## Install

First clean the binary files by:

    make clean

Use this command to compile:

    make

You can also compile with multi-thread:

    make -j4

## Run

First run Server.exe on one host, 

    Server.exe

then run Client.exe for each user participating in the chat.

    Client.exe

### Attentions

1. Each client needs to enter the IP address of the Server host (IPv4 support only).
2. Server and Client should choose the same protocol.
3. Remember to allow Server.exe pass the firewall.