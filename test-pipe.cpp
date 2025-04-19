#include <windows.h>
#include <iostream>

int main()
{
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

            // echo back a response
            const char *response = "Acknowledged";
            DWORD bytesWritten;
            WriteFile(hPipe, response, strlen(response), &bytesWritten, NULL);
        }
    }

    CloseHandle(hPipe);
    return 0;
}