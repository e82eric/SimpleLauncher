#include <windows.h>
#include <stdio.h>
#pragma comment(lib, "user32.lib")

#define VK_A 0x41
#define VK_B 0x42
#define VK_C 0x43
#define VK_D 0x44
#define VK_E 0x45
#define VK_F 0x46
#define VK_G 0x47
#define VK_H 0x48
#define VK_I 0x49
#define VK_J 0x4A
#define VK_K 0x4B
#define VK_L 0x4C
#define VK_M 0x4D
#define VK_N 0x4E
#define VK_O 0x4F
#define VK_P 0x50
#define VK_Q 0x51
#define VK_R 0x52
#define VK_S 0x53
#define VK_T 0x54
#define VK_U 0x55
#define VK_V 0x56
#define VK_W 0x57
#define VK_X 0x58
#define VK_Y 0x59
#define VK_Z 0x5A

#define VK_0 0x30
#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33
#define VK_4 0x34
#define VK_5 0x35
#define VK_6 0x36
#define VK_7 0x37
#define VK_8 0x38
#define VK_9 0x39

static TCHAR *cmdLineExe = L"C:\\Windows\\System32\\cmd.exe";
static TCHAR *cmdScratchCmd = L"cmd /k";
static TCHAR *launcherCommand = L"cmd /c for /f \"delims=\" %i in ('fd -t f -g \"*{.lnk,.exe}\" \"%USERPROFILE\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\" \"C:\\ProgramData\\Microsoft\\Windows\\Start Menu\" \"C:\\Users\\eric\\AppData\\Local\\Microsoft\\WindowsApps\" ^| fzf --bind=\"ctrl-c:execute(echo {} | clip)\"') do start \" \" \"%i\"";
static TCHAR *allFilesCommand = L"cmd /c set /p \"pth=Enter Path: \" && for /f \"delims=\" %i in ('fd . -t f %^pth% ^| fzf --bind=\"ctrl-c:execute(echo {} | clip)\"') do start \" \" \"%i\"";
static TCHAR *processListCommand = L"/c tasklist /nh | sort | fzf -e --bind=\"ctrl-k:execute(for /f \\\"tokens=2\\\" %f in ({}) do taskkill /f /pid %f)+reload(tasklist /nh | sort)\" --bind=\"ctrl-r:reload(tasklist /nh | sort)\" --header \"ctrl-k Kill Pid\" --reverse";

enum Modifiers
{
    LShift = 0x1,
    RShift = 0x2,
    LAlt = 0x4,
    RAlt = 0x8,
    LWin = 0x10,
    RWin = 0x20,
    RCtl = 0x40,
    LCtl = 0x80
};

typedef struct KeyBinding KeyBinding;

struct KeyBinding
{
    int modifiers;
    unsigned int key;
    void (*action) (void);
    void (*cmdAction) (TCHAR*);
    TCHAR *cmdArg;
};

int numberOfKeyBindings;
KeyBinding **keyBindings;

HHOOK g_kb_hook = 0;

LRESULT CALLBACK key_bindings(int code, WPARAM w, LPARAM l)
{
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
                    return 1;
                }
            }
        }
    }

    return CallNextHookEx(g_kb_hook, code, w, l);
}

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

KeyBinding* keybinding_create_cmd_args(int modifiers, unsigned int key, void (*action) (TCHAR*), TCHAR *cmdArg)
{
    KeyBinding *result = calloc(1, sizeof(KeyBinding));
    result->modifiers = modifiers;
    result->key = key;
    result->cmdAction = action;
    result->cmdArg = cmdArg;
    return result;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    HINSTANCE moduleHandle = GetModuleHandle(NULL);
    g_kb_hook = SetWindowsHookEx(WH_KEYBOARD_LL, &key_bindings, moduleHandle, 0);
    if (g_kb_hook == NULL)
    {
        fprintf (stderr, "SetWindowsHookEx WH_KEYBOARD_LL [%p] failed with error %d\n", moduleHandle, GetLastError ());
        return 0;
    };

    numberOfKeyBindings = 7;
    keyBindings = calloc(numberOfKeyBindings, sizeof(KeyBinding*));
    keyBindings[0] = keybinding_create_cmd_args(LWin, VK_2, start_scratch_not_elevated, cmdScratchCmd);
    keyBindings[1] = keybinding_create_cmd_args(LShift | LWin, VK_2, start_launcher, cmdScratchCmd);
    keyBindings[2] = keybinding_create_cmd_args(LWin, VK_3, start_scratch_not_elevated, launcherCommand);
    keyBindings[3] = keybinding_create_cmd_args(LShift | LWin, VK_3, start_launcher, launcherCommand);
    keyBindings[4] = keybinding_create_cmd_args(LWin, VK_4, start_scratch_not_elevated, allFilesCommand);
    keyBindings[5] = keybinding_create_cmd_args(LShift | LWin, VK_4, start_launcher, allFilesCommand);
    keyBindings[6] = keybinding_create_cmd_args(LWin, VK_9, start_launcher, processListCommand);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    };

    return 0;
}
