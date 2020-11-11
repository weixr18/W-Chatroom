#include <iostream>
#include <windows.h>

int main()
{
    printf("Hello, world!\n");

    SYSTEMTIME st = {0};
    GetLocalTime(&st); //获取当前时间 可精确到ms
    printf("%d-%02d-%02d %02d:%02d:%02d\n",
           st.wYear,
           st.wMonth,
           st.wDay,
           st.wHour,
           st.wMinute,
           st.wSecond);

    return 0;
}