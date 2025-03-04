#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>
#include "MinHook.h"

// Перехватываем функции
typedef void (WINAPI* glDepthFunc_t)(GLenum func);
glDepthFunc_t original_glDepthFunc = nullptr;

typedef void (WINAPI* glEnable_t)(GLenum cap);
glEnable_t original_glEnable = nullptr;

typedef void (WINAPI* glDisable_t)(GLenum cap);
glDisable_t original_glDisable = nullptr;

typedef void (WINAPI* glDepthMask_t)(GLboolean flag);
glDepthMask_t original_glDepthMask = nullptr;

// Глобальные переменные
bool wallhackEnabled = false;

// Хук для glDepthFunc
void WINAPI Hooked_glDepthFunc(GLenum func) {
    if (wallhackEnabled) {
        original_glDepthFunc(GL_ALWAYS); // Всегда проходим проверку глубины
    }
    else {
        original_glDepthFunc(func);
    }
}

// Хук для glEnable
void WINAPI Hooked_glEnable(GLenum cap) {
    if (wallhackEnabled && cap == GL_DEPTH_TEST) {
        return; // Блокируем включение Depth Test
    }
    original_glEnable(cap);
}

// Хук для glDisable
void WINAPI Hooked_glDisable(GLenum cap) {
    if (wallhackEnabled && cap == GL_DEPTH_TEST) {
        return; // Оставляем Depth Test выключенным
    }
    original_glDisable(cap);
}

// Хук для glDepthMask
void WINAPI Hooked_glDepthMask(GLboolean flag) {
    if (wallhackEnabled) {
        original_glDepthMask(GL_FALSE); // Отключаем запись в буфер глубины
    }
    else {
        original_glDepthMask(flag); // Оригинальное поведение
    }
}

// Проверка нажатия клавиши
void CheckWallhackToggle() {
    static bool lastState = false;
    if (GetAsyncKeyState(VK_F1) & 0x8000) {
        wallhackEnabled = !wallhackEnabled;
        if (wallhackEnabled != lastState) {
            printf("[Wallhack] Toggled to: %s\n", wallhackEnabled ? "ON" : "OFF");
            lastState = wallhackEnabled;
            if (wallhackEnabled) {
                original_glDisable(GL_DEPTH_TEST); // Принудительно отключаем Depth Test
                original_glDepthMask(GL_FALSE);   // Отключаем запись глубины
            }
            else {
                original_glEnable(GL_DEPTH_TEST); // Восстанавливаем нормальный рендер
                original_glDepthMask(GL_TRUE);
            }
        }
        Sleep(200); // Анти-спам
    }
}

// Инициализация хуков
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

    // Хук glDepthFunc
    void* glDepthFunc_ptr = GetProcAddress(opengl32, "glDepthFunc");
    if (MH_CreateHook(glDepthFunc_ptr, &Hooked_glDepthFunc, (LPVOID*)&original_glDepthFunc) != MH_OK ||
        MH_EnableHook(glDepthFunc_ptr) != MH_OK) {
        printf("[Error] Failed to hook glDepthFunc\n");
    }

    // Хук glEnable
    void* glEnable_ptr = GetProcAddress(opengl32, "glEnable");
    if (MH_CreateHook(glEnable_ptr, &Hooked_glEnable, (LPVOID*)&original_glEnable) != MH_OK ||
        MH_EnableHook(glEnable_ptr) != MH_OK) {
        printf("[Error] Failed to hook glEnable\n");
    }

    // Хук glDisable
    void* glDisable_ptr = GetProcAddress(opengl32, "glDisable");
    if (MH_CreateHook(glDisable_ptr, &Hooked_glDisable, (LPVOID*)&original_glDisable) != MH_OK ||
        MH_EnableHook(glDisable_ptr) != MH_OK) {
        printf("[Error] Failed to hook glDisable\n");
    }

    // Хук glDepthMask
    void* glDepthMask_ptr = GetProcAddress(opengl32, "glDepthMask");
    if (MH_CreateHook(glDepthMask_ptr, &Hooked_glDepthMask, (LPVOID*)&original_glDepthMask) != MH_OK ||
        MH_EnableHook(glDepthMask_ptr) != MH_OK) {
        printf("[Error] Failed to hook glDepthMask\n");
    }

    printf("[Success] All hooks initialized!\n");
}

// Основной поток
DWORD WINAPI MainThread(LPVOID lpParam) {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    printf("DLL Successfully Injected into Quake 3 Arena!\n");
    printf("Press F1 to toggle Wallhack\n");

    InitializeHooks();

    while (true) {
        CheckWallhackToggle();
        Sleep(100);
    }

    return 0;
}

// Точка входа
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