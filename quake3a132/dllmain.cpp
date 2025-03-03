#include <windows.h>
#include <stdio.h> // Добавлен для FILE, printf, freopen_s, fclose и stdout

// Функция, которая будет выполняться в отдельном потоке
DWORD WINAPI MainThread(LPVOID lpParam) {
    // Создаем консоль для отладки
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    printf("DLL Successfully Injected into Quake 3 Arena!\n");

    // Ждем 5 секунд для проверки, затем закрываем консоль
    Sleep(5000);
    fclose(f);
    FreeConsole();
    return 0;
}

// Точка входа для DLL
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        // Отключаем ненужные уведомления о загрузке других DLL
        DisableThreadLibraryCalls(hModule);
        // Создаем отдельный поток для нашей логики
        CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
        break;
    case DLL_PROCESS_DETACH:
        // Очистка при выгрузке (пока ничего не делаем)
        break;
    }
    return TRUE;
}