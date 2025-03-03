#include <windows.h>
#include <stdio.h>
#include <GL/gl.h> // Добавлен для GLenum и других типов OpenGL
#include "MinHook.h" // Предполагается, что MinHook уже подключен

// Определяем тип функции glDepthFunc
typedef void (WINAPI* glDepthFunc_t)(GLenum func);
glDepthFunc_t original_glDepthFunc = nullptr; // Указатель на оригинальную функцию

// Наша замена для glDepthFunc
void WINAPI Hooked_glDepthFunc(GLenum func) {
    printf("[Wallhack] glDepthFunc called with parameter: %d\n", func);
    original_glDepthFunc(func); // Вызываем оригинальную функцию
}

// Функция для инициализации хуков
void InitializeHooks() {
    if (MH_Initialize() != MH_OK) {
        printf("[Error] Failed to initialize MinHook\n");
        return;
    }

    // Получаем адрес glDepthFunc из opengl32.dll
    HMODULE opengl32 = GetModuleHandle(L"opengl32.dll");
    if (!opengl32) {
        printf("[Error] Failed to get handle to opengl32.dll\n");
        return;
    }

    void* glDepthFunc_ptr = GetProcAddress(opengl32, "glDepthFunc");
    if (!glDepthFunc_ptr) {
        printf("[Error] Failed to find glDepthFunc address\n");
        return;
    }

    // Создаем хук
    if (MH_CreateHook(glDepthFunc_ptr, &Hooked_glDepthFunc, (LPVOID*)&original_glDepthFunc) != MH_OK) {
        printf("[Error] Failed to create hook for glDepthFunc\n");
        return;
    }

    // Активируем хук
    if (MH_EnableHook(glDepthFunc_ptr) != MH_OK) {
        printf("[Error] Failed to enable hook for glDepthFunc\n");
        return;
    }

    printf("[Success] glDepthFunc hooked successfully!\n");
}

// Основной поток DLL
DWORD WINAPI MainThread(LPVOID lpParam) {
    // Создаем консоль для отладки
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    printf("DLL Successfully Injected into Quake 3 Arena!\n");

    // Инициализируем хуки
    InitializeHooks();

    // Консоль остается открытой
    return 0;
}

// Точка входа для DLL
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
        break;
    case DLL_PROCESS_DETACH:
        MH_Uninitialize(); // Очищаем MinHook при выгрузке
        break;
    }
    return TRUE;
}