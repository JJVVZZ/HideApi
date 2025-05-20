#include<iostream>
#include<math.h>
#include<iomanip>
#include<string>
#include<ctime>
#include<windows.h>
#include<new>
#include<fstream>
#include<ostream>
#include<memory>
#include<exception>
#include<vector>
#include<list>
#include<forward_list>
#include<array>
#include<deque>
#include<stack>
#include<queue>
#include<set>
#include<map>
#include<thread>
#include<chrono> 
#include<functional>
#include<algorithm>
#include<numeric>
#include<mutex>
#include<regex>
#include<unordered_map>
#include<unordered_set>
#include<cstring>
#include <cstdlib>
#include <typeinfo>
#include<future>
#include <type_traits>
#include<ranges>


using namespace std;
#define ID_HIDE 1
#define ID_SHOW 2
#define ID_EXIT 3

vector<HWND> hiddenWindows;

void RestoreWindow(HWND hwnd)
{
	if (!IsWindow(hwnd)) return;

	
	if (IsIconic(hwnd)) {
		ShowWindow(hwnd, SW_RESTORE);
	}
	else {
		ShowWindow(hwnd, SW_SHOW);    
	}

	SetForegroundWindow(hwnd);       
	SetActiveWindow(hwnd);         
}


void FocusNextWindow(HWND hwnd)
{
	HWND hNext = GetWindow(hwnd, GW_HWNDNEXT);

	while (hNext && (!IsWindowVisible(hNext) || hNext == hwnd))
	{
		hNext = GetWindow(hNext, GW_HWNDNEXT);
	}

	if (hNext)
	{
		DWORD currThread = GetCurrentThreadId();
		DWORD nextThread = GetWindowThreadProcessId(hNext, NULL);

		if (currThread != nextThread)
		{
			AttachThreadInput(currThread, nextThread, TRUE);
		}

		SetForegroundWindow(hNext);
		SetFocus(hNext);

		if (currThread != nextThread)
		{
			AttachThreadInput(currThread, nextThread, FALSE);
		}
	}


}

void AddStartup()
{
	HKEY hKey;
	LPCWSTR keyPatch = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

	if (RegOpenKeyEx(HKEY_CURRENT_USER, keyPatch, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
	{
		WCHAR path[MAX_PATH];
		GetModuleFileName(NULL, path, MAX_PATH);

		RegSetValueEx(hKey, L"HideApi", 0, REG_SZ, (LPBYTE)path, (wcslen(path) + 1) * sizeof(WCHAR));

		RegCloseKey(hKey);
	}
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{


	AddStartup();

	RegisterHotKey(NULL, ID_HIDE, MOD_ALT, 'H');
	RegisterHotKey(NULL, ID_SHOW, MOD_ALT, 'S');
	RegisterHotKey(NULL, ID_EXIT, MOD_ALT, 'E');

	MSG msg;
	HWND hwnd = NULL;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_HOTKEY)
		{
			switch (msg.wParam)
			{
			case ID_HIDE:
				hwnd = GetForegroundWindow();
				if (hwnd && IsWindowVisible(hwnd))
				{
					hiddenWindows.push_back(hwnd);
					ShowWindow(hwnd, SW_HIDE);
					FocusNextWindow(hwnd);
				}
				break;
			case ID_SHOW:
				for (auto z : hiddenWindows)
				{
					RestoreWindow(z);
				}
				hiddenWindows.clear();
				break;
			case ID_EXIT:

				return 0;
			}
		}
	}

	return 0;
}

