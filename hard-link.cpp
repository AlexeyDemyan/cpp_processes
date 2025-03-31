#include <Windows.h>
#include <Winbase.h>
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("usage: %s <existing file name> <new file name>\n", argv[0]);
        return 1;
    }

    BOOL hlBool = CreateHardLinkA(argv[2], argv[1], NULL);

    if (hlBool)
    {
        printf("\n Hard Link created! \n");
        return 0;
    }
    else
    {
        printf("\n Error Code: %d \n", GetLastError());
        return 1;
    }
}