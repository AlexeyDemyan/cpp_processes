#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>
#include <processthreadsapi.h>

int main()
{
    DWORD processes[1024], cbNeeded, cbProcesses;

    BOOL procBool = EnumProcesses(processes, sizeof(processes), &cbNeeded);

    std::cout << "Here are the process ids: \n"
              << processes[4] << "\n"
              << "And here is the byte count: \n"
              << &cbNeeded;

    HANDLE procInfo = OpenProcess();
}