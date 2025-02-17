﻿#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <WinUser.h>
#include <string>
#include <future>
#include <thread>
#include <conio.h>
#include <io.h>
#include <fcntl.h>

#include "GlobalFunction/GlobalFunction.h"
#include "./Features/Features.h"
#include "DynamicArray/DynamicArray.h"
#include "ImprovedDynamicMenu/DynamicMenu.h"
#include "LoadingScreen/LoadingScreen.h"

// Hook Variable
HHOOK _hook;

// This struct contains the data received by the hook callback. As you see in the callback function
// it contains the thing you will need: vkCode = virtual key code.
KBDLLHOOKSTRUCT kbdStruct;

LRESULT CALLBACK HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
	{
		return CallNextHookEx(_hook, nCode, wParam, lParam);
	}

	kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);

	if(EquationClass::Enabled)
	{
		switch(wParam)
		{
		// if keydown hook event (allows for holding and "spamming" key)
		case WM_KEYDOWN:
			// Pointer to UnicodeCharacter Array
			wchar_t UnicodeCharacter[3] = {};
			*UnicodeCharacter = *GlobalFunction::GetUnicodeCharacter(lParam, kbdStruct);

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
				EquationClass::InputStorageArray.Append(UnicodeCharacter[0]);
				return -1;

			case 8: // {BACKSPACE}
				EquationClass::RemovePreviousCharacter();
				return -1;

			case 47: // /
				wprintf(L"/");
				EquationClass::InputStorageArray.Append(UnicodeCharacter[0]);
				return -1;

			case 43: // +
				wprintf(L"+");
				EquationClass::InputStorageArray.Append(UnicodeCharacter[0]);
				return -1;

			case 45: // -
				wprintf(L"-");
				EquationClass::InputStorageArray.Append(UnicodeCharacter[0]);
				return -1;

			case 42: // *
				wprintf(L"*");
				EquationClass::InputStorageArray.Append(UnicodeCharacter[0]);
				return -1;

			case 46: // .
				wprintf(L".");
				EquationClass::InputStorageArray.Append(UnicodeCharacter[0]);
				return -1;

			case 13: // {ENTER}
			case 61: // =
				EquationClass::FinishFeature(kbdStruct);
				return -1;
			}
		}
	}
	else if (AutoSelectClass::Enabled)
	{
		switch (wParam)
		{
		// if keydown hook event (allows for holding and "spamming" key)
		case WM_KEYDOWN:
			// Pointer to UnicodeCharacter Array
			wchar_t UnicodeCharacter[3] = {};
			*UnicodeCharacter = *GlobalFunction::GetUnicodeCharacter(lParam, kbdStruct);

			switch (UnicodeCharacter[0])
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
				AutoSelectClass::InputStorageArray.Append(UnicodeCharacter[0]);
				return -1;

			case 8: // {BACKSPACE}
				AutoSelectClass::RemovePreviousCharacter();
				return -1;

			case 13: // {ENTER}
				AutoSelectClass::FinishFeature(kbdStruct);
				return -1;
			}

			break;
		}
	}
	else // if nothing is enabled, check for "global" keys
	{
		switch (wParam)
		{
		case WM_KEYUP:
			wchar_t UnicodeCharacter[3] = {};
			*UnicodeCharacter = *GlobalFunction::GetUnicodeCharacter(lParam, kbdStruct);

			switch (UnicodeCharacter[0])
			{
			case 'h':
			case 'H':
				GlobalFunction::clear_screen();
				std::wcout << LR"(COMMANDS:
ctrl + alt + shift + k - Value Calculation Mode
ctrl + alt + shift + L - Auto Select Mode
ctrl + alt + shift + S - Settings)" << std::endl;
				break;
			}

			break;
		}
	}

	// call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
	return CallNextHookEx(_hook, nCode, wParam, lParam);
}

#pragma region HookRelatedFunctions
void SetHook()
{
	// Set the hook and set it to use the callback function above
	// WH_KEYBOARD_LL means it will set a low level keyboard hook. More information about it at MSDN.
	// The last 2 parameters are NULL, 0 because the callback function is in the same thread and window as the
	// function that sets and releases the hook. If you create a hack you will not need the callback function 
	// in another place then your own code file anyway. Read more about it at MSDN.
	if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0)))
	{
		wprintf(L"Failed to install hook!");
	}
}

void ReleaseHook()
{
	UnhookWindowsHookEx(_hook);
}
#pragma endregion

int main()
{
	SetConsoleTitle(L"SWT By Noscka");

#pragma region Program Already Running Check
	CreateMutexA(0, FALSE, "Local\\$myprogram$"); // try to create a named mutex
	if (GetLastError() == ERROR_ALREADY_EXISTS) // Quit if mutex already exists (program already running)
	{
		printf("Program is already running\n");
		printf("Press any key to exit");
		_getch();
		return -1;
	}
#pragma endregion
	_setmode(_fileno(stdout), _O_U16TEXT);

	LoadingScreen::InitilizeFont();
	
#pragma region Loading Screen
	std::wstring splash = LR"(
                          ████████                ███████
                        ▄██▀    ▀██▄ ▄███████▄  ███▀   ▀████████▄
              ▄███████████▌      ██████     ▀█████       ███     ▀▀███▄
         ▄██▀▀         ██▌        ████       ████▌       ███           ▀▀███▄
       ██▀            ███         ███▌       ▐███        ▐██▄               ▀▀███▄
     ██▀       ███    ███         ███▌       ▐███        ▐████▀                  ▀██
    ██▌       ▀███▄▄▄▄███         ███        ▐███        ████▌                     ██
    ██▌               ██▌         ███        ▐███        ███▌          ████▄▄     ▄██
    ▀██▄              ██▌         ███        ▐███        ███          ███    ▀█████▀
      ▀██████████████▄███         ███        ████       ███          ███
        ██▀       ████▀██         ███        ▐██▌      ▐██▌          ██▌
       ███             ██▌        ██▌         ██       ███▌         ███
       ███             ▐██                            █████▄       ███
        ▀██▄▄       ▄▄▄████▄                         ███   ▀███▄▄███▀
           ▀▀▀███▀▀▀▀    ▀██▄         ▄██▄         ▄██▀
                           ▀███▄▄▄▄▄███▀████▄▄▄▄▄███▀
                               ▀▀▀▀▀        ▀▀▀▀▀)";

	LoadingScreen basic = LoadingScreen(LoadingScreen::Unknown, &SettingsClass::initialize , splash);
	basic.StartLoading();
#pragma endregion

	GlobalFunction::clear_screen();

	wprintf(LR"(
                          ████████                ███████
                        ▄██▀    ▀██▄ ▄███████▄  ███▀   ▀████████▄
              ▄███████████▌      ██████     ▀█████       ███     ▀▀███▄
         ▄██▀▀         ██▌        ████       ████▌       ███           ▀▀███▄
       ██▀            ███         ███▌       ▐███        ▐██▄               ▀▀███▄
     ██▀       ███    ███         ███▌       ▐███        ▐████▀                  ▀██
    ██▌       ▀███▄▄▄▄███         ███        ▐███        ████▌                     ██
    ██▌               ██▌         ███        ▐███        ███▌          ████▄▄     ▄██
    ▀██▄              ██▌         ███        ▐███        ███          ███    ▀█████▀
      ▀██████████████▄███         ███        ████       ███          ███
        ██▀       ████▀██         ███        ▐██▌      ▐██▌          ██▌
       ███             ██▌        ██▌         ██       ███▌         ███
       ███             ▐██                            █████▄       ███
        ▀██▄▄       ▄▄▄████▄                         ███   ▀███▄▄███▀
           ▀▀▀███▀▀▀▀    ▀██▄         ▄██▄         ▄██▀
                           ▀███▄▄▄▄▄███▀████▄▄▄▄▄███▀
                               ▀▀▀▀▀        ▀▀▀▀▀


▀█████████▄  ▄██   ▄        ███▄▄▄▄    ▄██████▄     ▄████████  ▄████████    ▄█   ▄█▄    ▄████████ 
  ███    ███ ███   ██▄      ███▀▀▀██▄ ███    ███   ███    ███ ███    ███   ███ ▄███▀   ███    ███ 
  ███    ███ ███▄▄▄███      ███   ███ ███    ███   ███    █▀  ███    █▀    ███▐██▀     ███    ███ 
 ▄███▄▄▄██▀  ▀▀▀▀▀▀███      ███   ███ ███    ███   ███        ███         ▄█████▀      ███    ███ 
▀▀███▀▀▀██▄  ▄██   ███      ███   ███ ███    ███ ▀███████████ ███        ▀▀█████▄    ▀███████████ 
  ███    ██▄ ███   ███      ███   ███ ███    ███          ███ ███    █▄    ███▐██▄     ███    ███ 
  ███    ███ ███   ███      ███   ███ ███    ███    ▄█    ███ ███    ███   ███ ▀███▄   ███    ███ 
▄█████████▀   ▀█████▀        ▀█   █▀   ▀██████▀   ▄████████▀  ████████▀    ███   ▀█▀   ███    █▀  
                                                                           ▀                      
Begin by pressing h)");

	// Set the hook
	SetHook();

	enum
	{
		EquationEnableHK = 1, // Ctrl+Shift+alt+k - Enable Input
		AutoSelectHK = 2, // Ctrl+Shift+alt+L - Run Auto Select Function
		SettingsHK = 3, // Ctrl+Shift+alt+S - Opens Settings menu
	};
	RegisterHotKey(0, EquationEnableHK, MOD_SHIFT | MOD_CONTROL | MOD_ALT, 'K'); // register 1 key as hotkey
	RegisterHotKey(0, AutoSelectHK, MOD_SHIFT | MOD_CONTROL | MOD_ALT, 'L'); // register 2 key as hotkey
	RegisterHotKey(0, SettingsHK, MOD_SHIFT | MOD_CONTROL | MOD_ALT, 'S'); // register 3 key as hotkey
	MSG msg;
	while(GetMessage(&msg, 0, 0, 0))
	{
		PeekMessage(&msg, 0, 0, 0, 0x0001);
		switch(msg.message)
		{
		case WM_HOTKEY:
			switch (msg.wParam)
			{
			case EquationEnableHK:
				if (AutoSelectClass::Enabled) // Check if Auto Select is enabled (will need a function for all)
					break;

				EquationClass::Enabled = !EquationClass::Enabled;

				GlobalFunction::clear_screen();

				if (EquationClass::Enabled)
				{
					wprintf(L"Enabled Equation Mode\n");
				}
				else
				{
					wprintf(L"Stopped Equation Mode\n");
				}
				break;

			case AutoSelectHK:
				if (EquationClass::Enabled) // Check if Equation is enabled (will need a function for all)
					break;

				AutoSelectClass::Enabled = !AutoSelectClass::Enabled;

				GlobalFunction::clear_screen();

				if (AutoSelectClass::Enabled)
				{
					wprintf(L"Input amount to go right by: ");
				}
				else
				{
					wprintf(L"Stopped Auto Select Mode\n");
				}
				break;

			case SettingsHK:
			{
				ReleaseHook();
				GlobalFunction::clear_screen();

				SettingsClass::SettingsMenu.StartMenu();
				
				GlobalFunction::clear_screen();
				SetHook();
				break;
			}
			}
		}
	}

	LoadingScreen::TerminateFont();
	return 0;
}