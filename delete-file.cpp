#include <Windows.h>
#include <fileapi.h>
#include <iostream>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

int main(int argc, char **argv)
{
    BOOL flag = PathFileExistsA(argv[1]);

    // I seem to be lacking the shlwapi library which needs to be installed
    // Will ignore for now, let's see if it bites me later
    // https://dev.to/tbhaxor/windows-system-programming-move-delete-a-file-or-folder-3lc9

    // if (flag)
    // {
    //     printf("\n File %s exists! \n", argv[1]);
    // }
    // else
    // {
    //     printf("\n FIle %s not found \n", argv[1]);
    // }
    // return 0;
}