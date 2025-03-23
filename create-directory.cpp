#include <Windows.h>
#include <iostream>
#include <fileapi.h>

int main(int argc, char *argv[])
{
    std::cout << "\nCreating directory here \n";

    if (argc == 1)
    {
        printf("usage: %s\n", argv[0]);
        return 1;
    }

    BOOL flag = CreateDirectoryA(argv[1], NULL);

    if (flag)
    {
        printf("Directory Created\n");
    }
    else
    {
        switch (GetLastError())
        {
        case ERROR_ALREADY_EXISTS:
            printf("ERR: Directory already exists\n");
            break;
        case ERROR_PATH_NOT_FOUND:
            printf("ERR: Path not found\n");
            break;
        default:
            break;
        }
        return 1;
    }
    return 0;
}