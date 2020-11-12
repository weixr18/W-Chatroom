#include "Server.h"

int main(int argc, char *argv[])
{
    printf("Choose protocal: TCP(T)/UDP(U) ?");
    char s[10] = {0};
    gets(s);
    if (s[0] == 'T')
    {
        runTCPServer(argc, argv);
    }
    else if (s[0] == 'U')
    {
        runUDPServer(argc, argv);
    }
}
