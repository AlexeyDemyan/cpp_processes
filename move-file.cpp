#include <Windows.h>
#include <iostream>
#include <fileapi.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        printf("\n need to supply two arguments!");
        if (argc < 3)
        {
            printf("usage: %s <old filename / dirname> <new filename / dirname>\n", argv[0]);
            return 1;
        }
    }

    BOOL flag = MoveFileA(argv[1], argv[2]);

    if (flag)
    {
        printf("\n File Moved/Renamed successfully! ");
    }

    else
    {
        switch (GetLastError())
        {
        case ERROR_FILE_NOT_FOUND:
            printf("\nERR: File %s not found\n", argv[1]);
            break;
        case ERROR_ALREADY_EXISTS:
            printf("\nERR: File %s already exists\n", argv[2]);
            break;
        default:
            printf("\nERR: %d\n", GetLastError());
            break;
        }
        return 1;
    }
    return 0;
}