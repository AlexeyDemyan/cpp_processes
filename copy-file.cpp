#include <Windows.h>
#include <iostream>
#include <fileapi.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        printf("\nusage: %s\n", argv[0]);
        return 1;
    }

    BOOL flag = CopyFileA(argv[1], argv[2], TRUE);

    if (flag)
    {
        printf("File copied!\n");
    }
    else
    {
        switch (GetLastError())
        {
        case ERROR_FILE_NOT_FOUND:
            printf("ERR: File %s not found\n", argv[1]);
            break;
        case ERROR_FILE_EXISTS:
            printf("ERR: File %s already exists\n", argv[2]);
            break;
        default:
            printf("ERR: %d\n", GetLastError());
            break;
        }
        return 1;
    }
    return 0;
}
