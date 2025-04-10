#include "Console.h"
#include <Windows.h>
#include <stdio.h>

namespace Console {
    void Initialize() {
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

        printf("Console initialized.\n");
    }

    void Cleanup() {
        printf("Console cleaning up...\n");
        FreeConsole();
        fclose(stdout);
    }
}