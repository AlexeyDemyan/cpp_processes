#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>
#include <processthreadsapi.h>

#define GAME_POINTER_OFFSET 0x7030
#define PLAYER_HEALTH_OFFSET 0x4

BOOL lookForMyProcess(TCHAR *processName)
{
    TCHAR toFind[] = _T("game-loop.exe");
    if (_tcscmp(processName, toFind) == 0)
    {
        std::wcout << L"[+] Found match: " << processName << std::endl;
        return TRUE;
    }
    return FALSE;
}

void GetProcessNameById(DWORD pId)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pId);

    TCHAR procName[MAX_PATH] = _T("<Unknown>");

    if (hProcess != NULL)
    {
        HMODULE hMod;
        DWORD cbNeeded;

        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
        {
            if (GetModuleBaseName(hProcess, hMod, procName, sizeof(procName) / sizeof(TCHAR)))
            {
                if (lookForMyProcess(procName))
                {
                    MODULEINFO modInfo = {0};
                    if (GetModuleInformation(hProcess, hMod, &modInfo, sizeof(modInfo)))
                    {
                        uintptr_t baseAddress = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
                        uintptr_t pointerAddress = baseAddress + GAME_POINTER_OFFSET;

                        uintptr_t pointer1 = 0;
                        SIZE_T bytesRead = 0;

                        // Step 1: Read pointer1 from base + GAME_POINTER_OFFSET
                        if (ReadProcessMemory(hProcess, (LPCVOID)pointerAddress, &pointer1, sizeof(pointer1), &bytesRead))
                        {
                            std::cout << "[+] Pointer 1: 0x" << std::hex << pointer1 << std::endl;

                            uintptr_t pointer2 = 0;

                            // Step 2: Read pointer2 from pointer1
                            if (ReadProcessMemory(hProcess, (LPCVOID)pointer1, &pointer2, sizeof(pointer2), &bytesRead))
                            {
                                std::cout << "[+] Pointer 2 (Player Object): 0x" << std::hex << pointer2 << std::endl;

                                int playerHealth = 0;
                                uintptr_t healthAddr = pointer2 + PLAYER_HEALTH_OFFSET;

                                // Step 3: Read player health
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
                            std::cerr << "[-] Failed to read pointer1. Error: " << GetLastError() << std::endl;
                        }
                    }
                    else
                    {
                        std::cerr << "[-] Failed to get module information. Error: " << GetLastError() << std::endl;
                    }
                }
            }
        }
    }

    CloseHandle(hProcess);
}

int ListProcesses()
{
    DWORD processes[1024], cbNeeded, cbProcesses;

    if (!EnumProcesses(processes, sizeof(processes), &cbNeeded))
    {
        std::cout << "[-] Unable to fetch processes\n";
        return 1;
    }

    cbProcesses = cbNeeded / sizeof(DWORD);
    std::cout << "[*] Number of processes: " << cbProcesses << "\n";

    for (int i = 0; i < cbProcesses; i++)
    {
        if (processes[i] != 0)
        {
            GetProcessNameById(processes[i]);
        }
    }

    return 0;
}

int main()
{
    ListProcesses();
    return 0;
}
