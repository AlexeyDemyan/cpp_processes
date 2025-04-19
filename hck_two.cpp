#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>
#include <processthreadsapi.h>

#define GAME_POINTER_OFFSET 0x7030
#define PLAYER_HEALTH_OFFSET 0x4

BYTE buffer[4];
SIZE_T lpNumberOfBytesRead = 0;

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
                MODULEINFO lpmodinfo;

                GetModuleInformation(hProcess, hMod, &lpmodinfo, sizeof(lpmodinfo));
                std::cout << "Here is value of lpmodinfo: " << &lpmodinfo << "\n";
                std::cout << "Reading process memory: \n";

                uintptr_t baseAddress = reinterpret_cast<uintptr_t>(lpmodinfo.lpBaseOfDll);
                uintptr_t pointerAddress = baseAddress + GAME_POINTER_OFFSET;

                uintptr_t pointer1 = 0;
                SIZE_T bytesRead = 0;

                if (ReadProcessMemory(hProcess, lpmodinfo.lpBaseOfDll, buffer, sizeof(buffer), &lpNumberOfBytesRead))
                {
                    std::cout << "Here is what we have in process memory buffer: " << buffer << "\n";
                    std::cout << "Here is buffer value as int: " << *(int *)buffer << "\n";
                    std::cout << "Here is what we have in process memory buffer derefed: " << &buffer << "\n";
                    std::cout << "Here is amount of bytes read: " << lpNumberOfBytesRead << "\n";

                    std::cout << "[+] Pointer 1: 0x" << std::hex << pointer1 << std::endl;
                    uintptr_t pointer2 = 0;

                    if (ReadProcessMemory(hProcess, (LPCVOID)pointer1, &pointer2, sizeof(pointer2), &bytesRead))
                    {
                        std::cout << "[+] Pointer 2 (Player Object): 0x" << std::hex << pointer2 << std::endl;

                        // Step 3: Read health from pointer2 + PLAYER_HEALTH_OFFSET
                        int playerHealth = 0;
                        uintptr_t healthAddr = pointer2 + PLAYER_HEALTH_OFFSET;

                        if (ReadProcessMemory(hProcess, (LPCVOID)healthAddr, &playerHealth, sizeof(playerHealth), &bytesRead))
                        {
                            std::cout << "[+] Player Health: " << std::dec << playerHealth << std::endl;
                        }
                        else
                        {
                            std::cerr << "[-] Failed to read player health. Error: " << GetLastError() << std::endl;
                        }
                    }
                    else
                    {
                        std::cerr << "[-] Failed to read pointer2. Error: " << GetLastError() << std::endl;
                    }
                }
                else
                {
                    printf("\n Error Code: %d \n", GetLastError());
                }
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