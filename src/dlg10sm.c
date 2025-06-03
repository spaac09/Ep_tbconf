/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Animations dialog
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

#define SetComboIndex(iControl, index) \
    SendDlgItemMessage(g_hDlg, iControl, CB_SETCURSEL, (WPARAM)index, 0L)	
	
static const TCHAR g_explorerPatcherKey[] =
    TEXT("Software\\ExplorerPatcher");
	
typedef struct tagTBSETTINGS
{
    BOOL iMode;

} TBSETTINGS;

static TBSETTINGS g_oldSettings;
static TBSETTINGS g_newSettings;

static HWND g_hDlg;

static
void InitComboBoxes(void)
{
    int iElement;
    TCHAR text[60] = TEXT("\0");

#define InitCombo(iControl, iString, nElements) \
    for (iElement = 0; iElement < nElements; iElement++) { \
        LoadString(g_propSheet.hInstance, iString + iElement, \
            (TCHAR *)&text, 59); \
        SendDlgItemMessage(g_hDlg, iControl, CB_ADDSTRING, 0L, (LPARAM)&text); \
    }

    InitCombo(IDC_SM_10DLG_MODE, IDS_TB_POS_L, 4);

#undef InitCombo
}


_Success_(return)
/*static
void LoadRegSettings(void)
{
    HKEY hKey;
    DWORD dwType;
    DWORD dwData = 0;
    DWORD dwSize;

#define ReadDword(valueName) \
    dwSize = sizeof(DWORD); \
    status = RegQueryValueEx(hKey, valueName, 0, &dwType, \
        (BYTE *)&dwData, &dwSize)

#define ReadInt(valueName, member) \
    ReadDword(valueName); \
    if (status == ERROR_SUCCESS && dwType == REG_DWORD) \
        g_oldSettings.member = (int)dwData

#define ReadInvertedBool(valueName, member) \
    ReadDword(valueName); \
    if (status == ERROR_SUCCESS && dwType == REG_DWORD) \
        g_oldSettings.member = !dwData

    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, g_explorerPatcherKey, 0,
        KEY_QUERY_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return;

    ReadInt(TEXT("StartUI_EnableRoundedCorners"), iMode);

    RegCloseKey(hKey);

#undef ReadInvertedBool
#undef ReadInt
#undef ReadDword
}*/

static
void LoadDefaultSettings(void)
{
    g_oldSettings.iMode = FALSE;
}


static
void LoadSettings(void)
{
    LoadDefaultSettings();

    g_newSettings = g_oldSettings;
}

static
void UpdateControls(void)
{
    SetComboIndex(IDC_SM_10DLG, g_oldSettings.iMode);

}


static
void InitPage(void)
{
    LoadSettings();
    InitComboBoxes();
	UpdateControls();
}

#define HasChanged(member) \
    (g_newSettings.member != g_oldSettings.member)

static
BOOL WriteRegSettings(void)
{
#define RestoreSetting(member) \
    g_newSettings.member = g_oldSettings.member

    HKEY hKey;
    LSTATUS status = RegCreateKeyEx(HKEY_CURRENT_USER, g_explorerPatcherKey, 0, NULL, 0,
        KEY_SET_VALUE, NULL, &hKey, NULL);
    if (status != ERROR_SUCCESS)
    {
        RestoreSetting(iMode);
        return FALSE;
    }

    BOOL ret = TRUE;
    DWORD dwData;

#define SetDword(valueName) \
    status = RegSetValueEx(hKey, valueName, 0, REG_DWORD, \
        (BYTE *)&dwData, sizeof(DWORD))

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

    UpdateDword(TEXT("StartUI_EnableRoundedCorners"), iMode);

#undef UpdateDwordInverted
#undef UpdateDword
#undef SetDword

    RegCloseKey(hKey);

    return ret;

#undef RestoreSetting
}

static
void ApplySettings(void)
{
    BOOL bSendSettingChange = HasChanged(iMode);

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
#define GetComboIndex() \
    (BYTE)SendDlgItemMessage(g_hDlg, iControl, CB_GETCURSEL, 0L, 0L)
	
    switch (iControl)
    {
    case IDC_SM_10DLG_MODE:
        g_newSettings.iMode = GetComboIndex();
        break;
		
	case IDC_SM_OK_BUTTON:
		EndDialog(g_hDlg, iControl);
		break;
			
    default:
        return;
    }

    EnableApply();

#undef GetComboIndex
}

INT_PTR CALLBACK AnimationsDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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


