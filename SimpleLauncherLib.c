#include <stdio.h>
#include <windows.h>
#include <shellapi.h>
#include "SimpleLauncherLib.h"
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Shell32.lib")

static TCHAR *cmdLineExe = L"C:\\Windows\\System32\\cmd.exe";
void start_scratch_not_elevated(TCHAR *cmdArgs)
{
    HWND hwnd = GetShellWindow();

    SIZE_T size = 0;

    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);

    HANDLE process =
        OpenProcess(PROCESS_CREATE_PROCESS, FALSE, pid);

    STARTUPINFOEX siex;
    ZeroMemory(&siex, sizeof(siex));
    InitializeProcThreadAttributeList(NULL, 1, 0, &size);
    siex.StartupInfo.cb = sizeof(siex);
    siex.StartupInfo.lpTitle = L"SimpleWindowManager Scratch";
    siex.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(
        GetProcessHeap(),
        0,
        size
    );

    InitializeProcThreadAttributeList(siex.lpAttributeList, 1, 0, &size);

    UpdateProcThreadAttribute(
        siex.lpAttributeList,
        0,
        PROC_THREAD_ATTRIBUTE_PARENT_PROCESS,
        &process,
        sizeof(process),
        NULL,
        NULL);

    PROCESS_INFORMATION pi;

    CreateProcessW(
        cmdLineExe,
        cmdArgs,
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE | EXTENDED_STARTUPINFO_PRESENT,
        NULL,
        NULL,
        &siex.StartupInfo,
        &pi);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(process);
}

void start_launcher(TCHAR *cmdArgs)
{
    STARTUPINFO si = { 0 };
    si.dwFlags = STARTF_USEPOSITION |  STARTF_USESIZE | STARTF_USESHOWWINDOW;
    si.dwX= 200;
    si.dwY = 100;
    si.dwXSize = 2000;
    si.dwYSize = 1200;
    si.wShowWindow = SW_HIDE;
    si.lpTitle = L"SimpleWindowManager Scratch";

    PROCESS_INFORMATION pi = { 0 };

    if( !CreateProcess(
        cmdLineExe,
        cmdArgs,
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &si,
        &pi)
    ) 
    {
        return;
    }

    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}

void start_app(TCHAR *cmdArgs)
{
    STARTUPINFO si = { 0 };
    si.dwFlags = STARTF_USEPOSITION |  STARTF_USESIZE | STARTF_USESHOWWINDOW;
    si.dwX= 200;
    si.dwY = 100;
    si.dwXSize = 2000;
    si.dwYSize = 1200;
    si.wShowWindow = SW_HIDE;
    si.lpTitle = L"SimpleWindowManager Scratch";

    PROCESS_INFORMATION pi = { 0 };

    if( !CreateProcess(
        cmdArgs,
        NULL,
        NULL,
        NULL,
        FALSE,
        NORMAL_PRIORITY_CLASS,
        NULL,
        NULL,
        &si,
        &pi)
    ) 
    {
        return;
    }

    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}

BOOL handle_key_press(int code, WPARAM w, LPARAM l, KeyBinding** keyBindings, int numberOfKeyBindings)
{
    BOOL result = FALSE;
    PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)l;
    if (code ==0 && (w == WM_KEYDOWN || w == WM_SYSKEYDOWN))
    {
        for(int i = 0; i < numberOfKeyBindings; i++)
        {
            if(p->vkCode == keyBindings[i]->key)
            {
                int modifiersPressed = 0;
                if(GetKeyState(VK_LSHIFT) & 0x8000)
                {
                    modifiersPressed |= LShift;
                }
                if(GetKeyState(VK_RSHIFT) & 0x8000)
                {
                    modifiersPressed |= RShift;
                }
                if(GetKeyState(VK_LMENU) & 0x8000)
                {
                    modifiersPressed |= LAlt;
                }
                if(GetKeyState(VK_RMENU) & 0x8000)
                {
                    modifiersPressed |= RAlt;
                }
                if(GetKeyState(VK_CONTROL) & 0x8000)
                {
                    modifiersPressed |= LCtl;
                }
                if(GetKeyState(VK_LWIN) & 0x8000)
                {
                    modifiersPressed |= LWin;
                }
                if(GetKeyState(VK_RWIN) & 0x8000)
                {
                    modifiersPressed |= RWin;
                }

                if(keyBindings[i]->modifiers == modifiersPressed)
                {
                    if(keyBindings[i]->action)
                    {
                        keyBindings[i]->action();
                    }
                    else if(keyBindings[i]->cmdAction)
                    {
                        keyBindings[i]->cmdAction(keyBindings[i]->cmdArg);
                    }
                    /* else if(keyBindings[i]->workspaceAction) */
                    /* { */
                    /*     keyBindings[i]->workspaceAction(keyBindings[i]->workspaceArg); */
                    /* } */
                    result = TRUE;
                }
            }
        }
    }

    return result;
}

KeyBinding* keybinding_create_cmd_args(int modifiers, unsigned int key, void (*action) (TCHAR*), TCHAR *cmdArg)
{
    KeyBinding *result = calloc(1, sizeof(KeyBinding));
    result->modifiers = modifiers;
    result->key = key;
    result->cmdAction = action;
    result->cmdArg = cmdArg;
    return result;
}
