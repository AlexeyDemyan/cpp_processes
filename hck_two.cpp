#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>
#include <processthreadsapi.h>

int GAME_POINTER_OFFSET = 0x7030;
int PLAYER_HEALTH_OFFSET = 0x4;
LPVOID buffer[4];
SIZE_T lpNumberOfBytesRead;

BOOL lookForMyProcess(TCHAR *processName)
{
    TCHAR toFind[] = _T("game-loop.exe");
    if (_tcscmp(processName, toFind) == 0)
    {
        std::cout << "Found match!! : " << processName << "\n";
        return TRUE;
    }
    return FALSE;
}

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
            // std::cout << "Here's the process name from the buffer: " << procName << '\n';
            if (lookForMyProcess(procName))
            {
                LPMODULEINFO lpmodinfo;

                GetModuleInformation(hProcess, hMod, lpmodinfo, sizeof(lpmodinfo));
                std::cout << "Here is value of lpmodinfo: " << lpmodinfo << "\n";
                std::cout << "Reading process memory: \n";
                ReadProcessMemory(hProcess, lpmodinfo, &buffer, 0x4, &lpNumberOfBytesRead);
                printf("\n Error Code: %d \n", GetLastError());
                std::cout << "Here is what we have in process memory buffer: " << buffer << "\n";
                std::cout << "Here is what we have in process memory buffer derefed: " << &buffer << "\n";
                std::cout << "Here is amount of bytes read: " << lpNumberOfBytesRead << "\n";

                LPMODULEINFO gamePointer = lpmodinfo + GAME_POINTER_OFFSET;
                std::cout << "This is supposed to be game pointer:" << lpmodinfo + GAME_POINTER_OFFSET << "\n";
                std::cout << "Let me read its value " << &gamePointer << "\n";
                std::cout << "Reading process memory: \n";
                ReadProcessMemory(hProcess, gamePointer, &buffer, 4, &lpNumberOfBytesRead);
                printf("\n Error Code: %d \n", GetLastError());
                std::cout << "Here is what we have in process memory buffer: " << buffer << "\n";
                std::cout << "Here is what we have in process memory buffer derefed: " << &buffer << "\n";
                std::cout << "Here is amount of bytes read: " << lpNumberOfBytesRead << "\n";

                LPMODULEINFO health = gamePointer + 4;
                std::cout << "Maybe health is here:" << &health << "\n";
                std::cout << "Maybe health is here:" << health << "\n";
                std::cout << "Reading process memory: \n";
                ReadProcessMemory(hProcess, health, &buffer, 4, &lpNumberOfBytesRead);
                printf("\n Error Code: %d \n", GetLastError());
                std::cout << "Here is what we have in process memory buffer: " << buffer << "\n";
                std::cout << "Here is what we have in process memory buffer derefed: " << &buffer << "\n";
                std::cout << "Here is amount of bytes read: " << lpNumberOfBytesRead << "\n";
            };
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
        GetProcessNameById(processes[i]);
    }
    return 0;
}

int main()
{
    ListProcesses();
}