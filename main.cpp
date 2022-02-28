////////////////////////////////////////////////////////////////////////////////
// SOURCES
// https://docs.microsoft.com/en-us/windows/win32/
// https://github.com/tringi/win32-dpi
////////////////////////////////////////////////////////////////////////////////



//******************************************************************************
// INCLUDES
//******************************************************************************
#include <windows.h>
#include <commctrl.h>

#include <vcl.h>
#pragma hdrstop

#include "utils.h"

#include "msgBoxHooked.h"
#include "customDlg1.h"
#include "customDlg2.h"
//******************************************************************************


//******************************************************************************
// DEFINES
//******************************************************************************
#define WINDOW_CLASS_NAME L"WND_MAIN"
#define BUTTON_CLASS_NAME L"BUTTON"

#define BUTTONS_WIDTH 200
#define BUTTONS_HEIGHT 100

#define WND_MAIN_NUM_CTRLS 7

#define IDC_DLG_BUTTON_1 101
#define IDC_DLG_BUTTON_2 102
#define IDC_DLG_BUTTON_3 103
#define IDC_DLG_BUTTON_4 104
#define IDC_DLG_BUTTON_5 105
#define IDC_DLG_BUTTON_6 106

#define IDC_CHK_BOX_1 107
//******************************************************************************


//******************************************************************************
// GLOBAL VARS
//******************************************************************************
const bool useDefaultWndProcForPaint = true;

int wndMainCtrlNum = 0;
//******************************************************************************


//******************************************************************************
// METHODS
//******************************************************************************

void wndAutosize(HWND hWndParent)
{
	BOOL checked = IsDlgButtonChecked(hWndParent, IDC_CHK_BOX_1);
	if(!checked) return;

	HDWP hdwp = BeginDeferWindowPos(WND_MAIN_NUM_CTRLS);

	if(hdwp) hdwp = DeferWindowPos(	hdwp, GetDlgItem(hWndParent, IDC_DLG_BUTTON_1), NULL,
									0, 0, 100, 200,
									SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);

	if(hdwp) hdwp = DeferWindowPos(	hdwp, GetDlgItem(hWndParent, IDC_DLG_BUTTON_2), NULL,
									0, 0, 100, 200,
									SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);

	if(hdwp) EndDeferWindowPos(hdwp);
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// SubClassing Callbacks
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
LRESULT CALLBACK subClassWnd_Main(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDC_DLG_BUTTON_1:
				{
					customDlg1();
					break;
				}

				case IDC_DLG_BUTTON_2:
				{
					customDlg2();
					break;
				}

				case IDC_DLG_BUTTON_3:
				{
					msgBoxCustom();
					break;
				}

				case IDC_DLG_BUTTON_4:
				{
					msgBoxHook();
					break;
				}

				case IDC_DLG_BUTTON_5:
				{
					MessageBox(NULL, NULL, L"Custom message box", NULL);
					break;
				}

				case IDC_DLG_BUTTON_6:
				{
					msgBoxUnHook();
					break;
				}

				case IDC_CHK_BOX_1:
				{
					BOOL checked = IsDlgButtonChecked(hWnd, IDC_CHK_BOX_1);

					if(checked)
						CheckDlgButton(hWnd, IDC_CHK_BOX_1, BST_UNCHECKED);
					else
						CheckDlgButton(hWnd, IDC_CHK_BOX_1, BST_CHECKED);

					wndAutosize(hWnd);

					break;
				}
			}
		}

		case WM_PAINT:
		{
			if(useDefaultWndProcForPaint) break;

			PAINTSTRUCT ps;
			HDC hDc = BeginPaint(hWnd, &ps);

			HBRUSH hBrush = (HBRUSH)GetClassLongPtr(hWnd, GCLP_HBRBACKGROUND);
			FillRect(hDc, &ps.rcPaint, hBrush);

			EndPaint(hWnd, &ps);

			return 0;
		}

		case WM_ERASEBKGND:
		{
			if(useDefaultWndProcForPaint) break;

			//return 0 means WM_PAINT handles the background
			return 0;
		}
		//*/
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//                                                   	   SubClassing Callbacks
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


HWND createWnd_Main(HINSTANCE hInstance)
{
	//--------------------------------------------------------------------------
	// INIT data
	//--------------------------------------------------------------------------
	wchar_t WINDOW_TITLE[]  = L"MAIN WINDOW";
	COLORREF windowColor = RGB(255, 0, 0);


	//--------------------------------------------------------------------------
	// Create and register the window class
	//--------------------------------------------------------------------------
	WNDCLASSEX wc = {0};
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.lpfnWndProc   = subClassWnd_Main;
	wc.hInstance     = hInstance;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.hbrBackground = CreateSolidBrush(windowColor);

	RegisterClassEx(&wc);


	//--------------------------------------------------------------------------
	// Create window
	//--------------------------------------------------------------------------
	HWND hWndMain = CreateWindowEx
	(
		NULL,                           // Window optional styles
		WINDOW_CLASS_NAME,              // Window class
		WINDOW_TITLE,    				// Window title
		WS_OVERLAPPEDWINDOW,            // Window style

		CW_USEDEFAULT,                  // Window X position
		CW_USEDEFAULT,                  // Window Y position
		CW_USEDEFAULT,                  // Window width
		CW_USEDEFAULT,                  // Window height

		NULL,       				  	// Window parent control
		NULL,       					// Window control ID
		hInstance,  					// Window parent instance handle
		NULL        					// Window additional data
	);

	if (hWndMain == NULL) return (HWND)0;


	//--------------------------------------------------------------------------
	// Create buttons
	//--------------------------------------------------------------------------
	HWND hwndButtonDlg1 = CreateWindowEx
	(
		NULL,                       // Button optional styles
		BUTTON_CLASS_NAME,  		// Button class
		L"CUSTOM DLG1",      		// Button text
		WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON,  // Button Styles
		100,         				// Button X position
		100,         				// Button Y position
		BUTTONS_WIDTH,        		// Button width
		BUTTONS_HEIGHT,        		// Button height
		hWndMain,       			// Button parent control
		(HMENU)IDC_DLG_BUTTON_1,    // Button control ID
		hInstance,                  // Button parent instance handle
		NULL                        // Button additional data
	);

	HWND hwndButtonDlg2 = CreateWindowEx
	(
		NULL,                       // Button optional styles
		BUTTON_CLASS_NAME,  		// Button class
		L"CUSTOM DLG2",      		// Button text
		WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON,  // Button Styles
		400,         				// Button X position
		100,         				// Button Y position
		BUTTONS_WIDTH,        		// Button width
		BUTTONS_HEIGHT,        		// Button height
		hWndMain,       			// Button parent control
		(HMENU)IDC_DLG_BUTTON_2,    // Button control ID
		hInstance,                  // Button parent instance handle
		NULL                        // Button additional data
	);

	HWND hwndButtonDlg3 = CreateWindowEx
	(
		NULL,                       // Button optional styles
		BUTTON_CLASS_NAME,  		// Button class
		L"MESSAGEBOX_HOOKED", 		// Button text
		WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON,  // Button Styles
		700,         				// Button X position
		100,         				// Button Y position
		BUTTONS_WIDTH,        		// Button width
		BUTTONS_HEIGHT,        		// Button height
		hWndMain,       			// Button parent control
		(HMENU)IDC_DLG_BUTTON_3,    // Button control ID
		hInstance,                  // Button parent instance handle
		NULL                        // Button additional data
	);

	HWND hwndButtonDlg4 = CreateWindowEx
	(
		NULL,                       // Button optional styles
		BUTTON_CLASS_NAME,  		// Button class
		L"HOOK",      				// Button text
		WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON,  // Button Styles
		100,         				// Button X position
		400,         				// Button Y position
		BUTTONS_WIDTH,        		// Button width
		BUTTONS_HEIGHT,        		// Button height
		hWndMain,       			// Button parent control
		(HMENU)IDC_DLG_BUTTON_4,    // Button control ID
		hInstance,                  // Button parent instance handle
		NULL                        // Button additional data
	);

	HWND hwndButtonDlg5 = CreateWindowEx
	(
		NULL,                       // Button optional styles
		BUTTON_CLASS_NAME,  		// Button class
		L"MESSAGE BOX",      		// Button text
		WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON,  // Button Styles
		400,         				// Button X position
		400,         				// Button Y position
		BUTTONS_WIDTH,        		// Button width
		BUTTONS_HEIGHT,        		// Button height
		hWndMain,       			// Button parent control
		(HMENU)IDC_DLG_BUTTON_5,    // Button control ID
		hInstance,                  // Button parent instance handle
		NULL                        // Button additional data
	);

	HWND hwndButtonDlg6 = CreateWindowEx
	(
		NULL,                       // Button optional styles
		BUTTON_CLASS_NAME,  		// Button class
		L"UNHOOK",      			// Button text
		WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON,  // Button Styles
		700,         				// Button X position
		400,         				// Button Y position
		BUTTONS_WIDTH,        		// Button width
		BUTTONS_HEIGHT,        		// Button height
		hWndMain,       			// Button parent control
		(HMENU)IDC_DLG_BUTTON_6,    // Button control ID
		hInstance,                  // Button parent instance handle
		NULL                        // Button additional data
	);


	//--------------------------------------------------------------------------
	// Create checkboxes
	//--------------------------------------------------------------------------
	HWND hwndChkBox1 = CreateWindowEx
	(
		NULL,                       // Button optional styles
		BUTTON_CLASS_NAME,  		// Button class
		L"AUTOSIZE",      			// Button text
		WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_CHECKBOX,  // Button Styles
		400,         				// Button X position
		300,         				// Button Y position
		185,        				// Button width
		35,        					// Button height
		hWndMain,       			// Button parent control
		(HMENU)IDC_CHK_BOX_1,    	// Button control ID
		hInstance,                  // Button parent instance handle
		NULL                        // Button additional data
	);


	return hWndMain;
}


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// MAIN
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    //--------------------------------------------------------------------------
	// Create main window
	//--------------------------------------------------------------------------
	HWND hWndMain = createWnd_Main(hInstance);
	if(!(int)hWndMain)
	{
		return 0;
	}


	//--------------------------------------------------------------------------
	// Show main window and run the message receive/dispatch loop
	//--------------------------------------------------------------------------
	ShowWindow(hWndMain, nCmdShow);

	MSG msg = {};
	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//                                                                      	MAIN
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//******************************************************************************

