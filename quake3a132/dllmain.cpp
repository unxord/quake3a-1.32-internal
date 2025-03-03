#include <windows.h>

// Точка входа для DLL
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        // Выполняется при загрузке DLL в процесс
        MessageBox(NULL, L"DLL Successfully Injected into Quake 3 Arena!", L"Success", MB_OK | MB_ICONINFORMATION);
        break;
    case DLL_PROCESS_DETACH:
        // Выполняется при выгрузке DLL
        break;
    }
    return TRUE;
}