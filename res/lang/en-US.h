#define RES_LANG         LANG_ENGLISH
#define RES_SUBLANG      SUBLANG_ENGLISH_US

#define S_ERROR          "Error"
#define S_ERROR_GENERIC  "An error has occurred"
#define S_ERROR_MEM      "Could not allocate memory"

#define S_TB  "Taskbar"
#define S_TB_TITLE  "Taskbar and Start Menu"

#define S_TB_LOCK               "&Lock the taskbar"
#define S_TB_AUTOHIDE           "A&uto-hide the taskbar"
#define S_TB_SMALLBUTTONS       "Us&e small taskbar buttons"
#define S_TB_BADGES             "Show badges on buttons"
#define S_TB_LOCATION           "&Taskbar location on screen:"
#define S_TB_COMBINEBUTTONS     "Taskbar &buttons:"
#define S_TB_TRAYWND            "Notification area:"
#define S_TB_TRAYWND_CUSTOMIZE  "&Customize..."
#define S_TB_PEEK               "Use &Peek to preview the desktop when you move the mouse to the Show desktop button at the end of the taskbar"

#define S_TB_MM                "Multiple displays"
#define S_TB_ALLDISPLAYS       "&Show taskbar on all displays"
#define S_TB_MMDISPLAYS        "S&how buttons on:"
#define S_TB_MMCOMBINEBUTTONS  "Combine buttons on &other taskbars:"

#define S_TB_TB_7               "Taskbar appearance"
#define S_TB_SMALLBUTTONS_7		"Use small &icons"
#define S_TB_NA_7               "Notification area"
#define S_TB_TRAYWND_7          "Customise which icons and notifications appear in the notification area."
#define S_TB_PEEKBOX_7          "Preview desktop with Aero Peek"
#define S_TB_PEEKBOX_TEXT_7     "Temporarily view the desktop when you move your mouse to the Show desktop button at end of the taskbar."
#define S_TB_PEEK_7             "Use Aero &Peek to preview the desktop"

#define S_SM  "Start Menu"

#define S_SM_DEFAULTMENU  "Default Start menu"
#define S_SM_STARTSCREEN  "Windows 8 Start screen"

#define S_SM_10DLG  "Customize Start Menu"

#define S_NA  "Notification Area"
#define S_NA_ICON  "Icons"
#define S_NA_ICON_TEXT "You can keep the notification area uncluttered by hiding icons that you have not clicked recently."
#define S_NA_SYSICON  "System icons"
#define S_NA_WIN32BATTERY "Use legacy battery flyout"
#define S_NA_WIN32SOUND "Use legacy sound flyout"
#define S_NA_CLOCK_TEXT "Clock flyout:"
#define S_NA_NETWORK_TEXT "Network flyout:"
#define S_NA_USERTILE "Show user tile"

#define S_ADV  "Advanced"

#define S_ADV_PERFORMANCE     "Performance"
#define S_ADV_ANIMATIONS      "Enable additional &animations"
#define S_ADV_SAVETHUMBNAILS  "Save window preview &thumbnails"

#define S_ADV_NAVIGATION      "Navigation"
#define S_ADV_WINXPOWERSHELL  "Replace Command Prompt with &PowerShell in the menu when right-clicking the start button or pressing Windows+X *"

#define S_ADV_MISC            "Miscellaneous"
#define S_ADV_SHOWDESKTOP     "Enable Show &Desktop button"
#define S_ADV_TOGGLEAUTOHIDE  "Toggle &hiding the taskbar when double clicking it **"

#define S_ADV_RESTARTEXPLORER_T  "* Requires restarting Explorer after applying to take effect"
#define S_ADV_EXPLORERPATCHER_T  "** Requires ExplorerPatcher or similar software"
#define S_ADV_RESTARTEXPLORER    "<A ID=""restart"">Restart Explorer</A>"

#define S_TB_POS_L      "Left"
#define S_TB_POS_T      "Top"
#define S_TB_POS_R      "Right"
#define S_TB_POS_B      "Bottom"
#define S_TB_COMB_YES   "Always combine, hide labels"
#define S_TB_COMB_FULL  "Combine when taskbar is full"
#define S_TB_COMB_NO    "Never combine"
#define S_TB_MMALL      "All taskbars"
#define S_TB_MMMAIN     "Main taskbar and taskbar where the window is open"
#define S_TB_MMCUR      "Taskbar where the window is open"
#define S_TB_HELPLINK	"<A ID=""helplink"">How do I customize taskbars?</A>"
#define S_TB_HELPLINK_7	"<A ID=""helplink"">How do I customize the taskbar?</A>"

#define S_SM_PRIVACY	"Privacy"
#define S_SM_TRACKPROGS	"Store and display recently opened &programs in the Start menu"
#define S_SM_TRACKDOCS	"Store and display recently opened items in the Start &menu and the taskbar"
#define S_SM_HELPLINK	"<A ID=""helplink"">How do I change the way the Start menu looks?</A>"

#define S_SM_10DLG_MODE_DEFAULT      "Default look"
#define S_SM_10DLG_MODE_ROUNDED      "Rounded corners"
#define S_SM_10DLG_MODE_FLOATING     "Rounded corners, floating menu"

#define S_NA_CLOCK_WIN32      "Windows 7"
#define S_NA_CLOCK_10         "Windows 10"
#define S_NA_CLOCK_ACTION     "Action Center"

#define S_NA_NETWORK_10			"Windows 10"
#define S_NA_NETWORK_8			"Windows 8"
#define S_NA_NETWORK_SETTINGS	"Network section in the Settings app"
#define S_NA_NETWORK_NETCENTER	"Network and Sharing Center in Control Panel"
#define S_NA_NETWORK_NETCON		"Network Connections in Control Panel"

#ifdef BLUEPILL
#include "template7.rc"
#else
#include "template.rc"
#endif


#include "undef.h"
