#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>
#include <processthreadsapi.h>

// void GetProcessNameById(DWORD pId)
// {

// }

int ListProcesses()
{
    DWORD processes[1024], cbNeeded, cbProcesses;

    // Enumerating all processes
    BOOL procBool = EnumProcesses(processes, sizeof(processes), &cbNeeded);

    if (!procBool)
    {
        std::cout << "Unable to fetch processes\n";
        return 1;
    }

    // This is basically the amount of currently running processes
    cbProcesses = cbNeeded / sizeof(DWORD);

    std::cout << "Value of cbProcesses: " << cbProcesses << "\n";

    for (int i = 0; i < cbProcesses; i++)
    {
        // For the time being, just printing the process ID here instead of process name
        std::cout << "Id of procces: " << processes[i] << "\n";
    }
    return 0;
}

int main()
{
    ListProcesses();
}