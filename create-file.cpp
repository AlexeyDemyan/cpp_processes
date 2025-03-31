#include <Windows.h>
#include <fileapi.h>
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        printf("\n usage: %s <filename>", argv[0]);
        return 1;
    }

    /*
     * Function Documentation
     * https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
     */

    HANDLE hFile = CreateFileA(
        argv[1],
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        switch (GetLastError())
        {
        case ERROR_FILE_EXISTS:
            printf("\nFile %s already exists \n", argv[1]);
            break;
        default:
            printf("\n ERR: %d\n", GetLastError());
            break;
        }
        return 1;
    }
    else
    {
        printf("\n File created! \n");
    }

    // WRITING TO FILE:

    std::string strBuff = "Random text to place in file";
    // I think I would prefer to separate this into 2 lines:
    DWORD bytesToWrite = strBuff.size(), bytesWritten;

    // writing buffer to file
    BOOL bWrite = WriteFile(hFile, strBuff.c_str(), bytesToWrite, &bytesWritten, NULL);

    if (bWrite)
    {
        printf("\n Buffer written to file \n");
    }
    else if (bytesToWrite != bytesWritten)
    {
        printf("Bytes written != bytes to write");
        printf("\nSuccessfully wrote %d bytes to %s", bytesWritten, argv[1]);
        CloseHandle(hFile);
        return 1;
    }
    else
    {
        switch (GetLastError())
        {
        default:
            printf("Error: WRITEFILE: %d\n", GetLastError());
            CloseHandle(hFile);
            return 1;
        }
    }
    CloseHandle(hFile);

    // opening handle to the already created file to read it:
    hFile = CreateFileA(
        argv[1],
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING, // this is to open already existing file with name of argv[1]
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    DWORD nToRead = 15; // will read 15 bytes
    char chBuff[16]; // mmm not sure if it's necessary to specify the size of this buffer
    DWORD nRead;

    // READING file:
    BOOL bRead = ReadFile(hFile, &chBuff, nToRead, &nRead, NULL);
    chBuff[15] = '\0'; // hmmm, terminating the string

    if(bRead) {
        printf("\n DATA read: %s\n", chBuff);
    } else if (nToRead != nRead) {
        printf("Number of bytes to read != number of bytes read\n");
    } else { 
        switch (GetLastError())
        {
        case ERROR_NOACCESS:
            printf("\n ERROR: No access to read file %s \n", argv[1]);
            break;
        default:
            printf("\n Error: READFILE %d \n", GetLastError());
        } CloseHandle(hFile);
        return 1;
    }
    CloseHandle(hFile);
    return 0;
}