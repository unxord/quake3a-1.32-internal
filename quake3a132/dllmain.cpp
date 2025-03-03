#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>
#include "MinHook.h"

// Определяем тип функции glDepthFunc
typedef void (WINAPI* glDepthFunc_t)(GLenum func);
glDepthFunc_t original_glDepthFunc = nullptr;

// Глобальная переменная для включения/выключения Wallhack
bool wallhackEnabled = false;

// Наша замена для glDepthFunc
void WINAPI Hooked_glDepthFunc(GLenum func) {
    if (wallhackEnabled) {
        // Отключаем проверку глубины для Wallhack
        glDisable(GL_DEPTH_TEST);
        printf("[Wallhack] Depth Test disabled\n");
    }
    else {
        // Восстанавливаем нормальное поведение
        glEnable(GL_DEPTH_TEST);
        original_glDepthFunc(func);
        printf("[Wallhack] Depth Test enabled, original func: %d\n", func);
    }
}

// Функция для проверки нажатия клавиши
void CheckWallhackToggle() {
    if (GetAsyncKeyState(VK_F1) & 0x8000) { // Проверяем нажатие F1
        wallhackEnabled = !wallhackEnabled; // Переключаем состояние
        printf("[Wallhack] Toggled to: %s\n", wallhackEnabled ? "ON" : "OFF");
        Sleep(200); // Задержка для предотвращения многократного срабатывания
    }
}

// Функция для инициализации хуков
void InitializeHooks() {
    if (MH_Initialize() != MH_OK) {
        printf("[Error] Failed to initialize MinHook\n");
        return;
    }

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

    if (MH_CreateHook(glDepthFunc_ptr, &Hooked_glDepthFunc, (LPVOID*)&original_glDepthFunc) != MH_OK) {
        printf("[Error] Failed to create hook for glDepthFunc\n");
        return;
    }

    if (MH_EnableHook(glDepthFunc_ptr) != MH_OK) {
        printf("[Error] Failed to enable hook for glDepthFunc\n");
        return;
    }

    printf("[Success] glDepthFunc hooked successfully!\n");
}

// Основной поток DLL
DWORD WINAPI MainThread(LPVOID lpParam) {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    printf("DLL Successfully Injected into Quake 3 Arena!\n");
    printf("Press F1 to toggle Wallhack\n");

    InitializeHooks();

    // Цикл для проверки клавиши
    while (true) {
        CheckWallhackToggle();
        Sleep(100); // Уменьшаем нагрузку на процессор
    }

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
        MH_Uninitialize();
        break;
    }
    return TRUE;
}