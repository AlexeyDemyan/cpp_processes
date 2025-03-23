#include <Windows.h>
#include <iostream>
#include <fileapi.h>

int main(int argc, char* argv[])
{
    std::cout << "\n Creating directory here \n";

    if (argc == 1) {
        printf("usage %s <directory name>\n");
        return 1;
    }

BOOL flag = CreateDirectoryA(argv[1], NULL);
}