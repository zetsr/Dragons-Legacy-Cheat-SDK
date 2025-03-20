#include "PointerScanner.h"
#include "Console.h"
#include <stdio.h>

namespace PointerScanner {
    std::pair<DWORD64, bool> Scan(DWORD64 baseAddress, const std::vector<DWORD64>& offsets, std::vector<std::pair<DWORD64, bool>>& debugSteps) {
        debugSteps.clear();
        DWORD64 currentAddress = baseAddress;
        debugSteps.push_back({ currentAddress, false }); // 初始基址

        if (offsets.empty()) {
            printf("Offsets empty.\n");
            return { 0, false };
        }

        __try {
            // 第一步：解引用基址
            currentAddress = *(volatile DWORD64*)currentAddress;
            debugSteps.push_back({ currentAddress, true }); // 解引用结果
            // printf("Step 0: Base = 0x%llX, Dereferenced = 0x%llX\n", baseAddress, currentAddress);

            if (!currentAddress) {
                printf("Base dereference failed.\n");
                return { 0, false };
            }

            // 应用后续偏移
            for (size_t i = 0; i < offsets.size(); ++i) {
                currentAddress += offsets[i];
                debugSteps.push_back({ currentAddress, false }); // 加偏移
                // printf("Step %zu: Address = 0x%llX, Offset = 0x%llX\n", i + 1, currentAddress, offsets[i]);

                // 非最后一级时解引用
                if (i < offsets.size() - 1) {
                    currentAddress = *(volatile DWORD64*)currentAddress;
                    debugSteps.push_back({ currentAddress, true }); // 解引用结果
                    // printf("Step %zu: Dereferenced = 0x%llX\n", i + 1, currentAddress);

                    if (!currentAddress) {
                        // printf("Pointer chain broken at offset: 0x%llX (Step %zu)\n", offsets[i], i);
                        return { 0, false };
                    }
                }
            }
            // printf("Scan completed: Final address = 0x%llX\n", currentAddress);
            return { currentAddress, true };
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            // printf("Exception in pointer scan: 0x%X\n", GetExceptionCode());
            return { 0, false };
        }
    }
}