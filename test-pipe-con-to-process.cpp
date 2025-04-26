#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>
#include <processthreadsapi.h>

#define GAME_POINTER_OFFSET 0x7030
#define PLAYER_HEALTH_OFFSET 0x4
#define YELL_FUNCTION_OFFSET 0x29C0
#define SPEAK_FUNCTION_OFFSET 0x2A00

BOOL lookForMyProcess(TCHAR *processName)
{
    TCHAR toFind[] = _T("game-run.exe");
    if (_tcscmp(processName, toFind) == 0)
    {
        std::wcout << L"[+] Found match: " << processName << std::endl;
        return TRUE;
    }
    return FALSE;
}

void CallRemoteFunction(HANDLE hProcess, uintptr_t functionToCall)
{
    // Assembly bytes:
    // mov rax, <address>  (10 bytes)
    // call rax            (2 bytes)
    // ret                 (1 byte)

    unsigned char stub[] = {
        0x48, 0xB8,             // mov rax, <address>
        0, 0, 0, 0, 0, 0, 0, 0, // (address placeholder)
        0xFF, 0xD0,             // call rax
        0xC3                    // ret
    };

    // Fill in the address
    *(uintptr_t *)(stub + 2) = functionToCall;

    LPVOID remoteStub = VirtualAllocEx(
        hProcess, nullptr, sizeof(stub), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    if (!remoteStub)
    {
        std::cerr << "[-] VirtualAllocEx failed. Error: " << GetLastError() << std::endl;
        return;
    }

    SIZE_T bytesWritten;

    if (!WriteProcessMemory(hProcess, remoteStub, stub, sizeof(stub), &bytesWritten))
    {
        std::cerr << "[-] WriteProcessMemory failed. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, remoteStub, 0, MEM_RELEASE);
        return;
    }

    HANDLE hThread = CreateRemoteThread(
        hProcess,
        nullptr,
        0,
        (LPTHREAD_START_ROUTINE)remoteStub,
        nullptr,
        0,
        nullptr);

    if (!hThread)
    {
        std::cerr << "[-] CreateRemoteThread failed. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, remoteStub, 0, MEM_RELEASE);
        return;
    }

    // Wait for the remote call to finish
    WaitForSingleObject(hThread, INFINITE);

    // Clean up
    CloseHandle(hThread);
    VirtualFreeEx(hProcess, remoteStub, 0, MEM_RELEASE);

    std::cout << "[+] Successfully called remote function!" << std::endl;
}

void GetProcessNameById(DWORD pId, int newHealthValueFromInput)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pId);

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
                        uintptr_t yellPointer = baseAddress + YELL_FUNCTION_OFFSET;
                        uintptr_t speakPointer = baseAddress + SPEAK_FUNCTION_OFFSET;

                        uintptr_t pointer1 = 0;
                        SIZE_T bytesRead = 0;

                        if (ReadProcessMemory(hProcess, (LPCVOID)speakPointer, &pointer1, sizeof(pointer1), &bytesRead))
                        {
                            std::cout << "[+] Pointer 1: 0x" << std::hex << pointer1 << std::endl;
                            std::cout << "Here is health value that we are passing: " << newHealthValueFromInput << std::endl;

                            CallRemoteFunction(hProcess, pointer1);
                        }
                        else
                        {
                            std::cerr << "[-] Failed to read pointer1. Error: " << GetLastError() << std::endl;
                        }

                        // Step 1: Read pointer1 from base + GAME_POINTER_OFFSET
                        // if (ReadProcessMemory(hProcess, (LPCVOID)pointerAddress, &pointer1, sizeof(pointer1), &bytesRead))
                        // {
                        //     std::cout << "[+] Pointer 1: 0x" << std::hex << pointer1 << std::endl;

                        //     uintptr_t pointer2 = 0;

                        //     // Step 2: Read pointer2 from pointer1
                        //     if (ReadProcessMemory(hProcess, (LPCVOID)pointer1, &pointer2, sizeof(pointer2), &bytesRead))
                        //     {
                        //         std::cout << "[+] Pointer 2 (Player Object): 0x" << std::hex << pointer2 << std::endl;

                        //         int playerHealth = 0;
                        //         uintptr_t healthAddr = pointer2 + PLAYER_HEALTH_OFFSET;

                        //         // Step 3: Read player health
                        //         if (ReadProcessMemory(hProcess, (LPCVOID)healthAddr, &playerHealth, sizeof(playerHealth), &bytesRead))
                        //         {
                        //             std::cout << "[+] Player Health: " << std::dec << playerHealth << std::endl;

                        //             int newHealthValue = newHealthValueFromInput;
                        //             SIZE_T bytesWritten = 0;

                        //             if (WriteProcessMemory(hProcess, (LPVOID)healthAddr, &newHealthValue, sizeof(newHealthValue), &bytesWritten))
                        //             {
                        //                 std::cout << "[+] Managed to overwrite Player Health, new value is: " << std::dec << newHealthValue << std::endl;
                        //             }
                        //             else
                        //             {
                        //                 std::cerr << "[-] Failed to overwrite Player Health. Error: " << GetLastError() << std::endl;
                        //             }
                        //         }
                        //         else
                        //         {
                        //             std::cerr << "[-] Failed to read player health. Error: " << GetLastError() << std::endl;
                        //         }
                        //     }
                        //     else
                        //     {
                        //         std::cerr << "[-] Failed to read pointer2. Error: " << GetLastError() << std::endl;
                        //     }
                        // }
                        // else
                        // {
                        //     std::cerr << "[-] Failed to read pointer1. Error: " << GetLastError() << std::endl;
                        // }
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

int ListProcesses(int newHealthValue)
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
            GetProcessNameById(processes[i], newHealthValue);
        }
    }

    return 0;
}

int main()
{
    ListProcesses(565);

    const wchar_t *pipeName = L"\\\\.\\pipe\\TestPipe";

    std::wcout << L"[+] Creating named pipe: " << pipeName << std::endl;

    HANDLE hPipe = CreateNamedPipeW(
        pipeName,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1, 1024, 1024, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE)
    {
        std::wcerr << L"[-] Failed to create named pipe. Error: " << GetLastError() << std::endl;
        return 1;
    }

    std::wcout << L"[+] Waiting for client to connect..." << std::endl;

    if (ConnectNamedPipe(hPipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED)
    {
        std::wcout << L"[+] Client connected!" << std::endl;

        char buffer[128];
        DWORD bytesRead;

        while (true)
        {
            BOOL result = ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
            if (!result || bytesRead == 0)
                break;

            buffer[bytesRead] = '\0';
            std::cout << "[Client]" << buffer << std::endl;

            try
            {
                int receivedNumber = std::stoi(std::string(buffer));
                ListProcesses(receivedNumber);
            }
            catch (const std::exception &e)
            {
                std::cerr << "[-] Failed to convert input to number: " << e.what() << std::endl;
            }

            // echo back a response
            const char *response = "Acknowledged";
            DWORD bytesWritten;
            WriteFile(hPipe, response, strlen(response), &bytesWritten, NULL);
        }
    }

    CloseHandle(hPipe);
    return 0;
}