// SpacebarFilter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <stdio.h>
#include <iostream>

static LARGE_INTEGER StartingTime = {0};
static LARGE_INTEGER Frequency    = {0};

#define FILTER_WINDOW (1000 * 60)

bool isGood()
{
    LARGE_INTEGER EndingTime;
    LARGE_INTEGER ElapsedMicroseconds;

    QueryPerformanceCounter(&EndingTime);

    ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
    ElapsedMicroseconds.QuadPart *= 1000000;
    ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

    StartingTime = EndingTime;

    if (ElapsedMicroseconds.QuadPart > FILTER_WINDOW)
    {
        //std::cout << "good  " << ElapsedMicroseconds.QuadPart/1000 << std::endl;
        return true;
    }
    else
    {
        //std::cout << "bad   " << ElapsedMicroseconds.QuadPart / 1000 << std::endl;
        return false ;
    }
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    static bool goodDown;

    if (nCode == HC_ACTION)
    {
        switch (wParam)
        {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
            PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
            if (p->vkCode == VK_SPACE)  // is spacebar
            {
                if ((wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN)) // Keydown
                {
                    goodDown = isGood();
                    if (!goodDown)
                    { 
                        return 1;
                    }
                }
                else if ((wParam == WM_KEYUP) || (wParam == WM_SYSKEYUP)) // Keyup
                {
                    if (!goodDown)
                    { 
                        goodDown = true;
                        return 1;
                    }
                }
            }
            break;
        }

    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main(int argc, char* argv[])
{
    std::cout << "Spacebar filter for HP Envy 32 AIO keybaords v1.0" << std::endl;
    std::cout << "Filter window is " << (FILTER_WINDOW/1000) << "mS" << std::endl;

    QueryPerformanceFrequency(&Frequency);

    // Install the low-level keyboard & mouse hooks
    HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);

    // Keep this app running until we're told to stop
    MSG msg;
    while (!GetMessage(&msg, NULL, NULL, NULL)) 
    {
        //this while loop keeps the hook
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hhkLowLevelKybd);

    return(0);
}

