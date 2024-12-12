#include "InputManager.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <Windows.h>
#include <imm.h>
#pragma comment(lib, "imm32.lib")
#endif

void InputManager::disableIME()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    // 获取当前窗口句柄
    HWND hwnd = GetForegroundWindow();

    if (hwnd)
    {
        // 获取输入法上下文
        HIMC himc = ImmGetContext(hwnd);
        if (himc)
        {
            // 关闭输入法
            ImmSetOpenStatus(himc, FALSE);
            // 释放上下文
            ImmReleaseContext(hwnd, himc);
        }
    }
#endif
}

void InputManager::enableIME()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    HWND hwnd = GetForegroundWindow();

    if (hwnd)
    {
        HIMC himc = ImmGetContext(hwnd);
        if (himc)
        {
            // 开启输入法
            ImmSetOpenStatus(himc, TRUE);
            ImmReleaseContext(hwnd, himc);
        }
    }
#endif
}