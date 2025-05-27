#include "pch.h"
#include "ProcessMemory.h"
#include <psapi.h>

DWORD GetProcessMemoryUsage(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!hProcess) return 0;

    PROCESS_MEMORY_COUNTERS pmc;
    DWORD memory = 0;
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        memory = static_cast<DWORD>(pmc.WorkingSetSize / 1024); // В килобайтах
    }
    CloseHandle(hProcess);
    return memory;
}
