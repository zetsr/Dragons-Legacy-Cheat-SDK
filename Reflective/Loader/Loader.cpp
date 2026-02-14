#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <direct.h>

// 获取当前目录下的 DLL 全路径
std::string GetFullDllPath(const std::string& dllName) {
    char buffer[MAX_PATH];
    _getcwd(buffer, MAX_PATH);
    return std::string(buffer) + "\\" + dllName;
}

// 检查并获取目标进程 ID
DWORD GetProcessIdByName(const std::wstring& processName) {
    DWORD pid = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W entry;
        entry.dwSize = sizeof(entry);
        if (Process32FirstW(snapshot, &entry)) {
            do {
                if (std::wstring(entry.szExeFile) == processName) {
                    pid = entry.th32ProcessID;
                    break;
                }
            } while (Process32NextW(snapshot, &entry));
        }
        CloseHandle(snapshot);
    }
    return pid;
}

// 注入逻辑
bool InjectDLL(DWORD pid, const std::string& dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess) return false;

    void* loc = VirtualAllocEx(hProcess, NULL, dllPath.length() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!loc) {
        CloseHandle(hProcess);
        return false;
    }

    WriteProcessMemory(hProcess, loc, dllPath.c_str(), dllPath.length() + 1, NULL);

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, NULL);
    if (hThread) {
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
    }

    VirtualFreeEx(hProcess, loc, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return hThread != NULL;
}

int main() {
    const std::wstring targetProcess = L"Dragons-Win64-Shipping.exe";
    const std::string dllName = "Reflective.dll";
    std::string fullPath = GetFullDllPath(dllName);

    printf("Waiting for Dragons-Win64-Shipping.exe\n");

    while (true) {
        DWORD pid = GetProcessIdByName(targetProcess);
        if (pid != 0) {
            printf("Detected Dragons-Win64-Shipping.exe");
            if (InjectDLL(pid, fullPath)) {
                break;
            }
            else {
            }
        }
        Sleep(100); // 100ms 检查一次
    }

    return 0;
}