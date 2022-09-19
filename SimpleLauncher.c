#include <windows.h>
#include <stdio.h>
#include "SimpleLauncherLib.h"
#pragma comment(lib, "user32.lib")

int numberOfKeyBindings;
KeyBinding **keyBindings;

HHOOK g_kb_hook = 0;

LRESULT CALLBACK key_press_callback(int code, WPARAM w, LPARAM l)
{
    BOOL handleResult = handle_key_press(code, w, l, keyBindings, numberOfKeyBindings);

    if(handleResult)
    {
        return 1;
    }

    return CallNextHookEx(g_kb_hook, code, w, l);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    HINSTANCE moduleHandle = GetModuleHandle(NULL);
    keyBindings = create_custom_key_bindings(&numberOfKeyBindings);

    g_kb_hook = SetWindowsHookEx(WH_KEYBOARD_LL, &key_press_callback, moduleHandle, 0);
    if (g_kb_hook == NULL)
    {
        fprintf (stderr, "SetWindowsHookEx WH_KEYBOARD_LL [%p] failed with error %d\n", moduleHandle, GetLastError ());
        return 0;
    };

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    };

    return 0;
}
