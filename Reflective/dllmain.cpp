#include "Minimal-D3D12-Hook-ImGui-1.0.2/Main/mdx12_api.h"

#include <windows.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include "g_dll.h"

extern "C" {
#include "Minimal-D3D12-Hook-ImGui-1.0.2/MinHook/src/buffer.c"
#include "Minimal-D3D12-Hook-ImGui-1.0.2/MinHook/src/hook.c"
#include "Minimal-D3D12-Hook-ImGui-1.0.2/MinHook/src/trampoline.c"
#include "Minimal-D3D12-Hook-ImGui-1.0.2/MinHook/src/hde/hde64.c"
}

const char* title = "[github.com/zetsr] ";
typedef BOOL(WINAPI* f_DLL_ENTRY_POINT)(void*, DWORD, void*);

bool ReflectiveLoad(unsigned char* pRawData) {
    if (!pRawData) return false;

    auto* pDosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(pRawData);
    if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) return false;

    auto* pOldNtHeader = reinterpret_cast<IMAGE_NT_HEADERS64*>(pRawData + pDosHeader->e_lfanew);
    auto* pOldOptHeader = &pOldNtHeader->OptionalHeader;

    // 1. 分配空间
    unsigned char* pTargetBase = reinterpret_cast<unsigned char*>(VirtualAlloc(
        nullptr, pOldOptHeader->SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
    ));
    if (!pTargetBase) return false;

    printf("%s[+] Allocated Memory at: 0x%p (Size: 0x%X)\n", title, pTargetBase, pOldOptHeader->SizeOfImage);

    // 2. 拷贝 Headers & Sections
    memcpy(pTargetBase, pRawData, pOldOptHeader->SizeOfHeaders);
    auto* pSectionHeader = IMAGE_FIRST_SECTION(pOldNtHeader);
    for (int i = 0; i < pOldNtHeader->FileHeader.NumberOfSections; ++i, ++pSectionHeader) {
        if (pSectionHeader->SizeOfRawData) {
            memcpy(pTargetBase + pSectionHeader->VirtualAddress, pRawData + pSectionHeader->PointerToRawData, pSectionHeader->SizeOfRawData);
            printf("%s[*] Section %s mapped to 0x%p\n", title, pSectionHeader->Name, pTargetBase + pSectionHeader->VirtualAddress);
        }
    }

    // 3. 修复重定位 (Relocations)
    auto delta = reinterpret_cast<ULONG_PTR>(pTargetBase) - pOldOptHeader->ImageBase;
    if (delta != 0) {
        printf("%s[>] Relocating image (Delta: 0x%llX)...\n", title, delta);
        auto* pRelocDir = &pOldOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
        if (pRelocDir->Size) {
            auto* pRelocBlock = reinterpret_cast<IMAGE_BASE_RELOCATION*>(pTargetBase + pRelocDir->VirtualAddress);
            while (pRelocBlock->VirtualAddress && pRelocBlock->SizeOfBlock) {
                UINT amount = (pRelocBlock->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
                WORD* pRelativeInfo = reinterpret_cast<WORD*>(pRelocBlock + 1);

                for (UINT i = 0; i < amount; ++i) {
                    if ((pRelativeInfo[i] >> 12) == IMAGE_REL_BASED_DIR64) {
                        ULONG_PTR* pPatch = reinterpret_cast<ULONG_PTR*>(pTargetBase + pRelocBlock->VirtualAddress + (pRelativeInfo[i] & 0xFFF));
                        *pPatch += delta;
                        printf("%s[%d] Patching at 0x%p\n", title, i, pPatch);
                    }
                }
                pRelocBlock = reinterpret_cast<IMAGE_BASE_RELOCATION*>(reinterpret_cast<unsigned char*>(pRelocBlock) + pRelocBlock->SizeOfBlock);
            }
        }
    }

    // 4. 修复导入表 (IAT)
    auto* pImportDir = &pOldOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (pImportDir->Size) {
        auto* pImportDescr = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(pTargetBase + pImportDir->VirtualAddress);
        while (pImportDescr->Name) {
            char* szMod = reinterpret_cast<char*>(pTargetBase + pImportDescr->Name);
            bool isNewlyLoaded = false;
            HINSTANCE hMod = GetModuleHandleA(szMod);
            if (!hMod) {
                hMod = LoadLibraryA(szMod);
                isNewlyLoaded = true;
            }

            // 有反作弊的话加上这个
            /*
            if (!hMod) {
                printf("[!] ERROR: Module %s is missing from process memory!\n", szMod);
                printf("[!] Reflective load aborted to stay stealthy.\n");
                return false;
            }
            */

            if (isNewlyLoaded) {
                printf("[>] Patching IAT for module: %s (Base: 0x%p) [NEWLY LOADED]\n", szMod, hMod);
            }
            else {
                printf("[>] Patching IAT for module: %s (Base: 0x%p)\n", szMod, hMod);
            }

            auto* pThunk = reinterpret_cast<IMAGE_THUNK_DATA64*>(pTargetBase + pImportDescr->FirstThunk);
            auto* pRawThunk = reinterpret_cast<IMAGE_THUNK_DATA64*>(pTargetBase + pImportDescr->OriginalFirstThunk);

            while (pRawThunk->u1.AddressOfData) {
                ULONG_PTR fnAddr = 0;
                if (IMAGE_SNAP_BY_ORDINAL64(pRawThunk->u1.Ordinal)) {
                    fnAddr = reinterpret_cast<ULONG_PTR>(GetProcAddress(hMod, reinterpret_cast<char*>(pRawThunk->u1.Ordinal & 0xFFFF)));
                }
                else {
                    auto* pImportData = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(pTargetBase + pRawThunk->u1.AddressOfData);
                    fnAddr = reinterpret_cast<ULONG_PTR>(GetProcAddress(hMod, pImportData->Name));
                }
                pThunk->u1.Function = fnAddr;
                pThunk++;
                pRawThunk++;
            }
            pImportDescr++;
        }
    }

    // 5. 处理 TLS
    auto* pTlsDir = &pOldOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];
    if (pTlsDir->Size) {
        auto* pTls = reinterpret_cast<IMAGE_TLS_DIRECTORY64*>(pTargetBase + pTlsDir->VirtualAddress);
        if (pTls->AddressOfIndex) {
            static DWORD dummy_index = 0;
            *reinterpret_cast<DWORD*>(pTls->AddressOfIndex) = dummy_index;
        }

        if (pTls->AddressOfCallBacks) {
            auto** ppCallback = reinterpret_cast<PIMAGE_TLS_CALLBACK*>(pTls->AddressOfCallBacks);
            printf("[>] Executing TLS Callbacks at 0x%p...\n", ppCallback);
            while (ppCallback && *ppCallback) {
                (*ppCallback)(pTargetBase, DLL_PROCESS_ATTACH, nullptr);
                ppCallback++;
            }
        }
    }

    // 没啥用
    // 6 注册异常处理表 (针对 x64 异常回溯修复)
    auto* pExceptionDir = &pOldOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];
    if (pExceptionDir->Size) {
        auto* pRuntimeFunction = reinterpret_cast<PRUNTIME_FUNCTION>(
            pTargetBase + pExceptionDir->VirtualAddress
            );
        DWORD count = pExceptionDir->Size / sizeof(RUNTIME_FUNCTION);

        printf("[>] Registering %d Exception Handler entries...\n", count);

        if (RtlAddFunctionTable(
            pRuntimeFunction,
            count,
            reinterpret_cast<DWORD64>(pTargetBase)
        )) {
            printf("[+] Exception Table registered successfully.\n");
        }
        else {
            printf("[!] Failed to register Exception Table.\n");
        }
    }

    // 7. 执行 DllMain
    auto pEntry = reinterpret_cast<f_DLL_ENTRY_POINT>(pTargetBase + pOldOptHeader->AddressOfEntryPoint);
    printf("[!] Calling Entry Point at 0x%p...\n", pEntry);
    return pEntry(pTargetBase, DLL_PROCESS_ATTACH, nullptr);
}

void MainThread(HMODULE hModule) {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    SetConsoleTitleA(title);

    g_MDX12::Initialize();

    printf("[*] Wating for d3d12 init.\n");
    while (!g_MDX12::g_InitState::g_Initialized) {
        Sleep(100);
    }

    printf("[*] d3d12 SUCCESS.\n");
    Sleep(1000);
    FreeConsole();

    AllocConsole();
    freopen_s(&f, "CONOUT$", "w", stdout);
    SetConsoleTitleA(title);
    if (ReflectiveLoad(g_dll::g_dll)) {
        printf("\n[SUCCESS] DLL is now running in memory.\n");
    }
    else {
        printf("\n[ERROR] Failed to load DLL.\n");
    }

    FreeConsole();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        HANDLE hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, nullptr);
        if (hThread) CloseHandle(hThread);
    }
    return TRUE;
}