//******************************************************************************
// INCLUDES
//******************************************************************************
#include <windows.h>
#include <commctrl.h>

#include <vcl.h>
#pragma hdrstop

#include "utils.h"

#include "msgBoxHooked.h"
//******************************************************************************


//******************************************************************************
// DEFINES
//******************************************************************************
#define staticText L"1234567890"

#define IDC_DLG_BUTTON_OK 101
//******************************************************************************


//******************************************************************************
// GLOBAL VARS
//******************************************************************************
HFONT font;
HBRUSH hBrush;

HWND hWndMsgBox;
HWINEVENTHOOK hookMsgBox;
//******************************************************************************


//******************************************************************************
// METHODS
//******************************************************************************

void CALLBACK timerEventProc(HWND hWnd, UINT uTimerMsg, UINT uTimerID, DWORD dwTime)
{
	static int timercount = 99;

	TCHAR countdown[3] = L"";
	wsprintf(countdown, L"%d", timercount);
	SetWindowText(hWnd, countdown);

	--timercount;
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// SubClassing Callbacks
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
LRESULT CALLBACK wndSubClass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch(uMsg)
	{
		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDC_DLG_BUTTON_OK:
				{
					SendMessage(hWndMsgBox, WM_CLOSE, 0, 0);
					break;
				}
			}
		}

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			SetBkMode(hdc, TRANSPARENT);

			FillRect(hdc, &ps.rcPaint, hBrush);

			EndPaint(hWnd, &ps);

			return 0;
		}

		case WM_ERASEBKGND:
			//return 0 means WM_PAINT handles the background
			return 0;

		case WM_CLOSE:
		{
			RemoveWindowSubclass(hWnd, &wndSubClass, 1);
			DestroyWindow(hWnd);
			break;
		}

		case WM_DESTROY:
		{
			return 0;
		}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK staticSubClass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch(uMsg)
	{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			SetBkMode(hdc, TRANSPARENT);
			SelectObject(hdc,font);

			RECT rc;
			GetClientRect(hWnd, &rc);
			DrawText(hdc, staticText, _tcslen(staticText), &rc, DT_VCENTER);

			EndPaint(hWnd, &ps);

			return 0;
		}

		case WM_ERASEBKGND:
			//return 0 means WM_PAINT handles the background
			return 0;
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK btnSubClass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch(uMsg)
	{
		case WM_MOUSEMOVE:
			SetCursor(LoadCursor(NULL, IDC_HAND));
			break;

		case WM_LBUTTONDOWN:
			// better handle in parent
			// SendMessage(hWndMsgBox, WM_CLOSE, 0, 0);
			break;


		case WM_CLOSE:
			RemoveWindowSubclass(hWnd, &btnSubClass, 1);
			return 0;
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//                                                   	   SubClassing Callbacks
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// Hook Callbacks
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void CALLBACK wndHook(	HWINEVENTHOOK hook,
						DWORD event,
						HWND hWnd,
						LONG idObject,
						LONG idChild,
						DWORD dwEventThread,
						DWORD dwmsEventTime)
{
	//--------------------------------------------------------------------------
	// STYLES
	//--------------------------------------------------------------------------
	// Size
	int msgBoxWidth = 500;
	int msgBoxHeight = 300;

	// Font to be used
	font = CreateFont(35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("Times New Roman"));

	// Background color
	hBrush = CreateSolidBrush(RGB(0, 0, 255));


	//--------------------------------------------------------------------------
	// INIT - general rework of message dialog
	//--------------------------------------------------------------------------
	hWndMsgBox = hWnd;

    RECT rc;
	GetClientRect(hWnd, &rc);
	HWND staticBoxHide = CreateWindowEx(NULL,
										L"static",
										L"",
										WS_CHILD | WS_VISIBLE,
										0,
										0,
										msgBoxWidth,
										msgBoxHeight,
										hWnd,
										NULL,
										NULL,
										NULL);

	SetWindowSubclass(staticBoxHide, &wndSubClass, 1, 0);

	HWND okBtn = GetDlgItem(hWnd, IDCANCEL);
	ShowWindow(okBtn, SW_HIDE);

	SetWindowPos(hWnd,
				 HWND_TOP,
				 0, 0,
				 msgBoxWidth,
				 msgBoxHeight,
				 SWP_NOMOVE);


	//--------------------------------------------------------------------------
	// LABELS
	//--------------------------------------------------------------------------
	HWND staticBox = CreateWindowEx(NULL,
									L"static",
									staticText,
									WS_CHILD | WS_VISIBLE,
									5,
									5,
									200,
									35,
									staticBoxHide,
									NULL,
									NULL,
									NULL);

	SendMessage(staticBox, WM_SETFONT, (WPARAM)font, 0);
	SetWindowSubclass(staticBox, &staticSubClass, 1, 0);


	//--------------------------------------------------------------------------
	// BUTTONS
	//--------------------------------------------------------------------------
	HWND hWndButtonOK = CreateWindowEx( NULL,
										L"BUTTON",
										L"ZATVORI",
										WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON,
										5,
										60,
										200,
										200,
										staticBoxHide,
										(HMENU) IDC_DLG_BUTTON_OK,
										(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
										NULL);

	SendMessage(hWndButtonOK, WM_SETFONT, (WPARAM)font, 0);
	SetWindowSubclass(hWndButtonOK, &btnSubClass, 1, 0);


	//--------------------------------------------------------------------------
	// MISC
	//--------------------------------------------------------------------------
	//SetTimer(staticBox, 1, 1000, (TIMERPROC)timerEventProc);

	UnhookWinEvent(hook);
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//                                                   	   		  Hook Callbacks
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// INTERFACE METHODS
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void msgBoxHook()
{
	hookMsgBox = SetWinEventHook(	EVENT_OBJECT_CREATE,
									EVENT_OBJECT_CREATE,
									NULL,
									wndHook,
									0,
									0,
									WINEVENT_OUTOFCONTEXT);
}

void msgBoxUnHook()
{
	UnhookWinEvent(hookMsgBox);
}


void msgBoxCustom()
{
	msgBoxHook();

	MessageBox(NULL, NULL, L"Custom message box", NULL);
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//                                                             INTERFACE METHODS
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//******************************************************************************

