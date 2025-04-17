#include <Windows.h>
#include <Winbase.h>
#include <iostream>
#include <string>
#include <fileapi.h>
#define print printf

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("usage: %s <existing file name> <new file name>\n", argv[0]);
        return 1;
    }

    // specifying that target is a file
    // for folder this would be 0x1
    DWORD flags = 0x0;

    if (GetFileAttributesA(argv[1]) == FILE_ATTRIBUTE_DIRECTORY)
    {
        print("dir");
        flags = SYMBOLIC_LINK_FLAG_DIRECTORY;
    }

    BOOLEAN slBool = CreateSymbolicLinkA(argv[2], argv[1], flags);

    if (slBool)
    {
        printf("Symlink created\n");
        return 0;
    }
    else
    {
        DWORD err = GetLastError();
        if (err == 1314)
        {
            printf("Try again with administrator account\n");
            return 1;
        }
        printf("Error Code: %d\n", err);
        return 1;
    }
}