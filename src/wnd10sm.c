/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Advanced property page
 *
 * PROGRAMMER: Franco Tortoriello (torto09@gmail.com)
 */

#include "app.h"
#include "resource.h"
#include "util.h"

#include <commctrl.h>
#include <shellapi.h>

#include <psapi.h>

#define EnableApply() \
    SendMessage(g_propSheet.hWnd, PSM_CHANGED, (WPARAM)g_hDlg, 0L)

static const TCHAR g_explorerPatcherKey[] =
    TEXT("Software\\ExplorerPatcher");

typedef struct tagTBSETTINGS
{
    BOOL b10StartMenu;
    BOOL bStartScreen;
} TBSETTINGS;

static TBSETTINGS g_oldSettings;
static TBSETTINGS g_newSettings;

static HWND g_hDlg;

static
void LoadDefaultSettings(void)
{
    g_oldSettings.b10StartMenu = TRUE;
    g_oldSettings.bStartScreen = FALSE;
}

static
void LoadRegSettings(void)
{
    HKEY hKey;
    LSTATUS status;
    DWORD dwType;
    DWORD dwData = 0;
    DWORD dwSize;

#define ReadDword(valueName) \
    dwSize = sizeof(DWORD); \
    status = RegQueryValueEx(hKey, valueName, 0, \
        &dwType, (BYTE *)&dwData, &dwSize)

#define ReadInt(valueName, member) \
    ReadDword(valueName); \
    if (status == ERROR_SUCCESS && dwType == REG_DWORD) \
        g_oldSettings.member = (int)dwData

#define ReadInvertedBool(valueName, member) \
    ReadDword(valueName); \
    if (status == ERROR_SUCCESS && dwType == REG_DWORD) \
        g_oldSettings.member = !dwData

    status = RegOpenKeyEx(HKEY_CURRENT_USER, g_explorerPatcherKey, 0,
        KEY_QUERY_VALUE, &hKey);
    if (status == ERROR_SUCCESS)
    {
        ReadInt(TEXT("UseImmersiveLauncher"), bStartScreen);
		ReadInvertedBool(TEXT("UseImmersiveLauncher"), b10StartMenu);
        RegCloseKey(hKey);
    }
    
#undef ReadInvertedBool
#undef ReadInt
#undef ReadDword
}

static
void LoadSettings(void)
{
    LoadDefaultSettings();
    LoadRegSettings();

    g_newSettings = g_oldSettings;
}

static
void UpdateControls(void)
{
#define SetChecked(iControl, bChecked) \
    SendDlgItemMessage(g_hDlg, iControl, \
        BM_SETCHECK, (WPARAM)(bChecked == 1), 0L)

    SetChecked(IDC_SM_10STARTMENU,    g_oldSettings.b10StartMenu);
    SetChecked(IDC_SM_STARTSCREEN, g_oldSettings.bStartScreen);

#undef SetChecked
}

static
void InitPage(void)
{
    LoadSettings();
    UpdateControls();
}

#define HasChanged(member) \
    (g_newSettings.member != g_oldSettings.member)

_Success_(return)
static
BOOL WriteRegSettings(void)
{
    HKEY hKey;
    LSTATUS status;
    BOOL ret = TRUE;
    DWORD dwData;

#define RestoreSetting(member) \
    g_newSettings.member = g_oldSettings.member

#define SetDword(valueName) \
    status = RegSetValueEx( \
        hKey, valueName, 0, REG_DWORD, (BYTE *)&dwData, sizeof(DWORD))

#define UpdateDword(valueName, member) \
    if (HasChanged(member)) { \
        dwData = (DWORD)g_newSettings.member; \
        SetDword(valueName); \
        if (status != ERROR_SUCCESS) { \
            RestoreSetting(member); \
            ret = FALSE; \
        } \
    }

#define UpdateDwordInverted(valueName, member) \
    if (HasChanged(member)) { \
        dwData = (DWORD)!g_newSettings.member; \
        SetDword(valueName); \
        if (status != ERROR_SUCCESS) { \
            RestoreSetting(member); \
            ret = FALSE; \
        } \
    }

    if (HasChanged(bStartScreen) || HasChanged(b10StartMenu)) {
        status = RegCreateKeyEx(HKEY_CURRENT_USER, g_explorerPatcherKey, 0, NULL,
            0, KEY_SET_VALUE, NULL, &hKey, NULL);
        if (status == ERROR_SUCCESS)
        {
            UpdateDword(TEXT("UseImmersiveLauncher"), bStartScreen);
            UpdateDwordInverted(TEXT("UseImmersiveLauncher"), b10StartMenu);
            RegCloseKey(hKey);
        }
        else
        {
            RestoreSetting(bStartScreen);
			RestoreSetting(b10StartMenu);
            ret = FALSE;
        }
    }

    if (HasChanged(b10StartMenu))
        SetCustomVisualFx();

#undef UpdateDwordInverted
#undef UpdateDword
#undef SetDword
#undef RestoreSetting

    return ret;
}

static
void ApplySettings(void)
{
    BOOL bSendSettingChange = HasChanged(b10StartMenu) || HasChanged(bStartScreen);

    if (!WriteRegSettings())
    {
        /* An error has ocurred, show previous settings */
        UpdateControls();
    }

    g_oldSettings = g_newSettings;

    if (bSendSettingChange)
    {
        SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE,
            0L, (LPARAM)TEXT("TraySettings"));
    }
}

#undef HasChanged

static
void HandleCommand(WORD iControl)
{
#define GetChecked() \
    (SendDlgItemMessage(g_hDlg, iControl, BM_GETCHECK, 0L, 0L) == BST_CHECKED)

    switch (iControl)
    {
    case IDC_SM_10STARTMENU:
        g_newSettings.b10StartMenu = GetChecked();
        break;

    case IDC_SM_STARTSCREEN:
        g_newSettings.bStartScreen = GetChecked();
        break;

    default:
        return;
    }

    EnableApply();

#undef GetChecked
}

INT_PTR CALLBACK StartMenu10PageProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        g_hDlg = hWnd;
        InitPage();
        return 0;

    case WM_COMMAND:
        switch HIWORD(wParam)
        {
        case BN_CLICKED:
            HandleCommand(LOWORD(wParam));
            break;
        }

        return 0;

    case WM_NOTIFY:
        switch (((NMHDR *)lParam)->code)
        {
        case PSN_APPLY:
            ApplySettings();
            SetWindowLongPtr(hWnd, DWLP_MSGRESULT, (LONG_PTR)PSNRET_NOERROR);
            return TRUE;

        case PSN_KILLACTIVE:
            SetWindowLongPtr(hWnd, DWLP_MSGRESULT, (LONG_PTR)FALSE);
            return TRUE;

        }

        return 0;
    }

    return 0;
}
