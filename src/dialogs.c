/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Customize Start menu dialogs
 *
 * PROGRAMMERS: SpaofSpaac
 *              Franco Tortoriello (torto09@gmail.com)
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
	
static const TCHAR g_explorerKey[] =
    TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced");
	
static const TCHAR g_explorerPatcherKey[] =
    TEXT("Software\\ExplorerPatcher");
	
typedef struct tagTBDSETTINGS
{
	int iPrograms;
	int iItems;
    int iMode;

} TBDSETTINGS;

static TBDSETTINGS g_oldSettings;
static TBDSETTINGS g_newSettings;

static HWND g_hDlg;

#define UpdateUpDown(iControl, member) \
    SendDlgItemMessage(g_hDlg, iControl, \
        UDM_SETPOS, 0L, (LPARAM)member)

static
void SetRanges(void)
{
#define SetRange(iControl, min, max) \
    SendDlgItemMessage(g_hDlg, iControl, UDM_SETRANGE, 0L, MAKELONG(max, min))

    SetRange(IDC_SM_MFU_PROGRAMS_SPIN, 0, 30);
    SetRange(IDC_SM_MFU_ITEMS_SPIN, 0, 60);

#undef SetRanges
}

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

    InitCombo(IDC_SM_10DLG_MODE, IDS_SM_10DLG_MODE_DEFAULT, 3);

#undef InitCombo
}


_Success_(return)
static
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
	
	status = RegOpenKeyEx(HKEY_CURRENT_USER, g_explorerKey, 0,
        KEY_QUERY_VALUE, &hKey);
    if (status != ERROR_SUCCESS)
        return;

    ReadInt(TEXT("Start_MinMFU"), iPrograms);
    ReadInt(TEXT("Start_JumpListItems"), iItems);

    RegCloseKey(hKey);

#undef ReadInvertedBool
#undef ReadInt
#undef ReadDword
}

static
void LoadDefaultSettings(void)
{
    g_oldSettings.iMode = 0;
	
	g_oldSettings.iPrograms = 10;
	g_oldSettings.iItems = 10;
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
    SetComboIndex(IDC_SM_10DLG, g_oldSettings.iMode);
	
	UpdateUpDown(IDC_SM_MFU_PROGRAMS_SPIN, g_oldSettings.iPrograms);
	UpdateUpDown(IDC_SM_MFU_ITEMS_SPIN, g_oldSettings.iItems);

}


static
void InitPage(void)
{
    LoadSettings();
    InitComboBoxes();
	UpdateControls();
	SetRanges();
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
	
	   if (HasChanged(iPrograms) || HasChanged(iItems))
    {
        status = RegCreateKeyEx(HKEY_CURRENT_USER, g_explorerKey, 0, NULL, 0,
            KEY_SET_VALUE, NULL, &hKey, NULL);
        if (status == ERROR_SUCCESS)
        {
            UpdateDword(TEXT("Start_MinMFU"), iPrograms);
            UpdateDword(TEXT("Start_JumpListItems"), iItems);

            RegCloseKey(hKey);
        }
        else
        {
            RestoreSetting(iPrograms);
            RestoreSetting(iItems);
            
            ret = FALSE;
        }
    }

#undef UpdateDwordInverted
#undef UpdateDword
#undef SetDword

    RegCloseKey(hKey);

    return ret;

}

static
void ApplySettings(void)
{
    BOOL bSendSettingChange = HasChanged(iMode) || HasChanged(iPrograms) || HasChanged(iItems);

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
void HandleEditChange(WORD iControl)
{
#define GetUdPos(iUd) \
    (int)LOWORD(SendDlgItemMessage(g_hDlg, iUd, UDM_GETPOS, 0L, 0L))

    switch (iControl)
    {
    case IDC_SM_MFU_PROGRAMS_SPIN:
        g_newSettings.iPrograms = GetUdPos(IDC_SM_MFU_PROGRAMS_SPIN);
        break;

    case IDC_SM_MFU_ITEMS_SPIN:
        g_newSettings.iItems = GetUdPos(IDC_SM_MFU_ITEMS_SPIN);
        break;

    default:
        return;
    }

#undef GetUdPos

    EnableApply();
}

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
		
    case IDOK:
        EndDialog(g_hDlg, iControl);
        break;
		
    case IDCANCEL:
        EndDialog(g_hDlg, iControl);
        break;
		
	case IDC_SM_DEFAULT_BUTTON: 
        RestoreSetting(iPrograms);
        RestoreSetting(iItems);
        break;
			
    default:
        return;
    }

    EnableApply();

#undef GetComboIndex
}

static
void HandleComboBoxSelChange(WORD iControl)
{
#define GetComboIndex() \
    (BYTE)SendDlgItemMessage(g_hDlg, iControl, CB_GETCURSEL, 0L, 0L)

    switch (iControl)
    {
    case IDC_SM_10DLG_MODE:
        g_newSettings.iMode = GetComboIndex();
        break;

    default:
        return;
    }

    EnableApply();

#undef GetComboIndex
}

INT_PTR CALLBACK StartMenu10DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
			
        case CBN_SELCHANGE:
            HandleComboBoxSelChange(LOWORD(wParam));
            break;			
        }

        return 0;

	case WM_CLOSE:
		EndDialog(g_hDlg, uMsg);
		return 0;

    }

    return 0;
}

INT_PTR CALLBACK StartMenu7DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
			
        case CBN_SELCHANGE:
            HandleComboBoxSelChange(LOWORD(wParam));
            break;			
        }

        return 0;

	case WM_CLOSE:
		EndDialog(g_hDlg, uMsg);
		return 0;

    }

    return 0;
}