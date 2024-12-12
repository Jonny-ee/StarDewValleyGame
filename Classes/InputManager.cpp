#include "InputManager.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <Windows.h>
#include <imm.h>
#pragma comment(lib, "imm32.lib")
#endif

void InputManager::disableIME()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    // ��ȡ��ǰ���ھ��
    HWND hwnd = GetForegroundWindow();

    if (hwnd)
    {
        // ��ȡ���뷨������
        HIMC himc = ImmGetContext(hwnd);
        if (himc)
        {
            // �ر����뷨
            ImmSetOpenStatus(himc, FALSE);
            // �ͷ�������
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
            // �������뷨
            ImmSetOpenStatus(himc, TRUE);
            ImmReleaseContext(hwnd, himc);
        }
    }
#endif
}