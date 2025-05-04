#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>
#include <processthreadsapi.h>

BOOL EnableDebugPrivilege()
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;

    // Open the current process token with TOKEN_ADJUST_PRIVILEGES and TOKEN_QUERY access
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        std::cerr << "OpenProcessToken failed. Error: " << GetLastError() << std::endl;
        return FALSE;
    }

    // Lookup the LUID for SE_DEBUG_NAME privilege
    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
    {
        std::cerr << "LookupPrivilegeValue failed. Error: " << GetLastError() << std::endl;
        CloseHandle(hToken);
        return FALSE;
    }

    // Set up the TOKEN_PRIVILEGES structure
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // Adjust the privileges
    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
    {
        std::cerr << "AdjustTokenPrivileges failed. Error: " << GetLastError() << std::endl;
        CloseHandle(hToken);
        return FALSE;
    }

    // Check if the privilege is successfully enabled
    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
    {
        std::cerr << "SE_DEBUG_NAME privilege is not assigned." << std::endl;
        CloseHandle(hToken);
        return FALSE;
    }

    // Close the token handle
    CloseHandle(hToken);
    return TRUE;
}

std::string GetErrorMessage(DWORD dwError)
{
    char *msgBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, dwError, 0, (LPSTR)&msgBuffer, 0, NULL);
    std::string errorMsg(msgBuffer, size);
    LocalFree(msgBuffer);
    return errorMsg;
}

void GetProcessNameById(DWORD pId)
{

    if (!EnableDebugPrivilege())
    {
        std::cerr << "Failed to enable SE_DEBUG_NAME privilege." << std::endl;
        return;
    }

    // opening process and getting handle for it
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                  FALSE, pId);

    if (hProcess == NULL)
    {
        DWORD dwError = GetLastError();
        std::cerr << "[-] OpenProcess failed for PID " << pId
                  << ". Error code: " << dwError
                  << " (" << GetErrorMessage(dwError) << ")" << std::endl;
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