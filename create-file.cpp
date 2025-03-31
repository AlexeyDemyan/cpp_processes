#include <Windows.h>
#include <fileapi.h>
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        printf("\n usage: %s <filename>", argv[0]);
        return 1;
    }

    /*
     * Function Documentation
     * https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
     */

    HANDLE hFile = CreateFileA(
        argv[1],
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        switch (GetLastError())
        {
        case ERROR_FILE_EXISTS:
            printf("\nFile %s already exists \n", argv[1]);
            break;
        default:
            printf("\n ERR: %d\n", GetLastError());
            break;
        }
        return 1;
    }
    else
    {
        printf("\n File created! \n");
    }
}