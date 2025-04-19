#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <thread>

#define PATH_TO_GAME L"C:\\Users\\demon\\Desktop\\Codes_and_Repos\\cpp_processes\\game-run.exe"

int main()
{
    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi;

    // Step 1: Start game-run.exe
    if (!CreateProcessW(
            PATH_TO_GAME,
            NULL,
            NULL,
            NULL,
            FALSE,
            CREATE_DEFAULT_ERROR_MODE,
            NULL,
            NULL,
            &si,
            &pi))
    {

        std::wcerr << L"[-] Failed to start process. Error: " << GetLastError() << std::endl;
        return 1;
    }

    std::wcout << L"[+] Game started. PID: " << pi.dwProcessId << std::endl;

    // Step 2: Let it initialize a bit
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    HANDLE hProcess = OpenProcess(
        PROCESS_ALL_ACCESS, // if needed, restrict to what's required
        FALSE, pi.dwProcessId);

    if (!hProcess)
    {
        std::wcerr << L"[-] Failed to open process. Error: " << GetLastError() << std::endl;
        return 1;
    }

    // -- INSERT ANY MEMORY PATCHING / MANIPULATION LOGIC HERE --
    // You can replicate the logic from your working memory reading/writing code here

    // Step 3: Get the base address of the game module (e.g., BloogsQuest.exe)
    HMODULE hMods[1024];
    DWORD cbNeeded;
    uintptr_t baseAddress = 0;

    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        baseAddress = reinterpret_cast<uintptr_t>(hMods[0]);
        std::wcout << L"[+] Base address: 0x" << std::hex << baseAddress << std::endl;
    }
    else
    {
        std::wcerr << L"[-] Failed to get module base. Error: " << GetLastError() << std::endl;
        return 1;
    }

    // Step 4: Calculate addresses
    uintptr_t pointerAddress = baseAddress + 0x7030;
    uintptr_t pointer1 = 0;
    uintptr_t pointer2 = 0;
    SIZE_T bytesRead = 0;

    // Read pointer1
    if (!ReadProcessMemory(hProcess, (LPCVOID)pointerAddress, &pointer1, sizeof(pointer1), &bytesRead))
    {
        std::wcerr << L"[-] Failed to read pointer1. Error: " << GetLastError() << std::endl;
        return 1;
    }

    // Read pointer2 (player object)
    if (!ReadProcessMemory(hProcess, (LPCVOID)pointer1, &pointer2, sizeof(pointer2), &bytesRead))
    {
        std::wcerr << L"[-] Failed to read pointer2. Error: " << GetLastError() << std::endl;
        return 1;
    }

    // Final player health address
    uintptr_t healthAddr = pointer2 + 0x4;
    int playerHealth = 0;

    // Read player health
    if (ReadProcessMemory(hProcess, (LPCVOID)healthAddr, &playerHealth, sizeof(playerHealth), &bytesRead))
    {
        std::wcout << L"[+] Current player health: " << std::dec << playerHealth << std::endl;
    }
    else
    {
        std::wcerr << L"[-] Failed to read player health. Error: " << GetLastError() << std::endl;
        return 1;
    }

    // Modify player health
    int newHealth = 999;
    SIZE_T bytesWritten = 0;

    if (WriteProcessMemory(hProcess, (LPVOID)healthAddr, &newHealth, sizeof(newHealth), &bytesWritten))
    {
        std::wcout << L"[+] Player health overwritten to: " << newHealth << std::endl;
    }
    else
    {
        std::wcerr << L"[-] Failed to write player health. Error: " << GetLastError() << std::endl;
    }

    // Example: Wait for user to exit
    std::wcout << L"[+] Press Enter to exit..." << std::endl;
    std::wcin.get();

    // Clean up
    CloseHandle(hProcess);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
