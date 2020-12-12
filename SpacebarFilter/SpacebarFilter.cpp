// SpacebarFilter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <stdio.h>
#include <iostream>

static LARGE_INTEGER StartingTime = {0};
static LARGE_INTEGER Frequency    = {0};

bool isGood()
{
    LARGE_INTEGER EndingTime;
    LARGE_INTEGER ElapsedMicroseconds;

    QueryPerformanceCounter(&EndingTime);

    ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
    ElapsedMicroseconds.QuadPart *= 1000000;
    ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

    StartingTime = EndingTime;

    if (ElapsedMicroseconds.QuadPart > (1000 * 10))  // 10mS
    {
        return true;
    }
    else
    {
        return false;
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
                        std::cout << "bad down" << std::endl;
                        return 1;
                    }
                    //std::cout << "good down" << std::endl; 
                }
                else if ((wParam == WM_KEYUP) || (wParam == WM_SYSKEYUP)) // Keyup
                {
                    if (!goodDown)
                    { 
                        std::cout << "bad up" << std::endl;
                        goodDown = true;
                        return 1;
                    }
                    //std::cout << "good up" << std::endl;
                }
            }
            break;
        }

    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main(int argc, char* argv[])
{
    std::cout << "Space bar filter for HP 32 AIO keybaords (10mS window)" << std::endl;

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

