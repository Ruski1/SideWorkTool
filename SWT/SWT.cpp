#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <WinUser.h>

#include "./TinyExpr/tinyexpr.h"

// If Program is enabled
bool Enabled = true;

// Equation Char Array
char Equation[20] = {};
int EquationArrayIndex = 0; // Equation Array Index

// Hook Variable
HHOOK _hook;

// This struct contains the data received by the hook callback. As you see in the callback function
// it contains the thing you will need: vkCode = virtual key code.
KBDLLHOOKSTRUCT kbdStruct;

COORD GetConsoleCursorPosition(HANDLE hConsoleOutput)
{
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	if(GetConsoleScreenBufferInfo(hConsoleOutput, &cbsi))
	{
		return cbsi.dwCursorPosition;
	}
	else
	{
		// The function failed. Call GetLastError() for details.
		return {0, 0};
	}
}

bool AddToEquation(char Character)
{
	try
	{
		Equation[EquationArrayIndex] = Character;
		EquationArrayIndex++;
	}
	catch(...)
	{
		return false;
	}
	return true;
}


LRESULT CALLBACK HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(Enabled)
	{
		if(nCode >= 0)
		{
			switch(wParam)
			{
			case WM_KEYDOWN:
				// lParam is the pointer to the struct containing the data needed, so cast and assign it to kdbStruct.
				kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);

				// KeyBoardState BYTE array
				BYTE KeyboardState[256] = {};

				// UnicodeCharacter output from ToUnicodeEx
				wchar_t UnicodeCharacter[3] = {};

				// Get keystate from Shift and alt when getting keyboardstate
				GetKeyState(VK_SHIFT);
				GetKeyState(VK_MENU);
				GetKeyboardState(KeyboardState);

				// Get the key hit while taking into account the modifiers (shift+/ -> ?)
				ToUnicodeEx((UINT)kbdStruct.vkCode, (UINT)kbdStruct.scanCode, KeyboardState, UnicodeCharacter, sizeof(UnicodeCharacter) / sizeof(*UnicodeCharacter) - 1, (UINT)kbdStruct.flags, GetKeyboardLayout(0));

				// Coord for backspace cursor position editing
				COORD NewCoord;
				
				switch(UnicodeCharacter[0])
				{
				case 48: // 0
				case 49: // 1
				case 50: // 2
				case 51: // 3
				case 52: // 4
				case 53: // 5
				case 54: // 6
				case 55: // 7
				case 56: // 8
				case 57: // 9
					std::wcout << UnicodeCharacter;
					AddToEquation(UnicodeCharacter[0]);
					return -1;

				case 8: // {BACKSPACE}
					if(EquationArrayIndex > 0)
					{
						NewCoord = {GetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE)).X, GetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE)).Y};
						NewCoord.X -= 1;
						SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), NewCoord);
						printf(" ");
						SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), NewCoord);
						EquationArrayIndex--;
					}

					Equation[EquationArrayIndex] = NULL;
					return -1;

				case 13: // {ENTER}
					std::cout << Equation << " = " << te_interp(Equation, 0) << "\n";
					return -1;

				case 47: // /
					printf("/");
					AddToEquation(UnicodeCharacter[0]);
					return -1;

				case 43: // +
					printf("+");
					AddToEquation(UnicodeCharacter[0]);
					return -1;

				case 45: // -
					printf("-");
					AddToEquation(UnicodeCharacter[0]);
					return -1;

				case 42: // *
					printf("*");
					AddToEquation(UnicodeCharacter[0]);
					return -1;

				case 61: // =
					std::cout << Equation << " = " << te_interp(Equation, 0);
					return -1;

				default:
					//std::wcout << "ToUnicodeEx: " << UnicodeCharacter << '\n';
					//std::wcout << "ToUnicodeEx INT: " << (int)UnicodeCharacter[0] << '\n';
					break;
				}
			}
		}
	}

	// call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
	return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void SetHook()
{
	// Set the hook and set it to use the callback function above
	// WH_KEYBOARD_LL means it will set a low level keyboard hook. More information about it at MSDN.
	// The last 2 parameters are NULL, 0 because the callback function is in the same thread and window as the
	// function that sets and releases the hook. If you create a hack you will not need the callback function 
	// in another place then your own code file anyway. Read more about it at MSDN.
	if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0)))
	{
		printf("Failed to install hook!");
	}
}

void ReleaseHook()
{
	UnhookWindowsHookEx(_hook);
}

int main()
{
	// Set the hook
	SetHook();
	
	enum
	{ 
		KEYID = 1 // Ctrl+Shift+alt+k key ID
	};
	RegisterHotKey(0, KEYID, MOD_SHIFT | MOD_CONTROL | MOD_ALT, 'K'); // register 1 key as hotkey
	MSG msg;
	while(GetMessage(&msg, 0, 0, 0))
	{
		PeekMessage(&msg, 0, 0, 0, 0x0001);
		switch(msg.message)
		{
		case WM_HOTKEY:
			if(msg.wParam == KEYID)
			{
				Enabled = !Enabled;

				if(Enabled)
				{
					printf("Enabled\n");
				}
				else
				{
					printf("Disabled\n");
				}
			}
		}
	}
	return 0;
}
