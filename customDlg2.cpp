////////////////////////////////////////////////////////////////////////////////
// SOURCES
// https://stackoverflow.com/questions/18745447/how-can-i-change-the-background-color-of-a-button-winapi-c/18773311
////////////////////////////////////////////////////////////////////////////////



//******************************************************************************
// INCLUDES
//******************************************************************************
#include <windows.h>
#include <commctrl.h>

#include <vcl.h>
#pragma hdrstop

#include "utils.h"

#include "customDlg2.h"
//******************************************************************************


//******************************************************************************
// DEFINES
//******************************************************************************
#define ClassName  L"CUSTOM_DLG_2"
//******************************************************************************


//******************************************************************************
// GLOBAL VARS
//******************************************************************************
HINSTANCE hInst;
HBITMAP hBitmap = NULL;
//******************************************************************************



//******************************************************************************
// METHODS
//******************************************************************************

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// SubClassing Callbacks
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
	case WM_CREATE:
        {
            // Owner draw button

			CreateWindow(L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |
                BS_OWNERDRAW, 10, 10, 60, 30, hWnd,
                (HMENU)10001, hInst, NULL);

            // Custom draw button

            CreateWindow(L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 80,
                10, 60, 30, hWnd, (HMENU)10002, hInst, NULL);

            // Bitmap button

            HWND hBitmapButton = CreateWindow(L"BUTTON", L"", WS_CHILD | WS_VISIBLE
                | BS_PUSHBUTTON | BS_BITMAP,
                150, 10, 60, 30, hWnd,
				(HMENU)10003, hInst, NULL);

            HDC hDC = GetDC(hWnd);

            HDC hMemDC = CreateCompatibleDC(hDC);

            hBitmap = CreateCompatibleBitmap(hDC, 55, 25);

            SelectObject(hMemDC, hBitmap);

            SetDCBrushColor(hMemDC, RGB(0, 0, 255));

            RECT r = {0};
            r.left = 0;
            r.right = 55;
            r.top = 0;
            r.bottom = 25;

            FillRect(hMemDC, &r, (HBRUSH)GetStockObject(DC_BRUSH));

            DeleteDC(hMemDC);
            ReleaseDC(hWnd, hDC);

            SendMessage(hBitmapButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP,
                (LPARAM)hBitmap);

            return 0;
        }

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 10001:
            MessageBox(hWnd, L"Owner draw button clicked", L"Message", NULL);
            return 0;
        case 10002:
            MessageBox(hWnd, L"Custom draw button clicked", L"Message", NULL);
            return 0;
        case 10003:
            MessageBox(hWnd, L"Bitmap button clicked", L"Message", NULL);
            return 0;
        }
        break;

    // Owner draw button

    case WM_DRAWITEM:
        if (wParam == 10001)
        {
            LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT)lParam;

            SetDCBrushColor(lpDIS -> hDC, RGB(255, 0, 0));

            SelectObject(lpDIS -> hDC, GetStockObject(DC_BRUSH));

            RoundRect(lpDIS -> hDC, lpDIS -> rcItem.left, lpDIS -> rcItem.top,
                lpDIS -> rcItem.right, lpDIS -> rcItem.bottom, 5, 5);

            return TRUE;
        }
        break;

    // Custom draw button

    case WM_NOTIFY:
        switch (((LPNMHDR)lParam) -> code)
        {
        case NM_CUSTOMDRAW:
            if (((LPNMHDR)lParam) -> idFrom == 10002)
            {
                LPNMCUSTOMDRAW lpnmCD = (LPNMCUSTOMDRAW)lParam;

                switch (lpnmCD -> dwDrawStage)
                {
                case CDDS_PREPAINT:

                    SetDCBrushColor(lpnmCD -> hdc, RGB(0, 255, 0));
                    SetDCPenColor(lpnmCD -> hdc, RGB(0, 255, 0));
                    SelectObject(lpnmCD -> hdc, GetStockObject(DC_BRUSH));
                    SelectObject(lpnmCD -> hdc, GetStockObject(DC_PEN));

                    RoundRect(lpnmCD -> hdc, lpnmCD -> rc.left + 3,
                        lpnmCD -> rc.top + 3,
                        lpnmCD -> rc.right - 3,
                        lpnmCD -> rc.bottom - 3, 5, 5);

                    return TRUE;
                }
            }
            break;
        }
        break;

    case WM_DESTROY:
        if (hBitmap != NULL)
			DeleteObject((HBITMAP)hBitmap);
		return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//                                                   	   SubClassing Callbacks
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// Helpers
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
bool RegisterWndClass()
{
	WNDCLASS wndClass = {0};
	if(!GetClassInfo(hInst, ClassName, &wndClass))
	{
		ZeroMemory(&wndClass, sizeof(wndClass));

		wndClass.style = CS_DBLCLKS;
		wndClass.lpfnWndProc = MainWndProc;
		wndClass.hInstance = hInst;
		wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = ClassName;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;

		if(!RegisterClass(&wndClass))
			return false;

		return true;
	}

	return true;
 }

BOOL CreateMainWnd(int nCmdShow)
{
	HWND hWnd = CreateWindow(ClassName, L"Buttons sample",
         WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		 GetSystemMetrics(SM_CXSCREEN) / 2 - 115,
		 GetSystemMetrics(SM_CYSCREEN) / 2 - 50,
		 230, 100, NULL, NULL, hInst, NULL);

    if (!hWnd)
		return FALSE;

	ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

	return TRUE;
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//                                                          			 Helpers
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// INTERFACE METHODS
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void customDlg2()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	INITCOMMONCONTROLSEX icex = {0};
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_LISTVIEW_CLASSES | ICC_USEREX_CLASSES | ICC_BAR_CLASSES |
				  ICC_COOL_CLASSES | ICC_TAB_CLASSES | ICC_WIN95_CLASSES |
                  ICC_PROGRESS_CLASS | ICC_PAGESCROLLER_CLASS;

	InitCommonControlsEx(&icex);

    MSG msg;

	hInst = hInstance;

	if(!RegisterWndClass())
	{
		String errMsg = "RegisterClassEx failed with error message: " + getErrorMessage(GetLastError());
		MessageBox(NULL, errMsg.c_str(), L"Error", MB_ICONEXCLAMATION | MB_OK);
		return;
	}

	if(!CreateMainWnd(SW_SHOWNORMAL))
	{
		String errMsg = "CreateMainWnd failed with error message: " + getErrorMessage(GetLastError());
		MessageBox(NULL, errMsg.c_str(), L"Error", MB_ICONEXCLAMATION | MB_OK);
		return;
	}
};
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//                                                             INTERFACE METHODS
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//******************************************************************************
