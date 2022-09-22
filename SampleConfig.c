#include <stdio.h>
#include <windows.h>
#include "SimpleLauncherLib.h"
#pragma comment(lib, "user32.lib")

static TCHAR *cmdScratchCmd = L"cmd /k";
static TCHAR *launcherCommand = L"cmd /c for /f \"delims=\" %i in ('fd -t f -g \"*{.lnk,.exe}\" \"%USERPROFILE\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\" \"C:\\ProgramData\\Microsoft\\Windows\\Start Menu\" \"C:\\Users\\eric\\AppData\\Local\\Microsoft\\WindowsApps\" ^| fzf --bind=\"ctrl-c:execute(echo {} | clip)\"') do start \" \" \"%i\"";
static TCHAR *allFilesCommand = L"cmd /c set /p \"pth=Enter Path: \" && for /f \"delims=\" %i in ('fd . -t f %^pth% ^| fzf --bind=\"ctrl-c:execute(echo {} | clip)\"') do start \" \" \"%i\"";
static TCHAR *processListCommand = L"/c tasklist /nh | sort | fzf -e --bind=\"ctrl-k:execute(for /f \\\"tokens=2\\\" %f in ({}) do taskkill /f /pid %f)+reload(tasklist /nh | sort)\" --bind=\"ctrl-r:reload(tasklist /nh | sort)\" --header \"ctrl-k Kill Pid\" --reverse";

KeyBinding** create_custom_key_bindings(int* outSize)
{
    int *numberOfKeyBindings = malloc(sizeof(int));
    *numberOfKeyBindings = 7;

    KeyBinding** result = calloc(*numberOfKeyBindings, sizeof(KeyBinding*));
    result[0] = keybinding_create_cmd_args(LWin, VK_2, start_scratch_not_elevated, cmdScratchCmd);
    result[1] = keybinding_create_cmd_args(LShift | LWin, VK_2, start_launcher, cmdScratchCmd);
    result[2] = keybinding_create_cmd_args(LWin, VK_3, start_scratch_not_elevated, launcherCommand);
    result[3] = keybinding_create_cmd_args(LShift | LWin, VK_3, start_launcher, launcherCommand);
    result[4] = keybinding_create_cmd_args(LWin, VK_4, start_scratch_not_elevated, allFilesCommand);
    result[5] = keybinding_create_cmd_args(LShift | LWin, VK_4, start_launcher, allFilesCommand);
    result[6] = keybinding_create_cmd_args(LWin, VK_9, start_launcher, processListCommand);

    *outSize = *numberOfKeyBindings;
    return result;
}
