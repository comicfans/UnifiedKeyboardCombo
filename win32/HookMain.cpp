/*
	This project serves as a simple demonstration for the article "Combining Raw Input and keyboard Hook to selectively block input from multiple keyboards",
	which you should be able to find in this project folder (HookingRawInput.html), or on the CodeProject website (http://www.codeproject.com/).
	The project is based on the idea shown to me by Petr Medek (http://www.hidmacros.eu/), and is published with his permission, huge thanks to Petr!
	The source code is licensed under The Code Project Open License (CPOL), feel free to adapt it.
	Vít Blecha (sethest@gmail.com), 2014
*/

#include <deque>
#include <cstdio>
#include "HookMain.h"
#include "HookDll.h"


#include "WatchConfigFileChange.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING]=L"abccc";			// the main window class name

// HWND of main executable
HWND mainHwnd;
// Windows message for communication between main executable and DLL module
UINT const WM_HOOK = WM_APP + 1;
// How long should Hook processing wait for the matching Raw Input message (ms)
DWORD maxWaitingTime = 100;
// Device name of my numeric keyboard
const WCHAR * const numericKeyboardDeviceName = L"\\\\?\\HID#VID_04D9&PID_1203&MI_00#8&13a87ad5&0&0000#{884b96c3-56ef-11d1-bc8c-00a0c91405dd}";
// Buffer for the decisions whether to block the input with Hook
std::deque<DecisionRecord> decisionBuffer;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);

 	// TODO: Place code here.
	MSG msg;

	// Initialize global strings
	//LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	//LoadString(hInstance, IDC_HOOKINGRAWINPUTDEMO, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

    
	//hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HOOKINGRAWINPUTDEMO));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	return (int) msg.wParam;
}

    
WatchThreadParam param;

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;//LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HOOKINGRAWINPUTDEMO));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//MAKEINTRESOURCE(IDC_HOOKINGRAWINPUTDEMO);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow (szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	// Save the HWND
	mainHwnd = hWnd;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Register for receiving Raw Input for keyboards
	RAWINPUTDEVICE rawInputDevice[1];
	rawInputDevice[0].usUsagePage = 1;
	rawInputDevice[0].usUsage = 6;
	rawInputDevice[0].dwFlags = RIDEV_INPUTSINK;
	rawInputDevice[0].hwndTarget = hWnd;
	RegisterRawInputDevices (rawInputDevice, 1, sizeof (rawInputDevice[0]));

	// Set up the keyboard Hook
	InstallHook (hWnd);

    param.quitEvent=CreateEvent(NULL,FALSE,FALSE,NULL);

    CreateThread(nullptr,0,watchConfigChangeThread,&param,0,nullptr);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	

	switch (message)
	{
	// Raw Input Message
	case WM_INPUT:
	{
		UINT bufferSize;

		// Prepare buffer for the data
		GetRawInputData ((HRAWINPUT)lParam, RID_INPUT, NULL, &bufferSize, sizeof (RAWINPUTHEADER));
		LPBYTE dataBuffer = new BYTE[bufferSize];
		// Load data into the buffer
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, dataBuffer, &bufferSize, sizeof (RAWINPUTHEADER));

		RAWINPUT* raw = (RAWINPUT*)dataBuffer;

		// Get the virtual key code of the key and report it
		USHORT virtualKeyCode = raw->data.keyboard.VKey;
		USHORT keyPressed = raw->data.keyboard.Flags & RI_KEY_BREAK ? 0 : 1;
		WCHAR text[128];
		swprintf (text, 128, L"Raw Input: %X (%d)\n", virtualKeyCode, keyPressed);
		OutputDebugString (text);

		// Prepare string buffer for the device name
		GetRawInputDeviceInfo (raw->header.hDevice, RIDI_DEVICENAME, NULL, &bufferSize);
		WCHAR* stringBuffer = new WCHAR[bufferSize];

		// Load the device name into the buffer
		GetRawInputDeviceInfo (raw->header.hDevice, RIDI_DEVICENAME, stringBuffer, &bufferSize);

		// Check whether the key struck was a "7" on a numeric keyboard, and remember the decision whether to block the input
		if (virtualKeyCode == 0x67 && wcscmp (stringBuffer, numericKeyboardDeviceName) == 0)
		{
			decisionBuffer.push_back (DecisionRecord (virtualKeyCode, TRUE));
		}
		else
		{
			decisionBuffer.push_back (DecisionRecord (virtualKeyCode, FALSE));
		}

		delete[] stringBuffer;
		delete[] dataBuffer; 
		return 0;
	}

	// Message from Hooking DLL
	case WM_HOOK:
	{
		USHORT virtualKeyCode = (USHORT)wParam;
		USHORT keyPressed = lParam & 0x80000000 ? 0 : 1;
		WCHAR text[128];
		swprintf_s (text, 128, L"Hook: %X (%d)\n", virtualKeyCode, keyPressed);
		OutputDebugString (text);

		// Check the buffer if this Hook message is supposed to be blocked; return 1 if it is
		BOOL blockThisHook = FALSE;
		BOOL recordFound = FALSE;
		int index = 1;
		if (!decisionBuffer.empty ())
		{
			// Search the buffer for the matching record
			std::deque<DecisionRecord>::iterator iterator = decisionBuffer.begin ();
			while (iterator != decisionBuffer.end ())
			{
				if (iterator->virtualKeyCode == virtualKeyCode)
				{
					blockThisHook = iterator->decision;
					recordFound = TRUE;
					// Remove this and all preceding messages from the buffer
					for (int i = 0; i < index; ++i)
					{
						decisionBuffer.pop_front ();
					}
					// Stop looking
					break;
				}
				++iterator;
				++index;
			}
		}

		// Wait for the matching Raw Input message if the decision buffer was empty or the matching record wasn't there
		DWORD currentTime, startTime;
		startTime = GetTickCount ();
		while (!recordFound)
		{
			MSG rawMessage;
			while (!PeekMessage (&rawMessage, mainHwnd, WM_INPUT, WM_INPUT, PM_REMOVE))
			{
				// Test for the maxWaitingTime
				currentTime = GetTickCount ();
				// If current time is less than start, the time rolled over to 0
				if ((currentTime < startTime ? ULONG_MAX - startTime + currentTime : currentTime - startTime) > maxWaitingTime)
				{
					// Ignore the Hook message, if it exceeded the limit
					WCHAR text[128];
					swprintf_s (text, 128, L"Hook TIMED OUT: %X (%d)\n", virtualKeyCode, keyPressed);
					OutputDebugString (text);
					return 0;
				}
			}

			// The Raw Input message has arrived; decide whether to block the input
			UINT bufferSize;

			// Prepare buffer for the data
			GetRawInputData ((HRAWINPUT)rawMessage.lParam, RID_INPUT, NULL, &bufferSize, sizeof (RAWINPUTHEADER));
			LPBYTE dataBuffer = new BYTE[bufferSize];
			// Load data into the buffer
			GetRawInputData((HRAWINPUT)rawMessage.lParam, RID_INPUT, dataBuffer, &bufferSize, sizeof (RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)dataBuffer;

			// Get the virtual key code of the key and report it
			USHORT rawVirtualKeyCode = raw->data.keyboard.VKey;
			USHORT rawKeyPressed = raw->data.keyboard.Flags & RI_KEY_BREAK ? 0 : 1;
			WCHAR text[128];
			swprintf_s (text, 128, L"Raw Input WAITING: %X (%d)\n", rawVirtualKeyCode, rawKeyPressed);
			OutputDebugString (text);

			// Prepare string buffer for the device name
			GetRawInputDeviceInfo (raw->header.hDevice, RIDI_DEVICENAME, NULL, &bufferSize);
			WCHAR* stringBuffer = new WCHAR[bufferSize];

			// Load the device name into the buffer
			GetRawInputDeviceInfo (raw->header.hDevice, RIDI_DEVICENAME, stringBuffer, &bufferSize);

			// If the Raw Input message doesn't match the Hook, push it into the buffer and continue waiting
			if (virtualKeyCode != rawVirtualKeyCode)
			{
				// Check whether the key struck was a "7" on a numeric keyboard, and decide whether to block the input
				if (rawVirtualKeyCode == 0x67 && wcscmp (stringBuffer, numericKeyboardDeviceName) == 0)
				{
					decisionBuffer.push_back (DecisionRecord (rawVirtualKeyCode, TRUE));
				}
				else
				{
					decisionBuffer.push_back (DecisionRecord (rawVirtualKeyCode, FALSE));
				}
			}
			else
			{
				// This is correct Raw Input message
				recordFound = TRUE;
				// Check whether the key struck was a "7" on a numeric keyboard, and decide whether to block the input
				if (rawVirtualKeyCode == 0x67 && wcscmp (stringBuffer, numericKeyboardDeviceName) == 0)
				{
					blockThisHook = TRUE;
				}
				else
				{
					blockThisHook= FALSE;
				}
			}
			delete[] stringBuffer;
			delete[] dataBuffer;
		}
		// Apply the decision
		if (blockThisHook)
		{
			swprintf_s (text, 128, L"Keyboard event: %X (%d) is being blocked!\n", virtualKeyCode, keyPressed);
			OutputDebugString (text);
			return 1;
		}
		return 0;
	}
	case WM_PAINT:{
                  
    PAINTSTRUCT ps;
    HDC hdc;
    hdc = BeginPaint(hWnd, &ps);
		TextOut (hdc, 10, 10, L"This application is for demonstration purposes only and doesn't have any practical use.", 87);
		TextOut (hdc, 10, 40, L"Check Help/About for additional information.", 44);
		EndPaint(hWnd, &ps);
		break;}
	case WM_DESTROY:
		UninstallHook ();
        SetEvent(param.quitEvent);

		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
