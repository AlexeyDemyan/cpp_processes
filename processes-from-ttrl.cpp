#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>
#include <processthreadsapi.h>

void GetProcessNameById(DWORD pId)
{

    // opening process and getting handle for it
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
                                      PROCESS_VM_READ,
                                  FALSE, pId);

    TCHAR procName[MAX_PATH] = _T("<Unknown>");

    // checking if process is opened
    if (hProcess != NULL)
    {
        HMODULE hMod;
        DWORD cbNeeded;

        // enumerating process modules
        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
        {
            // getting the base module name, which is actually the name of the process
            // and then this name we're writing to a buffer
            DWORD baseName = GetModuleBaseName(hProcess, hMod, procName, sizeof(procName) / sizeof(TCHAR));
            std::cout << "Here's the process name from the buffer: " << procName << '\n';
        }
    }

    CloseHandle(hProcess);
}

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
        // std::cout << "Id of procces: " << processes[i] << "\n";
        GetProcessNameById(processes[i]);
    }
    return 0;
}

int main()
{
    ListProcesses();
}