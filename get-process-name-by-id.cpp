#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>
#include <processthreadsapi.h>

void GetProcessNameById(DWORD pId)
{

    // opening process and getting handle for it
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,
                                  FALSE, pId);

    if (hProcess == NULL)
    {
        std::cerr << "[-] OpenProcess failed for PID " << pId
                  << ". Error: " << GetLastError() << std::endl;
        return;
    }

    TCHAR procName[MAX_PATH] = _T("<Unknown>");
    HMODULE hMod;
    DWORD cbNeeded;

    if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
    {
        if (GetModuleBaseName(hProcess, hMod, procName, sizeof(procName) / sizeof(TCHAR)))
        {
            std::wcout << L"[+] Process name for PID " << pId << L": " << procName << std::endl;
        }
        else
        {
            std::cerr << "[-] GetModuleBaseName failed. Error: " << GetLastError() << std::endl;
        }
    }
    else
    {
        std::cerr << "[-] EnumProcessModules failed. Error: " << GetLastError() << std::endl;
    }

    CloseHandle(hProcess);
}

int main()
{
    GetProcessNameById(4828);
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
}